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

#include "DataManager.h"
#include "DomainManager.h"
#include "InteractionManager.h"
#include "layouts/FreeLayout.h"
#include "Loggers.h"
#include "RepresentationCreatorManager.h"
#include "reps/Item.h"
#include "reps/ConnectivityRep.h"
#include "SelectionManager.h"
#include "ZeroEQManager.h"
#include <shift/Entity.h>
#include <shift/Entities.h>
#include <unordered_set>

#include <QGuiApplication>

namespace nslib
{
  QMenu* InteractionManager::_contextMenu = nullptr;
  FreeLayout* InteractionManager::_movingLayout = nullptr;
  EntityEditWidget* InteractionManager::_entityEditWidget = nullptr;
  ConnectionRelationshipEditWidget*
    InteractionManager::_conRelationshipEditWidget = nullptr;
  QGraphicsItem* InteractionManager::_item = nullptr;
  Qt::MouseButtons InteractionManager::_buttons = Qt::MouseButtons( );
  std::unique_ptr< TemporalConnectionLine >
    InteractionManager::_tmpConnectionLine =
    std::unique_ptr< TemporalConnectionLine >( nullptr );
  QAbstractGraphicsShapeItem*
    InteractionManager::lastShapeItemHoveredOnMouseMove = nullptr;
  EntityConnectionListWidget* InteractionManager::_entityConnectListWidget =
      nullptr;
  QStatusBar* InteractionManager::_statusBar = nullptr;

  void InteractionManager::start( )
  {
    _entityEditWidget = new EntityEditWidget( );
    _entityConnectListWidget = new EntityConnectionListWidget( );
    _conRelationshipEditWidget = new ConnectionRelationshipEditWidget( );
    _contextMenu = new QMenu( );
  }

  void InteractionManager::highlightConnectivity(
    QAbstractGraphicsShapeItem* shapeItem, bool highlight )
  {
    auto& relConnectsTo = *( DataManager::entities( ).
      relationships( )[ "connectsTo" ]->asOneToN( ));
    auto& relConnectedBy = *( DataManager::entities( ).
      relationships( )[ "connectedBy" ]->asOneToN( ));
    auto& relAgrConnectedBy = *( DataManager::entities( ).
      relationships( )[ "aggregatedConnectedBy" ]->asAggregatedOneToN( ));
    auto& relAgrConnectsTo = *( DataManager::entities( ).
      relationships( )[ "aggregatedConnectsTo" ]->asAggregatedOneToN( ));

    std::vector< std::tuple< shift::Relationship*, scoop::Color, bool, bool >>
      rels;

    rels.reserve( 4 );
    rels.push_back( std::make_tuple(
      &relConnectsTo, scoop::Color( 0, 204, 255 ), false, false ));
    rels.push_back( std::make_tuple(
      &relConnectedBy, scoop::Color( 255, 204, 0 ), true, false ));
    rels.push_back( std::make_tuple(
      &relAgrConnectedBy, scoop::Color( 255, 204, 0 ), true, true ));
    rels.push_back( std::make_tuple(
      &relAgrConnectsTo, scoop::Color( 0, 204, 255 ), false, true ));

    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );
    const auto& relatedEntities =
      RepresentationCreatorManager::relatedEntities( );

    auto item = dynamic_cast< Item* >( shapeItem );
    if ( item )
    {
      if( item->parentRep( ))
      {
        const auto& entities = repsToEntities.find( item->parentRep( ));
        if ( entities != repsToEntities.end( ))
        {
          auto entityGid = ( *entities->second.begin( ))->entityGid( );
          for ( const auto& relPair : rels )
          {
            std::vector< unsigned  int > connectingEntitiesGID;
            if ( std::get< HiglightRelationPair::HLC_AGGREGATED >( relPair ))
            {
              const auto& rel = std::get< HiglightRelationPair::HLC_RELATIONSHIP >
                ( relPair )->asAggregatedOneToN( )->mapAggregatedRels( );

              auto connectingEntities = rel.find( entityGid );
              if ( connectingEntities != rel.end( ))
              {
                for( const auto& connectingEntity : *connectingEntities->second )
                {
                  highlightConnection( highlight, relatedEntities,
                    entityGid, relPair, connectingEntity.first );}
              }
            }
            else
            {
              const auto rel = std::get< HiglightRelationPair::HLC_RELATIONSHIP >
                ( relPair )->asOneToN( );

              auto connectingEntities = rel->find( entityGid );
              if ( connectingEntities != rel->end( ))
              {
                for ( const auto& connectingEntity : connectingEntities->second )
                {
                  highlightConnection( highlight, relatedEntities,
                    entityGid, relPair, connectingEntity.first );
                }
              }
            }
          }
        }
      }
    }
  }

  void InteractionManager::highlightConnection( const bool highlight,
    const shift::TRelatedEntitiesReps& relatedEntities,
    const unsigned int& entityGid,
    const std::tuple<shift::Relationship*, scoop::Color, bool, bool>& relPair,
    const unsigned int connectingEntityGID )
  {
    const auto& relationRep = relatedEntities.find(
      ( std::get< HiglightRelationPair::HLC_INVERT >( relPair ) ?
      std::make_pair( connectingEntityGID, entityGid ) :
      std::make_pair( entityGid, connectingEntityGID )));

    if (relationRep != relatedEntities.end())
    {
      shift::Representation *rep = std::get<0>(relationRep->second);
      auto *connRep = dynamic_cast<ConnectivityRep*>(rep);
      if (connRep)
      {
        if (highlight)
        {
          connRep->highlight(
              std::get<HiglightRelationPair::HLC_COLOR>(relPair));
        }
        else
        {
          connRep->unHighlight();
        }
      }
    }
  }

  void InteractionManager::hoverEnterEvent(
    QAbstractGraphicsShapeItem* shapeItem,
    QGraphicsSceneHoverEvent* event )
  {
    if( lastShapeItemHoveredOnMouseMove != shapeItem )
    {
      hoverLeaveEvent( lastShapeItemHoveredOnMouseMove, event );

      auto selectableItem = dynamic_cast< SelectableItem* >( shapeItem );
      if ( selectableItem )
      {
        selectableItem->hover( true );
        selectableItem->setSelected( selectableItem->selectedState( ));
      }
      else
      {
        shapeItem->setPen( SelectableItem::hoverUnselectedPen( ));
      }

      if ( event && event->modifiers( ).testFlag( Qt::ControlModifier ))
      {
        auto item = dynamic_cast< Item* >( shapeItem );
        if ( item )
        {
          assert( item->parentRep( ));
          const auto& repsToEntities = RepresentationCreatorManager::repsToEntities( );

          if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
          {
            const auto entities = repsToEntities.at( item->parentRep( ));
            createOrEditEntity( *entities.begin( ),
              EntityEditWidget::TEntityEditWidgetAction::EDIT_ENTITY );
          }
        }
      }
      lastShapeItemHoveredOnMouseMove = shapeItem;
    }
  }

  void InteractionManager::hoverLeaveEvent(
    QAbstractGraphicsShapeItem* item,
    QGraphicsSceneHoverEvent* /*event*/ )
  {
    if ( lastShapeItemHoveredOnMouseMove &&
      item == lastShapeItemHoveredOnMouseMove )
    {
      for( const auto& pane : PaneManager::panes( ))
      {
        if( pane->scene( ).items( ).contains( item ))
        {
          auto selectableItem = dynamic_cast< SelectableItem* >( item );
          if ( selectableItem )
          {
            selectableItem->hover( false );
            selectableItem->setSelected( selectableItem->selectedState( ) );
          }
          else
          {
            item->setPen( SelectableItem::unselectedPen( ));
          }

          break;
        }
      }

      lastShapeItemHoveredOnMouseMove = nullptr;
    }
  }

  void InteractionManager::contextMenuEvent(
    QAbstractGraphicsShapeItem* shapeItem,
    QGraphicsSceneContextMenuEvent* event )
  {
    _contextMenu->clear( );

    auto dataEntities = DataManager::entities( );
    auto dataRelations = dataEntities.relationships( );

    // If clicking outside item, new item menu is showed
    if ( !shapeItem )
    {
      auto domain = DomainManager::getActiveDomain( );
      if ( domain )
      {
        const shift::EntitiesTypes domainEntitiesTypes =
          domain->entitiesTypes( );

        //Detect scene parent
        auto& relChildOf = *( dataRelations[ "isChildOf" ]->asOneToOne( ));
        const auto& sceneEntities =
          PaneManager::activePane( )->sceneEntities( ).vector( );
        int commonParent = -1;
        if ( !sceneEntities.empty( ))
        {
          commonParent = ( int ) relChildOf[ sceneEntities.front( )
            ->entityGid( ) ].entity;
          for ( auto sceneEntity : sceneEntities )
          {
            if ( commonParent !=
                 ( int ) relChildOf[ sceneEntity->entityGid( ) ].entity )
            {
              commonParent = -1;
              break;
            }
          }
        }
        auto menuEntitiesTypes = new std::vector< std::string >;
        auto isParentEntity = new std::vector< bool >;
        std::unordered_map< QAction*, unsigned int > actionToIdx;
        shift::Entity* parentEntity = nullptr;
        unsigned int entityIdx = addCreateEntitiesContextMenu(
          commonParent, domainEntitiesTypes, dataEntities, parentEntity,
          actionToIdx, menuEntitiesTypes, false, isParentEntity );
        const bool notHierarchyInDomain =
          !domainEntitiesTypes.notHierarchyTypes( ).empty( );
        if ( entityIdx != 0 && notHierarchyInDomain )
        {
          _contextMenu->addSeparator( );
        }
        for ( const auto& type : domainEntitiesTypes.notHierarchyTypes( ))
        {
          if ( !std::get< shift::EntitiesTypes::IS_SUBENTITY >( type ))
          {
            std::string childType =
              std::get< shift::EntitiesTypes::ENTITY_NAME >( type );
            QAction* action = _contextMenu->addAction(
              QString( "Add " ) + QString::fromStdString( childType ));
            menuEntitiesTypes->push_back( childType );
            actionToIdx[ action ] = entityIdx++;
          }
        }
        QAction* selectedAction =_contextMenu->exec( event->screenPos( ));
        if ( selectedAction )
        {
          createOrEditEntity(  domainEntitiesTypes.getEntityObject(
            menuEntitiesTypes->at( actionToIdx[ selectedAction ] )),
            EntityEditWidget::TEntityEditWidgetAction::NEW_ENTITY,
            parentEntity, false );
        }
      } // if ( domain )
    }
    else
    {
      auto item = dynamic_cast< Item* >( shapeItem );
      if ( item )
      {
        assert( item->parentRep( ));
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        if ( repsToEntities.find( item->parentRep( )) != repsToEntities.end( ))
        {
          const auto entities = repsToEntities.at( item->parentRep( ));
          auto entity = *entities.begin( );
          auto entityGid = entity->entityGid( );

          auto& relParentOf = *( dataRelations[ "isParentOf" ]->asOneToN( ));
          const auto& children = relParentOf[ entityGid ];

          auto& relChildOf = *( dataRelations[ "isChildOf" ]->asOneToOne( ));
          const auto& parent = relChildOf[ entityGid ].entity;

          const auto& grandParent =
            relChildOf[ parent ].entity;

          auto& relAGroupOf = *( dataRelations[ "isAGroupOf" ]->asOneToN( ));
          const auto& groupedEntities = relAGroupOf[ entityGid ];

          QAction* editEntity = nullptr;
          QAction* deleteEntity = nullptr;
          QAction* dupEntity = nullptr;
          QAction* autoEntity = nullptr;
          if ( !entity->isSubEntity( ))
          {
            editEntity = _contextMenu->addAction( "Edit" );
            deleteEntity = _contextMenu->addAction( "Delete" );
            dupEntity = _contextMenu->addAction( "Duplicate" );
            if ( shift::RelationshipPropertiesTypes::isConstrained(
              "ConnectedTo", entity->typeName( ),entity->typeName( )))
            {
              autoEntity = _contextMenu->addAction( "Add Auto Connection" );
            }
          }
          if ( editEntity || dupEntity || autoEntity || deleteEntity )
          {
            _contextMenu->addSeparator( );
          }

          bool childAction = false;

          std::unordered_map< QAction*, unsigned int > actionToIdx;
          auto domain = DomainManager::getActiveDomain( );
          shift::EntitiesTypes domainEntitiesTypes;
          auto newEntitiesTypes = new std::vector< std::string >;
          auto isParentEntity = new std::vector< bool >;
          if ( domain )
          {
            domainEntitiesTypes = domain->entitiesTypes( );
            shift::Entity* parentEntity = nullptr;
            int commonParent = static_cast<int>(entityGid);
            childAction = addCreateEntitiesContextMenu( commonParent,
              domainEntitiesTypes, dataEntities, parentEntity,
              actionToIdx, newEntitiesTypes, true, isParentEntity ) != 0;
            if ( childAction )
            {
              _contextMenu->addSeparator( );
            }
          }

          auto relConnectsTo = *( dataRelations[ "connectsTo" ]->asOneToN( ));
          auto relConnectedBy = *( dataRelations[ "connectedBy" ]->asOneToN( ));
          auto relAggregatedConnectsTo = dataRelations[ "aggregatedConnectsTo" ]
            ->asAggregatedOneToN( )->mapAggregatedRels( );
          auto relAggregatedConnectBy = dataRelations[ "aggregatedConnectedBy" ]
            ->asAggregatedOneToN( )->mapAggregatedRels( );

         auto connectsToIt = relConnectsTo.find( entityGid );
         shift::RelationshipOneToNMapDest* connectsToMap =
           ( connectsToIt == relConnectsTo.end( )
           || connectsToIt->second.empty( ))
           ? nullptr : & connectsToIt->second;

          auto connectedByIt = relConnectedBy.find( entityGid );
          shift::RelationshipOneToNMapDest* connectedByMap =
            ( connectedByIt == relConnectedBy.end( )
            || connectedByIt->second.empty( ))
            ? nullptr : & connectedByIt->second;

          auto aggregatedConnectsToIt =
            relAggregatedConnectsTo.find( entityGid );
          shift::AggregatedOneToNAggregatedDests* aggregatedConnectsToMap =
            ( aggregatedConnectsToIt == relAggregatedConnectsTo.end( )
            || aggregatedConnectsToIt->second->empty( ))
            ? nullptr : aggregatedConnectsToIt->second.get( );

          auto aggregatedConnectedByIt =
            relAggregatedConnectBy.find( entityGid );
          shift::AggregatedOneToNAggregatedDests* aggregatedConnectedByMap =
            ( aggregatedConnectedByIt == relAggregatedConnectBy.end( )
            || aggregatedConnectedByIt->second->empty( ))
            ? nullptr : aggregatedConnectedByIt->second.get( );

          QAction* showConnections = nullptr;
          if( connectsToMap || connectedByMap || aggregatedConnectsToMap
            || aggregatedConnectedByMap )
          {
            showConnections = _contextMenu->addAction( "Show connections" );
            _contextMenu->addSeparator( );
          }

          const bool hasChildren = !children.empty( );
          const bool grouped = !groupedEntities.empty( );

          const QAction* levelUp = ( parent != 0 )
            ? _contextMenu->addAction( "Level up" ) : nullptr;
          const QAction* levelDown = ( hasChildren )
            ? _contextMenu->addAction( "Level down" ) : nullptr;
          const QAction* expandGroup = ( grouped )
            ? _contextMenu->addAction( "Expand group" ) : nullptr;
          const QAction* expandChildren = ( hasChildren )
            ? _contextMenu->addAction( "Expand children" ) : nullptr;

          if ( levelUp || levelDown || expandGroup || expandChildren )
          {
            _contextMenu->addSeparator( );
          }

          const QAction* levelUpToNewPane = ( parent != 0 )
            ? _contextMenu->addAction( "Level up [new pane]" ) : nullptr;
          const QAction* levelDownToNewPane = ( hasChildren )
            ? _contextMenu->addAction( "Level down [new pane]" ) : nullptr;
          const QAction* expandGroupToNewPane = ( grouped )
            ? _contextMenu->addAction( "Expand group [new pane]" ) : nullptr;
          const QAction* expandChildrenToNewPane = ( hasChildren )
            ? _contextMenu->addAction( "Expand children [new pane]" ) : nullptr;

          if ( editEntity || deleteEntity || dupEntity || autoEntity || levelUp
            || levelDown|| expandGroup || expandChildren ||  levelUpToNewPane
            || levelDownToNewPane || expandGroupToNewPane
            || expandChildrenToNewPane || showConnections )
          {
            shift::Representations representations;
            QAction* selectedAction = _contextMenu->exec( event->screenPos( ));
            if ( editEntity && editEntity == selectedAction )
            {
              createOrEditEntity( entity,
                EntityEditWidget::TEntityEditWidgetAction::EDIT_ENTITY );
            }
            else if ( dupEntity && dupEntity == selectedAction )
            {
              createOrEditEntity( entity,
                EntityEditWidget::TEntityEditWidgetAction::DUPLICATE_ENTITY );
            }
            else if ( deleteEntity && deleteEntity == selectedAction )
            {
              InteractionManager::deleteEntity( entityGid );
            }
            else if ( autoEntity && autoEntity == selectedAction )
            {
              createConnectionRelationship( entity, entity );
            }
            else if ( ( levelUp && levelUp == selectedAction ) ||
              ( levelUpToNewPane && levelUpToNewPane == selectedAction ))
            {
              if ( levelUpToNewPane && levelUpToNewPane == selectedAction )
              {
                nslib::PaneManager::activePane(
                  nslib::PaneManager::newPaneFromActivePane( ));
              }
              shift::Entities targetEntities;
              if ( grandParent == 0 )
              {
                for ( const auto& rootEntity
                  : DataManager::rootEntities( ).vector( ))
                {
                  targetEntities.add( rootEntity );
                }
              }
              else
              {
                targetEntities.addRelatedEntitiesOneToN( relParentOf,
                  dataEntities.at( grandParent ),
                  dataEntities, 1 );
                if ( targetEntities.empty( ))
                {
                  targetEntities.add( dataEntities.at( parent ));
                }
              }
              PaneManager::activePane( )->
                displayEntities( targetEntities, false, true );
            }
            else if ( ( levelDown && levelDown == selectedAction ) ||
              ( levelDownToNewPane && levelDownToNewPane == selectedAction ))
            {
              if ( levelDownToNewPane &&
                levelDownToNewPane == selectedAction )
              {
                nslib::PaneManager::activePane(
                  nslib::PaneManager::newPaneFromActivePane( ));
              }
              shift::Entities targetEntities;
              targetEntities.addRelatedEntitiesOneToN( relParentOf, entity,
                dataEntities, 1 );
              PaneManager::activePane( )->
                displayEntities( targetEntities, false, true );
            }
            else if ( ( expandChildren && expandChildren == selectedAction ) ||
              ( expandChildrenToNewPane
              && expandChildrenToNewPane == selectedAction ))
            {
              shift::Entities targetEntities =
                PaneManager::activePane( )->sceneEntities( );
              targetEntities.remove( entity );
              targetEntities.addRelatedEntitiesOneToN( relParentOf, entity,
                dataEntities, 1 );

              if ( expandChildrenToNewPane
                && expandChildrenToNewPane == selectedAction )
              {
                nslib::PaneManager::activePane(
                  nslib::PaneManager::newPaneFromActivePane( ));
              }
              PaneManager::activePane( )->
                displayEntities( targetEntities, false, true );
            }
            else if ( ( expandGroup && expandGroup == selectedAction ) ||
              ( expandGroupToNewPane &&
              expandGroupToNewPane == selectedAction ))
            {
              if ( expandGroupToNewPane &&
                expandGroupToNewPane == selectedAction )
              {
                nslib::PaneManager::activePane(
                  nslib::PaneManager::newPaneFromActivePane( ));
              }
              shift::Entities targetEntities;
              targetEntities.addRelatedEntitiesOneToN( relAGroupOf, entity,
                dataEntities, 1 );
              PaneManager::activePane( )->
                displayEntities( targetEntities, false, true );
            }
            else if ( showConnections && showConnections == selectedAction )
            {
              if( !_entityConnectListWidget )
              {
                _entityConnectListWidget = new EntityConnectionListWidget( );
              }
              _entityConnectListWidget->setConnections( entity,
                connectsToMap, connectedByMap, aggregatedConnectsToMap,
                aggregatedConnectedByMap );
            }
            else if ( selectedAction && childAction )
            {
              auto selectedEntity = actionToIdx[ selectedAction ];
              auto createAsParent = isParentEntity->at( selectedEntity );
              auto entityType = domainEntitiesTypes.getEntityObject(
               newEntitiesTypes->at( selectedEntity ));
              createOrEditEntity( entityType,
                EntityEditWidget::TEntityEditWidgetAction::NEW_ENTITY,
                entity, createAsParent, createAsParent );
            }
          }
        }
        else
        {
          Loggers::get( )->log( "item without entity",
            LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
        }
      }
      else
      {
        Loggers::get( )->log( "Clicked element is not item",
          LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
      }
    }

    tmpConnectionLineRemove( );
  }

  void InteractionManager::tmpConnectionLineRemove( )
  {
    if ( _tmpConnectionLine && _tmpConnectionLine.get( )
      && _tmpConnectionLine->scene( ))
    {
      _tmpConnectionLine->scene( )->removeItem( _tmpConnectionLine.get( ));
      _tmpConnectionLine.reset( nullptr );
    }
  } // context menu

  void InteractionManager::mousePressEvent( const QGraphicsView* graphicsView,
    QGraphicsItem* item, const QMouseEvent* event )
  {
    if ( item )
    {
      if ( event->modifiers( ).testFlag( Qt::ShiftModifier ))
      {
        Canvas* movingPane = PaneManager::activePane( );
        movingPane->layoutChanged( Layout::TLayoutIndexes::FREE );
        _movingLayout = dynamic_cast< FreeLayout* >(
          movingPane->layouts( ).getLayout( Layout::TLayoutIndexes::FREE ));
        auto newPos = graphicsView->mapToScene( event->pos( ));
        _movingLayout->startMoveRepresentation(item, newPos );
      }
      else
      {
        _tmpConnectionLine.reset(new TemporalConnectionLine( ));
        _tmpConnectionLine->setLine(
          QLineF( item->scenePos( ), item->scenePos( )));
        _tmpConnectionLine->setZValue( -100000 );
        _tmpConnectionLine->setPen( QPen( QColor( 128, 128, 128 ),
          nslib::Config::scale( ), Qt::DotLine ));

        auto scene = item->scene( );
        scene->addItem( _tmpConnectionLine.get( ));
      }
      auto parentItem = item->parentItem( );
      while ( parentItem )
      {
        auto selectableItem = dynamic_cast< SelectableItem*>( item );
        if ( selectableItem )
        {
          break;
        }
        item = parentItem;
        parentItem = item->parentItem( );
      }
      _item = item;
      _buttons = event->buttons( );
    }
    else
    {
      _item = nullptr;
      _buttons = Qt::MouseButtons( );
    }
  }

  void InteractionManager::mouseMoveEvent( const QGraphicsView* graphicsView,
    QAbstractGraphicsShapeItem* shapeItem, const QMouseEvent* event )
  {
    // It _item has value means that a link its being drawn
    if ( _item )
    {
      auto newPos = graphicsView->mapToScene( event->pos( ));
      if ( _movingLayout )
      {
        _movingLayout->moveRepresentation( newPos );
      }
      else if( _tmpConnectionLine.get( ))
      {
        const auto& initPoint = _tmpConnectionLine->line( ).p1( );
        _tmpConnectionLine->setLine( QLineF( initPoint, newPos ));
        auto item = dynamic_cast< Item* >( shapeItem );
        if ( shapeItem && item && !item->connectionRep( ))
        {
          InteractionManager::hoverEnterEvent( shapeItem, nullptr );
        }
      }
    }
  }

  void InteractionManager::mouseReleaseEvent( QGraphicsItem* item_,
    const QMouseEvent* /*event*/ )
  {
    if( _item )
    {
      if( _movingLayout )
      {
        _movingLayout->stopMoveActualRepresentation();
        _movingLayout = nullptr;
      }
      else if( item_ )
      {
        auto parentItem = item_->parentItem( );
        while ( parentItem )
        {
          auto selectableItem = dynamic_cast< SelectableItem*>( item_ );
          if ( selectableItem )
            break;

          item_ = parentItem;
          parentItem = item_->parentItem( );
        }

        if ( _item == item_ )
        {
          // Selection event
          if ( _buttons & Qt::LeftButton )
          {
            auto item = dynamic_cast< Item* >( item_ );

            auto selectableItem = dynamic_cast< SelectableItem* >( item );
            if ( selectableItem )
            {
              const auto& repsToEntities =
                RepresentationCreatorManager::repsToEntities( );
              if ( repsToEntities.find( item->parentRep( )) !=
                 repsToEntities.end( ))
              {
                const auto entities = repsToEntities.at( item->parentRep( ));
                if ( selectableItem->partiallySelected( ))
                {
                  selectableItem->setSelected( );
                }
                else
                {
                  selectableItem->toggleSelected( );
                }

                for( const auto& entity : entities )
                {
                  if (selectableItem->selected())
                  {
                    SelectionManager::setSelectedState(entity,
                        SelectedState::SELECTED);
                  }
                  else
                  {
                    SelectionManager::setSelectedState(entity,
                        SelectedState::UNSELECTED);
                  }

                  auto entityState = SelectionManager::getSelectedState( entity );
                  auto entityGid = entity->entityGid( );

                  const auto& allEntities = DataManager::entities( );
                  const auto& relationships = allEntities.relationships( );
                  const auto& relChildOf =
                    *( relationships.at( "isChildOf" )->asOneToOne( ));
                  const auto& relParentOf =
                    *( relationships.at( "isParentOf" )->asOneToN( ));
                  const auto& relSubEntityOf =
                    *( relationships.at( "isSubEntityOf" )->asOneToOne( ));
                  const auto& relSuperEntityOf =
                    *( relationships.at( "isSuperEntityOf" )->asOneToN( ));
                  const auto& relAGroupOf =
                    *( relationships.at( "isAGroupOf" )->asOneToN( ));

                  if ( relSubEntityOf.count( entityGid ) > 0 )
                  {
                    std::unordered_set< unsigned int > parentIds;
                    if ( relAGroupOf.count( entityGid ) > 0 )
                    {
                      const auto& groupedIds = relAGroupOf.at( entityGid );
                      for( auto const& groupedId : groupedIds )
                      {
                        SelectionManager::setSelectedState(
                          allEntities.at( groupedId.first ), entityState );

                        // Save unique parent set for updating only once per parent
                        if( relChildOf.count( groupedId.first ) > 0 )
                          parentIds.insert( relChildOf.at(groupedId.first ).entity );
                      }

                      _updateSelectedStateOfSubEntities(allEntities, relSuperEntityOf, relAGroupOf,
                          relSubEntityOf.at( entityGid ).entity );

                      std::unordered_set< unsigned int > uniqueParentChildIds;
                      for ( auto const& parentId : parentIds )
                      {
                        uniqueParentChildIds.insert(relParentOf.at( parentId ).begin( )->first );
                      }

                      for ( auto const& uniqueParentChildId : uniqueParentChildIds )
                      {
                        _propagateSelectedStateToParent(allEntities, relChildOf, relParentOf,
                            relSuperEntityOf, relAGroupOf,uniqueParentChildId, entityState );
                      }
                    }
                  } // if subentity
                  else
                  {
                    if ( relSuperEntityOf.count( entityGid ) > 0 )
                    {
                      const auto& subEntities =
                        relSuperEntityOf.at( entityGid );
                      for ( const auto& subEntity : subEntities )
                        SelectionManager::setSelectedState(allEntities.at( subEntity.first ), entityState );
                    }

                    _propagateSelectedStateToChilds(allEntities, relParentOf, relSuperEntityOf,entityGid, entityState );
                    _propagateSelectedStateToParent(allEntities, relChildOf, relParentOf, relSuperEntityOf, relAGroupOf, entityGid, entityState );
                  }
                  PaneManager::updateSelection( );
                }
              }
              else
              {
                Loggers::get( )->log( "item without entity", LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
                tmpConnectionLineRemove( );
                _item = nullptr;
                _buttons = Qt::MouseButtons( );

                return;
              }
            }

            // Publish selection
            std::vector< unsigned int > ids;
            SelectionManager::selectableEntitiesIds( ids );

            if ( Config::autoPublishSelection( ))
              ZeroEQManager::publishSelection( ids );

            if ( Config::autoPublishFocusOnSelection( ))
              ZeroEQManager::publishFocusOnSelection( ids );
          } // selection event
        }
        else
        {
          // drag and drop
          if ( _buttons & Qt::LeftButton )
          {
            auto originItem = dynamic_cast< Item* >( _item );
            auto destinationItem = dynamic_cast< Item* >( item_ );

            if ( destinationItem )
            {
              const auto& repsToEntities =
                RepresentationCreatorManager::repsToEntities( );
              if ( ( repsToEntities.find( originItem->parentRep( )) !=
                    repsToEntities.end( )) &&
                  ( repsToEntities.find( destinationItem->parentRep( )) !=
                    repsToEntities.end( )))
              {
                const auto originEntity =
                  *( repsToEntities.at( originItem->parentRep( )).begin( ));
                const auto destinationEntity = *( repsToEntities.at(
                  destinationItem->parentRep( )).begin( ));

                createConnectionRelationship( originEntity, destinationEntity );
              }
            }
          }
        }
      }
    }

    tmpConnectionLineRemove( );
    _item = nullptr;
    _buttons = Qt::MouseButtons( );
  }

  void InteractionManager::createConnectionRelationship(
    shift::Entity* originEntity_, shift::Entity* destinationEntity_,
    ConnectionRelationshipEditWidget::TConnectionType connectionType_ )
  {
    _conRelationshipEditWidget->updateWidget(
      originEntity_, destinationEntity_, connectionType_);
  }

  void InteractionManager::_propagateSelectedStateToChilds(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relParentOf,
    const shift::RelationshipOneToN& relSuperEntityOf,
    unsigned int entityGid,
    SelectedState state )
  {
    if ( relParentOf.count( entityGid ) == 0 )
      return;
    const auto& childrenIds = relParentOf.at( entityGid );
    for ( auto const& childId : childrenIds )
    {
      if ( relSuperEntityOf.count( childId.first ) > 0 )
      {
        const auto& subEntities = relSuperEntityOf.at( childId.first );
        for ( const auto& subEntity : subEntities )
          SelectionManager::setSelectedState(
            DataManager::entities( ).at( subEntity.first ), state );
      }
      SelectionManager::setSelectedState(
        entities.at( childId.first ), state );
      _propagateSelectedStateToChilds( entities, relParentOf, relSuperEntityOf,
        childId.first, state );
    }
  }

  void InteractionManager::_propagateSelectedStateToParent(
    const shift::Entities& entities,
    const shift::RelationshipOneToOne& relChildOf,
    const shift::RelationshipOneToN& relParentOf,
    const shift::RelationshipOneToN& relSuperEntityOf,
    const shift::RelationshipOneToN& relAGroupOf,
    unsigned int entityGid,
    SelectedState childState )
  {
    if ( relChildOf.count( entityGid ) == 0 )
      return;
    const auto& parentId = relChildOf.at( entityGid ).entity;

    if ( parentId == 0 ) return;

    if ( childState == SelectedState::PARTIALLY_SELECTED )
    {
      SelectionManager::setSelectedState(
        entities.at( parentId ), childState );
      _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
                                       relSuperEntityOf, relAGroupOf,
                                       parentId, childState );
      return;
    }

    bool allChildrenSelected, noChildrenSelected;
    queryChildrenSelectedState( entities, relParentOf, parentId,
                                allChildrenSelected, noChildrenSelected );

    SelectedState state;
    if ( noChildrenSelected )
      state = SelectedState::UNSELECTED;
    else if ( allChildrenSelected )
      state = SelectedState::SELECTED;
    else
      state = SelectedState::PARTIALLY_SELECTED;

    SelectionManager::setSelectedState(
      entities.at( parentId ), state );

    _updateSelectedStateOfSubEntities(
      entities, relSuperEntityOf, relAGroupOf, parentId );

    _propagateSelectedStateToParent( entities, relChildOf, relParentOf,
       relSuperEntityOf, relAGroupOf, parentId, state );
  }

  void InteractionManager::_updateSelectedStateOfSubEntities(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relSuperEntityOf,
    const shift::RelationshipOneToN& relAGroupOf,
    unsigned int entityGid )
  {
    if ( relSuperEntityOf.count( entityGid ) == 0 )
      return;

    for ( const auto& subEntityId : relSuperEntityOf.at( entityGid ))
    {
      bool allGroupedSelected, noGroupedSelected;
      _queryGroupedSelectedState( entities, relAGroupOf, subEntityId.first,
                                  allGroupedSelected, noGroupedSelected );
      SelectedState groupedState;
      if ( noGroupedSelected )
        groupedState = SelectedState::UNSELECTED;
      else if ( allGroupedSelected )
        groupedState = SelectedState::SELECTED;
      else
        groupedState = SelectedState::PARTIALLY_SELECTED;

      SelectionManager::setSelectedState(
        entities.at( subEntityId.first ), groupedState );
    }
  }

  void InteractionManager::queryChildrenSelectedState(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relParentOf,
    unsigned int entityGid,
    bool& allChildrenSelected,
    bool& noChildrenSelected )
  {
    allChildrenSelected = true;
    noChildrenSelected = true;
    const auto& childrenIds = relParentOf.at( entityGid );
    for ( auto const& childId : childrenIds )
    {
      if ( SelectionManager::getSelectedState( entities.at( childId.first )) !=
           SelectedState::SELECTED )
        allChildrenSelected = false;
      if ( SelectionManager::getSelectedState( entities.at( childId.first )) ==
           SelectedState::SELECTED )
        noChildrenSelected = false;
    }
  }

  void InteractionManager::_queryGroupedSelectedState(
    const shift::Entities& entities,
    const shift::RelationshipOneToN& relAGroupOf,
    unsigned int entityGid,
    bool& allGroupedSelected,
    bool& noGroupedSelected )
  {
    allGroupedSelected = true;
    noGroupedSelected = true;
    if ( relAGroupOf.count( entityGid ) == 0 )
    {
      allGroupedSelected = false;
      noGroupedSelected = true;
      return;
    }
    const auto& groupedIds = relAGroupOf.at( entityGid );
    for ( auto const& groupedId : groupedIds )
    {
      if ( SelectionManager::getSelectedState( entities.at( groupedId.first )) !=
           SelectedState::SELECTED )
        allGroupedSelected = false;
      if ( SelectionManager::getSelectedState( entities.at( groupedId.first )) ==
           SelectedState::SELECTED )
        noGroupedSelected = false;
    }
  }

  void InteractionManager::createOrEditEntity( shift::Entity* entity_,
    EntityEditWidget::TEntityEditWidgetAction action_,
    shift::Entity* parentEntity_, const bool changeParent_,
    const bool addToScene_)
  {
    _entityEditWidget->updateEntity( entity_, action_, parentEntity_,
      changeParent_, addToScene_ );
    _entityEditWidget->activateWindow();
  }

  void InteractionManager::updateEntityConnectionList(
    shift::EntityGid origEntity_, shift::EntityGid destEntity,
    const bool updateAggregatedTo_,
    const bool updateAggregatedBy_ )
  {
    if( _entityConnectListWidget && !_entityConnectListWidget->isHidden( ))
    {
      _entityConnectListWidget->updateConnections( origEntity_, destEntity,
        updateAggregatedTo_, updateAggregatedBy_ );
    }
  }

  void InteractionManager::refreshEntityConnectionList( void )
  {
    if( _entityConnectListWidget && !_entityConnectListWidget->isHidden( ))
    {
      _entityConnectListWidget->hide( );
      _entityConnectListWidget->show( );
    }
  }

  void InteractionManager::updateConnectionRelationship(
    shift::Entity* originEntity_, shift::Entity* destEntity_ )
  {
    if( _conRelationshipEditWidget && !_conRelationshipEditWidget->isHidden( ))
    {
      auto destEntity = _conRelationshipEditWidget->destEntity( );
      auto origEntity = _conRelationshipEditWidget->originEntity( );
      const bool isAggregated = _conRelationshipEditWidget->isAggregated( );
      if( isAggregated ||
        ( destEntity == destEntity_ && origEntity == originEntity_ ))
      {
        auto type = isAggregated
          ? ConnectionRelationshipEditWidget::TConnectionType::AGGREGATED
          : ConnectionRelationshipEditWidget::TConnectionType::SIMPLE;
        _conRelationshipEditWidget->updateWidget(
          origEntity, destEntity, type );
      }
    }
  }

  void InteractionManager::deleteEntity( shift::EntityGid entityGid_ )
  {
    auto& dataEntities = DataManager::entities( );
    auto relations = dataEntities.relationships( );
    auto entity = dataEntities.at( entityGid_ );
    if( entity->isSubEntity( ))
    {
      Loggers::get( )->log( "Deleting a subEntity",
        LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
    }
    else
    {
      auto& relParentOf = *( relations[ "isParentOf" ]->asOneToN( ));
      //const auto& children = relParentOf[ entityGid_ ];

      auto& relChildOf = *( relations[ "isChildOf" ]->asOneToOne( ));
      const auto& parent = relChildOf[ entityGid_ ].entity;
      shift::Entity* parentEntity = nullptr;
      if( parent > 0 )
      {
        parentEntity = dataEntities.at(parent);
        shift::Relationship::relationBreak( relParentOf, relChildOf,
          parentEntity, entity );
        //All dependent aggregateds will be break so they wont be reclaculated.
      }
      else if ( !entity->isNotHierarchy( ))
      {
        DataManager::rootEntities( ).remove( entity );
      }
      auto& relConnectsTo = *( relations[ "connectsTo" ]->asOneToN( ));
      auto& relConnectedBy = *( relations[ "connectedBy" ]->asOneToN( ));
      auto& relAggregatedConnectsTo = *( relations[ "aggregatedConnectsTo" ]
        ->asAggregatedOneToN( ));
      auto& relAggregatedConnectBy = *( relations[ "aggregatedConnectedBy" ]
        ->asAggregatedOneToN( ));

      auto& relSubEntityOf = *( relations.at( "isSubEntityOf" )->asOneToOne( ));
      auto& relSuperEntityOf =
        *( relations.at( "isSuperEntityOf" )->asOneToN( ));
      auto& relAGroupOf = *( relations[ "isAGroupOf" ]->asOneToN( ));
      auto& relAPartOf = *( relations[ "isPartOf" ]->asOneToN( ));

      basicDeleteEntity( entity, dataEntities, relConnectsTo, relConnectedBy,
        relParentOf, relChildOf, relAggregatedConnectsTo,
        relAggregatedConnectBy, relSubEntityOf, relSuperEntityOf,
        relAGroupOf, relAPartOf );

      if( parentEntity )
      {
        parentEntity->autoUpdateProperties( );
      }
      if( _entityConnectListWidget
        && !_entityConnectListWidget->isHidden( ))
      {
        _entityConnectListWidget->updateConnections( );
      }
      //Updated if it's an aggregated
      updateConnectionRelationship( nullptr, nullptr );
      for ( auto pane : PaneManager::panes( ))
      {
        pane->resizeEvent( nullptr );
      }
    }
  }

  void InteractionManager::basicDeleteEntity( shift::Entity* entity_,
    shift::Entities& dataEntities_, shift::RelationshipOneToN& relConnectsTo_,
    shift::RelationshipOneToN& relConnectedBy_,
    shift::RelationshipOneToN& relParentOf_,
    shift::RelationshipOneToOne& relChildOf_,
    shift::RelationshipAggregatedOneToN& relAggregatedConnectsTo_,
    shift::RelationshipAggregatedOneToN& relAggregatedConnectBy_,
    shift::RelationshipOneToOne& relSubEntityOf_,
    shift::RelationshipOneToN& relSuperEntityOf_,
    shift::RelationshipOneToN& relAGroupOf_,
    shift::RelationshipOneToN& relAPartOf_)
  {
    if( !entity_->isSubEntity( ))
    {
      shift::Entities connectedEntites;
      connectedEntites.addRelatedEntitiesOneToN( relParentOf_, entity_,
                                                 dataEntities_, 1 );
      for( auto connectEntity : connectedEntites.vector( ) )
      {
        shift::Relationship::relationBreak( relParentOf_, relChildOf_, entity_,
          connectEntity );
        basicDeleteEntity( connectEntity, dataEntities_, relConnectsTo_,
          relConnectedBy_, relParentOf_, relChildOf_, relAggregatedConnectsTo_,
          relAggregatedConnectBy_, relSubEntityOf_, relSuperEntityOf_,
          relAGroupOf_, relAPartOf_ );
      }
      connectedEntites.clear( );
      connectedEntites.addRelatedEntitiesOneToN( relConnectsTo_, entity_,
        dataEntities_, 1 );
      for( auto connectEntity : connectedEntites.vector( ))
      {
        shift::Relationship::BreakAnAggregatedRelation(
          relAggregatedConnectsTo_, relAggregatedConnectBy_,
          entity_, connectEntity );
      }
      connectedEntites.clear( );
      connectedEntites.addRelatedEntitiesOneToN( relConnectedBy_, entity_,
        dataEntities_, 1 );
      for( auto connectEntity : connectedEntites.vector( ))
      {
        shift::Relationship::BreakAnAggregatedRelation(
          relAggregatedConnectsTo_, relAggregatedConnectBy_,
          connectEntity, entity_ );
      }
      connectedEntites.clear( );
      connectedEntites.addRelatedEntitiesOneToN( relAGroupOf_, entity_,
        dataEntities_, 1 );
      for( auto connectEntity : connectedEntites.vector( ))
      {
        shift::Relationship::relationBreak( relAGroupOf_,
          relAPartOf_, connectEntity, entity_ );
      }
      connectedEntites.clear( );
      connectedEntites.addRelatedEntitiesOneToN( relAPartOf_, entity_,
        dataEntities_, 1 );
      for( auto connectEntity : connectedEntites.vector( ))
      {
        shift::Relationship::relationBreak( relAGroupOf_,  relAPartOf_, entity_,
          connectEntity );
      }
      connectedEntites.clear( );
      connectedEntites.addRelatedEntitiesOneToN( relSuperEntityOf_, entity_,
        dataEntities_, 1 );
      for( auto connectEntity : connectedEntites.vector( ))
      {
        shift::Relationship::relationBreak( relSuperEntityOf_,
          relSubEntityOf_, entity_, connectEntity );
        basicDeleteEntity( connectEntity, dataEntities_, relConnectsTo_,
          relConnectedBy_, relParentOf_, relChildOf_,
          relAggregatedConnectsTo_, relAggregatedConnectBy_, relSubEntityOf_,
          relSuperEntityOf_, relAGroupOf_, relAPartOf_ );
      }
    }

    dataEntities_.remove( entity_ );
    if( _entityEditWidget && !_entityEditWidget->isHidden( )
        && _entityEditWidget->entity( ) == entity_ )
    {
      EntityEditWidget::parentDock( )->hide( );
      _entityEditWidget->hide( );
    }
    if( _conRelationshipEditWidget && !_conRelationshipEditWidget->isHidden( )
        && ( _conRelationshipEditWidget->originEntity( ) == entity_
        || _conRelationshipEditWidget->destEntity( ) == entity_ ))
    {
      ConnectionRelationshipEditWidget::parentDock( )->hide( );
      _conRelationshipEditWidget->hide( );
    }
    if( _entityConnectListWidget && !_entityConnectListWidget->isHidden( )
        && _entityConnectListWidget->entity( ) == entity_ )
    {
      EntityConnectionListWidget::parentDock( )->hide( );
      _entityConnectListWidget->hide( );
    }
    for( auto pane : PaneManager::panes( ))
    {
      pane->removeEntity( entity_ );
    }
    RepresentationCreatorManager::removeEntity( entity_ );
    delete entity_;
  }

  unsigned int InteractionManager::addCreateEntitiesContextMenu(
    const int selectedId_,
    const shift::EntitiesTypes& entitiesTypes_,
    shift::Entities& dataEntities_,
    shift::Entity*& selectedEntity_,
    std::unordered_map< QAction*, unsigned int >& actionToIdx_,
    std::vector< std::string >*& newEntitiesTypes_,
    const bool addParentTypes_,
    std::vector< bool >*& isParentEntity_ )
  {
    unsigned int entityIdx = 0;
    if( selectedId_ <= 0 )
    {
      for( const auto& type : entitiesTypes_.entitiesTypes( ))
      {
        if( !( std::get< shift::EntitiesTypes::IS_SUBENTITY >( type )
          || std::get< shift::EntitiesTypes::IS_NOT_HIERARCHY >( type )))
        {
          std::string childType =
            std::get< shift::EntitiesTypes::ENTITY_NAME >( type );
          QAction* action = _contextMenu->addAction(
            QString( "Add " ) + QString::fromStdString( childType ) );
          newEntitiesTypes_->push_back( childType );
          actionToIdx_[ action ] = entityIdx++;
        }
      }
    }
    else
    {
      selectedEntity_ = dataEntities_.at( selectedId_ );
      const auto selectedEntityType = selectedEntity_->typeName( );

      shift::RelationshipPropertiesTypes::rel_constr_range typesNames;
      typesNames =
        shift::RelationshipPropertiesTypes::constraints( "ParentOf",
          selectedEntityType );
      QAction* action = nullptr;
      for( auto childTypeName = typesNames.first;
        childTypeName != typesNames.second; ++childTypeName )
      {
        action = _contextMenu->addAction(
          QString( "Add " ) + QString::fromStdString(
          childTypeName->second ) + QString( " as child" ));
        actionToIdx_[ action ] = entityIdx++;
        newEntitiesTypes_->push_back( childTypeName->second );
        isParentEntity_->push_back( false );
      }
      if ( addParentTypes_ )
      {
        if ( action )
        {
          _contextMenu->addSeparator( );
        }
        typesNames = shift::RelationshipPropertiesTypes::constraints(
          "ChildOf", selectedEntityType );
        auto& relChildOf = *( DataManager::entities( ).relationships( )
          [ "isChildOf" ]->asOneToOne( ));
        const auto parentIt = relChildOf.find( selectedId_ );
        const auto parentType =
          ( parentIt != relChildOf.end( ) && parentIt->second.entity != 0 )
          ? dataEntities_.at( parentIt->second.entity )->typeName( ) : "";

        for( auto parentTypeName = typesNames.first;
          parentTypeName != typesNames.second; ++parentTypeName )
        {
          if ( parentType == "" || shift::RelationshipPropertiesTypes::
            isConstrained( "ChildOf", parentTypeName->second, parentType ))
          {
            action = _contextMenu->addAction(
              QString( "Add " ) + QString::fromStdString(
              parentTypeName->second ) + QString( " as parent" ));
            actionToIdx_[ action ] = entityIdx++;
            newEntitiesTypes_->push_back( parentTypeName->second );
            isParentEntity_->push_back( true );
          }
        }
      }

    }
    return entityIdx;
  }

  QStatusBar* InteractionManager::statusBar( void )
  {
    return _statusBar;
  }

  void InteractionManager::statusBar( QStatusBar* statusBar_ )
  {
    _statusBar = statusBar_;
  }

  QMenu* InteractionManager::contextMenu( )
  {
    return _contextMenu;
  }

  void InteractionManager::updateEntityParents( shift::Entity* entity_ )
  {
    const bool freeLayoutInUse = PaneManager::freeLayoutInUse( );
    auto& dataEntities = DataManager::entities( );
    auto& dataRelations = dataEntities.relationships( );
    auto& relChildOf = *( dataRelations[ "isChildOf" ]->asOneToOne( ));
    shift::Entities updatedEntities_;
    updatedEntities_.add( entity_ );
    auto parentID = relChildOf[ entity_->entityGid( ) ].entity;
    while( parentID > 0 )
    {
      const auto parent = dataEntities.at( parentID );
      parent->autoUpdateProperties( );
      updatedEntities_.add( parent );
      parentID = relChildOf[ parentID ].entity;
    }
    for (auto& creator : RepresentationCreatorManager::creators( ))
    {
      RepresentationCreatorManager::updateEntities( updatedEntities_,
        creator.first, freeLayoutInUse );
    }
  }

} // namespace nslib
