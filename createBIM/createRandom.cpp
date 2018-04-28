// createBIM.c
// purpose - give the building.csv and parcel.csv files create a BIM model
// written: fmckenna

// r=randi(1843351,1,500);
// r=sort(r);
// fileID = fopen('random.txt','w');
// fprintf(fileID,"%d\n",r);
// fclose(fileID)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, const char **argv) {
  std::ifstream input( "random.txt" );
  std::ifstream buildingsIN( "buildings.csv" );
  std::ofstream buildingsOUT( "randomBuildings.csv" );

  int value = 0;
  int count = 0;
  string line;

  buildingsIN >> line;
  buildingsOUT << line;
  
  while(input >> value) {
    
    while (count <= value) {
      //std::cerr << count << " value->" << value << "<-----line------>" << line <<" <--\n";
      count++;
      getline(buildingsIN,  line);
    }
   
    std::cerr << "value->" << value << "<-----line------>" << line <<" <--\n";

    buildingsOUT << line << "\n";
  }
  input.close();
  buildingsIN.close();
  buildingsOUT.close();
		    
}
