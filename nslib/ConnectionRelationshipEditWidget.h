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

#ifndef __NSLIB_CONNECTION_RELATIONSHIP_EDIT_WIDGET__
#define __NSLIB_CONNECTION_RELATIONSHIP_EDIT_WIDGET__

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <shift/shift.h>

namespace nslib
{
  class ConnectionRelationshipEditWidget:
    public QWidget
  {
    Q_OBJECT

  public:
    ConnectionRelationshipEditWidget( shift::Entity*  originEntity_,
                                      shift::Entity* destinationEntity_,
                                      QWidget* parent = 0);

  public slots:
    void validateDialog( );
    void cancelDialog( );
    void refreshSubproperties( void );

  private:
    shift::Entity* _originEntity;
    shift::Entity* _destinationEntity;
    std::unique_ptr< QLineEdit > _entityLabel;
    typedef enum { COMBO, LINE_EDIT } TWidgetType;
    typedef enum { WIDGET_TYPE, LABEL, WIDGET } TEditTuple;
    std::vector< std::tuple< TWidgetType, QLabel*, QWidget* >> _propParamCont;
    shift::Properties* _propObject;
  };
}
#endif
