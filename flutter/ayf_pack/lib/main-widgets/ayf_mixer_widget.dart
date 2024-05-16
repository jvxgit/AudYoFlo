import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

class AudYoFloMixerWidget extends StatelessWidget {
  final String fNameMixerImage;
  AudYoFloMixerWidget(this.fNameMixerImage);
  @override
  Widget build(BuildContext context) {
    AudYoFloUiModel theModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);
    return Container(
        child: Row(children: [
      Container(
        child: Expanded(
          child: Column(children: [
            Expanded(
              child: Row(
                children: [
                  AudYoFloMixerSliderWithSize(
                      orientation: Axis.vertical,
                      longerSide: 400,
                      shorterSide: 75)
                ],
              ),
            ),
            Padding(padding: EdgeInsets.all(16.0))
          ]),
        ),
      ),
      Container(color: Colors.green)
    ]));
  }
}
