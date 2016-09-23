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
#include "InteractionManager.h"
#include "RepresentationCreatorManager.h"
#include "objs/Neuron.h"
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
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneContextMenuEvent* event )
  {

    if ( !_contextMenu )
      _contextMenu = new QMenu( );
    else
      _contextMenu->clear( );

    // std::cout << "InteractionManager::contextMenu" << std::endl;

    auto neuronItem = dynamic_cast< NeuronItem* >( item );
    if ( neuronItem )
    {
      const auto objs =
        RepresentationCreatorManager::repsToObjects( ).at(
          neuronItem->parentRep( ));
      if ( objs.size( ) < 1 )
        Log::log( NS_LOG_HEADER + "neuron item without object",
                  LOG_LEVEL_ERROR );
      std::cout << "--------------"
                << (int) ( *objs.begin( ))->getProperty( "gid" ).
        value< unsigned int >( ) << std::endl;
      // // QAction* action1 =
      // _contextMenu->addAction( QString( "Show minicolumns" ));
      // // QAction* action2 =
      // _contextMenu->addAction( QString( "Show columns" ));
      // // QAction* selectedAction =
      // _contextMenu->exec( event->screenPos( ));
      return;
    }

    auto columnItem = dynamic_cast< ColumnItem* >( item );
    if ( columnItem )
    {
      // QAction* action1 =
      _contextMenu->addAction( QString( "Show minicolumns" ));
      _contextMenu->exec( event->screenPos( ));
      return;
    }

    auto miniColumnItem = dynamic_cast< MiniColumnItem* >( item );
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
