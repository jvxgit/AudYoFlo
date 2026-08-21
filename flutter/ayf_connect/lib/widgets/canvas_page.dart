import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../editor/default_node_types.dart';
import '../editor/diagram_editor_mode.dart';
import '../editor/fl_nodes_adapter.dart';
import '../editor/node_type_definition.dart';
import '../models/diagram.dart';
import '../state/diagram_controller.dart';
import 'move_only_node.dart';
import 'node_header.dart';
import 'node_inspector.dart';
import 'node_palette.dart';
import 'read_only_node.dart';

/// Haupt-Screen: Node-Palette links, fl_nodes-Canvas in der Mitte,
/// Eigenschaften-Panel rechts.
///
/// [mode] steuert, welche Interaktionen erlaubt sind — siehe
/// [DiagramEditorMode] für die Details der drei Stufen ([DiagramEditorMode.edit]
/// Standard, [DiagramEditorMode.moveOnly], [DiagramEditorMode.readOnly]).
///
/// [nodeTypes] ist der Katalog, gegen den fl_nodes seine Node-Prototypen
/// registriert (Standard: [defaultNodeTypes]). [diagram], falls gesetzt, wird
/// beim Start (und erneut bei jeder Änderung der Referenz) via
/// [FlNodesAdapter.loadDiagram] geladen — gedacht für extern befüllte
/// Diagramme (z.B. aus einem Backend-Cache), typischerweise kombiniert mit
/// [DiagramEditorMode.readOnly].
class CanvasPage extends StatefulWidget {
  final DiagramEditorMode mode;
  final List<NodeTypeDefinition> nodeTypes;
  final Diagram? diagram;

  const CanvasPage({
    super.key,
    this.mode = DiagramEditorMode.edit,
    this.nodeTypes = defaultNodeTypes,
    this.diagram,
  });

  @override
  State<CanvasPage> createState() => _CanvasPageState();
}

class _CanvasPageState extends State<CanvasPage> {
  late final FlNodesAdapter _adapter;
  Diagram? _pendingDiagram;
  bool _diagramLoadScheduled = false;

  bool get _isEditable => widget.mode == DiagramEditorMode.edit;

  @override
  void initState() {
    super.initState();
    // context.read (statt .watch) ist hier bewusst: wir wollen den
    // Controller einmalig zum Verdrahten des Adapters holen, aber nicht auf
    // Änderungen re-buildn (dafür ist build() unten via context.watch da).
    _adapter = FlNodesAdapter(
      diagramController: context.read<DiagramController>(),
      nodeTypes: widget.nodeTypes,
    );
    final initialDiagram = widget.diagram;
    if (initialDiagram != null) {
      _scheduleLoadDiagram(initialDiagram);
    }
  }

  @override
  void didUpdateWidget(covariant CanvasPage oldWidget) {
    super.didUpdateWidget(oldWidget);
    final newDiagram = widget.diagram;
    if (newDiagram != null && !identical(newDiagram, oldWidget.diagram)) {
      _scheduleLoadDiagram(newDiagram);
    }
  }

  // fl_nodes' NodeEditorRenderBox keeps an internal diff-cache of the node
  // list that it only reconciles against the widget tree once per frame. A
  // Diagram carrying many nodes/edges triggers many addNode/addLink calls on
  // the controller in one synchronous burst (see FlNodesAdapter.loadDiagram);
  // firing that burst from within didUpdateWidget (i.e. while this widget's
  // own tree is being rebuilt) races that diff-cache and throws
  // "NodeEditorRenderBox: Found N children, but only M nodes in the
  // controller." Deferring the actual load to after the current frame lets
  // the render tree settle first. Bursts of updates within the same frame
  // are coalesced into a single load of the most recent diagram.
  void _scheduleLoadDiagram(Diagram diagram) {
    _pendingDiagram = diagram;
    if (_diagramLoadScheduled) return;
    _diagramLoadScheduled = true;
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      _diagramLoadScheduled = false;
      final diagramToLoad = _pendingDiagram;
      _pendingDiagram = null;
      if (mounted && diagramToLoad != null) {
        // loadDiagram's Future only resolves once each node's own title
        // (not just its shared type name) has actually landed in
        // DiagramController - see the comment on loadDiagram itself.
        await _adapter.loadDiagram(diagramToLoad);
        if (!mounted) return;
        // Externally supplied diagrams come with generated, not
        // user-chosen, positions - fixed grid steps computed without
        // knowing each node's actual (label-dependent) rendered size, which
        // routinely overlap once a label makes a node wider/taller than the
        // step. Re-flowing with arrangeRow (constant gap, real measured
        // sizes) fixes that; the default viewport also has no reason to
        // already frame the result, so zoom to fit afterwards. Both have to
        // wait for one more frame: fl_nodes computes each node's bounds from
        // its GlobalKey's RenderBox, which for nodes just added by
        // loadDiagram() above doesn't exist until this frame has actually
        // been laid out.
        WidgetsBinding.instance.addPostFrameCallback((_) {
          if (!mounted) return;
          _adapter.arrangeRow();
          _zoomToFit();
        });
      }
    });
  }

  @override
  void dispose() {
    // DiagramController gehört dem ChangeNotifierProvider (main.dart) und
    // wird von dort automatisch disposed.
    _adapter.dispose();
    super.dispose();
  }

  void _addNode(NodeTypeDefinition nodeType) {
    // nextFreeSlot places the new node to the right of every existing one
    // with a constant gap, based on their actually rendered sizes - a fixed
    // per-add offset (e.g. cascading by a constant step) would overlap as
    // soon as a label makes a node wider/taller than that step.
    final offset = _adapter.nextFreeSlot();
    _adapter.flController.addNode(nodeType.typeId, offset: offset);
  }

  void _zoomToFit() {
    final nodeIds = _adapter.flController.nodes.keys.toSet();
    if (nodeIds.isEmpty) return;
    _adapter.flController.focusNodesById(nodeIds);
  }

  void _arrangeRow() {
    if (_adapter.flController.nodes.isEmpty) return;
    _adapter.arrangeRow();
    _zoomToFit();
  }

  Future<void> _clearCanvas() async {
    if (_adapter.flController.nodes.isEmpty) return;

    final confirmed = await showDialog<bool>(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Canvas leeren?'),
        content: const Text(
          'Alle Nodes und Verbindungen werden entfernt. Das lässt sich über '
          'Strg+Z rückgängig machen.',
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(false),
            child: const Text('Abbrechen'),
          ),
          TextButton(
            onPressed: () => Navigator.of(context).pop(true),
            child: const Text('Leeren'),
          ),
        ],
      ),
    );

    if (confirmed != true) return;

    for (final nodeId in _adapter.flController.nodes.keys.toList()) {
      _adapter.flController.removeNodeById(nodeId);
    }
  }

  @override
  Widget build(BuildContext context) {
    final canvas = FlNodeEditorWidget(
      controller: _adapter.flController,
      expandToParent: true,
      overlay: () => [],
      headerBuilder: (context, node, style, onToggleCollapse) =>
          DiagramNodeHeader(
        node: node,
        onToggleCollapse: onToggleCollapse,
      ),
      // In moveOnly/readOnly ersetzt nodeBuilder das komplette, standardmäßig
      // interaktive Node-Widget durch eine gestenreduzierte Variante — siehe
      // ReadOnlyNode/MoveOnlyNode dazu, warum ein Config-Flag dafür nicht
      // reicht.
      nodeBuilder: switch (widget.mode) {
        DiagramEditorMode.edit => null,
        DiagramEditorMode.moveOnly => (context, node) => MoveOnlyNode(
              controller: _adapter.flController,
              node: node,
            ),
        DiagramEditorMode.readOnly => (context, node) => ReadOnlyNode(
              controller: _adapter.flController,
              node: node,
            ),
      },
    );

    return Scaffold(
      appBar: AppBar(
        title: const Text('ConnectorFlo'),
        actions: [
          IconButton(
            icon: const Icon(Icons.fit_screen_outlined),
            tooltip: 'Zoom to fit',
            onPressed: _zoomToFit,
          ),
          if (widget.mode != DiagramEditorMode.readOnly)
            IconButton(
              icon: const Icon(Icons.grid_view_outlined),
              tooltip: 'Automatisch anordnen',
              onPressed: _arrangeRow,
            ),
          if (_isEditable)
            IconButton(
              icon: const Icon(Icons.delete_sweep_outlined),
              tooltip: 'Canvas leeren',
              onPressed: _clearCanvas,
            ),
        ],
      ),
      body: Row(
        children: [
          if (_isEditable) ...[
            SizedBox(
              width: 240,
              child: NodePalette(
                nodeTypes: widget.nodeTypes,
                onAddNode: _addNode,
              ),
            ),
            const VerticalDivider(width: 1),
          ],
          Expanded(
            child: _isEditable
                ? FlNodeEditorShortcutsWidget(
                    controller: _adapter.flController,
                    child: canvas,
                  )
                : canvas,
          ),
          const VerticalDivider(width: 1),
          SizedBox(
            width: 280,
            child: NodeInspector(readOnly: !_isEditable),
          ),
        ],
      ),
    );
  }
}
