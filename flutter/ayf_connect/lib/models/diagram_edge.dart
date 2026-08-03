/// Eine Verbindung zwischen einem Output-Port und einem Input-Port.
class DiagramEdge {
  final String id;
  final String fromNodeId;
  final String fromPortId;
  final String toNodeId;
  final String toPortId;

  const DiagramEdge({
    required this.id,
    required this.fromNodeId,
    required this.fromPortId,
    required this.toNodeId,
    required this.toPortId,
  });

  bool touchesNode(String nodeId) => fromNodeId == nodeId || toNodeId == nodeId;

  Map<String, dynamic> toJson() => {
        'id': id,
        'fromNodeId': fromNodeId,
        'fromPortId': fromPortId,
        'toNodeId': toNodeId,
        'toPortId': toPortId,
      };

  factory DiagramEdge.fromJson(Map<String, dynamic> json) {
    return DiagramEdge(
      id: json['id'] as String,
      fromNodeId: json['fromNodeId'] as String,
      fromPortId: json['fromPortId'] as String,
      toNodeId: json['toNodeId'] as String,
      toPortId: json['toPortId'] as String,
    );
  }
}
