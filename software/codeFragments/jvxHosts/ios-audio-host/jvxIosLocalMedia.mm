#import "jvxIosLocalMedia.h"
#import "jvxIosLocalMediaItem.h"
// ========================================

@interface jvxIosLocalMedia ()

@end

@implementation jvxIosLocalMedia

- (id) init
{
    _allCurrentItems = [[NSMutableArray alloc] init];
    _allCurrentArtists = [[NSMutableArray alloc] init];
    return(self);
}

- (jvxErrorType) getItemsForArtist:(NSString*)artist
           returnList:(NSMutableArray**) list
{
    int i;
    NSMutableArray* listCreate = [[NSMutableArray alloc] init];

    for(i = 0; i < _allCurrentItems.count; i++)
    {
        jvxIosLocalMediaItem* item = [_allCurrentItems objectAtIndex:i];
        if(artist)
        {
            if([item.nmArtist isEqualToString:artist])
            {
                [listCreate addObject:item];
            }
        }
    }
    if(list)
    {
        *list = listCreate;
    }
    return(JVX_NO_ERROR);
}

@end
