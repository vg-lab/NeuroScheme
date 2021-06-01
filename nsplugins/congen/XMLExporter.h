/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
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

#ifndef __NSLIB_XML_EXPORTER__
#define __NSLIB_XML_EXPORTER__

#include <QDomDocument>
#include <QFile>

#include <fstream>
#include <iostream>
#include <unordered_map>

#include <shift/Entities.h>

namespace nslib
{
  namespace congen
  {
    class XMLExporter
    {
          QDomDocument  domDoc;

          QDomElement   nodeRoot;
          QDomElement*  nodePopulations;
          QDomElement*  nodeProjections;
          QDomElement*  nodeImpulses;

      public:
          XMLExporter( );
          ~XMLExporter( );

          const QDomDocument & getXMLConGenDomDocument( ) { return domDoc; }

          void loadConGenXML( const QString& pFilePath );

          void exportConGenXML( const std::string& fileName );

          void addPopulation( const QString& xmlns, const QString& name, const QString& cell_type,
              const QString& population_size, const QString& x, const QString& y,
              const QString& z, const QString& width, const QString& height, const QString& depth );

          void addProjection ( const std::unordered_map< std::string, std::string >& params );

          void addInput( const QString& name,
            const bool isRandomStim,
            const QString& delay,const QString& duration,
            const QString& amplitude, const QString& frequency,
            const QString& synaptic_mechanism,
            const shift::Entities& connectedEntities );

      private:
          QDomElement addElement(	QDomDocument& doc,
                                  QDomNode& node,
                                  const QString& tag,
                                  const QString& value = QString());

          void parseConGenXML( );
    };
  }
}
#endif // __NSLIB__CONGEN_XML_EXPORTER__
