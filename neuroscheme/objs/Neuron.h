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
#ifndef __NEUROSCHEME__NEURON__
#define __NEUROSCHEME__NEURON__

#include <shift/shift.h>
#include "../Color.h"

namespace neuroscheme
{

  class Neuron : public shift::Object
  {
  public:
    typedef enum
    {
      UNDEFINED_MORPHOLOGICAL_TYPE = 0,
      INTERNEURON,
      PYRAMIDAL
    } TMorphologicalType;

    typedef enum
    {
      UNDEFINED_FUNCTIONAL_TYPE = 0,
      INHIBITORY,
      EXCITATORY
    } TFunctionalType;

    Neuron( TMorphologicalType morphoType =  UNDEFINED_MORPHOLOGICAL_TYPE,
            TFunctionalType funcType = UNDEFINED_FUNCTIONAL_TYPE,
            float somaVolume = 0.0f,
            float somaArea = 0.0f,
            float dendVolume = 0.0f,
            float dendArea = 0.0f );

    Neuron( const Neuron& other );
  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__NEURON__
