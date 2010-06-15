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

#include <mrpt/hwdrivers/CBoardENoses.h>
#include <mrpt/utils.h>

using namespace mrpt;
using namespace mrpt::hwdrivers;
using namespace mrpt::system;
using namespace mrpt::slam;
using namespace mrpt::utils;
using namespace std;

int main()
{
	try
	{
		CBoardENoses			eNoses;
		std::string				firmVers;
		CObservationGasSensors	obs;
		FILE					*f_log = os::fopen("./log.txt","wt");
		TTimeStamp				timStart = mrpt::system::getCurrentTime();
		
		/*
		// Load configuration:
		ASSERT_( mrpt::system::fileExists("_CONFIG_eNoses.ini") );
		CConfigFile conf("_CONFIG_eNoses.ini");
		eNoses.loadConfig( conf, "eNoses" );

		
		if (!eNoses.queryFirmwareVersion( firmVers ) )
		{
			printf("Error!!\n");
			return -1;
		}
		else
			std::cout << "FIRMWARE VERSION: " << firmVers << std::endl;
		*/

		while ( !mrpt::system::os::kbhit() )
		{
			if (! eNoses.getObservation( obs ) )
			{
				cout << "- Could not retrieve an observation from the eNoses..." << endl;
			}
			else
			{
				cout << obs.m_readings.size() << " eNoses:" << endl;

				if (f_log) fprintf(f_log,"%f ", mrpt::system::timeDifference(timStart,obs.timestamp) );

				for (size_t i=0;i<obs.m_readings.size();i++)
				{
					//E-Nose Pose
					printf("#%u (%.02f,%.02f,%.02f): ",(unsigned int)i,obs.m_readings[i].eNosePoseOnTheRobot.x,obs.m_readings[i].eNosePoseOnTheRobot.y,obs.m_readings[i].eNosePoseOnTheRobot.z);
					
					//E-Nose Sensor's Data
					for (size_t j=0;j<obs.m_readings[i].sensorTypes.size();j++)
					{
						//printf("%04X: %.03fV ", obs.m_readings[i].sensorTypes[j], obs.m_readings[i].readingsVoltage[j] );

						if ( j<(obs.m_readings[i].sensorTypes.size() -1) ){
							if( (obs.m_readings[i].sensorTypes[j]==obs.m_readings[i].sensorTypes[j+1]) ){
								printf("\n%04X: %.03fV \n ", obs.m_readings[i].sensorTypes[j], obs.m_readings[i].readingsVoltage[j]-obs.m_readings[i].readingsVoltage[j+1] );
								j++;	//skip the next sensor as it has been used already.
							}else{
								printf("\n%04X: %.03fV \n ", obs.m_readings[i].sensorTypes[j], obs.m_readings[i].readingsVoltage[j] );
							}
						}else{
								printf("\n%04X: %.03fV \n ", obs.m_readings[i].sensorTypes[j], obs.m_readings[i].readingsVoltage[j] );
						}

						if (f_log) fprintf(f_log,"%f ",obs.m_readings[i].readingsVoltage[j]);
					}

					printf("\nTemp: ");
					if (obs.m_readings[i].hasTemperature)
							printf("%.04f C",obs.m_readings[i].temperature);
					else	printf("NO");

					printf("\n");
					printf("-----------------------------------------\n");
				}
				if (f_log) fprintf(f_log,"\n");
			}

			mrpt::system::sleep(500);
		}

		if (f_log) os::fclose(f_log);
	}
	catch(std::exception &e)
	{
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}