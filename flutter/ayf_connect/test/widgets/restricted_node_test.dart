import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

import 'package:connectorflo/models/diagram_node.dart';
import 'package:connectorflo/models/port_definition.dart';
import 'package:connectorflo/models/port_type.dart';
import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/widgets/restricted_node.dart';

// RestrictedNode enthält DiagramNodeHeader, das seinen Titel über
// Provider<DiagramController> liest — daher brauchen auch diese isolierten
// Tests einen (leeren) Provider in der Widget-Umgebung.
Widget _wrapNode(Widget node, {DiagramController? diagramController}) {
  return ChangeNotifierProvider<DiagramController>(
    create: (_) => diagramController ?? DiagramController(),
    child: MaterialApp(
      home: Scaffold(body: Center(child: node)),
    ),
  );
}

FlNodeEditorController _controllerWithSourceSinkPrototypes() {
  final controller = FlNodeEditorController(
    config: const FlNodeEditorConfig(
      autoBuildGraph: false,
      autoRunGraph: false,
    ),
  );

  controller.registerNodePrototype(
    FlNodePrototype(
      idName: 'source',
      displayName: (context) => 'Source',
      description: (context) => 'Source',
      ports: [
        FlDataOutputPortPrototype<String>(
          idName: 'out',
          displayName: (context) => 'Out',
          styleBuilder: flDefaultPortStyleBuilder,
        ),
      ],
      onExecute: (ports, fields, state, f, p) async {},
    ),
  );

  return controller;
}

// In der echten App initialisiert fl_nodes' interne DefaultNodeWidget-
// Pipeline builtStyle/builtHeaderStyle in initState(), bevor sie den Node an
// nodeBuilder übergibt. Da diese Tests RestrictedNode bewusst isoliert (ohne
// FlNodeEditorWidget drumherum) rendern, holen wir das hier manuell nach.
void _primeStyles(FlNodeDataModel node) {
  node.builtStyle = node.prototype.styleBuilder(node.state);
  node.builtHeaderStyle = node.prototype.headerStyleBuilder(node.state);
}

void main() {
  // Verhalten, das für Move-Only und Read-Only identisch sein muss (Auswahl,
  // Farbe, Port-Beschriftung) läuft hier für beide movable-Werte durch, damit
  // ein künftiger Regressionstest nicht - wie es zweimal passiert ist, als
  // move_only_node.dart und read_only_node.dart noch getrennte Kopien waren -
  // nur für eine der beiden Varianten geschrieben wird und die andere
  // stillschweigend zurückfällt.
  for (final movable in [true, false]) {
    testWidgets(
      'Tippen auf RestrictedNode(movable: $movable) wählt den Node aus',
      (tester) async {
        final controller = _controllerWithSourceSinkPrototypes();
        addTearDown(controller.dispose);
        final node = controller.addNode('source');
        _primeStyles(node);

        await tester.pumpWidget(
          _wrapNode(
            RestrictedNode(
              controller: controller,
              node: node,
              movable: movable,
            ),
          ),
        );

        expect(find.text('Source'), findsOneWidget);
        expect(find.text('Out'), findsOneWidget);
        expect(controller.selectedNodeIds, isEmpty);

        await tester.tap(find.text('Source'));
        await tester.pump();

        expect(controller.selectedNodeIds, {node.id});
      },
    );

    testWidgets(
      'RestrictedNode(movable: $movable) zeigt die groupColor aus dem '
      'DiagramController',
      (tester) async {
        final controller = _controllerWithSourceSinkPrototypes();
        addTearDown(controller.dispose);
        final node = controller.addNode('source');
        _primeStyles(node);

        final diagramController = DiagramController();
        diagramController.addNode(
          id: node.id,
          typeId: 'source',
          title: 'Source',
          position: Offset.zero,
        );
        diagramController.applyNodeOverrides(
          node.id,
          DiagramNode(
            id: node.id,
            typeId: 'source',
            title: 'Source',
            position: Offset.zero,
            groupColor: Colors.red,
          ),
        );

        await tester.pumpWidget(
          _wrapNode(
            RestrictedNode(
              controller: controller,
              node: node,
              movable: movable,
            ),
            diagramController: diagramController,
          ),
        );

        final container =
            tester.widget<Container>(find.byType(Container).first);
        final decoration = container.decoration as BoxDecoration;
        expect(decoration.color, Colors.red);
      },
    );

    testWidgets(
      'RestrictedNode(movable: $movable) zeigt den echten Connector-Namen '
      'aus dem DiagramController statt des generischen Prototyp-Labels "Out"',
      (tester) async {
        final controller = _controllerWithSourceSinkPrototypes();
        addTearDown(controller.dispose);
        final node = controller.addNode('source');
        _primeStyles(node);

        final diagramController = DiagramController();
        diagramController.addNode(
          id: node.id,
          typeId: 'source',
          title: 'Source',
          position: Offset.zero,
          outputs: const [
            PortDefinition(
              id: 'out',
              label: 'Mikrofon Eingang',
              type: PortType.any,
              direction: PortDirection.output,
            ),
          ],
        );

        await tester.pumpWidget(
          _wrapNode(
            RestrictedNode(
              controller: controller,
              node: node,
              movable: movable,
            ),
            diagramController: diagramController,
          ),
        );

        expect(find.text('Mikrofon Eingang'), findsOneWidget);
        expect(find.text('Out'), findsNothing);
      },
    );

    testWidgets(
      'RestrictedNode(movable: $movable) dreht die Port-Markierung gemäß '
      'markerDirection aus dem DiagramController',
      (tester) async {
        final controller = _controllerWithSourceSinkPrototypes();
        addTearDown(controller.dispose);
        final node = controller.addNode('source');
        _primeStyles(node);

        final diagramController = DiagramController();
        diagramController.addNode(
          id: node.id,
          typeId: 'source',
          title: 'Source',
          position: Offset.zero,
          outputs: const [
            PortDefinition(
              id: 'out',
              label: 'Out',
              type: PortType.any,
              direction: PortDirection.output,
              markerDirection: PortMarkerDirection.left,
            ),
          ],
        );

        await tester.pumpWidget(
          _wrapNode(
            RestrictedNode(
              controller: controller,
              node: node,
              movable: movable,
            ),
            diagramController: diagramController,
          ),
        );

        final rotatedBox = tester.widget<RotatedBox>(
          find.ancestor(
            of: find.byIcon(Icons.play_arrow),
            matching: find.byType(RotatedBox),
          ),
        );
        expect(rotatedBox.quarterTurns, PortMarkerDirection.left.quarterTurns);
      },
    );
  }

  testWidgets(
    'Ziehen auf RestrictedNode(movable: true) verschiebt den Node',
    (tester) async {
      final controller = _controllerWithSourceSinkPrototypes();
      addTearDown(controller.dispose);
      final node = controller.addNode('source', offset: Offset.zero);
      _primeStyles(node);

      await tester.pumpWidget(
        _wrapNode(
          RestrictedNode(controller: controller, node: node, movable: true),
        ),
      );

      // Grid-ausgerichteter Delta (snapToGridSize = 64), siehe CLAUDE.md.
      await tester.drag(find.text('Source'), const Offset(128, 64));
      await tester.pump();

      expect(node.offset, const Offset(128, 64));
      expect(controller.selectedNodeIds, {node.id});
    },
  );

  testWidgets(
    'Ziehen auf RestrictedNode(movable: false) verschiebt den Node nicht',
    (tester) async {
      final controller = _controllerWithSourceSinkPrototypes();
      addTearDown(controller.dispose);
      final node = controller.addNode('source', offset: Offset.zero);
      _primeStyles(node);
      final originalOffset = node.offset;

      await tester.pumpWidget(
        _wrapNode(
          RestrictedNode(controller: controller, node: node, movable: false),
        ),
      );

      await tester.drag(find.text('Source'), const Offset(100, 60));
      await tester.pump();

      expect(node.offset, originalOffset);
    },
  );

  testWidgets(
    'Ziehen an einem Node, der Teil einer vorbestehenden Mehrfachauswahl '
    'ist (z.B. durch fl_nodes\' Flächenauswahl auf dem Hintergrund), '
    'bewegt nur diesen Node - nicht die restliche alte Auswahl mit',
    (tester) async {
      final controller = _controllerWithSourceSinkPrototypes();
      addTearDown(controller.dispose);
      final nodeA = controller.addNode('source', offset: Offset.zero);
      final nodeB = controller.addNode('source', offset: const Offset(320, 0));
      _primeStyles(nodeA);
      _primeStyles(nodeB);

      // Simuliert eine vorbestehende Mehrfachauswahl beider Nodes, wie sie
      // fl_nodes' Flächenauswahl (Ziehen auf leerem Hintergrund,
      // standardmäßig in jedem Editor-Modus aktiv) erzeugen kann - ohne
      // dass hier je onTap/onPanStart auf einem der beiden Nodes lief.
      controller.selectNodesById({nodeA.id, nodeB.id});
      expect(controller.selectedNodeIds, {nodeA.id, nodeB.id});

      await tester.pumpWidget(
        _wrapNode(
          Column(
            children: [
              RestrictedNode(
                controller: controller,
                node: nodeA,
                movable: true,
              ),
              RestrictedNode(
                controller: controller,
                node: nodeB,
                movable: true,
              ),
            ],
          ),
        ),
      );

      await tester.drag(find.text('Source').first, const Offset(128, 64));
      await tester.pump();

      expect(
        nodeA.offset,
        const Offset(128, 64),
        reason: 'Der gezogene Node A sollte sich bewegt haben.',
      );
      expect(
        nodeB.offset,
        const Offset(320, 0),
        reason:
            'Node B wurde NICHT gezogen und sollte unverändert bleiben, '
            'auch wenn er vor dem Drag mitselektiert war.',
      );
      expect(controller.selectedNodeIds, {nodeA.id});
    },
  );
}
