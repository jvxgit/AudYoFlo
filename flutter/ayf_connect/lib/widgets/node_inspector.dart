import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../models/port_definition.dart';
import '../state/diagram_controller.dart';

/// Eigenschaften-Panel für den aktuell ausgewählten Node: Name umbenennen
/// und die vorhandenen Ports einsehen. Holt sich den [DiagramController]
/// selbst über [Provider], statt ihn durchgereicht zu bekommen.
///
/// Im Read-Only-Modus ([readOnly]) bleibt die Auswahl/Anzeige nutzbar, das
/// Namensfeld wird aber nicht editierbar dargestellt.
class NodeInspector extends StatefulWidget {
  final bool readOnly;

  const NodeInspector({super.key, this.readOnly = false});

  @override
  State<NodeInspector> createState() => _NodeInspectorState();
}

class _NodeInspectorState extends State<NodeInspector> {
  final _titleController = TextEditingController();
  String? _boundNodeId;

  @override
  void dispose() {
    _titleController.dispose();
    super.dispose();
  }

  void _submitRename(String nodeId) {
    final newTitle = _titleController.text.trim();
    if (newTitle.isEmpty) return;
    context.read<DiagramController>().renameNode(nodeId, newTitle);
  }

  @override
  Widget build(BuildContext context) {
    // context.watch abonniert den Controller: sobald notifyListeners()
    // feuert (Auswahl, Umbenennen, ...), baut dieses Widget sich neu.
    final diagramController = context.watch<DiagramController>();
    final nodeId = diagramController.selectedNodeId;
    final node = nodeId == null ? null : diagramController.nodeById(nodeId);

    if (node == null) {
      _boundNodeId = null;
      return Material(
        color: Theme.of(context).colorScheme.surface,
        child: const Padding(
          padding: EdgeInsets.all(16),
          child: Text(
            'Kein Node ausgewählt.',
            style: TextStyle(color: Colors.white54),
          ),
        ),
      );
    }

    if (_boundNodeId != node.id) {
      _boundNodeId = node.id;
      _titleController.text = node.title;
    }

    return Material(
      color: Theme.of(context).colorScheme.surface,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Eigenschaften',
              style: Theme.of(context).textTheme.titleMedium,
            ),
            const SizedBox(height: 4),
            Text(
              'Typ: ${node.typeIdentification ?? node.typeId}',
              style: Theme.of(context).textTheme.bodySmall,
            ),
            const SizedBox(height: 16),
            if (widget.readOnly)
              Text(node.title, style: Theme.of(context).textTheme.bodyLarge)
            else
              TextField(
                controller: _titleController,
                decoration: const InputDecoration(labelText: 'Name'),
                onSubmitted: (_) => _submitRename(node.id),
                onEditingComplete: () => _submitRename(node.id),
              ),
            const SizedBox(height: 24),
            ..._portSection(context, 'Inputs', node.inputs),
            ..._portSection(context, 'Outputs', node.outputs),
          ],
        ),
      ),
    );
  }

  List<Widget> _portSection(
    BuildContext context,
    String label,
    List<PortDefinition> ports,
  ) {
    if (ports.isEmpty) return const [];
    return [
      Text(label, style: Theme.of(context).textTheme.labelLarge),
      const SizedBox(height: 4),
      for (final port in ports)
        Padding(
          padding: const EdgeInsets.symmetric(vertical: 2),
          child: Row(
            children: [
              Expanded(child: Text(port.label)),
              Text(
                port.type.name,
                style: const TextStyle(color: Colors.white54, fontSize: 12),
              ),
            ],
          ),
        ),
      const SizedBox(height: 16),
    ];
  }
}
