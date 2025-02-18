import 'dart:typed_data';
import 'dart:math';

import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:flutter/material.dart';
import 'package:ayf_pack/ayf_pack_local.dart';
import 'package:collection/collection.dart';
import 'package:provider/provider.dart';

class ChartData {
  final double x;
  final double y;

  ChartData(this.x, this.y);
}

class AudYoFloPropertyPlot extends StatefulWidget {
  final JvxComponentIdentification cpId;
  final List<String> linePropertiesInit;
  final List<String> lineNamesInit;
  final int sRate;
  /*
  final List<double> xValues;
  List<double> xValues = new List<double>.generate(
          fftLength, (i) => i * sampleRate / 2.0 / fftLength);
          */
  final String? titleXAxis;
  final String? titleYAxis;
  final bool logarithmicX;
  final bool logarithmicY;
  final Duration? updateInterval;
  final List<String>? labels;

  AudYoFloPropertyPlot(
      this.cpId, this.linePropertiesInit, this.lineNamesInit, this.sRate,
      {this.logarithmicX = false,
      this.logarithmicY = false,
      this.titleXAxis,
      this.titleYAxis,
      this.updateInterval,
      this.labels,
      super.key});

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloPropertyPlotState(linePropertiesInit, lineNamesInit);
  }
}

class _AudYoFloPropertyPlotState extends State<AudYoFloPropertyPlot>
    with
        AudYoFloMultiComponentsPropertyOnChange,
        AudYoFloPropertyOnChangeGroups {
  AudYoFloPropertyOnChangeOneGroup? oneGroupPlotWidget;
  List<double> xValues = [];
  List<String> lineProperties;
  List<String> lineNames;
  // late Timer _timer;
  Map<String, List<ChartData>> _chartData = <String, List<ChartData>>{};
  AudYoFloBackendCache? theBeCache;
  // Need to declare a component but we will change that later when starting
  AudYoFloOneComponentSettingsBackend mySettings =
      AudYoFloOneComponentSettingsBackend(
          JvxComponentIdentification(
              cpTp: JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN,
              slotid: -1,
              slotsubid: -1),
          'tagPropertyPlot',
          []);

  _AudYoFloPropertyPlotState(this.lineProperties, this.lineNames) {
    // Set required member variable in base class
    /*
    super.cpId = JvxComponentIdentification(
        cpTp: JvxComponentTypeEnum.JVX_COMPONENT_AUDIO_NODE,
        slotid: 0,
        slotsubid: 0);
        */

    // If some events do not reach the rebuild function, we can produce some
    // additional output printfs
    // dbgOutput = true;
    oneGroupPlotWidget = AudYoFloPropertyOnChangeOneGroup(lineProperties);
    theGroups['plotWidget'] = oneGroupPlotWidget!;

    createList();

    // Store the local property component
    mySettings.propsDescriptors = propsAllGroups;
  }

  @override
  void initState() {
    super.initState();
    // Simulate data update
    updateChartData();
    /*
        if (widget.updateInterval != null) {
      _timer = Timer.periodic(widget.updateInterval!, (timer) {
        setState(() {
          updateChartData();
        });
      });
    }*/
  }

  void updateChartData() {
    // dbgPrint("Update Char Data");

    int fftLength2P1 = 0;
    for (var prop in lineProperties) {
      var pp = oneGroupPlotWidget!.extractPropMultiContent(prop);
      if (pp != null) {
        AudYoFloPropertyMultiContentBackend? propertyData;
        if (pp is AudYoFloPropertyMultiContentBackend<Float64List>?) {
          propertyData =
              pp as AudYoFloPropertyMultiContentBackend<Float64List>?;
        } else if (pp is AudYoFloPropertyMultiContentBackend<Float32List>?) {
          propertyData =
              pp as AudYoFloPropertyMultiContentBackend<Float32List>?;
        }

        if (propertyData != null) {
          fftLength2P1 = max(fftLength2P1, propertyData.fldSz);
        }
      }
    }

    // If the the size requires an update, update
    if (fftLength2P1 != xValues.length) {
      xValues = new List<double>.generate(
          fftLength2P1, (i) => i * widget.sRate / 2.0 / fftLength2P1);
    }

    for (int j = 0; j < lineProperties.length; j++) {
      final String property = lineProperties[j];
      var pp = oneGroupPlotWidget!.extractPropMultiContent(property);
      if (pp != null) {
        AudYoFloPropertyMultiContentBackend? propertyData;
        if (pp is AudYoFloPropertyMultiContentBackend<Float64List>?) {
          propertyData =
              pp as AudYoFloPropertyMultiContentBackend<Float64List>?;
        } else if (pp is AudYoFloPropertyMultiContentBackend<Float32List>?) {
          propertyData =
              pp as AudYoFloPropertyMultiContentBackend<Float32List>?;
        }

        if (propertyData == null || propertyData.fldSz != xValues.length)
          continue; // TODO log warning/error

        String nmShow = property;
        if (j < lineNames.length) {
          nmShow = lineNames[j];
        }
        _chartData.update(nmShow, (data) {
          data.clear();
          for (int i = 0; i < xValues.length; i++) {
            double value = propertyData!.fld[i];
            // TODO NaN/Infinite workaround..
            data.add(ChartData(
                xValues[i], (value.isNaN || value.isInfinite) ? 0 : value));
          }
          return data;
        }, ifAbsent: () {
          List<ChartData> data = <ChartData>[];
          for (int i = 0; i < xValues.length; i++) {
            double value = propertyData!.fld[i];
            // TODO NaN/Infinite workaround..
            data.add(ChartData(
                xValues[i], (value.isNaN || value.isInfinite) ? 0 : value));
          }
          return data;
        });
      }
    }
  }

  @override
  void dispose() {
    // _timer.cancel();
    super.dispose();
  }

  @override
  AudYoFloOneComponentSettings? requestedPropertiesList(
      JvxComponentIdentification cpId) {
    if (cpId == mySettings.cpId) {
      return mySettings;
    }
    return null;
  }

  void reconstructProperties(List<AudYoFloPropertyContainer> props) {
    reconstructProperyReferences(props);
  }

  @override
  Widget innerWidget(BuildContext context) {
    bool validData = false;

    // Reconstruct the properties from the latestResults struct
    var fElm = latestResult.firstWhereOrNull((str) {
      if (str is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        return (str.cpId == mySettings.cpId);
      }
      return false;
    });
    if (fElm != null) {
      if (fElm is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        if (fElm.bprops != null) {
          validData = true;
          reconstructProperties(fElm.bprops!);
        }
      }
    }

    // This widget is redrawn every tomestamp of the slow thread.
    // Then, typically the data is invalid - property reference are null.
    // In this case, no data update is required.
    // Because the properties were invalid, however, the AudYoFloMultiComponentsPropertyOnChange
    // class will run the future to actually get valid data!
    // Then, in the following request, the data is valid. We will update the plots and finally invalidate the
    // properties again to expect new data on the next timer period.
    if (validData) {
      // =================================================================
      // dbgPrint("Updating chart");
      updateChartData();

      // dbgPrint("Invalidating property");
      if (theBeCache != null) {
        theBeCache!
            .invalidatePropertiesComponent(widget.cpId, lineProperties, true);
      }
    } else {
      // dbgPrint("No new data available");
    }

    ChartAxis? xAxis;
    if (this.widget.logarithmicX) {
      xAxis = LogarithmicAxis(
        title: AxisTitle(text: this.widget.titleXAxis),
        logBase: 10,
        //minorTicksPerInterval: 9, // NOTE minor ticks are logarithmically spaced (in data; so visually linear) unfortunately
      );
    } else {
      xAxis = NumericAxis(title: AxisTitle(text: this.widget.titleXAxis));
    }

    ChartAxis? yAxis;
    if (this.widget.logarithmicY) {
      yAxis = LogarithmicAxis(
        title: AxisTitle(text: this.widget.titleYAxis),
        logBase: 10,
        //minorTicksPerInterval: 9, // NOTE minor ticks are logarithmically spaced (in data; so visually linear) unfortunately
      );
    } else {
      yAxis = NumericAxis(
        title: AxisTitle(text: this.widget.titleYAxis),
        minimum: -80,
        maximum: 10,
      );
    }

    List<CartesianSeries> series = <CartesianSeries>[];
    for (final MapEntry<String, List<ChartData>> entry in _chartData.entries) {
      series.add(FastLineSeries<ChartData, double>(
        name: entry.key,
        dataSource: entry.value,
        xValueMapper: (ChartData data, _) => data.x,
        yValueMapper: (ChartData data, _) => data.y,
      ));
    }

    // Update this widget if there is a notification in the uiModel
    return Consumer<AudYoFloUiModel>(builder: (context, notifier, child) {
      return SfCartesianChart(
        title: ChartTitle(
            text: "Ipsilateral CTC transfer function absolute values"),
        primaryXAxis: xAxis,
        primaryYAxis: yAxis,
        series: series,
        legend: Legend(isVisible: true),
      );
    });
  }

  @override
  Widget build(BuildContext context) {
    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    if (configured == false) {
      // Read out the component of focus from the widget
      mySettings.cpId = widget.cpId;
      resetMultiPropertySettings();
      setComponentConsider(mySettings.cpId);
    }
    return localBuild(context);
  }
}

/*
Selector<AudYoFloPeriodicNotifier, bool>(
              selector: (context, perNotify) => perNotify.fast,
              //shouldRebuild: (previous, next) => next ? false : true,
              shouldRebuild: (previous, next) => next ? true : false,
              builder: (context, notifier, child) {
              */
