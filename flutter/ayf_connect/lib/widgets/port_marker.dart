import 'package:flutter/material.dart';

import '../models/port_type.dart';

/// Horizontaler Versatz (in logischen Pixeln), mit dem die Port-Markierung
/// aus dem Node-Inhalt heraus zurück auf die Node-Kante geschoben wird -
/// dorthin, wo fl_nodes den Verbindungspunkt ankert und früher den runden
/// Port-Punkt gezeichnet hat. Entspricht dem `EdgeInsets.all(12)`, mit dem
/// sowohl `RestrictedNode` als auch fl_nodes' Standard-Node-Widget den
/// Port-Bereich vom Rand einrücken. Wird per [Transform.translate]
/// angewendet (nur Darstellung; die für das Link-Ankern relevante
/// Port-Geometrie bleibt unberührt).
const double kPortMarkerEdgeInset = 12.0;

/// Kleine, drehbare Dreiecks-Markierung für einen Port. Ersetzt optisch den
/// runden Punkt, den fl_nodes sonst pro Port zeichnet (dessen eingebautes
/// Dreieck zeigt fix nach +x und lässt sich nicht drehen) — deshalb wird der
/// native Marker unterdrückt (Port-Style `radius: 0`, siehe
/// `FlNodesAdapter`) und stattdessen dieses Widget im `nodeBuilder`/
/// `portBuilder` mitgerendert.
///
/// [direction] bestimmt, wohin die Spitze zeigt; das Basis-Icon
/// ([Icons.play_arrow]) zeigt nach rechts und wird über
/// [PortMarkerDirection.quarterTurns] via [RotatedBox] gedreht.
class PortMarker extends StatelessWidget {
  final PortMarkerDirection direction;
  final double size;
  final Color color;

  const PortMarker({
    super.key,
    this.direction = PortMarkerDirection.right,
    this.size = 24,
    this.color = const Color(0xFF42A5F5),
  });

  @override
  Widget build(BuildContext context) {
    return RotatedBox(
      quarterTurns: direction.quarterTurns,
      child: Icon(Icons.play_arrow, size: size, color: color),
    );
  }
}
