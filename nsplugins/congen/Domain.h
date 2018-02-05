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

namespace nslib
{

  namespace congen
  {

    class DomainGUI : QObject
    {
      Q_OBJECT;

    public:
      DomainGUI( QMenuBar* menubar )
      {
        for ( auto action : menubar->actions( ))
        {
          if ( action->menu( ))
          {
            auto menu = dynamic_cast< QMenu* >( action->parent( ));
            if ( action->text( ) == "File" && menu )
            {
              _actionLoadNeuroML.reset( new QAction( "Load NeuroML", menu ));
              menu->addAction( _actionLoadNeuroML.get( ));

              connect( _actionLoadNeuroML.get( ), SIGNAL( triggered( )),
                       this, SLOT( loadNeuroML( )));

            }
          }
        }
    }
    public slots:
      void loadNeuroML( void )
      {
        std::cout << "TODO" << std::endl;
      };
    protected:
      std::unique_ptr< QAction > _actionLoadNeuroML;

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

      bool isSelectableEntity( shift::Entity* entity ) const;
      unsigned int selectableEntityId( shift::Entity* entity ) const;
      const Vector4f entity3DPosition ( shift::Entity* entity ) const;
      static void usageMessage( void );
      void createGUI( QMenuBar* menubar ) final
      {
        _domainGUI.reset( new DomainGUI( menubar ));
      }


    protected:
      std::unique_ptr< DomainGUI > _domainGUI;
    };

  }
}

#endif
