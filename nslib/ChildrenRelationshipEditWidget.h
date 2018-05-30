/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
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

#ifndef __NSLIB_CHILDREN_RELATIONSHIP_EDIT_WIDGET__
#define __NSLIB_CHILDREN_RELATIONSHIP_EDIT_WIDGET__

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <shift/shift.h>

namespace nslib
{
  class ChildrenRelationshipEditWidget:
    public QDialog
  {
    Q_OBJECT

  public:
    ChildrenRelationshipEditWidget(shift::Entity*  parentEntity_,
      std::vector< shift::Entity* > childrenEntities_ , QWidget* parent_ = 0,
      bool modal = false );

  public slots:
    void validateDialog( );
    void cancelDialog( );
    void refreshSubproperties( void );

  private:
    shift::Entity* _parentEntity;
    std::vector< shift::Entity* > _childrenEntities;
    typedef enum { COMBO, LINE_EDIT } TWidgetType;
    typedef enum { WIDGET_TYPE, LABEL, WIDGET } TEditTuple;
    std::vector< std::tuple< TWidgetType, QLabel*, QWidget* >> _propParamCont;
    shift::Properties* _propObject;
  };
}
#endif
