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
#ifndef CColouredPointsMap_H
#define CColouredPointsMap_H

#include <mrpt/slam/CPointsMap.h>
#include <mrpt/slam/CObservation2DRangeScan.h>
#include <mrpt/slam/CObservationImage.h>
#include <mrpt/utils/CSerializable.h>
#include <mrpt/math/CMatrix.h>
#include <mrpt/utils/stl_extensions.h>

#include <mrpt/maps/link_pragmas.h>

namespace mrpt
{
	namespace slam
	{

		DEFINE_SERIALIZABLE_PRE_CUSTOM_BASE_LINKAGE( CColouredPointsMap, CPointsMap,MAPS_IMPEXP )

		/** A map of 2D/3D points with individual colours (RGB).
		 *  For different color schemes, see CColouredPointsMap::colorScheme
		 *  Colors are defined in the range [0,1].
		 * \sa mrpt::slam::CPointsMap, mrpt::slam::CMetricMap, mrpt::utils::CSerializable
		 */
		class MAPS_IMPEXP CColouredPointsMap : public CPointsMap
		{
			// This must be added to any CSerializable derived class:
			DEFINE_SERIALIZABLE( CColouredPointsMap )

		protected:
			/** The color data */
			vector_float	m_color_R,m_color_G,m_color_B;

			/** Minimum distance from where the points have been seen */
			vector_float	m_min_dist;

			/** Clear the map, erasing all the points.
			 */
			virtual void  internal_clear();

			 /** Insert the observation information into this map. This method must be implemented
			  *    in derived classes.
			  * \param obs The observation
			  * \param robotPose The 3D pose of the robot mobile base in the map reference system, or NULL (default) if you want to use CPose2D(0,0,deg)
			  *
			  * \sa CObservation::insertObservationInto
			  */
			virtual bool  internal_insertObservation( const CObservation *obs, const CPose3D *robotPose = NULL );

		public:
			 /** Destructor
			   */
			 virtual ~CColouredPointsMap();

			 /** Default constructor
			  */
			 CColouredPointsMap();

			 /** Copy operator
			  */
			 void  copyFrom(const CPointsMap &obj);

			/** Transform the range scan into a set of cartessian coordinated
			  *	 points. The options in "insertionOptions" are considered in this method.
			  * \param rangeScan The scan to be inserted into this map
			  * \param robotPose The robot 3D pose, default to (0,0,0|0deg,0deg,0deg). It is used to compute the sensor pose relative to the robot actual pose. Recall sensor pose is embeded in the observation class.
			  *
			  * \note Only ranges marked as "valid=true" in the observation will be inserted
			  *
			  * \sa CObservation2DRangeScan
			  */
			void  loadFromRangeScan(
					const CObservation2DRangeScan &rangeScan,
					const CPose3D				  *robotPose = NULL );

			/** Transform the range scan into a set of cartessian coordinated
			  *	 points. The options in "insertionOptions" are considered in this method.
			  * \param rangeScan The scan to be inserted into this map
			  * \param robotPose The robot 3D pose, default to (0,0,0|0deg,0deg,0deg). It is used to compute the sensor pose relative to the robot actual pose. Recall sensor pose is embeded in the observation class.
			  *
			  * \sa CObservation3DRangeScan
			  */
			void  loadFromRangeScan(
					const CObservation3DRangeScan &rangeScan,
					const CPose3D				  *robotPose = NULL );

			/** Load from a text file. In each line there are a point coordinates.
			 *   Returns false if any error occured, true elsewere.
			 */
			bool  load2D_from_text_file(std::string file);

			/** Load from a text file. In each line there are a point coordinates.
			 *   Returns false if any error occured, true elsewere.
			 */
			bool  load3D_from_text_file(std::string file);

			/** Save to a text file. In each line contains X Y Z (meters) R G B (range [0,1]) for each point in the map.
			 *     Returns false if any error occured, true elsewere.
			 */
			bool  save3D_and_colour_to_text_file(const std::string &file) const;

			/** Insert the contents of another map into this one, fusing the previous content with the new one.
			 *    This means that points very close to existing ones will be "fused", rather than "added". This prevents
			 *     the unbounded increase in size of these class of maps.
			 *		NOTICE that "otherMap" is neither translated nor rotated here, so if this is desired it must done
			 *		 before calling this method.
			 * \param otherMap The other map whose points are to be inserted into this one.
			 * \param minDistForFuse Minimum distance (in meters) between two points, each one in a map, to be considered the same one and be fused rather than added.
			 * \param notFusedPoints If a pointer is supplied, this list will contain at output a list with a "bool" value per point in "this" map. This will be false/true according to that point having been fused or not.
			 * \sa insertAnotherMap
			 */
			void  fuseWith(	CPointsMap			*otherMap,
										float				minDistForFuse  = 0.02f,
										std::vector<bool>	*notFusedPoints = NULL);

			/** Changes a given point from map, as a 2D point. First index is 0.
			 * \exception Throws std::exception on index out of bound.
			 */
			virtual void  setPoint(size_t index,CPoint2D &p);

			/** Changes a given point from map, as a 3D point. First index is 0.
			 * \exception Throws std::exception on index out of bound.
			 */
			virtual void  setPoint(size_t index,CPoint3D &p);

			/** Changes a given point from map. First index is 0.
			 * \exception Throws std::exception on index out of bound.
			 */
			virtual void  setPoint(size_t index,float x, float y);

			/** Changes a given point from map. First index is 0.
			 * \exception Throws std::exception on index out of bound.
			 */
			virtual void  setPoint(size_t index,float x, float y, float z);

			/** Provides a way to insert individual points into the map:
			  */
			void  insertPoint( float x, float y, float z = 0 );

			/** Provides a way to insert individual points into the map:
			  */
			void  insertPoint( CPoint3D p );

			/** Remove from the map the points marked in a bool's array as "true".
			  *
			  * \exception std::exception If mask size is not equal to points count.
			  */
			void  applyDeletionMask( std::vector<bool> &mask );

			/** Adds a new point given its coordinates and color.
			  */
			void  insertPoint( float x, float y, float z, float R, float G, float B );

			/** Retrieves a point and its color
			  */
			virtual void  getPoint( size_t index, float &x, float &y, float &z, float &R, float &G, float &B ) const;

			/** Returns true if the point map has a color field for each point */
			virtual bool hasColorPoints() const { return true; }

			/** This method is called at the end of each "prediction-update-map insertion" cycle within "mrpt::slam::CMetricMapBuilderRBPF::processActionObservation".
			  *  This method should normally do nothing, but in some cases can be used to free auxiliary cached variables.
			  */
			void  auxParticleFilterCleanUp();

			/** Reserves memory for a given number of points: the size of the map does not change, it only reserves the memory.
			  *  This is useful for situations where it is approximately known the final size of the map. This method is more
			  *  efficient than constantly increasing the size of the buffers. Refer to the STL C++ library's "reserve" methods.
			  */
			void reserve(size_t newLength);

			/** Set all the points at once from vectors with X,Y and Z coordinates.  */
			void setAllPoints(const vector_float &X,const vector_float &Y,const vector_float &Z);

			/** Set all the points at once from vectors with X and Y coordinates (Z=0).  */
			void setAllPoints(const vector_float &X,const vector_float &Y);

			/** Override of the default 3D scene builder to account for the individual points' color.
			  */
			virtual void  getAs3DObject ( mrpt::opengl::CSetOfObjectsPtr	&outObj ) const;

			/** Colour a set of points from a CObservationImage and the global pose of the robot
			  */
			bool colourFromObservation( const CObservationImage &obs, const CPose3D &robotPose );

			/** The choices for coloring schemes:
			  *		- cmFromHeightRelativeToSensor: The Z coordinate wrt the sensor will be used to obtain the color using the limits z_min,z_max.
			  * 	- cmFromIntensityImage: When inserting 3D range scans, take the color from the intensity image channel, if available.
			  * \sa TColourOptions
			  */
			enum TColouringMethod
			{
				cmFromHeightRelativeToSensor = 0,
				cmFromHeightRelativeToSensorJet = 0,
				cmFromHeightRelativeToSensorGray = 1,
				cmFromIntensityImage = 2
			};

			/** The definition of parameters for generating colors from laser scans */
			 struct MAPS_IMPEXP TColourOptions : public utils::CLoadableOptions
			 {
				/** Initilization of default parameters */
				TColourOptions( );
				virtual ~TColourOptions() {}
				/** See utils::CLoadableOptions
				  */
				void  loadFromConfigFile(
					const mrpt::utils::CConfigFileBase  &source,
					const std::string &section);

				/** See utils::CLoadableOptions
				  */
				void  dumpToTextStream(CStream	&out) const;

				TColouringMethod	scheme;
				float				z_min,z_max;
				float				d_max;
			 };

			 TColourOptions	colorScheme;	//!< The options employed when inserting laser scans in the map.

			 void resetPointsMinDist( float defValue = 2000.0f ); //!< Reset the minimum-observed-distance buffer for all the points to a predefined value

		}; // End of class def.

	} // End of namespace
} // End of namespace

#endif