import 'package:flutter_test/flutter_test.dart';

import 'package:connectorflo/models/port_definition.dart';
import 'package:connectorflo/models/port_type.dart';
import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/state/invalid_connection_exception.dart';

const _stringOut = PortDefinition(
  id: 'out',
  label: 'Out',
  type: PortType.string,
  direction: PortDirection.output,
);
const _stringIn = PortDefinition(
  id: 'in',
  label: 'In',
  type: PortType.string,
  direction: PortDirection.input,
);
const _numberIn = PortDefinition(
  id: 'in',
  label: 'In',
  type: PortType.number,
  direction: PortDirection.input,
);

void main() {
  group('DiagramController Nodes', () {
    test('addNode fügt Node hinzu und benachrichtigt Listener', () {
      final controller = DiagramController();
      var notified = false;
      controller.addListener(() => notified = true);

      final node = controller.addNode(
        typeId: 'source',
        title: 'Source',
        position: const Offset(0, 0),
        outputs: const [_stringOut],
      );

      expect(controller.nodes, [node]);
      expect(notified, isTrue);
    });

    test('moveNode aktualisiert die Position', () {
      final controller = DiagramController();
      final node = controller.addNode(
        typeId: 'source',
        title: 'Source',
        position: const Offset(0, 0),
      );

      controller.moveNode(node.id, const Offset(50, 75));

      expect(controller.nodeById(node.id)!.position, const Offset(50, 75));
    });

    test('removeNode entfernt Node und daran hängende Edges', () {
      final controller = DiagramController();
      final source = controller.addNode(
        typeId: 'source',
        title: 'Source',
        position: const Offset(0, 0),
        outputs: const [_stringOut],
      );
      final sink = controller.addNode(
        typeId: 'sink',
        title: 'Sink',
        position: const Offset(200, 0),
        inputs: const [_stringIn],
      );
      controller.addEdge(
        fromNodeId: source.id,
        fromPortId: 'out',
        toNodeId: sink.id,
        toPortId: 'in',
      );

      controller.removeNode(source.id);

      expect(controller.nodes, [sink]);
      expect(controller.edges, isEmpty);
    });
  });

  group('DiagramController Edges', () {
    late DiagramController controller;

    setUp(() {
      controller = DiagramController();
    });

    test('addEdge verbindet kompatible Ports', () {
      final source = controller.addNode(
        typeId: 'source',
        title: 'Source',
        position: const Offset(0, 0),
        outputs: const [_stringOut],
      );
      final sink = controller.addNode(
        typeId: 'sink',
        title: 'Sink',
        position: const Offset(200, 0),
        inputs: const [_stringIn],
      );

      final edge = controller.addEdge(
        fromNodeId: source.id,
        fromPortId: 'out',
        toNodeId: sink.id,
        toPortId: 'in',
      );

      expect(controller.edges, [edge]);
    });

    test('addEdge lehnt inkompatible Port-Typen ab', () {
      final source = controller.addNode(
        typeId: 'source',
        title: 'Source',
        position: const Offset(0, 0),
        outputs: const [_stringOut],
      );
      final sink = controller.addNode(
        typeId: 'sink',
        title: 'Sink',
        position: const Offset(200, 0),
        inputs: const [_numberIn],
      );

      expect(
        () => controller.addEdge(
          fromNodeId: source.id,
          fromPortId: 'out',
          toNodeId: sink.id,
          toPortId: 'in',
        ),
        throwsA(isA<InvalidConnectionException>()),
      );
      expect(controller.edges, isEmpty);
    });

    test('addEdge lehnt Selbstverbindung ab', () {
      final node = controller.addNode(
        typeId: 'transform',
        title: 'Transform',
        position: const Offset(0, 0),
        inputs: const [_stringIn],
        outputs: const [_stringOut],
      );

      expect(
        () => controller.addEdge(
          fromNodeId: node.id,
          fromPortId: 'out',
          toNodeId: node.id,
          toPortId: 'in',
        ),
        throwsA(isA<InvalidConnectionException>()),
      );
    });

    test('addEdge lehnt Duplikate ab', () {
      final source = controller.addNode(
        typeId: 'source',
        title: 'Source',
        position: const Offset(0, 0),
        outputs: const [_stringOut],
      );
      final sink = controller.addNode(
        typeId: 'sink',
        title: 'Sink',
        position: const Offset(200, 0),
        inputs: const [_stringIn],
      );
      controller.addEdge(
        fromNodeId: source.id,
        fromPortId: 'out',
        toNodeId: sink.id,
        toPortId: 'in',
      );

      expect(
        () => controller.addEdge(
          fromNodeId: source.id,
          fromPortId: 'out',
          toNodeId: sink.id,
          toPortId: 'in',
        ),
        throwsA(isA<InvalidConnectionException>()),
      );
      expect(controller.edges, hasLength(1));
    });

    test('addEdge lehnt falsche Richtung ab (input -> input)', () {
      final a = controller.addNode(
        typeId: 'sink',
        title: 'A',
        position: const Offset(0, 0),
        inputs: const [_stringIn],
      );
      final b = controller.addNode(
        typeId: 'sink',
        title: 'B',
        position: const Offset(200, 0),
        inputs: const [_stringIn],
      );

      expect(
        () => controller.addEdge(
          fromNodeId: a.id,
          fromPortId: 'in',
          toNodeId: b.id,
          toPortId: 'in',
        ),
        throwsA(isA<InvalidConnectionException>()),
      );
    });

    test('removeEdge entfernt eine bestehende Verbindung', () {
      final source = controller.addNode(
        typeId: 'source',
        title: 'Source',
        position: const Offset(0, 0),
        outputs: const [_stringOut],
      );
      final sink = controller.addNode(
        typeId: 'sink',
        title: 'Sink',
        position: const Offset(200, 0),
        inputs: const [_stringIn],
      );
      final edge = controller.addEdge(
        fromNodeId: source.id,
        fromPortId: 'out',
        toNodeId: sink.id,
        toPortId: 'in',
      );

      controller.removeEdge(edge.id);

      expect(controller.edges, isEmpty);
    });
  });
}
