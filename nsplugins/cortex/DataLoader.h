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
#ifndef __NSLIB__CORTEX_DATA_LOADER__
#define __NSLIB__CORTEX_DATA_LOADER__

#include <nslib/Config.h>
#include <nslib/DataLoader.h>
#include <nslib/Log.h>
#include <unordered_map>
#include <string>

#ifdef NEUROSCHEME_USE_NSOL
#include <nsol/nsol.h>
#endif

namespace nslib
{
  namespace cortex
  {
    class DataLoader
      : public ::nslib::DataLoader
    {
    public:

      virtual ~DataLoader( void ) {}

      virtual bool loadData(
        const ::nslib::NeuroSchemeInputArguments& arguments ) final;

#ifdef NEUROSCHEME_USE_NSOL
      void createEntitiesFromNsolColumns(
        const nsol::Columns& columns,
        const nsol::Circuit& circuit,
        bool withMorphologies = true,
        const std::string& csvNeuronStatsFileName = "" );
#endif
    };
  }
}

#endif
