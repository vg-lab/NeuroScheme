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

#include "DataLoader.h"
#include "Domain.h"
#include "RepresentationCreator.h"
#include <nslibcongen/api.h>
#include <nslib/DataManager.h>
#include <nslib/PaneManager.h>
#include <nslib/RepresentationCreatorManager.h>
#include <shift_NeuronPop.h>
#include <shift_NeuronSuperPop.h>
#include <shift_congen_entities.h>
#include <shift_congen_relationshipProperties.h>
#include <QFileDialog>

namespace nslib
{
  namespace congen
  {
    DomainGUI::DomainGUI( QMainWindow* mw_, QMenuBar* menubar )
      : _mw( mw_ )
    {
      for ( auto action : menubar->actions( ))
      {
        if ( action->menu( ))
        {
          auto menu = dynamic_cast< QMenu* >( action->parent( ));
          if ( action->text( ) == "&File" && menu )
          {
            _actionLoadNeuroML.reset( new QAction( "&Load NeuroML", menu ));
            menu->insertAction( menu->actions( ).last( ),
              _actionLoadNeuroML.get( ));
            connect( _actionLoadNeuroML.get( ), SIGNAL( triggered( )),
                     this, SLOT( loadNeuroML( )));

            _actionSaveNeuroML.reset( new QAction( "&Save NeuroML", menu ));
            menu->insertAction( menu->actions( ).last( ),
              _actionSaveNeuroML.get( ));
            connect( _actionSaveNeuroML.get( ), SIGNAL( triggered( )),
              this, SLOT( saveNeuroML( )));

            QIcon iconSave;
            iconSave.addFile( QStringLiteral( ":/icons/save.svg" ),
              QSize( ), QIcon::Normal, QIcon::Off );
            _actionSaveNeuroML->setIcon( iconSave );

            QIcon iconLoad;
            iconLoad.addFile( QStringLiteral( ":/icons/open.svg" ),
              QSize( ), QIcon::Normal, QIcon::Off );
            _actionLoadNeuroML->setIcon( iconLoad );
          }
        }
      }
    }

    void DomainGUI::loadNeuroML( void )
    {
      const auto title = tr( "Open Xml Scene (NeuroML format)" );
      const auto options = QFileDialog::Option::DontUseNativeDialog;
      const auto filters = tr( "NeuroML ( *.xml );; All files (*)");
      auto selectedFilter = tr("NeuroML ( *.xml )");
      const auto directory = _lastOpenedFileName.isEmpty() ? QDir::currentPath() : _lastOpenedFileName;

      const auto path = QFileDialog::getOpenFileName(_mw, title, directory, filters, &selectedFilter, options);

      if ( !path.isEmpty( ))
      {
        _lastOpenedFileName = QFileInfo( path ).path( );
        auto fileName = path.toStdString( );

        Loggers::get( )->log( "Loading blue config",
          LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

        congen::DataLoader::loadNeuroML( fileName );

        auto canvas = PaneManager::activePane( );
        canvas->displayEntities(
          DataManager::rootEntities( ), false, true );
        PaneManager::panes( ).insert( canvas );
      }
    };

    void DomainGUI::saveNeuroML( void )
    {
      congen::DataSaver::saveXmlScene( _mw );
    };

    using NeuronPop = shiftgen::NeuronPop;

    Domain::Domain( void )
    {
      this->_exportRelations = { "isParentOf", "connectsTo",
        "aggregatedConnectsTo", "aggregatedConnectedBy" };
      this->_exportAggregatedRelations = { false, false, true, true };
      this->_domainName = "congen";
      this->_dataLoader = new DataLoader;
      this->_entitiesTypes = new congen::shiftgen::EntitiesTypes( );
      this->_relationshipPropertiesTypes =
        new congen::shiftgen::RelationshipPropertiesTypes( );
      auto repCreator = new congen::RepresentationCreator( );
      RepresentationCreatorManager::addCreator( repCreator );
      auto& _entities = DataManager::entities( );
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

      shift::RelationshipProperties* connectsToObj =
        _relationshipPropertiesTypes->
        getRelationshipProperties( "aggregatedConnectsTo" );

      _entities.relationships( )[ "aggregatedConnectsTo" ] =
        new shift::RelationshipAggregatedOneToN( "aggregatedConnectsTo",
        connectsToObj, relChildOf, relConnectsTo );
      _entities.relationships( )[ "aggregatedConnectedBy" ] =
        new shift::RelationshipAggregatedOneToN( "aggregatedConnectedBy",
        connectsToObj, relChildOf, relConnectedBy );
    }

    bool Domain::isSelectableEntity( shift::Entity* entity ) const
    {
      return dynamic_cast< NeuronPop* >( entity );
    }

    unsigned int Domain::selectableEntityId( shift::Entity* /* entity*/ ) const
    {
      return 0;
    }

    const Vector4f Domain::entity3DPosition ( shift::Entity* /* entity*/ ) const
    {
      return Vector4f( 0.f, 0.f, 0.f, 0.f  );
    }

    void Domain::usageMessage( void )
    {
    }

    void Domain::importRelationshipsJSON(
      const boost::property_tree::ptree& relationships,
      std::unordered_map < unsigned int, shift::Entity* >* oldGIDToEntity )
    {
      addIsParentOfRelationshipsFromJSON( getRelationsOfType(
        "isParentOf", relationships ), oldGIDToEntity );

      addConnectsToRelationsFromJSON( getRelationsOfType(
        "connectsTo", relationships ), oldGIDToEntity );

      addAggregatedConnectionFromJSON( getRelationsOfType(
        "aggregatedConnectedBy", relationships ), "aggregatedConnectedBy",
        oldGIDToEntity );

      addAggregatedConnectionFromJSON( getRelationsOfType(
        "aggregatedConnectsTo", relationships ),"aggregatedConnectsTo",
        oldGIDToEntity );
    }

    void Domain::exportRepresentationMaxMin( std::ostream& outputStream,
      bool minimizeStream ) const
    {
      std::string maxWeightLabel;
      std::string maxNbNeuronsLabel;
      std::string maxSuperPopLevelsLabel;
      std::string closeQuotationsLabel;
      if ( minimizeStream )
      {
        maxWeightLabel = "\"maxWeight\":\"";
        maxNbNeuronsLabel = "\",\"maxNbNeurons\":\"";
        maxSuperPopLevelsLabel = "\",\"maxSuperPopLevels\":\"";
        closeQuotationsLabel = "\"";
      }
      else
      {
        maxWeightLabel = "    \"maxWeight\": \"";
        maxNbNeuronsLabel = "\",\n    \"maxNbNeurons\": \"";
        maxSuperPopLevelsLabel = "\",\n    \"maxSuperPopLevels\": \"";
        closeQuotationsLabel = "\"\n";
      }
      auto repCreator = ( RepresentationCreator* )
        RepresentationCreatorManager::getCreator( );
      outputStream << maxWeightLabel << repCreator->maxAbsoluteWeight( )
        << maxNbNeuronsLabel << repCreator->maxNeuronsPerPopulation( )
        << maxSuperPopLevelsLabel << repCreator->maxLevelsPerSuperPop( )
        << closeQuotationsLabel;
    }

    void Domain::importMaximumsJSON( const boost::property_tree::ptree& maximums )
    {
      auto repCreator = ( RepresentationCreator* )
        RepresentationCreatorManager::getCreator( );

      try
      {
        unsigned int maxNeuronsPerPopulation =
          maximums.get< unsigned int >( "maxNbNeurons" );
        repCreator->maxNeuronsPerPopulation( maxNeuronsPerPopulation, true );
      }
      catch ( std::exception const& ex )
      {
        Loggers::get( )->log( "ERROR: getting maxNbNeurons from JSON: "
          + std::string( ex.what( )), LOG_LEVEL_WARNING );
      };

      try
      {
        float maxAbsoluteWeight =
          maximums.get< float >( "maxWeight" );
        repCreator->maxAbsoluteWeight( maxAbsoluteWeight, true );
      }
      catch ( std::exception const& ex )
      {
        Loggers::get( )->log( "ERROR: getting maxWeight from JSON: "
          + std::string( ex.what( )), LOG_LEVEL_WARNING );
      };

      try
      {
        unsigned int maxSuperPopLevels =
          maximums.get< unsigned int >( "maxSuperPopLevels" );
        repCreator->maxLevelsPerSuperPop( maxSuperPopLevels, true );
      }
      catch ( std::exception const& ex )
      {
        Loggers::get( )->log( "ERROR: getting maxSuperPopLevels from JSON: "
          + std::string( ex.what( )), LOG_LEVEL_WARNING );
      };
    }
  }
}
