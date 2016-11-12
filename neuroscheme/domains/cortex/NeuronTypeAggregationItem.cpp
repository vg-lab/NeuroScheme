#include "NeuronTypeAggregationItem.h"
#include "NeuronItem.h"
#include "../../reps/CollapseButtonItem.h"
#include "../../error.h"
#include <QPen>

namespace neuroscheme
{

  NeuronTypeAggregationItem::NeuronTypeAggregationItem(
    const NeuronTypeAggregationRep& parentRep )
  {
    setAcceptHoverEvents( true );
    setFlags( QGraphicsItem::ItemIsPanel );
    assert( this->isPanel( ));
    this->_parentRep = &( const_cast< NeuronTypeAggregationRep& >( parentRep ));
  }

  class Triangle
    : public QGraphicsPolygonItem
    , public SelectableItem
    , public Item
  {
  public:
    Triangle( QPolygonF polygon_, QGraphicsItem* parent_ )
      : QGraphicsPolygonItem( polygon_, parent_ )
    {
      setAcceptHoverEvents( true );
      setFlags( QGraphicsItem::ItemIsPanel );
    }
    shift::Representation* parentRep( void ) const final
    {
      return dynamic_cast< Item* >(this ->parentItem( ))->parentRep( );
    }

    void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ ) final
    {
      InteractionManager::hoverEnterEvent( this, event_ );
        // dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
        // event_ );
    }

    void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ ) final
    {
      InteractionManager::hoverLeaveEvent( this, event_ );
        // dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
        // event_ );
    }

    void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ ) final
    {
      InteractionManager::contextMenuEvent(
        dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
        event_ );
    }

    void mousePressEvent( QGraphicsSceneMouseEvent* event_ ) final
    {
      InteractionManager::mousePressEvent(
        dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
        event_ );
    }
  };

  class Circle
    : public QGraphicsEllipseItem
    , public SelectableItem
    , public Item

  {
  public:
    Circle( QGraphicsItem* parent_ )
      : QGraphicsEllipseItem( parent_ )
    {
      setAcceptHoverEvents( true );
      setFlags( QGraphicsItem::ItemIsPanel );
    }

    shift::Representation* parentRep( void ) const final
    {
      return dynamic_cast< Item* >(this ->parentItem( ))->parentRep( );
    }

    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverEnterEvent( this, event_ );
      // dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
      // event_ );
    }
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverLeaveEvent( this, event_ );
      // dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
      // event_ );
    }
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      InteractionManager::contextMenuEvent(
        dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
        event_ );
    }

    virtual void mousePressEvent( QGraphicsSceneMouseEvent* event_ )
    {
      InteractionManager::mousePressEvent(
        dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
        event_ );
    }

  };

  void NeuronTypeAggregationItem::create( unsigned int size )
  {
    const auto& symbol =
      this->_parentRep->getProperty( "symbol" ).value<
        NeuronTypeAggregationRep::TSymbol >( );
    QPolygonF triangle;
    QGraphicsEllipseItem* ellipseItem;
    switch ( symbol )
    {
    case NeuronTypeAggregationRep::TRIANGLE:
      std::cout << "NeuronTypeAggregationItem::TRIANGLE" << std::endl;
      triangle.append( QPointF(              0, -int( size ) / 2.8f ));
      triangle.append( QPointF(         size/2,  size / 2 ));
      triangle.append( QPointF( -int( size )/2,  size / 2 ));
      triangle.append( QPointF(              0, -int( size ) / 2.8f ));
      _symbolItem =
      new Triangle( triangle, this );
      break;
    case NeuronTypeAggregationRep::CIRCLE:
      std::cout << "NeuronTypeAggregationItem::CIRCLE" << std::endl;
      ellipseItem = new Circle( this );
      ellipseItem->setRect( -int( size ) / 2.5f , -int( size ) / 2.5f,
                            size, size );
      _symbolItem = ellipseItem;
      break;
    default:
      std::cout << "NeuronTypeAggregationItem::UNDEFINED" << std::endl;
      assert( false );
    }
  }


} // namespace neuroscheme
