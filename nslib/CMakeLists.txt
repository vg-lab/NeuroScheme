set( NSLIB_SOURCES
  Canvas.cpp
  Config.cpp
  DataManager.cpp
  DomainManager.cpp
  FilterWidget.cpp
  InteractionManager.cpp
  Log.cpp
  PaneManager.cpp
  RepresentationCreatorManager.cpp
  ScatterPlotWidget.cpp
  SelectionManager.cpp
  SortWidget.cpp
  ZeroEQManager.cpp
  layouts/CircularLayout.cpp
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

set( NSLIB_HEADERS
  Canvas.h
  Color.h
  Config.h
  DataLoader.h
  DataManager.h
  DomainManager.cpp
  error.h
  FilterWidget.h
  InteractionManager.h
  Log.h
  PaneManager.h
  RepresentationCreatorManager.h
  ScatterPlotWidget.h
  SelectedState.h
  SelectionManager.h
  SortWidget.h
  ZeroEQManager.h
  layouts/CameraBasedLayout.h
  layouts/CircularLayout.h
  layouts/GridLayout.h
  layouts/Layout.h
  layouts/LayoutManager.h
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

set( NSLIB_LINK_LIBRARIES
  ShiFT
  scoop
  Qt5::Widgets
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
