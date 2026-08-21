import 'dart:ui';

import 'package:flutter/foundation.dart';
import 'package:uuid/uuid.dart';

import '../models/diagram.dart';
import '../models/diagram_edge.dart';
import '../models/diagram_node.dart';
import '../models/port_definition.dart';
import '../models/port_type.dart';
import 'invalid_connection_exception.dart';

/// Single Source of Truth für den Diagramm-Zustand. UI-Widgets (Canvas,
/// Palette, Inspector) sowie der fl_nodes-Adapter beobachten diesen
/// Controller, statt eigenen Zustand zu halten.
class DiagramController extends ChangeNotifier {
  static const _uuid = Uuid();

  final List<DiagramNode> _nodes = [];
  final List<DiagramEdge> _edges = [];
  String? _selectedNodeId;

  List<DiagramNode> get nodes => List.unmodifiable(_nodes);
  List<DiagramEdge> get edges => List.unmodifiable(_edges);
  String? get selectedNodeId => _selectedNodeId;
  Diagram get diagram => Diagram(nodes: nodes, edges: edges);

  DiagramNode? nodeById(String nodeId) {
    for (final node in _nodes) {
      if (node.id == nodeId) return node;
    }
    return null;
  }

  DiagramNode addNode({
    String? id,
    required String typeId,
    required String title,
    required Offset position,
    List<PortDefinition> inputs = const [],
    List<PortDefinition> outputs = const [],
  }) {
    final node = DiagramNode(
      id: id ?? _uuid.v4(),
      typeId: typeId,
      title: title,
      position: position,
      inputs: inputs,
      outputs: outputs,
    );
    _nodes.add(node);
    notifyListeners();
    return node;
  }

  void moveNode(String nodeId, Offset newPosition) {
    final index = _nodes.indexWhere((node) => node.id == nodeId);
    if (index == -1) return;
    _nodes[index] = _nodes[index].copyWith(position: newPosition);
    notifyListeners();
  }

  void renameNode(String nodeId, String newTitle) {
    final index = _nodes.indexWhere((node) => node.id == nodeId);
    if (index == -1) return;
    _nodes[index] = _nodes[index].copyWith(title: newTitle);
    notifyListeners();
  }

  /// Overrides title, ports and group color of an existing node in one go,
  /// straight from the given [source] node (not merged via copyWith's
  /// null-means-unchanged convention - [groupColor] in particular needs to
  /// be settable back to null). Used by [FlNodesAdapter.loadDiagram] to
  /// restore per-instance details that its prototype-driven mirroring
  /// (nodeType.label / nodeType.inputs / nodeType.outputs) can't carry.
  void applyNodeOverrides(String nodeId, DiagramNode source) {
    final index = _nodes.indexWhere((node) => node.id == nodeId);
    if (index == -1) return;
    final current = _nodes[index];
    _nodes[index] = DiagramNode(
      id: current.id,
      typeId: current.typeId,
      title: source.title,
      position: current.position,
      inputs: source.inputs,
      outputs: source.outputs,
      groupColor: source.groupColor,
    );
    notifyListeners();
  }

  void removeNode(String nodeId) {
    final existed = _nodes.any((node) => node.id == nodeId);
    if (!existed) return;
    _nodes.removeWhere((node) => node.id == nodeId);
    _edges.removeWhere((edge) => edge.touchesNode(nodeId));
    if (_selectedNodeId == nodeId) _selectedNodeId = null;
    notifyListeners();
  }

  /// Erstellt eine Verbindung von einem Output- zu einem Input-Port.
  /// Wirft [InvalidConnectionException], wenn die Verbindung fachlich
  /// ungültig ist (Nodes/Ports unbekannt, falsche Richtung, inkompatible
  /// Typen, Duplikat oder Selbstverbindung).
  DiagramEdge addEdge({
    String? id,
    required String fromNodeId,
    required String fromPortId,
    required String toNodeId,
    required String toPortId,
  }) {
    if (fromNodeId == toNodeId) {
      throw const InvalidConnectionException(
        'Ein Node kann nicht mit sich selbst verbunden werden.',
      );
    }

    final fromNode = nodeById(fromNodeId);
    final toNode = nodeById(toNodeId);
    if (fromNode == null || toNode == null) {
      throw const InvalidConnectionException('Node nicht gefunden.');
    }

    final fromPort = fromNode.findPort(fromPortId);
    final toPort = toNode.findPort(toPortId);
    if (fromPort == null || toPort == null) {
      throw const InvalidConnectionException('Port nicht gefunden.');
    }

    if (fromPort.direction != PortDirection.output ||
        toPort.direction != PortDirection.input) {
      throw const InvalidConnectionException(
        'Verbindungen müssen von einem Output- zu einem Input-Port gehen.',
      );
    }

    if (!fromPort.type.isCompatibleWith(toPort.type)) {
      throw InvalidConnectionException(
        'Inkompatible Port-Typen: ${fromPort.type.name} -> ${toPort.type.name}.',
      );
    }

    final alreadyExists = _edges.any((edge) =>
        edge.fromNodeId == fromNodeId &&
        edge.fromPortId == fromPortId &&
        edge.toNodeId == toNodeId &&
        edge.toPortId == toPortId);
    if (alreadyExists) {
      throw const InvalidConnectionException('Verbindung existiert bereits.');
    }

    final edge = DiagramEdge(
      id: id ?? _uuid.v4(),
      fromNodeId: fromNodeId,
      fromPortId: fromPortId,
      toNodeId: toNodeId,
      toPortId: toPortId,
    );
    _edges.add(edge);
    notifyListeners();
    return edge;
  }

  void removeEdge(String edgeId) {
    final existed = _edges.any((edge) => edge.id == edgeId);
    if (!existed) return;
    _edges.removeWhere((edge) => edge.id == edgeId);
    notifyListeners();
  }

  void selectNode(String? nodeId) {
    if (_selectedNodeId == nodeId) return;
    _selectedNodeId = nodeId;
    notifyListeners();
  }

  void clear() {
    _nodes.clear();
    _edges.clear();
    _selectedNodeId = null;
    notifyListeners();
  }
}
