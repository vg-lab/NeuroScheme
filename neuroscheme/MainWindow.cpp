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
#include "MainWindow.h"
#include "DataManager.h"
#include "DomainManager.h"
#include "GridLayout.h"
#include "CameraBasedLayout.h"
#include "LayoutManager.h"
#include "PaneManager.h"
#include "SelectionManager.h"
#include "Log.h"

#include <QGridLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QLineEdit>
#include <QDateTime>
#include <QLabel>

MainWindow::MainWindow( QWidget* parent_ )
  : QMainWindow( parent_ )
  , _ui( new Ui::MainWindow )
{
  _ui->setupUi( this );
  // This is a WAR to show the menu in some
  // systems that does not appear
  //_ui->menubar->setNativeMenuBar( false );

#ifdef NSOL_USE_BRION
  _ui->actionLoadBlueConfig->setEnabled( true );
#endif

#ifdef NSOL_USE_QT5CORE
  _ui->actionOpenXmlScene->setEnabled( true );
#endif

  // Set up main central layout for pane placing
  auto mainGridLayout = new QGridLayout;
  mainGridLayout->setContentsMargins( 0, 0, 0, 0 );
  mainGridLayout->setSpacing( 0 );
  QWidget* widget = new QWidget( this );
  widget->setLayout( mainGridLayout );
  this->setCentralWidget( widget );

  // Active domain
  neuroscheme::DomainManager::setActiveDomain(
    new neuroscheme::cortex::Domain );

  neuroscheme::DataManager::loadData( );
  neuroscheme::PaneManager::mainLayout( mainGridLayout );


  // First pane
  auto canvas = neuroscheme::PaneManager::newPane( );
  canvas->activeLayoutIndex( 0 );
  canvas->layouts( ).addLayout( new neuroscheme::GridLayout( ));
  canvas->layouts( ).addLayout( new neuroscheme::CameraBasedLayout( ));
  canvas->displayReps( neuroscheme::DataManager::representations( ), false );
  neuroscheme::PaneManager::panes( ).insert( canvas );
  neuroscheme::PaneManager::activePane( canvas );

  // Common sizes for docks
  unsigned int tabWidth = 300;
  unsigned int tabMargin = 25;
  unsigned int halfSceneHeight =
    ( this->centralWidget( )->height( ) - tabMargin) / 3;


  // Layouts dock
  {
    _layoutsDock = new QDockWidget( );
    this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                         _layoutsDock, Qt::Vertical);
    _layoutsDock->setMinimumSize( tabWidth,
                                  halfSceneHeight - tabMargin / 3);
    _layoutsDock->setSizePolicy( QSizePolicy::MinimumExpanding,
                                 QSizePolicy::Expanding );

    _layoutsDock->setFeatures( QDockWidget::DockWidgetClosable |
                               QDockWidget::DockWidgetMovable |
                               QDockWidget::DockWidgetFloatable );

    _layoutsDock->setWindowTitle( QString( "Layouts" ));
    _layoutsDock->close( );
    _ui->actionLayouts->setChecked( false );
    connect( _layoutsDock->toggleViewAction( ), SIGNAL( toggled( bool )),
             _ui->actionLayouts, SLOT( setChecked( bool )));
    connect( _ui->actionLayouts, SIGNAL( triggered( )),
             this, SLOT( updateLayoutsDock( )));

    QGridLayout* layoutsConfigLayout = new QGridLayout( );
    layoutsConfigLayout->setAlignment( Qt::AlignTop );
    _layoutsDock->setLayout( layoutsConfigLayout );
    neuroscheme::PaneManager::layout( layoutsConfigLayout );
    //layoutsConfigLayout->addWidget( new QLabel( "hola" ), 0, 0 );

    QWidget* dockWidget = new QWidget( );
    dockWidget->setLayout( layoutsConfigLayout );
    _layoutsDock->setWidget( dockWidget );
  }

  // Stored selections dock
  {
    _storedSelections.dock = new QDockWidget();
    _storedSelections.dock->setWindowTitle( QString( "Stored selections" ));
    _storedSelections.dock->setMinimumSize( tabWidth,
                                            halfSceneHeight - tabMargin / 3 );
    _storedSelections.dock->resize( tabWidth, halfSceneHeight - tabMargin / 3 );
    _storedSelections.dock->setSizePolicy( QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding );

    _storedSelections.dock->setFeatures( QDockWidget::DockWidgetClosable |
                                         QDockWidget::DockWidgetMovable |
                                         QDockWidget::DockWidgetFloatable);
    this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                         _storedSelections.dock,
                         Qt::Vertical );

    _storedSelections.dock->close( );
    _ui->actionStoredSelections->setChecked( false );
    connect( _storedSelections.dock->toggleViewAction( ),
             SIGNAL( toggled( bool )),
             _ui->actionStoredSelections,
             SLOT( setChecked( bool )));
    connect( _ui->actionStoredSelections, SIGNAL( triggered( )),
             this, SLOT( updateStoredSelectionsDock( )));


    QGridLayout* selectionLayout = new QGridLayout( );
    QWidget* dockWidget = new QWidget( );

    dockWidget->setLayout( selectionLayout );

    _storedSelections.counter = 0;
    _storedSelections.table = new QTableWidget( );
    _storedSelections.table->setColumnCount( TTableColumns::COLUMN_MAX_COLUMS );

    QStringList tableHeader;
    for ( unsigned int i = 0; i < TTableColumns::COLUMN_MAX_COLUMS; i++ )
    {
      tableHeader << _tableColumnToString( TTableColumns( i ));
    }

    _storedSelections.table->setHorizontalHeaderLabels( tableHeader );
    _storedSelections.table->horizontalHeader( )->setSectionResizeMode(
      QHeaderView::ResizeToContents );
    _storedSelections.table->horizontalHeader( )->setSectionsClickable( true );
    _storedSelections.table->verticalHeader( )->setVisible( false );

    connect( _storedSelections.table->horizontalHeader( ),
             SIGNAL( sectionClicked( int )), this,
             SLOT( sortStoredSelectionsTable( int )));

    _storedSelections.table->setEditTriggers(
      QAbstractItemView::NoEditTriggers );
    _storedSelections.table->setSelectionBehavior(
      QAbstractItemView::SelectRows );
    _storedSelections.table->setSelectionMode(
      QAbstractItemView::SingleSelection );

    selectionLayout->addWidget( _storedSelections.table, 0, 0, 1, 4 );

    QPushButton* saveButton = new QPushButton( "Save selection" );
    saveButton->setToolTip( QString( "Saves the current selection" ));
    selectionLayout->addWidget( saveButton, 1, 0, 1, 2 );

    connect( saveButton, SIGNAL(clicked()), this, SLOT( storeSelection( ) ));

    QPushButton* restoreButton = new QPushButton( " Restore selection" );
    restoreButton->setToolTip( QString ( "Restores the selection to current" ));
    selectionLayout->addWidget( restoreButton, 1, 2, 1, 2);

    connect( restoreButton, SIGNAL( clicked( )),
             this, SLOT( restoreSelection( )));

    QPushButton* deleteButton = new QPushButton( "Delete selected" );
    deleteButton->setToolTip( QString( "Deletes the selected selection"));
    selectionLayout->addWidget( deleteButton, 2, 1, 1, 2 );

    connect( deleteButton, SIGNAL( clicked( )), this,
             SLOT( deleteStoredSelection( )));

    _storedSelections.dock->setWidget( dockWidget );
  } // END selection dock


}

void MainWindow::resizeEvent( QResizeEvent* /* event_ */ )
{
  //std::cout << "MainWindow::resizeEvent" << std::endl;
  for ( const auto& _canvas : neuroscheme::PaneManager::panes( ))
  {
    const QSize viewerSize = _canvas->view( ).size( );
    const QRectF rectf = QRectF( - viewerSize.width( ) / 2,
                                 - viewerSize.height( ) / 2,
                                 viewerSize.width( ) -2,
                                 viewerSize.height( ) -2);

    QTransform transf = _canvas->view( ).transform( );
    _canvas->view( ).fitInView( rectf, Qt::KeepAspectRatio );
    _canvas->view( ).setSceneRect( rectf );
    _canvas->view( ).setTransform( transf );

    // std::cout << _canvas->view( ).width( ) << " x "
    //           << _canvas->view( ).height( ) << std::endl;

    _canvas->layouts( ).getLayout( _canvas->activeLayoutIndex( ))->refresh(
      &( _canvas->scene( )));

    // neuroscheme::LayoutManager::setScene( &_canvas->scene( ));
    // neuroscheme::LayoutManager::update( );
  }

}



MainWindow::~MainWindow( void )
{
  delete _ui;
  for ( const auto& _canvas :  neuroscheme::PaneManager::panes( ))
    delete _canvas;
}

QString MainWindow::_tableColumnToString( TTableColumns column )
{
  switch ( column)
  {
    case TTableColumns::COLUMN_LABEL:
      return QString( "Label" );
    case TTableColumns::COLUMN_COUNT:
      return QString( "Count" );
    case TTableColumns::COLUMN_DATETIME:
      return QString( "Date" );
    default:
      return QString();
  }
}

void MainWindow::updateStoredSelectionsDock( void )
{
  if ( _ui->actionStoredSelections->isChecked( ))
    _storedSelections.dock->show( );
  else
    _storedSelections.dock->close( );

  resizeEvent( nullptr );
}

void MainWindow::updateLayoutsDock( void )
{
  if ( _ui->actionLayouts->isChecked( ))
    _layoutsDock->show( );
  else
    _layoutsDock->close( );

  resizeEvent( nullptr );
}

void MainWindow::storeSelection( void )
{

  // In case selection is empty return
  if ( neuroscheme::SelectionManager::activeSelectionSize( ) == 0 )
  {
    neuroscheme::Log::log( NS_LOG_HEADER +
                           "Tried to store an empty selection ",
                           neuroscheme::LOG_LEVEL_VERBOSE );
    return;
  }

  QString automaticLabel = tr( "Selection%1" ).arg( _storedSelections.counter );
  bool ok;

  QString label = QInputDialog::getText(
    this, tr( "Please select a name" ),
    tr( "Name:" ), QLineEdit::Normal,
    automaticLabel, &ok );

  if ( !ok || label.isEmpty( ))
    return;

  bool updateExistingRow =
    neuroscheme::SelectionManager::existsStoredSelection( label.toStdString( ));

  neuroscheme::SelectionManager::storeActiveSelection( label.toStdString( ));
  unsigned int numberOfSelectedEntities =
    neuroscheme::SelectionManager::storedSelectionSize( label.toStdString( ));

  // Update counter just in case selection was saved and using the proposed name
  if ( label == automaticLabel )
    _storedSelections.counter++;

  QString date( QDateTime::currentDateTime( ).toString( "dd.MM.yy hh:mm:ss" ));
  QString selectedEntities ( tr( "%1" ).arg( numberOfSelectedEntities ));

  if ( updateExistingRow )
  {
    //std::unordered_map< std::string, QTableWidgetItem*>::iterator it;
    auto it = _storedSelections.tableWidgets.find( label.toStdString( ));
    if ( it == _storedSelections.tableWidgets.end( ))
    {
      neuroscheme::Log::log( NS_LOG_HEADER +
                             "Stored selection row not found ",
                             neuroscheme::LOG_LEVEL_ERROR );
      return;
    }

    unsigned int row = _storedSelections.table->row( it->second );
    _storedSelections.table->item(
        row, TTableColumns::COLUMN_COUNT )->setText( selectedEntities );

    _storedSelections.table->item(
        row, TTableColumns::COLUMN_DATETIME )->setText( date );

  }
  else
  {
    unsigned int row = _storedSelections.table->rowCount( );
    _storedSelections.table->insertRow( row );

    QTableWidgetItem* labelItem = new QTableWidgetItem( label );

    _storedSelections.table->setItem( row, TTableColumns::COLUMN_LABEL,
                                      labelItem );

    _storedSelections.tableWidgets.insert(
      make_pair( label.toStdString( ), labelItem ));

    _storedSelections.table->setItem( row, TTableColumns::COLUMN_COUNT,
                                      new QTableWidgetItem( selectedEntities ));

    _storedSelections.table->setItem( row, TTableColumns::COLUMN_DATETIME,
                                      new QTableWidgetItem( date ));
  }
// #ifdef NEUROSCHEME_USE_ZEROEQ
//   updateCellSetOperationSelections( );
// #endif
}


void MainWindow::restoreSelection( void )
{
  if ( _storedSelections.table->selectedItems( ).size( ) > 0 )
  {

    QString label;
    QTableWidgetItem* firstItem =
      _storedSelections.table->selectedItems( ).at( 0 );
    label = firstItem->text( );

    neuroscheme::SelectionManager::restoreStoredSelection(
      label.toStdString( ));
    resizeEvent( nullptr );

  }
}

void MainWindow::deleteStoredSelection( void )
{
  if ( _storedSelections.table->selectedItems( ).size( ) > 0 )
  {
    QString label;
    unsigned int row;
    QTableWidgetItem* firstItem =
      _storedSelections.table->selectedItems( ).at( 0 );
    label = firstItem->text( );

    // Delete selection from storage
    //deleteSelectedSelection( label );

    // Remove selection from table
    row = _storedSelections.table->row( firstItem );
    _storedSelections.table->removeRow( row );
    _storedSelections.tableWidgets.erase( label.toStdString( ));

    if ( !neuroscheme::SelectionManager::deleteStoredSelection(
           label.toStdString( )))
    {
      neuroscheme::Log::log( NS_LOG_HEADER +
                             "Tried to delete a non existing saved selection ",
                             neuroscheme::LOG_LEVEL_WARNING );

    }
// #ifdef NEUROSCHEME_USE_ZEROEQ
//     updateCellSetOperationSelections( );
// #endif
  }
}


void MainWindow::sortStoredSelectionsTable( int column )
{
  _storedSelections.table->sortByColumn( column );
}

