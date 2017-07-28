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
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef __NSLIB__SELECTION_MANAGER__
#define __NSLIB__SELECTION_MANAGER__

#include <nslib/api.h>
#include "SelectedState.h"
#include <shift/shift.h>

namespace nslib
{

  class SelectableEntity
  {
  public:
    virtual ~SelectableEntity( void ) {}
  };

  class SelectionManager
  {
  public:
    typedef std::unordered_map< shift::Entity*, SelectedState > TSelection;
    typedef std::unordered_map< std::string, TSelection > TSelections;

    NSLIB_API
    static void setSelectedState( shift::Entity* entity,
                                  SelectedState state );
    NSLIB_API
    static SelectedState getSelectedState( shift::Entity* entity );
    NSLIB_API
    static std::vector< shift::Entity* > getActiveSelection( void );
    NSLIB_API
    static void clearActiveSelection( void );
    NSLIB_API
    static void clearStoredSelections( void );
    NSLIB_API
    static unsigned int activeSelectionSize( void );

    NSLIB_API
    static void storeActiveSelection( const std::string& selectionName );
    NSLIB_API
    static void restoreStoredSelection( const std::string& selectionName );
    NSLIB_API
    static unsigned int storedSelectionSize( const std::string& selectionName );
    NSLIB_API
    static bool existsStoredSelection( const std::string& selectionName );
    NSLIB_API
    static bool deleteStoredSelection( const std::string& selectionName );

    NSLIB_API
    static void selectableEntitiesIds(
      std::vector< unsigned int >& selectableEntitiesIds );
    NSLIB_API
    static void setSelectionFromSelectableEntitiesIds(
      const std::vector< unsigned int >& selectableEntitiesIds );

  protected:

    static TSelection _activeSelection;
    static TSelections _storedSelections;
    //static unsigned int _storedSelectionNextId;
  };
}

#endif
