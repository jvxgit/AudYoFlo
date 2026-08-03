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
