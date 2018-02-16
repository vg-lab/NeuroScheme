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
    * (TODO) Deflect: enables streaming the main window of the app

(1) Automatically download and build
(2) Automatically download and build uncommenting .gitsubprojects entry

In order to check which optional dependencies have been used the following
command can be used:

```bash
./NeuroScheme --version
```


## Building

NeuroScheme has been successfully built and used on Ubuntu 16.04/17.04, Mac OSX
Yosemite/Sierra and Windows 7/8/10 Visual Studio 2015 Win64. The following steps should be
enough to build it just with the required dependencies.

```bash
git clone --recursive https://gitlab.gmrv.es/nsviz/NeuroScheme.git
mkdir NeuroScheme/build && cd NeuroScheme/build
cmake .. -DCLONE_SUBPROJECTS=ON -DCMAKE_BUILD_TYPE=Release
make
```

## Running

At the moment NeuroScheme running options have to be defined via command line
interface. To take a look to the arguments NeuroScheme accepts you can run the
following:

```bash
./bin/NeuroScheme --help
```

NeuroScheme accepts different domains (in example, cortex and congen domains)
which can be selected when GUI is started or via the "-d" parameter. Each domain
can have each own specific parameters, which can be displayed with --help
argument.

### Loading and exporting data

#### Cortex domain

Loading data in cortex domain can be done from both nsol's XML scene and
BlueBrain's BlueConfig files. If you have built NeuroScheme with nsol support
(explained in the [Dependencies](#dependencies) section) you can
try to visualize a very simple scene by running:

```bash
./bin/NeuroScheme -d cortex -xml nsol/testData/scene.xml
```

Cortex domain does not support exporting data.

#### Congen domain

Congen domain at the moment only allows creating data but does not support
loading data yet (soon we hope!).

### Interacting with data

Once a dataset has been loaded you can navigate through the hierarchy or
visualize the grouped items right-clicking in the representation and selecting
the proper option (if available in the specific dataset). The new visualization
can be shown in the same panel or in a new one (horizontal or vertical pane
splitting can be selected under "Panes" menu).

Clicking in the toolbar "gear" icon a side panel is shown containing the options
regarding layouts, filtering and sorting capabilities.

Clicking in the toolbar "drawer" icon a side panel is shown containing the
options regarding stored selections.

### Creating data

The creating feature is still in early development stage. If you want to try it
out you can start with a clear canvas or load a dataset as specified before. To
create entities you can click on the background of the canvas, select the type
of entity and define its properties in the side panel. To edit or duplicate an
entity right click on its representation and select the apropriated option. You
can also define connection relationships between entities using drag and
drop. To visualize connections you have to enable the option in the toolbar or
under the menu "Scene".
