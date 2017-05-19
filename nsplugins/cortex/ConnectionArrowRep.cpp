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
#include "ConnectionArrowItem.h"
#include "NeuronItem.h"
#include <nslib/Color.h>
#include <stdint.h>


namespace nslib
{
  namespace cortex
  {

    ConnectionArrowRep::ConnectionArrowRep( void )
      : shiftgen::ConnectionArrowRep( )
    {
    }

    ConnectionArrowRep::ConnectionArrowRep( const ConnectionArrowRep& other )
      : shiftgen::ConnectionArrowRep( other )
    {
    }

    ConnectionArrowRep::ConnectionArrowRep( const shiftgen::ConnectionArrowRep& other )
      : shiftgen::ConnectionArrowRep( other )
    {
    }


    QGraphicsItem* ConnectionArrowRep::item( QGraphicsScene* scene, bool create )
    {
      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new ConnectionArrowItem( *this );
      }
      return _items.at( scene );
    }

  } // namespace cortex
} // namespace nslib
