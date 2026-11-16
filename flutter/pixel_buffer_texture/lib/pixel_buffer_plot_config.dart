import 'package:flutter/painting.dart';
import 'package:collection/collection.dart';

/// Legend position, same order as jvxRenderLegendPosition in jvxVideoOverlay.h
enum PixelBufferPlotLegendPosition {
  notShown,
  leftTop,
  rightTop,
  leftBottom,
  rightBottom
}

class PixelBufferPlotLegendEntry {
  final Color color;
  final String text;
  const PixelBufferPlotLegendEntry(this.color, this.text);

  @override
  bool operator ==(Object other) =>
      other is PixelBufferPlotLegendEntry &&
      other.color == color &&
      other.text == text;

  @override
  int get hashCode => Object.hash(color, text);
}

/// Overlay of a plot rendered by the backend (frame, axes, grid, tick labels,
/// axis labels, title and legend), corresponds to jvxVideoOverlayConfig.
class PixelBufferPlotConfig {
  final bool active;
  final int numDivX;
  final int numDivY;
  final bool showGrid;

  /// Tick labels. x: first entry left, y: first entry at the bottom.
  /// With num_div + 1 entries, every tick is labeled.
  final List<String> labelsX;
  final List<String> labelsY;
  final String axisLabelX;
  final String axisLabelY;
  final String title;
  final int fontSize;
  final int fontSizeTitle;
  final List<PixelBufferPlotLegendEntry> legend;
  final PixelBufferPlotLegendPosition legendPos;

  const PixelBufferPlotConfig({
    this.active = true,
    this.numDivX = 8,
    this.numDivY = 4,
    this.showGrid = true,
    this.labelsX = const [],
    this.labelsY = const [],
    this.axisLabelX = '',
    this.axisLabelY = '',
    this.title = '',
    this.fontSize = 11,
    this.fontSizeTitle = 13,
    this.legend = const [],
    this.legendPos = PixelBufferPlotLegendPosition.rightTop,
  });

  PixelBufferPlotConfig copyWith({
    bool? active,
    int? numDivX,
    int? numDivY,
    bool? showGrid,
    List<String>? labelsX,
    List<String>? labelsY,
    String? axisLabelX,
    String? axisLabelY,
    String? title,
    int? fontSize,
    int? fontSizeTitle,
    List<PixelBufferPlotLegendEntry>? legend,
    PixelBufferPlotLegendPosition? legendPos,
  }) {
    return PixelBufferPlotConfig(
      active: active ?? this.active,
      numDivX: numDivX ?? this.numDivX,
      numDivY: numDivY ?? this.numDivY,
      showGrid: showGrid ?? this.showGrid,
      labelsX: labelsX ?? this.labelsX,
      labelsY: labelsY ?? this.labelsY,
      axisLabelX: axisLabelX ?? this.axisLabelX,
      axisLabelY: axisLabelY ?? this.axisLabelY,
      title: title ?? this.title,
      fontSize: fontSize ?? this.fontSize,
      fontSizeTitle: fontSizeTitle ?? this.fontSizeTitle,
      legend: legend ?? this.legend,
      legendPos: legendPos ?? this.legendPos,
    );
  }

  static String _escape(String v) =>
      v.replaceAll('\\', '\\\\').replaceAll('\n', '\\n');

  /// Text representation as expected by the backend (configuration key
  /// "config", see TpjvxPixBuffer.h)
  String encode() {
    final lines = <String>[
      'active=${active ? 1 : 0}',
      'ndx=$numDivX',
      'ndy=$numDivY',
      'grid=${showGrid ? 1 : 0}',
      for (final l in labelsX) 'lx=${_escape(l)}',
      for (final l in labelsY) 'ly=${_escape(l)}',
      'alx=${_escape(axisLabelX)}',
      'aly=${_escape(axisLabelY)}',
      'title=${_escape(title)}',
      'fs=$fontSize',
      'fst=$fontSizeTitle',
      'legpos=${legendPos.index}',
      for (final e in legend)
        // ignore: deprecated_member_use
        'leg=${e.color.value.toRadixString(16).padLeft(8, '0')}:${_escape(e.text)}',
    ];
    return lines.join('\n');
  }

  static const _listEq = ListEquality();

  @override
  bool operator ==(Object other) {
    return other is PixelBufferPlotConfig &&
        other.active == active &&
        other.numDivX == numDivX &&
        other.numDivY == numDivY &&
        other.showGrid == showGrid &&
        _listEq.equals(other.labelsX, labelsX) &&
        _listEq.equals(other.labelsY, labelsY) &&
        other.axisLabelX == axisLabelX &&
        other.axisLabelY == axisLabelY &&
        other.title == title &&
        other.fontSize == fontSize &&
        other.fontSizeTitle == fontSizeTitle &&
        _listEq.equals(other.legend, legend) &&
        other.legendPos == legendPos;
  }

  @override
  int get hashCode => Object.hash(
      active,
      numDivX,
      numDivY,
      showGrid,
      _listEq.hash(labelsX),
      _listEq.hash(labelsY),
      axisLabelX,
      axisLabelY,
      title,
      fontSize,
      fontSizeTitle,
      _listEq.hash(legend),
      legendPos);
}
