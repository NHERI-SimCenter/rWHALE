// createEDP.cpp
// purpose - given a building, return an EVENT for the Haywired data.
// written: fmckenna

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include "csvparser.h"   // for parsing csv file
#include <jansson.h>     // for writing json
#include <nanoflann.hpp> // for searching for nearest point

#include <map>
#include <string>
#include <cstring>

using namespace nanoflann;

struct locations {
  locations():x(0),y(0) {}
  locations(std::string st,double a,double b):station(st),x(a),y(b) {}
  std::string station;
  double x;
  double y;
};


template <typename T>
struct PointCloud
{
  struct Point
  {
    Point(): stationTag(0),x(0.),y(0.) {}
    Point(int tag, T(a), T(b)): stationTag(tag),x(a),y(b) {}
    int stationTag;
    T  x,y;
  };
  
  std::vector<Point>  pts;
  
  inline size_t kdtree_get_point_count() const { return pts.size(); }
  
  inline T kdtree_distance(const T *p1, const size_t idx_p2,size_t /*size*/) const
  {
    const T d0=p1[0]-pts[idx_p2].x;
    const T d1=p1[1]-pts[idx_p2].y;
    return d0*d0+d1*d1;
  }
  
  inline T kdtree_get_pt(const size_t idx, int dim) const
  {
    if (dim==0) return pts[idx].x;
    else return pts[idx].y;
  }
  
  template <class BBOX>
  bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }
};

int main(int argc, char **argv) {

  const char *filenameBIM =0;
  const char *filenameEVENT =0;
  const char *filenameHFmeta =0;
  const char *pathSW4Results =0;

  bool getRV = false;

  // 
  // parse the inputs
  //

  int arg = 1;
  while (arg < argc) {
    if (strcmp(argv[arg], "-filenameBIM") ==0) {
      arg++;
      filenameBIM = argv[arg];
    }
    else if (strcmp(argv[arg], "-filenameEVENT") ==0) {
      arg++;
      filenameEVENT = argv[arg];
    }
    else if (strcmp(argv[arg], "-filenameHFmeta") ==0) {
      arg++;
      filenameHFmeta = argv[arg];
    }
    else if (strcmp(argv[arg], "-pathSW4results") ==0) {
      arg++;
      pathSW4Results = argv[arg];
    }
    else if (strcmp(argv[arg], "-getRV") ==0) {
      getRV = true;
    }
    arg++;
  }

  // check inputs all there
  if(pathSW4Results == 0 || filenameHFmeta == 0 || filenameBIM == 0 || filenameEVENT == 0) {
    std::cerr << "ERROR: Not all input args provided\n";
    exit(-1);
  }

  // if just asking for RV, this app adds none (just make sure file exists and contanis an empty RV section)
  if (getRV == false) {
    return 0;
  }







  std::map<int, locations> stationLocations;
  PointCloud<float> cloud;

  //
  // first parse the station file & put each station into the cloud of points
  //
  
  CsvParser *csvparser = CsvParser_new(filenameHFmeta, " ", 1);
  const CsvRow *header = CsvParser_getHeader(csvparser);
  
  if (header == NULL) {
        printf("%s\n", CsvParser_getErrorMessage(csvparser));
        return 1;
  }
  
  const char **headerFields = CsvParser_getFields(header);
  // for (int i = 0 ; i < CsvParser_getNumFields(header) ; i++) {
  //   printf("TITLE: %d %s\n", i, headerFields[i]);
  //}    

  CsvRow *row;
  
  int count = 0;
  while ((row = CsvParser_getRow(csvparser))) {
    const char **rowFields = CsvParser_getFields(row);

    //           for (int i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
    //             printf("FIELD: %s\n", rowFields[i]);
    //           }

    char *pEnd;
    std::string station(rowFields[0]);
    double x = strtod(rowFields[4],&pEnd);
    double y = strtod(rowFields[5],&pEnd);
    stationLocations[count]=locations(station,x,y);
    cloud.pts.resize(count+1);
    cloud.pts[count].stationTag = count;
    cloud.pts[count].x = x;
    cloud.pts[count].y = y;
    count++;
  }
  
  CsvParser_destroy(csvparser);

  //
  // now parse the bim file for the location and 
  //

  json_error_t error;
  json_t *root = json_load_file(filenameBIM, 0, &error);

  if(!root) {
    printf("ERROR reading BIM file: %s\n", filenameBIM);
  }

  json_t *GI = json_object_get(root,"GI");
  json_t *location = json_object_get(GI,"location");

  float buildingLoc[2];
  buildingLoc[0] = json_number_value(json_object_get(location,"latitude"));
  buildingLoc[1] = json_number_value(json_object_get(location,"longitude"));

  json_object_clear(root);  

  //
  // now find nearest point in the cloud
  //

  // build the kd tree
  typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PointCloud<float> >,
    PointCloud<float>,
    2
    > my_kd_tree_t;

  my_kd_tree_t   index(2, cloud, KDTreeSingleIndexAdaptorParams(10) );
  index.buildIndex();

  //
  // do a knn search to find nearest point
  //

  long unsigned int num_results = 1;
  size_t ret_index;
  float out_dist_sqr;
  nanoflann::KNNResultSet<float> resultSet(num_results);
  resultSet.init(&ret_index, &out_dist_sqr);
  index.findNeighbors(resultSet, &buildingLoc[0], nanoflann::SearchParams(10));

  // 
  // create the event
  //

  int stationTag = ret_index;

  std::map<int, locations>::iterator stationIter;

  stationIter = stationLocations.find(stationTag);
  std::string stationName;

  if (stationIter != stationLocations.end()) {
    //std::cerr << stationIter->second.station;
    
    stationName = pathSW4Results + stationIter->second.station + ".json";
    //    stationIter->second.station + ".json";
    std::cerr << stationName;
  }

  //
  // add acceleration record at station to event array in events file
  //

  root = json_load_file(filenameEVENT, 0, &error);
  json_t *eventsArray;

  if(!root) {

    root = json_object();    
    eventsArray = json_array();    
    json_object_set(root,"Events",eventsArray);
    json_t *rvArray=json_array();    
    json_object_set(root,"RandomVariables",rvArray);

  } else {
    eventsArray = json_object_get(root,"Events");
  }

  json_t *newEvent = json_object();

  json_t *dataSW4 = json_load_file(stationName.c_str(), 0, &error);

  if(!dataSW4) {
    std::cerr << "THAT FAILED\n" << stationName;
  } else {

    json_t *timeSeriesArray;
    json_t *patternArray;
    
    timeSeriesArray = json_array();    
    patternArray = json_array();    
    
    json_t *dt  = json_object_get(dataSW4,"dT");
    json_t *name  = json_object_get(dataSW4,"name");
    json_t *dataX = json_object_get(dataSW4,"data_x");
    json_t *dataY = json_object_get(dataSW4,"data_y");
    json_t *dataZ = json_object_get(dataSW4,"data_z");

    json_object_set(newEvent,"name",name);    
    json_object_set(newEvent,"type",json_string("Seismic"));
    json_object_set(newEvent,"dT",dt);
    int numSteps = json_array_size(dataX);
    json_object_set(newEvent,"numSteps",json_integer(numSteps));

    json_t *seriesX = json_object();
    json_object_set(seriesX,"name",json_string("accel_X"));
    json_object_set(seriesX,"type",json_string("Value"));
    json_object_set(seriesX,"dT",dt);
    json_object_set(seriesX,"data",dataX);
    json_array_append(timeSeriesArray, seriesX);

    json_t *seriesY = json_object();
    json_object_set(seriesY,"name",json_string("accel_Y"));
    json_object_set(seriesY,"type",json_string("Value"));
    json_object_set(seriesY,"data",dataY);
    json_object_set(seriesY,"dT",dt);
    json_array_append(timeSeriesArray, seriesY);

    json_t *patternX = json_object();
    json_object_set(patternX,"type",json_string("UniformAcceleration"));
    json_object_set(patternX,"timeSeries",json_string("accel_X"));
    json_object_set(patternX,"dof",json_integer(1));
    json_array_append(patternArray, patternX);

    json_t *patternY = json_object();
    json_object_set(patternY,"type",json_string("UniformAcceleration"));
    json_object_set(patternY,"timeSeries",json_string("accel_Y"));
    json_object_set(patternY,"dof",json_integer(2));
    json_array_append(patternArray, patternY);

    json_object_set(newEvent,"timeSeries",timeSeriesArray);
    json_object_set(newEvent,"pattern",patternArray);

    json_array_append(eventsArray, newEvent);
  }

  json_dump_file(root,filenameEVENT,0);
  json_object_clear(root);

  return 0;
}
