#pragma once

#ifndef __CJVXAPPLICATIONCONTROLLERNODE_H__
#define __CJVXAPPLICATIONCONTROLLERNODE_H__

#include "jvx.h"

#include "common/CjvxProperties.h"

class CjvxApplicationControllerNode : public IjvxNode, public CjvxObject
{
protected:


public:

	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
	JVX_CALLINGCONVENTION CjvxApplicationControllerNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxApplicationControllerNode();

	// ======================================================
	
#define JVX_STATE_MACHINE_DEFINE_READY
#define JVX_STATE_MACHINE_DEFINE_ACTIVATEDEACTIVATE
#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#undef JVX_STATE_MACHINE_DEFINE_ACTIVATEDEACTIVATE
#undef JVX_STATE_MACHINE_DEFINE_READY

#include "codeFragments/simplify/jvxObjects_simplify.h"

#include "codeFragments/simplify/jvxHiddenInterfaceFwd_simplify.h"

	//#include "codeFragments/simplify/jvxProperties_simplify.h"

};

#endif

