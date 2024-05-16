import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

// Class to finally receive the properties to be show in this
// property-edit section
class AudYoFloSettingsPropertyListContent extends StatefulWidget {
  final List<AudYoFloMultiComponentsPropertyOnChangeEntry> props;
  final bool dbgMessages;
  const AudYoFloSettingsPropertyListContent(this.props, this.dbgMessages);

  @override
  State<StatefulWidget> createState() {
    return _AudYoFloSettingsPropertyListContentState();
  }
}

class AudYoFloPropertyContentSettingsEntry {
  String tag = '';
  String ttip = '';
}

class AudYoFloPropertyContentSettingsEntryBackend
    extends AudYoFloPropertyContentSettingsEntry {
  AudYoFloPropertyContainer? bprop;
}

class AudYoFloPropertyContentSettingsEntryLocal
    extends AudYoFloPropertyContentSettingsEntry {
  AudYoFloPropertyContentLocal? lprop;
}

// ============================================================================

class _AudYoFloSettingsPropertyListContentState
    extends State<AudYoFloSettingsPropertyListContent> {
  final ScrollController _controllerL = ScrollController();
  List<AudYoFloMultiComponentsPropertyOnChangeEntry> props = [];

  @override
  Widget build(BuildContext context) {
    props = widget.props;

    AudYoFloUiModel theUiModel =
        Provider.of<AudYoFloUiModel>(context, listen: false);
    // From a "multi-component" lst we transform into a linear
    // property list
    List<AudYoFloPropertyContentSettingsEntry> lstProps = [];

    int cnt = 0;

    for (AudYoFloMultiComponentsPropertyOnChangeEntry elm in props) {
      theUiModel.filterListProperties('Settings', elm);

      if (elm is AudYoFloMultiComponentsPropertyOnChangeEntryBackend) {
        if (elm.bprops != null) {
          AudYoFloPropertyContentSettingsEntry newElm =
              AudYoFloPropertyContentSettingsEntry();
          if (elm.settings.compTag.isNotEmpty) {
            newElm.tag = elm.settings.compTag;
          } else {
            newElm.tag = elm.cpId.toString();
          }
          lstProps.add(newElm);

          List<AudYoFloPropertyContainer> propL = elm.bprops!;

          // Should be of this tye due to previous functions
          AudYoFloOneComponentSettings beProps = elm.settings;
          List<String> tagl = beProps.propTags;
          List<String> tooltipl = beProps.propTooltips;
          cnt = 0;
          for (var elmp in propL) {
            AudYoFloPropertyContentSettingsEntryBackend newElmBe =
                AudYoFloPropertyContentSettingsEntryBackend();
            newElmBe.bprop = elmp;
            if (cnt < tagl.length) {
              newElmBe.tag = tagl[cnt];
            } else {
              newElmBe.tag = elmp.description;
            }

            if (cnt < tooltipl.length) {
              newElmBe.ttip = tooltipl[cnt];
            } else {
              newElmBe.ttip = elmp.description;
            }
            // newElmBe.propertyInBackend = elm.ext.propertiesFromBackend;
            lstProps.add(newElmBe);
            cnt++;
          }
        }
      } else {
        if (elm is AudYoFloMultiComponentsPropertyOnChangeEntryLocal) {
          if (elm.lprops != null) {
            AudYoFloPropertyContentSettingsEntry newElm =
                AudYoFloPropertyContentSettingsEntry();
            newElm.tag = elm.settings.compTag;
            lstProps.add(newElm);

            AudYoFloOneComponentSettings loProps = elm.settings;
            List<String> tagl = loProps.propTags;
            List<String> tooltipl = loProps.propTooltips;
            List<AudYoFloPropertyContentLocal> propL = elm.lprops!;

            cnt = 0;
            for (var elmp in propL) {
              AudYoFloPropertyContentSettingsEntryLocal newElmLo =
                  AudYoFloPropertyContentSettingsEntryLocal();
              newElmLo.lprop = elmp;
              if (cnt < tagl.length) {
                newElmLo.tag = tagl[cnt];
              } else {
                newElmLo.tag = elmp.description;
              }

              if (cnt < tooltipl.length) {
                newElmLo.ttip = tooltipl[cnt];
              } else {
                newElmLo.ttip = elmp.description;
              }
              // newElmBe.propertyInBackend = elm.ext.propertiesFromBackend;
              lstProps.add(newElmLo);
              cnt++;
            }
          }
        }
      }
    }

    //return Container(color: Colors.blue);
    return CustomScrollView(controller: _controllerL, slivers: [
      /*AudYoFloDeviceSelectionSliverAppBar(
                        'Inactive Audio Devices'),
                        */
      SliverList(
        delegate: SliverChildBuilderDelegate((BuildContext context, int index) {
          // One element for each of the entries
          return AudYoFloOnePropertySettingSliver(lstProps[index]);
        }, childCount: lstProps.length),
      )
    ]);
  }
}

class AudYoFloOnePropertySettingSliver extends StatelessWidget {
  final AudYoFloPropertyContentSettingsEntry propEntry;
  AudYoFloOnePropertySettingSliver(this.propEntry);

  @override
  Widget build(BuildContext context) {
    // Produce Device Name

    String tag = propEntry.tag;
    String toolTip = propEntry.ttip;
    Widget? mainWidget;
    if (propEntry is AudYoFloPropertyContentSettingsEntryBackend) {
      AudYoFloPropertyContentSettingsEntryBackend propEntryBe =
          propEntry as AudYoFloPropertyContentSettingsEntryBackend;

      mainWidget = AudYoFloOnePropertySettingSliverCore(propBe: propEntryBe);
    } else {
      if (propEntry is AudYoFloPropertyContentSettingsEntryLocal) {
        AudYoFloPropertyContentSettingsEntryLocal propEntryLo =
            propEntry as AudYoFloPropertyContentSettingsEntryLocal;
        mainWidget = AudYoFloOnePropertySettingSliverCore(propLo: propEntryLo);
      } else {
        mainWidget = Card(
            child: Row(mainAxisAlignment: MainAxisAlignment.center, children: [
          //SizedBox(height: 30, width: 60, child: Icon(Icons.settings)),
          Expanded(
              child: Padding(
                  padding: const EdgeInsets.all(8.0),
                  child: Center(
                      child: Container(
                          color: Colors.lightBlueAccent[50],
                          child: Text(
                            tag,
                            style: TextStyle(fontStyle: FontStyle.italic),
                          )))))
        ]));

        //Text('===============================');
      }
    }

    return mainWidget;
  }
}

class AudYoFloOnePropertySettingSliverCore extends StatelessWidget {
  final AudYoFloPropertyContentSettingsEntryBackend? propBe;
  final AudYoFloPropertyContentSettingsEntryLocal? propLo;

  AudYoFloOnePropertySettingSliverCore({this.propBe, this.propLo});

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    String text = 'not-set';
    String ttip = 'not-set';
    String tag = 'not-set';
    Widget? mainSubWidget;

    if (propBe != null) {
      var propCore = propBe!.bprop;
      ttip = propBe!.ttip;
      tag = propBe!.tag;
      if (propCore != null) {
        if (propCore is AudYoFloPropertyContentBackend) {
          text = propCore.descriptor;

          if (propCore.isValid) {
            if (propCore is AudYoFloPropertySelectionListBackend) {
              mainSubWidget = AudYoFloPropListComboBoxWidget(
                  selLstBe: propCore as AudYoFloPropertySelectionListBackend);
            } else {
              mainSubWidget = AudYoFloSimpleTextField(propBe: propCore);
            }
          } else {
            mainSubWidget = Text('-- Not available --  ',
                style: TextStyle(fontStyle: FontStyle.italic));
          }
        }
      }
    } else if (propLo != null) {
      var propCore = propLo!.lprop;
      ttip = propLo!.ttip;
      tag = propLo!.tag;
      if (propCore != null) {
        text = propCore.descriptor;

        if (propCore is AudYoFloPropertySelectionListLocal) {
          mainSubWidget = AudYoFloPropListComboBoxWidget(selLstLo: propCore);
        } else {
          mainSubWidget = AudYoFloSimpleTextField(propLo: propCore);
        }
      }
    }
    if (mainSubWidget == null) {
      mainSubWidget = Text(text);
    }

    return Tooltip(
        message: ttip,
        child: Card(
          child: LayoutBuilder(
            builder: (context, constraints) {
              return SizedBox(
                width: constraints.maxWidth,
                height: 40,
                child: Row(
                    //mainAxisAlignment: MainAxisAlignment.start, children: [
                    children: [
                      SizedBox(
                          height: 30, width: 60, child: Icon(Icons.settings)),
                      Expanded(
                        child: Row(
                            mainAxisAlignment: MainAxisAlignment.spaceBetween,
                            children: [
                              Flexible(
                                flex: 1,
                                child: Text(tag),
                              ),
                              Flexible(flex: 1, child: mainSubWidget!),
                            ]),
                      )
                    ]),
              );
            },
          ),
        ));
  }
}

/*
class AudYoFloOneLocalPropertySettingSliver extends StatelessWidget {
  final AudYoFloPropertyContentSettingsEntryLocal propLo;

  AudYoFloOneLocalPropertySettingSliver(this.propLo);

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    return Tooltip(
        message: propLo.ttip,
        child: Card(
          child: Row(
              //mainAxisAlignment: MainAxisAlignment.start, children: [
              children: [
                SizedBox(height: 30, width: 60, child: Icon(Icons.settings)),
                Expanded(
                    child: Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text(propLo.tag))),
                Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: Container(
                        color: Colors.red,
                        child: Text(propLo.lprop!.descriptor))),
              ]),
        ));
  }
}
*/