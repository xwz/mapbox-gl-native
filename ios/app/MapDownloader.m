//
//  MapDownloader.m
//  ios
//
//  Created by Wei Zhuo on 18/05/2015.
//
//

#import "MapDownloader.h"
#import "AFNetworking.h"

@implementation MapDownloader


+ (NSString *)localMapDBFileForCity:(NSDictionary *)city {
  NSString *dbfile = [NSString stringWithFormat:@"map_%@.db", city[@"ID"]];
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *filePath = [[paths objectAtIndex:0] stringByAppendingPathComponent:dbfile];
  return filePath;
}

+ (void)downloadMapForCity:(NSDictionary *)city {
  NSString *url = [NSString stringWithFormat:@"http://stay:2015@beta.stay.com/vector/map_%@.db", city[@"ID"]];
  NSLog(@"Downloading %@", url);
  NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:url]];

  AFURLConnectionOperation *operation =   [[AFHTTPRequestOperation alloc] initWithRequest:request];
  NSString *filePath = [self localMapDBFileForCity:city];
  operation.outputStream = [NSOutputStream outputStreamToFileAtPath:filePath append:NO];
  NSString __block *ID = city[@"ID"];

  [operation setDownloadProgressBlock:^(NSUInteger bytesRead, long long totalBytesRead, long long totalBytesExpectedToRead) {
    float progress = (float)totalBytesRead / totalBytesExpectedToRead;
    NSDictionary *info = @{@"ID": ID, @"Progress": [NSNumber numberWithFloat:progress]};
    [[NSNotificationCenter defaultCenter] postNotificationName:@"DownloadProgress" object:info];
  }];

  [operation setCompletionBlock:^{
    NSLog(@"downloadComplete!");
    [[NSNotificationCenter defaultCenter] postNotificationName:@"CityDownloaded" object:city];
  }];
  [operation start];
}


+ (CLLocationCoordinate2D)southWestCorner:(NSDictionary *)city {
  return CLLocationCoordinate2DMake([city[@"South"] floatValue], [city[@"West"] floatValue]);
}

+ (CLLocationCoordinate2D)northEastCorner:(NSDictionary *)city {
  return CLLocationCoordinate2DMake([city[@"North"] floatValue], [city[@"East"] floatValue]);
}

+ (CLLocationCoordinate2D)cityCentre:(NSDictionary *)city {
  return CLLocationCoordinate2DMake([city[@"Latitude"] floatValue], [city[@"Longitude"] floatValue]);
}

@end
