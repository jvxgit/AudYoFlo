import 'package:flutter/material.dart';

import 'package:collection/collection.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

class AudYoFloPropLstSelEntry {
  String txt = 'not-set';
  bool isSel = false;
  int posi = -1;
}

// ===========================================================================
// Combobox widget to show a single selection list property. The property
// can be a backend property OR a frontend property.
// The set-operation may report change of property OR change of component
// in backend cache.
// ===========================================================================
class AudYoFloPropListComboBoxWidget extends StatelessWidget {
  final AudYoFloPropertySelectionListBackend? selLstBe;
  final AudYoFloPropertyMultiStringBackend? strLstBe;
  final AudYoFloPropertySelectionListLocal? selLstLo;
  final AyfPropertyReportLevel reportSet;
  final bool invalidatePropOnSet;
  final double height;
  final int numCharsMax = 25;
  final String prefix;
  final int offset;

  /* This combobox can be used to control backend and local 
    properties. There should always be either selLstBe or 
    selLstLo. It constructs the combobox with all options and
    then waits for a selection.
    In  reportSet the report type is specified.*/
  AudYoFloPropListComboBoxWidget(
      {this.selLstBe,
      this.selLstLo,
      this.strLstBe,
      this.reportSet =
          AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT,
      this.invalidatePropOnSet = true,
      this.height = 40,
      this.prefix = ' ',
      this.offset = 0});

  @override
  Widget build(BuildContext context) {
    // This function is called if the grid view becomes visible -
    // maybe it is kind of a resize procedure.
    int posi = 0;

    List<AudYoFloPropLstSelEntry> entriesCombo = [];
    AudYoFloPropLstSelEntry? sel;

    AudYoFloBackendCache? theBeCache;
    AudYoFloUiModel? theUiModel;
    // Get model contexts
    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    theUiModel = Provider.of<AudYoFloUiModel>(context, listen: false);

    if (selLstBe != null) {
      // Backend property
      for (var sgl in selLstBe!.parpropms.entries) {
        AudYoFloPropLstSelEntry newEntry = AudYoFloPropLstSelEntry();
        newEntry.txt = sgl;
        newEntry.posi = posi;
        newEntry.isSel = selLstBe!.selection.bitTest(posi, offset: offset);
        if (newEntry.isSel) {
          sel ??= newEntry;
        }
        entriesCombo.add(newEntry);
        posi++;
      }
    } else if (selLstLo != null) {
      // Frontend property
      for (var sgl in selLstLo!.entries) {
        AudYoFloPropLstSelEntry newEntry = AudYoFloPropLstSelEntry();
        newEntry.txt = sgl;
        newEntry.posi = posi;
        newEntry.isSel = selLstLo!.selection.bitTest(posi, offset: offset);
        if (newEntry.isSel) {
          sel ??= newEntry;
        }
        entriesCombo.add(newEntry);
        posi++;
      }
    } else if (strLstBe != null) {
      // Frontend property
      for (var sgl in strLstBe!.entries) {
        AudYoFloPropLstSelEntry newEntry = AudYoFloPropLstSelEntry();
        newEntry.txt = sgl;
        newEntry.posi = posi;
        newEntry.isSel = false;
        sel ??= newEntry;
        entriesCombo.add(newEntry);
        posi++;
      }
    } else {
      // No property
      // assert(false);
    }

    return SizedBox(
      height: height,
      child: Container(
          height: height,
          decoration: BoxDecoration(
            border: Border.all(
              color: Colors.black,
            ),
            borderRadius: const BorderRadius.all(Radius.circular(10)),
            color: Colors.white,
          ),

          //color: Colors.white,
          child: Center(
            child: DropdownButtonFormField(
                items: entriesCombo
                    .map<DropdownMenuItem<AudYoFloPropLstSelEntry>>(
                        (AudYoFloPropLstSelEntry value) {
                  String txt = value.txt;
                  txt = limitString(txt, numCharsMax);
                  txt = prefix + txt;
                  Widget cW = Text(txt);
                  if (value.isSel) {
                    cW = Text(txt,
                        style: const TextStyle(fontWeight: FontWeight.bold));
                  }
                  return DropdownMenuItem<AudYoFloPropLstSelEntry>(
                    value: value,
                    child: cW,
                  );
                }).toList(),
                value: sel,
                isExpanded: true,
                isDense: true,
                /*
              HK: This does not work: the OutlineInputBorder hides the text if
              the selection is not clicked.

              decoration: InputDecoration(
                enabledBorder: OutlineInputBorder(
                    borderRadius: BorderRadius.circular(25.0),
                    borderSide: BorderSide(color: Colors.pinkAccent)),
                    
                filled: true,
                fillColor: Colors.white,
                //border: OutlineInputBorder(
                //    gapPadding: 12, borderRadius: BorderRadius.circular(12)),
              ),*/
                onChanged: (AudYoFloPropLstSelEntry? newValue) {
                  // This triggers setProperty and reports change
                  if (theBeCache != null) {
                    if (newValue != null) {
                      AudYoFloPropLstSelEntry? entry = entriesCombo
                          .firstWhereOrNull((element) => element == newValue);
                      if (entry != null) {
                        if (selLstBe != null) {
                          // Run and apply user input
                          applyChangeSelectionList(
                              selLstBe!.selection,
                              newValue.posi,
                              selLstBe!.parpropms.decoderHintType,
                              offset: offset);

                          JvxComponentIdentification cpId =
                              selLstBe!.parpropms.assCpIdent;
                          List<String> propContents = [
                            selLstBe!.parpropms.descriptor
                          ];

                          // We need to propagate the updated property to the
                          // higher level widget to refresh the property in cache
                          theBeCache!.triggerSetProperties(cpId, propContents,
                              report: reportSet,
                              invalidateProperty: invalidatePropOnSet);
                        } else if (selLstLo != null) {
                          applyChangeSelectionList(selLstLo!.selection,
                              newValue.posi, selLstLo!.decTp);
                          theUiModel!.reportSetProperty(selLstLo!);
                        }
                      }
                    }
                  }
                }),
          )),
    );
  }
}
