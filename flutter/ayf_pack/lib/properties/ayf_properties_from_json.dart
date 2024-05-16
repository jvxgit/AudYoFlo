import 'dart:typed_data';
import 'dart:convert';
import 'package:fixnum/fixnum.dart' as fn;
import 'package:ayf_pack/ayf_pack.dart';

typedef int AudYoFloPropertyContentFieldConverter(
    AudYoFloPropertyContentBackend prop, Map newValues);

class AudYoFloPropertyContentFromJson {
  // ===========================================================
  // Create a property descriptor (and content) from json map
  // This function is the processor of a call to get_decsription
  // ===========================================================
  static AudYoFloPropertyContentBackend createPropertyFromJsonMap(
      JvxComponentIdentification cpId,
      AudYoFloBackendAdapterIf ref,
      String descriptor,
      Map jsonMap,
      AudYoFloCompileFlags flags, int numDigits) {
    // ===================================================
    // Start to read format and size to allocate variable
    bool error = false;
    AudYoFloPropertyContentBackend? retVal;
    jvxDataFormatEnum format = jvxDataFormatEnum.JVX_DATAFORMAT_NONE;
    String? typeStr = extractStringFromJson(jsonMap, "format");
    if (typeStr != null) {
      format = jvxDataFormatEEnum.fromInt(ref.translateEnumString(
          typeStr, jvxDataFormatEEnum.formatName,
          flags));
    } else {
      error = true;
    }

    int num = getIntEntryValueMap(jsonMap, "number_elements");
    if (num < 0) {
      error = true;
    }

    // If there is an error, return an "error" descriptor
    if (error) {
      AudYoFloPropertyContentBackend retVal =
          AudYoFloPropertyContentBackend(cpId, descriptor: descriptor);
      retVal.last_error = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      retVal.cache_status.bitClear(jvxPropertyCacheStatusFlagShifts
          .JVX_PROPERTY_DESCRIPTOR_INVALID.index);
      retVal.cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);

      return retVal;
    }

    // Allocate the variable, this resets error code and status to 0
    retVal = allocatePropertyContent(cpId, format, num, flags, numDigits);

    int errCode = updatePropertyDescriptionFromJson(
        cpId, ref, retVal, descriptor, jsonMap, flags, false, numDigits);

    return retVal;
  }

  static int updatePropertyDescriptionFromJson(
      JvxComponentIdentification cpId,
      AudYoFloBackendAdapterIf ref,
      AudYoFloPropertyContentBackend prop,
      String descriptor,
      Map jsonMap,
      AudYoFloCompileFlags flags,
      bool checkFormatNum,
      int numDigits) {
    String? typeStr;
    // Now fill in all properties

    // Reset the flag from invalid to anything else
    prop.cache_status.bitClear(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_INVALID.index);
    prop.last_error = jvxErrorType.JVX_NO_ERROR;

    if (checkFormatNum) {
      jvxDataFormatEnum format = jvxDataFormatEnum.JVX_DATAFORMAT_NONE;
      String? typeStr = extractStringFromJson(jsonMap, "format");
      if (typeStr != null) {
        format = jvxDataFormatEEnum.fromInt(
            ref.translateEnumString(typeStr, jvxDataFormatEEnum.formatName, flags));
      } else {
        prop.last_error = jvxErrorType.JVX_ERROR_PARSE_ERROR;
      }

      int num = getIntEntryValueMap(jsonMap, "number_elements");
      if (num < 0) {
        return jvxErrorType.JVX_ERROR_PARSE_ERROR;
      }

      if ((format != prop.jvxFormat) || (num != prop.num)) {
        prop.dispose();

        // Create new property
        prop = allocatePropertyContent(cpId, format, num, flags, numDigits);
      }
    }

    // Simply use the int value
    prop.globalIdx = getIntEntryValueMap(jsonMap, "global_idx");
    if (prop.globalIdx < 0) {
      prop.last_error = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      prop.cache_status.bitZSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);
    }

    // Convert to string and map to enum
    typeStr = extractStringFromJson(jsonMap, "category");
    if (typeStr != null) {
      prop.category = jvxPropertyCategoryTypeEEnum.fromInt(
          ref.translateEnumString(
              typeStr, jvxPropertyCategoryTypeEEnum.formatName, flags));
    } else {
      prop.last_error = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      prop.cache_status.bitZSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);
    }

    // Convert to string and map to enum
    typeStr = extractStringFromJson(jsonMap, "decoder_type");
    if (typeStr != null) {
      prop.decoderHintType = jvxPropertyDecoderHintTypeEEnum.fromInt(
          ref.translateEnumString(
              typeStr, jvxPropertyDecoderHintTypeEEnum.formatName, flags));
    } else {
      prop.last_error = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      prop.cache_status.bitZSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);
    }

    typeStr = extractStringFromJson(jsonMap, "access_type");
    if (typeStr != null) {
      prop.accessType = jvxPropertyAccessTypeEEnum.fromInt(ref
          .translateEnumString(typeStr, jvxPropertyAccessTypeEEnum.formatName, flags));
    } else {
      prop.last_error = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
      prop.cache_status.bitZSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);
    }

    if (prop.cache_status.bitTest(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index)) {
      prop.dispose();
      return prop.last_error;
    } else {
      prop.valid_parts.fld |=
          (1 << jvxPropertyDescriptorEnum.JVX_PROPERTY_DESCRIPTOR_CORE) |
              (1 << jvxPropertyDescriptorEnum.JVX_PROPERTY_DESCRIPTOR_MIN);
    }

    // ======================================================================
    // ======================================================================

    // We need to parse into states
    bool controlOk = true;

    // Preset to be valid in all states if no additional info is available
    prop.allowedStateMask.fld = fn.Int32(jvxState.JVX_STATE_SELECTED |
        jvxState.JVX_STATE_ACTIVE |
        jvxState.JVX_STATE_PREPARED |
        jvxState.JVX_STATE_PROCESSING);
    typeStr = extractStringFromJson(jsonMap, "allowed_smask");
    if (typeStr != null) {
      var lst = extractTextToStrList(typeStr);
      prop.allowedStateMask.reset();
      for (var elm in lst) {
        prop.allowedStateMask.fld |= jvxStateEInt.fromStringSingle(elm);
      }
    } else {
      controlOk = false;
    }

    // Set the default to all threads if not specified
    prop.allowedThreadingMask.fld = fn.Int32(-1);
    typeStr = extractStringFromJson(jsonMap, "thread_mask");
    if (typeStr != null) {
      prop.allowedThreadingMask.fromHexString(typeStr);
    } else {
      controlOk = false;
    }

    // Property valid
    prop.isValid = false;
    typeStr = extractStringFromJson(jsonMap, "valid");
    if (typeStr != null) {
      prop.isValid = AudYoFloPropertyContentFromString.boolFromString(typeStr);
    } else {
      controlOk = false;
    }

    prop.installable = false;
    typeStr = extractStringFromJson(jsonMap, "instable");
    if (typeStr != null) {
      prop.installable =
          AudYoFloPropertyContentFromString.boolFromString(typeStr);
    } else {
      controlOk = false;
    }

    prop.reqInvalidateOnStateSwitch =
        jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_INACTIVE;
    prop.reqInvalidateOnTest =
        jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_INACTIVE;

    typeStr = extractStringFromJson(jsonMap, "inval_sswitch");
    if (typeStr != null) {
      if (typeStr == '*') {
        prop.reqInvalidateOnStateSwitch =
            jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_CONTENT;
      } else {
        prop.reqInvalidateOnStateSwitch =
            jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_INACTIVE;
        // AudYoFloPropertyContentFromString.boolFromString(typeStr);
      }
    } else {
      controlOk = false;
    }

    typeStr = extractStringFromJson(jsonMap, "inval_test");
    if (typeStr != null) {
      if (typeStr == '*') {
        prop.reqInvalidateOnTest =
            jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_CONTENT;
      } else {
        prop.reqInvalidateOnTest =
            jvxPropertyInvalidateTypeEnum.JVX_PROPERTY_INVALIDATE_INACTIVE;
        // AudYoFloPropertyContentFromString.boolFromString(typeStr);
      }
    } else {
      controlOk = false;
    }

    if (controlOk) {
      prop.valid_parts.fld |=
          (1 << jvxPropertyDescriptorEnum.JVX_PROPERTY_DESCRIPTOR_CONTROL);
    }

    // ======================================================================
    // ======================================================================

    bool fullOk = true;
    // All strings
    typeStr = extractStringFromJson(jsonMap, "descriptor");
    if (typeStr != null) {
      prop.descriptor = typeStr;
    } else {
      fullOk = false;
    }

    typeStr = extractStringFromJson(jsonMap, "description");
    if (typeStr != null) {
      prop.description = typeStr;
    } else {
      fullOk = false;
    }

    typeStr = extractStringFromJson(jsonMap, "name");
    if (typeStr != null) {
      prop.name = typeStr;
    } else {
      fullOk = false;
    }
    if (fullOk) {
      prop.valid_parts.fld |=
          (1 << jvxPropertyDescriptorEnum.JVX_PROPERTY_DESCRIPTOR_FULL);
    }
    // ======================================================================
    // ======================================================================

    bool fullpOk = true;
    typeStr = extractStringFromJson(jsonMap, "acc_flags");
    if (typeStr != null) {
      int err = prop.accessFlags.fromHexString(typeStr);
      if (err != jvxErrorType.JVX_NO_ERROR) {
        fullpOk = false;
      }
    } else {
      fullpOk = false;
    }

    typeStr = extractStringFromJson(jsonMap, "cfg_flags");
    if (typeStr != null) {
      resultParseInt32 ret32 = resultParseInt32();
      int err = prop.configFlags.fromHexString(typeStr);
      if (err != jvxErrorType.JVX_NO_ERROR) {
        fullpOk = false;
      }
    } else {
      fullpOk = false;
    }
    if (fullpOk) {
      prop.valid_parts.fld |=
          (1 << jvxPropertyDescriptorEnum.JVX_PROPERTY_DESCRIPTOR_FULL_PLUS);
    }
    // Origin
    typeStr = extractStringFromJson(jsonMap, "origin");
    if (typeStr != null) {
      prop.origin = typeStr;
    }

    if (prop.descriptor != descriptor) {
      prop.last_error = jvxErrorType.JVX_ERROR_INVALID_SETTING;
      prop.dispose();
    }

    if (prop.last_error == jvxErrorType.JVX_NO_ERROR) {
      prop.cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_VALID.index);
    } else {
      prop.cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_ERROR.index);
    }
    return prop.last_error;
  }

  static String removeBraces(
      String inStr, String braceLeft, String braceRight) {
    String retVal = inStr;
    int idxStart = inStr.indexOf(braceLeft);
    String rev = inStr.split('').reversed.join();
    int idxStop = rev.indexOf(braceRight);
    if ((idxStart >= 0) && (idxStop >= 0)) {
      retVal = inStr.substring(idxStart + 1, inStr.length - 1 - idxStop);
    }
    return retVal;
  }

  static String removeBegin(String inStr, String tokenStart) {
    String retVal = inStr;
    if (tokenStart.length <= inStr.length) {
      String cp = inStr.substring(0, tokenStart.length);
      if (cp == tokenStart) {
        retVal = inStr.substring(tokenStart.length);
      }
    }
    return retVal;
  }

  // ===========================================================
  // Function to update the content of a property. It expects a
  // valid container for the property and fills in whatever comes in.
  // ===========================================================
  static int updatePropertyContentFromJsonMap(
      AudYoFloPropertyContentBackend prop, Map newValues, AudYoFloBackendTranslator trans) {
    int errCode = jvxErrorType.JVX_NO_ERROR;

    if (prop.cache_status.bitTest(
        jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_DESCRIPTOR_VALID.index)) {
      // Set the status from invalid to something else
      prop.cache_status.bitClear(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_INVALID.index);

      switch (prop.jvxFormat) {
        // STRINGLIST
        case jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST:
          if (prop is AudYoFloPropertyMultiStringBackend) {
            prop.entries.clear();
            List? lst_fld = getListValueMap(newValues, 'property_fld');
            if (lst_fld != null) {
              prop.entries = lst_fld.cast<String>();
              errCode = jvxErrorType.JVX_NO_ERROR;
            } else {
              errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
            }
          } else {
            errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
          }
          break;

        case jvxDataFormatEnum.JVX_DATAFORMAT_STRING:
          if (prop is AudYoFloPropertySingleStringBackend) {
            String? txt = getStringEntryValueMap(newValues, 'property');
            if (txt != null) {
              prop.value = txt;
            } else {
              errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
            }
          } else {
            errCode = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
          }
          break;

        case jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST:
          if (prop is AudYoFloPropertySelectionListBackend) {
            AudYoFloPropertySelectionListBackend propSel =
                prop as AudYoFloPropertySelectionListBackend;
            propSel.parpropms.entries.clear();
            List? lst_fld = getListValueMap(newValues, 'property_fld');
            if (lst_fld != null) {
              // Update string list - if it was transferred
              propSel.parpropms.entries = lst_fld.cast<String>();
            }
            String? txt = getStringEntryValueMap(newValues, 'property');
            if (txt != null) {
              List<String> lstFld = txt.split('::');
              if (lstFld.length == 4) {
                String selected = lstFld[0];
                String selectable = lstFld[1];
                String exclusive = lstFld[2];
                String numElms =
                    lstFld[3]; // Number elements entries - could also be 0!

                selected = removeBraces(selected, '[', ']');
                var lstFlds = selected.split(',');

                updatePropertyContentSelListFromJsonMap(
                    propSel, lstFlds, selectable, exclusive, numElms);
              } else {
                assert(false);
              }
            }
          }
          break;

        case jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE:
          if (prop is AudYoFloPropertyValueInRangeBackend) {
            AudYoFloPropertyValueInRangeBackend propVal =
                prop as AudYoFloPropertyValueInRangeBackend;

            String? txt = getStringEntryValueMap(newValues, 'property');
            if (txt != null) {
              List<String> lstFld = txt.split('::');
              if (lstFld.length == 3) {
                String value = lstFld[0];
                String minV = lstFld[1];
                String maxV = lstFld[2];

                value = removeBraces(value, '[', ']');
                var lstFlds = value.split(',');

                errCode = updatePropertyContentValueInRageFromJsonMap(
                    propVal, lstFlds, minV, maxV);
              } else {
                assert(false);
              }
            }
          }
          break;
        //64 BIT FIELDS
        case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
        case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
          errCode = updatePropertyContentFromJsonMap_fields(prop, newValues, trans);
          break;

        default:
          errCode = jvxErrorType.JVX_ERROR_WRONG_STATE;
          break;
      }
    } else {
      // This if the descriptor is invalid
      errCode = jvxErrorType.JVX_ERROR_INVALID_SETTING;
    }

    prop.last_error = errCode;
    if (errCode == jvxErrorType.JVX_NO_ERROR) {
      prop.ssUpdateId++;
      prop.cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_VALID.index);
    } else {
      prop.cache_status.bitSet(
          jvxPropertyCacheStatusFlagShifts.JVX_PROPERTY_CONTENT_ERROR.index);
      prop.ssUpdateId++;
    }
    return errCode;
  }

  // ==========================================================================
  // ==========================================================================

  static int updatePropertyContentSelListFromJsonMap(
      AudYoFloPropertySelectionListBackend propSel,
      List<String> selectedLst,
      String selectable,
      String exclusive,
      String numElms) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    if (propSel.selection is AudYoFloBitFieldBackend) {
      AudYoFloBitFieldBackend sel =
          propSel.selection as AudYoFloBitFieldBackend;
      if (sel.bitfield != null) {
        errCode = AudYoFloPropertyContentFromString.updateFieldBitField(
            sel.bitfield!,
            sel.numEntries,
            selectedLst,
            sel.compileNum32BitBitfield);
      }
    }
    if (errCode != jvxErrorType.JVX_NO_ERROR) return errCode;

    if (propSel.exclusive is AudYoFloBitFieldBackend) {
      List<String> selaLst = [selectable];
      AudYoFloBitFieldBackend sela =
          propSel.selectable as AudYoFloBitFieldBackend;
      if (sela.bitfield != null) {
        errCode = AudYoFloPropertyContentFromString.updateFieldBitField(
            sela.bitfield!,
            sela.numEntries,
            selaLst,
            sela.compileNum32BitBitfield);
      }
    }
    if (errCode != jvxErrorType.JVX_NO_ERROR) return errCode;

    if (propSel.exclusive is AudYoFloBitFieldBackend) {
      List<String> exclusiveLst = [exclusive];
      AudYoFloBitFieldBackend excl =
          propSel.exclusive as AudYoFloBitFieldBackend;
      if (excl.bitfield != null) {
        errCode = AudYoFloPropertyContentFromString.updateFieldBitField(
            excl.bitfield!,
            excl.numEntries,
            exclusiveLst,
            excl.compileNum32BitBitfield);
      }
    }
    if (errCode != jvxErrorType.JVX_NO_ERROR) return errCode;

    int? ll = int.tryParse(numElms);
    if (ll != null) {
      if (propSel.parpropms.entries.length != ll) {
        // What to do here?
        assert(false);
      }
    }
    return errCode;
  }

  static int updatePropertyContentValueInRageFromJsonMap(
      AudYoFloPropertyValueInRangeBackend propVal,
      List<String> valueLst,
      String minV,
      String maxV) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    propVal.minVal = double.parse(minV);
    propVal.maxVal = double.parse(maxV);
    var valFld = propVal.parpropmc;
    if (valFld is AudYoFloPropertyMultiContentBackend<Float64List>) {
      var valFldFlt =
          valFld as AudYoFloPropertyMultiContentBackend<Float64List>;

      // We might reuse this function here
      // AudYoFloPropertyContentFromJson.convertTextToField<Int64List>(
      //      prop.fld, prop.fldSz, newValues, prop.decoderHintType);
      if (valueLst.length != valFldFlt.fldSz) {
        errCode = jvxErrorType.JVX_ERROR_POSTPONE;
        return errCode;
      }

      for (var idx = 0; idx < valueLst.length; idx++) {
        var errCodeLoc =
            AudYoFloPropertyContentFromString.convertElementFldListDouble(
                valFldFlt.fld!, idx, valueLst[idx]);
        if (errCodeLoc != jvxErrorType.JVX_NO_ERROR) errCode = errCodeLoc;
      }
    }
    else if (valFld is AudYoFloPropertyMultiContentBackend<Float32List>) {
      var valFldFlt =
          valFld as AudYoFloPropertyMultiContentBackend<Float32List>;

      // We might reuse this function here
      // AudYoFloPropertyContentFromJson.convertTextToField<Int64List>(
      //      prop.fld, prop.fldSz, newValues, prop.decoderHintType);
      if (valueLst.length != valFldFlt.fldSz) {
        errCode = jvxErrorType.JVX_ERROR_POSTPONE;
        return errCode;
      }

      for (var idx = 0; idx < valueLst.length; idx++) {
        var errCodeLoc =
            AudYoFloPropertyContentFromString.convertElementFldListFloat(
                valFldFlt.fld!, idx, valueLst[idx]);
        if (errCodeLoc != jvxErrorType.JVX_NO_ERROR) errCode = errCodeLoc;
      }
    }
    return errCode;
  }

  static List<String> extractTextToStrList(String txt) {
    var lst = txt.split('[');
    if (lst.length == 2) lst = lst[1].split(']');
    String txtRemain = lst[0];
    lst = txtRemain.split(',');
    return lst;
  }

  static int convertTextToField<T1 extends List>(T1? fldData, int fldSz,
      Map newValues, jvxPropertyDecoderHintTypeEnum decHtTp, AudYoFloBackendTranslator trans) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    // Get the content
    String? base64Str = extractStringFromJson(newValues, 'property_fld');
    if (base64Str != null) {
      // Coming as a base64 string
      var lst64 = base64Decode(base64Str);
      if(fldData is Float32List)
      {
        Float32List tLst = lst64.buffer.asFloat32List();
        for(var i = 0; i < fldData.length; i++)
        {
          if(i < tLst.length) {
            fldData[i] = tLst[i];
          }
          else 
          { 
            // report error!
          }
        } 
      }
      else if(fldData is Float64List)
      {
        Float64List tLst = lst64.buffer.asFloat64List();
        for(var i = 0; i < fldData.length; i++)
        {
          if(i < tLst.length) {
            fldData[i] = tLst[i];
          }
          else 
          { 
            // report error!
          }
        } 
      }
      errCode = jvxErrorType.JVX_NO_ERROR;
    } else {
      String? txt = getStringEntryValueMap(newValues, 'property');
      if (txt != null) {
        int numElms = getIntEntryValueMap(newValues, 'number_elements');
        int offs = getIntEntryValueMap(newValues, 'offset');

        if (numElms != fldSz) {
          errCode = jvxErrorType.JVX_ERROR_POSTPONE;
          return errCode;
        }
        // Parse result
        // Remove leading and trailing []
        var lst = txt.split('[');
        if (lst.length == 2) lst = lst[1].split(']');
        String txtRemain = lst[0];
        lst = txtRemain.split(',');

        for (int idx = 0; idx < numElms; idx++) {
          String token = '0';
          if (idx < lst.length) {
            token = lst[idx];
          }
          int idxFld = offs + idx;
          if (idxFld < fldSz) {
            if (fldData != null) {
              errCode = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
              if (T1 == Int64List) {
                errCode = AudYoFloPropertyContentFromString
                    .convertElementFldListInt64(
                        fldData as Int64List, idx, token);
              } else {
                if (T1 == Uint64List) {
                  errCode = AudYoFloPropertyContentFromString
                      .convertElementFldListUint64(
                          fldData as Uint64List, idx, token);
                } else {
                  if (T1 == Int32List) {
                    errCode = AudYoFloPropertyContentFromString
                        .convertElementFldListInt32(
                            fldData as Int32List, idx, token, decHtTp);
                  } else {
                    if (T1 == Uint32List) {
                      errCode = AudYoFloPropertyContentFromString
                          .convertElementFldListUint32(
                              fldData as Uint32List, idx, token);
                    } else {
                      if (T1 == Int16List) {
                        errCode = AudYoFloPropertyContentFromString
                            .convertElementFldListInt16(
                                fldData as Int16List, idx, token, decHtTp, trans);
                      } else {
                        if (T1 == Uint16List) {
                          errCode = AudYoFloPropertyContentFromString
                              .convertElementFldListUint16(
                                  fldData as Uint16List, idx, token, decHtTp, trans);
                        } else {
                          if (T1 == Int8List) {
                            errCode = AudYoFloPropertyContentFromString
                                .convertElementFldListInt8(
                                    fldData as Int8List, idx, token);
                          } else {
                            if (T1 == Uint8List) {
                              errCode = AudYoFloPropertyContentFromString
                                  .convertElementFldListUint8(
                                      fldData as Uint8List, idx, token);
                            } else {
                              if (T1 == Float64List) {
                                errCode = AudYoFloPropertyContentFromString
                                    .convertElementFldListDouble(
                                        fldData as Float64List, idx, token);
                              } else {
                                if (T1 == Float32List) {
                                  errCode = AudYoFloPropertyContentFromString
                                      .convertElementFldListFloat(
                                          fldData as Float32List, idx, token);
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            if (errCode != jvxErrorType.JVX_NO_ERROR) return errCode;
          } else {
            errCode = jvxErrorType.JVX_ERROR_ID_OUT_OF_BOUNDS;
          }
        }
      }
    }
    return errCode;
  }
}
