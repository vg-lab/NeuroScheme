//
// Created by iagocl on 25/01/18.
//
#include "LineConnectionArrowRep.h"
#include "ConnectionArrowItem.h"
#include "LineConnectionArrowItem.h"
#include <nslib/Color.h>
#include <stdint.h>

#include <QVector2D>
#include <nslib/reps/Item.h>
#include <nslib/Loggers.h>


namespace nslib
{
  namespace congen
  {

    LineConnectionArrowRep::LineConnectionArrowRep(
        shift::Representation* originRep_,
        shift::Representation* destRep_ )

    {
      _originRep = originRep_;
      _destRep = destRep_;
      //IAGODEBUG
      std::cout << "Creada Conexion NORMAL" << std::endl;
    }

    LineConnectionArrowRep::LineConnectionArrowRep(
        const LineConnectionArrowRep& other )
    {
      _originRep = other._originRep;
      _destRep = other._destRep;
    }

    void LineConnectionArrowRep::preRender( shift::OpConfig* opConfig_ )
    {
      OpConfig* opConfig = dynamic_cast< OpConfig* >( opConfig_ );
      if ( !opConfig )
        return;

      GraphicsScene* scene = opConfig->scene( );

      auto  arrowItem   = this->item( scene );

      if (opConfig->isAnimating())
      {
        auto originRep =
            dynamic_cast< QGraphicsItemRepresentation* >( _originRep );
        auto destRep =
            dynamic_cast< QGraphicsItemRepresentation* >( _destRep );

        auto originItem = dynamic_cast< Item* >( originRep->item( scene ));
        auto destItem = dynamic_cast< Item* >( destRep->item( scene ));

        if ( originItem == nullptr )
          Loggers::get( )->log( "No successfully dynamic cast on originItem",
                                LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

        if ( destItem == nullptr )
          Loggers::get( )->log( "No successfully dynamic cast on destItem",
                                LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );

        auto originArrowItem = dynamic_cast< ConnectionArrowItem* >( arrowItem );
        auto& lineAnim = originArrowItem->lineAnim( );
        lineAnim.setPropertyName( "line" );
        lineAnim.setTargetObject( originArrowItem );
        lineAnim.setDuration( ANIM_DURATION );


        auto originPosAnimStart = originItem->posAnim( ).startValue( ).toPointF( );
        auto originPosAnimEnd = originItem->posAnim( ).endValue( ).toPointF( );
        auto destPosAnimStart = destItem->posAnim( ).startValue( ).toPointF( );
        auto destPosAnimEnd = destItem->posAnim( ).endValue( ).toPointF( );
        auto originWidth_2 =
            originRep->item( scene )->boundingRect( ).width( ) * 0.5f;
        auto destWidth_2 =
            destRep->item( scene )->boundingRect( ).width( ) * 0.5f;

        const auto& originScaleAnim = originItem->scaleAnim( );
        const auto& destScaleAnim = destItem->scaleAnim( );

        auto normAnimStart =
            QVector2D( destPosAnimStart - originPosAnimStart ).normalized( );
        auto normAnimEnd =
            QVector2D( destPosAnimEnd - originPosAnimEnd ).normalized( );


        auto destIniOri =
            QVector2D( originPosAnimStart ) + originWidth_2 *
                                              originScaleAnim.startValue( ).toDouble( ) * normAnimStart;

        auto destIniDest =
            QVector2D( destPosAnimStart ) - destWidth_2 *
                                            originScaleAnim.startValue( ).toDouble( ) * normAnimStart;

        auto destEndOri =
            QVector2D( originPosAnimEnd ) + originWidth_2 *
                                            originScaleAnim.endValue( ).toDouble( ) * normAnimEnd;

        auto destEndDest =
            QVector2D( destPosAnimEnd ) - destWidth_2 *
                                          destScaleAnim.endValue( ).toDouble( ) * normAnimEnd;


        lineAnim.setStartValue(
            QLineF( QPointF( destIniOri.x( ),destIniOri.y( )),
                    QPointF( destIniDest.x( ), destIniDest.y( ))));
        lineAnim.setEndValue(
            QLineF( QPointF( destEndOri.x( ), destEndOri.y( )),
                    QPointF( destEndDest.x( ), destEndDest.y( ))));

        lineAnim.start( );

      }
      else
      {
        auto originItem = dynamic_cast< QGraphicsItem* > (
            dynamic_cast< QGraphicsItemRepresentation* >(
                _originRep )->item( scene ) );

        auto destItem = dynamic_cast< QGraphicsItem* > (
            dynamic_cast< QGraphicsItemRepresentation* >(
                _destRep )->item( scene ));

        auto destOri = QVector2D( originItem->pos( )) +
                       (( originItem->boundingRect().width( ) * 0.5f * originItem->scale( )) *
                        QVector2D(destItem->pos( ) - originItem->pos( )).normalized( ));

        auto destDest =  QVector2D( destItem->pos( )) -
                         ((destItem->boundingRect().width() * 0.5f * originItem->scale( )) *
                          QVector2D(destItem->pos( ) - originItem->pos( )).normalized());

        dynamic_cast< LineConnectionArrowItem* >( arrowItem )->
            createArrow( QPointF(destOri.x( ), destOri.y( ) ),
                         QPointF(destDest.x( ), destDest.y( ) ));

      }
    }

    QGraphicsItem* LineConnectionArrowRep::item( QGraphicsScene* scene, bool create )
    {
      //IAGODEBUG
      std::cout << "HOLAHOLAHOLAHOLAHOLA"<<std::endl;

      if ( create && ( _items.find( scene ) == _items.end( )) &&
           !_items[ scene ] )
      {
        _items[ scene ] = new LineConnectionArrowItem( *this );
      }
      return _items.at( scene );
    }
  } // namespace congen
} // namespace nslib