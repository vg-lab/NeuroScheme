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
#include "SelectableItem.h"
#include "../Config.h"

namespace  nslib
{
  // These QPen are initialized in init method
  QPen SelectableItem::_selectedPen = Qt::NoPen;
  QPen SelectableItem::_hoverSelectedPen = Qt::NoPen;
  QPen SelectableItem::_partiallySelectedPen = Qt::NoPen;
  QPen SelectableItem::_hoverPartiallySelectedPen = Qt::NoPen;
  QPen SelectableItem::_unselectedPen = Qt::NoPen;
  QPen SelectableItem::_hoverUnselectedPen = Qt::NoPen;

  void SelectableItem::init( void )
  {
    auto penWidth = 2 * nslib::Config::scale( );

    _selectedPen =
      QPen( Qt::red, penWidth, Qt::SolidLine,
            Qt::RoundCap, Qt::RoundJoin );

    _hoverSelectedPen =
      QPen( Qt::red, penWidth, Qt::DotLine,
            Qt::RoundCap, Qt::RoundJoin );

    _partiallySelectedPen =
      QPen( QColor( 255, 170, 80, 255 ),
            penWidth, Qt::SolidLine,
            Qt::RoundCap, Qt::RoundJoin );

    _hoverPartiallySelectedPen =
      QPen( QColor( 255, 170, 80, 255 ),
            penWidth, Qt::DotLine,
            Qt::RoundCap, Qt::RoundJoin );

    _unselectedPen = QPen( Qt::NoPen );

    _hoverUnselectedPen =
      QPen( QColor( 90, 90, 90, 255 ),
            penWidth, Qt::DotLine,
            Qt::RoundCap, Qt::RoundJoin );

    _selectedPen.setCosmetic( true );
    _hoverSelectedPen.setCosmetic( true );
    _partiallySelectedPen.setCosmetic( true );
    _hoverPartiallySelectedPen.setCosmetic( true );
    _unselectedPen.setCosmetic( true );
    _hoverUnselectedPen.setCosmetic( true );
  }
}
