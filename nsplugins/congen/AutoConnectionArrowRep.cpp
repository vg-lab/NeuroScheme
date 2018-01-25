//
// Created by iagocl on 24/01/18.
//

#include "AutoConnectionArrowRep.h"
#include "ConnectionArrowItem.h"
#include "../../ShiFT/shift/Representation.h"
#include "AutoConnectionArrowItem.h"
#include <iostream>


namespace nslib
{
  namespace congen
  {

    AutoConnectionArrowRep::AutoConnectionArrowRep(
        shift::Representation* rep_)
    {
      //IAGODEBUG
      std::cout << "creada autoconexion" << rep_ << std::endl;

    }

    void AutoConnectionArrowRep::preRender( shift::OpConfig* opConfig_ ){
      std::cout<<opConfig_;
      //TODO
    }

    QGraphicsItem* AutoConnectionArrowRep::item( QGraphicsScene* scene, bool create )
    {
      //IAGODEBUG
      std::cout << "BBBBBBBBBBBBB"<<std::endl;
      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new AutoConnectionArrowItem( *this );
      }
      return _items.at( scene );
    }


  } // namespace congen
} // namespace nslib