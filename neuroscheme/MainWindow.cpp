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
#include <nslib/version.h>
#include <nslib/DataManager.h>
#include <nslib/DomainManager.h>
#include <nslib/Config.h>
#include <nslib/PaneManager.h>
#include <nslib/SelectionManager.h>
#include <nslib/InteractionManager.h>
#include <nslib/Log.h>
#include <nslib/layouts/CircularLayout.h>
#include <nslib/layouts/GridLayout.h>
#include <nslib/layouts/CameraBasedLayout.h>
#include <nslib/layouts/LayoutManager.h>
#include <nslib/layouts/ScatterPlotLayout.h>
#include <nslib/EntityEditWidget.h>
#include <cortex/Domain.h>
#include <congen/Domain.h>

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

// #ifdef NSOL_USE_BRION
//   _ui->actionLoadBlueConfig->setEnabled( true );
// #endif

// #ifdef NSOL_USE_QT5CORE
//   _ui->actionOpenXmlScene->setEnabled( true );
// #endif

  // Connect save dialog
  connect( _ui->actionSave, SIGNAL( triggered( )), this, SLOT( saveScene( )));

  // Connect about dialog
  connect( _ui->actionAbout, SIGNAL( triggered( )), this, SLOT( aboutDialog( )));

  connect( _ui->actionShowConnectivity, SIGNAL( triggered( )),
           this, SLOT( toggleShowConnectivity( )));

  QActionGroup* splitTypeGroup = new QActionGroup( this );
  _ui->actionSplitHorizontally->setCheckable( true );
  _ui->actionSplitHorizontally->setActionGroup( splitTypeGroup );
  _ui->actionSplitVertically->setCheckable( true );
  _ui->actionSplitVertically->setActionGroup( splitTypeGroup );
  _ui->actionSplitVertically->setChecked( true );
  connect( splitTypeGroup, SIGNAL( triggered( QAction* )),
           this, SLOT( paneDivisionChanged(  )));

  connect( _ui->actionDuplicatePane, SIGNAL( triggered( )),
           this, SLOT( duplicateActivePane( )));
  connect( _ui->actionKillPane, SIGNAL( triggered( )),
           this, SLOT( killActivePane( )));

  connect( _ui->actionHome, SIGNAL( triggered( )),
           this, SLOT( home( )));

  QSplitter* widget = new QSplitter( this );
  widget->setSizePolicy( QSizePolicy::Expanding,
                         QSizePolicy::Expanding );
  this->setCentralWidget( widget );

  // Active domain
  auto domainArg = nslib::Config::isArgumentDefined( { "--domain", "-d"} );
  if ( !domainArg.empty( ) &&
       nslib::Config::inputArgs( )[ domainArg ].size( ) == 1 )
  {
    if ( nslib::Config::inputArgs( )[ domainArg ][0] == "cortex" )
    {
      nslib::Domain* domain = new nslib::cortex::Domain;
      nslib::DomainManager::setActiveDomain( domain );
      if ( !domain->dataLoader( )->loadData( nslib::Config::inputArgs( )))
        exit( -1 );
    }
    else if ( nslib::Config::inputArgs( )[ domainArg ][0] == "congen" )
    {
      nslib::Domain* domain = new nslib::congen::Domain;
      nslib::DomainManager::setActiveDomain( domain );
      if ( !domain->dataLoader( )->loadData( nslib::Config::inputArgs( )))
        exit( -1 );

    }
  }
  nslib::PaneManager::splitter( widget );

  // First pane
  nslib::Log::log( NS_LOG_HEADER + "Creating first pane",
                         nslib::LOG_LEVEL_VERBOSE );
  auto canvas = nslib::PaneManager::newPane( );
  canvas->activeLayoutIndex( 0 );
  canvas->setSizePolicy( QSizePolicy::Expanding,
                         QSizePolicy::Expanding );
  canvas->addLayout( new nslib::GridLayout( ));
  canvas->addLayout( new nslib::CameraBasedLayout( ));
  canvas->addLayout( new nslib::ScatterPlotLayout( ));
  canvas->addLayout( new nslib::CircularLayout( ));

  canvas->displayEntities(
    nslib::DataManager::rootEntities( ), false, true );
  nslib::PaneManager::panes( ).insert( canvas );

  // Layouts dock
  {
    _layoutsDock = new QDockWidget( );
    this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                         _layoutsDock, Qt::Vertical);
    _layoutsDock->setMinimumSize( 300, 300 );
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

    QWidget* dockWidget = new QWidget( );
    QGridLayout* layoutsConfigLayout = new QGridLayout( dockWidget );
    layoutsConfigLayout->setAlignment( Qt::AlignTop );
    nslib::PaneManager::layout( layoutsConfigLayout );
    nslib::PaneManager::activePane( canvas );

    // QWidget* dockWidget = new QWidget( );
    // dockWidget->setLayout( layoutsConfigLayout );
    _layoutsDock->setWidget( dockWidget );
  }

  // Stored selections dock
  {
    _storedSelections.dock = new QDockWidget( );
    _storedSelections.dock->setWindowTitle( QString( "Stored selections" ));
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


    QWidget* dockWidget = new QWidget( );
    QGridLayout* selectionLayout = new QGridLayout( dockWidget );

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

  {
    _entityEditDock = new QDockWidget;
    nslib::EntityEditWidget::parentDock( _entityEditDock );
    _entityEditDock->setWindowTitle( QString( "Entity Inspector" ));
    _entityEditDock->setSizePolicy( QSizePolicy::MinimumExpanding,
                                    QSizePolicy::MinimumExpanding );

    _entityEditDock->setFeatures( QDockWidget::DockWidgetClosable |
                                  QDockWidget::DockWidgetMovable |
                                  QDockWidget::DockWidgetFloatable );

    this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                         _entityEditDock,
                         Qt::Vertical );
    _entityEditDock->close( );
  }
  
  resizeEvent( 0 );
}

MainWindow::~MainWindow( void )
{
  delete _ui;
  for ( const auto& _canvas :  nslib::PaneManager::panes( ))
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
  if ( nslib::SelectionManager::activeSelectionSize( ) == 0 )
  {
    nslib::Log::log( NS_LOG_HEADER +
                           "Tried to store an empty selection ",
                           nslib::LOG_LEVEL_VERBOSE );
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
    nslib::SelectionManager::existsStoredSelection( label.toStdString( ));

  nslib::SelectionManager::storeActiveSelection( label.toStdString( ));
  unsigned int numberOfSelectedEntities =
    nslib::SelectionManager::storedSelectionSize( label.toStdString( ));

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
      nslib::Log::log( NS_LOG_HEADER +
                             "Stored selection row not found ",
                             nslib::LOG_LEVEL_ERROR );
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

    nslib::SelectionManager::restoreStoredSelection(
      label.toStdString( ));
    resizeEvent( nullptr );
    for ( const auto& pane : nslib::PaneManager( ).panes( ))
    {
      pane->resizeEvent( 0 );
    }
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

    if ( !nslib::SelectionManager::deleteStoredSelection(
           label.toStdString( )))
    {
      nslib::Log::log( NS_LOG_HEADER +
                             "Tried to delete a non existing saved selection ",
                             nslib::LOG_LEVEL_WARNING );

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


void MainWindow::paneDivisionChanged( void )
{
  nslib::PaneManager::paneDivision(
    _ui->actionSplitVertically->isChecked( ) ?
    nslib::PaneManager::VERTICAL :
    nslib::PaneManager::HORIZONTAL );
}


void MainWindow::killActivePane( void )
{
  nslib::PaneManager::killActivePane( );
}

void MainWindow::duplicateActivePane( void )
{
  nslib::PaneManager::newPaneFromActivePane( );

}

void MainWindow::home( void )
{
    nslib::PaneManager::activePane( )->displayEntities(
      nslib::DataManager::rootEntities( ), false, true );
}

void MainWindow::aboutDialog( void )
{
  QMessageBox::about(
    this, tr("About neuroscheme"),
    tr("<p><BIG><b>NeuroScheme</b></BIG><br><br>") +
    tr( "version " ) +
    tr( nslib::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( nslib::Version::getRevision( )).c_str( )) +
    tr( ")" ) +
    tr ("<br><br>GMRV - Universidad Rey Juan Carlos<br><br>"
        "<a href=www.gmrv.es>www.gmrv.es</a><br>"
        "<a href='mailto:gmrv@gmrv.es'>gmrv@gmrv.es</a><br><br>"
        "<br>(c) 2015. Universidad Rey Juan Carlos<br><br>"
        "<img src=':/icons/logoGMRV.png' > &nbsp; &nbsp;"
        "<img src=':/icons/logoURJC.png' >"
        "</p>"
        ""));
}

void MainWindow::toggleShowConnectivity( void )
{

  nslib::Config::showConnectivity( _ui->actionShowConnectivity->isChecked( ));
  // WAR to force repaint
  resizeEvent( 0 );
  for ( auto canvas : nslib::PaneManager::panes( ))
  {
    canvas->resizeEvent( nullptr );
    // canvas->layouts( ).getLayout(
    //   canvas->activeLayoutIndex( ))->refresh( false );
  }
}


void MainWindow::saveScene( void )
{
  //Depending on the selected domain, a certain "saver" could be available
  //At this moment, only NeuroML
  emit nslib::congen::DataSaver::saveXmlScene( this );
}


