#import "jvxIosAudioMedia.h"
#import "jvxIosAudioMediaItem.h"
// ========================================

@interface jvxIosAudioMedia ()

@end

@implementation jvxIosAudioMedia

- (id) init
{
    _allCurrentItems = [[NSMutableArray alloc] init];
    _allCurrentArtists = [[NSMutableArray alloc] init];
    return(self);
}

- (jvxErrorType) refreshMediaItems
{
    jvxErrorType res = JVX_NO_ERROR;
    int i;
    NSString* last_artist = NULL;
    
    [_allCurrentItems removeAllObjects];
    [_allCurrentArtists removeAllObjects];
    
    MPMediaQuery* theMedia = [MPMediaQuery artistsQuery];
    if(theMedia)
    {
        NSArray<MPMediaItem*> *theItemsOnPhone = theMedia.items;
        for(i = 0; i < theItemsOnPhone.count;i++)
        {
            MPMediaItem* oneItem = [theItemsOnPhone objectAtIndex:i];
            
            jvxIosAudioMediaItem* newItem = [[jvxIosAudioMediaItem alloc] init];
            
            NSURL* theUrl = [oneItem valueForProperty: MPMediaItemPropertyAssetURL];
            if(theUrl)
            {
                newItem.fHandle = theUrl;

                NSString* artist = [oneItem valueForKey:MPMediaItemPropertyArtist];
                if(artist == NULL)
                {
                    artist = @"Unknown";
                }
                
                newItem.nmArtist = artist;
                
                // ===================================
                // Store the different artists
                // ===================================
                if(last_artist == NULL)
                {
                    [_allCurrentArtists addObject:artist];
                }
                else
                {
                    if(![artist isEqualToString:last_artist])
                    {
                        [_allCurrentArtists addObject:artist];
                    }
                }
                last_artist = artist;
                // ===================================
                // ===================================

                
                NSString* title = [oneItem valueForKey:MPMediaItemPropertyTitle];
                if(title == NULL)
                {
                    title = @"Unknown";
                }
                newItem.nmTitle = title;
                //NSLog(@"Asset #%i: %@ :: %@ ", i, artist, title);
                
                [_allCurrentItems addObject:newItem];
            }
            else
            {
                NSString* art = [oneItem valueForKey:MPMediaItemPropertyArtist];
                NSString* tit = [oneItem valueForKey:MPMediaItemPropertyTitle];
                NSString* txt = [NSString stringWithFormat:@"Song <%@> <%@> is not playable.", art, tit];
                NSLog(@"%@", txt);
            }
        }
    }
    return(res);
}

- (jvxErrorType) getItemsForArtist:(NSString*)artist
           returnList:(NSMutableArray**) list
{
    int i;
    NSMutableArray* listCreate = [[NSMutableArray alloc] init];
    
    for(i = 0; i < _allCurrentItems.count; i++)
    {
        jvxIosAudioMediaItem* item = [_allCurrentItems objectAtIndex:i];
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

