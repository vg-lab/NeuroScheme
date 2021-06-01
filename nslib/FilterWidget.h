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
#ifndef __NSLIB__FILTER_WIDGET__
#define __NSLIB__FILTER_WIDGET__

#include <nslib/api.h>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QObject>
#include <QSignalMapper>
#include <QToolButton>
#include <fires/fires.h>

namespace nslib
{
  class Layout;

  class NSLIB_API FilterWidget : public QFrame
  {
    Q_OBJECT;
  public:
    FilterWidget( Layout* parentLayout_, QWidget* parent_ = 0 );
    ~FilterWidget( void );
    QComboBox* propertiesSelector( void ) { return _propertiesSelector; }
    void clear( void );
    const fires::FilterSetConfig& filterSetConfig( void ) const
    { return _filterSetConfig; }
    fires::FilterSetConfig& filterSetConfig( void )
    { return _filterSetConfig; }

    bool useOpacityForFiltering( void )
    { return _useOpacityCheckBox->isChecked( ); }
    int opacityValue( void ) { return _opacitySlider->value( ); }

  public slots:
    void addedFilterProperty( void );
    void removeFilterProperty( const QString& propertyLabel );
    void sliderChanged( const QString& propertyLabel );
    void refreshParentLayout( void );

  protected slots:
    void _autoFilterCheckBoxChanged( void );
    void _useOpacityCheckBoxChanged( void );

  protected:
    Layout* _parentLayout;
    QComboBox*_propertiesSelector;
    QToolButton* _addFilterButton;
    unsigned int _numFilterProperties;
    fires::FilterSetConfig _filterSetConfig;
    QSignalMapper* _removeSignalMapper;
    QSignalMapper* _changeSliderSignalMapper;
    std::map< std::string, unsigned int > _layoutRowsMap;
    QPushButton* _filterButton;
    QLabel* _autoFilterLabel;
    QCheckBox* _autoFilterCheckBox;
    QLabel* _useOpacityLabel;
    QSlider* _opacitySlider;
    QCheckBox* _useOpacityCheckBox;
  };

  class QRangeLabel: public QLabel
  {
      Q_OBJECT
    public:
      explicit QRangeLabel(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags())
      : QLabel(parent, f)
      {};

      explicit QRangeLabel(const QString &text, QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags())
      : QLabel(text, parent, f)
      {};

    public slots:
      void updateRange(int l, int u);
  };
}

#endif
