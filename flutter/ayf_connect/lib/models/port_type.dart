/// Datentyp eines Ports. Bestimmt, ob ein Output- mit einem Input-Port
/// verbunden werden darf.
enum PortType {
  string,
  number,
  boolean,
  any;

  bool isCompatibleWith(PortType other) {
    return this == PortType.any || other == PortType.any || this == other;
  }

  String toJson() => name;

  static PortType fromJson(String json) {
    return PortType.values.firstWhere(
      (value) => value.name == json,
      orElse: () => PortType.any,
    );
  }
}

enum PortDirection {
  input,
  output;

  String toJson() => name;

  static PortDirection fromJson(String json) {
    return PortDirection.values.firstWhere((value) => value.name == json);
  }
}

/// Richtung, in die die Dreiecks-Markierung eines Ports zeigt. Kodiert den
/// Datenfluss durch diesen Anschluss (nicht Ein-/Ausgang selbst — das steckt
/// in [PortDirection] bzw. der Node-Seite, an der der Port sitzt).
///
/// Standard ist [right]: bei einem Links-nach-rechts-Layout zeigt die Spitze
/// damit an einem Eingang in die Node hinein und an einem Ausgang aus ihr
/// heraus — beides „Fluss nach rechts". Fließen Daten künftig andersherum
/// (z.B. ein Rückkanal, oder eine rechts-nach-links gezeichnete Kette), wird
/// pro Port [left] (bzw. [up]/[down]) gesetzt.
///
/// Der Wert bildet direkt die `quarterTurns` einer [RotatedBox] um ein nach
/// rechts zeigendes Icon ab (siehe `PortMarker`): right=0, down=1, left=2,
/// up=3.
enum PortMarkerDirection {
  right,
  down,
  left,
  up;

  /// `quarterTurns` (im Uhrzeigersinn) für eine [RotatedBox] um ein
  /// standardmäßig nach rechts zeigendes Icon.
  int get quarterTurns => index;

  String toJson() => name;

  static PortMarkerDirection fromJson(String? json) {
    return PortMarkerDirection.values.firstWhere(
      (value) => value.name == json,
      orElse: () => PortMarkerDirection.right,
    );
  }
}
