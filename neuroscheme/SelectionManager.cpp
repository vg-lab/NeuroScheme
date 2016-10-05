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
#include "SelectionManager.h"

namespace neuroscheme
{

  SelectionManager::TSelection SelectionManager::_activeSelection =
    TSelection( );
  SelectionManager::TSelections SelectionManager::_savedSelections =
    TSelections( );

  unsigned int SelectionManager::_savedSelectionNextId = 0;

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

  void SelectionManager::clearSavedSelections( void )
  {
    _savedSelections.clear( );
  }

  unsigned int SelectionManager::saveActiveSelection( void )
  {
    _savedSelections[_savedSelectionNextId] = _activeSelection;
    return _savedSelectionNextId++;
  }

  void SelectionManager::restoreSavedSelection( unsigned int savedSelectionId )
  {
    _activeSelection = _savedSelections[savedSelectionId];
  }

  //   static void setSelectedState( shift::Entity* entity,
  //                                 SelectedState state );
  //   static SelectedState getSelectedState( shift::Entity* entity );
  //   static void clearActiveSelection( unsigned int selectionId = 0 );
  //   static void clearSavedSelections( void );
  //   static void saveActiveSelection( void );
  //   static void restoreSavedSelection( void );


  // void SelectionManager::setSelectedState( shift::Entity* entity,
  //                                          SelectedState state,
  //                                          unsigned int selectionId )
  // {
  //   _selectedState[ selectionId][ entity ] = state;
  // }

  // SelectedState SelectionManager::getSelectedState( shift::Entity* entity,
  //                                                   unsigned int selectionId )
  // {
  //   const auto&  selection =
  //     _selectedState[ selectionId ];
  //   if ( selection.find( entity ) == selection.end( ))
  //     return SelectedState::UNSELECTED;
  //   else
  //     return selection.at( entity );
  // }

  // void SelectionManager::clearSelection( unsigned int selectionId )
  // {
  //   _selectedState[ selectionId ].clear( );
  // }

  // void SelectionManager::clearAllSelections( void )
  // {
  //   _selectedState.clear( );
  // }


} // namespace neuroscheme
