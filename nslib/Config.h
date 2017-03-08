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
#ifndef __NSLIB_CONFIG__
#define __NSLIB_CONFIG__
#include <nslib/api.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>

namespace nslib
{

  typedef std::unordered_map< std::string, std::vector< std::string >>
  NeuroSchemeInputArguments;


  class Config
  {
  public:
    // typedef enum
    // {
    //   CLI_NODATA,
    //   CLI_SWCLIST,
    //   CLI_BLUECONFIG,
    //   CLI_XMLSCENE
    // } TCliDataSource;
    NSLIB_API
    static NeuroSchemeInputArguments& inputArgs( void );
    // static TCliDataSource cliDataSource;
    // static std::string cliInputFile;
    // static std::string targetLabel;
    // static bool loadMorphologies;
    // static std::string csvNeuronStatsFileName;
    // static std::string zeroeqSession;
    // static std::string deflectHost;
    // static bool deflectExitOnClose;
    // static bool autoPublishSelection;
  protected:
    static NeuroSchemeInputArguments _inputArgs;
  };

}

#endif // __NSLIB_CONFIG__
