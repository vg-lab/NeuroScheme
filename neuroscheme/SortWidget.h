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
#ifndef __NEUROSCHEME__SORT_WIDGET__
#define __NEUROSCHEME__SORT_WIDGET__

#include <QComboBox>
#include <QFrame>
#include <QObject>
#include <QSignalMapper>
#include <QToolButton>
#include <fires/fires.h>

namespace neuroscheme
{
  class Layout;

  class SortWidget : public QFrame
  {
    Q_OBJECT;
  public:
    SortWidget( Layout* parentLayout_, QWidget* parent_ = 0 );
    ~SortWidget( void );
    QComboBox* propertiesSelector( void ) { return _propertiesSelector; }
    void clear( );
    fires::SortConfig& sortConfig( ) { return _sortConfig; }

  public slots:
    void addedSortProperty( void );
    void removeSortProperty( const QString& propertyLabel );
    void changeSortPropertyDir( const QString& propertyLabel );

  protected:
    Layout* _parentLayout;
    QComboBox*_propertiesSelector;
    QToolButton* _sortButton;
    unsigned int _numSortProperties;
    fires::SortConfig _sortConfig;
    QSignalMapper* _removeSignalMapper;
    QSignalMapper* _changeDirSignalMapper;
    std::map< std::string, unsigned int > _layoutRowsMap;
  };

}

#endif
