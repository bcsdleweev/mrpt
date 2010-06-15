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
#ifndef CFeatureExtraction_H
#define CFeatureExtraction_H

#include <mrpt/utils/CImage.h>
#include <mrpt/utils/CImageFloat.h>
#include <mrpt/utils/CTicTac.h>
#include <mrpt/vision/utils.h>
#include <mrpt/vision/CFeature.h>

namespace mrpt
{
	namespace vision
	{
		/** The central class from which images can be analyzed in search of different kinds of interest points and descriptors computed for them.
		  *  To extract features from an image, create an instance of CFeatureExtraction,
		  *   fill out its CFeatureExtraction::options field, including the algorithm to use (see
		  *   CFeatureExtraction::TMethodExtraction), and call CFeatureExtraction::detectFeatures.
		  *  This will return a set of features of the class mrpt::vision::CFeature, which include
		  *   details for each interest point as well as the desired descriptors and/or patches.
		  *
		  *  By default, a 21x21 patch is extracted for each detected feature. If the patch is not needed,
		  *   set patchSize to 0 in CFeatureExtraction::options
		  *
		  *  The implemented <b>detection</b> algorithms are (see CFeatureExtraction::TMethodExtraction):
		  *		- KLT (Kanade-Lucas-Tomasi): A detector (no descriptor vector).
		  *		- Harris: A detector (no descriptor vector).
		  *		- BCD (Binary Corner Detector): A detector (no descriptor vector) (Not implemented yet).
		  *		- SIFT: An implementation of the SIFT detector and descriptor. The implemention may be selected with CFeatureExtraction::TOptions::SIFTOptions::implementation.
		  *		- SURF: OpenCV's implementation of SURF detector and descriptor.
		  *
		  *  Additionally, given a list of interest points onto an image, the following
		  *   <b>descriptors</b> can be computed for each point by calling CFeatureExtraction::computeDescriptors :
		  *		- SIFT descriptor (Lowe's descriptors).
		  *		- SURF descriptor (OpenCV's implementation - Requires OpenCV 1.1.0 from SVN or later).
		  *		- Intensity-domain spin images (SpinImage): Creates a vector descriptor with the 2D histogram as a single row.
		  *		- A circular patch in polar coordinates (Polar images): The matrix descriptor is a 2D polar image centered at the interest point.
		  *		- A log-polar image patch (Log-polar images): The matrix descriptor is the 2D log-polar image centered at the interest point.
		  *
		  * \note The descriptor "Intensity-domain spin images" is described in "A sparse texture representation using affine-invariant regions", S Lazebnik, C Schmid, J Ponce, 2003 IEEE Computer Society Conference on Computer Vision.
		  *
		  * \sa mrpt::vision::CFeature
		  */
		class VISION_IMPEXP CFeatureExtraction
		{
		public:
			enum TSIFTImplementation
			{
				LoweBinary = 0,
				CSBinary,
				VedaldiBinary,
				Hess,
				OpenCV
			};

			/** The set of parameters for all the detectors & descriptor algorithms */
			struct VISION_IMPEXP TOptions : public utils::CLoadableOptions
			{
				/** Initalizer
				*/
				TOptions();

				/** See utils::CLoadableOptions
				  */
				void  loadFromConfigFile(
					const mrpt::utils::CConfigFileBase  &source,
					const std::string &section);

				/** See utils::CLoadableOptions
				  */
				void  dumpToTextStream(CStream		&out) const;

				/** Type of the extracted features
				*/
				TFeatureType featsType;

				/** Size of the patch to extract, or 0 if no patch is desired (default=21).
				  */
				unsigned int patchSize;

				/** Indicates if subpixel accuracy is desired for the extracted points (only applicable to KLT and Harris features)
				  */
				bool FIND_SUBPIXEL;

				struct VISION_IMPEXP TKLTOptions
				{
					/** KLT Options
					  */
					int		radius;			// size of the block of pixels used
					float	threshold;		// for rejecting weak local maxima (with min_eig < threshold*max(eig_image))
					float	min_distance;	// minimum distance between features
					bool	tile_image;		// splits the image into 8 tiles and search for the best points in all of them (distribute the features over all the image)
				} KLTOptions;

				struct VISION_IMPEXP THarrisOptions
				{
					/** Harris Options
					  */
					float	threshold;		// for rejecting weak local maxima (with min_eig < threshold*max(eig_image))
					float	k;				// k factor for the Harris algorithm
					float	sigma;			// standard deviation for the gaussian smoothing function
					int		radius;			// size of the block of pixels used
					float	min_distance;	// minimum distance between features
					bool	tile_image;		// splits the image into 8 tiles and search for the best points in all of them (distribute the features over all the image)
				} harrisOptions;

				struct VISION_IMPEXP TBCDOptions
				{
					/** BCD Options
					  */
				} BCDOptions;

				struct VISION_IMPEXP TFASTOptions
				{
					/** FAST Options
					  */
					int 	threshold;
					bool	nonmax_suppression;		//!< Default = true
				} FASTOptions;

				struct VISION_IMPEXP TSIFTOptions
				{
					/** SIFT Options
					  */
					TSIFTImplementation implementation;
				} SIFTOptions;

				struct VISION_IMPEXP TSURFOptions
				{
					/** SURF Options
					  */
					bool   rotation_invariant; //!< Compute the rotation invariant SURF (dim=128) if set to true (default), or the smaller uSURF otherwise (dim=64)
				} SURFOptions;

				struct VISION_IMPEXP TSpinImagesOptions
				{
					/** SpinImages Options
					  */
					unsigned int hist_size_intensity; //!< Number of bins in the "intensity" axis of the 2D histogram (default=10).
					unsigned int hist_size_distance;  //!< Number of bins in the "distance" axis of the 2D histogram (default=10).
					float        std_dist;      //!< Standard deviation in "distance", used for the "soft histogram" (default=0.4 pixels)
					float        std_intensity; //!< Standard deviation in "intensity", used for the "soft histogram" (default=20 units [0,255])
					unsigned int radius;		//!< Maximum radius of the area of which the histogram is built, in pixel units (default=20 pixels)
				} SpinImagesOptions;

				/** PolarImagesOptions Options
				  */
				struct VISION_IMPEXP TPolarImagesOptions
				{
					unsigned int bins_angle;     //!< Number of bins in the "angular" axis of the polar image (default=8).
					unsigned int bins_distance;  //!< Number of bins in the "distance" axis of the polar image (default=6).
					unsigned int radius;         //!< Maximum radius of the area of which the polar image is built, in pixel units (default=20 pixels)
				} PolarImagesOptions;

				/** LogPolarImagesOptions Options
				  */
				struct VISION_IMPEXP TLogPolarImagesOptions
				{
					unsigned int radius;		//!< Maximum radius of the area of which the log polar image is built, in pixel units (default=30 pixels)
					unsigned int num_angles;	//!< (default=16) Log-Polar image patch will have dimensions WxH, with:  W=num_angles,  H= rho_scale * log(radius)
					double rho_scale;			//!< (default=5) Log-Polar image patch will have dimensions WxH, with:  W=num_angles,  H= rho_scale * log(radius)
				} LogPolarImagesOptions;

			};

			TOptions options;  //!< Set all the parameters of the desired method here before calling "detectFeatures"

			/** Constructor
			*/
			CFeatureExtraction();

			/** Virtual destructor.
			*/
			virtual ~CFeatureExtraction();

			/** Extract features from the image based on the method defined in TOptions.
			* \param img (input) The image from where to extract the images.
			* \param feats (output) A complete list of features (containing a patch for each one of them if options.patchsize > 0).
			* \param nDesiredFeatures (op. input) Number of features to be extracted. Default: all possible.
			* \param ROI (op. input) Region of Interest. Default: The whole image.
			*
			* \sa computeDescriptors
			*/
			void  detectFeatures(	const CImage		&img,
									CFeatureList			&feats,
									unsigned int			init_ID = 0,
									unsigned int			nDesiredFeatures = 0,
									const TImageROI			&ROI = TImageROI()) const;

			//void  detectFeatures2(
			//		const CImage			&img,
			//		std::vector<CFeature>	&feats,
			//		unsigned int			init_ID = 0,
			//		unsigned int			nDesiredFeatures = 0,
			//		const TImageROI			&ROI = TImageROI()) const;


			/** Compute one (or more) descriptors for the given set of interest points onto the image, which may have been filled out manually or from \a detectFeatures
			* \param in_img (input) The image from where to compute the descriptors.
			* \param inout_features (input/output) The list of features whose descriptors are going to be computed.
			* \param in_descriptor_list (input) The bitwise OR of one or several descriptors defined in TDescriptorType.
			*
			*  Each value in "in_descriptor_list" represents one descriptor to be computed, for example:
			*  \code
			*    // This call will compute both, SIFT and Spin-Image descriptors for a list of feature points lstFeats.
			*    fext.computeDescriptors(img, lstFeats, descSIFT | descSpinImages );
			*  \endcode
			*
			* \note The SIFT descriptors for already located features can only be computed through the Hess and
			*        CSBinary implementations which may be specified in CFeatureExtraction::TOptions::SIFTOptions.
			*
			* \note This call will also use additional parameters from \a options
			*/
			void  computeDescriptors(
				const CImage	&in_img,
				CFeatureList		&inout_features,
				TDescriptorType		in_descriptor_list) const;

			/** Extract more features from the image (apart from the provided ones) based on the method defined in TOptions.
			* \param img (input) The image from where to extract the images.
			* \param inList (input) The actual features in the image.
			* \param outList (output) The list of new features (containing a patch for each one of them if options.patchsize > 0).
			* \param nDesiredFeatures (op. input) Number of features to be extracted. Default: all possible.
			*/
			void  findMoreFeatures( const CImage &img,
									const CFeatureList &inList,
									CFeatureList &outList,
									unsigned int nDesiredFeats = 0) const;

		private:
			/** Compute the SIFT descriptor of the provided features into the input image
			* \param in_img (input) The image from where to compute the descriptors.
			* \param in_features (input/output) The list of features whose descriptors are going to be computed.
			*
			* \note The SIFT descriptors for already located features can only be computed through the Hess and
			        CSBinary implementations which may be specified in CFeatureExtraction::TOptions::SIFTOptions.
			*/
			void  internal_computeSiftDescriptors( const CImage	&in_img,
										  CFeatureList		&in_features) const;


			/** Compute the SURF descriptor of the provided features into the input image
			* \param in_img (input) The image from where to compute the descriptors.
			* \param in_features (input/output) The list of features whose descriptors are going to be computed.
			*/
			void  internal_computeSurfDescriptors( const CImage	&in_img,
										  CFeatureList		&in_features) const;

			/** Compute the intensity-domain spin images descriptor of the provided features into the input image
			* \param in_img (input) The image from where to compute the descriptors.
			* \param in_features (input/output) The list of features whose descriptors are going to be computed.
			*
			* \note Additional parameters from CFeatureExtraction::TOptions::SpinImagesOptions are used in this method.
			*/
			void  internal_computeSpinImageDescriptors( const CImage	&in_img,
										  CFeatureList		&in_features) const;

			/** Compute a polar-image descriptor of the provided features into the input image
			* \param in_img (input) The image from where to compute the descriptors.
			* \param in_features (input/output) The list of features whose descriptors are going to be computed.
			*
			* \note Additional parameters from CFeatureExtraction::TOptions::PolarImagesOptions are used in this method.
			*/
			void  internal_computePolarImageDescriptors( const CImage	&in_img,
										  CFeatureList		&in_features) const;

			/** Compute a log-polar image descriptor of the provided features into the input image
			* \param in_img (input) The image from where to compute the descriptors.
			* \param in_features (input/output) The list of features whose descriptors are going to be computed.
			*
			* \note Additional parameters from CFeatureExtraction::TOptions::LogPolarImagesOptions are used in this method.
			*/
			void  internal_computeLogPolarImageDescriptors( const CImage	&in_img,
										  CFeatureList		&in_features) const;

			/** Select good features using the openCV implementation of the KLT method.
			* \param img (input) The image from where to select extract the images.
			* \param feats (output) A complete list of features (containing a patch for each one of them if options.patchsize > 0).
			* \param nDesiredFeatures (op. input) Number of features to be extracted. Default: all possible.
			* \param omitPixels (op. input) A mask for determining the ROI. (0: do not omit this pixel, 1: omit this pixel)
			*/
			void  selectGoodFeaturesKLT(
				const CImage	&inImg,
				CFeatureList		&feats,
				unsigned int		init_ID = 0,
				unsigned int		nDesiredFeatures = 0,
				void				*mask_ = NULL) const;

			/** Extract features from the image based on the KLT method.
			* \param img The image from where to extract the images.
			* \param feats The list of extracted features.
			* \param nDesiredFeatures Number of features to be extracted. Default: authomatic.
			* \param ROI (op. input) Region of Interest. Default: All the image.
			*/
			void  extractFeaturesKLT(
				const CImage		&img,
				CFeatureList			&feats,
				unsigned int			init_ID = 0,
				unsigned int			nDesiredFeatures = 0,
				const TImageROI			&ROI = TImageROI()) const;

			// ------------------------------------------------------------------------------------
			//											BCD
			// ------------------------------------------------------------------------------------
			/** Extract features from the image based on the BCD method.
			* \param img The image from where to extract the images.
			* \param feats The list of extracted features.
			* \param nDesiredFeatures Number of features to be extracted. Default: authomatic.
			* \param ROI (op. input) Region of Interest. Default: All the image.
			*/
			void  extractFeaturesBCD(
				const CImage 		&img,
				CFeatureList			&feats,
				unsigned int			init_ID = 0,
				unsigned int			nDesiredFeatures = 0,
				const TImageROI			&ROI = TImageROI()) const;

			// ------------------------------------------------------------------------------------
			//											SIFT
			// ------------------------------------------------------------------------------------
			/** Extract features from the image based on the SIFT method.
			* \param img The image from where to extract the images.
			* \param feats The list of extracted features.
			* \param nDesiredFeatures Number of features to be extracted. Default: authomatic.
			* \param ROI (op. input) Region of Interest. Default: All the image.
			*/
			void  extractFeaturesSIFT(
				const CImage		&img,
				CFeatureList			&feats,
				unsigned int			init_ID = 0,
				unsigned int			nDesiredFeatures = 0,
				const TImageROI			&ROI = TImageROI()) const;

			// ------------------------------------------------------------------------------------
			//											SURF
			// ------------------------------------------------------------------------------------
			/** Extract features from the image based on the SURF method.
			* \param img The image from where to extract the images.
			* \param feats The list of extracted features.
			* \param nDesiredFeatures Number of features to be extracted. Default: authomatic.
			* \param ROI (op. input) Region of Interest. Default: All the image.
			*/
			void  extractFeaturesSURF(
				const CImage		&img,
				CFeatureList			&feats,
				unsigned int			init_ID = 0,
				unsigned int			nDesiredFeatures = 0,
				const TImageROI			&ROI = TImageROI())  const;

			// ------------------------------------------------------------------------------------
			//											FAST
			// ------------------------------------------------------------------------------------
			/** Extract features from the image based on the FAST method.
			* \param img The image from where to extract the images.
			* \param feats The list of extracted features.
			* \param nDesiredFeatures Number of features to be extracted. Default: authomatic.
			* \param ROI (op. input) Region of Interest. Default: All the image.
			*/
			void  extractFeaturesFAST(
				const CImage			&img,
				CFeatureList			&feats,
				unsigned int			init_ID = 0,
				unsigned int			nDesiredFeatures = 0,
				const TImageROI			&ROI = TImageROI())  const;

			//void  extractFeaturesFAST2(
			//	const CImage			&img,
			//	std::vector<CFeature>	&feats,
			//	unsigned int			init_ID = 0,
			//	unsigned int			nDesiredFeatures = 0,
			//	const TImageROI			&ROI = TImageROI())  const;


			// ------------------------------------------------------------------------------------
			//								my_scale_space_extrema
			// ------------------------------------------------------------------------------------
			/** Computes extrema in the scale space.
			* \param dog_pyr Pyramid of images.
			* \param octvs Number of considered octaves.
			* \param intvls Number of intervales in octaves.
			*/
			void* my_scale_space_extrema(
				CFeatureList &featList, void* dog_pyr,
				int octvs, int intvls, double contr_thr, int curv_thr,
				void* storage ) const;

			/** Adjust scale if the image was initially doubled.
			* \param features The sequence of features.
			*/
			void	my_adjust_for_img_dbl( void* features ) const;

			/** Gets the number of times that a point in the image is higher or lower than the surroundings in the image-scale space
			* \param dog_pyr Pyramid of images.
			* \param octvs Number of considered octaves.
			* \param intvls Number of intervales in octaves.
			* \param row The row of the feature in the original image.
			* \param col The column of the feature in the original image.
			* \param nMin [out]: Times that the feature is lower than the surroundings.
			* \param nMax [out]: Times that the feature is higher than the surroundings.
			*/
			void	getTimesExtrema( void* dog_pyr, int octvs, int intvls, float row, float col, unsigned int &nMin, unsigned int &nMax ) const;

			/** Computes the Laplacian value of the feature in the corresponing image in the pyramid.
			* \param dog_pyr Pyramid of images.
			* \param octvs Number of considered octaves.
			* \param intvls Number of intervales in octaves.
			* \param row The row of the feature in the original image.
			* \param col The column of the feature in the original image.
			*/
			double	getLaplacianValue( void* dog_pyr, int octvs, int intvls, float row, float col ) const;

			/** Append a sequence of openCV features into an MRPT feature list.
			* \param features The sequence of features.
			* \param list [in-out] The list of MRPT features.
			* \param init_ID [in] The initial ID for the new features.
			*/
			void	insertCvSeqInCFeatureList( void* features, CFeatureList &list, unsigned int init_ID = 0 ) const;

			/** Converts a sequence of openCV features into an MRPT feature list.
			* \param features The sequence of features.
			* \param list [in-out] The list of MRPT features.
			* \param init_ID [in][optional] The initial ID for the features (default = 0).
			* \param ROI [in][optional] The initial ID for the features (default = empty ROI -> not used).
			*/
			void	convertCvSeqInCFeatureList( void* features, CFeatureList &list, unsigned int init_ID = 0, const TImageROI &ROI = TImageROI() ) const;

		}; // end of class
	} // end of namespace
} // end of namespace
#endif