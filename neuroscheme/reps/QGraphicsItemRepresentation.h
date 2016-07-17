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
#ifndef __NEUROSCHEME_QGRAPHICSITEM_REPRESENTATION__
#define __NEUROSCHEME_QGRAPHICSITEM_REPRESENTATION__

#ifdef NEUROSCHEME_USE_QT5WIDGETS

#include <QGraphicsItem>

namespace neuroscheme
{

  class QGraphicsItemRepresentation
  {

  public:

    QGraphicsItemRepresentation( void )
      : _item( nullptr )
    {}
    virtual ~QGraphicsItemRepresentation( void ) {}
    virtual QGraphicsItem* item( bool create = true ) = 0;

  protected:

    QGraphicsItem* _item;

  };


} // namespace neuroscheme

#endif // NEUROSCHEME_USE_QT5WIDGETS
#endif // __NEUROSCHEME_QGRAPHICSITEM_REPRESENTATION__
