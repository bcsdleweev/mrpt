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
#ifndef  CObservable_H
#define  CObservable_H

#include <mrpt/utils/utils_defs.h>
#include <mrpt/utils/mrptEvent.h>

namespace mrpt
{
	namespace utils
	{
		class CObserver;

		/** Inherit from this class for those objects capable of being observed by a CObserver class.
		  *
		  *  The only thing to do in your child class is to call CObservable::publishEvent() whenever needed and all the 
		  *   observer classes will be notified.
		  *
		  * \note The pairs CObservable / CObserver automatically notify each other the destruction of any of them, effectively ending the subscription of events.
		  *
		  *  \sa CObserver, mrptEvent
		  */
		class BASE_IMPEXP CObservable
		{
			friend class CObserver;

		public:
			CObservable();
			virtual ~CObservable();

		private:
			std::set<CObserver*>  m_subscribers; 
			void internal_observer_begin(CObserver*);
			void internal_observer_end(CObserver*);

		protected:
			/** Called when you want this object to emit an event to all the observers currently subscribed to this object.
			  */
			void publishEvent(const mrptEvent &e) const;

		}; // End of class def.

	} // End of namespace
} // end of namespace
#endif