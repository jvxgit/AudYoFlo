import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

import 'package:connectorflo/models/diagram_node.dart';
import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/widgets/node_inspector.dart';

Widget _wrap(DiagramController controller) {
  return ChangeNotifierProvider<DiagramController>.value(
    value: controller,
    child: const MaterialApp(home: NodeInspector()),
  );
}

void main() {
  testWidgets('Zeigt Platzhalter, wenn kein Node ausgewählt ist',
      (WidgetTester tester) async {
    final controller = DiagramController();

    await tester.pumpWidget(_wrap(controller));

    expect(find.text('Kein Node ausgewählt.'), findsOneWidget);
  });

  testWidgets('Zeigt Node-Daten und erlaubt Umbenennen',
      (WidgetTester tester) async {
    final controller = DiagramController();
    final node = controller.addNode(
      typeId: 'source',
      title: 'Source',
      position: Offset.zero,
    );
    controller.selectNode(node.id);

    await tester.pumpWidget(_wrap(controller));

    expect(find.text('Typ: source'), findsOneWidget);
    expect(find.widgetWithText(TextField, 'Source'), findsOneWidget);

    await tester.enterText(find.byType(TextField), 'Meine Quelle');
    await tester.testTextInput.receiveAction(TextInputAction.done);
    await tester.pump();

    expect(controller.nodeById(node.id)!.title, 'Meine Quelle');
  });

  testWidgets(
      'Zeigt typeIdentification statt typeId, wenn gesetzt (Backend-Node)',
      (WidgetTester tester) async {
    final controller = DiagramController();
    final node = controller.addNode(
      typeId: 'backend_component_i1_o2',
      title: 'Audio Node',
      position: Offset.zero,
    );
    // loadDiagram() setzt per-instance-Details (u.a. typeIdentification)
    // nachträglich via applyNodeOverrides - hier direkt nachgestellt.
    controller.applyNodeOverrides(
      node.id,
      DiagramNode(
        id: node.id,
        typeId: node.typeId,
        title: node.title,
        position: node.position,
        typeIdentification: 'JVX_COMPONENT_AUDIO_NODE<3,0>',
      ),
    );
    controller.selectNode(node.id);

    await tester.pumpWidget(_wrap(controller));

    expect(find.text('Typ: JVX_COMPONENT_AUDIO_NODE<3,0>'), findsOneWidget);
    expect(find.text('Typ: backend_component_i1_o2'), findsNothing);
  });
}
