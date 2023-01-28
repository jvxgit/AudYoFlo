#ifndef __JVX_REMOTE_CALL_LISTS_H__
#define __JVX_REMOTE_CALL_LISTS_H__

// the macros below help generating the remote call function mapping
#define RC_I(name) name##_paramsIn
#define RC_O(name) name##_paramsOut
// function parameter declaration macros
#define RC_I_DEF(name) const jvxRcProtocol_parameter_description RC_I(name)[]
#define RC_O_DEF(name) const jvxRcProtocol_parameter_description RC_O(name)[]
// number of argument macros
#define RC_NUM_I(name) (sizeof(RC_I(name))/sizeof(RC_I(name)[0]))
#define RC_NUM_O(name) (sizeof(RC_O(name))/sizeof(RC_O(name)[0]))
// function mapping macros
#define RC_MAP_IO(name) {#name, &name, RC_NUM_I(name), RC_I(name), RC_NUM_O(name), RC_O(name)}
#define RC_MAP_I(name) {#name, &name, RC_NUM_I(name), RC_I(name), 0, NULL}
#define RC_MAP_O(name) {#name, &name, 0, NULL, RC_NUM_O(name), RC_O(name)}
#define RC_MAP(name) {#name, &name, 0, NULL, 0, NULL}

typedef jvxDspBaseErrorType(*rc_function_wrapper)(jvxRCOneParameter* paramsIn, jvxRCOneParameter* paramsOut);


typedef struct {
    const char* description;
    rc_function_wrapper function;
    jvxSize numParamsIn;
    const jvxRcProtocol_parameter_description *paramsInDesc;
    jvxSize numParamsOut;
    const jvxRcProtocol_parameter_description *paramsOutDesc;
} rc_function_mapping;

// these variables implement the actual remote calls
extern const rc_function_mapping function_mapping[];
extern const jvxSize rc_function_mapping_length;
extern volatile int rc_disable_hwi_on_exec;

#endif
