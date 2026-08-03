import '../models/port_type.dart';
import 'node_type_definition.dart';

/// Vorläufiger Katalog verfügbarer Node-Typen. Löst die im Spike (Meilenstein 1)
/// hart codierten Nodes ab; wird in Meilenstein 5 von der Palette angezeigt.
const List<NodeTypeDefinition> defaultNodeTypes = [
  NodeTypeDefinition(
    typeId: 'source',
    label: 'Source',
    outputs: [
      PortSpec(id: 'out', label: 'Out', type: PortType.string),
    ],
  ),
  NodeTypeDefinition(
    typeId: 'transform',
    label: 'Transform',
    inputs: [
      PortSpec(id: 'in', label: 'In', type: PortType.string),
    ],
    outputs: [
      PortSpec(id: 'out', label: 'Out', type: PortType.string),
    ],
  ),
  NodeTypeDefinition(
    typeId: 'sink',
    label: 'Sink',
    inputs: [
      PortSpec(id: 'in', label: 'In', type: PortType.string),
    ],
  ),
];
