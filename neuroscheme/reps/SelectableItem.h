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
#ifndef __NEUROSCHEME__SELECTABLE_ITEM__
#define __NEUROSCHEME__SELECTABLE_ITEM__

#include "../SelectedState.h"

namespace neuroscheme
{

  class SelectableItem
  {

  public:

    SelectableItem( SelectedState selected_ = SelectedState::UNSELECTED )
      : _selected( selected_ ) { }

    virtual ~SelectableItem( void ) { }

    void select( void ) { _selected = SelectedState::SELECTED; }
    void unselect( void ) { _selected = SelectedState::UNSELECTED; }
    void setSelected( SelectedState selected_ = SelectedState::SELECTED )
    {
      _selected = selected_;
    }
    void toggleSelected( void )
    {
      if ( _selected == SelectedState::SELECTED )
        _selected = SelectedState::UNSELECTED;
      else if ( _selected == SelectedState::UNSELECTED )
        _selected = SelectedState::SELECTED;
    }

    bool selected( void ) const
    {
      return _selected == SelectedState::SELECTED;
    };

    bool partiallySelected( void ) const
    {
      return _selected == SelectedState::PARTIALLY_SELECTED;
    };

  protected:

    SelectedState _selected;

  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__SELECTABLE_ITEM__
