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
#ifndef __NSLIB__SELECTABLE_ITEM__
#define __NSLIB__SELECTABLE_ITEM__

#include <nslib/api.h>
#include "../SelectedState.h"
#include <QPen>
#include <QAbstractGraphicsShapeItem>
#include <iostream>

namespace nslib
{

  class NSLIB_API SelectableItem
  {

  public:

    SelectableItem( SelectedState selected_ = SelectedState::UNSELECTED )
      : _selected( selected_ )
      , _hover( false ) { }

    virtual ~SelectableItem( void ) { }

    virtual void select( void )
    { _selected = SelectedState::SELECTED; _updatePen( ); }
    virtual void unselect( void )
    { _selected = SelectedState::UNSELECTED; _updatePen( ); }
    virtual void setSelected(
      SelectedState selected_ = SelectedState::SELECTED )
    {
      _selected = selected_;
      _updatePen( );
    }
    void toggleSelected( void )
    {
      if ( _selected == SelectedState::SELECTED )
        _selected = SelectedState::UNSELECTED;
      else if ( _selected == SelectedState::UNSELECTED )
        _selected = SelectedState::SELECTED;
      _updatePen( );
    }

    bool selected( void ) const
    {
      return _selected == SelectedState::SELECTED;
    };

    SelectedState selectedState( void ) const
    {
      return _selected;;
    };

    bool partiallySelected( void ) const
    {
      return _selected == SelectedState::PARTIALLY_SELECTED;
    };

    void hover( bool hover_ ) { _hover = hover_; }

    static const QPen& hoverUnselectedPen( ) { return _hoverUnselectedPen; };
    static const QPen& selectedPen( ) { return _selectedPen; };
    static const QPen& unselectedPen( ) { return _unselectedPen; };
    static const QPen& partiallySelectedPen( )
    { return _partiallySelectedPen; };
    static void init( void );
  protected:

    void _updatePen( )
    {
      // std::cout << "update pen1" << std::endl;
      auto item = dynamic_cast< QAbstractGraphicsShapeItem* >( this );
      if ( item )
      {
        // std::cout << "update pen2" << std::endl;
        switch ( _selected )
        {
        case SelectedState::SELECTED:
          item->setPen( _hover ? _hoverSelectedPen : _selectedPen );
          break;
        case SelectedState::UNSELECTED:
          item->setPen( _hover ? _hoverUnselectedPen : _unselectedPen );
          break;
        case SelectedState::PARTIALLY_SELECTED:
          item->setPen( _hover ? _hoverPartiallySelectedPen :
                        _partiallySelectedPen );
          break;
        }
      }
    }
    SelectedState _selected;
    bool _hover;
    static QPen _selectedPen;
    static QPen _partiallySelectedPen;
    static QPen _unselectedPen;
    static QPen _hoverSelectedPen;
    static QPen _hoverPartiallySelectedPen;
    static QPen _hoverUnselectedPen;

  };

} // namespace nslib

#endif // __NSLIB__SELECTABLE_ITEM__
