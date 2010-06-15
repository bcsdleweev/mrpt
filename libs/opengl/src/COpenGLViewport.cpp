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

#include <mrpt/opengl.h>  // Precompiled header


#include <mrpt/opengl/COpenGLViewport.h>
#include <mrpt/opengl/COpenGLScene.h>
#include <mrpt/opengl/CSetOfObjects.h>
#include <mrpt/opengl/CTexturedPlane.h>
#include <mrpt/utils/CStringList.h>
#include <mrpt/math/CVectorTemplate.h>

#include "opengl_internals.h"

using namespace mrpt;
using namespace mrpt::poses;
using namespace mrpt::opengl;
using namespace mrpt::utils;
using namespace mrpt::math;
using namespace std;

#include <mrpt/utils/metaprogramming.h>
using namespace mrpt::utils::metaprogramming;

IMPLEMENTS_SERIALIZABLE( COpenGLViewport, CSerializable, mrpt::opengl )


/*--------------------------------------------------------------

			IMPLEMENTATION OF COpenGLViewport

  ---------------------------------------------------------------*/

/*--------------------------------------------------------------
					Constructor
  ---------------------------------------------------------------*/
COpenGLViewport::COpenGLViewport( COpenGLScene *parent, const string &name  ) :
	m_camera(),
	m_parent( parent ),
	m_isCloned(false),
	m_isClonedCamera(false),
	m_clonedViewport(),
	m_name(name),
	m_isTransparent(false),
	m_borderWidth(0),
	m_view_x(0),
	m_view_y(0),
	m_view_width(1),
	m_view_height(1),
	m_clip_min(0.1),
	m_clip_max(10000),
	m_custom_backgb_color(false),
	m_background_color(0.6,0.6,0.6),
	m_objects()
{

}


/*--------------------------------------------------------------
					Destructor
  ---------------------------------------------------------------*/
COpenGLViewport::~COpenGLViewport()
{
	clear();
}

/*--------------------------------------------------------------
					setCloneView
  ---------------------------------------------------------------*/
void COpenGLViewport::setCloneView( const string &clonedViewport )
{
	clear();
	m_isCloned = true;
	m_clonedViewport = clonedViewport;
}

/*--------------------------------------------------------------
					setViewportPosition
  ---------------------------------------------------------------*/
void COpenGLViewport::setViewportPosition(
	const double x,
	const double y,
	const double width,
	const double height )
{
	MRPT_START
	ASSERT_( m_view_width>0)
	ASSERT_( m_view_height>0)

	m_view_x = x;
	m_view_y = y;
	m_view_width = width;
	m_view_height = height;

	MRPT_END
}

/*--------------------------------------------------------------
					getViewportPosition
  ---------------------------------------------------------------*/
void COpenGLViewport::getViewportPosition(
	double &x,
	double &y,
	double &width,
	double &height )
{
	x = m_view_x;
	y = m_view_y;
	width = m_view_width;
	height = m_view_height;
}

/*--------------------------------------------------------------
					clear
  ---------------------------------------------------------------*/
void COpenGLViewport::clear()
{
	m_objects.clear();
}

/*--------------------------------------------------------------
					insert
  ---------------------------------------------------------------*/
void COpenGLViewport::insert( const CRenderizablePtr &newObject )
{
	m_objects.push_back(newObject);
}


/*---------------------------------------------------------------
						render
 ---------------------------------------------------------------*/
void  COpenGLViewport::render( const int render_width, const int render_height  ) const
{
#if MRPT_HAS_OPENGL_GLUT
	CListOpenGLObjects::const_iterator		it=m_objects.end();
	try
	{
		// Change viewport:
		// -------------------------------------------
		const GLint vx = m_view_x>1 ?  GLint(m_view_x) : ( m_view_x<0 ? GLint(render_width+m_view_x)  : GLint( render_width  * m_view_x ) );
		const GLint vy = m_view_y>1 ?  GLint(m_view_y) : ( m_view_y<0 ? GLint(render_height+m_view_y) : GLint( render_height * m_view_y ) );

		GLint vw;
		if (m_view_width>1)  // >1 -> absolute pixels:
			vw = GLint(m_view_width);
		else if (m_view_width<0)
		{  // Negative numbers: Specify the right side coordinates instead of the width:
			if (m_view_width>=-1)
			     vw = GLint( -render_width * m_view_width - vx +1 );
			else vw = GLint( -m_view_width - vx + 1 );
		}
		else  // A factor:
		{
			vw = GLint( render_width * m_view_width );
		}


		GLint vh;
		if (m_view_height>1)  // >1 -> absolute pixels:
			vh = GLint(m_view_height);
		else if (m_view_height<0)
		{  // Negative numbers: Specify the right side coordinates instead of the width:
			if (m_view_height>=-1)
			     vh = GLint( -render_height * m_view_height - vy + 1);
			else vh = GLint( -m_view_height - vy +1 );
		}
		else  // A factor:
			vh = GLint( render_height * m_view_height );

        glViewport(vx,vy, vw, vh );

		// Clear depth&/color buffers:
		// -------------------------------------------
		m_lastProjMat.viewport_width  = vw;
		m_lastProjMat.viewport_height = vh;

        glScissor(vx,vy,vw,vh);

        glEnable(GL_SCISSOR_TEST);
        if ( !m_isTransparent )
        {   // Clear color & depth buffers:
        	// Save?
			GLdouble old_colors[4];
        	if (m_custom_backgb_color)
        	{
        		glGetDoublev(GL_COLOR_CLEAR_VALUE, old_colors );
        		glClearColor(m_background_color.R,m_background_color.G,m_background_color.B,m_background_color.A);
        	}

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |  GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Restore old colors:
        	if (m_custom_backgb_color)
        		glClearColor(old_colors[0],old_colors[1],old_colors[2],old_colors[3]);
        }
        else
        {   // Clear depth buffer only:
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        glDisable(GL_SCISSOR_TEST);

		// Set camera:
		// -------------------------------------------
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

		const CListOpenGLObjects	*objectsToRender;
		COpenGLViewport		*viewForGetCamera;

		if (m_isCloned)
		{	// Clone: render someone's else objects.
			ASSERT_(m_parent.get()!=NULL);

			COpenGLViewportPtr view = m_parent->getViewport( m_clonedViewport );
			if (!view)
				THROW_EXCEPTION_CUSTOM_MSG1("Cloned viewport '%s' not found in parent COpenGLScene",m_clonedViewport.c_str());

			objectsToRender = &view->m_objects;
			viewForGetCamera = m_isClonedCamera ? view.pointer() : const_cast<COpenGLViewport*>(this);
		}
		else
		{	// Normal case: render our own objects:
			objectsToRender = &m_objects;
			viewForGetCamera = const_cast<COpenGLViewport*>(this);
		}

		// Get camera:
		// 1st: if there is a CCamera in the scene:
		CRenderizablePtr cam_ptr = viewForGetCamera->getByClass<CCamera>();

		CCamera *myCamera=NULL;
		if (cam_ptr.present())
		{
			myCamera = getAs<CCamera>(cam_ptr);
		}

		// 2nd: the internal camera of all viewports:
		if (!myCamera)
			myCamera = &viewForGetCamera->m_camera;

        ASSERT_(m_camera.m_distanceZoom>0);


        m_lastProjMat.azimuth = DEG2RAD(myCamera->m_azimuthDeg);
        m_lastProjMat.elev    = DEG2RAD(myCamera->m_elevationDeg);

        m_lastProjMat.eye.x = myCamera->m_pointingX +  max(0.01f,myCamera->m_distanceZoom) * cos(m_lastProjMat.azimuth)*cos(m_lastProjMat.elev);
        m_lastProjMat.eye.y = myCamera->m_pointingY +  max(0.01f,myCamera->m_distanceZoom) * sin(m_lastProjMat.azimuth)*cos(m_lastProjMat.elev);
        m_lastProjMat.eye.z = myCamera->m_pointingZ +  max(0.01f,myCamera->m_distanceZoom) * sin(m_lastProjMat.elev);


        if (fabs(fabs(myCamera->m_elevationDeg)-90)>1e-6)
        {
            m_lastProjMat.up.x=0;
            m_lastProjMat.up.y=0;
            m_lastProjMat.up.z=1;
        }
        else
        {
            float sgn = myCamera->m_elevationDeg>0 ? 1:-1;
            m_lastProjMat.up.x = -cos(DEG2RAD(myCamera->m_azimuthDeg))*sgn;
            m_lastProjMat.up.y = -sin(DEG2RAD(myCamera->m_azimuthDeg))*sgn;
            m_lastProjMat.up.z = 0;
        }

		m_lastProjMat.is_projective = myCamera->m_projectiveModel;
		m_lastProjMat.FOV           = myCamera->m_projectiveFOVdeg;
		m_lastProjMat.pointing.x    = myCamera->m_pointingX;
		m_lastProjMat.pointing.y    = myCamera->m_pointingY;
		m_lastProjMat.pointing.z    = myCamera->m_pointingZ;
		m_lastProjMat.zoom    		= myCamera->m_distanceZoom;

        if (myCamera->m_projectiveModel)
        {
            gluPerspective( myCamera->m_projectiveFOVdeg, vw/double(vh),m_clip_min,m_clip_max);
        }
        else
        {
			const double ratio = vw/double(vh);
			double Ax = myCamera->m_distanceZoom*0.5;
			double Ay = myCamera->m_distanceZoom*0.5;

			if (ratio>1)
				Ax *= ratio;
			else
			{
				if (ratio!=0) Ay /=ratio;
			}

            glOrtho( -Ax,Ax,-Ay,Ay,-0.5*m_clip_max,0.5*m_clip_max);
        }
		// This command is common to ortho and perspective:
		gluLookAt(
			m_lastProjMat.eye.x,
			m_lastProjMat.eye.y,
			m_lastProjMat.eye.z,
			m_lastProjMat.pointing.x,
			m_lastProjMat.pointing.y,
			m_lastProjMat.pointing.z,
			m_lastProjMat.up.x,
			m_lastProjMat.up.y,
			m_lastProjMat.up.z);
		CRenderizable::checkOpenGLError();

		// Render objects:
		// -------------------------------------------
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // GL_LEQUAL

		for (it=objectsToRender->begin();it!=objectsToRender->end();it++)
		{
			if (!it->present()) continue;
			if (!(*it)->isVisible()) continue;

			// 3D coordinates transformation:
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			CRenderizable::checkOpenGLError();

			// This is the right order so that the transformation results in the standard matrix.
			// The order seems to be wrong, but it's not.
			glTranslated((*it)->m_x, (*it)->m_y, (*it)->m_z);
			glRotated((*it)->m_yaw, 0.0, 0.0, 1.0);
			glRotated((*it)->m_pitch, 0.0, 1.0, 0.0);
			glRotated((*it)->m_roll, 1.0, 0.0, 0.0);

			// Do scaling after the other transformations!
			glScalef((*it)->m_scale_x,(*it)->m_scale_y,(*it)->m_scale_z);

			// Set color:
			glColor4f( (*it)->m_color_R,(*it)->m_color_G,(*it)->m_color_B,(*it)->m_color_A);

			(*it)->render();

			if ((*it)->m_show_name)
			{
				glDisable(GL_DEPTH_TEST);
				glRasterPos3f(0.0f,0.0f,0.0f);
				glColor3f(1.0f,1.0f,1.0f);

				GLfloat		raster_pos[4];
				glGetFloatv( GL_CURRENT_RASTER_POSITION, raster_pos);
				float eye_distance= raster_pos[3];

				void *font=NULL;
				if (eye_distance<2)
						font = GLUT_BITMAP_TIMES_ROMAN_24;
				else if(eye_distance<200)
					font = GLUT_BITMAP_TIMES_ROMAN_10;

				if (font)
					CRenderizable::renderTextBitmap( (*it)->m_name.c_str(), font);

				glEnable(GL_DEPTH_TEST);
			}

			glPopAttrib();
			CRenderizable::checkOpenGLError();

			glPopMatrix();
			CRenderizable::checkOpenGLError();

		} // end foreach object


		// Finally, draw the border:
		// --------------------------------
		if (m_borderWidth>0)
		{
		    glLineWidth( 2*m_borderWidth );
		    glColor4f(0,0,0,1);
            glDisable(GL_DEPTH_TEST);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glBegin( GL_LINE_LOOP );
                glVertex2f(-1,-1);
                glVertex2f(-1, 1);
                glVertex2f( 1, 1);
                glVertex2f( 1,-1);
            glEnd();

            glEnable(GL_DEPTH_TEST);
		}

	}
	catch(exception &e)
	{
		string		msg;
		if (it!=m_objects.end())
				msg = format("Exception while rendering a class '%s'\n%s", (*it)->GetRuntimeClass()->className, e.what() );
		else	msg = format("Exception while rendering:\n%s", e.what() );

		THROW_EXCEPTION(msg);
	}
	catch(...)
	{
		THROW_EXCEPTION("Runtime error!");
	}
#else
	THROW_EXCEPTION("The MRPT has been compiled with MRPT_HAS_OPENGL_GLUT=0! OpenGL functions are not implemented");
#endif
}


/*---------------------------------------------------------------
   Implements the writing to a CStream capability of
     CSerializable objects
  ---------------------------------------------------------------*/
void  COpenGLViewport::writeToStream(CStream &out,int *version) const
{
	if (version)
		*version = 1;
	else
	{
		// Save data:
		out << m_camera
			<< m_isCloned << m_isClonedCamera << m_clonedViewport
			<< m_name
			<< m_isTransparent
			<< m_borderWidth
			<< m_view_x << m_view_y << m_view_width << m_view_height;

		// Added in v1:
		out << m_custom_backgb_color << m_background_color.R << m_background_color.G << m_background_color.B << m_background_color.A;

		// Save objects:
		uint32_t	n;
		n = (uint32_t)m_objects.size();
		out << n;
		for (CListOpenGLObjects::const_iterator	it=m_objects.begin();it!=m_objects.end();++it)
			out << **it;
	}
}

/*---------------------------------------------------------------
	Implements the reading from a CStream capability of
		CSerializable objects
  ---------------------------------------------------------------*/
void  COpenGLViewport::readFromStream(CStream &in,int version)
{
	switch(version)
	{
	case 0:
	case 1:
		{
			// Load data:
			in  >> m_camera
				>> m_isCloned >> m_isClonedCamera >> m_clonedViewport
				>> m_name
				>> m_isTransparent
				>> m_borderWidth
				>> m_view_x >> m_view_y >> m_view_width >> m_view_height;

			// in v1:
			if (version>=1)
			{
				in >> m_custom_backgb_color >> m_background_color.R >> m_background_color.G >> m_background_color.B >> m_background_color.A;
			}
			else
			{
				m_custom_backgb_color = false;
			}

			// Load objects:
			uint32_t	n;
			in >> n;
			clear();
			m_objects.resize(n);

			for_each(m_objects.begin(), m_objects.end(), ObjectReadFromStream(&in) );

		} break;
	default:
		MRPT_THROW_UNKNOWN_SERIALIZATION_VERSION(version)

	};
}

/*---------------------------------------------------------------
							getByName
  ---------------------------------------------------------------*/
CRenderizablePtr COpenGLViewport::getByName( const string &str )
{
	for (CListOpenGLObjects::iterator	it=m_objects.begin();it!=m_objects.end();++it)
	{
		if ((*it)->m_name == str)
			return *it;
		else if ( (*it)->GetRuntimeClass() == CLASS_ID_NAMESPACE(CSetOfObjects,opengl))
		{
			CRenderizablePtr ret = getAs<CSetOfObjects>(*it)->getByName(str);
			if (ret.present())
				return ret;
		}
	}
	return CRenderizablePtr();
}

/*---------------------------------------------------------------
					initializeAllTextures
  ---------------------------------------------------------------*/
void  COpenGLViewport::initializeAllTextures()
{
#if MRPT_HAS_OPENGL_GLUT
	for (CListOpenGLObjects::iterator it=m_objects.begin();it!=m_objects.end();++it)
	{
		if ( IS_DERIVED(*it, CTexturedObject ))
			getAs<CTexturedObject>(*it)->loadTextureInOpenGL();
		else if ( IS_CLASS( *it, CSetOfObjects) )
			getAs<CSetOfObjects>(*it)->initializeAllTextures();
	}
#endif
}

/*--------------------------------------------------------------
					dumpListOfObjects
  ---------------------------------------------------------------*/
void COpenGLViewport::dumpListOfObjects( utils::CStringList  &lst )
{
	for (CListOpenGLObjects::iterator	it=m_objects.begin();it!=m_objects.end();++it)
	{
		// Single obj:
		string  s( (*it)->GetRuntimeClass()->className );
		if ((*it)->m_name.size())
			s+= string(" (") +(*it)->m_name + string(")");
		lst.add( s );

		if ((*it)->GetRuntimeClass() == CLASS_ID_NAMESPACE(CSetOfObjects,mrpt::opengl))
		{
			utils::CStringList  auxLst;

			getAs<CSetOfObjects>(*it)->dumpListOfObjects(auxLst);

			for (size_t i=0;i<auxLst.size();i++)
				lst.add( string(" ")+auxLst(i) );
		}
	}
}

/*--------------------------------------------------------------
					removeObject
  ---------------------------------------------------------------*/
void COpenGLViewport::removeObject( const CRenderizablePtr &obj )
{
	for (CListOpenGLObjects::iterator	it=m_objects.begin();it!=m_objects.end();++it)
		if (it->pointer() == obj.pointer())
		{
			m_objects.erase(it);
			return;
		}
		else if ( (*it)->GetRuntimeClass()==CLASS_ID_NAMESPACE(CSetOfObjects,opengl) )
			getAs<CSetOfObjects>(*it)->removeObject(obj);
}

/*--------------------------------------------------------------
					setViewportClipDistances
  ---------------------------------------------------------------*/
void COpenGLViewport::setViewportClipDistances(const double clip_min, const double clip_max)
{
	ASSERT_(clip_max>clip_min);

	m_clip_min = clip_min;
	m_clip_max = clip_max;
}

/*--------------------------------------------------------------
					getViewportClipDistances
  ---------------------------------------------------------------*/
void COpenGLViewport::getViewportClipDistances(double &clip_min, double &clip_max) const
{
	clip_min = m_clip_min;
	clip_max = m_clip_max;
}

/*--------------------------------------------------------------
					get3DRayForPixelCoord
  ---------------------------------------------------------------*/
void COpenGLViewport::get3DRayForPixelCoord(
		const double x_coord,
		const double y_coord,
		mrpt::math::TLine3D &out_ray,
		mrpt::poses::CPose3D *out_cameraPose
		 ) const
{
	ASSERTDEB_(m_lastProjMat.viewport_height>0 && m_lastProjMat.viewport_width>0)

	const double ASPECT = m_lastProjMat.viewport_width / double(m_lastProjMat.viewport_height);

	// unitary vector between (eye) -> (pointing):
	TPoint3D pointing_dir;
	pointing_dir.x = - cos(m_lastProjMat.azimuth)*cos(m_lastProjMat.elev);
	pointing_dir.y = - sin(m_lastProjMat.azimuth)*cos(m_lastProjMat.elev);
	pointing_dir.z = - sin(m_lastProjMat.elev);

	// The camera X vector (in 3D) can be computed from the camera azimuth angle:
	TPoint3D cam_x_3d;
	cam_x_3d.x = -sin(m_lastProjMat.azimuth);
	cam_x_3d.y = cos(m_lastProjMat.azimuth);
	cam_x_3d.z = 0;

	// The camera real UP vector (in 3D) is the cross product:
	//     X3d x pointing_dir:
	TPoint3D cam_up_3d;
	crossProduct3D(cam_x_3d,pointing_dir, cam_up_3d);


	if (!m_lastProjMat.is_projective)
	{
		// Ortho projection:
		// -------------------------------
		double Ax = m_lastProjMat.zoom*0.5;
		double Ay = Ax;

		if (ASPECT>1)
			Ax *= ASPECT;
		else {
			if (ASPECT!=0) Ay /=ASPECT;
		}

		const double point_lx =  (-0.5 + x_coord/m_lastProjMat.viewport_width)*2*Ax;
		const double point_ly = -(-0.5 + y_coord/m_lastProjMat.viewport_height)*2*Ay;

		const TPoint3D ray_origin(
			m_lastProjMat.eye.x  +  point_lx * cam_x_3d.x + point_ly * cam_up_3d.x,
			m_lastProjMat.eye.y  +  point_lx * cam_x_3d.y + point_ly * cam_up_3d.y,
			m_lastProjMat.eye.z  +  point_lx * cam_x_3d.z + point_ly * cam_up_3d.z
			);

		out_ray.pBase = ray_origin;
		out_ray.director[0]=pointing_dir.x;
		out_ray.director[1]=pointing_dir.y;
		out_ray.director[2]=pointing_dir.z;
	}
	else
	{
		// Perspective camera
		// -------------------------------

		// JL: This can derived from: http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml
		//  where one arrives to:
		//    tan(FOVx/2) = ASPECT_RATIO * tan(FOVy/2)
		//
		const double FOVy = DEG2RAD(m_lastProjMat.FOV);
		const double FOVx = 2.0*atan(ASPECT*tan(FOVy*0.5));

		const double ang_horz = (-0.5 + x_coord/m_lastProjMat.viewport_width)* FOVx;
		const double ang_vert = -(-0.5 + y_coord/m_lastProjMat.viewport_height)* FOVy;

		const TPoint3D l(tan(ang_horz),tan(ang_vert),1.0); // Point in camera local reference frame

		const TPoint3D ray_director(
			l.x * cam_x_3d.x + l.y * cam_up_3d.x + l.z * pointing_dir.x,
			l.x * cam_x_3d.y + l.y * cam_up_3d.y + l.z * pointing_dir.y,
			l.x * cam_x_3d.z + l.y * cam_up_3d.z + l.z * pointing_dir.z
			);

		// Set out ray:
		out_ray.pBase = m_lastProjMat.eye;
		out_ray.director[0]=ray_director.x;
		out_ray.director[1]=ray_director.y;
		out_ray.director[2]=ray_director.z;

	} // end projective

	// Camera pose:
	if (out_cameraPose)
	{
		mrpt::math::CMatrixDouble44  M(UNINITIALIZED_MATRIX);
		M.get_unsafe(0,0) = cam_x_3d.x;
		M.get_unsafe(1,0) = cam_x_3d.y;
		M.get_unsafe(2,0) = cam_x_3d.z;
		M.get_unsafe(3,0) = 0;

		M.get_unsafe(0,1) = cam_up_3d.x;
		M.get_unsafe(1,1) = cam_up_3d.y;
		M.get_unsafe(2,1) = cam_up_3d.z;
		M.get_unsafe(3,1) = 0;

		M.get_unsafe(0,2) = pointing_dir.x;
		M.get_unsafe(1,2) = pointing_dir.y;
		M.get_unsafe(2,2) = pointing_dir.z;
		M.get_unsafe(3,2) = 0;

		M.get_unsafe(0,3) = m_lastProjMat.eye.x;
		M.get_unsafe(1,3) = m_lastProjMat.eye.y;
		M.get_unsafe(2,3) = m_lastProjMat.eye.z;
		M.get_unsafe(3,3) = 1;

		*out_cameraPose = CPose3D(M);
	}
}

void COpenGLViewport::getCurrentCameraPose( mrpt::poses::CPose3D &out_cameraPose ) const
{
	mrpt::math::TLine3D dum;
	get3DRayForPixelCoord(0,0,dum, &out_cameraPose);
}