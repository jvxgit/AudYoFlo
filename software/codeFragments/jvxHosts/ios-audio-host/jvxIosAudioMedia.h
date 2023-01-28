#import <MediaPlayer/MediaPlayer.h>
#import "jvx_dataformats.h"
#import "typedefs/TjvxTypes.h"

typedef enum
{
    JVX_MEDIA_ARTISTS,
    JVX_MEDIA_TITLE
} mediaFilterType;

@interface jvxIosAudioMedia : NSObject
{
    
}

@property (strong, nonatomic) NSMutableArray* allCurrentItems;
@property (strong, nonatomic) NSMutableArray* allCurrentArtists;

- (jvxErrorType) refreshMediaItems;
- (jvxErrorType) getItemsForArtist:(NSString*)artist
                        returnList:(NSMutableArray**) list;

@end
