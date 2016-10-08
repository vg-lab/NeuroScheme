#include "error.h"
#include "InteractionManager.h"
#include "LayoutManager.h"
#include "SelectionManager.h"
#include "RepresentationCreatorManager.h"
#include "reps/Item.h"
#include "reps/QGraphicsItemRepresentation.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsView>

namespace neuroscheme
{
  QGraphicsScene* LayoutManager::_scene = nullptr;
  // shift::Representations LayoutManager::_representations =
  //   shift::Representations( );

  std::map< QGraphicsScene*, shift::Representations >
  LayoutManager::_representations =
    std::map< QGraphicsScene*, shift::Representations >( );

  std::set< QGraphicsScene* > LayoutManager::_scenes =
    std::set< QGraphicsScene* >( );

  void LayoutManager::displayItems(
    const shift::Representations& reps,
    bool clearFirst )
  {
    if ( !_scene ) return;
    auto& scene = *_scene;

    _representations[ _scene ] = reps;
    if ( reps.empty( ))
      return;

    if ( clearFirst )
    {
      // Remove top items without dostroying them
      QList< QGraphicsItem* > items_ = scene.items( );
      for ( auto item = items_.begin( ); item != items_.end( ); ++item )
      {
        auto item_ = dynamic_cast< Item* >( *item );
        if ( item_ && item_->parentRep( ))
          scene.removeItem( *item );
      }

      // Remove the rest
      scene.clear( );
    }


    NEUROSCHEME_DEBUG_CHECK( scene.views( ).size( ) != 0,
                             "Scene with no view" );
    QGraphicsView* gv = scene.views( )[0];

#define SHOW_SCENE_CORNERS
#ifdef SHOW_SCENE_CORNERS
    {
      // std::cout << gv->width( ) << " x " << gv->height( ) << std::endl;
      QGraphicsEllipseItem* center =
        new QGraphicsEllipseItem( -10, -10, 20, 20 );
      scene.addItem( center );
      QGraphicsEllipseItem* tl =
        new QGraphicsEllipseItem( -gv->width( ) / 2 - 10,
                                  -gv->height( ) / 2 - 10,
                                  20, 20 );

      scene.addItem( tl );
      QGraphicsEllipseItem* br =
        new QGraphicsEllipseItem( gv->width( ) / 2 - 10,
                                  gv->height( ) / 2 - 10,
                                  20, 20 );

      scene.addItem( br );
    }
#endif

    unsigned int maxItemWidth = 0, maxItemHeight = 0;

    for ( const auto representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        std::cerr << "Item null" << std::endl;
      }
      else
      {
        auto item = graphicsItemRep->item( &scene );
        // std::cout << "new item " << item << std::endl;

        // Find out if its entity is selected
        // and if so set its pen
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        const auto entities = repsToEntities.at( representation );
        if ( entities.size( ) < 1 )
          Log::log( NS_LOG_HEADER +
                    "No entities associated to representation",
                    LOG_LEVEL_ERROR );

        auto selectableItem = dynamic_cast< SelectableItem* >( item );
        if ( selectableItem )
        {
          auto selectedState = SelectionManager::getSelectedState(
            *entities.begin( ));

            // if ( selectedState == selectedStateSelectedState::SELECTED )
              selectableItem->setSelected( selectedState );


              auto shapeItem =
                dynamic_cast< QAbstractGraphicsShapeItem* >( item );
              if ( shapeItem )
              {
                if ( selectedState == SelectedState::SELECTED )
                  shapeItem->setPen( InteractionManager::getSelectedPen( ));
                else if ( selectedState == SelectedState::PARTIALLY_SELECTED )
                shapeItem->setPen(
                  InteractionManager::getPartiallySelectedPen( ));

              }
        }
        std::cout << &scene << " add item " << std::endl;
        scene.addItem( item );
        QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );

        if ( rect.width( ) > maxItemWidth )
          maxItemWidth = rect.width( );

        if ( rect.height( ) > maxItemHeight )
          maxItemHeight = rect.height( );
      }
    }

    bool forceScale = false;
    float forcedScale = 1.0f;
    const unsigned int marginX = 20;
    const unsigned int marginY = 20;
    const float scalePaddingX = 1.1f;
    const float scalePaddingY = 1.1f;
    const unsigned int fixedPaddingX = 0;
    const unsigned int fixedPaddingY = 0;

    unsigned int deltaX = scalePaddingX * maxItemWidth + fixedPaddingX;
    unsigned int deltaY = scalePaddingY * maxItemHeight + fixedPaddingY;

    // std::cout << "maxItemWidth = " << maxItemWidth << std::endl;
    // std::cout << "maxItemHeight = " << maxItemHeight << std::endl;
    // std::cout << "deltaX = " << deltaX << std::endl;
    // std::cout << "deltaY = " << deltaY << std::endl;
    int _x = 0;
    int _y = 0;

    float iconAspectRatio = float( maxItemWidth ) / float( maxItemHeight);
    float canvasAspectRatio;

    if ( gv->width( ) > gv->height( ))
      canvasAspectRatio = float( gv->width( )) / float( gv->height( ));
    else
      canvasAspectRatio = float( gv->height( )) / float( gv->width( ));


    unsigned int numRows =
      floor( sqrt( iconAspectRatio * float( reps.size( )) /
                   canvasAspectRatio ));

    if ( numRows < 1 && reps.size( ) > 0 )
      numRows = 1;

        unsigned int numColumns =
          ceil( float( reps.size( )) / float( numRows ));

    if ( numColumns < 1 && reps.size( ) > 0 )
      numColumns = 1;

    if ( gv->width( ) < gv->height( ) )
      std::swap( numColumns, numRows );

    // std::cout << "Num rows: " << numRows << " Num cols: " << numColumns << std::endl;

    float scale;
    float scaleX = float( gv->width( ) - 2 * marginX ) /
      float( numColumns * deltaX );
    float scaleY = float( gv->height( ) - 2 * marginY ) /
      float( numRows * deltaY );
    scale = std::min( scaleX, scaleY );

    // std::cout << "Scale: " << scaleX << " " << scaleY << " " << scale << std::endl;

    int leftMargin = (( deltaX * scale )  +
                      ( gv->width( ) - numColumns * deltaX * scale )) / 2;
    int topMargin = (( deltaY * scale ) +
                     ( gv->height( ) - numRows * deltaY * scale )) / 2;

    for ( const auto representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        std::cerr << "Item null" << std::endl;
      }
      else
      {
        auto item = graphicsItemRep->item( &scene );
        if ( item )
        {
          QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );
          qreal posX = _x * deltaX * scale - gv->width( ) / 2 +
            leftMargin - scale * rect.center( ).x( );
          qreal posY = _y * deltaY * scale - gv->height( ) / 2 +
            topMargin - scale * rect.center( ).y( );
          qreal scale_ = forceScale ? forcedScale : scale;

          item->setPos( posX, posY );
          item->setScale( scale_ );
          // std::cout << posX << " " << posY << " " << scale_ << std::endl;
        }
      }

      _x++;
      if (((unsigned int ) _x ) >= numColumns )
      {
        _x = 0;
        _y++;
      }

    }

    // for ( auto rootItem = rootItems.begin( );
    //       rootItem != rootItems.end( ); ++rootItem )
    // {
    //   QGraphicsItem* item = dynamic_cast< QGraphicsItem* >( *rootItem );
    //   if ( item )
    //   {
    //     _scene->addItem( item );

    //     QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );

    //     if ( rect.width( ) > maxItemWidth )
    //       maxItemWidth = rect.width( );

    //     if ( rect.height( ) > maxItemHeight )
    //       maxItemHeight = rect.height( );


    //   }
    // }




    // unsigned int x = 0;
    // for ( const auto representation : reps )
    // {
    //   auto graphicsItemRep =
    //     dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
    //       representation );
    //   if ( !graphicsItemRep )
    //   {
    //     std::cerr << "Item null" << std::endl;
    //   }
    //   else
    //   {
    //     scene.addItem( graphicsItemRep->item( ));
    //     if ( graphicsItemRep->item( ))
    //     {
    //       graphicsItemRep->item( )->setPos( x, 0 ); x+= 120;
    //       // graphicsItemRep->item( )->setScale( 10 );
    //     }
    //   }
    // }

  } // displayItems

  void LayoutManager::updateSelection( void )
  {
    QList< QGraphicsItem* > items_ = _scene->items( );
    for ( auto qitem = items_.begin( ); qitem != items_.end( ); ++qitem )
    {
      auto selectableItem_ = dynamic_cast< SelectableItem* >( *qitem );
      if ( selectableItem_ )
      {
        auto item = dynamic_cast< Item* >( *qitem );
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );

        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          // Check only the first entity, this might not be ok
          // TODO check this
          if ( entities.size( ) > 0 )
          {
            const auto& entity = *entities.begin( );
            auto shapeItem =
              dynamic_cast< QAbstractGraphicsShapeItem* >( item );

            const auto state = SelectionManager::getSelectedState( entity );
            if ( state == SelectedState::UNSELECTED )
              shapeItem->setPen( InteractionManager::getUnselectedPen( ));
            else if ( state == SelectedState::SELECTED )
              shapeItem->setPen( InteractionManager::getSelectedPen( ));
            else if ( state == SelectedState::PARTIALLY_SELECTED )
              shapeItem->setPen(
                InteractionManager::getPartiallySelectedPen( ));

          }
        }
      }
    }
  }

} // namespace neuroscheme
