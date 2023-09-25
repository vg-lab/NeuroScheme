/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#ifndef __NSLIB_ITEM__
#define __NSLIB_ITEM__

#include <shift/shift.h>
#include "QGraphicsItemRepresentation.h"
#include <QPropertyAnimation>

namespace nslib
{
  class Item
  {
  public:
    Item( void )
      : _parentRep( nullptr )
    {
      _scaleAnim.setPropertyName( "scale" );
      _posAnim.setPropertyName( "pos" );
    }

    virtual ~Item( void )
    {
     if ( _parentRep )
      {
        auto* parentRep_ =
          dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
        if ( parentRep_ )
          parentRep_->clearItems( );
      }
    }

    virtual shift::Representation* parentRep( void ) const
    {
      return _parentRep;
    }

    virtual void parentRep( shift::Representation* parentRep_ )
    {
      _parentRep = parentRep_;
    }

    virtual bool connectionRep( void ) const
    {
      return false;
    }

    QPropertyAnimation& posAnim( ) { return _posAnim; }
    QPropertyAnimation& scaleAnim( ) { return _scaleAnim; }

  protected:
    shift::Representation* _parentRep;
    QPropertyAnimation _posAnim;
    QPropertyAnimation _scaleAnim;
  };
} // namespace nslib

#endif // __NSLIB_ITEM__
