import 'package:flutter_test/flutter_test.dart';

import 'package:connectorflo/main.dart';

void main() {
  testWidgets('Klick in der Palette fügt einen Node im Canvas hinzu',
      (WidgetTester tester) async {
    await tester.pumpWidget(const ConnectorFloApp());
    await tester.pump(const Duration(milliseconds: 500));

    expect(find.text('Source'), findsOneWidget);

    await tester.tap(find.text('Source'));
    await tester.pump(const Duration(milliseconds: 500));

    expect(find.text('Source'), findsNWidgets(2));
  });
}
