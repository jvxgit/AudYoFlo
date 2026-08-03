import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:connectorflo/editor/default_node_types.dart';
import 'package:connectorflo/editor/node_type_definition.dart';
import 'package:connectorflo/widgets/node_palette.dart';

void main() {
  testWidgets('Zeigt alle Node-Typen und meldet Taps auf onAddNode',
      (WidgetTester tester) async {
    final tapped = <NodeTypeDefinition>[];

    await tester.pumpWidget(
      MaterialApp(
        home: NodePalette(
          nodeTypes: defaultNodeTypes,
          onAddNode: tapped.add,
        ),
      ),
    );

    for (final nodeType in defaultNodeTypes) {
      expect(find.text(nodeType.label), findsOneWidget);
    }

    await tester.tap(find.text('Transform'));
    await tester.pump();

    expect(tapped, hasLength(1));
    expect(tapped.single.typeId, 'transform');
  });
}
