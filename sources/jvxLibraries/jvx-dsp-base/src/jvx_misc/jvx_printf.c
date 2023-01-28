#include <stdio.h>

#include "jvx_misc/jvx_printf.h"

typedef struct
{
	the_jvx_printf_ext the_ext_prntf;
	void* the_ext_priv;
} jvx_settingPrintf;

int my_default_printf (void* priv, const char * txt )
{
    return(printf( "%s", txt));
}

static jvx_settingPrintf the_current_printf = { my_default_printf, NULL };

#define JVX_STATIC_BUF_SIZE_PRINTF 1024

int jvxprintf(const char * format, ...)
{
	int retVal = 0;
    va_list args;

#if 0   
    char* mybuf = NULL;

	va_start( args, format );
    int numAlloc = vsnprintf(NULL, 0, format, args);
    va_end( args );

#else
    int numAlloc = JVX_STATIC_BUF_SIZE_PRINTF-1;
    char mybuf[JVX_STATIC_BUF_SIZE_PRINTF] = { 0 };
#endif
    va_start( args, format );
#if 0 
    JVX_DSP_SAFE_ALLOCATE_FIELD_Z(mybuf, char, numAlloc+1);
#endif
    vsnprintf(mybuf, numAlloc, format, args);
    va_end( args );

    retVal = the_current_printf.the_ext_prntf(the_current_printf.the_ext_priv, mybuf);
    
#if 0 
    JVX_DSP_SAFE_DELETE_FIELD(mybuf);
#endif

    // printf("Hier2\n");

    return(retVal);
}


// ===================================================================

void jvx_set_printf(void* thePriv, the_jvx_printf_ext myPrintf)
{
	the_current_printf.the_ext_prntf = myPrintf;
	the_current_printf.the_ext_priv = thePriv;
}

void jvx_reset_printf()
{
	the_current_printf.the_ext_prntf = my_default_printf;
	the_current_printf.the_ext_priv = NULL;
}

