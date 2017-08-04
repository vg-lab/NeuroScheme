/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
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

#ifndef __NSLIB__CONGEN_DATA_SAVER_H_
#define __NSLIB__CONGEN_DATA_SAVER_H_

#include <nslib/api.h>
#include <QWidget>

#include "XMLConGenExporter.h"

namespace nslib
{
  namespace congen
  {
    class NSLIB_API  DataSaver
        : public QWidget
    {
      public:

      signals:
        static void saveConGenXmlScene( QWidget * ptr );
    };
  } /* namespace congen */
} /* namespace nslib */

#endif /* __NSLIB__CONGEN_DATA_SAVER_H_ */
