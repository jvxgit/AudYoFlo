import 'package:collection/collection.dart';
import '../ayf_pack_local.dart';

/* One property defintion in group: valid descriptor, null prop ref at first */
class AudYoFloPropertyOnChangeOneGroupOneProp {
  final String propertyDescr;
  AudYoFloPropertyContainer? prop;
  AudYoFloPropertyOnChangeOneGroupOneProp(this.propertyDescr);
}

/* One property group to combine multiples properties */
class AudYoFloPropertyOnChangeOneGroup {
  List<AudYoFloPropertyOnChangeOneGroupOneProp> propsInGroup = [];
  AudYoFloPropertyOnChangeOneGroup(List<String> props) {
    for (var elm in props) {
      AudYoFloPropertyOnChangeOneGroupOneProp oneProp =
          AudYoFloPropertyOnChangeOneGroupOneProp(elm);
      propsInGroup.add(oneProp);
    }
  }

  AudYoFloPropertySingleStringBackend? extractPropString(String tag) {
    AudYoFloPropertySingleStringBackend? retVal;

    var elm = extractPropContent(tag);
    if (elm != null) {
      if (elm is AudYoFloPropertySingleStringBackend) {
        retVal = elm;
      }
    }
    return retVal;
  }

  AudYoFloPropertyContainer? extractPropContent(String tag) {
    AudYoFloPropertyContainer? retVal;
    var elm = propsInGroup
        .firstWhereOrNull((element) => element.propertyDescr == tag);
    if (elm != null) {
      retVal = elm.prop;
    }
    return retVal;
  }

  AudYoFloPropertySelectionListBackend? extractPropSelectionList(String tag) {
    AudYoFloPropertySelectionListBackend? retVal;
    var elm = extractPropContent(tag);
    if (elm != null) {
      if (elm is AudYoFloPropertySelectionListBackend) {
        retVal = elm as AudYoFloPropertySelectionListBackend;
      }
    }
    return retVal;
  }

  AudYoFloPropertyValueInRangeBackend? extractPropValueInRange(String tag) {
    AudYoFloPropertyValueInRangeBackend? retVal;
    var elm = extractPropContent(tag);
    if (elm != null) {
      if (elm is AudYoFloPropertyValueInRangeBackend) {
        retVal = elm as AudYoFloPropertyValueInRangeBackend;
      }
    }
    return retVal;
  }

  AudYoFloPropertyMultiContentBackend? extractPropMultiContent(String tag) {
    AudYoFloPropertyMultiContentBackend? retVal;
    var elm = extractPropContent(tag);
    if (elm != null) {
      if (elm is AudYoFloPropertyMultiContentBackend) {
        retVal = elm as AudYoFloPropertyMultiContentBackend;
      }
    }
    return retVal;
  }

  AudYoFloPropertyMultiStringBackend? extractPropMultiString(String tag) {
    AudYoFloPropertyMultiStringBackend? retVal;
    var elm = extractPropContent(tag);
    if (elm != null) {
      if (elm is AudYoFloPropertyMultiStringBackend) {
        retVal = elm;
      }
    }
    return retVal;
  }
}

/* Multiple groups combined **/
mixin AudYoFloPropertyOnChangeGroups {
  Map<String, AudYoFloPropertyOnChangeOneGroup> theGroups = {};
  List<String> propsAllGroups = [];

  void createList() {
    propsAllGroups.clear();
    for (var elmG in theGroups.entries) {
      for (var elm in elmG.value.propsInGroup) {
        var fElm = propsAllGroups.firstWhereOrNull((str) {
          return (str == elm.propertyDescr);
        });
        if (fElm == null) {
          // Add element only if not in the list yet
          propsAllGroups.add(elm.propertyDescr);
        }
      }

      // Here we have a groupt which contains only unique properties
    }
  }

  void reconstructProperyReferences(List<AudYoFloPropertyContainer> props) {
    for (var elmG in theGroups.entries) {
      for (var elm in elmG.value.propsInGroup) {
        // Property references may be used in multiple groups or multiple times in single group!
        // It is always a REFERNCE!
        AudYoFloPropertyContainer? propRef = props.firstWhereOrNull(
            (element) => element.descriptor == elm.propertyDescr);
        elm.prop = propRef;
      }
    }
  }
}
