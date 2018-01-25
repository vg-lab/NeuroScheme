//
// Created by iagocl on 25/01/18.
//

#ifndef NEUROSCHEME_LINECONNECTIONARROWREP_H
#define NEUROSCHEME_LINECONNECTIONARROWREP_H

#include "ConnectionArrowRep.h"

namespace nslib
{
  namespace congen
  {
    class LineConnectionArrowRep : public nslib::congen::ConnectionArrowRep
    {
      public:
      QGraphicsItem* item( QGraphicsScene* scene = nullptr,
                           bool create = true );
      LineConnectionArrowRep( shift::Representation* originRep_,
        shift::Representation* destRep_ );
      LineConnectionArrowRep( const LineConnectionArrowRep& );
      void preRender( shift::OpConfig* opConfig = nullptr );
    };
  } // namespace congen
} // namespace nslib



#endif //NEUROSCHEME_LINECONNECTIONARROWREP_H
