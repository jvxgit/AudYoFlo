import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

import 'package:connectorflo/editor/diagram_editor_mode.dart';
import 'package:connectorflo/state/diagram_controller.dart';
import 'package:connectorflo/widgets/canvas_page.dart';

Widget _wrap(DiagramEditorMode mode) {
  return ChangeNotifierProvider<DiagramController>(
    create: (_) => DiagramController(),
    child: MaterialApp(
      localizationsDelegates: const [
        FlNodeEditorLocalizationsDelegate(),
        GlobalMaterialLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
        GlobalCupertinoLocalizations.delegate,
      ],
      supportedLocales: const [Locale('de'), Locale('en')],
      home: CanvasPage(mode: mode),
    ),
  );
}

void main() {
  testWidgets('Read-Only-Modus blendet Palette und Canvas-leeren-Button aus',
      (tester) async {
    await tester.pumpWidget(_wrap(DiagramEditorMode.readOnly));
    await tester.pump(const Duration(milliseconds: 500));

    expect(find.text('Nodes'), findsNothing);
    expect(find.byIcon(Icons.delete_sweep_outlined), findsNothing);
    expect(find.byIcon(Icons.fit_screen_outlined), findsOneWidget);
  });

  testWidgets('Edit-Modus zeigt weiterhin Palette und Canvas-leeren-Button',
      (tester) async {
    await tester.pumpWidget(_wrap(DiagramEditorMode.edit));
    await tester.pump(const Duration(milliseconds: 500));

    expect(find.text('Nodes'), findsOneWidget);
    expect(find.byIcon(Icons.delete_sweep_outlined), findsOneWidget);
    expect(find.byIcon(Icons.fit_screen_outlined), findsOneWidget);
  });
}
