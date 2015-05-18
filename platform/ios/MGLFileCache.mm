#import "MGLFileCache.h"
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/storage/mbtiles_source.hpp>

@interface MGLFileCache ()

@property (nonatomic) MGLFileCache *sharedInstance;
@property (nonatomic) mbgl::FileCache *sharedCache;
@property (nonatomic) NSHashTable *retainers;

@end

@implementation MGLFileCache

const std::string &fileInCacheDirectory(NSString *filename) {
    static const std::string path = [filename]() -> std::string {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        if ([paths count] == 0) {
            // Disable the cache if we don't have a location to write.
            return "";
        }

        NSString *libraryDirectory = [paths objectAtIndex:0];
        return [[libraryDirectory stringByAppendingPathComponent:filename] UTF8String];
    }();
    return path;
}

const std::string &fileInBundle(NSString *filename) {
  static const std::string path = [filename]() -> std::string {
    NSURL *res = [[NSBundle mainBundle] URLForResource:filename withExtension:nil];
    return [[res absoluteString] UTF8String];
  }();
  return path;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _retainers = [NSHashTable weakObjectsHashTable];
    }
    return self;
}

+ (instancetype)sharedInstance {
    static dispatch_once_t onceToken;
    static MGLFileCache *_sharedInstance;
    dispatch_once(&onceToken, ^{
        _sharedInstance = [[self alloc] init];
    });
    return _sharedInstance;
}

- (void)teardown {
    if (self.sharedCache) {
        delete self.sharedCache;
        self.sharedCache = nullptr;
    }
}

- (void)dealloc {
    [self.retainers removeAllObjects];
    [self teardown];
}

+ (mbgl::FileCache *)obtainSharedMBTilesSource:(NSString *)db withObject:(NSObject *)object {
  return [[MGLFileCache sharedInstance] obtainSharedMBTilesSource:db withObject:object];
}

- (mbgl::FileCache *)obtainSharedMBTilesSource:(NSString *)db withObject:(NSObject *)object {
  assert([[NSThread currentThread] isMainThread]);
  if (!self.sharedCache) {
    self.sharedCache = new mbgl::MBTilesSource([db UTF8String]);
  }
  [self.retainers addObject:object];
  return self.sharedCache;
}

+ (mbgl::FileCache *)obtainSharedCacheWithObject:(NSObject *)object {
  return [[MGLFileCache sharedInstance] obtainSharedCache:@"cache.db" withObject:object];
}

- (mbgl::FileCache *)obtainSharedCache:(NSString *)db withObject:(NSObject *)object {
    assert([[NSThread currentThread] isMainThread]);
    if (!self.sharedCache) {
        self.sharedCache = new mbgl::SQLiteCache(fileInCacheDirectory(db));
    }
    [self.retainers addObject:object];
    return self.sharedCache;
}

+ (void)releaseSharedCacheForObject:(NSObject *)object {
    return [[MGLFileCache sharedInstance] releaseSharedCacheForObject:object];
}

- (void)releaseSharedCacheForObject:(NSObject *)object {
    assert([[NSThread currentThread] isMainThread]);
    if ([self.retainers containsObject:object]) {
        [self.retainers removeObject:object];
    }
    if ([self.retainers count] == 0) {
        [self teardown];
    }
}

@end
