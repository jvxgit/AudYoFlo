// TpjvxPropertyExtender.h

#define JVX_PRODUCT_PROPERTY_EXTENDER_TYPES \
	, JVX_PROPERTY_EXTENDER_PROPERTY_STREAM_AUDIO_PLUGIN \
	, JVX_PROPERTY_EXTENDER_DIRECT_MIXER_CONTROL \
	, JVX_PROPERTY_EXTENDER_HRTF_DISPENSER \
	, JVX_PROPERTY_EXTENDER_CHAIN_CONTROL \
	, JVX_PROPERTY_EXTENDER_HEADTRACKER_PROVIDER \
	, JVX_PROPERTY_EXTENDER_NFTF_PROVIDER \
	, JVX_PROPERTY_EXTENDER_SPATIAL_POSITION_DIRECT

/*
 * JVX_PROPERTY_EXTENDER_PROPERTY_STREAM_AUDIO_PLUGIN -> sources/jvxLibraries/jvx-system-product/include/interfaces/property-extender/IjvxPropertyExtenderStreamAudioPlugin.h
 * JVX_PROPERTY_EXTENDER_DIRECT_MIXER_CONTROL -> sources/jvxLibraries/jvx-system-product/include/interfaces/property-extender/IjvxPropertyExtenderDirectMixerControl.h
 * JVX_PROPERTY_EXTENDER_HRTF_DISPENSER -> sources/jvxLibraries/jvx-system-product/include/interfaces/property-extender/IjvxPropertyExtenderHrtfDispenser.h
 * JVX_PROPERTY_EXTENDER_CHAIN_CONTROL -> sources/jvxLibraries/jvx-system-product/include/interfaces/property-extender/IjvxPropertyExtenderChainControl.h
 * JVX_PROPERTY_EXTENDER_HEADTRACKER_PROVIDER -> sources/jvxLibraries/jvx-system-product/include/interfaces/property-extender/IjvxPropertyExtenderHeadTrackerProvider.h
 * JVX_PROPERTY_EXTENDER_SPATIAL_POSITION_DIRECT -> sources/jvxLibraries/jvx-system-product/include/interfaces/property-extender/IjvxPropertyExtenderSpatialDirectionDirect.h
 */

//
// If you add a new interface here, make sure, you add this in the typecast function, 
// -> sources\jvxLibraries\jvx-system-product\include\helpers\HpjvxCast.h
//
