import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

typedef AudYoFloActiveTextFieldOnCompleteCallback = void Function(String txt);

class AudYoFloActiveTextField extends StatefulWidget {
  final String showTextOnBuild;
  final AudYoFloActiveTextFieldOnCompleteCallback? onEditingComplete;
  final bool reportOnFocusLost;
  final bool haveCommandHistory;
  AudYoFloActiveTextField(
      {this.onEditingComplete,
      this.showTextOnBuild = "",
      this.reportOnFocusLost = false,
      this.haveCommandHistory = true});
  @override
  State<StatefulWidget> createState() {
    return _AudYoFloActiveTextFieldStates();
  }
}

class _AudYoFloActiveTextFieldStates extends State<AudYoFloActiveTextField> {
  String showTextOnBuild = '';
  final TextEditingController _controllerIn = TextEditingController();
  List<String> previousCommands = [];
  int hist_cnt = -1;
  int widgetHash = -1;

  @override
  Widget build(BuildContext context) {
    // Check if a new widget was in use. In this case, we need to reset the text field
    // We need to set the value, not just the text!
    // https://stackoverflow.com/questions/51127241/how-do-you-change-the-value-inside-of-a-textfield-flutter

    if (widgetHash != widget.hashCode) {
      showTextOnBuild = widget.showTextOnBuild;
      widgetHash = widget.hashCode;
    }

    _controllerIn.value = TextEditingValue(
        text: showTextOnBuild,
        selection: TextSelection.fromPosition(
          TextPosition(offset: showTextOnBuild.length),
        ));

    return Focus(
      child: TextFormField(
        controller: _controllerIn,
        // focusNode: myNode,
        keyboardType: TextInputType.text,
        maxLines: 1,
        onEditingComplete: () {
          String text = _controllerIn.text;
          _controllerIn.clear();
          if (widget.haveCommandHistory) {
            String? elm =
                previousCommands.firstWhereOrNull((element) => element == text);
            if (elm == null) {
              previousCommands.add(text);
            } else {
              previousCommands.remove(text);
              previousCommands.add(text);
            }
            hist_cnt = -1;
          }
          //myNode.unfocus();
          widget.onEditingComplete?.call(text);
        },
        onTap: () {
          //myNode.requestFocus();
        },
        decoration: const InputDecoration(border: OutlineInputBorder()),
      ),
      onFocusChange: (value) {
        if (!value) {
          if (widget.reportOnFocusLost) {
            String text = _controllerIn.text;
            widget.onEditingComplete?.call(text);
          }
        }
      },
      onKeyEvent: (node, ev) {
        if (ev is KeyUpEvent) {
          if (ev.logicalKey == LogicalKeyboardKey.arrowUp) {
            setState(() {
              if (hist_cnt == -1) {
                if (previousCommands.isNotEmpty) {
                  hist_cnt = previousCommands.length - 1;
                }
              }

              if (hist_cnt >= 0) {
                showTextOnBuild = previousCommands.elementAt(hist_cnt);
              }
              if (hist_cnt > 0) {
                hist_cnt--;
              }
              print('Arrow Up: $hist_cnt');
            });
          }
          if (ev.logicalKey == LogicalKeyboardKey.arrowDown) {
            setState(() {
              if (hist_cnt < previousCommands.length - 1) {
                hist_cnt++;
              } else {
                hist_cnt = -1;
              }
              if (hist_cnt >= 0) {
                showTextOnBuild = previousCommands.elementAt(hist_cnt);
              } else {
                showTextOnBuild = '';
              }
              print('Arrow Down: $hist_cnt');
            });
          }
        }
        return KeyEventResult.ignored;
      },
    );
  }

  @override
  void initState() {
    super.initState();
  }
}
