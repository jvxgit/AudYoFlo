import 'package:flutter/foundation.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
// ignore: depend_on_referenced_packages
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/widgets/canvas_page.dart';
import 'package:fl_nodes/fl_nodes.dart';

// fl_nodes' default node widget (default_node.dart) picks its gesture
// implementation by `defaultTargetPlatform`: a touch-oriented GestureDetector
// for Android/iOS, an ImprovedListener keyed to specific mouse buttons for
// every other platform (including Windows, this app's primary target). Since
// `flutter test` defaults `defaultTargetPlatform` to Android regardless of
// the host OS, a test that doesn't override it exercises the touch branch -
// not what a Windows mouse user actually gets. Every test in this file
// forces the desktop branch so it reflects the real app.
void main() {
  // TestWidgetsFlutterBinding checks that debug-only foundation variables
  // are back to their default before the test callback returns (not merely
  // before the surrounding tearDown() runs), so the override has to be
  // cleared inside the test body itself - see the try/finally in each test.
  Future<void> pumpEditPage(
    WidgetTester tester,
    DiagramController diagramController,
  ) async {
    await tester.pumpWidget(
      MaterialApp(
        localizationsDelegates: const [
          FlNodeEditorLocalizationsDelegate(),
          GlobalMaterialLocalizations.delegate,
          GlobalWidgetsLocalizations.delegate,
          GlobalCupertinoLocalizations.delegate,
        ],
        supportedLocales: const [Locale('de'), Locale('en')],
        home: ChangeNotifierProvider<DiagramController>.value(
          value: diagramController,
          child: const CanvasPage(),
        ),
      ),
    );
    await tester.pumpAndSettle();
  }

  testWidgets(
    'Node im Edit-Modus lässt sich per Maus-Drag (linke Taste) verschieben',
    (tester) async {
      debugDefaultTargetPlatformOverride = TargetPlatform.windows;
      try {
        final diagramController = DiagramController();
        await pumpEditPage(tester, diagramController);

        // Über die Palette hinzufügen (echter Klick, kein direkter
        // Controller-Aufruf).
        await tester.tap(find.text('Source').first);
        await tester.pumpAndSettle();
        expect(diagramController.nodes, hasLength(1));

        final before = diagramController.nodes.single.position;

        // fl_nodes paints nodes through a raw Canvas.transform (viewport
        // centering + zoom + pan) applied only at paint time, which
        // Flutter's own hit-test tree - and therefore
        // tester.getCenter()/getRect() on a node's own widgets - knows
        // nothing about (see NodeEditorRenderObject._getTransformMatrix vs.
        // the plain childParentData.offset = node.offset used for layout).
        // A screen point that actually lands on the node has to be
        // computed by re-deriving that same transform instead: newly
        // added nodes start at the default zoom (1.0) and viewport offset
        // (0,0), so the node's world offset maps to screen as
        // `editorRect.center + node.offset` (see fl_nodes' own
        // hitTestChildren, which computes the inverse of exactly this).
        final editorRect = tester.getRect(find.byType(FlNodeEditorWidget));
        // A small delta into the node's body (well clear of its single
        // output port on the right edge), not its exact center, so this
        // doesn't depend on knowing the node's rendered size.
        final start = editorRect.center + before + const Offset(30, 20);

        final gesture = await tester.startGesture(
          start,
          kind: PointerDeviceKind.mouse,
        );
        await tester.pump(const Duration(milliseconds: 50));
        await gesture.moveBy(const Offset(150, 100));
        await tester.pump(const Duration(milliseconds: 50));
        await gesture.moveBy(const Offset(20, 20));
        await tester.pump(const Duration(milliseconds: 50));
        await gesture.up();
        await tester.pumpAndSettle();

        final after = diagramController.nodes.single.position;
        expect(
          after,
          isNot(equals(before)),
          reason:
              'Node-Position vor Drag: $before, nach Drag: $after - hat '
              'sich nicht verändert.',
        );
      } finally {
        debugDefaultTargetPlatformOverride = null;
      }
    },
  );

  testWidgets(
    'Canvas lässt sich per mittlerer Maustaste verschieben (fl_nodes-Pan-Geste)',
    (tester) async {
      debugDefaultTargetPlatformOverride = TargetPlatform.windows;
      try {
        final diagramController = DiagramController();
        await pumpEditPage(tester, diagramController);

        String? offsetText() {
          final texts = tester
              .widgetList<Text>(find.textContaining('Offset:'))
              .map((t) => t.data)
              .toList();
          return texts.isEmpty ? null : texts.first;
        }

        final before = offsetText();

        final center = tester.getCenter(find.byType(FlNodeEditorWidget));
        final gesture = await tester.startGesture(
          center,
          kind: PointerDeviceKind.mouse,
          buttons: kMiddleMouseButton,
        );
        await tester.pump(const Duration(milliseconds: 50));
        await gesture.moveBy(const Offset(120, 0));
        await tester.pump(const Duration(milliseconds: 50));
        await gesture.up();
        await tester.pumpAndSettle();

        final after = offsetText();

        expect(
          after,
          isNot(equals(before)),
          reason:
              'Debug-Overlay Offset vor Drag: $before, nach Drag: $after - '
              'hat sich nicht verändert.',
        );
      } finally {
        debugDefaultTargetPlatformOverride = null;
      }
    },
  );
}
