import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

class AudYoFloDebugWidget extends StatefulWidget {
  const AudYoFloDebugWidget({Key? key}) : super(key: key);

  @override
  _AudYoFloDebugWidgetState createState() => _AudYoFloDebugWidgetState();
}

class _AudYoFloDebugWidgetState extends State<AudYoFloDebugWidget> {
  List<String> lstText = ['a', 'b', 'c'];

  // Important stuff here: we need two controllers, one for the multiline
  // textfield and one for the scroll function
  final TextEditingController _controllerOut = TextEditingController();
  final ScrollController _scrollController = ScrollController();

  @override
  Widget build(BuildContext context) {
    AudYoFloBackendCache theBeCache =
        Provider.of<AudYoFloBackendCache>(context, listen: false);
    AudYoFloDebugModel theDbgModel =
        Provider.of<AudYoFloDebugModel>(context, listen: false);

    return LayoutBuilder(builder: (_, constraints) {
      double h1 = 40;
      double h2 = constraints.maxHeight - 40;
      AudYoFloDebugModel theDbgModel =
          Provider.of<AudYoFloDebugModel>(context, listen: false);

      return Column(children: [
        Container(
            height: h1,
            width: constraints.maxWidth,
            child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  Text('Debug'),
                  Checkbox(
                      checkColor: Colors.white,
                      activeColor: Colors.black,
                      //fillColor: MaterialStateProperty.resolveWith(getColor),
                      value: theDbgModel.dbgOut,
                      onChanged: (bool? value) {
                        setState(() {
                          theDbgModel.dbgOut = value!;
                        });
                      }),
                  Text('Functionname'),
                  Checkbox(
                      checkColor: Colors.white,
                      activeColor: Colors.black,
                      value: theDbgModel.funcOut,
                      onChanged: (bool? value) {
                        setState(() {
                          theDbgModel.funcOut = value!;
                        });
                      }),
                  ElevatedButton(
                      style: ButtonStyle(backgroundColor:
                          MaterialStateProperty.resolveWith<Color>(
                        (Set<MaterialState> states) {
                          if (states.contains(MaterialState.pressed))
                            return Colors.grey;
                          return Colors.black; // Use the component's default.
                        },
                      )),
                      child: Text('Clear'),
                      // color: Colors.black,q
                      //child: Icon(Icons.clear),
                      onPressed: () {
                        theDbgModel.clearAll();
                      }),
                  Text('Command'),
                  Container(
                      width: constraints.maxHeight / 2,
                      child: FocusScope(child: AudYoFloActiveTextField(
                          onEditingComplete: (String text) async {
                        // Return json expression but convert for display!
                        String transferResult =
                            await theBeCache.transferTextCommand(text, false);
                        theDbgModel.addLineOutConvert(transferResult);
                      }))),
                ])),
        Container(
          height: h2,
          child: Consumer<AudYoFloDebugModel>(builder: (_, value, __) {
            String txt = '';
            for (final txtLine in value.out_debug) {
              txt += txtLine + '\n';
            }
            _controllerOut.text = txt;
            return DecoratedBox(
                decoration: BoxDecoration(
                    color: Colors.grey[350],
                    border: Border.all(
                      color: Colors.grey,
                      width: 4,
                    ),
                    borderRadius: BorderRadius.circular(15)),
                child: Padding(
                  padding: EdgeInsets.all(20),
                  child: SingleChildScrollView(
                      scrollDirection: Axis.vertical,
                      reverse: true,
                      controller: _scrollController,
                      child: TextField(
                        style: TextStyle(fontSize: 12),
                        keyboardType: TextInputType.text,
                        maxLines: null,
                        readOnly: true,
                        controller: _controllerOut,
                      )),
                ));
          }),
        )
      ]);
    });
  }
}
