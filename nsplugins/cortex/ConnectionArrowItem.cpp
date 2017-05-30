/*
 * ConnectionArrowItem.cpp
 *
 *  Created on: 30/5/2017
 *      Author: juanpebm
 */


#include "ConnectionArrowItem.h"

namespace nslib
{
  namespace cortex
  {
	  ConnectionArrowItem::ConnectionArrowItem( const ConnectionArrowRep& connectionArrowRep )
	  {
		   this->_parentRep =
           &( const_cast< ConnectionArrowRep& >( connectionArrowRep ));

			//DONE
			arrowWidht= //30;
			arrowLengh=30;

			QPoint mLineInit(rand()%800, rand()%600);
			QPoint mLineEnd(rand()%800, rand()%600);
			QLineF lAuxLine(mLineInit,mLineEnd);

			double angle = ::acos(lAuxLine.dx() / lAuxLine. length());
			if (lAuxLine.dy() >= 0) angle = (M_PI * 2) - angle;

			QPointF arrowInit 	= lAuxLine.pointAt(1.0f - (arrowLengh/lAuxLine.length()));
			QPointF arrowP1 	= arrowInit - QPointF(sin(angle + M_PI / 3) 		* arrowWidht
													  ,cos(angle + M_PI / 3) 		* arrowWidht);
			QPointF arrowP2 	= arrowInit - QPointF(sin(angle + M_PI - M_PI / 3) 	* arrowWidht
													  ,cos(angle + M_PI - M_PI / 3) * arrowWidht);
			arrowShape.clear();
			arrowShape		<< lAuxLine.p1()
							<< arrowInit
							<< arrowP1
							<< lAuxLine.p2()
							<< arrowP2
							<< arrowInit
							;

			this->setPolygon(arrowShape);
  	  }
   }
}
