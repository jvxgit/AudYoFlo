#ifndef __JVX_H__
#define __JVX_H__

#define JVX_PROPERTY_ACCESS_CHECK_VERBOSE

// Platform extensions for multi-platform
#include "jvx_platform.h"

// Namespace definition (only if desired)
#include "jvx_ns.h"

// Global debug definitions
#include "jvx_debug.h"

// =============================================================
// TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS TYPEDEFS
// =============================================================

// System typedefs
#include "typedefs/TjvxTypes.h"

#ifdef __cplusplus
#include "typedefs/TjvxTypes_cpp.h"
#endif

JVX_INTERFACE IjvxObject;
JVX_INTERFACE IjvxGlobalInstance;

#include "typedefs/TjvxLinkDataDescriptor.h"
#include "typedefs/TjvxConnector.h"
#include "typedefs/TjvxTriggerConnector.h"

#include "helpers/HjvxCastMin.h"

// Some typedefs
#include "typedefs/TjvxConfigTokens.h"

#include "typedefs/TjvxProperties.h"
#include "typedefs/TjvxPropertyDescriptor.h"
#include "typedefs/TjvxPropertyAddress.h"
#include "typedefs/TjvxPropertyExternalBuffer.h"
#include "typedefs/TjvxPropertyRawPointer.h"
#include "typedefs/TjvxPropertyContainer.h"
#include "typedefs/TjvxPropertySpecifics.h"
#include "typedefs/TjvxPropertyCompact.h"

#include "typedefs/TjvxConfiglines.h"
#include "typedefs/TjvxBinaryFastDataControl.h"
#include "typedefs/TjvxEventLoop.h"
#include "typedefs/TjvxDataConnections.h"
#include "typedefs/TjvxHttpApi.h"
#include "typedefs/TjvxVariant.h"

// Some tool typedefs
#include "typedefs/tools/TjvxDataLogger.h"
#include "typedefs/tools/TjvxConfigProcessor.h"
#include "typedefs/tools/TjvxExternalCall.h"
#include "typedefs/tools/TjvxWebServer.h"
#include "typedefs/tools/TjvxPacketFilter.h"

#include "typedefs/tools/TjvxConnection.h"

#include "typedefs/tools/TjvxRs232.h"
#include "typedefs/tools/TjvxSocket.h"

#include "typedefs/tools/TjvxText2Speech.h"
#include "typedefs/tools/TjvxTextLog.h"

#include "typedefs/tools/TjvxLogRemoteHandler.h"

// Hidden interfaces
#include "typedefs/TjvxSequencer.h"
#include "typedefs/TjvxSequencerControl.h"
#include "typedefs/TjvxReport.h"
#include "typedefs/TjvxReportCommandRequest.h"

// Definition of function entry points for all components
#include "typedefs/TjvxAccess.h"

// Definition of Javox protocol for all remote data transfers
#include "typedefs/network/TjvxProtocol.h"

// Dependent tool definition
#include "typedefs/tools/TjvxRemoteCall.h"

#include "typedefs/property-extender/TjvxPropertyExtender.h"

// Local delegate link types for hosts
#include "typedefs/hosts/TjvxMainWindowControl.h"

// Finally some simplifying strings
#include "typedefs/TjvxTypes_str.h"

#include "typedefs/TjvxManipulate.h"

#include "helpers/HjvxCastMin.h"

// =============================================================
// INTERFACES INTERFACES INTERFACES INTERFACES INTERFACES INTERFACES
// =============================================================

#include "interfaces/IjvxCommandLine.h"

// Main System interfaces
#include "interfaces/IjvxInformation.h"
#include "interfaces/IjvxObject.h"
#include "interfaces/IjvxInterface.h"
#include "interfaces/IjvxHiddenInterface.h"
#include "interfaces/IjvxInterfaceFactory.h"
#include "interfaces/IjvxProperties.h"
#include "interfaces/IjvxSystemStatus.h"
#include "interfaces/IjvxBinaryFastDataControl.h"
#include "interfaces/IjvxPrintf.h"
#include "interfaces/IjvxGlobalInstance.h"
#include "interfaces/IjvxSchedule.h"
#include "interfaces/IjvxUniqueId.h"
#include "interfaces/IjvxPackage.h"

// Tool Interfaces
#include "interfaces/tools/IjvxDataLogger.h"
#include "interfaces/tools/IjvxDataLogReader.h"
#include "interfaces/tools/IjvxConnection.h"
#include "interfaces/tools/IjvxSectionOriginList.h"
#include "interfaces/tools/IjvxConfigProcessor.h"
#include "interfaces/tools/IjvxToolsHost.h"
#include "interfaces/tools/IjvxExternalCallTarget.h"
#include "interfaces/tools/IjvxExternalCall.h"
#include "interfaces/tools/IjvxThreadController.h"
#include "interfaces/tools/IjvxWebServer.h"
#include "interfaces/tools/IjvxPacketFilter.h"
#include "interfaces/tools/IjvxCrypt.h"
#include "interfaces/tools/IjvxText2Speech.h"
#include "interfaces/tools/IjvxSystemTextLog.h"
#include "interfaces/tools/IjvxLocalTextLog.h"
#include "interfaces/tools/IjvxThreads.h"
#include "interfaces/tools/IjvxSharedMemory.h"
#include "interfaces/tools/IjvxLogRemoteHandler.h"
#include "interfaces/tools/IjvxGlobalLock.h"


// Dependency for this header: IjvxRemoteCall uses IjvxConnection!!
#include "interfaces/tools/IjvxRemoteCall.h"

// Hidden interfaces
#include "interfaces/IjvxSequencer.h"
#include "interfaces/IjvxSequencerControl.h"
#include "interfaces/IjvxConfigurationLine.h"
#include "interfaces/IjvxPropertyPool.h"
#include "interfaces/IjvxHostTypeHandler.h"
#include "interfaces/IjvxReport.h"
#include "interfaces/IjvxReportOnConfig.h"
#include "interfaces/IjvxReportStateSwitch.h"
#include "interfaces/IjvxConfiguration.h"
#include "interfaces/IjvxConfigurationExtender.h"
#include "interfaces/IjvxEventLoop.h"
#include "interfaces/IjvxManipulate.h"
#include "interfaces/IjvxBootSteps.h"
#include "interfaces/IjvxCommandInterpreter.h"
#include "interfaces/IjvxPropertyAttach.h"
#include "interfaces/IjvxConfigurationAttach.h"

// Specific interfaces for event loop
#include "interfaces/console/IjvxEventLoop_backend_ctrl.h"
#include "interfaces/console/IjvxEventLoop_frontend_ctrl.h"

// Host interface
#include "interfaces/IjvxStateMachine.h"
#include "interfaces/IjvxExternalModuleFactory.h"
#include "interfaces/IjvxHostInteraction.h"
#include "interfaces/IjvxComponentHost.h"
#include "interfaces/IjvxFactoryHost.h"
#include "interfaces/IjvxHost.h"
#include "interfaces/IjvxConnectionIterator.h"
#include "interfaces/IjvxConnector.h"
#include "interfaces/IjvxTriggerConnector.h"
#include "interfaces/IjvxConnectionMaster.h"
#include "interfaces/IjvxEventLoopObject.h"
#include "interfaces/IjvxDataConnections.h"
#include "interfaces/IjvxHttpApi.h"

// Main Component types
#include "interfaces/IjvxSimpleNode.h"
#include "interfaces/IjvxMainComponent.h"
#include "interfaces/IjvxNode.h"
#include "interfaces/IjvxDevice.h"
#include "interfaces/IjvxTechnology.h"
#include "interfaces/IjvxSimpleComponent.h"

// Local delegate link interface for hosts
#include "interfaces/hosts/IjvxMainWindowControl.h"

// Scheduler component interface
#include "interfaces/IjvxScheduler.h"

// Introduce abstract interface for access to properties
#include "interfaces/properties_hosts/IjvxAccessProperties.h"

// Introduce abstract interface for access to properties
#include "interfaces/property-extender/IjvxPropertyExtender.h"

// Data chining interface
#include "interfaces/IjvxDataChainInterceptorReport.h"

// Additional special functions for system automation
#include "interfaces/IjvxAutoDataConnect.h"

// Extension of the component host to expose additional interface
#include "interfaces/IjvxComponentHostExt.h"

// =============================================================
// MORE TYPEDEFS MORE TYPEDEFS MORE TYPEDEFS MORE TYPEDEFS MORE TYPEDEFS
// =============================================================
// Access to components and interfaces (must be here as it requires definition on IjvxObject)
#include "typedefs/TjvxModuleOnStart.h"

#include "typedefs/TjvxAccess.h"

// =============================================================
// Cast functions
// =============================================================
#include "helpers/HjvxCast.h"

// =============================================================
// HELPERS HELPERS HELPERS HELPERS HELPERS HELPERS HELPERS HELPERS
// =============================================================

#include "jvx-helpers.h"

#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "jvx_product.h"
#endif

#include "helpers/HjvxCastProduct.h"

// Macros
#include "macros/MjvxMacros.h"

#endif
