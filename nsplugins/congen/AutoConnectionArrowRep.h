//
// Created by iagocl on 24/01/18.
//

#ifndef NEUROSCHEME_AUTOCONNECTIONARROWREP_H
#define NEUROSCHEME_AUTOCONNECTIONARROWREP_H

#include "ConnectionArrowRep.h"


namespace nslib
{
  namespace congen
  {
    class AutoConnectionArrowRep : public nslib::congen::ConnectionArrowRep
    {
      public :
      QGraphicsItem* item( QGraphicsScene* scene = nullptr,
                           bool create = true );
      AutoConnectionArrowRep( const AutoConnectionArrowRep& );
      AutoConnectionArrowRep( shift::Representation* rep);
      void preRender( shift::OpConfig* opConfig = nullptr );
    };
  } // namespace congen
} // namespace nslib


#endif //NEUROSCHEME_AUTOCONNECTIONARROWREP_H
