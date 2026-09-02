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
        .ffi_process_decode_iterator_number_branches(spec.opaque_hdl, spec.itRef);
    Pointer<component_ident> idPtr = spec.natLib
        .ffi_process_decode_iterator_component(spec.opaque_hdl, spec.itRef);
    Pointer<Char> modPtr = spec.natLib
        .ffi_process_decode_iterator_module(spec.opaque_hdl, spec.itRef);
    Pointer<Char> descrPtr = spec.natLib
        .ffi_process_decode_iterator_descriptor(spec.opaque_hdl, spec.itRef);

    if ((idPtr != nullptr) && (modPtr != nullptr) && (descrPtr != nullptr)) {

      // Decode Module name
      nameModule = modPtr.cast<Utf8>().toDartString();

      // Get the component description
      descriptionComponent = (descrPtr.cast<Utf8>()).toDartString();

      // Decode component ident
      component_ident cpIdRef = idPtr.ref;
      ret.cpTp = JvxComponentTypeEEnum.fromInt(cpIdRef.tp);
      ret.slotid = cpIdRef.slotid;
      ret.slotsubid = cpIdRef.slotsubid;
      ret.uid = cpIdRef.uId;

      // Delete moved to later (line 58) since we still need the component_ident
      cpId = ret;

    // Free all the memory
      spec.natLib.ffi_host_delete(idPtr.cast<Void>(),
          ffiDeleteDatatype.JVX_DELETE_DATATYPE_ONE_COMPONENT_IDENT);

      spec.natLib.ffi_host_delete(descrPtr.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);

      spec.natLib.ffi_host_delete(modPtr.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CHAR_ARRAY);
    }

    for (int ii = 0; ii < numBranches; ii++) {
      Pointer<connector_bridge> conBridge = spec.natLib
          .ffi_process_iterator_next_handle(spec.opaque_hdl, spec.itRef, ii);
      if (conBridge != nullptr) {
 
        var conBridgeInst = conBridge.ref;

        AudYoFloOneComponentInProcess connectTo =
            AudYoFloOneComponentInProcessNative();
        connectTo.fill(AudYoFloBackendSpecificNative(
            spec.natLib, spec.opaque_hdl, conBridgeInst.next));

            // ================================================================================
        if(conBridgeInst.nmOconFrom!= nullptr)
        { 
          connectTo.nmOutputConnectorFrom = (conBridgeInst.nmOconFrom.cast<Utf8>()).toDartString();
        }
         if(conBridgeInst.nmIconTo!= nullptr)
        {
          connectTo.nmInputConnectorTo = (conBridgeInst.nmIconTo.cast<Utf8>()).toDartString();
        }

        // ================================================================================
          spec.natLib.ffi_host_delete(conBridge.cast<Void>(),
            ffiDeleteDatatype.JVX_DELETE_DATATYPE_CONNECTOR_BRIDGE);    
        attach(connectTo);
      }
    }
  }
}
