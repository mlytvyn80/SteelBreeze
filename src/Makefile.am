
#################################################################################
# 										#
# This file is part of SteelBreeze.						#
# 										#
# SteelBreeze, the geodetic VLBI data analysing software.			#
# Copyright (C) 1998--2003 Sergei Bolotin, MAO NASU, Kiev, Ukraine.		#
# 										#
# SteelBreeze is free software; you can redistribute it and/or modify		#
# it under the terms of the GNU General Public License as published by		#
# the Free Software Foundation; either version 2 of the License, or		#
# (at your option) any later version.						#
# 										#
# SteelBreeze is distributed in the hope that it will be useful,		#
# but WITHOUT ANY WARRANTY; without even the implied warranty of		#
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			#
# GNU General Public License for more details.					#
# 										#
# You should have received a copy of the GNU General Public License		#
# along with SteelBreeze; if not, write to the Free Software Foundation,	#
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		#
#										#
#										#
#################################################################################

EXTRA_DIST		= xpm/*.xpm
CLEANFILES		= *.bak *~ *% #* $(SB_Ifc_m)
SUFFIXES 		= moc.C

bin_PROGRAMS		= SteelBreeze
noinst_PROGRAMS 	= ctinfo
ctinfo_SOURCES		= ctinfo.c


MOC			= @MOC@
INCLUDES 		= @QT_INCLUDE@
SteelBreeze_LDADD 	= $(SB_Ifc_mo) @QT_LIB@
SbSrcDir		= @srcdir@
SbBackupDir		= @top_srcdir@/backup

########### headers:
SB_Gen_h   = SbGeneral.H SbSetup.H SbVector3.H SbVector.H
SB_Gen_cuh = SteelBreeze.C SbCompileInfo.C 
SB_Math_h  = SbMathDataSeries.H SbMathFilters.H SbMathMEM.H SbMathSpline.H
SB_Math_cuh= 
SB_Geo_h   = SbGeo.H SbGeoAtmLoad.H SbGeoDelay.H SbGeoEop.H SbGeoEphem.H \
	     SbGeoEstimator.H SbGeoObs.H SbGeoObsVLBI.H SbGeoObsVLBI_IO.H SbGeoObsVLBIStat.H \
	     SbGeoParameter.H SbGeoProject.H SbGeoRefFrame.H SbGeoRefraction.H \
	     SbGeoSINEX.H SbGeoSources.H SbGeoSourceMap.H SbGeoStations.H SbGeoTide.H SbGeoTime.H
SB_Geo_cuh = SbGeoNutValues.C SbGeoNROValues.C SbGeoProjectReports.C 
SB_Ifc_hum = 
SB_Ifc_hm  = SbInterface.H SbIMainWin.H SbIFilters.H SbIMEM.H SbIObsVLBI.H SbIObsVLBIStat.H \
	     SbIPlot.H SbIPreProcessing.H SbIProject.H SbISetup.H SbISolution.H SbIStuff.H \
	     SbIStuffSource.H SbIStuffStation.H SbITest.H

########### sources, mocs (implicitly):
SB_Math_c = $(SB_Math_h:.H=.C) $(SB_Math_cuh)
SB_Gen_c  = $(SB_Gen_h:.H=.C) $(SB_Gen_cuh)
SB_Geo_c  = $(SB_Geo_h:.H=.C) $(SB_Geo_cuh)
SB_Ifc_h  = $(SB_Ifc_hm) $(SB_Ifc_hum)
SB_Ifc_m  = $(SB_Ifc_hm:.H=.moc.C)
SB_Ifc_mo = $(SB_Ifc_m:.C=.o)
SB_Ifc_c  = $(SB_Ifc_hm:.H=.C)

noinst_HEADERS	= $(SB_Gen_h) $(SB_Math_h) $(SB_Geo_h) $(SB_Ifc_h)
#BUILT_SOURCES	= SbCompileInfo.H $(SB_Ifc_m)

SteelBreeze_SOURCES = $(SB_Gen_c) $(SB_Math_c) $(SB_Geo_c) $(SB_Ifc_c) $(noinst_HEADERS)
SteelBreeze_DEPENDENCIES = SbCompileInfo.H $(SB_Ifc_m) $(SB_Ifc_mo)
SbCompileInfo.C:SbCompileInfo.H
%.moc.C:%.H; $(MOC) $< -o $@
all-local:;@rm -f SbCompileInfo.H SbCompileInfo.o; size SteelBreeze
SbCompileInfo.H:ctinfo; @./ctinfo @SB_VERSION_MAJOR@ @SB_VERSION_MINOR@ @SB_VERSION_PATCHLEVEL@ >$@

showfiles:;@echo $(SteelBreeze_SOURCES)
showgeo:;@echo $(SB_Geo_h) $(SB_Geo_c)
showif:;@echo $(SB_Ifc_h) $(SB_Ifc_c)
calc:;@find . -not -name '*.o' -and -not -name '*.moc.C' -and -not -name '*.P' \
-and -not -name '*.Po' -and -not -name '*~' -and -not -name '#*#' -and -not -name '*%' \
-and -not -name "SteelBreeze" -not -name "ctinfo" -and -not -name "Makefile" \
-and -not -name "Makefile.in" -and -not -type d -print | sort -r | xargs wc -lc

