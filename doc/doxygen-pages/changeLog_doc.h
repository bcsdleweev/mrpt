/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2004-2008  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Perception and Robotics               |
   |      research group, University of Malaga (Spain).                        |
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

/** \page changelog Change Log
 *

<p> <b>Note:</b> <i>If you are displaying a local version of this page and you have not built the whole HTML documentation, the links above will be broken. Either build the documentation invoking <code>make documentation_html</code> or browse it on-line <a href="http://www.mrpt.org/" target="_blank">here</a>.</i></p>

  <a name="0.9.1">
  <h2>Version 0.9.1: (In developement)</h2></a>
	- <b>Detailed list of changes:</b>
		- Improved classes:
			- mrpt::slam::CObservationGPS: Fixed const'ness of the getAsStruct<>() methods.
			- Absolute pixel coordinates are now permited where previously there were only factors over width/height. See:
				- mrpt::opengl::COpenGLViewport::setViewportPosition and mrpt::opengl::COpenGLViewport::getViewportPosition.
				- mrpt::gui::CDisplayWindow3D::addTextMessage.
			- mrpt::hwdrivers::CGenericSensor::registerClass is now public.
			- All MRPT containers (mrpt::vector_double, all kind of matrices, arrays, etc) now have two new methods for saving to and loading from text files "as vectors", i.e. even for matrices, all the elements are saved/loaded as a simple sequence:
				- loadFromTextFileAsVector
				- saveToTextFileAsVector
		- New functions:
			- mrpt::topography::ENUToGeocentric()
			- mrpt::gui::CMyGLCanvasBase::textBitmapWidth(), a new function to compute the width of a bitmap string.
		- BUGFIXES:
			- Fixed compilation against OpenCV 1.1.0 and 2.0.0.


  <br> <hr> <br>

  <a name="0.9.0">
  <h2>Version 0.9.0: Released 3-JUN-2010 (SVN 1880) </h2></a>
	- <b>Most relevant changes:</b>
		- New minor version number started (0.9.0) due to incompatible changes in the ABI and in some parts of the API (read below for "Important changes in the API").
		- The previous "mrpt-core" library has been split into several smaller libs for better modularity, less dependencies of user programs and faster recompiling.
		- Compile/build: Support for MSVC 2010.
		- Support for the 3D cameras SR3000/SR4000. See mrpt::hwdrivers::CSwissRanger3DCamera and <a href="http://www.mrpt.org/Example:SwissRanger_SR4000_3D_camera" >this example</a>.
		- The library mrpt-htmslam is now part of public MRPT releases (I tried to also release hmt-slam-gui but it would delay for too long... still in SVN only for now).
		- The program 'icp-slam', and mrpt::slam::CMetricMapBuilderICP, are now capable of handling two kinds of rawlogs: based on CSensoryFrame's (as usual), and the newest only containing CObservation's. Note however that ICP-SLAM now requires all odometry actions/observations to have a valid timestamp (very old datasets <=2007 would need to be fixed in RawlogViewer).
		- An important conceptual milestone: New base class mrpt::utils::CObject created to unify the MRPT RTTI system, instead of loading everything on CSerializable (which is now a derived class of CObject). This allows taking advantage of MRPT's smart pointers without forcing to implement serialization. Existing user code should be not affected at all.
		- Fixed critical bugs in rawlog-grabber and in 6D KF-SLAM (read below).
		- MRPT now uses Google's unit test framework (gtest). Dozens of automated tests already exist to provide a robust, continuous checking of the health of MRPT core functionality.

  <a name="0.9.0.API">
	- <b>Important changes in the API:</b> Changes that *may* break existing user code.
		- Calls to buildAuxPointsMap() in some CObservation-derived classes must be replaced by buildAuxPointsMap<CPointsMap>().
		- Localization with particle filters is now implemented in mrpt::slam::CMonteCarloLocalization2D, and mrpt::poses::CPosePDFParticles is now just a data storage class, used as a base class. If you used this class in your code, an option is to employ macros like "#if MRPT_VERSION>=0x090", etc. to handle different versions of MRPT.
		- mrpt::slam::CObservationImage and mrpt::slam::CObservationStereoImages now use a structure mrpt::utils::TCamera instead of separate fields for focal length, intrinsic and distortion parameters.
		- color functions moved from mrpt::vision to mrpt::utils (See mrpt::utils::colormap, ...).
		- mrpt::utils::CImage: The method cross_correlation() is now what was previously called openCV_cross_correlation().
		- mrpt::vision::openCV_cross_correlation() function deleted, since it's already in CImage.
		- mrpt::slam::CObservation::likelihoodWith() has been removed. It's somehow replaced by mrpt:::slam::observationsOverlap().
		- COccupancyGridMap2D::extractFeatures(), COccupancyGridMap2D::extractFeatures(), ... have been moved out of the map itself to a new class: mrpt::slam::COccupancyGridMapFeatureExtractor.
		- CImageGrabber_OpenCV, CImageGrabber_dc1394 and CStereoGrabber_Bumblebee: Have been moved from mrpt::vision to mrpt::hwdrivers, its natural place. Users using these classes thru CCameraSensor will not realize of the difference.
		- mrpt::utils::TMatchingPair and mrpt::utils::TMatchingPairList have been moved from been member classes of mrpt::slam::CMetricMap to be independent structures, now sited in mrpt-base for a more spread usage. Note that for compatibility there are still typedefs within CMetricMap, but it's recommended to use the new names of the structs.
		- The member mrpt::slam::CPointsMap::TInsertionOptions::matchStaticPointsOnly has been removed, since it had little practical utility and broke independence between sub-libraries.
		- The header <mrpt/slam/pose_graph_optimizers.h> has been renamed <mrpt/slam/graph_slam.h>.
		- The class CRobotSimulator has been moved from mrpt::slam to mrpt::utils, since it had nothing to do with SLAM.
		- The function mrpt::hwdrivers::registerAllSensors() has been removed, since it's now not needed (registration is now done reliably at startup).


	- <b>Detailed list of changes:</b>
		- Applications:
			- simul-landmarks:
				- New parameter "show_in_3d" to display a 3D view with the complete simulated robot path and landmarks.
				- It can now simulate random walks of a robot in 6D, not only over a 2D plane as in previous versions.
				- Generated rawlogs are gz-compressed.
			- RawlogViewer:
				- Much more efficient operations of cutting a rawlog (removing a part, keeping a part only), via a new method mrpt::CRawlog::remove() for ranges of indices.
				- The program can now export all the images from a given camera as an AVI file (or two, for stereo cameras).
				- New menu option to regenerate the odometry timestamps of very old datasets (<=2007).
			- icp-slam:
				- The program 'icp-slam', and mrpt::slam::CMetricMapBuilderICP, are now capable of handling two kinds of rawlogs: based on CSensoryFrame's (as usual), and the newest only containing CObservation's.
				- New parameters to set a minimum distance between pose correction with ICP, for faster execution.
			- kf-slam:
				- Now has a much more rich live visualization as a 3D view. See http://www.mrpt.org/Application:kf-slam
			- ICP and RBPF SLAM now have a new option to always insert any kind of CObservation-derived class. See mrpt::slam::CMetricMapBuilder::TOptions::alwaysInsertByClass
			- rawlog-grabber: Now supports extra sensors defined by the user by means of compile-time "plug ins". See http://www.mrpt.org/Application:rawlog-grabber
		- New namespaces:
			- mrpt::global_settings for global switches to tune MRPT behavior at runtime.
			- mrpt::math::jacobians that unifies Jacobian-related functions.
		- New classes:
			- Support for the SwissRanger 3D cameras SR3000/SR4000. See the C++ class mrpt::hwdrivers::CSwissRanger3DCamera and <a href="http://www.mrpt.org/Example:SwissRanger_SR4000_3D_camera" >this example</a>.
			- Implementation of the observer/observable design pattern in the classes: mrpt::utils::CObservable & mrpt::utils::CObserver . See also the example "observer_pattern".
			- Usage of the observer/observable design to dispatch events in GUI windows. See mrpt::gui::CBaseGUIWindow.
			- Usage of the observer/observable design to enable anyone to subscribe to changes in metric maps. See mrpt::slam::mrptEventMetricMapInsert, 			- New set of classes to represent 6D poses using 3D translation + a quaternion for rotation:
				- mrpt::poses::CPose3DQuat  - which can be transparently converted to/from mrpt::poses::CPose3D
				- mrpt::poses::CPose3DQuatPDF
				- mrpt::poses::CPose3DQuatPDFGaussian
			- New class mrpt::utils::CStartUpClassesRegister useful to automatically register CObject-derived classes from user code (and all the MRPT libs).
			- New OpenGL class: mrpt::opengl::CBox, for representing rectangular solid or wireframed boxes.
			- New class: mrpt::utils::CListOfClasses
			- New class mrpt::poses::CRobot2DPoseEstimator: A simple filter to estimate and extrapolate the robot 2D (x,y,phi) pose from asynchronous odometry and localization data.
			- New thread-safe template queue: mrpt::utils::CThreadSafeQueue, which is a generalization of the previous mrpt::utils::CMessageQueue.
			- New hardware driver: mrpt::hwdriver::CBoardIR for a MAPIR custom I/O board for infrared sensors.
		- Improved classes:
			- mrpt::bayes::CKalmanFilterCapable now has a new parameter (debug_verify_analytic_jacobians) to automatically verify the user-providen Jacobians vs. numerical ones.
			- mrpt::slam::CMetricMap (all metric maps) now have a hook method to be called after successful observation insertion.
			- mrpt::hwdrivers::CRovio has been cleaned and redesigned for offering an easier interface.
			- mrpt::gui::CDisplayWindow3D now switches full-screen pressing Alt+Enter.
			- mrpt::vision::CFeatureExtraction now supports the FAST feature detector.
			- Many mrpt::poses classes now have a more complete support for composition/inverse composition of poses/poses, poses/points, etc. including their uncertainties and Jacobians for the PDF classes.
			- mrpt::slam::CHeightGridMap2D now has a method to compute the intersection of 3D rays with its cells.
			- New serialization format of CPose3D, which saves about a 5% in (uncompressed) rawlog file sizes.
			- Pose composition operator += added to mrpt::poses::CPosePDFGaussian, since it was missing there, but existing in the 3D version mrpt::poses::CPose3DPDFGaussian.
			- Lightweight geometry structs TPose* & TPoint* now also have method asString() and fromString().
			- mrpt::poses::CPose3D now has a constructor from a quaternion + a (x,y,z) displacement.
			- mrpt::opengl::CSetOfObjects and mrpt::opengl::COpenGLViewport now have begin() end() and iterators.
			- mrpt::slam::COccupancyGridMap2D now accepts inserting IR and sonar observations (by rhn - mrhemali.rhn at porcupinefactory dot org)
			- Added event "file accessed" to mrpt::system::CFileSystemWatcher:
		- Build system:
			- Support for MSVC 2010.
		- New or improved functions:
			- In mrpt::topography:
				- Introduction of the structure mrpt::topography::TGeodeticCoords
				- New functions implementing more conversions.
			- Set of new functions for propagating a Gaussian PDF trough an arbitrary function (see also example 'unscented_transformation_test')
				- mrpt::math::transform_gaussian_montecarlo : Monte Carlo approximation
				- mrpt::math::transform_gaussian_linear     : First order approximation (as in EKF)
				- mrpt::math::transform_gaussian_unscented  : Scaled Unscented Transformation (as in UKF).
			- mrpt::slam::CLandmarksMap::saveToTextFile() now generates a text header in each file explaining the format of the file (compatible with MATLAB comments).
			- mrpt::opengl::COpenGLViewport::get3DRayForPixelCoord now also works with orthogonal projections.
			- 3D windows can display an arbitrary number of 2D text messages. See mrpt::gui::CDisplayWindow3D::add2DTextMessage().
			- mrpt::utils::TTypeName now also supports matrix and CArray-derived classes.
			- Functions that have been "templatized":
				- mrpt::math::normalPDF
				- mrpt::math::covariancesAndMean
			- New methods in all kind of matrices (some were only in CMatrixTemplate).
				- insertMatrix, insertTransposeMatrix
				- extractCol, extractRow
			- New functions:
				- mrpt::system::terminateThread.
				- mrpt::math::unscented_transform_gaussian
				- mrpt::math::montecarlo_transform_gaussian
				- mrpt::math::covariancesAndMeanWeighted
				- mrpt::math::extractColumnFromVectorOfVectors
		- New examples:
			- observer_pattern
			- quaternions
			- unscented_transformation_test
			- grab3Dvideo
	- <b>BUG FIXES:</b>
		- mrpt::utils::CConfigFileBase didn't always remove extra leading and trailing whitespaces in read_string().
		- Initial size of the window was ignored in mrpt::gui::CDisplayWindow3D.
		- Error returning 2x1 size() in dynamic matrices, since there was only one static member shared by all matrices.
		- Swapped red-blue channels in mrpt::gui::wxImage2MRPTImage (Closes <a href="http://www.mrpt.org/node/70" >this issue</a>).
		- mrpt::poses::CPose3D undefined values when initializing from a "-" operator, due to bug in mrpt::math::homogeneousMatrixInverse().
		- mrpt::math::CHistogram::getBinRatio() and hence mrpt::math::histogram when do_normalization=true, always returned zero.
		- Fixed crash in rawlog-grabber when grabbing from a (non-stereo) camera and enabling preview (Closes <a href="https://sourceforge.net/tracker/?func=detail&aid=2988657&group_id=205280&atid=993006" >SF #2988657 </a>).
		- When grabbing images with mrpt::hwdrivers::CCameraSensor and decimation is enabled in mrpt::hwdrivers::CGenericSensor, all the images were saved to disk. It's been fixed so only the decimated ones are saved.
		- Fixed direction errors in the computation of 3D rays of OpenGL viewports for perspective projection. See mrpt::opengl::COpenGLViewport::get3DRayForPixelCoord()
		- RawLogViewer: When loading gz-compressed rawlogs, the "Loading" dialog may close too early.
		- fixed compilation with GCC < 4.4.1 - thanks to zhangbo (张波) for testing.
		- mrpt::opengl::CPointCloud: When color_from_Z is enabled, sometimes the colors switched when the range in Z was very close to 0.
		- rawlog-grabber: Parameter SF_max_time_span is now observed BEFORE inserting observations in sensory frames, thus the time span limit is strictly respected now.
		- More robust handling of strings with \ r \ n chars in matrixes fromMatlabFormat().
		- Fixed wrong results with mrpt::math::mean() and other functions when applied to vectors of integers (By Edu).
		- Fixed mrpt::system::tokenize's wrong behavior with GCC.


  <hr>

  <a name="0.8.1">
  <h2>Version 0.8.1: Released 6-MAR-2010 (svn 1590). </h2></a>
	- A very large reorganization and improvement of "mrpt::math". Thanks to Pablo Moreno for his big work here.
		- Added a forward declarations header <mrpt/math/math_fwrds.h>
		- New header <mrpt/math/ops_containers.h> with general arithmetic operations applicable to vectors, arrays and matrices of any kind.
		- Most matrix template implementations moved to <mrpt/math/ops_matrices.h>. User code that already included <mrpt/slam.h> will need no change to keep compiling OK.
		- More small functions marked as inline.
		- Fixed some specializations that weren't actually called due to issues with forward declarations.
		- Clean up of the mrpt::math namespace by moving auxiliary functions to mrpt::math::detail.
		- All kind of matrices now have dozens of common methods (multiply, leftDivide, rightDivide, inverse, det, eigenValues,...); this includes matrix views.
	- Added md5 cryptographic hash function to utils.
	- MRPT can be now built in Win64.
	- Sonar observations are now processed in point maps. See CSimplePointsMap::insertObservation for arguments of type mrpt::slam::CObservationRange.
	- More robust wxSubsystem in mrpt::gui classes.
	- Occupancy grid maps now have a method to simulate sonar sensors. See COccupancyGridMap2D::sonarSimulator.
	- All 3D objects in mrpt::opengl now have a visible/invisible property. See mrpt::opengl::CRenderizable::setVisibility
	- 3D views (mrpt::gui::CDisplayWindow3D and mrpt::gui::CMyGLCanvasBase) now have:
		- Functions to determine the 3D rays for coordinates picked by the user with the mouse. See the example "display3D".
		- A way to obtain the camera pose as a CPose3D object (see COpenGLViewport::getCurrentCameraPose).
	- New application: navlog-viewer. A GUI tool to reproduce and examine navigation logs. See the applications wiki for more details.
	- mrpt::opengl::CTexturedObject, a new base class to factor code for all mrpt::opengl classes loading/unloading texture images.
	- Particle filters now have new options: verbose, pfAuxFilterOptimal_MLE.
	- read_bool() for INI-like files now accepts "yes/no" and "true/false" apart from "1/0".
	- mrpt::utils::CTimeLogger now can export all the timing data to a CSV file.
	- mrpt::math::CQuaternion has been "modernized": it now inherits from CArray<T,4> and many methods are inline and templates.
	- An important refactoring of code in all Particle Filters, both for localization and RBPF-mapping. As a result, with thousands lines of code less, many more algorithms and applications are now supported:
		- The following classes: mrpt::poses::CPosePDFParticles, mrpt::slam::CMultiMetricMapPDF  now support any combination of these algorithms:
			- pfStandardProposal (SIR), sample size: fixed OR KLD-based dynamic size, different resampling strategies (mrpt::bayes::CParticleFilter::TParticleResamplingAlgorithm).
			- pfAuxiliaryPFStandard (APF), sample size: fixed OR KLD-based dynamic size.
			- pfAuxiliaryPFOptimal (Blanco et al.), sample size: fixed OR KLD-based dynamic size.
	- mrpt::system::aligned_malloc and family now work on all OS's and are used to align the memory of rows in dynamic matrices.
	- Due to many enhancements in the "template-fication" of MRPT, it's no longer equivalent to use std::vector<double> or vector_double, etc... The later form is the only one supported.
	- New set of classes to allow operating on a transformation of a matrix without building the auxiliary matrix, e.g. transpose, submatrices, etc... See mrpt::math::CMatrixView
	- The following classes have fully STL-like iterators, reverse_iterators and container-like methods (begin(),end(),...):
		- mrpt::math::CMatrixFixedNumeric (and derived classes)
		- mrpt::math::CMatrixTemplate (and derived classes)
		- mrpt::math::CArray (and derived classes)
	- Application 2d-slam-demo:
		- It now supports command-line arguments. Try "2d-slam-demo --help"
		- Several improvements in the GUI.
	- Functions modified as templates to allow its usage with a wider variety of parameter types:
		- mrpt::math::estimateJacobian, mrpt::math::homogeneousMatrixInverse
	- New "STL extension" class: mrpt::utils::bimap<KEY,VAL>, a bidirectional equivalent of std::map.
	- Reorganization of header <mrpt/utils/stl_extensions.h>: it now actually includes a few new headers, one for each important class.
	- News in Kalman Filter implementation:
		- New option to compute custom "new landmark"'s covariance, which is needed by, eg. MonoSLAM. The old version of "OnInverseObservationModel" still exists for backward-compatibility. See the new mrpt::bayes::OnInverseObservationModel.
		- Implementing the Jacobians is now optional. If the corresponding virtual methods are not implemented, it will be automatically detected and numeric approximation of the Jacobians will be estimated.
	- All plots based on wxMathPlot now has a new entry in the popup menu for printing the plot to PS/PDF.
	- wxMathPlot updated to version 0.1.2 (Dec-2009).
	- Poses and point classes in mrpt::poses now have the operator[], usable in the new template constructor of mrpt::math::CArrayNumeric
	- mrpt::utils::CLoadableOptions now also allows writting the configuration to files, not only reading.
	- Build system:
		- Fixed the detection of some standard functions (erfc,lrint,...) under GCC for defining the HAVE_XXX's in config.h
	- New small functions:
		- mrpt::math::chi2PDF, mrpt::math::chi2CDF, mrpt::math::noncentralChi2CDF  (adapted from code in the Vigra project).
		- trace() (in all matrices), mrpt::math::KLD_Gaussians, mrpt::math::condidenceIntervals
		- mrpt::random::random_generator_for_STL
	- New examples:
		- math_iterators_test
	- BUGFIXES:
		- mrpt::hwdrivers::CBoardSonars was not automatically registered as CGenericSensor.
		- Fixed compiling with ARIA disabled.
		- mrpt::utils::CSimpleDataBase: fixed crash when loading a record with an empty string in any field.
		- Linux only: Fixed errors compiling with glut.h instead of freeglut.h (Thanks rhn for reporting).
		- Fixed crash if mrpt::gui windows are created, all destroyed, then some created again.
		- Linux only: Fixed ignored argument in CSemaphore::release()
		- Fixed wrong serialization of CSetOfTexturedTriangles.
		- Fixed compile errors for latest OpenCV versions due to need to include <cvaux.h> (By Vicente Arevalo).
		- CStream::printf would crash on very large strings. It now supports arbitrarily long strings, just like mrpt::format.
		- Fixed an error compiling against OpenCV 1.1 (mainly for Fedora repositories).
		- Fixed crash if serializing a CObservation2DRangeScan or a CObservation3DRangeScan with zero scan points (will not happen in real situations, but it's safer now).
		- fixed a conceptual error in mrpt::slam::data_association: The observations do NOT have a covariance matrix! Formulas corrected accordingly.
		- data_association with JCBB only worked with Mahalanobis distance. Fixed for ML.
		- Error loading some parameters from a config file in mrpt::hwdrivers::CSickLaserSerial

  <br>
  <hr>

  <a name="0.8.0">
  <h2>Version 0.8.0: Released 30-Dec-2009 (SVN 1398). </h2></a>
	- <b>Important changes:</b>
		- New minor version number due to a few incompatible changes in the API. The affected classes are:
			- mrpt::bayes::CKalmanFilterCapable
			- mrpt::reactivenav::CReactiveInterfaceImplementation
			- Headers for namespace mrpt::system have been factored in a number of smaller files.
		- rbpf-slam now builds maps of points.
		- Data association implemented as a generic algorithm and demonstrated in the program 2d-slam-demo.
		- The interface of Kalman filters has been greatly redesigned for a better factoring of code and to explote fixed-size matrices.
		- Hardware and sensors:
			- PointGrey Research (PGR) Bumblebee & Bumblebee2 cameras: Better support in Win32 and supported for the first time in Linux as well. See mrpt::vision::CStereoGrabber_Bumblebee and mrpt::hwdrivers::CCameraSensor.
			- New interface to SICK LMS100 via ethernet. Contributed by Adrien Barral (Robopec). See mrpt::hwdrivers::CLMS100Eth and new example "SICK_lms100eth_test".
			- Support for advanced GPS devices: NTRIP client + emitter, RTK GPS, etc.
			- New class for accessing SICK LMS laser scanners with a standard serial port or USB-to-serial RS-232 or RS-424, working for Windows and Linux, up to 75 scans/sec.
		- New algorithms (e.g. A*), many new methods in existing classes and several bug fixes.

	- <b>Detailed list of all changes.</b> New stuff and enhancements:
		- MRPT can be now completely built under MinGW (GCC) if wxWidgets and OpenCV are also built with that compiler first. See the instructions in the Wiki.
		- Fixed size matrices can be declared with one or both of the dimensions equal to zero and that wouldn't lead to an error - until an element is tried to be accessed.
		- The observation mrpt::slam::CRangeBearingKFSLAM now has independent covariance matrices for each sensor point and also the field-of-view have been separated for yaw and pitch.
		- Added a simple profiler class mrpt::utils::CTimeLogger.
		- Added a new option in CMake: MRPT_ENABLE_EMBEDDED_GLOBAL_PROFILER that transform all MRPT_START/MRPT_END macros into calls to the embedded profiler, dumping the results to std::cout (and MSVC output tab) at the end of each execution.
		- mrpt::gui windows now have a method CBaseGUIWindow::getPushedKey, which also returns the key modifiers (shift, alt, etc...).
		- CCameraSensor can now display a preview of the captured images with a "preview_reduction" factor to make them smaller.
		- Great improvements in creating threads by allowing arbitrary function parameters and non-static member methods. See the different signatures of mrpt::system::createThread and mrpt::system::createThreadFromObjectMethod (By Pablo Moreno, small adds by JLBC).
		- mrpt::utils::CImage::saveToFile now has a parameter to select the desired quality of JPEG files.
		- mrpt::hwdrivers::CCameraSensor can now span several worker threads to save the grabbed images to disk, avoiding the CPU bottleneck when capturing high rate video.
		- mrpt::math::CArray: A STL-like container for C fixed-size arrays.
		- New method in CImageGrabber_dc1394 to enumerate all existing Firewire classes. See mrpt::vision::CImageGrabber_dc1394::enumerateCameras.
		- mrpt::hwdrivers::CGPSInterface now can send initialization commands to Javad/TopCon GPS units to setup RTK sources.
		- mrpt::gui window classes now have a static class factory "::Create(...)" similar to their constructor but returning smart pointers.
		- CPointsMap now has a proper observation likelihood method, similar to Sebastian Thrun's "Likelihood Field" for grids, but using points and KD-trees for the look-ups. See CPointsMap::TLikelihoodOptions
		- rbpf-slam:
			- Many improvements to rbpf-slam, especially it now supports map building with maps of points.
			- ICP parameters of rbpf-slam can be now configured via the .ini file.
			- See the example file shared/mrpt/config_files/rbpf-slam/gridmapping_ICPbased.ini
		- The application 2d-slam-demo now shows and computes stats on data association.
		- mrpt::hwdrivers::CHokuyoURG:
			- Now has methods setSerialPort/getSerialPort instead of the public-access variable "m_serial_port".
			- Added  mrpt::hwdrivers::CHokuyoURG::setReducedFOV.
		- ReactiveNavigation: All functors are finally replaced by the new virtual class mrpt::reactivenav::CReactiveInterfaceImplementation.
		- Data association is now correctly implemented and integrated into mrpt::slam::CRangeBearingKFSLAM2D.
		- mrpt::slam::CColouredPointsMap: It can be now selected different schemes for coloring (new: cmFromHeightRelativeToSensorGray).
		- Added missing loadFromTextFile method to fixed-size matrices.
		- Poses and points have new methods: asString, fromString. See mrpt::poses::CPoseOrPoint
		- getByName in mrpt::opengl classes now is recursive when there are CSetOfObjects objects.
		- New class mrpt::utils::CLog, by Vicente Arevalo.
		- mrpt::utils::CClientTCPSocket: Connect and DNS look-up now have a configurable timeout.
		- For all classes that inherit from mrpt::utils::CDebugOutputCapable, calling printf_debug now also sends the strings to the Visual Studio output window.
		- mrpt::hwdrivers::CGenericSensor:
			- The class now implements "loadConfig" and the old virtual "loadConfig" has been renamed to "loadConfig_sensorSpecific". The former calls the latter automatically.
			- New generic parameter "grab_decimation" that enable decimating any source of observations.
		- New class: mrpt::utils::circular_buffer. Used to enhance the reception buffers of these hardware drivers:
			- mrpt::hwdrivers::CHokuyoURG
			- mrpt::hwdrivers::CInterfaceFTDI
		- Utilities for multithread programming (See mrpt::synch)
			- Added a pattern class for creating thread-safe variables protected by a critical section. See mrpt::synch::CThreadSafeVariable.
			- Macro THREADSAFE_OPERATION(critsect, operation).
		- Added serialization of missing std::vector<T> for T=std::string, int8_t, int16_t.
		- New methods to read & write the settings of a generic CCameraSensor object from/to a MRPT-provided wxPanel for GUI applications. See mrpt::hwdrivers::writeConfigFromVideoSourcePanel, mrpt::hwdrivers::readConfigIntoVideoSourcePanel
		- COpenGLViewport now has settable min/max clipping values. See mrpt::opengl::COpenGLViewport::setViewportClipDistances.
		- mrpt::utils::CConfigFile now can be created without specifying the associated file and do the association later with a new method.
		- Apart from 2D exclusion areas in laser scanners, 3D prism zones are now supported as well. C2DRangeFinderAbstract::loadExclusionAreas
		- A* algorithm implemented in mrpt::math::CAStarAlgorithm. (By Pablo Moreno)
		- The central registry of MRPT classes (derived from CSerializable) rewritten as a proper Singleton.
		- New interface to SICK LMS100 via ethernet. Contributed by Adrien Barral (Robopec). See mrpt::hwdrivers::CLMS100Eth and new example "SICK_lms100eth_test".
		- mrpt::utils::CClientTCPSocket now gives a textual description of the error reasons (win32).
		- New namespace with small utilities for networking. See mrpt::utils::net.
		- New class for downloading differential GPS corrections from a NTRIP server. See mrpt::hwdrivers::CNTRIPClient and the example "ntrip-client".
		- Change in mrpt::bayes::CKalmanFilterCapable: The functionality to be implemented in the virtual method OnInverseObservationModel has been splitted in two parts: the same method, and the new OnNewLandmarkAddedToMap. This factorization allows more code reutilization.
		- Data association is now fully implemented in mrpt::slam::data_association_full_covariance and related methods.
		- New class for accessing SICK LMS laser scanners with a standard serial port or USB-to-serial RS-232 or RS-424, working for Windows and Linux, up to 75 scans/sec. See mrpt::hwdrivers::CSickLaserSerial, or the program rawlog-grabber. See also the new example configuration file "SICK_LMS_serial.ini".
		- New generic function for CRC16 computation. See mrpt::utils::compute_CRC16
		- mrpt::math::CQuaternion now has methods for fixed size matrices.
		- Assembler optimized versions of small functions (mrpt::utils::round,...).
	- Changes in the build process:
		- Clean up of options in cmake-gui. Many options are now "advanced" so they are hidden in the "simple view".
		- Better detection of wxWidgets libraries (static/dynamic libs), through a patch submitted to CMake. You'll need to update CMake to enjoy this feature.
		- Fixed building errors with wxWidgets 2.9.0
		- Fixed compilation of MRPT without opencv (in MSVC it reported an error: library 'mrpt-sifthess' missing).
		- Boost is not required anymore. boost-program-options has been replaced by the STL-based TCLAP library.
	- New small functions:
		- mrpt::system::getNumberOfProcessors
		- mrpt::poses::CPose3D::isHorizontal
		- mrpt::math::meanAndCov and mrpt::math::cov now also accept matrices as inputs (just like in MATLAB).
		- mrpt::utils::ObjectToRawString and mrpt::utils::RawStringToObject
		- mrpt::topography::ENU_axes_from_WGS84
		- mrpt::system::os::timegm
		- mrpt::math::CMatrixTemplate::extractSubmatrixSymmetricalBlocks
		- mrpt::utils::keep_max and mrpt::utils::keep_min
		- mrpt::math::make_vector
	- New examples:
		- SICK_laser_serial_test
		- setOfTexturedTrianglesTest
		- ntrip-client
		- http_tests
		- SICK_lms100eth_test
		- a_starAlgorithm
		- cameraCampureAskDialog
	- BUG FIXES:
		- CDisplayWindow3D won't process char key events (Thanks to Miyamoto Musashi for reporting a tested fix).
		- Linking errors due to missing template instantiations of some mrpt::math functions (Thanks to Diego for reporting).
		- Fixed data association routines.
		- Fixed error when deserializing objects of type mrpt::opengl::CPointCloudColoured
		- Fixed potential crash in mrpt::system::dateTimeLocalToString and other time functions when passed an invalid timestamp value.
		- mrpt::system::formatTimeInterval won't show the decimal part of seconds.
		- Win32-MSVC only: Removed the "-D_SECURE_SCL=0" flag in user code that uses MRPT through CMake, since it may lead to increadibly-hard-to-debug crashes.
		- Duplicated landmarkIDs in range-bearing observations are now detected as errors.
		- mrpt::opengl::CTexturedPlane didn't reflect the change of texture after the first time it was rendered.
		- CCameraSensor::setPathForExternalImages didn't work for saving; it was neccesary to also set CImage::IMAGES_PATH_BASE, which is NOT the expected behavior by the user.
		- Fixed linking errors in user programs linked to MRPT thru CMake, when MRPT is compiled as static libs and ffmpeg support is enabled.
		- mrpt::utils::CConfigFileMemory::write may raise an exception even if there was no error.
		- Linux: Fixed mrpt::hwdrivers::CSerialPort won't write the whole data block on Write().
		- Fixed OpenGLCanvas not creating a depth buffer in some platforms/systems.
		- (Win32 only) mrpt::hwdrivers::CInterfaceFTDI won't neither close the USB link or unload the FT2XX.DLL at destruction.
		- C locales changed by default in wxWidgets >=2.9.0 will mess up reading from config files. Fixed by forcing "C" numerics locale in all GUI applications.
		- Fixed potential random crash in mrpt::system::buildTimestampFromParts when used in multi-threaded applications.
		- lib3ds: Won't render scenes with small scales (LIB3DS_EPSILON has been reduced).
		- GUI apps: Many errors in wxWidgets wxFlexSizers fixed, as detected by the recent wx2.9.0 in Debug mode.
		- The following functions (in mrpt::system) did crash when a malformed TTimeStamp was passed as argument: timestampToParts, dateTimeToString, extractDayTimeFromTimestamp, timeToString, dateToString.
		- mrpt::utils::CClientTCPSocket::connect won't resolve server names.
		- Fixed hardcoded type "double" in a method of the generic template mrpt::utils::TParameters.
		- Fixed random crashes when opening several CDisplayWindow*'s at the same time.
		- Fixed infinite recursion (stack overflow) when deleting an arc in a mrpt::hmtslam::CHierarchicalMHMap.
		- Potential crash when freeing OpenGL textures from CTexturedPlane due to incorrect call to "glDeleteTextures".
		- Potential error in mrpt::system::getCurrentThreadTimes with modern MSVC compilers.
		- Fixed Debian bug <a href="http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=543621" >#543621</a>.
		- Fixed a minor error in mrpt::math::computeAverageLogLik, giving a constant offset.


  <a name="0.7.1">
  <h2>Version 0.7.1: Released 17-Aug-2009 (SVN 1144). </h2></a>
	- New methods in mrpt::math to compute the covariance from a list of samples (See mrpt::math::cov,...)
	- Small optimizations to random generation of samples from multivariate Gaussians.
	- RBPF-SLAM: all observations are not processed now, but only those after a given minimum increment in odometry (like in GMapping). See mrpt::slam::CMetricMapBuilderRBPF
	- New efficient operators "+=" to compose CPose2D and CPose3D objects.
	- New methods in matrix classes:
		- add_Ac
		- substract_Ac
	- Slightly more efficient and more correct computation of covariance matrix in mrpt::scan_matching::leastSquareErrorRigidTransformation.
	- Fixed the internal detection of OpenCV version according to the newest convention used in OpenCV SVN. This should fix compiling errors in some Linux distributions.
	- Reduction in the number of MRPT libraries: "mrpt-ann" and "mrpt-3ds" have disappeared (for all platforms), since their code is now integrated in "mrpt-core".
	- <b>The following classes have been renamed</b> (and their corresponding header files as well). Note that typedefs with the old names will be maintained, but they are now deprecated and will disappear in future releases.
		- CMRPTImage --> CImage
		- CMRPTImageFloat --> CImageFloat
		- CMRPTCanvas --> CCanvas
		- CMRPTMessage --> CMessage
	- New wrapper class added for rendering a scene off-screen using framebuffer object extension (see mrpt::opengl::CFBORender class and example fbo_render_test) (By Vicente Arevalo).
	- Embedded lib3ds 1.3 (Library Author: Jan Eric Kyprianidis), for use in all platforms.
	- PTGs can be created through a class factory. See mrpt::reactivenav::CParameterizedTrajectoryGenerator::CreatePTG.
	- PTG classes rewritten and greatly simplified by using standard STL containers.
	- New utility mrpt::utils::TParameters.
	- mrpt::math::CPolygon is now a wrapper around a mrpt::math::TPolygon2D, implementing serialization, backwards compatible with old code.
	- Added new header "mrpt/reactivenav/motion_planning_utils.h" with global utility functions for reactive navigation and motion planning. See mrpt::reactivenav.
	- New methods in mrpt::slam::CPointsMap classes to set all the points at once (setAllPoints).
	- (UNCOMPLETED!!) New robot navigation algorithm in the mrpt-reactivenav library: PTG-based Rapidly-exploring Random Tree (PRRT) method. See mrpt::reactivenav::CPRRTNavigator
	- Smart pointers are now thread safe through atomic increment/decrement operations.
	- New class for atomic operations. See mrpt::synch::CAtomicCounter
	- Support for custom menu entries in the popup menu of mrpt::gui::CDisplayWindowPlots. See example displayPlots and class CDisplayWindowPlots.
	- Compilation under MinGW completely fixed and tested, including OpenCV and wxWidgets.
	- Added == operator to fixed-size matrices and some CPosePDF* classes.
	- Added STL containers utility "erase_return_next" to stl_extensions.h
	- Implemented missing converter from SOG PDFs in CPose3DPDF::createFrom2D.
	- Windows Installer: Added missing links for some GUI applications.
	- Added several insertion-related methods in some mrpt::opengl classes.
	- (UNCOMPLETED!!): Recovered "mono-slam" as a GUI application for both off-line and real-time monocular camera SLAM. Algorithm recovered by Mohammad Ali Amiri Atashgah (University of Sharif Technology, Tehran, Iran), GUI implementation by Jose Luis Blanco.
	- New examples:
		- fbo_render_test
	- New programs:
		- mono-slam: Monocular SLAM (experimental state, UNCOMPLETED!).
		- prrt-navigator-demo: A GUI to experiment with the new PTG-based Rapidly-exploring Random Tree (PRRT) method (UNCOMPLETED!).
	- BUG FIXES:
		- 2D ellipses won't render properly in mrpt::opengl::CEllipsoid.
		- Potential static/dynamic library errors when linking against Boost. (Thanks Philipp Robbel for noticing!)
		- CColouredPointsMap will contain inconsistent data after calling insertPoint() or many other methods.
		- CPose3D might accept angle values out of the range ]-pi,pi]. Thanks to Philipp Robbel for noticing!
		- COccupancyGridMap2D::laserScanSimulator: Simulation of scan rays ending in free space were not marked as invalid.
		- Only the header files for mrpt-core were installed in Linux with "make install" (this also affected each Linux distribution packages!). (Closes <a href="http://sourceforge.net/tracker/?func=detail&atid=993006&aid=2807979&group_id=205280" > SF #2807979 </a>, thanks Joel).
		- ransac_detect_2D_lines (and probably all RANSAC methods) may raise exceptions for some degenerate cases. Thanks to Joel Mckay for reporting and debugging.
		- Compile errors when using automatic serialization of the STL containers "set" and "multiset".
		- NULL pointer access when de-serializing an object of a non-registered class.
		- rbpf-slam: The program did get stuck when used in "fixed number of samples" and one particle has a likelihood >20 orders of magnitud than the others. Thanks to Philipp Robbel for detecting and helping to isolate the issue.
		- rbpf-slam: Images "mapping_%05i.png" files were vertically flipped.
		- Won't fuse two point maps due to an error in CSimplePointsMap::fuseWith. Thanks to Wilian França Costa for the patch.
		- Fixed a little bug in mrpt::opengl::CEllipsoid ray tracing.
		- Wrong computation of ICP covariance. Thanks to Wilian França Costa for the patch.

  <br><hr><br>

  <a name="0.7.0">
  <h2>Version 0.7.0: Released 27-May-2009 (SVN 1042). </h2></a>
	- <b>IMPORTANT CHANGES</b>:
		- Old deprecated methods removed.
		- Changes which may require modifications in user's code:
			- Interface of all probability density functions (PDF) changed for efficiency: getEstimatedMean -> getMean, getEstimatedCov -> getCovarianceAndMean or getCovariance ; and objects returned by referenced instead of returning a copy. Old PDF interface methods marked as deprecated (and will dissapear in future releases).
			- Members x y z (and phi yaw pitch roll where applicable) from mrpt::poses::CPoseOrPoint and derived classes are not public anymore. They must be read out with inline members x(), y() and so on, and written with x(newval), y(newval), etc... This change is needed to assure that the internal state, e.g. in mrpt::poses::CPose3D, is always consistent.
			- A few matrix arithmetic methods renamed for consistency: Now all start with "multiply_XXX", "add_XXX", etc...
			- Some constructors in poses,points & matrices marked as "explicit" to avoid unintended conversions.
			- Random number generators marked as deprecated. New programs should use the new object mrpt::random::randomGenerator or instance objects of the class mrpt::random::CRandomGenerator for thread-safety.
		- All covariance matrices passed to a new matrix class with compile time fixed-size, with dramatic speed ups.
		- Data declarations in geometry.h have been moved to lightweight_geom_data, although TPolygons and TObjects are not really lightweight (Pablo).
		- mrpt::vision now integrates SURF features (Requires OpenCV 1.1.0, integrated by Francisco-Angel Moreno), and new interest point descriptors.
		- New hardware supported:
			- IP cameras via rtsp:// protocol (due to support of FFmpeg libraries).
			- xSens IMU, MTi devices (Built-in version of XSens library).
		- Win32 only: .lib and .dll files now have a postfix with the MRPT version, that is, "libmrpt-core070.lib" instead of "libmrpt-core.lib", etc...
		- New algorithms: A generic and applied RANSAC, Dijkstra for graphs, an optimizer for networks of relative poses and data association algorithms. See the list of changes below.
		- Efficient geometry structures and dozens of new geomtry methods (mainly by Pablo Moreno).
		- Geometry methods improved, corrected and expanded in mrpt::math (by Pablo Moreno). See the list of changes below.
		- A more powerful serialization engine capable of handling arbitrarily complex data types built with STL containers and MRPT classes. See the <a href="http://www.mrpt.org/Serialization" > wiki page</a>.
		- Several bug fixes.
		- New programs:
			- camera-calib: A new GUI program (fused of old "camera-calib" & "camera-calib-gui") for camera calibration with live camera capture.
			- features-matching: A new computer vision demo application.
			- 2d-slam-demo: A GUI application which allows the user to run a Extended Kalman Filter (EKF) implementation of range-bearing 2D SLAM in step-by-step or continuous mode.
		- Many new examples:
			- createVideoFile.
			- data-association-demo
			- dijkstra-example
			- gauss_img_filtering
			- grab_camera_ffmpeg
			- optimize_pose_network
			- polyhedronIntersection: Demonstration of the capabilities of polygon intersection in 3D (Pablo).
			- polygonSplit: Polygon split (Pablo).
			- ransac-demo-applications
			- ransac-demo-plane3D
			- smart_pointers_test
			- stl_containers_serialize
			- tuMicos
			- type_name

	- Detailed list of changes:
		- New methods in CPolyhedron to apply common operations to polyhedra. As a corollary, a lot of new polyhedra are now available. polyhedronIntersection demo now includes four times more polyhedra.
		- New powerful method in CPolyhedron to create different types of polyhedra using a regular base. Many existing methods have been adapted to this interface.
		- Added support for base64 encoding & decoding. See mrpt::system::decodeBase64, mrpt::system::encodeBase64
		- Polyhedra intersection demo improved with twelve additional pairs of polyhedra, including a random one (Pablo).
		- Ray tracing now works with CEllipsoid (Pablo).
		- Ray tracing now works with CCylinder (Pablo).
		- mrpt::utils::CSimpleDatabase interface converted to smart pointers.
		- Added "hold_on" and "hold_off" to mrpt::gui::CDisplayWindowPlots.
		- New class for 2D range-bearing SLAM. See mrpt::slam::CRangeBearingKFSLAM2D
		- CGeneralizedCylinder now works internally using CPose3D instead of TPoint3D.
		- Fixed a bug in the calculation of the distance between TPoints3D.
		- XSens Technology has granted us permission to ship the Xsens CMT library along MRPT, thus MRPT now has out-of-the-box support for XSENS MTi devices. See rawlog-grabber and mrpt::hwdrivers::CIMUXSens
		- rawlog-grabber can now save odometry and sonar data from an ARIA-compatible robot (Pioneers 3-DX, AT, etc...). See the new configuration file "share/config_files/rawlog-grabber/activmedia_robot.ini".
		- rawlog-grabber can record video from IP-cameras thru FFMpeg via the common camera class mrpt::hwdrivers::CCameraSensor.
		- (paco) Bumblebee stereo camera interface mrpt::vision::CStereoGrabber_Bumblebee updates to the lattest vendor's API for Win32 (Francisco Angel Moreno).
		- mrpt::hwdrivers::CActivMediaRobotBase extended to read sonar data, bumpers and enable the guidance of the robot through a joystick attached to the computer.
		- Ray tracing has been considerably sped up.
		- New methods to mrpt::math::CHistogram to ease the computation of PDFs.
		- New methods to control section visibility in mrpt::opengl::CGeneralizedCylinder.
		- CDisplayWindowPlots now have templatized methods.
		- Added missing 2D point PDF. See mrpt::poses::CPoint2DPDF.
		- Fixed-size matrices (see mrpt::math::CMatrixFixedNumeric) used in many places now where the size can be known at compile-time, representing a dramatic sped up.
		- CGeneralizedCylinder now supports ray tracing (Pablo).
		- Added a non-optimal method to split polygons in theirs convex components, including a simple demo (Pablo).
		- Matrices now have hard-coded optimized implementations for some 2x2 and 3x3 operations (eg. determinant, inverse)
		- (experimental!!) A new generic method, mrpt::slam::data_association, which implements NN (nearest neightbor) and JCBB (Joint Compatibility Branch & Bound).
		- Additional methods to retrieve data from mrpt::opengl::CGeneralizedCylinder (Pablo).
		- Added some methods to mrpt::math::CMatrixTemplate (Pablo).
		- New methods in the geometric classes and in mrpt::opengl::CPolyhedron (Pablo).
		- New methods in mrpt::utils::CImage: scaleImage, rotateImage, ...
		- Added new malloc/realloc/free equivalents for aligned memory. See mrpt::system::os::aligned_malloc
		- New application: <i>features-matching</i>, for demonstrating and evaluating the performance of several image feature detectors & descriptors. See <a href="http://www.mrpt.org/Application:features-matching" >its wiki page</a>.
		- mrpt::registerAllClasses mustn't be called manually anymore: it's always automatically executed at startup.
		- Added a generic mechanism to obtain the (compiler independant, unlike typeid) name of a type. See mrpt::utils::TTypeName, and the <a href="http://www.mrpt.org/Metaprogramming:Type_name_to_string" >documentation on the wiki</a>.
		- Now all STL containers are serialized automatically via template << and >> operators, thus the old classes vector_serializable, etc.. are not needed anymore and has been removed. See utils/stl_extensions.h
		- Small changes in polyhedron intersection demo (Pablo).
		- Added methods to agglutinate vectors of mrpt::math::TSegment3D into vectors of (probably skew) mrpt::math::TPolygon3D. Due to their similarities, this code may evolve into a template in the future, so that it's suitable for both 2D and 3D objects (Pablo).
		- Added methods to extract vectors of mrpt::math::TObject2D and mrpt::math::TObject3D by type (Pablo).
		- More mrpt::opengl classes adapted to mrpt::math::TPoint3D and other geometric lightweight types (Pablo).
		- Maps of landmarks now reuse the feature descriptors defined in mrpt::vision::CFeature. See mrpt::slam::CLandmarksMap
		- Geometric lightweight types have been serialized (JL/Pablo).
		- CSetOfLines class has been almost completely redone, although mantaining its former interface (Pablo).
		- mrpt::opengl::CPolyhedron::rayTrace now works properly (Pablo).
		- mrpt::vision::CFeatureExtraction now has a more uniform interface which separates interest point detectors and descriptors (Francisco-Angel Moreno & Jose Luis)
		- New feature detector: Intensity-domain spin images (SpinImage). See mrpt::vision::CFeatureExtractor
		- Safer constructors of smart pointers from base to derived classes since classes are now always checked at runtime (unless ASSERT_'s are disabled from CMake).
		- Added a CSparseMatrix template, which relies on std::map (Pablo).
		- Added support for intersections in vectors and generic objects (Pablo).
		- Minor change in mrpt::poses::CPose3D::composePoint for efficiency purposes (Pablo).
		- Added methods to estimate lines and planes using linear regression (Pablo).
		- Added method mrpt::math::covariancesAndMean to get covariances and mean of any set of points in N-dimensions (Pablo).
		- The Levenberg-Marquardt implementation now has a second template parameter for the type of the "user parameter" passed to the evaluation functor. Instead of forcing it being a numeric vector, it can now be any arbitrary object. See mrpt::math::CLevenbergMarquardtTempl
		- New consistent pose-network optimizers (Experimental!):
			- Based on Levenberg-Marquardt. See mrpt::slam::optimizePoseGraph_levmarq
		- A new generic implementation of Dijkstra algorithm for shortest paths on a directed, possibly weighted, graph. See mrpt::math::CDijkstra
		- New centralized class for random number generators: mrpt::random::CRandomGenerator . Old methods marked as deprecated.
		- Random Generators rewritten as a class for thread-safety: Each object has its own internal state for the PRNG.
		- New base class mrpt::utils::CProbabilityDensityFunction for all PDF classes.
		- Added bi-directional conversions between 3D poses and quaternion. See mrpt::math::CQuaternion and  mrpt::poses::CPose3D::getAsQuaternion
		- A new common base class for all GUI windows. See mrpt::gui::CBaseGUIWindow
		- Matrix constructors from poses and points moved from CMatrix & CMatrixD to CMatrixTemplateNumeric for greater utility.
		- Added unary "-" operators to mrpt::poses::CPose3D and mrpt::poses::CPose2D for obtaining the inverse poses quickly.
		- Removed deprecated methods:
			- From mrpt::utils::CImage: loadFromBMP, loadFromJPEG, saveToBMP, saveToJPEG. Replaced by saveToFile and loadFromFile.
			- From mrpt::slam::CRawlog and its iterators: isAction. Replace by getType
		- Integration of ffmpeg libraries for frame-by-frame access to any kind of video or streams, like IP cameras via rtsp:// protocol. See mrpt::hwdrivers::CFFMPEG_InputStream
		- 3D windows has a method to obtain the rendered image for user processing as desired. See mrpt::gui::CDisplayWindow3D::captureImagesStart
		- Added copy constructors for mrpt::math::TObject2D and mrpt::math::TObject3D to allow use in containers (Pablo).
		- mrpt::opengl::CPolyhedron and mrpt::opengl::CGeneralizedCylinder classes accustomed to the new geometric methods (Pablo).
		- Added non-secure methods to access geometric object's coordinates (like in mrpt::math::TPoint2D) as an array (Pablo).
		- Intersections in mrpt::math now work properly in almost every case (Pablo).
		- Added a signWithZero function in utils_defs (Pablo).
		- Added methods in mrpt::opengl::CPolyhedron to work with sets of mrpt::math::TPolygon3D (Pablo).
		- Added a new test for pragma pack (Pablo).
		- Added a video file writer from a sequence of images. See mrpt::vision::CVideoFileWriter
		- Deleted old obsolete classes from mrpt::vision: CGaussianConvolutionKernel, CImageConvolution
		- New utilities added to mrpt::utils::metaprogramming
		- Applied uses of RANSAC added in ransac_applications.h. See mrpt::math::ransac_detect_3D_planes, mrpt::math::ransac_detect_2D_lines and <a href="http://www.mrpt.org/RANSAC_C++_examples" > examples</a>.
		- Fonts used in mrpt::utils::CCanvas are now internally saved as gz-compressed data streams, automatically decompressed on first use. This saves ~1Mb in the .DLL/.so file for mrpt-core when compiled with Asian fonts.
		- Two new gz-compression methods: mrpt::compress::zip::compress_gz_data_block and mrpt::compress::zip::decompress_gz_data_block
		- Load/Save of matrices as text files moved from mrpt::math::CMatrixTemplateNumeric to mrpt::math::CMatrixTemplate.
		- mrpt::scan_matching::robustRigidTransformation (and the program grid-matching) dynamically determine the number of RANSAC iterations by default.
		- Exceptions now show a more detailed stack trace. See mrpt::system::stack_trace (requires wxWidgets and building in "Debug").
		- Added methods to retrieve polygons (as defined in geometry.h) for some graphical classes.
		- Added constructors to transparently swap between heavy and lightweight pose classes (heavy poses are intended to stop being used in some classes in the near future).
		- Geometry methods improved, although still not fully functional.
		- Added generic RANSAC implementation. See mrpt::math::RANSAC_Template, and <a href="http://www.mrpt.org/RANSAC_C++_examples" > examples</a>.
		- Edit CPtuHokuyo rawlog-grabber profile to work with CTuMicos.
		- New classes for Micos Tu DT-80. See class mrpt::hwdrivers::CTuMicos.
		- mrpt::system::deleteFilesInDirectory now has a uniform behavior in all platforms, and do not raise "consoles" in Windows GUI applications.
		- lightweight_geom_data (.h and .cpp) added to store some 3D types which require little storage (Pablo Moreno).
		- mrpt::math::geometry methods heavily improved (not fully implemented) (Pablo Moreno).
		- COpenGLStandardObject now includes some new methods (Pablo Moreno).
		- Minor changes in other OpenGL classes (Pablo Moreno).
		- Fixed a bug which caused CMyOpenGLCanvasBase to crash in some cases.
	- BUG FIXES:
		- RawLogViewer didn't change the label of sensors when multiple observations of the same name were in one Sensory Frame.
		- Visual Leak Detector didn't work for multi-threaded applications.
		- Fixed failing mrpt::system::directoryExists when using a trailing "/"
		- Fixed dealing with image assignments when the source is externally stored in mrpt::utils::CImage.
		- CMetricMapBuilder::loadCurrentMapFromFile won't load a GZ-compressed .simplemap file (Reported by Zoltan Tuza).
		- Bad serialization of grayscale image origin (Francisco Moreno).
		- rbpf-slam won't show the live 3D view (Thanks Yuan Fang for reporting).
		- There was no way to retrieve the main viewport's camera parameters for CDisplayWindow3D (Thanks Vijay Somers).
		- mrpt::utils::CImage::getAsMatrix didn't return intensity values in the normalized range [0,1] as documented.
		- mrpt::utils::CMemoryStream: won't read the last byte of the memory block; won't assign an external memory block with assignMemoryNotOwn
		- Compile error in Linux with libdc1394 >=2.0.0 but <2.2.0 (Thanks Allemant David for reporting).
		- Compile errors when compiling in Linux without OpenGL (Thanks Michael Lisowski for reporting).
		- Linking error with Visual Studio when building as DLLs: missing implementation of CAngularObservationMesh::TFloatRange::XXX. (Closes SF <a href="https://sourceforge.net/tracker/index.php?func=detail&aid=2693938&group_id=205280&atid=993006" >2693938</a>).
		- SceneViewer3D does not save the menu image when saving a snapshot from the menu.
		- mrpt::gui::CDisplayWindow now correctly detects key-strokes.
		- mrpt::gui::CDisplayWindow now does not flick (on Win32) anymore when updating the image.
		- mrpt::system::vectorToBinaryFile crashes for vectors of zero-length.
		- Errors when compiling with ASSERTs off (Thanks Scott! Closes SF <a href="https://sourceforge.net/tracker2/?func=detail&aid=2616902&group_id=205280&atid=993006" >2616902</a>).
		- mrpt::poses::CPose3DInterpolator::getPreviousPoseWithMinDistance won't return the first pose in the path as a valid result.
		- mrpt::system::joinThread could block in Windows if the thread was already closed.
		- Fixed pragmas in mrpt-hwdrivers headers, which may lead to undefined functins while linking against hwdrivers in Visual Studio, non CMake-based projects.
		- Solved wrong name of .lib for mrpt-xsens under Windows, for Debug build (Thanks Vicente Arevalo!).
		- mrpt::utils::CSimpleDatabase::loadFromXML won't load any valid XML file.
		- Fixed two extremely unusual cases where a mrpt::poses::CPose3D could not be properly created from its homogeneous matrix.



  <br><hr><br>
	<a name="0.6.5">
  <h2>Version 0.6.5: Released 8-Feb-2009.</h2></a>
	- New method for 3D-ICP, aligning pairs of 3D point clouds using a KD-tree for looking for correspondences and Horn's equation for each iteration. See the class mrpt::slam::CICP, mrpt::slam::CMetricMapsAlignmentAlgorithm::Align3D, and the <a href="http://www.mrpt.org/3D-ICP_Example" >tutorial page</a>.
	- Added class COpenGLStandardObject to render custom objects using OpenGL primitives like GL_TRIANGLES or GL_QUAD_STRIP (Pablo Moreno).
	- CText now accepts color changes (Pablo Moreno).
	- Added "contains" method for CSetOfObjects (Pablo Moreno).
	- Added methods for loading matrices from MATLAB-like strings. See  mrpt::math::CMatrixTemplate<T>::fromMatlabStringFormat  and mrpt::utils::CConfigFileBase::read_matrix.
	- New method for computing the mahalanobis distance. See mrpt::math::mahalanobisDistance
	- Levenberg-Marquardt and many other functions in mrpt::random converted into templates.
	- Added some minor methods to graphic classes, especially CSetOfLines (Pablo Moreno).
	- Matrix's method to save as plain text has been extended with the possibility of adding user comments and selecting the output format. See  mrpt::math::TMatrixTextFileFormat.
	- Added ray trace simulation methods to CRenderizable and every children class except for C3DSScene, CEllipsoid, CCylinder, CGeneralizedCylinder, CPolyhedron and some classes which don't represent any surfaced object (Pablo Moreno).
	- Added some classes to mrpt::opengl namespace (Pablo Moreno):
		- CAngularObservationMesh to represent a 3D world obtained from a set of traced rays.
		- CCylinder to represent a cylinder, cone or truncated cylinder.
		- CGeneralizedCylinder to represent a generalized cylinder with an axis roughly perpendicular to the Z coordinate.
		- CPolyhedron to represent any object without curves. Includes methods to create common polyhedron, although still lacks some geometric methods.
	- CRenderizable::setColor method now works properly (affecting every child) when used in a CSetOfObjects or CSetOfTriangles (Pablo Moreno).
	- Added method mrpt::CMatrixTemplateNumeric::rank to calculate the rank of a matrix (Pablo Moreno).
	- mrpt::gui windows now correctly detect key-strokes on both the console and the windows themselves. Key codes can be obtained from the method mrpt::gui::CDisplayWindow::waitForKey
	- Header files defining the mrpt fonts (used in mrpt::utils::CCanvas) have been removed from the external headers and are internal now.
	- OpenGL objects now have a scale factor. See mrpt::opengl::CRenderizable::setScale
	- Added type mrpt::math::CMatrixLongDouble for matrices of "long double" (becomes "double" if the compiler does not suppor them).
	- New namespaces:
		- mrpt::vision::pinhole with functions related to camera projection models.
		- mrpt::utils::metaprogramming for grouping the metaprogramming classes.
	- Points and poses constructors from other classes that imply a loss of information are defined as explicit, e.g. CPose3D -> CPose2D, etc...
	- Fixed unnecessary link dependencies on 3ds,zlib & ann libs when linking as DLL in Windows.
	- Quick method mrpt::opengl::COpenGLScene::saveToFile to save 3D scenes.
	- New method for automated camera calibration: mrpt::vision::checkerBoardCameraCalibration
	- Important changes to improve the design of mrpt::opengl classes: "render" method is "const", more set/get methods instead of public members, etc.
	- New exception class mrpt::utils::CExceptionEOF, used to distinguish between normal and erroneous EOFs found while de-serializing objects from a stream.
	- Added method for RTK GPS path reconstruction: mrpt::topography::path_from_rtk_gps
	- New math methods:  mrpt::math::unwrap2PiSequence, mrpt::math::averageWrap2Pi
	- Added template function mrpt::utils::find_in_vector for searching an element in any std::vector container (like "find" in other containers).
	- New interpolation methods for 1D functions: mrpt::math::interpolate2points, mrpt::math::leastSquareLinearFit.
	- CStream::printf now correctly returns an "int" instead of "void".
	- Types vector_XXX moved from global to "mrpt" namespace.
	- New color types mrpt::utils::TColor & mrpt::utils::TColorf.
	- Added support for exclusion areas to mark sensed point as invalid in laser scanners. See C2DRangeFinderAbstract::loadExclusionAreas
	- New method CPose3D::composePoint for more efficient 3D pose+point composition.
	- New examples:
		- icp3D: Demonstration of usage for 3D-ICP.
		- rayTrace: Demo of 3D ray tracing (Pablo Moreno).
		- leastSquares: Demonstration of linear least squares interpolation.
	- SceneViewer3D:
		- New menu action "Take snapshot" (F2).
	- RawLogViewer:
		- New combo-box for selecting among different, automatically detected paths for external images. For example, this allows quickly changing between rectified and original images.
		- All I/O file operations now support the gz-compressed format.
		- Edit dialog: Support for multiple selections instead of the old combos.
		- Menu "regenerate GPS" timestamps now allows operating over several sensor labels at once.
		- "Raw Map" module: Now builds 3D maps from sequences with 3 RTK GPS.
	- Support for configuration blocks in rawlogs. See mrpt::slam::CRawlog::getCommentTextAsConfigFile
	- Many improvements in the CMake build system for Linux systems, as part of the Fedora package review process (Thanks to Mamoru Tasaka).
	- BUG FIXES:
		- Error when compiling without GL/GLUT under Linux (Thanks to Chun-Wei for reporting).
		- Wrong computation of the likelihood in COccupancyGridMap2D::computeObservationLikelihood_CellsDifference (Thanks to Fabian Menges, SF <a href="https://sourceforge.net/tracker2/?func=detail&atid=993006&aid=2570053&group_id=205280" >2570053</a>).
		- mrpt::utils::ObjectToString won't really save the object as a string.
		- mrpt::utils::format now works for strings of arbitrarily large lengths.
		- Visual Studio: Exception when mrpt::utils::format tries to generate a too long string.
		- Wrong name of mrpt-zlib libs when compiling without wxWidgets and as DLL.
		- Compilation of app grid-matching fails (SF <a href="https://sourceforge.net/tracker2/?func=detail&aid=2490011&group_id=205280&atid=993006" >2490011</a>, Fabian Menges).
		- CPose3DInterpolator: Solved spureous transitions when "yaw" crosses the +-PI threshold.
		- RawLogViewer: Comments text block was lost after some edit operations.
		- Fixed all the warnings: "dpkg-shlibdeps: warning: dependency on ... could be avoided if ... were not uselessly linked against it (they use none of its symbols)".
		- Fixed an issue with architectures with big endiannes. Fixed Debian bug <a href="http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=509366" > #509366 </a>.


  <br><hr><br>
  <a name="0.6.4">
  <h2>Version 0.6.4: Released 20-Dec-2008 (SVN: 696).</h2></a>
	- Full support for compiling all the libraries as DLLs under Windows.
	- Topography helper methods are now grouped in a new namespace: mrpt::topography
	- New class mrpt::utils::CConsoleRedirector for redirecting all the output from std::cout / std::cerr to a log file.
	- New methods in mrpt::utils::CDebugOutputCapable for making easy reporting messages to both the console and a log file.
	- New boolean matrix classes: mrpt::math::CMatrixBool and mrpt::math::CMatrixB
	- mrpt::opengl container classes now have a templatized getByClass<>() method.
	- New interface class for generic PTU called CPtuBase. (Jose Raul Ruiz Sarmiento)
	- New classes for Directed Perception PTU-46-17.5 and PTU Hokuyo interfacing. See class mrpt::hwdrivers::CPtu and mrpt::hwdrivers::CPtuHokuyo. (Jose Raul Ruiz Sarmiento)
	- Fixed the destination of MRPTConfig.cmake in "make install" (and debian packages) such CMake can now automatically find MRPT without asking the user.
	- Added the target "make uninstall" for Linux/Unix systems.
	- New predefined 3D objects (By Miguel Angel Espada Bernal). See http://www.mrpt.org/Predefined_3D_Objects
	- Added mime type files for installation in Linux systems (FreeDesktop-compatible, e.g. KDE, Gnome).
	- Created test targets. Use: "make", "make test"
	- Better portability with new HAVE_XXX entries in the "config.h" file.
	- Compilation with MinGW/GCC under Windows now supported. Refer to the <a href="http://www.mrpt.org/Guide:Installing_MinGW_(GCC_for_Windows)" > guide on the wiki </a>.
	- Occupancy grids now have an alternative method to insert laser scans (now set by default), which takes into account the "widening" of the laser beams with distance. See the insertionOptions member of mrpt::slam::COccupancyGridMap, and also <a href="http://www.mrpt.org/Occupancy_Grids" >the gridmaps tutorial</a>.
	- New configuration pragma MRPT_ALWAYS_CHECKS_DEBUG_MATRICES, enabled by default. The old MRPT_ALWAYS_CHECKS_DEBUG is not disabled by default for speed up in a number of classes under Release compilation.
	- New class for pixel color retrieval with sub-pixel accuracy from images with transformed coordinates. See mrpt::utils::CMappedImage
	- New image processing operations: rectify, Median and Gaussian filter, etc... In mrpt::utils::CImage.
	- New method to merge several multivariate Gaussians in a Gaussian mixture (SOG) by minimizing the KL-divergence. See mrpt::poses::CPosePDFSOG::mergeModes.
	- Application SceneViewer renamed to SceneViewer3D to solve the Debian bug <a href="http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=505272" > #505272 </a>.
	- New class mrpt::opengl::CSetOfTexturedTriangles for arbitrary shapes in 3D scenes with textures (by Vicente Arevalo).
	- Preview of image capture is posible in rawlog-grabber, thru new option in mrpt::hwdrivers::CCameraSensor.
	- Added support for grabbing FireWire cameras using the library dc1394-2 (The Windows port of dc1394-2 is not released yet). See the new class mrpt::vision::CImageGrabber_dc1394.
	- New methods in matrix classes (and for vectors in mrpt::math::loadVector) for initializing directly from const C arrays.
	- RawlogViewer:
		- Many small improvements in laser & image animations.
		- New menu commands for transforming rawlogs, manipulating gps data, etc...
	- rawlog-grabber:
		- Camera sensors now save images to a external directory. This format is also supported by RawlogViewer and dramatically reduces the cost of visualizing and loading large rawlogs.
		- Support for high-speed & high-resolution IEEE1394 cameras through the class mrpt::hwdrivers::CCameraSensor -> grabber_type = dc1394.
	- New examples:
		- ptuDPerception: Example of use performs a simple scan.
		- ptuHokuyo: A demostration of how coordinate ptu and hokuyo for obtain a map of points that we show using a opengl window.
		- sog-merge: Algorithm for merging SOG pose pdfs.
		- pioneerRobotDemo: A demonstration of connection to a Pioneer robot (ActiveMedia Robotics) and some data requests.
		- gps-coordinates:  Computation of XYZ local coordinates between two precise GPS measurements.
	- New applications:
		- grid-matching: A demonstration of algorithms for occupancy grid map matching. See the <a href="http://www.mrpt.org/Applications" >application page</a>.
		- stereo-calib-gui: A tool for calibrating stereo cameras. Based on code from the new OpenCV 1.1.0 and the OpenCV book (requires OpenCV 1.1.0).
	- BUG FIXES:
		- removeObject didn't work in mrpt::opengl classes.
		- mrpt::utils::CConfigFile reported keys as non-existing if they existed but the actual value was an empty string.
		- occupancy grid map: Potential seg.fault when inserting a laser scan with the first range being invalid.
		- color gradient in mrpt::opengl::CPointCloud was lost with time when color_from_z=true.
		- mrpt::system::extractFileExtension flag for ignore gz extensions didn't work.
		- Serialization is now compatible between systems with different endianness.
		- Fail to compile with GCC 4.4, Debian bug <a href="http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=505668" > #505668 </a>. Thanks to Martin Michlmayr.
		- Hopefuly fixed all wrong calculations of very precise (x,y,z) coordinates from latitude, longitude and height, in mrpt::slam::CObservationGPS.
		- Wrong data grabbed from xSens IMU: Gyroscope data saved as XYZ velocities. In mrpt::hwdrivers::CIMUXSens
		- Possible lock when reading from a serial port from two different objects mrpt::hwdrivers::CSerialPort
		- mrpt::system::os::getch: Now really does NOT wait for an enter, but just one keystroke in Linux.
		- mrpt::system::extractFileName didn't work for filenames already without an extension.
		- mrpt::utils::CMemoryChunk led to segmentation faults since the re-implementation with smart pointers.
		- Wrongly report missing parser for de-serializing a mrpt::slam::CSimplePointsMap object in the lastest binary format.
		- OpenGL render system had an inconsistence with the internal MRPT representation of 3D objects (by Pablo Moreno Olalla).

  <br><hr><br>
  <a name="0.6.3">
  <h2>Version 0.6.3: Released 23-Oct-2008.</h2></a>
	- Updated "INSTALL-COMPILE.html"
	- mrpt::gui windows can now be used also from user GUIs. This was previously possible from console apps only.
	- Supports compiling without OpenCV.
	- BUG FIXES:
		- GridmapNavSimul crashes on exit, for Windows only (SF <a href="https://sourceforge.net/tracker2/?func=detail&aid=2189710&group_id=205280&atid=993006" >2189710</a>).
		- Fixed possible compiling errors related to Boost (Thanks Xuan Vu!).
		- Wrong headers in some embedded jpeglib sources (Thanks Hai Li!).

  <br><hr><br>
  <a name="0.6.2">
  <h2>Version 0.6.2: Released 21-Oct-2008.</h2></a>
	- <b>IMPORTANT CHANGES</b>:
		- All metric maps now accept full 6D poses for evaluating observation likelihood, and rbpf-slam now always uses internally  6D poses.
		- mrpt::vision now integrates SIFT features computation thru Hess' C library (http://web.engr.oregonstate.edu/~hess/) (by Francisco Angel Moreno).
		- Many bug fixes, and new samples and applications.

	- Detailed list of changes:
		- All metric maps now accept full 6D poses for evaluating observation likelihood.
		- Added new macros for easier RTTI: IS_CLASS and IS_DERIVED.
		- New sensor class compatible with rawlog-grabber: A camera image grabber. See mrpt::hwdrivers::CCameraSensor.
		- xSens IMU MTi now works on both Windows & Linux with the new xSens software sources.
		- mrpt::hwdrivers::CIMUXSens now asks the IMU for calibrated acc. & gyr. data as well.
		- New methods in mrpt::math.
		- Linux & gcc only: Use --as-needed to avoid executables depending on useless .so libraries.
		- mrpt::gui::CDisplayWindow3D now can save the render canvas directy as a sequence of images, which helps to create videos from real-time animations.
		- mrpt::bayes::CParticleFilterCapable::computeResampling now implements all 4 resampling algorithms.
		- mrpt::vision now integrates SIFT features computation thru Hess' C library (http://web.engr.oregonstate.edu/~hess/) (by Francisco Angel Moreno).
		- mrpt::math::CMatrixTemplateNumeric::loadFromTextFile: Lines starting with '%' or '#' are interpreted as comments and ignored.
		- New methods in mrpt::utils::CImage for fast flip and Red-Blue swap.
		- More efficient capture of video in mrpt::vision::CImageGrabber_OpenCV (by Vicente Arevalo).
		- Applications renamed for consistency:
			- "pf-localization"
			- "rawlog-grabber"
		- New class mrpt::poses::CPoseRandomSampler to efficiently draw samples from any 2D/3D pose PDF. Used to refactor code in some SLAM classes. E.g: pf-localization is now 20% faster.
		- Boost libraries are required for compiling some parts of MRPT. Follow the updated instructions in http://www.mrpt.org/Building_and_Installing_Instructions.
		- mrpt::utils::CReferencedMemBlock redesigned: it was too obscure, now it's based on a simpler stlplus::smart_ptr
		- mrpt::utils::CFileOutputStream & mrpt::utils::CFileInputStream now has default constructors and "open" methods.
		- mrpt::opengl::CPlanarLaserScan implemented.
		- mrpt::slam::COccupancyGridMap2D: Simulator of laser scans now includes an optional noise in bearing angles.
		- rbpf-slam:
			- New version 0.3: A full 6D state if always kept, even if the robot moves on a flat surface only. This enables experimenting with 6D SLAM approaches based on particle filtering.
		- RawLogViewer, changes:
			- Proper display of IMU observations with all their data fields.
			- Dialog "Edit" now also processes files gz-compressed.
			- Dialog "Motion model" allows modification of a part of the rawlog only.
		- New examples:
			- feature_extraction: Test for image feature detection using KLT, Harris and SIFTs.
			- resampling-test: Particle filtering resampling schemes.
		- New applications:
			- ro-localization: Range-Only Localization with particle filters. See <a href="http://www.mrpt.org/Applications" >its wiki page</a>.
			- simul-grid: A command-line application to simulate rawlogs of laser scans using a world modelled by a grid map.
			- GridmapNavSimul: A GUI application which simulates a robot controlled by a joystick and generate rawlogs with noisy odometry and laser scans plus ground truth. See <a href="http://www.mrpt.org/Applications" >the wiki</a>.
		- BUGS FIXED:
			- Link errors in examples if paths have blank spaces.
			- ReactiveNavigationDemo crashes with VFF method (Xuan Vu: SF <a href="https://sourceforge.net/tracker2/?func=detail&aid=2183187&group_id=205280&atid=993006" >#2183187</a>).
			- RawlogViewer crashes when opening logs with embedded images (Xuan Vu: SF <a href="https://sourceforge.net/tracker2/?func=detail&aid=2182167&group_id=205280&atid=993006" >#2182167</a>).
			- USB laser scanner data acquisition: wrong bit mask in mm mode.
			- rawlog-grabber: Loss of 1 observation per grabbing period fixed.
			- mrpt::utils::CFileStream: Error when opening a file for append (fomAppend) and the file didn't exist.
			- mrpt::slam:COccupancyGridMap2D: fixed marking as occupied cells at the end of truncated rays when shorter than the "maxDistanceInsertion".
			- mrpt::vision::CImageGrabber_OpenCV didn't report an error when reaching the end of a video file.
			- Timestamps were lost in mrpt::slam::CAction 's when copied.
			- Linker errors when compiling an external application using MRPTConfig.cmake with GCC and in Debug.
			- Wrong destruction of critical sections when embedded into objects within smart_ptr's.
			- mrpt::slam::CActionRobotMovement2D: Wrong copy of objects with the = operator.
			- mrpt::utils::CFileGZOutputStream crashes if "close" called instead of closing at destructor.
			- Errors compiling with BUILD_ARIA=0.

  <a name="0.6.1">
  <h2>Version 0.6.1: Released 27-Aug-2008.</h2></a>
	- HOKUYO communications reconnect even if USB cable unplugged & plugged again.
	- New, more robust version of Linux serial port.
	- particleFilterApplication now supports ".gz" input files as well.
	- "zlib" & "jpeglib" included as separate libraries, for Windows in the case of not having wxWidgets.
	- xmlParser library files (src/core/utils/xmlparser/*): Legal issue fixed by replacing license with a BSD-like license from Frank Vanden Berghen (thanks!).
	- New target (Linux only): "make documentation_tutorial".
	- New examples:
		- gravity3d: Demonstration of 3D animation and physics simulation.
	- New applications:
		- simul-landmarks
		- simul-beacons
	- BUGS FIXED:
		- Communications with HOKUYO in Linux eventually crashes.
		- Error in "make install", tried to copy tutorial.ps.gz intended for Debian packages. (Thanks sisyphus me)
		- Wrong text format of dates & times in mrpt::system functions.
		- Mismatched new / free[] solved in mrpt::system::createThread (Linux).
		- Applications icp-slam, kf-slam, rbpf-slam didn't recognize gz-compressed rawlogs.
	- All Debian scripts & files moved away from MRPT source tree for consistency.

  <br><hr><br>
  <a name="0.6.0">
  <h2>Version 0.6.0: Released 14-Aug-2008.</h2></a>

	- <b>IMPORTANT CHANGES</b>:
		- All classes reorganized into a new set of namespaces. Existing user-code out of MRPT will not compile, though changes typically imply just changing the names of the included files and adding some "using namespace XXX;" lines.
		- Better support of user applications using CMake command "FIND_PACKAGE( MRPT REQUIRED )". Refer to: http://www.mrpt.org/Writing_applications_with_the_MRPT_C%2B%2B_Library
		- Pointers have been passed to smart pointer all across the code.
		- RawLog files: a new "observations-only" format has been introduced, and now all rawlog are saved compressed using "gzip" automatically (they are decompressed automatically to the user when loaded). Alternatively the user can rename a rawlog file to "name.rawwlog.gz", then use standard tools to extract the uncompressed version, if necessary.
		- The MRPT libraries can now be compiled as a shared library (.dll/.so), by setting MRPT_BUILD_SHARED_LIB from the CMake GUI.
		- New hardware libraries supported:
			- Cross-platform (Windows/Linux) library ARIA (distributed by ActivMedia Robotics under GNU GPL) integrated into MRPT. Select the appropriate switch in CMake to enable it. See hwdrivers::CActivMediaRobotBase.
			- New hardware driver for XSens inertial measuring units (IMU). See hwdrivers::CIMUXSens.
		- New applications:
			- RawLogGrabber, for gathering data in real-time from a robotic platform. See wiki page: http://www.mrpt.org/Application:RawLogGrabber
			- hmtMapViewer, an inspection tool for HMT-SLAM. See wiki page: http://www.mrpt.org/HMT-SLAM:An_Open_Source_Implementation
		- Visual Studio 6 is not supported anymore from now on: it's a too old compiler, buggy, and lacks many modern C++ features.
		- Many bugs fixed.

	- All changes:
		- Many additions to conform to Debian packaging standards and freedesktop.org standard files for creating application menus, etc.
		- Added new "colored points" map & opengl objects. See mrpt::slam::CColouredPointMap and mrpt::opengl::CPointCloudColoured.
		- SceneViewer also is able to read from .gz'd files (.3Dscene files, but can be renamed to .3Dscene.gz if desired).
		- Management of local and UTC times clarifies with new methods. See mrpt::system, time functions.
		- Added a new class for transparent disk-swapped loading of very large datasets. see mrpt::slam::CRawlogXXL.
		- Exception handling prepared to correctly manage bad_alloc exceptions (for out of memory scenarios).
		- Added a 1D function interpolator using splines. See mrpt::math::CSplineInterpolator1D.
		- Added a 6D poses interpolator, using splines. See mrpt::poses::CPose3DInterpolator.
		- Removed built-in zlib library, replaced by wxWidget's one.
		- Added support for easy read and write of .gz files. For the automatic streams, see mrpt::utils::CFileGZInputStream and mrpt::utils::CFileGZOutputStream. For manual operation, see mrpt::compress::zip.
		- New opengl object: mrpt::opengl::C3DSScene for importing complete scenes from 3D Studio files (using the open source lib3ds library, Copyright (C) 1996-2007 by Jan Eric Kyprianidis <www.kyprianidis.com> ).
		- Class for storing rawlogs renamed as "mrpt::slam::CRawlog", and enabled for the new observation-only <a href="http://www.mrpt.org/http://www.mrpt.org/Rawlog_Format">rawlog file format</a>.
		- Introduced precompiled headers for a dramatic reduction of compilation time thru the header <mrpt/slam.h>.
		- The method CSerializable::duplicate has been optimized using directly the derived class copy constructor instead of a temporary memory buffer.
		- Created a new metric map: grid map of heights. See mrpt::slam::CHeightGridMap2D.
		- Added a method for estimating the Jacobian of a function by finite differences. See math::estimateJacobian.
		- Source tree has undergone a re-ordering and rename. Now, libraries are named as: mrpt-core, mrpt-hwdrivers, mrpt-ann, etc. and all the sources are in <mrpt>/src.
		- Added generic class for Levenberg-Marquart optimization problems. See utils::CLevenbergMarquardt and the <a href="http://www.mrpt.org/Levenberg%E2%80%93Marquardt_algorithm">wiki tutorial on the topic</a>.
		- Added communications support for the new HOKUYO UTM-30LX (Windows and Linux). Refer to the <a href="http://www.mrpt.org/Example:HOKUYO_URG_Laser_Scanner" >wiki page</a>.
		- New method introduced to gas distribution mapping: optimized Kalman filter with constant time update. See mrpt::slam::CGasConcentrationGridMap2D.
		- The class utils::CTicTac now does not use dynamic memory, so it's constructor & destructor are faster.
		- Several optimizations applied to 2D and 3D geometry classes (poses::CPoseOrPoint derived ones).
		- Added cross-platform support for "alloca". See system::alloca.
		- Sources updated to be compatible with GCC 4.3 and Visual Studio C++ 2008.
		- Thread statistics now are also implemented for Linux. See system::getCurrentThreadTimes.
		- HWDRIVERS classes unified under the "generic sensor model". See hwdrivers::CGenericSensor.
		- 3D render canvas classes unified under the new class utils::WX_gui::CMyGLCanvas_Base.
		- Added serializable STL classes. See utils::vector_serializable, etc. See utils/stl_extensions.h
		- Added serialization to the whole mapping framework state in HMT-SLAM, and many related classes. See mrpt::hmtslam::CHMTSLAM.
		- lib_hwdrivers: Custom device-dependent initialization commands can be sent when opening a GPS interface. See hwdrivers::CGPSInterface.
		- Joysticks and game-pads fully supported now under both Windows and Linux. See hwdrivers::CJoystick.
		- Added new applications:
			- RawLogGrabber, for gathering data in real-time from a robotic platform. See wiki page: http://www.mrpt.org/Application:RawLogGrabber
			- hmtMapViewer, an inspection tool for HMT-SLAM. See wiki page: http://www.mrpt.org/HMT-SLAM:An_Open_Source_Implementation
		- Added new examples:
			- /samples/UTILS/fileSystemWatcher: Demonstrating watching of directory changes. Based on class utils::CFileSystemWatcher.
		- RawLogViewer:
			- A new facy custom tree view, extremely more efficient for very large datasets and with a time-line to graphically visualize timestamps.
			- Many new commands: resort from timestamps, regenerate timestamps from GPS observations, etc.
			- In the "raw-map" window the user can change now the number of types of maps to build from odometry + raw observations.
			- Implemented building a map from a rawlog with RTK GPS data. See "RawMap" dialog.
		- BUGS FIXED:
			- Textual representation of dates and times with seconds represented with just one digit.
			- Fixed wrong computation of transformation from GPS to local coordinates.
			- Exception raised if an empty CImage is serialized.
			- Wrong conversion of old serialization versions of gridmaps.
			- Parameters of a gas gridmap were ignored after loading from .ini.
			- Changing by hand the coordinates in mrpt::poses::CPose3D was not persistent when serializing the object.
			- Added missing "+ operators" to mrpt::poses::CPoint3D.
			- mrpt::slam::CSimplePointsMap: Serialization was not cross-platform (it used "unsigned long" data type).
			- mrpt::slam::CObservationGPS didn't save the "sensorPose" when streaming.

	- MAY-2008:
		- mrpt::poses::CPointsMap now also has a horizontal tolerance range for "horizontal" or "planar" maps.
		- Pseudorandom number generator algorithm Mersenne twister MT19937 implemented in utils::RandomGenerator for uniform distributions (in turn used for the rest). This assures same PRNs over different platforms.
		- gui::CDisplayWindowPlots now can also manage 2D bitmaps plots, that can be combined with 2D vectorial plots. See gui::CDisplayWindowPlots::image.
		- CPointsMap::compute3DMatchingRatio is now ~6 times faster (using KD-tree).
		- Missing implementation of KD-tree for 3D points in mrpt::poses::CPointsMap now complete.
		- Added support for file-system watching. See utils::CFileSystemWatcher.
		- SceneViewer:
			- New option to allow free rotation and zoom of the camera even for scenes with the flag "followCamera".
		- RawLogViewer:
			- Added importer for "rtl" log files.
			- Added capability to export several GPSs to text files.
			- Sensor labels appear in the tree view.
		- BUGS FIXED:
			- "mrpt::slam::CSensFrameProbSequence::operator =" was wrongly implemented.
			- In gridmaps (mrpt::slam::COccupancyGridMap2D):
				- Calling mrpt::slam::COccupancyGridMap2D::resizeGrid only supports growing the grid (not shrinking). This is now forced internally to fix impredictable results.
				- In some situations, inserting a 2D laser scan ray ending very close to the border didn't make the grid to extend its size, and may lead to memory corruption.
				- Disabled ROWSIZE_MULTIPLE_16 in mrpt::slam::COccupancyGridMap2D to avoid rare memory corruptions (bug to be tracked down if wanna enable it in the future again...).
			- Fixed support for Bumblebee in external applications through CMake system (MRPTconfig.cmake).
			- Wrong reporting of stacked exceptions messages.
			- Fixed wrong calculation of (x,y,z) coordinates from latitude, longitude and height, in mrpt::slam::CObservationGPS.
			- utils::CClientSocket did send debug information in the middle of actual data!. Thanks to Vicente Arevalo for reporting.

  <br><hr><br>
  <a name="0.5.5">
  <h2>Version 0.5.5: Released April 30, 2008.</h2></a>
	- APR-2008:
		- New 2D font render engine in utils::CCanvas. It now supports a number of different fonts, including CJK (Chinese-Japanese-Korean) characters.
		- HOKUYO URG04 laser scanner interface now works for Linux and Windows. See hwdrivers::CHokuyoURG and <a href="http://www.mrpt.org/Example:HOKUYO_URG_Laser_Scanner" >the wiki page</a>.
		- Serialization format changed: added an end-flag and 5 bytes saved per dumped object. See the serialization description <a href="http://www.mrpt.org/Serialization">here</a>.
		- Management of 3D scenes greatly improved through the introduction of multi-viewports. This has been a major change, but compatibility with old code has been maintained as much as possible. See opengl::COpenGLScene, or the <a href="http://www.mrpt.org/Tutorial_3D_Scenes">tutorial</a>.
		- Generation of HTML and CHM documentation has been integrated in the project workspace, as "make documentation_XXX" for "make" or new projects in Visual Studio.
		- Occupancy grids reimplemented with a discrete representation of log-odds. Most common operations are now faster (e.g. inserting a laser scan is x10 faster!). See mrpt::slam::COccupancyGridMap and <a href="http://www.mrpt.org/Occupancy_Grids" >the gridmaps tutorial</a>.
		- The file "MRPT_version.h" is now automatically updated from CMake with the version + SVN (if local copy is a repository).
		- Socket classes are now fully implemented for Linux and Windows. See utils::CClientTCPSocket and utils::CServerTCPSocket.
		- Added two new CStream classes: utils::CFileInputStream, utils::CFileOutputStream, specializations of the old utils::CFileStream.
		- File stream classes rewritten based on standard std::iostream classes (faster I/O). See utils::CFileStream.
		- Examples can now be built and run from any directory, through the new config file: "MRPT_examples_config.h".
		- Added support to any number of points map within mrpt::slam::CMultiMetricMap.
		- Added a new way to store images within utils::CImage: external storage in separate files, to enable managing very large rawlogs efficiently. It is transparent to the user, the images will be loaded automatically from disk when needed.
		- Options of mrpt::slam::COccupancyGridMap2D, mrpt::poses::CPointsMap and mrpt::slam:::CMultiMetricMap are now persistent (included in the serialization).
		- Macros max,min,max3,min3 passed to inline template functions, reusing std::max & std::min.
		- The following synchronization classes can now be safely copied (eg. within STL containers) and they still refer to the same OS's synchronization object: synch::CSemaphore, synch::CCriticalSection, synch::CEvent.
		- New class utils::CReferencedMemBlock for memory blocks that have to be shared between objects when copied with "=".
		- Major revision of many classes to satisfy design rules in Scott Meyers' "Effective C++ book" (-Weffc++ gcc option).
		- Added new examples:
			- /samples/MRML/pathPlanning: Demonstrating path finding in occupancy grid maps for circular robots.
			- /samples/MRML/benchmark-gridmap
			- /samples/MRML/gridMapLikelihoodCharacterization
			- /samples/HWDRIVERS/HOKUYO_laser_test: Added example for HOKUYO laser range finder.
		- Re-factorization of the main library (lib_MRPT) for faster rebuild and better modularity. New libraries in "/apps/lib_XXX":
			- lib_HMTSLAM.
			- lib_HWDRIVERS.
		- RawLogViewer version 2.2:
			- Now the user can choose the format (png,jpg,...) of the image files when generating image lists, transforming into external storage image observations, etc.
		- BUGS FIXED:
			- No timestamp saved in rawlogs for some observations.
			- Extra NULL character stored in type list in utils::CTypeSelector.

	- MAR-2008:
		- New classes added in file "UTILS/safe_pointers.h" for making classes with pointer members safer.
		- Added switches to CMake system to disable MRPT_START/END and ASSERT_ blocks, for speed up on well tested applications.
		- Added new class utils::CMemoryChunk.
		- Added a new application for building metric maps from ".simplemap" files.
		- Added a class for message passing between threads. See utils::CMessageQueue.
		- Created mrpt::slam::TKLDParams to unify KLD-sampling parameters across different classes.
		- mrpt::slam::CHybridMetricMap finally renamed mrpt::slam::CMultiMetricMapPDF, for consistency with mrpt::slam::CMultiMetricMap.
		- Particle filter classes re-organized: options are now kept in bayes::CParticleFilter, and steps are run with CParticleFilter::executeOn, to avoid pointers (less safe) and be more sensible.
		- Added a tolerance threshold for the pitch/roll of laser scans to be treated as "horizontal" in mrpt::slam::COccupancyGridMap2D.
		- A textual label added to all observations to describe its sensor (See CObservation::sensorLabel).
		- mrpt::slam::CSensoryFrame and mrpt::slam::CActionCollection now behave like STL containers (begin,end,iterator,erase,...); internal lists are protected.
		- All geometry, PDFs, and Kalman Filter classes passed from float -> double.
		- Added a new, more efficient implementation to bayes::CKalmanFilterCapable, processing one observation (not each scalar component) at once.
		- Added new fast operations to matrices. See math::CMatrixTemplateNumeric.
		- Fixed the small memory leak of the ANN library by calling annClose.
		- SceneViewer:
			- Scenes are compiled into OpenGL lists for efficiency.
		- RawLogViewer version 2.2:
			- Added a command to convert images of old rawlogs into delayed-load images.
			- Support for delayed-load images: now much larger rawlogs can be managed efficiently.
			- Added "compact rawlog" command: group consecutive actions & observations.
			- Added "loss-less decimation".
			- Added "edit rawlog" by name of sensor.
			- Now two monocular images can be displayed simultaneously in the "View images as video"-module.
			- Added "export/import to ALOG format".
		- BUGS FIXED:
			- Fixed compiling errors when compiling under Linux without GL/GLUT.
			- Wrong copy of 3D angles in CPose3DPDFGaussian::copyFrom.
			- Missing a valid, secure operator (operator =) for bayes::CParticleFilterData.
			- Compiling errors due to non-ASCII Spanish characters (thanks to Zhe Zhang).
			- Unpredictable result if a 2D laser scan with all invalid points is inserted in a pointsmap.

  <h2>Version 0.5.4: Released March 6, 2008.</h2>
	- MAR-2008:
		- Size variables in mrpt::poses::CPointsMap, math::CPolygon, and ReactiveNavigationLibrary passed to "size_t".
		- Thread join implemented in Windows.
		- Added a new class synch::CCriticalSectionLocker to assure consistent critical section leaving in any situation (exceptions, programmer's mistakes,...).
		- Implemented Iterative Kalman Filter (IKF). See bayes::CKalmanFilterCapable.
		- Next MRPT release will be published under the GNU GPL version 3 (comment blocks updated in all source files).
		- Correspondences between point maps implemented with KD-tree: "classic ICP" is now ~10x faster.
		- Removed wrong covariance estimation in mrpt::poses::CPointsMap::computeMatchingWith2D and added a new better one to mrpt::slam::CICP.
		- The library ANN (Approximate Nearest Neighbor) by Sunil Arya and David Mount (University of Maryland) has been integrated in the build tree.
		- Added an option to utils::CImage to disable ZIP compression when storing images.
		- Added the all new implementation of HMT-SLAM. See mrpt::slam::CHMTSLAM.
		- Fixed all 64-bit portability issues detected by Visual Studio 2005.
		- Created a repository of 3D objects in opengl::stock_objects.
		- COpenGLScene & opengl::CSetOfObjects now have the list of objects as "protected" for assure safe memory-management.
		- Added build options as switchs in the CMake build system for:
			- The GCC stdlibc++ "parallel mode". See http://algo2.iti.uni-karlsruhe.de/singler/mcstl/.
			- Selecting 8bits/16bits cell size in occupancy grid maps.
			- Enabling code profiling (GNU GCC only).
		- Added new applications:
			- icp-slam: A very naive mapping algorithm based on a single metric map & ICP laser scan alignment. See mrpt::slam::CMetricMapBuilderICP.
		- BUGS FIXED:
			- Copy operator of 6D poses didn't work always as expected.
			- Pose composition of 6D Gaussian poses (poses::CPose3DPDFGaussian) used wrong Jacobians.
	- FEB-2008:
		- Added a new application: camera-calib,  by Vicente Arevalo (University of Malaga).
		- Added "point size" property to UTILS:opengl::CPointCloud.
		- Windows only: OpenCV linked again as a DLL instead of a static library, in order to free 30Mb in MRPT packages, useless for non-Windows users.
		- RawlogViewer & SceneViewer: Directory of last opened file now saved when invoking programs with file as command-line argument.
		- Added a new mechanism for displaying mathematical graphs using MATLAB-like "plot" commands. See gui::CDisplayWindowPlots.
		- Visual Leak Detector integrated in the CMake build system.
		- Extended Kalman Filter implementation rewritten from scratch. See bayes::CKalmanFilterCapable.
		- Implemented missing operations (move,resize,...) in gui::CDisplayWindow & gui::CDisplayWindow3D.
		- New implementation of the Kalman Filter in mrpt::slam::CRangeBearingKFSLAM: 'a la Davison', orders of magnitude faster than naive Kalman (new version 0.2 of application "kf-slam").
		- Improved 3D/6D geometry implementation: x2 speed-up in (inverse) pose compositions (utils::CPoseOrPoint and derived classes).
		- Addition of a new matrix class: math::CMatrixFixed, for compile-time known sizes (more efficient??).
		- Fixed compiling errors if MRPT_HAS_WXWIDGETS = 0.
		- Generation of CMakeLists fully automated for samples.
		- Added support for "pkg-config libMRPT --libs --cflags" if the library is make install'ed,  UNIX only.
		- "Save as..." implemented in gui::CDisplayWindow.
		- Added new examples:
			- samples/UTILS/threadsTest, demonstrating the creation of threads and synchronization.
			- UTILS/benchmark-matrix.
			- samples/UTILS/bayesianTracking, implementing a very simple bearing & tracking problem with both a EKF and a particle filter (PF).
			- samples/UTILS/displayPlots.
			- MRML/grid-matching.
		- BUGS FIXED:
			- Core dump due to a wrong initialization of mrpt::slam::CSensoryFrame using the *copy constuctor*.
			- RawlogViewer: Changing a sensor pose in the "sensor/camera modification dialog" didn't change it for 6D poses.
			- utils::CDirectoryExplorer missed files under Windows when marked as FILE_ATTRIB_NORMAL.
			- Core dump (double free) if a matrix is set to size (0,0) and then resized again (it affected the new matrix loadFromTextFile method).
			- Inverted axes in CDisplayWindow3D & SceneViewer when camera elevation=-90deg.
			- Wrong generation of random samples in random::randomNormalMultiDimensional.
			- Missing synchronization within WxSubsystem's thread.
			- Not using double-buffering in SceneViewer.
			- Wrong simulation of bearing-range observations for any 6D pose in mrpt::slam::CLandmarksMap.
			- gui::CDisplayWindow & gui::CDisplayWindow3D timeout waiting to close windows after class destruction.

  <h2>Version 0.5.3  (First public release, January 31, 2008)</h2>
	- JAN-2008:
		- Small changes in CMake files to prepare first public source release.
		- utils::WxSubsystem created as a centralized manager of GUI windows in the MRPT using the cross-platform wxWidgets. Now used for CDisplayWindow & CDisplayWindow3D.
		- Application "ReactiveNavigationDemo" rewritten using wxWidgets and integrated in the MRPT CMake tree.
		- Bumblebee camera support integrated in CMake file system.
		- RawLogViewer 2.1 finished.
		- SceneViewer 1.1 finished.
		- New methods added to matrixes and to the MATH namespace.
		- "mrpt::slam::CSensFrameProbSequence" and "mrpt::slam::CIncrementalMapPartitioner" modified to work with full 3D/6D poses rather than planar robot poses only.
		- Added a working example of EKF-SLAM including a sample dataset.
		- Tree structure modified for clarity, clean up of files at the root directory.
		- RawLogViewer: Added viewer of mrpt::slam::CObservationBearingRange.
		- Added a new method to compute spherical coordinates of a 3D point from a 6D pose (see mrpt::poses::CPose3D).
		- MRPT_ALWAYS_CHECKS_DEBUG is now enabled by default.
		- Added new matrix operations to employ them in a more efficient EKF implementation.
		- Changes all across the library to enable compilation in 64 bit targets (without warnings).
		- getCurrentTime now gives high-resolution time (usecs) in Linux.
		- Added the option "justOneOccupiedCellPerRay" to mrpt::slam::COccupancyGridMap2D.
		- Added new applications:
			- map-partition.
			- KF-SLAM.
		- Added new examples:
			- MRML/landmarkRawlogSimulator, a simulator of a robot with a 3D range-bearing sensor.
		- BUGS FIXED:
			- CSemaphore::waitForSignal now also works for an timeout=0 (no timeout).
			- In openCV_cross_correlation: wrong order of (v,u) coordinates! Have been replaced by (x,y) to prevent more errors.
			- Wrong calculation of mean in CGraphPartitioner::SpectralBisection.
			- Wrong calculations in "multiply_HCHt"
			- "dateTimeToString" gave wrong data.
			- Out of vector bound exception in RawLogViewer exporter to text files.
			- Wrong normalization of 3D vectors in generateAxisBaseFromDirection.

	- DEC-2007:
		- Speed up in rendering of 3D ellipsoids by computing eigenvectors only once.
		- Implemented RO-SLAM with SOG (mrpt::slam::CBeaconMap)
		- Added a method to generate permutations of vectors (See utils::RandomGenerator).
		- Started RawLogViewer 2.1: Added a new module for laser scan animations.
		- Added a new cross-platform 3DSceneViewer application.
		- Deleted the old application x3DSceneViewer
		- New example: MRML/geometry3D
		- BUGS FIXED:
			- Wrong initialization of CMatrix from a CPoint3D
			- Wrong calculation of yaw/pitch/roll fields in CPose3D when composing poses.

  <br><hr><br>
  <h2>Version 0.5.2 (Released December 26, 2007)</h2>
	- DEC-2007:
		- Release of RawLogViewer 2.0, the first portable (win32/linux-gtk) version.
		- Old win32 version "RawLogsViewer" removed from the repository!
		- GUI classes (CDisplayWindow, CDisplayWindow3D) have been rewritten in:
			- Windows: Native MSW API.
			- Linux: GTK2
		- New features to RawLogViewer (change sensor poses,...)
		- Added general abstract methods to CObservation for getting/setting the sensor pose.
		- Added to RawLogViewer: Import a directory of images as a rawlog.
		- utils::CDirectoryExplorer rewritten & ported to Linux.
		- BUG FIXED: 'system::sleep' was aborted by signals in Linux. It now resumes until the desired delay.
		- Linux implementation for CSerialPort.
		- RawLogViewer: ScanMatching module finished.
		- New class for landmark observations by bearing and range (See mrpt::slam::CObservationBearingRange).
		- Timestamps have been added to CAction's.
		- BUG FIXED: Invalid encoder info in CActionRobotMovement2D.
		- New methods added to CRawlog to enable a better encapsulation of the internal list of objects as "private".
	- NOV-2007:
		- BUG FIXED: CObservationGPS didn't load from CStream in Linux. Serialization version 1 is now portable.
		- Added the sprintf-like function "format" for std::string's.
		- Integration of xmlParser library within utils::CSimpleDatabase.
		- A new class for managing the custom USB board that interfaces SRF10 ultrasonic range finders. Class hwdrivers::CBoardSonars.
		- Class hwdrivers::CBoardENoses has been finished and tested with the real devices.
		- Many includes in "utils_defs.h" moved to those source files really requiring them, to speed up compilation.
		- Fixs to allow compiling using the free Borland C++ 5.5.1 compiler from CMake makefiles.
		- Added new sample for eNose board interface.

  <h2>Version 0.5.1 (Released for BABEL, November 13, 2007)</h2>
	- NOV-2007:
		- BUG FIXED: math::generateAxisBaseFromDirection now returns all the three vectors normalized.
		- Implemented Sum of Gaussians (SOG) as a way to create the PDF of a beacon for RO-SLAM (in mrpt::slam::CBeaconMaps).
		- Added a new example to "samples/MRML": beaconRawlogSimulator.
		- Added a new class for interfacing an "e-Noses" board via USB, in hwdrivers::CBoardENoses.
		- OpenCV added as a monolithic, static linking library to avoid DLL dependencies (for Windows).
		- New wiki-based website. Old doxygen-based documentation headers (.h) removed.
		- BUG FIXED: Memory leaks in the MRPT class registration system and in utils::CTicTac have been removed (Using "Visual Leak Detector" it seems there are no more leaks by now...).
		- BUG FIXED: THROW_EXCEPTION_XXX macros redesigned to avoid eventual exceptions in wxWidgets applications.

	- OCT-2007:
		- Methods "computeMatchingWith2D" modified for a sensible usage of the "const" modifier.
		- Changed mrpt::slam::CMetricMapsAlignmentAlgorithm::Align to avoid the warning "break strict-aliasing rules" about the CPosePDF**.
		- Added tutorial: Serialization.
		- Added new class mrpt::poses::CPose3DPDFSOG for Sum of Gaussians-representation of a 6D pose probability density.
		- Reactive Navigation Library ported from Win32 API to standard C++.
		- Resampling methods now are separated in a generic, static method bayes::CParticleFilterCapable::computeResampling.

  <h2>Version 0.4 (Released for BABEL, October 13, 2007)</h2>
	- OCT-2007:
		- BUG FIXED: Loading grayscale images CImageFloat now forces to load images from files as grayscale always.
		- BUG FIXED: To allow inserting into gridmaps laser scans taken with the sensor bottom-up.
		- All the makefiles of MRPT has been moved to CMake.
	- SEP-2007:
		- Structure of "include" directory changed to include the prefix "<MRPT/...". This is for leading to a one-directory install into "usr/include" in Linux systems.
		- CSemaphore implemented for Linux using pthreads.
		- CDisplayWindow3D is now based on GLUT and works in Windows & Linux.
		- Added a new application by Antonio J. Ortiz de Galistea: a MonoSLAM demo.
		- Different PF resampling methods are now supported. See CParticleFilter::resample.
		- CParticleFilter now has a centralized structure for all the PF-related options.
		- BUG FIXED: CDisplayWindow2D now has an extra (global) thread to process OpenCV messages without the need for the application to call "waitForKey".
		- Added a new procedure into CMultiMetricMapPDF for expanding the potential range of observation likelihoods without having a numerical overflow.
		- Add Class for Sonar SRF10 Interface (AJOGD).
		- CImage now is a wrapper class over OpenCV. More image file types are supported now.
		- CDisplayWindow now completely portable between windows/Linux thorugh OpenCV.
	- AUG-2007:
		- BUG FIXED: In utils::CConfigFile.
		- Started new application RawLogViewer using wxWidGets for a portable version (windows/linux).
		- Added utils::getAllRegisteredClasses to obtain at runtime the list of all MRPT registered classes.
		- Started the porting towards "bakefiles"
		- A new function added to obtain the compilation date and the library version (including SVN). See utils::SystemUtils.
		- Started a new version of the application "RawLogViewer" in portable wxWidGets for linux compatibility!.
		- Borland C++ 5.5 not supported any more, due to a lot of internal compiler errors. We use now BCC version 6.0.
		- Range-Only SLAM will be integrated into existing SLAM classes through the new class mrpt::slam::CBeaconMap, now fully integrated into mrpt::slam::CMultiMetricMap.
		- A new class for integrating different PDF representations of beacons (mrpt::slam::CBeacon) and the corresponding map (mrpt::slam::CBeaconMap).
	- JUL-2007:
		- A new class of CAction for 6D movements (mrpt::slam::CActionRobotMovement3D).
		- CStream & CSerializable now have more consistent "const" declarations.

  <hr>
  <h2>Version 0.3 (Released for BABEL v3.800, July 2007)</h2>

	- JUL-2007:
		- A new class for Range-Only Localization (mrpt::slam::CRejectionSamplingRangeOnlyLocalization).
		- Range-Only SLAM is now supported with a new map class (mrpt::slam::CBeaconMap), which has been also integrated in mrpt::slam::CMultiMetricMap.
		- A new class for parsing messages from/to a USB FTDI device (hwdrivers::CInterfaceFTDIMessages).
		- Added a new class for 3D Point PDF representation as a SOG (poses::CPoint3DPFGSOG).


  <br><hr><br>
  <h2>Version 0.2</h2>

	- JUN-2007:
		- From now on MRPT is updated through Subversion (SVN).
		- Start solving portability issues towards a full Linux support.
	- MAY-2007:
		- Included the makefile project for "Code::Blocks" compiler, for supporting Linux GCC C++ compiler.
	- FEB-2007:
		- BUG FIXED: In "CSerializable::duplicate()"
		- Added new config file-like interfaces. See utils::CConfigFileBase and derived classes.
		- Added utils::CStringList.
	- JAN-2007:
		- New fast image correlation method based on 2D FFT. See CImage::cross_correlation_FFT
		- 2D FFT functions added to utils::MATH
		- The class utils::CConfigFile now can read entries as "std::vector"'s.
		- New utilities added to utils::MRPT_OS for tokenizing strings.

  <br><hr><br>
  <h2>Version ALFA 0.1 (Published with BABEL v3.0.0, Jan-2007)</h2>

	- JAN-2007:
		- The classes "utils::CBitmapXX" have been removed, and BMP image format operations appropriately integrated into existing utils::CImage and utils::CImageFloat classes.
		- Now 3D scenes are supported through "opengl::COpenGLScene", objects in "mrpt::opengl" namespace, and an external 3D Scene viewer application for Windows.
	- DEC-2006:
		- Addition of TCP/IP sockets, through classes utils::CServerTCPSocket and utils::CClientTCPSocket, allowing serialization of MRPT objects through a TCP socket.
		- New classes in HWDRIVERS namespace allow using serial ports and interfacing GPS units.
		- A new interface utils::CCanvas has been defined for enabling drawing primitives to be implemented in graphical classes.
		- New class gui::CDisplayWindow for visualization of images in a GUI.
		- Portability issues solved for compatibility with Microsoft Visual Studio 2005 (VC8), including the new standard C++ library "secure" functions.
		- A new class "mrpt::slam::CMultiMetricMap" has been introduced for storing any set of metric maps, in a much more convenient and flexible way that the previous (non existing now) class "mrpt::slam::CHybridMetricMap"
	- NOV-2006:
		- The new Optimal Auxiliary Particle Filter has been implemented in "poses::CPosePDFParticles", and in "mrpt::slam::CMultiMetricMapPDF"
		- BUG FIXED: In matrix inversion, into class "math::CMatrixTemplateNumeric".
		- RANSAC method added for correspondences-pair lists, in "mrpt::slam::CICP::robustRigidTransformation"
	- OCT-2006:
		- The Dieter Fox's algorithm "KLD-sampling" for adative particle filter has been implemented in the class "poses::CPosePDFParticles"
		- Major changes in the way bayes::CParticleFilter works: Now there are four diferent particle filter algorithms defined.
		- New utils::MATH functions added (chi-square and normal quantiles, etc...)
		- A new probabilistic motion model has been added to "mrpt::slam::CActionRobotMovement2D", the particles draw process described in Thrun's 2006 book.
	- SEP-2006:
		- The Lu & Milios algorithm now works with any user-supplied matrix of pose constrains. See "mrpt::slam::CConsistentObservationAlignment".
		- A new generic matrix template has been created, "math::CMatrixTemplate" and "math::CMatrixTemplateNumeric", which are now the base for math::CMatrix and math::CMatrixD.
		- Template functions defined for dealing with STL's vectors: basic math operations, printing to screen,... in "utils::MATH"
		- The mrpt::slam::CICP algorithm now also deals with mrpt::slam::CLandmarksMap metric maps.
	- AUG-2006:
		- A new SLAM framework has been implemented in the main class mrpt::slam::CHierarchicalMappingFramework
		- A new class added for managing INI-like config files "utils::CConfigFile".
		- Hill-climbing algorithm "utils::CHillClimbing" added to the library.
		- Improved SIFT features detection, triangulation and matching in "mrpt::slam::CLandmarksMap" for pairs of stereo images.
	- JUN-2006:
		- New functions added to "poses::CPointPDFGaussian" for Bayesian fusion, correspondence likelihood,...
	- MAY-2006:
		- Introduction of "pixel to 3D" functions in vision
		- SIFT features extractor moved to vision
	- ABR-2006:
		- mrpt::slam::CLandmarksMap now works with SIFT features: loading from images, fusing,...
		- SIFT features extractor in "utils::CImage"
	- FEB-2006:
		- The class mrpt::slam::COccupancyGrid2D now uses 16bit for storing cell values, and many new options added.
		- Rao-Blackwellized Particle filters now supported in class mrpt::slam::CMultiMetricMapPDF
	- JAN-2006:
		- Landmarks-based maps now supported with mrpt::slam::CLandmarksMap
		- ZIP compression supported with the introduction of classes in the new namespace compress::zip.
		- Rao-Balckwellized particle filter mapping supported with mrpt::slam::CMetricMapBuilderRBPF
		- Points map are now fused considering the weights of points (mrpt::slam::CSimplePointsMap)
		- Definition of a new class mrpt::slam::CMetricMapBuilding for grouping map building methods.
		- Addition of a new class for 3D occupancy grid building.
		- New types of points map derived from mrpt::slam::CMetricMap.
	- DEC-2005:
		- New methods added to MRML, for circular robots paths planning. See the base class mrpt::slam::CPathPlanningMethod
		- The method by Lu & Milios for consistent maps alignment has been integrated into the MRML library, in the mrpt::slam::CConsistentObservationAlignment
		- The CObservation-derived classes has been modified to adhere to the "BABEL modules proposal for robotics architecture" (Technical report, dec.2005)
		- Metrics maps has been grouped using a common base class: mrpt::slam::CMetricMap
		- The spectral-based method for maps partitioning has been integrated into the MRML library, in the class mrpt::slam::CIncrementalMapPartitioner.
		- CObservationLaserRangeScan renamed as CObservation2DRangeScan.
	- NOV-2005:
		- New types of observations added (mrpt::slam::CObservation descendant classes) for images (from a camera) and for sonar readings.
	- OCT-2005:
		- New class added: A simple SLAM method for 2D localization and simple, metric maps building (non-hierarchical global maps). See mrpt::slam::CSimple2DSLAM.
		- Addition of "streaming versioning" for mrpt::utils::CSerializable classes.
		- A new observation type has been introduced, the mrpt::slam::CObservationIRRing.
		- Introduction of 3D poses using 4x4 transformation matrices and yaw/pitch/roll angles.
		- Polymorphism mechanishs for mrpt::utils::CSerializable derived classes and correct streaming using "<<" and ">>" operators has been provided through a set of "#define"'s and functions. See CSerializable.h
	- SEP-2005:
		- Reactive Navigator totally rewritten for integration into MRPT applications repository (See MRPT::reactivenav::CReactiveNavigationSystem).
		- Added MRPT applications repository section.
	- AUG-2005:
		- Particle filters basic algorithm added to UTILS library.
 		- Bitmap classes (utils::CBitmap) totally rewritten for a sensible use of classes inheritance.
	- JUN-2005:
		- First version of the library! there are a few classes for matrix manipulation, 2D point map management, and an implementation of the ICP algorithm.

*/
