/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2005-2010  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Machine Perception and Intelligent    |
   |      Robotics Lab, University of Malaga (Spain).                          |
   |    Contact: Jose-Luis Blanco  <jlblanco@ctima.uma.es>                     |
   |                                                                           |
   |  This file is part of the MRPT project.                                   |
   |                                                                           |
   |     MRPT is free software: you can redistribute it and/or modify          |
   |     it under the terms of the GNU General Public License as published by  |
   |     the Free Software Foundation, either version 3 of the License, or     |
   |     (at your option) any later version.                                   |
   |                                                                           |
   |   MRPT is distributed in the hope that it will be useful,                 |
   |     but WITHOUT ANY WARRANTY; without even the implied warranty of        |
   |     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
   |     GNU General Public License for more details.                          |
   |                                                                           |
   |     You should have received a copy of the GNU General Public License     |
   |     along with MRPT.  If not, see <http://www.gnu.org/licenses/>.         |
   |                                                                           |
   +---------------------------------------------------------------------------+ */
#ifndef opengl_StockObjects_H
#define opengl_StockObjects_H

#include <mrpt/opengl/CSetOfObjects.h>

namespace mrpt
{
	namespace opengl
	{
		/** A collection of pre-built 3D objects for quick insertion in opengl::COpenGLScene objects.
		  */
		namespace stock_objects
		{
			/** Returns a representation of a Pioneer II mobile base.
			  *  The generated object must be inserted in a opengl::COpenGLScene or opengl::CSetOfObjects.
			  */
			CSetOfObjectsPtr OPENGL_IMPEXP RobotPioneer();

			/** Returns three arrows representing a X,Y,Z 3D corner.
			  *  The generated object must be inserted in a opengl::COpenGLScene or opengl::CSetOfObjects.
			  * \sa CornerXYZSimple
			  */
			CSetOfObjectsPtr OPENGL_IMPEXP CornerXYZ(float scale=1.0);

			/** Returns three arrows representing a X,Y,Z 3D corner (just thick lines instead of complex arrows for faster rendering than CornerXYZ).
			  *  The generated object must be inserted in a opengl::COpenGLScene or opengl::CSetOfObjects.
			  * \sa CornerXYZ
			  */
			CSetOfObjectsPtr OPENGL_IMPEXP CornerXYZSimple(float scale=1.0, float lineWidth=1.0);

			/** Returns a high-quality 3D model of a SICK laser scanner (Model by Miguel Angel Espada Bernal)
			  */
			CSetOfObjectsPtr OPENGL_IMPEXP SICKLaser();

			/** Returns a simple 3D model of a PointGrey Bumblebee stereo camera.
			  *  The generated object must be inserted in a opengl::COpenGLScene or opengl::CSetOfObjects.
			  */
			CSetOfObjectsPtr OPENGL_IMPEXP BumblebeeCamera();

		} // end namespace stock_objects
	}

} // End of namespace


#endif