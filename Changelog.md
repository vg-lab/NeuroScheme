# NeuroScheme

## git master

## version 0.4

* [!106] Release 0.4 preparation.
* [!105] Changed GMRV logo to VG-Lab logo. Added dev
* [!104] Fix warnings. Bump ShiFT. Added the option to enable nsol from cmake CLI
* [!103] Changed gmrvvis to vglab.
* [!95] Cortex bugs
* [!100] Clean canvas
* [!99] Connections with same origin and destination are not draw.
* [!98] Fixes for Qt-5.12 and minor fix in AppImage link in README.md.
* [!97] Free layout recalculates scene bounding rect.
* [!96] New no hierarchical entity: Output. Rename stimulator to input
* [!94] Scatter plot layout use 0 for undefined properties.
* [!92] Access to edit connection from representations.
* [!89] Aggregated connections are higlighted with different colors.
* [!88] Improvements to 3D/Camera Layout
* [!91] Configure CI to generate git-master app.
* [!90] Changed build deploy only for master-based tags.

## version 0.3

* [!86] Release 0.3 preparation.
* [!87] Circulat Layout now supports a single entity.
* [!85] Added Windows and OSX CI bundle creation and upload.
* [!84] Added buildbot creation of an AppImage and its upload to GitHub.
* [!82] Improved README.
* [!81] Update NeuronPop and Stimulator icons. Bump ShiFT.
* [!78] New icons for congen domain.
* [!76] Added a free movement layout.
* [!74] Added entities outside hierarchy. Fixed bugs. Bump dependencies.
* [!79] Changed LICENSE file to LGPLv3 to match license on source and header files.
* [!77] Added push to github in .gitlab-ci.yml.
* [!72] Aggregated connections.
* [!73] Updated nsol tag, brion tag and common submodule tag.
* [!71] Added .json extension in exportToJSON method.
* [!70] Import/export the scene from/to JSON.
* [!68] Congen parent relationship.
* [!69] Fixed typo on stats data loading process for cortex domain.
* [!67] Fixed bug in cortex establishing parent relationships.
* [!66] Bump Brion and ShiFT.
* [!65] Uniqueness support for entity and relationship names.

## version 0.2

* [!64] Updated Changelog and version. Release 0.2 preparation
* [!63] Fixed style of load menu using static QFileDialog::getOpenFileName. Fixed return value of DataLoader::loadNeuroML.
* [!62] First approach to auto-computed properties
* [!60] NeuroML maximums bug fix
* [!54] Auto-connections implementation
* [!59] Fixed hexagon bottom line in congen
* [!57] Bugfix: Unreferenced memory
* [!56] Improved NeuroML congen reader.
* [!55] New UI for pane Split/Close
* [!47] Improvements on README.md
* [!53] Added focus on selection. Bump nsol and deps. Changed icons with svg tango-based ones. Added menu entries for loading BlueConfig and XML in cortex and NeuroML in congen.
* [!52] Fixed hexagon points in NeuronPopItem.
* [!51] Fixed small bug from congen building.
* [!49] Changed log to silo.
* [!48] Remove warnings
* [!45] Rewrite about dialog to make it verbose about built components version.
* [!46] Bump ShiFT.
* [!44] Added input dialog for selection domain in case its not defined via CLI.
* [!42] Member shadowing bugfix
* [!43] Added warning in case no domain is defined via CLI. Fixes for Log.
* [!41] Bump nsol
* [!40] OSX Sierra fixes and moved CI config to Sierra.
* [!10] Added first implementation for congen domain
* [!38] Added contraints for subproperties in congen
* [!37] Added basic NeuroML exportation for congen
* [!39] Added a line following the mouse movement while defining a new projection.
* [!35] Fixed bug clearing reps cache
* [!34] Added drag&drop for relationship creation.
* [!33] Fixed bug that crashed loading morphologies with out connectivity.
* [!32] Bump CMake/common and use it as a submodule.
* [!31] Added subentities creation.
* [!30] In cortex added update of maximums when new neuron is added or edited.
* [!29] Added edit or new entiy update virtual method.
* [!27] Entity edit widget into a new dock. Create or copy N entities at the same time. Added label to the EntityEditWidget.
* [!26] Added scale CLI input parameter for hdpi scaling.
* [!25] Checking unique values on editor.
* [!24] Added loading entity types in domain. Added first version of editing and creating capabilities for entities.
* [!23] Bump ShiFT (and thus FiReS) and moved Windows CI to VS14.
* [!22] Connectivity highlight and multi-pane synced hover.
* [!21] Basic representation of connectivity.
* [!11] Added nsol Xml scene loading via CLI.
* [!9] Added CLI help message.
* [!8] Enabled building in Windows without console. Separate lib, GUI app and plugins.
* [!7] Bumped dependencies versions. Updated README.md.
* [!6] Added home button. Added circular layout. Fixed bugs.
* [!5] Added scoop for color mappers.
* [!4] Enabled horizontal and vertical splitting of panes.


# version 0.1

* First version of the refactor of legacy neuroscheme.
