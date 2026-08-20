import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/widgets/move_only_node.dart';

// MoveOnlyNode enthält DiagramNodeHeader, das seinen Titel über
// Provider<DiagramController> liest — daher brauchen auch diese isolierten
// Tests einen (leeren) Provider in der Widget-Umgebung.
Widget _wrapNode(Widget node) {
  return ChangeNotifierProvider<DiagramController>(
    create: (_) => DiagramController(),
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
// nodeBuilder übergibt. Da diese Tests MoveOnlyNode bewusst isoliert (ohne
// FlNodeEditorWidget drumherum) rendern, holen wir das hier manuell nach.
void _primeStyles(FlNodeDataModel node) {
  node.builtStyle = node.prototype.styleBuilder(node.state);
  node.builtHeaderStyle = node.prototype.headerStyleBuilder(node.state);
}

void main() {
  testWidgets('Tippen auf MoveOnlyNode wählt den Node aus', (tester) async {
    final controller = _controllerWithSourceSinkPrototypes();
    addTearDown(controller.dispose);
    final node = controller.addNode('source');
    _primeStyles(node);

    await tester.pumpWidget(
      _wrapNode(MoveOnlyNode(controller: controller, node: node)),
    );

    expect(find.text('Source'), findsOneWidget);
    expect(find.text('Out'), findsOneWidget);
    expect(controller.selectedNodeIds, isEmpty);

    await tester.tap(find.text('Source'));
    await tester.pump();

    expect(controller.selectedNodeIds, {node.id});
  });

  testWidgets('Ziehen auf MoveOnlyNode verschiebt den Node', (tester) async {
    final controller = _controllerWithSourceSinkPrototypes();
    addTearDown(controller.dispose);
    final node = controller.addNode('source', offset: Offset.zero);
    _primeStyles(node);

    await tester.pumpWidget(
      _wrapNode(MoveOnlyNode(controller: controller, node: node)),
    );

    // Grid-ausgerichteter Delta (snapToGridSize = 64), siehe CLAUDE.md.
    await tester.drag(find.text('Source'), const Offset(128, 64));
    await tester.pump();

    expect(node.offset, const Offset(128, 64));
    expect(controller.selectedNodeIds, {node.id});
  });
}
