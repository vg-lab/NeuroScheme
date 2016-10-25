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
#include "error.h"
#include "Layout.h"
#include "reps/Item.h"
#include "reps/SelectableItem.h"
#include "RepresentationCreatorManager.h"
namespace neuroscheme
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

  Layout::Layout( const std::string& name_,
                  unsigned int flags_ )
    : _flags( flags_ )
    , _optionsWidget( new LayoutOptionsWidget )
    , _name( name_ )
    , _toolbox( new QToolBox( _optionsWidget ))
  {
    _optionsWidget->layout( )->addWidget( _toolbox, 0, 0 );

    _sortWidget = 0;
    _filterWidget = 0;

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

  void Layout::displayEntities( const shift::Entities& entities,
                                bool animate,
                                bool refreshProperties )
  {
    _entities = entities;
    _representations.clear( );

    if ( _sortWidget &&
         _sortWidget->sortConfig( ).properties( ).size( ) > 0 &&
         !refreshProperties )
    {
      std::cout << "//Sorting using fires " << std::endl;
      std::cout << "//Num properties "
                << _sortWidget->sortConfig( ).properties( ).size( )
                << std::endl;
      fires::Sort firesSort;
      fires::Objects objects;
      //fires::FilterSet _firesFilterSet;

      for ( const auto& entity : _entities.vector( ))
        objects.add( entity );
      firesSort.eval( objects, _sortWidget->sortConfig( ));

      shift::Entities sortedEntities;
      for ( const auto& entity : objects )
        sortedEntities.add( static_cast< shift::Entity* >( entity ));

      neuroscheme::RepresentationCreatorManager::create(
        sortedEntities, _representations,
        true, true );
    }
    else
    {
      std::cout << "Sorting by entitygid " << std::endl;
      auto& vector = _entities.vector( );
      std::sort( vector.begin( ), vector.end( ),
                 []( shift::Entity*& a, shift::Entity*& b )
                 { return b->entityGid( ) < a->entityGid( ); } );
      neuroscheme::RepresentationCreatorManager::create(
        _entities, _representations,
        true, true );
    }
    // _representations = reps;
    // if ( reps.empty( ))
    //   return;

    if ( refreshProperties )
    {
      std::cout << "Refresh properties" << std::endl;
      _refreshProperties( );
    }
    if ( !animate )
    {
      std::cout << "Clear scene" << std::endl;
      _clearScene( );
    }
    _drawCorners( );
    if ( !animate )
    {
      std::cout << "Adding reps" << std::endl;
      _addRepresentations( _representations );
    }
    _arrangeItems( _representations, animate );
  }

  // void Layout::displayItems( const shift::Representations& reps,
  //                            bool animate )
  // {
  //   _representations = reps;
  //   if ( reps.empty( ))
  //     return;

  //   _refreshProperties( );
  //   _clearScene( );
  //   _drawCorners( );
  //   _addRepresentations( reps );
  //   _arrangeItems( reps, animate );
  // }

  void Layout::_refreshProperties( void )
  {
    fires::Objects objs;
    fires::Objects filteredObjs;
    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    _properties.clear( );

    for ( const auto& representation : _representations )
    {
      const auto entities = repsToEntities.at( representation );
      if ( entities.size( ) < 1 )
        Log::log( NS_LOG_HEADER +
                  "No entities associated to representation",
                  LOG_LEVEL_ERROR );
      const auto& entity = *( entities.begin( ));

      objs.push_back( entity );

      for ( auto property_ : entity->properties( ))
      {
        // Check if property is scalar
        if ( fires::PropertyManager::getAggregator( property_.first ))
        {
          if ( _properties.find( property_.first ) == _properties.end( ))
            _properties[ property_.first  ] = TPropertyData{ 0, 0 };
        }
      }
    }

    if ( _sortWidget &&
         _sortWidget->propertiesSelector( ))
    {
      _sortWidget->clear( );
      auto selector = _sortWidget->propertiesSelector( );
      std::cout << selector << std::endl;
      int i = -1;
      for ( const auto& prop : _properties )
      {
        //if ( !std::is_scalar< p.first >( ))
        // if ( !fires::PropertyManager::getAggregator( p.first ))
        // std::cout << prop.first << std::endl;
        selector->insertItem( ++i, QString( prop.first.c_str( )));
        // std::cout << "selector->insertItem( " << i << ", QString( " << prop.first.c_str( ) << ")); " << std::endl;
      }
    }
    if ( _filterWidget &&
         _filterWidget->propertiesSelector( ))
    {
      _filterWidget->clear( );
      auto selector = _filterWidget->propertiesSelector( );
      std::cout << selector << std::endl;
      int i = -1;
      for ( const auto& prop : _properties )
      {
        //if ( !std::is_scalar< p.first >( ))
        // if ( !fires::PropertyManager::getAggregator( p.first ))
        // std::cout << prop.first << std::endl;
        selector->insertItem( ++i, QString( prop.first.c_str( )));
        // std::cout << "selector->insertItem( " << i << ", QString( " << prop.first.c_str( ) << ")); " << std::endl;
      }
    }

    fires::AggregateConfig aggregateConfigMax;
    fires::AggregateConfig aggregateConfigMin;

    for ( const auto& p : _properties )
    {
      aggregateConfigMax.addProperty(
        p.first,
        fires::PropertyManager::getAggregator( p.first ),
        fires::PropertyAggregator::MAX );
      aggregateConfigMin.addProperty(
        p.first,
        fires::PropertyManager::getAggregator( p.first ),
        fires::PropertyAggregator::MIN );

      }

      fires::Aggregate aggregate;
      fires::Objects aggregatedMinObjects = objs;
      aggregate.eval( aggregatedMinObjects, aggregateConfigMin );
      fires::Object* aggregatedMinObj = aggregatedMinObjects[ 0 ];

      fires::Objects aggregatedMaxObjects = objs;
      aggregate.eval( aggregatedMaxObjects, aggregateConfigMax );
      fires::Object* aggregatedMaxObj = aggregatedMaxObjects[ 0 ];

    for ( auto& p : _properties )
    {
      std::string label = p.first;

      p.second = TPropertyData
        {
          fires::PropertyManager::getPropertyCaster( label )->toInt(
            aggregatedMinObj->getProperty( label ),
            fires::PropertyCaster::FLOOR ),
          fires::PropertyManager::getPropertyCaster( label )->toInt(
            aggregatedMaxObj->getProperty( label ),
            fires::PropertyCaster::CEIL )
        };
    }
  }

  void Layout::_drawCorners( )
  {
    NEUROSCHEME_DEBUG_CHECK( _scene->views( ).size( ) != 0,
                             "Scene with no view" );
    QGraphicsView* gv = _scene->views( )[0];

    // std::cout << gv->width( ) << " x " << gv->height( ) << std::endl;
    QGraphicsEllipseItem* center =
      new QGraphicsEllipseItem( -10, -10, 20, 20 );
    _scene->addItem( center );
    QGraphicsEllipseItem* tl =
      new QGraphicsEllipseItem( -gv->width( ) / 2 - 10,
                                -gv->height( ) / 2 - 10,
                                20, 20 );

    _scene->addItem( tl );
    QGraphicsEllipseItem* br =
      new QGraphicsEllipseItem( gv->width( ) / 2 - 10,
                                gv->height( ) / 2 - 10,
                                20, 20 );

    _scene->addItem( br );
  }

  void Layout::_clearScene( void )
  {
    // Remove top items without destroying them
    QList< QGraphicsItem* > items_ = _scene->items( );
    for ( auto item = items_.begin( ); item != items_.end( ); ++item )
    {
      auto item_ = dynamic_cast< Item* >( *item );
      if ( item_ && item_->parentRep( ))
        _scene->removeItem( *item );
    }

    // Remove the rest
    _scene->clear( );
  }

  void Layout::_addRepresentations( const shift::Representations& reps )
  {
    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    for ( const auto representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        std::cerr << "Item null" << std::endl;
      }
      else
      {
        auto item = graphicsItemRep->item( _scene );

        // Find out if its entity is selected
        // and if so set its pen
        // const auto& repsToEntities =
        //   RepresentationCreatorManager::repsToEntities( );
        const auto entities = repsToEntities.at( representation );
        if ( entities.size( ) < 1 )
          Log::log( NS_LOG_HEADER +
                    "No entities associated to representation",
                    LOG_LEVEL_ERROR );

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
              shapeItem->setPen( InteractionManager::getSelectedPen( ));
            else if ( selectedState == SelectedState::PARTIALLY_SELECTED )
              shapeItem->setPen(
                InteractionManager::getPartiallySelectedPen( ));

          }
        }
        //std::cout << &scene << " add item " << std::endl;
        _scene->addItem( item );
      }
    }
  } // _addRepresentations

  void Layout::updateSelection( void )
  {
    QList< QGraphicsItem* > items_ = _scene->items( );
    for ( auto qitem = items_.begin( ); qitem != items_.end( ); ++qitem )
    {
      auto selectableItem_ = dynamic_cast< SelectableItem* >( *qitem );
      if ( selectableItem_ )
      {
        auto item = dynamic_cast< Item* >( *qitem );
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );

        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          // Check only the first entity, this might not be ok
          // TODO check this
          if ( entities.size( ) > 0 )
          {
            const auto& entity = *entities.begin( );
            auto shapeItem =
              dynamic_cast< QAbstractGraphicsShapeItem* >( item );

            const auto state = SelectionManager::getSelectedState( entity );
            selectableItem_->setSelected( state );
            if ( state == SelectedState::UNSELECTED )
              shapeItem->setPen( InteractionManager::getUnselectedPen( ));
            else if ( state == SelectedState::SELECTED )
              shapeItem->setPen( InteractionManager::getSelectedPen( ));
            else if ( state == SelectedState::PARTIALLY_SELECTED )
              shapeItem->setPen(
                InteractionManager::getPartiallySelectedPen( ));

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

#define ANIM_DURATION 500

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
    posAnim.setEndValue( toPos );

    posAnim.start( );
    scaleAnim.start( );

  }


  ScatterplotLayout::ScatterplotLayout( void )
    : Layout( "Scatterplot" )
  {
    _optionsWidget->layout( )->addWidget( new QPushButton(
                                            "hola scatterplot" ));
  }


  void ScatterplotLayout::displayItems( const shift::Representations& reps,
                                        bool /* animate */ )
  {
    std::cout << "ScatterplotLayout:: Display items" << std::endl;
    std::cout << "1" << std::endl;
    if ( !_scene ) return;
    std::cout << "2" << std::endl;

    _representations = reps;
    if ( reps.empty( ))
      return;

    auto clearFirst = true;
    if ( clearFirst )
    {
      std::cout << "Clearing" << std::endl;
      // Remove top items without destroying them
      QList< QGraphicsItem* > items_ = _scene->items( );
      for ( auto item = items_.begin( ); item != items_.end( ); ++item )
      {
        auto item_ = dynamic_cast< Item* >( *item );
        if ( item_ && item_->parentRep( ))
          _scene->removeItem( *item );
      }

      // Remove the rest
      _scene->clear( );
    }

  }


}
