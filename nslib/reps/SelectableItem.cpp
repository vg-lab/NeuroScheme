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
#include "SelectableItem.h"

namespace  nslib
{
  QPen SelectableItem::_selectedPen =
    QPen( Qt::red, 2, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_hoverSelectedPen =
    QPen( Qt::red, 2, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_partiallySelectedPen =
    QPen( QColor( 255, 170, 80, 255 ), 2, Qt::SolidLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_hoverPartiallySelectedPen =
    QPen( QColor( 255, 170, 80, 255 ), 2, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  QPen SelectableItem::_unselectedPen = QPen( Qt::NoPen );

  QPen SelectableItem::_hoverUnselectedPen =
    QPen( QColor( 90, 90, 90, 255 ), 2, Qt::DotLine,
          Qt::RoundCap, Qt::RoundJoin );

  void SelectableItem::init( void )
  {
    char* scaleFactorString = getenv( "NEUROSCHEME_SCALE_FACTOR" );
    if ( scaleFactorString )
    {
      auto scaleFactor = atof( scaleFactorString );
      _selectedPen.setWidth( _selectedPen.width( ) * scaleFactor );
      _hoverSelectedPen.setWidth( _hoverSelectedPen.width( ) * scaleFactor );
      _partiallySelectedPen.setWidth( _partiallySelectedPen.width( ) * scaleFactor );
      _hoverPartiallySelectedPen.setWidth( _hoverPartiallySelectedPen.width( ) *
                                           scaleFactor );
      _unselectedPen.setWidth( _unselectedPen.width( ) * scaleFactor );
      _hoverUnselectedPen.setWidth( _hoverUnselectedPen.width( ) * scaleFactor );
    }
    _selectedPen.setCosmetic( true );
    _hoverSelectedPen.setCosmetic( true );
    _partiallySelectedPen.setCosmetic( true );
    _hoverPartiallySelectedPen.setCosmetic( true );
    _unselectedPen.setCosmetic( true );
    _hoverUnselectedPen.setCosmetic( true );
  }

}
