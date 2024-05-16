import 'package:collection/collection.dart';
import 'package:ayf_pack/ayf_pack.dart';

/*
 * This is the base class for the backend adapter. It realizes the 
 * common functions required for every backend adapter.
 */ //=============================================================
abstract class AudYoFloBackendAdapter {
  // ==========================================================
  // Interfaces for inheriting component: MUST BE IMPLEMENTED IN DERIVED CLASS!!
  // ==========================================================

  // Translate the component identification struct into a string
  String translateComponent(JvxComponentIdentification elm);

  // Creator for component property reference
  dynamic createComponentPropertyRef(
      JvxComponentIdentification cpId, dynamic optParam);

  // opposite of the creator
  void destroyComponentPropertyReference(
      JvxComponentIdentification cpId, dynamic removeme);

  // ==========================================================
  // Member variables
  // ==========================================================

  // Reference to the backend adapter (Interface only)
  late AudYoFloBackendCacheBectrlIf theBeCache;

  // Map to hold all components
  Map<JvxComponentIdentification, dynamic> componentReferences = {};

  // ==========================================================
  // Initialize cross references
  // ==========================================================
  void initializeWithBackendCacheReference(
      AudYoFloBackendCacheBectrlIf beCacheArg) {
    theBeCache = beCacheArg;
  }

  // ==============================================================
  // Interface functions to be called from derived class
  // ==============================================================
  void reportSelectedComponent(JvxComponentIdentification cpId,
      {dynamic optParam}) {
    // Get the additional data as stored

    MapEntry? entryProps = componentReferences.entries
        .firstWhereOrNull((element) => element.key == cpId);
    if (entryProps == null) {
      dynamic addMe = createComponentPropertyRef(cpId, optParam);
      componentReferences[cpId] = addMe;
    } else {
      assert(false);
    }

    // Translate component into a descriptive name in the BackendAdapter
    String descr = translateComponent(cpId);
    theBeCache.reportSelectedComponent(cpId, descr);

    // Invalidate the proper selection list in cache
    // We need to report the new device at first and
    // THEN validate the option listr
    theBeCache.invalidateComponentListCache(cpId);
  }

  void reportUnselectedComponent(JvxComponentIdentification cpId) {
    // Invalidate the proper selection list in cache
    theBeCache.invalidateComponentListCache(cpId);

    MapEntry? entry = componentReferences.entries
        .firstWhereOrNull((element) => element.key == cpId);
    if (entry != null) {
      destroyComponentPropertyReference(cpId, entry.value);
      componentReferences.removeWhere((key, value) => key == cpId);
    } else {
      assert(false);
    }
    return theBeCache.reportUnselectedComponent(cpId);
  }
}
