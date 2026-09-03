import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../models/port_type.dart';
import '../state/diagram_controller.dart';
import 'node_header.dart';
import 'port_marker.dart';

/// Ersetzt fl_nodes' Standard-Node-Widget in den beiden nicht voll
/// editierbaren Canvas-Modi ([DiagramEditorMode.moveOnly]/
/// [DiagramEditorMode.readOnly], über `FlNodeEditorWidget.nodeBuilder`).
/// Bewusst ein Neuaufbau statt einer Konfiguration: fl_nodes hat kein Flag,
/// um Verschieben/Verbinden abzuschalten — der komplette Gesture-Code dafür
/// hängt am Standard-Node-Widget (siehe `DefaultNodeWidget.controlsWrapper`),
/// das `nodeBuilder` vollständig ersetzt statt es zu erweitern. Wir bauen
/// daher nur den Anzeige-Teil nach (Layout, Styling, Ports inkl. deren
/// GlobalKeys, damit bestehende Verbindungslinien weiterhin korrekt
/// gezeichnet werden) und verzichten bewusst auf die Port-Erkennung/
/// Linking-Logik des fl_nodes-Standard-Widgets
/// (`DefaultNodeWidget._isNearPort`/`_onTmpLink*`), damit Nodes hier nie
/// verbunden oder getrennt werden können — in keinem der beiden Modi.
///
/// [movable] ist der einzige Verhaltensunterschied zwischen den beiden
/// Modi und daher auch die einzige Verzweigung im Widget: `true`
/// (Move-Only) fügt eine Pan-Geste für `controller.dragSelection` hinzu,
/// `false` (Read-Only) lässt Nodes nur auswählbar (für den Inspector), aber
/// unbeweglich. Layout, Styling, Farbe und Port-Beschriftung sind für beide
/// Modi identisch und daher nicht dupliziert.
class RestrictedNode extends StatelessWidget {
  final FlNodeEditorController controller;
  final FlNodeDataModel node;
  final bool movable;

  const RestrictedNode({
    super.key,
    required this.controller,
    required this.node,
    required this.movable,
  });

  @override
  Widget build(BuildContext context) {
    final inPorts = node.ports.values
        .where((port) => port.prototype.direction == FlPortDirection.input)
        .toList();
    final outPorts = node.ports.values
        .where((port) => port.prototype.direction == FlPortDirection.output)
        .toList();

    // Per-instance override of the (per-type shared) prototype style - see
    // DiagramNode.groupColor - used to mark several node instances that
    // represent the same real-world entity as visually related.
    final groupColor =
        context.watch<DiagramController>().nodeById(node.id)?.groupColor;
    final decoration = groupColor != null
        ? node.builtStyle.decoration.copyWith(color: groupColor)
        : node.builtStyle.decoration;

    return GestureDetector(
      onTap: () => controller.selectNodesById({node.id}),
      // Nur im Move-Only-Modus gesetzt (sonst bleiben onPanStart/onPanUpdate
      // null, wodurch GestureDetector gar keinen Pan-Recognizer registriert -
      // exakt das bisherige ReadOnlyNode-Verhalten).
      //
      // Die Auswahl wird beim Pan-Start immer (nicht nur `if
      // (!node.state.isSelected)`, wie fl_nodes' eigenes DefaultNodeWidget es
      // für sein eingebautes Gruppen-Verschieben macht) auf genau diesen Node
      // eingeengt: fl_nodes' Flächenauswahl (Ziehen auf dem leeren
      // Hintergrund, standardmäßig aktiv und unabhängig vom Editor-Modus -
      // siehe _NodeEditorDataLayerState._onHighlightEnd,
      // FlNodeEditorConfig.enableAreaSelection) kann Nodes selektieren, ohne
      // dass hier je ein onTap/onPanStart auf ihnen lief. Ohne dieses
      // Erzwingen würde ein Drag, der zufällig auf einem so vorselektierten
      // Node beginnt, die GESAMTE alte Auswahl mitziehen statt nur den
      // gerade gegriffenen Node - Move-Only bietet (anders als der volle
      // Editor) keine Mehrfachauswahl-Semantik an und darf sie daher auch
      // nicht heimlich zulassen.
      onPanStart:
          movable ? (_) => controller.selectNodesById({node.id}) : null,
      onPanUpdate:
          movable ? (details) => controller.dragSelection(details.delta) : null,
      child: IntrinsicHeight(
        child: IntrinsicWidth(
          child: Stack(
            key: node.key,
            clipBehavior: Clip.none,
            children: [
              Container(decoration: decoration),
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
                                  _RestrictedPort(port: port, nodeId: node.id),
                              ],
                            ),
                          ),
                          const SizedBox(width: 16),
                          Flexible(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.end,
                              children: [
                                for (final port in outPorts)
                                  _RestrictedPort(port: port, nodeId: node.id),
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

class _RestrictedPort extends StatelessWidget {
  final FlPortDataModel port;
  final String nodeId;

  const _RestrictedPort({required this.port, required this.nodeId});

  @override
  Widget build(BuildContext context) {
    final isInput = port.prototype.direction == FlPortDirection.input;
    // DiagramNode.inputs/outputs carry this instance's own port label (e.g.
    // a real backend connector name) and marker direction; the prototype's
    // displayName is only the shared, per-type fallback (see node_header.dart
    // for the same pattern applied to the node title).
    final portDef = context
        .watch<DiagramController>()
        .nodeById(nodeId)
        ?.findPort(port.prototype.idName);
    final label = portDef?.label ?? port.prototype.displayName(context);
    final markerDirection =
        portDef?.markerDirection ?? PortMarkerDirection.right;

    // Transform.translate schiebt die Markierung optisch auf die Node-Kante
    // (dorthin, wo die Verbindungslinie andockt und fl_nodes früher den
    // runden Punkt zeichnete), ohne die Layout-Geometrie zu ändern, an der
    // fl_nodes die Port-Position berechnet.
    final marker = Transform.translate(
      offset: Offset(
        isInput ? -kPortMarkerEdgeInset : kPortMarkerEdgeInset,
        0,
      ),
      child: PortMarker(direction: markerDirection),
    );
    final text = Flexible(
      child: Text(
        label,
        style: const TextStyle(color: Colors.white70, fontSize: 13),
        overflow: TextOverflow.ellipsis,
        textAlign: isInput ? TextAlign.left : TextAlign.right,
      ),
    );

    return Row(
      key: port.key,
      mainAxisAlignment:
          isInput ? MainAxisAlignment.start : MainAxisAlignment.end,
      mainAxisSize: MainAxisSize.min,
      children: [
        // Marker at the node-edge side: left for inputs, right for outputs -
        // i.e. right where the link line meets the node.
        if (isInput) ...[marker, const SizedBox(width: 4), text] else ...[
          text,
          const SizedBox(width: 4),
          marker,
        ],
      ],
    );
  }
}
