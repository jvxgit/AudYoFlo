import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
// ignore: depend_on_referenced_packages
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:connectorflo/editor/fl_nodes_adapter.dart';
import 'package:connectorflo/editor/node_type_definition.dart';
import 'package:connectorflo/models/port_type.dart';
import 'package:connectorflo/state/diagram_controller.dart';

// Regressionstests für [FlNodesAdapter.arrangeRow] und
// [FlNodesAdapter.nextFreeSlot]: beide sollen Nodes so platzieren, dass sie
// sich nie überlappen - unabhängig davon, wie stark ihre (labelabhängige)
// gerenderte Größe variiert, und unabhängig von fl_nodes' Snap-to-Grid
// (FlNodeEditorConfig.enableSnapToGrid, standardmäßig an, 64px-Raster), das
// eine naiv berechnete Zielposition durch eigenes Runden wieder zunichte
// machen kann. Diese Tests pumpen echte Widgets (statt nur Positionen zu
// vergleichen), damit tatsächlich gerenderte RenderBox-Größen einfließen -
// genau die Größe, die im echten Canvas auch für Überlappungen sorgt.
void main() {
  // Bewusst stark unterschiedliche Label-Längen/Port-Zahlen, damit die
  // gerenderte Breite/Höhe der Nodes deutlich variiert.
  const nodeTypes = [
    NodeTypeDefinition(typeId: 'short', label: 'A'),
    NodeTypeDefinition(
      typeId: 'long',
      label: 'A Very Long Node Label That Forces A Wide Box',
    ),
    NodeTypeDefinition(
      typeId: 'medium',
      label: 'Medium Label Node',
      inputs: [
        PortSpec(id: 'in1', label: 'Input One', type: PortType.string),
      ],
      outputs: [
        PortSpec(id: 'out1', label: 'Output One', type: PortType.string),
      ],
    ),
  ];

  Future<FlNodesAdapter> pumpEditor(WidgetTester tester) async {
    final diagramController = DiagramController();
    final adapter = FlNodesAdapter(
      diagramController: diagramController,
      nodeTypes: nodeTypes,
    );
    addTearDown(adapter.dispose);

    await tester.pumpWidget(
      MaterialApp(
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
            ),
          ),
        ),
      ),
    );
    await tester.pumpAndSettle();
    return adapter;
  }

  Rect boundsOf(FlNodesAdapter adapter, String id) {
    final node = adapter.flController.getNodeById(id)!;
    final renderObject =
        node.key.currentContext!.findRenderObject()! as RenderBox;
    return node.offset & renderObject.size;
  }

  void expectNoOverlaps(List<Rect> rects) {
    for (var i = 0; i < rects.length; i++) {
      for (var j = i + 1; j < rects.length; j++) {
        expect(
          rects[i].overlaps(rects[j]),
          isFalse,
          reason: 'Node $i (${rects[i]}) überlappt mit Node $j (${rects[j]})',
        );
      }
    }
  }

  testWidgets(
    'arrangeRow ordnet unterschiedlich große Nodes ohne Überlappung an',
    (tester) async {
      final adapter = await pumpEditor(tester);

      final ids = <String>[];
      for (final typeId in ['long', 'short', 'medium', 'long', 'short']) {
        ids.add(adapter.flController.addNode(typeId).id);
      }
      await tester.pumpAndSettle();

      adapter.arrangeRow();
      await tester.pumpAndSettle();

      final rects = [for (final id in ids) boundsOf(adapter, id)];
      expectNoOverlaps(rects);

      // Nicht nur "irgendwie kein Overlap", sondern mindestens der
      // angeforderte Abstand zwischen aufeinanderfolgenden Nodes - sonst
      // würde z.B. ein Grid-Snap, der den Gap zufällig auf 0 rundet, den
      // Test unbemerkt durchrutschen lassen.
      final byLeft = [...rects]..sort((a, b) => a.left.compareTo(b.left));
      for (var i = 0; i < byLeft.length - 1; i++) {
        expect(byLeft[i + 1].left - byLeft[i].right, greaterThanOrEqualTo(48));
      }
    },
  );

  testWidgets(
    'nextFreeSlot platziert neu hinzugefügte Nodes ohne Überlappung',
    (tester) async {
      final adapter = await pumpEditor(tester);

      final ids = <String>[];
      for (final typeId in ['short', 'long', 'medium', 'long']) {
        final offset = adapter.nextFreeSlot();
        final node = adapter.flController.addNode(typeId, offset: offset);
        ids.add(node.id);
        // Jeder Palette-Klick im echten UI liegt hinter einem eigenen
        // Frame - genau das simuliert dieses pumpAndSettle je Iteration,
        // damit nextFreeSlot beim nächsten Aufruf die reale Größe des
        // gerade hinzugefügten Node sieht (statt fallbackSize).
        await tester.pumpAndSettle();
      }

      final rects = [for (final id in ids) boundsOf(adapter, id)];
      expectNoOverlaps(rects);
    },
  );
}
