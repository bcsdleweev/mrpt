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
#ifndef  TCamera_H
#define  TCamera_H

#include <mrpt/math/CMatrixTemplateNumeric.h>
#include <mrpt/math/CMatrixFixedNumeric.h>
#include <mrpt/utils/CLoadableOptions.h>
#include <mrpt/utils/CConfigFileBase.h>
#include <mrpt/utils/CSerializable.h>

namespace mrpt
{
	namespace utils
	{
		using namespace mrpt::math;

		DEFINE_SERIALIZABLE_PRE_CUSTOM_BASE( TCamera, mrpt::utils::CSerializable )

		/** Structure to hold the parameters of a pinhole camera model.
		  *
		  * \sa mrpt::vision::CCamModel, the application <a href="http://www.mrpt.org/Application:camera-calib-gui" >camera-calib-gui</a> for calibrating a camera
		 */
		class BASE_IMPEXP TCamera : public mrpt::utils::CSerializable
		{
			DEFINE_SERIALIZABLE( TCamera )

		public:
			TCamera() : ncols(640), nrows(480), focalLengthMeters(0)
			{
				intrinsicParams.set_unsafe(0,0,507.808);
				intrinsicParams.set_unsafe(1,1,507.808);
				intrinsicParams.set_unsafe(0,2,356.2368);
				intrinsicParams.set_unsafe(1,2,252.9216);
				intrinsicParams.set_unsafe(2,2,1);
				for (size_t i=0;i<5;i++)
					dist[i] = 0;
			}

			/** @name Camera parameters
			    @{ */

			uint32_t			ncols,nrows;        //!< Camera resolution
			CMatrixDouble33 	intrinsicParams;    //!< Matrix of intrinsic parameters (containing the focal length and principal point coordinates)
			CArrayDouble<5> 	dist;               //!< [k1 k2 t1 t2 t3] -> k_i: parameters of radial distortion, t_i: parameters of tangential distortion (default=0)
			double  			focalLengthMeters;  //!< The focal length of the camera, in meters (can be used among 'intrinsicParams' to determine the pixel size).

			/** @} */

			/**  Save as a config block:
			  *  \code
			  *  [SECTION]
			  *  resolution = [NCOLS NROWS]
			  *  cx         = CX
			  *  cy         = CY
			  *  fx         = FX
			  *  fy         = FY
			  *  dist       = [K1 K2 T1 T2 T3]
			  *  focal_length = FOCAL_LENGTH
			  *  \endcode
			  */
			void saveToConfigFile( const std::string &section, mrpt::utils::CConfigFileBase &cfg ) const;

			/**  Load all the params from a config source, in the format used in saveToConfigFile(), that is:
			  *
			  *  \code
			  *  [SECTION]
			  *  resolution = [NCOLS NROWS]
			  *  cx         = CX
			  *  cy         = CY
			  *  fx         = FX
			  *  fy         = FY
			  *  dist       = [K1 K2 T1 T2 T3]
			  *  focal_length = FOCAL_LENGTH  [optional field]
			  *  \endcode
			  *  \exception std::exception on missing fields
			  */
			void loadFromConfigFile(const std::string &section, const mrpt::utils::CConfigFileBase &cfg );


			/** Set the matrix of intrinsic params of the camera from the individual values of focal length and principal point coordinates (in pixels)
			  */
			inline void setIntrinsicParamsFromValues ( double fx, double fy, double cx, double cy )
			{
				intrinsicParams.set_unsafe( 0, 0, fx );
				intrinsicParams.set_unsafe( 1, 1, fy );
				intrinsicParams.set_unsafe( 0, 2, cx );
				intrinsicParams.set_unsafe( 1, 2, cy );
			}

			/** Get the vector of distortion params of the camera  */
			inline void getDistortionParamsVector ( CMatrixDouble15 &distParVector ) const
			{
				for (size_t i=0;i<5;i++)
					distParVector.set_unsafe(0,i, dist[i]);
			}

			/** Get a vector with the distortion params of the camera  */
			inline vector_double getDistortionParamsAsVector () const {
				mrpt::vector_double  v(5);
				for (size_t i=0;i<5;i++)
					v[i] = dist[i];
				return v;
			}

			/** Set the whole vector of distortion params of the camera */
			void setDistortionParamsVector( const CMatrixDouble15 &distParVector )
			{
				for (size_t i=0;i<5;i++)
					dist[i] = distParVector.get_unsafe(0,i);
			}

			/** Set the whole vector of distortion params of the camera from a 4 or 5-vector */
			template <class VECTORLIKE>
			void setDistortionParamsVector( const VECTORLIKE &distParVector )
			{
				ASSERT_(distParVector.size()==4 || distParVector.size()==5)
				dist[4] = 0; // Default value
				for (size_t i=0;i<distParVector.size();i++)
					dist[i] = distParVector[i];
			}

			/** Set the vector of distortion params of the camera from the individual values of the distortion coefficients
			  */
			inline void setDistortionParamsFromValues( double k1, double k2, double p1, double p2, double p3 = 0 )
			{
				dist[0] = k1;
				dist[1] = k2;
				dist[2] = p1;
				dist[3] = p2;
				dist[4] = p3;
			}

			/** Get the value of the principal point x-coordinate (in pixels).
			  */
			inline double cx() const { return intrinsicParams.get_unsafe(0,2); }
			/** Get the value of the principal point y-coordinate  (in pixels).
			  */
			inline double cy() const { return intrinsicParams.get_unsafe(1,2); }
			/** Get the value of the focal length x-value (in pixels).
			  */
			inline double fx() const { return intrinsicParams.get_unsafe(0,0); }
			/** Get the value of the focal length y-value (in pixels).
			  */
			inline double fy() const { return intrinsicParams.get_unsafe(1,1); }

			/** Get the value of the k1 distortion parameter.
			  */
			inline double k1() const { return dist[0]; }
			/** Get the value of the k2 distortion parameter.
			  */
			inline double k2() const { return dist[1]; }
			/** Get the value of the p1 distortion parameter.
			  */
			inline double p1() const { return dist[2]; }
			/** Get the value of the p2 distortion parameter.
			  */
			inline double p2() const { return dist[3]; }
			/** Get the value of the p3 distortion parameter.
			  */
			inline double p3() const { return dist[4]; }

		};

	} // End of namespace
} // end of namespace
#endif