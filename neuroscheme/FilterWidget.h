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
#ifndef __NEUROSCHEME__FILTER_WIDGET__
#define __NEUROSCHEME__FILTER_WIDGET__

#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QObject>
#include <QSignalMapper>
#include <QToolButton>
#include <fires/fires.h>

namespace neuroscheme
{
  class Layout;

  class FilterWidget : public QFrame
  {
    Q_OBJECT;
  public:
    FilterWidget( Layout* parentLayout_, QWidget* parent_ = 0 );
    ~FilterWidget( void );
    QComboBox* propertiesSelector( void ) { return _propertiesSelector; }
    void clear( void );

  public slots:
    void addedFilterProperty( void );
    void removeFilterProperty( const QString& propertyLabel );
    void sliderChanged( const QString& propertyLabel );

  protected:
    Layout* _parentLayout;
    QComboBox*_propertiesSelector;
    QToolButton* _addFilterButton;
    unsigned int _numFilterProperties;
    fires::FilterSetConfig _filterSetConfig;
    QSignalMapper* _removeSignalMapper;
    QSignalMapper* _changeSliderSignalMapper;
    std::map< std::string, unsigned int > _layoutRowsMap;
    QCheckBox* _autoFilterCheckBox;
  };

}

#endif
