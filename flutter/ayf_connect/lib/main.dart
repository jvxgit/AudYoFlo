import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:provider/provider.dart';

import 'state/diagram_controller.dart';
import 'widgets/canvas_page.dart';

void main() {
  runApp(const ConnectorFloApp());
}

class ConnectorFloApp extends StatelessWidget {
  const ConnectorFloApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ConnectorFlo',
      localizationsDelegates: const [
        FlNodeEditorLocalizationsDelegate(),
        GlobalMaterialLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
        GlobalCupertinoLocalizations.delegate,
      ],
      supportedLocales: const [Locale('de'), Locale('en')],
      theme: ThemeData.dark().copyWith(
        colorScheme: ColorScheme.fromSeed(
          seedColor: Colors.blue,
          brightness: Brightness.dark,
        ),
      ),
      debugShowCheckedModeBanner: false,
      home: ChangeNotifierProvider<DiagramController>(
        create: (_) => DiagramController(),
        child: const CanvasPage(),
      ),
    );
  }
}
