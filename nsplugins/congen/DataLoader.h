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
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef __NSLIB__CONGEN_DATA_LOADER__
#define __NSLIB__CONGEN_DATA_LOADER__

#include <nslib/Config.h>
#include <nslib/DataLoader.h>
#include <nslib/Loggers.h>
#include <unordered_map>
#include <string>
#include <QXmlStreamReader>

#ifdef NEUROSCHEME_USE_NSOL
#include <nsol/nsol.h>
#endif

namespace nslib
{
  namespace congen
  {
    class DataLoader
      : public ::nslib::DataLoader
    {
    public:

      virtual ~DataLoader( void ) {}

      bool cliLoadData(
        const ::nslib::NeuroSchemeInputArguments& arguments ) final;

      static bool loadNeuroML( const std::string& fileName );

    protected:
      static void _loadPopulation(
        QXmlStreamReader& xml,
        std::unordered_map< std::string, unsigned int >& popNameToGid,
        unsigned int& maxNeuronsPerPopulation );

      static void _loadProjection(
        QXmlStreamReader& xml,
        const std::unordered_map< std::string, unsigned int >& popNameToGid,
        float& maxAbsoluteWeight );

      static void _loadInput(
          QXmlStreamReader& xml,
          std::unordered_map< std::string, unsigned int >& popNameToGid );

    };
  }
}

#endif
