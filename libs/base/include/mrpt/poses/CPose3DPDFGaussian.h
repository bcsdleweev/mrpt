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
#ifndef CPose3DPDFGaussian_H
#define CPose3DPDFGaussian_H

#include <mrpt/poses/CPose3DPDF.h>
#include <mrpt/poses/CPosePDF.h>
#include <mrpt/math/CMatrixD.h>

namespace mrpt
{
namespace poses
{
	class CPosePDFGaussian;
	class CPose3DQuatPDFGaussian;

	DEFINE_SERIALIZABLE_PRE_CUSTOM_BASE( CPose3DPDFGaussian , CPose3DPDF )

	/** Declares a class that represents a Probability Density function (PDF) of a 3D pose \f$ p(\mathbf{x}) = [x ~ y ~ z ~ yaw ~ pitch ~ roll]^t \f$.
	 *
	 *   This class implements that PDF using a mono-modal Gaussian distribution. See mrpt::poses::CPose3DPDF for more details.
	 *
	 *  Uncertainty of pose composition operations (\f$ y = x \oplus u \f$) is implemented in the method "CPose3DPDFGaussian::operator+=".
	 *
	 *  For further details on implemented methods and the theory behind them, see the report:
	 *  * "6D poses as Euler angles, transformation matrices and quaternions: equivalences, compositions and uncertainty", Jose-Luis Blanco (Technical Report), 2010.
	 *
	 * \sa CPose3D, CPose3DPDF, CPose3DPDFParticles
	 */
	class BASE_IMPEXP CPose3DPDFGaussian : public CPose3DPDF
	{
		// This must be added to any CSerializable derived class:
		DEFINE_SERIALIZABLE( CPose3DPDFGaussian )

	protected:
		/** Assures the symmetry of the covariance matrix (eventually certain operations in the math-coprocessor lead to non-symmetric matrixes!)
		  */
		void  assureSymmetry();

	 public:
		 /** Default constructor
		  */
		CPose3DPDFGaussian();

		/** Constructor
		  */
		explicit CPose3DPDFGaussian( const CPose3D &init_Mean );

		/** Uninitialized constructor: leave all fields uninitialized - Call with UNINITIALIZED_POSE as argument
		  */
		CPose3DPDFGaussian(bool,bool);

		MRPT_DECLARE_DEPRECATED_FUNCTION("Deprecated: use the constructor accepting a CMatrixDouble66 instead.",
		/** *DEPRECATED* Constructor  */
		CPose3DPDFGaussian( const CPose3D &init_Mean, const CMatrixD &init_Cov )
		);

		/** Constructor  */
		CPose3DPDFGaussian( const CPose3D &init_Mean, const CMatrixDouble66 &init_Cov );

		/** Constructor from a Gaussian 2D pose PDF (sets to 0 the missing variables z,pitch, and roll).
		  */
		explicit CPose3DPDFGaussian( const CPosePDFGaussian &o );

		/** Constructor from a 6D pose PDF described as a Quaternion
		  */
		explicit CPose3DPDFGaussian( const CPose3DQuatPDFGaussian &o);

		/** The mean value
		 */
		CPose3D		mean;

		/** The 6x6 covariance matrix
		 */
		CMatrixDouble66		cov;

		 /** Returns an estimate of the pose, (the mean, or mathematical expectation of the PDF).
		   * \sa getCovariance
		   */
		void getMean(CPose3D &mean_pose) const;

		/** Returns an estimate of the pose covariance matrix (6x6 cov matrix) and the mean, both at once.
		  * \sa getMean
		  */
		void getCovarianceAndMean(CMatrixDouble66 &cov,CPose3D &mean_point) const;

		/** Copy operator, translating if necesary (for example, between particles and gaussian representations)
		  */
		void  copyFrom(const CPose3DPDF &o);

		/** Copy operator, translating if necesary (for example, between particles and gaussian representations)
		  */
		void  copyFrom(const CPosePDF &o);

		/** Copy from a 6D pose PDF described as a Quaternion
		  */
		void copyFrom( const CPose3DQuatPDFGaussian &o);


		/** Save the PDF to a text file, containing the 3D pose in the first line, then the covariance matrix in next 3 lines.
		 */
		void  saveToTextFile(const std::string &file) const;

		/** This can be used to convert a PDF from local coordinates to global, providing the point (newReferenceBase) from which
		  *   "to project" the current pdf. Result PDF substituted the currently stored one in the object.
		  */
		void  changeCoordinatesReference(  const CPose3D &newReferenceBase );

		/** Draws a single sample from the distribution
		  */
		void  drawSingleSample( CPose3D &outPart ) const;

		/** Draws a number of samples from the distribution, and saves as a list of 1x6 vectors, where each row contains a (x,y,phi) datum.
		  */
		void  drawManySamples( size_t N, std::vector<vector_double> & outSamples ) const;

		/** Bayesian fusion of two points gauss. distributions, then save the result in this object.
		  *  The process is as follows:<br>
		  *		- (x1,S1): Mean and variance of the p1 distribution.
		  *		- (x2,S2): Mean and variance of the p2 distribution.
		  *		- (x,S): Mean and variance of the resulting distribution.
		  *
		  *    S = (S1<sup>-1</sup> + S2<sup>-1</sup>)<sup>-1</sup>;
		  *    x = S * ( S1<sup>-1</sup>*x1 + S2<sup>-1</sup>*x2 );
		  */
		void  bayesianFusion( const CPose3DPDF &p1, const CPose3DPDF &p2 );

		/** Returns a new PDF such as: NEW_PDF = (0,0,0) - THIS_PDF
		  */
		void	 inverse(CPose3DPDF &o) const;

		/** Unary - operator, returns the PDF of the inverse pose.  */
		inline CPose3DPDFGaussian operator -() const
		{
			CPose3DPDFGaussian p(UNINITIALIZED_POSE);
			this->inverse(p);
			return p;
		}


		/** Makes: thisPDF = thisPDF + Ap, where "+" is pose composition (both the mean, and the covariance matrix are updated).
		  */
		void  operator += ( const CPose3D &Ap);

		/** Makes: thisPDF = thisPDF + Ap, where "+" is pose composition (both the mean, and the covariance matrix are updated).
		  */
		void  operator += ( const CPose3DPDFGaussian &Ap);

		/** Makes: thisPDF = thisPDF - Ap, where "-" is pose inverse composition (both the mean, and the covariance matrix are updated).
		  */
		void  operator -= ( const CPose3DPDFGaussian &Ap);

		/** Evaluates the PDF at a given point.
		  */
		double  evaluatePDF( const CPose3D &x ) const;

		/** Evaluates the ratio PDF(x) / PDF(MEAN), that is, the normalized PDF in the range [0,1].
		  */
		double  evaluateNormalizedPDF( const CPose3D &x ) const;

		/** Computes the Mahalanobis distance between the centers of two Gaussians.
		  *  The variables with a variance exactly equal to 0 are not taken into account in the process, but
		  *   "infinity" is returned if the corresponding elements are not exactly equal.
		  */
		double  mahalanobisDistanceTo( const CPose3DPDFGaussian& theOther);

		/** This static method computes the pose composition Jacobians, with these formulas:
			\code
				df_dx =
				[ 1, 0, 0, -sin(yaw)*cos(p)*xu+(-sin(yaw)*sin(p)*sin(r)-cos(yaw)*cos(r))*yu+(-sin(yaw)*sin(p)*cos(r)+cos(yaw)*sin(r))*zu, -cos(yaw)*sin(p)*xu+cos(yaw)*cos(p)*sin(r)*yu+cos(yaw)*cos(p)*cos(r)*zu, (cos(yaw)*sin(p)*cos(r)+sin(yaw)*sin(r))*yu+(-cos(yaw)*sin(p)*sin(r)+sin(yaw)*cos(r))*zu]
				[ 0, 1, 0,    cos(yaw)*cos(p)*xu+(cos(yaw)*sin(p)*sin(r)-sin(yaw)*cos(r))*yu+(cos(yaw)*sin(p)*cos(r)+sin(yaw)*sin(r))*zu, -sin(yaw)*sin(p)*xu+sin(yaw)*cos(p)*sin(r)*yu+sin(yaw)*cos(p)*cos(r)*zu, (sin(yaw)*sin(p)*cos(r)-cos(yaw)*sin(r))*yu+(-sin(yaw)*sin(p)*sin(r)-cos(yaw)*cos(r))*zu]
				[ 0, 0, 1,                                                                                                             0, -cos(p)*xu-sin(p)*sin(r)*yu-sin(p)*cos(r)*zu,                            cos(p)*cos(r)*yu-cos(p)*sin(r)*zu]
				[ 0, 0, 0, 1, 0, 0]
				[ 0, 0, 0, 0, 1, 0]
				[ 0, 0, 0, 0, 0, 1]

				df_du =
				[ cos(yaw)*cos(p), cos(yaw)*sin(p)*sin(r)-sin(yaw)*cos(r), cos(yaw)*sin(p)*cos(r)+sin(yaw)*sin(r), 0, 0, 0]
				[ sin(yaw)*cos(p), sin(yaw)*sin(p)*sin(r)+cos(yaw)*cos(r), sin(yaw)*sin(p)*cos(r)-cos(yaw)*sin(r), 0, 0, 0]
				[ -sin(p),         cos(p)*sin(r),                          cos(p)*cos(r),                          0, 0, 0]
				[ 0, 0, 0, 1, 0, 0]
				[ 0, 0, 0, 0, 1, 0]
				[ 0, 0, 0, 0, 0, 1]
			\endcode
		  */
		static void jacobiansPoseComposition(
			const CPose3D &x,
			const CPose3D &u,
			CMatrixDouble66  &df_dx,
			CMatrixDouble66	 &df_du);


		/** Returns a 3x3 matrix with submatrix of the covariance for the variables (x,y,yaw) only.
		  */
		void getCovSubmatrix2D( CMatrixDouble &out_cov ) const;


	}; // End of class def.


	/** Pose composition for two 3D pose Gaussians  \sa CPose3DPDFGaussian::operator +=  */
	inline CPose3DPDFGaussian operator +( const CPose3DPDFGaussian &x, const CPose3DPDFGaussian &u )
	{
		CPose3DPDFGaussian 	res(x);
		res+=u;
		return res;
	}

	/** Pose composition for two 3D pose Gaussians  \sa CPose3DPDFGaussian::operator -=  */
	inline CPose3DPDFGaussian operator -( const CPose3DPDFGaussian &x, const CPose3DPDFGaussian &u )
	{
		CPose3DPDFGaussian 	res(x);
		res-=u;
		return res;
	}

	/** Dumps the mean and covariance matrix to a text stream.
	  */
	std::ostream  BASE_IMPEXP & operator << (std::ostream & out, const CPose3DPDFGaussian& obj);

	bool BASE_IMPEXP operator==(const CPose3DPDFGaussian &p1,const CPose3DPDFGaussian &p2);

	} // End of namespace
	
	
	/** Global variables to change the run-time behaviour of some MRPT classes within mrpt-core.
	  *  See each variable for the description of what classes it affects.
	  */
	namespace global_settings
	{
		/** If set to true (false), a Scaled Unscented Transform is used instead of a linear approximation with Jacobians.
		  * Affects to:
		  *		- CPose3DPDFGaussian::CPose3DPDFGaussian( const CPose3DQuatPDFGaussian &o)
		  */
		extern BASE_IMPEXP bool USE_SUT_QUAT2EULER_CONVERSION;
	}
	
} // End of namespace

#endif