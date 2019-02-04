/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *          Iago Calvo <i.calvol@alumnos.urjc.es>
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
#ifndef __NSLIB_FREE_LAYOUT__
#define __NSLIB_FREE_LAYOUT__

#include <nslib/api.h>
#include <QtWidgets/QStatusBar>
#include "Layout.h"

namespace nslib
{

  class NSLIB_API FreeLayout : public Layout
  {
    public:
    FreeLayout( QStatusBar* statusBar_ );

    void startMoveRepresentation( QGraphicsItem* item_,
      const QPointF clickPos_ );

    void stopMoveActualRepresentation( void );

    void init( void );

    void moveRepresentation( const QPointF newPos_ );

    void display( shift::Entities& entities,
      shift::Representations& representations,
      bool animate = true ) override;

    void removeRelationshipsReps( void );

    protected:
    void _addRepresentations( const shift::Representations& reps ) override;
    void _addRepresentations( const shift::Representations& reps,
      const bool isEntity );
    void _removeRepresentations( const shift::Representations& reps );

    void _arrangeItems( const shift::Representations& reps, bool animate = true,
      const shift::Representations& postFilterReps = shift::Representations( ))
    override;

    void _updateOptionsWidget( void ) override;

    Layout* clone( void ) const override;

    private:
    QGraphicsItem* _movedItem;
    QPointF _moveStart;
    QCheckBox* _moveNewCheckBox;
    OpConfig preRenderOpConfig;
    shift::Representations _relationshipReps;
    shift::Representations _entitiesReps;
    QStatusBar* _statusBar;
  };

}

#endif