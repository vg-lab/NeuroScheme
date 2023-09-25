/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#ifndef __NSLIB_ITEM_TEXT__
#define __NSLIB_ITEM_TEXT__

#include <nslib/api.h>
#include <QString>
#include <QColor>
#include <QGraphicsSimpleTextItem>


namespace nslib
{
  class NSLIB_API ItemText :
    public QGraphicsSimpleTextItem
  {
    public:
    ItemText( const QString& sting, QGraphicsItem* item_,
      const float maxHeight_ = 0.15, const float maxWidth_ = 0.90,
      const QColor colorBrush_ = QColor::fromRgb( 5, 5, 5, 255 ),
      const QColor colorPen_ = QColor::fromRgb( 245, 245, 245, 255 ));
    virtual ~ItemText() {};
  };
}
#endif
