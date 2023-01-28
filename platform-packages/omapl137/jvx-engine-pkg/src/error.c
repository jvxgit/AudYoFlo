/*
 * ==============================================
 * Filename: error.c
 * ==============================================
 * Purpose: Central place where to reach in case a fatal error has been
 *  		detected. Note that assert(0) leads to a lot of problems since
 *			it does not work in a HWI/SWI interrupt.
 *			COPYRIGHT HK, Javox, 2011
 */
 
#include "projectAcfg.h"
#include <std.h>
#include <swi.h>
#include <c62.h>
#include <clk.h>

 // Function to reach once an unexpected error has occurred

// ===========================================================
// List of error IDs:
// 0: no error
// 1: The required size of the incoming package is higher than the static field used
// 2: RS 232 module is in wrong state
// 10: Not enough mem space in parameter input list field
// 11: Not enough mem space in parameter output list field
// 12: Not enough mem space in content parameter list input
// 13: Not enough mem space in content input field
// 14: Not enough mem space in content parameter list output
// 15: Not enough mem space in content output field
void
error_Report(int errId)
{
	LOG_printf (&trace, "Error in remote calls: ID is %d\n", errId);

	// What to do here?
	HWI_disable();
	SWI_disable();
	while(1)
	{
		// Hang..
	}
}

// =========================================================
