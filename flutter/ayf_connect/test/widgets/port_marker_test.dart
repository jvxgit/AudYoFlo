import 'package:connectorflo/models/port_type.dart';
import 'package:connectorflo/widgets/port_marker.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  Future<void> pump(WidgetTester tester, PortMarkerDirection direction) {
    return tester.pumpWidget(
      Directionality(
        textDirection: TextDirection.ltr,
        child: PortMarker(direction: direction),
      ),
    );
  }

  testWidgets('rendert ein nach rechts zeigendes Basis-Icon', (tester) async {
    await pump(tester, PortMarkerDirection.right);
    expect(find.byIcon(Icons.play_arrow), findsOneWidget);
  });

  testWidgets('dreht das Icon je nach Richtung (quarterTurns)', (tester) async {
    for (final entry in {
      PortMarkerDirection.right: 0,
      PortMarkerDirection.down: 1,
      PortMarkerDirection.left: 2,
      PortMarkerDirection.up: 3,
    }.entries) {
      await pump(tester, entry.key);
      final rotatedBox = tester.widget<RotatedBox>(
        find.ancestor(
          of: find.byIcon(Icons.play_arrow),
          matching: find.byType(RotatedBox),
        ),
      );
      expect(rotatedBox.quarterTurns, entry.value,
          reason: 'für ${entry.key}');
    }
  });
}
