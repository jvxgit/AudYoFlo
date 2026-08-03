import 'package:flutter/material.dart';

import '../editor/node_type_definition.dart';

/// Seitenleiste mit den verfügbaren Node-Typen. Ein Klick fügt eine neue
/// Instanz des jeweiligen Typs im Canvas hinzu.
class NodePalette extends StatelessWidget {
  final List<NodeTypeDefinition> nodeTypes;
  final void Function(NodeTypeDefinition nodeType) onAddNode;

  const NodePalette({
    super.key,
    required this.nodeTypes,
    required this.onAddNode,
  });

  @override
  Widget build(BuildContext context) {
    return Material(
      color: Theme.of(context).colorScheme.surface,
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Padding(
            padding: const EdgeInsets.all(16),
            child: Text(
              'Nodes',
              style: Theme.of(context).textTheme.titleMedium,
            ),
          ),
          const Divider(height: 1),
          Expanded(
            child: ListView.builder(
              padding: const EdgeInsets.symmetric(vertical: 8),
              itemCount: nodeTypes.length,
              itemBuilder: (context, index) {
                final nodeType = nodeTypes[index];
                return ListTile(
                  leading: const Icon(Icons.widgets_outlined),
                  title: Text(nodeType.label),
                  subtitle: Text(
                    '${nodeType.inputs.length} In · ${nodeType.outputs.length} Out',
                  ),
                  onTap: () => onAddNode(nodeType),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}
