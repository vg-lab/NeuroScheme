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
#ifndef __NSLIB_ZEROEQ_MANAGER__
#define __NSLIB_ZEROEQ_MANAGER__

#include <nslib/api.h>
#include <shift/shift.h>
#include "reps/SelectableItem.h"
#include <QObject>
#include <QTimer>

#ifndef NEUROSCHEME_USE_ZEROEQ
#define NEUROSCHEME_USE_ZEROEQ_IMPL { }
#else
#define NEUROSCHEME_USE_ZEROEQ_IMPL ;
#endif

#ifndef NEUROSCHEME_USE_GMRVLEX
#define NEUROSCHEME_USE_GMRVLEX_IMPL { }
#else
#define NEUROSCHEME_USE_GMRVLEX_IMPL ;
#endif

#ifdef NEUROSCHEME_USE_ZEROEQ
#include <zeroeq/zeroeq.h>
#include <lexis/lexis.h>

#ifdef NEUROSCHEME_USE_GMRVLEX
#include <gmrvlex/gmrvlex.h>
#endif
#endif

namespace nslib
{
  class NSLIB_API SubscriberTimer : public QTimer
  {
    Q_OBJECT;
  public:
    SubscriberTimer( void ) NEUROSCHEME_USE_ZEROEQ_IMPL;

  public slots:
    void receiveEvents( void ) NEUROSCHEME_USE_ZEROEQ_IMPL;
  };

  class NSLIB_API ZeroEQManager
  {
  public:

    static void connect( const std::string& /*session*/ )
    NEUROSCHEME_USE_ZEROEQ_IMPL;
    static void disconnect( void )
    NEUROSCHEME_USE_ZEROEQ_IMPL;
    ~ZeroEQManager( void ) NEUROSCHEME_USE_ZEROEQ_IMPL;
    static void receiveEvents( void ) NEUROSCHEME_USE_ZEROEQ_IMPL;
    static void publishSelection(
      const std::vector< unsigned int >& ) NEUROSCHEME_USE_GMRVLEX_IMPL;
    static void publishFocusOnSelection(
      const std::vector< unsigned int >& ) NEUROSCHEME_USE_GMRVLEX_IMPL;

#ifdef NEUROSCHEME_USE_ZEROEQ
    static zeroeq::Subscriber* subscriber( void );
#endif
  protected:

#ifdef NEUROSCHEME_USE_LEXIS
    static void _selectionUpdateCallback ( lexis::data::ConstSelectedIDsPtr )
      NEUROSCHEME_USE_ZEROEQ_IMPL;
    static void _modelViewUpdatedCallback ( lexis::render::ConstLookOutPtr )
      NEUROSCHEME_USE_ZEROEQ_IMPL;
#endif

#ifdef NEUROSCHEME_USE_ZEROEQ
    static zeroeq::Subscriber *_subscriber;
    static zeroeq::Publisher *_publisher;
#endif

    static SubscriberTimer* _timer;

  };
} // namespace nslib

#endif // __NSLIB_ZEROEQ_MANAGER__
