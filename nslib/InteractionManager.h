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
#ifndef __NSLIB__INTERACTION_MANAGER__
#define __NSLIB__INTERACTION_MANAGER__


#include <nslib/api.h>
#include "CreationDialog.h"
#include "reps/SelectableItem.h"
#include <shift/shift.h>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPen>
#include <iostream> // Borrar
namespace nslib
{

  class NSLIB_API InteractionManager
  {
  public:
    static void highlightConnectivity( QAbstractGraphicsShapeItem* item,
                                       bool highlight = true );


    static void hoverEnterEvent( QAbstractGraphicsShapeItem* item,
                                 QGraphicsSceneHoverEvent* event );

    static void hoverLeaveEvent( QAbstractGraphicsShapeItem* item,
                                 QGraphicsSceneHoverEvent* event );

    static void contextMenuEvent( QAbstractGraphicsShapeItem* item,
                                  QGraphicsSceneContextMenuEvent* event );

    static void mousePressEvent( QAbstractGraphicsShapeItem* item,
                                 QGraphicsSceneMouseEvent* event );


    // static const QPen& getSelectedPen( ) { return _selectedPen; }
    // static const QPen& getPartiallySelectedPen( )
    // {
    //   return _partiallySelectedPen;
    // }
    // static const QPen& getUnselectedPen( )
    // {
    //   return _unselectedPen;
    // }

    static void queryChildrenSelectedState(
      const shift::Entities& entities,
      const shift::RelationshipOneToN& relParentOf,
      unsigned int entityGid,
      bool& allChildrenSelected,
      bool& noChildrenSelected );

  protected:


  // void InteractionManager::_PropagateSelectedToChilds(
  //   shift::Entities& entities,
  //   shift::RelationshipOneToN& relParentOf,
  //   unsigned int entityGid,
  //   SelectedState state

    static void _propagateSelectedStateToChilds(
      const shift::Entities& entities,
      const shift::RelationshipOneToN& relParentOf,
      const shift::RelationshipOneToN& relSuperEntityOf,
      unsigned int entityGid,
      SelectedState state );

    static void _propagateSelectedStateToParent(
      const shift::Entities& entities,
      const shift::RelationshipOneToOne& relChildOf,
      const shift::RelationshipOneToN& relParentOf,
      const shift::RelationshipOneToN& relSuperEntityOf,
      const shift::RelationshipOneToN& relAGroupOf,
      unsigned int entityGid,
      SelectedState state = SelectedState::SELECTED );

    static void _updateSelectedStateOfSubEntities(
      const shift::Entities& entities,
      const shift::RelationshipOneToN& relSuperEntityOf,
      const shift::RelationshipOneToN& relAGroupOf,
      unsigned int entityGid );

    static void _queryGroupedSelectedState(
      const shift::Entities& entities,
      const shift::RelationshipOneToN& relAGroupOf,
      unsigned int entityGid,
      bool& allGroupedSelected,
      bool& noGroupedSelected );

    static QMenu* _contextMenu;
    static CreationDialog* _creationDialog;

  };
}

#endif
