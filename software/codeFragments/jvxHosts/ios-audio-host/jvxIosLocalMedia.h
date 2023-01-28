#import "jvx_dataformats.h"
#import "typedefs/TjvxTypes.h"
#import <Foundation/NSObject.h>
#import <Foundation/NSString.h>
#import <Foundation/Foundation.h>

@interface jvxIosLocalMedia : NSObject
{

}

@property (strong, nonatomic) NSMutableArray* allCurrentItems;
@property (strong, nonatomic) NSMutableArray* allCurrentArtists;

- (jvxErrorType) getItemsForArtist:(NSString*)artist
                        returnList:(NSMutableArray**) list;

@end
