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

#ifndef CQuaternion_H
#define CQuaternion_H

#include <mrpt/math/CMatrixTemplateNumeric.h>
#include <mrpt/math/CVectorTemplate.h>
#include <mrpt/math/CArray.h>

namespace mrpt
{
	namespace math
	{

		// For use with a constructor of CQuaternion
		#define UNINITIALIZED_QUATERNION   false,false

		/** A quaternion, which can represent a 3D rotation as pair \f$ (r,\mathbf{u}) \f$, with a real part "r" and a 3D vector \f$ \mathbf{u} = (x,y,z) \f$, or alternatively, q = r + ix + jy + kz.
		 *
		 *  The elements of the quaternion can be accessed by either:
		 *		- r(), x(), y(), z(), or
		 *		- the operator [] with indices running from 0 (=r) to 3 (=z).
		 *
		 *  Users will usually employ the typedef "CQuaternionDouble" instead of this template.
		 *
		 * For more information about quaternions, see:
		 *  - http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
		 *  - http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
		 *
		 * \sa mrpt::poses::CPose3D
		 */
		template <class T>
		class CQuaternion : public CArrayNumeric<T,4>
		{
			typedef CArrayNumeric<T,4> BASE;
		public:
	/* @{ Constructors
	 */

		/**	Can be used with UNINITIALIZED_QUATERNION as argument, does not initialize the 4 elements of the quaternion (use this constructor when speed is critical). */
		inline CQuaternion(bool, bool ) { }

		/**	Default constructor: construct a (1, (0,0,0) ) quaternion representing no rotation. */
		inline CQuaternion()
		{
			(*this)[0] = 1;
			(*this)[1] = 0;
			(*this)[2] = 0;
			(*this)[3] = 0;
		}

		/**	Construct a quaternion from its parameters 'r', 'x', 'y', 'z', with q = r + ix + jy + kz. */
		inline CQuaternion(const T r,const T x,const T y,const T z)
		{
			(*this)[0] = r;
			(*this)[1] = x;
			(*this)[2] = y;
			(*this)[3] = z;
			ASSERTDEBMSG_(std::abs(normSqr()-1.0)<1e-5, format("Initialization data for quaternion is not normalized: %f %f %f %f -> sqrNorm=%f",r,x,y,z,normSqr()) );
		}

		/* @}
		 */


		inline T  r()const {return (*this)[0];}	//!< Return r coordinate of the quaternion
		inline T  x()const {return (*this)[1];}	//!< Return x coordinate of the quaternion
		inline T  y()const {return (*this)[2];}	//!< Return y coordinate of the quaternion
		inline T  z()const {return (*this)[3];}	//!< Return z coordinate of the quaternion
		inline void  r(const T r) {(*this)[0]=r;}	//!< Set r coordinate of the quaternion
		inline void  x(const T x) {(*this)[1]=x;}	//!< Set x coordinate of the quaternion
		inline void  y(const T y) {(*this)[2]=y;}	//!< Set y coordinate of the quaternion
		inline void  z(const T z) {(*this)[3]=z;}	//!< Set z coordinate of the quaternion

		/**	Set this quaternion to the rotation described by a 3D Rodrigues rotation vector.
		  */
		template <class ARRAYLIKE>
		void  fromRodriguesVector(const ARRAYLIKE &in)
		{
			if (in.size()!=3) THROW_EXCEPTION("Wrong Dimension in input vector for quaternion Constructor");

			const T x = in[0];
			const T y = in[1];
			const T z = in[2];
			if ((x==0)&&(y==0)&&(z==0))
			{
				(*this)[0] = 1;
				(*this)[1] = 0;
				(*this)[2] = 0;
				(*this)[3] = 0;
			}
			else
			{
				const T angle = sqrt(x*x+y*y+z*z);
				const T s = (sin(angle/2))/angle;
				const T c = cos(angle/2);
				(*this)[0] = c;
				(*this)[1] = x * s;
				(*this)[2] = y * s;
				(*this)[3] = z * s;
			}
		}

		/**	Calculate the "cross" product (or "composed rotation") of two quaternion: this = q1 x q2
		  *   After the operation, "this" will represent the composed rotations of q1 and q2 (q2 applied "after" q1).
		  */
		inline void  crossProduct(const CQuaternion &q1, const CQuaternion &q2)
		{
			this->r(  q1.r()*q2.r() - q1.x()*q2.x() - q1.y()*q2.y() - q1.z()*q2.z() );
			this->x(  q1.r()*q2.x() + q2.r()*q1.x() + q1.y()*q2.z() - q2.y()*q1.z() );
            this->y(  q1.r()*q2.y() + q2.r()*q1.y() + q1.z()*q2.x() - q2.z()*q1.x() );
			this->z(  q1.r()*q2.z() + q2.r()*q1.z() + q1.x()*q2.y() - q2.x()*q1.y() );
			this->normalize();
		}

		/** Rotate a 3D point (lx,ly,lz) -> (gx,gy,gz) as described by this quaternion
		  */
		void rotatePoint(const double lx,const double ly,const double lz, double &gx,double &gy,double &gz ) const
		{
			const double t2 = r()*x(); const double t3 = r()*y(); const double t4 = r()*z(); const double t5 =-x()*x(); const double t6 = x()*y();
			const double t7 = x()*z(); const double t8 =-y()*y(); const double t9 = y()*z(); const double t10=-z()*z();
			gx = 2*((t8+ t10)*lx+(t6 - t4)*ly+(t3+t7)*lz)+lx;
			gy = 2*((t4+  t6)*lx+(t5 +t10)*ly+(t9-t2)*lz)+ly;
			gz = 2*((t7-  t3)*lx+(t2 + t9)*ly+(t5+t8)*lz)+lz;
		}

		/** Rotate a 3D point (lx,ly,lz) -> (gx,gy,gz) as described by the inverse (conjugate) of this quaternion
		  */
		void inverseRotatePoint(const double lx,const double ly,const double lz, double &gx,double &gy,double &gz ) const
		{
			const double t2 =-r()*x(); const double t3 =-r()*y(); const double t4 =-r()*z(); const double t5 =-x()*x(); const double t6 = x()*y();
			const double t7 = x()*z(); const double t8 =-y()*y(); const double t9 = y()*z(); const double t10=-z()*z();
			gx = 2*((t8+ t10)*lx+(t6 - t4)*ly+(t3+t7)*lz)+lx;
			gy = 2*((t4+  t6)*lx+(t5 +t10)*ly+(t9-t2)*lz)+ly;
			gz = 2*((t7-  t3)*lx+(t2 + t9)*ly+(t5+t8)*lz)+lz;
		}

		/** Return the squared norm of the quaternion */
		inline double normSqr() const { return square(r()) + square(x()) + square(y()) + square(z()); }

		/**	Normalize this quaternion, so its norm becomes the unitity.
		  */
		inline void normalize()
		{
			const T qq = 1.0/sqrt( normSqr() );
			for (unsigned int i=0;i<4;i++)
				(*this)[i] *= qq;
		}

		/** Calculate the 4x4 Jacobian of the normalization operation of this quaternion.
		  *  The output matrix can be a dynamic or fixed size (4x4) matrix.
		  */
		template <class MATRIXLIKE>
		void  normalizationJacobian(MATRIXLIKE &J) const
		{
			const T n = 1.0/pow(normSqr(),T(1.5));
			J.setSize(4,4);
			J.get_unsafe(0,0)=x()*x()+y()*y()+z()*z();
			J.get_unsafe(0,1)=-r()*x();
			J.get_unsafe(0,2)=-r()*y();
			J.get_unsafe(0,3)=-r()*z();

			J.get_unsafe(1,0)=-x()*r();
			J.get_unsafe(1,1)=r()*r()+y()*y()+z()*z();
			J.get_unsafe(1,2)=-x()*y();
			J.get_unsafe(1,3)=-x()*z();

			J.get_unsafe(2,0)=-y()*r();
			J.get_unsafe(2,1)=-y()*x();
			J.get_unsafe(2,2)=r()*r()+x()*x()+z()*z();
			J.get_unsafe(2,3)=-y()*z();

			J.get_unsafe(3,0)=-z()*r();
			J.get_unsafe(3,1)=-z()*x();
			J.get_unsafe(3,2)=-z()*y();
			J.get_unsafe(3,3)=r()*r()+x()*x()+y()*y();
			J *=n;
		}

		/** Compute the Jacobian of the rotation composition operation \f$ p = f(\cdot) = q_{this} \times r \f$, that is the 4x4 matrix \f$ \frac{\partial f}{\partial q_{this} }  \f$.
		  *  The output matrix can be a dynamic or fixed size (4x4) matrix.
		  */
		template <class MATRIXLIKE>
		inline void rotationJacobian(MATRIXLIKE &J) const
		{
			J.setSize(4,4);
			J.get_unsafe(0,0)=r(); J.get_unsafe(0,1)=-x(); J.get_unsafe(0,2)=-y(); J.get_unsafe(0,3)=-z();
			J.get_unsafe(1,0)=x(); J.get_unsafe(1,1)= r(); J.get_unsafe(1,2)=-z(); J.get_unsafe(1,3)= y();
			J.get_unsafe(2,0)=y(); J.get_unsafe(2,1)= z(); J.get_unsafe(2,2)= r(); J.get_unsafe(2,3)=-x();
			J.get_unsafe(3,0)=z(); J.get_unsafe(3,1)=-y(); J.get_unsafe(3,2)= x(); J.get_unsafe(3,3)= r();
		}

		/** Calculate the 3x3 rotation matrix associated to this quaternion */
		template <class MATRIXLIKE>
        inline void  rotationMatrix(MATRIXLIKE &M) const
		{
			M.setSize(3,3);
			rotationMatrixNoResize(M);
		}

		/** Fill out the top-left 3x3 block of the given matrix with the rotation matrix associated to this quaternion (does not resize the matrix, for that, see rotationMatrix) */
		template <class MATRIXLIKE>
        inline void  rotationMatrixNoResize(MATRIXLIKE &M) const
		{
			M.get_unsafe(0,0)=r()*r()+x()*x()-y()*y()-z()*z();		M.get_unsafe(0,1)=2*(x()*y() -r()*z());			M.get_unsafe(0,2)=2*(z()*x()+r()*y());
			M.get_unsafe(1,0)=2*(x()*y()+r()*z());				M.get_unsafe(1,1)=r()*r()-x()*x()+y()*y()-z()*z();		M.get_unsafe(1,2)=2*(y()*z()-r()*x());
			M.get_unsafe(2,0)=2*(z()*x()-r()*y());				M.get_unsafe(2,1)=2*(y()*z()+r()*x());				M.get_unsafe(2,2)=r()*r()-x()*x()-y()*y()+z()*z();
		}


		/**	Return the conjugate quaternion  */
		inline void conj(CQuaternion &q_out) const
		{
			q_out.r( r() );
			q_out.x(-x() );
			q_out.y(-y() );
			q_out.z(-z() );
		}

		/**	Return the conjugate quaternion  */
		inline CQuaternion conj() const
		{
			CQuaternion q_aux;
			conj(q_aux);
			return q_aux;
		}

		/**	Return the yaw, pitch & roll angles associated to quaternion
		  *  \sa For the equations, see The MRPT Book, or see http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/Quaternions.pdf
		  *  \sa rpy_and_jacobian
		*/
		inline void rpy(T &roll, T &pitch, T &yaw) const
		{
			rpy_and_jacobian(roll,pitch,yaw,static_cast<mrpt::math::CMatrixDouble*>(NULL));
		}

		/**	Return the yaw, pitch & roll angles associated to quaternion, and (optionally) the 3x4 Jacobian of the transformation.
		  *  Note that both the angles and the Jacobian have one set of normal equations, plus other special formulas for the degenerated cases of |pitch|=90 degrees.
		  *  \sa For the equations, see The MRPT Book, or http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/Quaternions.pdf
		  * \sa rpy
		*/
		template <class MATRIXLIKE>
		void rpy_and_jacobian(T &roll, T &pitch, T &yaw, MATRIXLIKE *out_dr_dq = NULL, bool resize_out_dr_dq_to3x4 = true ) const
		{
			if (out_dr_dq && resize_out_dr_dq_to3x4)
				out_dr_dq->setSize(3,4);
			const T discr = r()*y()-x()*z();
			if (fabs(discr)>0.49999)
			{	// pitch = 90 deg
				pitch =  0.5*M_PI;
				yaw   =-2*atan2(x(),r());
				roll  = 0;
				if (out_dr_dq) {
					out_dr_dq->zeros();
					out_dr_dq->get_unsafe(0,0) = +2/x();
					out_dr_dq->get_unsafe(0,2) = -2*r()/(x()*x());
				}
			}
			else if (discr<-0.49999)
			{	// pitch =-90 deg
				pitch = -0.5*M_PI;
				yaw   =+2*atan2(x(),r());
				roll  = 0;
				if (out_dr_dq) {
					out_dr_dq->zeros();
					out_dr_dq->get_unsafe(0,0) = -2/x();
					out_dr_dq->get_unsafe(0,2) = +2*r()/(x()*x());
				}
			}
			else
			{	// Non-degenerate case:
				yaw   = atan2( 2*(r()*z()+x()*y()), 1-2*(y()*y()+z()*z()) );
				pitch = asin ( 2*discr );
				roll  = atan2( 2*(r()*x()+y()*z()), 1-2*(x()*x()+y()*y()) );
				if (out_dr_dq) {
					// Auxiliary terms:
					const double val1=(2*x()*x() + 2*y()*y() - 1);
					const double val12=square(val1);
					const double val2=(2*r()*x() + 2*y()*z());
					const double val22=square(val2);
					const double xy2 = 2*x()*y();
					const double rz2 = 2*r()*z();
					const double ry2 = 2*r()*y();
					const double val3 = (2*y()*y() + 2*z()*z() - 1);
					const double val4 = ((square(rz2 + xy2)/square(val3) + 1)*val3);
					const double val5 = (4*(rz2 + xy2))/square(val3);
					const double val6 = 1.0/(square(rz2 + xy2)/square(val3) + 1);
					const double val7 = 2.0/ sqrt(1 - square(ry2 - 2*x()*z()));
					const double val8 = (val22/val12 + 1);
					const double val9 = -2.0/val8;
					// row 1:
					out_dr_dq->get_unsafe(0,0) = -2*z()/val4;
					out_dr_dq->get_unsafe(0,1) = -2*y()/val4;
					out_dr_dq->get_unsafe(0,2) = -(2*x()/val3 - y()*val5)*val6 ;
					out_dr_dq->get_unsafe(0,3) = -(2*r()/val3 - z()*val5)*val6;
					// row 2:
					out_dr_dq->get_unsafe(1,0) = y()*val7  ;
					out_dr_dq->get_unsafe(1,1) = -z()*val7 ;
					out_dr_dq->get_unsafe(1,2) = r()*val7 ;
					out_dr_dq->get_unsafe(1,3) = -x()*val7 ;
					// row 3:
					out_dr_dq->get_unsafe(2,0) = val9*x()/val1 ;
					out_dr_dq->get_unsafe(2,1) = val9*(r()/val1 - (2*x()*val2)/val12) ;
					out_dr_dq->get_unsafe(2,2) = val9*(z()/val1 - (2*y()*val2)/val12) ;
					out_dr_dq->get_unsafe(2,3) = val9*y()/val1 ;
				}
			}
		}

		inline CQuaternion  operator * (const T &factor)
		{
			CQuaternion q = *this;
			q*=factor;
			return q;
		}

		};	// end class

		typedef CQuaternion<double> CQuaternionDouble;	//!< A quaternion of data type "double"
		typedef CQuaternion<float>  CQuaternionFloat;	//!< A quaternion of data type "float"

	}	// end namespace

} // end namespace mrpt

#endif