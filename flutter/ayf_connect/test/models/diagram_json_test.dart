import 'package:flutter_test/flutter_test.dart';

import 'package:connectorflo/models/diagram.dart';
import 'package:connectorflo/models/diagram_edge.dart';
import 'package:connectorflo/models/diagram_node.dart';
import 'package:connectorflo/models/port_definition.dart';
import 'package:connectorflo/models/port_type.dart';

void main() {
  test('Diagram JSON round-trip erhält alle Felder', () {
    final diagram = Diagram(
      nodes: [
        DiagramNode(
          id: 'n1',
          typeId: 'source',
          title: 'Source',
          position: const Offset(10, 20),
          outputs: const [
            PortDefinition(
              id: 'out',
              label: 'Out',
              type: PortType.string,
              direction: PortDirection.output,
            ),
          ],
        ),
        DiagramNode(
          id: 'n2',
          typeId: 'sink',
          title: 'Sink',
          position: const Offset(300, 20),
          inputs: const [
            PortDefinition(
              id: 'in',
              label: 'In',
              type: PortType.string,
              direction: PortDirection.input,
            ),
          ],
        ),
      ],
      edges: const [
        DiagramEdge(
          id: 'e1',
          fromNodeId: 'n1',
          fromPortId: 'out',
          toNodeId: 'n2',
          toPortId: 'in',
        ),
      ],
    );

    final restored = Diagram.fromJson(diagram.toJson());

    expect(restored.nodes.length, 2);
    expect(restored.edges.length, 1);
    expect(restored.nodes[0].id, 'n1');
    expect(restored.nodes[0].position, const Offset(10, 20));
    expect(restored.nodes[0].outputs.single.type, PortType.string);
    expect(restored.edges.single.fromPortId, 'out');
    expect(restored.edges.single.toPortId, 'in');
  });
}
