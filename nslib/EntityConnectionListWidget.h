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

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <shift/shift.h>
#include <QDockWidget>
#include <QGridLayout>

namespace nslib
{
  class EntityConnectionListWidget:
    public QFrame
  {
    Q_OBJECT

  public:
    EntityConnectionListWidget( shift::Entity*  entity_,
      shift::RelationshipOneToNMapDest*  connectsToMap_,
      shift::RelationshipOneToNMapDest*  connectedByMap_,
      shift::AggregatedOneToNAggregatedDests* aggregatedConnectsToMap_,
      shift::AggregatedOneToNAggregatedDests* aggregatedConnectedByMap_,
      QWidget* parentWidget_ = nullptr );
    static void parentDock( QDockWidget* parentDock_ );
    static QDockWidget* parentDock( void );

  public slots:
    void connectionDialog(
      shift::Entity* originEntity_, shift::Entity* destinationEntity_ );
    void cancelDialog( void );

  private:
    std::vector< std::tuple< QLabel*, QLabel*, QPushButton* >> _connectionsCont;


    static QDockWidget* _parentDock;
    static bool _autoCloseChecked;
  };
}
#endif
