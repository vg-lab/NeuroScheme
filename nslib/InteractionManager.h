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
#include "EntityEditWidget.h"
#include "reps/SelectableItem.h"
#include "ConnectionRelationshipEditWidget.h"
#include "EntityConnectionListWidget.h"
#include <shift/shift.h>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QPen>
namespace nslib
{

  class TemporalConnectionLine : public QGraphicsLineItem
  {
  public:
    ~TemporalConnectionLine( void )
    {
      // This check could be removed but for the moment serves as a watchdog
      // that ensures that if the line is deleted by any of the QtGraphicsScene
      // methods we will get notified
      // assert( false );
    }
  };

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

    static void mousePressEvent( QGraphicsItem* item,
                                 QMouseEvent* event );

    static void mouseReleaseEvent( QGraphicsItem* item,
                                   QMouseEvent* event );

    static void mouseMoveEvent( QGraphicsView* graphicsView,
                                QAbstractGraphicsShapeItem* shapeItem,
                                QMouseEvent* event );

    static void updateEntityConnectionList( shift::EntityGid origEntity_,
      shift::EntityGid destEntity, bool updateAggregatedTo_ = true,
      bool updateAggregatedBy_ = true );

    static void updateConnectionRelationship(
        shift::Entity* originEntity_, shift::Entity* destEntity_ );

    static void createConnectionRelationship(
      shift::Entity* originEntity_, shift::Entity* destinationEntity_ );
    static void createOrEditEntity(
      shift::Entity* entity_, EntityEditWidget::TEntityEditWidgetAction action_,
      shift::Entity* parentEntity_= nullptr,
      bool addToScene_ = true, QWidget *parentWidget_ = nullptr );

    static void queryChildrenSelectedState(
      const shift::Entities& entities,
      const shift::RelationshipOneToN& relParentOf,
      unsigned int entityGid,
      bool& allChildrenSelected,
      bool& noChildrenSelected );

    static void deleteEntity( shift::EntityGid entityGid_ );

  protected:

    static void basicDeleteEntity( shift::Entity* entity_,
      shift::Entities& dataEntities_, shift::RelationshipOneToN& relConnectsTo_,
      shift::RelationshipOneToN& relConnectedBy_,
      shift::RelationshipOneToN& relParentOf_,
      shift::RelationshipOneToOne& relChildOf_,
      shift::RelationshipAggregatedOneToN& relAggregatedConnectsTo_,
      shift::RelationshipAggregatedOneToN& relAggregatedConnectBy_,
      shift::RelationshipOneToOne& relSubEntityOf_,
      shift::RelationshipOneToN& relSuperEntityOf_,
      shift::RelationshipOneToN& relAGroupOf_,
      shift::RelationshipOneToN& relAPartOf_);

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
    static ConnectionRelationshipEditWidget* _conRelationshipEditWidget;
    static EntityEditWidget* _entityEditWidget;
    static EntityConnectionListWidget* _entityConnectionListWidget;
    static QGraphicsItem* _item;
    static Qt::MouseButtons _buttons;
    static std::unique_ptr< TemporalConnectionLine > _tmpConnectionLine;
    static QAbstractGraphicsShapeItem* lastShapeItemHoveredOnMouseMove;

  };
}

#endif
