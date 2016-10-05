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
#ifndef __NEUROSCHEME__SELECTION_MANAGER__
#define __NEUROSCHEME__SELECTION_MANAGER__

#include "SelectedState.h"
#include <shift/shift.h>

namespace neuroscheme
{

  class SelectionManager
  {
  public:
    typedef std::unordered_map< shift::Entity*, SelectedState > TSelection;
    typedef std::unordered_map< unsigned int, TSelection > TSelections;

    static void setSelectedState( shift::Entity* entity,
                                  SelectedState state );
    static SelectedState getSelectedState( shift::Entity* entity );
    static void clearActiveSelection( void );
    static void clearSavedSelections( void );
    static unsigned int saveActiveSelection( void );
    static void restoreSavedSelection( unsigned int );

  protected:

    static TSelection _activeSelection;
    static TSelections _savedSelections;
    static unsigned int _savedSelectionNextId;
  };
}

#endif
