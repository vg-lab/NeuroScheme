/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#include "layouts/Layout.h"
#include "SortWidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <algorithm>

namespace nslib
{
  SortWidget::SortWidget( Layout* parentLayout_, QWidget* parent_ )
    : QFrame( parent_ )
    , _parentLayout( parentLayout_ )
    , _numSortProperties( 0 )
  {
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    this->setLayout( layout_ );

    _propertiesSelector = new QComboBox( );
    _propertiesSelector->setSizePolicy( QSizePolicy::Expanding,
                                        QSizePolicy::Expanding );
    _propertiesSelector->setMinimumSize( 190, 20 );

    QIcon addIcon( QString::fromUtf8(":/icons/add.svg"));
    _sortButton = new QToolButton( );
    _sortButton->setIcon( addIcon );

    layout_->addWidget( _propertiesSelector, 0, 0, 1, 1,
      Qt::AlignCenter | Qt::AlignLeft );
    layout_->addWidget( _sortButton, 0, 1, 1, 2,
      Qt::AlignLeft );

    connect( _sortButton, SIGNAL( pressed( )),
             this, SLOT( addedSortProperty( )));
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

    auto propertyQLabel = new QLabel( _propertiesSelector->currentText( ));

    QIcon changeSortDirIcon(":/icons/alphabetical_sorting_az.svg");
    QToolButton* changeSortDirButton = new QToolButton( this );;
    changeSortDirButton->setIcon( changeSortDirIcon );
    changeSortDirButton->setCheckable(true);

    QIcon removePropertyIcon(":/icons/close.svg");
    QToolButton* removePropertyButton = new QToolButton( this );
    removePropertyButton->setIcon( removePropertyIcon );

    auto layout_ = dynamic_cast< QGridLayout* >( this->layout( ));
#define LABEL_COLUMN 0
#define DIRBUTTON_COLUMN 1
#define REMOVE_COLUMN 2
    ++_numSortProperties;
    layout_->addWidget( propertyQLabel, _numSortProperties,
                       LABEL_COLUMN, 1, 1, Qt::AlignLeft );
    layout_->addWidget( changeSortDirButton, _numSortProperties,
                       DIRBUTTON_COLUMN, 1, 1, Qt::AlignLeft );
    layout_->addWidget( removePropertyButton, _numSortProperties,
                       REMOVE_COLUMN, 1, 1, Qt::AlignLeft );

    _sortConfig.addProperty( propertyLabel,
                             fires::PropertyManager::getSorter( propertyLabel ),
                             fires::SortConfig::ASCENDING );

    _layoutRowsMap[ propertyLabel ] = _numSortProperties;

    if ( _changeDirSignalMappers.find( propertyLabel ) !=
         _changeDirSignalMappers.end( ))
      delete _changeDirSignalMappers[ propertyLabel ];
    _changeDirSignalMappers[ propertyLabel ] = new QSignalMapper;

    if ( _removeSignalMappers.find( propertyLabel ) !=
         _removeSignalMappers.end( ))
      delete _removeSignalMappers[ propertyLabel ];
    _removeSignalMappers[ propertyLabel ] = new QSignalMapper;

    connect( removePropertyButton, SIGNAL( pressed( )),
             _removeSignalMappers[ propertyLabel ], SLOT( map( )));
    _removeSignalMappers[ propertyLabel ]->setMapping(
      removePropertyButton, QString( propertyLabel.c_str( )));
    connect( _removeSignalMappers[ propertyLabel ],
             SIGNAL( mapped( const QString& )),
             this, SLOT( removeSortProperty( const QString&  )));

    connect( changeSortDirButton, SIGNAL( clicked( )),
             _changeDirSignalMappers[ propertyLabel ], SLOT( map( )));
    _changeDirSignalMappers[ propertyLabel ]->setMapping( changeSortDirButton,
                                        QString( propertyLabel.c_str( )));
    connect( _changeDirSignalMappers[ propertyLabel ],
             SIGNAL( mapped( const QString& )),
             this, SLOT( changeSortPropertyDir( const QString&  )));

    _parentLayout->refresh( true );
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
    _changeDirSignalMappers.clear( );
    _removeSignalMappers.clear( );
    _numSortProperties = 0;
    _sortConfig.properties( ).clear( );
  }

  void SortWidget::removeSortProperty( const QString& propertyLabel_ )
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
    if ( sortProperty == sortProperties.end( )) return;

    sortProperties.erase( sortProperty );

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

    _layoutRowsMap.erase( propertyLabel );
    _removeSignalMappers.erase( propertyLabel );
    _changeDirSignalMappers.erase( propertyLabel );

    _parentLayout->refresh( true );
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

    _parentLayout->refresh( true );
  }

  SortWidget::~SortWidget( void )
  {
    delete _propertiesSelector;
    delete _sortButton;
  }

}
