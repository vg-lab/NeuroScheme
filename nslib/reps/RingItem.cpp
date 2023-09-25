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
#include "RingItem.h"
#include <QBrush>

namespace nslib
{
  RingItem::RingItem( unsigned int initAngle,
                      unsigned int xRadius,
                      unsigned int yRadius,
                      unsigned int width,
                      int angle,
                      Color color )
  {
    // Define outer circle
    int x_ = static_cast<int>( xRadius );
    int y_ = static_cast<int>( yRadius );
    QPainterPath outerArc( QPoint( 0, 0 ));
    outerArc.arcTo( -x_, -y_, x_ * 2, y_ * 2, initAngle, angle );
    QPainterPath outerFill( QPoint( 0, 0 ));
    outerFill.arcTo( - x_, - y_, x_ * 2, y_ * 2, initAngle, angle );
    outerFill.closeSubpath( );
    ( void ) outerFill.united( outerArc );

    // Define inner circle
    x_ = static_cast<int>( xRadius ) - static_cast<int>( width );
    y_ = static_cast<int>( yRadius ) - static_cast<int>( width );
    QPainterPath innerArc( QPoint( 0, 0 ));
    innerArc.arcTo( -x_, -y_, x_ * 2, y_ * 2, initAngle, angle );
    QPainterPath innerFill( QPoint( 0, 0 ));
    innerFill.arcTo( - x_, - y_, x_ * 2, y_ * 2, initAngle, angle );
    innerFill.closeSubpath( );
    ( void ) innerFill.united( innerArc );

    // Subtract inner from outer
    QPainterPath ringFill = outerFill.subtracted( innerFill );
    this->setPath( ringFill );

    setBrush( QBrush( color ));
  }
} // namespace nslib
