import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:collection/collection.dart';
import 'package:ayf_pack/ayf_pack_local.dart';

class AudYoFloBinRenderWidget extends StatelessWidget {
  final JvxComponentIdentification cpId;
  AudYoFloBinRenderWidget(this.cpId);
  @override
  Widget build(BuildContext context) {
    AudYoFloUiModel uiModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);

    // Update this widget if there is a notification in the uiModel
    return Consumer<AudYoFloUiModel>(
      builder: (context, notifier, child) {
        return Container(
            child: Row(children: [
          Expanded(
            child: Column(children: [
              Expanded(
                child: Row(
                  children: [
                    Flexible(
                      flex: 1,
                      child: LayoutBuilder(builder: (context, constraints) {
                        // Create the 360° render canvas
                        return Container(
                          color: Colors.yellow,
                          width: constraints.maxWidth,
                          height: constraints.maxHeight,
                        );
                      }),
                    )
                  ],
                ),
              ),
              const Padding(padding: EdgeInsets.all(16.0))
            ]),
          ),
          Container(color: Color.fromARGB(255, 162, 182, 163))
        ]));
      },
    );
  }
}
