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

  Layout::Layout( const std::string& name_ )
    : _optionsWidget( new LayoutOptionsWidget )
    , _name( name_ )
  {
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

  void Layout::updateSelection( QGraphicsScene* scene )
  {
    QList< QGraphicsItem* > items_ = scene->items( );
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

  CameraBasedLayout::CameraBasedLayout( void )
    : Layout( "3D" )
  {
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D1" ), 0, 0 );
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D2" ), 0, 1 );

  }

  void CameraBasedLayout::displayItems( QGraphicsScene* scene_,
                                        const shift::Representations& reps )
  {
    std::cout << "ScatterplotLayout:: Display items" << std::endl;
    // ( void ) reps;
    // ( void ) scene_;
    std::cout << "1" << std::endl;
    if ( !scene_ ) return;
    std::cout << "2" << std::endl;
    auto& scene = *scene_;

    _representations = reps;
    if ( reps.empty( ))
      return;

    auto clearFirst = true;
    if ( clearFirst )
    {
      std::cout << "Clearing" << std::endl;
      // Remove top items without destroying them
      QList< QGraphicsItem* > items_ = scene.items( );
      for ( auto item = items_.begin( ); item != items_.end( ); ++item )
      {
        auto item_ = dynamic_cast< Item* >( *item );
        if ( item_ && item_->parentRep( ))
          scene.removeItem( *item );
      }

      // Remove the rest
      scene.clear( );
    }

  }

  ScatterplotLayout::ScatterplotLayout( void )
    : Layout( "Scatterplot" )
  {
    _optionsWidget->layout( )->addWidget( new QPushButton(
                                            "hola scatterplot" ));
  }


  void ScatterplotLayout::displayItems( QGraphicsScene* scene_,
                                        const shift::Representations& reps )
  {
    std::cout << "ScatterplotLayout:: Display items" << std::endl;
    std::cout << "1" << std::endl;
    if ( !scene_ ) return;
    std::cout << "2" << std::endl;
    auto& scene = *scene_;

    _representations = reps;
    if ( reps.empty( ))
      return;

    auto clearFirst = true;
    if ( clearFirst )
    {
      std::cout << "Clearing" << std::endl;
      // Remove top items without destroying them
      QList< QGraphicsItem* > items_ = scene.items( );
      for ( auto item = items_.begin( ); item != items_.end( ); ++item )
      {
        auto item_ = dynamic_cast< Item* >( *item );
        if ( item_ && item_->parentRep( ))
          scene.removeItem( *item );
      }

      // Remove the rest
      scene.clear( );
    }

  }

}
