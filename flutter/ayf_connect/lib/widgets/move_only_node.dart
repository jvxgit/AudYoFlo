import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';

import 'node_header.dart';

/// Ersetzt fl_nodes' Standard-Node-Widget im Move-Only-Modus (über
/// FlNodeEditorWidget.nodeBuilder). Wie [ReadOnlyNode] ein Neuaufbau statt
/// Konfiguration (siehe dessen Doku dazu, warum fl_nodes das nicht per Flag
/// hergibt) — hier zusätzlich mit einer Pan-Geste für
/// `controller.dragSelection`, aber bewusst ohne die Port-Erkennung/
/// Linking-Logik des fl_nodes-Standard-Widgets (`DefaultNodeWidget._isNearPort`
/// / `_onTmpLink*`), damit Nodes verschoben, aber nicht verbunden oder
/// getrennt werden können. Ports bleiben daher wie bei `ReadOnlyNode` rein
/// visuell inkl. ihrer GlobalKeys, damit bestehende Verbindungslinien
/// weiterhin korrekt gezeichnet werden.
class MoveOnlyNode extends StatelessWidget {
  final FlNodeEditorController controller;
  final FlNodeDataModel node;

  const MoveOnlyNode({
    super.key,
    required this.controller,
    required this.node,
  });

  @override
  Widget build(BuildContext context) {
    final inPorts = node.ports.values
        .where((port) => port.prototype.direction == FlPortDirection.input)
        .toList();
    final outPorts = node.ports.values
        .where((port) => port.prototype.direction == FlPortDirection.output)
        .toList();

    return GestureDetector(
      onTap: () => controller.selectNodesById({node.id}),
      onPanStart: (_) {
        if (!node.state.isSelected) {
          controller.selectNodesById({node.id});
        }
      },
      onPanUpdate: (details) => controller.dragSelection(details.delta),
      child: IntrinsicHeight(
        child: IntrinsicWidth(
          child: Stack(
            key: node.key,
            clipBehavior: Clip.none,
            children: [
              Container(decoration: node.builtStyle.decoration),
              Column(
                mainAxisSize: MainAxisSize.min,
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  DiagramNodeHeader(
                    node: node,
                    onToggleCollapse: () => controller
                        .toggleCollapseSelectedNodes(!node.state.isCollapsed),
                  ),
                  Offstage(
                    offstage: node.state.isCollapsed,
                    child: Padding(
                      padding: const EdgeInsets.all(12),
                      child: Row(
                        mainAxisAlignment: MainAxisAlignment.spaceBetween,
                        children: [
                          Flexible(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                for (final port in inPorts)
                                  _MoveOnlyPort(port: port),
                              ],
                            ),
                          ),
                          const SizedBox(width: 16),
                          Flexible(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.end,
                              children: [
                                for (final port in outPorts)
                                  _MoveOnlyPort(port: port),
                              ],
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _MoveOnlyPort extends StatelessWidget {
  final FlPortDataModel port;

  const _MoveOnlyPort({required this.port});

  @override
  Widget build(BuildContext context) {
    final isInput = port.prototype.direction == FlPortDirection.input;

    return Row(
      key: port.key,
      mainAxisAlignment:
          isInput ? MainAxisAlignment.start : MainAxisAlignment.end,
      mainAxisSize: MainAxisSize.min,
      children: [
        Flexible(
          child: Text(
            port.prototype.displayName(context),
            style: const TextStyle(color: Colors.white70, fontSize: 13),
            overflow: TextOverflow.ellipsis,
            textAlign: isInput ? TextAlign.left : TextAlign.right,
          ),
        ),
      ],
    );
  }
}
