INCLUDE(../../cmakemodules/AssureCMakeRootFile.cmake) # Avoid user mistake in CMake source directory

#-----------------------------------------------------------------
# CMake file for the MRPT application:  prrt-navigator-demo
#
#  Run with "cmake ." at the root directory
#
#  October 2007, Jose Luis Blanco <jlblanco@ctima.uma.es>
#-----------------------------------------------------------------

PROJECT(prrt_navigator_demo)

IF(wxWidgets_FOUND)

	# ---------------------------------------------
	# TARGET:
	# ---------------------------------------------
	SET(SRCS
	prrtnavdemoApp.cpp prrtnavdemoApp.h
	prrtnavdemoMain.cpp prrtnavdemoMain.h
	 CIniEditor.cpp  CIniEditor.h
	 ../ReactiveNavigationDemo/DEFAULT_GRIDMAP_DATA.h
	 ../wx-common/CMyRedirector.h
	 CAboutBox.cpp  CAboutBox.h
	 )

	IF(WIN32)
		SET(SRCS ${SRCS} resource.rc)
	ENDIF(WIN32)

	# Define the executable target:
	ADD_EXECUTABLE(prrt-navigator-demo WIN32 ${SRCS})

	SET(TMP_TARGET_NAME "prrt-navigator-demo")




	# Add the required libraries for linking:
	TARGET_LINK_LIBRARIES(${TMP_TARGET_NAME} ${wxWidgets_LIBRARIES} )
	TARGET_LINK_LIBRARIES(${TMP_TARGET_NAME} ${MRPT_LINKER_LIBS})

	TARGET_LINK_LIBRARIES(${TMP_TARGET_NAME} mrpt-reactivenav)

	# Dependencies on MRPT libraries:
	#  Just mention the top-level dependency, the rest will be detected automatically,
	#  and all the needed #include<> dirs added (see the script DeclareAppDependencies.cmake for further details)
	DeclareAppDependencies(${TMP_TARGET_NAME} mrpt-reactivenav mrpt-gui)

	DeclareAppForInstall(${TMP_TARGET_NAME})

	AppStartMenuLink(${TMP_TARGET_NAME} "PRRT robot navigator demo")   # Add link to the Start menu (in Windows Installable packages)

ELSE(wxWidgets_FOUND)
	# ----------------------------------------------------------------------------------
    # For convenience. When we cannot continue, inform the user
	# ----------------------------------------------------------------------------------
    MESSAGE(STATUS "wxWidgets not found!: **This target will not be included in the Makefiles**")
ENDIF(wxWidgets_FOUND)


