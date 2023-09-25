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

#include "MiniColumnRep.h"
#include "MiniColumnItem.h"
#include "NeuronRep.h"
#include <nslib/Color.h>
#include <stdint.h>

namespace nslib
{
  namespace cortex
  {
    MiniColumnRep::MiniColumnRep( void )
      : shiftgen::NeuronAggregationRep( )
    {}

    MiniColumnRep::MiniColumnRep( const MiniColumnRep& other )
      : shiftgen::NeuronAggregationRep( other )
    {}

    MiniColumnRep::MiniColumnRep(
      const shiftgen::NeuronAggregationRep& other )
      : shiftgen::NeuronAggregationRep( other )
    {}

    QGraphicsItem* MiniColumnRep::item( QGraphicsScene* scene, bool create )
    {
      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new MiniColumnItem( *this, scene );
      }
      return _items.at( scene );
    }
  } // namespace cortex
} // namespace nslib
