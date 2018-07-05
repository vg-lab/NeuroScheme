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
#ifndef __NSLIB__CONGEN_DOMAIN_MANAGER__
#define __NSLIB__CONGEN_DOMAIN_MANAGER__

#include <nslibcongen/api.h>
#include <nslib/DomainManager.h>
#include "DataLoader.h"
#include "DataSaver.h"
#include <QObject>
#include <QMainWindow>

namespace nslib
{

  namespace congen
  {

    class DomainGUI : QObject
    {
      Q_OBJECT;

    public:
      DomainGUI( QMainWindow* mw_, QMenuBar* menubar );

    public slots:
      void loadNeuroML( void );
      void saveNeuroML( void );
    protected:
      QMainWindow* _mw;
      QString _lastOpenedFileName;
      std::unique_ptr< QAction > _actionLoadNeuroML;
      std::unique_ptr< QAction > _actionSaveNeuroML;

    };

    class NSLIBCONGEN_API Domain
      : public ::nslib::Domain
    {

    public:

      Domain( void );

      virtual ~Domain( void )
      {
        delete _dataLoader;
        delete this->_entitiesTypes;
        delete this->_relationshipPropertiesTypes;
      }

      bool isSelectableEntity( shift::Entity* entity ) const override ;
      unsigned int selectableEntityId( shift::Entity* entity ) const override;
      const Vector4f entity3DPosition ( shift::Entity* entity ) const override;
      static void usageMessage( void );
      void createGUI( QMainWindow* mw, QMenuBar* menubar ) final
      {
        _domainGUI.reset( new DomainGUI( mw, menubar ));
      }

    protected:
      std::unique_ptr< DomainGUI > _domainGUI;

      void addRelationsOfType( const boost::property_tree::ptree& relations,
        std::string relationName, std::unordered_map
        < unsigned int, shift::Entity* >* oldGUIToEntity ) override;

      virtual void exportRepresentationMaxMin(
        std::ostream& outputStream, bool minimizeStream ) const override;

      void importMaximumsJSON(
        const boost::property_tree::ptree& maximums ) override;
    };

  }
}

#endif
