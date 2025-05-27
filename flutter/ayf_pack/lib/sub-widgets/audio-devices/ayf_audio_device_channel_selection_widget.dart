import 'package:provider/provider.dart';
import 'package:flutter/material.dart';
import 'package:collection/collection.dart';
import '../../ayf_pack_local.dart';

class AudYoFloChannelSelectionSliverAppBar extends StatelessWidget {
  final String text;
  final AudYoFloPropertySelectionListBackend? props;
  AudYoFloChannelSelectionSliverAppBar(this.text, this.props);

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);
    return SliverAppBar(
      title: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
        Flexible(
          child: Center(
              child: Text(
            text,
            style: TextStyle(color: Colors.white, fontSize: 16),
          )),
          flex: 1,
        ),
        Flexible(
            child: ElevatedButton(
              child: Padding(
                  padding: const EdgeInsets.all(8.0), child: Icon(Icons.add)),
              onPressed: () {
                if (props != null) {
                  List<String> lst = [];
                  for (int posi = 0;
                      posi < props!.parpropms.entries.length;
                      posi++) {
                    props!.selection.bitSet(posi);
                  }
                  lst.add(props!.parpropms.descriptor);
                  theBeCache.triggerSetProperties(
                      props!.parpropms.assCpIdent, lst);
                }
              },
            ),
            flex: 1),
        Flexible(
            child: ElevatedButton(
              child: Padding(
                  padding: const EdgeInsets.all(8.0),
                  child: Icon(Icons.remove)),
              onPressed: () {
                if (props != null) {
                  List<String> lst = [];
                  props!.selection.bitFClear();
                  lst.add(props!.parpropms.descriptor);
                  theBeCache.triggerSetProperties(
                      props!.parpropms.assCpIdent, lst);
                }
              },
            ),
            flex: 1),
      ]),
      backgroundColor: Colors.blueGrey,
      pinned: false,
      stretch: false,
      onStretchTrigger: () async {
        print('Hello');
      },
      floating: true,
      snap: true,
      //expandedHeight: 200
    );
  }
}

mixin AudYoFloOneChannelSliverReport {
  void reportTapChannel(int idChannel, bool isInput);
}

class AudYoFloOneChannelSliver extends StatelessWidget {
  final String channelName;
  final idChannel;
  final bool isSelected;
  final bool isInput;
  final AudYoFloOneChannelSliverReport report;

  AudYoFloOneChannelSliver(this.channelName, this.idChannel, this.isSelected,
      this.isInput, this.report);

  @override
  Widget build(BuildContext context) {
    Color col = Colors.grey;

    AudYoFloChannelTypeClass clsOnReturn = AudYoFloChannelTypeClass();
    if (isSelected) col = Colors.yellow;

    AudYoFloHelper.decodeInputOutputChannel(
        channelName, clsOnReturn, idChannel);
    String showChannelName = clsOnReturn.name;
    return GestureDetector(
      onTap: () {
        report.reportTapChannel(idChannel, isInput);
      },
      child: Card(
          color: col,
          child: Row(
              //mainAxisAlignment: MainAxisAlignment.start, children: [
              children: [
                Flexible(
                    flex: 10,
                    child: Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Container(
                            color: col, child: Text(showChannelName)))),
              ])),
    );
  }
}

class AudYoFloAudioDeviceInputOutputChannelWidget extends StatefulWidget {
  AudYoFloAudioDeviceInputOutputChannelWidget(this.cpId);
  final JvxComponentIdentification cpId;
  @override
  State<StatefulWidget> createState() {
    return _AudYoFloAudioDeviceInputOutputChannelWidgetStates();
  }
}

class _AudYoFloAudioDeviceInputOutputChannelWidgetStates
    extends State<AudYoFloAudioDeviceInputOutputChannelWidget>
    with AudYoFloOneChannelSliverReport {
  ScrollController _controllerL = ScrollController();
  ScrollController _controllerR = ScrollController();
  AudYoFloPropertySelectionListBackend? inChannels;
  AudYoFloPropertySelectionListBackend? outChannels;
  late AudYoFloBackendCache theBeCache;
  JvxComponentIdentification cpId = JvxComponentIdentification();

  void reportTapChannel(int idChannel, bool isInput) {
    List<String> lst = [];
    if (isInput) {
      if (inChannels != null) {
        if (inChannels!.selection.bitTest(idChannel)) {
          inChannels!.selection.bitClear(idChannel);
        } else {
          inChannels!.selection.bitSet(idChannel);
        }
        lst.add(inChannels!.parpropms.descriptor);
        theBeCache.triggerSetProperties(outChannels!.parpropms.assCpIdent, lst);
      }
    } else {
      if (outChannels != null) {
        if (outChannels!.selection.bitTest(idChannel)) {
          outChannels!.selection.bitClear(idChannel);
        } else {
          outChannels!.selection.bitSet(idChannel);
        }
        lst.add(outChannels!.parpropms.descriptor);
        theBeCache.triggerSetProperties(outChannels!.parpropms.assCpIdent, lst);
      }
    }
  }

  // ===============================================================
  @override
  Widget build(BuildContext context) {
    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);

    // Take cpId from
    if (widget.cpId.cpTp != JvxComponentTypeEnum.JVX_COMPONENT_UNKNOWN) {
      cpId = widget.cpId;
    }
    return Selector<AudYoFloBackendCache, int>(
        selector: (context, beCache) {
          // if property is not there, return a -1
          int retVal = -1;

          // We are waiting for an update of the properties <ssUpdateId>
          retVal = beCache.updateIdComponentInCache(cpId);

          // Return the current id
          return retVal;
        },

        // Return true if the id is a different one than it was before
        shouldRebuild: (previous, next) => (next != previous) ? true : false,

        // The builder
        builder: (context, notifier, child) {
          Future<int> triggerChannelLists(
              JvxComponentIdentification cpTp) async {
            int errCode = jvxErrorType.JVX_NO_ERROR;
            List<AudYoFloPropertyContainer>? props;
            List<String> lst = [
              '/system/sel_input_channels',
              '/system/sel_output_channels',
            ];
            props = theBeCache.referenceValidPropertiesComponents(cpTp, lst);
            if (props == null) {
              errCode =
                  await theBeCache.triggerUpdatePropertiesComponent(cpTp, lst);
              if (errCode == jvxErrorType.JVX_NO_ERROR) {
                props =
                    theBeCache.referenceValidPropertiesComponents(cpTp, lst);
              } else {
                cpId.reset();
              }
            }

            inChannels = null;
            outChannels = null;
            if (props != null) {
              AudYoFloPropertyContainer? iP = props.firstWhereOrNull(
                  (element) =>
                      element.descriptor == '/system/sel_input_channels');
              if (iP != null) {
                if (iP is AudYoFloPropertySelectionListBackend) {
                  inChannels = iP as AudYoFloPropertySelectionListBackend;
                }
              }

              AudYoFloPropertyContainer? oP = props.firstWhereOrNull(
                  (element) =>
                      element.descriptor == '/system/sel_output_channels');
              if (oP != null) {
                if (oP is AudYoFloPropertySelectionListBackend) {
                  outChannels = oP as AudYoFloPropertySelectionListBackend;
                }
              }
            }

            return errCode;
          }

          return FutureBuilder<int>(
              future: triggerChannelLists(cpId),
              builder: (BuildContext context, AsyncSnapshot<int> snapshot) {
                //if (snapshot.connectionState == ConnectionState.done) {
                // The future has been completed. Check the errorcode
                int errCode = jvxErrorType.JVX_ERROR_INTERNAL;
                int? errCodePtr = snapshot.data;
                if (errCodePtr != null) {
                  errCode = errCodePtr;
                }

                //if (errCode == jvxErrorType.JVX_NO_ERROR) {
                // Looks good, future operated as desired
                return Row(children: [
                  Flexible(
                    flex: 1,
                    child: CustomScrollView(
                      controller: _controllerL,
                      slivers: [
                        AudYoFloChannelSelectionSliverAppBar(
                            'Input Channels', inChannels),
                        SliverList(
                          delegate: SliverChildBuilderDelegate(
                              (BuildContext context, int index) {
                            return AudYoFloOneChannelSliver(
                                inChannels!.parpropms.entries[index],
                                index,
                                inChannels!.selection.bitTest(index),
                                true,
                                this);
                          },
                              childCount: (inChannels != null)
                                  ? inChannels!.parpropms.entries.length
                                  : 0),
                        )
                      ],
                    ),
                  ),
                  Flexible(
                    flex: 1,
                    child: CustomScrollView(
                      controller: _controllerR,
                      slivers: [
                        AudYoFloChannelSelectionSliverAppBar(
                            'Output Channels', outChannels),
                        SliverList(
                          delegate: SliverChildBuilderDelegate(
                              (BuildContext context, int index) {
                            // Prepare the drag data

                            return AudYoFloOneChannelSliver(
                                outChannels!.parpropms.entries[index],
                                index,
                                outChannels!.selection.bitTest(index),
                                false,
                                this);
                          },
                              childCount: (outChannels != null)
                                  ? outChannels!.parpropms.entries.length
                                  : 0),
                        )
                      ],
                    ),
                  ),
                ]);
              });
        });
  }
}
