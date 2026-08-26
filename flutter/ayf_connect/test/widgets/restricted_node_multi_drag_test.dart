import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
// ignore: depend_on_referenced_packages
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

import 'package:connectorflo/editor/fl_nodes_adapter.dart';
import 'package:connectorflo/editor/node_type_definition.dart';
import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/widgets/restricted_node.dart';

// Reproduces the real AudYoFloConnectFlowWidget setup (moveOnly mode, several
// nodes loaded onto one canvas) inside a full FlNodeEditorWidget, unlike
// restricted_node_test.dart which renders RestrictedNode in isolation and
// therefore never exercises NodeEditorRenderObject.hitTestChildren's
// spatial-grid-based pointer routing at all.
void main() {
  const nodeTypes = [
    NodeTypeDefinition(typeId: 'a', label: 'A'),
    NodeTypeDefinition(typeId: 'b', label: 'B'),
  ];

  testWidgets(
    'Ziehen an einem Node im Move-Only-Modus bewegt nur diesen Node, '
    'nicht alle',
    (tester) async {
      debugDefaultTargetPlatformOverride = TargetPlatform.windows;
      try {
        final diagramController = DiagramController();
        final adapter = FlNodesAdapter(
          diagramController: diagramController,
          nodeTypes: nodeTypes,
        );
        addTearDown(adapter.dispose);

        final nodeA = adapter.flController.addNode(
          'a',
          offset: const Offset(0, 0),
        );
        final nodeB = adapter.flController.addNode(
          'b',
          offset: const Offset(300, 0),
        );

        await tester.pumpWidget(
          ChangeNotifierProvider<DiagramController>.value(
            value: diagramController,
            child: MaterialApp(
              localizationsDelegates: const [
                FlNodeEditorLocalizationsDelegate(),
                GlobalMaterialLocalizations.delegate,
                GlobalWidgetsLocalizations.delegate,
                GlobalCupertinoLocalizations.delegate,
              ],
              supportedLocales: const [Locale('de'), Locale('en')],
              home: Scaffold(
                body: SizedBox(
                  width: 1200,
                  height: 800,
                  child: FlNodeEditorWidget(
                    controller: adapter.flController,
                    expandToParent: true,
                    overlay: () => [],
                    nodeBuilder: (context, node) => RestrictedNode(
                      controller: adapter.flController,
                      node: node,
                      movable: true,
                    ),
                  ),
                ),
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();

        final beforeA = nodeA.offset;
        final beforeB = nodeB.offset;

        final editorRect = tester.getRect(find.byType(FlNodeEditorWidget));
        final zoom = adapter.flController.viewportZoom;
        final vpOffset = adapter.flController.viewportOffset;
        Offset screenPointFor(Offset worldOffset) =>
            editorRect.center + (worldOffset + vpOffset) * zoom;

        // Startpunkt innerhalb Node A, aber abseits seines Output-Ports.
        final start = screenPointFor(beforeA) + const Offset(20, 20);

        final gesture = await tester.startGesture(
          start,
          kind: PointerDeviceKind.mouse,
        );
        await tester.pump(const Duration(milliseconds: 50));
        await gesture.moveBy(const Offset(150, 100));
        await tester.pump(const Duration(milliseconds: 50));
        // Der erste moveBy oben wird vom Pan-Slop der Erkennung verbraucht
        // und löst noch kein onPanUpdate aus (siehe canvas_page_drag_test.dart,
        // das aus demselben Grund zwei moveBy-Aufrufe braucht) - erst dieser
        // zweite, deutlich über der halben Grid-Zelle (32px) liegende moveBy
        // erzeugt eine tatsächlich sichtbare, snap-übrig-bleibende
        // Verschiebung.
        await gesture.moveBy(const Offset(100, 100));
        await tester.pump(const Duration(milliseconds: 50));
        await gesture.up();
        await tester.pumpAndSettle();

        final afterA = nodeA.offset;
        final afterB = nodeB.offset;

        expect(
          afterA,
          isNot(equals(beforeA)),
          reason: 'Node A wurde gezogen, sollte sich bewegt haben.',
        );
        expect(
          afterB,
          equals(beforeB),
          reason:
              'Node B wurde NICHT gezogen, hat sich aber trotzdem bewegt '
              '(vorher: $beforeB, nachher: $afterB) - alle Nodes bewegen '
              'sich offenbar gemeinsam statt nur der gezogene.',
        );
      } finally {
        debugDefaultTargetPlatformOverride = null;
      }
    },
  );
}
