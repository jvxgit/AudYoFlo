import 'diagram_edge.dart';
import 'diagram_node.dart';

/// Der vollständige Zustand eines Diagramms: alle Nodes und Verbindungen.
/// Bewusst reines Datenobjekt ohne Flutter-Widget-Abhängigkeiten (bis auf
/// [Offset] und [Color] in [DiagramNode]), damit es sich 1:1 als JSON
/// speichern und später an ein Backend senden lässt.
class Diagram {
  final List<DiagramNode> nodes;
  final List<DiagramEdge> edges;

  const Diagram({
    this.nodes = const [],
    this.edges = const [],
  });

  Map<String, dynamic> toJson() => {
        'nodes': nodes.map((node) => node.toJson()).toList(),
        'edges': edges.map((edge) => edge.toJson()).toList(),
      };

  factory Diagram.fromJson(Map<String, dynamic> json) {
    return Diagram(
      nodes: (json['nodes'] as List<dynamic>)
          .map((e) => DiagramNode.fromJson(e as Map<String, dynamic>))
          .toList(),
      edges: (json['edges'] as List<dynamic>)
          .map((e) => DiagramEdge.fromJson(e as Map<String, dynamic>))
          .toList(),
    );
  }
}
