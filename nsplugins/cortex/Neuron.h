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
#ifndef __NSLIB__NEURON__
#define __NSLIB__NEURON__

#include <shift_Neuron.h>
#include <shift_Column.h>
#include <shift_Layer.h>
#include <shift_MiniColumn.h>
#include <shift_NeuronTypeAggregation.h>
#include <nslib/SelectionManager.h>

namespace nslib
{
  namespace cortex
  {

    using Neuron = shiftgen::Neuron;
    using Column = shiftgen::Column;
    using MiniColumn = shiftgen::MiniColumn;
    using Layer = shiftgen::Layer;
    using NeuronTypeAggregation = shiftgen::NeuronTypeAggregation;

  }
} // namespace nslib

#endif // __NSLIB__CORTEX_NEURON__
