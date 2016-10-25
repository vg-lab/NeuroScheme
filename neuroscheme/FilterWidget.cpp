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
#include "Layout.h"
#include "FilterWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <algorithm>
#include "qxt/qxtspanslider.h"

namespace neuroscheme
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

    QIcon addIcon( QString::fromUtf8(":/icons/add.png"));
    _addFilterButton = new QToolButton( );
    _addFilterButton->setIcon( addIcon );

    auto autoFilterLabel = new QLabel( "Auto refresh" );
    _autoFilterCheckBox = new QCheckBox( );
    auto filterButton = new QPushButton( "Filter" );

    layout_->addWidget( _propertiesSelector, 0, 0, 1, 1,
                        Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _addFilterButton, 0, 1, 1, 2,
                        Qt::AlignLeft );

    layout_->addWidget( autoFilterLabel, 1, 0, 1, 2,
                        Qt::AlignLeft );
    layout_->addWidget( _autoFilterCheckBox, 1, 1, 1, 2,
                        Qt::AlignLeft );
    layout_->addWidget( filterButton, 2, 0, 1, 2,
                        Qt::AlignLeft );

    connect( _addFilterButton, SIGNAL( pressed( )),
             this, SLOT( addedFilterProperty( )));

  }


  void FilterWidget::addedFilterProperty( void )
  {
    auto& filters = _filterSetConfig.filters( );
    auto propertyLabel = _propertiesSelector->currentText( ).toStdString( );

    // If property already inserted just return
    if ( std::find_if(
           filters.begin( ), filters.end( ),
           [ propertyLabel ]( fires::FilterSetConfig::TFilterPair& p )->bool
           {
             return p.first == propertyLabel;
           }) != filters.end( ))
      return;

    std::cout << "<><><><> Added filtered propery "
              << _propertiesSelector->currentText( ).toStdString( )
              << std::endl;

    QLabel* propertyQLabel = new QLabel( _propertiesSelector->currentText( ));
    QIcon removePropertyIcon( QString::fromUtf8( ":/icons/close.png" ));
    QToolButton* removePropertyButton = new QToolButton( this );
    removePropertyButton->setIcon( removePropertyIcon );

    auto spanSlider = new QxtSpanSlider( Qt::Horizontal, this );
    spanSlider->setSpan( 0, 100 );
    spanSlider->setMinimum(
      _parentLayout->_properties[ propertyLabel ].rangeMin );
    spanSlider->setMaximum(
      _parentLayout->_properties[ propertyLabel ].rangeMax );
    spanSlider->setLowerPosition(
      _parentLayout->_properties[ propertyLabel ].rangeMin );
    spanSlider->setUpperPosition(
      _parentLayout->_properties[ propertyLabel ].rangeMax );
    spanSlider->setEnabled( true );

    auto rangeLabel =  new QLabel(
      QString( "[" ) + QString::number( spanSlider->lowerValue( )) +
      QString( "," ) + QString::number( spanSlider->upperValue( )) +
      QString( "]" ) );

    auto layout = dynamic_cast< QGridLayout* >( this->layout( ));

#define LABEL_POS   3 + idx * 3, 0
#define REMOVE_POS  3 + idx * 3, 1
#define SLIDER_POS  4 + idx * 3, 0
#define RANGE_POS   5 + idx * 3, 0
    auto idx = _numFilterProperties;
    layout->addWidget( propertyQLabel, LABEL_POS, 1, 1, Qt::AlignLeft );
    layout->addWidget( removePropertyButton, REMOVE_POS, 1, 1, Qt::AlignRight );
    layout->addWidget( spanSlider, SLIDER_POS, 1, 2 ); //, Qt::AlignLeft );
    layout->addWidget( rangeLabel, RANGE_POS, 1, 2, Qt::AlignLeft );

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

//     std::cout << "["
//               << _filterConfig.properties( ).size( ) << "]: ";
//     for ( const auto& p : _filterConfig.properties( ))
//     {
//       std::cout << "{" << p.label << "," << (int) p.order << "}";
//       assert( p.filterer );
//     }
//     std::cout <<  std::endl;

  }

  void FilterWidget::clear( void )
  {
    if ( _propertiesSelector )
      _propertiesSelector->clear( );
    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));
    for ( const auto& row : _layoutRowsMap )
    {
      auto idx = row.second;
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
    auto propertyLabel = propertyLabel_.toStdString( );

    // If property not inserted just return
    auto filter = std::find_if(
      filters.begin( ), filters.end( ),
      [ propertyLabel ]( fires::FilterSetConfig::TFilterPair& p )->bool
      {
             return p.first == propertyLabel;
      });
    if ( filter == filters.end( ))
      return;

    std::cout << "Remove property "
              << propertyLabel_.toStdString( ) << std::endl;

    filters.erase( filter );

    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));

    auto idx = _layoutRowsMap[ propertyLabel ];
    std::cout << "remove idx = " << idx << " " << propertyLabel << std::endl;
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

    // std::cout << "["
    //           << _filterConfig.properties( ).size( ) << "]: ";
    // for ( const auto& p : _filterConfig.properties( ))
    // {
    //   std::cout << "{" << p.label << "," << (int) p.order << "}";
    //   assert( p.filterer );
    // }
    // std::cout <<  std::endl;

  }

  void FilterWidget::sliderChanged( const QString& propertyLabel_ )
  {
    auto& filters = _filterSetConfig.filters( );
    auto propertyLabel = propertyLabel_.toStdString( );

    // If property not inserted just return
    auto filter = std::find_if(
      filters.begin( ), filters.end( ),
      [ propertyLabel ]( fires::FilterSetConfig::TFilterPair& p )->bool
      {
             return p.first == propertyLabel;
      });
    if ( filter == filters.end( ))
      return;

    std::cout << "Changed slider of property "
              << propertyLabel_.toStdString( ) << std::endl;

    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));

    auto idx = _layoutRowsMap[ propertyLabel ];
    auto item = qGridLayout->itemAtPosition( SLIDER_POS );
    assert( item );
    auto spanSlider = dynamic_cast< QxtSpanSlider* >( item->widget( ));
    assert( spanSlider );

    // Update fires filter
    auto firesFilter = fires::PropertyManager::getFilter( propertyLabel );
    fires::PropertyManager::setFilterRange( firesFilter,
                                            spanSlider->lowerPosition( ),
                                            spanSlider->upperPosition( ));

  }

  FilterWidget::~FilterWidget( void )
  {
    delete _propertiesSelector;
    delete _addFilterButton;
    delete _removeSignalMapper;
    delete _changeSliderSignalMapper;
  }

}
