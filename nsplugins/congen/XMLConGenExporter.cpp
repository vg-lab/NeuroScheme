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

#include "XMLConGenExporter.h"

namespace nslib
{
  namespace congen
  {
    XMLConGenExporter::XMLConGenExporter( )
    {
      domDoc.clear( );

      nodePopulations =
      nodeProjections =
      nodeImpulses    = nullptr;

      QDomProcessingInstruction instr = domDoc.
          createProcessingInstruction( "xml", "version='1.0' encoding='UTF-8'" );
      domDoc.appendChild( instr );

      //External element
      nodeRoot = addElement( domDoc, domDoc, "neuroml", QString( ) );
      nodeRoot.setAttribute( "xmlns","http://morphml.org/neuroml/schema" );
      nodeRoot.setAttribute( "xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance" );
      nodeRoot.setAttribute( "xmlns:net","http://morphml.org/networkml/schema" );
      nodeRoot.setAttribute( "xmlns:mml","http://morphml.org/morphml/schema" );
      nodeRoot.setAttribute( "xmlns:meta","http://morphml.org/metadata/schema" );
      nodeRoot.setAttribute( "xmlns:bio","http://morphml.org/biophysics/schema" );
      nodeRoot.setAttribute( "xmlns:cml","http://morphml.org/channelml/schema" );
      nodeRoot.setAttribute( "xsi:schemaLocation","http://morphml.org/neuroml/schema http://www.neuroml.org/NeuroMLValidator/NeuroMLFiles/Schemata/v1.8.1/Level3/NeuroML_Level3_v1.8.1.xsd" );
      nodeRoot.setAttribute( "length_units","micrometer" );
    }

    QDomElement	XMLConGenExporter::addElement( QDomDocument& doc,
                                               QDomNode& node,
                                               const QString& tag,
                                               const QString& value)
    {
      QDomElement lQDomElement = doc.createElement( tag );

      node.appendChild( lQDomElement );

      if ( value != "" )
      {
        QDomText txt = doc.createTextNode( value );
        lQDomElement.appendChild( txt );
      }

      return lQDomElement;
    }

    void XMLConGenExporter::addPopulation( const QString& xmlns, const QString& name,
        const QString& cell_type, const QString& population_size, const QString& x,
        const QString& y, const QString& z, const QString& width, const QString& height,
        const QString& depth )
    {
      if ( nodePopulations == nullptr )
      {
         nodePopulations = new QDomElement( );
         *nodePopulations = addElement( domDoc, nodeRoot , "populations" );
         nodePopulations->setAttribute( "xmlns",xmlns );
      }
      QDomElement population = addElement( domDoc, *nodePopulations, "population" );
      population.setAttribute( "name", name );
      population.setAttribute( "cell_type", cell_type );

      QDomElement pop_location = addElement( domDoc, population, "pop_location" );

      QDomElement random_arrangement = addElement( domDoc, pop_location,
                                                   "random_arrangement" );
      random_arrangement.setAttribute( "population_size", population_size );

      QDomElement rectangular_location = addElement( domDoc, random_arrangement,
                                                     "rectangular_location" );

      QDomElement corner = addElement( domDoc, rectangular_location, "corner" );
      corner.setAttribute( "x", x );
      corner.setAttribute( "y", y );
      corner.setAttribute( "z", z );

      QDomElement size = addElement( domDoc, rectangular_location, "size" );
      size.setAttribute( "width", width );
      size.setAttribute( "height", height );
      size.setAttribute( "depth", depth );
    }

    void XMLConGenExporter::addProjection ( const std::unordered_map< std::string, std::string >& params )
    {
      if ( nodeProjections == nullptr )
      {
        nodeProjections = new QDomElement( );
        *nodeProjections = addElement( domDoc, nodeRoot, "projections" );
        nodeProjections->setAttribute( "units",
            QString::fromStdString( params.at( "units" )));
        nodeProjections->setAttribute( "xmlns",
            QString::fromStdString( params.at( "xmlns" )));
      }
      QDomElement projection = addElement( domDoc, *nodeProjections, "projection" );
      projection.setAttribute( "name",QString::fromStdString( params.at( "name" )));
      projection.setAttribute( "source",QString::fromStdString( params.at( "source" )));
      projection.setAttribute( "target",QString::fromStdString( params.at( "target" )));

      QDomElement synapse_props = addElement( domDoc, projection, "synapse_props");
      synapse_props.setAttribute( "synapse_type",
          QString::fromStdString( params.at( "synapse_type" )));
      synapse_props.setAttribute( "threshold",
          QString::fromStdString( params.at( "Threshold" )).replace( ",", "." ));

      QDomElement weight_;
      QDomElement weight_extraParam;
      if ( params.at( "Weight Type" ) == "Fixed" )
      {
        weight_= addElement( domDoc, synapse_props, "weight",
            QString::fromStdString( params.at( "Weight" )).replace( ",", "." ));
      } else if ( params.at( "Weight Type" ) == "Gaussian" )
      {
        weight_= addElement( domDoc, synapse_props, "weight", nullptr );
        weight_extraParam = addElement( domDoc, weight_, "GaussianDistribution", nullptr);
        weight_extraParam.setAttribute( "center",
            QString::fromStdString( params.at( "Weight Gaussian Mean" )).replace( ",","." ));
        weight_extraParam.setAttribute( "deviation",
            QString::fromStdString( params.at( "Weight Gaussian Sigma" )).replace( ",","." ));
      } //else throw "Unknown Weight Type";

      QDomElement internal_delay_;
      QDomElement internal_delay_extraParam;
      if ( params.at( "Delay Type" ) == "Fixed" )
      {
        internal_delay_  = addElement( domDoc, synapse_props, "internal_delay",
            QString::fromStdString( params.at( "Delay" )).replace( ",", "." ));
      } else if ( params.at( "Delay Type" ) == "Gaussian" )
      {
        internal_delay_  = addElement( domDoc, synapse_props, "internal_delay", nullptr );
        internal_delay_extraParam = addElement( domDoc, internal_delay_, "GaussianDistribution", nullptr);
        internal_delay_extraParam.setAttribute( "center",
            QString::fromStdString( params.at( "Delay Gaussian Mean" )).replace( ",", "." ));
        internal_delay_extraParam.setAttribute( "deviation",
            QString::fromStdString( params.at( "Delay Gaussian Sigma" )).replace( ",", "." ));
      } //else throw "Unknown Weight Type";

      QDomElement connectivity_pattern_ = addElement( domDoc, projection, "connectivity_pattern" );
      QDomElement connectivity_pattern_type;
      if  (( params.at( "Connectivity Model" ) == "All-to-all" ) ||
           ( params.at( "Connectivity Model" ) == "One-to-one"))
      {
        connectivity_pattern_type = addElement( domDoc, connectivity_pattern_,
            QString::fromStdString( params.at( "Connectivity Model" )));
      } else if ( params.at( "Connectivity Model" ) == "Random" )
      {
        connectivity_pattern_type = addElement( domDoc, connectivity_pattern_,
            QString::fromStdString( "fixed_probability" ));
        connectivity_pattern_type.setAttribute( "probability",
            QString::fromStdString( params.at( "Random probability" )).replace( ",", "." ));
      } else if ( params.at( "Connectivity Model" ) == "FanOut" )
      {
        connectivity_pattern_type = addElement( domDoc, connectivity_pattern_,
            QString::fromStdString( "per_cell_connection" ));
        connectivity_pattern_type.setAttribute( "num_per_source",
            QString::fromStdString( params.at( "FanOut Outdegree" )).replace( ",", "." ));
        connectivity_pattern_type.setAttribute( "direction",
            QString::fromStdString( "PostToPre" ));
      } else if ( params.at( "Connectivity Model" ) == "FanIn" )
      {
        connectivity_pattern_type = addElement( domDoc, connectivity_pattern_,
            QString::fromStdString( "per_cell_connection" ));
        connectivity_pattern_type.setAttribute( "num_per_source",
            QString::fromStdString( params.at( "FanIn Indegree" )).replace( ",", "." ));
        connectivity_pattern_type.setAttribute( "direction",
            QString::fromStdString( "PreToPost" ));

      } else if ( params.at( "Connectivity Model" ) == "Spatial Gaussian" )
      {
        connectivity_pattern_type = addElement( domDoc, connectivity_pattern_,
            QString::fromStdString( "SpatialGaussian_StillToDefine" ));
        connectivity_pattern_type.setAttribute( "SpatialGaussianProbability_StillToDefine",
            QString::fromStdString( params.at( "Spatial Gaussian Probability" )).replace( ",","." ));
        connectivity_pattern_type.setAttribute( "SpatialGaussianSigma_StillToDefine",
            QString::fromStdString( params.at( "Spatial Gaussian Sigma" )).replace( ",","." ));
      } else throw "Unknown Connectivity Model";
    }


    void  XMLConGenExporter::addInput( const QString& name, const QString& frecuency,
        const QString& population, const QString& site_patterns )
    {
      if ( nodeImpulses == nullptr )
      {
        nodeImpulses = new QDomElement( );
        *nodeImpulses = addElement( domDoc, nodeRoot , "inputs" );
      }
      QDomElement input = addElement( domDoc, *nodeImpulses, "input" );
      input.setAttribute( "name", name );

      QDomElement random_stim = addElement( domDoc, input, "random_stim" );
      random_stim.setAttribute( "frecuency", frecuency );

      QDomElement target = addElement( domDoc, input, "target" );
      target.setAttribute( "population", population );

      QDomElement site_pattern_ = addElement( domDoc, target, "site_pattern" );
      QDomElement site_pattern_type = addElement( domDoc, site_pattern_, site_patterns);
    }

    void XMLConGenExporter::loadConGenXML( const QString& pFilePath )
    {
      domDoc.clear( );

      if ( !pFilePath.isNull( ) )
      {
        QFile f( pFilePath );
        QString errorMsg;
        int errorLine, errorColumn;
        if ( domDoc.setContent( &f, &errorMsg, &errorLine, &errorColumn ))
        {
          parseConGenXML( );
        }
      }
    }

    XMLConGenExporter::~XMLConGenExporter( )
    {
      if ( nodePopulations != nullptr)   delete nodePopulations;
      if ( nodeProjections != nullptr)   delete nodeProjections;
      if ( nodeImpulses != nullptr)      delete nodeImpulses;
    }


    void XMLConGenExporter::exportConGenXML( const std::string& fileName )
    {
      std::string fileContent = domDoc.toString( ).toStdString( );

      std::string fileTXT = fileName;
      fileTXT += ".xml";

      std::ofstream( fileTXT.c_str( ));
      std::ofstream outputFileTXT;
      outputFileTXT.open(fileTXT.c_str( ), std::ios::app);

      outputFileTXT  << fileContent.c_str( ) << std::endl;
      outputFileTXT.close( );
    }

    void XMLConGenExporter::parseConGenXML( )
    {
      throw	"-->>Parse ConGen XML not done yet!";
    }
  }
}
