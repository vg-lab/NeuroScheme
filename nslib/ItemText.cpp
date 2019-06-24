/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo <iago.calvo@urjc.es>
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
#include "ItemText.h"


namespace nslib
{


  ItemText::ItemText( QString string, QGraphicsItem* item_, float maxHeight_,
    float maxWidth_ )
    : QGraphicsTextItem( string )
  {
    auto boundingRect = this->boundingRect( );
    auto height = boundingRect.height( );
    auto width = boundingRect.width( );
    auto itemBounding = item_->boundingRect( );
    auto scale = std::min( maxHeight_ * itemBounding.height() / height,
      maxWidth_ * itemBounding.width( ) / width);
    auto halfScale = -0.5f * scale;
    this->setPos(  halfScale * width, halfScale * height );
    this->setScale( scale );
    this->setDefaultTextColor( QColor::fromRgb( 0, 0, 0, 255 ));
    this->setParentItem( item_ );
  }

}
