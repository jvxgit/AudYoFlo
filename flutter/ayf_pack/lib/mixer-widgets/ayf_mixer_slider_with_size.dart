import 'package:flutter/material.dart';
import 'ayf_mixer_slider.dart';

class AudYoFloMixerSliderWithSize extends StatelessWidget {
  final double longerSide;
  final double shorterSide;

  final double currentGainValue;
  final Axis orientation;
  final int numChans;
  final Color backgroundColor;
  final IconData iconDat;
  final int numCnt;
  final double gainShow;
  final double ftSizeMax;
  final String title;
  final List<AyfLabelPosition>? labels;
  final List<double>? positionMarksShort;
  final List<double>? positionMarksLong;
  final List<int> idIdent;

  final void Function(double gainVal)? onSliderMoved;
  final void Function(double gainVal)? onSliderReleased;
  final void Function(List<double> avrgMax, int idIdent)? onUpdatePeriodic;
  final String Function(String proposed)? onSliderTooltipFormat;

  AudYoFloMixerSliderWithSize(
      {this.orientation = Axis.vertical,
      this.longerSide = 400,
      this.shorterSide = 75,
      this.currentGainValue = 10,
      this.backgroundColor = Colors.white,
      this.numChans = 1,
      this.iconDat = Icons.mic,
      this.numCnt = 100,
      this.gainShow = 100,
      this.ftSizeMax = 16,
      this.labels,
      this.positionMarksShort,
      this.positionMarksLong,
      this.onSliderMoved,
      this.onSliderReleased,
      this.onUpdatePeriodic,
      this.onSliderTooltipFormat,
      this.idIdent = const [],
      this.title = ""});

  @override
  Widget build(BuildContext context) {
    double width = shorterSide;
    double height = longerSide;

    if (orientation == Axis.horizontal) {
      width = longerSide;
      height = shorterSide;
    }
    if (shorterSide > 0) {
      return SizedBox(
          width: width,
          height: height,
          child: AudYoFloMixerSlider(
            orientation: orientation,
            currentGainValue: currentGainValue,
            backgroundColor: backgroundColor,
            numChans: numChans,
            iconDat: iconDat,
            numCnt: numCnt,
            ftSizeMax: ftSizeMax,
            labels: labels,
            positionMarksShort: positionMarksShort,
            positionMarksLong: positionMarksLong,
            onSliderMoved: onSliderMoved,
            onSliderReleased: onSliderReleased,
            onUpdatePeriodic: onUpdatePeriodic,
            onSliderTooltipFormat: onSliderTooltipFormat,
            idIdent: idIdent,
            title: title,
          ));
    } else {
      return LayoutBuilder(builder: (context, constraints) {
        Axis axis = Axis.vertical;
        if (constraints.maxWidth > constraints.maxHeight) {
          axis = Axis.horizontal;
        }
        return SizedBox(
            width: constraints.maxWidth,
            height: constraints.maxHeight,
            child: AudYoFloMixerSlider(
                orientation: orientation,
                currentGainValue: currentGainValue,
                backgroundColor: backgroundColor,
                numChans: numChans,
                iconDat: iconDat,
                numCnt: numCnt,
                ftSizeMax: ftSizeMax,
                labels: labels,
                positionMarksShort: positionMarksShort,
                positionMarksLong: positionMarksLong,
                onSliderMoved: onSliderMoved,
                onSliderReleased: onSliderReleased,
                onUpdatePeriodic: onUpdatePeriodic,
                onSliderTooltipFormat: onSliderTooltipFormat,
                idIdent: idIdent,
                title: title));
      });
    }
  }
}
