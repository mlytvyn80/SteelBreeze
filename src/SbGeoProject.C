/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 1998-2004 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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


#include <qapplication.h>
#include <qdatetime.h>
#include <qdir.h> 
#include <qfile.h> 
#include <qfileinfo.h> 
#include <qmessagebox.h>
#include <qregexp.h> 

#include "SbGeoAtmLoad.H"
#include "SbGeoEstimator.H"
#include "SbGeoParameter.H"
#include "SbGeoEphem.H"
#include "SbGeoProject.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"
#include "SbGeoRefFrame.H"
#include "SbGeoEop.H"
#include "SbSetup.H"
#include "SbGeoDelay.H"
#include "SbGeoTide.H"
#include "SbGeoRefraction.H"


const SBConfig	DefaultConfig;
SBConfig	Config;



/*==============================================================================================*/
/*												*/
/* class SBConfig implementation								*/
/*												*/
/*==============================================================================================*/
/** A constructor.
 * Creates a copy of object.
 */
SBConfig::SBConfig()
  : P()
{
  IsChanged		= FALSE;

  //System Transformation:
  SysTransform		= STR_Classic;

  //Light propagation/Ephemerides:
  EphemModel		= EM_LEDE405;
  IsGravDelayHO		= FALSE;
  GravHOThreshold	= 5.0*DEG2RAD;
  for (int i=0; i<11; i++) IsEphBody[i] = TRUE;

  //Solid Tide Loading:
  SolidTideLd		= ST_IERS96;
  SolidTideDegree	= STDEGREE_3;
  IsSolidTideAnelastic	= TRUE;
  IsSolidTideLatDepend	= TRUE;
  IsRemovePermTide	= FALSE;

  //EOP:
  EOPDiurnal		= EDM_Ray1995;
  EOPDiurnalAtm		= EDMA_NONE;
  EOPDiurnalNonTidal	= EDMNT_NONE;
  EOPNumIntrplPoints	= 8;
  IsEOP_IAU2K_XY	= FALSE;
  IsEOPResrerved_1	= FALSE;

  //Nutation:
  NutModel		= NM_IAU1980;
  NutEquinox		= NEE_IERS96;
  IsNutGeodNut		= TRUE;

  //Station motion:
  PlateMotion		= PM_NNR_NUVEL1A;
  IsUseStationVelocity	= FALSE;
  IsRollBack2Plates	= FALSE;
  IsUseAploEphem	= FALSE;

  //Refraction:
  RefrDryZenith		= RDZ_Saastamoinen;
  RefrWetZenith		= RWZ_Saastamoinen;
  RefrDryMapping	= RDM_MTT;
  RefrWetMapping	= RWM_MTT;
  IsAltAxsCorr		= TRUE;
  IsRollBack2NMF	= TRUE;

  //Data processing:
  QCodeThreshold	= 3;
  DUFlagThreshold	= -1;
  IsUseDelay		= TRUE;
  IsUseRate		= FALSE;
  IsUseBadIon		= FALSE;

  //Solution reports:
  ReportMaps		= TRUE;
  ReportCRF_Changes	= FALSE;
  ReportTRF_Changes	= FALSE;
  ReportCRF		= FALSE;
  ReportTRF_SINEX	= FALSE;
  ReportEOP		= FALSE;
  ReportTechDesc	= FALSE;
  ReportNormalEqs	= FALSE;
  ReportNormalEqsSRCs	= FALSE;
  ReportNormalEqsSTCs	= TRUE;
  ReportNormalEqsEOPs	= TRUE;
  ReportTroposhere	= FALSE;
  TropSamplInterval	= 3600.0;
  TropStartSec		= 64800;

  //Alternative DB:
  AltStations		= "";
  AltSources		= "";
  AltEOPs		= "";
  UseAltStations	= FALSE;
  UseAltSources		= FALSE;
  UseAltEOPs		= FALSE;

  //Run-Time options:
  IsGenerateDSINEXes	= FALSE;
  
};

SBConfig& SBConfig::operator= (const SBConfig& C)
{
  IsChanged		= TRUE;
  P			= C.P;

  //System Transformation:
  SysTransform		= C.SysTransform;

  //Light propagation/Ephemerides:
  EphemModel		= C.EphemModel;
  IsGravDelayHO		= C.IsGravDelayHO;
  GravHOThreshold	= C.GravHOThreshold;
  for (int i=0; i<11; i++) IsEphBody[i] = C.IsEphBody[i];

  //Tides:
  SolidTideLd		= C.SolidTideLd;
  SolidTideDegree	= C.SolidTideDegree;
  IsSolidTideAnelastic	= C.IsSolidTideAnelastic;
  IsSolidTideLatDepend	= C.IsSolidTideLatDepend;
  IsRemovePermTide	= C.IsRemovePermTide;

  //EOP:
  EOPDiurnal		= C.EOPDiurnal;
  EOPDiurnalAtm		= C.EOPDiurnalAtm;
  EOPDiurnalNonTidal	= C.EOPDiurnalNonTidal;
  EOPNumIntrplPoints	= C.EOPNumIntrplPoints;
  IsEOP_IAU2K_XY	= C.IsEOP_IAU2K_XY;
  IsEOPResrerved_1	= C.IsEOPResrerved_1;

  //Nutation:
  NutModel		= C.NutModel;
  NutEquinox		= C.NutEquinox;
  IsNutGeodNut		= C.IsNutGeodNut;

  //Station motion:
  PlateMotion		= C.PlateMotion;
  IsUseStationVelocity	= C.IsUseStationVelocity;
  IsRollBack2Plates	= C.IsRollBack2Plates;
  IsUseAploEphem	= C.IsUseAploEphem;

  //Refraction:
  RefrDryZenith		= C.RefrDryZenith;
  RefrWetZenith		= C.RefrWetZenith;
  RefrDryMapping	= C.RefrDryMapping;
  RefrWetMapping	= C.RefrWetMapping;
  IsAltAxsCorr		= C.IsAltAxsCorr;
  IsRollBack2NMF	= C.IsRollBack2NMF;

  //Data processing:
  QCodeThreshold	= C.QCodeThreshold;
  DUFlagThreshold	= C.DUFlagThreshold;
  IsUseDelay		= C.IsUseDelay;
  IsUseRate		= C.IsUseRate;
  IsUseBadIon		= C.IsUseBadIon;

  //Solution reports:
  ReportMaps		= C.ReportMaps;
  ReportCRF_Changes	= C.ReportCRF_Changes;
  ReportTRF_Changes	= C.ReportTRF_Changes;
  ReportCRF		= C.ReportCRF;
  ReportTRF_SINEX	= C.ReportTRF_SINEX;
  ReportEOP		= C.ReportEOP;
  ReportTechDesc	= C.ReportTechDesc;
  ReportNormalEqs	= C.ReportNormalEqs;
  ReportNormalEqsSRCs	= C.ReportNormalEqsSRCs;
  ReportNormalEqsSTCs	= C.ReportNormalEqsSTCs;
  ReportNormalEqsEOPs	= C.ReportNormalEqsEOPs;
  ReportTroposhere	= C.ReportTroposhere;
  TropSamplInterval	= C.TropSamplInterval;
  TropStartSec		= C.TropStartSec;

  //Alternative DB:
  AltStations		= C.AltStations.copy();
  AltSources		= C.AltSources.copy();
  AltEOPs		= C.AltEOPs.copy();
  UseAltStations	= C.UseAltStations;
  UseAltSources		= C.UseAltSources;
  UseAltEOPs		= C.UseAltEOPs;

  //Run-Time options:
  IsGenerateDSINEXes	= C.IsGenerateDSINEXes;

  return *this;
};

void SBConfig::setAllPars2Type(SBParameterCfg::PType Type)
{
  for (int i=0; i<P.num(); i++)
    P.setType(i, Type);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBConfig's friends implementation							*/
/*												*/
/*==============================================================================================*/
/**\relates SBConfig
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBConfig& C)
{
  //Light propagation/Ephemerides:
  for (int i=0; i<11; i++) s << (uint)C.IsEphBody[i];

  //  System transformation:
  s << (uint)C.SysTransform
    
    //Light propagation/Ephemerides:
    << (uint)C.EphemModel << (uint)C.IsGravDelayHO << C.GravHOThreshold
    
    //Solid Tide Loading:
    << (uint)C.SolidTideLd << (uint)C.SolidTideDegree << (uint)C.IsSolidTideAnelastic 
    << (uint)C.IsSolidTideLatDepend << (uint)C.IsRemovePermTide 
    
    //EOP:
    << (uint)C.EOPDiurnal << (uint)C.EOPDiurnalAtm << (uint)C.EOPDiurnalNonTidal
    << C.EOPNumIntrplPoints << (uint)C.IsEOP_IAU2K_XY << (uint)C.IsEOPResrerved_1
    
    //Nutation:
    << (uint)C.NutModel << (uint)C.NutEquinox << (uint)C.IsNutGeodNut
    
    //Station motion:
    << (uint)C.PlateMotion << (uint)C.IsUseStationVelocity 
    << (uint)C.IsRollBack2Plates << (uint)C.IsUseAploEphem
    
    //Refraction:
    << (uint)C.RefrDryZenith << (uint)C.RefrWetZenith 
    << (uint)C.RefrDryMapping << (uint)C.RefrWetMapping 
    << (uint)C.IsAltAxsCorr << (uint)C.IsRollBack2NMF
    
    //Data processing:
    << C.QCodeThreshold << C.DUFlagThreshold
    << (uint)C.IsUseDelay << (uint)C.IsUseRate << (uint)C.IsUseBadIon
    
    //Solution reports:
    << (uint)C.ReportMaps << (uint)C.ReportCRF_Changes 
    << (uint)C.ReportTRF_Changes  << (uint)C.ReportCRF
    << (uint)C.ReportTRF_SINEX << (uint)C.ReportEOP << (uint)C.ReportTechDesc
    << (uint)C.ReportNormalEqs
    << (uint)C.ReportTroposhere << C.TropSamplInterval << C.TropStartSec

    //Alternative DB:
    << C.AltStations << C.AltSources << C.AltEOPs
    << (uint)C.UseAltStations << (uint)C.UseAltSources << (uint)C.UseAltEOPs

    << (uint)C.IsGenerateDSINEXes

    // Parameters:
    << C.P;

  // addings:
  s << (uint)C.ReportNormalEqsSRCs << (uint)C.ReportNormalEqsSTCs << (uint)C.ReportNormalEqsEOPs;
  
  //  std::cout << "sizeof(uint): " << sizeof(uint) << "\n";
  //  std::cout << "sizeof(bool): " << sizeof(bool) << "\n";
  return s;
};

/**\relates SBConfig
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBConfig& C)
{
  uint Is1, Is2, Is3, Is4;
  //Light propagation/Ephemerides:
  for (int i=0; i<11; i++) s >> (uint&)C.IsEphBody[i];
  
  //  System transformation:
  s >> (uint&)C.SysTransform

    //Light propagation/Ephemerides:
    >> (uint&)C.EphemModel >> (uint&)C.IsGravDelayHO >> C.GravHOThreshold

    //Solid Tide Loading:
    >> (uint&)C.SolidTideLd >> (uint&)C.SolidTideDegree >> (uint&)C.IsSolidTideAnelastic 
    >> (uint&)C.IsSolidTideLatDepend >> (uint&)C.IsRemovePermTide 

    //EOP:
    >> (uint&)C.EOPDiurnal >> (uint&)C.EOPDiurnalAtm >> (uint&)C.EOPDiurnalNonTidal
    >> C.EOPNumIntrplPoints >> (uint&)C.IsEOP_IAU2K_XY >> (uint&)C.IsEOPResrerved_1

    //Nutation:
    >> (uint&)C.NutModel >> (uint&)C.NutEquinox >> (uint&)C.IsNutGeodNut

    //Station motion:
    >> (uint&)C.PlateMotion >> (uint&)C.IsUseStationVelocity 
    >> (uint&)C.IsRollBack2Plates >> (uint&)C.IsUseAploEphem

    //Refraction:
    >> (uint&)C.RefrDryZenith >> (uint&)C.RefrWetZenith 
    >> (uint&)C.RefrDryMapping >> (uint&)C.RefrWetMapping 
    >> (uint&)C.IsAltAxsCorr >> (uint&)C.IsRollBack2NMF

    //Data processing:
    >> C.QCodeThreshold >> C.DUFlagThreshold
    >> (uint&)C.IsUseDelay >> (uint&)C.IsUseRate >> (uint&)C.IsUseBadIon

    //Solution reports:
    >> (uint&)C.ReportMaps >> (uint&)C.ReportCRF_Changes 
    >> (uint&)C.ReportTRF_Changes >> (uint&)C.ReportCRF
    >> (uint&)C.ReportTRF_SINEX >> (uint&)C.ReportEOP >> (uint&)C.ReportTechDesc 
    >> (uint&)C.ReportNormalEqs
    >> (uint&)C.ReportTroposhere >> C.TropSamplInterval >> C.TropStartSec;

  //Alternative DB:
  s >> C.AltStations >> C.AltSources >> C.AltEOPs
    >> Is1 >> Is2 >> Is3 >> Is4
    
    // Parameters:
    >> C.P;
  
  //FIX ME
  C.UseAltStations = (bool)Is1;
  C.UseAltSources  = (bool)Is2;
  C.UseAltEOPs     = (bool)Is3;
  C.IsGenerateDSINEXes = (bool)Is4;

  
  // addings:
  if (!s.atEnd())
    {
      s >> Is1 >> Is2 >> Is3;
    };
  C.ReportNormalEqsSRCs = (bool)Is1;
  C.ReportNormalEqsSTCs = (bool)Is2;
  C.ReportNormalEqsEOPs = (bool)Is3;

  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBProject implementation								*/
/*												*/
/*==============================================================================================*/
SBProject::SBProject(const QString& Name_) : SBNamed(Name_), Cfg(), Comments("")
{
  VLBISessionList.setAutoDelete(TRUE);
  StationList	 .setAutoDelete(TRUE);
  BaseList	 .setAutoDelete(TRUE);
  SourceList	 .setAutoDelete(TRUE);
  Version = 1;
  NumObs  = 0;
  
  SessionByName = new QDict<SBVLBISesInfo>(30);
  SessionByName-> setAutoDelete(FALSE);
  
  StationByAka = new QDict<SBStationInfo>(20);
  StationByAka-> setAutoDelete(FALSE);

  BaseByAka = new QDict<SBBaseInfo>(40);
  BaseByAka-> setAutoDelete(FALSE);
  
  SourceByAka  = new QDict<SBSourceInfo>(60);
  SourceByAka -> setAutoDelete(FALSE);
};

SBProject::~SBProject()
{
  if (SessionByName) delete SessionByName;
  if (StationByAka) delete StationByAka;
  if (BaseByAka) delete BaseByAka;
  if (SourceByAka) delete SourceByAka;
};

void SBProject::fillSessionDict(int N)
{
  if (SessionByName) delete SessionByName;
  SessionByName = new QDict<SBVLBISesInfo>(VLBISessionList.count() + N);
  SessionByName -> setAutoDelete(FALSE);
  for (SBVLBISesInfo *si=VLBISessionList.first(); si; si=VLBISessionList.next())
    if (si && SessionByName->find((const char*)si->name()))
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate name for session [" 
		 + si->name() + "]");
    else 
      {
	SessionByName->insert((const char*)si->name(), si);
	NumObs+=si->numObs();
      };
};

void SBProject::fillStationDict(int N)
{
  if (StationByAka) delete StationByAka;
  StationByAka = new QDict<SBStationInfo>(StationList.count() + N);
  StationByAka-> setAutoDelete(FALSE);
  for (SBStationInfo *st=StationList.first(); st; st=StationList.next())
    if (st && StationByAka->find((const char*)st->aka()))
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate aka `" + 
		 st->aka() + "' for station [" + st->toString() + "]");
    else 
      StationByAka->insert((const char*)st->aka(), st);
};

void SBProject::fillSourceDict(int N)
{
  if (SourceByAka) delete SourceByAka;
  SourceByAka = new QDict<SBSourceInfo>(SourceList.count() + N);
  SourceByAka-> setAutoDelete(FALSE);
  for (SBSourceInfo *so=SourceList.first(); so; so=SourceList.next())
    if (so && SourceByAka->find((const char*)so->aka()))
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate aka `" + 
		 so->aka() + "' for source [" + so->name() + "]");
    else 
      SourceByAka->insert((const char*)so->aka(), so);
};

void SBProject::fillBaseDict(int N)
{
  if (BaseByAka) delete BaseByAka;
  BaseByAka = new QDict<SBBaseInfo>(BaseList.count() + N);
  BaseByAka-> setAutoDelete(FALSE);
  for (SBBaseInfo *b=BaseList.first(); b; b=BaseList.next())
    if (b && BaseByAka->find((const char*)b->aka()))
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate aka `" + 
		 b->aka() + "' for base [" + b->name() + "]");
    else 
      BaseByAka->insert((const char*)b->aka(), b);
};

bool SBProject::addSession(const SBVLBISet *Wvlbi, const SBVLBISesInfo *SI)
{
  if (SessionByName->find(SI->name()))
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": session [" + SI->name()
		 + "] already exist in the project");
      return FALSE;
    };

  SBVLBISesInfo *SessInfo;
  SBStationInfo *st_proj;
  SBSourceInfo	*so_proj;
  SBBaseInfo	*b_proj;

  //this takes a time
  SBVLBISession *Session = Wvlbi->loadSession(SI->name());
  
  VLBISessionList.inSort(SessInfo = new SBVLBISesInfo(*Session));
  SessionByName->insert((const char*)SessInfo->name(), SessInfo);
  SessInfo->delAttr(SBVLBISesInfo::notValid);
  SessInfo->clearRMSs();

  for (SBStationInfo *st=Session->stationList()->first(); st; st=Session->stationList()->next())
    if (st && (st_proj=StationByAka->find((const char*)st->aka()))) //separate stations by aka
      st_proj->setNum(st_proj->num() + st->num());
    else 
      {
	st_proj = new SBStationInfo(*st, StationList.count(), st->aka());
	st_proj->setNum(st->num());
	StationList.inSort(st_proj);
	StationByAka->insert((const char*)st_proj->aka(), st_proj);
      };
  
  for (SBSourceInfo *so=Session->sourceList()->first(); so; so=Session->sourceList()->next())
    if (so && (so_proj=SourceByAka->find((const char*)so->aka()))) //separate sources by aka
      so_proj->setNum(so_proj->num() + so->num());
    else 
      {
	so_proj = new SBSourceInfo(so->name(), SourceList.count(), so->aka());
	so_proj->setNum(so->num());
	SourceList.inSort(so_proj);
	SourceByAka->insert((const char*)so_proj->aka(), so_proj);
      };
  
  for (SBBaseInfo *b=Session->baseList()->first(); b; b=Session->baseList()->next())
    if (b && (b_proj=BaseByAka->find((const char*)b->aka()))) //separate bases by aka
      b_proj->setNum(b_proj->num() + b->num());
    else 
      {
	b_proj = new SBBaseInfo(*b, BaseList.count());
	b_proj->setNum(b->num());
	BaseList.inSort(b_proj);
	BaseByAka->insert((const char*)b_proj->aka(), b_proj);
      };
  
  delete Session;

  if (SessionByName->size()<=SessionByName->count()) fillSessionDict(20);
  if (StationByAka ->size()<=StationByAka ->count()) fillStationDict(20);
  if (SourceByAka  ->size()<=SourceByAka  ->count()) fillSourceDict(20);
  if (BaseByAka    ->size()<=BaseByAka    ->count()) fillBaseDict(20);

  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": session [" + SI->name() +
	     "] imported to the project '" + name() + "'");


  return TRUE;
};

bool SBProject::delSession(const SBVLBISet *Wvlbi, const SBVLBISesInfo *SI)
{
  if (!SessionByName->find(SI->name()))
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": session [" + SI->name()
		 + "] does not exist in the project");
      return FALSE;
    };

  SBStationInfo *st_proj;
  SBSourceInfo	*so_proj;
  SBBaseInfo	*b_proj;
  QString	SessName=SI->name();

  //this takes a time
  SBVLBISession *Session = Wvlbi->loadSession(SI->name());
  
  // auxiliary lists:
  for (SBStationInfo *st=Session->stationList()->first(); st; st=Session->stationList()->next())
    if (st && (st_proj=StationByAka->find((const char*)st->aka()))) //separate stations by aka
      {
	st_proj->setNum(st_proj->num() - st->num());
	if (!st_proj->num())
	  {
	    if (!StationByAka->remove((const char*)st_proj->aka()))
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove station [" + 
			 st_proj->aka() + "] from the station dict of the project");
	    if (!StationList.remove(st_proj))
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove station [" + 
			 st_proj->aka() + "] from the station list of the project");
	  };
      }
    else 
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot find station [" + 
		 st->aka() + "] in the project");
  
  for (SBSourceInfo *so=Session->sourceList()->first(); so; so=Session->sourceList()->next())
    if (so && (so_proj=SourceByAka->find((const char*)so->aka()))) //separate sources by aka
      {
	so_proj->setNum(so_proj->num() - so->num());
	if (!so_proj->num())
	  {
	    if (!SourceByAka->remove((const char*)so_proj->aka()))
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove source [" + 
			 so_proj->aka() + "] from the source dict of the project");
	    if (!SourceList.remove(so_proj))
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove source [" + 
			 so_proj->aka() + "] from the source list of the project");
	  };
      }
    else 
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot find source [" + 
		 so->aka() + "] in the project ");
  
  for (SBBaseInfo *b=Session->baseList()->first(); b; b=Session->baseList()->next())
    if (b && (b_proj=BaseByAka->find((const char*)b->aka()))) //separate bases by aka
      {
	b_proj->setNum(b_proj->num() - b->num());
	if (!b_proj->num())
	  {
	    if (!BaseByAka->remove((const char*)b_proj->aka()))
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove base [" + 
			 b_proj->aka() + "] from the base dict of the project");
	    if (!BaseList.remove(b_proj))
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove base [" + 
			 b_proj->aka() + "] from the base list of the project");
	  };
      }
    else 
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot find base [" + 
		 b->aka() + "] in the project ");

  if (!SessionByName->remove((const char*)SI->name()))
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove session [" + 
		 SI->name() + "] from the session dict of the project");
      delete Session;
      return FALSE;
    };

  if (!VLBISessionList.remove(SI))
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot remove session [" + 
		 SI->name() + "] from the session list of the project");
      delete Session;
      return FALSE;
    };
  
  delete Session;

  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": session [" + SessName +
	     "] removed from the project '" + name() + "'");

  return TRUE;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBProject's friends implementation							*/
/*												*/
/*==============================================================================================*/
/**\relates SBProject
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBProject& P)
{
  return s << (const SBNamed&)P << P.Version << P.History << P.Comments
	   << P.VLBISessionList << P.StationList << P.SourceList << P.BaseList << P.Cfg;
};

/**\relates SBProject
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBProject& P)
{
  s >> (SBNamed&)P >> P.Version >> P.History >> P.Comments
    >> P.VLBISessionList >> P.StationList >> P.SourceList >> P.BaseList >> P.Cfg;

  P.fillDicts();
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBRunManager implementation								*/
/*												*/
/*==============================================================================================*/
SBRunManager::SBRunManager(SBProject* Prj_, SBVLBISet* VLBI_, SBSolution* Solution_)
{
  // inits:
  if( (Prj = Prj_) )
    IsPrjOwner = FALSE;
  else 
    IsPrjOwner = TRUE;

  if( (Solution = Solution_) )
    IsSolutionOwner = FALSE;
  else 
    IsSolutionOwner = TRUE;

  Estimator	= NULL;
  ObsIt		= NULL;
  TRF		= NULL;
  Plates	= NULL;
  RefPlates	= NULL;
  Frame		= NULL;
  Ephem		= NULL;
  AploEphem	= NULL;

  if ( (VLBI=VLBI_) )
    IsVLBIOwner = FALSE;
  else
    IsVLBIOwner	= TRUE;

  CRF		= NULL;
  VLBISessIt	= NULL;
  TideLd	= NULL;
  Refr		= NULL;
  Delay		= NULL;
  AllGlobalList	= NULL;
  GlobalList	= NULL;
  ArcList	= NULL;
  LocalList	= NULL;
  StochList	= NULL;
  T_2load = T_2save = Tb = Te = TZero;
  prcsVLBISess	= 0;

  // creating lists:
  ObsList.setAutoDelete(FALSE);		//just a list of pointers, not an owner.
  if (ObsList.count()) 
    ObsList.clear();
  VLBISessCache.setAutoDelete(TRUE);
  if (VLBISessCache.count()) 
    VLBISessCache.clear();
  VLBISessions.setAutoDelete(FALSE);
  if (VLBISessions.count()) 
    VLBISessions.clear();

  // preparing the stuff:
  if (!Prj)
    {
      mainSetOperation("Loading Project");
      Prj = SetUp->loadProject();
      IsPrjOwner = TRUE; // have to delete the Prj
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + 
		 ": loaded current project [" + Prj->name() + "]");
    };
  // initial values:
  Tb=Prj->tStart();
  Te=Prj->tFinis() + 1.0; // the date that greater than the last observation.
  
  if (!TRF)
    {
      mainSetOperation("Loading TRF database");
      TRF = SetUp->loadTRF();
      stripTRF();
      TRF->prepare4Run();
      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + 
		 ": TRF contained a list of " + QString("_").setNum(TRF->stations()->count()) +
		 " stations w/ parameters");
    };

  if (!Plates)
    {
      mainSetOperation("Creating Tectonic Plate Motion model");
      switch (prj()->cfg().plateMotion())
	{
	default:
	case SBConfig::PM_NONE:		Plates  = new SBPlateMotion; break;
	case SBConfig::PM_NNR_NUVEL1A:	Plates  = new SBPMNNR_NUVEL1A; break;
	case SBConfig::PM_NUVEL1A:	Plates  = new SBPMNUVEL1A; break; 
	};
    };
  if (!RefPlates)
    {
      mainSetOperation("Creating Reference model of Tectonic Plate Motion");
      RefPlates = new SBPMNNR_NUVEL1A;
    };

  if (!CRF)
    {
      mainSetOperation("Loading CRF database");
      CRF = SetUp->loadCRF();
      stripCRF();
      CRF->prepare4Run();
      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + 
		 ": CRF contained a list of " + QString("_").setNum(CRF->count()) +
		 " sources w/ parameters");
    };

  if (!Frame)
    {
      mainSetOperation("Creating Reference Frame Transformer");
      switch (prj()->cfg().sysTransform())
	{
	default:
	case SBConfig::STR_Classic:	Frame = new SBFrameClassic(&prj()->cfg(), this); break;
	case SBConfig::STR_NRO:		Frame = new SBFrameNRO(&prj()->cfg(), this); break; 
	};
      Frame->eop()->createParameters();
    };

  if (!Ephem)
    {
      mainSetOperation("Loading Ephemeris");
      switch (prj()->cfg().ephemModel())
	{
	case SBConfig::EM_LEDE200: Ephem = new SBEphemDE200(&prj()->cfg()); break;
	default:
	case SBConfig::EM_LEDE403: Ephem = new SBEphemDE403(&prj()->cfg()); break;
	case SBConfig::EM_LEDE405: Ephem = new SBEphemDE405(&prj()->cfg()); break;
	};
    };
  
  if (!AploEphem && prj()->cfg().isUseAploEphem())
    {
      if (!(AploEphem=SetUp->loadAploEphem()))
	AploEphem = new SBAploEphem;
    };

  if (!TideLd)
    {
      mainSetOperation("Creating Tide Loader");
      TideLd = new SBTideLd(&Prj->cfg());
    };

  if (!Refr)
    {
      mainSetOperation("Creating Refraction");
      Refr = new SBRefraction(&Prj->cfg());
    };

  if (!Delay)
    {
      mainSetOperation("Creating Delay calculaor");
      Delay = new SBDelay(&Prj->cfg(), Frame, Ephem, TideLd, Refr);
    };
  
  if (!Solution)
    {
      mainSetOperation("Creating Solution");
      Solution = new SBSolution(Prj);
      IsSolutionOwner = TRUE;
    };

  if (!Estimator)
    {
      mainSetOperation("Creating Estimator");
      Estimator = new SBEstimator(Prj, this, Solution);
    };
};

SBRunManager::~SBRunManager()
{
  if (Plates)
    {
      delete Plates;
      Plates = NULL;
    };
  if (RefPlates)
    {
      delete RefPlates;
      RefPlates = NULL;
    };
  if (TRF)
    {
      TRF->finisRun();
      delete TRF;
      TRF = NULL;
    };
  if (CRF)
    {
      CRF->finisRun();
      delete CRF;
      CRF = NULL;
    };
  if (Frame)     
    {
      Frame->eop()->releaseParameters();
      delete Frame;
      Frame = NULL;
    };
  if (Ephem) 
    {
      delete Ephem;
      Ephem = NULL;
    };
  if (AploEphem) 
    {
      delete AploEphem;
      AploEphem = NULL;
    };
  if (TideLd) 
    {
      delete TideLd;
      TideLd = NULL;
    };
  if (Refr)
    {
      delete Refr;
      Refr = NULL;
    };
  if (Delay)
    {
      delete Delay;
      Delay = NULL;
    };
  if (Estimator) 
    {
      delete Estimator;
      Estimator = NULL;
    };
  if (IsSolutionOwner && Solution)
    {
      delete Solution;
      Solution = NULL;
    };

  if (IsVLBIOwner && VLBI) 
    {
      delete VLBI;
      VLBI = NULL;
    };
  if (ObsIt) 
    {
      delete ObsIt;
      ObsIt = NULL;
    };
  if (VLBISessIt) 
    {
      delete VLBISessIt;
      VLBISessIt = NULL;
    };

  if (IsPrjOwner && Prj ) delete Prj;
  if (AllGlobalList) delete AllGlobalList;
  if (GlobalList) delete GlobalList;
  if (ArcList) delete ArcList;
  if (LocalList) delete LocalList;
  if (StochList) delete StochList;
};

// remove unused radiosources
void SBRunManager::stripCRF()
{
  int	NRm=0;
  for (uint i=0; i<CRF->count(); i++)
    if (!Prj->sourceList()->find(CRF->at(i)))
      {
	if (CRF->remove(i))
	  {
	    NRm++;
	    i--;
	  }
	else
	  Log->write(SBLog::ERR, SBLog::SOURCE, ClassName() + ": cannot remove the source [" + 
		     CRF->at(i)->name() + "] from the catalogue");
      };
  Log->write(SBLog::DBG, SBLog::SOURCE, ClassName() + ": cleaned CRF, " + QString("_").setNum(NRm) +
	     " sources freed");
};

// remove unused stations
void SBRunManager::stripTRF()
{
  int	NRmSi=0;
  int	NRmSt=0;
  SBSite	*Site=NULL;
  SBStation	*Station=NULL;
  SBStationID	id;
  for (uint i=0; i<TRF->count(); i++)
    for (uint j=0; j<(Site=TRF->at(i))->stations().count(); j++)
      {
	Station=Site->stations().at(j);
	if (!Prj->stationList()->find(&(id=Station->id())))
	  {
	    if (Site->delStation(Station))
	      {
		NRmSt++;
		j--;
	      }
	    else
	      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot remove the station [" + 
			 Station->name() + "] from the catalogue");
	  }
      };
  for (uint i=0; i<TRF->count(); i++)
    if (!(Site=TRF->at(i))->stations().count())
      {
	if (TRF->remove(Site->domeMajor()))
	  {
	    NRmSi++;
	    i--;
	  }
	else
	  Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot remove the site [" + 
		     Site->name() + "] from the catalogue");
      }
  Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": cleaned TRF, " + QString("_").setNum(NRmSi) +
	     " sites freed, " + QString("_").setNum(NRmSt) + " stations freed");
};

QString	SBRunManager::listOfCurrentSessNames()
{
  QString Q("<none>");
  if (VLBISessCache.count())
    {
      Q="";
      for (SBVLBISession *S=VLBISessCache.first(); S; S=VLBISessCache.next())
	Q+=S->name() + ", ";
      Q=Q.left(Q.length()-2);
    };
  return Q;
};

bool SBRunManager::isObsEligible(SBObservation* Obs)
{
  bool		IsEligible = Obs->isEligible(this);

  return IsEligible;
};

void SBRunManager::constrainClocks(bool IsGlobal)
{
  double	sigC0 = 1.0e-16;
  double	sigC1 = 1.0e-17;
  double	sigC2 = 1.0e-18;
  for (SBVLBISession *S=VLBISessCache.first(); S; S=VLBISessCache.next())
    {
      int NumOfConstr = 0;
      for (SBStationInfo *SI=S->stationList()->first(); SI; SI=S->stationList()->next())
	{
	  if (SI->isAttr(SBStationInfo::refClock))
	    {
	      SBStation		*Station;
	      if ((Station=TRF->find(*SI)))
		{
		  bool isDone = FALSE;
		  if (Prj->cfg().p().clock0().type() != SBParameterCfg::PT_NONE)
		    {
		      Station->p_C0()->setD(1.0, TZero);
		      if (IsGlobal)
			Estimator->processGlobalConstr(0.0, sigC0);
		      else
			Estimator->processConstr(0.0, sigC0);
		      isDone = TRUE;
		    };
		  if (Prj->cfg().p().clock1().type() != SBParameterCfg::PT_NONE)
		    {
		      Station->p_C1()->setD( 1.0,	TZero);
		      if (IsGlobal)
			Estimator->processGlobalConstr(0.0, sigC1);
		      else
			Estimator->processConstr(0.0, sigC1);
		      isDone = TRUE;
		    };
		  if (Prj->cfg().p().clock2().type() != SBParameterCfg::PT_NONE)
		    {
		      Station->p_C2()    ->setD( 1.0,	TZero);
		      if (IsGlobal)
			Estimator->processGlobalConstr(0.0, sigC2);
		      else
			Estimator->processConstr(0.0, sigC2);
		      isDone = TRUE;
		    };
		  if (isDone)
		    {
		      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Clocks of station " + 
				 SI->aka() + " [" + SI->toString() + "] have been constrained");
		      NumOfConstr++;
		    };
		}
	      else
		Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find the station " + SI->aka() +
			   " [" + SI->toString() + "] in the TRF, constrains ignored");
	    };
	};
      if (!NumOfConstr)
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + 
		   ": ERROR: there is no reference clocks on the session " + " [" + S->name() + 
		   "], the solution will fail");
      else if (NumOfConstr>1)
	Log->write(SBLog::WRN, SBLog::RUN, ClassName() + 
		   ": WARNING: more than one station used as a `reference clocks` on the session " +
		   " [" + S->name() + "]");
    };
};

void SBRunManager::constraintStationCoord(bool IsGlobal)
{
  SBStation	*Station = NULL;
  double	sigma = 0.000001;

  // make a list of stations which are used as in constraints:
  QList<SBStation> Stations;
  Stations.setAutoDelete(FALSE);
  for (SBStationInfo *SI=Prj->stationList()->first(); SI; SI=Prj->stationList()->next())
    if ( SI->isAttr(SBStationInfo::ConstrCoo) )
      {
	if ( (Station=TRF->find(*SI)) )
	  {
	    Stations.append(Station);
	    Station->calcDisplacement(this, TZero);
	    //	  Station->calcDisplacement(this, TRF->epoch());
	  }
	else
	  Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find the station " + SI->aka() +
		     " [" + SI->toString() + "] in the TRF, constraints is ignored");
      }
  
  if (!Stations.count())
    {
      Stations.clear();
      return;
    };
  

  // No Net Translation Position:
  // x-coo:
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      Station->p_DRX()->setD(1.0, TZero);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Translation Position Constraints: used X-coord of the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(0.0, sigma);
  else
    Estimator->processConstr(0.0, sigma);

  // y-coo:
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      Station->p_DRY()->setD(1.0, TZero);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Translation Position Constraints: used Y-coord of the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(0.0, sigma);
  else
    Estimator->processConstr(0.0, sigma);
  
  // z-coo:
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      Station->p_DRZ()->setD(1.0, TZero);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Translation Position Constraints: used Z-coord of the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(0, sigma);
  else
    Estimator->processConstr(0, sigma);



  //  sigma /= 6378136.6*6378136.6;
  // No Net Rotation of Position:
  // x-coo:
  double	r2= 0.0, r_scale=6378136.6*6378136.6;
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      r2 = Station->r(TZero)*Station->r(TZero)/r_scale;
      Station->p_DRZ()->setD( Station->r(TZero).at(Y_AXIS)/r2, TZero);
      Station->p_DRY()->setD(-Station->r(TZero).at(Z_AXIS)/r2, TZero);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Rotation of Position Constraints: (X-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(0.0, sigma);
  else
    Estimator->processConstr(0.0, sigma);

  // y-coo:
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      r2 = Station->r(TZero)*Station->r(TZero)/r_scale;
      Station->p_DRX()->setD( Station->r(TZero).at(Z_AXIS)/r2, TZero);
      Station->p_DRZ()->setD(-Station->r(TZero).at(X_AXIS)/r2, TZero);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Rotation of Position Constraints: (Y-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(0.0, sigma);
  else
    Estimator->processConstr(0.0, sigma);
  
  // z-coo:
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      r2 = Station->r(TZero)*Station->r(TZero)/r_scale;
      Station->p_DRY()->setD( Station->r(TZero).at(X_AXIS)/r2, TZero);
      Station->p_DRX()->setD(-Station->r(TZero).at(Y_AXIS)/r2, TZero);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Rotation of Position Constraints: (Z-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(0, sigma);
  else
    Estimator->processConstr(0, sigma);


  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": Coordinates of new TRF is constrained to the " + TRF->label().toString());
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": The number of used in constraint stations: " + QString().setNum(Stations.count()));
  Stations.clear();
};

void SBRunManager::constraintStationVeloc(bool IsGlobal)
{
  SBStation	*Station = NULL;
  double	sigma = 0.000001;
  double	v;

  // link velocities to values from the catalog, otherwice -- to RefPlate[NNR-NUVEL-1A]:
  bool		IsLinkToSSVField = TRUE;


  // make a list of stations which are used as velocities-constrain
  QList<SBStation> Stations;
  Stations.setAutoDelete(FALSE);
  for (SBStationInfo *SI=Prj->stationList()->first(); SI; SI=Prj->stationList()->next())
    if ( SI->isAttr(SBStationInfo::ConstrVel) )
      {
	if ( (Station=TRF->find(*SI)) )
	  {
	    Stations.append(Station);
	    Station->calcDisplacement(this, TZero);
	    //	  Station->calcDisplacement(this, TRF->epoch());
	  }
	else
	  Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find the station " + SI->aka() +
		     " [" + SI->toString() + "] in the TRF, constraints is ignored");
      }
  
  if (!Stations.count())
    {
      Stations.clear();
      return;
    };
  

  // No Net Translation Velocity:
  // x-coo:
  v = 0.0;
  Vector3	V(v3Zero);
  Vector3	r(v3Zero);
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      V = IsLinkToSSVField?v3Zero:(RefPlates->velocity(*Station) - Station->vt());
      r = Station->r(TZero).unit();
      
      Station->p_DVX()->setD(1.0 - r.at(X_AXIS)*r.at(X_AXIS), TZero);
      Station->p_DVY()->setD(	 - r.at(X_AXIS)*r.at(Y_AXIS), TZero);
      Station->p_DVZ()->setD(	 - r.at(X_AXIS)*r.at(Z_AXIS), TZero);
      
      v += 
	(1.0 - r.at(X_AXIS)*r.at(X_AXIS))	*V.at(X_AXIS) 
	- r.at(X_AXIS)*r.at(Y_AXIS)		*V.at(Y_AXIS) 
	- r.at(X_AXIS)*r.at(Z_AXIS)		*V.at(Z_AXIS);
      //      Station->p_DVX()->setD(1.0, TZero);
      //      v += RefPlates->velocity(*Station).at(X_AXIS) - Station->vt().at(X_AXIS);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Translation Velocity Constraints: (X-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(v, sigma);
  else
    Estimator->processConstr(v, sigma);

  // y-coo:
  v = 0.0;
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      V = IsLinkToSSVField?v3Zero:(RefPlates->velocity(*Station) - Station->vt());
      r = Station->r(TZero).unit();
      
      Station->p_DVX()->setD(	 - r.at(Y_AXIS)*r.at(X_AXIS), TZero);
      Station->p_DVY()->setD(1.0 - r.at(Y_AXIS)*r.at(Y_AXIS), TZero);
      Station->p_DVZ()->setD(	 - r.at(Y_AXIS)*r.at(Z_AXIS), TZero);
      
      v += 
	- r.at(Y_AXIS)*r.at(X_AXIS)		*V.at(X_AXIS) 
	+ (1.0 - r.at(Y_AXIS)*r.at(Y_AXIS))	*V.at(Y_AXIS) 
	- r.at(Y_AXIS)*r.at(Z_AXIS)		*V.at(Z_AXIS);
      //      Station->p_DVY()->setD(1.0, TZero);
      //      v += RefPlates->velocity(*Station).at(Y_AXIS) - Station->vt().at(Y_AXIS);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Translation Velocity Constraints: (Y-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(v, sigma);
  else
    Estimator->processConstr(v, sigma);
  
  // z-coo:
  v = 0.0;
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      V = IsLinkToSSVField?v3Zero:(RefPlates->velocity(*Station) - Station->vt());
      r = Station->r(TZero).unit();
      
      Station->p_DVX()->setD(	 - r.at(Z_AXIS)*r.at(X_AXIS), TZero);
      Station->p_DVY()->setD(	 - r.at(Z_AXIS)*r.at(Y_AXIS), TZero);
      Station->p_DVZ()->setD(1.0 - r.at(Z_AXIS)*r.at(Z_AXIS), TZero);
      
      v += 
	- r.at(Z_AXIS)*r.at(X_AXIS)		*V.at(X_AXIS) 
	- r.at(Z_AXIS)*r.at(Y_AXIS)		*V.at(Y_AXIS)
	+ (1.0 - r.at(Z_AXIS)*r.at(Z_AXIS))	*V.at(Z_AXIS); 
      //      Station->p_DVZ()->setD(1.0, TZero);
      //      v += RefPlates->velocity(*Station).at(Z_AXIS) - Station->vt().at(Z_AXIS);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Translation Velocity Constraints: (Z-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(v, sigma);
  else
    Estimator->processConstr(v, sigma);
  
  
  
  // No Net Spin:
  sigma = 0.000001;
  //  sigma /= 6378136.6*6378136.6;
  // x-coo:
  double	r2= 0.0, r_scale=6378136.6*6378136.6;
  v = 0.0;
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      V = IsLinkToSSVField?v3Zero:(RefPlates->velocity(*Station) - Station->vt());
      r2 = Station->r(TZero)*Station->r(TZero)/r_scale;

      Station->p_DVZ()->setD( Station->r(TZero).at(Y_AXIS)/r2, TZero);
      Station->p_DVY()->setD(-Station->r(TZero).at(Z_AXIS)/r2, TZero);

      v += (Station->r(TZero).at(Y_AXIS)*V.at(Z_AXIS) - Station->r(TZero).at(Z_AXIS)*V.at(Y_AXIS))/r2;
      //      v += (RefPlates->velocity(*Station).at(Y_AXIS) - Station->vt().at(Y_AXIS))*Station->r(TZero).at(Z_AXIS)
      //	-  (RefPlates->velocity(*Station).at(Z_AXIS) - Station->vt().at(Z_AXIS))*Station->r(TZero).at(Y_AXIS)
      //	-  (Station->vt().at(Y_AXIS)*Station->p_DRZ()->v() - Station->vt().at(Z_AXIS)*Station->p_DRY()->v());
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Spin (aka NNR of the Velocities) Constraints: (X-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(v, sigma);
  else
    Estimator->processConstr(v, sigma);
  
  // y-coo:
  v = 0.0;
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      V = IsLinkToSSVField?v3Zero:(RefPlates->velocity(*Station) - Station->vt());
      r2 = Station->r(TZero)*Station->r(TZero)/r_scale;

      Station->p_DVX()->setD( Station->r(TZero).at(Z_AXIS)/r2, TZero);
      Station->p_DVZ()->setD(-Station->r(TZero).at(X_AXIS)/r2, TZero);

      v += (Station->r(TZero).at(Z_AXIS)*V.at(X_AXIS) - Station->r(TZero).at(X_AXIS)*V.at(Z_AXIS))/r2;
      //      Station->p_DVX()->setD(-(Station->r(TZero).at(Z_AXIS) + Station->p_DRZ()->v()), TZero);
      //      Station->p_DVZ()->setD(  Station->r(TZero).at(X_AXIS) + Station->p_DRX()->v(),  TZero);
      //      v += (RefPlates->velocity(*Station).at(Z_AXIS) - Station->vt().at(Z_AXIS))*Station->r(TZero).at(X_AXIS) 
      //	-  (RefPlates->velocity(*Station).at(X_AXIS) - Station->vt().at(X_AXIS))*Station->r(TZero).at(Z_AXIS)
      //	-  (Station->vt().at(Z_AXIS)*Station->p_DRX()->v() - Station->vt().at(X_AXIS)*Station->p_DRZ()->v());
      
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Spin (aka NNR of the Velocities) Constraints: (Y-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(v, sigma);
  else
    Estimator->processConstr(v, sigma);
  
  // Z-coo:
  v = 0.0;
  for (Station = Stations.first(); Station; Station = Stations.next())
    {
      V = IsLinkToSSVField?v3Zero:(RefPlates->velocity(*Station) - Station->vt());
      r2 = Station->r(TZero)*Station->r(TZero)/r_scale;

      Station->p_DVY()->setD( Station->r(TZero).at(X_AXIS)/r2, TZero);
      Station->p_DVX()->setD(-Station->r(TZero).at(Y_AXIS)/r2, TZero);

      v += (Station->r(TZero).at(X_AXIS)*V.at(Y_AXIS) - Station->r(TZero).at(Y_AXIS)*V.at(X_AXIS))/r2;
      //      Station->p_DVX()->setD(  Station->r(TZero).at(Y_AXIS) + Station->p_DRY()->v(),  TZero);
      //      Station->p_DVY()->setD(-(Station->r(TZero).at(X_AXIS) + Station->p_DRX()->v()), TZero);
      //      v += (RefPlates->velocity(*Station).at(X_AXIS) - Station->vt().at(X_AXIS))*Station->r(TZero).at(Y_AXIS) 
      //	-  (RefPlates->velocity(*Station).at(Y_AXIS) - Station->vt().at(Y_AXIS))*Station->r(TZero).at(X_AXIS)
      //	-  (Station->vt().at(X_AXIS)*Station->p_DRY()->v() - Station->vt().at(Y_AXIS)*Station->p_DRX()->v());
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": TRF No Net Spin (aka NNR of the Velocities) Constraints: (Z-coord) used the station [" + 
		 Station->id().toString() + "], " + Station->name());
    };
  if (IsGlobal)
    Estimator->processGlobalConstr(v, sigma);
  else
    Estimator->processConstr(v, sigma);


  
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": Velocity field of new TRF is constrained to the " + QString(IsLinkToSSVField?TRF->label().toString():RefPlates->modelName()));
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": The number of used in constraint stations: " + QString().setNum(Stations.count()));
  Stations.clear();
};

void SBRunManager::constraintEOP(bool IsGlobal)
{
  double	sigma	= 0.3; //mas

  // EOP: X & Y:
  if (Prj->cfg().p().polusXY().type() != SBParameterCfg::PT_NONE)
    {
      Frame->eop()->p_Px()->setD(1.0, TZero);
      if (IsGlobal)
	Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
      else
	Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);
      Frame->eop()->p_Py()->setD(1.0, TZero);
      if (IsGlobal)
	Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
      else
	Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": Pole cooridinates X and Y have been constrained");
    };

  // EOP: d(UT1-UTC):
  sigma	= 0.02; //ms
  if (Prj->cfg().p().polusUT().type() != SBParameterCfg::PT_NONE)
    {
      Frame->eop()->p_Put()->setD(1.0, TZero);
      if (IsGlobal)
	Estimator->processGlobalConstr(0.0, sigma/DAY2SEC*1.0e-3);
      else
	Estimator->processConstr(0.0, sigma/DAY2SEC*1.0e-3);
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": Pole cooridinate d(UT1-UTC) hase been constrained");
    };

  // EOP: dPsi & dEps:
  
  sigma	= 0.3; //mas
  if (Prj->cfg().p().polusNut().type() != SBParameterCfg::PT_NONE)
    {
      Frame->eop()->p_Npsi()->setD(1.0, TZero);
      if (IsGlobal)
	Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
      else
	Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);
      Frame->eop()->p_Neps()->setD(1.0, TZero);
      if (IsGlobal)
	Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
      else
	Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);

      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": Pole cooridinates dPsi and dEps have been constrained");
    };
};

void SBRunManager::constraintSourceCoord(bool IsGlobal)
{
  SBSource	*Source = NULL;
  double	sigma	= 0.000001;

  // make a list of sources which are used as constrains:
  QList<SBSource> Sources;
  Sources.setAutoDelete(FALSE);

  for (SBSourceInfo *SI=Prj->sourceList()->first(); SI; SI=Prj->sourceList()->next())
    if ( SI->isAttr(SBSourceInfo::ConstrCoo) )
      {
	if ( (Source=CRF->find(SI->name())) )
	  Sources.append(Source);
	else
	  Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find the source " + SI->aka() +
		     " [" + SI->name() + "] in the CRF, constrains ignored");
      }
  
  if (!Sources.count())
    {
      Sources.clear();
      return;
    };

  /*--
  // Right Ascension:
  for (Source=Sources.first(); Source; Source=Sources.next())
  {
  Source->p_RA()->setD(1.0, TZero);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
  ": CRF No Net Translation Position Constraints: used RA-coord of the source [" + 
  Source->name() + "]");
  };
  
  if (IsGlobal)
  Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
  else
  Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);
  --*/
  
  double	s_ra=0.0, c_ra=0.0, s_dn=0.0, c_dn=0.0;

  // (1):
  for (Source=Sources.first(); Source; Source=Sources.next())
    {
      sincos(Source->ra(), &s_ra, &c_ra);
      sincos(Source->dn(), &s_dn, &c_dn);
      Source->p_DN()->setD(s_ra, TZero);
      Source->p_RA()->setD(-s_dn*c_dn*c_ra, TZero);

      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": CRF No Net Source Rotation Position Constraints (1): used the source [" + 
		 Source->name() + "]");
    };
  
  if (IsGlobal)
    Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
  else
    Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);

  // (2):
  for (Source=Sources.first(); Source; Source=Sources.next())
    {
      sincos(Source->ra(), &s_ra, &c_ra);
      sincos(Source->dn(), &s_dn, &c_dn);
      Source->p_DN()->setD(c_ra, TZero);
      //      Source->p_RA()->setD(-s_dn*c_dn*s_ra, TZero);
      Source->p_RA()->setD(s_dn*c_dn*s_ra, TZero);

      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": CRF No Net Source Rotation Position Constraints (2): used the source [" + 
		 Source->name() + "]");
    };
  
  if (IsGlobal)
    Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
  else
    Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);

  // (3):
  for (Source=Sources.first(); Source; Source=Sources.next())
    {
      c_dn = cos(Source->dn());
      //      Source->p_DN()->setD(c_dn*c_dn, TZero);
      Source->p_RA()->setD(c_dn*c_dn, TZero);

      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
		 ": CRF No Net Source Rotation Position Constraints (3): used the source [" + 
		 Source->name() + "]");
    };
  
  if (IsGlobal)
    Estimator->processGlobalConstr(0.0, sigma*SEC2RAD*1.0e-3);
  else
    Estimator->processConstr(0.0, sigma*SEC2RAD*1.0e-3);


  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": Coordinates of new CRF is constrained to the " + CRF->label().toString());
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": The number of used in constraint stations: " + QString().setNum(Sources.count()));
};


//
//
// Mode 1:
//

//
// data processing mode_1 // testing
void SBRunManager::process_m1()
{
  SBMJD		Tf, Tc, T;
  SBObservation	*Obs=NULL, *ObsNext=NULL;
  QTime		t;
  SBMJD		StartDate(SBMJD::currentMJD());
  int		dy=0,hr=0,mi=0,sec=0, t_val=0;
  QString	BatchName;
  t.start();
  bool		IsNeedFixEOP;
  bool		IsTmp_1 = TRUE;

  if (prepare4Run_m1()) // there the Estimator->prepare4Run() is called
    {
      int Num=0;
      
      mainPrBarSetTotalSteps(Prj->numObs());
      while (VLBISessIt->current() && !IsNeedToStop)
	{
	  IsNeedFixEOP = FALSE;
	  loadVLBISession_m1(VLBISessIt->current()->name());
	  if (VLBISessIt->current()->isAttr(SBVLBISesInfo::ConstrEOP))
	    IsNeedFixEOP = TRUE;
	  Tf = VLBISessIt->current()->tFinis();
	  Tc = VLBISessIt->current()->tMean();
	  ++*VLBISessIt; // get next session
	  while (VLBISessIt->current() && 
		 (VLBISessIt->current()->tMean()<Tf || VLBISessIt->current()->tStart()<Tc))
	    {
	      loadVLBISession_m1(VLBISessIt->current()->name());
	      if (VLBISessIt->current()->isAttr(SBVLBISesInfo::ConstrEOP))
		IsNeedFixEOP = TRUE;
	      ++*VLBISessIt; // get next session
	    };

	  mainSetSessName(BatchName=listOfCurrentSessNames());
	  
	  if (!ObsList.count())
	    Log->write(SBLog::INF, SBLog::RUN, ClassName() + 
		       ": there is no any eligible observation on the batch [" + 
		       BatchName + "], skipping the data ");
	  else
	    {
	      // at this point the set/pack/batch of sessions are loaded and obs are kept in ObsList/ObsIt,
	      // and the VLBISessCache keeps the list of loaded sessions
	      T = ObsList.first()->mjd();
	      checkParameterLists(T); // created the lists of parameters
	      Estimator->prepare4Local(ObsList.count(), T);
	      // loading local parameters:
	      Solution->prepare4Batch(BatchName, TRUE);
	      // update statistic info:
	      Solution->statGeneral()->addNumPars(LocalList->count());
	      Solution->statGeneral()->addNumPars(StochList->count());
	      Solution->stat4CurrentBatch()->addNumPars(GlobalList->count());
	      Solution->stat4CurrentBatch()->addNumPars(LocalList->count());
	      Solution->stat4CurrentBatch()->addNumPars(StochList->count());
	      
	      
	      // prepare EOP for interpolation:
	      Frame->eop()->prepare4Date(T);

	      // prepare AploEphem for interpolation:
	      if (AploEphem && prj()->cfg().isUseAploEphem())
		{
		  for (SBStation *Station=TRF->stations()->first(); Station; Station=TRF->stations()->next())
		    {
		      SBStationID	id(Station->id());
		      for (SBVLBISession *Session=VLBISessCache.first(); Session; Session=VLBISessCache.next())
			if (Session->stationList()->find(&id))
			  AploEphem->registerStation(Station);
		    };
		  AploEphem->getReady(ObsList.first()->mjd() - 1.5*AploEphem->interval(), 
				      ObsList.last ()->mjd() + 1.5*AploEphem->interval());
		};
	      
	      //make constrains:
	      if ( (Prj->cfg().p().clock0().type() != SBParameterCfg::PT_NONE) && 
		   (Prj->cfg().p().clock0().type() != SBParameterCfg::PT_GLB)   )
		constrainClocks(FALSE);

	      if ( (Prj->cfg().p().staCoo().type() != SBParameterCfg::PT_NONE) && 
		   (Prj->cfg().p().staCoo().type() != SBParameterCfg::PT_GLB)   )
		{
		  if (Prj->cfg().p().staCoo().type() != SBParameterCfg::PT_STH)
		    constraintStationCoord(FALSE);
		  else
		    if (IsTmp_1)
		      {
			constraintStationCoord(FALSE);
			IsTmp_1 = FALSE;
		      };
		};
	      
	      if ( (Prj->cfg().p().staVel().type() != SBParameterCfg::PT_NONE) && 
		   (Prj->cfg().p().staVel().type() != SBParameterCfg::PT_GLB)
		   )
		constraintStationVeloc(FALSE);

	      if ( (Prj->cfg().p().srcCoo().type() != SBParameterCfg::PT_NONE) && 
		   (Prj->cfg().p().srcCoo().type() != SBParameterCfg::PT_GLB)
		   )
		constraintSourceCoord(FALSE);
	      
	      if (IsNeedFixEOP)
		if ( ((Prj->cfg().p().polusXY(). type() != SBParameterCfg::PT_NONE) && 
		      (Prj->cfg().p().polusXY(). type() != SBParameterCfg::PT_GLB)) ||
		     ((Prj->cfg().p().polusUT(). type() != SBParameterCfg::PT_NONE) && 
		      (Prj->cfg().p().polusUT(). type() != SBParameterCfg::PT_GLB)) ||
		     ((Prj->cfg().p().polusNut().type() != SBParameterCfg::PT_NONE) && 
		      (Prj->cfg().p().polusNut().type() != SBParameterCfg::PT_GLB))  )
		  constraintEOP(FALSE);
	      
	      
	      // temporary:
	      uint	SavedDBGLogFac = Log->logFacilities(SBLog::DBG);
	      bool	IsShunted = FALSE;
	      
	      // now, process the batch:
	      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": main loop of the batch " + 
			 BatchName + " has been started");
	      for (Obs=ObsList.first(); Obs && !IsNeedToStop; Num++)
		{
		  // temporary:
		  if (Num>5000 && !IsShunted)
		    {
		      Log->setLogFacilities(SBLog::DBG, 0);
		      IsShunted = TRUE;
		    };
		  
		  Obs->process(this);
		  
		  if ((ObsNext=ObsList.next()))
		    T = ObsNext->mjd();
		  Estimator->processObs(Obs->mjd(), T, Obs->O_C(), Obs->Sigma());
		  
		  Obs=ObsNext;
		  
		  if (Num%500==0)
		    {
		      mainPrBarSetProgress(Num);
		      mainSetProcObs(Num, Prj->numObs());
		      qApp->processEvents();
		    };
		  //qApp->processEvents();
		  
		};
	      // temporary:
	      Log->setLogFacilities(SBLog::DBG, SavedDBGLogFac);
	      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": main loop of the batch " + 
			 BatchName + " has been finised");

	      if (AploEphem)
		AploEphem->dismissed();

	      if (GlobalList)
		{
		  for (SBParameter *p=GlobalList->first(); p; p=GlobalList->next())
		    if (!p->name().contains("So: ")) // sources have special handling
		      p->addNumSes(VLBISessCache.count());

		  // Sources: calc actual number of sessions:
		  for (SBVLBISession *S=VLBISessCache.first(); S; S=VLBISessCache.next())
		    {
		      // for IVS ICRF2 WG output (cat format):
		      for (SBSourceInfo *SI=S->sourceList()->first(); SI; SI=S->sourceList()->next())
			{
			  SBSource	*Src;
			  if ((Src=CRF->find(SI->name())))
			    {
			      if (GlobalList->find(Src->p_DN()->name()) && !SI->isAttr(SBSourceInfo::notValid) &&
				  SI->isAttr(SBSourceInfo::EstCoo) && SI->procNum())
				{
				  Src->p_RA()->addNumSes(1);
				  Src->p_DN()->addNumSes(1);
				};
			    }
			  else
			    Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find source " + 
				       SI->name() + " in the catalogue");
			};
		    };
 		};

	      if (LocalList)
		{
		  for (SBVLBISession *S=VLBISessCache.first(); S; S=VLBISessCache.next())
		    {
		      // for IVS ICRF2 WG output (ts format):
		      for (SBSourceInfo *SI=S->sourceList()->first(); SI; SI=S->sourceList()->next())
			{
			  SBSource	*Src;
			  if ((Src=CRF->find(SI->name())))
			    {
			      if (LocalList->find(Src->p_DN()->name()) &&
				  !SI->isAttr(SBSourceInfo::notValid) &&
				  SI->isAttr(SBSourceInfo::EstCoo) &&
				  SI->procNum())
				{
				  Src->p_RA()->addNumSes(1);
				  Src->p_DN()->addNumSes(1);
				};
			    }
			  else
			    Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find source " + 
				       SI->name() + " in the catalogue");
			};
		    };
		};

	      Estimator->finisLocal();
	    };
	  //
	  // at last, save the sessions in the batch:
	  saveVLBISessions_m1();
	};

      
      // make global constraints:
      Estimator->prepare4GlobConstr(20);

      if (Prj->cfg().p().staCoo().type() == SBParameterCfg::PT_GLB)
	constraintStationCoord(TRUE);
      if (Prj->cfg().p().staVel().type() == SBParameterCfg::PT_GLB)
	constraintStationVeloc(TRUE);
      if (Prj->cfg().p().srcCoo().type() == SBParameterCfg::PT_GLB)
	constraintSourceCoord(TRUE);

      Estimator->finisGlobConstr();
      //.
      //
      
      finisRun_m1(); // here Estimator->finisRun() is called
      Log->write(SBLog::INF, SBLog::RUN, ClassName() + ": the project [" + Prj->name() + "] finished");
      Log->write(SBLog::INF, SBLog::RUN, ClassName() + ": number of observations/parameters: " + 
		 QString().sprintf("%d/%d", Solution->statGeneral()->numObs(), 
				   Solution->statGeneral()->numPars()));
      Log->write(SBLog::INF, SBLog::RUN, ClassName() + ": weighted root means squares      : " + 
		 QString().sprintf("%.6f ", SetUp->scale()*Solution->statGeneral()->WRMS_unit()) + 
		 SetUp->scaleName());
      Log->write(SBLog::INF, SBLog::RUN, ClassName() + ": variance factor                  : " + 
		 QString().sprintf("%.6f ", SetUp->scale()*Solution->statGeneral()->VarFactor_unit()) + 
		 SetUp->scaleName());
    }
  else 
    Log->write(SBLog::ERR, SBLog::RUN, ClassName() + 
	       ": can't prepare for run project [" + (Prj?Prj->name():QString("NULL")) + "]");
  
  t_val = t.elapsed();
  dy =(int)floor(SBMJD::currentMJD() - StartDate);
  sec=(int)round(t_val/1000.0);
  hr = sec/3600;
  mi =(sec-hr*3600)/60;
  sec-=hr*3600+mi*60;

  if (dy==0)
    Log->write(SBLog::INF, SBLog::RUN, ClassName() + 
	       ": elapsed time for the project [" + Prj->name() + "]: " + 
	       QString().sprintf("%02d:%02d:%02d, (%g sec)", hr,mi,sec,  t_val/1000.0));
  else 
    Log->write(SBLog::INF, SBLog::RUN, ClassName() + 
	       ": elapsed time for the project [" + Prj->name() + "]: " + 
	       QString().sprintf("%dday(s), %02d:%02d:%02d, (%g sec)", dy,hr,mi,sec,  
				 dy*86400.0+t_val/1000.0));
  return;
};

bool SBRunManager::prepare4Run_m1()
{
  Prj->clearRMSs();

  // lists of all global parameters:
  AllGlobalList = new SBParameterList;
  fillParameterList(AllGlobalList, SBParameterCfg::PT_GLB, Te);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": the list of all global parameters (" + 
	     QString("_").setNum(AllGlobalList->count()) + ") has been created");

  Estimator->prepare4Run(AllGlobalList);
  Solution->statGeneral()->clearStatistics();
  Solution->statGeneral()->addNumPars(AllGlobalList->count());
  
  // lists of gloabal parameters:
  GlobalList = new SBParameterList;
  // lists of arc parameters:
  ArcList = new SBParameterList;
  // lists of local parameters:
  LocalList = new SBParameterList;
  // lists of stochastic parameters:
  StochList = new SBParameterList;

  // creating sorted by TStart list of the VLBI sessions:
  for (SBVLBISesInfo *si=Prj->VLBIsessionList()->first(); si; si=Prj->VLBIsessionList()->next()) 
    if (!si->isAttr(SBVLBISesInfo::notValid)) 
      VLBISessions.inSort(si);
  VLBISessIt = new SBVLBISesInfoIterator(VLBISessions);
  if (!VLBISessIt->count())
    Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": no any valid VLBI session");
  else 
    {
      // loading VLBI sessions into the chache:
      if (!VLBI)
	{
	  mainSetOperation("Loading VLBI dataset");
	  if (!(VLBI=SetUp->loadVLBI())) VLBI=new SBVLBISet;
	  IsVLBIOwner = TRUE;
	  Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		     ": loaded default VLBI dataset;");
	};
    };
  // observations' iterator:
  if (ObsIt) delete ObsIt;
  ObsIt = new SBObsIterator(ObsList);

  return VLBISessIt->count();
};

void SBRunManager::finisRun_m1()
{
  Estimator->finisRun();

  if (IsVLBIOwner)
    SetUp->saveVLBI(VLBI);

  // is checked only once (user has to check it again if want to create DSINEXes):
  if (Prj->cfg().isGenerateDSINEXes())
    Prj->cfg().setIsGenerateDSINEXes(FALSE);

  if (IsPrjOwner)
    SetUp->saveProject(Prj);

  if (ObsIt) 
    {
      delete ObsIt; 
      ObsIt=NULL;
    };
  if (VLBISessIt) 
    {
      delete VLBISessIt;
      VLBISessIt=NULL;
    };

  VLBISessions.clear();

  // lists of parameters:
  if (AllGlobalList)
    {
      delete AllGlobalList;
      AllGlobalList = NULL;
    };
  if (GlobalList)
    {
      delete GlobalList;
      GlobalList = NULL;
    };
  if (ArcList)
    {
      delete ArcList;
      ArcList = NULL;
    };
  if (LocalList)
    {
      delete LocalList;
      LocalList = NULL;
    };
  if (StochList)
    {
      delete StochList;
      StochList = NULL;
    };
};

void SBRunManager::loadVLBISession_m1(const QString& Name_)
{
  mainSetOperation("Loading session " + Name_);
  qApp->processEvents();
  SBVLBISession *S=NULL;
  if ((S = VLBI->loadSession(Name_)))
    if (S->count())
      {
	// check eccentricities
	TRF->prepareEccs4Session(SBMJD(S->tFinis().date()), S);

	mainSetOperation("Inserting observations for " + S->name());
	Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": session [" + S->name() + 
		   "] has been loaded");
	S->clearRMSs();
	qApp->processEvents();
	VLBISessCache.append(S);
	for (SBObservation* Obs=S->first(); Obs; Obs=S->next()) 
	  if (isObsEligible(Obs)) 
	    ObsList.append(Obs);
	ObsList.sort();
	mainSetCacheSizes(ObsList.count(), VLBISessCache.count(), CRF->count(), TRF->count());
      }
    else
      {
	Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		   ": the VLBI session [" + S->name() + 
		   "] doesn't contain observations, skipped");
	delete S;
      }
  else // oops!..
    Log->write(SBLog::ERR, SBLog::DATA | SBLog::IO, ClassName() + 
	       ": can't load VLBI session [" + Name_ + "];");
  
  mainSetSessName(listOfCurrentSessNames());
  mainSetOperation("Processing");
  qApp->processEvents();
};

// save and release all sessions with date of the last observation after time T_:
void SBRunManager::saveVLBISessions_m1()
{
  /**Some other very important comments.
   * We're testing the doxygen (test).
   */
  ObsList.clear();
  while (VLBISessCache.count())
    {
      SBVLBISession* S = VLBISessCache.first();
      // update RMSs:
      SBVLBISesInfo	*SI = NULL;
      if (VLBISessions.find(S) != -1)
	{
	  SI = VLBISessions.current();
	  SI->setDelayRMS2 (S->delayRMS2 ());
	  SI->setRateRMS2  (S->rateRMS2  ());
	  SI->setDelaySumW2(S->delaySumW2());
	  SI->setRateSumW2 (S->rateSumW2 ());
	}
      else
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't find session [" + S->name() + 
		   "] in the list of sessions");

      // save the sessions:
      mainSetOperation("Saving session " + S->name());
      qApp->processEvents();
      if (!VLBI->saveSession(S))
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't save session [" + S->name() + "]");
      else
	Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": session [" + S->name() + "] has been saved");

      // remove the sessions from the cache:
      VLBISessCache.remove(S);
      prcsVLBISess++;
    };
  
  mainSetCacheSizes(ObsList.count(), VLBISessCache.count(), CRF->count(), TRF->count());
  mainSetSessName(listOfCurrentSessNames());
  mainSetProcSes(prcsVLBISess, Prj->numVLBISessions());
  mainSetOperation("Processing");
  qApp->processEvents();
};

//
// ----------------------------

//
// Mode 2:
//
// data processing mode_2 // testing
void SBRunManager::process_m2()
{
  QTime		t;
  int		hr=0,mi=0,sec=0;
  QString	str = "NULL";
  t.start();
  if (prepare4Run_m2())
    {
      int Num=0;
      mainPrBarSetTotalSteps(Prj->numObs());
      for (SBObservation* Obs=ObsIt->toFirst(); Obs && !IsNeedToStop; Obs=nextObs_m2(), Num++)
	{
	  Obs->process(this);
	  
	  if (Num%500==0)
	    {
	      mainPrBarSetProgress(Num);
	      mainSetProcObs(Num, Prj->numObs());
	    };
	  qApp->processEvents();
	};
      finisRun_m2();
    }
  else 
    Log->write(SBLog::ERR, SBLog::RUN, ClassName() + 
	       ": can't prepare for run project [" + (Prj?Prj->name():str) + "]");
  sec=(int)round(t.elapsed()/1000.0);
  hr=sec/3600;
  mi=(sec-hr*3600)/60;
  sec-=hr*3600+mi*60;
  str.sprintf("%02d:%02d:%02d, (%g sec)", hr,mi,sec,  t.elapsed()/1000.0);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": elapsed time for the project [" + Prj->name() + "]: " + str);
  return;
};

bool SBRunManager::prepare4Run_m2()
{
  // lists of all global parameters:
  AllGlobalList = new SBParameterList;
  // for AllGlobalList "fillParameterList(AllGlobalList, SBParameterCfg::PT_GLB)" should be called 
  // before the first call to "loadVLBISessions(Tb)" <grin>
  fillParameterList(AllGlobalList, SBParameterCfg::PT_GLB, Te);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": the list of all global parameters (" + 
	     QString("_").setNum(AllGlobalList->count()) + ") has been created");

  Estimator->prepare4Run(AllGlobalList);
  
  // lists of gloabal parameters:
  GlobalList = new SBParameterList;
  // lists of arc parameters:
  ArcList = new SBParameterList;
  // lists of local parameters:
  LocalList = new SBParameterList;
  // lists of stochastic parameters:
  StochList = new SBParameterList;

  // creating date sorted list of the VLBI sessions:
  for (SBVLBISesInfo *si=Prj->VLBIsessionList()->first(); si; si=Prj->VLBIsessionList()->next()) 
    if (!si->isAttr(SBVLBISesInfo::notValid)) 
      VLBISessions.inSort(si);
  VLBISessIt = new SBVLBISesInfoIterator(VLBISessions);
  if (!VLBISessIt->count())
    Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": no any valid VLBI session");
  else 
    {
      // loading VLBI sessions into the chache:
      if (!VLBI)
	{
	  mainSetOperation("Loading VLBI dataset");
	  if (!(VLBI=SetUp->loadVLBI())) VLBI=new SBVLBISet;
	  Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		     ": loaded default VLBI dataset;");
	};
      // first session:
      Tb=VLBISessions.first()->tStart();	// correct Tb & Te for session's flag "notValid"
      Te=VLBISessions.last ()->tFinis() + 1.0;	// the date that greater than the last observation.
      // load first VLBI session:
      loadVLBISessions_m2(Tb);
      checkParameterLists(Tb);
      T_2save=findT_2save();
      T_2load=findT_2load();
    };
  
  // some other stuff here:

  // observations' iterator:
  if (ObsIt) delete ObsIt;
  ObsIt = new SBObsIterator(ObsList);

  return ObsIt->count();
};

void SBRunManager::finisRun_m2()
{
  saveVLBISessions_m2(Te);

  Estimator->finisRun();

  if (ObsIt) 
    {
      delete ObsIt; 
      ObsIt=NULL;
    };
  if (VLBISessIt) 
    {
      delete VLBISessIt;
      VLBISessIt=NULL;
    };

  ObsList.clear();
  VLBISessCache.clear();
  VLBISessions.clear();

  // lists of parameters:
  if (AllGlobalList)
    {
      delete AllGlobalList;
      AllGlobalList = NULL;
    };
  if (GlobalList)
    {
      delete GlobalList;
      GlobalList = NULL;
    };
  if (ArcList)
    {
      delete ArcList;
      ArcList = NULL;
    };
  if (LocalList)
    {
      delete LocalList;
      LocalList = NULL;
    };
  if (StochList)
    {
      delete StochList;
      StochList = NULL;
    };
};

// save and release all sessions with date of the last observation after time T_:
void SBRunManager::saveVLBISessions_m2(const SBMJD& T)
{
  /**Some other very important comments.
   * We're testing the doxygen (test).
   */
  if (VLBISessCache.count())
    {
      SBVLBISession* S;
      QListIterator<SBVLBISession> it(VLBISessCache);
      for (it.toFirst(); (S=it.current()); ++it) 
	if (S->last()->mjd()<T)
	  {
	    // save the sessions:
	    mainSetOperation("Saving session " + S->name());
	    qApp->processEvents();
	    if (!VLBI->saveSession(S))
	      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't save session [" + 
			 S->name() + "]");
	    else
	      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": session [" + S->name() + 
			 "] has been saved");
	    // remove saved observations from the obs. list:
	    mainSetOperation("Clearing observations for " + S->name());
	    qApp->processEvents();
	    for (SBObservation* Obs=S->first(); Obs; Obs=S->next()) 
	      if (!ObsList.remove(Obs))
		Log->write(SBLog::ERR, SBLog::DATA, ClassName() + 
			   ": can't remove observation [" + Obs->toString() + 
			   "] from the observation list;");
	    // remove the sessions from the cache:
	    VLBISessCache.remove(S);
	    prcsVLBISess++;
	    mainSetCacheSizes(ObsList.count(), VLBISessCache.count(), CRF->count(), TRF->count());
	  };
    }
  else
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't save any VLBI session;");

  mainSetSessName(listOfCurrentSessNames());
  mainSetProcSes(prcsVLBISess, Prj->numVLBISessions());
  mainSetOperation("Processing");
  qApp->processEvents();
};

// loads all unprocessed sessions with date of the first observation at time T_ or before it:
void SBRunManager::loadVLBISessions_m2(SBMJD& T)
{
  for (; VLBISessIt->current() && VLBISessIt->current()->tStart()<=T; ++*VLBISessIt)
    {
      mainSetOperation("Loading session " + VLBISessIt->current()->name());
      qApp->processEvents();
      SBVLBISession *S=NULL;
      if ((S = VLBI->loadSession(VLBISessIt->current()->name())))
	if (S->count())
	  {
	    mainSetOperation("Inserting observations for " + S->name());
	    Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": session [" + S->name() + 
		       "] has been loaded");
	    qApp->processEvents();
	    VLBISessCache.append(S);
	    for (SBObservation* Obs=S->first(); Obs; Obs=S->next()) 
	      ObsList.inSort(Obs);
	    mainSetCacheSizes(ObsList.count(), VLBISessCache.count(), CRF->count(), TRF->count());
	  }
	else
	  {
	    Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		       ": the VLBI session [" + S->name() + 
		       "] doesn't contain observations, skipped");
	    delete S;
	  }
      else // oops!..
	Log->write(SBLog::ERR, SBLog::DATA | SBLog::IO, ClassName() + 
		   ": can't load VLBI session [" + VLBISessIt->current()->name() + "];");
    };

  mainSetSessName(listOfCurrentSessNames());
  mainSetOperation("Processing");
  qApp->processEvents();
};

SBObservation* SBRunManager::nextObs_m2()
{
  if (ObsIt->current())
    {
      SBMJD T(*ObsIt->current());
      // there are some unsaved sessions:
      if (T_2save<T)
	{
	  saveVLBISessions_m2(T);
	  T_2save=findT_2save();
	  if (!ObsIt->atFirst() // if ObsIt->atFirst()	=> there is no overlapping sessions
	      && T!=T_2save)	// if T==T_2save	=> the overlapped session is ending at T
	    checkParameterLists(T);
	};
      // we have to load new session (new sessions start before the end of current session(s)):
      if (!ObsIt->atLast())
	{
	  if (T_2load<=(T=*++*ObsIt))
	    {
	      --*ObsIt;
	      loadVLBISessions_m2(T);
	      T_2load=findT_2load();
	      T_2save=findT_2save();
	      checkParameterLists(*ObsIt->current());
	    }
	  else --*ObsIt;
	}
      else
	// just a new session (in this case T_2save doesn't need modification):
	if (T<=T_2load && T_2load!=Te)
	  {
	    loadVLBISessions_m2(T_2load);
	    checkParameterLists(T);
	    T_2load=findT_2load();
	  };
      return ++*ObsIt;
    }
  else 
    return NULL;
};

//
//
// Parameter manipulations:
//
void SBRunManager::checkParameterLists(const SBMJD& T)
{
  Estimator->parameterListsAboutToChange();

  // lists of gloabal parameters:
  fillParameterList(GlobalList, SBParameterCfg::PT_GLB, T);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": " + T.toString() + 
	     ": the list of global parameters (" + 
	     QString("_").setNum(GlobalList->count()) + ") has been created");

  // lists of arc parameters:
  fillParameterList(ArcList, SBParameterCfg::PT_ARC, T);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": " + T.toString() +
	     ": the list of arc parameters (" + 
	     QString("_").setNum(ArcList->count()) + ") has been created");

  // lists of local parameters:
  fillParameterList(LocalList, SBParameterCfg::PT_LOC, T);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": " + T.toString() +
	     ": the list of local parameters (" + 
	     QString("_").setNum(LocalList->count()) + ") has been created");

  // lists of stochastic parameters:
  fillParameterList(StochList, SBParameterCfg::PT_STH, T);
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": " + T.toString() +
	     ": the list of stochastic parameters (" + 
	     QString("_").setNum(StochList->count()) + ") has been created");

  Estimator->parameterListsChanged();
};

void SBRunManager::fillParameterList(SBParameterList*& List, SBParameterCfg::PType Type, const SBMJD& T)
{
  bool isGood = TRUE;
  bool isNeedForSomeSession = FALSE;

  if (!List)
    {
      QString sTmp;
      switch (Type)
	{
	case SBParameterCfg::PT_GLB: sTmp = "global";
	  break;
	case SBParameterCfg::PT_ARC: sTmp = "arc";
	  break;
	case SBParameterCfg::PT_LOC: sTmp = "local";
	  break;
	case SBParameterCfg::PT_STH: sTmp = "stochastic";
	  break;
	default:
	case SBParameterCfg::PT_NONE: sTmp = "unknown";
	  break;
	};
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": the list of " + sTmp +
		 " parameters is wrong (NULL)");
      return;
    };

  // remoove `in run' flag:
  for (SBParameter *p=List->first(); p; p=List->next())
    p->delAttr(SBDerivation::IsInRun);
  
  // clear the list:
  List->clear();

  // fill the list according to project's config:
  // stations:
  for (SBStation *Station=TRF->stations()->first(); Station; Station=TRF->stations()->next())
    {
      isGood = TRUE;
      SBStationID	id(Station->id());
      SBStationInfo	*SI_prj;
      SBStationInfo	*SI_session=NULL, *SI_a_session=NULL;
      if (!(SI_prj = Prj->stationList()->find(&id)))
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find station [" + 
		   Station->name() + "] in the project");
      else
	{
	  if (SI_prj->isAttr(SBStationInfo::notValid))
	    isGood=FALSE;
	  if (isGood && VLBISessCache.count())
	    {
	      isGood=FALSE;
	      isNeedForSomeSession = FALSE;
	      for (SBVLBISession *Session=VLBISessCache.first(); Session; Session=VLBISessCache.next())
		if (!Session->isAttr(SBVLBISesInfo::notValid) &&
		    Session->last()->mjd() > T &&
		    (SI_a_session = Session->stationList()->find(&id)) &&
		    !SI_a_session->isAttr(SBStationInfo::notValid))
		  {
		    isNeedForSomeSession = TRUE;
		    SI_session = SI_a_session;
		  };
	      
	      if (isNeedForSomeSession) // at least one session declared the station as a valid one
		isGood = TRUE;
	    };
	  
	  if (isGood)
	    {
	      // clock behaviour:
	      if (Prj->cfg().p().clock0().type() == Type)
		{
		  // "session" will overwrite "project":
		  if (SI_session->isAttr(SBStationInfo::UseLocalClocks))
		    Station->p_C0()->tuneParameter(SI_session->clocks());
		  else if (SI_prj->isAttr(SBStationInfo::UseLocalClocks))
		    Station->p_C0()->tuneParameter(SI_prj->clocks());
		  else
		    Station->p_C0()->tuneParameter(Prj->cfg().p().clock0());
		  List->inSort(Station->p_C0());
		}
	      if (Prj->cfg().p().clock1().type() == Type)
		{
		  Station->p_C1()->tuneParameter(Prj->cfg().p().clock1());
		  List->inSort(Station->p_C1());
		}
	      if (Prj->cfg().p().clock2().type() == Type)
		{
		  //		  Station->p_C2()->tuneParameter(Prj->cfg().p().clock2());
		  //		  List->inSort(Station->p_C2());
		  // !_
		  // quick'n'dirty: make an estimation of higher order polinoms
		  if (SI_session->cl_Order()>2)
		    for (int i=2; i<SI_session->cl_Order(); i++)
		      {
			Station->p_Clock(i)->tuneParameter(Prj->cfg().p().clock2());
			List->inSort(Station->p_Clock(i));
		      };
		};
	      
	      // zenith delay:
	      if (Prj->cfg().p().zenith().type() == Type)
		{
		  // "session" will overwrite "project":
		  if (Type!=SBParameterCfg::PT_GLB && SI_session->isAttr(SBStationInfo::UseLocalZenith))
		    Station->p_Zenith()->tuneParameter(SI_session->zenith());
		  else if (SI_prj->isAttr(SBStationInfo::UseLocalZenith))
		    Station->p_Zenith()->tuneParameter(SI_prj->zenith());
		  else
		    Station->p_Zenith()->tuneParameter(Prj->cfg().p().zenith());
		  List->inSort(Station->p_Zenith());
		};

	      // gradients:
	      if (Prj->cfg().p().atmGrad().type() == Type)
		{
		  Station->p_AtmGradN()->tuneParameter(Prj->cfg().p().atmGrad());
		  Station->p_AtmGradE()->tuneParameter(Prj->cfg().p().atmGrad());
		  List->inSort(Station->p_AtmGradN());
		  List->inSort(Station->p_AtmGradE());
		};

	      // axis offset:
	      if ( (Prj->cfg().p().axisOffset().type() == Type) &&
		   (Prj->stationList()->find(&id)->isAttr(SBStationInfo::EstCoo)))
		{
		  Station->p_Axis()->tuneParameter(Prj->cfg().p().axisOffset());
		  List->inSort(Station->p_Axis());
		};
	      
	      // cable corrections:
	      if (Prj->cfg().p().cable().type() == Type && !SI_session->isAttr(SBStationInfo::BadCable))
		{
		  Station->p_Cable()->tuneParameter(Prj->cfg().p().cable());
		  List->inSort(Station->p_Cable());
		};

	      // coordinates:
	      if ( (Prj->cfg().p().staCoo().type() == Type) &&
		   (Prj->stationList()->find(&id)->isAttr(SBStationInfo::EstCoo)) )
		{
		  Station->p_DRX()->tuneParameter(Prj->cfg().p().staCoo());
		  Station->p_DRY()->tuneParameter(Prj->cfg().p().staCoo());
		  Station->p_DRZ()->tuneParameter(Prj->cfg().p().staCoo());
		  List->inSort(Station->p_DRX());
		  List->inSort(Station->p_DRY());
		  List->inSort(Station->p_DRZ());
		};
	      // velocities:
	      if ( (Prj->cfg().p().staVel().type() == Type) &&
		   (Prj->stationList()->find(&id)->isAttr(SBStationInfo::EstVel)))
		{
		  Station->p_DVX()->tuneParameter(Prj->cfg().p().staVel());
		  Station->p_DVY()->tuneParameter(Prj->cfg().p().staVel());
		  Station->p_DVZ()->tuneParameter(Prj->cfg().p().staVel());
		  List->inSort(Station->p_DVX());
		  List->inSort(Station->p_DVY());
		  List->inSort(Station->p_DVZ());
		};
	      // AUX parameters:
	      // H2:
	      if (Prj->cfg().p().love_h2().type() == Type)
		{
		  Station->p_TideH2()->tuneParameter(Prj->cfg().p().love_h2());
		  List->inSort(Station->p_TideH2());
		};  
	      // L2:
	      if (Prj->cfg().p().love_l2().type() == Type)
		{
		  Station->p_TideL2()->tuneParameter(Prj->cfg().p().love_l2());
		  List->inSort(Station->p_TideL2());
		};
	      // Lag:
	      if (Prj->cfg().p().tideLag().type() == Type)
		{
		  Station->p_TideLag()->tuneParameter(Prj->cfg().p().tideLag());
		  List->inSort(Station->p_TideLag());
		};
	      
	      //..............................................................
	      // tests:
	      /*
		if (Prj->cfg().p().test().type() == Type)
		{
		Station->p_Aplo()->tuneParameter(Prj->cfg().p().test());
		Station->p_Aplo()->setScale(1.0);
		List->inSort(Station->p_Aplo());
		};
	      */
	      // tests.
	    };
	};
    };

  // sources:
  for (SBSource *Source=CRF->first(); Source; Source=CRF->next())
    {
      SBSourceInfo *SI;
      isGood = TRUE;
      if (!(SI = Prj->sourceList()->find(Source)))
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find source [" + 
		   Source->name() + "] in the project");
      else
	{
	  if (SI->isAttr(SBSourceInfo::notValid))
	    isGood=FALSE;
	  if (isGood && VLBISessCache.count())
	    {
	      isGood=FALSE;
	      isNeedForSomeSession = FALSE;
	      for (SBVLBISession *Session=VLBISessCache.first(); Session; Session=VLBISessCache.next())
		if (!Session->isAttr(SBVLBISesInfo::notValid) && 
		    Session->last()->mjd() > T &&
		    (SI = Session->sourceList()->find(Source)) && 
		    !SI->isAttr(SBSourceInfo::notValid))
		  isNeedForSomeSession = TRUE;
	      
	      if (isNeedForSomeSession) // at least one session declared the source as a valid one
		isGood = TRUE;
	    };
	  
	  if (isGood)
	    {
	      // coordinates:
	      if (Prj->cfg().p().srcCoo().type() == Type &&
		  Prj->sourceList()->find(Source)->isAttr(SBSourceInfo::EstCoo))
		{
		  Source->p_RA()->tuneParameter(Prj->cfg().p().srcCoo());
		  Source->p_DN()->tuneParameter(Prj->cfg().p().srcCoo());
		  List->inSort(Source->p_RA());
		  List->inSort(Source->p_DN());
		};
	    };
	};
    };

  // EOPs:
  isGood = TRUE;
  // put here: checking for constrains...

  if (isGood && VLBISessCache.count())
    {
      isGood=FALSE;
      isNeedForSomeSession = FALSE;
      for (SBVLBISession *Session=VLBISessCache.first(); Session; Session=VLBISessCache.next())
	if (!Session->isAttr(SBVLBISesInfo::notValid) && 
	    Session->last()->mjd() > T )
	  isNeedForSomeSession = TRUE;
      if (isNeedForSomeSession)
	isGood = TRUE;
    };
  
  if (isGood)
    {
      if (Prj->cfg().p().polusXY().type() == Type)
	{
	  Frame->eop()->p_Px()->tuneParameter(Prj->cfg().p().polusXY());
	  Frame->eop()->p_Py()->tuneParameter(Prj->cfg().p().polusXY());
	  List->inSort(Frame->eop()->p_Px());
	  List->inSort(Frame->eop()->p_Py());
	};
      if (Prj->cfg().p().polusUT().type() == Type)
	{
	  Frame->eop()->p_Put()->tuneParameter(Prj->cfg().p().polusUT());
	  List->inSort(Frame->eop()->p_Put());
	};
      if (Prj->cfg().p().polusNut().type() == Type)
	{
	  Frame->eop()->p_Npsi()->tuneParameter(Prj->cfg().p().polusNut());
	  Frame->eop()->p_Neps()->tuneParameter(Prj->cfg().p().polusNut());
	  List->inSort(Frame->eop()->p_Npsi());
	  List->inSort(Frame->eop()->p_Neps());
	};

      // tmps:
      if (Prj->cfg().p().test().type() == Type)
	{
	  SBParameterCfg	CfgTestXY(Prj->cfg().p().test());
	  SBParameterCfg	CfgTestUT(Prj->cfg().p().test());

	  CfgTestXY.setScale(1.e3*RAD2SEC);
	  CfgTestUT.setScale(1.e3*DAY2SEC);

	  // degree 1 and 2:
	  Frame->eop()->p_Xp_m2()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m2()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p1()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p1()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p2()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p2()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c1()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s1()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_c2()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s2()->tuneParameter(CfgTestUT);

	  // degree 3:
	  Frame->eop()->p_Xp_m3()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m3()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p3()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p3()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c3()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s3()->tuneParameter(CfgTestUT);

	  // degree 4:
	  Frame->eop()->p_Xp_m4()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m4()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p4()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p4()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c4()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s4()->tuneParameter(CfgTestUT);
	  

	  // 4test (other high degrees):
	  Frame->eop()->p_Xp_m5()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m5()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p5()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p5()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c5()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s5()->tuneParameter(CfgTestUT);

	  Frame->eop()->p_Xp_m6()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m6()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p6()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p6()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c6()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s6()->tuneParameter(CfgTestUT);

	  Frame->eop()->p_Xp_m7()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m7()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p7()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p7()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c7()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s7()->tuneParameter(CfgTestUT);

	  Frame->eop()->p_Xp_m8()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m8()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p8()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p8()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c8()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s8()->tuneParameter(CfgTestUT);

	  Frame->eop()->p_Xp_m9()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m9()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p9()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p9()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c9()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s9()->tuneParameter(CfgTestUT);

	  Frame->eop()->p_Xp_m10()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_m10()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Xp_p10()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_Yp_p10()->tuneParameter(CfgTestXY);
	  Frame->eop()->p_UT_c10()->tuneParameter(CfgTestUT);
	  Frame->eop()->p_UT_s10()->tuneParameter(CfgTestUT);
	  //

	  
	  // degree 1 and 2:
	  List->inSort(Frame->eop()->p_Xp_m2());
	  List->inSort(Frame->eop()->p_Yp_m2());
	  List->inSort(Frame->eop()->p_Xp_p1());
	  List->inSort(Frame->eop()->p_Yp_p1());
	  List->inSort(Frame->eop()->p_Xp_p2());
	  List->inSort(Frame->eop()->p_Yp_p2());
	  List->inSort(Frame->eop()->p_UT_c1());
	  List->inSort(Frame->eop()->p_UT_s1());
	  List->inSort(Frame->eop()->p_UT_c2());
	  List->inSort(Frame->eop()->p_UT_s2());

	  // degree 3:
	  /**/
	  List->inSort(Frame->eop()->p_Xp_m3());
	  List->inSort(Frame->eop()->p_Yp_m3());
	  List->inSort(Frame->eop()->p_Xp_p3());
	  List->inSort(Frame->eop()->p_Yp_p3());
	  List->inSort(Frame->eop()->p_UT_c3());
	  List->inSort(Frame->eop()->p_UT_s3());
	  /**/
	  
	  // degree 4:
	  /*
	    List->inSort(Frame->eop()->p_Xp_m4());
	    List->inSort(Frame->eop()->p_Yp_m4());
	    List->inSort(Frame->eop()->p_Xp_p4());
	    List->inSort(Frame->eop()->p_Yp_p4());
	    List->inSort(Frame->eop()->p_UT_c4());
	    List->inSort(Frame->eop()->p_UT_s4());
	  */
	  
	  // 4test (other high degrees):
	  /*
	    List->inSort(Frame->eop()->p_Xp_m5());
	    List->inSort(Frame->eop()->p_Yp_m5());
	    List->inSort(Frame->eop()->p_Xp_p5());
	    List->inSort(Frame->eop()->p_Yp_p5());
	    List->inSort(Frame->eop()->p_UT_c5());
	    List->inSort(Frame->eop()->p_UT_s5());
	    
	    List->inSort(Frame->eop()->p_Xp_m6());
	    List->inSort(Frame->eop()->p_Yp_m6());
	    List->inSort(Frame->eop()->p_Xp_p6());
	    List->inSort(Frame->eop()->p_Yp_p6());
	    List->inSort(Frame->eop()->p_UT_c6());
	    List->inSort(Frame->eop()->p_UT_s6());
	    
	    List->inSort(Frame->eop()->p_Xp_m7());
	    List->inSort(Frame->eop()->p_Yp_m7());
	    List->inSort(Frame->eop()->p_Xp_p7());
	    List->inSort(Frame->eop()->p_Yp_p7());
	    List->inSort(Frame->eop()->p_UT_c7());
	    List->inSort(Frame->eop()->p_UT_s7());
	    
	    List->inSort(Frame->eop()->p_Xp_m8());
	    List->inSort(Frame->eop()->p_Yp_m8());
	    List->inSort(Frame->eop()->p_Xp_p8());
	    List->inSort(Frame->eop()->p_Yp_p8());
	    List->inSort(Frame->eop()->p_UT_c8());
	    List->inSort(Frame->eop()->p_UT_s8());
	    
	    List->inSort(Frame->eop()->p_Xp_m9());
	    List->inSort(Frame->eop()->p_Yp_m9());
	    List->inSort(Frame->eop()->p_Xp_p9());
	    List->inSort(Frame->eop()->p_Yp_p9());
	    List->inSort(Frame->eop()->p_UT_c9());
	    List->inSort(Frame->eop()->p_UT_s9());
	    
	    List->inSort(Frame->eop()->p_Xp_m10());
	    List->inSort(Frame->eop()->p_Yp_m10());
	    List->inSort(Frame->eop()->p_Xp_p10());
	    List->inSort(Frame->eop()->p_Yp_p10());
	    List->inSort(Frame->eop()->p_UT_c10());
	    List->inSort(Frame->eop()->p_UT_s10());
	  */
	};
      
      /*--
	if (Prj->cfg().p().test().type() == Type)
	{
	Frame->eop()->p_Ox()->tuneParameter(Prj->cfg().p().test());
	Frame->eop()->p_Ox()->setScale(1.e3*RAD2SEC);
	List->inSort(Frame->eop()->p_Ox());
	
	Frame->eop()->p_Oy()->tuneParameter(Prj->cfg().p().test());
	Frame->eop()->p_Oy()->setScale(1.e3*RAD2SEC);
	List->inSort(Frame->eop()->p_Oy());
	
	Frame->eop()->p_Oz()->tuneParameter(Prj->cfg().p().test());
	Frame->eop()->p_Oz()->setScale(1.e3*RAD2SEC);
	//List->inSort(Frame->eop()->p_Oz());
	};
	--*/
      
    };
  for (SBParameter *p=List->first(); p; p=List->next())
    p->addAttr(SBDerivation::IsInRun);
};

void SBRunManager::makeReports()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": starting reports for the project " + Prj->name());

  makeReportSessionStatistics();

  if (Prj->cfg().reportMaps())
    makeReportMaps();
  
  if (Prj->cfg().reportCRF_Changes())
    {
      //makeReportCRFVariations();
      makeReportCRFVariations4IVS();
    };

  if (Prj->cfg().reportTRF_Changes())
    makeReportTRFVariations();
  
  if (Prj->cfg().reportCRF())
    makeReportCRF();
  
  if (Prj->cfg().reportTRF_SINEX())
    makeReportTRF();
  
  if (Prj->cfg().reportEOP())
    makeReportEOP();
  
  if (Prj->cfg().reportTechDesc())
    makeReportTechDescription();
  
  if (Prj->cfg().reportNormalEqs())
    makeReportNormalEqs();
  
  if (Prj->cfg().reportTroposhere())
    makeReportTroposphere();
  
  Log->write(SBLog::INF, SBLog::RUN, ClassName() + ": finished reports for the project " + Prj->name());
};
/*==============================================================================================*/



/*==============================================================================================*/
