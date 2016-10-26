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
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
#include "SelectionManager.h"
#include "ZeroEQManager.h"
#include "domains/domains.h"

namespace neuroscheme
{

  QPen InteractionManager::_selectedPen =
    QPen( Qt::red, 3, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_hoverSelectedPen =
    QPen( Qt::red, 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_partiallySelectedPen =
    QPen( Qt::yellow, 3, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_hoverPartiallySelectedPen =
    QPen( Qt::yellow, 3, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen InteractionManager::_unselectedPen = QPen( Qt::NoPen );

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
      else if ( selectableItem->partiallySelected( ))
        item->setPen( _hoverPartiallySelectedPen );
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
      else if ( selectableItem->partiallySelected( ))
        item->setPen( _partiallySelectedPen );
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

    //std::cout << "InteractionManager::contextMenu" << std::endl;
    {
      auto item = dynamic_cast< Item* >( shapeItem );
      if ( item )
      {
        assert( item->parentRep( ));
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          auto entityGid = ( *entities.begin( ))->entityGid( );
          // std::cout << "-- ShiFT gid: "
          //           << int( entityGid ) << std::endl;

          auto& relParentOf = *( DataManager::entities( ).
                                 relationships( )[ "isParentOf" ]->asOneToN( ));
          const auto& children = relParentOf[ entityGid ];
          // std::cout << " -- Parent of: ";
          // for ( auto const& child : children )
          //   std::cout << child << " ";
          // std::cout << std::endl;;

          auto& relChildOf = *( DataManager::entities( ).relationships( )
                                [ "isChildOf" ]->asOneToOne( ));
          const auto& parent = relChildOf[ entityGid ];
          // std::cout << " -- Child of: ";
          // std::cout << parent << std::endl;;

          const auto& grandParent = relChildOf[ relChildOf[ entityGid ]];
          // std::cout << " -- GrandChild of: ";
          // std::cout << grandParent << std::endl;;

          const auto& parentSiblings = relParentOf[ grandParent ];
          // std::cout << " -- Parent of: ";
          // for ( auto const& parentSibling : parentSiblings )
          //   std::cout << parentSibling << " ";
          // std::cout << std::endl;;

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
              // std::cout << "up" << std::endl;
              if ( parentSiblings.size( ) > 0 )
                for ( const auto& parentSibling : parentSiblings )
                  targetEntities.add(
                    DataManager::entities( ).at( parentSibling ));
              else
                targetEntities.add( DataManager::entities( ).at( parent ));


            }
            if ( levelDown && levelDown == selectedAction )
            {
              //std::cout << "down" << std::endl;
              for ( const auto& child : children )
                targetEntities.add( DataManager::entities( ).at( child ));
            }
            if ( targetEntities.size( ) > 0 )
            {
              // neuroscheme::RepresentationCreatorManager::create(
              //   targetEntities, representations,
              //   true, true );

              auto canvas = dynamic_cast< Canvas* >(
                shapeItem->scene( )->parent( ));
              assert( canvas );
              canvas->displayEntities( targetEntities, false, true );
              //canvas->displayReps( representations, false );
              // neuroscheme::LayoutManager::setScene( shapeItem->scene( ));
              // neuroscheme::LayoutManager::displayItems(
              //   representations, true );
              // neuroscheme::LayoutManager::displayItems(
              //   representations, true );
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
      //std::cout << "--------------"
        //         << (int) ( *entities.begin( ))->getProperty( "gid" ).
        // value< unsigned int >( ) << std::endl;
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


  void InteractionManager::mousePressEvent(
    QAbstractGraphicsShapeItem* shapeItem,
    QGraphicsSceneMouseEvent* event )
  {
    // Selection event
    if ( event->buttons( ) & Qt::LeftButton )
    {
      auto item = dynamic_cast< Item* >( shapeItem );

      auto selectableItem = dynamic_cast< SelectableItem* >( item );
      if ( selectableItem )
      {
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          // auto entityGid = ( *entities.begin( ))->entityGid( );
          if ( selectableItem->partiallySelected( ))
            selectableItem->setSelected( );
          else
            selectableItem->toggleSelected( );

          for ( const auto& entity : entities )
          {
            // std::cout << "-- ShiFT gid: "
            //           << int( entity->entityGid( )) << std::endl;

            if ( selectableItem->selected( ))
            {
              SelectionManager::setSelectedState(
                entity, SelectedState::SELECTED );
//              shapeItem->setPen( _selectedPen );
            }
            else
            {
//              shapeItem->setPen( _unselectedPen );
              SelectionManager::setSelectedState(
                entity, SelectedState::UNSELECTED );
            }

            auto parentState = SelectionManager::getSelectedState( entity );


            auto entityGid = ( *entities.begin( ))->entityGid( );

            _propagateSelectedStateToChilds(
              DataManager::entities( ),
              *( DataManager::entities( ).
                 relationships( )[ "isParentOf" ]->asOneToN( )),
              entityGid,
              parentState );

            _propagateSelectedStateToParent(
              DataManager::entities( ),
              *( DataManager::entities( ).
                 relationships( )[ "isChildOf" ]->asOneToOne( )),
              *( DataManager::entities( ).
                 relationships( )[ "isParentOf" ]->asOneToN( )),
              entityGid,
              parentState );

            //std::cout << std::endl;

            //LayoutManager::updateAllScenesSelection( );
            PaneManager::updateSelection( );
          }
        }
        else
        {
          Log::log( NS_LOG_HEADER + "item without entity",
                    LOG_LEVEL_ERROR );
          return;
        }
      }

      // Publish selection
      std::vector< unsigned int > ids;
      SelectionManager::selectableEntitiesIds( ids );
      ZeroEQManager::publishSelection( ids );

    } // selection event
  }

  void InteractionManager::_propagateSelectedStateToChilds(
    shift::Entities& entities,
    shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid,
    SelectedState state )
  {
    const auto& childrenIds = relParentOf[ entityGid ];
    //std::cout << " -- Parent of: ";
    for ( auto const& childId : childrenIds )
    {
      //  std::cout << childId << " ";
      SelectionManager::setSelectedState(
        entities.at( childId ), state );
      _propagateSelectedStateToChilds( entities, relParentOf, childId, state );
    }

  }

  void InteractionManager::_propagateSelectedStateToParent(
    shift::Entities& entities,
    shift::RelationshipOneToOne& relChildOf,
    shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid,
    SelectedState childState )
  {
    const auto& parentId = relChildOf[ entityGid ];

    if ( parentId == 0 ) return;

    if ( childState == SelectedState::PARTIALLY_SELECTED )
    {
      //std::cout << "<>Partially selected" << std::endl;
      SelectionManager::setSelectedState(
        entities.at( parentId ), childState );
      _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
                                       parentId, childState );
      return;
    }

    bool allChildrenSelected, noChildrenSelected;
    queryChildrenSelectedState( entities, relParentOf, parentId,
                                allChildrenSelected, noChildrenSelected );
    //std::cout << "<>AllChildSelected? = " << allChildrenSelected << std::endl;
    SelectedState state;
    if ( noChildrenSelected )
      state = SelectedState::UNSELECTED;
    else if ( allChildrenSelected )
      state = SelectedState::SELECTED;
    else
      state = SelectedState::PARTIALLY_SELECTED;

    SelectionManager::setSelectedState(
      entities.at( parentId ), state );
    _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
                                     parentId, state );
  }

  void InteractionManager::queryChildrenSelectedState(
    const shift::Entities& entities,
    shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid,
    bool& allChildrenSelected,
    bool& noChildrenSelected )
  {
    allChildrenSelected = true;
    noChildrenSelected = true;
    const auto& childrenIds = relParentOf[ entityGid ];
    for ( auto const& childId : childrenIds )
    {
      if ( SelectionManager::getSelectedState( entities.at( childId )) !=
           SelectedState::SELECTED )
        allChildrenSelected = false;
      if ( SelectionManager::getSelectedState( entities.at( childId )) ==
           SelectedState::SELECTED )
        noChildrenSelected = false;
    }
    return;
  }
}
