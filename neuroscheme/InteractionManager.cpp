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
#include <unordered_set>

namespace neuroscheme
{

  QMenu* InteractionManager::_contextMenu = nullptr;


  void InteractionManager::hoverEnterEvent(
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneHoverEvent* /* event */ )
  {
    // std::cout << "hover" << std::endl;
    auto selectableItem = dynamic_cast< SelectableItem* >( item );
    if ( selectableItem )
    {
      selectableItem->hover( true );
      selectableItem->setSelected( selectableItem->selectedState( ));
    }
    else
    {
      item->setPen( SelectableItem::hoverUnselectedPen( ));
    }
  }


  void InteractionManager::hoverLeaveEvent(
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneHoverEvent* /* event */ )
  {
    auto selectableItem = dynamic_cast< SelectableItem* >( item );
    if ( selectableItem )
    {
      selectableItem->hover( false );
      selectableItem->setSelected( selectableItem->selectedState( ));
    }
    else
    {
      item->setPen( SelectableItem::unselectedPen( ));
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

          auto& relParentOf = *( DataManager::entities( ).
                                 relationships( )[ "isParentOf" ]->asOneToN( ));
          const auto& children = relParentOf[ entityGid ];

          auto& relChildOf = *( DataManager::entities( ).relationships( )
                                [ "isChildOf" ]->asOneToOne( ));
          const auto& parent = relChildOf[ entityGid ];

          const auto& grandParent = relChildOf[ relChildOf[ entityGid ]];

          const auto& parentSiblings = relParentOf[ grandParent ];

          auto& relAGroupOf = *( DataManager::entities( ).relationships( )
                                [ "isAGroupOf" ]->asOneToN( ));
          const auto& groupedEntities = relAGroupOf[ entityGid ];

          QAction* levelUp = nullptr;
          QAction* levelDown = nullptr;
          QAction* expandGroup = nullptr;
          QAction* levelUpToNewPane = nullptr;
          QAction* levelDownToNewPane = nullptr;
          QAction* expandGroupToNewPane = nullptr;

          if ( parent != 0 )
            levelUp = _contextMenu->addAction( QString( "Level up" ));
          if ( children.size( ) > 0 )
            levelDown = _contextMenu->addAction( QString( "Level down" ));
          if ( groupedEntities.size( ) > 0 )
            expandGroup = _contextMenu->addAction( QString( "Expand group" ));

          if ( levelUp || levelDown || expandGroup )
            _contextMenu->addSeparator( );

          if ( parent != 0 )
            levelUpToNewPane =
              _contextMenu->addAction( QString( "Level up [new pane]" ));
          if ( children.size( ) > 0 )
            levelDownToNewPane =
              _contextMenu->addAction( QString( "Level down [new pane]" ));
          if ( groupedEntities.size( ) > 0 )
            expandGroupToNewPane = _contextMenu->addAction(
              QString( "Expand group [new pane]" ));

          if ( levelUp || levelDown || expandGroup ||
               levelUpToNewPane || levelDownToNewPane || expandGroupToNewPane )
          {
            shift::Representations representations;
            shift::Entities targetEntities;
            QAction* selectedAction = _contextMenu->exec( event->screenPos( ));

            if (( levelUpToNewPane &&
                  levelUpToNewPane == selectedAction ) ||
                ( levelDownToNewPane &&
                  levelDownToNewPane == selectedAction ) ||
                ( expandGroupToNewPane &&
                  expandGroupToNewPane == selectedAction ))
            {
              neuroscheme::PaneManager::activePane(
                neuroscheme::PaneManager::newPaneFromActivePane( ));
            }

            if (( levelUp && levelUp == selectedAction ) ||
                ( levelUpToNewPane && levelUpToNewPane == selectedAction ))
            {
              if ( parentSiblings.size( ) > 0 )
                for ( const auto& parentSibling : parentSiblings )
                  targetEntities.add(
                    DataManager::entities( ).at( parentSibling ));
              else
                targetEntities.add( DataManager::entities( ).at( parent ));
            }

            if (( levelDown && levelDown == selectedAction ) ||
                ( levelDownToNewPane && levelDownToNewPane == selectedAction ))
            {
              for ( const auto& child : children )
                targetEntities.add( DataManager::entities( ).at( child ));
            }

            if (( expandGroup && expandGroup == selectedAction ) ||
                ( expandGroupToNewPane &&
                  expandGroupToNewPane == selectedAction ))
            {
              for ( const auto& groupedEntity : groupedEntities )
                targetEntities.add(
                  DataManager::entities( ).at( groupedEntity ));
            }
            if ( targetEntities.size( ) > 0 )
            {
              auto canvas = PaneManager::activePane( );
              canvas->displayEntities( targetEntities, false, true );
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
          if ( selectableItem->partiallySelected( ))
            selectableItem->setSelected( );
          else
            selectableItem->toggleSelected( );

          // std::cout << "-------- Selecting entities "
          //           << entities.size( ) << std::endl;
          for ( const auto& entity : entities )
          {
            // std::cout << "-------- " << entity->entityGid( ) << std::endl;
            // std::cout << "-- ShiFT gid: "
            //           << int( entity->entityGid( )) << std::endl;

            if ( selectableItem->selected( ))
            {
              SelectionManager::setSelectedState(
                entity, SelectedState::SELECTED );
            }
            else
            {
              SelectionManager::setSelectedState(
                entity, SelectedState::UNSELECTED );
            }

            auto entityState = SelectionManager::getSelectedState( entity );
            // auto entityGid = ( *entities.begin( ))->entityGid( );
            auto entityGid = entity->entityGid( );

            const auto& allEntities = DataManager::entities( );
            const auto& relationships = allEntities.relationships( );
            const auto& relChildOf =
              *( relationships.at( "isChildOf" )->asOneToOne( ));
            const auto& relParentOf =
              *( relationships.at( "isParentOf" )->asOneToN( ));
            const auto& relSubEntityOf =
              *( relationships.at( "isSubEntityOf" )->asOneToOne( ));
            const auto& relSuperEntityOf =
              *( relationships.at( "isSuperEntityOf" )->asOneToN( ));
            const auto& relAGroupOf =
              *( relationships.at( "isAGroupOf" )->asOneToN( ));

            if ( relSubEntityOf.count( entityGid ) > 0 )
            {
              std::unordered_set< unsigned int > parentIds;
              if ( relAGroupOf.count( entityGid ) > 0 )
              {
                const auto& groupedIds = relAGroupOf.at( entityGid );
                // std::cout << " -- Parent of: ";
                // std::cout << ",,,, Grouped " << groupedIds.size( ) << std::endl;
                for ( auto const& groupedId : groupedIds )
                {
                  SelectionManager::setSelectedState(
                    allEntities.at( groupedId ), entityState );
                  // Save unique parent set for updating only once per parent
                  if ( relChildOf.count( groupedId ) > 0 )
                    parentIds.insert( relChildOf.at( groupedId ));
                }
                _updateSelectedStateOfSubEntities(
                  allEntities, relSuperEntityOf, relAGroupOf,
                  relSubEntityOf.at( entityGid ));
                std::unordered_set< unsigned int > uniqueParentChildIds;
                for ( auto const& parentId : parentIds )
                {
                  uniqueParentChildIds.insert(
                    *relParentOf.at( parentId ).begin( ));
                }
                // std::cout << ",,,, Parents: " << parentIds.size( ) << std::endl;
                for ( auto const& uniqueParentChildId : uniqueParentChildIds )
                {
                  _propagateSelectedStateToParent(
                    allEntities, relChildOf, relParentOf,
                    relSuperEntityOf, relAGroupOf,
                    uniqueParentChildId, entityState );
                }
              }
            } // if subentity
            else
            {
              if ( relSuperEntityOf.count( entityGid ) > 0 )
              {
                const auto& subEntities = relSuperEntityOf.at( entityGid );
                for ( const auto& subEntity : subEntities )
                  SelectionManager::setSelectedState(
                    allEntities.at( subEntity ), entityState );
              }

              // std::cout << "Propagate to children of " << entityGid << std::endl;
              _propagateSelectedStateToChilds(
                allEntities, relParentOf, relSuperEntityOf,
                entityGid, entityState );

              _propagateSelectedStateToParent(
                allEntities, relChildOf, relParentOf,
                relSuperEntityOf, relAGroupOf,
                entityGid, entityState );
            //std::cout << std::endl;
            }
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
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relParentOf,
    const shift::RelationshipOneToN& relSuperEntityOf,
    unsigned int entityGid,
    SelectedState state )
  {
    if ( relParentOf.count( entityGid ) == 0 )
      return;
    const auto& childrenIds = relParentOf.at( entityGid );
    // std::cout << " -- Parent of: ";
    for ( auto const& childId : childrenIds )
    {
      // std::cout << childId << " ";
      if ( relSuperEntityOf.count( childId ) > 0 )
      {
        const auto& subEntities = relSuperEntityOf.at( childId );
        for ( const auto& subEntity : subEntities )
          SelectionManager::setSelectedState(
            DataManager::entities( ).at( subEntity ), state );
      }
      SelectionManager::setSelectedState(
        entities.at( childId ), state );
      _propagateSelectedStateToChilds( entities, relParentOf, relSuperEntityOf,
                                       childId, state );
    }

  }

  void InteractionManager::_propagateSelectedStateToParent(
    const shift::Entities& entities,
    const shift::RelationshipOneToOne& relChildOf,
    const shift::RelationshipOneToN& relParentOf,
    const shift::RelationshipOneToN& relSuperEntityOf,
    const shift::RelationshipOneToN& relAGroupOf,
    unsigned int entityGid,
    SelectedState childState )
  {
    if ( relChildOf.count( entityGid ) == 0 )
      return;
    const auto& parentId = relChildOf.at( entityGid );

    if ( parentId == 0 ) return;

    if ( childState == SelectedState::PARTIALLY_SELECTED )
    {
      //std::cout << "<>Partially selected" << std::endl;
      SelectionManager::setSelectedState(
        entities.at( parentId ), childState );
      _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
                                       relSuperEntityOf, relAGroupOf,
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

    _updateSelectedStateOfSubEntities(
      entities, relSuperEntityOf, relAGroupOf, parentId );

    _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
                                     relSuperEntityOf, relAGroupOf,
                                     parentId, state );
  }

  void InteractionManager::_updateSelectedStateOfSubEntities(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relSuperEntityOf,
    const shift::RelationshipOneToN& relAGroupOf,
    unsigned int entityGid )
  {
    for ( const auto& subEntityId : relSuperEntityOf.at( entityGid ))
    {
      bool allGroupedSelected, noGroupedSelected;
      _queryGroupedSelectedState( entities, relAGroupOf, subEntityId,
                                  allGroupedSelected, noGroupedSelected );
      SelectedState groupedState;
      if ( noGroupedSelected )
        groupedState = SelectedState::UNSELECTED;
      else if ( allGroupedSelected )
        groupedState = SelectedState::SELECTED;
      else
        groupedState = SelectedState::PARTIALLY_SELECTED;

      SelectionManager::setSelectedState(
        entities.at( subEntityId ), groupedState );

    }
  }

  void InteractionManager::queryChildrenSelectedState(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid,
    bool& allChildrenSelected,
    bool& noChildrenSelected )
  {
    allChildrenSelected = true;
    noChildrenSelected = true;
    const auto& childrenIds = relParentOf.at( entityGid );
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

  void InteractionManager::_queryGroupedSelectedState(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relAGroupOf,
    unsigned int entityGid,
    bool& allGroupedSelected,
    bool& noGroupedSelected )
  {
    allGroupedSelected = true;
    noGroupedSelected = true;
    if ( relAGroupOf.count( entityGid ) == 0 )
    {
      allGroupedSelected = false;
      noGroupedSelected = true;
      return;
    }
    const auto& groupedIds = relAGroupOf.at( entityGid );
    for ( auto const& groupedId : groupedIds )
    {
      if ( SelectionManager::getSelectedState( entities.at( groupedId )) !=
           SelectedState::SELECTED )
        allGroupedSelected = false;
      if ( SelectionManager::getSelectedState( entities.at( groupedId )) ==
           SelectedState::SELECTED )
        noGroupedSelected = false;
    }
  }

} // namespace neuroscheme
