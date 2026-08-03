import 'dart:ui';

import 'port_definition.dart';

/// Eine einzelne Node-Instanz im Diagramm ("Kästchen").
class DiagramNode {
  final String id;

  /// Verweist auf den registrierten Node-Typ (z.B. in der Palette), von dem
  /// diese Instanz erzeugt wurde. Nicht zu verwechseln mit [id].
  final String typeId;

  final String title;
  final Offset position;
  final List<PortDefinition> inputs;
  final List<PortDefinition> outputs;

  const DiagramNode({
    required this.id,
    required this.typeId,
    required this.title,
    required this.position,
    this.inputs = const [],
    this.outputs = const [],
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
      inputs: inputs,
      outputs: outputs,
    );
  }

  Map<String, dynamic> toJson() => {
        'id': id,
        'typeId': typeId,
        'title': title,
        'position': {'dx': position.dx, 'dy': position.dy},
        'inputs': inputs.map((port) => port.toJson()).toList(),
        'outputs': outputs.map((port) => port.toJson()).toList(),
      };

  factory DiagramNode.fromJson(Map<String, dynamic> json) {
    final positionJson = json['position'] as Map<String, dynamic>;
    return DiagramNode(
      id: json['id'] as String,
      typeId: json['typeId'] as String,
      title: json['title'] as String,
      position: Offset(
        (positionJson['dx'] as num).toDouble(),
        (positionJson['dy'] as num).toDouble(),
      ),
      inputs: (json['inputs'] as List<dynamic>)
          .map((e) => PortDefinition.fromJson(e as Map<String, dynamic>))
          .toList(),
      outputs: (json['outputs'] as List<dynamic>)
          .map((e) => PortDefinition.fromJson(e as Map<String, dynamic>))
          .toList(),
    );
  }
}
