## Creating Building Information Models (createBIM)

A createBIM application is a tool that reads building information from a database and creates a BIM file in JSON format. The application should be able to create BIM files for a single building or a range of buildings in a given region. Currenlty, there are two different implementations of createBIM included with the workflow.

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

### UrbanSimDatabase

In collaboration with [UrbanSIM](http://www.urbansim.com/home/), the SimCenter is providing a buildings dataset for SF Bay Area (1.8 million buildings). Sample files for 100 building are distributed with the repository. UrbanSim database includes two files:

* **Buildings File**: This is a csv file that contains building information such as area, number of stories, year built, building occupancy and parcel id.

* **Parcels File**: This file contain information about the parcels conatining the buildings, including the geographic location (latitude and longitude).

The application reads the building information from the buildings file, and uses the parcel id to obtain the parcel location from the parcel file.

In UrbanSim buildings file, the occupancy is represented using an integer, building occupancy and replacement costs are mapped to the occupancy id according to this table:

|Id |Occupancy|Replacement Cost/Unit Area|
|---|---------|---|
1-3,12  |Residential    |137.5452*(1+0.5)|
4,14    |Office         |131.8863*(1+1)
5       |Hotel          |137.271225*(1+0.5)
6       |School         |142.134265*(1+1.25)
7       |Industrial     |97.5247*(1+1.5)
8       |Industrial     |85.9586*(1+1.5)
9       |Industrial     |104.033475*(1+1.5)
10,11,13|Retail         |105.33705*(1+1)
Default |Residential    |137.5452*(1+0.5)
|

### GenericBimDatabase:

This database application is a generic application that only requires the minimal set of information needed for each building. The application uses as its input a single csv file that contains building Id, area, number of stories, year built, occupancy and location. Sample GenericDatabase.csv file is distributed with this repository.

Except for the simplified format of inputs, the implementation of this application is similar to UrbanSimDatabase, that includes mapping occupany, replacement cost, structure type,...etc.
