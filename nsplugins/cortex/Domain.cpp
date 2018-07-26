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
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "Domain.h"
#include "Neuron.h"
#include <nslib/RepresentationCreatorManager.h>
#include <nslib/DataManager.h>
#include <nslib/PaneManager.h>
#include "RepresentationCreator.h"
#include <shift_cortex_entities.h>
#include <shift_cortex_relationshipProperties.h>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QtWidgets/QGridLayout>

namespace nslib
{
  namespace cortex
  {
    DomainGUI::DomainGUI( QMainWindow* mw_, QMenuBar* menubar )
      : _mw( mw_ )
      , _lastOpenedFileName( QDir::currentPath( ))
    {
      for ( auto action : menubar->actions( ))
      {
        if ( action->menu( ))
        {
          auto menu = dynamic_cast< QMenu* >( action->parent( ));
          if ( action->text( ) == "&File" && menu )
          {
            _actionLoadBlueConfig.reset( new QAction( "Load &BlueConfig", menu ));
            menu->insertAction( menu->actions( ).last( ), _actionLoadBlueConfig.get( ));
            connect( _actionLoadBlueConfig.get( ), SIGNAL( triggered( )),
                     this, SLOT( loadBlueConfig( )));

            _actionLoadXmlScene.reset( new QAction( "Load &XML Scene", menu ));
            menu->insertAction( menu->actions( ).last( ), _actionLoadXmlScene.get( ));
            connect( _actionLoadXmlScene.get( ), SIGNAL( triggered( )),
                     this, SLOT( loadXmlScene( )));

            QIcon iconSave;
            iconSave.addFile( QStringLiteral( ":/icons/open.svg" ),
                              QSize( ), QIcon::Normal, QIcon::Off );
            _actionLoadBlueConfig->setIcon( iconSave );
            _actionLoadXmlScene->setIcon( iconSave );

            #ifndef NSOL_USE_BRION
            _actionLoadBlueConfig->setEnabled( false );
            #endif
            #ifndef NEUROSCHEME_USE_NSOL
            _actionLoadXmlScene->setEnabled( false );
            #endif
          }
        }
      }
    }

    class BlueConfigOptions : public QDialog
    {
    public:
      BlueConfigOptions( QWidget* parent = Q_NULLPTR,
                         Qt::WindowFlags flags = Qt::WindowFlags( ))
        : QDialog( parent, flags )
      {
        this->setWindowTitle( "BlueConfig loading options:" );

        _layout.reset( new QGridLayout );
        int row = 0;

        _labelLoadMorpho.reset(
          new QLabel( tr( "Load morphologies:" )));
        _checkLoadMorpho.reset( new QCheckBox( ));
        _checkLoadMorpho->setChecked( true );
        _labelLoadMorpho->setBuddy( _checkLoadMorpho.get( ));
        _layout->addWidget( _labelLoadMorpho.get( ), row, 0);
        _layout->addWidget( _checkLoadMorpho.get( ), row, 1);
        row++;

        _labelLoadConnectivity.reset(
          new QLabel( tr( "Load connectivity:" )));
        _checkLoadConnectivity.reset( new QCheckBox( ));
        _checkLoadConnectivity->setChecked( false );
        _labelLoadConnectivity->setBuddy( _checkLoadConnectivity.get( ));
        _layout->addWidget( _labelLoadConnectivity.get( ), row, 0);
        _layout->addWidget( _checkLoadConnectivity.get( ), row, 1);
        row++;

        _labelLoadCSVStats.reset(
          new QLabel( tr( "Load csv stats file:" )));
        _checkLoadCSVStats.reset( new QCheckBox( ));
        _checkLoadCSVStats->setChecked( false );
        _labelLoadCSVStats->setBuddy( _checkLoadCSVStats.get( ));
        _layout->addWidget( _labelLoadCSVStats.get( ), row, 0);
        _layout->addWidget( _checkLoadCSVStats.get( ), row, 1);
        row++;

        _okButton.reset( new QPushButton( "Ok" ));
        _layout->addWidget( _okButton.get( ), row, 1);
        connect( _okButton.get( ), SIGNAL( clicked( )), this, SLOT( close( )));
        setLayout( _layout.get( ));
      }

      bool loadMorphology( void ) { return _checkLoadMorpho->isChecked( ); }
      bool loadConnectivity( void ) { return _checkLoadConnectivity->isChecked( ); }
      bool loadCSVStats( void ) { return _checkLoadCSVStats->isChecked( ); }

    // public slots:
    //   void ok( void ) { this->close( ); }

    protected:
      std::unique_ptr< QLabel > _labelLoadMorpho;
      std::unique_ptr< QCheckBox > _checkLoadMorpho;
      std::unique_ptr< QLabel > _labelLoadConnectivity;
      std::unique_ptr< QCheckBox > _checkLoadConnectivity;
      std::unique_ptr< QLabel > _labelLoadCSVStats;
      std::unique_ptr< QCheckBox > _checkLoadCSVStats;
      std::unique_ptr< QGridLayout > _layout;
      std::unique_ptr< QPushButton > _okButton;
    };

    void DomainGUI::loadBlueConfig( void )
    {
#ifdef NSOL_USE_BRION
      QString path;
      QString filter(
        tr( "BlueConfig ( BlueConfig CircuitConfig);; All files (*)" ));
      auto fd = new QFileDialog( _mw, QString( "Open BlueConfig" ),
                                 _lastOpenedFileName, filter );

      fd->setOption( QFileDialog::DontUseNativeDialog, true );
      fd->setFileMode( QFileDialog/*::FileMode*/::AnyFile );
      if ( fd->exec( ))
        path = fd->selectedFiles( )[0];

      delete fd;

      if ( !path.isEmpty( ))
      {
        bool ok;
        QString text = QInputDialog::getText( _mw, tr("Please select target"),
                                              tr("Cell target:"),
                                              QLineEdit::Normal,
                                              "Column", &ok );
        if ( ok && !text.isEmpty( ))
        {

          auto blueConfigOpts = new BlueConfigOptions( _mw );
          blueConfigOpts->exec( );
          {
            bool loadMorphology = blueConfigOpts->loadMorphology( );
            bool loadConnectivity = blueConfigOpts->loadConnectivity( );

            std::string cns;
            if ( blueConfigOpts->loadCSVStats( ))
            {
              QString csvFilter(
                tr( "csv ( *.csv )" ));
              fd = new QFileDialog( _mw, QString( "Open CSV stats file" ),
                                    _lastOpenedFileName, csvFilter );

              fd->setOption( QFileDialog::DontUseNativeDialog, true );
              fd->setFileMode( QFileDialog/*::FileMode*/::AnyFile );
              if ( fd->exec( ))
                cns = fd->selectedFiles( )[0].toStdString( );

              delete fd;

            }

            auto targetLabel = text.toStdString( );
            Loggers::get( )->log( std::string(" Loading target " ) +
                                  std::string( targetLabel ));

            _lastOpenedFileName = QFileInfo( path ).path( );
            std::string fileName = path.toStdString( );

            Loggers::get( )->log( "Loading blue config",
                                  nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );
            nslib::DataManager::loadBlueConfig(
              fileName, targetLabel, loadMorphology, cns, loadConnectivity );

            DataLoader::createEntitiesFromNsolColumns(
              nslib::DataManager::nsolDataSet( ).columns( ),
              nslib::DataManager::nsolDataSet( ).circuit( ),
              loadMorphology, cns );

            auto canvas = nslib::PaneManager::activePane( );
            canvas->displayEntities(
              nslib::DataManager::rootEntities( ), false, true );
            nslib::PaneManager::panes( ).insert( canvas );

          }
        }
      }
#endif
    }

    void DomainGUI::loadXmlScene( void )
    {
#ifdef NEUROSCHEME_USE_NSOL
      QString path;
      QString filter( tr("Xml Scene ( *.xml );; All files (*)" ));
      auto fd = new QFileDialog( _mw, QString( "Open Xml Scene" ),
                                 _lastOpenedFileName, filter );

      fd->setOption( QFileDialog::DontUseNativeDialog, true );
      fd->setFileMode( QFileDialog/*::FileMode*/::AnyFile );
      if ( fd->exec( ))
        path = fd->selectedFiles( )[0];

      if ( path != QString(""))
      {
        _lastOpenedFileName = QFileInfo( path ).path( );
        auto fileName = path.toStdString( );

        Loggers::get( )->log( "Loading blue config",
                              nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );
        nslib::DataManager::loadNsolXmlScene( fileName );

        DataLoader::createEntitiesFromNsolColumns(
          nslib::DataManager::nsolDataSet( ).columns( ),
          nslib::DataManager::nsolDataSet( ).circuit( ));

        auto canvas = nslib::PaneManager::activePane( );
        canvas->displayEntities(
          nslib::DataManager::rootEntities( ), false, true );
        nslib::PaneManager::panes( ).insert( canvas );
      }
#endif
    }

    Domain::Domain( void )
    {
      fires::PropertyManager::setTypePropertyCaster( typeid( Eigen::Vector4f ),
        new Eigen4VectorCaster( ));

      this->_exportRelations =
        { "isParentOf", "isAGroupOf", "isSuperEntityOf", "connectsTo", };
      this->_domainName = "cortex";
      this->_dataLoader = new DataLoader;
      this->_entitiesTypes = new nslib::cortex::shiftgen::EntitiesTypes;
      this->_relationshipPropertiesTypes =
        new nslib::cortex::shiftgen::RelationshipPropertiesTypes( );
      auto repCreator = new RepresentationCreator( );
      nslib::RepresentationCreatorManager::addCreator( repCreator );
      auto& _entities = nslib::DataManager::entities( );
      //fires::PropertyManager::clear( );
      _entities.clear( );

      _entities.relationships( )[ "isParentOf" ] =
        new shift::RelationshipOneToN( "isParentOf" );
      auto relChildOf = new shift::RelationshipOneToOne( "isChildOf" );
      _entities.relationships( )[ "isChildOf" ] = relChildOf;

      _entities.relationships( )[ "isAGroupOf" ] =
        new shift::RelationshipOneToN( "isAGroupOf" );
      _entities.relationships( )[ "isPartOf" ] =
        new shift::RelationshipOneToN( "isPartOf" );

      _entities.relationships( )[ "isSuperEntityOf" ] =
        new shift::RelationshipOneToN( "isSuperEntityOf" );
      _entities.relationships( )[ "isSubEntityOf" ] =
        new shift::RelationshipOneToOne( "isSubEntityOf" );

      auto relConnectsTo = new shift::RelationshipOneToN( "connectsTo" );
      auto relConnectedBy = new shift::RelationshipOneToN( "connectedBy" );
      _entities.relationships( )[ "connectsTo" ] = relConnectsTo;
      _entities.relationships( )[ "connectedBy" ] = relConnectedBy;

      shift::RelationshipProperties* connectsToObj = _relationshipPropertiesTypes->getRelationshipProperties(
        "connectsTo" );

      _entities.relationships( )[ "aggregatedConnectsTo" ] =
        new shift::RelationshipAggregatedOneToN( "aggregatedConnectsTo", connectsToObj, relChildOf, relConnectsTo );
      _entities.relationships( )[ "aggregatedConnectedBy" ] =
        new shift::RelationshipAggregatedOneToN( "aggregatedConnectedBy", connectsToObj, relChildOf,relConnectedBy);

    }

    bool Domain::isSelectableEntity( shift::Entity* entity ) const
    {
      return dynamic_cast< Neuron* >( entity );
    }

    unsigned int Domain::selectableEntityId( shift::Entity* entity ) const
    {
      assert( dynamic_cast< Neuron* >( entity ));
      return dynamic_cast< Neuron* >( entity )->
        getProperty( "gid" ).value< uint >( );
    }

    const Vector4f Domain::entity3DPosition ( shift::Entity* entity ) const
    {
      // std::cout << entity->getProperty( "Position3D" ).type( ) << std::endl;
      return entity->getProperty( "Position3D" ).value< Vector4f >( );
    }

    void Domain::usageMessage( void )
    {
      std::cerr << "\t\t[ -bc blue_config_path " // | " -swc swc_file_list "
                << " | -xml scene_xml ] "
                << std::endl
                << "\t\t[ -target target_label (*1) ] "
                << std::endl
                << "\t\t[ --no-morphologies | -nm ] "
                << std::endl
                << "\t\t[ -cns | --csv-neuron-stats ] csv_file"
                << std::endl
                << "\t\t[ -lc | --load-connectivity ] "
                << std::endl << std::endl
                << "\t\t(*1) only for BlueConfig files" << std::endl;

    }

    void Domain::importRelationshipsJSON(
      const boost::property_tree::ptree& relationships,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
    {

      addIsParentOfRelationshipsToJSON( getRelationsOfType(
        "isParentOf", relationships), oldGUIToEntity );

      addConnectsToRelationsToJSON( getRelationsOfType(
        "connectsTo", relationships), oldGUIToEntity );

      addIsAGroupOfRelationshipsToJSON( getRelationsOfType(
        "connectsTo", relationships), oldGUIToEntity );

      addIsSuperEntityOfRelationshipsToJSON( getRelationsOfType(
        "connectsTo", relationships), oldGUIToEntity );
    }

    void Domain::addIsAGroupOfRelationshipsToJSON(
      const boost::property_tree::ptree&  relations,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
    {
      auto& relGroupOf = *( DataManager::entities( )
        .relationships( )[ "isAGroupOf" ]->asOneToN( ) );
      auto& relGroupBy = *( DataManager::entities( )
        .relationships( )[ "isPartOf" ]->asOneToN( ) );

      for ( const auto& relation : relations )
      {
        shift::Entity* origEntity;
        shift::Entity* destEntity;

        importJSONRelationGIDS( relation.second, oldGUIToEntity, origEntity,
          destEntity, "GroupOf" );

        shift::Relationship::Establish( relGroupOf, relGroupBy,
          origEntity, destEntity );
      }

    }

    void
    Domain::addIsSuperEntityOfRelationshipsToJSON(
      const boost::property_tree::ptree&  relations,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
    {
      auto& relSuperEntity = *( DataManager::entities( )
        .relationships( )[ "isSuperEntityOf" ]->asOneToN( ));
      auto& relSubEntity = *( DataManager::entities( )
        .relationships( )[ "isSubEntityOf" ]->asOneToOne( ));

      for ( const auto& relation : relations )
      {
        shift::Entity* origEntity;
        shift::Entity* destEntity;

        importJSONRelationGIDS( relation.second, oldGUIToEntity, origEntity,
          destEntity, "SuperEntityOf" );

        shift::Relationship::Establish( relSuperEntity, relSubEntity,
          origEntity, destEntity );
      }
    }

    void Domain::exportRepresentationMaxMin( std::ostream& outputStream,
      bool minimizeStream ) const
    {
      std::string maxNeuronSomaVolumeLabel;
      std::string maxNeuronSomaAreaLabel;
      std::string maxNeuronDendsVolumeLabel;
      std::string maxNeuronDendsAreaLabel;
      std::string maxNeuronsLabel;
      std::string maxNeuronsPerColumnLabel;
      std::string maxNeuronsPerMiniColumnLabel;
      std::string maxConnsPerEntityLabel;
      std::string closeQuotationsLabel;
      if ( minimizeStream )
      {
        maxNeuronSomaVolumeLabel = "\"maxNeuronSomaVolume\":\"";
        maxNeuronSomaAreaLabel = "\",\"maxNeuronSomaArea\":\"";
        maxNeuronDendsVolumeLabel = "\",\"maxNeuronDendsVolume\":\"";
        maxNeuronDendsAreaLabel = "\",\"maxNeuronDendsArea\":\"";
        maxNeuronsLabel = "\",\"maxNeurons\":\"";
        maxNeuronsPerColumnLabel = "\",\"maxNeuronsPerColumn\":\"";
        maxNeuronsPerMiniColumnLabel = "\",\"maxNeuronsPerMiniColumn\":\"";
        maxConnsPerEntityLabel = "\",\"maxConnectionsPerEntity\":\"";
        closeQuotationsLabel = "\"";

      }
      else
      {
        maxNeuronSomaVolumeLabel = "    \"maxNeuronSomaVolume\": \"";
        maxNeuronSomaAreaLabel = "\",\n    \"maxNeuronSomaArea\": \"";
        maxNeuronDendsVolumeLabel = "\",\n    \"maxNeuronDendsVolume\": \"";
        maxNeuronDendsAreaLabel = "\",\n    \"maxNeuronDendsArea\": \"";
        maxNeuronsLabel = "\",\n    \"maxNeurons\": \"";
        maxNeuronsPerColumnLabel = "\",\n    \"maxNeuronsPerColumn\": \"";
        maxNeuronsPerMiniColumnLabel = "\",\n    \"maxNeuronsPerMiniColumn\": \"";
        maxConnsPerEntityLabel = "\",\n    \"maxConnectionsPerEntity\": \"";
        closeQuotationsLabel = "\"\n";
      }
      auto repCreator = ( RepresentationCreator* )
        nslib::RepresentationCreatorManager::getCreator( );

      outputStream << maxNeuronSomaVolumeLabel
        << repCreator->maxNeuronSomaVolume( ) << maxNeuronSomaAreaLabel
        << repCreator->maxNeuronSomaArea( ) << maxNeuronDendsVolumeLabel
        << repCreator->maxNeuronDendsVolume( ) << maxNeuronDendsAreaLabel
        << repCreator->maxNeuronDendsArea( )  << maxNeuronsLabel
        << repCreator->maxNeurons( ) << maxNeuronsPerColumnLabel
        << repCreator->maxNeuronsPerColumn( ) << maxNeuronsPerMiniColumnLabel
        << repCreator->maxNeuronsPerMiniColumn( ) << maxConnsPerEntityLabel
        << repCreator->maxConnectionsPerEntity( ) << closeQuotationsLabel;
    }

    void Domain::importMaximumsJSON(
      const boost::property_tree::ptree& maximums )
    {
      auto repCreator = ( RepresentationCreator* )
        nslib::RepresentationCreatorManager::getCreator( );

      try
      {
        float maxNeuronSomaVolume =
          maximums.get< float >( "maxNeuronSomaVolume" );
        repCreator->maxNeuronSomaVolume( maxNeuronSomaVolume, true );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxNeuronSomaVolume for JSON: "
           + std::string( ex.what( )));
      };

      try
      {
        float maxNeuronSomaArea =
          maximums.get< float >( "maxNeuronSomaArea" );
        repCreator->maxNeuronSomaArea( maxNeuronSomaArea, true);
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxNeuronSomaArea for JSON: "
          + std::string( ex.what( )));
      };

      try
      {
        float maxNeuronDendsVolume =
          maximums.get< float >( "maxNeuronDendsVolume" );
        repCreator->maxNeuronDendsVolume( maxNeuronDendsVolume, true );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxNeuronDendsVolume for JSON: "
          + std::string( ex.what( )));
      };

      try
      {
        float maxNeuronDendsArea =
          maximums.get< float >( "maxNeuronDendsArea" );
        repCreator->maxNeuronDendsArea( maxNeuronDendsArea, true );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxNeuronDendsArea for JSON: "
          + std::string( ex.what( )));
      };

      try
      {
        unsigned int maxNeurons =
          maximums.get< unsigned int >( "maxNeurons" );
        repCreator->maxNeurons( maxNeurons, true );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxNeurons for JSON: "
          + std::string( ex.what( )));
      };

      try
      {
        unsigned int maxNeuronsPerColumn =
          maximums.get< unsigned int >( "maxNeuronsPerColumn" );
        repCreator->maxNeuronsPerColumn( maxNeuronsPerColumn, true );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxNeuronsPerColumn for JSON: "
          + std::string( ex.what( )));
      };

      try
      {
        unsigned int maxNeuronsPerMiniColumn =
          maximums.get< unsigned int >( "maxNeuronsPerMiniColumn" );
        repCreator->maxNeuronsPerMiniColumn( maxNeuronsPerMiniColumn, true );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxNeuronsPerMiniColumn for JSON: "
          + std::string( ex.what( )));
      };

      try
      {
        unsigned int maxConnsPerEntity =
          maximums.get< unsigned int >( "maxConnectionsPerEntity" );
        repCreator->maxConnectionsPerEntity( maxConnsPerEntity, true );
      }
      catch ( std::exception const& ex )
      {
        SHIFT_THROW( "ERROR: getting maxConnsPerEntity for JSON: "
          + std::string( ex.what( )));
      };
    }

  }
}
