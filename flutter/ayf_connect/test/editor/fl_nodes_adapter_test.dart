import 'package:flutter_test/flutter_test.dart';

import 'package:connectorflo/editor/default_node_types.dart';
import 'package:connectorflo/editor/fl_nodes_adapter.dart';
import 'package:connectorflo/models/diagram.dart';
import 'package:connectorflo/models/diagram_edge.dart';
import 'package:connectorflo/models/diagram_node.dart';
import 'package:connectorflo/models/port_definition.dart';
import 'package:connectorflo/models/port_type.dart';
import 'package:connectorflo/state/diagram_controller.dart';

void main() {
  late DiagramController diagramController;
  late FlNodesAdapter adapter;

  setUp(() {
    diagramController = DiagramController();
    adapter = FlNodesAdapter(
      diagramController: diagramController,
      nodeTypes: defaultNodeTypes,
    );
  });

  tearDown(() {
    adapter.dispose();
  });

  test('Node-Erstellung in fl_nodes wird in den DiagramController gespiegelt',
      () async {
    final flNode = adapter.flController.addNode('source');
    await Future.delayed(Duration.zero);

    expect(diagramController.nodes, hasLength(1));
    final mirrored = diagramController.nodes.single;
    expect(mirrored.id, flNode.id);
    expect(mirrored.typeId, 'source');
    expect(mirrored.outputs.single.id, 'out');
  });

  test('Verbindung in fl_nodes wird als Edge gespiegelt', () async {
    final source = adapter.flController.addNode('source');
    final sink = adapter.flController.addNode('sink');
    await Future.delayed(Duration.zero);

    adapter.flController.addLink(source.id, 'out', sink.id, 'in');
    await Future.delayed(Duration.zero);

    expect(diagramController.edges, hasLength(1));
    final edge = diagramController.edges.single;
    expect(edge.fromNodeId, source.id);
    expect(edge.toNodeId, sink.id);
  });

  test('Entfernen eines Links in fl_nodes entfernt die Edge im Controller',
      () async {
    final source = adapter.flController.addNode('source');
    final sink = adapter.flController.addNode('sink');
    await Future.delayed(Duration.zero);
    final link = adapter.flController.addLink(source.id, 'out', sink.id, 'in');
    await Future.delayed(Duration.zero);
    expect(diagramController.edges, hasLength(1));

    adapter.flController.removeLinkById(link!.id);
    await Future.delayed(Duration.zero);

    expect(diagramController.edges, isEmpty);
  });

  test('Entfernen eines Node in fl_nodes entfernt ihn auch im Controller',
      () async {
    final node = adapter.flController.addNode('source');
    await Future.delayed(Duration.zero);
    expect(diagramController.nodes, hasLength(1));

    adapter.flController.removeNodeById(node.id);
    await Future.delayed(Duration.zero);

    expect(diagramController.nodes, isEmpty);
  });

  test('Verschieben in fl_nodes aktualisiert die Position im Controller',
      () async {
    final node = adapter.flController.addNode('source', offset: Offset.zero);
    await Future.delayed(Duration.zero);

    // fl_nodes rastet Positionen standardmäßig auf ein 64px-Raster ein
    // (FlNodeEditorConfig.snapToGridSize), daher ein rasterkonformes Delta.
    adapter.flController.selectNodesById({node.id});
    adapter.flController.dragSelection(
      const Offset(64, 128),
      isWorldDelta: true,
    );
    await Future.delayed(Duration.zero);

    expect(
      diagramController.nodeById(node.id)!.position,
      const Offset(64, 128),
    );
  });

  test('Auswahl in fl_nodes wird im Controller gespiegelt', () async {
    final node = adapter.flController.addNode('source');
    await Future.delayed(Duration.zero);

    adapter.flController.selectNodesById({node.id});
    await Future.delayed(Duration.zero);
    expect(diagramController.selectedNodeId, node.id);

    adapter.flController.selectNodesById({});
    await Future.delayed(Duration.zero);
    expect(diagramController.selectedNodeId, isNull);
  });

  group('loadDiagram', () {
    const sourcePort = PortDefinition(
      id: 'out',
      label: 'Out',
      type: PortType.string,
      direction: PortDirection.output,
    );
    const sinkPort = PortDefinition(
      id: 'in',
      label: 'In',
      type: PortType.string,
      direction: PortDirection.input,
    );

    Diagram sampleDiagram() {
      return Diagram(
        nodes: [
          DiagramNode(
            id: 'external-source',
            typeId: 'source',
            title: 'Source',
            // fl_nodes rastet Positionen standardmäßig auf ein 64px-Raster
            // ein (FlNodeEditorConfig.snapToGridSize), daher rasterkonform.
            position: const Offset(64, 64),
            outputs: const [sourcePort],
          ),
          DiagramNode(
            id: 'external-sink',
            typeId: 'sink',
            title: 'Sink',
            position: const Offset(256, 64),
            inputs: const [sinkPort],
          ),
        ],
        edges: const [
          DiagramEdge(
            id: 'external-edge',
            fromNodeId: 'external-source',
            fromPortId: 'out',
            toNodeId: 'external-sink',
            toPortId: 'in',
          ),
        ],
      );
    }

    test('baut Nodes und Edges im Canvas auf und spiegelt sie', () async {
      adapter.loadDiagram(sampleDiagram());
      await Future.delayed(Duration.zero);

      expect(adapter.flController.nodes, hasLength(2));
      expect(adapter.flController.links, hasLength(1));

      expect(diagramController.nodes, hasLength(2));
      expect(diagramController.edges, hasLength(1));

      final source = diagramController.nodes
          .firstWhere((node) => node.typeId == 'source');
      final sink =
          diagramController.nodes.firstWhere((node) => node.typeId == 'sink');
      expect(source.position, const Offset(64, 64));
      final edge = diagramController.edges.single;
      expect(edge.fromNodeId, source.id);
      expect(edge.toNodeId, sink.id);
      // fl_nodes vergibt eigene IDs - die ursprünglichen IDs aus dem
      // geladenen Diagram tauchen nicht mehr auf.
      expect(source.id, isNot('external-source'));
    });

    test('ersetzt bestehenden Canvas-Inhalt statt ihn zu ergänzen', () async {
      adapter.flController.addNode('transform');
      await Future.delayed(Duration.zero);
      expect(diagramController.nodes, hasLength(1));

      adapter.loadDiagram(sampleDiagram());
      await Future.delayed(Duration.zero);

      expect(diagramController.nodes, hasLength(2));
      expect(
        diagramController.nodes.any((node) => node.typeId == 'transform'),
        isFalse,
      );
    });

    test('wirft ArgumentError bei unbekanntem Node-Typ und ändert nichts',
        () async {
      adapter.flController.addNode('source');
      await Future.delayed(Duration.zero);

      final badDiagram = Diagram(
        nodes: [
          DiagramNode(
            id: 'n1',
            typeId: 'does-not-exist',
            title: 'Unbekannt',
            position: Offset.zero,
          ),
        ],
      );

      expect(() => adapter.loadDiagram(badDiagram), throwsArgumentError);
      // Unverändert, weil die Validierung vor jeder Mutation läuft.
      expect(adapter.flController.nodes, hasLength(1));
    });

    test('wirft ArgumentError bei Edge auf unbekannten Node', () async {
      final badDiagram = Diagram(
        nodes: [
          DiagramNode(
            id: 'n1',
            typeId: 'source',
            title: 'Source',
            position: Offset.zero,
            outputs: const [sourcePort],
          ),
        ],
        edges: const [
          DiagramEdge(
            id: 'e1',
            fromNodeId: 'n1',
            fromPortId: 'out',
            toNodeId: 'does-not-exist',
            toPortId: 'in',
          ),
        ],
      );

      expect(() => adapter.loadDiagram(badDiagram), throwsArgumentError);
      expect(adapter.flController.nodes, isEmpty);
    });
  });
}
