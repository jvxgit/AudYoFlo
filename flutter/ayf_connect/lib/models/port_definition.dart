import 'port_type.dart';

/// Beschreibt einen einzelnen Connector-Anschluss eines Nodes.
class PortDefinition {
  final String id;
  final String label;
  final PortType type;
  final PortDirection direction;

  const PortDefinition({
    required this.id,
    required this.label,
    required this.type,
    required this.direction,
  });

  Map<String, dynamic> toJson() => {
        'id': id,
        'label': label,
        'type': type.toJson(),
        'direction': direction.toJson(),
      };

  factory PortDefinition.fromJson(Map<String, dynamic> json) {
    return PortDefinition(
      id: json['id'] as String,
      label: json['label'] as String,
      type: PortType.fromJson(json['type'] as String),
      direction: PortDirection.fromJson(json['direction'] as String),
    );
  }

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is PortDefinition &&
          runtimeType == other.runtimeType &&
          id == other.id;

  @override
  int get hashCode => id.hashCode;
}
