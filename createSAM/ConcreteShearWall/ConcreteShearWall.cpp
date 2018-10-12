#include "ConcreteShearWall.h"
#include <jansson.h> // for Json

#include <iostream>
#include <cmath>
#include <cstring>

static int nL = 1;
static int nH = 1;

static ConcreteShearWall *theModel = 0;
static int eleTag = 0;
static double tol = 1e-7; //1e-15;
//double maxEleWallLength =

Node::Node(int t, double x, double y, double z)
    : tag(t), xLoc(x), yLoc(y), zLoc(z)
{
}

int Material::numTagND = 0;
int Material::numTagUni = 0;

Material::Material()
    : writtenUniaxial(false), writtenND(false)
{
}

int Material::getNewTag()
{
  int thisTag = ++numTagND;
  return thisTag;
}

int Concrete::readFromJSON(json_t *obj)
{
  //  double masspervolume, E, fpc, nu;
  masspervolume = json_number_value(json_object_get(obj, "masspervolume"));
  E = json_number_value(json_object_get(obj, "E"));
  fpc = json_number_value(json_object_get(obj, "fpc"));
  nu = json_number_value(json_object_get(obj, "nu"));
  return 0;
}

int Concrete::writeUniaxialJSON(json_t *obj)
{
  std::cerr << "concrete: E: " << E << " fpc: " << fpc << " nu:" << nu << "\n";

  return 0;
}

int Concrete::writeNDJSON(json_t *ndMaterials)
{
  if (this->writtenND == false)
  {
    ndTag = ++numTagND;

    json_t *obj = json_object();

    json_object_set(obj, "name", json_integer(ndTag));
    json_object_set(obj, "type", json_string("Concrete"));
    json_object_set(obj, "E", json_real(E));
    json_object_set(obj, "fpc", json_real(fpc));
    //    json_object_set(obj,"b",json_real((fu-fy)/(epsu-fy/E)));
    json_object_set(obj, "nu", json_real(nu));

    json_array_append(ndMaterials, obj);
    writtenND = true;
  }

  return 0;
}

int Steel::writeUniaxialJSON(json_t *obj)
{
  return 0;
}

int Steel::writeNDJSON(json_t *obj)
{
  return 0;
}

int Steel::readFromJSON(json_t *obj)
{
  //    double masspervolume, E, fu,fy,nu;
  masspervolume = json_number_value(json_object_get(obj, "masspervolume"));
  E = json_number_value(json_object_get(obj, "E"));
  fu = json_number_value(json_object_get(obj, "fu"));

  fy = json_number_value(json_object_get(obj, "fy"));
  nu = json_number_value(json_object_get(obj, "nu"));

  return 0;
}

int SteelRebar::readFromJSON(json_t *obj)
{
  // double masspervolume, E, epsu,fu,fy;
  masspervolume = json_number_value(json_object_get(obj, "masspervolume"));
  E = json_number_value(json_object_get(obj, "E"));
  fu = json_number_value(json_object_get(obj, "fu"));
  fy = json_number_value(json_object_get(obj, "fy"));
  epsu = json_number_value(json_object_get(obj, "epsu"));

  return 0;
}

int SteelRebar::writeUniaxialJSON(json_t *uniaxialArray)
{
  if (json_array_size(uniaxialArray) < 1)
  {
    uniaxialTag = ++numTagUni;

    json_t *obj = json_object();

    json_object_set(obj, "name", json_integer(uniaxialTag));
    json_object_set(obj, "type", json_string("Steel01"));
    json_object_set(obj, "E", json_real(E));
    json_object_set(obj, "fy", json_real(fy));
    //    json_object_set(obj,"b",json_real((fu-fy)/(epsu-fy/E)));
    json_object_set(obj, "b", json_real(.01));

    json_array_append(uniaxialArray, obj);
    return uniaxialTag;
  }
  else
  {
    json_t *unixialMat;
    int index;
    json_array_foreach(uniaxialArray, index, unixialMat)
    {
      int thisunixialMatTag = json_integer_value(json_object_get(unixialMat, "name"));
      double thisunixialMatE = json_number_value(json_object_get(unixialMat, "E"));
      double thisunixialMatfy = json_number_value(json_object_get(unixialMat, "fy"));
      double thisunixialMatb = json_number_value(json_object_get(unixialMat, "b"));
      if (abs(thisunixialMatE - E) < 1e-7 && abs(thisunixialMatfy - fy) < 1e-7 && abs(thisunixialMatb - 0.01) < 1e-7) //(abs(thisunixialMat-uniaxialTag)<1e-7)
      {
        printf("unixal mat %d already exists in uniaxialMaterials. \n", thisunixialMatTag);
        return thisunixialMatTag;
      }
    }

    uniaxialTag = ++numTagUni;
    json_t *obj = json_object();
    json_object_set(obj, "name", json_integer(uniaxialTag));
    json_object_set(obj, "type", json_string("Steel01"));
    json_object_set(obj, "E", json_real(E));
    json_object_set(obj, "fy", json_real(fy));
    //    json_object_set(obj,"b",json_real((fu-fy)/(epsu-fy/E)));
    json_object_set(obj, "b", json_real(.01));
    json_array_append(uniaxialArray, obj);
    return uniaxialTag;
  }

  /*
  if (this->writtenUniaxial == false)
  {
    uniaxialTag = numTag++;

    json_t *obj = json_object();

    json_object_set(obj, "name", json_integer(uniaxialTag));
    json_object_set(obj, "type", json_string("Steel01"));
    json_object_set(obj, "E", json_real(E));
    json_object_set(obj, "fy", json_real(fy));
    //    json_object_set(obj,"b",json_real((fu-fy)/(epsu-fy/E)));
    json_object_set(obj, "b", json_real(.01));

    json_array_append(uniaxialArray, obj);
    writtenUniaxial = true;
  }
  */

  return 0;
}

int SteelRebar::writeNDJSON(json_t *obj)
{
  std::cerr << "steel rebar: E: " << E << " fu: " << fu << " fy: " << fy << " epsu:" << epsu << "\n";
  return 0;
}

WallSection::WallSection()
    : writtenBeamSection(false), writtenND(false), beamTag(0), ndTag(0)
{
}

int ConcreteRectangularWallSection::readFromJSON(json_t *obj)
{
  thickness = json_number_value(json_object_get(obj, "thickness"));
  be_length = json_number_value(json_object_get(obj, "boundaryElementLength"));

  json_t *long_rebar = json_object_get(obj, "longitudinalRebar");
  const char *t = json_string_value(json_object_get(long_rebar, "material"));
  lr_mat.assign(t);
  lr_numBarsThickness = json_integer_value(json_object_get(long_rebar, "numBarsThickness"));
  lr_area = json_number_value(json_object_get(long_rebar, "barArea"));
  lr_spacing = json_number_value(json_object_get(long_rebar, "spacing"));
  lr_cover = json_number_value(json_object_get(long_rebar, "cover"));

  json_t *tran_rebar = json_object_get(obj, "transverseRebar");
  t = json_string_value(json_object_get(tran_rebar, "material"));
  tr_mat.assign(t);
  tr_numBarsThickness = json_integer_value(json_object_get(tran_rebar, "numBarsThickness"));
  tr_area = json_number_value(json_object_get(tran_rebar, "barArea"));
  tr_cover = json_number_value(json_object_get(tran_rebar, "cover"));
  tr_spacing = json_number_value(json_object_get(tran_rebar, "spacing"));

  if (be_length > 0.0) // has boundary elements
  {
    json_t *long_rebarB = json_object_get(obj, "longitudinalBoundaryElementRebar");
    //t = json_string_value(json_object_get(long_rebarB, "material"));
    lrb_mat.assign(json_string_value(json_object_get(long_rebarB, "material")));
    lrb_numBarsThickness = json_integer_value(json_object_get(long_rebarB, "numBarsThickness"));
    lrb_numBarsLength = json_integer_value(json_object_get(long_rebarB, "numBarsLength"));
    lrb_area = json_number_value(json_object_get(long_rebarB, "barArea"));
    lrb_cover = json_number_value(json_object_get(long_rebarB, "cover"));

    json_t *tranb_rebar = json_object_get(obj, "transverseBoundaryElementRebar");
    //t = json_string_value(json_object_get(tranb_rebar, "material"));
    if (tranb_rebar > 0)
    {
      trb_mat.assign(json_string_value(json_object_get(tranb_rebar, "material")));
      trb_numBarsThickness = json_integer_value(json_object_get(tranb_rebar, "numBarsThickness"));
      trb_numBarsLength = json_integer_value(json_object_get(tranb_rebar, "numBarsLength"));
      trb_area = json_number_value(json_object_get(tranb_rebar, "barArea"));
      //trb_cover = json_number_value(json_object_get(tranb_rebar, "cover"));
      trb_spacing = json_number_value(json_object_get(tranb_rebar, "spacing"));
    }
    else
    { // didn't find transverse rebar for BE, assume there is. set barArea=0;
      trb_mat.assign(json_string_value(json_object_get(long_rebarB, "material")));
      trb_numBarsThickness = 0;
      trb_numBarsLength = 0;
      trb_area = 0.0;
      trb_spacing = json_number_value(json_object_get(long_rebarB, "spacing"));
    }
  }

  return 0;
}

int ConcreteRectangularWallSection::writeBeamSectionJSON(json_t *obj)
{
  return 0;
}

int ConcreteRectangularWallSection::writeNDJSON(json_t *elements, json_t *nodes, json_t *properties, json_t *nodeMapping, string cline1, string cline2, string floor1, string floor2)
{

  json_t *uniaxialMaterials = json_object_get(properties, "uniaxialMaterials");
  json_t *ndMaterials = json_object_get(properties, "ndMaterials");

  int lr_tag, lrb_tag, tr_tag, trb_tag, lrb_tag_ND, trb_tag_ND;

  std::cout << "lr_mat: " << lr_mat << std::endl;

  // Write out unixial steel materials
  Material *theLR_Material = theModel->getMaterial(lr_mat); // longitudinalRebar
  if (theLR_Material != 0)
  {
    lr_tag = theLR_Material->writeUniaxialJSON(uniaxialMaterials);
    //lr_tag = theLR_Material->uniaxialTag;
  }
  else
  {
    std::cerr << "ConcreteRectangularWallSection: long reinf mat not found: " << lr_mat << "\n";
    return 0;
  }

  if (be_length > 0.0)
  {
    Material *theLRB_Material = theModel->getMaterial(lrb_mat); // longitudinalBoundaryElementRebar
    if (theLRB_Material != 0)
    {
      lrb_tag = theLRB_Material->writeUniaxialJSON(uniaxialMaterials);
      //lrb_tag = theLRB_Material->uniaxialTag;
      //printf("lrb_tag is  =  %d\n", lrb_tag);
    }
    else
    {
      std::cerr << "ConcreteRectangularWallSection: long reinf mat not found: " << lrb_mat << "\n";
      return 0;
    }
  }

  Material *theTR_Material = theModel->getMaterial(tr_mat); // transverseRebar
  if (theTR_Material != 0)
  {
    tr_tag = theTR_Material->writeUniaxialJSON(uniaxialMaterials);
    //printf("tr_tag is  =  %d\n", tr_tag);
    cout << "tr_mat is: " << tr_mat << endl;
    //tr_tag = theTR_Material->uniaxialTag;
  }
  else
  {
    std::cerr << "ConcreteRectangularWallSection: long reinf mat not found: " << lrb_mat << "\n";
    return 0;
  }

  Material *theTRb_Material = theModel->getMaterial(trb_mat); // transverseRebar
  if (theTRb_Material != 0)
  {
    trb_tag = theTRb_Material->writeUniaxialJSON(uniaxialMaterials);
    //printf("trb_tag is  =  %d\n", trb_tag);
    //tr_tag = theTR_Material->uniaxialTag;
  }
  else
  {
    // TODO
    std::cerr << "ConcreteRectangularWallSection: transverse boundary elem reinf mat not found: " << trb_mat << "\n";
    trb_tag = tr_tag;
    //return 0;
  }

  //printf("Material::numTagND =  %d\n", Material::numTagND);
  //printf("size of ndMaterial =  %d\n", json_array_size(ndMaterials));

  // Write out ND rebar (utilizes steel)
  json_t *rebarLR = json_object();
  json_t *rebarLRB = json_object();
  json_t *rebarTR = json_object();
  json_t *rebarTRB = json_object();

  json_object_set(rebarLR, "material", json_integer(lr_tag));
  json_object_set(rebarLR, "angle", json_real(90));
  int lr_tag_ND = ndMaterialExistInArray(rebarLR, ndMaterials);
  if (!lr_tag_ND)
  {
    int lr_name = ++Material::numTagND;
    lr_tag_ND = lr_name;
    json_object_set(rebarLR, "name", json_integer(lr_name));
    json_object_set(rebarLR, "type", json_string("PlaneStressRebar"));

    json_array_append(ndMaterials, rebarLR);
  }

  if (be_length > 0.0)
  {
    json_object_set(rebarLRB, "material", json_integer(lrb_tag));
    json_object_set(rebarLRB, "angle", json_real(90));
    lrb_tag_ND = ndMaterialExistInArray(rebarLRB, ndMaterials);
    if (!lrb_tag_ND)
    {
      int lrb_name = ++Material::numTagND;
      lrb_tag_ND = lrb_name;
      json_object_set(rebarLRB, "name", json_integer(lrb_name));
      json_object_set(rebarLRB, "type", json_string("PlaneStressRebar"));

      json_array_append(ndMaterials, rebarLRB);
    }

    json_object_set(rebarTRB, "material", json_integer(trb_tag));
    json_object_set(rebarTRB, "angle", json_real(0));
    trb_tag_ND = ndMaterialExistInArray(rebarTRB, ndMaterials);
    if (!trb_tag_ND)
    {
      int trb_name = ++Material::numTagND;
      trb_tag_ND = trb_name;
      json_object_set(rebarTRB, "name", json_integer(trb_name));
      json_object_set(rebarTRB, "type", json_string("PlaneStressRebar"));

      json_array_append(ndMaterials, rebarTRB);
    }
  }

  json_object_set(rebarTR, "material", json_integer(tr_tag));
  json_object_set(rebarTR, "angle", json_real(0));
  int tr_tag_ND = ndMaterialExistInArray(rebarTR, ndMaterials);
  if (!tr_tag_ND)
  {
    int tr_name = ++Material::numTagND;
    tr_tag_ND = tr_name;
    json_object_set(rebarTR, "name", json_integer(tr_name));
    json_object_set(rebarTR, "type", json_string("PlaneStressRebar"));

    json_array_append(ndMaterials, rebarTR);
  }

  //printf("Material =  %d\n", json_integer_value(json_object_get(rebarTR, "material")));

  //printf("Material::numTagND =  %d\n", Material::numTagND);

  int ndMatSize = json_array_size(json_object_get(properties, "ndMaterials"));
  //printf("ndMatSize at entry of writeToJSON: %d\n", ndMatSize);

  // write out concrete
  Material *concrete = theModel->getMaterial(string("Concrete"));
  concrete->writeNDJSON(ndMaterials);
  int concTag = concrete->ndTag;

  // now write out the reinforced-sections
  double tTR = tr_area * tr_numBarsThickness / tr_spacing; // thickness of transver bar
  double tLR = lr_area * lr_numBarsThickness / lr_spacing; // thickness of logitudinal bar
  double tConc = thickness - tTR - tLR;                    // thickness of concrete

  int tag_matM = concrete->getNewTag();
  int tag_matB;

  json_t *matM = json_object();
  json_object_set(matM, "name", json_integer(tag_matM));
  json_object_set(matM, "type", json_string("LayeredConcrete"));
  json_t *matLayers = json_array();

  json_t *conc = json_object();
  json_object_set(conc, "thickness", json_real(tConc));
  json_object_set(conc, "material", json_integer(concTag));
  json_array_append(matLayers, conc);

  json_t *longM = json_object();
  json_object_set(longM, "thickness", json_real(tLR));
  json_object_set(longM, "material", json_integer(lr_tag_ND));
  json_array_append(matLayers, longM);

  json_t *horizM = json_object();
  json_object_set(horizM, "thickness", json_real(tTR));
  json_object_set(horizM, "material", json_integer(tr_tag_ND));
  json_array_append(matLayers, horizM);
  json_object_set(matM, "layers", matLayers);

  json_array_append(ndMaterials, matM);

  if (be_length > 0.0)
  {
    tag_matB = concrete->getNewTag();

    double tLRB = lrb_area * lrb_numBarsThickness * lrb_numBarsLength / be_length; // thickness logitudinal rebar
    double tTRB = trb_area * trb_numBarsThickness * trb_numBarsLength / be_length; // ask Frank how to calculate, TODO
    double tConcB = thickness - tLRB - tTRB;                                       // thickness of concrete

    json_t *matBE = json_object();
    json_object_set(matBE, "name", json_integer(tag_matB));
    json_object_set(matBE, "type", json_string("LayeredConcrete"));
    json_t *matLayersB = json_array();

    json_t *concB = json_object();
    json_object_set(concB, "thickness", json_real(tConcB));
    json_object_set(concB, "material", json_integer(concTag));
    json_array_append(matLayersB, concB);

    json_t *longB = json_object();
    json_object_set(longB, "thickness", json_real(tLRB));
    json_object_set(longB, "material", json_integer(lrb_tag_ND));
    json_array_append(matLayersB, longB);

    json_t *horizB = json_object();
    json_object_set(horizB, "thickness", json_real(tTRB));
    json_object_set(horizB, "material", json_integer(trb_tag_ND));
    json_array_append(matLayersB, horizB);
    json_object_set(matBE, "layers", matLayersB);

    json_array_append(ndMaterials, matBE);
  }

  double floor1Loc = theModel->getFloorLocation(floor1);
  double floor2Loc = theModel->getFloorLocation(floor2);
  vector<double> loc1 = theModel->getCLineLoc(cline1);
  vector<double> loc2 = theModel->getCLineLoc(cline2);
  double cline1XLoc = loc1[0];
  double cline1YLoc = loc1[1];
  double cline2XLoc = loc2[0];
  double cline2YLoc = loc2[1];

  double dX = cline2XLoc - cline1XLoc;
  double dY = cline2YLoc - cline1YLoc;
  double dZ = floor2Loc - floor1Loc;
  double length = sqrt(dX * dX + dY * dY);
  if (length == 0 || dZ == 0)
  {
    std::cerr << "ERROR Wall : has 0 length or height";
    return -1;
  }

  // mesh the section
  int numBoundary = 2;
  /*
  int numLength = nL; //int numLength = ceil(dX/maxWallEleLength);
  int numHeight = nH; // int numHeight = ceil(dX/maxWallEleLength);
  double deltaX1 = be_length / 2.0;
  double deltaX2 = (dX - 2 * be_length) / (numLength * 1.0);
  double deltaY = dY / numLength;
  double deltaZ = dZ / numHeight;
  */
  double deltaX1 = be_length / 2.0;
  int numLength, numHeight;
  if (nL < 1.0)
  {
    numLength = ceil((dX - 2 * be_length) / deltaX1);
  }
  else
  {
    numLength = nL;
  }
  double deltaX2 = (dX - 2 * be_length) / numLength;

  double deltaY = dY / deltaX2; // TODO
  if (nH < 1.0)
  {
    numHeight = ceil(dZ / deltaX2);
  }
  else
  {
    numHeight = nH;
  }
  double deltaZ = dZ / numHeight;

  double x1Loc = 0;
  double x2Loc = deltaX1;

  for (int j = 0; j < numLength + (numBoundary * 2); j++)
  {

    for (int k = 0; k < numHeight; k++)
    {
      int iNode = theModel->addNode(cline1XLoc + x1Loc, cline1YLoc + deltaY * j, floor1Loc + deltaZ * k, nodes);
      int jNode = theModel->addNode(cline1XLoc + x2Loc, cline1YLoc + deltaY * (j + 1), floor1Loc + deltaZ * k, nodes);
      int kNode = theModel->addNode(cline1XLoc + x2Loc, cline1YLoc + deltaY * (j + 1), floor1Loc + deltaZ * (k + 1), nodes);
      int lNode = theModel->addNode(cline1XLoc + x1Loc, cline1YLoc + deltaY * j, floor1Loc + deltaZ * (k + 1), nodes);
      if (abs(iNode - jNode) > 1e-7)
      {
        json_t *theElement = json_object();
        json_object_set(theElement, "name", json_integer(eleTag++));
        json_object_set(theElement, "type", json_string("FourNodeQuad"));
        json_t *theNodes = json_array();
        json_array_append(theNodes, json_integer(iNode));
        json_array_append(theNodes, json_integer(jNode));
        json_array_append(theNodes, json_integer(kNode));
        json_array_append(theNodes, json_integer(lNode));
        json_object_set(theElement, "nodes", theNodes);
        if (j < 2 || j > numLength + 1) // boundary
          json_object_set(theElement, "material", json_integer(tag_matB));
        else
          json_object_set(theElement, "material", json_integer(tag_matM));

        json_array_append(elements, theElement);
      }
    }

    x1Loc = x2Loc;

    if (j < numBoundary - 1)
    {
      x2Loc += deltaX1;
    }
    else if (j > numLength + numBoundary - 2)
    {
      x2Loc += deltaX1;
    }
    else
    {
      x2Loc += deltaX2;
    }
  }

  // add nodeMapping
  int cline1Floor1 = theModel->addNode(cline1XLoc, cline1YLoc, floor1Loc, nodes);
  int cline1Floor2 = theModel->addNode(cline1XLoc, cline1YLoc, floor2Loc, nodes);
  int cline2Floor1 = theModel->addNode(cline2XLoc, cline2YLoc, floor1Loc, nodes);
  int cline2Floor2 = theModel->addNode(cline2XLoc, cline2YLoc, floor2Loc, nodes);

  json_t *nodeMap = json_object();
  json_object_set(nodeMap, "cline", json_integer(stoi(cline1)));
  json_object_set(nodeMap, "floor", json_integer(stoi(floor1)));
  json_object_set(nodeMap, "node", json_integer(cline1Floor1));
  json_array_append(nodeMapping, nodeMap);

  nodeMap = json_object();
  json_object_set(nodeMap, "cline", json_integer(stoi(cline1)));
  json_object_set(nodeMap, "floor", json_integer(stoi(floor2)));
  json_object_set(nodeMap, "node", json_integer(cline1Floor2));
  json_array_append(nodeMapping, nodeMap);

  nodeMap = json_object();
  json_object_set(nodeMap, "cline", json_integer(stoi(cline2)));
  json_object_set(nodeMap, "floor", json_integer(stoi(floor1)));
  json_object_set(nodeMap, "node", json_integer(cline2Floor1));
  json_array_append(nodeMapping, nodeMap);

  nodeMap = json_object();
  json_object_set(nodeMap, "cline", json_integer(stoi(cline2)));
  json_object_set(nodeMap, "floor", json_integer(stoi(floor2)));
  json_object_set(nodeMap, "node", json_integer(cline2Floor2));
  json_array_append(nodeMapping, nodeMap);

  return 0;
}

int Wall::readFromJSON(json_t *theWall)
{
  json_t *clineArray = json_object_get(theWall, "cline");
  json_t *floorArray = json_object_get(theWall, "floor");
  cline1 = string(json_string_value(json_array_get(clineArray, 0)));
  cline2 = string(json_string_value(json_array_get(clineArray, 1)));
  floor1 = string(json_string_value(json_array_get(floorArray, 0)));
  floor2 = string(json_string_value(json_array_get(floorArray, 1)));

  json_t *segments = json_object_get(theWall, "segment");
  int numSegment = json_array_size(segments);
  if (numSegment > 1)
  {
    std::cerr << "CANNOT HABDLE MORE THAN 1 WALL SEGMENT .. UISNG LAST ALL WAY UP\n";
  }
  for (int i = 0; i < numSegment; i++)
  {
    json_t *segment = json_array_get(segments, i);
    section = string(json_string_value(json_object_get(segment, "section")));
  }
  return 0;
}

int Wall::writeToJSON(json_t *elements, json_t *nodes, json_t *properties, json_t *nodeMapping)
{

  WallSection *theSection = theModel->getWallSection(section);
  if (theSection != 0)

    theSection->writeNDJSON(elements, nodes, properties, nodeMapping, cline1, cline2, floor1, floor2);
  /*
  double floor1Loc = theModel->getFloorLocation(floor1);
  double floor2Loc = theModel->getFloorLocation(floor2);
  vector<double> loc1 = theModel->getCLineLoc(cline1);
  vector<double> loc2 = theModel->getCLineLoc(cline2);
  double cline1XLoc = loc1[0];
  double cline1YLoc = loc1[1];
  double cline2XLoc = loc2[0];
  double cline2YLoc = loc2[1];

  double dX = cline2XLoc - cline1XLoc;
  double dY = cline2YLoc - cline1YLoc;
  double dZ = floor2Loc - floor1Loc;
  double length = sqrt(dX * dX + dY * dY);
  if (length == 0 || dZ == 0)
  {
    std::cerr << "ERROR Wall : " << name << " has 0 length or height";
    return -1;
  }

  int numLength = 10; //int numLength = ceil(dX/maxWallEleLength);
  int numHeight = 10; // int numHeight = ceil(dX/maxWallEleLength);
  double deltaX = dX / numLength;
  double deltaY = dY / numLength;
  double deltaZ = dZ / numHeight;

  for (int j = 0; j < numLength; j++)
  {
    for (int k = 0; k < numHeight; k++)
    {
      int iNode = theModel->addNode(cline1XLoc + deltaX * j, cline1YLoc + deltaY * j, floor1Loc + deltaZ * k, nodes);
      int jNode = theModel->addNode(cline1XLoc + deltaX * (j + 1), cline1YLoc + deltaY * (j + 1), floor1Loc + deltaZ * k, nodes);
      int kNode = theModel->addNode(cline1XLoc + deltaX * (j + 1), cline1YLoc + deltaY * (j + 1), floor1Loc + deltaZ * (k + 1), nodes);
      int lNode = theModel->addNode(cline1XLoc + deltaX * j, cline1YLoc + deltaY * j, floor1Loc + deltaZ * (k + 1), nodes);

      json_t *theElement = json_object();
      json_object_set(theElement, "name", json_integer(eleTag++));
      json_object_set(theElement, "type", json_string("FourNodeQuad"));
      json_t *theNodes = json_array();
      json_array_append(theNodes, json_integer(iNode));
      json_array_append(theNodes, json_integer(jNode));
      json_array_append(theNodes, json_integer(kNode));
      json_array_append(theNodes, json_integer(lNode));
      json_object_set(theElement, "nodes", theNodes);
      json_object_set(theElement, "material", json_string("FourNodeQuad"));

      json_array_append(elements, theElement);
    }
  }
  */

  return 0;
}

ConcreteShearWall::ConcreteShearWall()
    : numFloors(0), numNode(0)
{
  theModel = this;
}

void ConcreteShearWall::error(string message, int errorFlag)
{
  std::cerr << message << "\n";
  if (errorFlag != 1)
    exit(errorFlag);
}

int ConcreteShearWall::readBIM(const char *event, const char *bim)
{
  //Parse BIM Json input file
  json_error_t error;
  json_t *rootBIM = json_load_file(bim, 0, &error);

  json_t *GI = json_object_get(rootBIM, "GeneralInformation");
  json_t *yType = json_object_get(GI, "yBuilt");
  int nStory = json_integer_value(json_object_get(GI, "stories"));

  numFloors = nStory;

  SI = json_object_get(rootBIM, "StructuralInformation");

  const char *type = json_string_value(json_object_get(SI, "type"));

  int year = json_integer_value(yType);

  if (strcmp(type, "A1 - SpecialReinforcedConcreteShearWall") != 0)
  {
    return -1;
  }

  //
  // read floor and cline locations
  //

  json_t *layout = json_object_get(SI, "layout");

  json_t *floorArray = json_object_get(layout, "floors");
  if (floorArray == NULL)
  {
    return -2;
  }

  int numFLOOR = json_array_size(floorArray);
  for (int i = 0; i < numFLOOR; i++)
  {
    json_t *theFloor = json_array_get(floorArray, i);
    const char *name = json_string_value(json_object_get(theFloor, "name"));
    double location = json_number_value(json_object_get(theFloor, "elevation"));
    floors.insert(std::pair<string, double>(string(name), location));

    std::cerr << "floor name: " << name << " loc: " << location << "\n";
  }

  json_t *clineArray = json_object_get(layout, "clines");
  if (clineArray == NULL)
  {
    return -2;
  }

  int numCLINE = json_array_size(clineArray);
  for (int i = 0; i < numCLINE; i++)
  {
    json_t *theCline = json_array_get(clineArray, i);
    const char *name = json_string_value(json_object_get(theCline, "name"));
    json_t *location = json_object_get(theCline, "location");
    std::vector<double> loc(2);
    loc[0] = json_number_value(json_array_get(location, 0));
    loc[1] = json_number_value(json_array_get(location, 1));
    clines.insert(std::pair<string, vector<double>>(string(name), loc));

    std::cerr << "cline name: " << name << " loc: " << loc[0] << " " << loc[1] << "\n";
  }

  //
  // read the geometry & process elements
  //

  // read materials
  json_t *properties = json_object_get(SI, "properties");

  json_t *materialsArray = json_object_get(properties, "materials");
  if (materialsArray != NULL)
  {

    int numMat = json_array_size(materialsArray);
    for (int i = 0; i < numMat; i++)
    {
      json_t *material = json_array_get(materialsArray, i);
      const char *name = json_string_value(json_object_get(material, "name"));
      const char *type = json_string_value(json_object_get(material, "type"));
      Material *theMaterial = 0;
      if (strcmp(type, "concrete") == 0)
      {
        theMaterial = new Concrete();
      }
      else if (strcmp(type, "steel") == 0)
      {
        theMaterial = new Steel();
      }
      else if (strcmp(type, "steel rebar") == 0)
      {
        theMaterial = new SteelRebar();
      }
      else
      {
        std::cerr << "unknown material type: " << type << "\n";
      }
      theMaterial->readFromJSON(material);
      theMaterials.insert(std::pair<string, Material *>(string(name), theMaterial));
    }
  }

  json_t *wallSectionsArray = json_object_get(properties, "wallsections");
  if (wallSectionsArray != NULL)
  {

    int numSection = json_array_size(wallSectionsArray);
    for (int i = 0; i < numSection; i++)
    {
      json_t *section = json_array_get(wallSectionsArray, i);
      const char *name = json_string_value(json_object_get(section, "name"));
      const char *type = json_string_value(json_object_get(section, "type"));
      WallSection *theSection = 0;
      if (strcmp(type, "concrete rectangular wall") == 0)
      {
        theSection = new ConcreteRectangularWallSection();
      }
      else
      {
        std::cerr << "unknown wall section type: " << type << "\n";
        exit(-1);
      }
      theSection->readFromJSON(section);
      theWallSections.insert(std::pair<string, WallSection *>(string(name), theSection));
    }
  }

  //
  // process wall information
  //

  json_t *geometry = json_object_get(SI, "geometry");

  json_t *wallArray = json_object_get(geometry, "walls");
  if (wallArray != NULL)
  {

    int numWall = json_array_size(wallArray);
    for (int i = 0; i < numWall; i++)
    {
      json_t *theWallData = json_array_get(wallArray, i);
      Wall *theWall = new Wall();
      theWall->readFromJSON(theWallData);
      const char *name = json_string_value(json_object_get(theWallData, "name"));
      theWalls.insert(std::pair<string, Wall *>(string(name), theWall));
    }

    /*
      json_t *clineArray = json_object_get(theWall,"cline");      
      json_t *floorArray = json_object_get(theWall,"floor");      
      const char *cline1 = json_string_value(json_array_get(clineArray,0));
      const char *cline2 = json_string_value(json_array_get(clineArray,1));
      const char *floor1 = json_string_value(json_array_get(floorArray,0));
      const char *floor2 = json_string_value(json_array_get(floorArray,1));

      
      double floor1Loc = this->getFloorLocation(string(floor1));
      double floor2Loc = this->getFloorLocation(string(floor2));
      vector<double> loc1 = this->getCLineLoc(cline1);
      vector<double> loc2 = this->getCLineLoc(cline2);
      double cline1XLoc = loc1[0];       double cline1YLoc = loc1[1];
      double cline2XLoc = loc2[0];       double cline2YLoc = loc2[1];

      double dX = cline2XLoc - cline1XLoc;
      double dY = cline2YLoc - cline1YLoc;
      double dZ = floor2Loc - floor1Loc;
      double length = sqrt(dX*dX + dY*dY);
      if (length == 0 || dZ == 0) {
	std::cerr << "ERROR Wall : " << name << " has 0 length or height";
	break;
      }

      int numLength = 10; //int numLength = ceil(dX/maxWallEleLength);
      int numHeight = 10; // int numHeight = ceil(dX/maxWallEleLength);
      double deltaX = dX/numLength;
      double deltaY = dY/numLength;
      double deltaZ = dZ/numHeight;
      
      for (int j=0; j<numLength; j++) {
	for (int k=0; k<numHeight; k++) {
	  int iNode = addNode(cline1XLoc + deltaX*j, cline1YLoc + deltaY*j, floor1Loc + deltaZ*k);
	  int jNode = addNode(cline1XLoc + deltaX*(j+1), cline1YLoc + deltaY*(j+1), floor1Loc + deltaZ*k);
	  int kNode = addNode(cline1XLoc + deltaX*(j+1), cline1YLoc + deltaY*(j+1), floor1Loc + deltaZ*(k+1));
	  int lNode = addNode(cline1XLoc + deltaX*j, cline1YLoc + deltaY*j, floor1Loc + deltaZ*(k+1));
	}
      }
      */
  }
  return 0;
}

int ConcreteShearWall::writeSAM(const char *path, int nLtmp, int nHtmp)
{
  nL = nLtmp;
  nH = nHtmp;

  int nStory = numFloors;
  json_t *root = json_object();
  json_t *sam = json_object();
  json_t *properties = json_object();
  json_t *geometry = json_object();

  json_t *uniaxialMaterials = json_array();
  json_t *ndMaterials = json_array();
  json_t *sections = json_array();
  json_t *crdTransformations = json_array();
  json_t *nodes = json_array();
  json_t *elements = json_array();

  //
  // write the model
  //

  json_object_set(properties, "uniaxialMaterials", uniaxialMaterials);
  json_object_set(properties, "ndMaterials", ndMaterials);
  json_object_set(properties, "sections", sections);
  json_object_set(properties, "crdTransformations", crdTransformations);

  json_t *nodeMapping = json_array();

  map<string, Wall *>::iterator it;
  for (it = theWalls.begin(); it != theWalls.end(); it++)
  {
    it->second->writeToJSON(elements, nodes, properties, nodeMapping);
  }

  json_object_set(geometry, "nodes", nodes);
  json_object_set(geometry, "elements", elements);
  json_object_set(sam, "geometry", geometry);

  json_object_set(sam, "properties", properties);
  json_object_set(root, "Structural Analysis Model", sam);

  /*
    // add nodeMapping
    json_t *nodeMapping = json_array();
    json_t *nodeMap = json_object();
    json_object_set(nodeMap,"cline",json_integer(1));
    json_object_set(nodeMap,"floor",json_integer(1));
    json_object_set(nodeMap,"node",json_integer(1));
    json_array_append(nodeMapping, nodeMap);
    for(int i=0;i<nStory;++i) {
      nodeMap = json_object();
      json_object_set(nodeMap,"cline",json_integer(1));
      json_object_set(nodeMap,"floor",json_integer(i+2));
      json_object_set(nodeMap,"node",json_integer(i+2));
      json_array_append(nodeMapping, nodeMap);
    }
    */
  json_object_set(sam, "nodeMapping", nodeMapping);

  /*
    json_t *properties = json_object();
    json_t *geometry = json_object();
    json_t *materials = json_array();
    json_t *nodes = json_array();
    json_t *elements = json_array();
    json_t *nodeMapping = json_array();

    // add node at ground
    json_t *node = json_object();
    json_object_set(node, "name", json_integer(1)); // +2 as we need node at 1 
    json_t *nodePosn = json_array();      
    json_array_append(nodePosn,json_real(0.0));
    json_array_append(nodePosn,json_real(0.0));
    json_object_set(node, "crd", nodePosn);
    json_object_set(node, "ndf", json_integer(ndf));
    json_array_append(nodes,node);

    json_t *nodeMap = json_object();
    json_object_set(nodeMap,"cline",json_integer(1));
    json_object_set(nodeMap,"floor",json_integer(1));
    json_object_set(nodeMap,"node",json_integer(1));
    json_array_append(nodeMapping, nodeMap);

    for(int i=0;i<nStory;++i) {

      json_t *node = json_object();
      json_t *element = json_object();
      json_t *material = json_object();

      json_object_set(node, "name", json_integer(i+2)); // +2 as we need node at 1 
      json_object_set(node, "mass", json_real(floorParams[i].mass));
      json_t *nodePosn = json_array();      
      json_array_append(nodePosn,json_real(floorParams[i].floor*storyheight));
      json_array_append(nodePosn,json_real(0.0));
      json_object_set(node, "crd", nodePosn);
      json_object_set(node, "ndf", json_integer(ndf));

      nodeMap = json_object();
      json_object_set(nodeMap,"cline",json_integer(1));
      json_object_set(nodeMap,"floor",json_integer(i+2));
      json_object_set(nodeMap,"node",json_integer(i+2));
      json_array_append(nodeMapping, nodeMap);

      json_object_set(element, "name", json_integer(interstoryParams[i].story));
      if (ndf == 1)
	json_object_set(element, "type", json_string("shear_beam"));
      else
	json_object_set(element, "type", json_string("shear_beam2d"));

      json_object_set(element, "uniaxial_material", json_integer(i+1));
      json_t *eleNodes = json_array();      
      json_array_append(eleNodes,json_integer(i+1));
      json_array_append(eleNodes,json_integer(i+2));
      json_object_set(element, "nodes", eleNodes);
      
      json_object_set(material,"name",json_integer(i+1));
      json_object_set(material,"type",json_string("shear"));
      json_object_set(material,"K0",json_real(interstoryParams[i].K0*kFactor));
      json_object_set(material,"Sy",json_real(interstoryParams[i].Sy));
      json_object_set(material,"eta",json_real(interstoryParams[i].eta));
      json_object_set(material,"C",json_real(interstoryParams[i].C));
      json_object_set(material,"gamma",json_real(interstoryParams[i].gamma));
      json_object_set(material,"alpha",json_real(interstoryParams[i].alpha));
      json_object_set(material,"beta",json_real(interstoryParams[i].beta));
      json_object_set(material,"omega",json_real(interstoryParams[i].omega));
      json_object_set(material,"eta_soft",json_real(interstoryParams[i].eta_soft));
      json_object_set(material,"a_k",json_real(interstoryParams[i].a_k));
      
      json_array_append(nodes,node);
      json_array_append(materials,material);
      json_array_append(elements, element);
    }
    
    json_object_set(properties,"dampingRatio",json_real(dampingRatio*dampFactor));
    json_object_set(properties,"uniaxialMaterials",materials)
    json_object_set(root,"Properties",propertiesn);

    json_object_set(geometry,"nodes",nodes);
    json_object_set(geometry,"elements",elements);
    json_object_set(root,"Geometry",geometry);

    json_object_set(root,"NodeMapping",nodeMapping);
    */

  // write the file & clean memory
  json_dump_file(root, path, 0);
  //json_dump_file(sam,path,0);
  json_object_clear(root);

  return 0;
}

int ConcreteShearWall::writeOpenSeesModel(const char *path)
{
  return 0;
}

int ConcreteShearWall::addNode(double x, double y, double z, json_t *theNodeArray)
{
  //
  // iterate over nodes to see if one exists, if not add one
  //

  int result = -1;
  for (std::list<Node>::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    double xLoc = it->xLoc;
    double yLoc = it->yLoc;
    double zLoc = it->zLoc;
    if (fabs(xLoc - x) < tol && fabs(yLoc - y) < tol && fabs(zLoc - z) < tol)
      result = it->tag;
  }

  if (result == -1)
  {
    numNode++;
    Node *newNode = new Node(numNode, x, y, z);
    nodes.push_back(*newNode);

    json_t *theNode = json_object();
    json_object_set(theNode, "name", json_integer(numNode));
    json_object_set(theNode, "ndf", json_integer(2));
    json_t *crdArray = json_array();
    json_array_append(crdArray, json_real(x));
    //json_array_append(crdArray, json_real(y));
    json_array_append(crdArray, json_real(z));
    json_object_set(theNode, "crd", crdArray);
    json_array_append(theNodeArray, theNode);

    return numNode;
  }
  return result;
}

int ConcreteShearWall::addNode(double x, double y, double z, char *floor, char *col, json_t *)
{
  return -1;
}

vector<double>
ConcreteShearWall::getCLineLoc(string cline)
{
  std::map<string, vector<double>>::iterator it;
  it = clines.find(cline);
  if (it != clines.end())
    return it->second;
  else
  {
    std::cerr << "ConcreteShearWall = floor not found: " << cline << "\n";
    std::vector<double> err(2);
    return err;
  }
}

double
ConcreteShearWall::getFloorLocation(string floor)
{
  std::map<string, double>::iterator it;
  it = floors.find(floor);
  if (it != floors.end())
    return it->second;
  else
  {
    std::cerr << "ConcreteShearWall = floor not found: " << floor << "\n";
    return 0;
  }
}

Material *
ConcreteShearWall::getMaterial(string name)
{
  std::map<string, Material *>::iterator it;
  it = theMaterials.find(name);
  if (it != theMaterials.end())
  {
    cout << "Found material '" << it->first << "' exists in theMaterials." << endl;
    return it->second;
  }
  else
  {
    std::cerr << "ConcreteShearWall = material not found: " << name << "\n";
    return 0;
  }
}

WallSection *
ConcreteShearWall::getWallSection(string name)
{
  std::map<string, WallSection *>::iterator it;
  it = theWallSections.find(name);
  if (it != theWallSections.end())
  {
    return it->second;
  }
  else
  {
    std::cerr << "ConcreteShearWall = material not found: " << name << "\n";
    return 0;
  }
}

int ConcreteRectangularWallSection::ndMaterialExistInArray(json_t *obj, json_t *array)
{
  if (json_array_size(array) < 1)
  {
    return 0;
  }
  else
  {
    double material = json_number_value(json_object_get(obj, "material"));
    double angle = json_number_value(json_object_get(obj, "angle"));
    
    json_t *ndlMat;
    size_t index;
    json_array_foreach(array, index, ndlMat)
    {

      double thisMaterial = json_number_value(json_object_get(ndlMat, "material"));
      double thisAngle = json_number_value(json_object_get(ndlMat, "angle"));
      int name = json_integer_value(json_object_get(ndlMat, "name"));

      if (abs(thisMaterial - material) < 1e-7 && abs(thisAngle - angle) < 1e-7) //(abs(thisunixialMat-uniaxialTag)<1e-7)
      {
        printf("This nd mat already exists. \n");
        return name;
      }
    }
  }
  return 0;
}