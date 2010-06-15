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

#include <mrpt/base.h>  // Precompiled headers

#include <mrpt/random.h>
#include <mrpt/math/utils.h>
#include <mrpt/math/CMatrixViews.h>
#include <mrpt/math/transform_gaussian.h>

#include <mrpt/poses/CPose3DPDFGaussian.h>
#include <mrpt/poses/CPose3DQuatPDFGaussian.h>
#include <mrpt/poses/CPosePDFGaussian.h>

using namespace mrpt;
using namespace mrpt::poses;
using namespace mrpt::math;
using namespace mrpt::random;
using namespace mrpt::utils;
using namespace std;

bool mrpt::global_settings::USE_SUT_QUAT2EULER_CONVERSION = false;


IMPLEMENTS_SERIALIZABLE( CPose3DPDFGaussian, CPose3DPDF, mrpt::poses )


/*---------------------------------------------------------------
	Constructor
  ---------------------------------------------------------------*/
CPose3DPDFGaussian::CPose3DPDFGaussian() : mean(0,0,0), cov()
{
}

/*---------------------------------------------------------------
	Constructor
  ---------------------------------------------------------------*/
CPose3DPDFGaussian::CPose3DPDFGaussian(bool,bool) : mean(UNINITIALIZED_POSE), cov(UNINITIALIZED_MATRIX)
{
}

/*---------------------------------------------------------------
	Constructor
  ---------------------------------------------------------------*/
CPose3DPDFGaussian::CPose3DPDFGaussian(
	const CPose3D	&init_Mean,
	const CMatrixD	&init_Cov ) : mean(init_Mean), cov(init_Cov)
{
	ASSERT_( init_Cov.getColCount()==6 && init_Cov.getRowCount()==6 );
}

/*---------------------------------------------------------------
	Constructor
  ---------------------------------------------------------------*/
CPose3DPDFGaussian::CPose3DPDFGaussian( const CPose3D &init_Mean, const CMatrixDouble66 &init_Cov ) :
	mean(init_Mean), cov(init_Cov)
{
}

/*---------------------------------------------------------------
	Copy Constructor from 2D PDF
  ---------------------------------------------------------------*/
CPose3DPDFGaussian::CPose3DPDFGaussian( const CPosePDFGaussian &o )
	 : mean( o.mean.x(),o.mean.y(),0,o.mean.phi(),0,0 ),
	   cov(o.cov)
{
}


/*---------------------------------------------------------------
	Constructor
  ---------------------------------------------------------------*/
CPose3DPDFGaussian::CPose3DPDFGaussian(
	const CPose3D	&init_Mean ) : mean(init_Mean), cov()
{
}


//#define DO_TEST_JACOB

#ifdef DO_TEST_JACOB
void ffff(const vector_double &x,const CQuaternionDouble &Q, vector_double &OUT)
{
	OUT.resize(3);
	CQuaternionDouble q(x[0],x[1],x[2],x[3]);
	q.normalize();
	q.rpy(OUT[2],OUT[1],OUT[0]);
}
#endif

void aux_posequat2poseypr(const CArrayDouble<7> &x,const double&dummy, CArrayDouble<6> &y)
{
	y[0]=x[0]; y[1]=x[1]; y[2]=x[2];
	CQuaternionDouble q(x[3],x[4],x[5],x[6]);
	q.normalize();
	q.rpy(y[5],y[4],y[3]);
}

/*---------------------------------------------------------------
					CPose3DPDFGaussian
 ---------------------------------------------------------------*/
CPose3DPDFGaussian::CPose3DPDFGaussian( const CPose3DQuatPDFGaussian &o)  :
	mean(UNINITIALIZED_POSE), cov(UNINITIALIZED_MATRIX)
{
	this->copyFrom(o);
}

/*---------------------------------------------------------------
						copyFrom
 ---------------------------------------------------------------*/
void CPose3DPDFGaussian::copyFrom( const CPose3DQuatPDFGaussian &o)
{
	MRPT_START
	if (!mrpt::global_settings::USE_SUT_QUAT2EULER_CONVERSION)
	{
		// Convert using Jacobians and first order approximation:

		//         [  I_3   |    0         ]
		// dr_dq = [ -------+------------- ]
		//         [  0     | dr_dq_angles ]
#ifdef DO_TEST_JACOB
		// Test Jacob:
		{
			vector_double x(4);
			for (int i=0;i<4;i++) x[i] = o.mean.quat()[i];
			vector_double Ax(4,1e-7);
			CMatrixDouble H;
			jacobians::jacob_numeric_estimate(x,ffff,Ax, o.mean.quat(),H);
			cout << "num:" <<endl <<H << endl << endl;
			CMatrixDouble J;
			double a,b,c;
			o.mean.quat().rpy_and_jacobian(a,b,c,&J);
			CMatrixDouble NJ;
			o.mean.quat().normalizationJacobian(NJ);
			cout << "lin:" <<endl<< J*NJ << endl << endl;
		}
#endif

		double yaw,pitch,roll;
		CMatrixFixedNumeric<double,3,4> dr_dq_sub_aux(UNINITIALIZED_MATRIX);

		o.mean.quat().rpy_and_jacobian(roll,pitch,yaw,&dr_dq_sub_aux,false);

		CMatrixDouble44 dnorm_dq(UNINITIALIZED_MATRIX);
		o.mean.quat().normalizationJacobian(dnorm_dq);

		CMatrixFixedNumeric<double,3,4> dr_dq_sub(UNINITIALIZED_MATRIX);
		dr_dq_sub.multiply(dr_dq_sub_aux,dnorm_dq);

		// Set the mean:
		this->mean.setFromValues(o.mean.x(),o.mean.y(),o.mean.z(),yaw,pitch,roll);

		// Cov:
		CMatrixDouble44 cov_Q(UNINITIALIZED_MATRIX);
		CMatrixDouble33 cov_T(UNINITIALIZED_MATRIX);
		CMatrixFixedNumeric<double,3,4> cov_TQ(UNINITIALIZED_MATRIX);
		o.cov.extractMatrix(3,3,cov_Q);
		o.cov.extractMatrix(0,0,cov_T);
		o.cov.extractMatrix(0,3,cov_TQ);

		// [        S_T       |   S_TQ * H^t    ]
		// [ -----------------+---------------- ]
		// [  (S_TQ * H^t)^t  |  H * S_Q * H^t  ]

		// top-left:
		this->cov.insertMatrix(0,0,cov_T);

		// diagonals:
		CMatrixFixedNumeric<double,3,3> cov_TR(UNINITIALIZED_MATRIX);
		cov_TR.multiply_ABt(cov_TQ,dr_dq_sub);
		this->cov.insertMatrix         (0,3,cov_TR);
		this->cov.insertMatrixTranspose(3,0,cov_TR);

		// bottom-right:
		CMatrixDouble33 cov_r(UNINITIALIZED_MATRIX);
		dr_dq_sub.multiply_HCHt(cov_Q,cov_r);
		this->cov.insertMatrix(3,3,cov_r);
	}
	else
	{
		// Use UT transformation:
		//   f: R^7 => R^6
		const CArrayDouble<7> x_mean(o.mean);
		CArrayDouble<6> y_mean;
		static const bool elements_do_wrapPI[6] = {false,false,false,true,true,true};  // xyz yaw pitch roll

		static const double dummy=0;
		mrpt::math::transform_gaussian_unscented(
			x_mean, o.cov,
			aux_posequat2poseypr,
			dummy,
			y_mean,
			this->cov,
			elements_do_wrapPI
			);
		this->mean.setFromValues(y_mean[0],y_mean[1],y_mean[2],y_mean[3],y_mean[4],y_mean[5]);
	}
	MRPT_END
}

/*---------------------------------------------------------------
						getMean
  Returns an estimate of the pose, (the mean, or mathematical expectation of the PDF)
 ---------------------------------------------------------------*/
void CPose3DPDFGaussian::getMean(CPose3D &p) const
{
	p=mean;
}

/*---------------------------------------------------------------
						getCovarianceAndMean
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::getCovarianceAndMean(CMatrixDouble66 &C, CPose3D &p) const
{
	C=cov;
	p=mean;
}

/*---------------------------------------------------------------
						writeToStream
  ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::writeToStream(CStream &out,int *version) const
{
	if (version)
		*version = 1;
	else
	{
		out << mean;
		for (size_t r=0;r<size(cov,1);r++)
			out << cov.get_unsafe(r,r);
		for (size_t r=0;r<size(cov,1);r++)
			for (size_t c=r+1;c<size(cov,2);c++)
				out << cov.get_unsafe(r,c);
	}
}

/*---------------------------------------------------------------
						readFromStream
  ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::readFromStream(CStream &in,int version)
{
	switch(version)
	{
	case 0:
		{
			in >> mean;

			for (size_t r=0;r<size(cov,1);r++)
				in >> cov.get_unsafe(r,r);
			for (size_t r=0;r<size(cov,1);r++)
				for (size_t c=r+1;c<size(cov,2);c++)
				{
					float x;
					in >> x;
					cov.get_unsafe(r,c) = cov.get_unsafe(c,r) = x;
				}

		} break;
	case 1:
		{
			in >> mean;

			for (size_t r=0;r<size(cov,1);r++)
				in >> cov.get_unsafe(r,r);
			for (size_t r=0;r<size(cov,1);r++)
				for (size_t c=r+1;c<size(cov,2);c++)
				{
					double x;
					in >> x;
					cov.get_unsafe(r,c) = cov.get_unsafe(c,r) = x;
				}
		} break;
	default:
		MRPT_THROW_UNKNOWN_SERIALIZATION_VERSION(version)

	};
}



/*---------------------------------------------------------------
						operator =
  ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::copyFrom(const CPose3DPDF &o)
{
	if (this == &o) return;		// It may be used sometimes

	// Convert to gaussian pdf:
	o.getCovarianceAndMean(cov,mean);
}

/*---------------------------------------------------------------
						operator =
  ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::copyFrom(const CPosePDF &o)
{
	// Convert to gaussian pdf:
	CMatrixDouble33 C;
	CPose2D p;
	o.getCovarianceAndMean(C,p);
	mean = CPose3D(p);

	cov.zeros();
	cov.get_unsafe(0,0)=   C.get_unsafe(0,0);
	cov.get_unsafe(1,1)=   C.get_unsafe(1,1);
	cov.get_unsafe(3,3)=   C.get_unsafe(2,2);

	cov.get_unsafe(0,1)=
	cov.get_unsafe(1,0)=   C.get_unsafe(0,1);

	cov.get_unsafe(0,3)=
	cov.get_unsafe(3,0)=   C.get_unsafe(0,2);

	cov.get_unsafe(1,3)=
	cov.get_unsafe(3,1)=   C.get_unsafe(1,2);
}


/*---------------------------------------------------------------

  ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::saveToTextFile(const string &file) const
{
	FILE	*f=os::fopen(file.c_str(),"wt");
	if (!f) return;

	os::fprintf(f,"%e %e %e %e %e %e\n", mean.x(), mean.y(), mean.z(), mean.yaw(), mean.pitch(), mean.roll() );

	for (unsigned int i=0;i<6;i++)
		os::fprintf(f,"%e %e %e %e %e %e\n", cov(i,0),cov(i,1),cov(i,2),cov(i,3),cov(i,4),cov(i,5));

	os::fclose(f);
}

/*---------------------------------------------------------------
						changeCoordinatesReference
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::changeCoordinatesReference( const CPose3D &newReferenceBase )
{
	MRPT_START

	CMatrixDouble44	HM;
	newReferenceBase.getHomogeneousMatrix(HM);

	CMatrixDouble66   M = CMatrixDouble66(HM);	// Clip the 4x4 matrix

	// The variance in yaw,pitch & roll is unmodified:
	M.get_unsafe(3,3) = M.get_unsafe(4,4) = M.get_unsafe(5,5) = 1;
	M.get_unsafe(0,3) = M.get_unsafe(1,3) = M.get_unsafe(2,3) = 0;

	// The mean:
	mean = newReferenceBase + mean;

	// The covariance:
	//cov = M * cov * (~M);
	M.multiply_HCHt( CMatrixDouble66(cov), cov );  // CMatrixDouble66() makes a temporary copy of the input so it can be used as output.

	MRPT_END
}

/*---------------------------------------------------------------
					drawSingleSample
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::drawSingleSample( CPose3D &outPart ) const
{
	MRPT_UNUSED_PARAM(outPart);
	MRPT_START;

	vector_double	v;
	randomGenerator.drawGaussianMultivariate(v,cov);

	outPart.setFromValues(
		mean.x() + v[0],
		mean.y() + v[1],
		mean.z() + v[2],
		mean.yaw() + v[3],
		mean.pitch() + v[4],
		mean.roll() + v[5] );

	MRPT_END_WITH_CLEAN_UP( \
        cov.saveToTextFile("__DEBUG_EXC_DUMP_drawSingleSample_COV.txt"); \
		);
}

/*---------------------------------------------------------------
					drawManySamples
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::drawManySamples(
	size_t						N,
	vector<vector_double>	&outSamples ) const
{
	MRPT_START;

	randomGenerator.drawGaussianMultivariateMany(outSamples,N,cov);

	for (vector<vector_double>::iterator it=outSamples.begin();it!=outSamples.end();++it)
	{
		it->at(0) += mean.x();
		it->at(1) += mean.y();
		it->at(2) += mean.z();
		it->at(3) = math::wrapToPi( it->at(3) + mean.yaw() );
		it->at(4) = math::wrapToPi( it->at(4) + mean.pitch() );
		it->at(5) = math::wrapToPi( it->at(5) + mean.roll() );
	}

	MRPT_END;
}


/*---------------------------------------------------------------
					bayesianFusion
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::bayesianFusion( const CPose3DPDF &p1_, const CPose3DPDF &p2_ )
{
	MRPT_UNUSED_PARAM(p1_); MRPT_UNUSED_PARAM(p2_);
	MRPT_START;

	THROW_EXCEPTION("TO DO!!!");

/*	ASSERT_( p1_.GetRuntimeClass() == CLASS_ID( CPose3DPDFGaussian )  );
	ASSERT_( p2_.GetRuntimeClass() == CLASS_ID( CPose3DPDFGaussian )  );

	CPose3DPDFGaussian	*p1 = (CPose3DPDFGaussian*) &p1_;
	CPose3DPDFGaussian	*p2 = (CPose3DPDFGaussian*) &p2_;


	CMatrixD	x1(3,1),x2(3,1),x(3,1);
	CMatrixD	C1( p1->cov );
	CMatrixD	C2( p2->cov );
	CMatrixD	C1_inv = C1.inv();
	CMatrixD	C2_inv = C2.inv();
	CMatrixD	C;

	x1(0,0) = p1->mean.x; x1(1,0) = p1->mean.y; x1(2,0) = p1->mean.phi;
	x2(0,0) = p2->mean.x; x2(1,0) = p2->mean.y; x2(2,0) = p2->mean.phi;

	C = !(C1_inv + C2_inv);

	this->cov = C;
	this->assureSymmetry();

	x = C * ( C1_inv*x1 + C2_inv*x2 );

	this->mean.x = x(0,0);
	this->mean.y = x(1,0);
	this->mean.phi = x(2,0);
	this->mean.normalizePhi();
*/
	MRPT_END;

}

/*---------------------------------------------------------------
					inverse
 ---------------------------------------------------------------*/
void	 CPose3DPDFGaussian::inverse(CPose3DPDF &o) const
{
	ASSERT_(o.GetRuntimeClass() == CLASS_ID(CPose3DPDFGaussian));
	CPose3DPDFGaussian	&out = static_cast<CPose3DPDFGaussian&>(o);

	// This is like: b=(0,0,0)
	//  OUT = b - THIS
	CPose3DPDFGaussian b;  // Init: all zeros.
	out = b - *this;
}


/*---------------------------------------------------------------
							+=
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::operator += ( const CPose3D &Ap)
{
	// COV:
	const CMatrixDouble66  OLD_COV = this->cov;
	CMatrixDouble66  df_dx(UNINITIALIZED_MATRIX), df_du(UNINITIALIZED_MATRIX);

	CPose3DPDFGaussian::jacobiansPoseComposition(
		this->mean,  // x
		Ap,     // u
		df_dx,
		df_du );

	// this->cov = H1*this->cov*~H1 + H2*Ap.cov*~H2;
	df_dx.multiply_HCHt( OLD_COV, cov );
	// df_du: Nothing to do, since COV(Ap) = zeros

	// MEAN:
	this->mean = this->mean + Ap;
}

/*---------------------------------------------------------------
					jacobiansPoseComposition
 ---------------------------------------------------------------*/
void CPose3DPDFGaussian::jacobiansPoseComposition(
	const CPose3D &x,
	const CPose3D &u,
	CMatrixDouble66	 &df_dx,
	CMatrixDouble66	 &df_du)
{
	// Direct equations (for the covariances) in yaw-pitch-roll are too complex.
	//  Make a way around them and consider instead this path:
	//
	//      X(6D)       U(6D)
	//        |           |
	//        v           v
	//      X(7D)       U(7D)
	//        |           |
	//        +--- (+) ---+
	//              |
	//              v
	//            RES(7D)
	//              |
	//              v
	//            RES(6D)
	//

	// FUNCTION = f_quat2eul(  f_quat_comp(  f_eul2quat(x) , f_eul2quat(u)  )   )
	//  Jacobians chain rule:
	//
	//  JACOB_dx = J_Q2E (6x7) * quat_df_dx (7x7) * J_E2Q_dx (7x6)
	//  JACOB_du = J_Q2E (6x7) * quat_df_du (7x7) * J_E2Q_du (7x6)

	// J_E2Q_dx:
	CMatrixFixedNumeric<double,7,6>  J_E2Q_dx; // Init to zeros
	{
		CMatrixFixedNumeric<double,4,3>  dq_dr_sub(UNINITIALIZED_MATRIX);
		CQuaternionDouble  q_dumm(UNINITIALIZED_QUATERNION);
		x.getAsQuaternion(q_dumm, &dq_dr_sub);
		J_E2Q_dx.get_unsafe(0,0)=J_E2Q_dx.get_unsafe(1,1)=J_E2Q_dx.get_unsafe(2,2)=1;
		J_E2Q_dx.insertMatrix(3,3,dq_dr_sub);
	}

	// J_E2Q_du:
	CMatrixFixedNumeric<double,7,6>  J_E2Q_du; // Init to zeros
	{
		CMatrixFixedNumeric<double,4,3>  dq_dr_sub(UNINITIALIZED_MATRIX);
		CQuaternionDouble  q_dumm(UNINITIALIZED_QUATERNION);
		u.getAsQuaternion(q_dumm, &dq_dr_sub);
		J_E2Q_du.get_unsafe(0,0)=J_E2Q_du.get_unsafe(1,1)=J_E2Q_du.get_unsafe(2,2)=1;
		J_E2Q_du.insertMatrix(3,3,dq_dr_sub);
	}

	// quat_df_dx & quat_df_du
	CMatrixDouble77  quat_df_dx(UNINITIALIZED_MATRIX), quat_df_du(UNINITIALIZED_MATRIX);
	const CPose3DQuat  quat_x(x);
	const CPose3DQuat  quat_u(u);

	CPose3DQuatPDFGaussian::jacobiansPoseComposition(
		quat_x,  // x
		quat_u,     // u
		quat_df_dx,
		quat_df_du );

	// And now J_Q2E:
	//         [  I_3   |    0         ]
	// J_Q2E = [ -------+------------- ]
	//         [  0     | dr_dq_angles ]
	//
	CMatrixFixedNumeric<double,6,7> J_Q2E; // Init to zeros
	J_Q2E.get_unsafe(0,0) = J_Q2E.get_unsafe(1,1) = J_Q2E.get_unsafe(2,2) = 1;
	{
		// The end result of the pose composition, as a quaternion:
		CQuaternionDouble q_xu(UNINITIALIZED_QUATERNION);
		q_xu.crossProduct(quat_x.quat(),quat_u.quat());

		// Compute the jacobian:
		CMatrixFixedNumeric<double,3,4> dr_dq_sub_aux(UNINITIALIZED_MATRIX);
		double yaw,pitch,roll;
		q_xu.rpy_and_jacobian(roll,pitch,yaw,&dr_dq_sub_aux,false);

		CMatrixDouble44 dnorm_dq(UNINITIALIZED_MATRIX);
		q_xu.normalizationJacobian(dnorm_dq);

		CMatrixFixedNumeric<double,3,4> dr_dq_sub(UNINITIALIZED_MATRIX);
		dr_dq_sub.multiply(dr_dq_sub_aux,dnorm_dq);

		J_Q2E.insertMatrix(3,3,dr_dq_sub);
	}

	// And finally:
	//  JACOB_dx = J_Q2E (6x7) * quat_df_dx (7x7) * J_E2Q_dx (7x6)
	//  JACOB_du = J_Q2E (6x7) * quat_df_du (7x7) * J_E2Q_du (7x6)
	df_dx.multiply_ABC(J_Q2E, quat_df_dx, J_E2Q_dx);
	df_du.multiply_ABC(J_Q2E, quat_df_du, J_E2Q_du);
}


/*---------------------------------------------------------------
							+=
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::operator += ( const CPose3DPDFGaussian &Ap)
{
	// Direct equations (for the covariances) in yaw-pitch-roll are too complex.
	//  Make a way around them and consider instead this path:
	//
	//      X(6D)       U(6D)
	//        |           |
	//        v           v
	//      X(7D)       U(7D)
	//        |           |
	//        +--- (+) ---+
	//              |
	//              v
	//            RES(7D)
	//              |
	//              v
	//            RES(6D)
	//
	CPose3DQuatPDFGaussian   X7(*this);
	const CPose3DQuatPDFGaussian   U7(Ap);

	X7+=U7;

	this->copyFrom(X7);
}

/*---------------------------------------------------------------
							-=
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::operator -= ( const CPose3DPDFGaussian &Ap)
{
	// Direct equations (for the covariances) in yaw-pitch-roll are too complex.
	//  Make a way around them and consider instead this path:
	//
	//      X(6D)       U(6D)
	//        |           |
	//        v           v
	//      X(7D)       U(7D)
	//        |           |
	//        +--- (-) ---+
	//              |
	//              v
	//            RES(7D)
	//              |
	//              v
	//            RES(6D)
	//
	CPose3DQuatPDFGaussian   X7(*this);
	const CPose3DQuatPDFGaussian   U7(Ap);

	X7-=U7;

	this->copyFrom(X7);
}

/*---------------------------------------------------------------
						evaluatePDF
 ---------------------------------------------------------------*/
double  CPose3DPDFGaussian::evaluatePDF( const CPose3D &x ) const
{
	MRPT_UNUSED_PARAM(x);
	THROW_EXCEPTION("TO DO!!!");

/*	CMatrixD	X(6,1);
	X(0,0) = x.x;
	X(1,0) = x.y;
	X(2,0) = x.z;

	CMatrixD	MU(6,1);
	MU(0,0) = mean.x;
	MU(1,0) = mean.y;
	MU(2,0) = mean.z;

	return math::normalPDF( X, MU, this->cov );
*/
}

/*---------------------------------------------------------------
						evaluateNormalizedPDF
 ---------------------------------------------------------------*/
double  CPose3DPDFGaussian::evaluateNormalizedPDF( const CPose3D &x ) const
{
	MRPT_UNUSED_PARAM(x);
	THROW_EXCEPTION("TO DO!!!");
/*	CMatrixD	X(3,1);
	X(0,0) = x.x;
	X(1,0) = x.y;
	X(2,0) = x.phi;

	CMatrixD	MU(3,1);
	MU(0,0) = mean.x;
	MU(1,0) = mean.y;
	MU(2,0) = mean.phi;

	return math::normalPDF( X, MU, this->cov ) / math::normalPDF( MU, MU, this->cov );
*/
}

/*---------------------------------------------------------------
						assureSymmetry
 ---------------------------------------------------------------*/
void  CPose3DPDFGaussian::assureSymmetry()
{
	// Differences, when they exist, appear in the ~15'th significant
	//  digit, so... just take one of them arbitrarily!
	for (unsigned int i=0;i<size(cov,1)-1;i++)
		for (unsigned int j=i+1;j<size(cov,1);j++)
			cov.get_unsafe(i,j) = cov.get_unsafe(j,i);
}

/*---------------------------------------------------------------
						mahalanobisDistanceTo
 ---------------------------------------------------------------*/
double  CPose3DPDFGaussian::mahalanobisDistanceTo( const CPose3DPDFGaussian& theOther )
{
	MRPT_START

	CMatrixDouble66	COV_ = cov + theOther.cov;
	CMatrixDouble16	MU   = CMatrixDouble16(theOther.mean) - CMatrixDouble16(mean);

	for (int i=0;i<6;i++)
	{
		if (COV_.get_unsafe(i,i)==0)
		{
			if (MU.get_unsafe(0,i)!=0)
					return std::numeric_limits<double>::infinity();
			else COV_.get_unsafe(i,i) = 1;  // Any arbitrary value since MU(i)=0, and this value doesn't affect the result.
		}
	}

	CMatrixDouble66	COV_inv;
	COV_.inv(COV_inv);

	return std::sqrt( MU.multiply_HCHt_scalar(COV_inv) );

	MRPT_END
}

/*---------------------------------------------------------------
						operator <<
 ---------------------------------------------------------------*/
ostream &   mrpt::poses::operator << (
	ostream		&out,
	const CPose3DPDFGaussian	&obj )
{
	out << "Mean: " << obj.mean << "\n";
	out << "Covariance:\n" << obj.cov << "\n";

	return out;
}

/*---------------------------------------------------------------
						getCovSubmatrix2D
 ---------------------------------------------------------------*/
void CPose3DPDFGaussian::getCovSubmatrix2D( CMatrixDouble &out_cov ) const
{
	out_cov.setSize(3,3);

	for (int i=0;i<3;i++)
	{
		int a = i==2 ? 3:i;
		for (int j=i;j<3;j++)
		{
			int b = j==2 ? 3:j;
			double f = cov.get_unsafe(a,b);
			out_cov.set_unsafe(i,j, f);
			out_cov.set_unsafe(j,i, f);
		}
	}
}

bool mrpt::poses::operator==(const CPose3DPDFGaussian &p1,const CPose3DPDFGaussian &p2)
{
	return p1.mean==p1.mean && p1.cov==p2.cov;
}