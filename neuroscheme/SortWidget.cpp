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
#include "SortWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <algorithm>

namespace neuroscheme
{

  SortWidget::SortWidget( Layout* parentLayout_, QWidget* parent_ )
    : QFrame( parent_ )
    , _parentLayout( parentLayout_ )
    , _numSortProperties( 0 )
    , _removeSignalMapper( new QSignalMapper )
    , _changeDirSignalMapper( new QSignalMapper )
  {
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    this->setLayout( layout_ );

    _propertiesSelector = new QComboBox( );
    _propertiesSelector->setSizePolicy( QSizePolicy::Expanding,
                                        QSizePolicy::Expanding );
    _propertiesSelector->setMinimumSize( 190, 20 );

    QIcon addIcon( QString::fromUtf8(":/icons/add.png"));
    _sortButton = new QToolButton( );
    _sortButton->setIcon( addIcon );

    layout_->addWidget( _propertiesSelector, 0, 0, 1, 1,
                        Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _sortButton, 0, 1, 1, 2,
                        Qt::AlignLeft );

    connect( _sortButton, SIGNAL( pressed( )),
             this, SLOT( addedSortProperty( )));

    connect( _changeDirSignalMapper, SIGNAL( mapped( const QString& )),
             this, SLOT( changeSortPropertyDir( const QString&  )));

  }

  void SortWidget::addedSortProperty( void )
  {
    auto& sortProperties = _sortConfig.properties( );
    auto propertyLabel = _propertiesSelector->currentText( ).toStdString( );

    // If property already inserted just return
    if ( std::find_if(
           sortProperties.begin( ), sortProperties.end( ),
           [ propertyLabel ]( fires::SortConfig::TSortProperty& p )->bool
           {
             return p.label == propertyLabel;
           }) != sortProperties.end( ))
      return;

    std::cout << "<><><><> Added sorted propery "
              << _propertiesSelector->currentText( ).toStdString( )
              << std::endl;

    QLabel* propertyQLabel = new QLabel( _propertiesSelector->currentText( ));

    QIcon changeSortDirIcon(
      QString::fromUtf8( ":/icons/alphabetical_sorting_az.png"));
    QToolButton* changeSortDirButton = new QToolButton( this );;
    changeSortDirButton->setIcon( changeSortDirIcon );
    changeSortDirButton->setCheckable(true);

    QIcon removePropertyIcon( QString::fromUtf8(":/icons/close.png"));
    QToolButton* removePropertyButton = new QToolButton( this );
    removePropertyButton->setIcon( removePropertyIcon );

    auto layout = dynamic_cast< QGridLayout* >( this->layout( ));
#define LABEL_COLUMN 0
#define DIRBUTTON_COLUMN 1
#define REMOVE_COLUMN 2
    ++_numSortProperties;
    layout->addWidget( propertyQLabel, _numSortProperties,
                       LABEL_COLUMN, 1, 1, Qt::AlignLeft );
    layout->addWidget( changeSortDirButton, _numSortProperties,
                       DIRBUTTON_COLUMN, 1, 1, Qt::AlignLeft );
    layout->addWidget( removePropertyButton, _numSortProperties,
                       REMOVE_COLUMN, 1, 1, Qt::AlignLeft );

    _sortConfig.addProperty( propertyLabel,
                             fires::PropertyManager::getSorter( propertyLabel ),
                             fires::SortConfig::ASCENDING );

    _layoutRowsMap[ propertyLabel ] = _numSortProperties;
    // QSignalMapper* signalMapper = new QSignalMapper( this );
    connect( removePropertyButton, SIGNAL( pressed( )),
             _removeSignalMapper, SLOT( map( )));
    _removeSignalMapper->setMapping( removePropertyButton,
                                     QString( propertyLabel.c_str( )));
    connect( _removeSignalMapper, SIGNAL( mapped( const QString& )),
             this, SLOT( removeSortProperty( const QString&  )));

    connect( changeSortDirButton, SIGNAL( pressed( )),
             _changeDirSignalMapper, SLOT( map( )));
    _changeDirSignalMapper->setMapping( changeSortDirButton,
                                        QString( propertyLabel.c_str( )));
    // connect( _changeDirSignalMapper, SIGNAL( mapped( const QString& )),
    //          this, SLOT( changeSortPropertyDir( const QString&  )));

    std::cout << "["
              << _sortConfig.properties( ).size( ) << "]: ";
    for ( const auto& p : _sortConfig.properties( ))
    {
      std::cout << "{" << p.label << "," << (int) p.order << "}";
      assert( p.sorter );
    }
    std::cout <<  std::endl;

  }
  void SortWidget::clear( )
  {
    _propertiesSelector->clear( );
    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));
    for ( const auto& row : _layoutRowsMap )
    {
      auto idx = row.second;
      auto item = qGridLayout->itemAtPosition(
        idx, LABEL_COLUMN );
      assert( item );
      delete item->widget( );

      item = qGridLayout->itemAtPosition(
        idx, DIRBUTTON_COLUMN );
      assert( item );
      delete item->widget( );

      item = qGridLayout->itemAtPosition(
        idx, REMOVE_COLUMN );
      assert( item );
      delete item->widget( );

    }
    _layoutRowsMap.clear( );
    _numSortProperties = 0;
    _sortConfig.properties( ).clear( );
    delete _removeSignalMapper;
    _removeSignalMapper = new QSignalMapper;
    delete _changeDirSignalMapper;
    _changeDirSignalMapper = new QSignalMapper;

  }

  void SortWidget::removeSortProperty( const QString& propertyLabel_ )
  {
    std::cout << "Remove property "
              << propertyLabel_.toStdString( ) << std::endl;

    auto& sortProperties = _sortConfig.properties( );
    auto propertyLabel = propertyLabel_.toStdString( );

    fires::SortConfig::TSortProperties::iterator sortProperty = std::find_if(
      sortProperties.begin( ), sortProperties.end( ),
      [ propertyLabel ]( fires::SortConfig::TSortProperty& p )->bool
      {
        return p.label == propertyLabel;
      });
    // If property already inserted just return
    if ( sortProperty == sortProperties.end( )) return;

    auto qGridLayout = dynamic_cast< QGridLayout* >( this->layout( ));
    
    auto item = qGridLayout->itemAtPosition(
      _layoutRowsMap[ propertyLabel ], LABEL_COLUMN );
    assert( item );
    delete item->widget( );

    item = qGridLayout->itemAtPosition(
      _layoutRowsMap[ propertyLabel ], DIRBUTTON_COLUMN );
    assert( item );
    delete item->widget( );

    item = qGridLayout->itemAtPosition(
      _layoutRowsMap[ propertyLabel ], REMOVE_COLUMN );
    assert( item );
    delete item->widget( );

    std::cout << "["
              << _sortConfig.properties( ).size( ) << "]: ";
    for ( const auto& p : _sortConfig.properties( ))
    {
      std::cout << "{" << p.label << "," << (int) p.order << "}";
      assert( p.sorter );
    }
    std::cout <<  std::endl;

  }

  void SortWidget::changeSortPropertyDir( const QString& propertyLabel_ )
  {
    auto& sortProperties = _sortConfig.properties( );
    auto propertyLabel = propertyLabel_.toStdString( );

    fires::SortConfig::TSortProperties::iterator sortProperty = std::find_if(
      sortProperties.begin( ), sortProperties.end( ),
      [ propertyLabel ]( fires::SortConfig::TSortProperty& p )->bool
      {
        return p.label == propertyLabel;
      });
    // If property already inserted just return
    assert( sortProperty != sortProperties.end( ));

    sortProperty->order =
      ( sortProperty->order == fires::SortConfig::ASCENDING ) ?
      sortProperty->order =fires::SortConfig::DESCENDING :
      sortProperty->order = fires::SortConfig::ASCENDING;

    std::cout << "Change sort dir "
              << propertyLabel << " to "
              << (int) sortProperty->order << std::endl;

    std::cout << "["
              << _sortConfig.properties( ).size( ) << "]: ";
    for ( const auto& p : _sortConfig.properties( ))
    {
      std::cout << "{" << p.label << "," << (int) p.order << "}";
      assert( p.sorter );
    }
    std::cout <<  std::endl;

  }

  SortWidget::~SortWidget( void )
  {
    delete _propertiesSelector;
    delete _sortButton;
    delete _removeSignalMapper;
    delete _changeDirSignalMapper;
  }

}
