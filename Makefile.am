
#################################################################################
#                                                                               #
# This file is part of SteelBreeze.                                             #
#                                                                               #
# SteelBreeze, the geodetic VLBI data analysing software.                       #
# Copyright (C) 1998--2002 Sergei Bolotin, MAO NASU, Kiev, Ukraine.             #
#                                                                               #
# SteelBreeze is free software; you can redistribute it and/or modify           #
# it under the terms of the GNU General Public License as published by          #
# the Free Software Foundation; either version 2 of the License, or             #
# (at your option) any later version.                                           #
#                                                                               #
# SteelBreeze is distributed in the hope that it will be useful,                #
# but WITHOUT ANY WARRANTY; without even the implied warranty of                #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 #
# GNU General Public License for more details.                                  #
#                                                                               #
# You should have received a copy of the GNU General Public License             #
# along with SteelBreeze; if not, write to the Free Software Foundation,        #
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                #
#                                                                               #
#                                                                               #
#################################################################################


EXTRA_DIST = prepare

SUBDIRS = src doc info

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-`date +"%Y%m%d"`

html:
	cd doc && $(MAKE) html

rehtml:
	cd doc && $(MAKE) clean && $(MAKE) html

