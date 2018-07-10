# Workflow 1.1

This repository contains workflow applications to carry out a regional loss estimaion due to an earthquake event. The provided applications are organized in subdirectories:

1. **[createBIM](./createBIM/Readme.md)**: This directory contains applications that can obtain a Building Information Model (BIM) from a database of buildings. Some examples of these applications are:

   * **UrbanSimDatabase**: This application generates BIM models given UrbanSim buildings and parcels csv files.

   * **GenericBimDatabase**: This application generates BIM models given a generic database in csv format with a minimal set of information about each building.
   
   More detailed documentation for the createBIM applications is available [here](./createBIM/Readme.md)

2. **createEVENT**: This directory contains applications to generate hazard event input given BIM.

   * **LLNL-SW4**: This application will read the building location from the BIM model and will output ground motion for simulating building response. The application uses a set of precomputed ground motions for SF Bay Area earthquake event (Magnitude 7.0 earthquake at the Hayward fault) simulated using [SW4](https://geodynamics.org/cig/software/sw4/).

   * **SHA-GM** [Experimental]: This application combines seismic hazard analysis (SHA) and ground motion record selection/scaling to provide ground motions for structural analysis based on a user-defined earthquake scenarios.

3. **createSAM**:  This directory contains applications to create strctural analysis model (SAM) given BIM and Event data

   * **MDOF_LU**:  This application use Hazus data to generate a MDOF non-linear shear building model. 

   * **AI-M-3** [Under Development]: This is an application that is currently under development that uses artificial intelligence to obtain SAM from BIM.

4. **createEDP**: This directory contains applications that generates a list of the required Engineering Demand Parameters (EDPs) to be obtained from structural analysis

5. **performSIMULATION**: applications in this directory will perform preprocessing of SAM, structural analysis and post processing to obtain EDP values

6. **performUQ**: This directory contains scripts and applications to introduce uncertainties in the workflow applications. Current implementation relies on DAKOTA for handling uncertainties.

7. **createLOSS**:  Applications in this directory are used to determine damage and loss.
   * **FEMA_P58_LU**: Damage and loss assessment tool that uses FEMA-P58 methodology. This tool was developed by Prof. Xinzheng Lu's research group at Tsinghua University [1].
   * **SimCenter_P58** [Under developmeent]: an efficient damage and loss assessment tool using FEMA-P58 is currenlty under development.

8. **Workflow**: This directory contains scripts and examples to run the workflow with different configurations.

## Dependencies

* **Jansson Library**: Many of the provided applications require the [Jansson library](http://www.digip.org/jansson/) to be installed. Jansson is a free native C library for encoding, decoding and manipulating JSON data. It is licensed under the MIT license. For *nix systems, it is assumed installed in /usr/local/jansson as seen in the included Makefiles.

* **C++11 compliant compiler**: Some applications in the createLOSS and performUQ folders use C++11 features, consequently they may need a newer C++11 compliant compiler.

* **FEM and UQ Applciations**: The workflow applications require an installation of [OpenSees](http://opensees.berkeley.edu/) to carry out structural analysis and [DAKOTA](https://dakota.sandia.gov/) to handle uncertainties.

* **Python**: The workflow requires Python 2.7.

* **Perl**: The workflow makes use of a modified version of the `DPrePro` preprocessor Perl script distributed with DAKOTA, and thus requires Perl. Later releases may switch to the newer python version of the script as the Perl script is deprecated as of DAKOTA 6.8.

## Building from source on *nix systems
The repository uses Makefiles to build the applications.
To build all the workflow applications, use ```make``` or ```make all``` in the root directory. To build for debugging use ```make debug```  and to clean, use ```make clean```.

## Building from source on Windows
Visual studio projects is under development. Currently the workflow can be built and run in windows using the Windows subsystem for linux and building using the same commands for *nix systems.

## Data

Some data files required to run the workflow are not included in this repository, and is distributed using the [SimCenter Box account](https://berkeley.box.com/s/7es601ve766fprph88n67khfip7fqupd). If you cannot access the data, please contact the SimCenter using [Slack](https://designsafe-ci.slack.com/messages/C92HT3GG4) or by [email](nheri-simcenter@berkeley.edu).

* **BIM Datasets**: The workflow is distributed with a sample UrbanSim data to run a sample number of buildings, e.g. sample buildings.csv and parcels.csv files for UrbanSimDatabase with 100 buildigns. Data for SF Bay Area is available only for the GenericBimDatabase application and can be obtained from the SimCenter Box account.

* **Ground motion**: files for the LLNL Hayward 7.0 scenario is available through the SimCenter Box account.



## Pegasus Workflow

The Pegasus workflow can be found in the workflow/ directory. 

Before submitting, please run `make` to make sure all the executables
required by the workflow have been built.

The workflow is set up to execute in two different execution
environments: local HTCondor pool, Stampede 2 or TACC Wrangler. The first
is good for small to medium sized runs as turnaround time is usually
pretty good. Large workflows should be mapped to TACC Wrangler.

Submitting a workflow for local HTCondor pool execution can be done
by executing:

```shell
    ./submit-workflow
```

Submitting to TACC Stampede2 can be done on the workflow.isi.edu host with the command:

```shell
    ./submit-workflow stampede2
    ./submit-workflow wrangler
```


## Testing Workflow Components

If you wish to try out existing workflow components with new parameters, or to
develop new components, you may wish to consider using the 'test harness' provided in this repo.
See the [README](./WorkflowREADME.md) in the ``Workflow`` directory for details.

## References
[1] Zeng X., Lu X.Z., Yang T., Xu Z., "Application of the FEMA-P58 methodology for regional earthquake loss prediction", Natural Hazards (2016), 10.1007/s11069-016-2307-z