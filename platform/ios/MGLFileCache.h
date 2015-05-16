#import <Foundation/Foundation.h>

#include <mbgl/storage/sqlite_cache.hpp>

@interface MGLFileCache : NSObject

+ (mbgl::FileCache *)obtainSharedMBTilesSource:(NSString *)db withObject:(NSObject *)object;
+ (mbgl::FileCache *)obtainSharedCacheWithObject:(NSObject *)object;
+ (void)releaseSharedCacheForObject:(NSObject *)object;

@end
