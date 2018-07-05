### Testing Workflow Components

This directory contains a script for testing individual workflow components (e.g. the building model create scripts in createBIM)

The script ``RunTests.py`` will execute one or more already-configured workflow components and compare the results to a "reference" output file, 
and creating a file detailing the differences.

To run it, invoke it with one or more of the keywords for an already-configured component, e.g.

```bash
python RunTests.py genericbim
```

A log of the activity is written to STDOUT, and the actual output of the process is written to a logfile of the form ``workflow-log-YYYY-MM-DD-HH-MM-SS-utc.txt``.

A summary of the options for the script, in the form of a monologue:

```bash
$ pwd
..../WorkflowRegionalEarthquake/Workflow

$ python RunTests.py 

Syntax
    python RunTests.py <list of tests to run>

where <list of tests to run> is one or more of the following:

    createloss genericbim createsam adjustsam fixbim mdof-lu opensees edp simulation createevent

$ python RunTests.py createloss createsam genericbim fixbim mdof-lu opensees edp simulation createevent
2018-07-04T21:22:20Z ################################################################################
2018-07-04T21:22:20Z Start of test
2018-07-04T21:22:20Z ################################################################################
2018-07-04T21:22:20Z running test(s): createloss createsam genericbim fixbim mdof-lu opensees edp simulation createevent
2018-07-04T21:22:20Z running createloss
2018-07-04T21:22:20Z NON-ZERO RETURN CODE: -11
2018-07-04T21:22:20Z running createsam
2018-07-04T21:22:20Z running genericbim
2018-07-04T21:22:20Z running fixbim
2018-07-04T21:22:20Z running mdof-lu
2018-07-04T21:22:20Z running opensees
2018-07-04T21:22:20Z running edp
2018-07-04T21:22:20Z running simulation
2018-07-04T21:22:20Z running createevent
2018-07-04T21:22:20Z test output may be found in: workflow-log-2018-07-04-21-22-20-utc.txt
2018-07-04T21:22:20Z End of run.
```

### Adding components, or modifying existing components

The testable components are listed in a file called ``list_of_tests.json``, which is essentially an
ordered dictionary of possible tests. Each test must contain a few elements like the following:

```json
{
  "genericbim": {
    "directory": "../createBIM",
    "command": "../createBIM/GenericBimDatabase ../createBIM/buildings.json -Min 1 -Max 1 -buildingsFile ../createBIM/GenericBimDatabase.csv -getRV",
    "comment": "Run 'Generic BIM Creation' process to create a single building; save it in 1-BIM.json",
    "output": "1-BIM.json"
  }
}
```

Where ``genericbim`` is the keyword to use to invoke the test on the command line and the other attribute value pairs specify
the details of the test. 

The paths of all files and executables must be specified relative to the workflow directory.

(Currently, only ``command`` and ``output`` are used by the script.)

### Details of testable components

1. cd to create BIM directory and run the command:

``
./GenericBimDatabase buildings.json -Min 1 -Max 1 -buildingsFile GenericBimDatabase.csv -getRV
``

This creates the 1-BIM.json file

2. Open 1-BIM.json and replace "RV.height" with 3.0.

3. cd to createEVENT and run the command:

``
./LLNL_SW4 -filenameBIM ../createBIM/1-BIM.json -filenameEVENT 1-EVENT.json -filenameHFmeta HFmeta -pathSW4results ./Hayward7.0/ -getRV
``

This creates the 1-EVENT.json file

4. cd to createSAM and run:

``
./MDOF-LU -filenameBIM ../createBIM/1-BIM.json -filenameEVENT ../createEVENT/1-EVENT.json -filenameSAM 1-SAM.json -hazusData ./data/HazusData.txt -getRV
``

This creates a 1-SAM.json file (not similar to the attached one yet)

5. open 1-SAM.json and replace both "RV.kFactor" and "RV.dampFactor" with 1.0

6. run this command:

``
./MDOF-LU -filenameBIM ../createBIM/1-BIM.json -filenameEVENT ../createEVENT/1-EVENT.json -filenameSAM 1-SAM.json -hazusData ./data/HazusData.txt
``

This should modify the 1-SAM.json file to be similar to the attached one 

6. cd to createEDP and run this command:

``
./StandardEarthquakeEDP -filenameBIM ../createBIM/1-BIM.json -filenameEVENT ../createEVENT/1-EVENT.json -filenameSAM ../createSAM/1-SAM.json -filenameEDP 1-EDP.json -getRV
``

This creates a 1-EDP.json file (not similar to the attached one yet)

7. cd to performSIMULATION and run this command:

``
./OpenSeesSimulation.sh -filenameBIM ../createBIM/1-BIM.json -filenameSAM ../createSAM/1-SAM.json -filenameEVENT ../createEVENT/1-EVENT.json  -filenameEDP ../createEDP/1-EDP.json -filenameEDP 1-SIM.json -getRV
``

This creates the attached 1-SIM.json

8. run this command:

``
./OpenSeesSimulation.sh -filenameBIM ../createBIM/1-BIM.json -filenameSAM ../createSAM/1-SAM.json -filenameEVENT ../createEVENT/1-EVENT.json  -filenameEDP ../createEDP/1-EDP.json -filenameEDP 1-SIM.json
``

this should run opensees and update the 1-EDP.json file to be similar to the attached file

9. Finally, cd to createLOSS and run:

``
``

This creates a 1-DL.json file similar to the attached (may not be exactly the same values but very close)
values in the samples object may not match

