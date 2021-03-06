set( SILO_NAMESPACE nslib )
set( SILO_NAME NEUROSCHEME )
configure_file( ${CMAKE_SOURCE_DIR}/deps/silo/silo.in.hpp
  ${CMAKE_BINARY_DIR}/include/nslib/Logger.hpp )

set( NSLIB_PUBLIC_HEADERS
  ${CMAKE_BINARY_DIR}/include/nslib/Logger.hpp
  Canvas.h
  Color.h
  Config.h
  ConnectionRelationshipEditWidget.h
  ConnectionTableWidget.h
  DataLoader.h
  DataManager.h
  Domain.h
  DomainManager.h
  EntityConnectionListWidget.h
  EntityEditWidget.h
  error.h
  FilterWidget.h
  InteractionManager.h
  ItemText.h
  Loggers.h
  PaneManager.h
  RepresentationCreatorManager.h
  ScatterPlotWidget.h
  SelectedState.h
  SelectionManager.h
  SortWidget.h
  ZeroEQManager.h
  layouts/CameraBasedLayout.h
  layouts/CircularLayout.h
  layouts/FreeLayout.h
  layouts/GridLayout.h
  layouts/Layout.h
  layouts/ScatterPlotLayout.h
  mappers/VariableMapper.h
  qxt/qxtspanslider.h
  qxt/qxtspanslider_p.h
  reps/CollapsableItem.h
  reps/CollapseButtonItem.h
  reps/InteractiveItem.h
  reps/Item.h
  reps/QGraphicsItemRepresentation.h
  reps/RingItem.h
  reps/SelectableItem.h
  )

set( NSLIB_SOURCES
  Canvas.cpp
  Config.cpp
  ConnectionRelationshipEditWidget.cpp
  ConnectionTableWidget.cpp
  DataManager.cpp
  Domain.cpp
  DomainManager.cpp
  EntityConnectionListWidget.cpp
  EntityEditWidget.cpp
  FilterWidget.cpp
  InteractionManager.cpp
  ItemText.cpp
  Loggers.cpp
  PaneManager.cpp
  RepresentationCreatorManager.cpp
  ScatterPlotWidget.cpp
  SelectionManager.cpp
  SortWidget.cpp
  ZeroEQManager.cpp
  layouts/CircularLayout.cpp
  layouts/FreeLayout.cpp
  layouts/GridLayout.cpp
  layouts/Layout.cpp
  layouts/CameraBasedLayout.cpp
  layouts/ScatterPlotLayout.cpp
  mappers/VariableMapper.cpp
  reps/CollapseButtonItem.cpp
  reps/RingItem.cpp
  reps/SelectableItem.cpp
  qxt/qxtspanslider.cpp
  )

set( NSLIB_LINK_LIBRARIES
  ShiFT
  scoop
  Qt5::Widgets
  Qt5::Xml
  )

if ( TARGET ZeroEQ AND TARGET Lexis AND TARGET Servus )
  list( APPEND NSLIB_LINK_LIBRARIES Lexis ZeroEQ Servus )
endif( )

if ( TARGET gmrvlex )
  list( APPEND NSLIB_LINK_LIBRARIES gmrvlex )
endif( )

if ( TARGET nsol )
  list( APPEND NSLIB_LINK_LIBRARIES nsol )
endif( )


set( NSLIB_INCLUDE_NAME nslib )
set( NSLIB_NAMESPACE nslib )
common_library( nslib )
