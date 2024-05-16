import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'dart:async';
import '../ayf_pack_local.dart';

/*
 *
 * Hint on how to create a "really customized appbar":
 * https://dev.to/pazozzoo/creating-a-custom-appbar-in-flutter-572k
 */
class AudYoFloAppBar extends StatefulWidget with PreferredSizeWidget {
  final String titletxt;
  final String logoPath;
  final double widthScale;
  final double height;
  final double overallScale;
  final String routeNavigatorSettingsTag;

  @override
  final Size preferredSize;

  const AudYoFloAppBar(BuildContext context, this.titletxt, this.logoPath,
      this.widthScale, this.overallScale, this.height, theBeCache,
      {Key? key, this.routeNavigatorSettingsTag = ''})
      : preferredSize = const Size.fromHeight(50.0),
        super(key: key);

  @override
  _MyAppBarState createState() => _MyAppBarState();
}

class _MyAppBarState extends State<AudYoFloAppBar> {
  Color col = Colors.black;
  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);

    //theBEBridge.triggeredClose();
    //return true;

    return AppBar(
      backgroundColor: Colors.white,
      title: Row(mainAxisAlignment: MainAxisAlignment.spaceAround, children: [
        Flexible(
          flex: 1,
          /*child: DecoratedBox(
                  decoration: BoxDecoration(
                    // color: Colors.blueGrey,
                    border: Border.all(),
                    borderRadius: BorderRadius.circular(5),
                    /*
                      boxShadow: [
                        BoxShadow(
                            color: Colors.white,
                            offset: Offset(-2, -2),
                            blurRadius: 3),
                        BoxShadow(
                            color: Colors.black,
                            offset: Offset(10, 10),
                            blurRadius: 8)
                      ]*/
                  ),*/
          child:
              Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: [
            GestureDetector(
                onTap: () {
                  theBeCache.triggerSaveConfig();

                  setState(() {
                    col = Colors.red;
                  });

                  Timer wait1Sec = Timer(const Duration(seconds: 1), () {
                    setState(() {
                      col = Colors.black;
                    });
                  });
                },
                child: Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: Icon(Icons.file_copy_outlined, color: col))),
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
            GestureDetector(
                onTap: () {
                  if (widget.routeNavigatorSettingsTag.isNotEmpty) {
                    Navigator.pushNamed(
                        context, widget.routeNavigatorSettingsTag);
                  } else {
                    showDialog(
                        context: context,
                        builder: (context) {
                          return AudYoFloSettingsDialog();
                        });
                  }
                  debugPrint('Settings');
                },
                child: const Icon(Icons.menu, color: Colors.black))
          ]),
        )
      ]),
      toolbarHeight: widget.height,
    );
  }
}

/*
class AudYoFloAppBar extends AppBar {
  final String titletxt;
  final String logoPath;
  final double widthScale;
  final double height;
  final double overallScale;
  final Color col;

  AudYoFloAppBar(BuildContext context, this.titletxt, this.logoPath,
      this.widthScale, this.overallScale, this.height, theBeCache, this.col,
      {String routeNavigatorSettingsTag = ''})
      : super(
          backgroundColor: Colors.white,
          title:
              Row(mainAxisAlignment: MainAxisAlignment.spaceAround, children: [
            Flexible(
              flex: 1,
              /*child: DecoratedBox(
                  decoration: BoxDecoration(
                    // color: Colors.blueGrey,
                    border: Border.all(),
                    borderRadius: BorderRadius.circular(5),
                    /*
                      boxShadow: [
                        BoxShadow(
                            color: Colors.white,
                            offset: Offset(-2, -2),
                            blurRadius: 3),
                        BoxShadow(
                            color: Colors.black,
                            offset: Offset(10, 10),
                            blurRadius: 8)
                      ]*/
                  ),*/
              child: Container(
                child: Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: [
                      GestureDetector(
                          onTap: () {
                            theBeCache.triggerSaveConfig();
                          },
                          child: Padding(
                              padding: const EdgeInsets.all(8.0),
                              child:
                                  Icon(Icons.file_copy_outlined, color: col))),
                      GestureDetector(
                          onTap: () {
                            print('File Save');
                          },
                          child: Padding(
                              padding: const EdgeInsets.all(8.0),
                              child: Icon(Icons.help, color: Colors.black)))
                    ]),
              ),
            ),
            Flexible(
              flex: 4,
              child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                    Text(
                      titletxt,
                      style: TextStyle(color: Colors.black),
                    ),
                    Container(
                        // color: Colors.grey,
                        //child: SvgPicture.asset(logoPath),
                        child: Image.asset(
                      logoPath,
                      filterQuality: FilterQuality.medium,
                      fit: BoxFit.fill,
                      width: widthScale * height * overallScale,
                      height: height * overallScale,
                    )),
                    GestureDetector(
                        onTap: () {
                          if (routeNavigatorSettingsTag.isNotEmpty) {
                            Navigator.pushNamed(
                                context, routeNavigatorSettingsTag);
                          } else {
                            showDialog(
                                context: context,
                                builder: (context) {
                                  return AudYoFloSettingsDialog();
                                });
                          }
                          print('Settings');
                        },
                        child: Icon(Icons.menu, color: Colors.black))
                  ]),
            )
          ]),
          toolbarHeight: height,
        );
}
*/
