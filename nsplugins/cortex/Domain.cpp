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
          if ( action->text( ) == "File" && menu )
          {
            _actionLoadBlueConfig.reset( new QAction( "Load BlueConfig", menu ));
            menu->addAction( _actionLoadBlueConfig.get( ));
            connect( _actionLoadBlueConfig.get( ), SIGNAL( triggered( )),
                     this, SLOT( loadBlueConfig( )));

            // QIcon iconSave;
            // iconSave.addFile( QStringLiteral( ":/icons/save.svg" ),
            //                   QSize( ), QIcon::Normal, QIcon::Off );
            // _actionSaveNeuroML->setIcon( iconSave );

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

    public slots:
      void close( void ) { this->close( ); }

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
//          _viewer->LoadBlueConfig( fileName, targetLabel, false, false );

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

    }

    Domain::Domain( void )
    {
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
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isChildOf" ] =
        new shift::RelationshipOneToOne;

      _entities.relationships( )[ "isAGroupOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isPartOf" ] =
        new shift::RelationshipOneToN;

      _entities.relationships( )[ "isSuperEntityOf" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "isSubEntityOf" ] =
        new shift::RelationshipOneToOne;

      _entities.relationships( )[ "connectsTo" ] =
        new shift::RelationshipOneToN;
      _entities.relationships( )[ "connectedBy" ] =
        new shift::RelationshipOneToN;

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

  }
}
