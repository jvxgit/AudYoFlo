import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../editor/default_node_types.dart';
import '../editor/diagram_editor_mode.dart';
import '../editor/fl_nodes_adapter.dart';
import '../editor/node_type_definition.dart';
import '../state/diagram_controller.dart';
import 'node_header.dart';
import 'node_inspector.dart';
import 'node_palette.dart';
import 'read_only_node.dart';

/// Haupt-Screen: Node-Palette links, fl_nodes-Canvas in der Mitte,
/// Eigenschaften-Panel rechts.
///
/// [mode] steuert, ob bearbeitet werden kann ([DiagramEditorMode.edit],
/// Standard) oder ob der Canvas nur Anzeige ist ([DiagramEditorMode.readOnly]
/// — Nodes bleiben per Klick auswählbar, damit der Inspector weiterhin
/// Details zeigt, aber Hinzufügen/Verschieben/Verbinden/Löschen/Umbenennen
/// sind gesperrt).
class CanvasPage extends StatefulWidget {
  final DiagramEditorMode mode;

  const CanvasPage({super.key, this.mode = DiagramEditorMode.edit});

  @override
  State<CanvasPage> createState() => _CanvasPageState();
}

class _CanvasPageState extends State<CanvasPage> {
  late final FlNodesAdapter _adapter;
  int _addedNodeCount = 0;

  bool get _isEditable => widget.mode == DiagramEditorMode.edit;

  @override
  void initState() {
    super.initState();
    // context.read (statt .watch) ist hier bewusst: wir wollen den
    // Controller einmalig zum Verdrahten des Adapters holen, aber nicht auf
    // Änderungen re-buildn (dafür ist build() unten via context.watch da).
    _adapter = FlNodesAdapter(
      diagramController: context.read<DiagramController>(),
      nodeTypes: defaultNodeTypes,
    );
  }

  @override
  void dispose() {
    // DiagramController gehört dem ChangeNotifierProvider (main.dart) und
    // wird von dort automatisch disposed.
    _adapter.dispose();
    super.dispose();
  }

  void _addNode(NodeTypeDefinition nodeType) {
    // Einfache Kaskadierung, damit neu hinzugefügte Nodes nicht exakt
    // übereinander landen. Ausgereiftere Platzierung (z.B. Viewport-Mitte)
    // ist ein möglicher Folgeschritt, kein Blocker für diesen Meilenstein.
    final offset = Offset(40.0 * _addedNodeCount, 40.0 * _addedNodeCount);
    _addedNodeCount++;
    _adapter.flController.addNode(nodeType.typeId, offset: offset);
  }

  void _zoomToFit() {
    final nodeIds = _adapter.flController.nodes.keys.toSet();
    if (nodeIds.isEmpty) return;
    _adapter.flController.focusNodesById(nodeIds);
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
    _addedNodeCount = 0;
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
      // Im Read-Only-Modus ersetzt nodeBuilder das komplette, standardmäßig
      // interaktive Node-Widget durch unseren gestenfreien ReadOnlyNode —
      // siehe dessen Doku dazu, warum ein Config-Flag dafür nicht reicht.
      nodeBuilder: _isEditable
          ? null
          : (context, node) => ReadOnlyNode(
                controller: _adapter.flController,
                node: node,
              ),
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
                nodeTypes: defaultNodeTypes,
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
