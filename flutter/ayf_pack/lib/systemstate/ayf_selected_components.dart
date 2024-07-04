import 'package:collection/collection.dart';
import '../ayf_pack_local.dart';

class AudYoFloPropertyCache {
  List<String>? availProperties;
  Map<String, AudYoFloPropertyContainer> cachedProperties = {};
  int ssPropertyCache = 0;
  AudYoFloPropertyContainer? findCachedProperty(String nm) {
    AudYoFloPropertyContainer? theProp;
    MapEntry<String, AudYoFloPropertyContainer>? entryP = cachedProperties
        .entries
        .firstWhereOrNull((element) => element.key == nm);
    if (entryP != null) {
      theProp = entryP.value;
    }
    return theProp;
  }

  int invalidateCachedProperty(String nm) {
    int errCode = jvxErrorType.JVX_NO_ERROR;
    MapEntry<String, AudYoFloPropertyContainer>? entryP = cachedProperties
        .entries
        .firstWhereOrNull((element) => element.key == nm);
    if (entryP != null) {
      entryP.value.ssUpdateId++;
      entryP.value.cache_status.bitZSet(jvxPropertyCacheStatusFlagShifts
          .JVX_PROPERTY_DESCRIPTOR_INVALID.index);
    } else {
      errCode = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    }
    return errCode;
  }
}

/*
class AudYoFloOneDevice {
  String descriptor = 'not-found';
  String description = 'not-found';
  AudYoFloCBitField32 caps = AudYoFloCBitField32();
  jvxDeviceDataFlowTypeEnum flow =
      jvxDeviceDataFlowTypeEnum.JVX_DEVICE_DATAFLOW_UNKNOWN;
  bool selectable = false;
  int subslotid = -1;
}
*/

// Store one component. The ssThisComponent field holds a counter that is increased
// whenever the component has changed - e.g. if switching the state
class AudYoFloOneSelectedComponent {
  String description = 'Unknown';
  String moduleName = 'unknwon';
  AudYoFloDirectPropertiesIf? directPropRef;
  int ssThisComponent = 0;
  //jvxAccessProperties_flutter? propRef;
  AudYoFloPropertyCache propertyCache = AudYoFloPropertyCache();
  //List<AudYoFloOneDevice>? devLst;
}

// Class to keep track of all components selected in the backend
class AudYoFloSelectedComponents {
  // Store the map, one entry for each component
  Map<JvxComponentIdentification, AudYoFloOneSelectedComponent>
      selectedComponents =
      Map<JvxComponentIdentification, AudYoFloOneSelectedComponent>();
  /*
  late fernLiveNative natLib;
  late Pointer<Void> opaqueHost;
*/
  AudYoFloBackendAdapterIf? be;

  // =================================================================
  AudYoFloSelectedComponents();

  void initialize(AudYoFloBackendAdapterIf? beArg) {
    be = beArg;
  }

  // Add a newly created component
  int addComponent(JvxComponentIdentification cpId, String description,
      {AudYoFloDirectPropertiesIf? directPropRef}) {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    MapEntry? entry = selectedComponents.entries
        .firstWhereOrNull((element) => element.key == cpId);
    if (entry != null) {
      retVal = retVal = jvxErrorType.JVX_ERROR_ALREADY_IN_USE;
    } else {
      //jvxAccessProperties_flutter? theComp = be?.requestPropertyReference(cpId);
      // if (theComp != null) {
      var compObj = AudYoFloOneSelectedComponent();
      //compObj.propRef = theComp;
      compObj.description = description;
      compObj.directPropRef = directPropRef;
      selectedComponents[cpId] = compObj;
      //} else {
      //  print('No property reference for component <${cpId.txt}> on select');
      //}
    }

    return retVal;
  }

  // Update a component. This may be due to a state switch
  int updateComponent(JvxComponentIdentification cpId) {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    MapEntry? entry = selectedComponents.entries
        .firstWhereOrNull((element) => element.key == cpId);
    if (entry == null) {
      retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    } else {
      AudYoFloOneSelectedComponent comp = entry.value;
      comp.ssThisComponent++;
    }
    return retVal;
  }

  // Remove a component from list of selected components
  int removeComponent(JvxComponentIdentification cpId) {
    int retVal = jvxErrorType.JVX_NO_ERROR;
    MapEntry? entry = selectedComponents.entries
        .firstWhereOrNull((element) => element.key == cpId);
    if (entry == null) {
      retVal = jvxErrorType.JVX_ERROR_ELEMENT_NOT_FOUND;
    } else {
      AudYoFloOneSelectedComponent entry_type = entry.value;
      // jvxAccessProperties_flutter? propRefPtr = entry_type.propRef;

      selectedComponents.removeWhere((key, value) => key == cpId);
    }
    return retVal;
  }
}
