# NeuroScheme

## Introduction

NeuroScheme is a tool for navigating, exploring and interacting with
NeuroScience data using abstract, schematic or expressive representations. This
project is a refactor of https://gitlab.gmrv.es/nsviz/neuroscheme.git

## Dependencies

* Required dependencies:
    * ShiFT (requires FiReS and Boost) (1)
    * scoop (1)
    * Qt5
    * Eigen3

* Optional dependencies:
    * nsol: needed to load data via nsol. In order to load BlueConfig files nsol
      has to be built with Brion support. (2)
    * ZeroEQ & Lexis: enables syncing selections, syncing camera position and
      sending binary cell set operations (2)
    * (TODO) gmrvlex: enables sending focus messages
    * (TODO) Deflect: eanbles streaming the main window of the app

(1) Automatically download and build
(2) Automatically download and build uncommenting .gitsubprojects entry

In order to check which optional dependencies have been used the following
command can be used:

```bash
./NeuroScheme --version
```


## Building

NeuroScheme has been succesfully built and used on Ubuntu 16.04/17.04, Mac OSX
Yosemite/Sierra and Windows 7/8/10 Visual Studio 2015 Win64. The following steps should be
enough to build it just with the required dependencies.

```bash
git clone --recursive https://gitlab.gmrv.es/nsviz/NeuroScheme.git
mkdir NeuroScheme/build && cd NeuroScheme/build
cmake .. -DCLONE_SUBPROJECTS=ON -DCMAKE_BUILD_TYPE=Release
make
```

## Running

All the features should be accesible using the GUI but some of the are also
available through CLI. Running the following can provide a list of the CLI
arguments NeuroScheme accepts:

```bash
./NeuroScheme --help
```
