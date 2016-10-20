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
#include "SortWidget.h"
#include <QGridLayout>

namespace neuroscheme
{

  SortWidget::SortWidget( QWidget* parent_ )
    : QFrame( parent_ )
  {
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    this->setLayout( layout_ );

    _propertiesSelector = new QComboBox( );
    _propertiesSelector->setSizePolicy( QSizePolicy::Expanding,
                                       QSizePolicy::Expanding );
    _propertiesSelector->setMinimumSize( 200, 20 );

    QIcon addIcon( QString::fromUtf8(":/icons/add.png"));
    _sortButton = new QToolButton( );
    _sortButton->setIcon( addIcon );

    layout_->addWidget( _propertiesSelector, 0, 0, 1, 2,
                        Qt::AlignCenter | Qt::AlignLeft);
    layout_->addWidget( _sortButton, 0, 3, 1, 1,
                        Qt::AlignCenter | Qt::AlignHCenter);

  }

}
