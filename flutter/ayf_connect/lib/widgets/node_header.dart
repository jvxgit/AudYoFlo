import 'package:fl_nodes/fl_nodes.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../state/diagram_controller.dart';

/// Ersetzt den Standard-Header von fl_nodes, damit der angezeigte Titel aus
/// unserem [DiagramController] kommt (und sich damit über den Inspector
/// umbenennen lässt) statt aus dem statischen, typweiten Prototyp-Namen.
///
/// fl_nodes baut diesen Header tief in seinem eigenen Widget-Baum (über
/// FlNodeEditorWidget.headerBuilder). Weil [DiagramController] via
/// [Provider] bereitgestellt wird, funktioniert context.watch trotzdem —
/// er muss nicht extra durchgereicht werden.
class DiagramNodeHeader extends StatelessWidget {
  final FlNodeDataModel node;
  final VoidCallback onToggleCollapse;

  const DiagramNodeHeader({
    super.key,
    required this.node,
    required this.onToggleCollapse,
  });

  @override
  Widget build(BuildContext context) {
    final diagramController = context.watch<DiagramController>();
    final title =
        diagramController.nodeById(node.id)?.title ??
            node.prototype.displayName(context);

    return Container(
      padding: node.builtHeaderStyle.padding,
      decoration: node.builtHeaderStyle.decoration,
      child: Row(
        children: [
          InkWell(
            splashColor: Colors.transparent,
            highlightColor: Colors.transparent,
            splashFactory: NoSplash.splashFactory,
            onTap: onToggleCollapse,
            child: Icon(
              node.builtHeaderStyle.icon,
              color: Colors.white,
              size: 20,
            ),
          ),
          const SizedBox(width: 8),
          Flexible(
            child: Text(
              title,
              style: node.builtHeaderStyle.textStyle,
              overflow: TextOverflow.ellipsis,
            ),
          ),
        ],
      ),
    );
  }
}
