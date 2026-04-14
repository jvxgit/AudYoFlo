#include "CayfATStarter.h"

#define JVX_PURPOSE_INPUT 0x1000
#define JVX_PURPOSE_OUTPUT 0x2000
#define JVX_PURPOSE_BARESIPIO_AUDIO 0x4000
#define JVX_PURPOSE_AUDIO_IO 0x8000
#define JVX_PURPOSE_AUDIO_IO_WITH_MODULES 0x10000
#define JVX_PURPOSE_VIDEO_IO 0x20000
#define JVX_PURPOSE_BARESIPIO_VIDEO 0x40000

#define JVX_CONNECTION_CATEGORY_FILEREADER_AUDIO 16
#define JVX_CONNECTION_CATEGORY_FILEWRITER_AUDIO 17
#define JVX_CONNECTION_CATEGORY_VIDEO_IO 18

#define JVX_ID_TRIGGER_FORWARD 1

CayfATStarter::CayfATStarter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAutomationReportConnect(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	nmComponent = "CayfATStarter";
}

CayfATStarter::~CayfATStarter()
{
}

// ==================================================================================================

#define SYNC_IO__

jvxErrorType 
CayfATStarter::activate()
{
	jvxErrorType res = CjvxAutomationReportConnect::activate();
	if (res == JVX_NO_ERROR)
	{
		
		ioAudioWithModules.activate(_common_set.theReport, refHostRef, this, JVX_PURPOSE_AUDIO_IO_WITH_MODULES,
			
			CayfAutomationModules::ayfConnectConfigCpEntrySyncIo(
				JVX_COMPONENT_SIGNAL_PROCESSING_NODE, // <- comppnent type to open component from
				"jvxSpNSynchronize", // <- module name
				"PriSyncChain", // Name of primary chain
				"default", // <- master name
				"default", "default",  // <- output and input connector name, tp-trigger 
				"default", "default", // <- input and output connector name, tp-triggered

				// Manipulate component once opened
				CayfAutomationModules::ayfConnectConfigCpManipulate(

					// Convert component to this type
					JVX_COMPONENT_DYNAMIC_NODE,

					// Attach uid
					true,

					// Manipulation suffix to add to better identify
					""),

				// Some misc args
				CayfAutomationModules::ayfConnectConfigConMiscArgs(
					// Connection category (default)
					JVX_SIZE_UNSELECTED,

					// Debug output
					false)
				),

			CayfAutomationModules::ayfConnectConfigSrc2SnkPreChain(

				// This is for the first part of the chain: default --> ... --> mix-in
				"sec-audio-out",
				{
					CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNConvert", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - input")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNForwardBuffer", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - input")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, "jvxSpNLevelControl", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - input")),
				}, "mix-in", jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
				CayfAutomationModules::ayfConnectConfigSrc2Snk("AdditionalDuplexDev#",
					"sec-audio-master",

					// This is for the second part of the chain! SP<2>-mix-out --> ... --> tp_active-default
					"mix-out", 
					{
						// Connection: device->convert->forward buffer -> enter ... leave -> forward buffer -> convert -> device
						// Pre chain : device->convert->forward buffer -> enter 


						// Chain : leave -> forward buffer -> convert -> device
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNForwardBuffer", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - output")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNConvert", "default", "default",
						CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - output"))
					}, 
					"sec-audio-in", 
					jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0),
					CayfAutomationModules::ayfConnectConfigConMiscArgs(JVX_PURPOSE_BARESIPIO_AUDIO, false),
					JVX_ID_TRIGGER_FORWARD, JVX_SIZE_UNSELECTED),
				JVX_SIZE_UNSELECTED, JVX_ID_TRIGGER_FORWARD),
			this);
		CayfAutomationModuleHandler::CayfAutomationModuleHandler::register_element(&ioAudioWithModules);

		// ===================================================================================================
		ioBaresipAudio.activate(_common_set.theReport, refHostRef, this, JVX_PURPOSE_BARESIPIO_AUDIO,

			// master="default":ocon="default" --> <first elements> --> "mix-out" --> ... -> default
			CayfAutomationModules::ayfConnectConfigSrc2SnkPreChain(

				// This is for the first part of the chain: default --> ... --> mix-in
				"audio",
				{
					// Connection: device->convert->forward buffer -> enter ... leave -> forward buffer -> convert -> device
					// Pre chain : device->convert->forward buffer -> enter 
					CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNConvert", "default", "default",
						CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - input")),
					CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNForwardBuffer", "default", "default",
						CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - input")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, "jvxSpNLevelControl", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - input"))
				},
				"mix-in", jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
				CayfAutomationModules::ayfConnectConfigSrc2Snk("baresipIO#",


					"audio",
					// This is for the second part of the chain! SP<2>-mix-out --> ... --> tp_active-default
					"mix-out",
					{
						// Connection: device->convert->forward buffer -> enter ... leave -> forward buffer -> convert -> device
						// Pre chain : device->convert->forward buffer -> enter 
						// Chain : leave -> forward buffer -> convert -> device
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNForwardBuffer", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - output")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNConvert", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - output"))
					},
					"audio",
					jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0),
					CayfAutomationModules::ayfConnectConfigConMiscArgs(JVX_PURPOSE_BARESIPIO_AUDIO, false),
					JVX_ID_TRIGGER_FORWARD, JVX_SIZE_UNSELECTED),
				JVX_SIZE_UNSELECTED, JVX_ID_TRIGGER_FORWARD),
			this);
		// ioBaresip.print(std::cout);
		CayfAutomationModuleHandler::CayfAutomationModuleHandler::register_element(&ioBaresipAudio);

		ioBaresipVideo.activate(_common_set.theReport, refHostRef, this, JVX_PURPOSE_BARESIPIO_VIDEO,

			// master="default":ocon="default" --> <first elements> --> "mix-out" --> ... -> default
			CayfAutomationModules::ayfConnectConfigSrc2SnkPreChain(

				// This is for the first part of the chain: default --> ... --> mix-in
				"video",
				{
				},
				"mix-in", jvxComponentIdentification(JVX_COMPONENT_VIDEO_NODE, 0, 0),
				CayfAutomationModules::ayfConnectConfigSrc2Snk("baresipVideo#",


					"video",
					// This is for the second part of the chain! SP<2>-mix-out --> ... --> tp_active-default
					"mix-out",
					{
					},
					"video",
					jvxComponentIdentification(JVX_COMPONENT_VIDEO_NODE, 0, 0),
					CayfAutomationModules::ayfConnectConfigConMiscArgs(JVX_PURPOSE_BARESIPIO_VIDEO, true),
					JVX_ID_TRIGGER_FORWARD, JVX_SIZE_UNSELECTED),
					JVX_SIZE_UNSELECTED, JVX_ID_TRIGGER_FORWARD),
			this);
		// ioBaresip.print(std::cout);
		CayfAutomationModuleHandler::CayfAutomationModuleHandler::register_element(&ioBaresipVideo);

		// ===================================================================================================

		iSources.activate(_common_set.theReport, refHostRef, this, JVX_PURPOSE_INPUT,
			CayfAutomationModules::ayfConnectConfigSrc2Snk("AudioFileINPUT#",


				// SOURCE Master name, SOURCE output connector name, SOURCE input connector name
				"default",
				"default",
				{
					CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNBitstreamDecoder", "default", "default",
						CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE)),
					CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNConvert", "default", "default",
					CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE)),
					CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNForwardBuffer", "default", "default",
					CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, "- input")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, "jvxSpNLevelControl", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, " - input"))/* */
				},
				"mix-in",
				jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
				CayfAutomationModules::ayfConnectConfigConMiscArgs(JVX_CONNECTION_CATEGORY_FILEREADER_AUDIO, false),
				JVX_SIZE_UNSELECTED, JVX_ID_TRIGGER_FORWARD),
			this);
		// iSources.print(std::cout);
		CayfAutomationModuleHandler::CayfAutomationModuleHandler::register_element(&iSources);

		// ===================================================================================================

		// Here, we are configuring the output streams for files (ffmpeg)
		oSinks.activate(_common_set.theReport, refHostRef, this, JVX_PURPOSE_OUTPUT,
			CayfAutomationModules::ayfConnectConfigSrc2SnkPreChain(

				// This declares the second part - the main chain
				"default",
				{
				},
				"mix-in",
				jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 0, 0),
				CayfAutomationModules::ayfConnectConfigSrc2Snk("AudioFileOUTPUT#",

				"default",
				"mix-out",
					{

						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNForwardBuffer", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, "- output")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNConvert", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, "- output")),
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_AUDIO_NODE, "jvxAuNBitstreamEncoder", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE))
					},
					"default",
					jvxComponentIdentification(JVX_COMPONENT_SIGNAL_PROCESSING_NODE, 1, 0),
					CayfAutomationModules::ayfConnectConfigConMiscArgs(JVX_CONNECTION_CATEGORY_FILEWRITER_AUDIO, false),
					JVX_ID_TRIGGER_FORWARD, JVX_SIZE_UNSELECTED),
				JVX_SIZE_UNSELECTED, JVX_ID_TRIGGER_FORWARD),
			this);
		oSinks.print(std::cout);
		CayfAutomationModuleHandler::CayfAutomationModuleHandler::register_element(&oSinks);
	
		// ===================================================================================================

		ioVideo.activate(_common_set.theReport, refHostRef, this, JVX_PURPOSE_VIDEO_IO,
			CayfAutomationModules::ayfConnectConfigSrc2SnkPreChain(

				// This declares the second part - the main chain
				"default",
				{

					CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_VIDEO_NODE, "ayfViNCameraConvert", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, ""))					
					
				},
				"video",
				jvxComponentIdentification(JVX_COMPONENT_VIDEO_NODE, 0, 0),
				CayfAutomationModules::ayfConnectConfigSrc2Snk("VideoIO#",

				"default",
				"video",
					{
						CayfAutomationModules::ayfConnectConfigCpEntry(JVX_COMPONENT_VIDEO_NODE, "jvxViNOpenGLViewer", "default", "default",
							CayfAutomationModules::ayfConnectConfigCpManipulate(JVX_COMPONENT_DYNAMIC_NODE, false, ""))						
					},
				"default",
				jvxComponentIdentification(JVX_COMPONENT_VIDEO_NODE, 0, 0),
				CayfAutomationModules::ayfConnectConfigConMiscArgs(JVX_CONNECTION_CATEGORY_VIDEO_IO, false), JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED, true)),
				this);
		ioVideo.print(std::cout);
		CayfAutomationModuleHandler::CayfAutomationModuleHandler::register_element(&ioVideo);

		// ===================================================================================================

		genATSimplePhone::init_all();
		genATSimplePhone::allocate_all();
		genATSimplePhone::register_all(this);
	}
	return res;
}

jvxErrorType
CayfATStarter::deactivate()
{
	jvxErrorType res = CjvxAutomationReportConnect::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genATSimplePhone::unregister_all(this);
		genATSimplePhone::deallocate_all();

		CayfAutomationModuleHandler::CayfAutomationModuleHandler::unregister_element(&ioAudioWithModules);
		ioAudioWithModules.deactivate();

		CayfAutomationModuleHandler::CayfAutomationModuleHandler::unregister_element(&ioBaresipAudio);
		ioBaresipAudio.deactivate();

		CayfAutomationModuleHandler::CayfAutomationModuleHandler::unregister_element(&ioBaresipVideo);
		ioBaresipVideo.deactivate();

		CayfAutomationModuleHandler::CayfAutomationModuleHandler::unregister_element(&iSources);
		iSources.deactivate();

		CayfAutomationModuleHandler::CayfAutomationModuleHandler::unregister_element(&oSinks);
		oSinks.deactivate();

		CjvxAutomationReportConnect::deactivate();
	}
	return res;
}

// ==========================================================================================================

jvxErrorType
CayfATStarter::handle_report_ss(
	jvxReportCommandRequest req,
	jvxComponentIdentification tp,
	jvxStateSwitch ss,
	CjvxAutomationReport::callSpecificParameters* params)
{
	IjvxObject* theObj = nullptr;
	IjvxProperties* props = nullptr;
	IjvxManipulate* manIf = nullptr;
	jvxVariant val;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxApiString astr;
	jvxSelectionList sel;
	jPROSL raw(sel);

	jvxSize numChannels = 12;
	jvxBool switchMixer = false;

	// Audio input l&R + file input L&R -> audio out L&R
	jvxApiString astrMixerTalkthrough = "[;;0 0 1 0 0 0 1; 0 0 0 1 0 0 0 1;;;0 0 1 0 0 0 1; 0 0 0 1 0 0 0 1]";

	// Phone input l&R -> audio out L&R -- Audio in L&R + file input L&R -> phone out L&R
	jvxApiString astrMixerPhone = "[;;0 0 0 0 1; 0 0 0 0 0 1; 0 0 1 0 0 0 1; 0 0 0 1 0 0 0 1;0 0 0 0 1; 0 0 0 0 0 1 ]";

	// -> INPUT [    A  B       C  D       E    F       G   H  ]
	//           < primary -< audio in >-< phone in >-< file i >-
	// -> OUTPUT [    A  B       C  D        E    F        G   H  ]
	//           < primary -< audio out >-< phone out >-< file out >-

	// !!!
	// We need to be a little bit careful here: the automation component may be triggered before state has been 
	// set to SELECTED. That is the case because the system automation reference is selected very early and
	// is released very late. It is then involved even on selection/unselection of the automation component itself!!
	// !!!

	if (refHostRef)
	{
		refHostRef->request_hidden_interface_selected_component(tp, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&props));
		refHostRef->request_hidden_interface_selected_component(tp, JVX_INTERFACE_MANIPULATE, reinterpret_cast<jvxHandle**>(&manIf));
	}

	if (ss == JVX_STATE_SWITCH_SELECT)
	{
		switch (tp.tp)
		{
		case JVX_COMPONENT_AUDIO_NODE:
			switch (tp.slotid)
			{
			case 0:

				propsMixerNode = props;
				switchMixer = true;
				break;
			}
			break;
		case JVX_COMPONENT_AUDIO_TECHNOLOGY:
			switch (tp.slotid)
			{
			case 0:
				if (props)
				{
					std::string searchToken = "no-specific";
					searchToken = "*Clock Sync*";
					ident.reset("/JVX_GENW_TECHNOLOGIES");
					resL = props->get_property(callGate, raw, ident);
					if (resL == JVX_NO_ERROR)
					{
						jvx_bitFClear(sel.bitFieldSelected());
						for (int i = 0; i < sel.strList.ll(); i++)
						{
							if (jvx_compareStringsWildcard(searchToken, sel.strList.std_str_at(i)))
							{
								jvx_bitZSet(sel.bitFieldSelected(), i);
								break;
							}
						}
						if (jvx_bitFieldValue32(sel.bitFieldSelected()))
						{
							props->set_property(callGate, raw, ident);
						}
					}

					ident.reset("/triggerActivateDefaultDevice");
					resL = props->set_property(callGate, jPRI<jvxCBool>(c_true), ident);
				}
				break;
			case 1:
				if (props)
				{
					std::string searchToken = "no-specific";

#ifdef JVX_AUDIO_TECHNOLOGY_SEARCH_TOKEN
					searchToken = JVX_AUDIO_TECHNOLOGY_SEARCH_TOKEN;
#endif

					std::cout << " ## Trying to activate technology <"  << searchToken << std::endl;

					ident.reset("/JVX_GENW_TECHNOLOGIES");
					resL = props->get_property(callGate, raw, ident);
					if (resL == JVX_NO_ERROR)
					{
						jvx_bitFClear(sel.bitFieldSelected());
						for (int i = 0; i < sel.strList.ll(); i++)
						{
							std::string token = sel.strList.std_str_at(i);
							std::cout << "Checking " << searchToken << " vs. " << token << "." << std::endl;
							if (jvx_compareStringsWildcard(searchToken, token))
							{
								std::cout << "-> Found it!!" << std::endl;
								jvx_bitZSet(sel.bitFieldSelected(), i);
								break;
							}
						}
						if (jvx_bitFieldValue32(sel.bitFieldSelected()))
						{
							props->set_property(callGate, raw, ident);
						}
					}
				}
				break;			
			}
			break;

		case JVX_COMPONENT_AUDIO_DEVICE:
			switch (tp.slotid)
			{
			case 2:
				// Here, we see a new sip device
				phoneConnections++;
				switchMixer = true;
				break;
			}
			break;		

		case JVX_COMPONENT_SIGNAL_PROCESSING_NODE:

			switch (tp.slotid)
			{
			case 0:

				// This is the MixChain input component!
				if (manIf)
				{
					astr = "MixChain - Input";
					val.assign(&astr);
					resL = manIf->set_manipulate_value(JVX_MANIPULATE_DESCRIPTION, &val);
				}

				ident.reset("/number_channels_side");
				resL = props->set_property(callGate, jPRIO<jvxSize>(numChannels), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				jvx_bitZSet(sel.bitFieldSelected(), 0); // <- Input
				ident.reset("/operation_mode");
				resL = props->set_property(callGate, jPROSL(sel), ident, jPD(true));
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the operation mode to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				ident.reset("/one_new_entry");
				astr = "JVX_COMPONENT_AUDIO_DEVICE<1\",\"" + jvx_size2String(JVX_SIZE_DONTCARE) + ">, 0, 2,\"Audio Input #\",yes";
				resL = props->set_property(callGate, jPROS(&astr), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				// For the phone, we need to decouple input from output. Otherwise, SIP i/o may stop if we do not deliver output data
				// The possible options here depend on the settings at this position: line 787, property /out_fail_no_report
				// If we do not remove the error and we link i/o, no output data will be written at the begining.
				// In this case, the baresip module will not output any data after init!!
				ident.reset("/one_new_entry");
				astr = "JVX_COMPONENT_AUDIO_DEVICE<2\",\"" + jvx_size2String(JVX_SIZE_DONTCARE) + ">, 2, 2,\"Phone Input #\",no";
				resL = props->set_property(callGate, jPROS(&astr), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				ident.reset("/one_new_entry");
				astr = "JVX_COMPONENT_AUDIO_DEVICE<3\",\"" + jvx_size2String(JVX_SIZE_DONTCARE) + ">, 4, 2,\"File Input #\",no";
				resL = props->set_property(callGate, jPROS(&astr), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				break;

			case 1:

				// This is the MixChain output component!					
				if (manIf)
				{
					astr = "MixChain - Output";
					val.assign(&astr);
					resL = manIf->set_manipulate_value(JVX_MANIPULATE_DESCRIPTION, &val);
				}

				ident.reset("/number_channels_side");
				resL = props->set_property(callGate, jPRIO<jvxSize>(numChannels), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				jvx_bitZSet(sel.bitFieldSelected(), 1); // <- Output
				ident.reset("/operation_mode");
				resL = props->set_property(callGate, jPROSL(sel), ident, jPD(true));
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the operation mode to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				ident.reset("/one_new_entry");
				astr = "JVX_COMPONENT_AUDIO_DEVICE<1\",\"" + jvx_size2String(JVX_SIZE_DONTCARE) + ">, 0, 2,\"Audio Output #\"";
				resL = props->set_property(callGate, jPROS(&astr), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				ident.reset("/one_new_entry");
				astr = "JVX_COMPONENT_AUDIO_DEVICE<2\",\"" + jvx_size2String(JVX_SIZE_DONTCARE) + ">, 2, 2,\"Phone Output #\"";
				resL = props->set_property(callGate, jPROS(&astr), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				ident.reset("/one_new_entry");
				astr = "JVX_COMPONENT_AUDIO_DEVICE<4\",\"" + jvx_size2String(JVX_SIZE_DONTCARE) + ">, 4, 2,\"File Output #\"";
				resL = props->set_property(callGate, jPROS(&astr), ident);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << __FUNCTION__ << " - On selection of <" << jvxComponentIdentification_txt(tp) << ">, setting the number channels to " <<
						astr.std_str() << " failed, reason: <" << jvxErrorType_descr(resL) << ">." << std::endl;
					JVX_STOP_LOCK_LOG(JVX_CREATE_CODE_LOCATION_TAG);
				}

				break;
			}
			break;
		}
		
		if (props)
		{			
			refHostRef->return_hidden_interface_selected_component(tp, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(props));			
		}
		
		if (manIf)
		{
			refHostRef->return_hidden_interface_selected_component(tp, JVX_INTERFACE_MANIPULATE, reinterpret_cast<jvxHandle*>(manIf));
		}		
	}
	else if (ss == JVX_STATE_SWITCH_UNSELECT)
	{
		switch (tp.tp)
		{
		case JVX_COMPONENT_AUDIO_NODE:
			switch (tp.slotid)
			{
			case 0:
				propsMixerNode = nullptr;
				break;
			}
			break;

		case JVX_COMPONENT_AUDIO_DEVICE:
			switch (tp.slotid)
			{
			case 2:
				// Here, we see a new sip device
				phoneConnections--;
				switchMixer = true;
				break;
			}
			break;
		}

		// !!!
		// Again here, note that this callback may happen very late. The very last report we see here is the
		// unselection of the host, indeed. The automation is already unselected
		if (refHostRef)
		{
			refHostRef->request_hidden_interface_selected_component(tp, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&props));
			if (props)
			{
				refHostRef->return_hidden_interface_selected_component(tp, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(props));
			} // if (props)
		}
	} // else if (ss == JVX_STATE_SWITCH_UNSELECT)

	if (switchMixer)
	{
		if (propsMixerNode)
		{
			if (phoneConnections > 0)
			{
				// jvxApiString astrMixer = "[;;0 0 0 0 1; 0 0 0 0 0 1 ]";
				propsMixerNode->set_property(callGate, jPRI<jvxApiString>(astrMixerPhone), jPAD("/user_input"), jPD());
			}
			else
			{
				// jvxApiString astrMixer = "[;;0 0 1; 0 0 0 1 ]";
				propsMixerNode->set_property(callGate, jPRI<jvxApiString>(astrMixerTalkthrough), jPAD("/user_input"), jPD());
			}
		}
	}

	return CjvxAutomationReportConnect::handle_report_ss(req, tp, ss, params);
}

jvxErrorType
CayfATStarter::adapt_single_property_on_event(jvxSize purposeId,
	const std::string& nmChain, const std::string& modName,
	const std::string& cpDescription, jvxReportCommandRequest req, IjvxProperties* props)
{
	jvxCallManagerProperties cGate;
	jvxCBool valB = c_false;
	jvxSelectionList selLst;
	jvxSize numBufsVid = 2;

	jvxErrorType res = JVX_NO_ERROR;

	switch (req)
	{
	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED:
		switch (purposeId)
		{		
		case JVX_PURPOSE_AUDIO_IO:
			break;
		case JVX_PURPOSE_AUDIO_IO_WITH_MODULES:

			if (modName == "jvxAuNConvert")
			{
				if (cpDescription == "JVX Audio Convert - input")
				{
					ident.reset("/fixed_rate_location_mode");
					trans.reset(true);
					jvx_bitZSet(selLst.bitFieldSelected(), 1); // <- activate the mode "input fixed rate"
					props->set_property(cGate, jPRI<jvxSelectionList>(selLst), ident, trans);
				}
				else if (cpDescription == "JVX Audio Convert - output")
				{
					// Select the output buffer mode
					ident.reset("/fixed_rate_location_mode");
					trans.reset(true);
					jvx_bitZSet(selLst.bitFieldSelected(), 0); // <- activate the mode "input fixed rate"
					props->set_property(cGate, jPRI<jvxSelectionList>(selLst), ident, trans);
				}
			}
			if (modName == "jvxAuNForwardBuffer")
			{
				valB = c_false;
				res = jvx_set_property(props, &valB, 0, 1, JVX_DATAFORMAT_BOOL,
					true, "/bypass_buffer", cGate);

				if (cpDescription == "JVX Audio Forward Buffer - input")
				{
					// Select the input buffer mode
					jvx_bitZSet(selLst.bitFieldSelected(), 0);
					res = jvx_set_property(props, &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST,
						true, "/buffer_location", cGate);

					res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxSize>(genATSimplePhone::config_audio.buffersize_sync_min_in.value),
						jPAD("/buffersize_msecs"));

					if (genATSimplePhone::config_audio.buffersize_sync_profiling.value)
					{
						// Enable buffer measurement
						res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxCBool>(1),
							jPAD("/enable_buffer_profiling"));
					}

				}
				else if (cpDescription == "JVX Audio Forward Buffer - output")
				{
					// Select the output buffer mode
					jvx_bitZSet(selLst.bitFieldSelected(), 1);
					res = jvx_set_property(props, &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST,
						true, "/buffer_location", cGate);

					res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxSize>(genATSimplePhone::config_audio.buffersize_sync_min_out.value),
						jPAD("/buffersize_msecs"));

					if (genATSimplePhone::config_audio.buffersize_sync_profiling.value)
					{
						// Enable buffer measurement
						res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxCBool>(1),
							jPAD("/enable_buffer_profiling"));
					}
				}
			}
			if (modName == "jvxSpNSynchronize")
			{
			}
			break;

		case JVX_PURPOSE_BARESIPIO_AUDIO:			
			if (modName == "jvxAuNConvert")
			{
				if (cpDescription == "JVX Audio Convert - input")
				{
					ident.reset("/fixed_rate_location_mode");
					trans.reset(true);
					jvx_bitZSet(selLst.bitFieldSelected(), 1); // <- activate the mode "input fixed rate"
					props->set_property(cGate, jPRI<jvxSelectionList>(selLst), ident, trans);
				}
				else if (cpDescription == "JVX Audio Convert - output")
				{
					// Select the output buffer mode
					ident.reset("/fixed_rate_location_mode");
					trans.reset(true);
					jvx_bitZSet(selLst.bitFieldSelected(), 0); // <- activate the mode "input fixed rate"
					props->set_property(cGate, jPRI<jvxSelectionList>(selLst), ident, trans);
				}
			}

			if (modName == "jvxAuNForwardBuffer")
			{
				valB = c_false;
				res = jvx_set_property(props, &valB, 0, 1, JVX_DATAFORMAT_BOOL,
					true, "/bypass_buffer", cGate);

				if (cpDescription == "JVX Audio Forward Buffer - input")
				{
					// Select the input buffer mode
					jvx_bitZSet(selLst.bitFieldSelected(), 0);
					res = jvx_set_property(props, &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST,
						true, "/buffer_location", cGate);

					res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxSize>(genATSimplePhone::config_sip.buffersize_sync_min_in.value),
						jPAD("/buffersize_msecs"));

					if (genATSimplePhone::config_sip.buffersize_sync_profiling.value)
					{
						// Enable buffer measurement
						res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxCBool>(1),
							jPAD("/enable_buffer_profiling"));
					}

					// Setup the input buffer to report events where data is lost
					valB = c_false;
					res = jvx_set_property(props, &valB, 0, 1, JVX_DATAFORMAT_BOOL,
						true, "/out_fail_no_report", cGate);					
				}
				else if (cpDescription == "JVX Audio Forward Buffer - output")
				{
					// Select the output buffer mode
					jvx_bitZSet(selLst.bitFieldSelected(), 1);
					res = jvx_set_property(props, &selLst, 0, 1, JVX_DATAFORMAT_SELECTION_LIST,
						true, "/buffer_location", cGate);

					res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxSize>(genATSimplePhone::config_sip.buffersize_sync_min_out.value),
						jPAD("/buffersize_msecs"));

					if (genATSimplePhone::config_sip.buffersize_sync_profiling.value)
					{
						// Enable buffer measurement
						res = props->set_property(lval<jvxCallManagerProperties>(jvxCallManagerProperties()), jPRI<jvxCBool>(1),
							jPAD("/enable_buffer_profiling"));
					}
				}
			}
			break;

		case JVX_PURPOSE_INPUT:
			if (modName == "jvxAuNForwardBuffer")
			{
				valB = c_false;
				res = jvx_set_property(props, &valB, 0, 1, JVX_DATAFORMAT_BOOL,
					true, "/bypass_buffer", cGate);
			}
			else if (modName == "jvxAuNConvert")
			{
				ident.reset("/fixed_rate_location_mode");
				trans.reset(true);
				jvx_bitZSet(selLst.bitFieldSelected(), 2); // <- activate the mode "output fixed rate"
				props->set_property(cGate, jPRI<jvxSelectionList>(selLst), ident, trans);
			}
			break;

		case JVX_PURPOSE_OUTPUT:
			if (modName == "jvxAuNForwardBuffer")
			{
				valB = c_false;
				res = jvx_set_property(props, &valB, 0, 1, JVX_DATAFORMAT_BOOL,
					true, "/bypass_buffer", cGate);

				// Select the output buffer mode
				jvxSelectionList sel;
				jvx_bitZSet(sel.bitFieldSelected(), 1);
				res = jvx_set_property(props, &sel, 0, 1, JVX_DATAFORMAT_SELECTION_LIST,
					true, "/buffer_location", cGate);
			}
			else if (modName == "jvxAuNConvert")
			{
				ident.reset("/fixed_rate_location_mode");
				trans.reset(true);
				jvx_bitZSet(selLst.bitFieldSelected(), 0); // <- activate the mode "input fixed rate"
				props->set_property(cGate, jPRI<jvxSelectionList>(selLst), ident, trans);
			}
			break;

		case JVX_PURPOSE_VIDEO_IO:

			// Specify the rendering mode
			if (modName == "jvxViNOpenGLViewer")
			{
				// What to do here? Set the selection to option "0" - no rendering
				ident.reset("/rendering_mode");
				trans.reset(true);
				selLst.bitFieldSelected() = genATSimplePhone::config_video.device_render_mode.value.selection();
				props->set_property(cGate, jPRI<jvxSelectionList>(selLst), ident, trans);
				break;
			}

			if (modName == "jvxViNMixer")
			{
				// What to do here? Set the selection to option "0" - no rendering
				ident.reset("/number_buffers");
				trans.reset(true);
				numBufsVid = genATSimplePhone::config_video.num_render_buffers.value;				
				props->set_property(cGate, jPRI<jvxSize>(numBufsVid), ident, trans);
				break;
			}
		
			break;			
		case JVX_PURPOSE_BARESIPIO_VIDEO:

			break;

		default:
			assert(0);
		}
		break;
	}
	return res;
}

jvxErrorType
CayfATStarter::allow_master_connect(
	jvxSize purposeId,
	jvxComponentIdentification tpIdTrigger)
{
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	switch (purposeId)
	{
	case JVX_PURPOSE_AUDIO_IO:
		switch (tpIdTrigger.tp)
		{
		case JVX_COMPONENT_AUDIO_DEVICE:

			// In case of audio device slotid = 1:
			if (tpIdTrigger.slotid == 1)
			{
				res = JVX_NO_ERROR;
			}
			break;
		}		
		break;

	case JVX_PURPOSE_AUDIO_IO_WITH_MODULES:
		switch (tpIdTrigger.tp)
		{
		case JVX_COMPONENT_AUDIO_DEVICE:

			// In case of audio device slotid = 1:
			if (tpIdTrigger.slotid == 1)
			{
				res = JVX_NO_ERROR;
			}
			break;
		}
		break;

	case JVX_PURPOSE_BARESIPIO_AUDIO:
		switch (tpIdTrigger.tp)
		{
		case JVX_COMPONENT_AUDIO_DEVICE:

			// In case of audio device slotid = 2:
			if (tpIdTrigger.slotid == 2)
			{
				res = JVX_NO_ERROR;
			}
			break;
		}
		break;

	case JVX_PURPOSE_BARESIPIO_VIDEO:
		switch (tpIdTrigger.tp)
		{
		case JVX_COMPONENT_AUDIO_DEVICE:

			// In case of audio device slotid = 2:
			if (tpIdTrigger.slotid == 2)
			{
				res = JVX_NO_ERROR;
			}
			break;
		}
		break;

	case JVX_PURPOSE_INPUT:
		switch (tpIdTrigger.tp)
		{
		case JVX_COMPONENT_AUDIO_DEVICE:
		
			// In case of audio device slotid = 2:
			if (tpIdTrigger.slotid == 3)
			{
				res = JVX_NO_ERROR;
			}
			break;		
		}
		break;

	case JVX_PURPOSE_OUTPUT:
		switch (tpIdTrigger.tp)
		{
		case JVX_COMPONENT_AUDIO_DEVICE:

			// In case of audio device slotid = 2:
			if (tpIdTrigger.slotid == 4)
			{
				res = JVX_NO_ERROR;
			}
			break;
		}
		break;

	case JVX_PURPOSE_VIDEO_IO:
		switch (tpIdTrigger.tp)
		{
		case JVX_COMPONENT_VIDEO_DEVICE:

			// In case of audio device slotid = 2:
			if(
				(tpIdTrigger.slotid == 0) &&
				(tpIdTrigger.slotsubid == 0))
			{
				// only one video device
				res = JVX_NO_ERROR;
			}
			break;
		}
		break;
	}
	return res;
}

jvxErrorType
CayfATStarter::report_connection_factory_to_be_added(
	jvxComponentIdentification tp_activated,
	IjvxConnectorFactory* add)
{
	return CjvxAutomationReportConnect::report_connection_factory_to_be_added(
		tp_activated, add);
};

jvxErrorType
CayfATStarter::report_connection_factory_removed(
	jvxComponentIdentification tp_deactivated,
	IjvxConnectorFactory* toremove)
{
	return CjvxAutomationReportConnect::report_connection_factory_removed(
		tp_deactivated, toremove);
}

jvxErrorType
CayfATStarter::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = CjvxAutomationReport::request_command(request);
	const CjvxReportCommandRequest_uid* elm = nullptr;
	jvxSize uid = JVX_SIZE_UNSELECTED;
	if (res == JVX_NO_ERROR)
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE:

			// Connect the chains in case some connections could not be achieved during configuration
			CayfAutomationModules::CayfAutomationModuleHandler::report_configuration_done();
			break;

		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED:

			// Connect the chains in case some connections could not be achieved during configuration
			// CayfAutomationModules::CayfAutomationModuleHandler::report_configuration_done();
			
			elm = castCommandRequest<CjvxReportCommandRequest_uid>(request);
			elm->uid(&uid);
			CayfAutomationModules::CayfAutomationModuleHandler::report_to_be_disconnected(uid);
			break;
		}
	}
	return res;
}

jvxErrorType 
CayfATStarter::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_INTERFACE_PROPERTIES:

		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
		break;

	case JVX_INTERFACE_CONFIGURATION:

		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
		break;

	default:
		res = CjvxAutomationReportConnect::request_hidden_interface(tp, hdl);
	}
	return res;
}

jvxErrorType 
CayfATStarter::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_INTERFACE_CONFIGURATION:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		res = CjvxAutomationReportConnect::return_hidden_interface(tp, hdl);
	}
	return res;
}

jvxErrorType 
CayfATStarter::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genATSimplePhone::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfATStarter::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		genATSimplePhone::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}
