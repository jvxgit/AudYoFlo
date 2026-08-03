import 'dart:async';

import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';

import '../models/diagram.dart';
import '../models/port_definition.dart';
import '../models/port_type.dart';
import '../state/diagram_controller.dart';
import '../state/invalid_connection_exception.dart';
import 'diagram_editor_mode.dart';
import 'node_type_definition.dart';

/// Verbindet [DiagramController] (unser entkoppeltes, JSON-fähiges
/// Domänenmodell) mit [FlNodeEditorController] (die tatsächliche
/// Interaktions-Engine von fl_nodes, die Drag/Connect/Delete-Gesten selbst
/// verarbeitet).
///
/// fl_nodes bleibt die "lebende" Quelle für Positionen/Verbindungen während
/// der Nutzer interagiert; dieser Adapter hört auf seine Events und
/// spiegelt jede Änderung in den [DiagramController], damit dieser jederzeit
/// ein aktuelles, framework-unabhängiges Abbild des Diagramms liefert.
class FlNodesAdapter {
  final DiagramController diagramController;
  final List<NodeTypeDefinition> nodeTypes;

  late final FlNodeEditorController flController;
  StreamSubscription<dynamic>? _subscription;

  FlNodesAdapter({
    required this.diagramController,
    required this.nodeTypes,
  }) {
    // autoBuildGraph/autoRunGraph gehören zur visuellen-Programmierung/
    // Execution-Funktionalität von fl_nodes, die wir hier nicht nutzen
    // (unsere Nodes definieren nur Datenfluss, kein onExecute). Ohne diese
    // Deaktivierung plant der Controller intern wiederkehrende Timer für
    // eine Funktion, die wir nie aufrufen.
    flController = FlNodeEditorController(
      config: const FlNodeEditorConfig(
        autoBuildGraph: false,
        autoRunGraph: false,
      ),
    );

    for (final nodeType in nodeTypes) {
      flController.registerNodePrototype(_buildPrototype(nodeType));
    }

    _subscription = flController.eventBus.events.listen(_handleEvent);
  }

  void dispose() {
    _subscription?.cancel();
    flController.dispose();
  }

  /// Ersetzt den kompletten aktuellen Canvas-Inhalt durch [diagram] — die
  /// Umkehrung dessen, was der Adapter sonst laufend tut (fl_nodes-Events →
  /// [DiagramController]). Gedacht dafür, dass externe Komponenten ein
  /// fertiges [Diagram] (z.B. aus JSON via [Diagram.fromJson]) zum Anzeigen
  /// übergeben, etwa in Kombination mit [DiagramEditorMode.readOnly].
  ///
  /// Validiert vorab, dass alle Node-Typen bekannt sind und alle Edges nur
  /// auf im Diagramm enthaltene Nodes verweisen — wirft sonst einen
  /// [ArgumentError], bevor der bestehende Canvas-Inhalt angetastet wird.
  ///
  /// Wichtig: fl_nodes vergibt beim Erzeugen jedes Node über
  /// [FlNodeEditorController.addNode] eine eigene, neue Id (es gibt keine
  /// Möglichkeit, eine vorgegebene Id zu übernehmen). Die IDs im laufenden
  /// Canvas (und im davon gespiegelten [DiagramController] danach) weichen
  /// deshalb von den IDs in [diagram] ab; nur die Struktur (Typen, Positionen,
  /// Verbindungen) bleibt erhalten.
  void loadDiagram(Diagram diagram) {
    for (final node in diagram.nodes) {
      if (!nodeTypes.any((type) => type.typeId == node.typeId)) {
        throw ArgumentError(
          'Unbekannter Node-Typ "${node.typeId}" (Node "${node.id}") ist '
          'nicht in den registrierten nodeTypes enthalten.',
        );
      }
    }

    final diagramNodeIds = diagram.nodes.map((node) => node.id).toSet();
    for (final edge in diagram.edges) {
      if (!diagramNodeIds.contains(edge.fromNodeId) ||
          !diagramNodeIds.contains(edge.toNodeId)) {
        throw ArgumentError(
          'Edge "${edge.id}" referenziert einen Node, der nicht Teil von '
          'diagram.nodes ist.',
        );
      }
    }

    for (final id in flController.nodes.keys.toList()) {
      // removeNodeById entfernt auch alle daran hängenden Links.
      flController.removeNodeById(id);
    }

    final idMap = <String, String>{};
    for (final node in diagram.nodes) {
      final flNode = flController.addNode(node.typeId, offset: node.position);
      idMap[node.id] = flNode.id;
    }

    for (final edge in diagram.edges) {
      flController.addLink(
        idMap[edge.fromNodeId]!,
        edge.fromPortId,
        idMap[edge.toNodeId]!,
        edge.toPortId,
      );
    }
  }

  NodeTypeDefinition _typeById(String typeId) {
    return nodeTypes.firstWhere(
      (nodeType) => nodeType.typeId == typeId,
      orElse: () =>
          throw StateError('Unbekannter Node-Typ registriert: $typeId'),
    );
  }

  FlNodePrototype _buildPrototype(NodeTypeDefinition nodeType) {
    return FlNodePrototype(
      idName: nodeType.typeId,
      displayName: (context) => nodeType.label,
      description: (context) => nodeType.label,
      ports: [
        for (final spec in nodeType.inputs) _buildInputPort(spec),
        for (final spec in nodeType.outputs) _buildOutputPort(spec),
      ],
      onExecute: (ports, fields, state, f, p) async {},
    );
  }

  // Hinweis: PortType.any wird hier bewusst nicht unterstützt. fl_nodes prüft
  // Kompatibilität über den Dart-Generic-Typ (T), und dynamic verhält sich
  // dabei asymmetrisch zu unserem PortType.isCompatibleWith (any passt in
  // unserem Modell in beide Richtungen). Solange der Katalog nur konkrete
  // Typen (string/number/boolean) nutzt, ist das kein Problem.
  FlPortPrototype _buildInputPort(PortSpec spec) {
    switch (spec.type) {
      case PortType.string:
        return FlDataInputPortPrototype<String>(
          idName: spec.id,
          displayName: (context) => spec.label,
        );
      case PortType.number:
        return FlDataInputPortPrototype<double>(
          idName: spec.id,
          displayName: (context) => spec.label,
        );
      case PortType.boolean:
        return FlDataInputPortPrototype<bool>(
          idName: spec.id,
          displayName: (context) => spec.label,
        );
      case PortType.any:
        return FlDataInputPortPrototype<dynamic>(
          idName: spec.id,
          displayName: (context) => spec.label,
        );
    }
  }

  FlPortPrototype _buildOutputPort(PortSpec spec) {
    switch (spec.type) {
      case PortType.string:
        return FlDataOutputPortPrototype<String>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: flDefaultPortStyleBuilder,
        );
      case PortType.number:
        return FlDataOutputPortPrototype<double>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: flDefaultPortStyleBuilder,
        );
      case PortType.boolean:
        return FlDataOutputPortPrototype<bool>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: flDefaultPortStyleBuilder,
        );
      case PortType.any:
        return FlDataOutputPortPrototype<dynamic>(
          idName: spec.id,
          displayName: (context) => spec.label,
          styleBuilder: flDefaultPortStyleBuilder,
        );
    }
  }

  void _handleEvent(dynamic event) {
    if (event is FlAddNodeEvent) {
      _mirrorAddNode(event.node);
    } else if (event is FlRemoveNodeEvent) {
      diagramController.removeNode(event.node.id);
    } else if (event is FlDragSelectionEvent) {
      _mirrorPositions(event.nodeIds);
    } else if (event is FlDragSelectionEndEvent) {
      _mirrorPositions(event.nodeIds);
    } else if (event is FlAddLinkEvent) {
      _mirrorAddLink(event.link);
    } else if (event is FlRemoveLinkEvent) {
      diagramController.removeEdge(event.link.id);
    } else if (event is FlNodeSelectionEvent) {
      _mirrorSelection(event);
    }
  }

  void _mirrorAddNode(FlNodeDataModel node) {
    final nodeType = _typeById(node.prototype.idName);
    diagramController.addNode(
      id: node.id,
      typeId: nodeType.typeId,
      title: nodeType.label,
      position: node.offset,
      inputs: [
        for (final spec in nodeType.inputs) _toPortDefinition(spec, PortDirection.input),
      ],
      outputs: [
        for (final spec in nodeType.outputs) _toPortDefinition(spec, PortDirection.output),
      ],
    );
  }

  PortDefinition _toPortDefinition(PortSpec spec, PortDirection direction) {
    return PortDefinition(
      id: spec.id,
      label: spec.label,
      type: spec.type,
      direction: direction,
    );
  }

  void _mirrorPositions(Set<String> nodeIds) {
    for (final nodeId in nodeIds) {
      final node = flController.getNodeById(nodeId);
      if (node != null) {
        diagramController.moveNode(nodeId, node.offset);
      }
    }
  }

  void _mirrorAddLink(FlLinkDataModel link) {
    try {
      // fl_nodes' FromTo-Record ist trotz seiner Feldnamen so belegt:
      // from = Output-Node-Id, to = Output-Port-Id,
      // fromPort = Input-Node-Id, toPort = Input-Port-Id
      // (siehe FlNodeEditorController.addLink in core.dart). Feldnamen und
      // tatsächliche Bedeutung stimmen dort nicht überein.
      diagramController.addEdge(
        id: link.id,
        fromNodeId: link.fromTo.from,
        fromPortId: link.fromTo.to,
        toNodeId: link.fromTo.fromPort,
        toPortId: link.fromTo.toPort,
      );
    } on InvalidConnectionException catch (error) {
      // fl_nodes prüft Port-Typ-Kompatibilität bereits selbst beim Ziehen der
      // Verbindung; schlägt unsere zusätzliche Domänen-Validierung trotzdem
      // fehl, nehmen wir den Link auch in fl_nodes wieder zurück, damit beide
      // Modelle konsistent bleiben.
      debugPrint('Verbindung von fl_nodes-Adapter abgelehnt: $error');
      flController.removeLinkById(link.id);
    }
  }

  void _mirrorSelection(FlNodeSelectionEvent event) {
    // Bei type == deselect enthält event.nodeIds die zuvor ausgewählten
    // Nodes (die gerade abgewählt wurden), nicht die neue Auswahl — fl_nodes'
    // clearSelection() leert danach immer die komplette Auswahl.
    // Hinweis: FlSelectionEventType selbst wird von fl_nodes nicht öffentlich
    // exportiert (Lücke im Package-Barrel), daher der Vergleich über .name.
    if (event.type.name == 'deselect') {
      diagramController.selectNode(null);
    } else {
      diagramController.selectNode(
        event.nodeIds.isEmpty ? null : event.nodeIds.first,
      );
    }
  }
}
