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
#ifndef __NSLIB__CORTEX_DOMAIN_MANAGER__
#define __NSLIB__CORTEX_DOMAIN_MANAGER__

#include <nslibcortex/api.h>
#include <nslib/DomainManager.h>
#include "DataLoader.h"
namespace nslib
{
  namespace cortex
  {

    class Eigen4VectorCaster : public fires::PropertyCaster
    {
    public:

      virtual ~Eigen4VectorCaster( void )
      {
      }


      int toInt( const fires::Property&, TIntRounding  ) override
      {
        return 0;
      };
      std::string toString( const fires::Property& prop) override
      {
        Eigen::Vector4f vector = prop.value< Eigen::Vector4f >( );

        float array[] = {vector[ 0 ], vector[ 1 ], vector[ 2 ], vector[ 3 ]};
        auto space = std::string( " " );
        return std::to_string( array[ 0 ] ) + space + std::to_string( array[ 1 ] ) + space + std::to_string( array[ 2 ] )
               + space + std::to_string( array[ 3 ] );
      };
      void fromString(
        fires::Property& property, const std::string& value_ ) override
      {
        float x, y, z, w;
        int nItemsRead = sscanf( value_.c_str( ),"%f %f %f %f\n", &x, &y, &z, &w);
        SHIFT_CHECK_THROW( 3 != nItemsRead, "ERROR: Cast to eigen::Vector4f failed" )
        Eigen::Vector4f vector( x, y, z, w);
        property.set( vector );
      };

      std::vector< std::string > categories( void ) override
      {
        return std::vector< std::string >( );
      }
    };

    class DomainGUI : QObject
    {
      Q_OBJECT;

    public:
      DomainGUI( QMainWindow* mw_, QMenuBar* menubar );

    public slots:
      void loadBlueConfig( void );
      void loadXmlScene( void );

    protected:
      QMainWindow* _mw;
      QString _lastOpenedFileName;
      std::unique_ptr< QAction > _actionLoadBlueConfig;
      std::unique_ptr< QAction > _actionLoadXmlScene;
    };

    class NSLIBCORTEX_API Domain
      : public ::nslib::Domain
    {

    public:

      Domain( void );

      virtual ~Domain( void )
      {
        delete this->_dataLoader;
        delete this->_entitiesTypes;
        delete this->_relationshipPropertiesTypes;
      }

      bool isSelectableEntity( shift::Entity* entity ) const override;
      unsigned int selectableEntityId( shift::Entity* entity ) const override;
      const Vector4f entity3DPosition ( shift::Entity* entity ) const override;
      static void usageMessage( void );

      void createGUI( QMainWindow* mw, QMenuBar* menubar ) final
      {
        _domainGUI.reset( new DomainGUI( mw, menubar ));
      }


    protected:
      std::unique_ptr< DomainGUI > _domainGUI;

      void exportRepresentationMaxMin(
        std::ostream& outputStream, bool minimizeStream ) const override;

      void importRelationshipsJSON(
        const boost::property_tree::ptree& relationships,
        std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity ) override;

      void addIsAGroupOfRelationshipsToJSON(
        const boost::property_tree::ptree&  relations,
        std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity );

      void addIsSuperEntityOfRelationshipsToJSON(
        const boost::property_tree::ptree&  relations,
        std::unordered_map < unsigned int, shift::Entity* >* oldGUIToEntity );

      void importMaximumsJSON( const boost::property_tree::ptree& maximums ) override;

    };
  }
}

#endif
