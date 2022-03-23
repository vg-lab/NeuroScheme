/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *          Jose M. Espadero <josemiguel.espadero@urjc.es>
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
#include <nslib/nslib.h>
#include <cortex/Domain.h>
#include <congen/Domain.h>
#include <scoop/version.h>

#ifdef NEUROSCHEME_USE_GMRVLEX
#include <gmrvlex/version.h>
#endif
#ifdef NEUROSCHEME_USE_DEFLECT
#include <deflect/version.h>
#endif
#ifdef NEUROSCHEME_USE_NSOL
#include <nsol/version.h>
#endif
#ifdef NEUROSCHEME_USE_ZEROEQ
#include <zeroeq/version.h>
#endif

#include <acuterecorder/acuterecorder.h>

#include <QGridLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QLineEdit>
#include <QDateTime>
#include <QLabel>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow( QWidget* parent_, bool zeroEQ )
  : QMainWindow( parent_ )
  , _recorder( nullptr )
  , minDockSizeX(305u)
  , minDockSizeY(25u)
  , _ui( new Ui::MainWindow )
{
#ifndef NEUROSCHEME_USE_ZEROEQ
  (void) zeroEQ;
#endif

  _ui->setupUi( this );

  connect( _ui->actionJSONImporter, SIGNAL( triggered( )),
           this, SLOT( importFromJSON( )));

  connect( _ui->actionCleanScene, SIGNAL( triggered( )),
           this, SLOT( cleanScene( )));

  connect( _ui->actionJSONExporter, SIGNAL( triggered( )),
           this, SLOT( exportToJSON( )));

  connect( _ui->actionQuit, SIGNAL( triggered( )),
           this, SLOT( close()));

  connect( _ui->actionRecorder , SIGNAL( triggered( void )) ,
           this ,SLOT( openRecorder( void )));

  // ZeroEQ related actions ///////////////////////////////////////
#ifdef NEUROSCHEME_USE_ZEROEQ
  _ui->actionToggleZeroEQ->setEnabled( true );
  _ui->actionToggleZeroEQ->setChecked( zeroEQ );
  connect( _ui->actionToggleZeroEQ, SIGNAL( triggered( )),
           this, SLOT( toggleZeroEQ( )));

  _ui->actionAutoPublishSelection->setEnabled( true );
  _ui->actionAutoPublishSelection->setChecked( true );
  nslib::Config::autoPublishSelection( true );
  connect( _ui->actionAutoPublishSelection, SIGNAL( triggered( )),
           this, SLOT( actionPublishSelectionToggle( )));

  connect( _ui->actionPublishSelection, SIGNAL( triggered( )),
           this, SLOT( actionPublishSelection( )));
#else
  _ui->actionPublishSelection->setEnabled( false );
  _ui->actionAutoPublishSelection->setEnabled( false );
#endif

#ifdef NEUROSCHEME_USE_GMRVLEX
  _ui->actionAutoFocusOnSelection->setEnabled( true );
  _ui->actionAutoFocusOnSelection->setChecked( false );
  nslib::Config::autoPublishFocusOnSelection( false );
  connect( _ui->actionAutoFocusOnSelection, SIGNAL( triggered( )),
           this, SLOT( actionPublishFocusOnSelectionToggle( )));

  _ui->actionAutoFocusOnDisplayed->setEnabled( true );
  _ui->actionAutoFocusOnDisplayed->setChecked( false );
  nslib::Config::autoPublishFocusOnDisplayed( false );
  connect( _ui->actionAutoFocusOnDisplayed, SIGNAL( triggered( )),
           this, SLOT( actionPublishFocusOnDisplayedToggle( )));

  connect( _ui->actionFocusOnSelection, SIGNAL( triggered( )),
           this, SLOT( actionPublishFocusOnSelection( )));
  connect( _ui->actionFocusOnDisplayed, SIGNAL( triggered( )),
           this, SLOT( actionPublishFocusOnDisplayed( )));

#else
  _ui->actionFocusOnDisplayed->setEnabled( false );
  _ui->actionAutoFocusOnDisplayed->setEnabled( false );
  _ui->actionFocusOnSelection->setEnabled( false );
  _ui->actionAutoFocusOnSelection->setEnabled( false );
#endif
  ////////////////////////////////////////////////////////////////

  // // Connect save dialog
  // connect( _ui->actionSave, SIGNAL( triggered( )), this, SLOT( saveScene( )));

  connect( _ui->actionAbout, SIGNAL( triggered( )), this, SLOT( aboutDialog( )));

  connect( _ui->actionShowConnectivity, SIGNAL( triggered( )),
    this, SLOT( toggleShowConnectivity( )));

  connect( _ui->actionShowNoHierarchyEntities, SIGNAL( triggered( )),
    this, SLOT( toggleShowNoHierarchyEntities( )));

  connect( _ui->actionShowEntitiesName, SIGNAL( triggered( )),
    this, SLOT( toggleShowEntitiesName( )));

  connect( _ui->actionSplitHorizontally, SIGNAL( triggered( )),
    this, SLOT( duplicateActivePane( )));

  connect( _ui->actionSplitVertically, SIGNAL( triggered( )),
    this, SLOT( duplicateActivePane( )));

  connect( _ui->actionKillPane, SIGNAL( triggered( )),
    this, SLOT( killActivePane( )));

  connect( _ui->actionHome, SIGNAL( triggered( )),
    this, SLOT( home( )));

  _ui->actionShowNoHierarchyEntities->setChecked( true );
  _ui->actionShowEntitiesName->setChecked( true );
  _ui->actionShowConnectivity->setChecked( true );
  nslib::Config::showNoHierarchyEntities( true );
  nslib::Config::showEntitiesName( true );
  nslib::Config::showConnectivity( true );

  auto widget = new QSplitter( this );
  widget->setSizePolicy( QSizePolicy::Expanding,
                         QSizePolicy::Expanding );
  this->setCentralWidget( widget );

  nslib::InteractionManager::statusBar( statusBar( ));

  nslib::PaneManager::splitter( widget );

  this->setWindowTitle( "NeuroScheme" );
  this->selectDomain( );

  // First pane
  nslib::Loggers::get( )->log( "Creating first pane",
    nslib::LOG_LEVEL_VERBOSE );
  auto canvas = nslib::PaneManager::newPane( );

  canvas->displayEntities(
    nslib::DataManager::rootEntities( ), false, true );

  auto fileJSON = nslib::Config::isArgumentDefined( { "--json" } );
  if ( !fileJSON.empty( ))
  {
    auto filePath = nslib::Config::inputArgs( )[ fileJSON ][0] ;
    std::ifstream inputfile( filePath );
    nslib::DomainManager::getActiveDomain( )->importJSON( inputfile );
  }

  auto createDock = [=](QDockWidget * &d, const QString title)
  {
    d = new QDockWidget;
    d->setMinimumSize( minDockSizeX, minDockSizeY );
    d->setWindowTitle(title);
    d->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    d->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget( Qt::RightDockWidgetArea, d, Qt::Vertical );

    d->close( );
  };

  createDock(_layoutsDock, "Layouts");
  createDock(_storedSelections.dock, "Stored Selections");
  createDock(_entityEditDock, "Entity Inspector");
  createDock(_connectionEditDock, "Connection Inspector");
  createDock(_connectionListDock, "Entity Connections List");

  // assign docks
  nslib::EntityEditWidget::parentDock(_entityEditDock);
  nslib::ConnectionRelationshipEditWidget::parentDock(_connectionEditDock);
  nslib::EntityConnectionListWidget::parentDock(_connectionListDock);

  // Layouts dock configuration
  {
    _ui->actionLayouts->setChecked( false );

    connect( _layoutsDock->toggleViewAction( ), SIGNAL( toggled( bool )),
      _ui->actionLayouts, SLOT( setChecked( bool )));
    connect( _ui->actionLayouts, SIGNAL( triggered( )),
      this, SLOT( updateLayoutsDock( )));

    auto dockWidget = new QWidget( );
    QGridLayout* layoutsConfigLayout = new QGridLayout( dockWidget );
    layoutsConfigLayout->setAlignment( Qt::AlignTop );
    nslib::PaneManager::layout( layoutsConfigLayout );
    nslib::PaneManager::activePane( canvas );

    _layoutsDock->setWidget( dockWidget );
  }

  // Stored selections dock config
  {
    _ui->actionStoredSelections->setChecked( false );
    connect( _storedSelections.dock->toggleViewAction( ),
      SIGNAL( toggled( bool )), _ui->actionStoredSelections,
      SLOT( setChecked( bool )));
    connect( _ui->actionStoredSelections, SIGNAL( triggered( )),
      this, SLOT( updateStoredSelectionsDock( )));

    auto dockWidget = new QWidget( );
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
    _storedSelections.table->horizontalHeader( )->setSectionResizeMode(QHeaderView::ResizeToContents );
    _storedSelections.table->horizontalHeader( )->setSectionsClickable( true );
    _storedSelections.table->verticalHeader( )->setVisible( false );

    connect(_storedSelections.table->horizontalHeader( ), SIGNAL( sectionClicked( int )),
        this, SLOT( sortStoredSelectionsTable( int )));

    _storedSelections.table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _storedSelections.table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _storedSelections.table->setSelectionMode(QAbstractItemView::SingleSelection );

    selectionLayout->addWidget( _storedSelections.table, 0, 0, 1, 2 );

    auto saveButton = new QPushButton( "Save selection" );
    saveButton->setToolTip( QString( "Saves the current selection" ));
    selectionLayout->addWidget( saveButton, 1, 0 );

    connect( saveButton, SIGNAL(clicked()), this, SLOT( storeSelection( ) ));

    auto restoreButton = new QPushButton( " Restore selection" );
    restoreButton->setToolTip( QString ( "Restores the selection to current" ));
    selectionLayout->addWidget( restoreButton, 1, 1 );

    connect( restoreButton, SIGNAL( clicked( )),
      this, SLOT( restoreSelection( )));

    auto deleteButton = new QPushButton( "Delete selected" );
    deleteButton->setToolTip( QString( "Deletes the selected selection"));
    selectionLayout->addWidget( deleteButton, 2, 0, 1, 2 );

    selectionLayout->setColumnStretch( 0, 1 );
    selectionLayout->setColumnStretch( 1, 1 );
    connect( deleteButton, SIGNAL( clicked( )), this,
      SLOT( deleteStoredSelection( )));

    _storedSelections.dock->setWidget( dockWidget );
    _storedSelections.dock->close( );
  }
}

void MainWindow::selectDomain( void )
{
  // Active domain
  auto domainArg = nslib::Config::isArgumentDefined( { "--domain", "-d"} );

  QStringList availableDomains( { "congen", "cortex" } );
  QString domainSelected = availableDomains.first( );

  // If not domain via CLI, then ask via GUI dialog
  if ( domainArg.empty( ))
  {
    domainSelected = QInputDialog::getItem(
      this, "Select domain", "Domain name:", availableDomains,  0, false );
  }
  else
  {
    domainSelected =
      QString::fromStdString( nslib::Config::inputArgs( )[ domainArg ][0] );
  }

  nslib::Domain* domain = nullptr;

  const auto &inputArgs = nslib::Config::inputArgs( );
  if ( domainSelected.compare("cortex", Qt::CaseInsensitive) == 0 )
  {
    domain = new nslib::cortex::Domain;
    nslib::DomainManager::setActiveDomain( domain );
    if (!inputArgs.empty() && !domain->dataLoader( )->cliLoadData( inputArgs ))
      exit( -1 );
  }
  else if ( domainSelected.compare("congen", Qt::CaseInsensitive) == 0 )
  {
    domain = new nslib::congen::Domain;
    nslib::DomainManager::setActiveDomain( domain );
    if (!inputArgs.empty() && !domain->dataLoader( )->cliLoadData( inputArgs ))
      exit( -1 );
  }
  else
  {
    QString msg( "domain \"" + domainSelected + "\" unknown. "
      "Valid values are: " );
    msg += availableDomains.join(", ");
    nslib::Loggers::get( )->log( msg.toStdString( ), nslib::LOG_LEVEL_ERROR );
    exit( -1 );
  }

  this->setWindowTitle( this->windowTitle( ) + " (" + domainSelected + ")" );

  if ( domain )
  {
    domain->createGUI( this, _ui->menubar );
  }

  resizeEvent( nullptr );
}

MainWindow::~MainWindow( void )
{
  delete _ui;
  for ( const auto& _canvas :  nslib::PaneManager::panes( ))
    delete _canvas;
}

QString MainWindow::_tableColumnToString( const TTableColumns column )
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
      break;
  }

  return QString();
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
    nslib::Loggers::get( )->log( "Tried to store an empty selection ",
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

  const bool updateExistingRow =
    nslib::SelectionManager::existsStoredSelection( label.toStdString( ));

  nslib::SelectionManager::storeActiveSelection( label.toStdString( ));
  const unsigned int numberOfSelectedEntities =
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
      nslib::Loggers::get( )->log(
        "Stored selection row not found ",
        nslib::LOG_LEVEL_ERROR );
      return;
    }

    const unsigned int row = _storedSelections.table->row( it->second );
    _storedSelections.table->item(
      row, TTableColumns::COLUMN_COUNT )->setText( selectedEntities );

    _storedSelections.table->item(
      row, TTableColumns::COLUMN_DATETIME )->setText( date );

  }
  else
  {
    const unsigned int row = _storedSelections.table->rowCount( );
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
}

void MainWindow::restoreSelection( void )
{
  if ( _storedSelections.table->selectedItems( ).size( ) > 0 )
  {
    QTableWidgetItem* firstItem =
      _storedSelections.table->selectedItems( ).at( 0 );
    const auto label = firstItem->text( );

    nslib::SelectionManager::restoreStoredSelection( label.toStdString( ));

    resizeEvent( nullptr );

    auto &panes = nslib::PaneManager().panes();
    std::for_each(panes.begin(), panes.end(), [](nslib::Canvas *c){ c->resizeEvent(nullptr); });
  }
}

void MainWindow::deleteStoredSelection( void )
{
  if ( _storedSelections.table->selectedItems( ).size( ) > 0 )
  {
    const auto firstItem = _storedSelections.table->selectedItems( ).at( 0 );
    const auto label = firstItem->text( );

    // Remove selection from table
    const unsigned int row = _storedSelections.table->row( firstItem );
    _storedSelections.table->removeRow( row );
    _storedSelections.tableWidgets.erase( label.toStdString( ));

    if ( !nslib::SelectionManager::deleteStoredSelection(
           label.toStdString( )))
    {
      nslib::Loggers::get( )->log(
        "Tried to delete a non existing saved selection ",
        nslib::LOG_LEVEL_WARNING );
    }
  }
}

void MainWindow::sortStoredSelectionsTable( int column )
{
  _storedSelections.table->sortByColumn( column, Qt::AscendingOrder );
}

void MainWindow::killActivePane( void )
{
  nslib::PaneManager::killActivePane( );
}

void MainWindow::duplicateActivePane( void )
{
  //Choose if split will be horizontal or vertical
  if (QObject::sender() == _ui->actionSplitHorizontally )
    nslib::PaneManager::paneDivision(nslib::PaneManager::HORIZONTAL);
  else
    nslib::PaneManager::paneDivision(nslib::PaneManager::VERTICAL);

  //Do the split
  nslib::PaneManager::newPaneFromActivePane( );
}

void MainWindow::home( void )
{
  nslib::PaneManager::activePane( )->displayEntities( nslib::DataManager::rootEntities( ), false, true );
}

void MainWindow::aboutDialog( void )
{
  QString msj = 
    QString( "<h2>NeuroScheme</h2>" ) +
    tr( "A tool for multi-scale visual exploration of neuroscientific data, based on schematic representations." ) + 
    "<br>" + 
    tr( "Version " ) + nslib::Version::getString( ).c_str( ) +
    tr( " rev (%1)<br>").arg(nslib::Version::getRevision( )) +
    "<a href='https://gmrv.es/neuroscheme/'>https://gmrv.es/neuroscheme</a>" + 
    "<h4>" + tr( "Build info:" ) + "</h4>" +
    "<ul><li>NSlib " + NSLIB_REV_STRING +    
    "</li><li>Scoop " + SCOOP_REV_STRING +     
    "</li><li>ShiFT " + SHIFT_REV_STRING + 
    "</li><li>FiReS " + FIRES_REV_STRING +
    
#ifdef NEUROSCHEME_USE_ZEROEQ
    "</li><li>ZeroEQ " + ZEROEQ_REV_STRING +
#else
    "</li><li>ZeroEQ " + tr ("support not built.") +
#endif

#ifdef NEUROSCHEME_USE_NSOL
    "</li><li>Nsol " + NSOL_REV_STRING +
#else
    "</li><li>Nsol " + tr ("support not built.") +
#endif

#ifdef NEUROSCHEME_USE_GMRVLEX
    "</li><li>GmrvLex " + GMRVLEX_REV_STRING +
#else
    "</li><li>GmrvLex " + tr ("support not built.") +
#endif

#ifdef NEUROSCHEME_USE_DEFLECT
    "</li><li>Deflect " + DEFLECT_REV_STRING +
#else
    "</li><li>Deflect " + tr ("support not built.") +
#endif

    "</li><li>AcuteRecorder " + ACUTERECORDER_REV_STRING +

    "</li><li>Qt " + QT_VERSION_STR + "</li></ul>" +
    "<h4>" + tr( "Developed by:" ) + "</h4>" +
    "VG-Lab / URJC / UPM"
    "<br><a href='https://vg-lab.es'>https://vg-lab.es</a>"
    "<br><a href='mailto:dev@vg-lab.es'>dev@vg-lab.es</a><br><br>"
    "<br>(c) 2015-2021<br><br>"
    "<a href='https://vg-lab.es'><img src=':/icons/logoVGLAB.png'/></a>"
    "&nbsp;&nbsp;&nbsp;&nbsp;"
    "<a href='https://www.urjc.es'><img src=':/icons/logoURJC.png' /></a>"
    "&nbsp;&nbsp;&nbsp;&nbsp;"
    "<a href='https://www.upm.es'><img src=':/icons/logoUPM.png' /></a>";
    
  QMessageBox::about(this, tr( "About NeuroScheme" ), msj );
}

void MainWindow::toggleShowConnectivity( void )
{
  nslib::Config::showConnectivity( _ui->actionShowConnectivity->isChecked( ));
  resizeEvent(nullptr);

  auto &panes = nslib::PaneManager::panes();
  std::for_each(panes.begin(), panes.end(), [](nslib::Canvas *c) { c->resizeEvent(nullptr); });
}

void MainWindow::toggleShowNoHierarchyEntities( void )
{
  nslib::Config::showNoHierarchyEntities( _ui->actionShowNoHierarchyEntities->isChecked( ));

  auto displayNon = [](nslib::Canvas *c){ c->displayEntities(c->sceneEntities(), false, false); };
  auto &panes = nslib::PaneManager::panes();
  std::for_each(panes.begin(), panes.end(), displayNon);
}

void MainWindow::toggleShowEntitiesName( void )
{
  nslib::Config::showEntitiesName( _ui->actionShowEntitiesName->isChecked( ));
  nslib::RepresentationCreatorManager::clearCaches( );

  auto displayEntities = [](nslib::Canvas *c){ c->displayEntities(false, false); };
  auto &panes = nslib::PaneManager::panes();
  std::for_each(panes.begin(), panes.end(), displayEntities);
}

// void MainWindow::saveScene( void )
// {
//   //Depending on the selected domain, a certain "saver" could be available
//   //At this moment, only NeuroML
//   emit nslib::congen::DataSaver::saveXmlScene( this );
// }

void MainWindow::actionPublishSelectionToggle( void )
{
  nslib::Config::autoPublishSelection(_ui->actionAutoPublishSelection->isChecked());
}

void MainWindow::actionPublishFocusOnSelectionToggle( void )
{
  nslib::Config::autoPublishFocusOnSelection(_ui->actionAutoFocusOnSelection->isChecked());
}

void MainWindow::actionPublishFocusOnDisplayedToggle( void )
{
  nslib::Config::autoPublishFocusOnDisplayed(_ui->actionAutoFocusOnDisplayed->isChecked());
}

void MainWindow::actionPublishSelection( void )
{
  std::vector< unsigned int > ids;
  nslib::SelectionManager::selectableEntitiesIds( ids );
  nslib::ZeroEQManager::publishSelection( ids );
}

void MainWindow::actionPublishFocusOnSelection( void )
{
  std::vector< unsigned int > ids;
  nslib::SelectionManager::selectableEntitiesIds( ids );
  nslib::ZeroEQManager::publishFocusOnSelection( ids );
}

void MainWindow::actionPublishFocusOnDisplayed( void )
{
  nslib::Loggers::get( )->log(
    std::string("actionPublishFocusOnDisplayed not implemented."),
    nslib::LOG_LEVEL_WARNING,
    NEUROSCHEME_FILE_LINE );
}

void MainWindow::toggleZeroEQ( void )
{
  auto disableZeroEQ = [this]()
  {
    _ui->actionToggleZeroEQ->blockSignals(true);
    _ui->actionToggleZeroEQ->setChecked(false);
    _ui->actionToggleZeroEQ->setEnabled(false);
    _ui->actionToggleZeroEQ->blockSignals(false);
    _ui->actionPublishSelection->setEnabled(false);
    _ui->actionAutoPublishSelection->setEnabled(false);
  };

  if ( _ui->actionToggleZeroEQ->isChecked( ))
  {
    bool ok;
    const QString text = QInputDialog::getText(
      this, tr("Please select ZeroEQ session" ),
      tr("ZeroEQ session:"), QLineEdit::Normal,
      QString::fromStdString( nslib::Config::zeroEQSession( )), &ok );

    if ( ok && !text.isEmpty( ))
    {
      try
      {
        nslib::Config::zeroEQSession( text.toStdString( ));
        nslib::ZeroEQManager::connect( nslib::Config::zeroEQSession( ));
      }
      catch(const std::exception &e)
      {
        nslib::Loggers::get( )->log(
          std::string("Unable to connect to ZeroEQ session: ") + e.what(),
          nslib::LOG_LEVEL_CRITICAL );

        disableZeroEQ();

        const auto title = tr("ZeroEQ");
        const auto message = QString("Unable to connect to ZeroEQ session: %1").arg(text);
        QMessageBox::critical(this, title, message, QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Ok);
      }
    }
    else
      _ui->actionToggleZeroEQ->setChecked( false );
  }
  else
  {
    try
    {
      nslib::ZeroEQManager::disconnect( );
    }
    catch(const std::exception &e)
    {
      nslib::Loggers::get( )->log(
        std::string("Unable to disconnect ZeroEQ: ") + e.what(),
        nslib::LOG_LEVEL_CRITICAL );

      disableZeroEQ();

      const auto title = tr("ZeroEQ");
      const auto message = QString("Unable to disconnect ZeroEQ session.");
      QMessageBox::critical(this, title, message, QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Ok);
    }
  }
}

void MainWindow::exportToJSON( void )
{
  const auto filter = tr("JSON File ( *.JSON *.json );; All files (*)");
  auto selectedFilter = tr("JSON File ( *.JSON *.json )");
  const auto title = tr( "Save JSON File" );
  const auto directory = _lastOpenedFileName.isEmpty() ? QDir::currentPath() : QFileInfo{_lastOpenedFileName}.path();
  const auto options = QFileDialog::Option::DontUseNativeDialog;

  auto path = QFileDialog::getSaveFileName( this, title, directory, filter, &selectedFilter , options);

  if ( !path.isEmpty( ))
  {
    _lastOpenedFileName = QFileInfo{path}.path( );

    if (!path.endsWith(".json"))
    {
      path += ".json";
    }
    auto fileName = path.toStdString( );

    std::ofstream outfile( fileName );
    nslib::DomainManager::getActiveDomain( )->exportJSON( outfile );
  }
}

void MainWindow::importFromJSON()
{
  const auto filter = tr("JSON File ( *.JSON *.json );; All files (*)");
  auto selectedFilter = tr("JSON File ( *.JSON *.json )");
  const auto title = tr( "Open JSON File" );
  const auto directory = _lastOpenedFileName.isEmpty() ? QDir::currentPath() : _lastOpenedFileName;
  const auto options = QFileDialog::Option::DontUseNativeDialog;
  const auto path = QFileDialog::getOpenFileName( this, title, directory, filter, &selectedFilter, options );

  if ( !path.isEmpty( ))
  {
    _lastOpenedFileName = QFileInfo{path}.path( );
    auto fileName = path.toStdString( );

    std::ifstream inputfile( fileName );
    nslib::DataManager::reset( );
    nslib::RepresentationCreatorManager::clearCaches( );
    nslib::RepresentationCreatorManager::clearMaximums( );
    nslib::DomainManager::getActiveDomain( )->importJSON( inputfile );
  }
}

void MainWindow::cleanScene( void )
{
  nslib::DataManager::reset( );
  nslib::RepresentationCreatorManager::clearCaches( );
  nslib::RepresentationCreatorManager::clearMaximums( );

  auto displayRoot = [](nslib::Canvas *c){ c->displayEntities( nslib::DataManager::rootEntities( ), false, true ); };
  auto &panes = nslib::PaneManager::panes();
  std::for_each(panes.begin(), panes.end(), displayRoot);
}

void MainWindow::openRecorder( void )
{

  // The button stops the recorder if found.
  if( _recorder != nullptr )
  {
    _ui->actionRecorder->setDisabled( true );
    _recorder->stop();

    // Recorder will be deleted after finishing.
    _recorder = nullptr;
    _ui->actionRecorder->setChecked( false );
    return;
  }

  RSWParameters params;
  params.widgetsToRecord.emplace_back( "Main Widget" , this );
  params.includeScreens = false;

  if(!_ui->actionAdvancedRecorderOptions->isChecked())
  {
    params.showWorker = false;
    params.showWidgetSourceMode = false;
    params.showSourceParameters = false;
  }

  auto dialog = new RecorderDialog( nullptr , params , true );
  dialog->setWindowIcon( QIcon( ":/icons/ns_icon.png" ));
  dialog->setFixedSize( 800 , 600 );
  if ( dialog->exec( ) == QDialog::Accepted)
  {
    _recorder = dialog->getRecorder( );
    connect( _recorder , SIGNAL( finished( )) ,
             _recorder , SLOT( deleteLater( )));
    connect( _recorder , SIGNAL( finished( )) ,
             this , SLOT( finishRecording( )));
    _ui->actionRecorder->setChecked( true );
  } else
  {
    _ui->actionRecorder->setChecked( false );
  }
  dialog->deleteLater( );
}

void MainWindow::finishRecording( )
{
  _ui->actionRecorder->setEnabled( true );
}
