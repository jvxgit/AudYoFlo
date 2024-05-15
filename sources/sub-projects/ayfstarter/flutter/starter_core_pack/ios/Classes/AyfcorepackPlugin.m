#import "AyfcorepackPlugin.h"
#if __has_include(<ayfcorepack/ayfcorepack-Swift.h>)
#import <ayfcorepack/ayfcorepack-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "ayfcorepack-Swift.h"
#endif

@implementation AyfcorepackPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftAyfcorepackPlugin registerWithRegistrar:registrar];
}
@end
