########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
PROJECT(STEELBREEZE CXX C)

SET(CXXFLAGS "-D_GNU_SOURCE -pipe -g -Wall -W -Wpointer-arith -Wcast-align -Wconversion -Wredundant-decls -Wunreachable-code -DDEBUG")


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)

SET(STEELBREEZE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeneral.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoAtmLoad.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeo.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoDelay.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoEop.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoEphem.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoEstimator.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoNROValues.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoNutValues.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoObs.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoObsVLBI.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoObsVLBI_IO.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoObsVLBIStat.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoParameter.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoProject.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoProjectReports.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoRefFrame.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoRefraction.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoSINEX.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoSourceMap.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoSources.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoStations.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoTide.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbGeoTime.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIFilters.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIMainWin.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIMEM.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbInterface.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIObsVLBI.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIObsVLBIStat.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIPlot.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIPreProcessing.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIProject.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbISetup.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbISolution.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIStuff.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIStuffSource.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbIStuffStation.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbITest.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbMathDataSeries.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbMathFilters.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbMathMEM.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbMathSpline.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbSetup.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbVector3.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SbVector.C
			${CMAKE_CURRENT_SOURCE_DIR}/src/SteelBreeze.C
)

if(UNIX)
  set(CMAKE_REQUIRED_LIBRARIES m)
  set(HAVE_LIBM 1)
  set(_GNU_SOURCE 1)
endif()

SET(PACKAGE "SteelBreeze")
SET(PACKAGE_BUGREPORT "mikhailo.lytvyn@gmail.com")
SET(PACKAGE_TARNAME "steelbreeze")
SET(PACKAGE_VERSION "2.0.2")
SET(VERSION ${PACKAGE_VERSION})

INCLUDE(config.cmake)


configure_file ("${PROJECT_SOURCE_DIR}/config_cmake.h.in"
                "${PROJECT_BINARY_DIR}/config_cmake.h" )
