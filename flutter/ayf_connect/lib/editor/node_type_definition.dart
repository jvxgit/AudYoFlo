import '../models/port_type.dart';

/// Beschreibt einen in der Palette verfügbaren Node-Typ: welche Ports er hat
/// und wie er dargestellt wird. Dient sowohl der fl_nodes-Registrierung
/// (Adapter) als auch der Palette (Milestone 5) als gemeinsame Quelle.
class NodeTypeDefinition {
  final String typeId;
  final String label;
  final List<PortSpec> inputs;
  final List<PortSpec> outputs;

  const NodeTypeDefinition({
    required this.typeId,
    required this.label,
    this.inputs = const [],
    this.outputs = const [],
  });
}

class PortSpec {
  final String id;
  final String label;
  final PortType type;

  const PortSpec({
    required this.id,
    required this.label,
    required this.type,
  });
}
