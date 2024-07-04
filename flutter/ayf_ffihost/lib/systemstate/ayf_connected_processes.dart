import 'package:collection/collection.dart';

import 'package:ffi/ffi.dart';
import 'dart:ffi';
import '../native-ffi/generated/ayf_ffi_gen_bind.dart';
import 'package:ayf_pack/ayf_pack.dart';

class AudYoFloBackendSpecificNative {
  AudioYoFloNative natLib;
  Pointer<Void> opaque_hdl;
  Pointer<Void> itRef;
  AudYoFloBackendSpecificNative(this.natLib, this.opaque_hdl, this.itRef);
}

class AudYoFloOneComponentInProcessNative
    extends AudYoFloOneComponentInProcess {
  @override
  void fill(dynamic beSpecific) {
    AudYoFloBackendSpecificNative spec =
        beSpecific as AudYoFloBackendSpecificNative;
    var ret = JvxComponentIdentification();
    var numBranches = spec.natLib
        .ffi_process_decode_iterator_context(spec.opaque_hdl, spec.itRef);
    Pointer<component_ident> idPtr = spec.natLib
        .ffi_process_decode_iterator_component(spec.opaque_hdl, spec.itRef);
    Pointer<Int8> modPtr = spec.natLib
        .ffi_process_decode_iterator_module(spec.opaque_hdl, spec.itRef);
    Pointer<Int8> cnamePtr = spec.natLib
        .ffi_process_decode_iterator_connector(spec.opaque_hdl, spec.itRef);

    if ((idPtr != nullptr) && (modPtr != nullptr)) {
      nameConnector = cnamePtr.cast<Utf8>().toDartString();
      spec.natLib.ffi_host_delete(cnamePtr.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);

      // Decode Module name
      nameModule = modPtr.cast<Utf8>().toDartString();
      spec.natLib.ffi_host_delete(modPtr.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);

      // Decode component ident
      component_ident cpIdRef = idPtr.ref;
      ret.cpTp = JvxComponentTypeEEnum.fromInt(cpIdRef.tp);
      ret.slotid = cpIdRef.slotid;
      ret.slotsubid = cpIdRef.slotsubid;
      ret.uid = cpIdRef.uId;

      // Delete moved to later (line 58) since we still need the component_ident
      cpId = ret;

      // Get the component description
      Pointer<Int8> descrPtr =
          spec.natLib.ffi_component_description(spec.opaque_hdl, idPtr);
      if (descrPtr != nullptr) {
        descriptionComponent = (descrPtr.cast<Utf8>()).toDartString();
        spec.natLib.ffi_host_delete(descrPtr.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
      }
      spec.natLib.ffi_host_delete(idPtr.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_ONE_COMPONENT_IDENT);
    }

    for (int ii = 0; ii < numBranches; ii++) {
      Pointer<Void> itNext = spec.natLib
          .ffi_process_iterator_next(spec.opaque_hdl, spec.itRef, ii);
      if (itNext != nullptr) {
        AudYoFloOneComponentInProcess connectTo =
            AudYoFloOneComponentInProcessNative();
        connectTo.fill(AudYoFloBackendSpecificNative(
            spec.natLib, spec.opaque_hdl, itNext));
        attach(connectTo);
      }
    }
  }
}
