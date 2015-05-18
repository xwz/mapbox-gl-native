//
//  MapDownloader.h
//  ios
//
//  Created by Wei Zhuo on 18/05/2015.
//
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface MapDownloader : NSObject

+ (NSString *)localMapDBFileForCity:(NSDictionary *)city;
+ (void)downloadMapForCity:(NSDictionary *)city;
+ (CLLocationCoordinate2D)southWestCorner:(NSDictionary *)city;
+ (CLLocationCoordinate2D)northEastCorner:(NSDictionary *)city;
+ (CLLocationCoordinate2D)cityCentre:(NSDictionary *)city;
@end
