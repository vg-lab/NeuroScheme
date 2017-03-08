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
#include "Config.h"

namespace nslib
{

  NeuroSchemeInputArguments& Config::inputArgs( void )
  {
    return _inputArgs;
  }
  // Config::TCliDataSource Config::cliDataSource = CLI_NODATA;
  // std::string Config::cliInputFile = std::string( );
  // std::string Config::targetLabel = std::string( );
  // bool Config::loadMorphologies = true;
  // std::string Config::csvNeuronStatsFileName = std::string( );
  // std::string Config::zeroeqSession = std::string( );
  // std::string Config::deflectHost = std::string( );
  // bool Config::deflectExitOnClose = true;
  // bool Config::autoPublishSelection = true;
  NeuroSchemeInputArguments Config::_inputArgs = NeuroSchemeInputArguments( );

}
