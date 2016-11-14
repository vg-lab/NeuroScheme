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
#ifndef __NEUROSCHEME_SCATTERPLOT_LAYOUT__
#define __NEUROSCHEME_SCATTERPLOT_LAYOUT__

#include "Layout.h"

namespace neuroscheme
{
  class ScatterPlotLayout : public Layout
  {
  public:
    ScatterPlotLayout( void );
    virtual void _arrangeItems( const shift::Representations& /* reps */,
                                bool /* animate */,
                                const shift::Representations&
                                preFilterReps =
                                shift::Representations( )) final;
    Layout* clone( void ) const
    {
      return new ScatterPlotLayout;
    }
  };
}

#endif
