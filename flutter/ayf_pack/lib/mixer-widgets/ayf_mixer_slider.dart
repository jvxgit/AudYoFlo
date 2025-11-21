import 'dart:math';
import 'package:flutter/material.dart';
import 'package:another_xlider/another_xlider.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

/*
 List<AyfLabelPosition> labels = List<AyfLabelPosition>.generate(11, (idx) {
    return AyfLabelPosition(idx.toString(), idx / 10);
  });
  List<double> positionMarksShort = List<double>.generate(101, (idx) {
    return idx / 100;
  });
  List<double> positionMarksLong = List<double>.generate(11, (idx) {
    return idx / 10;
  });
  */
class AudYoFloMixerSlider extends StatefulWidget {
  final double currentGainValue;
  final Axis orientation;
  final int numChans;
  final Color backgroundColor;
  final IconData iconDat;
  final int numCnt;
  final double ftSizeMax;
  final double widthMaxLine;
  final String title;
  final List<AyfLabelPosition>? labels;
  final List<double>? positionMarksShort;
  final List<double>? positionMarksLong;

  final void Function(double gainVal)? onSliderMoved;
  final void Function(double gainVal)? onSliderReleased;
  final void Function(List<double> avrgAndMax, int idIdent)? onUpdatePeriodic;
  final String Function(String proposed)? onSliderTooltipFormat;

  final List<int> idIdent;

  AudYoFloMixerSlider(
      {this.orientation = Axis.vertical,
      this.currentGainValue = 100,
      this.backgroundColor = Colors.white,
      this.numChans = 1,
      this.iconDat = Icons.mic,
      this.numCnt = 100,
      this.ftSizeMax = 16,
      this.widthMaxLine = 6,
      this.labels,
      this.positionMarksShort,
      this.positionMarksLong,
      this.onSliderMoved,
      this.onSliderReleased,
      this.onUpdatePeriodic,
      this.onSliderTooltipFormat,
      this.idIdent = const [],
      this.title = ""}) {
    // print('Gain on Reset: $currentGainValue');
  }

  @override
  _AudYoFloMixerSliderState createState() => _AudYoFloMixerSliderState();
}

class _AudYoFloMixerSliderState extends State<AudYoFloMixerSlider> {
  double gainShow = 0;
  bool mouseOverShow = false;
  int oldHashCodeWidget = -1;
  List<double> currentAvrgValues = [];
  List<double> currentMaxValues = [];

  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    // Take over the latest gain only if a new widget was created since
    // there may be another value in the backend
    if (widget.hashCode != oldHashCodeWidget) {
      gainShow = widget.currentGainValue;
      oldHashCodeWidget = widget.hashCode;
      if (widget.numChans != currentAvrgValues.length) {
        currentAvrgValues = List<double>.filled(widget.numChans, 0);
        currentMaxValues = List<double>.filled(widget.numChans, 0);
      }
      // print('Reset gain');
    }

    // Use the layout builder to find out the requested size
    return LayoutBuilder(builder: (context, constraints) {
      bool disableTooltip = true;
      double longerSide = constraints.maxHeight;
      double shorterSide = constraints.maxWidth;
      bool rtl = true;

      if (widget.onSliderTooltipFormat != null) {
        disableTooltip = false;
      }

      if (widget.orientation == Axis.horizontal) {
        rtl = false;
        longerSide = constraints.maxWidth;
        shorterSide = constraints.maxHeight;
      }

      double handlerWidth = shorterSide * 3 / 4;
      double handlerHeight = shorterSide / 2;
      double iconSize = handlerHeight;
      if (widget.orientation == Axis.horizontal) {
        handlerHeight = shorterSide * 3 / 4;
        handlerWidth = shorterSide / 2;
        iconSize = handlerWidth;
      }

      Widget? hatchWidget;
      double opacitySlider = 0.2; // do not show
      if (mouseOverShow) {
        opacitySlider = 1;
        hatchWidget = Icon(widget.iconDat, size: iconSize * 0.75);
      } else {
        hatchWidget = Container(color: Colors.white.withOpacity(0));
      }

      return MouseRegion(
        onEnter: (event) {
          if (mouseOverShow == false) {
            setState(() {
              mouseOverShow = true;
            });
          }
        },
        onExit: (event) {
          if (mouseOverShow == true) {
            setState(() {
              mouseOverShow = false;
            });
          }
        },
        child: Stack(fit: StackFit.expand, children: <Widget>[
          Container(
              decoration: BoxDecoration(
                  color: widget.backgroundColor,
                  borderRadius: BorderRadius.all(Radius.circular(10)),
                  border: Border.all(color: Colors.black, width: 2))),
          Selector<AudYoFloPeriodicNotifier, int>(
              selector: (context, perNotify) => perNotify.cntFast,
              //shouldRebuild: (previous, next) => next ? false : true,
              shouldRebuild: (previous, next) =>
                  (next != previous) ? true : false,
              builder: (context, notifier, child) {
                if (widget.onUpdatePeriodic != null) {
                  for (int idx = 0; idx < currentAvrgValues.length; idx++) {
                    int idIdent = -1;
                    if (idx < widget.idIdent.length) {
                      idIdent = widget.idIdent[idx];
                    }
                    List<double> valExchg = [
                      currentAvrgValues[idx],
                      currentMaxValues[idx]
                    ];

                    // here we get the current level values from the caller
                    widget.onUpdatePeriodic!(valExchg, idIdent);
                    currentAvrgValues[idx] = valExchg[0];
                    currentMaxValues[idx] = valExchg[1];
                  }
                }
                //FernLiveBackendBridge theBridge = new FernLiveBackendBridge();
                return CustomPaint(
                  //size: Size(200, 200),
                  painter: SliderPainter(
                      widget.numChans,
                      widget.numCnt,
                      currentAvrgValues,
                      currentMaxValues,
                      widget.orientation,
                      widget.ftSizeMax,
                      widget.labels,
                      widget.positionMarksShort,
                      widget.positionMarksLong,
                      widget.widthMaxLine,
                      title: widget.title),
                );
              }),
          FlutterSlider(
            tooltip: FlutterSliderTooltip(
                disabled: disableTooltip, format: widget.onSliderTooltipFormat),
            // numberFormat: intl.NumberFormat(),
            handlerWidth: handlerWidth,
            handlerHeight: handlerHeight,
            values: [gainShow],
            max: widget.numCnt.toDouble(),
            min: 0,
            axis: widget.orientation,
            rtl: rtl,
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(20),
              shape: BoxShape.rectangle,
              // color: Colors.yellow,
              //border: Border.all(width: 3, color: Colors.red),
            ),
            trackBar: FlutterSliderTrackBar(
              //activeTrackBarHeight: trackHeight,
              //inactiveTrackBarHeight: trackHeight,
              activeDisabledTrackBarColor: Colors.white,
              inactiveDisabledTrackBarColor: Colors.white,
              inactiveTrackBar: BoxDecoration(
                borderRadius: BorderRadius.circular(5),
                shape: BoxShape.rectangle,
                color: Colors.white.withOpacity(0),
                //border: Border.all(width: 1, color: Colors.black),
              ),
              activeTrackBar: BoxDecoration(
                borderRadius: BorderRadius.circular(5),
                color: Colors.white.withOpacity(0),
                shape: BoxShape.rectangle,
                //border: Border.all(width: 1, color: Colors.black),
              ),
            ),
            handler: FlutterSliderHandler(
                //opacity: opacitySlider,
                decoration: BoxDecoration(
                    color: Colors.white.withOpacity(opacitySlider),
                    shape: BoxShape.rectangle,
                    border: Border.all(
                        width: 1,
                        color: Colors.black.withOpacity(opacitySlider))),
                child: hatchWidget),
            rightHandler: FlutterSliderHandler(
              child: Icon(
                Icons.chevron_left,
                color: Colors.red,
                size: 24,
              ),
            ),
            /*
             * We do not use the hatchmark: it was almost impossible to place it cleanly
            hatchMark: FlutterSliderHatchMark(
              linesDistanceFromTrackBar: 0, // 0 means lower bound
              labelsDistanceFromTrackBar:
                  shiftLabs, // 0 means that labels are in middle
              displayLines: false,
              // density: 0.5, // means 50 lines, from 0 to 100 percent
              // labels: hatchLabels,
            ),*/
            onDragCompleted: (handlerIndex, lowerValue, upperValue) {
              // print('New value on Release: $lowerValue');
              if (widget.onSliderReleased != null) {
                widget.onSliderReleased!(lowerValue);
              }
              setState(() {
                gainShow = lowerValue;
              });
            },
            onDragging: (int x, lowerValue, upperValue) {
              // print('New value on Move: $lowerValue');
              if (widget.onSliderMoved != null) {
                widget.onSliderMoved!(lowerValue);
              }
              setState(() {
                gainShow = lowerValue;
              });
            },
          )
        ]),
      );
    });
  }
}

// ==========================================================================
// Painter for main are
// ==========================================================================
class SliderPainter extends CustomPainter {
  final List<double> currentAvrgValue;
  final List<double> currentMaxValue;
  Random randInst = Random();
  Axis orientation;
  final double ftSizeMax;
  final int numChans;
  final int numCnt;
  final widthMaxLine;
  final String title;

  List<AyfLabelPosition>? labels;
  List<double>? positionMarksShort;
  List<double>? positionMarksLong;

  SliderPainter(
      this.numChans,
      this.numCnt,
      this.currentAvrgValue,
      this.currentMaxValue,
      this.orientation,
      this.ftSizeMax,
      this.labels,
      this.positionMarksShort,
      this.positionMarksLong,
      this.widthMaxLine,
      {this.title = ""});

  @override
  void paint(Canvas canvas, Size size) {
    Offset p1;
    Offset p2;
    Offset offs = Offset(0, 0);

    double longAxisSize = size.height;
    double shortAxisSize = size.width;
    double widthLevelWidget = size.width / 4;
    if (orientation == Axis.horizontal) {
      widthLevelWidget = size.height / 4;
      longAxisSize = size.width;
      shortAxisSize = size.height;
    }
    double deltaInner = longAxisSize / 20;
    double ftSize = widthLevelWidget;
    ftSize = min(ftSize, ftSizeMax);
    // currentValue = 0.5;

    double shiftLine = widthLevelWidget / numChans;
    double devDist = shiftLine / 2 * 0.9;
    double offsetLevelWidget = -shiftLine * (numChans - 1) * 0.5;

    for (int idxChan = 0; idxChan < numChans; idxChan++) {
      AlignmentGeometry alignStart = Alignment.bottomCenter;
      AlignmentGeometry alignStop = Alignment.topCenter;

      Rect myRect = Rect.fromLTRB(
        shortAxisSize / 2 - devDist + offsetLevelWidget,
        deltaInner,
        shortAxisSize / 2 + devDist + offsetLevelWidget,
        longAxisSize - deltaInner,
      );

      double val = 0;
      if (idxChan < currentAvrgValue.length) {
        val = currentAvrgValue[idxChan] / numCnt;
      }
      val = min(1, val);

      if (orientation == Axis.vertical) {
        val = 1 - val;
      }

      Rect myRectCover = Rect.fromLTRB(
          shortAxisSize / 2 - devDist + offsetLevelWidget,
          deltaInner,
          shortAxisSize / 2 + devDist + offsetLevelWidget,
          deltaInner + (val) * (longAxisSize - 2 * deltaInner));

      if (orientation == Axis.horizontal) {
        alignStart = Alignment.centerLeft;
        alignStop = Alignment.centerRight;
        myRect = xySwapRect(myRect, false, 0);
        myRectCover = xySwapRect(myRectCover, true, longAxisSize);
      }

      // The two painters with gradient bgrd and black foreground
      final paintG = Paint()
        //..color = Colors.red
        ..shader = LinearGradient(
          colors: [Colors.green, Colors.red],
          begin: alignStart,
          end: alignStop,
          // transform: GradientTransform
        ).createShader(myRect);
      final paintC = Paint()..color = Colors.black;

      // Draw both rects
      canvas.drawRect(myRect, paintG);
      canvas.drawRect(myRectCover, paintC);

      val = 0;
      if (idxChan < currentMaxValue.length) {
        val = currentMaxValue[idxChan] / numCnt;
      }
      val = min(1, val);
      if (orientation == Axis.vertical) {
        val = 1 - val;
      }

      myRect = Rect.fromLTRB(
          shortAxisSize / 2 - devDist + offsetLevelWidget,
          deltaInner +
              (val) * (longAxisSize - 2 * deltaInner) -
              widthMaxLine / 2,
          shortAxisSize / 2 + devDist + offsetLevelWidget,
          deltaInner +
              (val) * (longAxisSize - 2 * deltaInner) +
              widthMaxLine / 2);
      if (orientation == Axis.horizontal) {
        myRect = xySwapRect(myRect, false, 0);
      }
      canvas.drawRect(myRect, paintG);
      offsetLevelWidget += shiftLine;
    }

    double startPosiMarker = shortAxisSize * 2 / 3;
    double stopPosiMarker = shortAxisSize * 3 / 4;

    startPosiMarker = shortAxisSize / 2 - devDist / 2;
    stopPosiMarker = shortAxisSize / 2 + devDist / 2;
    if (positionMarksShort != null) {
      for (var elm in positionMarksShort!) {
        final paint = Paint()
          ..color = Colors.white
          ..strokeWidth = 0.3;
        p1 = Offset(startPosiMarker,
            deltaInner + (longAxisSize - 2 * deltaInner) * elm / numCnt);
        p2 = Offset(stopPosiMarker,
            deltaInner + (longAxisSize - 2 * deltaInner) * elm / numCnt);
        if (orientation == Axis.horizontal) {
          p1 = xySwapOffset(p1);
          p2 = xySwapOffset(p2);
        }
        canvas.drawLine(p1, p2, paint);
      }
    }

    stopPosiMarker = shortAxisSize * 5 / 6;
    startPosiMarker = shortAxisSize / 2 - devDist * 0.8;
    stopPosiMarker = shortAxisSize / 2 + devDist * 0.8;

    if (positionMarksLong != null) {
      for (var elm in positionMarksLong!) {
        final paint = Paint()
          ..color = Colors.white
          ..strokeWidth = 0.8;

        p1 = Offset(startPosiMarker,
            deltaInner + (longAxisSize - 2 * deltaInner) * elm / numCnt);
        p2 = Offset(stopPosiMarker,
            deltaInner + (longAxisSize - 2 * deltaInner) * elm / numCnt);
        if (orientation == Axis.horizontal) {
          p1 = xySwapOffset(p1);
          p2 = xySwapOffset(p2);
        }
        canvas.drawLine(p1, p2, paint);
      }
    }

    // ==============================================================
    // Draw text labels
    // ==============================================================
    double offetLabels = shortAxisSize * 1 / 6;
    if (title.isNotEmpty) {
      offs = Offset(
          offetLabels, deltaInner + 0.5 * (longAxisSize - 2 * deltaInner));
      if (orientation == Axis.horizontal) {
        offs = xySwapOffset(offs);
      }
      localDrawText(title, canvas, offs, ftSize, fontStyle: FontStyle.normal);
    }
    offetLabels = shortAxisSize * 3 / 4;
    if (labels != null) {
      for (var elml in labels!) {
        double posiEff = elml.posi / numCnt.toDouble();
        if (orientation != Axis.horizontal) {
          posiEff = (1 - posiEff);
        }
        offs = Offset(offetLabels,
            deltaInner + posiEff * (longAxisSize - 2 * deltaInner));

        if (orientation == Axis.horizontal) {
          offs = xySwapOffset(offs);
        }
        localDrawText(elml.txt, canvas, offs, ftSize);
      }
    }
  }

  Offset xySwapOffset(Offset val) {
    return Offset(val.dy, val.dx);
  }

  Rect xySwapRect(Rect val, bool lrSwap, double longAxisSize) {
    if (lrSwap)
      return Rect.fromLTRB(
          val.bottom, val.left, longAxisSize - val.top, val.right);
    return Rect.fromLTRB(val.top, val.left, val.bottom, val.right);
  }

  @override
  bool shouldRepaint(SliderPainter old) {
    // currentValue = randInst.nextDouble();
    for (int idx = 0; idx < numChans; idx++) {
      if (old.currentAvrgValue[idx] != currentAvrgValue[idx]) {
        return true;
      }

      if (old.currentMaxValue[idx] != currentMaxValue[idx]) {
        return true;
      }
    }

    return true;
  }

  void localDrawText(String lab, Canvas canvas, Offset offs, double ftSize,
      {FontStyle fontStyle = FontStyle.italic, double rotate = 0}) {
    Paint pt = Paint();
    pt.color = Colors.white;
    double ww = 0;
    double hh = 0;
    Color colText = Colors.black;

    FontWeight ftW = FontWeight.normal;

    // https://stackoverflow.com/questions/49408146/drawing-text-in-a-specific-spot-using-flutter
    canvas.save();
    //canvas.translate(100.0, 100.0);
    canvas.rotate(rotate);

    // https://stackoverflow.com/questions/68984666/the-argument-type-textdirection-cant-be-assigned-to-the-parameter-type-textd
    TextSpan span1 = TextSpan(
        style: new TextStyle(
            color: colText,
            fontWeight: ftW,
            fontStyle: fontStyle,
            fontSize: ftSize),
        text: lab);
    TextPainter tp1 = new TextPainter(
        text: span1,
        textAlign: TextAlign.center,
        textDirection: TextDirection.ltr);
    tp1.layout();

    ww = tp1.width;
    hh = tp1.height;

    double xx = 0;
    double yy = 0;
    xx = offs.dx - ww / 2;
    yy = offs.dy - hh / 2;

    Offset drawHere = Offset(xx, yy);
    //Rect textBox = Rect.fromLTWH(xx, offs.dy, ww, hh).inflate(ref.deflateInner);
    //canvas.drawRect(textBox, pt);

    // double yy = offs.dy + ref.deflateInner / 2;
    //offs = Offset(xx, yy);
    tp1.paint(canvas, drawHere);
    canvas.restore();
  }
}

class AyfLabelPosition {
  String txt;
  double posi;
  AyfLabelPosition(this.txt, this.posi);
}
