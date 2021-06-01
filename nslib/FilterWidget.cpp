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
#include "Canvas.h"
#include "layouts/Layout.h"
#include "FilterWidget.h"
#include <QGridLayout>
#include <QToolButton>
#include <algorithm>
#include "qxt/qxtspanslider.h"

namespace nslib
{

  FilterWidget::FilterWidget( Layout* parentLayout_, QWidget* parent_ )
    : QFrame( parent_ )
    , _parentLayout( parentLayout_ )
    , _numFilterProperties( 0 )
    , _removeSignalMapper( new QSignalMapper )
    , _changeSliderSignalMapper( new QSignalMapper )
  {
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    this->setLayout( layout_ );

    _propertiesSelector = new QComboBox( );
    _propertiesSelector->setSizePolicy( QSizePolicy::Expanding,
                                        QSizePolicy::Expanding );
    _propertiesSelector->setMinimumSize( 190, 20 );

    QIcon addIcon(":/icons/add.svg");
    _addFilterButton = new QToolButton( );
    _addFilterButton->setIcon( addIcon );

    _useOpacityLabel = new QLabel( "Use opacity" );
    _useOpacityCheckBox = new QCheckBox( );
    _useOpacityCheckBox->setChecked( true );
    _opacitySlider = new QSlider( Qt::Horizontal );
    _opacitySlider->setMinimum( 0);
    _opacitySlider->setMaximum( 100 );
    _opacitySlider->setValue( 20 );

    _autoFilterLabel = new QLabel( "Auto refresh" );
    _autoFilterCheckBox = new QCheckBox( );
    _autoFilterCheckBox->setChecked( true );
    _filterButton = new QPushButton( "Refresh" );
    _filterButton->setEnabled( false );

    auto row = 0;
    layout_->addWidget( _propertiesSelector, row, 0, 1, 1,
      Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _addFilterButton, row, 1, 1, 2,
      Qt::AlignLeft );

    layout_->addWidget( _useOpacityLabel, ++row, 0, 1, 2,
      Qt::AlignLeft );
    layout_->addWidget( _useOpacityCheckBox, row, 1, 1, 2,
      Qt::AlignLeft );
    layout_->addWidget( _opacitySlider, ++row, 0, 1, 2 );

    layout_->addWidget( _autoFilterLabel, ++row, 0, 1, 2,
      Qt::AlignLeft );
    layout_->addWidget( _autoFilterCheckBox, row, 1, 1, 2,
      Qt::AlignLeft );
    layout_->addWidget( _filterButton, ++row, 0, 1, 2,
      Qt::AlignLeft );

    connect( _addFilterButton, SIGNAL( pressed( )),
      this, SLOT( addedFilterProperty( )));

    connect( _autoFilterCheckBox, SIGNAL( pressed( )),
      this, SLOT( _autoFilterCheckBoxChanged( )));

    connect( _useOpacityCheckBox, SIGNAL( clicked( )),
      this, SLOT( _useOpacityCheckBoxChanged( )));

    connect( _filterButton, SIGNAL( pressed( )),
      this, SLOT( refreshParentLayout( )));

    connect( _opacitySlider, SIGNAL( valueChanged( int )),
      this, SLOT( refreshParentLayout( )));
  }

  void FilterWidget::addedFilterProperty( void )
  {
    auto& filters = _filterSetConfig.filters( );
    auto propertyLabel = _propertiesSelector->currentText( ).toStdString( );

    // If property already inserted just return
    auto equalProp = [propertyLabel]( const fires::FilterSetConfig::TFilterPair& p )->bool
    {
          return p.first == propertyLabel;
    };
    if ( std::find_if(filters.cbegin( ), filters.cend( ), equalProp) != filters.cend())
      return;

    auto propertyQLabel = new QLabel( _propertiesSelector->currentText( ));
    QIcon removePropertyIcon(":/icons/close.svg");
    QToolButton* removePropertyButton = new QToolButton( this );
    removePropertyButton->setIcon( removePropertyIcon );

    const auto vMin = _parentLayout->_canvas->properties( ).at( propertyLabel ).rangeMin;
    const auto vMax = _parentLayout->_canvas->properties( ).at( propertyLabel ).rangeMax;
    auto spanSlider = new QxtSpanSlider( Qt::Horizontal, this );
    spanSlider->setSpan( 0, 100 );
    spanSlider->setMinimum(vMin);
    spanSlider->setMaximum(vMax);
    spanSlider->setLowerPosition(vMin);
    spanSlider->setUpperPosition(vMax);
    spanSlider->setEnabled( true );

    const auto rangeText = QString( "[" ) + QString::number( spanSlider->lowerValue( )) +
                           QString( "," ) + QString::number( spanSlider->upperValue( )) +
                           QString( "]" );

    auto rangeLabel =  new QRangeLabel(tr("%1 - Span %1").arg(rangeText));
    connect( spanSlider, SIGNAL( spanChanged( int, int )),
             rangeLabel, SLOT(updateRange(int, int)));

    auto layout_ = dynamic_cast< QGridLayout* >( this->layout( ));

#define LABEL_POS   5 + idx * 3, 0
#define REMOVE_POS  5 + idx * 3, 1
#define SLIDER_POS  6 + idx * 3, 0
#define RANGE_POS   7 + idx * 3, 0

    auto idx = _numFilterProperties;
    layout_->addWidget( propertyQLabel, LABEL_POS, 1, 1, Qt::AlignLeft );
    layout_->addWidget( removePropertyButton, REMOVE_POS,
                        1, 1, Qt::AlignRight );
    layout_->addWidget( spanSlider, SLIDER_POS, 1, 2 ); //, Qt::AlignLeft );
    layout_->addWidget( rangeLabel, RANGE_POS, 1, 2, Qt::AlignLeft );

    auto filter = fires::PropertyManager::getFilter( propertyLabel );
    fires::PropertyManager::setFilterRange( filter,
                                            spanSlider->lowerPosition( ),
                                            spanSlider->upperPosition( ));
    _filterSetConfig.filters( ).push_back(
      std::make_pair( propertyLabel, filter ));

    _layoutRowsMap[ propertyLabel ] = _numFilterProperties;
    ++_numFilterProperties;

    // Connect remove button
    connect( removePropertyButton, SIGNAL( pressed( )),
             _removeSignalMapper, SLOT( map( )));
    _removeSignalMapper->setMapping( removePropertyButton,
                                     QString( propertyLabel.c_str( )));
    connect( _removeSignalMapper, SIGNAL( mapped( const QString& )),
             this, SLOT( removeFilterProperty( const QString&  )));

    // Connect slider
    connect( spanSlider, SIGNAL( spanChanged( int, int )),
             _changeSliderSignalMapper, SLOT( map( )));
    _changeSliderSignalMapper->setMapping( spanSlider,
                                           QString( propertyLabel.c_str( )));
    connect( _changeSliderSignalMapper, SIGNAL( mapped( const QString& )),
             this, SLOT( sliderChanged( const QString&  )));
  }

  void FilterWidget::clear( void )
  {
    if ( _propertiesSelector )
      _propertiesSelector->clear( );

    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));
    for ( const auto& row : _layoutRowsMap )
    {
      const auto idx = row.second;
      auto item = qGridLayout->itemAtPosition( LABEL_POS );
      assert( item );
      delete item->widget( );

      item = qGridLayout->itemAtPosition( REMOVE_POS );
      assert( item );
      delete item->widget( );

      item = qGridLayout->itemAtPosition( SLIDER_POS );
      assert( item );
      delete item->widget( );

      item = qGridLayout->itemAtPosition( RANGE_POS );
      assert( item );
      delete item->widget( );
    }

    _layoutRowsMap.clear( );
    _numFilterProperties = 0;
    _filterSetConfig.clear( );
    delete _removeSignalMapper;
    _removeSignalMapper = new QSignalMapper;
  }

  void FilterWidget::removeFilterProperty( const QString& propertyLabel_ )
  {
    auto& filters = _filterSetConfig.filters( );
    const auto propertyLabel = propertyLabel_.toStdString( );

    // If property not inserted just return
    auto equalProp = [propertyLabel](fires::FilterSetConfig::TFilterPair& p )->bool
    {
      return p.first == propertyLabel;
    };
    auto it = std::find_if(filters.begin( ), filters.end( ), equalProp);
    if ( it == filters.end( ))
      return;

    filters.erase( it );

    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));

    const auto idx = _layoutRowsMap[ propertyLabel ];

    auto item = qGridLayout->itemAtPosition( LABEL_POS );
    assert( item );
    delete item->widget( );

    item = qGridLayout->itemAtPosition( REMOVE_POS );
    assert( item );
    delete item->widget( );

    item = qGridLayout->itemAtPosition( SLIDER_POS );
    assert( item );
    delete item->widget( );

    item = qGridLayout->itemAtPosition( RANGE_POS );
    assert( item );
    delete item->widget( );

    _layoutRowsMap.erase( propertyLabel );
  }

  void FilterWidget::sliderChanged( const QString& propertyLabel_ )
  {
    auto& filters = _filterSetConfig.filters( );
    auto propertyLabel = propertyLabel_.toStdString( );

    // If property not inserted just return
    auto equalProp = [propertyLabel]( const fires::FilterSetConfig::TFilterPair& p )->bool
    {
      return p.first == propertyLabel;
    };
    auto it = std::find_if(filters.cbegin( ), filters.cend( ), equalProp);
    if ( it == filters.cend( ))
      return;

    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));

    const auto idx = _layoutRowsMap[ propertyLabel ];
    auto item = qGridLayout->itemAtPosition( SLIDER_POS );
    assert( item );
    auto spanSlider = dynamic_cast< QxtSpanSlider* >( item->widget( ));
    assert( spanSlider );

    // Update fires filter
    auto firesFilter = fires::PropertyManager::getFilter( propertyLabel );
    fires::PropertyManager::setFilterRange( firesFilter,
                                            spanSlider->lowerPosition( ),
                                            spanSlider->upperPosition( ));

    if ( _autoFilterCheckBox->isChecked( ))
    {
      refreshParentLayout( );
    }
  }

  void FilterWidget::refreshParentLayout( void )
  {
    _parentLayout->refresh( false );
  }

  void FilterWidget::_autoFilterCheckBoxChanged( void )
  {
    _filterButton->setEnabled( _autoFilterCheckBox->isChecked( ));
    if ( !_autoFilterCheckBox->isChecked( ))
      refreshParentLayout( );
  }

  void FilterWidget::_useOpacityCheckBoxChanged( void )
  {
    _opacitySlider->setEnabled( _useOpacityCheckBox->isChecked( ));
    std::cout << "Refresh " << useOpacityForFiltering( ) << std::endl;
    refreshParentLayout( );
  }

  FilterWidget::~FilterWidget( void )
  {
    delete _propertiesSelector;
    delete _addFilterButton;
    delete _removeSignalMapper;
    delete _changeSliderSignalMapper;
    delete _autoFilterCheckBox;
    delete _useOpacityLabel;
    delete _opacitySlider;
  }

  void QRangeLabel::updateRange(int l, int u)
  {
    auto slider = qobject_cast<QxtSpanSlider *>(sender());
    if(slider)
    {
      auto rangeText = tr("[%1,%2]");
      auto current = rangeText.arg(QString::number(l)).arg(QString::number(u));
      auto total = rangeText.arg(QString::number(slider->minimum())).arg(QString::number(slider->maximum()));

      setText(tr("%1 - Span %2").arg(total).arg(current));
    }
  }
}
