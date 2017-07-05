/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Juan Pedro Brito <juanpedro.brito@upm.es>
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
#ifndef __NSLIB__ENTITY_EDIT_WIDGET__
#define __NSLIB__ENTITY_EDIT_WIDGET__

#include <nslib/api.h>
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QErrorMessage>
#include <QDockWidget>

#include <shift/shift.h>

namespace nslib
{
  class NSLIB_API EntityEditWidget : public QWidget
  {
    Q_OBJECT

  public:

    typedef enum { NEW_ENTITY, EDIT_ENTITY, DUPLICATE_ENTITY }
      TEntityEditWidgetAction;

    EntityEditWidget( shift::Entity* entity, TEntityEditWidgetAction action,
                      QWidget *parent = 0 );

    static void parentDock( QDockWidget* parentDock_ );
    static QDockWidget* parentDock( void );

  public slots:

    void validateDialog( );
    void cancelDialog( );


  private:
    typedef enum { COMBO, LINE_EDIT } TWidgetType;
    typedef enum { WIDGET_TYPE, LABEL, WIDGET } TEditTuple;
    std::vector< std::tuple< TWidgetType, QLabel*, QWidget* >> _entityParamCont;
    shift::Entity* _entity;
    TEntityEditWidgetAction _action;
    static QDockWidget* _parentDock;

  };
}

#endif /* CREATIONDIALOG_H_ */
