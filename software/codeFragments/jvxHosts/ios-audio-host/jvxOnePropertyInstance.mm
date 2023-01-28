#import "jvxOnePropertyInstance.h"
#import "jvxIosCppHost.h"

@interface jvxOnePropertyInstance()

@end

@implementation jvxOnePropertyInstance

- (BOOL)allocate_me:(jvxComponentType)tp
        withHostRef:(void*)hostRef;
{
    jvxErrorType res = JVX_NO_ERROR;
    self.cp_type = tp;
    JVX_SAFE_NEW_OBJ(_ref_props, CjvxPropertiesSimple);
    jvxIosCppHost* theHostIf = (jvxIosCppHost*)hostRef;
    IjvxHost* theHost = theHostIf->get_host_ref();
    assert(theHost);
    res = _ref_props->init_properties(theHost, tp);
    assert(res == JVX_NO_ERROR);
    return(TRUE);
    
}
- (BOOL)deallocate_me
{
    jvxErrorType res = JVX_NO_ERROR;
    res = _ref_props->terminate_properties();
    assert(res == JVX_NO_ERROR);
    return(TRUE);
}

@end
