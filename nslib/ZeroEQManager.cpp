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
#ifdef NEUROSCHEME_USE_ZEROEQ

#include "Loggers.h"
#include <zeroeq/zeroeq.h>
#include <lexis/lexis.h>
#include <QObject>
#include "PaneManager.h"

#ifdef NEUROSCHEME_USE_GMRVLEX
#include <gmrvlex/gmrvlex.h>
#endif

#include "ZeroEQManager.h"
#include "SelectionManager.h"

namespace nslib
{
  zeroeq::Subscriber* ZeroEQManager::_subscriber = nullptr;
  zeroeq::Publisher* ZeroEQManager::_publisher = nullptr;
  SubscriberTimer* ZeroEQManager::_timer = nullptr;

  SubscriberTimer::SubscriberTimer( void )
  {
    connect( this, SIGNAL( timeout( )),
             this, SLOT( receiveEvents( )));
  }

  void SubscriberTimer::receiveEvents( void )
  {
    auto subscriber = ZeroEQManager::subscriber( );
    if(subscriber)
    {
      while ( subscriber->receive( 0 ));
      this->start( 50 );
    }
  }

  void ZeroEQManager::connect( const std::string& session )
  {
    try
    {
      _subscriber = new zeroeq::Subscriber(
        session.empty() ? zeroeq::DEFAULT_SESSION : session );
      _publisher = new zeroeq::Publisher(
        session.empty() ? zeroeq::DEFAULT_SESSION : session );
      _timer = new SubscriberTimer( );

      // _subscriber.subscribe(
      //   lexis::render::LookOut::ZEROBUF_TYPE_IDENTIFIER(),
      //   [&]( const void* data, const size_t size )
      //   { cameraUpdateCallback( lexis::render::LookOut::create( data, size ));});

      // _subscriber.subscribe(
      //   lexis::data::ToggleIDRequest::ZEROBUF_TYPE_IDENTIFIER(),
      //   [&]( const void* data, const size_t size )
      //   { toggleIdSelectionCallback(
      //       lexis::data::ToggleIDRequest::create( data, size ));});


      // _subscriber.subscribe(
      //   lexis::data::SelectedIDs::ZEROBUF_TYPE_IDENTIFIER(),
      //   [&]( const void* data, const size_t size )
      //   { idSelectionUpdateCallback(
      //       lexis::data::SelectedIDs::create( data, size ));});

      _subscriber->subscribe(
        lexis::data::SelectedIDs::ZEROBUF_TYPE_IDENTIFIER(),
        [&]( const void* data, const size_t size )
        { _selectionUpdateCallback(
            lexis::data::SelectedIDs::create( data, size ));});

      _subscriber->subscribe(
        lexis::render::LookOut::ZEROBUF_TYPE_IDENTIFIER(),
        [&]( const void* data, const size_t size )
        { _modelViewUpdatedCallback(
            lexis::render::LookOut::create( data, size ));});

      _timer->start( 50 );
    }
    catch(const std::exception &e)
    {
      // cleanup
      ZeroEQManager::disconnect();

      throw;
    }
  }

  void ZeroEQManager::disconnect( void )
  {
    if(_timer)
    {
      delete _timer;
      _timer = nullptr;
    }

    if(_subscriber)
    {
      delete _subscriber;
      _subscriber = nullptr;
    }

    if(_publisher)
    {
      delete _publisher;
      _publisher = nullptr;
    }
  }

  void ZeroEQManager::publishSelection(const std::vector< unsigned int >& gids )
  {
  if ( _publisher )
    {
      nslib::Loggers::get( )->log(
        std::string( "NeuroScheme: publishing selection with " +
                     std::to_string( gids.size( )) +
                     std::string( " neurons" )),
        nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );
      _publisher->publish( lexis::data::SelectedIDs( gids ));
    }
  }

  void ZeroEQManager::publishFocusOnSelection(const std::vector< unsigned int >& gids )
  {
    if ( _publisher )
    {
      nslib::Loggers::get( )->log(
        std::string( "NeuroScheme: publishing focus on selection with " +
                     std::to_string( gids.size( )) +
                     std::string( " neurons" )),
        nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );
      _publisher->publish( zeroeq::gmrv::FocusedIDs( gids ));
    }
  }

  void ZeroEQManager::_selectionUpdateCallback (lexis::data::ConstSelectedIDsPtr event )
  {
    std::cout << "Received " << event->getIdsVector( ).size( ) << " GIDS: ";
    std::cout << std::endl;
    SelectionManager::setSelectionFromSelectableEntitiesIds(
      event->getIdsVector( ));
  }

  void ZeroEQManager::_modelViewUpdatedCallback (
    lexis::render::ConstLookOutPtr event )
  {
    PaneManager::setViewMatrix( event->getMatrix( ));
  }

#ifdef NEUROSCHEME_USE_ZEROEQ
  zeroeq::Subscriber* ZeroEQManager::subscriber( void )
  {
    return _subscriber;
  }
#endif

} // namespace nslib

#endif // NEUROSCHEME_USE_ZEROEQ
