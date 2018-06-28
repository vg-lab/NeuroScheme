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
        new Eigen4VectorCaster());

      this->_exportRelations =
        { "connectsTo", "isParentOf", "isAGroupOf", "isSuperEntityOf" };
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
      _entities.relationships( )[ "isChildOf" ] =
        new shift::RelationshipOneToOne( "isChildOf" );

      _entities.relationships( )[ "isAGroupOf" ] =
        new shift::RelationshipOneToN( "isAGroupOf" );
      _entities.relationships( )[ "isPartOf" ] =
        new shift::RelationshipOneToN( "isPartOf" );

      _entities.relationships( )[ "isSuperEntityOf" ] =
        new shift::RelationshipOneToN( "isSuperEntityOf" );
      _entities.relationships( )[ "isSubEntityOf" ] =
        new shift::RelationshipOneToOne( "isSubEntityOf" );

      _entities.relationships( )[ "connectsTo" ] =
        new shift::RelationshipOneToN( "connectsTo" );
      _entities.relationships( )[ "connectedBy" ] =
        new shift::RelationshipOneToN( "connectedBy");

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

    void Domain::addRelationsOfType( std::istream &inputStream,
      std::string relationName, std::unordered_map < unsigned int,
      shift::Entity* >* oldGUIToEntity )
    {
      if ( relationName == "connectsTo")
      {
        addConnectsToRelationsToJSON( inputStream, oldGUIToEntity );
      }
      else if ( relationName == "isParentOf" )
      {
        addIsParentOfRelationshipsToJSON( inputStream, oldGUIToEntity );
      }
      else if ( relationName == "isAGroupOf" )
      {
        addIsAGroupOfRelationshipsToJSON( inputStream, oldGUIToEntity );
      }
      else if ( relationName == "isSuperEntityOf" )
      {
        addIsSuperEntityOfRelationshipsToJSON( inputStream, oldGUIToEntity );
      }
      else
      {
        SHIFT_THROW( "ERROR: unknown type of relation: "+relationName );
      }
    }

    void Domain::addIsAGroupOfRelationshipsToJSON( std::istream& inputStream,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
    {
      auto& relGroupOf = *( DataManager::entities( )
        .relationships( )[ "isAGroupOf" ]->asOneToN( ) );
      auto& relGroupBy = *( DataManager::entities( )
        .relationships( )[ "isPartOf" ]->asOneToN( ) );

      std::string line;
      while ( std::getline( inputStream, line ) )
      {
        auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
        if ( line[ firstNotWhiteSpace ] == ']' )
        {
          break;
        }
        else
        {
          SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '{',
            "ERROR parsing object: this line must be a '{' for start the"
            " JSONobject of the entity or an ] to close the JSONArray of the"
            " entites, instead does: \"" + line + "\"." );

          shift::Entity* origEntity = nullptr;
          shift::Entity* destEntity = nullptr;
          importJSONRelationGIDS(inputStream, oldGUIToEntity,
            origEntity, destEntity);

          shift::Relationship::Establish( relGroupOf, relGroupBy,
            origEntity,destEntity );

          SHIFT_CHECK_THROW( std::getline( inputStream, line ),
            "ERROR parsing object: this line must close the JSONObject"
            " but instead it doesn't exit" );

          firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

          SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '}',
            "ERROR parsing object: this line must be a '}' for close the "
            "JSONObject, instead does: \"" + line + "\"." );
        }
      }
    }

    void
    Domain::addIsSuperEntityOfRelationshipsToJSON( std::istream &inputStream,
      std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity )
    {
      auto& relSuperEntity = *( DataManager::entities( )
        .relationships( )[ "isSuperEntityOf" ]->asOneToN( ) );
      auto& relSubEntity = *( DataManager::entities( )
        .relationships( )[ "isSubEntityOf" ]->asOneToOne( ) );

      std::string line;
      while ( std::getline( inputStream, line ) )
      {
        auto firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );
        if ( line[ firstNotWhiteSpace ] == ']' )
        {
          break;
        }
        else
        {
          SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '{',
            "ERROR parsing object: this line must be a '{' for start the "
            "JSONobject of the entity or an ] to close the JSONArray of the"
            " entites, instead does: \"" + line + "\"." );

          shift::Entity* origEntity = nullptr;
          shift::Entity* destEntity = nullptr;

          importJSONRelationGIDS(inputStream, oldGUIToEntity,
            origEntity, destEntity);

          shift::Relationship::Establish( relSuperEntity, relSubEntity,
            origEntity,destEntity );

          SHIFT_CHECK_THROW( std::getline( inputStream, line ),
            "ERROR parsing object: this line must close the JSONObject"
            " but instead it doesn't exit" );

          firstNotWhiteSpace = line.find_first_not_of( "  \r\t" );

          SHIFT_CHECK_THROW( line[ firstNotWhiteSpace ] == '}',
            "ERROR parsing object: this line must be a '}' for close the "
            "JSONObject, instead does: \"" + line + "\"." );
        }
      }
    }

  }
}
