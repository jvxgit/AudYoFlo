/*============================================================
 * FILE: dllMain.cpp
 *============================================================
 * PURPOSE: 
 *			Example algorithm component to demonstrate the use of the file writer library.
 *			This example is based on a dynamic library.
 *
 *			For a detailled description, refer to the rtprocdoc.pdf document.
 *
 *============================================================
 * COPYRIGHT/AUTHOR:	IND, RWTH-Aachen, Hauke Krueger, hauke@ind.rwth-aachen.de 
 *============================================================
 * For bugs, write an email to audiodev@ind.rwth-aachen.de
 *============================================================*/ 

#include <vector>

#include "CjvxThreadController.h"




#define COMPONENT_DESCRIPTION "JVX thread control module"
#define COMPONENT_TYPE CjvxThreadController
#define _JVX_ALLOW_MULTIPLE_INSTANCES

#include "templates/targets/factory_IjvxObject.cpp"

