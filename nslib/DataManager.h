/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#ifndef __NSLIB__DATA_MANAGER__
#define __NSLIB__DATA_MANAGER__

#include <nslib/api.h>
#include <shift/shift.h>
#include <QErrorMessage>

#ifdef NEUROSCHEME_USE_NSOL
#include <nsol/nsol.h>
#endif

namespace nslib
{
  class NSLIB_API DataManager
  {
  public:

    static shift::EntitiesWithRelationships& entities( void );
    static shift::Entities& rootEntities( void );
    static shift::Entities& noHierarchyEntities( void );

    static void reset( void );

    static bool loadBlueConfig( const std::string& blueConfig,
      const std::string& targetLabel,  const bool loadMorphologies,
      const std::string& csvNeuronStatsFileName,
      const bool loadConnectivity );

    static bool loadNsolXmlScene( const std::string& xmlSceneFile );

#ifdef NEUROSCHEME_USE_NSOL
    static nsol::DataSet& nsolDataSet( void ) { return _nsolDataSet; }
#endif

  protected:
    static shift::Entities _rootEntities;
    static shift::Entities _noHierarchyEntities;

#ifdef NEUROSCHEME_USE_NSOL
    static nsol::DataSet _nsolDataSet;
#endif
  };
}

#endif
