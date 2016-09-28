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
#include "DataManager.h"
#include "InteractionManager.h"
#include "LayoutManager.h"
#include "RepresentationCreatorManager.h"
#include "entities/Neuron.h"
#include "reps/ColumnItem.h"
#include "reps/MiniColumnItem.h"
#include "reps/NeuronItem.h"

namespace neuroscheme
{

  QPen InteractionManager::_selectedPen =
    QPen( Qt::red, 3, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_unselectedPen = QPen( Qt::NoPen );

  QPen InteractionManager::_hoverSelectedPen =
    QPen( Qt::red, 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_hoverUnselectedPen =
    QPen( QColor( 200, 200, 200, 255 ), 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );


  QMenu* InteractionManager::_contextMenu = nullptr;


  void InteractionManager::hoverEnterEvent(
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneHoverEvent* /* event */ )
  {
    auto selectableItem = dynamic_cast< SelectableItem* >( item );
    if ( selectableItem )
    {
      if ( selectableItem->selected( ))
        item->setPen( _hoverSelectedPen );
      else
        item->setPen( _hoverUnselectedPen );
    }
    else
    {
      item->setPen( _hoverUnselectedPen );
    }
  }


  void InteractionManager::hoverLeaveEvent(
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneHoverEvent* /* event */ )
  {
    auto selectableItem = dynamic_cast< SelectableItem* >( item );
    if ( selectableItem )
    {
      if ( selectableItem->selected( ))
        item->setPen( _selectedPen );
      else
        item->setPen( _unselectedPen );
    }
    else
    {
      item->setPen( _unselectedPen );
    }
  }


  void InteractionManager::contextMenuEvent(
    QAbstractGraphicsShapeItem* shapeItem,
    QGraphicsSceneContextMenuEvent* event )
  {

    if ( !_contextMenu )
      _contextMenu = new QMenu( );
    else
      _contextMenu->clear( );

    std::cout << "InteractionManager::contextMenu" << std::endl;
    {
      auto item = dynamic_cast< Item* >( shapeItem );
      if ( item )
      {
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          auto entityGid = ( *entities.begin( ))->entityGid( );
          std::cout << "-- ShiFT gid: "
                    << int( entityGid ) << std::endl;

          auto& relParentOf = *( DataManager::entities( ).
                                 relationships( )[ "isParentOf" ]->asOneToN( ));
          const auto& children = relParentOf[ entityGid ];
          std::cout << " -- Parent of: ";
          for ( auto const& child : children )
            std::cout << child << " ";
          std::cout << std::endl;;

          auto& relChildOf = *( DataManager::entities( ).relationships( )
                                [ "isChildOf" ]->asOneToOne( ));
          const auto& parent = relChildOf[ entityGid ];
          std::cout << " -- Child of: ";
          std::cout << parent << std::endl;;

          const auto& grandParent = relChildOf[ relChildOf[ entityGid ]];
          std::cout << " -- GrandChild of: ";
          std::cout << grandParent << std::endl;;

          const auto& parentSiblings = relParentOf[ grandParent ];
          std::cout << " -- Parent of: ";
          for ( auto const& parentSibling : parentSiblings )
            std::cout << parentSibling << " ";
          std::cout << std::endl;;

          QAction* levelUp = nullptr;
          QAction* levelDown = nullptr;
          if ( parent != 0 )
            levelUp = _contextMenu->addAction( QString( "Level up" ));
          if ( children.size( ) > 0 )
            levelDown = _contextMenu->addAction( QString( "Level down" ));
          if ( levelUp || levelDown )
          {
            shift::Representations representations;
            shift::Entities targetEntities;
            QAction* selectedAction = _contextMenu->exec( event->screenPos( ));
            if ( levelUp && levelUp == selectedAction )
            {
              std::cout << "up" << std::endl;
              if ( parentSiblings.size( ) > 0 )
              for ( const auto& parentSibling : parentSiblings )
                targetEntities[parentSibling] =
                  DataManager::entities( )[parentSibling];
              else
                targetEntities[parent] =
                  DataManager::entities( )[parent];


            }
            if ( levelDown && levelDown == selectedAction )
            {
              std::cout << "down" << std::endl;
              for ( const auto& child : children )
                targetEntities[child] =
                  DataManager::entities( )[child];
            }
            if ( targetEntities.size( ) > 0 )
            {
              neuroscheme::RepresentationCreatorManager::create(
                targetEntities, representations,
                true, true );

              neuroscheme::LayoutManager::displayItems(
                representations, true );
            }
          }
        }
        else
        {
          Log::log( NS_LOG_HEADER + "item without entity",
                    LOG_LEVEL_ERROR );
          return;
        }


      }
      else
        Log::log( NS_LOG_HEADER + "clicked element is not item",
                  LOG_LEVEL_ERROR );
    }

    return;

    auto neuronItem = dynamic_cast< NeuronItem* >( shapeItem );
    if ( neuronItem )
    {
      const auto entities =
        RepresentationCreatorManager::repsToEntities( ).at(
          neuronItem->parentRep( ));
      if ( entities.size( ) < 1 )
        Log::log( NS_LOG_HEADER + "neuron item without entity",
                  LOG_LEVEL_ERROR );
      std::cout << "--------------"
                << (int) ( *entities.begin( ))->getProperty( "gid" ).
        value< unsigned int >( ) << std::endl;
      // // QAction* action1 =
      // _contextMenu->addAction( QString( "Show minicolumns" ));
      // // QAction* action2 =
      // _contextMenu->addAction( QString( "Show columns" ));
      // // QAction* selectedAction =
      // _contextMenu->exec( event->screenPos( ));
      return;
    }

    auto columnItem = dynamic_cast< ColumnItem* >( shapeItem );
    if ( columnItem )
    {
      // QAction* action1 =
      _contextMenu->addAction( QString( "Show minicolumns" ));
      _contextMenu->exec( event->screenPos( ));
      return;
    }

    auto miniColumnItem = dynamic_cast< MiniColumnItem* >( shapeItem );
    if ( miniColumnItem )
    {
      // QAction* action1 =
      _contextMenu->addAction( QString( "Show neurons" ));
      _contextMenu->addAction( QString( "Show columns" ));
      _contextMenu->exec( event->screenPos( ));
      return;
    }

    Log::log( NS_LOG_HEADER + "context menu not handled for this item",
              LOG_LEVEL_WARNING );

  }


  void InteractionManager::mousePressEvent( QAbstractGraphicsShapeItem* item,
                                            QGraphicsSceneMouseEvent* event )
  {
    if ( event->buttons( ) & Qt::LeftButton )
    {
      auto selectableItem = dynamic_cast< SelectableItem* >( item );
      if ( selectableItem )
      {
        selectableItem->toggleSelected( );
        if ( selectableItem->selected( ))
          item->setPen( _selectedPen );
        else
          item->setPen( _unselectedPen );
      }
    }
  }


}
