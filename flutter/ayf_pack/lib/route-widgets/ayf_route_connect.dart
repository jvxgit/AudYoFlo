import 'package:flutter/material.dart';
import '../ayf_pack_local.dart';
import 'package:provider/provider.dart';
import 'dart:async';

class AudYoFloConnectWidget extends StatefulWidget {
  const AudYoFloConnectWidget({super.key});

  @override
  _AudYoFloConnectWidgetState createState() => _AudYoFloConnectWidgetState();
}

class _AudYoFloConnectWidgetState extends State<AudYoFloConnectWidget> {
  Timer? runningTimer;

  bool firstTimeConfigDone = false;
  String ipAddrSpecified = "127.0.0.1";
  int ipPortSpecified = 8000;
  String latestError = "";
  int timeOutMSecs = 500;
  bool autoConnect = false;
  AudYoFloUiModel? uiModel;
  AudYoFloBackendCache? beCache;
  AudYoFloDebugModel? dbgModel;
  int tickStart = 0;
  bool runLoopAutoConnect = false;

  @override
  void initState() {
    super.initState();
    runningTimer =
        Timer.periodic(Duration(milliseconds: timeOutMSecs), (timer) {
      if (beCache != null) {
        if (runLoopAutoConnect) {
          print('Granularity expired, ${timer.tick}');
          if (autoConnect) {
            print('Triggering auto-connect!');
            tryStartConnect();
          }
        }
        //}
      }
    });
  }

  @override
  void dispose() {
    super.dispose();
    runningTimer!.cancel();
  }

  @override
  Widget build(BuildContext context) {
    beCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    dbgModel = Provider.of<AudYoFloDebugModel>(context, listen: false);
    uiModel = Provider.of<AudYoFloUiModel>(context, listen: false);

    // Initial read of arguments from command line

    var myAppBar = AudYoFloConnectAppBar(
        context,
        uiModel!.textApp,
        uiModel!.imageNameApp,
        uiModel!.imageWScaleApp,
        uiModel!.imageOAScaleApp,
        uiModel!.heightUpperNavBar,
        beCache);

    if ((uiModel != null) && (!firstTimeConfigDone)) {
      ipAddrSpecified = uiModel!.httpConnect.ipAddr;
      ipPortSpecified = uiModel!.httpConnect.ipPort;
      autoConnect = uiModel!.httpConnect.autoConnect;

      int cnt = 0;

      for (; cnt < uiModel!.platformSpec!.cmdArgs.length; cnt++) {
        if (uiModel!.platformSpec!.cmdArgs[cnt] == "--httpAutoConnect") {
          autoConnect = true;
          continue;
        }
        if (uiModel!.platformSpec!.cmdArgs[cnt] == "--httpIpAddr") {
          cnt++;
          if (cnt < uiModel!.platformSpec!.cmdArgs.length) {
            ipAddrSpecified = uiModel!.platformSpec!.cmdArgs[cnt];
          }
          continue;
        }
        if (uiModel!.platformSpec!.cmdArgs[cnt] == "--httpIpPort") {
          cnt++;
          if (cnt < uiModel!.platformSpec!.cmdArgs.length) {
            ipPortSpecified = int.parse(uiModel!.platformSpec!.cmdArgs[cnt]);
          }
          continue;
        }

        firstTimeConfigDone = true;
      }
    }

    runLoopAutoConnect = autoConnect && !beCache!.httpConnect.isConnected;

    if (beCache!.backendAdapterIf != null) {
      latestError = beCache!.backendAdapterIf!.lastError;
    }

    if (!beCache!.httpConnect.isConnected) {
      tickStart = runningTimer!.tick;
    }

    String showText = 'Specify address and port of the backend';
    if (autoConnect) {
      showText += ' [auto]';
    } else {
      showText += ' [no-auto]';
    }
    Widget? centeredTextFragment = null;
    if (latestError.isEmpty) {
      centeredTextFragment = Center(
          child: Text(showText,
              style: TextStyle(
                  fontWeight: FontWeight.bold,
                  color: Colors.white,
                  fontSize: 16)));
    } else {
      centeredTextFragment = Center(
        child: Column(
          children: [
            Text(showText,
                style: TextStyle(
                    fontWeight: FontWeight.bold,
                    color: Colors.white,
                    fontSize: 16)),
            Text(latestError,
                style: TextStyle(
                    fontWeight: FontWeight.bold,
                    fontStyle: FontStyle.italic,
                    color: Colors.yellow,
                    fontSize: 12)),
          ],
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
        ),
      );
    }
    return LayoutBuilder(
      builder: (context, constraints) => Scaffold(
        appBar: myAppBar,
        //automaticallyImplyLeading: false,
        //title: Center(child: const Text('Connect to Html Backend')),
        //),
        body: Center(
            child: ConstrainedBox(
                constraints: constraints / 2,
                child: DecoratedBox(
                  decoration: BoxDecoration(
                      color: Colors.blueGrey,
                      border: Border.all(),
                      borderRadius: BorderRadius.circular(5),
                      boxShadow: [
                        BoxShadow(
                            color: Colors.white,
                            offset: Offset(-2, -2),
                            blurRadius: 3),
                        BoxShadow(
                            color: Colors.black,
                            offset: Offset(10, 10),
                            blurRadius: 8)
                      ]),
                  child: Container(
                    child: Column(
                        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: [
                          Expanded(
                              flex: 1,
                              child: Padding(
                                padding: const EdgeInsets.all(8.0),
                                child: Center(
                                  child: Container(
                                      // color: Colors.blueGrey,
                                      child: centeredTextFragment),
                                ),
                              )),
                          Expanded(
                            flex: 2,
                            child: SizedBox(
                              width: constraints.maxWidth / 2,
                              child: Container(
                                color: Colors.white,
                                child: Row(
                                  mainAxisAlignment:
                                      MainAxisAlignment.spaceEvenly,
                                  children: [
                                    const Expanded(
                                        flex: 1,
                                        child: Center(
                                            child: Text('Address',
                                                style: TextStyle(
                                                    color: Colors.black,
                                                    fontSize: 16)))),
                                    Expanded(
                                      flex: 2,
                                      child: Padding(
                                        padding: const EdgeInsets.all(8.0),
                                        child: Focus(
                                          child: AudYoFloActiveTextField(
                                              showTextOnBuild: ipAddrSpecified,
                                              onEditingComplete: (String text) {
                                                ipAddrSpecified = text;
                                              },
                                              reportOnFocusLost: true,
                                              haveCommandHistory: false),
                                        ),
                                      ),
                                    ),
                                    const Expanded(
                                        flex: 1,
                                        child: Center(
                                            child: Text('Port',
                                                style: TextStyle(
                                                    color: Colors.black,
                                                    fontSize: 16)))),
                                    Expanded(
                                        flex: 1,
                                        child: Padding(
                                            padding: const EdgeInsets.all(8.0),
                                            child: Focus(
                                              child: AudYoFloActiveTextField(
                                                  showTextOnBuild:
                                                      ipPortSpecified
                                                          .toString(),
                                                  onEditingComplete:
                                                      (String text) {
                                                    ipPortSpecified =
                                                        int.parse(text);
                                                    setState(() {});
                                                  },
                                                  reportOnFocusLost: true,
                                                  haveCommandHistory: false),
                                            ))),
                                  ],
                                ),
                              ),
                            ),
                          ),
                          Expanded(
                              flex: 1,
                              child: Container(
                                color: Colors.white,
                                child: Row(
                                    mainAxisAlignment: MainAxisAlignment.center,
                                    children: [
                                      ElevatedButton(
                                          style: ElevatedButton.styleFrom(
                                              backgroundColor: Colors.blueGrey,
                                              padding:
                                                  const EdgeInsets.symmetric(
                                                      horizontal: 8,
                                                      vertical: 8),
                                              textStyle: const TextStyle(
                                                  fontSize: 16,
                                                  fontWeight:
                                                      FontWeight.normal)),

                                          // Within the SecondScreen widget
                                          onPressed: () async {
                                            await tryStartConnect();
                                          },
                                          child: Text('Connect')),
                                    ]),
                              )),
                        ]),
                  ),
                ))
            // const Text('Go back!'),

            ),
      ),
    );
  }

  Future<void> tryStartConnect() async {
    if ((uiModel != null) && (uiModel!.platformSpec != null)) {
      runLoopAutoConnect = false;
      // Attach the connect parameters to the command line
      var cmdArgsLoc = uiModel!.platformSpec!.cmdArgs;

      // Make fixed length list to growable list
      cmdArgsLoc = cmdArgsLoc.toList();
      cmdArgsLoc.add('--connect-addr');
      cmdArgsLoc.add(ipAddrSpecified);
      cmdArgsLoc.add('--connect-port');
      cmdArgsLoc.add(ipPortSpecified.toString());
      uiModel!.platformSpec!.cmdArgs = cmdArgsLoc;

      // Run the platform specific initialization
      beCache!.httpConnect.isConnected = await uiModel!.platformSpec!
          .initializeSubSystem(beCache, uiModel, dbgModel);
    } else {
      print('Error: platform specific references not available!');
      assert(false);
    }
    // Navigate back to the first screen by popping the current route
    // off the stack.
    setState(() {
      latestError = uiModel!.platformSpec!.lastError;
      runLoopAutoConnect = false;
      if (beCache!.httpConnect.isConnected) {
        Navigator.pop(context);
      } else {
        runLoopAutoConnect = autoConnect;
      }
    });
  }
}

class AudYoFloConnectAppBar extends StatefulWidget implements PreferredSizeWidget {
  final String titletxt;
  final String logoPath;
  final double widthScale;
  final double height;
  final double overallScale;
  final String routeNavigatorSettingsTag;

  @override
  final Size preferredSize;

  const AudYoFloConnectAppBar(
      BuildContext context,
      this.titletxt,
      this.logoPath,
      this.widthScale,
      this.overallScale,
      this.height,
      theBeCache,
      {Key? key,
      this.routeNavigatorSettingsTag = ''})
      : preferredSize = const Size.fromHeight(50.0),
        super(key: key);

  @override
  _MyConnectAppBarState createState() => _MyConnectAppBarState();
}

class _MyConnectAppBarState extends State<AudYoFloConnectAppBar> {
  Color col = Colors.black;
  @override
  Widget build(BuildContext context) {
    return AppBar(
      automaticallyImplyLeading: false,
      backgroundColor: Colors.white,
      title: Row(mainAxisAlignment: MainAxisAlignment.spaceAround, children: [
        Flexible(
          flex: 1,
          child:
              Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
            GestureDetector(
                onTap: () {
                  debugPrint('Help triggered');
                },
                child: const Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Icon(Icons.help, color: Colors.black)))
          ]),
        ),
        Flexible(
          flex: 4,
          child:
              Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
            Text(
              widget.titletxt,
              style: const TextStyle(color: Colors.black),
            ),
            Image.asset(
              widget.logoPath,
              filterQuality: FilterQuality.medium,
              fit: BoxFit.fill,
              width: widget.widthScale * widget.height * widget.overallScale,
              height: widget.height * widget.overallScale,
            ),
          ]),
        )
      ]),
      toolbarHeight: widget.height,
    );
  }
}
