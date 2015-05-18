//
//  CitiesTableViewController.m
//
//  Created by Wei Zhuo on 14/08/2014.
//

#import "CitiesTableViewController.h"
#import "MBXViewController.h"
#import "MapDownloader.h"

@interface CitiesTableViewController ()
@property NSArray *cities;
@property NSArray *data;
@property NSMutableDictionary *downloaded;
@end

@implementation CitiesTableViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  self.downloaded = [NSMutableDictionary new];
  self.cities = [self loadJSONFile:@"cities.js"];
  [self updateDownloadedCities];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateDownloadedCities)
                                               name:@"CityDownloaded"
                                             object:nil];
  [self reloadCities];
}

- (void)reloadCities {
  [self updateCitiesWithKeyword:@""];
}

- (void)updateDownloadedCities {
  NSFileManager *manager = [NSFileManager defaultManager];
  for (NSDictionary *city in self.cities) {
    BOOL downloaded = [manager fileExistsAtPath:[MapDownloader localMapDBFileForCity:city]];
    [self.downloaded setObject:[NSNumber numberWithBool:downloaded] forKey:city[@"Name"]];
  }
  [self.tableView reloadRowsAtIndexPaths:[self.tableView indexPathsForVisibleRows]
                        withRowAnimation:UITableViewRowAnimationNone];
}


- (void)updateCitiesWithKeyword:(NSString *)keyword {
  NSArray *cities = [NSMutableArray array];
  if ([keyword length] > 0) {
    NSPredicate *search = [NSPredicate predicateWithFormat:@"Name CONTAINS[cd] %@", keyword];
    cities = [self.cities filteredArrayUsingPredicate:search];
  } else {
    cities = self.cities;
  }
  NSMutableArray *downloaded = [NSMutableArray array];
  NSMutableArray *others = [NSMutableArray array];
  for (NSDictionary *city in cities) {
    if ([self.downloaded[city[@"Name"]] boolValue]) {
      [downloaded addObject:city];
    } else {
      [others addObject:city];
    }
  }
  NSMutableArray *data = [NSMutableArray arrayWithArray:downloaded];
  [data addObjectsFromArray:others];
  self.data = data;
  [self.tableView reloadData];
}

- (NSDictionary *)cityWithName:(NSString *)name fromCities:(NSArray *)cities {
  NSArray *result = [cities filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"(Name == %@)", name]];
  return [result firstObject];
}

- (id)loadJSONFile:(NSString *)filename {
  NSString *file = [[NSBundle mainBundle] pathForResource:filename ofType:nil];
  NSData *data = [[NSFileManager defaultManager] contentsAtPath:file];
  return [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
  return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  return [self.data count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"city" forIndexPath:indexPath];
  NSDictionary *city = [self.data objectAtIndex:indexPath.row];
  NSString *name = city[@"Name"];
  if ([self.downloaded[name] boolValue]) {
    cell.textLabel.text = [NSString stringWithFormat:@"✓ %@", name];
  } else {
    cell.textLabel.text = name;
  }
  return cell;
}

#pragma mark - Navigation
// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
  if ([segue.identifier isEqualToString:@"viewCity"]) {
    NSString *name = [[[(UITableViewCell *)sender textLabel] text] stringByReplacingOccurrencesOfString:@"✓ " withString:@""];
    NSDictionary *city = [self cityWithName:name fromCities:self.cities];
    MBXViewController *controller = [segue destinationViewController];
    controller.city = city;
  }
}

#pragma mark - Search bar

- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText {
  [self updateCitiesWithKeyword:searchText];
}

- (void)searchBarTextDidBeginEditing:(UISearchBar *)searchBar {
  searchBar.showsCancelButton = YES;
}

- (void)searchBarTextDidEndEditing:(UISearchBar *)searchBar {
  searchBar.showsCancelButton = NO;
}

- (void)searchBarCancelButtonClicked:(UISearchBar *)searchBar {
  searchBar.text = @"";
  [searchBar resignFirstResponder];
  [self reloadCities];
}

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
  [searchBar resignFirstResponder];
}

@end
