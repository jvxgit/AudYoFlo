import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';

import 'node_header.dart';

/// Ersetzt fl_nodes' Standard-Node-Widget im Read-Only-Modus (über
/// FlNodeEditorWidget.nodeBuilder). Das ist bewusst ein Neuaufbau statt
/// einer Konfiguration: fl_nodes hat kein Flag, um Verschieben/Verbinden
/// abzuschalten — der komplette Gesture-Code dafür hängt am Standard-Node-
/// Widget (siehe DefaultNodeWidget.controlsWrapper), das nodeBuilder
/// vollständig ersetzt statt es zu erweitern. Wir bauen daher nur den
/// Anzeige-Teil nach (Layout, Styling, Ports inkl. deren GlobalKeys, damit
/// bestehende Verbindungslinien weiterhin korrekt gezeichnet werden) und
/// verzichten bewusst auf Drag-Gesten. Klick-Auswahl bleibt erhalten, damit
/// der Inspector weiterhin Node-Details anzeigen kann.
class ReadOnlyNode extends StatelessWidget {
  final FlNodeEditorController controller;
  final FlNodeDataModel node;

  const ReadOnlyNode({
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
                                  _ReadOnlyPort(port: port),
                              ],
                            ),
                          ),
                          const SizedBox(width: 16),
                          Flexible(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.end,
                              children: [
                                for (final port in outPorts)
                                  _ReadOnlyPort(port: port),
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

class _ReadOnlyPort extends StatelessWidget {
  final FlPortDataModel port;

  const _ReadOnlyPort({required this.port});

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
