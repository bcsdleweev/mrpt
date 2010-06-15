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

#include <mrpt/hwdrivers.h>
#include <mrpt/gui.h>
#include <mrpt/maps.h>

using namespace mrpt;
using namespace mrpt::hwdrivers;
using namespace mrpt::math;
using namespace mrpt::gui;
using namespace mrpt::utils;
using namespace std;

// ------------------------------------------------------
//				Test_SwissRanger
// ------------------------------------------------------
void Test_SwissRanger()
{
	CSwissRanger3DCamera   cam;

	// Set params:
	cam.setOpenFromUSB(true);

	cam.setSave3D(true);
	cam.setSaveRangeImage(true);
	cam.setSaveIntensityImage(true);
	cam.setSaveConfidenceImage(false);

	//cam.enablePreviewWindow(true);

	// Open:
	cam.initialize();

	if (cam.isOpen())
		cout << "[Test_SwissRanger] Camera open, serial #" << cam.getCameraSerialNumber()
			<< " resolution: " << cam.getColCount() << "x" << cam.getRowCount()
			<< " max. range: " << cam.getMaxRange()
			<< endl;

	const double aspect_ratio = cam.getRowCount() / double( cam.getColCount() );

	{
		std::string ver;
		cam.getMesaLibVersion(ver);
		cout << "[Test_SwissRanger] Version: " << ver << "\n";
	}

	CObservation3DRangeScan  obs;
	bool there_is_obs=true, hard_error;

	mrpt::gui::CDisplayWindow3D  win3D("3D camera view",800,600);

	win3D.setCameraAzimuthDeg(140);
	win3D.setCameraElevationDeg(20);
	win3D.setCameraZoom(6.0);
	win3D.setCameraPointingToPoint(2.5,0,0);

	//mrpt::gui::CDisplayWindow  win2D("2D range image",200,200);
	//mrpt::gui::CDisplayWindow  winInt("Intensity range image",200,200);
//	win2D.setPos(10,10);
//	winInt.setPos(350,10);
//	win3D.setPos(10,290);
//	win3D.resize(400,200);

	//mrpt::opengl::CPointCloudPtr gl_points = mrpt::opengl::CPointCloud::Create();
	mrpt::opengl::CPointCloudColouredPtr gl_points = mrpt::opengl::CPointCloudColoured::Create();
	gl_points->setPointSize(4.5);

	mrpt::opengl::CTexturedPlanePtr gl_img_range =  mrpt::opengl::CTexturedPlane::Create(0.5,-0.5,-0.5*aspect_ratio,0.5*aspect_ratio);
	mrpt::opengl::CTexturedPlanePtr gl_img_intensity =  mrpt::opengl::CTexturedPlane::Create(0.5,-0.5,-0.5*aspect_ratio,0.5*aspect_ratio);


	{
		mrpt::opengl::COpenGLScenePtr &scene = win3D.get3DSceneAndLock();

		// Create the Opengl object for the point cloud:
		scene->insert( gl_points );
		scene->insert( mrpt::opengl::CGridPlaneXY::Create() );
		scene->insert( mrpt::opengl::stock_objects::CornerXYZ() );

		// Create the Opengl objects for the planar images, as textured planes, each in a separate viewport:
		opengl::COpenGLViewportPtr viewRange = scene->createViewport("view2d_range");
		scene->insert( gl_img_range, "view2d_range");
		viewRange->setViewportPosition(0.005,0.65, 0.3,0.3 );
		viewRange->setTransparent(true);
		viewRange->getCamera().setOrthogonal(true);
		viewRange->getCamera().setAzimuthDegrees(90);
		viewRange->getCamera().setElevationDegrees(90);
		viewRange->getCamera().setZoomDistance(1.0);

		opengl::COpenGLViewportPtr viewInt = scene->createViewport("view2d_int");
		scene->insert( gl_img_intensity, "view2d_int");
		viewInt->setViewportPosition(0.005,0.1,0.3,0.3 );
		viewInt->setTransparent(true);
		viewInt->getCamera().setOrthogonal(true);
		viewInt->getCamera().setAzimuthDegrees(90);
		viewInt->getCamera().setElevationDegrees(90);
		viewInt->getCamera().setZoomDistance(1.0);

		win3D.addTextMessage(0.1,0.97,"Range data",TColorf(1,1,1), 1 );
		win3D.addTextMessage(0.1,0.42,"Intensity data",TColorf(1,1,1), 2 );

		win3D.unlockAccess3DScene();
		win3D.repaint();
	}


	CTicTac tictac;
	size_t nImgs = 0;

	bool endLoop = false;

	while (there_is_obs && !endLoop )
	{
		// Grab new observation from the camera:
		cam.getNextObservation(obs,there_is_obs,hard_error);

		// Show ranges as 2D:
		if (there_is_obs && obs.hasRangeImage )
		{
			mrpt::utils::CImage  img;
			// Normalize the image
			CMatrixFloat  range2D = obs.rangeImage;
			range2D*= 1.0/ cam.getMaxRange();
			img.setFromMatrix(range2D);
			
			win3D.get3DSceneAndLock();
				gl_img_range->assignImage_fast(img);
			win3D.unlockAccess3DScene();
		}

		// Show intensity image:
		if (there_is_obs && obs.hasIntensityImage )
		{
			win3D.get3DSceneAndLock();
				gl_img_intensity->assignImage(obs.intensityImage); // Note: "_fast" destroys the original image
			win3D.unlockAccess3DScene();
		}

		// Show 3D points:
		if (there_is_obs && obs.hasPoints3D )
		{
			//mrpt::slam::CSimplePointsMap  pntsMap;
			CColouredPointsMap pntsMap;
			pntsMap.colorScheme.scheme = CColouredPointsMap::cmFromIntensityImage;
			pntsMap.loadFromRangeScan(obs);

			win3D.get3DSceneAndLock();
				gl_points->loadFromPointsMap(&pntsMap);
			win3D.unlockAccess3DScene();
			win3D.repaint();
		}

		nImgs++;
		if (nImgs>10)
		{
			win3D.get3DSceneAndLock();
				win3D.addTextMessage(0.01,0.01, format("%.02f Hz", nImgs / tictac.Tac() ), TColorf(0,1,1), 100, MRPT_GLUT_BITMAP_HELVETICA_12 );
			win3D.unlockAccess3DScene();
			nImgs=0;
			tictac.Tic();
		}


		// Process possible keyboard commands:
		// --------------------------------------
		if (win3D.keyHit())
		{
			const int key = tolower( win3D.getPushedKey() );
			//cout << "key: " << key << endl;

			switch (key)
			{
				case 'h':
					cam.enableImageHistEqualization( !cam.isEnabledImageHistEqualization() );
					break;
				case 'g':
					cam.enableConvGray( !cam.isEnabledConvGray() );
					break;
				case 'd':
					cam.enableDenoiseANF( !cam.isEnabledDenoiseANF() );
					break;
				case 'f':
					cam.enableMedianFilter( !cam.isEnabledMedianFilter() );
					break;
				case 27:
					endLoop = true;
					break;
			}
		}

		win3D.get3DSceneAndLock();
		win3D.addTextMessage(0.08,0.02,
			format("Keyboard switches: H (hist.equal: %s) | G (convGray: %s) | D (denoise: %s) | F (medianFilter: %s)",
					cam.isEnabledImageHistEqualization() ? "ON" : "OFF",
					cam.isEnabledConvGray() ? "ON" : "OFF",
					cam.isEnabledDenoiseANF() ? "ON" : "OFF",
					cam.isEnabledMedianFilter() ? "ON" : "OFF" ),
				TColorf(0,0,1), 110, MRPT_GLUT_BITMAP_HELVETICA_18 );
		win3D.unlockAccess3DScene();

		mrpt::system::sleep(1);
	}
}


int main(int argc, char **argv)
{
	try
	{
		Test_SwissRanger();
		return 0;

	} catch (std::exception &e)
	{
		std::cout << "EXCEPCION: " << e.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		printf("Another exception!!");
		return -1;
	}

}