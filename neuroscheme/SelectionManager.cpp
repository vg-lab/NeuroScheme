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
#include "DomainManager.h"
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
      { return DomainManager::getActiveDomain( )->isSelectableEntity( statePair.first ) &&
          statePair.second == SelectedState::SELECTED ;} );

  }
  unsigned int SelectionManager::activeSelectionSize( void )
  {
    return selectionSize( _activeSelection );
  }

  unsigned int SelectionManager::storedSelectionSize( const std::string& selectionName )
  {
    return selectionSize( _storedSelections[ selectionName ] );
  }

  bool SelectionManager::existsStoredSelection( const std::string& selectionName )
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

} // namespace neuroscheme
