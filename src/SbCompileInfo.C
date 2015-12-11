/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 1998-2002 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
 *
 * SteelBreeze is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SteelBreeze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SteelBreeze; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "SbGeneral.H"
#include "SbCompileInfo.H"


#ifdef SB_COMP_NAME
    const QString SBCompilerName		(SB_COMP_NAME);
#else
    const QString SBCompilerName		("a c++ compiler");
#endif


#ifdef SB_COMP_VERSION
    const QString SBCompilerVersion		(SB_COMP_VERSION);
#else
    const QString SBCompilerVersion		("unknown");
#endif


#ifdef SB_COMP_HOST_NAME
    const QString SBCompilerHostName		(SB_COMP_HOST_NAME);
#else
    const QString SBCompilerHostName		("localhost");
#endif


#ifdef SB_COMP_DOMAIN_NAME
    const QString SBCompilerDomainName		(SB_COMP_DOMAIN_NAME);
#else
    const QString SBCompilerDomainName		("localdomain");
#endif


#ifdef SB_COMP_LOGIN_NAME
    const QString SBCompilerLoginName		(SB_COMP_LOGIN_NAME);
#else
    const QString SBCompilerLoginName		("unknown");
#endif


#ifdef SB_COMP_USER_NAME
    const QString SBCompilerUserName		(SB_COMP_USER_NAME);
#else
    const QString SBCompilerUserName		("some user");
#endif


#ifdef SB_COMP_DATE
    const QString SBCompilerDate		(SB_COMP_DATE);
#else
    const QString SBCompilerDate		("somewhere in time");
#endif


#ifdef SB_COMP_TIME
    const QString SBCompilerTime		(SB_COMP_TIME);
#else
    const QString SBCompilerTime		("");
#endif


#ifdef SB_COMP_OS
    const QString SBCompilerOS			(SB_COMP_OS);
#else
    const QString SBCompilerOS			("Some strange OS on some machine");
#endif


const SBVersion Version(SB_VER_MAJOR, SB_VER_MINOR, SB_VER_PATCHLEVEL);

QString SBVersion::SelfName("SteelBreeze");

