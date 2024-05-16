import 'dart:typed_data';
import 'dart:ffi';
import 'dart:math';
import 'package:ffi/ffi.dart';
import 'package:fixnum/fixnum.dart' as fn;
import '../native-ffi/generated/ayf_ffi_gen_bind.dart';
import 'package:ayf_pack/ayf_pack.dart';
import 'package:ayf_pack_native/ayf_pack_native.dart';

import '../backend/ayf_backend_adapter_native.dart';

class AudYoFloPropertyContentFromNative {
  // ========================================================================
  // ========================================================================

  static AudYoFloPropertyContentBackend createPropertyFromNative(
      AudYoFloBackendAdapterIf ref,
      String descriptor,
      AudYoFloNativePropertyHandle propRef,
      AudYoFloCompileFlags flags, int numDigits) {
    // =================================================================
    // Get the descriptor from the ffi backend
    // =================================================================

    Pointer<Utf8> descror = descriptor.toNativeUtf8();
    Pointer<one_property_full_plus> propDescrPtr = propRef.natLib
        .ffi_properties_descriptor_property_descr(
            propRef.opaqueHost,
            propRef.propRef,
            descror.cast<Int8>(),
            jvxPropertyDescriptorEnum.JVX_PROPERTY_DESCRIPTOR_FULL_PLUS);

    // A nullptr indicates an error
    if (propDescrPtr == nullptr) {
      // Create a property container anyway and set the error flag
      AudYoFloPropertyContentBackend retVal =
          AudYoFloPropertyContentBackend(propRef.cpId, descriptor: descriptor);
      retVal.last_error = propRef.natLib.ffi_last_error();

      retVal.invalidateDescriptor(true);
      return retVal;
    }

    // Successful descriptor return
    one_property_full_plus propDescrRef = propDescrPtr.ref;
    jvxDataFormatEnum format =
        jvxDataFormatEEnum.fromInt(propDescrRef.full.ctrl.core.format);

    // Allocate the variable
    AudYoFloPropertyContentBackend retVal = allocatePropertyContent(
        propRef.cpId, format, propDescrRef.full.ctrl.core.num, flags, numDigits);

    updatePropertyDescriptorFromNative(ref, retVal, propRef, flags, false,
        ffiPtr: propDescrPtr);

    return retVal;
  }

  // ========================================================================
  // ========================================================================

  static int updatePropertyDescriptorFromNative(
      AudYoFloBackendAdapterIf ref,
      AudYoFloPropertyContentBackend prop,
      AudYoFloNativePropertyHandle propRef,
      AudYoFloCompileFlags flags,
      bool checkFormatNum,
      {Pointer<one_property_full_plus>? ffiPtr = null}) {
    // =================================================================
    // Get the descriptor from the ffi backend
    // =================================================================
    Pointer<one_property_full_plus> propDescrPtr = nullptr;
    prop.last_error = jvxErrorType.JVX_NO_ERROR;

    if (ffiPtr != null) {
      propDescrPtr = ffiPtr;
    } else {
      Pointer<Utf8> descror = prop.descriptor.toNativeUtf8();
      propDescrPtr = propRef.natLib.ffi_properties_descriptor_property_descr(
          propRef.opaqueHost,
          propRef.propRef,
          descror.cast<Int8>(),
          jvxPropertyDescriptorEnum.JVX_PROPERTY_DESCRIPTOR_FULL_PLUS);
    }

    // A nullptr indicates an error
    if (propDescrPtr == nullptr) {
      // Create a property container anyway and set the error flag
      prop.last_error = propRef.natLib.ffi_last_error();
    } else {
      // Do not forget to set the success to "NO ERROR"!

      // Successful descriptor return
      one_property_full_plus propDescrRef = propDescrPtr.ref;

      // checkFormatNum checks if an update is required compared to what is already allocated
      // This affects only field entries where the field of this property may
      // vary from update to update. It is only necessary to call the check part
      // if the property was created before. If checkFormatNum is false, this is
      // due to the fact that the property has very recently be created.
      if (checkFormatNum) {
        jvxDataFormatEnum format =
            jvxDataFormatEEnum.fromInt(propDescrRef.full.ctrl.core.format);

        // Changing the dataformat is not supported
        if (prop.jvxFormat != format) {
          String t1 = prop.jvxFormat.toString();
          String t2 = format.toString();
          String t3 = prop.descriptor;
          print(
              'Error when rereading property description for property <$t3>: format has changed: old: $t1, new: $t2');
        }

        // Changing the number of entries, however, is ok and often used!
        // Check if the variable has been allocated with a different configuration
        if (prop.num != propDescrRef.full.ctrl.core.num) {
          // Dealloc
          if (prop is AudYoFloPropertyMultiContentNative) {
            prop.rebuild(propDescrRef.full.ctrl.core.num);
          }
          if (prop is AudYoFloPropertySelectionListNative) {
            prop.resize(propDescrRef.full.ctrl.core.num, true, false);
          }
        }
      }

      // Min descr
      prop.category = jvxPropertyCategoryTypeEEnum
          .fromInt(propDescrRef.full.ctrl.core.pmin.category);
      prop.globalIdx = propDescrRef.full.ctrl.core.pmin.globalIdx;

      // Core desc
      /*
       * Set in the alloc function call
      retVal.format =
          jvxDataFormatEEnum.fromInt(propDescrRef.full.ctrl.core.format);
      retVal.num = propDescrRef.full.ctrl.core.num;
      */
      prop.decoderHintType = jvxPropertyDecoderHintTypeEEnum
          .fromInt(propDescrRef.full.ctrl.core.decTp);
      prop.ctxt =
          jvxPropertyContexteEEnum.fromInt(propDescrRef.full.ctrl.core.ctxt);
      prop.accessType = jvxPropertyAccessTypeEEnum
          .fromInt(propDescrRef.full.ctrl.core.accessType);

      // Control desc
      prop.isValid = (propDescrRef.full.ctrl.isValid != 0);
      prop.reqInvalidateOnStateSwitch = jvxPropertyInvalidateTypeEEnum
          .fromInt(propDescrRef.full.ctrl.invalidateOnStateSwitch);
      prop.reqInvalidateOnTest = jvxPropertyInvalidateTypeEEnum
          .fromInt(propDescrRef.full.ctrl.invalidateOnTest);
      prop.allowedStateMask.fld =
          fn.Int32(propDescrRef.full.ctrl.allowedStateMask);
      prop.installable = (propDescrRef.full.ctrl.installable != 0);
      prop.allowedThreadingMask.fld =
          fn.Int32(propDescrRef.full.ctrl.allowedThreadingMask);

      // Full desc
      prop.name = propDescrRef.full.name.cast<Utf8>().toDartString();
      prop.description =
          propDescrRef.full.description.cast<Utf8>().toDartString();
      prop.descriptor =
          propDescrRef.full.descriptor.cast<Utf8>().toDartString();

      prop.accessFlags.fld = fn.Int64(propDescrRef.accessFlags);
      prop.configFlags.fld = fn.Int32(propDescrRef.configModeFlags);

      propRef.natLib.ffi_host_delete(propDescrRef.full.name.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
      propRef.natLib.ffi_host_delete(propDescrRef.full.description.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
      propRef.natLib.ffi_host_delete(propDescrRef.full.descriptor.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);

      if (ffiPtr == null) {
        propRef.natLib.ffi_host_delete(
            propDescrPtr.cast<Void>(),
            ffiDeleteDatatype
                .JVX_DELETE_DATATYPE_ONE_PROPERTY_DESCRIPTOR_FULL_PLUS);
      }
    }

    prop.validateDescriptor();
    return prop.last_error;
  }

  // ========================================================================
  // ========================================================================

  static int updatePropertyContentFromNative(
      AudYoFloPropertyContentBackend prop,
      String descriptor,
      AudYoFloNativePropertyHandle propRef,
      {int contentOnly = 0,
      int offset = 0,
      int numEntriesLimit = -1}) {
    prop.last_error = jvxErrorType.JVX_NO_ERROR;

    // Check that descriptor is ready
    if (!prop.checkValidDescriptor()) {
      prop.last_error = jvxErrorType.JVX_ERROR_NOT_READY;
    } else {
      // Descriptor is ready
      switch (prop.jvxFormat) {
        case jvxDataFormatEnum.JVX_DATAFORMAT_STRING_LIST:

          // Datatype string list, check that variable is of right format
          if (prop is AudYoFloPropertyMultiStringBackend) {
            // Cast it directly
            AudYoFloPropertyMultiStringBackend propStrLst = prop;

            // Return the ffi struct from backend
            Pointer<one_property_string_list> retVal = propRef.natLib
                .ffi_properties_get_string_list(
                    propRef.opaqueHost,
                    propRef.propRef,
                    prop.descriptor.toNativeUtf8().cast<Int8>(),
                    contentOnly,
                    offset);
            if (retVal != nullptr) {
              // It is a valid string list
              one_property_string_list retValRef = retVal.ref;

              // It seems there is no resize function in the List<>
              propStrLst.entries.clear();

              // Copy all string from the list
              for (int i = 0; i < retValRef.nEntries; i++) {
                Pointer<Int8> ptrStr = retValRef.pEntries[i];
                if (ptrStr != nullptr) {
                  propStrLst.entries.add(ptrStr.cast<Utf8>().toDartString());
                } else {
                  propStrLst.entries.add('');
                }
              }
              int errCodeLocal = propRef.natLib.ffi_host_delete(
                  retVal.cast<Void>(),
                  ffiDeleteDatatype.JVX_DELETE_DATATYPE_PROPERTY_STRING_LIST);
              assert(errCodeLocal == jvxErrorType.JVX_NO_ERROR);
            }
          } else {
            prop.last_error = jvxErrorType.JVX_ERROR_INTERNAL;
          }
          break;

        case jvxDataFormatEnum.JVX_DATAFORMAT_SELECTION_LIST:

          // Datatype string list, check that variable is of right format
          if (prop is AudYoFloPropertySelectionListBackend) {
            // Cast it directly
            AudYoFloPropertySelectionListBackend propSelLst =
                prop as AudYoFloPropertySelectionListBackend;

            int numEntries = propSelLst.parpropms.num - offset;
            if (numEntriesLimit > 0) {
              numEntries = min(numEntries, numEntriesLimit);
            }
            if (numEntries > 0) {
              Pointer<one_property_string_list> retVal = nullptr;
              if ((propSelLst.selection is AudYoFloBitFieldNative) &&
                  (propSelLst.selectable is AudYoFloBitFieldNative) &&
                  (propSelLst.exclusive is AudYoFloBitFieldNative)) {
                AudYoFloBitFieldNative sel =
                    propSelLst.selection as AudYoFloBitFieldNative;
                AudYoFloBitFieldNative sela =
                    propSelLst.selectable as AudYoFloBitFieldNative;
                AudYoFloBitFieldNative excl =
                    propSelLst.exclusive as AudYoFloBitFieldNative;

                if (sel.raw != nullptr) {
                  Pointer<Uint32> targetPtr = sel.raw;
                  int address = targetPtr.address;
                  address +=
                      offset * sel.compileNum32BitBitfield * sizeOf<Uint32>();
                  targetPtr = Pointer<Uint32>.fromAddress(address);

                  // Return the ffi struct from backend
                  retVal = propRef.natLib.ffi_properties_get_selection_list(
                      propRef.opaqueHost,
                      propRef.propRef,
                      prop.descriptor.toNativeUtf8().cast<Int8>(),
                      contentOnly,
                      offset,
                      targetPtr,
                      sela.raw,
                      excl.raw,
                      numEntries,
                      propSelLst.compileNum32BitBitfield);
                }
                if (retVal == nullptr) {
                  prop.last_error = propRef.natLib.ffi_last_error();
                } else {
                  // It is a valid string list
                  one_property_string_list retValRef = retVal.ref;

                  // It seems there is no resize function in the List<>
                  propSelLst.parpropms.entries.clear();

                  // Copy all string from the list
                  for (int i = 0; i < retValRef.nEntries; i++) {
                    Pointer<Int8> ptrStr = retValRef.pEntries[i];
                    if (ptrStr != nullptr) {
                      propSelLst.parpropms.entries
                          .add(ptrStr.cast<Utf8>().toDartString());
                    } else {
                      propSelLst.parpropms.entries.add('');
                    }
                  }

                  int errCodeLocal = propRef.natLib.ffi_host_delete(
                      retVal.cast<Void>(),
                      ffiDeleteDatatype
                          .JVX_DELETE_DATATYPE_PROPERTY_STRING_LIST);
                  assert(errCodeLocal == jvxErrorType.JVX_NO_ERROR);
                }
              } else {
                String nm = prop.descriptor;
                print(
                    'Property with descriptor $nm: Error reading field for property, field contains nullptr');
                prop.last_error = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
              }
            } else {
              String nm = prop.descriptor;
              print(
                  'Property with descriptor $nm: Error reading field for property, field has zero length');
              prop.last_error = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
            }
          }

          break;
        case jvxDataFormatEnum.JVX_DATAFORMAT_STRING:
          if (prop is AudYoFloPropertySingleStringBackend) {
            // Cast it directly
            AudYoFloPropertySingleStringBackend propString = prop;

            // Return the ffi struct from backend
            Pointer<Int8> retVal = propRef.natLib.ffi_properties_get_string(
                propRef.opaqueHost,
                propRef.propRef,
                prop.descriptor.toNativeUtf8().cast<Int8>());
            if (retVal == nullptr) {
              prop.last_error = propRef.natLib.ffi_last_error();
            } else {
              // It is a valid string list

              propString.value = retVal.cast<Utf8>().toDartString();

              int errCodeLocal = propRef.natLib.ffi_host_delete(
                  retVal.cast<Void>(),
                  ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
              assert(errCodeLocal == jvxErrorType.JVX_NO_ERROR);
            }
          }
          break;

        case jvxDataFormatEnum.JVX_DATAFORMAT_DATA:
        case jvxDataFormatEnum.JVX_DATAFORMAT_64BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_32BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_16BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_8BIT:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U64BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U32BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U16BIT_LE:
        case jvxDataFormatEnum.JVX_DATAFORMAT_U8BIT:
        case jvxDataFormatEnum.JVX_DATAFORMAT_SIZE:

          // =========================================================
          // Trying to cover all operations with one code fragment
          // =========================================================
          if (prop is AudYoFloPropertyMultiContentNative) {
            // Transfer all or less elements
            int numEntries = prop.fldSz;
            numEntries -= offset;
            if (numEntriesLimit > 0) {
              numEntries = min(numEntries, numEntriesLimit);
            }

            if (numEntries > 0) {
              if (prop.fldPtr != nullptr) {
                int address = prop.fldPtr.address;
                address += offset * prop.szNativeElement;
                Pointer<Void> targetPtr = Pointer<Void>.fromAddress(address);

                prop.last_error = propRef.natLib
                    .ffi_properties_get_content_property_fld(
                        propRef.opaqueHost,
                        propRef.propRef,
                        prop.descriptor.toNativeUtf8().cast<Int8>(),
                        contentOnly,
                        offset,
                        targetPtr,
                        prop.jvxFormat.index,
                        numEntries);
              } else {
                String nm = prop.descriptor;
                print(
                    'Property with descriptor $nm: Error reading field for property, field contains nullptr');
                prop.last_error = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
              }
            } else {
              String nm = prop.descriptor;
              print(
                  'Property with descriptor $nm: Error reading field for property, field has zero length');
              prop.last_error = jvxErrorType.JVX_ERROR_INVALID_ARGUMENT;
            }
          }
          break;

        case jvxDataFormatEnum.JVX_DATAFORMAT_VALUE_IN_RANGE:

          // Datatype string list, check that variable is of right format
          if (prop is AudYoFloPropertyValueInRangeNative) {
            // Cast it directly
            AudYoFloPropertyValueInRangeNative propValRange = prop;

            int numEntries = prop.fldSz;
            numEntries -= offset;
            if (numEntriesLimit > 0) {
              numEntries = min(numEntries, numEntriesLimit);
            }

            if (numEntries > 0) {
              if (propValRange.fldPtr != nullptr) {
                Pointer<Void> targetPtr = propValRange.fldPtr.cast<Void>();
                int address = targetPtr.address;
                address += offset * propValRange.szNativeElement;
                targetPtr = Pointer<Void>.fromAddress(address);

                // Return the ffi struct from backend
                Pointer<one_property_value_in_range> retVal = propRef.natLib
                    .ffi_properties_get_value_in_range(
                        propRef.opaqueHost,
                        propRef.propRef,
                        prop.descriptor.toNativeUtf8().cast<Int8>(),
                        contentOnly,
                        offset,
                        targetPtr,
                        numEntries);

                prop.last_error = propRef.natLib.ffi_last_error();

                if (contentOnly != 0) {
                  // It is a valid string list
                  one_property_value_in_range retValRef = retVal.ref;
                  propValRange.minVal = retValRef.minVal;
                  propValRange.maxVal = retValRef.maxVal;

                  int errCodeLocal = propRef.natLib.ffi_host_delete(
                      retVal.cast<Void>(),
                      ffiDeleteDatatype.JVX_DELETE_DATATYPE_VALUE_IN_RANGE);
                  assert(errCodeLocal == jvxErrorType.JVX_NO_ERROR);
                }
              }
            }
          }

          break;
        default:
          prop.last_error = jvxErrorType.JVX_ERROR_INVALID_FORMAT;
          break;
      }
    }

    if (prop.last_error == jvxErrorType.JVX_NO_ERROR) {
      prop.validateContent();
    } else {
      prop.invalidateContent(true);
    }

    return prop.last_error;
  }
}
