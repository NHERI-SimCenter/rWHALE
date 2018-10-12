#ifndef CONCRETE_SHEAR_WALL_H
#define CONCRETE_SHEAR_WALL_H

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <list>

using namespace std;




class json_t;

class Node
{
 public:
  Node(int, double, double, double);
  int tag;
  double xLoc;
  double yLoc;
  double zLoc;
};

class Material {
 public:
  Material();
  int getNewTag();

  virtual int readFromJSON(json_t *) =0;
  virtual int writeUniaxialJSON(json_t *uniaxialArray) =0;
  virtual int writeNDJSON(json_t *ndArray) =0;

  static int numTagUni;
  static int numTagND;

  int uniaxialTag, ndTag;
  bool writtenUniaxial, writtenND;
};

class Concrete: public Material
{
 public:
  int readFromJSON(json_t *);
  int writeUniaxialJSON(json_t *);
  int writeNDJSON(json_t *);

  double masspervolume, E, fpc, nu;

};

class SteelRebar: public Material
{
 public:
  int readFromJSON(json_t *);
  int writeUniaxialJSON(json_t *);
  int writeNDJSON(json_t *);
  
  double masspervolume, E, epsu,fu,fy;
};


class Steel: public Material
{
 public:
  int readFromJSON(json_t*);
  int writeUniaxialJSON(json_t *);
  int writeNDJSON(json_t *);
  
  double masspervolume, E, fu,fy,nu;
};

class WallSection
{
 public:
  WallSection();
  virtual int readFromJSON(json_t *) =0;
  virtual int writeBeamSectionJSON(json_t *) =0;
  //  virtual int writeNDJSON(json_t *) =0;
  virtual int writeNDJSON(json_t *elements, json_t *nodes, json_t *properties, json_t *nodeMapping, string, string, string, string) =0;


  bool writtenBeamSection, writtenND;
  int beamTag, ndTag;;
};


class ConcreteRectangularWallSection: public WallSection 
{
  double thickness, be_length;
  double lr_area, lr_spacing, lr_cover; int lr_matTag, lr_numBarsThickness;
  double lrb_area, lrb_cover; int lrb_matTag, lrb_numBarsThickness, lrb_numBarsLength, trb_numBarsLength;
  double tr_area, trb_area, tr_spacing, trb_spacing, tr_cover, trb_cover; 
  int tr_matTag, tr_numBarsThickness, trb_numBarsThickness;
  string lr_mat, lrb_mat, tr_mat, trb_mat;


  int readFromJSON(json_t*);
  int writeBeamSectionJSON(json_t *properties);
  int writeNDJSON(json_t *elements, json_t *nodes, json_t *properties, json_t *nodeMapping, string, string, string, string);
  int ndMaterialExistInArray(json_t *obj ,json_t *Array);
};


class Wall
{
 public:
  string cline1, cline2, floor1, floor2;
  string name, section;

  int readFromJSON(json_t *);
  int writeToJSON(json_t *elements, json_t *nodes, json_t *properties,json_t *nodeMapping);
};


class ConcreteShearWall
{
 public:

    ConcreteShearWall();    

    int readBIM(const char *fileEVENT, const char *fileBIM);
    int writeSAM(const char *path, int nL, int nH);
    int writeOpenSeesModel(const char *path);
    void setModelingParameters(int numX, int numY);

    int addNode(double x, double y, double z, json_t *nodeArray);
    int addNode(double x, double y, double z, char *floor, char *col, json_t *nodeArray);
  
    vector<double> getCLineLoc(string cline);
    double getFloorLocation(string floor);
    Material *getMaterial(string name);
    WallSection *getWallSection(string name);

    void error(string message, int errorFlag = 1);

    int numFloors;
    int numNode;

    map<string, Material *>    theMaterials;
    map<string, WallSection *> theWallSections;
    map<string, Wall *> theWalls;
    map<string, double> floors;
    map<string, vector<double> > clines;
    list<Node> nodes;

    json_t *SI;
};

#endif // CONCRETE_SHEAR_WALL_H
