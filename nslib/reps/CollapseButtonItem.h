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
#ifndef __NSLIB__COLLAPSE_BUTTON_ITEM__
#define __NSLIB__COLLAPSE_BUTTON_ITEM__

#include <nslib/api.h>
#include <QGraphicsEllipseItem>

namespace nslib
{
  class NSLIB_API CollapseButtonItem
  : public QGraphicsEllipseItem
  {
    public:
      CollapseButtonItem( void );
      void mousePressEvent( QGraphicsSceneMouseEvent* event );
      void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
      void hoverLeaveEvent( QGraphicsSceneHoverEvent* event );
  };
} // namespace nslib

#endif // __NSLIB__COLLAPSE_BUTTON_ITEM__
