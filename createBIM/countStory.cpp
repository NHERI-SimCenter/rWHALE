// createBIM.c
// purpose - give the building.csv and parcel.csv files create a BIM model
// written: fmckenna

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "csvparser.h"
#include <jansson.h>
#include <map>
#include <sstream>
#include <fstream>
using namespace std;
json_t * deterStructtype(int year, int bldtypeid, int story);
const char* deteroccupancy(int building_type);
double replacementcost(int building_type);

struct locations {
  locations():x(0),y(0) {}
  locations(double a,double b):x(a),y(b) {}
  double x;
  double y;
};

const char* deteroccupancy(int building_type){
    const char * buildoccupancy;
    //double replacementcost;
    switch (building_type) {
    case 1:
        buildoccupancy="Residential";
        break;
    case 2:
        buildoccupancy="Residential";
        break;
    case 3:
        buildoccupancy="Residential";
        break;
    case 4:
        buildoccupancy="Office";
        break;
    case 5:
        buildoccupancy="Hotel";
        break;
    case 6:
        buildoccupancy="School";
        break;
    case 7:
        buildoccupancy="Industrial";
        break;
    case 8:
        buildoccupancy="Industrial";
        break;
    case 9:
        buildoccupancy="Industrial";
        break;
    case 10:
        buildoccupancy="Retail";
        break;
    case 11:
        buildoccupancy="Retail";
        break;
    case 12:
        buildoccupancy="Residential";
        break;
    case 13:
        buildoccupancy="Retail";
        break;
    case 14:
        buildoccupancy="Office";
        break;
    default:
        buildoccupancy="Residential";
        break;
    }
    return buildoccupancy;
}

double replacementcost(int building_type){
    //const char * buildoccupancy;
    double replacementcost;
    switch (building_type) {
    case 1:
        replacementcost=137.5452*(1+0.5);
        break;
    case 2:
        replacementcost=137.5452*(1+0.5);
        break;
    case 3:
        replacementcost=137.5452*(1+0.5);
        break;
    case 4:
        replacementcost=131.8863*(1+1);
        break;
    case 5:
        replacementcost=137.271225*(1+0.5);
        break;
    case 6:
        replacementcost=142.134265*(1+1.25);
        break;
    case 7:
        replacementcost=97.5247*(1+1.5);
        break;
    case 8:
        replacementcost=85.9586*(1+1.5);
        break;
    case 9:
        replacementcost=104.033475*(1+1.5);
        break;
    case 10:
        replacementcost=105.33705*(1+1);
        break;
    case 11:
        replacementcost=105.33705*(1+1);
        break;
    case 12:
        replacementcost=137.5452*(1+0.5);
        break;
    case 13:
        replacementcost=105.33705*(1+1);
        break;
    case 14:
        replacementcost=131.8863*(1+1);
        break;
    default:
        replacementcost=137.5452*(1+0.5);
        break;
    }
    return replacementcost;
}

json_t *deterStructtype(int year, int bldtypeid, int story)
{
    json_t *value=json_array();
    if(year<=1900){
        json_array_append(value, json_integer(1));
        json_array_append(value, json_integer(7));
        json_array_append(value, json_integer(8));
        json_array_append(value, json_integer(9));
    }
    else if(year>1900){
        if(story<4){
            if(bldtypeid==1||bldtypeid==2||bldtypeid==3||bldtypeid==12){
                json_array_append(value, json_integer(1));
            }
            else if(bldtypeid==4||bldtypeid==5||bldtypeid==6||bldtypeid==10||bldtypeid==11||bldtypeid==13||bldtypeid==14){
                json_array_append(value, json_integer(1));
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
                json_array_append(value, json_integer(6));
                json_array_append(value, json_integer(7));
                json_array_append(value, json_integer(8));
            }
            else if(bldtypeid<10&&bldtypeid>6){
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
                json_array_append(value, json_integer(6));
                json_array_append(value, json_integer(7));
                json_array_append(value, json_integer(8));
            }
            else {
                json_array_append(value, json_integer(1));
            }

        }
        else if(story>3&&story<8){
            if(bldtypeid<7&&bldtypeid>0||bldtypeid>9&&bldtypeid<15){
                json_array_append(value, json_integer(1));
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
            }
            else if(bldtypeid<10&&bldtypeid>6){
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
            }
            else {
                json_array_append(value, json_integer(1));
                json_array_append(value, json_integer(2));
                json_array_append(value, json_integer(3));
                json_array_append(value, json_integer(4));
                json_array_append(value, json_integer(5));
            }

        }
        else {
            json_array_append(value, json_integer(2));
            json_array_append(value, json_integer(3));
            json_array_append(value, json_integer(4));
            json_array_append(value, json_integer(5));
        }
    }
    return value;
}
//double replacementcost(int building_type,double area)
int main(int argc, const char **argv) {

  int minRow = 10;
  int maxRow = 10;

  if (argc == 3) {
    minRow = atoi(argv[1]);
    maxRow = atoi(argv[2]);
  }

  int i =  0;

  //
  // now parse the building file, obtaining location form parcel info
  // writing and write a BIM file
  //
  
  CsvParser *csvparser = CsvParser_new("buildings.csv", ",", 1);
  const CsvRow *header = CsvParser_getHeader(csvparser);
  
  if (header == NULL) {
    printf("%s\n", CsvParser_getErrorMessage(csvparser));
    return 1;
  }
  
  const char **headerFields = CsvParser_getFields(header);
  for (i = 0 ; i < CsvParser_getNumFields(header) ; i++) {
    //      printf("TITLE: %d %s\n", i, headerFields[i]);
  }
  CsvRow *row;
  int currentRow = 1;
  
  json_t *root = json_object();

  int *numStories = new int[22];
  for (int i=0; i<22; i++)
    numStories[i] = 0;

  while ((row = CsvParser_getRow(csvparser))) {
    const char **rowFields = CsvParser_getFields(row);
    char *pEnd;
    
    const char *name = rowFields[0];
    int numStory = atoi(rowFields[10]);
    if (numStory < 0)
	numStories[0] += 1;
    else if (numStory > 20)
      numStories[21] += 1;
    else
      numStories[numStory] += 1;
    
    CsvParser_destroy_row(row);

    currentRow++;
  }
  for (int i=0; i<22; i++) {
    printf("%d %d\n",i,numStories[i]);
  }
  
  CsvParser_destroy(csvparser);
  
  return 0;
}
