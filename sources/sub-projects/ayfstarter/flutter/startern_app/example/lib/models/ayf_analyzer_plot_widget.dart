import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:pixel_buffer_texture/pixel_buffer_plot_renderer.dart';

// ============================================================================
// Real-time analyzer view (spectrogram + waveform) of the analyzer node
// (jvxAuNAnalyze, audio node slot 2). Flutter counterpart of
// CjvxQtMeasureAudio_spectrum in ayfstartern-qt: two plot views, each
// connected to one rendering target of the node ("/expose_plot/rendering_target"
// and "/expose_plot/rendering_target_sec"). Rendering happens in the backend
// with jvxVideoRenderCore (pixbuf connection, mode=plot_inc), the axis labels
// are derived from the axis description properties of the node.
// ============================================================================

// Buffer description properties, same as in CjvxQtMeasureAudio_spectrum
const String _propsBuffer = 'sx=/expose_plot/segmentsize_x'
    ':sy=/expose_plot/segmentsize_y'
    ':f=/expose_plot/format'
    ':fg=/expose_plot/subformat'
    ':om=/expose_plot/operation_mode'
    ':nb=/expose_plot/number_buffers'
    ':mode=plot_inc';

class AudYoFloAnalyzerPlotWidget extends StatefulWidget {
  final JvxComponentIdentification cpId;
  const AudYoFloAnalyzerPlotWidget(this.cpId, {super.key});

  @override
  State<StatefulWidget> createState() => _AudYoFloAnalyzerPlotWidgetState();
}

class _AudYoFloAnalyzerPlotWidgetState extends State<AudYoFloAnalyzerPlotWidget>
    with
        AudYoFloMultiComponentsPropertyOnChange,
        AudYoFloPropertyOnChangeGroups,
        AutomaticKeepAliveClientMixin<AudYoFloAnalyzerPlotWidget> {
  AudYoFloPropertyOnChangeOneGroup oneGroupAxis =
      AudYoFloPropertyOnChangeOneGroup([
    '/unit_x',
    '/descr_x',
    '/max_y_pri',
    '/min_y_pri',
    '/descr_y_pri',
    '/max_y_sec',
    '/min_y_sec',
    '/descr_y_sec',
    '/expose_plot/segmentsize_y',
    // Display parameters (as in CjvxQtMeasureAudio_spectrum)
    '/expose_plot/gain_factor_waveform',
    '/expose_plot/smooth_factor_waveform',
    '/expose_plot/sensitivity_spectrum',
    '/select_channel',
  ]);

  AudYoFloOneComponentSettingsBackend mySettings =
      AudYoFloOneComponentSettingsBackend(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
              slotid: -1,
              slotsubid: -1),
          'tagPropertyAnalyzerPlotWidget',
          []);

  // UI options as in CjvxQtMeasureAudio_spectrum
  PixelBufferPlotLegendPosition legendPos =
      PixelBufferPlotLegendPosition.rightTop;
  bool showGrid = true;

  // Number of divisions as in CjvxQtMeasureAudio_spectrum
  static const int numDivX = 8;
  static const int numDivY = 4;

  _AudYoFloAnalyzerPlotWidgetState() {
    theGroups['analyzerPlot'] = oneGroupAxis;
    createList();
    mySettings.propsDescriptors = propsAllGroups;
  }

  int get cache_status {
    assert(false);
    return 0;
  }

  @override
  AudYoFloOneComponentSettings? requestedPropertiesList(
      JvxComponentIdentification cpId) {
    if (cpId == mySettings.cpId) {
      return mySettings;
    }
    return null;
  }

  // ==========================================================================
  // Property helpers
  // ==========================================================================
  double _readDouble(String tag, double def) {
    var mc = oneGroupAxis.extractPropMultiContent(tag);
    if (mc != null) {
      var fld = mc.fld;
      if ((fld != null) && (mc.fldSz > 0)) {
        var v = fld[0];
        if (v is num) {
          return v.toDouble();
        }
      }
    }
    return def;
  }

  String _readString(String tag, String def) {
    var ps = oneGroupAxis.extractPropString(tag);
    if (ps != null) {
      return ps.value;
    }
    return def;
  }

  static String _fmt(double v) {
    double a = v.abs();
    if (a >= 100) return v.toStringAsFixed(0);
    if (a >= 10) return v.toStringAsFixed(1);
    return v.toStringAsFixed(2);
  }

  // Same label computation as in CjvxQtMeasureAudio_spectrum::processing_started:
  // x from -(ll * unit_x) to 0, y from min to max
  PixelBufferPlotConfig _overlay(
      {required String title,
      required String suffixY,
      List<PixelBufferPlotLegendEntry> legend = const []}) {
    double unitX = _readDouble('/unit_x', 0);
    double ll = _readDouble('/expose_plot/segmentsize_y', 0);
    double maxY = _readDouble('/max_y_$suffixY', 1);
    double minY = _readDouble('/min_y_$suffixY', 0);

    List<String> labelsX = [];
    List<String> labelsY = [];
    if (unitX > 0 && ll > 0) {
      int n = numDivX + 1;
      double axisFull = ll * unitX;
      for (int i = 0; i < n; i++) {
        labelsX.add(_fmt(-(axisFull - axisFull * i / (n - 1))));
      }
    }
    int n = numDivY + 1;
    for (int i = 0; i < n; i++) {
      labelsY.add(_fmt(minY + (maxY - minY) * i / (n - 1)));
    }

    return PixelBufferPlotConfig(
        numDivX: numDivX,
        numDivY: numDivY,
        showGrid: showGrid,
        labelsX: labelsX,
        labelsY: labelsY,
        axisLabelX: _readString('/descr_x', ''),
        axisLabelY: _readString('/descr_y_$suffixY', ''),
        title: title,
        legend: legend,
        legendPos:
            legend.isEmpty ? PixelBufferPlotLegendPosition.notShown : legendPos);
  }

  // ==========================================================================

  Widget _controls() {
    const Map<PixelBufferPlotLegendPosition, String> legendNames = {
      PixelBufferPlotLegendPosition.notShown: 'Off',
      PixelBufferPlotLegendPosition.leftTop: 'Top left',
      PixelBufferPlotLegendPosition.rightTop: 'Top right',
      PixelBufferPlotLegendPosition.leftBottom: 'Bottom left',
      PixelBufferPlotLegendPosition.rightBottom: 'Bottom right',
    };
    return Wrap(
        crossAxisAlignment: WrapCrossAlignment.center,
        spacing: 8,
        runSpacing: 4,
        children: [
      ..._propControls(),
      const SizedBox(width: 16),
      const Text('Legend: '),
      DropdownButton<PixelBufferPlotLegendPosition>(
          value: legendPos,
          items: legendNames.entries
              .map((e) => DropdownMenuItem(value: e.key, child: Text(e.value)))
              .toList(),
          onChanged: (v) {
            if (v != null) {
              setState(() => legendPos = v);
            }
          }),
      Checkbox(
          value: showGrid,
          onChanged: (v) => setState(() => showGrid = v ?? true)),
      const Text('Grid'),
    ]);
  }

  // Backend properties of the analyzer node: scalar values in text fields
  // (applied on <Enter>), channel selection in a combobox. Properties which
  // are not exposed by the node (e.g. waveform plotter not compiled in) are
  // skipped.
  List<Widget> _propControls() {
    List<Widget> lst = [];

    void addValue(String label, String tag) {
      var prop = oneGroupAxis.extractPropMultiContent(tag);
      if (prop != null) {
        lst.add(Text(label));
        lst.add(SizedBox(
            width: 90,
            child: AudYoFloSimpleTextField(
                propBe: prop.parpropc, idx: 0, height: 36)));
      }
    }

    addValue('Gain waveform:', '/expose_plot/gain_factor_waveform');
    // Smoothing in [0, 0.99999], clamped in the backend
    addValue('Smoothing:', '/expose_plot/smooth_factor_waveform');
    addValue('Sensitivity spectrum:', '/expose_plot/sensitivity_spectrum');

    var selChannel = oneGroupAxis.extractPropSelectionList('/select_channel');
    if (selChannel != null) {
      lst.add(const Text('Channel:'));
      lst.add(SizedBox(
          width: 160,
          child: AudYoFloPropListComboBoxWidget(
              selLstBe: selChannel, height: 36)));
    }
    return lst;
  }

  @override
  Widget innerWidget(BuildContext context) {
    bool validData = false;
    var fElm = latestResult.firstWhereOrNull((str) {
      if (str is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        return (str.cpId == mySettings.cpId);
      }
      return false;
    });
    if (fElm is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
      if (fElm.bprops != null) {
        reconstructProperyReferences(fElm.bprops!);
        validData = true;
      }
    }

    if (!validData) {
      return const Center(child: Text('Analyzer node not active'));
    }

    String rendererId = widget.cpId.txt;

    // Secondary display: "Spectral View" (upper), no legend
    Widget viewSpectrum = PixelBufferPlotRenderer(
        rendererId: rendererId,
        rendererCtxt: 'spectrum',
        rendererArg: '/expose_plot/rendering_target_sec:$_propsBuffer',
        config: _overlay(title: 'Spectrum', suffixY: 'sec'));

    // Primary display: "Waveform View" (lower), signal in white, with legend
    Widget viewWaveform = PixelBufferPlotRenderer(
        rendererId: rendererId,
        rendererCtxt: 'waveform',
        rendererArg: '/expose_plot/rendering_target:$_propsBuffer',
        config: _overlay(
            title: 'Waveform',
            suffixY: 'pri',
            legend: const [
              PixelBufferPlotLegendEntry(Colors.white, 'Signal x(k)')
            ]));

    return Padding(
        padding: const EdgeInsets.all(8.0),
        child: Column(children: [
          _controls(),
          Expanded(child: viewSpectrum),
          const SizedBox(height: 8),
          Expanded(child: viewWaveform),
        ]));
  }

  // Keep the plot views alive if another tab is shown: the rendering targets are
  // installed in the backend when processing starts - a view created later only
  // shows content after the next start.
  @override
  bool get wantKeepAlive => true;

  @override
  Widget build(BuildContext context) {
    super.build(context);
    if (configured == false) {
      mySettings.cpId = widget.cpId;
      resetMultiPropertySettings();
      setComponentConsider(mySettings.cpId);
    }
    return localBuild(context);
  }
}
