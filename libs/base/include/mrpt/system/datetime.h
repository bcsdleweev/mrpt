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
#ifndef  MRPT_SYSTEM_DATETIME_H
#define  MRPT_SYSTEM_DATETIME_H

#include <mrpt/utils/utils_defs.h>

/** Represents an invalid timestamp, where applicable. */
#define INVALID_TIMESTAMP (0)

namespace mrpt
{
	namespace system
	{
		/** @name Time and date functions
		@{ */

		/** A system independent time type, it holds the the number of 100-nanosecond intervals since January 1, 1601 (UTC).
		 * \sa system::getCurrentTime, system::timeDifference, INVALID_TIMESTAMP, TTimeParts
		 */
		typedef uint64_t TTimeStamp;

		/** The parts of a date/time (it's like the standard 'tm' but with fractions of seconds).
		 * \sa TTimeStamp, timestampToParts, buildTimestampFromParts
		 */
		struct TTimeParts
		{
			uint16_t	year;	/** The year */
			uint8_t		month;  /** Month (1-12) */
			uint8_t		day;    /** Day (1-31) */
			uint8_t		hour;   /** Hour (0-23) */
			uint8_t		minute; /** Minute (0-59) */
			double		second; /** Seconds (0.0000-59.9999) */
			uint8_t		day_of_week; /** Day of week (1:Sunday, 7:Saturday) */
			int			daylight_saving;
		};

		/** Builds a timestamp from the parts (Parts are in UTC)
		  * \sa timestampToParts
		  */
		mrpt::system::TTimeStamp BASE_IMPEXP buildTimestampFromParts( const mrpt::system::TTimeParts &p );

		/** Builds a timestamp from the parts (Parts are in local time)
		  * \sa timestampToParts, buildTimestampFromParts
		  */
		mrpt::system::TTimeStamp BASE_IMPEXP buildTimestampFromPartsLocalTime( const mrpt::system::TTimeParts &p );

		/** Gets the individual parts of a date/time (days, hours, minutes, seconds) - UTC time or local time
		  * \sa buildTimestampFromParts
		  */
		void BASE_IMPEXP timestampToParts( TTimeStamp t, TTimeParts &p, bool localTime = false );

		/** Returns the current (UTC) system time.
		  * \sa now,getCurrentLocalTime
		  */
		mrpt::system::TTimeStamp  BASE_IMPEXP getCurrentTime( );

		/** A shortcut for system::getCurrentTime
		  * \sa getCurrentTime, getCurrentLocalTime
		 */
		inline mrpt::system::TTimeStamp now() {
			return getCurrentTime();
		}

		/** Returns the current (local) time.
		  * \sa now,getCurrentTime
		  */
		mrpt::system::TTimeStamp  BASE_IMPEXP getCurrentLocalTime( );

		/** Transform from standard "time_t" (actually a double number, it can contain fractions of seconds) to TTimeStamp.
		  * \sa timestampTotime_t
		  */
		mrpt::system::TTimeStamp  BASE_IMPEXP time_tToTimestamp( const double &t );

		/** Transform from standard "time_t" to TTimeStamp.
		  * \sa timestampTotime_t
		  */
		mrpt::system::TTimeStamp  BASE_IMPEXP time_tToTimestamp( const time_t &t );

		/** Transform from TTimeStamp to standard "time_t" (actually a double number, it can contain fractions of seconds).
		  * \sa time_tToTimestamp, secondsToTimestamp
		  */
		double BASE_IMPEXP timestampTotime_t( const mrpt::system::TTimeStamp  &t );

		/** Retuns the time difference from t1 to t2 (positive if t2 is posterior to t1), in seconds.
		  * \sa secondsToTimestamp
          */
		double BASE_IMPEXP timeDifference( const mrpt::system::TTimeStamp &t_first, const mrpt::system::TTimeStamp &t_later );

		/** Transform a time interval (in seconds) into TTimeStamp (e.g. which can be added to an existing valid timestamp)
		  * \sa timeDifference
		  */
		mrpt::system::TTimeStamp BASE_IMPEXP secondsToTimestamp( const double &nSeconds );

		/** Returns a formated string with the given time difference (passed as the number of seconds), as a string [H]H:MM:SS.MILISECS
		  * \sa unitsFormat
		  */
		std::string BASE_IMPEXP formatTimeInterval( const double &timeSeconds );

		/** Convert a timestamp into this textual form (UTC time): YEAR/MONTH/DAY,HH:MM:SS.MMM
		  * \sa dateTimeLocalToString
		  */
		std::string  BASE_IMPEXP dateTimeToString(const mrpt::system::TTimeStamp &t);

		/** Convert a timestamp into this textual form (in local time): YEAR/MONTH/DAY,HH:MM:SS.MMM
		  * \sa dateTimeToString
		  */
		std::string  BASE_IMPEXP dateTimeLocalToString(const mrpt::system::TTimeStamp &t);

		/** Convert a timestamp into this textual form: YEAR/MONTH/DAY
		 */
		std::string  BASE_IMPEXP dateToString(const mrpt::system::TTimeStamp &t);

		/** Returns the number of seconds ellapsed from midnight in the given timestamp
		 */
		double  BASE_IMPEXP extractDayTimeFromTimestamp(const mrpt::system::TTimeStamp &t);

		/** Convert a timestamp into this textual form (UTC): HH:MM:SS.MMMMMM
		 */
		std::string  BASE_IMPEXP timeToString(const mrpt::system::TTimeStamp &t);

		/** Convert a timestamp into this textual form (in local time): HH:MM:SS.MMMMMM
		 */
		std::string  BASE_IMPEXP timeLocalToString(const mrpt::system::TTimeStamp &t, unsigned int secondFractionDigits=6);

		/** This function implements time interval formatting: Given a time in seconds, it will return a string describing the interval with the most appropriate unit.
		 * E.g.: 1.23 year, 3.50 days, 9.3 hours, 5.3 minutes, 3.34 sec, 178.1 ms,  87.1 us.
		 * \sa unitsFormat
		 */
		std::string BASE_IMPEXP intervalFormat(const double seconds);

		/** @} */

	} // End of namespace

} // End of namespace

#endif