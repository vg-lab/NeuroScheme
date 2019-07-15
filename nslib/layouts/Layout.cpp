/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include "../Loggers.h"
#include "../error.h"
#include "../Config.h"
#include "../DataManager.h"
#include "Layout.h"
#include "../reps/Item.h"
#include "../reps/SelectableItem.h"
#include "../RepresentationCreatorManager.h"
#include "../reps/CollapseButtonItem.h"
#include "../SelectionManager.h"

namespace nslib
{

  LayoutOptionsWidget::LayoutOptionsWidget( void )
    : _layout( new QGridLayout )
  {
    this->setLayout( _layout );
  }

  QGridLayout* LayoutOptionsWidget::layout( void )
  {
    return _layout;
  }

  Layout::Layout( const std::string& name_,  unsigned int flags_,
    QWidget* layoutOptions_ )
    : _canvas( nullptr )
    , _flags( flags_ )
    , _optionsWidget( new LayoutOptionsWidget )
    , _name( name_ )
    , _toolbox( new QToolBox( _optionsWidget ))
    , _sortWidget( nullptr )
    , _filterWidget( nullptr )
    , _scatterPlotWidget( nullptr )
    , _layoutSpecialProperties( layoutOptions_ )
    , _isGrid( false )
  {
    _optionsWidget->layout( )->addWidget( _toolbox, 0, 0 );

    if ( _layoutSpecialProperties )
    {
      _toolbox->addItem( layoutOptions_, QString("Layout options"));
    }
    if ( _flags & SORT_ENABLED )
    {
      _sortWidget = new SortWidget( this );
      QIcon sortIcon( QString::fromUtf8( ":/icons/sort.png"));
      _toolbox->addItem( _sortWidget, sortIcon, QString( "Sort" ));
    }
    if ( _flags & FILTER_ENABLED )
    {
      _filterWidget = new FilterWidget( this );
      QIcon filterIcon( QString::fromUtf8( ":/icons/filter.png"));
      _toolbox->addItem( _filterWidget, filterIcon, QString( "Filter" ));
    }
    if ( _flags & SCATTERPLOT_ENABLED )
    {
      _scatterPlotWidget = new ScatterPlotWidget( this );
      QIcon filterIcon( QString::fromUtf8( ":/icons/filter.png" ));
      _toolbox->addItem( _scatterPlotWidget, filterIcon,
        QString( "ScatterPlot" ));
    }

    // _toolbox->show( );
    // _optionsWidget->show( );
  }

  Layout::~Layout( void )
  {
    delete _optionsWidget;
  }

  const std::string& Layout::name( void )
  {
    return _name;
  }

  LayoutOptionsWidget* Layout::optionsWidget( void )
  {
    return _optionsWidget;
  }

  void Layout::refresh( bool animate )
  {
    display( _canvas->allEntities( ), _canvas->reps( ), animate );
  }

  void Layout::display( shift::Entities& entities,
    shift::Representations& representations,
    bool animate )
  {
    Loggers::get( )->log(
      "display " + std::to_string( entities.size( )),
      LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );
    representations.clear( );

    bool doFiltering =
      _filterWidget &&
      !_filterWidget->filterSetConfig( ).filters( ).empty( );

    bool doSorting =
      _sortWidget &&
      !_sortWidget->sortConfig( ).properties( ).empty( );

    fires::Objects objects;
    shift::Representations preFilterRepresentations;
    shift::Representations relationshipReps;
    if ( doFiltering || doSorting )
    {
      for ( const auto& entity : entities.vector( ))
      {
        // If the entity is a sub-entity skip it
        if ( DataManager::entities( ).
             relationships( ).count( "isSubEntityOf" ) == 0 )
        {
          const auto& relSubEntityOf =
            *( DataManager::entities( ).
               relationships( )[ "isSubEntityOf" ]->asOneToOne( ));
          if ( relSubEntityOf.count( entity->entityGid( )) == 0 )
            objects.add( entity );
        }
        else
          objects.add( entity );
      }
      if ( doSorting )
      {
        fires::Sort firesSort;
        firesSort.eval( objects, _sortWidget->sortConfig( ));
      }

      fires::Objects objectsPreFilter = objects;
      if ( doFiltering )
      {
        fires::FilterSet firesFilterSet;
        firesFilterSet.eval( objects, _filterWidget->filterSetConfig( ));
      }

      shift::Entities filteredAndSortedEntities;
      for ( const auto& entity : objects )
        filteredAndSortedEntities.add( static_cast< shift::Entity* >( entity ));

      RepresentationCreatorManager::create(
        filteredAndSortedEntities, representations,
        true, true );

      shift::Entities entitiesPreFilter;
      for ( const auto& entity : objectsPreFilter )
        entitiesPreFilter.add( static_cast< shift::Entity* >( entity ));

      RepresentationCreatorManager::create(
        entitiesPreFilter, preFilterRepresentations,
        true, true );

      if ( Config::showConnectivity( ))
      {
        // Generate relationship representations
        RepresentationCreatorManager::generateRelations(
          filteredAndSortedEntities, relationshipReps, "connectsTo", false );
        RepresentationCreatorManager::generateRelations(
          filteredAndSortedEntities, relationshipReps,
          "aggregatedConnectsTo", true );
      }

    }
    else
    {
      // Default sorting by ShiFT's gid
      auto& vector = entities.vector( );
      std::sort( vector.begin( ), vector.end( ),
                 []( const shift::Entity* a, const shift::Entity* b )
                 { return b->entityGid( ) < a->entityGid( ); } );
      RepresentationCreatorManager::create(
       entities, representations,
        true, true );

      // std::cout << "-----" << entities.size( ) << " " << representations.size( ) << std::endl;

      if ( Config::showConnectivity( ))
      {
        // Generate relationship representations
        RepresentationCreatorManager::generateRelations( entities,
          relationshipReps, "connectsTo", false );
        RepresentationCreatorManager::generateRelations( entities,
          relationshipReps, "aggregatedConnectsTo", true );
      }
    }

    // shift::Representations relationshipReps;
    // // Generate relationship representations
    // nslib::RepresentationCreatorManager::generateRelations( entities,
    //                                                         relationshipReps,
    //                                                         "connectsTo" );

    _clearScene( );
    if ( doFiltering && _filterWidget->useOpacityForFiltering( ))
      _addRepresentations( preFilterRepresentations );
    else
      _addRepresentations( representations );


    if ( doFiltering && _filterWidget->useOpacityForFiltering( ))
    {
      _arrangeItems( preFilterRepresentations, animate, representations );
    }
    else
    {
      _arrangeItems( representations, animate );
    }

    if ( Config::showConnectivity( ))
    {
      _addRepresentations( relationshipReps );

      OpConfig opConfig( &_canvas->scene( ), animate, _isGrid );

      for ( auto& relationshipRep : relationshipReps )
      {
        relationshipRep->preRender( &opConfig );
      }
    }
  }

   void Layout::refreshWidgetsProperties(
    const TProperties& properties )
   {
     Loggers::get( )->log( "Refreshing property " + _name, LOG_LEVEL_VERBOSE,
                          NEUROSCHEME_FILE_LINE );

    if ( _sortWidget &&
         _sortWidget->propertiesSelector( ))
    {
      _sortWidget->clear( );
      auto selector = _sortWidget->propertiesSelector( );

      int i = -1;
      for ( const auto& prop : properties )
      {
        selector->insertItem( ++i, QString( prop.first.c_str( )));
      }
    }
    if ( _filterWidget &&
         _filterWidget->propertiesSelector( ))
    {
      _filterWidget->clear( );
      auto selector = _filterWidget->propertiesSelector( );
      int i = -1;
      for ( const auto& prop : properties )
      {
        selector->insertItem( ++i, QString( prop.first.c_str( )));
      }
    }

    if ( _scatterPlotWidget &&
         _scatterPlotWidget->propertyXSelector( ) &&
         _scatterPlotWidget->propertyYSelector( ))
    {
      // Block signals while refreshing the widget
      _scatterPlotWidget->blockSignals( true );
      _scatterPlotWidget->blockChildrenSignals( true );
      auto selectorX = _scatterPlotWidget->propertyXSelector( );
      auto selectorY = _scatterPlotWidget->propertyYSelector( );
      selectorX->clear( );
      selectorY->clear( );
      int i = -1;
      for ( const auto& prop : properties )
      {
        selectorX->insertItem( ++i, QString( prop.first.c_str( )));
        selectorY->insertItem( ++i, QString( prop.first.c_str( )));
      }
      _scatterPlotWidget->blockChildrenSignals( false );
      _scatterPlotWidget->blockSignals( false );

    }

  }

  void Layout::_drawCorners( )
  {
    NEUROSCHEME_DEBUG_CHECK( _canvas->scene( ).views( ).size( ) != 0,
                             "Scene with no view" );
    QGraphicsView* gv = _canvas->scene( ).views( )[0];

    // std::cout << gv->width( ) << " x " << gv->height( ) << std::endl;
    QGraphicsEllipseItem* center =
      new QGraphicsEllipseItem( -10, -10, 20, 20 );
    _canvas->scene( ).addItem( center );
    QGraphicsEllipseItem* tl =
      new QGraphicsEllipseItem( -gv->width( ) / 2 - 10,
                                -gv->height( ) / 2 - 10,
                                20, 20 );

    _canvas->scene( ).addItem( tl );
    QGraphicsEllipseItem* br =
      new QGraphicsEllipseItem( gv->width( ) / 2 - 10,
                                gv->height( ) / 2 - 10,
                                20, 20 );

    _canvas->scene( ).addItem( br );
  }

  void Layout::_clearScene( void )
  {
    // Remove top items without destroying them
    for ( auto& item : _canvas->scene( ).items( ))
    {
      if ( dynamic_cast< Item* >( item ) && !item->parentItem( ))
      {
        _canvas->scene( ).removeItem( item );
      }
    }

    // // Remove the rest
    // for ( auto& item : _canvas->scene( ).items( ))
    // {
    //   _canvas->scene( ).removeItem( item );
    // }


    // Remove the rest
    // NOTE: removed this clear because it delete de objects
    //_canvas->scene( ).clear( );
  }

  void Layout::_addRepresentations( const shift::Representations& reps )
  {
    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    for ( const auto representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< nslib::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        Loggers::get( )->log( "Item null", LOG_LEVEL_WARNING );
      }
      else
      {
        // std::cout << "+++++ Retrieving item" << std::endl;
        auto item = graphicsItemRep->item( &_canvas->scene( ));

        if ( item->parentItem( ))
        {
          continue;
        }

        // Find out if its entity is selected
        // and if so set its pen
        // const auto& repsToEntities =
        //   RepresentationCreatorManager::repsToEntities( );

        if ( repsToEntities.count( representation ) > 0 )
        {
          const auto entities = repsToEntities.at( representation );
          if ( entities.empty( ))
            Loggers::get( )->log(
              "No entities associated to representation",
              LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

          auto selectableItem = dynamic_cast< SelectableItem* >( item );
          if ( selectableItem )
          {
            auto selectedState = SelectionManager::getSelectedState(
              *entities.begin( ));

            // if ( selectedState == selectedStateSelectedState::SELECTED )
            selectableItem->setSelected( selectedState );


            auto shapeItem =
              dynamic_cast< QAbstractGraphicsShapeItem* >( item );
            if ( shapeItem )
            {
              if ( selectedState == SelectedState::SELECTED )
                shapeItem->setPen( SelectableItem::selectedPen( ));
              else if ( selectedState == SelectedState::PARTIALLY_SELECTED )
                shapeItem->setPen(
                  SelectableItem::partiallySelectedPen( ));
            }
          }
        }
        //std::cout << &scene << " add item " << std::endl;
        // std::cout << "Adding item" << item << std::endl;

        if ( !item->parentItem( ))
          _canvas->scene( ).addItem( item );
      }
    }
  } // _addRepresentations

  void Layout::updateSelection( void )
  {
    QList< QGraphicsItem* > items_ = _canvas->scene( ).items( );
    for ( auto qitem = items_.begin( ); qitem != items_.end( ); ++qitem )
    {
      auto selectableItem_ = dynamic_cast< SelectableItem* >( *qitem );
      if ( selectableItem_ )
      {
        auto item = dynamic_cast< Item* >( *qitem );
        if ( !item ) continue;
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );

        // if ( item->parentRep( )) {
        //   std::cerr << "Item without parent" << std::endl;
        //   continue;
        // }
        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          // Check only the first entity, this might not be ok
          // TODO check this
          if ( !entities.empty( ))
          {
            const auto& entity = *entities.begin( );
            auto shapeItem =
              dynamic_cast< QAbstractGraphicsShapeItem* >( item );

            const auto state = SelectionManager::getSelectedState( entity );
            selectableItem_->setSelected( state );
            if ( state == SelectedState::UNSELECTED )
              shapeItem->setPen( SelectableItem::unselectedPen( ));
            else if ( state == SelectedState::SELECTED )
              shapeItem->setPen( SelectableItem::selectedPen( ));
            else if ( state == SelectedState::PARTIALLY_SELECTED )
              shapeItem->setPen(
                SelectableItem::partiallySelectedPen( ));

          }
        }
      }
    }
  }

  void Layout::_updateOptionsWidget( void )
  {
  }

  void Layout::animateItem( QGraphicsItem* graphicsItem,
                            float toScale, const QPoint& toPos )
  {
    auto obj = dynamic_cast< QObject* >( graphicsItem );
    auto item = dynamic_cast< Item* >( graphicsItem );

    auto& scaleAnim = item->scaleAnim( );
    if ( scaleAnim.state( ) == QAbstractAnimation::Running )
    {
      scaleAnim.stop( );
      assert( scaleAnim.state( ) == QAbstractAnimation::Stopped );
      scaleAnim.setStartValue( scaleAnim.currentValue( ));
      scaleAnim.setDuration( scaleAnim.currentTime( ));
      scaleAnim.setCurrentTime( 0 );
    }
    else
    {
      assert( scaleAnim.state( ) == QAbstractAnimation::Stopped );
      scaleAnim.setTargetObject( obj );
      scaleAnim.setDuration( ANIM_DURATION );
      scaleAnim.setStartValue( graphicsItem->scale( ));
    }
    scaleAnim.setEndValue( toScale );

    auto& posAnim = item->posAnim( );
    if ( posAnim.state( ) == QAbstractAnimation::Running )
    {
      posAnim.stop( );
      assert( posAnim.state( ) == QAbstractAnimation::Stopped );
      posAnim.setStartValue( posAnim.currentValue( ));
      posAnim.setDuration( posAnim.currentTime( ));
      posAnim.setCurrentTime( 0 );
    }
    else
    {
      assert( posAnim.state( ) == QAbstractAnimation::Stopped );
      posAnim.setTargetObject( obj );
      posAnim.setDuration( ANIM_DURATION );
      posAnim.setStartValue( graphicsItem->pos( ));
    }
    if ( toPos.x( ) != toPos.x( ) ||
         toPos.y( ) != toPos.y( ))
      posAnim.setEndValue( QPoint( 0, 0 ));
    else
      posAnim.setEndValue( toPos );

    posAnim.start( );
    scaleAnim.start( );

  }

  void Layout::refreshCanvas( void )
  {
    _canvas->displayEntities( false, false );
  }

}
