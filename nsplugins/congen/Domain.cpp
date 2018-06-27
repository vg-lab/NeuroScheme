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
#include <nslib/DataManager.h>
#include <nslib/PaneManager.h>
#include <nslib/RepresentationCreatorManager.h>
#include <shift_NeuronPop.h>
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
            menu->insertAction( menu->actions( ).last( ), _actionLoadNeuroML.get( ));
            connect( _actionLoadNeuroML.get( ), SIGNAL( triggered( )),
                     this, SLOT( loadNeuroML( )));

            _actionSaveNeuroML.reset( new QAction( "&Save NeuroML", menu ));
            menu->insertAction( menu->actions( ).last( ), _actionSaveNeuroML.get( ));
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
      QString path = QFileDialog::getOpenFileName(
        _mw, "Open Xml Scene", _lastOpenedFileName,
        tr("Xml Scene ( *.xml );; All files (*)" ));

      if ( !path.isEmpty( ))
      {
        _lastOpenedFileName = QFileInfo( path ).path( );
        auto fileName = path.toStdString( );

        Loggers::get( )->log( "Loading blue config",
          nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

        nslib::congen::DataLoader::loadNeuroML( fileName );

        // DataLoader::createEntitiesFromNsolColumns(
        //   nslib::DataManager::nsolDataSet( ).columns( ),
        //   nslib::DataManager::nsolDataSet( ).circuit( ));

        auto canvas = nslib::PaneManager::activePane( );
        canvas->displayEntities(
          nslib::DataManager::rootEntities( ), false, true );
        nslib::PaneManager::panes( ).insert( canvas );
      }
    };

    void DomainGUI::saveNeuroML( void )
    {
      nslib::congen::DataSaver::saveXmlScene( _mw );
    };

    using NeuronPop = shiftgen::NeuronPop;

    Domain::Domain( void )
    {
      this->_dataLoader = new DataLoader;
      this->_entitiesTypes = new nslib::congen::shiftgen::EntitiesTypes;
      this->_relationshipPropertiesTypes =
        new nslib::congen::shiftgen::RelationshipPropertiesTypes( );
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
        new shift::RelationshipOneToN( "connectedBy" );
    }

    bool Domain::isSelectableEntity( shift::Entity* entity ) const
    {
      return dynamic_cast< NeuronPop* >( entity );
    }

    unsigned int Domain::selectableEntityId( shift::Entity* /* entity */ ) const
    {
      return 0;
    }

    const Vector4f Domain::entity3DPosition ( shift::Entity* /* entity */ ) const
    {
      return Vector4f( 0.f, 0.f, 0.f, 0.f  );
    }

    void Domain::usageMessage( void )
    {
    }



  }
}
