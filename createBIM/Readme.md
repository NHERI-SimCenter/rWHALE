## Creating Building Information Models (createBIM)

A createBIM application is a tool that reads building information from a database and creates a BIM file in JSON format. The application should be able to create BIM files for a single building or a range of buildings in a given region.

An example BIM file obtained by a createBIM application will contain the following information:

```json
{
    "RandomVariables": [
        {
            "mean": 3,
            "distribution": "normal",
            "stdDev": 0.16667000000000001,
            "name": "height",
            "value": "RV.height"
        }
    ],
    "GI": {
        "area": 288.58525817400027,
        "name": "7",
        "structType": "W1",
        "numStory": 1,
        "yearBuilt": 2008,
        "occupancy": "Residential",
        "height": "RV.height",
        "location": {
            "latitude": 37.462235730000003,
            "longitude": -121.9172232
        },
        "replacementCost": 59540.275578891749,
        "replacementTime": 180
    }
}
```

Currently, there are two different implementations of createBIM included with the workflow.

### UrbanSimDatabase

In collaboration with [UrbanSIM](http://www.urbansim.com/home/), the SimCenter is providing a buildings dataset for SF Bay Area (1.8 million buildings). Sample files for 100 building are distributed with the repository. UrbanSim database includes two files:

* **Buildings File**: This is a csv file that contains building information such as area, number of stories, year built, building occupancy and parcel id.

* **Parcels File**: This file provides information about the parcels containing the buildings, including the geographic location (latitude and longitude).

The application reads the building information from the buildings file, and uses the parcel id to obtain the parcel location from the parcel file.

In UrbanSim buildings file, the occupancy is represented using an integer, building occupancy and replacement costs in 2017 $U.S. are mapped to the occupancy id according to this table:

|Id |Occupancy|Replacement Cost <br/>[$U.S./Sqft.] |Contents Factor <br/> [%]|
|:---|:---------|:---:|:---:|
1-3,12  |Residential    |137.5452|50
4,14    |Office         |131.8863|100
5       |Hotel          |137.271225|50
6       |School         |142.134265|125
7       |Industrial     |97.5247|150
8       |Industrial     |85.9586|150
9       |Industrial     |104.033475|150
10,11,13|Retail         |105.33705|100
Default |Residential    |137.5452|50


### GenericBimDatabase

This database application is a generic application that only requires the minimal set of information needed for each building. The application uses as its input a single csv file that contains building Id, area, number of stories, year built, occupancy and location. Sample GenericDatabase.csv file is distributed with this repository.

Except for the simplified format of inputs, the implementation of this application is similar to UrbanSimDatabase, that includes mapping occupancy, replacement cost, structure type,...etc.
