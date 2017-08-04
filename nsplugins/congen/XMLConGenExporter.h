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


#ifndef XMLCONGENEXPORTER_H
#define XMLCONGENEXPORTER_H

#include <QDomDocument>
#include <QFile>

#include <fstream>
#include <iostream>
#include <unordered_map>

class XMLConGenExporter
{
      QDomDocument  domDoc;

      QDomElement   nodeRoot;
      QDomElement*  nodePopulations;
      QDomElement*  nodeProjections;
      QDomElement*  nodeImpulses;

  public:
      XMLConGenExporter();

      ~XMLConGenExporter();

      const QDomDocument & getXMLConGenDomDocument() { return domDoc; }

      void loadConGenXML(QString pFilePath);

      void exportConGenXML(std::string fileName);

      void addPopulation(QString xmlns, QString name, QString cell_type, QString population_size,
                         QString x, QString y, QString z, QString width, QString height,
                         QString depth);

      void addProjection (std::unordered_map<std::string, std::string> params);

      void addInput(QString name, QString frecuency, QString population, QString site_patterns);

  private:

      QDomElement     addElement(	QDomDocument& doc,
                                  QDomNode& node,
                                  const QString& tag,
                                  const QString& value = nullptr);

      void parseConGenXML();
};

#endif // XMLCONGENEXPORTER_H
