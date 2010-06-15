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
#ifndef CPOINTSMAP_H
#define CPOINTSMAP_H

#include <mrpt/slam/CMetricMap.h>
#include <mrpt/utils/CSerializable.h>
#include <mrpt/math/CMatrix.h>
#include <mrpt/utils/CLoadableOptions.h>
#include <mrpt/utils/safe_pointers.h>

#include <mrpt/poses/CPoint2D.h>
#include <mrpt/math/lightweight_geom_data.h>

#include <mrpt/otherlibs/ann/ANN.h>   // ANN: for kd-tree


#include <mrpt/maps/link_pragmas.h>

namespace mrpt
{
namespace slam
{
	using namespace mrpt::poses;
	using namespace mrpt::math;

	class CObservation2DRangeScan;
	class CSimplePointsMap;
	class CMultiMetricMap;
	class CColouredPointsMap;
	class COccupancyGridMap2D;

	DEFINE_SERIALIZABLE_PRE_CUSTOM_BASE_LINKAGE( CPointsMap , CMetricMap, MAPS_IMPEXP )

	/** A cloud of points in 2D or 3D, which can be built from a sequence of laser scans or other sensors.
	 *  This is a virtual class, thus only a derived class can be instantiated by the user. The user most usually wants to use CSimplePointsMap.
	 * \sa CMetricMap, CPoint, mrpt::utils::CSerializable
	 */
	class MAPS_IMPEXP CPointsMap : public CMetricMap
	{
		friend class CMultiMetricMap;
		friend class CMultiMetricMapPDF;
		friend class CSimplePointsMap;
		friend class CColouredPointsMap;
		friend class COccupancyGridMap2D;

		// This must be added to any CSerializable derived class:
		DEFINE_VIRTUAL_SERIALIZABLE( CPointsMap )

	 protected:
		 /** The points coordinates
		  */
		 std::vector<float>		x,y,z;

		 /** The points weights
		  */
		 std::vector<uint32_t>		pointWeight;

		 /** Auxiliary variables used in "getLargestDistanceFromOrigin"
		   * \sa getLargestDistanceFromOrigin
		   */
		 mutable float	m_largestDistanceFromOrigin;

		 /** Auxiliary variables used in "getLargestDistanceFromOrigin"
		   * \sa getLargestDistanceFromOrigin
		   */
		 mutable bool	m_largestDistanceFromOriginIsUpdated;

		 /** Auxiliary variables used in "buildKDTree2D" / "buildKDTree3D"
		   */
		 mutable bool   m_KDTreeDataIsUpdated;


		/** Internal structure with a KD-tree representation.
		 */
		struct MAPS_IMPEXP TKDTreeData
		{
			/** Init the pointer to NULL.
			  */
			TKDTreeData();

			/** Copy constructor, invoked when copying CPointsMap: It actually does NOT copy the kd-tree, a new object will be created if required!
			  */
			TKDTreeData(const TKDTreeData &o);

			/** Copy operator: It actually does NOT copy the kd-tree, a new object will be created if required!
			  */
			TKDTreeData& operator =(const TKDTreeData &o);

			/** Free memory (if allocated)
			  */
			~TKDTreeData();

			/** Free memory (if allocated)
			  */
			void clear();

			ANNkd_tree		*m_pDataTree;
			ANNpointArray 	m_DataPoints;
			ANNdist 		m_NearNeighbourDistances[10];
			ANNidx			m_NearNeighbourIndices[10];
			ANNpoint 		m_QueryPoint;
			size_t 			m_nTreeSize;
			size_t 			m_nDim;
			size_t 			m_nk;
		};

		mutable TKDTreeData KDTreeData;

		/** Private method to construct the KD-tree (if required)
		  */
		void build_kdTree2D() const;

		/** Private method to construct the KD-tree (if required)
		  */
		void build_kdTree3D() const;

	 public:
		 /** Constructor
		   */
		 CPointsMap();

		 /** Virtual destructor.
		   */
		 virtual ~CPointsMap();


		/** KD Tree-based search for the closest point (only ONE) to some given 2D coordinates.
		  *  This method automatically build the "KDTreeData" structure when:
		  *		- It is called for the first time
		  *		- The map has changed
		  *		- The KD-tree was build for 3D.
		  *
		  * \param x0  The X coordinate of the query.
		  * \param y0  The Y coordinate of the query.
		  * \param out_x The X coordinate of the found closest correspondence.
		  * \param out_y The Y coordinate of the found closest correspondence.
		  * \param out_dist_sqr The square distance between the query and the returned point.
		  *
		  * \return The index of the closest point in the map array.
		  *  \sa kdTreeClosestPoint3D, kdTreeTwoClosestPoint2D
		  */
		size_t kdTreeClosestPoint2D(
			float   x0,
			float   y0,
			float   	  &out_x,
			float   	  &out_y,
			float		  &out_dist_sqr
			) const;

		inline size_t kdTreeClosestPoint2D(const TPoint2D &p0,TPoint2D &pOut,float &outDistSqr) const	{
			float dmy1,dmy2;
			size_t res=kdTreeClosestPoint2D(static_cast<float>(p0.x),static_cast<float>(p0.y),dmy1,dmy2,outDistSqr);
			pOut.x=dmy1;
			pOut.y=dmy2;
			return res;
		}

		/** Like kdTreeClosestPoint2D, but just return the square error from some point to its closest neighbor.
		  */
		float kdTreeClosestPoint2DsqrError(
			float   x0,
			float   y0 ) const;

		inline float kdTreeClosestPoint2DsqrError(const TPoint2D &p0) const	{
			return kdTreeClosestPoint2DsqrError(static_cast<float>(p0.x),static_cast<float>(p0.y));
		}

		/** Returns the square distance from the 2D point (x0,y0) to the closest correspondence in the map.
		  */
		virtual float squareDistanceToClosestCorrespondence(
			float   x0,
			float   y0 ) const;

		inline float squareDistanceToClosestCorrespondenceT(const TPoint2D &p0) const	{
			return squareDistanceToClosestCorrespondence(static_cast<float>(p0.x),static_cast<float>(p0.y));
		}


		/** KD Tree-based search for the TWO closest point to some given 2D coordinates.
		  *  This method automatically build the "KDTreeData" structure when:
		  *		- It is called for the first time
		  *		- The map has changed
		  *		- The KD-tree was build for 3D.
		  *
		  * \param x0  The X coordinate of the query.
		  * \param y0  The Y coordinate of the query.
		  * \param out_x1 The X coordinate of the first correspondence.
		  * \param out_y1 The Y coordinate of the first correspondence.
		  * \param out_x2 The X coordinate of the second correspondence.
		  * \param out_y2 The Y coordinate of the second correspondence.
		  * \param out_dist_sqr1 The square distance between the query and the first returned point.
		  * \param out_dist_sqr2 The square distance between the query and the second returned point.
		  *
		  *  \sa kdTreeClosestPoint2D
		  */
		void kdTreeTwoClosestPoint2D(
			float   x0,
			float   y0,
			float   	  &out_x1,
			float   	  &out_y1,
			float   	  &out_x2,
			float   	  &out_y2,
			float		  &out_dist_sqr1,
			float		  &out_dist_sqr2 ) const;

		inline void kdTreeTwoClosestPoint2D(const TPoint2D &p0,TPoint2D &pOut1,TPoint2D &pOut2,float &outDistSqr1,float &outDistSqr2) const	{
			float dmy1,dmy2,dmy3,dmy4;
			kdTreeTwoClosestPoint2D(p0.x,p0.y,dmy1,dmy2,dmy3,dmy4,outDistSqr1,outDistSqr2);
			pOut1.x=static_cast<double>(dmy1);
			pOut1.y=static_cast<double>(dmy2);
			pOut2.x=static_cast<double>(dmy3);
			pOut2.y=static_cast<double>(dmy4);
		}

		/** KD Tree-based search for the N closest point to some given 2D coordinates.
		  *  This method automatically build the "KDTreeData" structure when:
		  *		- It is called for the first time
		  *		- The map has changed
		  *		- The KD-tree was build for 3D.
		  *
		  * \param x0  The X coordinate of the query.
		  * \param y0  The Y coordinate of the query.
		  * \param N The number of closest points to search.
		  * \param out_x The vector containing the X coordinates of the correspondences.
		  * \param out_y The vector containing the Y coordinates of the correspondences.
		  * \param out_dist_sqr The vector containing the square distance between the query and the returned points.
		  *
		  * \return The list of indices
		  *  \sa kdTreeClosestPoint2D
		  *  \sa kdTreeTwoClosestPoint2D
		  */
		std::vector<size_t> kdTreeNClosestPoint2D(
			float			x0,
			float			y0,
			unsigned int  N,
			std::vector<float>  &out_x,
			std::vector<float>  &out_y,
			std::vector<float>  &out_dist_sqr ) const;

		inline std::vector<size_t> kdTreeNClosestPoint2D(const TPoint2D &p0,unsigned int N,std::vector<TPoint2D> &pOut,std::vector<float> &outDistSqr) const	{
			std::vector<float> dmy1,dmy2;
			std::vector<size_t> res=kdTreeNClosestPoint2D(static_cast<float>(p0.x),static_cast<float>(p0.y),N,dmy1,dmy2,outDistSqr);
			pOut.resize(dmy1.size());
			for (size_t i=0;i<dmy1.size();i++)	{
				pOut[i].x=static_cast<double>(dmy1[i]);
				pOut[i].y=static_cast<double>(dmy2[i]);
			}
			return res;
		}

		/** KD Tree-based search for the N closest point to some given 2D coordinates and returns their indexes.
		  *  This method automatically build the "KDTreeData" structure when:
		  *		- It is called for the first time
		  *		- The map has changed
		  *		- The KD-tree was build for 3D.
		  *
		  * \param x0  The X coordinate of the query.
		  * \param y0  The Y coordinate of the query.
		  * \param N The number of closest points to search.
		  * \param out_idx The indexes of the found closest correspondence.
		  * \param out_dist_sqr The square distance between the query and the returned point.
		  *
		  *  \sa kdTreeClosestPoint2D
		  */
		void kdTreeNClosestPoint2DIdx(
			float			x0,
			float			y0,
			unsigned int  N,
			std::vector<int>	&out_idx,
			std::vector<float>  &out_dist_sqr ) const;

		inline void kdTreeNClosestPoint2DIdx(const TPoint2D &p0,unsigned int N,std::vector<int> &outIdx,std::vector<float> &outDistSqr) const	{
			return kdTreeNClosestPoint2DIdx(static_cast<float>(p0.x),static_cast<float>(p0.y),N,outIdx,outDistSqr);
		}

  		/** KD Tree-based search for the closest point (only ONE) to some given 3D coordinates.
		  *  This method automatically build the "KDTreeData" structure when:
		  *		- It is called for the first time
		  *		- The map has changed
		  *		- The KD-tree was build for 2D.
		  *
		  * \param x0  The X coordinate of the query.
		  * \param y0  The Y coordinate of the query.
		  * \param z0  The Z coordinate of the query.
		  * \param out_x The X coordinate of the found closest correspondence.
		  * \param out_y The Y coordinate of the found closest correspondence.
		  * \param out_z The Z coordinate of the found closest correspondence.
		  * \param out_dist_sqr The square distance between the query and the returned point.
		  *
		  * \return The index of the closest point in the map array.
		  *  \sa kdTreeClosestPoint2D
		  */
		size_t kdTreeClosestPoint3D(
			float   x0,
			float   y0,
			float   z0,
			float   	  &out_x,
			float   	  &out_y,
			float   	  &out_z,
			float		  &out_dist_sqr
			) const;

		inline size_t kdTreeClosestPoint3D(const TPoint3D &p0,TPoint3D &pOut,float &outDistSqr) const	{
			float dmy1,dmy2,dmy3;
			size_t res=kdTreeClosestPoint3D(static_cast<float>(p0.x),static_cast<float>(p0.y),static_cast<float>(p0.z),dmy1,dmy2,dmy3,outDistSqr);
			pOut.x=static_cast<double>(dmy1);
			pOut.y=static_cast<double>(dmy2);
			pOut.z=static_cast<double>(dmy3);
			return res;
		}

		/** KD Tree-based search for the N closest points to some given 3D coordinates.
		  *  This method automatically build the "KDTreeData" structure when:
		  *		- It is called for the first time
		  *		- The map has changed
		  *		- The KD-tree was build for 2D.
		  *
		  * \param x0  The X coordinate of the query.
		  * \param y0  The Y coordinate of the query.
		  * \param z0  The Z coordinate of the query.
		  * \param N The number of closest points to search.
		  * \param out_x The vector containing the X coordinates of the correspondences.
		  * \param out_y The vector containing the Y coordinates of the correspondences.
		  * \param out_z The vector containing the Z coordinates of the correspondences.
		  * \param out_dist_sqr The vector containing the square distance between the query and the returned points.
		  *
		  *  \sa kdTreeNClosestPoint2D
		  */
		void kdTreeNClosestPoint3D(
			float			x0,
			float			y0,
			float			z0,
			unsigned int  N,
			std::vector<float>  &out_x,
			std::vector<float>  &out_y,
			std::vector<float>  &out_z,
			std::vector<float>  &out_dist_sqr ) const;

		inline void kdTreeNClosestPoint3D(const TPoint3D &p0,unsigned int N,std::vector<TPoint3D> &pOut,std::vector<float> &outDistSqr) const	{
			std::vector<float> dmy1,dmy2,dmy3;
			kdTreeNClosestPoint3D(static_cast<float>(p0.x),static_cast<float>(p0.y),static_cast<float>(p0.z),N,dmy1,dmy2,dmy3,outDistSqr);
			pOut.resize(dmy1.size());
			for (size_t i=0;i<dmy1.size();i++)	{
				pOut[i].x=static_cast<double>(dmy1[i]);
				pOut[i].y=static_cast<double>(dmy2[i]);
				pOut[i].z=static_cast<double>(dmy3[i]);
			}
		}

		/** KD Tree-based search for the N closest point to some given 3D coordinates and returns their indexes.
		  *  This method automatically build the "KDTreeData" structure when:
		  *		- It is called for the first time
		  *		- The map has changed
		  *		- The KD-tree was build for 2D.
		  *
		  * \param x0  The X coordinate of the query.
		  * \param y0  The Y coordinate of the query.
		  * \param z0  The Z coordinate of the query.
		  * \param N The number of closest points to search.
		  * \param out_idx The indexes of the found closest correspondence.
		  * \param out_dist_sqr The square distance between the query and the returned point.
		  *
		  *  \sa kdTreeClosestPoint2D
		  */
		void kdTreeNClosestPoint3DIdx(
			float			x0,
			float			y0,
			float			z0,
			unsigned int  N,
			std::vector<int>	&out_idx,
			std::vector<float>  &out_dist_sqr ) const;

		inline void kdTreeNClosestPoint3DIdx(const TPoint3D &p0,unsigned int N,std::vector<int> &outIdx,std::vector<float> &outDistSqr) const	{
			kdTreeNClosestPoint3DIdx(static_cast<float>(p0.x),static_cast<float>(p0.y),static_cast<float>(p0.z),N,outIdx,outDistSqr);
		}

		 /** With this struct options are provided to the observation insertion process.
		  * \sa CObservation::insertIntoPointsMap
		  */
		 struct MAPS_IMPEXP TInsertionOptions : public utils::CLoadableOptions
		 {
			/** Initilization of default parameters
			 */
			TInsertionOptions( );
			virtual ~TInsertionOptions() {}

			/** See utils::CLoadableOptions
			  */
			void  loadFromConfigFile(
				const mrpt::utils::CConfigFileBase  &source,
				const std::string &section);

			/** See utils::CLoadableOptions
			  */
			void  dumpToTextStream(CStream	&out) const;


			/** The minimum distance between points (in 3D): If two points are too close, one of them is not inserted into the map. Default is 0.02 meters.
			  */
			float	minDistBetweenLaserPoints;

			/** Applicable to "loadFromRangeScan" only! If set to false, the points from the scan are loaded, clearing all previous content. Default is false.
			  */
			bool	addToExistingPointsMap;

			/** If set to true, far points (<1m) are interpolated with samples at "minDistSqrBetweenLaserPoints" intervals (Default is false).
			  */
			bool	also_interpolate;

			/** If set to false (default=true) points in the same plane as the inserted scan and inside the free space, are erased: i.e. they don't exist yet.
			  */
			bool	disableDeletion;

			/** If set to true (default=false), inserted points are "fused" with previously existent ones. This shrink the size of the points map, but its slower.
			  */
			bool	fuseWithExisting;

			/** If set to true, only HORIZONTAL (in the XY plane) measurements will be inserted in the map (Default value is false, thus 3D maps are generated).
			  * \sa	horizontalTolerance
			  */
			bool	isPlanarMap;

			/** The tolerance in rads in pitch & roll for a laser scan to be considered horizontal, considered only when isPlanarMap=true (default=0).
			  */
			float	horizontalTolerance;

            /** The maximum distance between two points to interpolate between them (ONLY when also_interpolate=true)
			  */
			float	maxDistForInterpolatePoints;

		 };

		TInsertionOptions insertionOptions; //!< The options used when inserting observations in the map

		 /** Options used when evaluating "computeObservationLikelihood" in the derived classes.
		  * \sa CObservation::computeObservationLikelihood
		  */
		 struct MAPS_IMPEXP TLikelihoodOptions: public utils::CLoadableOptions
		 {
			/** Initilization of default parameters
			 */
			TLikelihoodOptions( );
			virtual ~TLikelihoodOptions() {}

			/** See utils::CLoadableOptions */
			void  loadFromConfigFile(
				const mrpt::utils::CConfigFileBase  &source,
				const std::string &section);

			/** See utils::CLoadableOptions */
			void  dumpToTextStream(CStream	&out) const;

			void writeToStream(CStream &out) const;		//!< Binary dump to stream - for usage in derived classes' serialization
			void readFromStream(CStream &in);			//!< Binary dump to stream - for usage in derived classes' serialization

			double 		sigma_dist; //!< Sigma (standard deviation, in meters) of the exponential used to model the likelihood (default= 0.5meters)
			double 		max_corr_distance; //!< Maximum distance in meters to consider for the numerator divided by "sigma_dist", so that each point has a minimum (but very small) likelihood to avoid underflows (default=1.0 meters)
			uint32_t	decimation; //!< Speed up the likelihood computation by considering only one out of N rays (default=10)
		 };

		 TLikelihoodOptions  likelihoodOptions;

		 /** Virtual assignment operator, to be implemented in derived classes.
		   */
		 virtual void  copyFrom(const CPointsMap &obj) = 0;

		/** Insert the contents of another map into this one, fusing the previous content with the new one.
		 *    This means that points very close to existing ones will be "fused", rather than "added". This prevents
		 *     the unbounded increase in size of these class of maps.
		 *		NOTICE that "otherMap" is neither translated nor rotated here, so if this is desired it must done
		 *		 before calling this method.
		 * \param otherMap The other map whose points are to be inserted into this one.
		 * \param minDistForFuse Minimum distance (in meters) between two points, each one in a map, to be considered the same one and be fused rather than added.
		 * \param notFusedPoints If a pointer is supplied, this list will contain at output a list with a "bool" value per point in "this" map. This will be false/true according to that point having been fused or not.
		 */
		virtual void  fuseWith(
			CPointsMap			*otherMap,
			float				minDistForFuse  = 0.02f,
			std::vector<bool>	*notFusedPoints = NULL) = 0;

		/** Transform the range scan into a set of cartessian coordinated
		  *	 points. The options in "insertionOptions" are considered in this method.
		  * \param rangeScan The scan to be inserted into this map
		  * \param robotPose The robot 3D pose, default to (0,0,0|0deg,0deg,0deg). It is used to compute the sensor pose relative to the robot actual pose. Recall sensor pose is embeded in the observation class.
		  *
		  *   NOTE: Only ranges marked as "valid=true" in the observation will be inserted
		  *
		  * \sa CObservation2DRangeScan
		  */
		virtual void  loadFromRangeScan(
				const CObservation2DRangeScan &rangeScan,
				const CPose3D				  *robotPose = NULL ) = 0;

		/** Load from a text file. In each line there are a point coordinates.
		 *   Returns false if any error occured, true elsewere.
		 */
		virtual bool  load2D_from_text_file(std::string file) = 0;

		/** Load from a text file. In each line there are a point coordinates.
		 *   Returns false if any error occured, true elsewere.
		 */
		virtual bool  load3D_from_text_file(std::string file) = 0;

		/**  Save to a text file. In each line there are a point coordinates.
		 *		Returns false if any error occured, true elsewere.
		 */
		bool  save2D_to_text_file(const std::string &file) const;

		/** Save to a text file. In each line there are a point coordinates.
		 *     Returns false if any error occured, true elsewere.
		 */
		bool  save3D_to_text_file(const std::string &file)const;

		/** This virtual method saves the map to a file "filNamePrefix"+< some_file_extension >, as an image or in any other applicable way (Notice that other methods to save the map may be implemented in classes implementing this virtual interface).
		  */
		void  saveMetricMapRepresentationToFile(
			const std::string	&filNamePrefix
			)const
		{
			std::string		fil( filNamePrefix + std::string(".txt") );
			save3D_to_text_file( fil );
		}


		/** Returns the number of stored points in the map.
		 */
		size_t size() const;

		/** Returns the number of stored points in the map (DEPRECATED, use "size()" instead better)
		 */
		size_t getPointsCount() const;

		/** Access to a given point from map, as a 2D point. First index is 0.
		 * \return The return value is the weight of the point (the times it has been fused)
		 * \exception Throws std::exception on index out of bound.
		 */
		unsigned long  getPoint(size_t index,CPoint2D &p) const;

		/** Access to a given point from map, as a 3D point. First index is 0.
		 * \return The return value is the weight of the point (the times it has been fused)
		 * \exception Throws std::exception on index out of bound.
		 */
		unsigned long  getPoint(size_t index,CPoint3D &p) const;

		/** Access to a given point from map, as a 3D point. First index is 0.
		 * \return The return value is the weight of the point (the times it has been fused)
		 * \exception Throws std::exception on index out of bound.
		 */
		unsigned long  getPoint(size_t index,mrpt::math::TPoint3D &p) const;

		/** Access to a given point from map, as a 2D point. First index is 0.
		 * \return The return value is the weight of the point (the times it has been fused)
		 * \exception Throws std::exception on index out of bound.
		 */
		unsigned long  getPoint(size_t index,mrpt::math::TPoint2D &p) const;

		/** Access to a given point from map. First index is 0.
		 * \return The return value is the weight of the point (the times it has been fused)
		 * \exception Throws std::exception on index out of bound.
		 */
		unsigned long  getPoint(size_t index,float &x,float &y) const;

		/** Access to a given point from map. First index is 0.
		 * \return The return value is the weight of the point (the times it has been fused)
		 * \exception Throws std::exception on index out of bound.
		 */
		unsigned long  getPoint(size_t index,float &x,float &y,float &z) const;


		/** Access to a given point from map, and its colors, if the map defines them (othersise, R=G=B=1.0). First index is 0.
		 * \return The return value is the weight of the point (the times it has been fused)
		 * \exception Throws std::exception on index out of bound.
		 */
		virtual void getPoint( size_t index, float &x, float &y, float &z, float &R, float &G, float &B ) const
		{
			getPoint(index,x,y,z);
			R=G=B=1;
		}

		/** Returns true if the point map has a color field for each point */
		virtual bool hasColorPoints() const { return false; }

		/** Changes a given point from map, as a 2D point. First index is 0.
		 * \exception Throws std::exception on index out of bound.
		 */
		virtual void  setPoint(size_t index,CPoint2D &p)=0;

		/** Changes a given point from map, as a 3D point. First index is 0.
		 * \exception Throws std::exception on index out of bound.
		 */
		virtual void  setPoint(size_t index,CPoint3D &p)=0;

		/** Changes a given point from map. First index is 0.
		 * \exception Throws std::exception on index out of bound.
		 */
		virtual void  setPoint(size_t index,float x, float y)=0;

		/** Changes a given point from map. First index is 0.
		 * \exception Throws std::exception on index out of bound.
		 */
		virtual void  setPoint(size_t index,float x, float y, float z)=0;

		/** Provides a direct access to points buffer, or NULL if there is no points in the map.
		  */
		void  getPointsBuffer( size_t &outPointsCount, const float *&xs, const float *&ys, const float *&zs ) const;

		/** Provides a direct access to a read-only reference of the internal point buffer. \sa getAllPoints */
		inline const std::vector<float> & getPointsBufferRef_x() const { return x; }
		/** Provides a direct access to a read-only reference of the internal point buffer. \sa getAllPoints */
		inline const std::vector<float> & getPointsBufferRef_y() const { return y; }
		/** Provides a direct access to a read-only reference of the internal point buffer. \sa getAllPoints */
		inline const std::vector<float> & getPointsBufferRef_z() const { return z; }

		/** Returns a copy of the 2D/3D points as a std::vector of float coordinates.
		  * If decimation is greater than 1, only 1 point out of that number will be saved in the output, effectively performing a subsampling of the points.
		  * \sa getPointsBufferRef_x, getPointsBufferRef_y, getPointsBufferRef_z
		  */
		void  getAllPoints( std::vector<float> &xs, std::vector<float> &ys,std::vector<float> &zs, size_t decimation = 1 ) const;

		inline void getAllPoints(std::vector<TPoint3D> &ps,size_t decimation=1) const	{
			std::vector<float> dmy1,dmy2,dmy3;
			getAllPoints(dmy1,dmy2,dmy3,decimation);
			ps.resize(dmy1.size());
			for (size_t i=0;i<dmy1.size();i++)	{
				ps[i].x=static_cast<double>(dmy1[i]);
				ps[i].y=static_cast<double>(dmy2[i]);
				ps[i].z=static_cast<double>(dmy3[i]);
			}
		}

		/** Returns a copy of the 2D/3D points as a std::vector of float coordinates.
		  * If decimation is greater than 1, only 1 point out of that number will be saved in the output, effectively performing a subsampling of the points.
		  * \sa setAllPoints
		  */
		void  getAllPoints( std::vector<float> &xs, std::vector<float> &ys, size_t decimation = 1 ) const;

		inline void getAllPoints(std::vector<TPoint2D> &ps,size_t decimation=1) const	{
			std::vector<float> dmy1,dmy2;
			getAllPoints(dmy1,dmy2,decimation);
			ps.resize(dmy1.size());
			for (size_t i=0;i<dmy1.size();i++)	{
				ps[i].x=static_cast<double>(dmy1[i]);
				ps[i].y=static_cast<double>(dmy2[i]);
			}
		}

		/** Provides a way to insert individual points into the map */
		virtual void  insertPoint( float x, float y, float z = 0 ) = 0;

		/** Provides a way to insert individual points into the map */
		inline void  insertPoint( const CPoint3D &p ) {
			insertPoint(p.x(),p.y(),p.z());
		}

		/** Provides a way to insert individual points into the map */
		inline void  insertPoint( const mrpt::math::TPoint3D &p ) {
			insertPoint(p.x,p.y,p.z);
		}

		/** Reserves memory for a given number of points: the size of the map does not change, it only reserves the memory.
		  *  This is useful for situations where it is approximately known the final size of the map. This method is more
		  *  efficient than constantly increasing the size of the buffers. Refer to the STL C++ library's "reserve" methods.
		  */
		virtual void reserve(size_t newLength) = 0;

		/** Set all the points at once from vectors with X,Y and Z coordinates. \sa getAllPoints */
		virtual void setAllPoints(const vector_float &X,const vector_float &Y,const vector_float &Z) = 0;

		/** Set all the points at once from vectors with X and Y coordinates (Z=0). \sa getAllPoints */
		virtual void setAllPoints(const vector_float &X,const vector_float &Y) = 0;

		/** Delete points out of the given "z" axis range have been removed.
		  */
		void  clipOutOfRangeInZ(float zMin, float zMax);

		/** Delete points which are more far than "maxRange" away from the given "point".
		  */
		void  clipOutOfRange(const CPoint2D	&point, float maxRange);

		/** Remove from the map the points marked in a bool's array as "true".
		  *
		  * \exception std::exception If mask size is not equal to points count.
		  */
		virtual void  applyDeletionMask( std::vector<bool> &mask ) = 0;

		/** Computes the matchings between this and another 2D/3D points map.
		   This includes finding:
				- The set of points pairs in each map
				- The mean squared distance between corresponding pairs.
		   This method is the most time critical one into the ICP algorithm.

		 * \param  otherMap					  [IN] The other map to compute the matching with.
		 * \param  otherMapPose				  [IN] The pose of the other map as seen from "this".
		 * \param  maxDistForCorrespondence [IN] Maximum 2D distance between two points to be matched.
		 * \param  maxAngularDistForCorrespondence [IN] Maximum angular distance in radians to allow far points to be matched.
		 * \param  angularDistPivotPoint      [IN] The point from which to measure the "angular distances"
		 * \param  correspondences			  [OUT] The detected matchings pairs.
		 * \param  correspondencesRatio		  [OUT] The number of correct correspondences.
		 * \param  sumSqrDist				  [OUT] The sum of all matched points squared distances.If undesired, set to NULL, as default.
		 * \param  covariance				  [OUT] The resulting matching covariance 3x3 matrix, or NULL if undesired.
		 * \param  onlyKeepTheClosest		  [OUT] Returns only the closest correspondence (default=false)
		 *
		 * \sa computeMatching3DWith
		 */
		void  computeMatchingWith2D(
				const CMetricMap						*otherMap,
				const CPose2D							&otherMapPose,
				float									maxDistForCorrespondence,
				float									maxAngularDistForCorrespondence,
				const CPose2D							&angularDistPivotPoint,
				TMatchingPairList						&correspondences,
				float									&correspondencesRatio,
				float									*sumSqrDist	= NULL,
				bool									onlyKeepTheClosest = false,
				bool									onlyUniqueRobust = false ) const;

		/** Computes the matchings between this and another 3D points map - method used in 3D-ICP.
		   This method finds the set of point pairs in each map.

		   The method is the most time critical one into the ICP algorithm.

		 * \param  otherMap					  [IN] The other map to compute the matching with.
		 * \param  otherMapPose				  [IN] The pose of the other map as seen from "this".
		 * \param  maxDistForCorrespondence   [IN] Maximum 2D linear distance between two points to be matched.
		 * \param  maxAngularDistForCorrespondence [IN] In radians: The aim is to allow larger distances to more distant correspondences.
		 * \param  angularDistPivotPoint      [IN] The point used to calculate distances from in both maps.
		 * \param  correspondences			  [OUT] The detected matchings pairs.
		 * \param  correspondencesRatio		  [OUT] The ratio [0,1] of points in otherMap with at least one correspondence.
		 * \param  sumSqrDist				  [OUT] The sum of all matched points squared distances.If undesired, set to NULL, as default.
		 * \param  onlyKeepTheClosest         [IN] If set to true, only the closest correspondence will be returned. If false (default) all are returned.
		 *
		 * \sa compute3DMatchingRatio
		 */
		void  computeMatchingWith3D(
			const CMetricMap						*otherMap,
			const CPose3D							&otherMapPose,
			float									maxDistForCorrespondence,
			float									maxAngularDistForCorrespondence,
			const CPoint3D							&angularDistPivotPoint,
			TMatchingPairList						&correspondences,
			float									&correspondencesRatio,
			float									*sumSqrDist	= NULL,
			bool									onlyKeepTheClosest = true,
			bool									onlyUniqueRobust = false ) const;


		/** Replace each point \f$ p_i \f$ by \f$ p'_i = b \oplus p_i \f$ (pose compounding operator).
		  */
		void   changeCoordinatesReference(const CPose2D &b);

		/** Replace each point \f$ p_i \f$ by \f$ p'_i = b \oplus p_i \f$ (pose compounding operator).
		  */
		void   changeCoordinatesReference(const CPose3D &b);

		/** Copy all the points from "other" map to "this", replacing each point \f$ p_i \f$ by \f$ p'_i = b \oplus p_i \f$ (pose compounding operator).
		  */
		void   changeCoordinatesReference(const CPointsMap &other, const CPose3D &b);

		/** Returns true if the map is empty/no observation has been inserted.
		   */
		virtual bool  isEmpty() const;

		/** STL-like method to check whether the map is empty: */
		inline bool  empty() const { return isEmpty(); }

		/** Returns a 3D object representing the map.
		  *  The color of the points is given by the static variables: COLOR_3DSCENE_R,COLOR_3DSCENE_G,COLOR_3DSCENE_B
		  */
		virtual void  getAs3DObject ( mrpt::opengl::CSetOfObjectsPtr	&outObj ) const;


		/** Computes the ratio in [0,1] of correspondences between "this" and the "otherMap" map, whose 6D pose relative to "this" is "otherMapPose"
		 *   In the case of a multi-metric map, this returns the average between the maps. This method always return 0 for grid maps.
		 * \param  otherMap					  [IN] The other map to compute the matching with.
		 * \param  otherMapPose				  [IN] The 6D pose of the other map as seen from "this".
		 * \param  minDistForCorr			  [IN] The minimum distance between 2 non-probabilistic map elements for counting them as a correspondence.
		 * \param  minMahaDistForCorr		  [IN] The minimum Mahalanobis distance between 2 probabilistic map elements for counting them as a correspondence.
		 *
		 * \return The matching ratio [0,1]
		 * \sa computeMatchingWith2D
		 */
		float  compute3DMatchingRatio(
				const CMetricMap						*otherMap,
				const CPose3D							&otherMapPose,
				float									minDistForCorr = 0.10f,
				float									minMahaDistForCorr = 2.0f
				) const;

		/** This method returns the largest distance from the origin to any of the points, such as a sphere centered at the origin with this radius cover ALL the points in the map (the results are buffered, such as, if the map is not modified, the second call will be much faster than the first one).
		  */
		float  getLargestDistanceFromOrigin() const;

		/** Computes the bounding box of all the points, or (0,0 ,0,0, 0,0) if there are no points. */
		void boundingBox( float &min_x,float &max_x,float &min_y,float &max_y,float &min_z,float &max_z ) const;

		inline void boundingBox(TPoint3D &pMin,TPoint3D &pMax) const	{
			float dmy1,dmy2,dmy3,dmy4,dmy5,dmy6;
			boundingBox(dmy1,dmy2,dmy3,dmy4,dmy5,dmy6);
			pMin.x=dmy1;
			pMin.y=dmy2;
			pMin.z=dmy3;
			pMax.x=dmy4;
			pMax.y=dmy5;
			pMax.z=dmy6;
		}

		void extractCylinder( const CPoint2D &center, const double radius, const double zmin, const double zmax, CPointsMap *outMap );


		/** The color [0,1] of points when extracted from getAs3DObject (default=blue) */
		static float COLOR_3DSCENE_R;
		static float COLOR_3DSCENE_G;
		static float COLOR_3DSCENE_B;


		/** Computes the likelihood of taking a given observation from a given pose in the world being modeled with this map.
		 * \param takenFrom The robot's pose the observation is supposed to be taken from.
		 * \param obs The observation.
		 * \return This method returns a likelihood in the range [0,1].
		 *
		 * \sa Used in particle filter algorithms, see: CMultiMetricMapPDF
		 * \note In CPointsMap this method is virtual so it can be redefined in derived classes, if desired.
		 */
		virtual double computeObservationLikelihood( const CObservation *obs, const CPose3D &takenFrom );

	}; // End of class def.

	} // End of namespace
} // End of namespace

#endif