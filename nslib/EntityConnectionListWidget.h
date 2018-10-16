/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo Lista <i.calvol@alumnos.urjc.es>
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

#ifndef __NSLIB_ENTITY_CONNECTION_LIST_WIDGET__
#define __NSLIB_ENTITY_CONNECTION_LIST_WIDGET__

#include <nslib/api.h>

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <shift/shift.h>
#include <QDockWidget>
#include <QGridLayout>
#include <QTableView>
#include <QItemDelegate>
#include "ConnectionTableWidget.h"


namespace nslib
{
  class ConnectionTableWidget;

  class NSLIB_API EntityConnectionListWidget:
    public QFrame
  {
    Q_OBJECT

  public:
    EntityConnectionListWidget( QWidget* parentWidget_ = nullptr );
    void setConnections( shift::Entity*  entity_,
      shift::RelationshipOneToNMapDest*  connectsToMap_,
      shift::RelationshipOneToNMapDest*  connectedByMap_,
      shift::AggregatedOneToNAggregatedDests* aggregatedConnectsToMap_,
      shift::AggregatedOneToNAggregatedDests* aggregatedConnectedByMap_ );
    void updateConnections( shift::EntityGid origEntity_,
      shift::EntityGid destEntity, const bool updateAggregatedTo = true,
      const bool updateAggregatedBy = true );
    static void parentDock( QDockWidget* parentDock_ );
    static QDockWidget* parentDock( void );
    void checkClose( void );
    void updateConnections( void );
    shift::Entity* entity( void );

    public slots:
    void cancelDialog( void );
    void toggleAutoClose( void );
    void toggleAllProperties( void );

  private:
    std::vector< std::tuple< QLabel*, QLabel*, QPushButton* >> _connectionsCont;
    static QDockWidget* _parentDock;
    shift::Entity* _entity;
    static bool _autoCloseChecked;
    static bool _allPropsChecked;
    ConnectionTableWidget* _connectsToTable;
    ConnectionTableWidget* _connectedByTable;
    ConnectionTableWidget* _aggregatedConnectsToTable;
    ConnectionTableWidget* _aggregatedConnectedByTable;
    std::unique_ptr< QGridLayout > _gridLayout;
    std::unique_ptr< QLabel > _autoCloseLabel;
    std::unique_ptr< QCheckBox > _autoCloseCheck;
    std::unique_ptr< QLabel > _allPropertiesLabel;
    std::unique_ptr< QCheckBox > _allPropertiesCheck;
    std::unique_ptr< QPushButton > _cancelButton;
  };

}
#endif
