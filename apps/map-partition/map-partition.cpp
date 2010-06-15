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

/*---------------------------------------------------------------
    APPLICATION: Map partitioning based on spectral graph
    FILE: map-partition.cpp
    AUTHOR: Jose Luis Blanco Claraco <jlblanco@ctima.uma.es>

	See README.txt for instructions.
  ---------------------------------------------------------------*/

#include <mrpt/slam.h>

using namespace mrpt;
using namespace mrpt::slam;
using namespace mrpt::opengl;
using namespace mrpt::math;
using namespace mrpt::utils;
using namespace mrpt::system;
using namespace std;

// The ".simplemap" file to process.
string MAP_FILE;

// The command-line passed threshold
float  THRESHOLD_NCUT = -1;


// ------------------------------------------------------
//		Test of maps partitioning
//   (Method in the ICRA06 paper)
// ------------------------------------------------------
void Test()
{
	CSimpleMap		in_map, out_map;
	CTicTac						tictac;
	CIncrementalMapPartitioner	imp;
	size_t						i,n;
	std::vector<vector_uint>	parts;

	deleteFilesInDirectory("./MAP-PARTITION_RESULTS");
	createDirectory("./MAP-PARTITION_RESULTS");
	printf("Output files will be saved in directory ./MAP-PARTITION_RESULTS\n");

	// Options:
	imp.options.partitionThreshold			 = 0.90f;
	imp.options.gridResolution				 = 0.10f;
	imp.options.minDistForCorrespondence	 = 0.20f;
	imp.options.minMahaDistForCorrespondence = 10.00f;
	imp.options.useMapMatching               = true;
	imp.options.debugSaveAllEigenvectors	 = false;

	CGraphPartitioner::VERBOSE = false;

	if (fileExists("./MAP-PARTITION_CONFIG.ini"))
	{
	    cout << "Loading params from 'MAP-PARTITION_CONFIG.ini'" << endl;
        CConfigFile  cfg("./MAP-PARTITION_CONFIG.ini");
        imp.options.loadFromConfigFile( cfg, "PARAMS" );
	}
	else
	{
	    cout << "Warning: 'MAP-PARTITION_CONFIG.ini' not found. Using default parameters" << endl;
	}

	if (THRESHOLD_NCUT>=0)
		imp.options.partitionThreshold = THRESHOLD_NCUT;

	imp.options.dumpToConsole();

	// Load map from the input file:
	printf("Loading input map:\n%s\n...",MAP_FILE.c_str());
	CFileGZInputStream(MAP_FILE) >> in_map;
	printf("Ok\n");

	// Execute the method:
	printf("\nExecuting the method...\n");

	tictac.Tic();

	n = in_map.size();
	for (i=0;i<n;i++)
	{
		CSensoryFramePtr sf;
		CPose3DPDFPtr posePDF;

		in_map.get(i,posePDF, sf);

		imp.addMapFrame( sf, posePDF );

		printf("[%u/%u]...",(unsigned int)i,(unsigned int)n);

//		if ((i%1)==0)
		if (i==n-1)
		{
			imp.updatePartitions( parts );
			printf("Map nodes:%u --- %u partitions:\n", (unsigned int)i+1,(unsigned int)parts.size() );
			for (size_t j=0;j<parts.size();j++)
			{
				printf("  Part#%u=", (unsigned int)j);
				for (size_t k=0;k<parts[j].size();k++)
					printf(" %u",parts[j][k]);
				printf("\n");
			}
		}

		//printf("\n");
	}

	printf("Done! in %.03fms\n", (float)(1000*tictac.Tac()) );

	// Save in different maps:
	// ------------------------------------
	FILE	*f=os::fopen( format("MAP-PARTITION_RESULTS/out_partitions_%s.txt", extractFileName(MAP_FILE).c_str() ).c_str() ,"wt");
	for (i=0;i<parts.size();i++)
	{
		for (size_t j=0;j<parts[i].size();j++)
			fprintf(f,"%u ",parts[i][j]);

		fprintf(f,"\n");
	}
	fclose(f);

	printf("Saving output maps...");

	for (i=0;i<parts.size();i++)
	{
		out_map.clear();
		for (size_t j=0;j<parts[i].size();j++)
		{
			CSensoryFramePtr	sf;
			CPose3DPDFPtr		posePDF;

			in_map.get(parts[i][j],posePDF, sf);

			out_map.insert(posePDF, sf);
		}

		CFileOutputStream( format("MAP-PARTITION_RESULTS/out_part#%03u.simplemap",(unsigned)i) ) << out_map;
	}

	printf("Ok\n");

	CMatrix		A;
	imp.getAdjacencyMatrix( A);
	A.saveToTextFile( "MAP-PARTITION_RESULTS/matrix_A.txt", MATRIX_FORMAT_FIXED);


	// ------------------------------------------------------------------
	//  Compute the rearranged matrix:
	// ------------------------------------------------------------------

	std::sort( parts.begin(), parts.end() );

	CMatrix			B( A.getRowCount(), A.getColCount() );
	vector_uint		rearrIndexes;
	vector_uint		separations;
	for (i=0;i<parts.size();i++)
	{
		uint32_t  maxIdx = 0;
		for (size_t j=0;j<parts[i].size();j++)
		{
			maxIdx = max(maxIdx, parts[i][j]);
			rearrIndexes.push_back( parts[i][j] );
		}
		separations.push_back( (unsigned int)rearrIndexes.size() );
	}

	for (size_t col=0;col<rearrIndexes.size();col++)
		for (size_t row=0;row<rearrIndexes.size();row++)
			B( row,col ) = A(rearrIndexes[row],rearrIndexes[col] );


	B.saveToTextFile( "MAP-PARTITION_RESULTS/matrix_B.txt" , MATRIX_FORMAT_FIXED);
	{
		gui::CDisplayWindow		win("Adjacency matrix");
		gui::CDisplayWindow		win2(" Rearranged adjacency matrix");
		CImageFloat		imgFl( A );
		CImageFloat		imgFl2( B );
		imgFl.saveToFile("MAP-PARTITION_RESULTS/ADJ_MATRIX_BEFORE.bmp");
		imgFl2.saveToFile("MAP-PARTITION_RESULTS/ADJ_MATRIX_AFTER.bmp");
		win.showImage( imgFl );
		win2.showImage( imgFl2 );
		win.setPos(20,20);
		win2.setPos((int)(40+A.getColCount()),20);
		cout << "Press any key to continue..." << endl;
		win2.waitForKey();
	}

	// ------------------------------------------------------------------
	//			Generate MATLAB script
	// ------------------------------------------------------------------
	printf("Generating MATLAB script for visualizing results...");
	f=os::fopen( "MAP-PARTITION_RESULTS/seePartitionResults.m" ,"wt");
	fprintf(f,"clear all; close all; \n\nR=0.10;STEP=4;\n");

	fprintf(f,"figure(1); hold on;\n");
	fprintf(f,"\nim2=imread('../GRID.BMP');M = size(im2,1); MM = size(im2,2);\n");
	fprintf(f,"for i=1:M,im(i,:)=im2(M-i+1,:);end;\n");
	fprintf(f,"D=load('../GRID_LIMITS.TXT');\n");
	fprintf(f,"imshow(linspace(D(1),D(2),MM),linspace(D(3),D(4),M),im);\n");
	fprintf(f,"set(gca,'YDir','normal');\n\n");
	fprintf(f,"set(gca,'Position',[0 0 1 1]);\n");


	for (i=0;i<parts.size();i++)
	{
		fprintf(f,"%% Partition #%03i\n", (unsigned int)i);
		fprintf(f,"poses=[...\n");

		CPose2D		meanPose;
		for (size_t j=0;j<parts[i].size();j++)
		{
			CSensoryFramePtr	sf;
			CPose3DPDFPtr		posePDF;

			// Get the pose:
			in_map.get(parts[i][j],posePDF, sf);

			meanPose = CPose2D( posePDF->getMeanVal());

			fprintf(f,"       %.03f,%.03f,%.03f",meanPose.x(), meanPose.y(), meanPose.phi() );

			if (j==(parts[i].size()-1))
					fprintf(f,"];\n");
			else	fprintf(f,";...\n");
		}
		fprintf(f,"\n");
		fprintf(f,"figure(1); hold on;\n");
		char	color='k';
		switch (i % 4)
		{
		case 0: color = 'k'; break;
		case 1: color = 'r'; break;
		case 2: color = 'b'; break;
		case 3: color = 'g'; break;
		}
		fprintf(f,"drawRobot(poses(1:STEP:end,:),R,'%c');\n", color);
	}
	fprintf(f,"axis equal;axis([-5 32 -2 15]);\n");

	fprintf(f,"A=load('matrix_a.txt'); B=load('matrix_b.txt');\n");
	fprintf(f,"\n figure(2); subplot(121); imagesc(A); axis equal;colormap(gray);axis([1 length(A) 1 length(A)]);\n");
	fprintf(f,"\n subplot(122);  imagesc(B); axis equal;hold on;colormap(gray);axis([1 length(A) 1 length(A)]);\n");

	// Draw separation lines:
	size_t	N = rearrIndexes.size();
	for (i=0;i<(separations.size()-1);i++)
	{
		fprintf(f,"plot([%i %i],[%i %i],'k');\n", 0, (unsigned int)N-1, separations[i],separations[i] );
		fprintf(f,"plot([%i %i],[%i %i],'k');\n", separations[i],separations[i], 0, (unsigned int)N-1 );
	}

	fclose(f);
	printf("Ok\n");
}


// ------------------------------------------------------
//						MAIN
// ------------------------------------------------------
int main(int argc, char **argv)
{
	try
	{
		printf(" map-partition version 0.2 - Part of the MRPT\n");
		printf(" MRPT C++ Library: %s - BUILD DATE %s\n", MRPT_getVersion().c_str(), MRPT_getCompilationDate().c_str());
		printf("-------------------------------------------------------------------\n");

		// Process arguments:
		if (argc<2)
		{
			printf("Use: map-partition <file.simplemap> [nCut threshold 0-2]\n");
			mrpt::system::pause();
			return -1;
		}

		MAP_FILE = std::string( argv[1] );

		if (argc==3)
		{
			bool err = false;

			if (!sscanf(argv[2],"%f",&THRESHOLD_NCUT) )
				err = true;
			else
				if (THRESHOLD_NCUT<0 || THRESHOLD_NCUT>2)
					err = true;

			if (err)
			{
				printf("Use: map-partition <file.simplemap> [nCut threshold 0-2]\n");
				printf("\n\n Invalid nCut threshold: '%s'\n",argv[2]);
				mrpt::system::pause();
				return -1;
			}
		}


		Test();

		return 0;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl << "Program finished for an exception!!" << std::endl;
		mrpt::system::pause();
		return -1;
	}
	catch (...)
	{
		std::cerr << "Untyped exception!!" << std::endl;
		mrpt::system::pause();
		return -1;
	}
}
