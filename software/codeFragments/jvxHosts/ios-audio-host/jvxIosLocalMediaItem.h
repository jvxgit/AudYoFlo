#import "jvx_dataformats.h"
#import "typedefs/TjvxTypes.h"
#import <Foundation/NSObject.h>

@interface jvxIosLocalMediaItem : NSObject
{

}
@property (strong, nonatomic) NSString* nmArtist;
@property (strong, nonatomic) NSString* nmTitle;
@property (strong, nonatomic) NSString* nmDate;
@property (strong, nonatomic) NSString* nmPath;
@property (assign, nonatomic) jvxSize channels;
@property (assign, nonatomic) jvxInt32 rate;
@property (assign, nonatomic) jvxSize length;



@end
