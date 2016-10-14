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
#include "DomainManager.h"
#include "PaneManager.h"
#include "SelectionManager.h"

namespace neuroscheme
{

  SelectionManager::TSelection SelectionManager::_activeSelection =
    TSelection( );
  SelectionManager::TSelections SelectionManager::_storedSelections =
    TSelections( );

  // unsigned int SelectionManager::_storedSelectionNextId = 0;

  void SelectionManager::setSelectedState( shift::Entity* entity,
                                           SelectedState state )
  {
    _activeSelection[ entity ] = state;

  }

  SelectedState SelectionManager::getSelectedState( shift::Entity* entity )
  {
    if ( _activeSelection.find( entity ) == _activeSelection.end( ))
      return SelectedState::UNSELECTED;
    else
      return _activeSelection.at( entity );

  }

  void SelectionManager::clearActiveSelection( void )
  {
    _activeSelection.clear( );
  }

  void SelectionManager::clearStoredSelections( void )
  {
    _storedSelections.clear( );
  }

  void SelectionManager::storeActiveSelection( const std::string& selectionName )
  {
    _storedSelections[ selectionName ] = _activeSelection;
//    return _storedSelectionNextId++;
  }

  void SelectionManager::restoreStoredSelection( const std::string& selectionName )
  {
    _activeSelection = _storedSelections[ selectionName ];
  }


  unsigned int selectionSize( const SelectionManager::TSelection& selection )
  {
    assert( DomainManager::getActiveDomain( ) != nullptr );
    return std::count_if(
      selection.begin( ), selection.end( ),
      []( const std::pair< shift::Entity*, SelectedState>& statePair )
      {
        return
          DomainManager::getActiveDomain( )->isSelectableEntity(
            statePair.first ) &&
          statePair.second == SelectedState::SELECTED;
      } );

  }
  unsigned int SelectionManager::activeSelectionSize( void )
  {
    return selectionSize( _activeSelection );
  }

  unsigned int SelectionManager::storedSelectionSize(
    const std::string& selectionName )
  {
    return selectionSize( _storedSelections[ selectionName ] );
  }

  bool SelectionManager::existsStoredSelection(
    const std::string& selectionName )
  {
    return _storedSelections.find( selectionName ) != _storedSelections.end( );
  }

  bool SelectionManager::deleteStoredSelection(
    const std::string& selectionName )
  {
    auto selectionToDelete = _storedSelections.find( selectionName );
    if ( selectionToDelete == _storedSelections.end( ))
      return false;
    _storedSelections.erase( selectionToDelete );

    return true;
  }

  void SelectionManager::selectableEntitiesIds(
      std::vector< unsigned int >& selectableEntitiesIds )
  {
    selectableEntitiesIds.clear( );
    const auto& domain = DomainManager::getActiveDomain( );
    for ( const auto& statePair : _activeSelection )
    {
      if ( domain->isSelectableEntity( statePair.first ) &&
           statePair.second == SelectedState::SELECTED )
        selectableEntitiesIds.push_back(
          domain->selectableEntityId( statePair.first ));
    }

  }

  void propagateSelectedToChilds(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid )
  {
    if ( relParentOf.find( entityGid ) == relParentOf.end( ))
      return;
    const auto& childrenIds = relParentOf.at( entityGid );
    for ( auto const& childId : childrenIds )
    {
      SelectionManager::setSelectedState(
        entities.at( childId ), SelectedState::SELECTED );
      propagateSelectedToChilds( entities, relParentOf, childId );
    }

  }

  void SelectionManager::setSelectionFromSelectableEntitiesIds(
      const std::vector< unsigned int >& selectableEntitiesIds )
  {
    _activeSelection.clear( );
    const auto& domain = DomainManager::getActiveDomain( );

    // This is very costly
    // TODO improve, this map could be computed once per domain
    std::unordered_map< unsigned int, shift::Entity* > idToEntity;
    const auto& entities = DataManager::entities( );
    for ( const auto& entity : entities)
    {
      if ( domain->isSelectableEntity( entity.second ))
        idToEntity[ domain->selectableEntityId( entity.second ) ] =
          entity.second;
    }

    auto& relParentOf = *( DataManager::entities( ).
                           relationships( )[ "isParentOf" ]->asOneToN( ));
    auto& relChildOf = *( DataManager::entities( ).relationships( )
                          [ "isChildOf" ]->asOneToOne( ));

    std::unordered_set< shift::Entity* > postCheckParentEntities;
    for ( auto entityId : selectableEntitiesIds )
    {
      // std::cout << "set selected state to "
      //           << idToEntity[ entityId ] << ":"
      //           << entityId << ":"
      //           << idToEntity[ entityId ]->entityGid( ) << ":"
      //           << std::endl;
      setSelectedState( idToEntity[ entityId ], SelectedState::SELECTED );
      propagateSelectedToChilds( entities, relParentOf,
                                 idToEntity[ entityId ]->entityGid( ));
      // const auto& children =
      //   relParentOf[ idToEntity[ entityId ]->entityGid( ) ];
      shift::Entity* entity = idToEntity[ entityId ];
      postCheckParentEntities.insert(
        entities.at( relChildOf[ entity->entityGid( ) ]));

    }

    // This might get ugly :(
    // TODO recheck
    while ( postCheckParentEntities.size( ) > 0 )
    {
      for ( auto entityIt = postCheckParentEntities.begin( );
            entityIt != postCheckParentEntities.end( ); )
      {
        // c++11 to erase iterators without invalidating
        const auto& entity = *entityIt;
        const auto entityGid = entity->entityGid( );
        entityIt = postCheckParentEntities.erase( entityIt );

        bool allChildrenSelected;
        bool noChildrenSelected;
        InteractionManager::queryChildrenSelectedState(
          entities, relParentOf, entityGid,
          allChildrenSelected, noChildrenSelected );

        // Partially selected
        if ( !allChildrenSelected && !noChildrenSelected )
          setSelectedState( entity,
                            SelectedState::PARTIALLY_SELECTED );
        else if ( allChildrenSelected )
          setSelectedState( entity, SelectedState::SELECTED );

        if ( relChildOf.find( entityGid ) != relChildOf.end( ) &&
             entities.find( relChildOf[ entityGid ] ) !=
             entities.end( ))
        {
          // std::cout << entity->entityGid( ) << " child of " <<
          //   relChildOf[ entity->entityGid( ) ] << std::endl;

        // TODO If partially selected parent is always partially selected which
        // can be an optimization

          postCheckParentEntities.insert(
            entities.at( relChildOf[ entityGid ]));
        }
      }
    }

    // for ( const auto  entity : entities )
    // {
    //   std::cout << entity.second->entityGid( ) << " "
    //             << int( getSelectedState( entity.second )) << std::endl;
    // }
    std::cout << _activeSelection.size( ) << std::endl;
    // LayoutManager::updateAllScenesSelection( );
    PaneManager::updateSelection( );
  }

  

} // namespace neuroscheme
