import 'dart:async';

import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';

import '../models/diagram.dart';
import '../models/port_definition.dart';
import '../models/port_type.dart';
import '../state/diagram_controller.dart';
import '../state/invalid_connection_exception.dart';
import 'diagram_editor_mode.dart';
import 'node_type_definition.dart';

/// Verbindet [DiagramController] (unser entkoppeltes, JSON-fähiges
/// Domänenmodell) mit [FlNodeEditorController] (die tatsächliche
/// Interaktions-Engine von fl_nodes, die Drag/Connect/Delete-Gesten selbst
/// verarbeitet).
///
/// fl_nodes bleibt die "lebende" Quelle für Positionen/Verbindungen während
/// der Nutzer interagiert; dieser Adapter hört auf seine Events und
/// spiegelt jede Änderung in den [DiagramController], damit dieser jederzeit
/// ein aktuelles, framework-unabhängiges Abbild des Diagramms liefert.
class FlNodesAdapter {
  final DiagramController diagramController;
  final List<NodeTypeDefinition> _nodeTypes;

  /// The node types currently registered with [flController], i.e. the ones
  /// [loadDiagram] will accept. Grows over time via [registerNodeTypes] -
  /// see that method for why the constructor's [nodeTypes] alone isn't
  /// always enough.
  List<NodeTypeDefinition> get nodeTypes => List.unmodifiable(_nodeTypes);

  late final FlNodeEditorController flController;
  StreamSubscription<dynamic>? _subscription;

  FlNodesAdapter({
    required this.diagramController,
    required List<NodeTypeDefinition> nodeTypes,
  }) : _nodeTypes = List.of(nodeTypes) {
    // autoBuildGraph/autoRunGraph gehören zur visuellen-Programmierung/
    // Execution-Funktionalität von fl_nodes, die wir hier nicht nutzen
    // (unsere Nodes definieren nur Datenfluss, kein onExecute). Ohne diese
    // Deaktivierung plant der Controller intern wiederkehrende Timer für
    // eine Funktion, die wir nie aufrufen.
    flController = FlNodeEditorController(
      config: const FlNodeEditorConfig(
        autoBuildGraph: false,
        autoRunGraph: false,
      ),
    );

    for (final nodeType in _nodeTypes) {
      flController.registerNodePrototype(_buildPrototype(nodeType));
    }

    _subscription = flController.eventBus.events.listen(_handleEvent);
  }

  /// Registers every type in [types] that isn't already known (matched by
  /// [NodeTypeDefinition.typeId]) with both [flController] and [nodeTypes];
  /// existing types are left untouched, so this is safe (and cheap) to call
  /// repeatedly with the same or a growing catalog.
  ///
  /// Needed for callers that derive node types from live, per-instance data
  /// rather than a fixed catalog known up front - e.g.
  /// AudYoFloConnectFlowWidget in the startern app builds one type per
  /// distinct input/output port *count* (a backend component's number of
  /// connectors isn't known statically), and a later refresh of the backend
  /// cache can well introduce a shape (say, three output branches) that no
  /// earlier [loadDiagram] call has seen yet. Without this, [loadDiagram]
  /// would reject such a diagram: it only validates against types registered
  /// at construction time.
  void registerNodeTypes(List<NodeTypeDefinition> types) {
    for (final type in types) {
      if (_nodeTypes.any((existing) => existing.typeId == type.typeId)) {
        continue;
      }
      _nodeTypes.add(type);
      flController.registerNodePrototype(_buildPrototype(type));
    }
  }

  void dispose() {
    _subscription?.cancel();
    flController.dispose();
  }

  /// Ersetzt den kompletten aktuellen Canvas-Inhalt durch [diagram] — die
  /// Umkehrung dessen, was der Adapter sonst laufend tut (fl_nodes-Events →
  /// [DiagramController]). Gedacht dafür, dass externe Komponenten ein
  /// fertiges [Diagram] (z.B. aus JSON via [Diagram.fromJson]) zum Anzeigen
  /// übergeben, etwa in Kombination mit [DiagramEditorMode.readOnly].
  ///
  /// Validiert vorab, dass alle Node-Typen bekannt sind und alle Edges nur
  /// auf im Diagramm enthaltene Nodes verweisen — wirft sonst einen
  /// [ArgumentError], bevor der bestehende Canvas-Inhalt angetastet wird.
  ///
  /// Wichtig: fl_nodes vergibt beim Erzeugen jedes Node über
  /// [FlNodeEditorController.addNode] eine eigene, neue Id (es gibt keine
  /// Möglichkeit, eine vorgegebene Id zu übernehmen). Die IDs im laufenden
  /// Canvas (und im davon gespiegelten [DiagramController] danach) weichen
  /// deshalb von den IDs in [diagram] ab; nur die Struktur (Typen, Positionen,
  /// Verbindungen) bleibt erhalten.
  ///
  /// Der zurückgegebene Future schließt ab, sobald auch Titel, Port-Labels,
  /// Gruppenfarbe und Typ-Identifikation aus [diagram] im [DiagramController]
  /// angekommen sind (siehe Kommentar weiter unten zum asynchronen
  /// Event-Bus) — vor dem Awaiten zeigen Nodes mit gemeinsamem Typ
  /// kurzzeitig dessen (identische) statische Katalog-Daten statt ihrer
  /// eigenen.
  Future<void> loadDiagram(Diagram diagram) async {
    for (final node in diagram.nodes) {
      if (!nodeTypes.any((type) => type.typeId == node.typeId)) {
        throw ArgumentError(
          'Unbekannter Node-Typ "${node.typeId}" (Node "${node.id}") ist '
          'nicht in den registrierten nodeTypes enthalten.',
        );
      }
    }

    final diagramNodeIds = diagram.nodes.map((node) => node.id).toSet();
    for (final edge in diagram.edges) {
      if (!diagramNodeIds.contains(edge.fromNodeId) ||
          !diagramNodeIds.contains(edge.toNodeId)) {
        throw ArgumentError(
          'Edge "${edge.id}" referenziert einen Node, der nicht Teil von '
          'diagram.nodes ist.',
        );
      }
    }

    for (final id in flController.nodes.keys.toList()) {
      // removeNodeById entfernt auch alle daran hängenden Links.
      flController.removeNodeById(id);
    }

    final idMap = <String, String>{};
    for (final node in diagram.nodes) {
      final flNode = flController.addNode(node.typeId, offset: node.position);
      idMap[node.id] = flNode.id;
    }

    for (final edge in diagram.edges) {
      flController.addLink(
        idMap[edge.fromNodeId]!,
        edge.fromPortId,
        idMap[edge.toNodeId]!,
        edge.toPortId,
      );
    }

    // flController's event bus is a non-sync broadcast StreamController, so
    // _mirrorAddNode (which seeds each DiagramController entry from
    // nodeType.label/inputs/outputs, the static per-type catalog data) only
    // runs on a later microtask, not synchronously within addNode() above.
    // Overriding right after addNode() would therefore target a
    // DiagramController entry that doesn't exist yet and silently no-op, and
    // doing it in the same synchronous burst is exactly what a caller has no
    // reliable way to sequence around - so do it here, after yielding once
    // for the mirror events to land.
    await Future<void>.delayed(Duration.zero);
    for (final node in diagram.nodes) {
      final flNodeId = idMap[node.id];
      if (flNodeId != null) {
        diagramController.applyNodeOverrides(flNodeId, node);
      }
    }
  }

  /// Rounds [value] up to the next line of fl_nodes' snap-to-grid (a no-op
  /// if snapping is off). Both [arrangeColumn] and [nextFreeSlot] use this
  /// to pre-align the *target* position they compute onto a grid line
  /// before applying it, so that fl_nodes' own grid-snapping (applied
  /// internally by [FlNodeEditorController.dragSelection]/`addNode`) is a
  /// no-op on that value instead of rounding it - independently - by up to
  /// half a grid cell. Without this, two neighbouring nodes' independent
  /// roundings could each shift towards the other by up to half a grid
  /// cell, silently eating the requested gap (and, with the default 64px
  /// grid and a gap smaller than that, going negative - producing exactly
  /// the visible overlaps these two methods exist to prevent).
  double _roundUpToGridLine(double value) {
    final grid = flController.config.snapToGridSize;
    if (!flController.config.enableSnapToGrid || grid <= 0) return value;
    return (value / grid).ceil() * grid;
  }

  /// [node]'s actually rendered size, or null if it hasn't been laid out yet
  /// (no [RenderBox] behind its GlobalKey - e.g. the very first frame after
  /// it was added) or if there's no [WidgetsBinding] at all. The latter
  /// isn't a corner case to shrug off: [FlNodesAdapter] is a plain Dart
  /// class with no widget of its own, and existing unit tests
  /// (fl_nodes_adapter_test.dart) construct it and drive
  /// `flController.addNode()`/`dragSelection()` directly with no widget tree
  /// - and therefore no binding - around it at all. `GlobalKey.currentContext`
  /// throws a [FlutterError] in that case; there's simply nothing rendered
  /// to measure, so this returns null instead of letting that propagate.
  Size? _measuredSize(FlNodeDataModel node) {
    final BuildContext? context;
    try {
      context = node.key.currentContext;
    } on FlutterError {
      return null;
    }
    final renderObject = context?.findRenderObject();
    if (renderObject is! RenderBox || !renderObject.hasSize) return null;
    return renderObject.size;
  }

  /// Repositions every node currently on the canvas into a single row
  /// (left to right, top-aligned), keeping a constant [gap] between each
  /// node's right edge and the next node's left edge. Reads each node's
  /// actually rendered [Size] via its GlobalKey's [RenderBox] (the node's
  /// box grows with its label/port count) rather than assuming a fixed box
  /// size, so nodes never overlap regardless of how large their labels made
  /// them.
  ///
  /// Each node's target X is chained off the *actually applied* offset of
  /// the node placed just before it (not an idealized running total) and
  /// pre-rounded via [_roundUpToGridLine] - see that method's doc comment
  /// for why both matter for avoiding overlap.
  ///
  /// A node without a measured size yet (see [_measuredSize]) keeps its
  /// current offset and is skipped; callers should invoke this only after
  /// at least one frame has been laid out (see the post-frame handling
  /// around `loadDiagram` in `CanvasPage`).
  ///
  /// Moves nodes via [FlNodeEditorController.dragSelection] - the same
  /// public API real drag gestures use - since that's the only way to
  /// reposition a node and have rendering, this adapter's own event
  /// mirroring, and fl_nodes' internal spatial hash grid all stay
  /// consistent. The previously selected nodes stay selected afterwards.
  void arrangeRow({double gap = 48}) {
    final previousSelection = flController.selectedNodeIds.toSet();

    double? top;
    var nextX = 0.0;

    for (final id in flController.nodes.keys.toList()) {
      final node = flController.getNodeById(id);
      if (node == null) continue;
      final size = _measuredSize(node);
      if (size == null) continue;

      top ??= node.offset.dy;
      final target = Offset(_roundUpToGridLine(nextX), top);
      final delta = target - node.offset;
      if (delta != Offset.zero) {
        flController.selectNodesById({id});
        flController.dragSelection(
          delta,
          isWorldDelta: true,
          resetUnboundOffset: true,
        );
      }

      // Chain off the offset fl_nodes actually applied, not the target
      // above - see the doc comment on this method for why.
      nextX = node.offset.dx + size.width + gap;
    }

    if (previousSelection.isEmpty) {
      flController.clearSelection();
    } else {
      flController.selectNodesById(previousSelection);
    }
  }

  /// Re-flows every node currently on the canvas into the row structure its
  /// *current* Y offset already encodes, fixing up X (and, between rows, Y)
  /// spacing from actually rendered sizes so nothing overlaps - without
  /// collapsing everything into a single row the way [arrangeRow] does.
  ///
  /// Nodes are grouped by their current `offset.dy` (exact match - callers
  /// that want a row together, e.g. [Diagram.nodes] built from a
  /// tree/forest with one row band per branch, already give every node in
  /// that band the identical Y before this runs; see
  /// AudYoFloConnectFlowWidget's `_buildDiagramFromBackendCache`, which sets
  /// each node's Y from an integer row index times a constant row height).
  /// Within a row, nodes are laid out left to right the same way
  /// [arrangeRow] lays out the whole canvas (chained off each node's
  /// actually applied offset, pre-rounded via [_roundUpToGridLine]); rows
  /// are then stacked top to bottom, each one starting [rowGap] below the
  /// previous row's tallest node.
  ///
  /// Meant to run after [loadDiagram] for diagrams whose positions already
  /// carry meaningful row/column structure (e.g. one row per connection),
  /// where [arrangeRow]'s single-row layout would destroy that structure.
  /// Like [arrangeRow], a node without a measured size yet (see
  /// [_measuredSize]) keeps its current offset and is skipped; callers
  /// should invoke this only after at least one frame has been laid out.
  void arrangeGrid({double columnGap = 48, double rowGap = 24}) {
    final previousSelection = flController.selectedNodeIds.toSet();

    final rows = <double, List<String>>{};
    for (final id in flController.nodes.keys) {
      final node = flController.getNodeById(id);
      if (node == null) continue;
      rows.putIfAbsent(node.offset.dy, () => []).add(id);
    }

    final sortedRowKeys = rows.keys.toList()..sort();

    var nextY = 0.0;
    for (final rowKey in sortedRowKeys) {
      final rowIds = rows[rowKey]!
        ..sort((a, b) {
          final aDx = flController.getNodeById(a)?.offset.dx ?? 0;
          final bDx = flController.getNodeById(b)?.offset.dx ?? 0;
          return aDx.compareTo(bDx);
        });

      var nextX = 0.0;
      var rowHeight = 0.0;
      final rowTop = _roundUpToGridLine(nextY);

      for (final id in rowIds) {
        final node = flController.getNodeById(id);
        if (node == null) continue;
        final size = _measuredSize(node);
        if (size == null) continue;

        final target = Offset(_roundUpToGridLine(nextX), rowTop);
        final delta = target - node.offset;
        if (delta != Offset.zero) {
          flController.selectNodesById({id});
          flController.dragSelection(
            delta,
            isWorldDelta: true,
            resetUnboundOffset: true,
          );
        }

        // Chain off the offset fl_nodes actually applied, not the target
        // above - see arrangeRow's doc comment for why.
        nextX = node.offset.dx + size.width + columnGap;
        if (size.height > rowHeight) rowHeight = size.height;
      }

      nextY = rowTop + rowHeight + rowGap;
    }

    if (previousSelection.isEmpty) {
      flController.clearSelection();
    } else {
      flController.selectNodesById(previousSelection);
    }
  }

  /// Position for a single newly added node so it can't overlap any
  /// existing one, regardless of how large those nodes' labels made them:
  /// directly to the right of the combined bounding box of all current
  /// nodes (top-aligned to the topmost one), offset by a constant [gap] and
  /// pre-rounded via [_roundUpToGridLine] (`addNode` itself grid-snaps
  /// whatever offset it's given - without pre-rounding, that could shift
  /// the node back towards the one before it, see that method's doc
  /// comment). A node without a measured size yet (see [_measuredSize])
  /// falls back to [fallbackSize] instead of being skipped, so a rapid
  /// sequence of adds (before an earlier node's first frame has laid out)
  /// still can't collide.
  Offset nextFreeSlot({
    double gap = 48,
    Size fallbackSize = const Size(160, 96),
  }) {
    Rect? bounds;
    for (final node in flController.nodes.values) {
      final size = _measuredSize(node) ?? fallbackSize;
      final rect = node.offset & size;
      bounds = bounds == null ? rect : bounds.expandToInclude(rect);
    }
    if (bounds == null) return Offset.zero;
    return Offset(_roundUpToGridLine(bounds.right + gap), bounds.top);
  }

  NodeTypeDefinition _typeById(String typeId) {
    return nodeTypes.firstWhere(
      (nodeType) => nodeType.typeId == typeId,
      orElse: () =>
          throw StateError('Unbekannter Node-Typ registriert: $typeId'),
    );
  }

  FlNodePrototype _buildPrototype(NodeTypeDefinition nodeType) {
    return FlNodePrototype(
      idName: nodeType.typeId,
      displayName: (context) => nodeType.label,
      description: (context) => nodeType.label,
      ports: [
        for (final spec in nodeType.inputs) _buildInputPort(spec),
        for (final spec in nodeType.outputs) _buildOutputPort(spec),
      ],
      onExecute: (ports, fields, state, f, p) async {},
    );
  }

  // fl_nodes zeichnet pro Port selbst einen Marker (Kreis/Dreieck aus
  // FlPortStyle.shape) im eigenen RenderObject - unabhängig von nodeBuilder/
  // portBuilder. Sein Dreieck zeigt fix nach +x und ist nicht drehbar. Wir
  // wollen die Richtung frei setzen können (PortMarker im Widget-Baum),
  // deshalb wird der native Marker hier über radius: 0 unsichtbar gemacht.
  // Das Link-Anankern (port.offset an der Node-Kante) und die Verbindungs-
  // Erkennung (fester 12px-Fangradius in _isNearPort) hängen nicht am
  // radius, bleiben also erhalten.
  static FlPortStyle _hiddenMarkerPortStyle(FlPortState state) =>
      const FlPortStyle(
        shape: FlPortShape.circle,
        color: Color(0xFF42A5F5),
        radius: 0,
        linkStyleBuilder: flDefaultLinkStyleBuilder,
      );

  // Hinweis: PortType.any wird hier bewusst nicht unterstützt. fl_nodes prüft
  // Kompatibilität über den Dart-Generic-Typ (T), und dynamic verhält sich
  // dabei asymmetrisch zu unserem PortType.isCompatibleWith (any passt in
  // unserem Modell in beide Richtungen). Solange der Katalog nur konkrete
  // Typen (string/number/boolean) nutzt, ist das kein Problem.
  FlPortPrototype _buildInputPort(PortSpec spec) {
    switch (spec.type) {
      case PortType.string:
        return FlDataInputPortPrototype<String>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
      case PortType.number:
        return FlDataInputPortPrototype<double>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
      case PortType.boolean:
        return FlDataInputPortPrototype<bool>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
      case PortType.any:
        return FlDataInputPortPrototype<dynamic>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
    }
  }

  FlPortPrototype _buildOutputPort(PortSpec spec) {
    switch (spec.type) {
      case PortType.string:
        return FlDataOutputPortPrototype<String>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
      case PortType.number:
        return FlDataOutputPortPrototype<double>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
      case PortType.boolean:
        return FlDataOutputPortPrototype<bool>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
      case PortType.any:
        return FlDataOutputPortPrototype<dynamic>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: _hiddenMarkerPortStyle,
        );
    }
  }

  void _handleEvent(dynamic event) {
    if (event is FlAddNodeEvent) {
      _mirrorAddNode(event.node);
      _scheduleSpatialGridInsert(event.node.id);
    } else if (event is FlRemoveNodeEvent) {
      diagramController.removeNode(event.node.id);
    } else if (event is FlDragSelectionEvent) {
      _mirrorPositions(event.nodeIds);
    } else if (event is FlDragSelectionEndEvent) {
      _mirrorPositions(event.nodeIds);
    } else if (event is FlAddLinkEvent) {
      _mirrorAddLink(event.link);
    } else if (event is FlRemoveLinkEvent) {
      diagramController.removeEdge(event.link.id);
    } else if (event is FlNodeSelectionEvent) {
      _mirrorSelection(event);
    }
  }

  void _mirrorAddNode(FlNodeDataModel node) {
    final nodeType = _typeById(node.prototype.idName);
    diagramController.addNode(
      id: node.id,
      typeId: nodeType.typeId,
      title: nodeType.label,
      position: node.offset,
      inputs: [
        for (final spec in nodeType.inputs) _toPortDefinition(spec, PortDirection.input),
      ],
      outputs: [
        for (final spec in nodeType.outputs) _toPortDefinition(spec, PortDirection.output),
      ],
    );
  }

  PortDefinition _toPortDefinition(PortSpec spec, PortDirection direction) {
    return PortDefinition(
      id: spec.id,
      label: spec.label,
      type: spec.type,
      direction: direction,
    );
  }

  void _mirrorPositions(Set<String> nodeIds) {
    for (final nodeId in nodeIds) {
      final node = flController.getNodeById(nodeId);
      if (node != null) {
        diagramController.moveNode(nodeId, node.offset);
        _updateSpatialGrid(nodeId);
      }
    }
  }

  /// fl_nodes 0.5.0+1 never populates its own controller-level
  /// `nodesSpatialHashGrid` - there is no `insert`/`update` call anywhere in
  /// the package (confirmed by reading its source: only `.remove()`, on node
  /// deletion). That grid is exactly what fl_nodes' own hit-testing queries
  /// to figure out which node - if any - a click/drag landed on
  /// (`NodeEditorRenderObject.hitTestChildren`), what's near a port
  /// (`_isNearPort` in `default_node.dart`/`node_editor_data_layer.dart`),
  /// and what falls inside a marquee (`FlNodeEditorController.
  /// selectNodesByArea`). Left empty, every one of those queries returns
  /// nothing, so clicks/drags on the canvas never reach any node's own
  /// gesture handling at all - not in edit mode, not in moveOnly mode -
  /// they silently fall through to the background layer instead, which is
  /// why nodes could be added/connected/styled but never selected or
  /// dragged with the mouse. This adapter keeps the grid in sync itself so
  /// fl_nodes' own built-in interactions actually work; a new node is
  /// inserted once its first frame has laid out (see
  /// [_scheduleSpatialGridInsert]), and every reposition - ours
  /// ([arrangeColumn], [nextFreeSlot]) or the user's own mouse drag - updates
  /// it via this method, since both go through
  /// [FlNodeEditorController.dragSelection] and therefore emit the
  /// [FlDragSelectionEvent]/[FlDragSelectionEndEvent] this is called from.
  void _updateSpatialGrid(String nodeId) {
    final node = flController.getNodeById(nodeId);
    if (node == null) return;
    final size = _measuredSize(node);
    if (size == null) return;
    flController.nodesSpatialHashGrid.update(
      (id: nodeId, rect: node.offset & size),
    );
  }

  /// Inserts a newly added node into the spatial hash grid once it has a
  /// measured size - see [_updateSpatialGrid] for why this grid needs
  /// populating at all. Deferred to a post-frame callback because
  /// [FlAddNodeEvent] fires before the node's first frame has laid out (no
  /// [RenderBox] behind its [GlobalKey] yet), same constraint documented on
  /// [arrangeRow].
  void _scheduleSpatialGridInsert(String nodeId) {
    final WidgetsBinding binding;
    try {
      // FlNodesAdapter is a plain Dart class with no widget of its own, and
      // existing unit tests (fl_nodes_adapter_test.dart) construct it and
      // call addNode() with no widget tree - and therefore no
      // WidgetsBinding - around it at all. WidgetsBinding.instance throws a
      // FlutterError in that case; there's simply nothing to insert into
      // the grid for (no rendering happening), so skip scheduling instead
      // of letting that propagate.
      binding = WidgetsBinding.instance;
    } on FlutterError {
      return;
    }
    binding.addPostFrameCallback((_) {
      final node = flController.getNodeById(nodeId);
      if (node == null) return;
      final size = _measuredSize(node);
      if (size == null) return;
      flController.nodesSpatialHashGrid.insert(
        (id: nodeId, rect: node.offset & size),
      );
    });
  }

  void _mirrorAddLink(FlLinkDataModel link) {
    try {
      // fl_nodes' FromTo-Record ist trotz seiner Feldnamen so belegt:
      // from = Output-Node-Id, to = Output-Port-Id,
      // fromPort = Input-Node-Id, toPort = Input-Port-Id
      // (siehe FlNodeEditorController.addLink in core.dart). Feldnamen und
      // tatsächliche Bedeutung stimmen dort nicht überein.
      diagramController.addEdge(
        id: link.id,
        fromNodeId: link.fromTo.from,
        fromPortId: link.fromTo.to,
        toNodeId: link.fromTo.fromPort,
        toPortId: link.fromTo.toPort,
      );
    } on InvalidConnectionException catch (error) {
      // fl_nodes prüft Port-Typ-Kompatibilität bereits selbst beim Ziehen der
      // Verbindung; schlägt unsere zusätzliche Domänen-Validierung trotzdem
      // fehl, nehmen wir den Link auch in fl_nodes wieder zurück, damit beide
      // Modelle konsistent bleiben.
      debugPrint('Verbindung von fl_nodes-Adapter abgelehnt: $error');
      flController.removeLinkById(link.id);
    }
  }

  void _mirrorSelection(FlNodeSelectionEvent event) {
    // Bei type == deselect enthält event.nodeIds die zuvor ausgewählten
    // Nodes (die gerade abgewählt wurden), nicht die neue Auswahl — fl_nodes'
    // clearSelection() leert danach immer die komplette Auswahl.
    // Hinweis: FlSelectionEventType selbst wird von fl_nodes nicht öffentlich
    // exportiert (Lücke im Package-Barrel), daher der Vergleich über .name.
    if (event.type.name == 'deselect') {
      diagramController.selectNode(null);
    } else {
      diagramController.selectNode(
        event.nodeIds.isEmpty ? null : event.nodeIds.first,
      );
    }
  }
}
