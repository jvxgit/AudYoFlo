import 'dart:ui';

import 'port_definition.dart';

/// Eine einzelne Node-Instanz im Diagramm ("Kästchen").
class DiagramNode {
  final String id;

  /// Verweist auf den registrierten Node-Typ (z.B. in der Palette), von dem
  /// diese Instanz erzeugt wurde. Nicht zu verwechseln mit [id].
  final String typeId;

  final String title;

  /// Optionale, extern gesetzte "echte" Typ-Kennung dieser Node-Instanz,
  /// unabhängig von [typeId] (das nur auf den registrierten Prototyp
  /// verweist). Gedacht für Nodes, die aus einer Backend-Quelle stammen und
  /// dort eine eigene, systemweit eindeutige Typ-Identifikation tragen (z.B.
  /// die jvxComponentIdentification einer AudYoFlo-Komponente). Wird im
  /// [NodeInspector] anstelle von [typeId] angezeigt, wenn gesetzt.
  final String? typeIdentification;

  final Offset position;
  final List<PortDefinition> inputs;
  final List<PortDefinition> outputs;

  /// Optionale Hervorhebungsfarbe, unabhängig vom (pro Typ geteilten)
  /// Node-Styling. Gedacht dafür, mehrere Node-Instanzen, die dieselbe
  /// reale Entität repräsentieren (z.B. dieselbe Backend-Komponente, die an
  /// mehreren Stellen einer Kette auftaucht), optisch als zusammengehörig zu
  /// kennzeichnen, ohne sie zu einer einzigen Node zusammenzufassen.
  final Color? groupColor;

  const DiagramNode({
    required this.id,
    required this.typeId,
    required this.title,
    required this.position,
    this.typeIdentification,
    this.inputs = const [],
    this.outputs = const [],
    this.groupColor,
  });

  PortDefinition? findPort(String portId) {
    for (final port in inputs) {
      if (port.id == portId) return port;
    }
    for (final port in outputs) {
      if (port.id == portId) return port;
    }
    return null;
  }

  DiagramNode copyWith({
    String? title,
    Offset? position,
  }) {
    return DiagramNode(
      id: id,
      typeId: typeId,
      title: title ?? this.title,
      position: position ?? this.position,
      typeIdentification: typeIdentification,
      inputs: inputs,
      outputs: outputs,
      groupColor: groupColor,
    );
  }

  Map<String, dynamic> toJson() => {
        'id': id,
        'typeId': typeId,
        'title': title,
        'position': {'dx': position.dx, 'dy': position.dy},
        if (typeIdentification != null)
          'typeIdentification': typeIdentification,
        'inputs': inputs.map((port) => port.toJson()).toList(),
        'outputs': outputs.map((port) => port.toJson()).toList(),
        if (groupColor != null) 'groupColor': groupColor!.toARGB32(),
      };

  factory DiagramNode.fromJson(Map<String, dynamic> json) {
    final positionJson = json['position'] as Map<String, dynamic>;
    final groupColorValue = json['groupColor'] as int?;
    return DiagramNode(
      id: json['id'] as String,
      typeId: json['typeId'] as String,
      title: json['title'] as String,
      position: Offset(
        (positionJson['dx'] as num).toDouble(),
        (positionJson['dy'] as num).toDouble(),
      ),
      typeIdentification: json['typeIdentification'] as String?,
      inputs: (json['inputs'] as List<dynamic>)
          .map((e) => PortDefinition.fromJson(e as Map<String, dynamic>))
          .toList(),
      outputs: (json['outputs'] as List<dynamic>)
          .map((e) => PortDefinition.fromJson(e as Map<String, dynamic>))
          .toList(),
      groupColor: groupColorValue != null ? Color(groupColorValue) : null,
    );
  }
}
