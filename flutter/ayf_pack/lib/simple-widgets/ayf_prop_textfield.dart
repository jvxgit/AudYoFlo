import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../ayf_pack_local.dart';

// ===========================================================================
// Simple text field to edit a property. The property may be a backend property
// OR a frontend property
// ===========================================================================
class AudYoFloSimpleTextField extends StatefulWidget {
  final AudYoFloPropertyContainer? propBe;
  final AudYoFloPropertyContentLocal? propLo;
  final AyfPropertyReportLevel reportSet;
  final bool invalidatePropOnSet;
  final double height;
  final int numCharsMax = 32;
  final String hintText;
  AudYoFloSimpleTextField(
      {this.propBe,
      this.propLo,
      this.reportSet =
          AyfPropertyReportLevel.AYF_BACKEND_REPORT_COMPONENT_PROPERTY_COLLECT,
      this.invalidatePropOnSet = true,
      this.height = 45,
      this.hintText = ''});
  @override
  State<StatefulWidget> createState() {
    return _AudYoFloSimpleTextFieldStates();
  }
}

// ===========================================================================

class _AudYoFloSimpleTextFieldStates extends State<AudYoFloSimpleTextField> {
  final TextEditingController _controllerIn = TextEditingController();
  AudYoFloBackendCache? theBeCache;
  AudYoFloUiModel? theUiModel;
  @override
  Widget build(BuildContext context) {
    // Get both models for backend and frontend properties
    theBeCache = Provider.of<AudYoFloBackendCache>(context, listen: false);
    theUiModel = Provider.of<AudYoFloUiModel>(context, listen: false);

    // We need to set the value, not just the text!
    // https://stackoverflow.com/questions/51127241/how-do-you-change-the-value-inside-of-a-textfield-flutter
    String textOnCreate = 'not-set';
    if (widget.propBe != null) {
      // Backend property
      textOnCreate = widget.propBe.toString();
    } else if (widget.propLo != null) {
      // local frontend property
      textOnCreate = widget.propLo.toString();
    } else {
      // No property
    }

    textOnCreate = limitString(textOnCreate, widget.numCharsMax);

    _controllerIn.value = TextEditingValue(
        text: textOnCreate,
        selection: TextSelection.fromPosition(
          TextPosition(offset: textOnCreate.length),
        ));
    InputDecoration? inDec;
    if (widget.hintText.isNotEmpty) {
      inDec = InputDecoration(
          hintText: widget.hintText,
          border: OutlineInputBorder(),
          alignLabelWithHint: true);
    }

    return SizedBox(
        height: widget.height,
        child: TextFormField(
          controller: _controllerIn,
          keyboardType: TextInputType.text,
          maxLines: 1,
          minLines: 1,
          decoration: InputDecoration(
            // Here I have tested a lot. If the text is too wide, it disappears!
            // We can avoid this by re-defining the default EdgeInsets. We need the left border inset however, to look nice.
            // isDense: true,
            contentPadding: const EdgeInsets.fromLTRB(5, 0, 0, 0),
            filled: true,
            fillColor: Colors.white,
            border: OutlineInputBorder(
                gapPadding: 12, borderRadius: BorderRadius.circular(12)),
          ),
          onEditingComplete: () {
            String text = _controllerIn.text;
            bool triggerSet = false;
            if (widget.propBe != null) {
              if (theBeCache != null) {
                JvxComponentIdentification cpId = widget.propBe!.assCpIdent;

                if (widget.propBe != null) {
                  if (widget.propBe is AudYoFloPropertySingleStringBackend) {
                    AudYoFloPropertySingleStringBackend propStr =
                        widget.propBe as AudYoFloPropertySingleStringBackend;
                    propStr.value = _controllerIn.text;
                    if ((propStr.decoderHintType ==
                            jvxPropertyDecoderHintTypeEnum
                                .JVX_PROPERTY_DECODER_FILENAME_OPEN) ||
                        (propStr.decoderHintType ==
                            jvxPropertyDecoderHintTypeEnum
                                .JVX_PROPERTY_DECODER_FILENAME_SAVE)) {
                      propStr.value = propertyStringBackSlashes(propStr.value);
                    }
                    triggerSet = true;
                  } else if (widget.propBe
                      is AudYoFloPropertyMultiContentBackend) {
                    AudYoFloPropertyMultiContentBackend propM =
                        widget.propBe as AudYoFloPropertyMultiContentBackend;
                    bool suc =
                        propertyValueMultiFromString(propM, _controllerIn.text);
                    if (suc == true) {
                      triggerSet = true;
                    }
                  } else {
                    print(
                        'Invalid property type for descriptor ${widget.propBe!.descriptor} in AudYoFloSimpleTextField.');
                  }
                } else {
                  print('Invalid null property in AudYoFloSimpleTextField.');
                }
                if (triggerSet) {
                  List<String> lst = [widget.propBe!.descriptor];

                  // Trigger set_property and notify all widgets to listen for all
                  // properties related to this component
                  theBeCache!.triggerSetProperties(cpId, lst,
                      invalidateProperty: widget.invalidatePropOnSet,
                      report: widget.reportSet);
                }
              }
            } else if (widget.propLo != null) {
              AudYoFloPropertyContentLocal prop = widget.propLo!;
              if (prop is AudYoFloPropertyStringLocal) {
                // Copy text to property
                prop.value = text;
              } else if (prop is AudYoFloPropertyValue) {
                // Copy text to property
                prop.value = double.parse(text);
              }
              theUiModel!.reportSetProperty(prop);
            }
          },
        ));
  }
}
