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
#include <QBrush>
#include <QPen>


namespace nslib
{


  ItemText::ItemText( const QString& string, QGraphicsItem* item_,
    const float maxHeight_, const float maxWidth_, const QColor colorBrush_,
    const QColor colorPen_ )
    : QGraphicsSimpleTextItem( string )
  {
    if( !string.isEmpty( ))
    {
      const auto boundingRect = this->boundingRect( );
      const auto height = boundingRect.height( );
      const auto width = boundingRect.width( );
      const auto itemBounding = item_->boundingRect( );
      const auto scale = std::min( maxHeight_ * itemBounding.height( ) / height,
        maxWidth_ * itemBounding.width( ) / width );
      const auto halfScale = -0.5f * scale;
      this->setPos( halfScale * width, halfScale * height );
      this->setScale( scale );
      this->setBrush( colorBrush_ );
      this->setPen( QPen( colorPen_, 0.5f ));
//      this->setDefaultTextColor( colorBrush_ );
      this->setParentItem( item_ );
    }
  }

}
