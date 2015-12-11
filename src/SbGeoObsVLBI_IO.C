/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 1998-2003 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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

#include <math.h>
#include <stdio.h> 

#include <qmessagebox.h>
#include <qregexp.h> 
#include <qdir.h> 
#include <qfile.h> 

#include "SbGeoObsVLBI_IO.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"
#include "SbSetup.H"






/*==============================================================================================*/
/*												*/
/* class SBVLBISession's friends implementation							*/
/* (continue)											*/
/*==============================================================================================*/
SBTS_ngs &operator<<(SBTS_ngs& s, const SBVLBISession&)
{
  return s;
};

/** \relates SBVLBISession
 * Makes data import from a file in NGS format.
 * \param s input stream, NGS format;
 * \param Session imported session.
 *
 * \par NGS Format desription.
 * The NGS data parsering has been implemented according to the following e-mail
 * (thanx to Konstantin Nurutdinov and Michael D. Abell):
 * \verbinclude ngs.fmt
 */
SBTS_ngs &operator>>(SBTS_ngs& s, SBVLBISession& Session)
{
  SB_CRF		*Wcrf;
  SB_TRF		*Wtrf;
  QString		Str;
  QString		a;
  int			i;
  int			j;
  bool			isOK;
  SBMJD			MJD;
  double		f;
  bool			isTrfModified=FALSE;
  bool			isCrfModified=FALSE;


  if (!(Wcrf = SetUp->loadCRF())) Wcrf=new SB_CRF;
  if (!Wcrf->count()) 
    Log->write(SBLog::INF, SBLog::DATA, Wcrf->ClassName() + ": got an empty set of VLBI catalogue");

  if (!(Wtrf=SetUp->loadTRF())) Wtrf = new SB_TRF;
  if (!Wtrf->count()) 
    Log->write(SBLog::INF, SBLog::DATA, Wtrf->ClassName() + ": got an empty station catalogue");
  if ((MJD=Wtrf->epoch())==TZero)
    Log->write(SBLog::INF, SBLog::DATA, Wtrf->ClassName() + ": undefined reference epoch");


  Session.clear();
  Session.StationList .clear();
  Session.BaseList    .clear();
  Session.SourceList  .clear();
  Session.DBHHistory  .clear();
  Session.LocalHistory.clear();

  if (Session.StationByIdx) delete Session.StationByIdx;
  Session.StationByIdx = new QIntDict<SBStationInfo>(20);
  Session.StationByIdx-> setAutoDelete(FALSE);
  if (Session.StationByAka) delete Session.StationByAka;
  Session.StationByAka = new QDict<SBStationInfo>(20);
  Session.StationByAka-> setAutoDelete(FALSE);

  if (Session.BaseByIdx) delete Session.BaseByIdx;
  Session.BaseByIdx = new QIntDict<SBBaseInfo>(40);
  Session.BaseByIdx-> setAutoDelete(FALSE);
  if (Session.BaseByAka) delete Session.BaseByAka;
  Session.BaseByAka = new QDict<SBBaseInfo>(40);
  Session.BaseByAka-> setAutoDelete(FALSE);

  if (Session.SourceByIdx) delete Session.SourceByIdx;
  Session.SourceByIdx = new QIntDict<SBSourceInfo>(60);
  Session.SourceByIdx-> setAutoDelete(FALSE);
  if (Session.SourceByAka) delete Session.SourceByAka;
  Session.SourceByAka = new QDict<SBSourceInfo>(60);
  Session.SourceByAka-> setAutoDelete(FALSE);

									
  /*
   * NGS format description:
   *
   * >From mikea@ray.grdl.noaa.gov Mon May  9 16:46:30 1994
   * Message-Id: <9405091445.AA10221@vega.astri.uni.torun.pl>
   * From: Mike Abell <mikea@ray.grdl.noaa.gov>
   * Subject: NGS Format for VLBI Data
   * To: nurut@astri.uni.torun.pl
   * Date: Mon, 9 May 94 10:49:04 EDT
   * Cc: jimr@ray.grdl.noaa.gov (Jim Ray (NOAA))
   * Mailer: Elm [revision: 72.14]
   * 
   * Konstantin Kh. NURUTDINOV.
   * Senior Research Worker
   * Main Astronomical Observatory,
   * Goloseevo, Kiev-22, 252022
   * UKRAINE
   *
   * Dear Dr. Nurutdinov,
   *    
   *   I received your telemail message and attempted to fax the
   * information you requested, but with no success.  Therefore,
   * I retyped it on my computer (it originally was typed on an
   * old-fashioned electric typewriter!), and so now I can simply
   * e-mail it to you.  This should be better, actually, since
   * now we do not have to worry about print quality from the
   * fax machines.
   * 
   * Sincerely,
   * 
   * Michael D. Abell
   * NOAA/NOS/OES
   * Geosciences Laboratory
   */

  //
  //                    NGS Format for VLBI Data Transfer
  //                              10/20/83
  //
  //
  // 1.  Header Card - 80 column ASCII descriptor for the data set

  //         1         2         3         4         5         6         7         8
  //12345678901234567890123456789012345678901234567890123456789012345678901234567890
  //DATA IN NGS FORMAT FROM DATA BASE $94JAN04X  VERSION    4
  Str = s.readLine();
  if (Str.left(33)!="DATA IN NGS FORMAT FROM DATA BASE")
    {
      Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: unknown NGS file format, skipped");
      if (Wtrf) delete Wtrf;
      if (Wcrf) delete Wcrf;
      return s;
    };
  Session.SubKey = (const char*)Str.mid(43,1);
  if ((i=Str.find("VERSION"))==-1) 
    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: could not find version number");
  else 
    {
      j = Str.mid(i+8,6).stripWhiteSpace().toInt(&isOK);
      if (isOK) Session.Version=j;
      else Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: could not parse version number");
    };
  
  a = Str.mid(34,10);
  if (a.mid(0,1)=="$" || a.mid(0,1)=="@")
    a = a.mid(1,a.length()-1);
  Session.setName(a.replace(QRegExp(" "), "_"));
  a = Session.name().mid(1,7);
  MJD.setMJD(SBMJD::F_YYMonDD, a, FALSE);
  Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: got header card: session [" + 
         Session.name() + "_V" + QString("123").sprintf("%03d",Session.version()) + "] ");


  // 2.  Site Cards
  //       Col  1- 8: Eight character site name
  //       Col 11-25: X-component of the site position (meters)
  //       Col 26-40: Y-component of the site position (meters)
  //       Col 41-55: Z-component of the site position (meters)
  //       Col 57-60: Axis type (must be AZEL, EQUA, X-YN (for
  //                   X axis N, or X-YE)
  //       Col 61-70: Axis offset (meters)
  //
  //    End of site cards is indicated by a card with $END in Col 1-4

  //         1         2         3         4         5         6         7         8
  //12345678901234567890123456789012345678901234567890123456789012345678901234567890
  //GILCREEK   -2281545.20130 -1453645.84000  5756993.70570 X-YN   7.31520
  while ((Str=s.readLine()).find("$END")==-1)
    {
      if (Str.find("@END")==0) break;
      if (Str.length()<70)
	Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: unknown Site Card: [" + Str + "]");
      else
	{
	  Vector3	R;
	  SBStation	*Station;
	  double	axis	= 0.0;
	  QString	StName	= Str.mid(0, 8);
	  QString	AxisType= Str.mid(56,4).stripWhiteSpace();
	  SBStation::SMountType	MntType=SBStation::Mnt_Unkn;

	  f = Str.mid(10,15).stripWhiteSpace().toDouble(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse st-x: [" + Str + "]");
	  else R[X_AXIS]=f;

	  f = Str.mid(25,15).stripWhiteSpace().toDouble(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse st-y: [" + Str + "]");
	  else R[Y_AXIS]=f;

	  f = Str.mid(40,15).stripWhiteSpace().toDouble(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse st-z: [" + Str + "]");
	  else R[Z_AXIS]=f;

	  f = Str.mid(60,10).stripWhiteSpace().toDouble(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse st-axis: [" + Str + "]");
	  else axis=f;

	  if (AxisType=="AZEL") MntType=SBStation::Mnt_AZEL;
	  else if (AxisType=="EQUA") MntType=SBStation::Mnt_EQUA;
	  else if (AxisType=="X-YN") MntType=SBStation::Mnt_X_YN;
	  else if (AxisType=="X-YE") MntType=SBStation::Mnt_X_YE;
	  else if (AxisType=="DELU") MntType=SBStation::Mnt_Richmond;
	  else Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: unknown Axis type: ["+AxisType+"]");

	  if (!(Station=Wtrf->lookupStation(StName)))
	    {
	      Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't find station [" + StName + 
			 "] in the catalogue");
	      double d;
	      if (!(Station=Wtrf->lookupNearest(MJD, R, d)))
		{
		  Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't find nearest to the "
			     "station [" + StName + "] in the catalogue");
		  // it should be smarter later...
		  QMessageBox::warning(0, "NGS format processing", "Cannot deal with the station \""
				       + StName + "\",\n" 
				       "not enough information to pick up this entry.\n","OK");
		}
	      else 
		switch (QMessageBox::warning
			(0, "NGS format processing", 
			 "The station \"" + StName + "\" was not found in the catalogue.\n" + 
			 "Its nearest VLBI station is \"" + Station->name() + "\" [" + 
			 Station->id().toString() + "\"] (d = " + Str.sprintf("%.3f", d) +
			 " meters).\nHave we treat it as an alias, a new station or "
			 "just ignore this entry?",
			 "Alias", "New", "Ignore")) 
		  {
		  case 0:
		    Wtrf->aliasDict()->
		      insert((const char*)StName, new QString(Station->name() + " " +
							      Station->id().toString()));
		    isTrfModified=TRUE;
		    break;
		  case 1:
		    SBSite *Site;
		    if (d<3000.0 && QMessageBox::warning
			(0, "NGS format processing", 
			 "The station \"" + StName + "\" is not far from the `" + 
			 Station->site()->name() + "' site (i.e., d = " + 
			 + Str.sprintf("%.3f", d) + " meters).\n"
			 "Should we use this site for new station or create a new one?",
			 "Yes", "New"))
		      {
			Site = new SBSite(StName + " Region");
			Site -> setDomeMajor(99998); //faked entry
			Wtrf->inSort(Site);
		      }
		    else Site = Station->site();
		    
		    Station = new SBStation(Site, StName);
		    Station->setDomeMinor(1);
		    Station->setPointType(SBStation::Pt_Antenna);
		    Site->addStation(Station);
		    Station->setDescription("Station has been imported from the NGS file `" + 
					    Session.name() + "'");
		    isCrfModified=TRUE;
		    break;
		  case 2: Station=NULL; break;
		  };
	    };
	  if (Station)
	    {
	      if (Station->mountType()==SBStation::Mnt_Unkn)
		{
		  Station->setMountType(MntType);
		  Station->setAxisOffset(axis);
		  Log->write(SBLog::INF, SBLog::DATA, 
			     "SBTS_ngs: set up axis offset & mount type for \"" + 
			     Station->name() + "\" [" + Station->id().toString() +
			     "] aka [" + StName + "]");
		  isTrfModified=TRUE;
		};
	      if (Station->mountType()!=MntType)
		Log->write(SBLog::INF, SBLog::DATA, 
			   "SBTS_ngs: axis mount type for [" + StName +
			   "] differs from catalogue entry for \"" +
			   Station->name() + "\" [" + Station->id().toString() + "]");
	      if (Station->axisOffset()!=axis)
		Log->write(SBLog::INF, SBLog::DATA, 
			   "SBTS_ngs: axis offset for [" + StName +
			   "] differs from catalogue entry for \"" +
			   Station->name() + "\" [" + Station->id().toString() + "]");
	      if (Station->r_first()==v3Zero)
		{
		  //		  Station->setR(R);
		  Station->setCoords(R, v3Unit, v3Zero, v3Unit, TZero, "Initial data");
		  Log->write(SBLog::INF, SBLog::DATA, 
			     "SBTS_ngs: set up coordinates for \"" + 
			     Station->name() + "\" [" + Station->id().toString() +
			     "] aka [" + StName + "] station");
		  Station->addAttr(SBStation::OrigObsData);
		  isTrfModified=TRUE;
		};
	      if (Station->tech()==TECH_UNKN)
		{
		  Station->setTech(TECH_VLBI);
		  Log->write(SBLog::INF, SBLog::DATA, 
			     "SBTS_dbh: set up appropriate technique for \"" + 
			     Station->name() + "\" [" + Station->id().toString() +
			     "] aka [" + StName + "] station");
		  isTrfModified=TRUE;
		};
	      SBStationInfo *st = 
		new SBStationInfo(Station->id(), Session.StationList.count(), StName);
	      Session.StationList.append(st);
	      Session.StationByIdx->insert((long)st->idx(), st);
	      Session.StationByAka->insert((const char*)st->aka(), st);
	      Log->write(SBLog::DBG, SBLog::DATA, 
			 "SBTS_ngs: picked up \"" + Station->name() + "\" [" + 
			 Station->id().toString() + "] aka [" + StName + "] station");
	    }
	  else 
	    Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: can't lookup a station [" 
		       + StName + "] ");
	};
    };


  // 3. Radio source position cards
  //       Col  1- 8: Eight character source name
  //       Col 11-12: Right ascension hours
  //       Col 14-15: Right ascension minutes
  //       Col 17-28: Right ascension seconds
  //       Col    30: Declination sign
  //       Col 31-32: Declination degrees
  //       Col 34-35: Declination minutes
  //       Col 37-48: Declination seconds
  //
  //    End of source cards is indicated by a card with $END in Col 1-4

  //         1         2         3         4         5         6         7         8
  //12345678901234567890123456789012345678901234567890123456789012345678901234567890
  //1739+522  17 40    36.977590  52 11    43.410000
  //1741-038  17 43    58.855850 - 3 50     4.610000
  while ((Str=s.readLine()).find("$END")==-1)
    {
      if (Str.find("@END")==0) break;
      if (Str.length()<48)
	Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: unknown Source Card: [" + Str + "]");
      else
	{
	  QString	SoName	= Str.mid(0, 8);
	  SBSource	*Source;
	  int		deg=0, min=0;
	  double	sec=0.0, ra=0.0, dn=0.0;
	  bool		isAka=FALSE;
	  
	  i = Str.mid(10,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse so-ra-hour: [" + Str + "]");
	  else deg=i;
	  i = Str.mid(13,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse so-ra-min: [" + Str + "]");
	  else min=i;
	  f = Str.mid(16,12).stripWhiteSpace().toDouble(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse so-ra-sec: [" + Str + "]");
	  else sec=f;
	  ra=hms2rad(deg, min, sec);
	  
	  i = Str.mid(30,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse so-ra-hour: [" + Str + "]");
	  else deg=i;
	  i = Str.mid(33,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse so-ra-min: [" + Str + "]");
	  else min=i;
	  f = Str.mid(36,12).stripWhiteSpace().toDouble(&isOK);
	  if (!isOK)
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse so-ra-sec: [" + Str + "]");
	  else sec=f;
	  dn=dms2rad(deg, min, sec);
	  if (Str.mid(29,1)=="-") dn=-dn;

	  if (!(Source=Wcrf->find(SoName)))
	    {
	    if (!(Source=Wcrf->lookupAlias(SoName)))
	      {
		Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't find alias for source [" 
			   + SoName + "] in the catalogue");
		double d=0.0;
		if (!(Source=Wcrf->lookupNearest(ra, dn, d)))
		  {
		    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't find nearest to the "
			       "source [" + SoName + "] in the catalogue");
		    // it should be smarter later...
		    QMessageBox::warning(0, "NGS format processing", "Cannot deal with the source \""
					 + SoName + "\",\n" 
					 "not enough time to pick up this entry.\n","OK");
		  }
		else 
		  switch(QMessageBox::warning
			 (0, "NGS format processing", 
			  "The source \"" + SoName + "\" was not found in the catalogue.\n" + 
			  "Its nearest source is \"" + Source->name() +
			  "\" (d = " + Str.sprintf("%.3f", (double)(d*3600.0*RAD2DEG)) +
			  " arcsec).\nHave we treat it as an alias, a new source or "
			  "just ignore this entry?",
			  "Alias", "New", "Ignore"))
		    {
		    case 0:
		      Wcrf->aliasDict()->insert((const char*)SoName, new QString(Source->name()));
		      isCrfModified=TRUE;
		      break;
		    case 1:
		      Wcrf->insert((Source=new SBSource(SoName)));
		      isCrfModified=TRUE;
		      break;
		    case 2: Source=NULL;
		      break;
		    };
	      }
	    else 
	      isAka=TRUE;
	    }
	  if (Source)
	    {
	      if (Source->ra()==0.0 && Source->dn()==0.0)
		{
		  Source->setRA(ra);
		  Source->setDN(dn);
		  Source->addAttr(SBSource::OrigObsData);
		  isCrfModified=TRUE;
		  Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: set up coordinates for \"" 
			     + Source->name() + "\" source");
		};

	      SBSourceInfo *so = 
		new SBSourceInfo(Source->name(), Session.SourceList.count(), SoName);
	      Session.SourceList.append(so);
	      Session.SourceByIdx->insert((long)so->idx(), so);
	      Session.SourceByAka->insert((const char*)so->aka(), so);
	      Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: picked up \"" + Source->name() 
			 + "\"" + (QString)(isAka?" aka [" + SoName + "]":(QString)"")
			 + " source");
	    }
	  else 
	    Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: can't find source [" + SoName + 
		       "] in the catalogue");
	};
    };

  // 4. Auxiliary parameters
  //       Col  1-20: Reference frequency (MHz)
  //       Col 21-30: Group delay ambiguity spacing (nsec)
  //       Col 32-33: Delay type (GR-group, PH-phase)
  //       Col 35-36: Delay rate type (GR-group, PH-phase)
  //
  //    This card must be followed by a card with $END in Col 1-4

  //         1         2         3         4         5         6         7         8
  //12345678901234567890123456789012345678901234567890123456789012345678901234567890
  //                               GR PH
  Str=s.readLine();
  if (Str.length()<36)
    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: unknown Aux Parameters Card: [" + Str + "]");
  else
    {
      double				RefFreq=0.0, GrDelayAmbig=0.0;
      SBVLBISession::ObsDelayType	DelayType=SBVLBISession::DT_UNKN;

      f = Str.mid(0,20).stripWhiteSpace().toDouble(&isOK);
      if (!isOK)
	Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: can't parse ref.freq: [" + Str + "]");
      else RefFreq=f;

      f = Str.mid(0,20).stripWhiteSpace().toDouble(&isOK);
      if (!isOK)
	Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: can't parse ambig.spacing: [" + Str + "]");
      else GrDelayAmbig=f;

      a= Str.mid(31,2);
      if (a=="GR") DelayType=SBVLBISession::DT_GR;
      else 
	if (a=="PH") DelayType=SBVLBISession::DT_PH;
      else
	Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: can't parse delay type: [" + Str + "]");

      a= Str.mid(34,2);
      if (!(a=="GR" || a=="PH"))
	Log->write(SBLog::DBG, SBLog::DATA, "SBTS_ngs: can't parse rate type: [" + Str + "]");

      Session.setRefFreq(RefFreq);
      Session.setGrDelayAmbig(GrDelayAmbig*1.0e-9); //nsec to sec
      Session.setDelayType(DelayType);
    };

  Str=s.readLine();
  if ( !(Str.find("@END")==0 || Str.find("$END")==0) )
    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: some strange card: [" + Str + "]");

  Session.fillDicts();
  SBObsVLBIEntry*	Entry=NULL;
  int			RecN=-1;
  while (!s.eof())
    {
      SBStationInfo		*Station_1=NULL, *Station_2=NULL;
      SBSourceInfo		*Source=NULL;
      SBBaseInfo		*Base=NULL;

      Str=s.readLine();
      
      if (Str.mid(78,2)=="01")
	{
	  int		year=0, month=0, day=0, hr=0, min=0;
	  double	sec=0.0;
	  if (Entry && Entry->check()) Session.inSort(Entry);
	  Entry = new SBObsVLBIEntry(&Session);
	  //
	  // 4. Data Cards
	  //
	  //    Card #1:
	  //       Col  1- 8: Eight character site name for site 1
	  //       Col 11-18: Eight character site name for site 2
	  //       Col 21-28: Eight character source name for radio source
	  //       Col 30-33: Year of observation (e.g. 1993)
	  //       Col 35-36: Month
	  //       Col 38-39: Day
	  //       Col 41-42: Hour
	  //       Col 44-45: Minute
	  //       Col 47-60: Seconds
	  //       Col 61-70: Run identification code (if desired)
	  //       Col 71-78: Sequence number
	  //       Col 79-80: 01
	  
	  //         1         2         3         4         5         6         7         8
	  //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //GILCREEK  KOKEE     1739+522 1994  1  4 18  4  57.0000000000             0   101
	  if ((Station_1=Session.StationByAka->find((const char*)Str.mid(0,8))))
	    {
	      Entry->Station_1 = Station_1->idx();
	      Station_1->incNum();
	    }
	  else Log->write(SBLog::INF, SBLog::DATA, 
			  "SBTS_ngs: can't find Station#1 in the catalogue: [" + Str + "]");

	  if ((Station_2=Session.StationByAka->find((const char*)Str.mid(10,8))))
	    {
	      Entry->Station_2 = Station_2->idx();
	      Station_2->incNum();
	    }
	  else Log->write(SBLog::INF, SBLog::DATA, 
			  "SBTS_ngs: can't find Station#2 in the catalogue: [" + Str + "]");
	  if (Station_1 && Station_2)
	    {
	      if ((Base=Session.BaseByAka->
		  find((const char*)(Station_1->aka() + ":" + Station_2->aka()))))
		Base->incNum();
	      else
		{
		  Base =
		    new SBBaseInfo(Station_1->toString(), Station_2->toString(), 
				   Session.BaseList.count(), Station_1->aka(), Station_2->aka());
		  Session.BaseList.append(Base);
		  Session.BaseByIdx->insert((long)Base->idx(), Base);
		  Session.BaseByAka->insert((const char*)Base->aka(), Base);
		  Base->incNum();
		  Log->write(SBLog::DBG, SBLog::DATA, 
			     "SBTS_ngs: picked up \"" + Base->name() + "\" aka [" + 
			     Base->aka() + "] base");
		};
	    };
	    
	  if ((Source=Session.SourceByAka->find((const char*)Str.mid(20,8))))
	    {
	      Entry->Source = Source->idx();
	      Source->incNum();
	    }
	  else Log->write(SBLog::INF, SBLog::DATA, 
			  "SBTS_ngs: can't find Source in the catalogue: [" + Str + "]");

	  i = Str.mid(29,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"year: [" + Str + "]");
	  else year=i;
	  i = Str.mid(34,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"month: [" + Str + "]");
	  else month=i;
	  i = Str.mid(37,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"day: [" + Str + "]");
	  else day=i;
	  i = Str.mid(40,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"hour: [" + Str + "]");
	  else hr=i;
	  i = Str.mid(43,2).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"min: [" + Str + "]");
	  else min=i;
	  f = Str.mid(46,14).stripWhiteSpace().toDouble(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"sec: [" + Str + "]");
	  else sec=f;
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK); // 74, is it a bug in the description?
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else RecN=i;
	  Entry->setMJD(year, month, day, hr, min, sec);
	  Entry->Idx=Session.count();
	}
      else if (Str.mid(78,2)=="02")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #2:
	      //       Col  1-20: Observed delay (ns)
	      //       Col 21-30: Formal error for the observed delay (ns)
	      //       Col 31-50: Observed delay rate (ps/sec)
	      //       Col 51-60: Formal error for the observed delay rate (ps/sec)
	      //       Col 61-62: Data quality flag (blank or 0 indicates good data)
	      //       Col 64-65: Delay type (blank if same as in Auxiliary Parameters)
	      //       Col 67-68: Delay rate type (blank if same as in Auxiliary Parameters)
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 02
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //    5420428.71413531    .04917  -425335.7865333761    .19635 0      I    0   102
	      f = Str.mid(0,20).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "delay: [" + Str + "]");
	      else Entry->Delay=f*1.0e-9; //sec
	      f = Str.mid(20,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "delay error: [" + Str + "]");
	      else Entry->DelayErr=(f!=0.0?f:1.0e3)*1.0e-9; //sec
	      f = Str.mid(30,20).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "rate: [" + Str + "]");
	      else Entry->Rate=f*1.0e-12; // sec/sec
	      f = Str.mid(50,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "rate err: [" + Str + "]");
	      else Entry->RateErr=(f!=0.0?f:1.0e3)*1.0e-12; //sec/sec
	      if (Str.mid(60,2)=="  ") Entry->DUFlag=0;
	      else 
		{
		  i = Str.mid(60,2).stripWhiteSpace().toInt(&isOK);
		  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
					"quality factor: [" + Str + "]");
		  else Entry->DUFlag=i;
		};
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else if (Str.mid(78,2)=="03")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #3:
	      //       Col  1-10: Correlation coefficient (0-1)
	      //       Col 11-20: Formal error for correlation coefficient
	      //       Col 21-30: Fringe amplitude (J)
	      //       Col 31-40: Formal error for fringe amplitude (J)
	      //       Col 41-60: Total fringe phase (radians)
	      //       Col 61-70: Formal error for total fringe phase (radians)
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 03
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //    .00096    .00000    .00000    .00000   1.794887750403505        0.   0   103
	      f = Str.mid(0,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "correlation coef: [" + Str + "]");
	      else Entry->Correlation=f;
	      f = Str.mid(10,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "correlation coef error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				    "real `correlation coef error'!!!");
	      f = Str.mid(20,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "fringe amplitude: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				    "real `fringe amplitude'!!!");
	      f = Str.mid(30,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "fringe amplitude error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				    "real `fringe amplitude error'!!!");
	      f = Str.mid(40,20).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "total fringe phase: [" + Str + "]");
	      else Entry->FringePhase=f;
	      f = Str.mid(60,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "total fringe phase error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				    "real `fringe phase error'!!!");
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else if (Str.mid(78,2)=="04")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #4:
	      //       Col  1-10: System temperature at site 1 (K)
	      //       Col 11-15: Formal error for system temperature at site 1 (K)
	      //       Col 16-25: System temperature at site 2 (K)
	      //       Col 26-30: Formal error for system temperature at site 2 (K)
	      //       Col 31-40: Antenna temperature at site 1 (K)
	      //       Col 41-45: Formal error for antenna temperature at site 1 (K)
	      //       Col 46-55: Antenna temperature at site 2 (K)
	      //       Col 56-60: Formal error for antenna temperature at site 2 (K)
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 04
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //       .00   .0       .00   .0       .00   .0       .00   .0             0   104
	      f = Str.mid(0,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "system temperature at site#1: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `system temperature at site#1'!!!");
	      f = Str.mid(10,5).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "system temperature at site#1 error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `system temperature at site#1 error'!!!");
	      f = Str.mid(15,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "system temperature at site#2: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `system temperature at site#2'!!!");
	      f = Str.mid(25,5).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "system temperature at site#2 error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `system temperature at site#2 error'!!!");
	      
	      f = Str.mid(30,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "antenna temperature at site#1: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `antenna temperature at site#1'!!!");
	      f = Str.mid(40,5).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "antenna temperature at site#1 error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `antenna temperature at site#1 error'!!!");
	      f = Str.mid(45,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "antenna temperature at site#2: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `antenna temperature at site#2'!!!");
	      f = Str.mid(55,5).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "antenna temperature at site#2 error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
					  "real `antenna temperature at site#2 error'!!!");
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else if (Str.mid(78,2)=="05")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #5:
	      //       Col  1-10: Cable calibration correction (one-way) for site 1 (ns)
	      //       Col 11-20: Cable calibration correction (one-way) for site 2 (ns)
	      //       Col 21-30: Water vapor radiometer parameter at site 1 (ns)
	      //       Col 31-40: Formal error for water vapor radiometer at site 1 (ns)
	      //       Col 41-50: Water vapor radiometer parameter at site 2 (ns)
	      //       Col 51-60: Formal error for water vapor radiometer at site 2 (ns)
	      //       Col 62   : Water vapor radiometer parameter definition code for
	      //                   site 1 as follows:
	      //                  0 - parameter is zenith path delay
	      //                  1 - parameter is path delay along line-of-sight
	      //       Col 64   : Water vapor radiometer parameter definition code for
	      //                   site 2 (see above)
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 05
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //    .02912   -.03540    .00000    .00000    .00000    .00000             0   105
	      f = Str.mid(0,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "cable calibration correction at site#1: [" + Str + "]");
	      else Entry->Cable_1=f*1.0e-9; //sec
	      f = Str.mid(10,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "cable calibration correction at site#2: [" + Str + "]");
	      else Entry->Cable_2=f*1.0e-9; //sec
	      f = Str.mid(20,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "water vapor radiometer par. at site#1: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				       "real `water vapor radiometer par. at site#1'!!!");
	      f = Str.mid(30,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "water vapor radiometer par. at site#1 error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				       "real `water vapor radiometer par. at site#1 error'!!!");
	      f = Str.mid(40,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "water vapor radiometer par. at site#2: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				       "real `water vapor radiometer par. at site#2'!!!");
	      f = Str.mid(50,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "water vapor radiometer par. at site#2 error: [" + Str + "]");
	      else if (f!=0.0) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is "
				       "real `water vapor radiometer par. at site#2 error'!!!");
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else if (Str.mid(78,2)=="06")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #6:
	      //       Col  1-10: Ambient atmospheric temperature at site 1 (deg. C)
	      //       Col 11-20: Ambient atmospheric temperature at site 2 (deg. C)
	      //       Col 21-30: Ambient atmospheric barometric pressure at site 1 (mb)
	      //       Col 31-40: Ambient atmospheric barometric pressure at site 2 (mb)
	      //       Col 41-50: Ambient atmospheric humidity parameter at site 1
	      //       Col 51-60: Ambient atmospheric humidity parameter at site 2
	      //       Col 62   : Humidity parameter definition code for site 1 as follows:
	      //                  0 - humidity parameter is relative humidity (%)
	      //                  1 - humidity parameter is dew point (deg. C)
	      //                  2 - humidity parameter is wet bulb temperature (deg. C)
	      //       Col 64   : Humidity parameter definition code for site 2 (see above)
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 06
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //   -14.719    13.198   979.677   888.685    43.549    80.518 0 0         0   106
	      f = Str.mid(0,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "atm. temperature at site#1 error: [" + Str + "]");
	      else Entry->AtmTemp_1=f;
	      f = Str.mid(10,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "atm. temperature at site#2 error: [" + Str + "]");
	      else Entry->AtmTemp_2=f;
	      f = Str.mid(20,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "atm. pressure at site#1 error: [" + Str + "]");
	      else Entry->AtmPress_1=f;
	      f = Str.mid(30,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "atm. pressure at site#2 error: [" + Str + "]");
	      else Entry->AtmPress_2=f;
	      f = Str.mid(40,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "atm. humidity at site#1 error: [" + Str + "]");
	      else Entry->AtmHum_1=f;
	      f = Str.mid(50,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "atm. humidity at site#2 error: [" + Str + "]");
	      else Entry->AtmHum_2=f;
	      a = Str.mid(61,1);
	      if (!(a==" " || a=="0"))Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: this session "
						 "has unusual `humidity flag @1'!!!");
	      a = Str.mid(63,1);
	      if (!(a==" " || a=="0"))Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: this session "
						 "has unusual `humidity flag @2'!!!");

	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else if (Str.mid(78,2)=="07")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #7:
	      //       Col  1-10: Time difference between the reference epoch (card 1) and
	      //                   the start of the observations (e.g. -60.) (seconds)
	      //       Col 11-20: Duration of the observation (seconds)
	      //       Col 21-30: A priori UTC offset at site 1 (if any) (seconds)
	      //       Col 31-50: Observation frequency (MHz) (blank if same as in
	      //                   Auxiliary Parameter)
	      //       Col 51-60: Group delay ambiguity (ns) (blank if same as in Auxiliary
	      //                   Parameter)
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 07
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //?
	      Log->write(SBLog::DBG, SBLog::DATA, // I never see it, just for a curiousity
			 "SBTS_ngs: !!! we've got a card#7 !!!: [" + Str + "]");
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else if (Str.mid(78,2)=="08")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #8:
	      //       Col  1-20: Delay ionosphere correction (ns)
	      //       Col 21-30: Delay ionosphere correction formal error (ns)
	      //       Col 31-50: Delay rate ionosphere correction (ps/s)
	      //       Col 51-60: Delay rate ionosphere correction formal error (ps/s)
	      //       Col 62-63: Ionosphere error flag (0 = ionosphere correction OK)
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 08
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //        1.9945292734    .00559        -.0188776319    .00168  0          0   108
	      f = Str.mid(0,20).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "delay ionosphere: [" + Str + "]");
	      else Entry->DelayIon=f*1.0e-9; //sec
	      f = Str.mid(20,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "delay ionosphere error: [" + Str + "]");
	      else Entry->DelayIonErr=f*1.0e-9; //sec

	      f = Str.mid(30,20).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "delay rate ionosphere: [" + Str + "]");
	      else Entry->RateIon=f*1.0e-12; //sec/sec
	      f = Str.mid(50,10).stripWhiteSpace().toDouble(&isOK);
	      if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				    "delay rate ionosphere error: [" + Str + "]");
	      else Entry->RateIonErr=f*1.0e-12; //sec/sec
	      if (Str.mid(61,2)=="  ") Entry->IonErrorFlag=0;
	      else 
		{
		  i = Str.mid(61,2).stripWhiteSpace().toInt(&isOK);
		  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
					"ionosphere error flag: [" + Str + "]");
		  else Entry->IonErrorFlag=i;
		};
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else if (Str.mid(78,2)=="09")
	{
	  i = Str.mid(74,4).stripWhiteSpace().toInt(&isOK);
	  if (!isOK) Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: can't parse "
				"record num: [" + Str + "]");
	  else if (RecN==i)
	    {
	      //    Card #9:
	      //       Col  1-70: Comment text
	      //       Col 71-78: Sequence number
	      //       Col 79-80: 09
	      
	      //         1         2         3         4         5         6         7         8
	      //12345678901234567890123456789012345678901234567890123456789012345678901234567890
	      //?
	      Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: there is a info card: [" + Str + "]");
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: wrong secuence num: [" + Str + "]; "
		       "entry ignored");
	}
      else 
	Log->write(SBLog::INF, SBLog::DATA, "SBTS_ngs: unknown card: [" + Str + "]; "
		   "entry ignored");
      //    Notes:
      //
      // 1. All values should include a decimal point or be right-hand justified in
      //    the indicated fields.
      //
      // 2. Station coordinates are given in a Greenwich right-hand Cartesian
      //    coordinate system (e.g., Z-axis toward the CIO pole, X-axis toward
      //    the Greenwich meridian and Y-axis toward the 90 deg. east meridian).
      //
      // 3. Source coordinates are given in J2000.0 coordinates using the IAU 1984
      //    definitions for precession, nutation, aberration, etc.
      //
      // 4. The reference frequency is defined as the frequency to which the total
      //    fringe phase observable (Card #4) is referred.
      //
      // 5. The epoch of the observation is defined as the UTC epoch at which the
      //    signal arrived at site 1; i.e., the site identified in columns 1-8 of
      //    card #1.
      //
      // 6. The sequence number in columns 71-78 of the data cards is to be the same
      //    number for each data card associated with a given observation.  The
      //    numbers in columns 79-80 serve to sequence the cards within a given
      //    observation.
      //
      // 7. The delay is defined as:
      //
      //                    D = T2 - T1
      //
      //    where
      //
      //                   T1 = time of signal arrival at site 1 as measured
      //                        by the site clock at site 1
      //                   T2 = time of signal arrival at site 2 as measured
      //                        by the site clock at site 2
      //
      //    No calibration corrections (e.g. atmosphere corrections) have been added
      //    to or subtracted from these values.
      //
      // 8. The delay rate is defined as the derivative of delay with respect to time
      //    as measured by the clock at site 1.
      //
      // 9. The sense of the cable calibration is that an increase means an increase
      //    in the (one-way) electrical length of the cable carrying the phase
      //    calibration signal to the receiver box.
      //
      //10. The barometric pressure should not be corrected to sea level.
      //
      //11. It is not necessary to include blank data cards for missing data.  For
      //    example, if no meteorological data are available, card #6 may be deleted.
      //
      //12. Card #1 may not be deleted.
      //
      //13. Data should be arranged in time order.
      //
      //14. If the cards are put on tape, the tape should be 9 track with the
      //    following DCB:
      //
      //                   DCB = (RECFM=FB,LRECL=80,BLKSIZE=3200)
      //  
      //
    };
  if (Entry && Entry->check()) Session.inSort(Entry);
  
  Session.ImportedFrom=SBVLBISession::IF_NGS;
  Session.calcTMean();
  if (isTrfModified) SetUp->saveTRF(Wtrf);
  if (isCrfModified) SetUp->saveCRF(Wcrf);
  if (Wtrf) delete Wtrf;
  if (Wcrf) delete Wcrf;
  return s;
};

/** \relates SBVLBISession
 * Makes data import from a file in MkIII DBH format.
 * \param s input data stream, DBH format;
 * \param Session imported session.
 *
 * \par DBH Format desription.
 * The DBH data parser has been implemented according to the following part of 
 * "VORIN" software description (thanx to Leonid Petrov):
 * \verbinclude dbh.fmt
 * \warning This text is in koi-8 codepage and was written in Russian;
 * unfortunately, I haven't got English version of this document.
 */
//[KAS34   ] <-> KASHIMA 21701S004
//[KASH_34 ] <-> KASHIMA 21701S004
//[MARCUS10] <-> MINAMI TORI SIMA 21733S002
//[MIZUSAWA] <-> MIZUSAWA 21702S009
//[GIFUUNIV] <-> GIFU3 21747S001

SBDS_dbh &operator>>(SBDS_dbh& s, SBVLBISession& Session)
{
  SB_CRF		*Wcrf;
  SB_TRF		*Wtrf;
  QString		Str;
  QString		RefClockName ="";
  bool			IsRefClock   =FALSE;
  bool			IsClockBreak =FALSE;
  int			DBHStaNum=0, DBHSouNum=0, DBHObsNum=0;
  double		GrDelayAmbig=0.0, RefFreq=0.0;
  bool			isOK;
  SBMJD			MJD;
  SBPMNNR_NUVEL1A	Plates;
  bool			isTrfModified=FALSE;
  bool			isCrfModified=FALSE;
  bool			IsNoAllCables=FALSE; // special case
  QDict<int>		*CableSign=NULL;

  QDict<Vector3>			*EccVal=NULL;
  SBEccentricity::EccType		EType;
  QDict<SBEccentricity::EccType>	*EccType=NULL;
  

  if (!(Wcrf = SetUp->loadCRF())) Wcrf=new SB_CRF;
  if (!Wcrf->count()) 
    Log->write(SBLog::INF, SBLog::DATA, Wcrf->ClassName() + ": got an empty VLBI catalogue");

  if (!(Wtrf=SetUp->loadTRF())) Wtrf = new SB_TRF;
  if (!Wtrf->count()) 
    Log->write(SBLog::INF, SBLog::DATA, Wtrf->ClassName() + ": got an empty station catalogue");
  if ((MJD=Wtrf->epoch())==TZero)
    Log->write(SBLog::INF, SBLog::DATA, Wtrf->ClassName() + ": undefined reference epoch");


  Session.clear();
  Session.StationList .clear();
  Session.BaseList    .clear();
  Session.SourceList  .clear();
  Session.DBHHistory  .clear();
  Session.LocalHistory.clear();

  if (Session.StationByIdx) delete Session.StationByIdx;
  Session.StationByIdx = new QIntDict<SBStationInfo>(20);
  Session.StationByIdx-> setAutoDelete(FALSE);
  if (Session.StationByAka) delete Session.StationByAka;
  Session.StationByAka = new QDict<SBStationInfo>(20);
  Session.StationByAka-> setAutoDelete(FALSE);

  if (Session.BaseByIdx) delete Session.BaseByIdx;
  Session.BaseByIdx = new QIntDict<SBBaseInfo>(40);
  Session.BaseByIdx-> setAutoDelete(FALSE);
  if (Session.BaseByAka) delete Session.BaseByAka;
  Session.BaseByAka = new QDict<SBBaseInfo>(40);
  Session.BaseByAka-> setAutoDelete(FALSE);

  if (Session.SourceByIdx) delete Session.SourceByIdx;
  Session.SourceByIdx = new QIntDict<SBSourceInfo>(60);
  Session.SourceByIdx-> setAutoDelete(FALSE);
  if (Session.SourceByAka) delete Session.SourceByAka;
  Session.SourceByAka = new QDict<SBSourceInfo>(60);
  Session.SourceByAka-> setAutoDelete(FALSE);
  //

  
  SBDBHStartBlock	StartBlock;
  SBDBHHistTriplet	TH;
  SBDBHFormat		F;
  SBDBHPar		*Par=NULL, *auxPar=NULL, *subPar=NULL;
  F.setDump(Session.Dump);


  //
  s >> StartBlock;
  if ((isOK=StartBlock.isOk())) 
    {
      if (Session.Dump) StartBlock.dump(*Session.Dump);
      // start block:
      Session.setName(StartBlock.sessID().left(1)=="$"?StartBlock.sessID().
		      right(StartBlock.sessID().length()-1):StartBlock.sessID());
      Session.Name = Session.Name.replace(QRegExp(" "), "_");
      Session.SubKey = Session.name().mid(8,1).copy();
      Session.Version=StartBlock.version();
      Session.
	setExpDescr(StartBlock.expDescr()==""?QString("=undescribed="):StartBlock.expDescr());
      Session.setUserFlag("A");
      Session.setDateCreat(StartBlock.date());
      Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: processing `" + Session.name() + "' session");
    }
  else 
    Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading Start Block");
  
  if (isOK)
    {
      if (Session.Dump ) *Session.Dump << "== History Block dump: ==\n";
      while (!TH.isLast() && (isOK = TH.isOk()))
	{
	  s >> TH;
	  if (TH.isHistLine()) 
	    {
	      if (Session.Dump) TH.dump(*Session.Dump);
	      Session.dbhHistory()->addHistoryLine(TH.verNum(), TH.text(), TH.date());
	    }
	};
      if (isOK) {if (Session.Dump) *Session.Dump << "==\n";}
      else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading History Block");
    };


  if (isOK)
    {
      s >> F;
      if ((isOK=F.isOk())) {if (Session.Dump) F.dump(*Session.Dump);}
      else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading Format Description Block");
    };
  
  if (isOK)
    {
      F.getBlock(s);
      if ((isOK=F.isOk())) 
	{
	  // TOC #1:

	  if ((Par=F.lookupPar("# SITES "))) DBHStaNum = F.i2(Par, 0,0,0);
	  else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find "
			  "Number of Stations in TOC#1");

	  if ((Par=F.lookupPar("# STARS "))) DBHSouNum = F.i2(Par, 0,0,0);
	  else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find "
			  "Number of Sources in TOC#1");

	  if ((Par=F.lookupPar("NUMB OBS"))) DBHObsNum = F.i2(Par, 0,0,0);
	  else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find "
			  "Number of Observations in TOC#1");
	  if ((Par=F.lookupPar("INTERVAL"))) 
	    {
	      int yr=F.i2(Par, 0,0,0), mo=F.i2(Par, 1,0,0), dy=F.i2(Par, 2,0,0);
	      int hr=F.i2(Par, 3,0,0), mi=F.i2(Par, 4,0,0);
	      MJD.setMJD(yr<79?yr+2000:yr,mo,dy,hr,mi,0.0);
	      Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: sess's date set to " 
			 + MJD.toString(SBMJD::F_Short));
	    }
	  else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find "
			  "Obs interval in TOC#1");
	  
	  if ((Par=F.lookupPar("DELTFLAG")))
	    switch (F.i2(Par, 0,0,0))
	      {
	      case 0: 
		Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: wrong Delay type flag"
			   " in TOC#1: DELTFLAG == '0', expected '1' or '2'"); 
		break;
	      case 1: Session.setDelayType(SBVLBISession::DT_GR); break;
	      case 2: Session.setDelayType(SBVLBISession::DT_PH); break;
	      default: 
		Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: unknown Delay type flag"
			   " in TOC#1: DELTFLAG == '" + QString().setNum(F.i2(Par, 0,0,0)) + 
			   "', expected '1' or '2'");
		break;
	      }
	  else 
	    Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find Delay type flag in TOC#1");
	  
	  //----------- check/update/get station info:
	  if ((Par=F.lookupPar("SITNAMES")))
	    {
	      if (DBHStaNum!=Par->dim2())
		Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: wrong Num of Stations");
	      for (int i=0; i<Par->dim2(); i++)
		{
		  Vector3	R;
		  SBStation	*Station;
		  QString	StName	= F.str(Par, i, 0);

		  SBStation::SMountType	MntType=SBStation::Mnt_Unkn;
		  switch ((auxPar=F.lookupPar("AXISTYPS"))?F.i2(auxPar, i,0,0):0)
		    {
		    case 0:
		      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't get "
				 "axis type for `" + StName + "' in TOC#1");
		      break;
		    case 1: MntType=SBStation::Mnt_EQUA; break;
		    case 2: MntType=SBStation::Mnt_X_YN; break;
		    case 3: MntType=SBStation::Mnt_AZEL; break;
		    case 5: MntType=SBStation::Mnt_Richmond; break;
		    case 4: MntType=SBStation::Mnt_X_YE; break;
		    default:
		      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: got unknown "
				 "axis type for [" + StName + "]!");
		      break;
		    };
		  
		  R[X_AXIS]=(auxPar=F.lookupPar("SITERECS"))?F.r8(auxPar, 0,i,0):0.0;
		  if (!auxPar) Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't get "
					  "x-coo for `" + StName + "' in TOC#1");

		  R[Y_AXIS]=(auxPar=F.lookupPar("SITERECS"))?F.r8(auxPar, 1,i,0):0.0;
		  if (!auxPar) Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't get "
					  "y-coo for `" + StName + "' in TOC#1");

		  R[Z_AXIS]=(auxPar=F.lookupPar("SITERECS"))?F.r8(auxPar, 2,i,0):0.0;
		  if (!auxPar) Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't get "
					  "z-coo for `" + StName + "' in TOC#1");
		  
		  double axis=(auxPar=F.lookupPar("AXISOFFS"))?F.r8(auxPar, i,0,0):0.0;
		  if (!auxPar) Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't get "
					  "axis for `" + StName + "' in TOC#1");

		  if (!(Station=Wtrf->lookupStation(StName)))
		    {
		      Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find station [" 
				 + StName + "] in the catalogue");
		      double	d;
		      bool	isQ = FALSE;
		      int	CDP = 0;
		      if (!(Station=Wtrf->lookupNearest(MJD, R, d)))
			{
			  Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find nearest to "
				     "the station [" + StName + "] in the catalogue");
			  // it should be smarter later...
			  QMessageBox::warning(0, "DBH format processing", 
					       "Cannot deal with the station \"" + StName + "\",\n" 
					       "not enough information to pick up this entry.\n",
					       "OK");
			}
		      else 
			switch (QMessageBox::warning
				(0, "DBH format processing", 
				 "The station \"" + StName + "\" was not found in the catalogue.\n" + 
				 "It's nearest VLBI station is \"" + Station->name() + "\" [" + 
				 Station->id().toString() + "\"] (d = " + Str.sprintf("%.3f", d) +
				 " meters).\nHave we treat it as an alias, a new station or "
				 "just ignore this entry?",
				 "Alias", "New", "Ignore")) 
			  {
			  case 0:
			    Wtrf->aliasDict()->
			      insert((const char*)StName, new QString(Station->name() + " " +
								      Station->id().toString()));
			    isTrfModified=TRUE;
			    break;
			  case 1:
			    SBSite *Site;
			    if (d<80000.0 && !QMessageBox::warning //80km max for a site
				(0, "DBH format processing", 
				 "The station \"" + StName + "\" is not far from the `" + 
				 Station->site()->name() + "' site (distance is " + 
				 + Str.sprintf("%.3f", d) + " meters).\n"
				 "Should we use this site for the station or create a new one?",
				 "Yes", "New"))
			      {
				Site = Station->site();
				Log->write(SBLog::INF, SBLog::DATA, 
					   "SBDS_dbh: assigned the site \"" + 
					   Site->name() + "\" to the station [" + Station->id().toString() +
					   "] aka [" + StName + "]");
			      }
			    else 
			      {
				Site = new SBSite(StName + " Region");
				Site -> setDomeMajor(Wtrf->lastUnusedNumber()); //faked entry
				Wtrf->inSort(Site);
				Log->write(SBLog::INF, SBLog::DATA, 
					   "SBDS_dbh: created the site \"" + Site->name() +
					   "\" from the database");
				
				// tectonic plate:
				if ((auxPar=F.lookupPar("TECTPLNM")))
				  {
				    Site->setPlate(F.str(auxPar, i,0));
				    Log->write(SBLog::INF, SBLog::DATA, 
					       "SBDS_dbh: assigned tectonic plate for the site \"" + 
					       Site->name() + "\" from the database");
				  }
				else
				  Log->write(SBLog::INF, SBLog::DATA, 
					     "SBDS_dbh: cannot find tectonic plate for the site \"" + 
					     Site->name() + "\" in the database");
			      };
			    
			    Station = new SBStation(Site, StName);
			    Station->setPointType(SBStation::Pt_Antenna);
			    Station->setDomeMinor(Site->lastUnusedNumber(SBStation::Pt_Antenna));
			    if (Site->v() == v3Zero)
			      {
				Station->setCoords(R - Plates(MJD, Wtrf->epoch(), *Station), 
						   v3Unit, v3Zero, v3Unit, TZero, "Initial data");
				//Station->setR(R - Plates(MJD, Wtrf->epoch(), *Station));
				Station->setV(Plates.velocity(*Station));
				Site   ->setV(Plates.velocity(*Station));
			      }
			    else
			      Station->setCoords(R - Site->v()*(MJD - Wtrf->epoch()),
						 v3Unit, v3Zero, v3Unit, TZero, "Initial data");
			    //Station->setR(R - Site->v()*(MJD - Wtrf->epoch()) );

			    Log->write(SBLog::INF, SBLog::DATA, 
				       "SBDS_dbh: initial coordinates for \"" + 
				       Station->name() + "\" [" + Station->id().toString() +
				       "] aka [" + StName + "] station from the database; session: \"" + 
				       Session.name() + "\"");
			    Station->addAttr(SBStation::OrigObsData);
			    Site->addStation(Station);
			    
			    if ((auxPar=F.lookupPar("ECCNAMES")))
			      for (int j=0; j<auxPar->dim2(); j++)
				if (F.str(auxPar, j,0).left(8)==StName)
				  CDP=F.str(auxPar, j,0).right(8).stripWhiteSpace().toInt(&isQ);
			    if (isQ) Station->setCDP(CDP);
			    Station->setDescription("Station has been imported from the "
						    "MkIII DBH file `" + Session.name() + "'");
			    Wtrf->aliasDict()->
			      insert((const char*)StName, new QString(Station->name() + " " +
								      Station->id().toString()));
			    isCrfModified=TRUE;
			    break;
			    
			  case 2:
			    Station=NULL; 
			    break;
			  };
		    };
		  if (Station)
		    {
		      if (Station->mountType()==SBStation::Mnt_Unkn)
			{
			  Station->setMountType(MntType);
			  Station->setAxisOffset(axis);
			  Log->write(SBLog::INF, SBLog::DATA, 
				     "SBDS_dbh: set up axis offset & mount type for \"" + 
				     Station->name() + "\" [" + Station->id().toString() +
				     "] aka [" + StName + "]");
			  isTrfModified=TRUE;
			};
		      if (Station->mountType()!=MntType)
			Log->write(SBLog::INF, SBLog::DATA, 
				   "SBDS_dbh: axis mount type for [" + StName +
				   "] differs from the catalogue entry for \"" +
				   Station->name() + "\" [" + Station->id().toString() + "]");
		      if (Station->axisOffset()!=axis)
			Log->write(SBLog::INF, SBLog::DATA, 
				   "SBDS_dbh: axis offset for [" + StName +
				   "] differs from the catalogue entry for \"" +
				   Station->name() + "\" [" + Station->id().toString() + "]: " + 
				   Str.sprintf("Cat: %12.7f vs DBH: %12.7f; diff: %.4g (m)", 
					       Station->axisOffset(), axis, Station->axisOffset()-axis));
		      if (Station->r_first()==v3Zero)
			{
			  if (Station->site()->v() == v3Zero)
			    {
			      Station->setCoords(R - Plates(MJD, Wtrf->epoch(), *Station), 
						 v3Unit, v3Zero, v3Unit, TZero, "Initial data");
			      Station->setV(Plates.velocity(*Station));
			      Station->site()->setV(Plates.velocity(*Station));
			    }
			  else
			    Station->setCoords(R - Station->site()->v()*(MJD - Wtrf->epoch()), 
					       v3Unit, v3Zero, v3Unit, TZero, "Initial data");
			  Log->write(SBLog::INF, SBLog::DATA, 
				     "SBDS_dbh: set up coordinates for \"" + 
				     Station->name() + "\" [" + Station->id().toString() +
				     "] aka [" + StName + "] station");
			  Station->addAttr(SBStation::OrigObsData);
			  isTrfModified=TRUE;
			};
		      if (Station->tech()==TECH_UNKN)
			{
			  Station->setTech(TECH_VLBI);
			  Log->write(SBLog::INF, SBLog::DATA, 
				     "SBDS_dbh: set up appropriate technique for \"" + 
				     Station->name() + "\" [" + Station->id().toString() +
				     "] aka [" + StName + "] station");
			  isTrfModified=TRUE;
			};
		      SBStationInfo *st = new 
			SBStationInfo(Station->id(), Session.StationList.count(), StName);
		      Session.StationList.append(st);
		      Session.StationByIdx->insert((long)st->idx(), st);
		      Session.StationByAka->insert((const char*)st->aka(), st);
		      Log->write(SBLog::DBG, SBLog::DATA, 
				 "SBDS_dbh: picked up \"" + Station->name() + "\" [" + 
				 Station->id().toString() + "] aka [" + StName + "] station");
		    }
		  else 
		    Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: can't lookup a station [" 
			       + StName + "] ");
		};
	    }
	  else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find station names in TOC#1");

	  if ((Par=F.lookupPar("CBL STAT")) && (auxPar=F.lookupPar("CBL SIGN")))
	    {
	      SBStation	*Station;
	      CableSign = new QDict<int>(Session.StationList.count()+5);
	      CableSign -> setAutoDelete(TRUE);
	      for (int i=0; i<Par->dim2(); i++)
		{		
		  if (!(Station=Wtrf->lookupStation(F.str(Par, i, 0))))
		    Log->write(F.str(Par, i, 0).length()?SBLog::ERR:SBLog::DBG, SBLog::DATA, 
			       "SBDS_dbh: can't find a station for Cable Sign [" + F.str(Par, i, 0) + 
			       "] in the catalogue for the session " + Session.name());
		  else
		    {
		      //		      CableSign->insert((const char*)F.str(Par, i, 0), 
		      CableSign->insert(Station->id().toString(), 
					new int(F.str(auxPar, 0,0).mid(2*i,2)=="+ "?1:-1) );
		      Str.setNum(*CableSign->find(Station->id().toString()));
		      Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: station `" + Station->name() +
				 "' [" + Station->id().toString() + "] assigned \"" + 
				 Str + "\" Cable Sign");
		    };
		};
	    }
	  else 
	    {
	      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find "
			  "stations for cable sign/signs in TOC#1");
	      CableSign = new QDict<int>(Session.StationList.count()+5);
	      CableSign -> setAutoDelete(TRUE);
	      Session.addHistoryLine("WARNING: There is no cable calibration correction at all.");
	      Session.addAttr(SBVLBISesInfo::BadAllCables);
	      IsNoAllCables = TRUE;
	    };
	  if ( (Par=F.lookupPar("CLK_SITE")) )
	    {
	      RefClockName = F.str(Par, 0,0);
	      Log->write(SBLog::DBG, SBLog::DATA, 
			 "SBDS_dbh: picked up \"" + RefClockName + "\" as a clock reference site");
	      IsRefClock = TRUE;
	    }
	  else
	    Log->write(SBLog::INF, SBLog::DATA, 
		       "SBDS_dbh: cannot find clock reference site name in the TOC#1");

	  if ( (Par=F.lookupPar("CLKBREAK")) )
	    {
	      Str = F.str(Par, 0,0);
	      if (Str=="YE")
		IsClockBreak=TRUE;
	      else if (Str=="NO") // it's ok
		IsClockBreak=FALSE;
	      else
		Log->write(SBLog::INF, SBLog::DATA, 
			   "SBDS_dbh: got unexpected `Status of clock break existence' flag :[" + Str + "]");
	    }
	  else
	    Log->write(SBLog::INF, SBLog::DATA, 
		       "SBDS_dbh: cannot find `Status of clock break existence' flag in the TOC#1");
	  //
	  // ECC:
	  //
	  // an example from the dump of the session "87JAN28X_":
	  //
	  //"ECCCOORD" (aka "Eccentricity coords. (m)") [3,3,1] =
	  //(
	  //   ((0, 0, 0), (0.052, 0.006, 2.84), (0.006, 0.066, 6.525))
	  //);

	  //"ECCTYPES" (aka "Eccentricy types - XY or NE.") [3,1,1] =
	  //(
	  //   ("XYNENE")
	  //);
	  //
	  //"ECCNAMES" (aka "Ecc. site and monument names.") [9,3,1] =
	  //(
	  //   ("MOJAVE127222      ", "PBLOSSOM7254      ", "VNDNBERG7223      ")
	  //);
	  if ((Par=F.lookupPar("ECCNAMES")) && (auxPar=F.lookupPar("ECCCOORD")) && 
	      (subPar=F.lookupPar("ECCTYPES")))
	    {
	      SBStation	*Station;
	      EccVal = new QDict<Vector3>(Session.StationList.count()+5);
	      EccVal -> setAutoDelete(TRUE);
	      EccType = new QDict<SBEccentricity::EccType>(Session.StationList.count()+5);
	      EccType -> setAutoDelete(TRUE);
	      Vector3	Ecc(v3Zero);

	      for (int i=0; i<Par->dim2(); i++)
		{		
		  if (!(Station=Wtrf->lookupStation(F.str(Par, i, 0).mid(0,8))))
		    Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find a station for "
			       "Eccentricity [" + F.str(Par, i, 0).mid(0,8) + "] in the catalogue");
		  else
		    {
		      EType = F.str(subPar, 0,0).mid(2*i,2)=="XY"?
			SBEccentricity::ET_XYZ:SBEccentricity::ET_NEU;

		      Ecc[X_AXIS]=F.r8(auxPar, 0,i,0);
		      Ecc[Y_AXIS]=F.r8(auxPar, 1,i,0);
		      Ecc[Z_AXIS]=F.r8(auxPar, 2,i,0);

		      if (Ecc != v3Zero)
			{

			  EccType -> insert(Station->id().toString(), new SBEccentricity::EccType(EType));
			  EccVal  -> insert(Station->id().toString(), new Vector3(Ecc));
			  
			  Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: station `" + Station->name() +
				     "' [" + Station->id().toString() + "] got eccentricity: " + 
				     Str.sprintf("%.5f, %.5f, %.5f [%s]", (double)Ecc[X_AXIS], 
						 (double)Ecc[Y_AXIS], (double)Ecc[Z_AXIS],
						 EType==SBEccentricity::ET_XYZ?"XYZ":"NEU") );
			};
		      if ( (Ecc == v3Zero) && (Station->id().ptType()==SBStation::Pt_Marker))
			{
			  Log->write(SBLog::WRN, SBLog::DATA, 
				     "SBDS_dbh: found zero vector of ECC data for the station `" +
				     Station->name() + "' [" + Station->id().toString() + "] in the TOC#1");
			  Session.addHistoryLine("WARNING: Station `" + Station->name() + 
						 "' [" + Station->id().toString() + 
						 "] got a zero ECC vector from the DBH file");
			};
		    };
		};
	    }
	  else 
	    {
	      Log->write(SBLog::WRN, SBLog::DATA, "SBDS_dbh: can't find "
			  "stations for eccentricities in TOC#1");
	      Session.addHistoryLine("WARNING: There is no ECC data at all.");
	    };

	  //----------- check/update/get station's info done.


	  //----------- check/update/get source's info:
	  if ((Par=F.lookupPar("STRNAMES")))
	    {
	      if (DBHSouNum!=Par->dim2())
		Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: wrong Num of Sources");
	      for (int i=0; i<Par->dim2(); i++)
		{
		  QString	SoName = F.str(Par, i, 0);
		  SBSource	*Source;
		  bool		isAka=FALSE;	  
		  double ra=(auxPar=F.lookupPar("STAR2000"))?F.r8(auxPar, 0,i,0):0.0;
		  if (!auxPar) Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't get "
					  "RA for `" + SoName + "' in TOC#1");
		  
		  double dn=(auxPar=F.lookupPar("STAR2000"))?F.r8(auxPar, 1,i,0):0.0;
		  if (!auxPar) Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't get "
					  "DN for `" + SoName + "' in TOC#1");
		  
		  if (!(Source=Wcrf->find(SoName)))
		    {
		      if (!(Source=Wcrf->lookupAlias(SoName)))
			{
			  Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find an alias "
				     "for the source [" + SoName + "] in the catalogue");
			  double d=0.0;
			  if (!(Source=Wcrf->lookupNearest(ra, dn, d)))
			    {
			      Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find nearest "
					 "to the source [" + SoName + "] in the catalogue");
			      // it should be smarter later...
			      QMessageBox::warning(0, "DBH format processing", "Cannot deal with the "
						   "source \"" + SoName + "\",\n" 
						   "not enough time to pick up this entry.\n","OK");
			    }
			  else 
			    {
			      if (d*RAD2DEG*60.0 < 10.0)	// 10 arcmin: ask user.
				switch(QMessageBox::warning
				       (0, "DBH format processing", 
					"The source \"" + SoName + "\" was not found in the catalogue.\n" + 
					"Its nearest source is \"" + Source->name() +
					"\" (d = " + Str.sprintf("%.3f", (double)(d*3600.0*RAD2DEG)) +
					" arcsec).\nHave we treat it as an alias, a new source or "
					"just ignore this entry?",
					"Alias", "New", "Ignore"))
				  {
				  case 0:
				    Wcrf->aliasDict()->insert((const char*)SoName,
							      new QString(Source->name()));
				    isCrfModified=TRUE;
				    break;
				  case 1:
				    Wcrf->insert((Source=new SBSource(SoName)));
				    isCrfModified=TRUE;
				    break;
				  case 2: Source=NULL; break;
				  }
			      else
				{
				  Wcrf->insert((Source=new SBSource(SoName)));
				  isCrfModified=TRUE;
				};
			    };
			}
		      else 
			isAka=TRUE;
		    }
		  if (Source)
		    {
		      if (Source->ra()==0.0 && Source->dn()==0.0)
			{
			  Source->setRA(ra);
			  Source->setDN(dn);
			  Source->addAttr(SBSource::OrigObsData);
			  isCrfModified=TRUE;
			  Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: set up coordinates "
				     "for \"" + Source->name() + "\" source");
			};
		      SBSourceInfo *so = 
			new SBSourceInfo(Source->name(), Session.SourceList.count(), SoName);
		      Session.SourceList.append(so);
		      Session.SourceByIdx->insert((long)so->idx(), so);
		      Session.SourceByAka->insert((const char*)so->aka(), so);
		      Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: picked up \"" 
				 + Source->name() + "\"" + 
				 (QString)(isAka?" aka [" + SoName + "]":(QString)"") + " source");
		    }
		  else 
		    Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: can't find source [" + SoName + 
			       "] in the catalogue");
		};
	    }; 
	  //----------- check/update/get source's info done.
	  
	      
	}
      else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading Data Block (TOC#1)");
    };
  
  Session.fillDicts();
  SBObsVLBIEntry*	Entry=NULL;
  int			CurrentTOC=F.tcBlocks()->count()+1;
  while (!s.eof() && isOK)
    {
      F.getBlock(s);
      if ((isOK=F.isOk()))
	{
	  if (CurrentTOC >= F.tcNum()) //new obs
	    {
	      SBStationInfo		*Station_1 = NULL;
	      SBStationInfo		*Station_2 = NULL;
	      SBSourceInfo		*Source    = NULL;
	      SBBaseInfo		*Base      = NULL;

	      if (Entry && Entry->check()) 
		{
		  // check() deals with indices only,
		  // now, check the data and set up the flags (if necessary)
		  SBStationInfo *st1 = Session.stationInfo(Entry->Station_1);
		  SBStationInfo *st2 = Session.stationInfo(Entry->Station_2);
		  
		  if (Entry->AtmTemp_1 < -100.0 || Entry->AtmTemp_1 > 100.0)
		    {
		      st1  ->addAttr(SBStationInfo ::BadMeteo);
		      Entry->addAttr(SBObsVLBIEntry::BadMeteo1);
		/* for what? 
		      // keep bad meteo in local copy:
		      if (400.0<=Entry->AtmPress_1 && Entry->AtmPress_1<=1600.0) Entry->setAltAtmPress_1(Entry->AtmPress_1);
		      if (0.0<=Entry->AtmHum_1 && Entry->AtmHum_1<=105.0)        Entry->setAltAtmHum_1(Entry->AtmHum_1);
		*/
		    };
		  if (Entry->AtmPress_1 < 400.0 || Entry->AtmPress_1 > 1600.0)
		    {
		      st1  ->addAttr(SBStationInfo ::BadMeteo);
		      Entry->addAttr(SBObsVLBIEntry::BadMeteo1);
		/* for what? 
		      // keep bad meteo in local copy:
		      if (-100.0<=Entry->AtmTemp_1 && Entry->AtmTemp_1<=100.0)
			Entry->setAltAtmTemp_1(Entry->AtmTemp_1);
		      if (0.0<=Entry->AtmHum_1 && Entry->AtmHum_1<=105.0)
			Entry->setAltAtmHum_1(Entry->AtmHum_1);
		*/
		    };
		  if (Entry->AtmHum_1 < 0.0 || Entry->AtmHum_1 > 150.0)
		    {
		      st1  ->addAttr(SBStationInfo ::BadMeteo);
		      Entry->addAttr(SBObsVLBIEntry::BadMeteo1);
		/* for what? 
		      // keep bad meteo in local copy:
		      if (-100.0<=Entry->AtmTemp_1 && Entry->AtmTemp_1<=100.0)
			Entry->setAltAtmTemp_1(Entry->AtmTemp_1);
		      if (400.0<=Entry->AtmPress_1 && Entry->AtmPress_1<=1600.0)
			Entry->setAltAtmPress_1(Entry->AtmPress_1);
		*/
		    };
		  
		  if (Entry->AtmTemp_2 < -100.0 || Entry->AtmTemp_2 > 100.0)
		    {
		      st2  ->addAttr(SBStationInfo ::BadMeteo);
		      Entry->addAttr(SBObsVLBIEntry::BadMeteo2);
		/* for what? 
		      // keep bad meteo in local copy:
		      if (400.0<=Entry->AtmPress_2 && Entry->AtmPress_2<=1600.0)
			Entry->setAltAtmPress_2(Entry->AtmPress_2);
		      if (0.0<=Entry->AtmHum_2 && Entry->AtmHum_2<=105.0)
			Entry->setAltAtmHum_2(Entry->AtmHum_2);
		*/
		    };
		  if (Entry->AtmPress_2 < 400.0 || Entry->AtmPress_2 > 1600.0)
		    {
		      st2  ->addAttr(SBStationInfo ::BadMeteo);
		      Entry->addAttr(SBObsVLBIEntry::BadMeteo2);
		/* for what? 
		      // keep bad meteo in local copy:
		      if (-100.0<=Entry->AtmTemp_2 && Entry->AtmTemp_2<=100.0)
			Entry->setAltAtmTemp_2(Entry->AtmTemp_2);
		      if (0.0<=Entry->AtmHum_2 && Entry->AtmHum_2<=105.0)
			Entry->setAltAtmHum_2(Entry->AtmHum_2);
		*/
		    };
		  if (Entry->AtmHum_2 < 0.0 || Entry->AtmHum_2 > 150.0)
		    {
		      st2  ->addAttr(SBStationInfo ::BadMeteo);
		      Entry->addAttr(SBObsVLBIEntry::BadMeteo2);
		/* for what? 
		      // keep bad meteo in local copy:
		      if (-100.0<=Entry->AtmTemp_2 && Entry->AtmTemp_2<=100.0)
			Entry->setAltAtmTemp_2(Entry->AtmTemp_2);
		      if (400.0<=Entry->AtmPress_2 && Entry->AtmPress_2<=1600.0)
			Entry->setAltAtmPress_2(Entry->AtmPress_2);
		*/
		    };
		  Session.inSort(Entry);
		};
	      
	      Entry = new SBObsVLBIEntry(&Session);

	      // get station names:
	      if ((Par=F.lookupPar("BASELINE")))
		{
		  if ((Station_1=Session.StationByAka->find((const char*)(Str=F.str(Par, 0,0)))))
		    Entry->Station_1 = Station_1->idx();
		  else Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: got an undeclared Station#1 "
				  "in the TOC#1: [" + Str + "], entry ignored");
		  if ((Station_2=Session.StationByAka->find((const char*)(Str=F.str(Par, 1,0)))))
		    Entry->Station_2 = Station_2->idx();
		  else Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: got an undeclared Station#2 "
				  "in the TOC#1: [" + Str + "], entry ignored");
		  if (Station_1 && Station_2)
		    {
		      if (!(Base=Session.BaseByAka->
			   find((const char*)(Station_1->aka() + ":" + Station_2->aka()))))
			{
			  Base =
			    new SBBaseInfo(Station_1->toString(), Station_2->toString(), 
					   Session.BaseList.count(), 
					   Station_1->aka(), Station_2->aka());
			  Session.BaseList.append(Base);
			  Session.BaseByIdx->insert((long)Base->idx(), Base);
			  Session.BaseByAka->insert((const char*)Base->aka(), Base);
			  //Base->incNum();
			  Log->write(SBLog::DBG, SBLog::DATA, 
				     "SBDS_dbh: picked up \"" + Base->name() + "\" aka [" + 
				     Base->aka() + "] base");
			};
		    };
		}
	      else Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find "
			      "`BASELINE'-Tag in TOC #" + Str.setNum(F.tcNum()));
	      
	      // get source name:
	      if ((Par=F.lookupPar("STAR ID ")))
		if ((Source=Session.SourceByAka->find((const char*)(Str=F.str(Par, 0,0)))))
		  Entry->Source = Source->idx();
		else 
		  Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find Source [" + Str +
			     "] in the SourceList of the session; entry ignored");
	      else Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find "
			      "`STAR ID '-Tag in TOC #" + Str.setNum(F.tcNum()));
	      
	      // get the epoch:
	      if ((Par=F.lookupPar("UTC TAG ")) && (auxPar=F.lookupPar("SEC TAG ")))
		{
		  int		year=F.i2(Par, 0,0,0), month=F.i2(Par, 1,0,0), 
		    day=F.i2(Par, 2,0,0), hr=F.i2(Par, 3,0,0), min=F.i2(Par, 4,0,0);
		  double	sec=F.r8(auxPar, 0,0,0);
		  Entry->setMJD(year<79?year+2000:year, month, day, hr, min, sec);
		}
	      else Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: can't find "
			      "`UTC TAG '-Tag or `SEC TAG '-Tag in TOC #" + Str.setNum(F.tcNum()));
	      Entry->Idx=Session.count();
	    };
	  CurrentTOC = F.tcNum();

	  // some aux parameters:
	  if ((Par=F.lookupPar("# AMBIG ")))		// `No. of ambiguities              '
	    Entry->AmbiguityFactor=F.i2(Par, 0,0,0);
	  if ((Par=F.lookupPar("GPDLAMBG")))		// `Group delay ambiguity (sec).....'
	    {
	      Entry->GrDelayAmbig=F.r8(Par, 0,0,0);
	      if (GrDelayAmbig==0.0) GrDelayAmbig=Entry->GrDelayAmbig;
	    };
	  if ((Par=F.lookupPar("REF FREQ")))		// `Freq to wh.phase is referred....'
	    {
	      Entry->RefFreq=F.r8(Par, 0,0,0);
	      if (RefFreq==0.0) RefFreq=Entry->RefFreq;
	    };
	  
	  // delay & rate:
	  if ((Par=F.lookupPar("DEL OBSV")))		// `Observed delay us in 2 parts    '
	    Entry->Delay=1.0e-6*(F.r8(Par, 0,0,0) + F.r8(Par, 1,0,0)); // us->sec
	  if ((Par=F.lookupPar("DELSIGMA")))		// `Delay err (sec).................'
	    Entry->DelayErr=F.r8(Par, 0,0,0);
	  if (Entry->DelayErr==0.0) Entry->DelayErr=1.0e-6;
	  if ((Par=F.lookupPar("DELUFLAG")))		// `Delay unweight flag             '
	    Entry->DUFlag=F.i2(Par, 0,0,0);

	  if ((Par=F.lookupPar("RAT OBSV")))		// `Obs rate (s per s)..............'
	    Entry->Rate=F.r8(Par, 0,0,0);
	  if ((Par=F.lookupPar("RATSIGMA")))		// `Rate err (sec per sec)..........'
	    Entry->RateErr=F.r8(Par, 0,0,0);
	  if (Entry->RateErr==0.0) Entry->RateErr=1.0e-9;
	  if ((Par=F.lookupPar("RATUFLAG")))		// `Delay rate unweight flag........'
	    Entry->RUFlag=F.i2(Par, 0,0,0);
	  
	  // quality code:
	  if ((Par=F.lookupPar("QUALCODE")))		// `FRNGE quality  index 0 --> 9....'
	    {
	      Str = F.str(Par, 0,0);
	      Entry->QualCode[0] = Str.at(0).latin1();
	      Entry->QualCode[1] = Str.at(1).latin1();
	    };
	  
	  /*
	    // correlation & phase:
	    if ((Par=F.lookupPar("COHERCOR")))		// `Corr coeff (0 --> 1)............'
	    Entry->Correlation=F.r8(Par, 0,0,0);
	    //  if ((Par=F.lookupPar("TOTPHASE")))	// `Total phase degrees mod 360.....'
	    // Test:
	    //if ((Par=F.lookupPar("GC PHASE")))		// `Tot phase ref to cen of Earth...'
	    //if ((Par=F.lookupPar("TOTPCENT")))		// `Tot phase at central epoch......'
	    if ((Par=F.lookupPar("GCRESPHS")))		// `Resid phs corrected to cen of E.'
	    Entry->FringePhase=F.r8(Par, 0,0,0);
	  */
	  if ((Par=F.lookupPar("AMPBYFRQ")))		// `Amp,phs by chan(0-1)(-180to180).'
	    {
	      Entry->Correlation=F.r8(Par, 0,0,0);
	      Entry->FringePhase=F.r8(Par, 1,0,0);
	      if ((Par=F.lookupPar("CALBYFRQ")))       	// `PC amp,phs,frq by sta,channel'
		{
		  int	CalAmp1=0, CalAmp2=0, CalPhs1=0, CalPhs2=0;

		  CalAmp1 = F.i2(Par, 0,0,0);
		  CalPhs1 = F.i2(Par, 1,0,0);
		  CalAmp2 = F.i2(Par, 0,1,0);
		  CalPhs2 = F.i2(Par, 1,1,0);
		  
		  //		  if (CalAmp1>0)
		  //		    Entry->Correlation*=CalAmp1;
		  //		  if (CalAmp2>0)
		  //		    Entry->Correlation*=CalAmp2;
		  
		  Entry->FringePhase-= (CalPhs2 - CalPhs1)/100.0;
		};
	      
	    };
	  
	  // cable correction:
	  if (IsNoAllCables)
	    {
	      Entry->addAttr(SBObsVLBIEntry::BadCable1);
	      Entry->addAttr(SBObsVLBIEntry::BadCable2);
	      SBStationInfo *st = Session.stationInfo(Entry->Station_1);
	      if (st) 
		st->addAttr(SBStationInfo::BadCable);
	      st = Session.stationInfo(Entry->Station_2);
	      if (st) 
		st->addAttr(SBStationInfo::BadCable);
	    }
	  else if ((Par=F.lookupPar("CABL DEL")) && Entry->Station_1>-1 && Entry->Station_2>-1)
	    // `Cable calibration data          '
	    {
	      int	*CSign=NULL;
	      Str = Session.stationInfo(Entry->Station_1)->toString();
	      if ( (CSign=CableSign->find(Str)) )
		{
		  Entry->Cable_1=F.r8(Par, 0,0,0)**CSign;
		  if (*CSign == 0)
		    Entry->addAttr(SBObsVLBIEntry::BadCable1);
		}
	      else 
		{
		  Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: can't find [" + Str +
			     "] station in CableSign dict");
		  CableSign->insert((const char*)Str, new int(1) );
		  Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: station `" + Str +
			     "' assigned `+1' Cable Sign");
		  Session.addHistoryLine("WARNING: Station [" + Str + 
					 "] has undefined cable calibration.");
		  SBStationInfo *st1 = Session.stationInfo(Entry->Station_1);
		  if (st1) 
		    st1->addAttr(SBStationInfo::BadCable);
		  Entry->addAttr(SBObsVLBIEntry::BadCable1);
		};
	      
	      Str = Session.stationInfo(Entry->Station_2)->toString();
	      if ( (CSign=CableSign->find(Str)) )
		{
		  Entry->Cable_2=F.r8(Par, 1,0,0)**CSign;
		  if (*CSign == 0)
		    Entry->addAttr(SBObsVLBIEntry::BadCable2);
		}
	      else 
		{
		  Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: can't find [" + Str +
			     "] station in CableSign dict");
		  CableSign->insert((const char*)Str, new int(1) );
		  Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: station `" + Str +
			     "' assigned `+1' Cable Sign");
		  Session.addHistoryLine("WARNING: Station [" + Str + 
					 "] has undefined cable calibration.");
		  SBStationInfo *st2 = Session.stationInfo(Entry->Station_2);
		  if (st2) 
		    st2->addAttr(SBStationInfo::BadCable);
		  Entry->addAttr(SBObsVLBIEntry::BadCable2);
		};
	    };
	  
	  // metheo data:
	  if ((Par=F.lookupPar("TEMP C  ")))		// `Temp in C at local WX station   '
	    {
	      Entry->AtmTemp_1=F.r8(Par, 0,0,0);
	      Entry->AtmTemp_2=F.r8(Par, 1,0,0);
	    };
	  if ((Par=F.lookupPar("ATM PRES")))		// `Pressure in mb at site          '
	    {
	      Entry->AtmPress_1=F.r8(Par, 0,0,0);
	      Entry->AtmPress_2=F.r8(Par, 1,0,0);
	    };
	  if ((Par=F.lookupPar("REL.HUM.")))		// `Rel.Hum. at local WX st (50%=.5)'
	    {
	      Entry->AtmHum_1=100.0*F.r8(Par, 0,0,0);	// %%
	      Entry->AtmHum_2=100.0*F.r8(Par, 1,0,0);	// %%
	    };
	  
	  // ionosphere:
	  if ((Par=F.lookupPar("ION CORR")))		// `Ion correction. Add to theo. sec'
	    {
	      Entry->DelayIon=F.r8(Par, 0,0,0);
	      Entry->RateIon =F.r8(Par, 1,0,0);
	    };
	  if ((Par=F.lookupPar("IONRMS  ")))		// `Ion correction to sigma. sec    '
	    {
	      Entry->DelayIonErr=F.r8(Par, 0,0,0);
	      Entry->RateIonErr =F.r8(Par, 1,0,0);
	    };
	  if ((Par=F.lookupPar("ION CODE")))		// `Ion corr code. -1=no good, 0=OK '
	    Entry->IonErrorFlag=F.i2(Par, 0,0,0);
	  
	}
      else Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading Data Block");
    };

  if (Entry && Entry->check()) // last entry:
    {
      // check() deals with indices only,
      // now, check the data and set up the flags (if necessary)
      SBStationInfo *st1 = Session.stationInfo(Entry->Station_1);
      SBStationInfo *st2 = Session.stationInfo(Entry->Station_2);

      if (Entry->AtmTemp_1 < -100.0 || Entry->AtmTemp_1 > 100.0)
	{
	  st1  ->addAttr(SBStationInfo ::BadMeteo);
	  Entry->addAttr(SBObsVLBIEntry::BadMeteo1);
	  /* for what? 
	  // keep bad meteo in local copy:
	  if (400.0<=Entry->AtmPress_1 && Entry->AtmPress_1<=1600.0)
	  Entry->setAltAtmPress_1(Entry->AtmPress_1);
	  if (0.0<=Entry->AtmHum_1 && Entry->AtmHum_1<=105.0)
	  Entry->setAltAtmHum_1(Entry->AtmHum_1);
	  */
	};
      if (Entry->AtmPress_1 < 400.0 || Entry->AtmPress_1 > 1600.0)
	{
	  st1  ->addAttr(SBStationInfo ::BadMeteo);
	  Entry->addAttr(SBObsVLBIEntry::BadMeteo1);
	  /* for what? 
	  // keep bad meteo in local copy:
	  if (-100.0<=Entry->AtmTemp_1 && Entry->AtmTemp_1<=100.0)
	  Entry->setAltAtmTemp_1(Entry->AtmTemp_1);
	  if (0.0<=Entry->AtmHum_1 && Entry->AtmHum_1<=105.0)
	  Entry->setAltAtmHum_1(Entry->AtmHum_1);
	  */
	};
      if (Entry->AtmHum_1 < 0.0 || Entry->AtmHum_1 > 150.0)
	{
	  st1  ->addAttr(SBStationInfo ::BadMeteo);
	  Entry->addAttr(SBObsVLBIEntry::BadMeteo1);
	  /* for what? 
	  // keep bad meteo in local copy:
	  if (-100.0<=Entry->AtmTemp_1 && Entry->AtmTemp_1<=100.0)
	  Entry->setAltAtmTemp_1(Entry->AtmTemp_1);
	  if (400.0<=Entry->AtmPress_1 && Entry->AtmPress_1<=1600.0)
	  Entry->setAltAtmPress_1(Entry->AtmPress_1);
	  */
	};
      
      if (Entry->AtmTemp_2 < -100.0 || Entry->AtmTemp_2 > 100.0)
	{
	  st2  ->addAttr(SBStationInfo ::BadMeteo);
	  Entry->addAttr(SBObsVLBIEntry::BadMeteo2);
	  /* for what? 
	  // keep bad meteo in local copy:
	  if (400.0<=Entry->AtmPress_2 && Entry->AtmPress_2<=1600.0)
	  Entry->setAltAtmPress_2(Entry->AtmPress_2);
	  if (0.0<=Entry->AtmHum_2 && Entry->AtmHum_2<=105.0)
	  Entry->setAltAtmHum_2(Entry->AtmHum_2);
	  */
	};
      if (Entry->AtmPress_2 < 400.0 || Entry->AtmPress_2 > 1600.0)
	{
	  st2  ->addAttr(SBStationInfo ::BadMeteo);
	  Entry->addAttr(SBObsVLBIEntry::BadMeteo2);
	  /* for what? 
	  // keep bad meteo in local copy:
	  if (-100.0<=Entry->AtmTemp_2 && Entry->AtmTemp_2<=100.0)
	  Entry->setAltAtmTemp_2(Entry->AtmTemp_2);
	  if (0.0<=Entry->AtmHum_2 && Entry->AtmHum_2<=105.0)
	  Entry->setAltAtmHum_2(Entry->AtmHum_2);
	  */
	};
      if (Entry->AtmHum_2 < 0.0 || Entry->AtmHum_2 > 150.0)
	{
	  st2  ->addAttr(SBStationInfo ::BadMeteo);
	  Entry->addAttr(SBObsVLBIEntry::BadMeteo2);
	  /* for what? 
	  // keep bad meteo in local copy:
	  if (-100.0<=Entry->AtmTemp_2 && Entry->AtmTemp_2<=100.0)
	  Entry->setAltAtmTemp_2(Entry->AtmTemp_2);
	  if (400.0<=Entry->AtmPress_2 && Entry->AtmPress_2<=1600.0)
	  Entry->setAltAtmPress_2(Entry->AtmPress_2);
	  */
	};
      Session.inSort(Entry);
    };
  
  
  //
  //  if (Session.BaseList.count())
  //    {
  //      Session.setRefFreq(Session.BaseList.at(0)->refFreq());
  //      Session.setGrDelayAmbig(Session.BaseList.at(0)->grDelayAmbig());//sec
  //    };

  Session.setRefFreq(RefFreq);
  Session.setGrDelayAmbig(GrDelayAmbig);//sec
  Session.ImportedFrom=SBVLBISession::IF_DBH;
  Session.calcTMean();
  
  SBStationInfo	*st_1=NULL;
  SBStationInfo	*st_2=NULL;
  SBNamed	 baseName;
  SBBaseInfo	*base=NULL;
  SBSourceInfo	*src =NULL;
  // calc number of observations per object  
  for (Entry=Session.first(); Entry; Entry=Session.next())
    {
      st_1 = Session.stationInfo(Entry->Station_1);
      if (st_1)
	st_1->incNum();
      st_2 = Session.stationInfo(Entry->Station_2);
      if (st_2)
	st_2->incNum();
      src = Session.sourceInfo(Entry->Source);
      if (src)
	src->incNum();
      baseName.setName(st_1->toString() + ":" + st_2->toString());
      if ( (base = Session.baseList()->find(&baseName)) )
	base->incNum();
    };


  // assign to a station the `reference clocks` mark
  if (IsRefClock)
    {
      if ( (st_1 = Session.stationInfo(RefClockName)) )
	{
	  st_1->addAttr(SBStationInfo::refClock);
	  Session.addHistoryLine("The clock reference site has been assigned to the station " + 
				 st_1->aka() + " [" + st_1->toString() + "] according to the DBH record.");
	}
      else
	{
	  Log->write(SBLog::WRN, SBLog::DATA, "SBDS_dbh: cannot find clock reference site [" + 
		     RefClockName + "] in the list of stations");
	  IsRefClock = FALSE;
	};
    };
  if (!IsRefClock) // just get from the first 'valid' obs
    {
      Entry = Session.first();
      while (Entry && Entry->DUFlag!=0)
	Entry = Session.next();
      
      st_1=Session.stationInfo(Entry->Station_1);
      st_2=Session.stationInfo(Entry->Station_2);
      if (st_1->num()>st_2->num())
	st_1->addAttr(SBStationInfo::refClock);
      else
	st_2->addAttr(SBStationInfo::refClock);
    };
  
  
  // sets some attrs:
  bool IsBadAllCables = TRUE;
  bool IsBadAllMeteos = TRUE;
  bool IsBadCable = FALSE;
  bool IsBadMeteo = FALSE;
  for (st_1=Session.stationList()->first(); st_1; st_1=Session.stationList()->next())
    {
      if (!st_1->isAttr(SBStationInfo::BadCable))
	IsBadAllCables = FALSE;
      else
	IsBadCable=TRUE;
      if (!st_1->isAttr(SBStationInfo::BadMeteo))
	IsBadAllMeteos = FALSE;
      else
	{
	  IsBadMeteo = TRUE;
	  Session.addHistoryLine("WARNING: The station " + st_1->aka() + " [" + st_1->toString() + 
				 "] has wrong meteo data.");
	};
    };
  if (IsBadAllCables)
    Session.addAttr(SBVLBISesInfo::BadAllCables);
  if (IsBadAllMeteos)
    {
      Session.addAttr(SBVLBISesInfo::BadAllMeteos);
      Session.addHistoryLine("WARNING: All stations have wrong meteo data.");
    }
  if (IsBadCable)
    Session.addAttr(SBVLBISesInfo::BadCable);
  if (IsBadMeteo)
    Session.addAttr(SBVLBISesInfo::BadMeteo);
  if (IsClockBreak)
    Session.addAttr(SBVLBISesInfo::HasClockBreak);

  // eccentricities:
  if (EccVal && EccType)
    {
      SBMJD				TSt, TFi;
      SBEccentricity			Ecc;
      Vector3				*EccVector=NULL;
      SBEccentricity::EccType		*eccType=NULL;
      TSt = *Session.first();
      TFi = *Session.last();
      for (st_1=Session.stationList()->first(); st_1; st_1=Session.stationList()->next())
	{
	  if ( (EccVector=EccVal->find(st_1->toString())) && (eccType = EccType->find(st_1->toString())) )
	    {
	      Ecc.setAka(st_1->aka());
	      Ecc.setTStart(TSt);
	      Ecc.setTFinis(TFi);
	      Ecc.setEcc(*EccVector);
	      Ecc.setType(*eccType);
	      Ecc.setSessionName(Session.name());
	/*
	      Wtrf->ecc().registerEcc(*st_1, Ecc);
	*/
	      isTrfModified = TRUE;
	    };
	};
    };
  

  // OK, here is a walk around UTC bug in MK-III database realization:
  SBMJD				TSt, TFi;
  TSt = *Session.first();
  TFi = *Session.last();
  if ( (TSt.date()==48803 && TFi.date()>=48804) || (TSt.date()==49533 && TFi.date()>=49534) )
    {
      Log->write(SBLog::WRN, SBLog::DATA, 
		 "SBDS_dbh: a session [" + Session.name() + 
		 "] with the poor UTC implementation is detected; a work around started");
      Session.addHistoryLine("WARNING: this is a \"poor UTC implementation\" session; "
			     "a correction to epochs after midnight is applied");
      for (SBObsVLBIEntry *E=Session.first(); E; E=Session.next())
	if (E->date()==48804 || E->date()==49534 || E->date()==49535)
	  {
	    int	Hour=E->hour(), Min=E->min();
	    double	Sec=E->sec();
	    E->setTime((Hour*60.0*60.0 + Min*60.0 + Sec + 1.0 )/86400.0);
	  };
    };
  
  
  if (isTrfModified) SetUp->saveTRF(Wtrf);
  if (isCrfModified) SetUp->saveCRF(Wcrf);
  if (CableSign) delete CableSign;
  if (EccVal) delete EccVal;
  if (EccType) delete EccType;
  if (Wtrf) delete Wtrf;
  if (Wcrf) delete Wcrf;
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHPhysRec's friends implementation							*/
/*												*/
/*==============================================================================================*/
SBDS_dbh &operator>>(SBDS_dbh& s, SBDBHPhysRec& PH)
{
  int		HDD1, EDD1, HD2, ED2;
  short		DD1;
  char		c;
  QString	str;
  PH.isOK=TRUE;
  s >> HDD1;
  if (HDD1!=2)
    {
      str.sprintf("[%d]", HDD1);
      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: corrupted DBH file "
		 "(HDD1!=2,=" + str + ")");
      PH.isOK=FALSE;
      return s;
    };
  s >> DD1 >> EDD1;
  if (EDD1!=HDD1)
    {
      str.sprintf("[%d] [%d]", EDD1, HDD1);
      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: corrupted DBH file (EDD1!=HDD1): " + str);
      PH.isOK=FALSE;
      return s;
    };

  s >> HD2;
  if (HD2!=DD1+DD1)
    {
      str.sprintf("[%d] [%d]", HD2, DD1);
      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: corrupted DBH file (HD2!=DD1+DD1) " + str);
      PH.isOK=FALSE;
      return s;
    };
  
  PH.Length = HD2;
  int i = PH.parseLR(s);

  if (i>PH.Length)
    {
      str.sprintf("[%d]>[%d]", i, PH.Length);
      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: corrupted DBH file: LR read more "
		 "then Length ("+str+")");
      PH.isOK=FALSE;
      return s;
    }
  else for (int j=i; j<PH.Length; j++) s>>(Q_UINT8 &)c;
  s >> ED2;
  if (ED2!=HD2)
    {
      str.sprintf("[%d] [%d]", ED2, HD2);
      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: corrupted DBH file (ED2!=HD2) " + str);
      PH.isOK=FALSE;
    };
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecPrefixed's  implementation							*/
/*												*/
/*==============================================================================================*/
bool SBDBHLRecPrefixed::isPrefixParsed(SBDS_dbh& s)
{
  s>>(Q_UINT8 &)Prefix[0]>>(Q_UINT8 &)Prefix[1];
  if (!isP())
    {
      if (!isAlter())
	Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: corrupted DBH file: not a `"
		   + QString(P) + "' record (got `" + 
		   QString().sprintf("%c%c", Prefix[0],Prefix[1])+ "')");
      return (isOK=FALSE);
    }
  else return TRUE;
};   
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecTC's  implementation								*/
/*												*/
/*==============================================================================================*/
int SBDBHLRecTC::parseLR(SBDS_dbh& s)
{
  TOCType=NumTEBlocks=0;
  IsZ2=FALSE;
  if (!isPrefixParsed(s))
    {
      unsigned char c[2];
      s >>c[0]>>c[1];
      if (Prefix[0]=='Z' && Prefix[1]=='Z' && 
	  c[0]==(unsigned char)7 && c[1]==(unsigned char)208){isOK=TRUE;IsZ2=TRUE;};
      return 4;
    }
  else s>>TOCType>>NumTEBlocks;
  return 6;
};   
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecDE's  implementation								*/
/*												*/
/*==============================================================================================*/
int SBDBHLRecDE::parseLR(SBDS_dbh& s)
{
  P1=P2=P3=P4=P5=P6=P7=0;
  IsZ3=FALSE;
  if (!isPrefixParsed(s)) 
    {
      short z3p1;
      s >>z3p1;
      if (Prefix[0]=='Z' && Prefix[1]=='Z'){isOK=TRUE;IsZ3=TRUE;P1=z3p1;};
      return 4;
    }
  else s>>P1>>P2>>P3>>P4>>P5>>P6>>P7;
  return 16;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHDRecString's implementation								*/
/*												*/
/*==============================================================================================*/
const QString SBDBHDRecString::val(SBDBHPar* Par, int dim2, int dim3) // 0-index
{
  int	l = 2*Par->dim1()+1;
  char	*c = new char[l]; 
  char	*p = LogRec + 2*(Par->offset() + Par->dim1()*dim2 + Par->dim1()*Par->dim2()*dim3);
  for (int i=0; i<l; i++) *(c+i)=*(p+i);
  *(c+l-1)=0;
  QString Ret = (const char*)c;
  delete []c;
  return Ret;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHHistTriplet's friends implementation						*/
/*												*/
/*==============================================================================================*/
SBDS_dbh &operator>>(SBDS_dbh& s, SBDBHHistTriplet& HT)
{
  s >> HT.Rec1;
  if (HT.Rec1.isP()) 
    {
      s >> HT.Rec2 >> HT.Rec3;
      if (2*HT.Rec1.histLength()!=HT.Rec3.length())
	Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: corrupted DBH file: wrong history "
		   "string length for `" + HT.Rec3.text() + "'");
    };
  HT.isOK = HT.Rec1.isOk() && HT.Rec2.isOk() && HT.Rec3.isOk();
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHTEBlock's implementation								*/
/*												*/
/*==============================================================================================*/
int SBDBHTEBlock::parseP3Rec(SBDS_dbh& s)
{
  char tmps[9];
  for (int i=0; i<Rec1.GroupNum; i++)
    {
      SBDBHPar *Parm = new SBDBHPar;
      s.readRawBytes(tmps, 8);
      *(tmps+8)=0;
      Parm->Abbrev=(const char*)tmps;
      s >> Parm->VerOfMod >> Parm->Dim1 >> Parm->Dim2 >> Parm->Dim3;
      ListParms.append(Parm);
    };
  return 16*Rec1.GroupNum;
};

int SBDBHTEBlock::parseP4Rec(SBDS_dbh& s)
{
  char tmps[33];
  SBDBHPar *Parm = ListParms.first();
  for (int i=0; i<Rec1.GroupNum; i++, Parm = ListParms.next())
    {
      s.readRawBytes(tmps, 32); 
      *(tmps+32)=0;
      Parm->Descr=(const char*)tmps;
    };
  return 32*Rec1.GroupNum;
};

void SBDBHTEBlock::dump(QTextStream& s)
{
  const char *sTypes[] = {"R8", "I2", "A2", "D8", "J4", "UN"};
  QString	Str;
  s << "== TE[" << Rec1.TEBlockNum << "] Block dump: ==\n";
  Rec1.dump(s);
  s << "TE Block format (Offsets are in sizeof(Par)):\n"
    << "---- --------  --- --- ---  --- ---- -- --------------------------------\n"
    << " Num Abbrev.    D1  D2  D3  Ver Offs Tp Description\n"
    << "---- --------  --- --- ---  --- ---- -- --------------------------------\n";
  int		i=0;
  for (SBDBHPar* Par=ListParms.first(); Par; Par=ListParms.next(), i++)
    {
      Str.sprintf("%3d. %8s [%3d,%3d,%3d] %3d %4d %2s %32s", 
		  i, (const char*)Par->abbrev(),Par->dim1(),Par->dim2(),Par->dim3(),
		  Par->verOfMod(),Par->offset(),sTypes[Par->type()],(const char*)Par->descr());
      s << Str << "\n";
    };
  s << "--------------------------------------------------------------------\n==\n";
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHTCBlock's friends implementation							*/
/*												*/
/*==============================================================================================*/
SBDS_dbh &operator>>(SBDS_dbh& s, SBDBHTCBlock& B)
{
  s>>B.Rec;
  B.ListTEBlock.clear();
  if (!B.isLast() && (B.isOK=B.Rec.isOk()))
    for (int i=0; i<B.Rec.numTEBlocks(); i++)
      {
	SBDBHTEBlock *TEBlock = new SBDBHTEBlock;
	s >> *TEBlock;
	if ((B.isOK=B.isOK&&TEBlock->isOk())) B.ListTEBlock.append(TEBlock);
	else delete TEBlock;
      };
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHStartBlock's implementation							*/
/*												*/
/*==============================================================================================*/
void SBDBHStartBlock::dump(QTextStream& s)
{
  s << "== Start Block dump: =="<<"\nDatabase name         : "<<DbName.text()
    << "\nDate of creation      : "<<SBMJD(DateVal).toString(SBMJD::F_Short) 
    << " ("<<Date[0]<<":"<<Date[1]<<":"<<Date[2]<<":"<<Date[3]<<":"<<Date[4]<<")"
    << "\nExperiment description: "<<ExpDescr.text().stripWhiteSpace() 
    << "\nSession ID            : "<<SessID.text()<< "\nVersion               : "<<Ver[0] 
    << "\nPrevious Database name: "<<PrevDb.text()<< "\nPrevious description  : "
    << Prev.text() << "\n==\n";
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHStartBlock's friends implementation						*/
/*												*/
/*==============================================================================================*/
SBDS_dbh &operator>>(SBDS_dbh& s, SBDBHStartBlock& S)
{
  s >> S.DbName >> S.Date >> S.ExpDescr >> S.SessID >> S.Ver >> S.PrevDb >> S.Prev;
  if ((S.isOK=S.DbName.isOk() && S.Date.isOk() && S.ExpDescr.isOk() 
       && S.SessID.isOk() && S.Ver.isOk() && S.PrevDb.isOk() && S.Prev.isOk()))
    {
      S.DateVal[0]=S.Date[0];
      S.DateVal[1]=S.Date[1];
      S.DateVal[2]=S.Date[2];
      S.DateVal[3]=S.Date[3];
      S.DateVal[4]=S.Date[4];
    };
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHTFormat's implementation								*/
/*												*/
/*==============================================================================================*/
void SBDBHFormat::postRead()
{
  int		Offset;
  short		tcCount, teCount;
  SBDBHPar::PType PType=SBDBHPar::T_UNKN;

  tcCount=0;
  for (SBDBHTCBlock *tc=ListTCBlock.first(); tc; tc=ListTCBlock.next(), tcCount++)
    {
      int ParCount=0;
      if (tc->ParByAbbrev) delete tc->ParByAbbrev;
      for (SBDBHTEBlock* te=tc->teBlocks()->first(); te; te=tc->teBlocks()->next())
	ParCount+=te->parms()->count();
      tc->ParByAbbrev = new QDict<SBDBHPar>(ParCount);
      tc->ParByAbbrev->setAutoDelete(FALSE);

      teCount=0;
      for (SBDBHTEBlock* te=tc->teBlocks()->first(); te; te=tc->teBlocks()->next(), teCount++)
	{
	  SBDBHPhysRec *rec;
	  Offset=0;
	  if (te->RecByType) delete te->RecByType;
	  te->RecByType = new QIntDict<SBDBHPhysRec>;
	  te->RecByType->setAutoDelete(FALSE);
	  te->recs()->clear();
	  for (SBDBHPar* Par=te->parms()->first(); Par; Par=te->parms()->next())
	    {
	      if (Par->abbrev()=="R-FILLER")
		{
		  Offset=0;
		  te->recs()->append((rec = new SBDBHDRecT<double>));
		  te->recByType()->insert((PType=SBDBHPar::T_R8), rec);
		}
	      else if (Par->abbrev()=="I-FILLER")
		{
		  Offset=0;
		  te->recs()->append((rec = new SBDBHDRecT<short>));
		  te->recByType()->insert((PType=SBDBHPar::T_I2), rec);
		}
	      else if (Par->abbrev()=="A-FILLER")
		{
		  Offset=0;
		  te->recs()->append((rec = new SBDBHDRecString));
		  te->recByType()->insert((PType=SBDBHPar::T_A2), rec);
		}
	      else if (Par->abbrev()=="D-FILLER")
		{
		  Offset=0;
		  te->recs()->append((rec = new SBDBHDRecT<double>));
		  te->recByType()->insert((PType=SBDBHPar::T_D8), rec);
		}
	      else if (Par->abbrev()=="J-FILLER")
		{
		  Offset=0;
		  te->recs()->append((rec = new SBDBHDRecT<int>));
		  te->recByType()->insert((PType=SBDBHPar::T_J4), rec);
		}
	      else
		tc->ParByAbbrev->insert((const char*)Par->abbrev(), Par);
	      
	      Par->setOffset(Offset);
	      Par->setType(PType);
	      Par->setNTC(tcCount);
	      Par->setNTE(teCount);
	      Offset+=Par->dim1()*Par->dim2()*Par->dim3();
	    };
	  switch (te->recs()->count())
	    {
	    case 0:
	      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: record's order not parsed correctly");
	      break;
	    case 1 ... 4:
	      Log->write(SBLog::INF, SBLog::DATA, "SBDS_dbh: some records missed");
	      break;
	    case 5:
	      Log->write(SBLog::DBG, SBLog::DATA, "SBDS_dbh: record's order parsed correctly");
	      break;
	    default:
	      Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: too many records");
	    };
	};
    };
};

void SBDBHFormat::getBlock(SBDS_dbh& s)
{
  SBDBHLRecDR		DR;
  SBDBHLRecDE		DE;
  SBDBHPhysRec		*rec=NULL;
  SBDBHTCBlock		*tc=NULL;

  s >> DR;
  if (Dump) 
    {
      *Dump << "== Data Block dump: ==\n";
      DR.dump(*Dump);
    }
  if (!(isOK = isOK && DR.isOk())) 
    Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading DR record");
  else
    tc=ListTCBlock.at((TCcurrent = DR.p1()-1)); // is it right???????????????????????????? 
  
  while (!DE.isZ3() && isOK)
    {
      s >> DE;
      if (!(isOK = isOK && DE.isOk()))
	Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading DE record");
      if (Dump && isOK) DE.dump(*Dump);
      if (DE.isP() && isOK)
	{
	  SBDBHTEBlock *te=tc->teBlocks()->at(DE.p1()-1); //???
	  for (SBDBHPhysRec *rec=te->recs()->first(); rec; rec=te->recs()->next())
	    {
	      s>>*rec;
	      if (!(isOK = isOK && rec->isOk())) break;
	    };
	};
    }; 
  if (!isOK)
    Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: error reading data record");
  if (Dump && isOK)
    {
      *Dump << "== TC Data Block dump: ==\n";
      for (SBDBHTEBlock* te=tc->teBlocks()->first(); te; te=tc->teBlocks()->next())
	{
	  *Dump << "== TE Data Block dump: ==\n";
	  te->Rec1.dump(*Dump);
	  for (SBDBHPar* Par=te->parms()->first(); Par; Par=te->parms()->next())
	    {
	      if ((rec = te->recByType()->find(Par->type())))
		{
		  *Dump << "\"" << Par->abbrev() << "\" (aka \"" 
			<< Par->descr().simplifyWhiteSpace() << "\") [" 
			<< Par->dim1() << "," << Par->dim2() << "," << Par->dim3() << "] =\n(\n";
		  for (int li=0; li<Par->dim3(); li++)
		    {
		      *Dump << "   (";
		      for (int lj=0; lj<Par->dim2(); lj++)
			{
			  if (Par->type()==SBDBHPar::T_A2)
			    *Dump << "\"" << ((SBDBHDRecString*)rec)->val(Par, lj, li) << "\"";
			  else
			    {
			      *Dump << "(";
			      for (int lk=0; lk<Par->dim1(); lk++)
				{
				  switch(Par->type())
				    {
				    case SBDBHPar::T_R8:
				      *Dump << ((SBDBHDRecT<double>*)rec)->val(Par, lk, lj, li);
				      break;
				    case SBDBHPar::T_I2:
				      *Dump << ((SBDBHDRecT<short>*)rec)->val(Par, lk, lj, li);
				      break;
				    case SBDBHPar::T_D8:
				      *Dump << ((SBDBHDRecT<double>*)rec)->val(Par, lk, lj, li);
				      break;
				    case SBDBHPar::T_J4:
				      *Dump << ((SBDBHDRecT<int>*)rec)->val(Par, lk, lj, li);
				      break;
				    default: *Dump << "????";
				    };
				  *Dump << (lk<Par->dim1()-1?", ":"");
				};
			      *Dump << ")";
			    };
			  *Dump << (lj<Par->dim2()-1?", ":"");
			};
		      *Dump << ")" << (li<Par->dim3()-1?",":"") << "\n";
		    };
		  *Dump << ");\n";
		}
	      else 
		Log->write(SBLog::ERR, SBLog::DATA, "SBDS_dbh: can't find record for `" 
			   + Par->abbrev() + "' parameter");
	    };
	};
      *Dump << "==\n";
    };
};
SBDBHPar *SBDBHFormat::lookupPar(const char* Tag)
{
  SBDBHPar *Par = currentTC()->parByAbbrev()->find(Tag);
  SBDBHTEBlock *te = NULL;
  return (Par) && Par->nTC()==TCcurrent && (te=currentTC()->teBlocks()->at(Par->nTE())) &&
    (te->recByType()->find(Par->type()))?Par:NULL;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHTFormat's friends implementation							*/
/*												*/
/*==============================================================================================*/
SBDS_dbh &operator>>(SBDS_dbh& s, SBDBHFormat& F)
{
  SBDBHTCBlock	*tc;
  do
    {
      tc = new SBDBHTCBlock;
      s >> *tc;
      if (!tc->isLast() && (F.isOK = F.isOK && tc->isOk())) F.ListTCBlock.append(tc);
      else delete tc;
    } while (tc && !tc->isLast() && F.isOK);
  F.postRead();
  return s;
};
/*==============================================================================================*/
