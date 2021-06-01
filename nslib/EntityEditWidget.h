/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Juan Pedro Brito <juanpedro.brito@upm.es>
 *          Iago Calvo Lista <i.calvol@alumnos.urjc.es>
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
#include <QDockWidget>
#include <QGridLayout>
#include <QPushButton>

#include <shift/shift.h>

namespace nslib
{
  class NSLIB_API EntityEditWidget : public QWidget
  {
    Q_OBJECT

  public:

    typedef enum { NEW_ENTITY, EDIT_ENTITY, DUPLICATE_ENTITY }
    TEntityEditWidgetAction;

    EntityEditWidget( QWidget* parentWidget_ = nullptr );
    virtual ~EntityEditWidget( void )
    {}

    shift::Entity* entity( );

    void updateEntity( shift::Entity* entity_, TEntityEditWidgetAction action_,
      shift::Entity* parentOrChildEntity_= nullptr,
      const bool changeParent_ = false, const bool addToScene_ = true );

    static void parentDock( QDockWidget* parentDock_ );
    static QDockWidget* parentDock( void );

  public slots:
    void validateDialog( void );
    void eraseEntity( void );
    void cancelDialog( void );
    void toggleAutoClose( void );
    void toggleCheckUniqueness( void );
    void refreshEntity( void );
    void refreshSubproperties( void );

  private:
    typedef enum { COMBO, LINE_EDIT } TWidgetType;
    typedef enum { WIDGET_TYPE, LABEL, WIDGET } TEditTuple;

    QGridLayout* _gridLayout;
    QGridLayout*_gridLayoutProps;
    QLineEdit* _numNewEntities;
    QCheckBox* _autoCloseCheck;
    QCheckBox* _checkUniquenessCheck;
    QPushButton* _eraseButton;
    QPushButton* _cancelButton;
    QPushButton* _validationButton;
    QLabel* _titleLabel;
    QLabel* _numEntitiesLabel;
    QFrame* _separation;
    std::vector< std::tuple< TWidgetType, QLabel*, QWidget* >> _entityParamCont;

    shift::Entity* _entity;
    shift::Entity* _updateEntity;
    shift::Entity* _parentOrChildEntity;
    shift::Entity* _updateParentOrChildEntity;

    TEntityEditWidgetAction _action;
    TEntityEditWidgetAction _updateAction;

    bool _isNew;
    bool _isNewOrDuplicated;
    bool _isEditing;
    bool _addToScene;
    bool _updateAddToScene;
    bool _changeParent;
    bool _updateChangeParent;

    static QDockWidget* _parentDock;
    static bool _autoCloseChecked;
    static bool _checkUniquenessChecked;

    void updateRelatedEntities( shift::EntitiesWithRelationships& dataEntities_,
      shift::RelationshipOneToOne& relChildOf_,
      bool freeLayoutInUse_, shift::Entity* entity_ ) const;
  };
}

#endif /* CREATIONDIALOG_H_ */
