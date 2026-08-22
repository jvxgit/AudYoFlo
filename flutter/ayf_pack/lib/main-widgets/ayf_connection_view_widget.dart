import 'package:connectorflo/editor/diagram_editor_mode.dart';
import 'package:connectorflo/editor/node_type_definition.dart';
import 'package:connectorflo/models/diagram.dart';
import 'package:connectorflo/models/diagram_edge.dart';
import 'package:connectorflo/models/diagram_node.dart';
import 'package:connectorflo/models/port_definition.dart';
import 'package:connectorflo/models/port_type.dart';
import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/widgets/canvas_page.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../ayf_pack_local.dart';

const double _columnWidth = 220.0;
const double _rowHeight = 120.0;

// Cycled through for components that show up more than once in the chain
// (see _buildDiagramFromBackendCache), so repeated occurrences of the same
// real backend component can be told apart from genuinely distinct ones.
const List<Color> _repeatedComponentColors = [
  Color(0xFF00796B), // teal
  Color(0xFFEF6C00), // deep orange
  Color(0xFF5C6BC0), // indigo
  Color(0xFFC2185B), // pink
  Color(0xFF7CB342), // light green
  Color(0xFF6D4C41), // brown
];

// Embeds the ConnectorFlo component-chaining canvas (flutter/ayf_connect) as
// a reusable widget. Read-only - it visualizes the component chains
// currently registered in the backend cache (processSection.
// theRegisteredProcesses), it does not edit them. Wiring edits back to the
// backend is a possible follow-up, not done here.
//
// Backend components carry an arbitrary, per-instance number of connectors,
// which doesn't fit a single fixed ayf_connect node type. Instead of one
// shared type, [_buildDiagramFromBackendCache] registers one
// [NodeTypeDefinition] per distinct *port-count shape* (see
// [_shapeTypeId]/[_shapeNodeType]) - e.g. a component with one input and two
// output branches gets typeId "backend_component_i1_o2" - and reuses it for
// every occurrence with that shape. The actual connector names
// (nmInputConnectorTo / nmOutputConnectorFrom) are then applied per instance
// as port labels, same as the node title (see visit() below).
//
// Non-root occurrences currently always get a single input port: the
// backend (AudYoFloOneComponentInProcess.nmInputConnectorTo) only ever
// reports one incoming connector per component today. Process roots get
// zero (see `hasParent` in visit()) - a root has no incoming edge in this
// diagram, whatever feeds it is outside the tracked chain, so there's
// nothing to label. The shape/port-list plumbing doesn't assume "always
// one" though, so if the backend starts reporting several named inputs per
// component, only the `inputLabels` construction in visit() needs to grow
// beyond one element - node types, edges and port ids already generalize to
// N.
class AudYoFloConnectFlowWidget extends StatelessWidget {
  const AudYoFloConnectFlowWidget({super.key});

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    return ChangeNotifierProvider<DiagramController>(
      create: (_) => DiagramController(),
      child: Selector<AudYoFloBackendCache, int>(
        selector: (context, cache) => cache.processSection.ssUpdateId,
        builder: (context, ssUpdateId, child) {
          final built = _buildDiagramFromBackendCache(theBeCache);
          return CanvasPage(
            mode: DiagramEditorMode.readOnly,
            nodeTypes: built.nodeTypes,
            diagram: built.diagram,
          );
        },
      ),
    );
  }

  // Walks every registered process' component tree (processSection.
  // theRegisteredProcesses) and flattens it into a Diagram: one node per
  // *occurrence* in the tree (not deduplicated by component identification),
  // so a component that legitimately shows up at several points in the
  // chain - e.g. a shared clock/sync device acting as both source at the
  // start and sink at the end - is drawn at each of those points rather than
  // collapsed into one box. Occurrences that share the same component
  // identification (JvxComponentIdentification, i.e. the same real backend
  // component) get the same groupColor so that relationship stays visible.
  static _BuiltDiagram _buildDiagramFromBackendCache(
      AudYoFloBackendCache theBeCache) {
    final reportedProcesses =
        theBeCache.processSection.theRegisteredProcesses.reportedProcesses;
    final roots = reportedProcesses.values
        .map((process) => process.involved)
        .whereType<AudYoFloOneComponentInProcess>();

    // Pass 1: count occurrences per component identification, to know which
    // ones need a group color.
    final occurrenceCount = <String, int>{};
    final countingVisited = <AudYoFloOneComponentInProcess>{};
    void countOccurrences(AudYoFloOneComponentInProcess comp) {
      if (!countingVisited.add(comp)) return;
      occurrenceCount.update(_identityKey(comp), (v) => v + 1,
          ifAbsent: () => 1);
      comp.next.forEach(countOccurrences);
    }

    roots.forEach(countOccurrences);

    final colorByIdentity = <String, Color>{};
    var nextColorIndex = 0;
    Color? colorFor(String identityKey) {
      if ((occurrenceCount[identityKey] ?? 0) <= 1) return null;
      return colorByIdentity.putIfAbsent(identityKey, () {
        final color = _repeatedComponentColors[
            nextColorIndex % _repeatedComponentColors.length];
        nextColorIndex++;
        return color;
      });
    }

    // Pass 2: build one DiagramNode per occurrence and one DiagramEdge per
    // parent -> next relationship, in a left-to-right layered layout (x by
    // depth, y by a row block reserved per subtree - see the rowSpan return
    // of visit() below) since the backend does not report a persisted
    // layout. Also collects the distinct port-count shapes seen, so a
    // NodeTypeDefinition can be registered for each (see class doc comment).
    final nodes = <DiagramNode>[];
    final edges = <DiagramEdge>[];
    final nodeTypesByShape = <String, NodeTypeDefinition>{};
    final buildVisited = <AudYoFloOneComponentInProcess>{};
    var positionCounter = 0;

    // Returns this occurrence's own node id together with the number of
    // rows its whole subtree occupies (>=1) - the latter lets both branch
    // placement within a chain and successive process roots (see the `roots`
    // loop below) reserve as many rows as a subtree actually needs instead
    // of a flat 1-row-per-branch guess, so sibling subtrees (or entirely
    // separate processes) never land on the same row.
    ({String id, int rowSpan})? visit(
        AudYoFloOneComponentInProcess comp, int depth, int row,
        {required bool hasParent}) {
      if (!buildVisited.add(comp)) return null;

      final positionId = 'n${positionCounter++}';

      // Mirrors outputLabels below: a process root has no incoming edge in
      // this diagram (whatever feeds it - a physical input device, say - is
      // outside the tracked chain), so it gets zero input ports, the same
      // way a leaf gets zero output ports from an empty `next`. Everyone
      // else gets their one connector (see class doc comment: today always
      // exactly one).
      final inputLabels = [
        if (hasParent)
          comp.nmInputConnectorTo.isNotEmpty ? comp.nmInputConnectorTo : 'default',
      ];
      // One output branch per `next` entry; the connector name for branch
      // `i` is stored on `comp.next[i]` itself (it describes `comp`'s own
      // outgoing connector for that link - see ffi_process_iterator_next_
      // ocon_name), not on `comp`.
      final outputLabels = [
        for (final next in comp.next)
          next.nmOutputConnectorFrom.isNotEmpty
              ? next.nmOutputConnectorFrom
              : 'default',
      ];

      final typeId = _shapeTypeId(inputLabels.length, outputLabels.length);
      nodeTypesByShape.putIfAbsent(
        typeId,
        () => _shapeNodeType(inputLabels.length, outputLabels.length),
      );

      nodes.add(DiagramNode(
        id: positionId,
        typeId: typeId,
        title: _nodeTitle(comp),
        position: Offset(depth * _columnWidth, row * _rowHeight),
        inputs: [
          for (var i = 0; i < inputLabels.length; i++)
            PortDefinition(
              id: 'in$i',
              label: inputLabels[i],
              type: PortType.any,
              direction: PortDirection.input,
            ),
        ],
        outputs: [
          for (var i = 0; i < outputLabels.length; i++)
            PortDefinition(
              id: 'out$i',
              label: outputLabels[i],
              type: PortType.any,
              direction: PortDirection.output,
            ),
        ],
        groupColor: colorFor(_identityKey(comp)),
      ));

      var nextChildRow = row;
      var subtreeRowSpan = 0;
      for (var i = 0; i < comp.next.length; i++) {
        final child =
            visit(comp.next[i], depth + 1, nextChildRow, hasParent: true);
        if (child != null) {
          edges.add(DiagramEdge(
            id: '$positionId->${child.id}',
            fromNodeId: positionId,
            fromPortId: 'out$i',
            toNodeId: child.id,
            toPortId: 'in0',
          ));
          nextChildRow += child.rowSpan;
          subtreeRowSpan += child.rowSpan;
        }
      }
      return (id: positionId, rowSpan: subtreeRowSpan > 0 ? subtreeRowSpan : 1);
    }

    var nextRootRow = 0;
    for (final root in roots) {
      final result = visit(root, 0, nextRootRow, hasParent: false);
      if (result != null) {
        nextRootRow += result.rowSpan;
      }
    }

    return _BuiltDiagram(
      diagram: Diagram(nodes: nodes, edges: edges),
      nodeTypes: nodeTypesByShape.values.toList(),
    );
  }

  // Two occurrences represent the same real backend component - and should
  // therefore be colored alike - when they share the same
  // JvxComponentIdentification, including uid (plain cpId equality/`.txt`
  // ignores uid, which is not precise enough: externally attached / dynamic-
  // node components can share cpTp/slotid/slotsubid while being genuinely
  // different instances - see attach_external_component).
  static String _identityKey(AudYoFloOneComponentInProcess comp) =>
      '${comp.cpId.txt}#${comp.cpId.uid}';

  static String _shapeTypeId(int numInputs, int numOutputs) =>
      'backend_component_i${numInputs}_o$numOutputs';

  static NodeTypeDefinition _shapeNodeType(int numInputs, int numOutputs) {
    return NodeTypeDefinition(
      typeId: _shapeTypeId(numInputs, numOutputs),
      label: 'Component',
      inputs: [
        for (var i = 0; i < numInputs; i++)
          PortSpec(id: 'in$i', label: 'In', type: PortType.any),
      ],
      outputs: [
        for (var i = 0; i < numOutputs; i++)
          PortSpec(id: 'out$i', label: 'Out', type: PortType.any),
      ],
    );
  }

  static String _nodeTitle(AudYoFloOneComponentInProcess comp) =>
      comp.descriptionComponent.isNotEmpty
          ? comp.descriptionComponent
          : comp.nameModule;
}

class _BuiltDiagram {
  final Diagram diagram;
  final List<NodeTypeDefinition> nodeTypes;

  const _BuiltDiagram({required this.diagram, required this.nodeTypes});
}
