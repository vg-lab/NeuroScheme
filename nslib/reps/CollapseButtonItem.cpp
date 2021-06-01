#include "CollapseButtonItem.h"
#include "CollapsableItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QBrush>

// Delete
#include <iostream>

namespace nslib
{
  CollapseButtonItem::CollapseButtonItem( void )
  {
    this->setAcceptHoverEvents( true );
  }

  void CollapseButtonItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
  {
    // Toggle collapse
    if ( event->buttons( ) & Qt::LeftButton )
    {
      CollapsableItem*  pI =
        dynamic_cast< CollapsableItem* >( this->parentItem( ));
      if ( pI )
        pI->toggleCollapse( );
    }
  }

  void CollapseButtonItem::hoverEnterEvent(
    QGraphicsSceneHoverEvent* /* event */ )
  {
    this->setBrush( QBrush( QColor( 120, 85, 100 )));
  }

  void CollapseButtonItem::hoverLeaveEvent(
    QGraphicsSceneHoverEvent* /* event */ )
  {
    this->setBrush( QBrush( QColor( 85, 85, 100 )));
  }

} // namespace nslib
