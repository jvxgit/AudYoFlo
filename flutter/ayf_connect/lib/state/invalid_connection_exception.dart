/// Wird geworfen, wenn [DiagramController.addEdge] eine fachlich ungültige
/// Verbindung ablehnt (z.B. inkompatible Port-Typen, falsche Richtung).
class InvalidConnectionException implements Exception {
  final String message;

  const InvalidConnectionException(this.message);

  @override
  String toString() => 'InvalidConnectionException: $message';
}
