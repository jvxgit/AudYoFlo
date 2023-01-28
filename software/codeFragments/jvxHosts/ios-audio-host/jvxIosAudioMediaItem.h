#import <MediaPlayer/MediaPlayer.h>
#import "jvx_dataformats.h"
#import "typedefs/TjvxTypes.h"

@interface jvxIosAudioMediaItem : NSObject
{
    
}
@property (strong, nonatomic) NSString* nmArtist;
@property (strong, nonatomic) NSString* nmTitle;
@property (strong, nonatomic) NSURL* fHandle;

@end
