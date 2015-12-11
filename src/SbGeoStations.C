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

#include "SbGeoStations.H"
#include "SbGeoSources.H"

#include <qmessagebox.h>
#include <qfile.h>
#include <qregexp.h>

#include "SbSetup.H"
#include "SbGeoAtmLoad.H"
#include "SbGeoParameter.H"
#include "SbGeoProject.H"





/*==============================================================================================*/
/*												*/
/* constants description									*/
/*												*/
/*==============================================================================================*/
const SBStationID SidZero(0, SBStation::Pt_Antenna, 0); // 00000S000, not valid

const SBPlateEntry SBPMNUVEL1A::NUVEL1A[14]=
{
  SBPlateEntry("AFRC",  2.401, -7.939, 13.893),
  SBPlateEntry("ANTA",  0.689, -6.541, 13.676),
  SBPlateEntry("ARAB",  8.195, -5.361, 16.730),
  SBPlateEntry("AUST",  9.349,  0.284, 16.253),
  SBPlateEntry("CARB",  1.332, -8.225, 11.551),
  SBPlateEntry("COCO", -8.915,-26.445, 20.896),
  SBPlateEntry("EURA",  0.529, -7.236, 13.123),
  SBPlateEntry("INDI",  8.180, -4.800, 16.760),
  SBPlateEntry("JDEF",  6.51 ,  3.17 ,  5.08 ),
  SBPlateEntry("NAZC", -0.022,-13.417, 19.579),
  SBPlateEntry("NOAM",  1.768, -8.439,  9.817),
  SBPlateEntry("PCFC",  0.0  ,  0.0  ,  0.0  ),
  SBPlateEntry("PHIL", 11.4  , 12.2  ,  0.0  ),
  SBPlateEntry("SOAM",  0.472, -6.355,  9.100)
};

const SBPlateEntry SBPMNNR_NUVEL1A::NNR_NUVEL1A[16]=
{
  SBPlateEntry("AFRC",  0.891, -3.099,  3.922),
  SBPlateEntry("ANTA", -0.821, -1.701,  3.706),
  SBPlateEntry("ARAB",  6.685, -0.521,  6.760),
  SBPlateEntry("AUST",  7.839,  5.124,  6.282),
  SBPlateEntry("CARB", -0.178, -3.385,  1.581),
  SBPlateEntry("COCO",-10.425,-21.605, 10.925),
  SBPlateEntry("EURA", -0.981, -2.395,  3.153),
  SBPlateEntry("INDI",  6.670,  0.040,  6.790),
  SBPlateEntry("JUFU",  5.200,  8.610, -5.820),
  SBPlateEntry("NAZC", -1.532, -8.577,  9.609),
  SBPlateEntry("NOAM",  0.258, -3.599, -0.153),
  SBPlateEntry("PCFC", -1.510,  4.840, -9.970),
  SBPlateEntry("PHIL", 10.090, -7.160, -9.670),
  SBPlateEntry("RIVR", -9.390,-30.960, 12.050), //??? 12.050 || -12.050????
  SBPlateEntry("SCOT", -0.410, -2.660, -1.270),
  SBPlateEntry("SOAM", -1.038, -1.515, -0.870)
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOLoad implementation									*/
/*												*/
/*==============================================================================================*/
SBOLoad::SBOLoad()
{
  Info		= "No tides";
  CommentLine1  = " ";
  CommentLine2  = " ";
  PickedUp	= P_NA;
  Dist		= 1024.0E3; // 1km
  for (int c=0; c<3; c++) for (int j=0; j<11; j++) 
    {
      Amplitude[c][j] = 0.0;
      Phase    [c][j] = 0.0;
    };
};

SBOLoad::SBOLoad(const SBOLoad& Ol)
{
  Info		= (const char*)Ol.Info;
  CommentLine1  = (const char*)Ol.CommentLine1;
  CommentLine2  = (const char*)Ol.CommentLine2;
  PickedUp	= Ol.PickedUp;
  Dist		= Ol.Dist;
  for (int c=0; c<3; c++) for (int j=0; j<11; j++) 
    {
      Amplitude[c][j] = Ol.Amplitude[c][j];
      Phase    [c][j] = Ol.Phase    [c][j];
    };
};

SBOLoad& SBOLoad::operator= (const SBOLoad& Ol)
{
  Info		= (const char*)Ol.Info;
  CommentLine1  = (const char*)Ol.CommentLine1;
  CommentLine2  = (const char*)Ol.CommentLine2;
  PickedUp	= Ol.PickedUp;
  Dist		= Ol.Dist;
  for (int c=0; c<3; c++) for (int j=0; j<11; j++)
    {
      Amplitude[c][j] = Ol.Amplitude[c][j];
      Phase    [c][j] = Ol.Phase    [c][j];
    };
  return *this;
};

bool SBOLoad::operator==(const SBOLoad& Ol) const 
{
  if (PickedUp	   != Ol.PickedUp)	return FALSE;
  if (Info         != Ol.Info)		return FALSE;
  if (CommentLine1 != Ol.CommentLine1)	return FALSE;
  if (CommentLine2 != Ol.CommentLine2)	return FALSE;
  if (Dist	   != Ol.Dist)		return FALSE;
  for (int c=0; c<3; c++) for (int j=0; j<11; j++) 
    {
      if (Amplitude[c][j] != Ol.Amplitude[c][j]) return FALSE;
      if (Phase    [c][j] != Ol.Phase    [c][j]) return FALSE;
    };
  return TRUE;
};

bool SBOLoad::operator!=(const SBOLoad& Ol) const 
{
  return !(Ol==*this);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOLoad's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBOLoad& Ol)
{
  s << (uint)Ol.PickedUp << Ol.Info << Ol.CommentLine1 << Ol.CommentLine2 << Ol.Dist;
  s.writeRawBytes((const char*)&Ol.Amplitude, 3*11*sizeof(double));
  return s.writeRawBytes((const char*)&Ol.Phase, 3*11*sizeof(double));
};

QDataStream& operator>>(QDataStream& s, SBOLoad& Ol)
{
  s >> (uint&)Ol.PickedUp >> Ol.Info >> Ol.CommentLine1 >> Ol.CommentLine2 >> Ol.Dist;
  s.readRawBytes((char*)&Ol.Amplitude, 3*11*sizeof(double));
  return s.readRawBytes((char*)&Ol.Phase, 3*11*sizeof(double));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPlateEntry implementation								*/
/*												*/
/*==============================================================================================*/
SBPlateEntry::SBPlateEntry(const QString& Name_, double Ox, double Oy, double Oz)
  : SBNamed(Name_)
{
  Omega[0]  = Ox;
  Omega[1]  = Oy;
  Omega[2]  = Oz;
};

Vector3	SBPlateEntry::velocity(const Vector3& R) const
{
  Vector3	D(v3Zero);

  D[X_AXIS] = Omega[Y_AXIS]*R.at(Z_AXIS) - Omega[Z_AXIS]*R.at(Y_AXIS);
  D[Y_AXIS] = Omega[Z_AXIS]*R.at(X_AXIS) - Omega[X_AXIS]*R.at(Z_AXIS);
  D[Z_AXIS] = Omega[X_AXIS]*R.at(Y_AXIS) - Omega[Y_AXIS]*R.at(X_AXIS);

  return D*1.0e-9; //nrad to rad
};

Vector3	SBPlateEntry::displacement(long double dt, const Vector3& R) const
{
  return velocity(R)*dt/365.25; //days to years
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPlateMotion implementation								*/
/*												*/
/*==============================================================================================*/
SBPlateMotion::SBPlateMotion(const SBPlateEntry Mod_[], int N_)
  : SBNList()
{
  Plates = Mod_;
  N	 = N_;
  PlDict = new QDict<SBPlateEntry>(N+5, FALSE);
  PlDict -> setAutoDelete(FALSE);
  for (int i=0; i<N; i++)
    PlDict->insert(Plates[i].name(), &Plates[i]);
};
  
SBPlateMotion::~SBPlateMotion()
{
  if (PlDict) delete PlDict;
};

Vector3	SBPlateMotion::displacement(const SBMJD& T_, const SBMJD& T0_, SBStation& Station) const
{
  SBPlateEntry	*e=NULL;
  Vector3	Disp;
  QString	Tmp("tmp");

  if (!PlDict->count()) // zero plate tectonic motion model
    Disp = v3Zero;
  else if (!Station.site())
    {
      Log->write(SBLog::ERR, SBLog::PLATES, ClassName() + ": got a siteless station [" +
		 Station.name() + "]; cannot define plate ID");
      return v3Zero;
    }
  else if (!(e=PlDict->find(Station.site()->plate())))
    {
      Log->write(SBLog::ERR, SBLog::PLATES, ClassName() + ": cannot find a plate for the station [" +
		 Station.name() + "]");
      return v3Zero;
    }
  else 
    Disp = e->displacement(T_-T0_, Station.rt());
  
  if (Log->isEligible(SBLog::DBG, SBLog::PLATES))
    {
      Tmp.sprintf("; displ.(m): %.4f, %.4f, %.4f; time interval: %.6f (days) ", 
		  Disp.at(X_AXIS), Disp.at(Y_AXIS), Disp.at(Z_AXIS), (double)(T_-T0_));
      Log->write(SBLog::DBG, SBLog::PLATES, ClassName() + 
		 ": Station : " + Station.name() + " [" + Station.id().toString() + 
		 "], PlateID : " + (e?e->name():QString("NONE")) + Tmp);
    };
  return Disp;
};

Vector3	SBPlateMotion::velocity(SBStation& Station) const
{
  SBPlateEntry	*e=NULL;
  Vector3	Disp;
  QString	Tmp("tmp");

  if (!PlDict->count()) // zero plate tectonic motion model
    Disp = v3Zero;
  else if (!Station.site())
    {
      Log->write(SBLog::ERR, SBLog::PLATES, ClassName() + ": got a siteless station [" +
		 Station.name() + "]; cannot define plate ID");
      return v3Zero;
    }
  else if (!(e=PlDict->find(Station.site()->plate())))
    {
      Log->write(SBLog::ERR, SBLog::PLATES, ClassName() + ": cannot find a plate for the station [" +
		 Station.name() + "]");
      return v3Zero;
    }
  else 
    Disp = e->velocity(Station.r_first());
  
  if (Log->isEligible(SBLog::DBG, SBLog::PLATES))
    {
      Tmp.sprintf("; vel.(m/y): %.4f, %.4f, %.4f", 
		  Disp.at(X_AXIS), Disp.at(Y_AXIS), Disp.at(Z_AXIS));
      Log->write(SBLog::DBG, SBLog::PLATES, ClassName() + 
		 ": Station : " + Station.name() + " [" + Station.id().toString() + 
		 "], PlateID : " + (e?e->name():QString("NONE")) + Tmp);
    };
  return Disp;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBCoordinates implementation								*/
/*												*/
/*==============================================================================================*/
SBCoordinates::SBCoordinates()
  : SBMJD(TZero), SBAttributed()
{
  Coo		= v3Zero;
  CooErrors	= Vector3(1.0, 1.0, 1.0);
  Vel		= v3Zero;
  VelErrors	= Vector3(1.0, 1.0, 1.0);
  EventName	= "Initial data";
  IsUseVels	= FALSE;
};

SBCoordinates::SBCoordinates(const Vector3& R, const Vector3& dR,
			     const Vector3& V, const Vector3& dV,
			     const SBMJD& T_, const QString& en_)
  : SBMJD(T_), SBAttributed()
{
  Coo		= R;
  CooErrors	= dR;
  Vel		= V;
  VelErrors	= dV;
  EventName	= (const char*)en_;
  IsUseVels	= FALSE;
};

SBCoordinates::SBCoordinates(const SBCoordinates& C_)
  : SBMJD(C_), SBAttributed(C_)
{
  Coo		= C_.coo();
  CooErrors	= C_.cooErrors();
  Vel		= C_.vel();
  VelErrors	= C_.velErrors();
  EventName	= (const char*)C_.eventName();
  IsUseVels	= C_.isUseVels();
};

SBCoordinates& SBCoordinates::operator=(const SBCoordinates& C_)
{
  SBMJD::operator=(C_);
  SBAttributed::operator=(C_);
  Coo		= C_.coo();
  CooErrors	= C_.cooErrors();
  Vel		= C_.vel();
  VelErrors	= C_.velErrors();
  EventName	= (const char*)C_.eventName();
  IsUseVels	= C_.isUseVels();
  return *this;
};

bool SBCoordinates::operator==(const SBCoordinates& C_) const
{
  return 
    ((SBMJD&)*this == C_) && (attributes() == C_.attributes()) &&
    (Coo == C_.coo()) && (CooErrors == C_.cooErrors()) &&
    (Vel == C_.vel()) && (VelErrors == C_.velErrors()) &&
    (EventName == C_.eventName()) && (IsUseVels == C_.isUseVels());
};

bool SBCoordinates::operator<(const SBCoordinates& C_) const
{
  return (SBMJD&)*this < C_;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBReferencePoint implementation							*/
/*												*/
/*==============================================================================================*/
SBReferencePoint::SBReferencePoint(const SBReferencePoint& RP_) : 
  QList<SBCoordinates>()
{
  setAutoDelete(TRUE);
  clear();
  // make a deep copy:
  if (RP_.count())
    {
      QListIterator<SBCoordinates> it(RP_);
      while (it.current())
	{
	  append(new SBCoordinates(*it.current()));
	  ++it;
	};
    };
  sort();
  checkRanges();
};

SBReferencePoint& SBReferencePoint::operator= (const SBReferencePoint& RP)
{
  // make a deep copy:
  clear();
  if (RP.count())
    {
      QListIterator<SBCoordinates> it(RP);
      while (it.current())
	{
	  append(new SBCoordinates(*it.current()));
	  ++it;
	};
    };
  sort();
  checkRanges();
  return *this;
};

SBCoordinates* SBReferencePoint::findCoords(const SBMJD& T)
{
  SBCoordinates*	coo = NULL;
  if (count())
    {
      SBCoordinates *c = first();
      do
	coo = c;
      while ((c=next()) && *c<T);
    };
  
  return coo;
};

bool SBReferencePoint::registerCoords(const Vector3& R_, const Vector3& dR_, 
				      const Vector3& V_, const Vector3& dV_, 
				      const SBMJD& T_, const QString& Name_)
{
  if (R_==v3Zero)
    return FALSE;

  SBCoordinates*	coo  = NULL;

  if (!count()) // first entry
    {
      inSort((coo = new SBCoordinates(R_, dR_, V_, dV_, T_, Name_)));
      R_0  = &coo->coo();
      DR_0 = &coo->cooErrors();
      V_0  = &coo->vel();
      DV_0 = &coo->velErrors();
      return TRUE;
    }
  else if ( !(coo=findCoords(T_)) )
    inSort(new SBCoordinates(R_, dR_, V_, dV_, T_, Name_));
  else if (coo->epoch()==T_)
    {
      coo->setCoo(R_);
      coo->setCooErrors(dR_);
      coo->setVel(V_);
      coo->setVelErrors(dV_);
      coo->setEventName(Name_);
    }
  else
    {
      inSort(new SBCoordinates(R_, dR_, V_, dV_, T_, Name_));
    }
  checkRanges();
  return TRUE;
};

const Vector3&	SBReferencePoint::r(const SBMJD& t)
{
  SBCoordinates	*coo = findCoords(t);
  return coo?coo->coo():v3Zero;
};

const Vector3&	SBReferencePoint::dR(const SBMJD& t)
{
  SBCoordinates	*coo = findCoords(t);
  return coo?coo->cooErrors():v3Zero;
};

const Vector3&	SBReferencePoint::v(const SBMJD& t)
{
  SBCoordinates	*coo = findCoords(t);
  return coo?(coo->isUseVels()?coo->vel():v3Zero):v3Zero;
};

const Vector3&	SBReferencePoint::dV(const SBMJD& t)
{
  SBCoordinates	*coo = findCoords(t);
  return coo?coo->velErrors():v3Zero;
};

void SBReferencePoint::checkRanges()
{
  R_0 = NULL;
  DR_0= NULL;
  V_0 = NULL;
  DV_0= NULL;
  if (count())
    {
      R_0  = &at(0)->coo();
      DR_0 = &at(0)->cooErrors();
      V_0  = &at(0)->vel();
      DV_0 = &at(0)->velErrors();
    };
  sort();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStation implementation								*/
/*												*/
/*==============================================================================================*/
SBStation::SBStation(SBSite* Site_, const char* Name_)
  : SBNamed(Name_), SBAttributed(), 
    RefPt(), Rt(v3Zero), Ecc(v3Zero),
    V(v3Zero), Vt(v3Zero), dV(1.0, 1.0, 1.0), FmVEN()
{
  Site		= Site_;
  CDPNum	= 9999;
  DomeMinor	= 0;
  Tech		= TECH_UNKN;
  PointType	= Pt_Antenna;
  ClassCoord	= Class_Unknown;
  Description	= "Null-T beacon 17.3W";
  Comments	= "A geodynamic station at Zabriski Point (near Irukansk), \n"
    "serves Irukan, Arkanar and Soam Geodynamics Test Areas \n"
    "(template)";
  MountType	= Mnt_Unkn;
  AxisOffset	= 0.0;
  CharID	= "UNKN";
  OLoad		= NULL; // use Site->OLoad instead
  Height	= 0.0;
  Longitude	= 0.0;
  Latitude	= 0.0;
  delAttr(hasOwnOLoad);
  
  for (int i=0; i<10; i++)
    P_Clock[i] = NULL;

  P_Zenith	= NULL;
  P_AtmGradN	= NULL;
  P_AtmGradE	= NULL;
  P_Axis	= NULL;
  P_DRX		= NULL;
  P_DRY		= NULL;
  P_DRZ		= NULL;
  P_DVX		= NULL;
  P_DVY		= NULL;
  P_DVZ		= NULL;
  P_Cable	= NULL;
  //
  P_TideLag	= NULL;
  P_TideH2	= NULL;
  P_TideL2	= NULL;
  P_Aplo	= NULL;

#ifdef ELEV_DEBUG_OUTPUT
  OutFile	= NULL;
  OutStream	= NULL;
#endif
};

SBStation::SBStation(SBSite* Site_, const SBStation& S)
  : SBNamed(S.Name), SBAttributed(S),
    //    R(S.R), dR(S.dR),
    RefPt(S.RefPt),
    Rt(S.Rt), Ecc(S.Ecc), V(S.V), Vt(S.Vt), dV(S.dV), FmVEN(S.FmVEN)
{
  Site		= Site_;
  CDPNum	= S.CDPNum;
  DomeMinor	= S.DomeMinor;
  Tech		= S.Tech;
  PointType	= S.PointType;
  ClassCoord	= S.ClassCoord;
  Description	= (const char*)S.Description;
  Comments	= (const char*)S.Comments;
  MountType	= S.MountType;
  AxisOffset	= S.AxisOffset;
  CharID	= (const char*)S.CharID;
  if (S.OLoad) OLoad = new SBOLoad (*S.OLoad);  else OLoad = NULL;
  Height	= S.Height;
  Longitude	= S.Longitude;
  Latitude	= S.Latitude;

  for (int i=0; i<10; i++)
    P_Clock[i] = NULL;

  P_Zenith	= NULL;
  P_AtmGradN	= NULL;
  P_AtmGradE	= NULL;
  P_Axis	= NULL;
  P_DRX		= NULL;
  P_DRY		= NULL;
  P_DRZ		= NULL;
  P_DVX		= NULL;
  P_DVY		= NULL;
  P_DVZ		= NULL;
  P_Cable	= NULL;
  //
  P_TideLag	= NULL;
  P_TideH2	= NULL;
  P_TideL2	= NULL;
  P_Aplo	= NULL;

  releaseParameters();

#ifdef ELEV_DEBUG_OUTPUT
  OutFile	= NULL;
  OutStream	= NULL;
#endif
};

SBStation& SBStation::operator= (const SBStation& S)
{
  Name		= S.Name;
  SBAttributed::operator= (S);
  //  Site	= S.Site;	// we shouldn't change the owner implicitly
  //  DomeMinor	= S.DomeMinor;	// for same reason 
  //  PointType	= S.PointType;	// for same reason
  CDPNum	= S.CDPNum;
  Tech		= S.Tech;
  ClassCoord	= S.ClassCoord;
  Description	= (const char*)S.Description;
  Comments	= (const char*)S.Comments;
  MountType	= S.MountType;
  AxisOffset	= S.AxisOffset;
  CharID	= (const char*)S.CharID;
  if (OLoad) 
    {
      delete OLoad;
      OLoad = NULL;
    };
  if (S.OLoad) OLoad = new SBOLoad (*S.OLoad);
  // otherwice, OLoad should be copied in other place (somewhere in the SBSite code)...

  Height	= S.Height;
  Longitude	= S.Longitude;
  Latitude	= S.Latitude;
  FmVEN		= S.FmVEN;
  //  R		= S.R;
  //  dR		= S.dR;
  RefPt		= S.RefPt;
  Rt		= S.Rt;
  Ecc		= S.Ecc;
  V		= S.V;
  Vt		= S.Vt;
  dV		= S.dV;

  for (int i=0; i<10; i++)
    P_Clock[i] = NULL;

  P_Zenith	= NULL;
  P_AtmGradN	= NULL;
  P_AtmGradE	= NULL;
  P_Axis	= NULL;
  P_DRX		= NULL;
  P_DRY		= NULL;
  P_DRZ		= NULL;
  P_DVX		= NULL;
  P_DVY		= NULL;
  P_DVZ		= NULL;
  P_Cable	= NULL;
  //
  P_TideLag	= NULL;
  P_TideH2	= NULL;
  P_TideL2	= NULL;
  P_Aplo	= NULL;

  releaseParameters();

#ifdef ELEV_DEBUG_OUTPUT
  OutFile	= NULL;
  OutStream	= NULL;
#endif
  return *this;
};

SBStationID SBStation::id() const
{
  SBStationID ID;
  if (Site) ID.setDomeMajor(Site->domeMajor());
  else ID.setDomeMajor(99999);
  ID.setPtType(PointType);
  ID.setDomeMinor(DomeMinor);
  return ID;
};

void SBStation::createParameters()
{
  QString Prefix("St: " + id().toString() + ": ");

  for (int i=0; i<10; i++)
    {
      if (P_Clock[i]) 
	delete P_Clock[i];
      P_Clock[i] = new SBParameter(Prefix + QString().sprintf("Clock_%d", i));
    };

  if (P_Zenith) delete P_Zenith;
  P_Zenith = new SBParameter(Prefix + "Zenith ");
  if (P_AtmGradN) delete P_AtmGradN;
  P_AtmGradN = new SBParameter(Prefix + "Grad N ");
  if (P_AtmGradE) delete P_AtmGradE;
  P_AtmGradE = new SBParameter(Prefix + "Grad E ");
  if (P_Axis) delete P_Axis;
  P_Axis = new SBParameter  (Prefix + "AxisOff");

  if (P_DRX) delete P_DRX;
  P_DRX = new SBParameter   (Prefix + "coord-X");
  if (P_DRY) delete P_DRY;
  P_DRY = new SBParameter   (Prefix + "coord-Y");
  if (P_DRZ) delete P_DRZ;
  P_DRZ = new SBParameter   (Prefix + "coord-Z");

  if (P_DVX) delete P_DVX;
  P_DVX = new SBParameter   (Prefix + "veloc-X");
  if (P_DVY) delete P_DVY;
  P_DVY = new SBParameter   (Prefix + "veloc-Y");
  if (P_DVZ) delete P_DVZ;
  P_DVZ = new SBParameter   (Prefix + "veloc-Z");
  if (P_Cable) delete P_Cable;
  P_Cable = new SBParameter (Prefix + "Cable  ");
  //
  if (P_TideLag) delete P_TideLag;
  P_TideLag = new SBParameter(Prefix + "TideLag");
  if (P_TideH2) delete P_TideH2;
  P_TideH2 = new SBParameter(Prefix + "TideH2 ");
  if (P_TideL2) delete P_TideL2;
  P_TideL2 = new SBParameter(Prefix + "TideL2 ");

  if (P_Aplo) delete P_Aplo;
  P_Aplo = new SBParameter(Prefix + "AploCf ");

};

void SBStation::releaseParameters()
{
  for (int i=0; i<10; i++)
    {
      if (P_Clock[i]) 
	delete P_Clock[i];
      P_Clock[i] = NULL;
    };
  if (P_Zenith) 
    {
      delete P_Zenith;
      P_Zenith = NULL;
    };
  if (P_AtmGradN) 
    {
      delete P_AtmGradN;
      P_AtmGradN = NULL;
    };
  if (P_AtmGradE) 
    {
      delete P_AtmGradE;
      P_AtmGradE = NULL;
    };
  if (P_Axis) 
    {
      delete P_Axis;
      P_Axis = NULL;
    };

  if (P_DRX) 
    {
      delete P_DRX;
      P_DRX = NULL;
    };
  if (P_DRY) 
    {
      delete P_DRY;
      P_DRY = NULL;
    };
  if (P_DRZ) 
    {
      delete P_DRZ;
      P_DRZ = NULL;
    };

  if (P_DVX) 
    {
      delete P_DVX;
      P_DVX = NULL;
    };
  if (P_DVY) 
    {
      delete P_DVY;
      P_DVY = NULL;
    };
  if (P_DVZ) 
    {
      delete P_DVZ;
      P_DVZ = NULL;
    };
  if (P_Cable)
    {
      delete P_Cable;
      P_Cable = NULL;
    };
  //
  if (P_TideLag) 
    {
      delete P_TideLag;
      P_TideLag = NULL;
    };
  if (P_TideH2)
    {
      delete P_TideH2;
      P_TideH2 = NULL;
    };
  if (P_TideL2) 
    {
      delete P_TideL2;
      P_TideL2 = NULL;
    };
  if (P_Aplo) 
    {
      delete P_Aplo;
      P_Aplo = NULL;
    };
};

void SBStation::updateParameters(SBRunManager* Mgr, const SBMJD& T, bool IsRealRun)
{
  if (Mgr)
    {
      Mgr->solution()->updateParameter(P_Clock[0], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[1], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[2], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[3], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[4], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[5], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[6], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[7], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[8], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Clock[9], T, IsRealRun);
      Mgr->solution()->updateParameter(P_Zenith,   T, IsRealRun);
      Mgr->solution()->updateParameter(P_AtmGradN, T, IsRealRun);
      Mgr->solution()->updateParameter(P_AtmGradE, T, IsRealRun);
      Mgr->solution()->updateParameter(P_Axis,T, IsRealRun);
      Mgr->solution()->updateParameter(P_DRX, T, IsRealRun);
      Mgr->solution()->updateParameter(P_DRY, T, IsRealRun);
      Mgr->solution()->updateParameter(P_DRZ, T, IsRealRun);
      Mgr->solution()->updateParameter(P_DVX, T, IsRealRun);
      Mgr->solution()->updateParameter(P_DVY, T, IsRealRun);
      Mgr->solution()->updateParameter(P_DVZ, T, IsRealRun);
      Mgr->solution()->updateParameter(P_Cable, T, IsRealRun);
      //
      Mgr->solution()->updateParameter(P_TideLag, T, IsRealRun);
      Mgr->solution()->updateParameter(P_TideH2, T, IsRealRun);
      Mgr->solution()->updateParameter(P_TideL2, T, IsRealRun);
      Mgr->solution()->updateParameter(P_Aplo, T, IsRealRun);
    };
};

Vector3 SBStation::vModel(SBRunManager* Mgr)
{
  Vector3	Vm(v3Zero);
  if (Mgr->prj()->cfg().plateMotion()==SBConfig::PM_NONE && Mgr->prj()->cfg().isUseStationVelocity())
    {
      Vm = V;
      if (Mgr->prj()->cfg().isRollBack2Plates() && Vm==v3Zero)
	Vm = Mgr->refPlates()->velocity(*this);
    }
  else
    Vm = Mgr->plates()->velocity(*this);
  return Vm;
};

void SBStation::getGlobalParameters4Report(SBRunManager* Mgr)
{
  if (Mgr)
    {
      Mgr->solution()->getGlobalParameter4Report(P_Clock[0]);
      Mgr->solution()->getGlobalParameter4Report(P_Clock[1]);
      Mgr->solution()->getGlobalParameter4Report(P_Clock[2]);//???
      Mgr->solution()->getGlobalParameter4Report(P_Zenith);
      Mgr->solution()->getGlobalParameter4Report(P_AtmGradN);
      Mgr->solution()->getGlobalParameter4Report(P_AtmGradE);
      Mgr->solution()->getGlobalParameter4Report(P_Axis);
      Mgr->solution()->getGlobalParameter4Report(P_DRX);
      Mgr->solution()->getGlobalParameter4Report(P_DRY);
      Mgr->solution()->getGlobalParameter4Report(P_DRZ);
      Mgr->solution()->getGlobalParameter4Report(P_DVX);
      Mgr->solution()->getGlobalParameter4Report(P_DVY);
      Mgr->solution()->getGlobalParameter4Report(P_DVZ);
      //
      Mgr->solution()->getGlobalParameter4Report(P_TideLag);
      Mgr->solution()->getGlobalParameter4Report(P_TideH2);
      Mgr->solution()->getGlobalParameter4Report(P_TideL2);
      Mgr->solution()->getGlobalParameter4Report(P_Aplo);
    };

  Rt = RefPt.r(P_DRX->tEpoch());
  
  Rt[X_AXIS] += P_DRX->v();
  Rt[Y_AXIS] += P_DRY->v();
  Rt[Z_AXIS] += P_DRZ->v();  

  if (Mgr->prj()->cfg().plateMotion()==SBConfig::PM_NONE && Mgr->prj()->cfg().isUseStationVelocity())
    {
      Vector3 V_aux = RefPt.v(P_DVX->tEpoch());
      Vt = V_aux==v3Zero?V:V_aux;
      if (Mgr->prj()->cfg().isRollBack2Plates() && Vt==v3Zero)
	Vt = Mgr->refPlates()->velocity(*this);
    }
  else
    Vt = Mgr->plates()->velocity(*this);

  Vt[X_AXIS] += P_DVX->v();
  Vt[Y_AXIS] += P_DVY->v();
  Vt[Z_AXIS] += P_DVZ->v();

};

void SBStation::getGlobalParameters4Report4Axel(SBRunManager* Mgr)
{
  // quick and dirty
  Vector3	Rtmp;
  if (Mgr)
    {
      Mgr->solution()->getGlobalParameter4Report(P_Clock[0]);
      Mgr->solution()->getGlobalParameter4Report(P_Clock[1]);
      Mgr->solution()->getGlobalParameter4Report(P_Clock[2]);//???
      Mgr->solution()->getGlobalParameter4Report(P_Zenith);
      Mgr->solution()->getGlobalParameter4Report(P_AtmGradN);
      Mgr->solution()->getGlobalParameter4Report(P_AtmGradE);
      Mgr->solution()->getGlobalParameter4Report(P_Axis);
      Mgr->solution()->getGlobalParameter4Report(P_DRX);
      Mgr->solution()->getGlobalParameter4Report(P_DRY);
      Mgr->solution()->getGlobalParameter4Report(P_DRZ);
      Mgr->solution()->getGlobalParameter4Report(P_DVX);
      Mgr->solution()->getGlobalParameter4Report(P_DVY);
      Mgr->solution()->getGlobalParameter4Report(P_DVZ);
      //
      Mgr->solution()->getGlobalParameter4Report(P_TideLag);
      Mgr->solution()->getGlobalParameter4Report(P_TideH2);
      Mgr->solution()->getGlobalParameter4Report(P_TideL2);
      Mgr->solution()->getGlobalParameter4Report(P_Aplo);
    };

  for (SBCoordinates *Coo=RefPt.first(); Coo; Coo=RefPt.next())
    {
      Rtmp = Coo->coo();
      Rtmp[X_AXIS] += P_DRX->v();
      Rtmp[Y_AXIS] += P_DRY->v();
      Rtmp[Z_AXIS] += P_DRZ->v();  
      Coo->setCoo(Rtmp);
      Rtmp[X_AXIS] = std::max(0.001, P_DRX->e());
      Rtmp[Y_AXIS] = std::max(0.001, P_DRY->e());
      Rtmp[Z_AXIS] = std::max(0.001, P_DRZ->e());
      Coo->setCooErrors(Rtmp);

      if (P_DRX->num()==0)
	Coo->addAttr(SBCoordinates::notValid);
    };

  if (Mgr->prj()->cfg().plateMotion()==SBConfig::PM_NONE && Mgr->prj()->cfg().isUseStationVelocity())
    {
      //      Vt = V;
      Vector3 V_aux = RefPt.v(P_DVX->tEpoch());
      Vt = V_aux==v3Zero?V:V_aux;
      if (Mgr->prj()->cfg().isRollBack2Plates() && Vt==v3Zero)
	{
	  Vt = Mgr->refPlates()->velocity(*this);
	  dV = Vector3(0.1,0.1,0.1);
	};
    }
  else
    {
      Vt = Mgr->plates()->velocity(*this);
      dV = Vector3(0.2,0.2,0.2);
    };

  Vt[X_AXIS] += P_DVX->v();
  Vt[Y_AXIS] += P_DVY->v();
  Vt[Z_AXIS] += P_DVZ->v();
  
  V = Vt;
  if (P_DVX->num())
    {
      dV[X_AXIS] = std::max(0.0001, P_DVX->e());
      dV[Y_AXIS] = std::max(0.0001, P_DVY->e());
      dV[Z_AXIS] = std::max(0.0001, P_DVZ->e());
    };
  //  else
  //    dV = Vector3(1.0,1.0,1.0);
};

// updates coordinates and velocities from the solution.
void SBStation::getGlobalParameters4UpdateTRF(SBRunManager* Mgr)
{
  Vector3	Rtmp;
  Vector3	Vtmp;
  bool		IsVelsInRefPt=FALSE;
  if (Mgr)
    {
      Mgr->solution()->getGlobalParameter4Report(P_Axis);
      Mgr->solution()->getGlobalParameter4Report(P_DRX);
      Mgr->solution()->getGlobalParameter4Report(P_DRY);
      Mgr->solution()->getGlobalParameter4Report(P_DRZ);
      Mgr->solution()->getGlobalParameter4Report(P_DVX);
      Mgr->solution()->getGlobalParameter4Report(P_DVY);
      Mgr->solution()->getGlobalParameter4Report(P_DVZ);
    };

  for (SBCoordinates *Coo=RefPt.first(); Coo; Coo=RefPt.next())
    {
      Rtmp = Coo->coo();
      Vtmp = Coo->vel();
      // coordinates in the list:
      Rtmp[X_AXIS] += P_DRX->v();
      Rtmp[Y_AXIS] += P_DRY->v();
      Rtmp[Z_AXIS] += P_DRZ->v();
      // velocities in the list:
      Vtmp[X_AXIS] += P_DVX->v();
      Vtmp[Y_AXIS] += P_DVY->v();
      Vtmp[Z_AXIS] += P_DVZ->v();

      Coo->setCoo(Rtmp);
      Rtmp[X_AXIS] = std::max(0.001, P_DRX->e());
      Rtmp[Y_AXIS] = std::max(0.001, P_DRY->e());
      Rtmp[Z_AXIS] = std::max(0.001, P_DRZ->e());
      Coo->setCooErrors(Rtmp);

      if (Coo->isUseVels()) // if the velocities are from RefPt's list (e.g., GILCREEK):
	{
	  Coo->setVel(Vtmp);
	  Vtmp[X_AXIS] = std::max(0.001, P_DVX->e());
	  Vtmp[Y_AXIS] = std::max(0.001, P_DVY->e());
	  Vtmp[Z_AXIS] = std::max(0.001, P_DVZ->e());
	  Coo->setVelErrors(Vtmp);
	  IsVelsInRefPt = TRUE;
	};
      
      if (P_DRX->num()<=2)
	Coo->addAttr(SBCoordinates::notValid);
    };

  if (!IsVelsInRefPt) // if the velocities not reffered to RefPt's list (common case):
    {
      if (Mgr->prj()->cfg().plateMotion()==SBConfig::PM_NONE && Mgr->prj()->cfg().isUseStationVelocity())
	{
	  //      Vt = V;
	  Vector3 V_aux = RefPt.v(P_DVX->tEpoch());
	  Vt = V_aux==v3Zero?V:V_aux;
	  if (Mgr->prj()->cfg().isRollBack2Plates() && Vt==v3Zero)
	    {
	      Vt = Mgr->refPlates()->velocity(*this);
	      dV = Vector3(0.1,0.1,0.1);
	    };
	}
      else
	{
	  Vt = Mgr->plates()->velocity(*this);
	  dV = Vector3(0.2,0.2,0.2);
	};
      
      Vt[X_AXIS] += P_DVX->v();
      Vt[Y_AXIS] += P_DVY->v();
      Vt[Z_AXIS] += P_DVZ->v();
      
      V = Vt;
      if (P_DVX->num())
	{
	  dV[X_AXIS] = std::max(0.0001, P_DVX->e());
	  dV[Y_AXIS] = std::max(0.0001, P_DVY->e());
	  dV[Z_AXIS] = std::max(0.0001, P_DVZ->e());
	};
    };
};

void SBStation::calcDisplacement(SBRunManager* Mgr, const SBMJD& T, bool IsRealRun)
{
  // takes into account the eccentricities:
  if (Log->isEligible(SBLog::DBG, SBLog::PLATES))
    Log->write(SBLog::DBG, SBLog::PLATES, ClassName() + ": Station: " + name() +
	       " [" + id().toString() + "]");

  Rt = IsRealRun?(RefPt.r(T) + Ecc):RefPt.r(T); // init R(t=0)
  

  //  SBStationID	ID;
  //  ID.setID("40408S002");
  //  if (id() == ID)
  //    {
  //      if (SBMJD(2002, 10, 22, 4, 0, 0.0)<=T && T<= SBMJD(2002, 10, 22, 11, 30, 0.0))
  //	{
  //	  Rt+=Vector3(0.0, 0.0, 0.1);
  //	};
  //    };
  
  // update parameters from a solution:
  updateParameters(Mgr, T, IsRealRun);
  Rt[X_AXIS] += P_DRX->v();
  Rt[Y_AXIS] += P_DRY->v();
  Rt[Z_AXIS] += P_DRZ->v();

  // adds tectonic plate motion (if exists):
  //  Rt += (*Mgr->plates())(T - Mgr->trf()->epoch(), *this);

  // takes into account the velocity of the station/site (if needs):
  if (Mgr->prj()->cfg().plateMotion()==SBConfig::PM_NONE && Mgr->prj()->cfg().isUseStationVelocity())
    {
      //      Vt = V;
      Vector3 V_aux = RefPt.v(T);
      Vt = V_aux==v3Zero?V:V_aux;
      if (Mgr->prj()->cfg().isRollBack2Plates() && Vt==v3Zero)
	Vt = Mgr->refPlates()->velocity(*this);
    }
  else
    Vt = Mgr->plates()->velocity(*this);

  Vt[X_AXIS] += P_DVX->v();
  Vt[Y_AXIS] += P_DVY->v();
  Vt[Z_AXIS] += P_DVZ->v();

  Rt += Vt*(T - Mgr->trf()->epoch())/365.25;
  
  // recalculate the actual Ro, Lambda and Phi and get the actual FmVEN matrix:
  // (?)
  calcRLF(Rt, Height, Longitude, Latitude);
  FmVEN = RotMatrix(NORT, -Longitude)*RotMatrix(EAST, Latitude);

  if (Mgr->aploEphem() && Mgr->prj()->cfg().isUseAploEphem() && T != TZero)
    {
      SBStationID ID(id());
      Rt += (FmVEN*Mgr->aploEphem()->displacement(ID, T))*(P_Aplo?P_Aplo->v():1.0);
    };
  
};

void SBStation::calcRLF(Vector3 R_, double& Ro, double& Lam, double& Fi)
{
  /*
    IERS TECHNICAL NOTE 21
    IERS Conventions (1996)
    page 12:
    
    Once the Cartesian coordinates (x,y,z) are known, they can be transformed to "datum" or
    curvilinear geodetic coordinates (lambda, fi, h) reffered to an ellipsoid of semi-major 
    axis a and flattening f, using the following code (Borkowski, 1989). First compute 
    lambda=tan^(-1)(y/x) properly determining the quadrant from x and y (0<=lamda<=2pi)
  */

  Lam = atan2(R_[Y_AXIS], R_[X_AXIS]);
  if (Lam<0.0) Lam+=M_PI*2.0;
  
  /* 
     and r=sqrt(x^2+y^2).
  */

  double r = hypot(R_[X_AXIS], R_[Y_AXIS]);

  /*
	subroutine GEOD(r,z,fi,h)
    c Program to transform Cartesian to geodetic coordinates
    c based on the exact solution (Borkowski, 1989)
    c Input : r, z = equatorial [m] and polar [m] components
    c Output: fi, h = geodetic coord's (latitude [rad], height [m])
	implicit real*8(a-h,o-z)
  */
  
  double b,E,F,P,Q,D,s,v,G,t;

  /*
    c GRS80 ellipsoid: semimajor axis (a) and inverse flattening (fr)
	data a,fr /6378137.d0,298.257222101d0/
  */
  const double a  = 6378137.0;
  const double fr = 298.257222101;

  /*
	b = dsign(a - a/fr,z)
	E = ((z + b)*b/a - a)/r
	F = ((z - b)*b/a + a)/r
  */

  b = sign(a - a/fr, R_[Z_AXIS]);
  E = ((R_[Z_AXIS] + b)*b/a - a)/r;
  F = ((R_[Z_AXIS] - b)*b/a + a)/r;
    
  /*
    c Find solution to:  t**4 + 2*E*t**3 + 2*F*t - 1 = 0
	P = (E*F + 1.)*4.d0/3.d0
	Q = (E*E - F*F)*2.d0
	D = P*P*P + Q*Q
	    if(D.ge.0d0) then
	s = dsqrt(D) + Q
	s = dsign(dexp(dlog(dabs(s))/3.d0),s)
	v = P/s - s
    c Improve the accuracy of numeric values of v
	v = -(Q + Q + v*v*v)/(3.d0*P)
	    else
	v = 2.d0*dsqrt(-P)*dcos(dacos(Q/P/dsqrt(-P))/3.d0)
	    endif
	G = .5d0*(E + dsqrt(E*E + v))
	t = dsqrt(G*G + (F - v*G)/(G + G - E)) - G
  */

  P = (E*F + 1.0)*4.0/3.0;
  Q = (E*E - F*F)*2.0;
  D = P*P*P + Q*Q;
  if (D>=0.0)
    {
      s = sqrt(D) + Q;
      s = sign(exp(log(fabs(s))/3.0), s);
      v = P/s - s;
      v = -(Q + Q + v*v*v)/(3.0*P);
    }
  else
    v = 2.0*sqrt(-P)*cos(acos(Q/P/sqrt(-P))/3.0);

  G = 0.5*(E + sqrt(E*E + v));
  t = sqrt(G*G + (F - v*G)/(G + G - E)) - G;
  
  /*
	fi = datan((1.d0 - t*t)*a/(2.d0*b*t))
	h = (r - a*t)*dcos(fi) + (z - b)*dsin(fi)
	return
	end
  */
  
  Fi = atan((1.0 - t*t)*a/(2.0*b*t));
  Ro = (r - a*t)*cos(Fi) + (R_[Z_AXIS] - b)*sin(Fi);
  return;
};

double SBStation::axisOffsett()	const 
{
  return AxisOffset + P_Axis->v();
};

double SBStation::axisOffsetLenght(const Vector3& s, const SBMJD& T_, double scal_)
{
  double	Phi_W=39.06*DEG2RAD, Eps=0.12*DEG2RAD, dd = 0.0;
  //  if (axisOffset()==0.0) 
  //    return 0.0;
  Vector3	I;
  switch (MountType)
    {
    case Mnt_AZEL:
      I = fmVEN()*Vector3(1.0, 0.0, 0.0); // here is a vertical direction @TRF
      break;
    case Mnt_EQUA:
      I = Vector3(0.0, 0.0, Rt[Z_AXIS]>=0.0?1.0:-1.0); // here is a north/south direction @TRF
      break;
    case Mnt_X_YN:
      I = fmVEN()*Vector3(0.0, 0.0, 1.0); // here is a north direction @TRF
      break;
    case Mnt_X_YE:
      I = fmVEN()*Vector3(0.0, 1.0, 0.0); // here is an east direction @TRF
      break;
    case Mnt_Richmond:
      I = fmVEN()*Vector3(sin(Phi_W), -cos(Phi_W)*sin(Eps), cos(Phi_W)*cos(Eps));
      break;
    default:
    case Mnt_Unkn:
      Log->write(SBLog::WRN, SBLog::AXISOFF, ClassName() + 
		 ": Station [" + id().toString() + "] " + name() + " has an unknown antenna mount type");
      break;  
    };

  if (MountType == Mnt_Unkn)
    return 0.0;

  double sI = s*I;
  dd	    = sqrt(1.0 - sI*sI);
  double l  = axisOffsett()*dd;
  P_Axis->setD(dd*scal_, T_);

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::AXISOFF))
    {
      QString MntTypeStr;
      switch (MountType)
	{
	case Mnt_AZEL: MntTypeStr = "Azimuth-elevation mount"; break;
	case Mnt_EQUA: MntTypeStr = "Equatorial mount"; break;
	case Mnt_X_YN: MntTypeStr = "X-Y North-South mount"; break;
	case Mnt_X_YE: MntTypeStr = "X-Y East-West mount"; break;
	case Mnt_Richmond: MntTypeStr = "Richmond (unique) mount"; break;
	default:
	case Mnt_Unkn: MntTypeStr = "Unknown mount"; break;
	};
      Log->write(SBLog::DBG, SBLog::AXISOFF, ClassName() + 
		 ": Axis offset at  : [" + id().toString() + "] " + name());
      Log->write(SBLog::DBG, SBLog::AXISOFF, ClassName() + 
		 ": Axis offset type: " + MntTypeStr);
      Log->write(SBLog::DBG, SBLog::AXISOFF, ClassName() + 
		 ": Axis offset  (m): " + QString().sprintf("%10.6f", axisOffsett()));
      I.report2Log(SBLog::DBG, SBLog::AXISOFF, ClassName() + 
		 ": Vector I        : ", 11, 8);
      s.report2Log(SBLog::DBG, SBLog::AXISOFF, ClassName() + 
		 ": Vector s        : ", 11, 8);
      Log->write(SBLog::DBG, SBLog::AXISOFF, ClassName() + 
		 ": Antenna corr.(m): " + QString().sprintf("%10.6f", l));
      Log->write(SBLog::DBG, SBLog::AXISOFF, ClassName() + ": +++");
    };
  return l;
};

QString SBStation::charID4SINEX()
{
  QString		Ret("");
  
  if (0<CDPNum && CDPNum<9999)
    Ret.sprintf("%04d", CDPNum);
  else if (CharID.length()==4)
    Ret=CharID;
  
  return Ret;
};

void SBStation::updateStation(const SBStation& S)
{
  Name		= S.Name;
  SBAttributed::operator=(S);
  CDPNum	= S.CDPNum;
  Tech		= S.Tech;
  ClassCoord	= S.ClassCoord;
  Description	= (const char*)S.Description;
  Comments	= (const char*)S.Comments;
  CharID	= (const char*)S.CharID;

  if (S.OLoad)
    OLoad = new SBOLoad (*S.OLoad);

  Height	= S.Height;
  Longitude	= S.Longitude;
  Latitude	= S.Latitude;
  FmVEN		= S.FmVEN;

  RefPt		= S.RefPt;
  Rt		= S.Rt;
  Ecc		= S.Ecc;
  V		= S.V;
  Vt		= S.Vt;
  dV		= S.dV;

  releaseParameters();

#ifdef ELEV_DEBUG_OUTPUT
  OutFile	= NULL;
  OutStream	= NULL;
#endif
};

#ifdef ELEV_DEBUG_OUTPUT
void SBStation::openOutFile(const QString& BaseName)
{
  if (OutFile)
    delete OutFile;
  if (OutStream)
    delete OutStream;
  OutFile = new QFile(BaseName + name() + ".debug");
  if (!OutFile->open(IO_WriteOnly))
    {
      delete OutFile;
      OutFile = NULL;
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + 
		 ": cannot open the file '" + BaseName + name() + ".debug'");
      return;
    };
  OutStream = new QTextStream(OutFile);
};

void SBStation::closeOutFile()
{
  if (OutFile && OutStream)
    {
      OutFile->close();
      OutStream->unsetDevice();
      delete OutFile;
      OutFile = NULL;
      delete OutStream;
      OutStream = NULL;
    };
};
#endif
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStation's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBStation& S)
{
  s << (const SBNamed&)S << (const SBAttributed&)S << S.CDPNum << S.DomeMinor 
    << (uint)S.Tech << (uint)S.PointType << (uint)S.ClassCoord << S.Description 
    << S.Comments << (uint)S.MountType << S.AxisOffset << S.RefPt << S.V 
    << S.dV << S.CharID;
  if (S.isAttr(SBStation::hasOwnOLoad)) s << *S.OLoad;
  return s;
};

QDataStream& operator>>(QDataStream& s, SBStation& S)
{
  s >> (SBNamed&)S >> (SBAttributed&)S >> S.CDPNum >> S.DomeMinor >> 
    (uint&)S.Tech >> (uint&)S.PointType >> (uint&)S.ClassCoord >> S.Description 
    >> S.Comments >> (uint&)S.MountType >> S.AxisOffset >> S.RefPt >> S.V 
    >> S.dV >> S.CharID;
  if (S.isAttr(SBStation::hasOwnOLoad)) 
    {
      S.OLoad = new SBOLoad;
      s >> *S.OLoad;
    };
  S.Rt = S.RefPt.r(TZero);
  if (S.Rt!=v3Zero) 
    {
      S.calcRLF(S.Rt, S.Height, S.Longitude, S.Latitude);
      RotMatrix V(NORT), W(EAST);
      S.FmVEN = V(-S.Longitude)*W(S.Latitude);
    };

  return s;
};

SBTS_dir1994 &operator<<(SBTS_dir1994& s, const SBStation& S)
{
  const char* sPtType[2] = {"S", "M"};
  const char* sClassC[7] = {"A", "B", "C", "D", "E", "Z", " "};
  QString	a;
  s << (const char*)a.sprintf("  %s%03d              %s  ", 
			      sPtType[S.pointType()], S.domeMinor(), sClassC[S.classCoord()]);
  s << (const char*)S.charID() << "    ";
  
  if (S.CDP()!=9999) s << S.CDP() << "   ";
  else s << "       ";

  s << (const char*)S.description() << "\n";
  return s;
};

SBTS_dir1994 &operator>>(SBTS_dir1994& s, SBStation& S)
{
  int		n;
  bool		isOK;  
  QString	Str = s.readLine();
  
  if(Str.length()<2)
    {
      // Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: got an empty string");
      S.DomeMinor=0;
      return s;
    };
  if(Str.length()<40)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: SBStation: unknown format: [" + Str + "]");
      S.DomeMinor=0;
      return s;
    };
  //          1         2         3         4         5         6         7
  //01234567890123456789012345678901234567890123456789012345678901234567890
  //  S009              A*         7222   Mojave fixed 12m VLBI ref. point
  //  S005              C  GOMA           DORIS Ant. Ref. Pt. 2 03-AUG-94 - 

  n = Str.mid(3, 3).toInt(&isOK);
  if (!isOK || !(Str.mid(2, 1)=="S" || Str.mid(2, 1)=="M"))
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: SBStation: not a Domes Number: [" + Str + "]");
      S.DomeMinor=0;
      return s;
    };
  S.setDomeMinor(n);
  if (Str.mid( 2, 1)=="S") S.setPointType(SBStation::Pt_Antenna);
  if (Str.mid( 2, 1)=="M") S.setPointType(SBStation::Pt_Marker);
  S.setCharID(Str.mid(23, 4));
  if (Str.mid(31, 4)!="    ") 
    {
      n = Str.mid(31, 4).toInt(&isOK);
      if (isOK) S.setCDP(n);
      else
	Log->write(SBLog::INF, SBLog::STATION, "SBTS_dir1994: wrong CDP number [" 
		   + Str.mid(31, 4) + "], ignored");
    };
  S.setDescription(Str.mid(38,100).simplifyWhiteSpace());
  return s;
};

SBTS_dir2000 &operator<<(SBTS_dir2000& s, const SBStation& S)
{
  const char* sPtType[2] = {"S", "M"};
  QString	a;

  s << (const char*)a.sprintf(" %s%03d              ", 
			      sPtType[S.pointType()], S.domeMinor());
  if (S.CDP()!=9999) 
    s << S.CDP() << " ";
  else s << "     ";

  s << (const char*)S.charID() << "      " << (const char*)S.description() << "\n";
  return s;
};

SBTS_dir2000 &operator>>(SBTS_dir2000& s, SBStation& S)
{
  int		n;
  bool		isOK;  
  QString	Str = s.readLine();
  
  if(Str.length()<2)
    {
      // Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: got an empty string");
      S.DomeMinor=0;
      return s;
    };
  if(Str.length()<36)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: SBStation: unknown format: [" + Str + "]");
      S.DomeMinor=0;
      return s;
    };

  //DOMES  SITE        CDP  IGS DORIS DESIGNATION
  //Number
  //          1         2         3         4         5         6         7         8
  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
  //14201  WETTZELL                                
  // M003              7595           SLR and GPS mark                              
  // M008                             Mark 1000 (bottom TI4100 12-NOV-90 -          
  //                                  27-JUL-89)                                    
  // M200              7594           Marker at TIGO-platform for the               
  //                                  TIGO-SLR-telescope.                           
  // S004              7224           VLBI Ref. Point                               
  //49921  COVINA
  // M001                   CVHS      Raised nipple on the lower part of the antenna

  n = Str.mid(2, 3).toInt(&isOK);
  if (!isOK || !(Str.mid(1, 1)=="S" || Str.mid(1, 1)=="M"))
    {
      if (Str.mid(0, 5) == "     ") // it's a continue of station description
	{
	  S.DomeMinor=-1;
	  S.setDescription(Str.mid(34,100).simplifyWhiteSpace());
	}
      else 
	{
	  S.DomeMinor=0;
	  S.setDescription("");
	  Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: SBStation: not a Domes Number: [" + 
		     Str + "]");
	};
      return s;
    };
  S.setDomeMinor(n);
  if (Str.mid( 1, 1)=="S") S.setPointType(SBStation::Pt_Antenna);
  if (Str.mid( 1, 1)=="M") S.setPointType(SBStation::Pt_Marker);
  S.setCharID(Str.mid(24, 4));
  if (Str.mid(19, 4)!="    ") 
    {
      n = Str.mid(19, 4).toInt(&isOK);
      if (isOK) S.setCDP(n);
      else
	Log->write(SBLog::INF, SBLog::STATION, "SBTS_dir2000: wrong CDP number [" 
		   + Str.mid(19, 4) + "', ignored");
    };
  S.setDescription(Str.mid(34,100).simplifyWhiteSpace());
  return s;
};

SBTS_ssc1994 &operator<<(SBTS_ssc1994& s, const SBStation& S)
{
  QString		a;
  static const char*	sTech[11]=
  { " VLBI", " OPTA", "  LLR", "  GPS", "  SLR", 
    "DORIS", " COMB", "CGEOD", "  ATM", "  TIE", " UNKN" };
  s << (const char*) a.sprintf("%s %-16s%5s ", 
			       (const char*)S.id().toString(), 
			       (const char*)S.name().left(16), sTech[S.tech()]);
  if (S.CDP()!=9999) s << (const char*) a.sprintf("%04d", S.CDP());
  else s << (const char*)S.charID().left(4);
  s << "                   ";
  s << (const char*)a.sprintf("%12.3f %12.3f %12.3f %5.3f %5.3f %5.3f\n", 
			      S. r_first().at(X_AXIS), S. r_first().at(Y_AXIS), S. r_first().at(Z_AXIS),
			      S.dR_first().at(X_AXIS), S.dR_first().at(Y_AXIS), S.dR_first().at(Z_AXIS));
  if (S.dV==Vector3(1.0,1.0,1.0)) return s;
  
  s << (const char*)a.sprintf("%s                                              "
			      "%12.4f %12.4f %12.4f%5.4f%5.4f%5.4f\n", 
			      (const char*)S.id().toString(), 
			      S. V.at(X_AXIS), S. V.at(Y_AXIS), S. V.at(Z_AXIS), 
			      S.dV.at(X_AXIS), S.dV.at(Y_AXIS), S.dV.at(Z_AXIS));
  return s;
};

SBTS_ssc2000 &operator<<(SBTS_ssc2000& s, const SBStation& S)
{
  QString		a;
  static const char*	sTech[11]=
  { " VLBI", " OPTA", "  LLR", "  GPS", "  SLR", 
    "DORIS", " COMB", "CGEOD", "  ATM", "  TIE", " UNKN" };

  SBCoordinates				*Coo=NULL;
  QListIterator<SBCoordinates>		it(S.RefPt);
  int					NumOfSolution;
  NumOfSolution = 0;
  for (; it.current(); ++it)
    {
      Coo = it.current();
      if (!Coo->isAttr(SBCoordinates::notValid))
	{
	  s << (const char*) a.sprintf("%s %-16s %5s ", 
				       (const char*)S.id().toString(), 
				       (const char*)S.name().left(16), sTech[S.tech()]);
	  if (S.CDP()!=9999) 
	    s << (const char*) a.sprintf("%04d", S.CDP());
	  else 
	    s << (const char*)S.charID().left(4);
	  s << " ";
	  
	  //1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //         1         2         3         4         5         6         7         8         9
	  //10002M003 GRASSE            VLBI 7605  4581697.646   556125.760  4389351.457  .003  .002  .003
	  //10002M003                                   -.0131        .0189        .0101 .0003 .0001 .0004
	  
	  s << (const char*)a.sprintf("%12.3f %12.3f %12.3f %6.3f %6.3f %6.3f",
				      Coo->coo().at(X_AXIS), Coo->coo().at(Y_AXIS), Coo->coo().at(Z_AXIS), 
				      Coo->cooErrors().at(X_AXIS), 
				      Coo->cooErrors().at(Y_AXIS), 
				      Coo->cooErrors().at(Z_AXIS));
	  
	  if (NumOfSolution)
	    {
	      a.sprintf(" %d AFTER %s", NumOfSolution+1, (const char*)Coo->toString(SBMJD::F_SINEX_S));
	      s << (const char*)a;
	    };
	  
	  s << "\n";
	  if (S.dV!=Vector3(1.0,1.0,1.0))
	    {
	      s << (const char*)a.sprintf("%s                             "
					  "%12.4f %12.4f %12.4f %5.4f %5.4f %5.4f\n",
					  (const char*)S.id().toString(), 
					  S. V.at(X_AXIS), S. V.at(Y_AXIS), S. V.at(Z_AXIS), 
					  S.dV.at(X_AXIS), S.dV.at(Y_AXIS), S.dV.at(Z_AXIS));
	    };
	  NumOfSolution++;
	};
    };
  
  return s;
};

SBTS_blqReq &operator<<(SBTS_blqReq& s, const SBStation& S)
{
  if (S.r_first()==v3Zero) 
    return s; // do not bother Hans-Georg

  QString		a;
  // Our fixed column layout: 24 characters for the station, one whitespace, three numerical 
  // fields with a width of 16 characters each:
  //
  //          1         2         3         4         5         6         7         8
  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
  //Name of station_________| |Longitude (deg) | Latitude (deg) | Height (m) 
  //Onsala                     11.9264          57.3958          0.0

  s << (const char*)a.sprintf("%s %-16s  %14.3f %14.3f %14.3f\n",
			      (const char*)(S.id().toString()), (const char*)(S.name().left(16)),
			      S.r_first().at(X_AXIS), S.r_first().at(Y_AXIS), S.r_first().at(Z_AXIS) );
  return s;
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* class SBStationID implementation								*/
/*												*/
/*==============================================================================================*/
bool SBStationID::isValidStr(const QString& Str)
{
  bool	isOK;
  int	n;
  //99999S000
  if (Str.length()!=9) return FALSE;
  n = Str.mid(0,5).toInt(&isOK);
  if (!isOK || n<10000) return FALSE;
  n = Str.mid(6,3).toInt(&isOK);
  if (!isOK) return FALSE;
  if (Str.mid(5,1)=="S" || Str.mid(5,1)=="M") return TRUE;
  else return FALSE;
};

void SBStationID::setID(const QString& Str)
{
  if (isValidStr(Str))
    {
      DomeMajor = Str.mid(0,5).toInt();
      DomeMinor = Str.mid(6,3).toInt();
      if (Str.mid(5,1)=="S") PtType = SBStation::Pt_Antenna;
      else PtType = SBStation::Pt_Marker;
    }
  else
    {
      DomeMajor = 99999;
      PtType    = SBStation::Pt_Unkn;
      DomeMinor = 0;
    };
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* class SBSite implementation									*/
/*												*/
/*==============================================================================================*/
SBSite::SBSite(const QString& Name_)
  : SBNamed(Name_), V(0.0, 0.0, 0.0), dV(1.0, 1.0, 1.0)
{
  DomeMajor	= 99999;
  Country	= "Irukan";
  Plate		= "UNKN";
  St_S		= NULL;
  St_M		= NULL;
  Stations	. setAutoDelete(TRUE);
  Height	= 0.0;
  Longitude	= 0.0;
  Latitude	= 0.0;
  prepareDicts();
};

SBSite::SBSite(const SBSite& S)
  : SBNamed(S.Name), OLoad(S.OLoad), V(S.V), dV(S.dV)
{
  DomeMajor	= S.DomeMajor;
  Country	= (const char*)S.Country;
  Plate		= (const char*)S.Plate;
  St_S		= NULL;
  St_M		= NULL;
  Stations	. setAutoDelete(TRUE);
  Height	= S.Height;
  Longitude	= S.Longitude;
  Latitude	= S.Latitude;
  copyStationsList(S.Stations);
  prepareDicts();
};

SBSite& SBSite::operator= (const SBSite& S)
{
  Name		= (const char*)S.Name;
  //  DomeMajor	= S.DomeMajor; //used by sort
  Country	= (const char*)S.Country;
  Plate		= (const char*)S.Plate;
  OLoad		= S.OLoad;
  V		= S.V;
  dV		= S.dV;
  Height	= S.Height;
  Longitude	= S.Longitude;
  Latitude	= S.Latitude;
  copyStationsList(S.Stations);
  prepareDicts();
  return *this;
};

void SBSite::copyStationsList(const SBStationList& SL)
{
  Stations	. clear();
  if (SL.count())
    {
      SBStationIterator it(SL);
      while (it.current())
	{
	  Stations.inSort(new SBStation(this, *it.current()));
	  ++it;
	};
    };
};

void SBSite::updateSite(const SBSite& S)
{
  Name		= (const char*)S.Name;
  Country	= (const char*)S.Country;
  Plate		= (const char*)S.Plate;
  OLoad		= S.OLoad;
  V		= S.V;
  dV		= S.dV;
  Height	= S.Height;
  Longitude	= S.Longitude;
  Latitude	= S.Latitude;
  updateStationsList(S.Stations);
  prepareDicts();
};

/*
void SBSite::updateRefTpsOnly(const SBSite& S)
{
  const SBStationList& SL = S.Stations;
  if (SL.count())
    {
      SBStationIterator it(SL);
      SBStation		*St=NULL;
      while ((St=it.current()))
	{
	  SBStation *W = NULL;
	  if ((W=find(St->id())))
	    W->updateRefTpsOnly(*St);
	  else
	    addStation(new SBStation (this, *St));
	  ++it;
	};
    };
};
*/

void SBSite::updateStationsList(const SBStationList& SL)
{
  if (SL.count())
    {
      SBStationIterator it(SL);
      SBStation		*St=NULL;
      while ((St=it.current()))
	{
	  SBStation *W = NULL;
	  if ((W=find(St->id())))
	    W->updateStation(*St);
	  else
	    addStation(new SBStation (this, *St));
	  ++it;
	};
    };
};

void SBSite::prepareDicts()
{
  int n = 10;
  if (Stations.count()) 
    n = Stations.count() + 5;
  if (St_S) 
    delete St_S;
  St_S	= new QIntDict<SBStation>(n);
  St_S	->setAutoDelete(FALSE);
  if (St_M) 
    delete St_M;
  St_M	= new QIntDict<SBStation>(n);
  St_M	->setAutoDelete(FALSE);

  SBStationIterator it(Stations);
  SBStation *W;
  while ((W = it.current()))
    {
      W->Site=this;
      if (W->pointType()==SBStation::Pt_Antenna) 
	St_S->insert(W->domeMinor(), W);
      else if (W->pointType()==SBStation::Pt_Marker ) 
	St_M->insert(W->domeMinor(), W);
      else 
	Log->write(SBLog::ERR, SBLog::STATION, ClassName() + 
		   ": oops: site: " + name() + " station: [" + W->id().toString() + "] missed!");
      ++it;
    };
};

SBStation* SBSite::find(const SBStationID& ID)
{
  switch (ID.PtType)
    {
    case SBStation::Pt_Antenna:
      return St_S->find(ID.DomeMinor);
      break;
    case SBStation::Pt_Marker:
      return St_M->find(ID.DomeMinor);
      break;
    case SBStation::Pt_Unkn:
    default:
      return NULL;
    };
};

void SBSite::addStation(SBStation* Station)
{
  if (!Station)
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot insert NULL");
      return;
    };
  if (Station->domeMinor()>999)
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": wrong station's domeMinor");
      return;
    };
  if (Station->pointType()==SBStation::Pt_Unkn)
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": wrong station's point type");
      return;
    };
  QIntDict<SBStation>* dict=Station->pointType()==SBStation::Pt_Antenna?St_S:St_M;
  if (!dict->find(Station->domeMinor()))
    {
      dict->insert(Station->domeMinor(), Station);
      Stations.inSort(Station);
      //      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": added the station [" + Station->name()
      //	       + "] to the site [" + name() + "]: entry [" + Station->id().toString() + "]");
    }
  else
    Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot add the station [" + Station->name()
	       + "] to the site [" + name() + "]: entry [" + Station->id().toString() + "] already exist");

};

bool SBSite::delStation(SBStation* Station)
{
  if (!Station)
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot delete NULL");
      return FALSE;
    };
  if (!find(Station->id()))
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": the station [" + Station->name() 
		 + "] is not in the list");
      return FALSE;
    };
  QIntDict<SBStation>* dict=Station->pointType()==SBStation::Pt_Antenna?St_S:St_M;
  if (dict->remove(Station->domeMinor()) && Stations.remove(Station))
    {
      Station=NULL;
      return TRUE;
    }
  else 
    Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": removing of the station ["
	       + Station->name() + "] failed");
  return FALSE;
};

void SBSite::calcRLF()
{
  int	n = 0;
  R  = v3Zero;
  for (SBStation *W=Stations.first(); W; W=Stations.next())
    if (W->r_first()!=v3Zero) 
      {
	R+=W->r_first(); 
	n++;
      };
  if (n) SBStation::calcRLF((R=R/n), Height, Longitude, Latitude);
};

/** Returns first unused domes number
 * \return least unused DomeMinor for this site (int).
 *
 */
int SBSite::firstUnusedNumber(SBStation::SPointType Tp)
{
  if (Tp==SBStation::Pt_Unkn) return 0;
  QIntDict<SBStation>* St=Tp==SBStation::Pt_Antenna?St_S:St_M;
  SBStation *s;
  int i=1;
  while (i<=999 && (s=St->find(i)) && i!=s->id().domeMinor()) i++;
  return i;
};

/** Returns last unused domes number
 * \return largest unused DomeMinor for this site (int).
 *
 */
int SBSite::lastUnusedNumber(SBStation::SPointType Tp)
{
  if (Tp==SBStation::Pt_Unkn) return 0;
  QIntDict<SBStation>* St=Tp==SBStation::Pt_Antenna?St_S:St_M;
  //  SBStation *s;
  int i=999;
  //  while (i>0 && (s=St->find(i)) && i!=s->id().domeMinor()) i--;
  while (i>0 && St->find(i)) i--;
  return i;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSite's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBSite& S)
{
  s << (const SBNamed&)S;
  return s << S.DomeMajor << S.Country << S.Plate << S.OLoad << S.V << S.dV << S.Stations;
};

QDataStream& operator>>(QDataStream& s, SBSite& S)
{
  s >> (SBNamed&)S;
  s >> S.DomeMajor >> S.Country >> S.Plate >> S.OLoad >> S.V >> S.dV >> S.Stations;
  S.prepareDicts();
  S.calcRLF();
  return s;
};

SBTS_site1994 &operator<<(SBTS_site1994& s, const SBSite& S)
{
  QString	a;
  int		lon_d, lon_m, lat_d, lat_m;
  double	f;
  
  rad2dmsl(S.longitude(), lon_d, lon_m, f);
  rad2dms(S.latitude(), lat_d, lat_m, f);
  
  s << (const char*)a.sprintf(" %05d   %-17s %-11s %3d %02d %3d %02d  %s\n", 
			      S.domeMajor(), (const char*)S.name().left(17), 
			      (const char*)S.country().left(17), lon_d, lon_m, lat_d, abs(lat_m),
			      (const char*)S.plate());
  return s;
};

SBTS_site1994 &operator>>(SBTS_site1994& s, SBSite& S)
{
  int		n;
  bool		isOK;  
  QString	Str = s.readLine();
  
  if(!Str.length())
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_site1994: got an empty string");
      S.DomeMajor=99999;
      return s;
    };
  if(Str.length()<58)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_site1994: unknown format");
      S.DomeMajor=99999;
      return s;
    };

  //          1         2         3         4         5         6
  //0123456789012345678901234567890123456789012345678901234567890
  // 10002   GRASSE            France        6 55  43 45  EURA   
  n = Str.mid(1, 5).toInt(&isOK);
  if (!isOK || n<10000 || n>99999)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_site1994: not a Domes Number");
      S.DomeMajor=99999;
      return s;
    };
  S.setDomeMajor(n);
  S.setName     (Str.mid( 9, 17).simplifyWhiteSpace());
  S.setCountry  (Str.mid(27, 11).simplifyWhiteSpace());
  S.setPlate    (Str.mid(54,  4));
  return s;
};

SBTS_site2000 &operator<<(SBTS_site2000& s, const SBSite& S)
{
  QString	a;
  int		lon_d, lon_m, lat_d, lat_m;
  double	f;
  
  rad2dmsl(S.longitude(), lon_d, lon_m, f);
  rad2dms (S.latitude(),  lat_d, lat_m, f);
  
  s << (const char*)a.sprintf(" %05d  %-25s %-20s %3d %02d %3d %02d  %s\n", 
			      S.domeMajor(), (const char*)S.name().left(25), 
			      (const char*)S.country().left(20), lon_d, lon_m, lat_d, abs(lat_m),
			      (const char*)S.plate());
  return s;
};

SBTS_site2000 &operator>>(SBTS_site2000& s, SBSite& S)
{
  int		n;
  bool		isOK;  
  QString	Str = s.readLine();
  
  if(!Str.length())
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_site2000: got an empty string");
      S.DomeMajor=99999;
      return s;
    };
  if(Str.length()<75)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_site2000: unknown format");
      S.DomeMajor=99999;
      return s;
    };

  //          1         2         3         4         5         6         7         8
  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
  // 10001  PARIS                     FRANCE                  2 25   48 51  EURA
  // 40475  WAIMEA (HAWAII)           USA                  -155 40   20 01  PCFC

  n = Str.mid(1, 5).toInt(&isOK);
  if (!isOK || n<10000 || n>99999)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_site2000: not a Domes Number");
      S.DomeMajor=99999;
      return s;
    };
  S.setDomeMajor(n);
  S.setName     (Str.mid( 8, 25).simplifyWhiteSpace());
  S.setCountry  (Str.mid(34, 20).simplifyWhiteSpace());
  S.setPlate    (Str.mid(72,  4));
  return s;
};

SBTS_dir1994 &operator<<(SBTS_dir1994& s, const SBSite& S)
{
  QString	a;
  s << (const char*)a.sprintf(" %05d  %s\n", S.domeMajor(), (const char*)S.name()); 
  SBStationIterator it(S.Stations);
  for (SBStation* W=it.toFirst(); it.current(); W=++it) s << *W;
  s << "\n";
  return s;
};

SBTS_dir1994 &operator>>(SBTS_dir1994& s, SBSite& S)
{
  int		n;
  bool		isOK;  
  QString	Str = s.readLine();
  
  if(Str.length()<2)
    {
      //Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: got an empty string");
      S.DomeMajor=99999;
      return s;
    };
  if(Str.length()<9)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: SBSite: unknown format: [" + Str + "]");
      S.DomeMajor=99999;
      return s;
    };

  //          1         2         3         4         5         6
  //0123456789012345678901234567890123456789012345678901234567890
  // 10101  Copenhagen/Buddinge 
  // 12749  Monte Venda         

  n = Str.mid(1, 5).toInt(&isOK);
  if (!isOK || n<10000)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: SBSite: not a Domes Number: [" + Str + "]");
      S.DomeMajor=99999;
      return s;
    };
  S.setDomeMajor(n);
  S.setName(Str.mid(8,40).simplifyWhiteSpace());
  
  SBStation *W;
  while (!s.eof())
    {
      W = new SBStation(&S, "_NONAME_");
      s >> *W;
      if (W->domeMinor()!=0) 
	{
	  if ((n=S.Stations.find(W))==-1) S.addStation(W);
	  else
	    {
	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: some strange things happend..");
	      SBStation* Q = S.Stations.at(n);
	      Str.sprintf("%05d vs %05d", Q->domeMinor(), W->domeMinor());
	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: domes    : " + Str);
	      Str.sprintf("[%c] vs [%c]", Q->pointType()==SBStation::Pt_Antenna?'S':'M', 
			  W->pointType()==SBStation::Pt_Antenna?'S':'M');
	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: PointType: " + Str);
	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: name     : [" 
			 + Q->name() + "] vs [" + W->name() + "]");
	      Str.sprintf("%04d vs %04d", Q->CDP(), W->CDP());
	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: CDPNum   : " + Str);
	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: descr    : [" 
			 + Q->description() + "] vs [" + W->description() + "]");
	    };
	}
      else return s;
    };
  return s;
};
//
SBTS_dir2000 &operator<<(SBTS_dir2000& s, const SBSite& S)
{
  QString	a;
  s << (const char*)a.sprintf("%05d  %s\n", S.domeMajor(), (const char*)S.name()); 
  SBStationIterator it(S.Stations);
  for (SBStation* W=it.toFirst(); it.current(); W=++it) s << *W;
  s << "\n";
  return s;
};

SBTS_dir2000 &operator>>(SBTS_dir2000& s, SBSite& S)
{
  int		n;
  bool		isOK;  
  QString	Str = s.readLine();
  
  if(Str.length()<2)
    {
      //Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: got an empty string");
      S.DomeMajor=99999;
      return s;
    };
  if(Str.length()<9)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: SBSite: unknown format: [" + Str + "]");
      S.DomeMajor=99999;
      return s;
    };

  //          1         2         3         4         5         6
  //0123456789012345678901234567890123456789012345678901234567890
  //10011  SAINT-MICHEL DE PROVENCE                

  n = Str.mid(0, 5).toInt(&isOK);
  if (!isOK || n<10000)
    {
      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: SBSite: not a Domes Number: [" + Str + "]");
      S.DomeMajor=99999;
      return s;
    };
  S.setDomeMajor(n);
  S.setName(Str.mid(7,80).simplifyWhiteSpace());
  
  SBStation *W;
  SBStation *last=NULL;
  while (!s.eof())
    {
      W = new SBStation(&S, "_NONAME_");
      s >> *W;
      if (W->domeMinor()!=0) 
	{
	  if (W->domeMinor()>0)
	    {
	      if ((n=S.Stations.find(W))==-1)
		{
		  S.addStation(W);
		  last = W;
		}
	      else
		{
		  Log->write(SBLog::DBG, SBLog::STATION, 
			     "SBTS_dir2000: oops, got a duplicate entry for the site: " + 
			     S.name() + ", DOMES Major: " + QString().setNum(S.DomeMajor));
		  SBStation* Q = S.Stations.at(n);
		  Str.sprintf("%05d vs %05d", Q->domeMinor(), W->domeMinor());
		  Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: DOMES    : " + Str);
		  Str.sprintf("[%c] vs [%c]", Q->pointType()==SBStation::Pt_Antenna?'S':'M', 
			      W->pointType()==SBStation::Pt_Antenna?'S':'M');
		  Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: PointType: " + Str);
		  Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: descr    : [" 
			     + Q->description() + "] vs [" + W->description() + "]");
		};
	    }
	  else if (last && (W->domeMinor()==-1) && (W->description().length()))
	    {
	      last->setDescription( (last->description() + W->description()).simplifyWhiteSpace() );
	      delete W;
	    };
	}
      else
	{
	  delete W;
	  return s;
	};
    };
  return s;
};

SBTS_ssc1994 &operator<<(SBTS_ssc1994& s, const SBSite& S)
{
  SBStationIterator it(S.Stations);
  for (SBStation* W=it.toFirst(); it.current(); W=++it) 
    if (s.ClassCoord==W->classCoord()) s << *W;
  return s;
};

SBTS_ssc2000 &operator<<(SBTS_ssc2000& s, const SBSite& S)
{
  SBStationIterator it(S.Stations);
  for (SBStation* W=it.toFirst(); it.current(); W=++it) 
    s << *W;
  return s;
};

SBTS_blqReq &operator<<(SBTS_blqReq& s, const SBSite& S)
{
  SBStationIterator it(S.Stations);
  for (SBStation* W=it.toFirst(); it.current(); W=++it) 
    s << *W;
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEccentricity implementation								*/
/*												*/
/*==============================================================================================*/
SBEccentricity::SBEccentricity()
{
  Aka		= "ZABRISKI";
  TStart	= TZero;
  TFinis	= TZero;
  Ecc		= v3Zero;
  Type		= ET_XYZ;
  SessionName	= "";
};

SBEccentricity::SBEccentricity(const SBEccentricity& Ecc_)
{
  *this = Ecc_;
};

SBEccentricity& SBEccentricity::operator=(const SBEccentricity& Ecc_)
{
  Aka		= Ecc_.aka().copy();
  TStart	= Ecc_.tStart();
  TFinis	= Ecc_.tFinis();
  Ecc		= Ecc_.ecc();
  Type		= Ecc_.type();
  SessionName	= Ecc_.sessionName().copy();
  return *this;
};

bool SBEccentricity::operator==(const SBEccentricity& Ecc_) const
{
  return 
    (Aka == Ecc_.aka()) &&
    (TStart == Ecc_.tStart()) && 
    (TFinis == Ecc_.tFinis()) &&
    (Ecc == Ecc_.ecc()) && 
    (Type == Ecc_.type()) &&
    (SessionName == Ecc_.sessionName());
};

bool SBEccentricity::operator<(const SBEccentricity& Ecc_) const
{
  if (TStart >= Ecc_.tStart()) 
    return FALSE;
  else if (TStart < Ecc_.tStart()) 
    return TRUE;
  else if (TFinis >= Ecc_.tFinis()) 
    return FALSE;
  else if (TFinis < Ecc_.tFinis()) 
    return TRUE;
  else 
    return Aka<Ecc_.aka();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEccentricity's friends implementation						*/
/*												*/
/*==============================================================================================*/
QTextStream &operator>>(QTextStream& s, SBEccentricity& /*Eccentricity*/)
{
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStationEcc implementation								*/
/*												*/
/*==============================================================================================*/
SBEccentricity* SBStationEcc::findEcc(const SBMJD& T)
{
  SBEccentricity*	ecc=NULL;
  
  for (SBEccentricity *e=Eccs.first(); e; e=Eccs.next())
    if ( (e->tStart()<=T) && (T<=e->tFinis()) )
      ecc=e;
  
  return ecc;
};

bool SBStationEcc::registerEcc(const SBEccentricity& Ecc_)
{
  bool			IsOK = TRUE;

  SBEccentricity*	ecc;
  if ( !(ecc=findEcc(Ecc_.tStart())) )
    {
      Eccs.inSort(new SBEccentricity(Ecc_));
      Log->write(SBLog::DBG, SBLog::STATION | SBLog::DATA, ClassName() +
		 ": Eccentricitiy of the station [" + toString() + 
		 "] aka `" + Ecc_.aka() + "' has been registered with the interval: [" + 
		 Ecc_.tStart().toString(SBMJD::F_Short) +  ":" + 
		 Ecc_.tFinis().toString(SBMJD::F_Short) + "]");
    }
  else if ( !(*ecc == Ecc_) ) // check && complain
    {
      if ((ecc->ecc()-Ecc_.ecc()).module()<=1.0e-5) // it's the same ecc, resize it
	{
	  if (ecc->tStart()>Ecc_.tStart())
	    ecc->setTStart(Ecc_.tStart());
	  if (ecc->tFinis()<Ecc_.tFinis())
	    ecc->setTFinis(Ecc_.tFinis());
	}
      else if (ecc->tStart()<Ecc_.tStart() && Ecc_.tFinis()<ecc->tFinis()) // split
	{
	  SBEccentricity*	eccLeft = new SBEccentricity(*ecc);
	  SBEccentricity*	eccRight= new SBEccentricity(*ecc);
	  
	  eccLeft ->setTFinis(Ecc_.tStart());
	  eccRight->setTStart(Ecc_.tFinis());
	  
	  Eccs.remove(ecc);

	  Eccs.inSort(eccLeft);
	  Eccs.inSort(eccRight);
	  Eccs.inSort(new SBEccentricity(Ecc_));
	  
	  Log->write(SBLog::DBG, SBLog::STATION | SBLog::DATA, ClassName() +
		     ": Eccentricitiy of the station `" + Ecc_.aka() +  "' [" + toString() + 
		     "] has been splitted on the interval: [" + 
		     eccLeft->tStart().toString(SBMJD::F_Short) +  ":" + 
		     eccLeft->tFinis().toString(SBMJD::F_Short) + "], [" +
		     Ecc_.tStart().toString(SBMJD::F_Short) +  ":" + 
		     Ecc_.tFinis().toString(SBMJD::F_Short) + "], [" +
		     eccRight->tStart().toString(SBMJD::F_Short) +  ":" + 
		     eccRight->tFinis().toString(SBMJD::F_Short) + "]");
	}
      else if (! ((ecc->tStart() == Ecc_.tStart()) && (ecc->tFinis() == Ecc_.tFinis()) && 
	     (ecc->ecc() == Ecc_.ecc()) && (ecc->type() == Ecc_.type())) ) // it's a real difference
	{
	  IsOK = FALSE;
	  Log->write(SBLog::WRN, SBLog::STATION | SBLog::DATA, ClassName() +
		     ": WARNING: Eccentricity for the station `" + Ecc_.aka() + "' [" + toString() +
		     "] already registered with different values for aka `" + ecc->aka() + "'");
	  Log->write(SBLog::WRN, SBLog::STATION | SBLog::DATA, ClassName() + 
		     ": Old: Interval: [" + ecc->tStart().toString(SBMJD::F_Short) + " : " + 
		     + ecc->tFinis().toString(SBMJD::F_Short) + "] type: " + 
		     (ecc->type()==SBEccentricity::ET_NEU?"NEU":"XYZ") + "; ECC: " + 
		     QString().sprintf("(%7.3f, %7.3f, %7.3f)", 
				       ecc->ecc().at(X_AXIS), 
				       ecc->ecc().at(Y_AXIS), 
				       ecc->ecc().at(Z_AXIS)));
	  Log->write(SBLog::WRN, SBLog::STATION | SBLog::DATA, ClassName() + 
		     ": New: Interval: [" + Ecc_.tStart().toString(SBMJD::F_Short) + " : " + 
		     + Ecc_.tFinis().toString(SBMJD::F_Short) + "] type: "  + 
		     (Ecc_.type()==SBEccentricity::ET_NEU?"NEU":"XYZ") + "; ECC: " + 
		     QString().sprintf("(%7.3f, %7.3f, %7.3f)", 
				       Ecc_.ecc().at(X_AXIS), 
				       Ecc_.ecc().at(Y_AXIS), 
				       Ecc_.ecc().at(Z_AXIS)));
	  
	};
    };
  //  else
  //    Log->write(SBLog::DBG, SBLog::STATION | SBLog::DATA, ClassName() +
  //	       ": Eccentricitiy of the station [" + toString() + 
  //	       "] aka `" + Ecc_.aka() + "' already registered");
  return IsOK;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEcc implementation									*/
/*												*/
/*==============================================================================================*/
SBEcc::SBEcc()
  : QList<SBStationEcc>()
{
  setAutoDelete(TRUE);

  EccByID= new QDict<SBStationEcc>(17);
  EccByID->setAutoDelete(FALSE);
};

SBEcc::~SBEcc()
{
  if (EccByID)
    {
      delete EccByID;
      EccByID = NULL;
    };
};

SBEcc::SBEcc(const SBEcc& Ecc_)
  : QList<SBStationEcc>(Ecc_)
{
  setAutoDelete(TRUE);
  EccByID= new QDict<SBStationEcc>(Ecc_.count() + 5);
  EccByID->setAutoDelete(FALSE);
  fillDicts();
};

void SBEcc::fillDicts()
{
  if (EccByID->count())
    EccByID->clear();
  if (EccByID->size()<=count())
    EccByID->resize(count() + 5);

  for (SBStationEcc *E = first(); E; E = next())
    EccByID ->insert(E->toString(), E);
};

SBEccentricity* SBEcc::findEcc(const SBStationID& ID_, const SBMJD& T)
{
  SBStationEcc		*StEcc = NULL;
  SBEccentricity	*ecc   = NULL;

  if ( (StEcc = EccByID->find(ID_.toString())) )
    ecc = StEcc->findEcc(T);
  else if (ID_.ptType() == SBStation::Pt_Marker)
    Log->write(SBLog::DBG, SBLog::STATION, ClassName() +
	       ": The station [" + ID_.toString() + "] has not got any ECC");
  return ecc;
};

bool SBEcc::registerEcc(const SBStationID& ID, const SBEccentricity& Ecc_)
{
  bool			IsOK = TRUE;
  SBStationEcc		*StEcc;

  if (ID.ptType() == SBStation::Pt_Antenna && Ecc_.ecc() == v3Zero) // it's a trivial eccentricities
    return IsOK;

  if ( !(StEcc = EccByID->find(ID.toString())) )
    {
      StEcc = new SBStationEcc();
      StEcc->setID(ID);
      inSort(StEcc);
      EccByID ->insert(StEcc->toString(), StEcc);

      Log->write(SBLog::DBG, SBLog::STATION | SBLog::DATA, ClassName() +
		 ": The station [" + StEcc->toString() + 
		 "] has been registered with the database of eccentricities");
    };
  
  if ( !(IsOK=StEcc->registerEcc(Ecc_)) )
    Log->write(SBLog::DBG, SBLog::STATION | SBLog::DATA, ClassName() +
	       ": Eccentricity registration failed for the station [" + StEcc->toString() + "]");

  return IsOK;
};

bool SBEcc::importEccDat(const QString& FileName, SB_TRF* Wtrf)
{
  bool IsLoaded = FALSE;

  QFile	f(FileName);
  if (f.open(IO_ReadOnly))
    {
      QTextStream	ts(&f);
      QString		Str;
      
      SBEccentricity::EccType	EType=SBEccentricity::ET_XYZ;
      SBEccentricity		Ecc;
      SBStationID		ID;
      int			nCDP;
      
      SBMJD			TStart;
      SBMJD			TFinis;
      double			r1, r2, r3;
      bool			IsOK;
      bool			IsValidEcc;
      
      Str = ts.readLine();
      if (Str.find("# ECC-FORMAT V 1.0")!=-1)
	{
	  while (!ts.eof())
	    {
	      Str = ts.readLine();
	      IsValidEcc = FALSE;
	      
	      if (Str.mid(0,1) != "$" && Str.mid(0,1) != "#") // it's a comment
		{
		  //          1         2         3         4         5         6         7         8         9
		  //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
		  //  AIRA     7348  1970.01.01-00:00  2050.01.01-00:00      0.0        0.0        0.0     XYZ 
		  //  PT.REYES 7251  1983.08.27-00:00  2050.01.01-00:00      0.3298     2.4913     4.4005  NEU
		  
		  ID = Wtrf->lookupID(Str.mid(2,8));
		  nCDP = Str.mid(11, 4).toInt(&IsOK);
		  if (!IsOK || nCDP<999)
		    {
		      nCDP=0;
		      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": not a CDP number: [" + 
				 Str.mid(11, 4) + "]");
		    };
		  
		  if (ID.isValidId())
		    IsValidEcc = TRUE;
		  else 
		    {
		      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + 
				 ": cannot find proper station for an alias (IVS station name) `" + 
				 Str.mid(2,8) + "'");
		      if (nCDP>999 && nCDP<=9999)
			{
			  SBStation *Station = Wtrf->lookupStation(nCDP);
			  if (Station) // it's a luck
			    {
			      IsValidEcc = TRUE;
			      ID.setID(Station->id());
			      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": got a station `" + 
					 Station->name() + "' [" + ID.toString() + "] for an alias `" + 
					 Str.mid(2,8) + "'!");
			      // register an alias:
			      Wtrf->aliasDict()->insert((const char*)Str.mid(2,8), 
							new QString(Station->name() + " " + 
								    Station->id().toString()));
			      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": added an alias `" + 
					 Str.mid(2,8) + "' for the station " + Station->name() + "' [" + 
					 ID.toString() + "]");
			    };
			};
		    };
		  
		  if (IsValidEcc)
		    {
		      TStart.setMJD(SBMJD::F_ECCDAT, Str.mid(17, 16));
		      if (!TStart.isGood())
			{
			  Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": cannot get TStart field");
			  IsValidEcc = FALSE;
			};
		      
		      if (IsValidEcc)
			{
			  TFinis.setMJD(SBMJD::F_ECCDAT, Str.mid(35, 16));
			  if (!TFinis.isGood())
			    {
			      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": cannot get TFinis field");
			      IsValidEcc = FALSE;
			    };
			};
		      
		      if (IsValidEcc)
			{
			  r1 = Str.mid(53,10).stripWhiteSpace().toDouble(&IsOK);
			  if (!IsOK)
			    {
			      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": cannot get r1 field: [" +
					 Str.mid(53,10) + "]");
			      IsValidEcc = FALSE;
			    };
			};
		      if (IsValidEcc)
			{
			  r2 = Str.mid(64,10).stripWhiteSpace().toDouble(&IsOK);
			  if (!IsOK) 
			    {
			      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": cannot get r2 field: [" +
					 Str.mid(64,10) + "]");
			      IsValidEcc = FALSE;
			    };
			};
		      if (IsValidEcc)
			{
			  r3 = Str.mid(75,10).stripWhiteSpace().toDouble(&IsOK);
			  if (!IsOK) 
			    {
			      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": cannot get r3 field: [" +
					 Str.mid(75,10) + "]");
			      IsValidEcc = FALSE;
			    };
			};
		      
		      if (IsValidEcc)
			{
			  if (Str.mid(87,3) == "NEU")
			    EType = SBEccentricity::ET_NEU;
			  else if (Str.mid(87,3) == "XYZ")
			    EType = SBEccentricity::ET_XYZ;
			  else
			    {
			      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": cannot get Type field");
			      IsValidEcc = FALSE;
			    };
			};
		      
		      if (IsValidEcc)
			{
			  Ecc.setAka(Str.mid(2,8));
			  Ecc.setTStart(TStart);
			  Ecc.setTFinis(TFinis);
			  Ecc.setEcc(Vector3(r1, r2, r3));
			  Ecc.setType(EType);
			  //Wtrf->ecc().registerEcc(ID, Ecc);
			  registerEcc(ID, Ecc);
			  IsLoaded = TRUE;
			}
		      else
			Log->write(SBLog::DBG, SBLog::STATION, ClassName() + 
				   ": cannot parse the string [" + Str + "]; entry ignored");
		    }
		  else
		    Log->write(SBLog::DBG, SBLog::STATION, ClassName() + 
			       ": cannot find proper station for an alias (IVS station name) [" + 
			       Str.mid(2,8) + "] and a CDP number: [" + Str.mid(11, 4) + "] entry is ignored");
		};
	    };  
	}
      else
	{
	  Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": unknown format of the `" + FileName + "'");
	  IsLoaded = FALSE;
	};
      
      f.close();
      ts.unsetDevice();
    };
  return IsLoaded;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SB_TRF implementation									*/
/*												*/
/*==============================================================================================*/
int SB_TRF::lastUnusedNumber() const
{
  int i=99999;
  while (i>0 && SiteByDome->find(i)) i--;
  return i;
}

void SB_TRF::inSort(const SBSite* N)
{
  if (!N)
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot insert NULL");
      return;
    };
  if (10000>N->domeMajor() || N->domeMajor()>99999)
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": wrong sites's domeMajor");
      return;
    };
  if (find(N->domeMajor()))
    {
      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": site [" + N->name() 
		 + "] already inserted ");
      return;
    };
  SBCatalog::inSort(N); 
  SiteByDome->insert(N->domeMajor(), N);
};

void SB_TRF::prepareDicts()
{
  if (SiteByDome)
    delete SiteByDome;
  SiteByDome = new QIntDict<SBSite>(count() + 10);
  SiteByDome ->setAutoDelete(FALSE);

  if (StationByCDP)
    delete StationByCDP;
  StationByCDP = new QIntDict<SBStation>(count() + 10);
  StationByCDP ->setAutoDelete(FALSE);

  if (StationNames) 
    {
      delete StationNames;
      StationNames = NULL;
    };

  for (SBSite *W=first(); W; W=next())
    {
      //W->prepareDicts();
      SiteByDome->insert(W->domeMajor(), W);
      SBStationIterator it(W->stations());
      SBStation *Station;
      while ((Station = it.current()))
	{
	  if (Station->CDP()>999 && Station->CDP()<=9999)
	    StationByCDP ->insert(Station->CDP(), Station);
	  ++it;
	};
    };

};

void SB_TRF::prepareReferences()
{
  if (StationNames) delete StationNames;
  StationNames = new SBNList;
  StationNames -> setAutoDelete(TRUE);

  for (SBSite *W=first(); W; W=next()) 
    for (SBStation *S=W->Stations.first(); S; S=W->Stations.next()) 
      StationNames->inSort(new SBNamed(S->name() + " " +  S->id().toString()));
};

SBStation* SB_TRF::lookupStation(const QString& Alias)
{
  SBStation*	Station = NULL;
  QString*	str = Aliases->find(Alias);
  if (str) 
    {
      SBStationID	id;
      id.setID(str->right(9));
      if (id.isValidId()) 
	Station = find(id);
      else
	Log->write(SBLog::INF, SBLog::STATION, ClassName() + ": cannot convert [" + Alias +
		   "] into ID");
    }
  return Station;
};

SBStationID SB_TRF::lookupID(const QString& Alias)
{
  QString*	str = Aliases->find(Alias);
  SBStationID	id;
  if (str) 
    id.setID(str->right(9));
  return id;
};

SBStation* SB_TRF::lookupNearest(const SBMJD& MJD, const Vector3& R, double &MinR, Technique Tech)
{
  SBStation*	Station=NULL;
  SBStation*	W;
  double	r;
  SBSite*	Site;
  MinR=2.0*6400000.0;
  for (Site=first(); Site; Site=next())
    for (W=Site->Stations.first(); W; W=Site->Stations.next())
      if ((Tech==TECH_ANY || W->tech()==Tech) && 
	  MinR>(r=((W->r(MJD) + W->V*(MJD-Epoch)/365.25) - R).module()))
	{
	  MinR=r;
	  Station=W;
	};
  if (Station)
    Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": found [" + Station->name()
	       + "] as the nearest station; the distance is " + 
	       QString().setNum(MinR, 'g', 4) + " meters");
  return Station;
};

bool SB_TRF::setSiteName(int DomeMajor_, const QString& Name_)
{
  SBSite *Site;
  if (!(Site = find(DomeMajor_))) return FALSE;
  Dict->remove(Site->name());
  Site->setName(Name_);
  Dict->insert(Site->name(), Site);
  return TRUE;
};

void SB_TRF::prepare4Run()
{
  QString BaseName = SetUp->path2Output() + SetUp->project() + "/";

  Stations = new QList<SBStation>;
  Stations -> setAutoDelete(FALSE);
  for (SBSite *W=first(); W; W=next())
    for (SBStation *S=W->Stations.first(); S; S=W->Stations.next())
      {
	S->createParameters();
#ifdef ELEV_DEBUG_OUTPUT
	S->openOutFile(BaseName);
#endif
	Stations->append(S);
      };
};

void SB_TRF::finisRun()
{
  for (SBStation *S=Stations->first(); S; S=Stations->next())
    {
      S->releaseParameters();
#ifdef ELEV_DEBUG_OUTPUT
      S->closeOutFile();
#endif
    };
  delete Stations;
  Stations = NULL;
};

void SB_TRF::prepareEccs4Session(const SBMJD& T, SBVLBISession *Session)
{
  SBStationID	id;
  for (SBStation *S=Stations->first(); S; S=Stations->next())
    {
      id.setID(S->id());
      if (Session->stationList()->find(&id) /*&& (S->pointType() == SBStation::Pt_Marker)*/ )// check for eccs:
	{
	  SBEccentricity*	Ecc=NULL;
	  if ( (Ecc=ECCs.findEcc(id, T)) )
	    {
	      Vector3 eccVector;
	      if (Ecc->type() == SBEccentricity::ET_NEU)
		{
		  eccVector[VERT] = Ecc->ecc().at(Z_AXIS); //U
		  eccVector[EAST] = Ecc->ecc().at(Y_AXIS); //E
		  eccVector[NORT] = Ecc->ecc().at(X_AXIS); //N
		  
		  eccVector = S->fmVEN()*eccVector;
		}
	      else 
		eccVector = Ecc->ecc();

	      S->setEcc(eccVector);
	    }
	  else if (S->pointType() == SBStation::Pt_Marker)
	    Log->write(SBLog::WRN, SBLog::DATA | SBLog::STATION, ClassName() + 
		       ": WARNING: cannot find eccentricity for the station `" + S->name() + 
		       "' [" + S->id().toString() + "]");
	};
    };
  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SB_TRF's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SB_TRF& TRF)
{
  s << (const SBCatalog&)TRF << TRF.ECCs;
  return s;
};

QDataStream &operator>>(QDataStream& s, SB_TRF& TRF)
{
  s >> (SBCatalog&)TRF >> TRF.ECCs;
//   TRF.ECCs.clear();
  TRF.prepareDicts();
  return s;
};

QTextStream &operator<<(QTextStream& s, const SB_TRF& /*TRF*/)
{
  return s;
};

QTextStream &operator>>(QTextStream& s, SB_TRF& /*TRF*/)
{
  return s;
};

SBTS_site1994 &operator<<(SBTS_site1994& s, const SB_TRF& TRF)
{
  s << "File : IERS.SITE\n\nRelease : 22-DEC-1995\n\n\n  \n"
    << "                CATALOGUE OF IERS SITES\n\n"
    << "DOMES    Name              Country     Long.  Lat.   Plate\n"
    << "Number                                 d   m  d   m       \n"
    << "------   ---------------   ---------- ------- ------ ----- \n";
  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) s << *W;
  s << "\nEOF\n";
  return s;
};

SBTS_site1994 &operator>>(SBTS_site1994& s, SB_TRF& TRF)
{
  SBSite	*site;
  SBSite	*W;
  while (!s.eof())
    {
      site = new SBSite;
      s>>*site;
      if (site->domeMajor()!=99999) 
	{
	  if (!(W=TRF[site->domeMajor()]))
	    {
	      TRF.inSort(site);
	      Log->write(SBLog::DBG, SBLog::STATION, 
			 "SBTS_site1994: site `" + site->name() + "' has been inserted");
	    }
	  else
	    {
	      if (site->name()!="00000") TRF.setSiteName(site->domeMajor(), site->name());
	      W->  setCountry  (site -> country());
	      W->  setPlate    (site -> plate());
	      Log->write(SBLog::DBG, SBLog::STATION, 
			 "SBTS_site1994: site `" + site->name() + "' has been updated");
	      
	      delete site;
	    };
	}
      else delete site;
    };  
  return s;
};

SBTS_site2000 &operator<<(SBTS_site2000& s, const SB_TRF& TRF)
{
  s << "File : iers.site\n\nRelease : 02  8 13 \n\n"
    << "                          CATALOGUE OF IERS SITES\n\n"
    << "DOMES   Site Name                 Country              Long.    Lat.   Plate\n"
    << "Number                                                    d m     d m\n"
    << "------  ---------------           ----------           -------  ------ -----\n";
  
  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) s << *W;
  s << "\n";
  return s;
};

SBTS_site2000 &operator>>(SBTS_site2000& s, SB_TRF& TRF)
{
  SBSite	*site;
  SBSite	*W;
  while (!s.eof())
    {
      site = new SBSite;
      s>>*site;
      if (site->domeMajor()!=99999) 
	{
	  if (!(W=TRF[site->domeMajor()]))
	    {
	      TRF.inSort(site);
	      Log->write(SBLog::DBG, SBLog::STATION, 
			 "SBTS_site2000: site `" + site->name() + "' has been inserted");
	    }
	  else
	    {
	      if (site->name()!="00000") TRF.setSiteName(site->domeMajor(), site->name());
	      W->  setCountry  (site -> country());
	      W->  setPlate    (site -> plate());
	      Log->write(SBLog::DBG, SBLog::STATION, 
			 "SBTS_site2000: site `" + site->name() + "' has been updated");
	      
	      delete site;
	    };
	}
      else delete site;
    };  
  return s;
};

SBTS_dir1994 &operator<<(SBTS_dir1994& s, const SB_TRF& TRF)
{
  s << "File : iers_dir.sta                                                             \n"
    << "                                                                                \n"
    << "Release: 08-MAR-1996                                                            \n"
    << "                                                                                \n"
    << "                        DIRECTORY OF IERS STATIONS                              \n"
    << "                                                                                \n"
    << "                                                                                \n"
    << " DOMES  SITE           4-CHAR  CDP    DESIGNATION                                  \n"
    << " Number            CLS ID      NUM.                                                \n"
    << " ------ ---------- --- ------  ----   ---------------------------------------      \n";
  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) s << *W;
  s << "\nEOF\n";
  return s;
};

SBTS_dir1994 &operator>>(SBTS_dir1994& s, SB_TRF& TRF)
{
  SBSite	*site;
  SBSite	*W;
  while (!s.eof())
    {
      site = new SBSite;
      s>>*site;
      if (site->domeMajor()!=99999) 
	{
	  if (!(W=TRF[site->domeMajor()]))
	    {
	      TRF.inSort(site);
	      Log->write(SBLog::DBG, SBLog::STATION, 
			 "SBTS_dir1994: site `" + site->name() + "' has been inserted (?!)");
	    }
	  else
	    {
	      // well, it's seems the site's name in *_dir.sta file more 
	      // informative then the *.site
	      if (W->name()!=site->name()) TRF.setSiteName(site->domeMajor(), site->name());
	      
	      // update stations info 4 this site:
	      if (site->Stations.count())
		{
		  SBStationIterator it(site->Stations);
		  SBStation*	S_in, *S;
		  int		i;
		  while ((S_in=it.current()))
		    {
		      if ((i=W->Stations.find(S_in))!=-1) // update
			{
			  S = W->Stations.at(i);
			  if (S_in->CDP()!=9999) S->setCDP(S_in->CDP());
			  if (S_in->name()!="_NONAME_") S->setName(S_in->name());
			  S->setDescription(S_in->description());
			}
		      else W->addStation(new SBStation(W, *S_in));
		      ++it;
		    };
		};

	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir1994: site `" + site->name() 
			 + "' has been updated");
	      delete site;
	    };
	}
      else delete site;
    };  
  return s;
};

SBTS_dir2000 &operator<<(SBTS_dir2000& s, const SB_TRF& TRF)
{
  s << "File : iers_dir.sta\n\nRelease: 02  8 13 \n\n"
    << "                          DIRECTORY OF IERS STATIONS\n"
    << "\nDOMES  SITE        CDP  IGS DORIS DESIGNATION\nNumber\n"
    << "------------------------------------------------------------------------------\n\n";

  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) s << *W;
  s << "\n";
  return s;
};

SBTS_dir2000 &operator>>(SBTS_dir2000& s, SB_TRF& TRF)
{
  SBSite	*site;
  SBSite	*W;
  while (!s.eof())
    {
      site = new SBSite;
      s>>*site;
      if (site->domeMajor()!=99999) 
	{
	  if (!(W=TRF[site->domeMajor()]))
	    {
	      TRF.inSort(site);
	      Log->write(SBLog::DBG, SBLog::STATION, 
			 "SBTS_dir2000: site `" + site->name() + "' has been inserted (?!)");
	    }
	  else
	    {
	      // well, it's seems the site's name in *_dir.sta file more 
	      // informative then the *.site
	      if (W->name()!=site->name()) 
		TRF.setSiteName(site->domeMajor(), site->name());
	      
	      // update stations info for this site:
	      if (site->Stations.count())
		{
		  SBStationIterator it(site->Stations);
		  SBStation*	S_in, *S;
		  int		i;
		  while ((S_in=it.current()))
		    {
		      if ((i=W->Stations.find(S_in))!=-1) // update
			{
			  S = W->Stations.at(i);
			  if (S_in->CDP()!=9999) 
			    S->setCDP(S_in->CDP());
			  if (S_in->name()!="_NONAME_") 
			    S->setName(S_in->name());
			  S->setDescription(S_in->description());
			}
		      else 
			W->addStation(new SBStation(W, *S_in));
		      ++it;
		    };
		};

	      Log->write(SBLog::DBG, SBLog::STATION, "SBTS_dir2000: site `" + site->name() 
			 + "' has been updated");
	      delete site;
	    };
	}
      else delete site;
    };  
  return s;
};

SBTS_ssc1994 &operator<<(SBTS_ssc1994& s, const SB_TRF& TRF)
{
  const char* sClassC[6] = {"A", "B", "C", "D", "E", "Z"};
  s << "File : ITRF.SSC" << sClassC[s.ClassCoord] << " : "
    << TRF.label() << " CLASS " << sClassC[s.ClassCoord] << " STATION COORDINATES AT EPOCH "
    << (const char*)TRF.epoch().toString(SBMJD::F_Year) << "\n" 
    << "\nDOMES NB. SITE NAME       TECH. ID.       *          "
    << "      X/Vx         Y/Vy         Z/Vz           Sigmas\n"
    << "                                                         "
    << "-------------------------m/m/y------------------------  \n"
    << "--------------------------------------------------------"
    << "------------------------------------------------------- \n";
  
  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) s << *W;

  s << "________________________________________________________"
    << "_______________________________________________________\n";
  return s;
};

SBTS_ssc2000 &operator<<(SBTS_ssc2000& s, const SB_TRF& TRF)
{
  s << "                      ITRF2000 STATION POSITIONS AT EPOCH "
    << (const char*)TRF.epoch().toString(SBMJD::F_Year)
    << " AND VELOCITIES\n"
    << "                                          VLBI STATIONS\n\n\n"
    << "DOMES NB. SITE NAME        TECH. ID.        X/Vx         Y/Vy         Z/Vz           Sigmas       SOLN\n"
    << "                                            -------------------------m/m/y----------------------- \n"
    << "-----------------------------------------------------------------------------------------------------\n";
  
  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) s << *W;

  s << "________________________________________________________"
    << "_______________________________________________________\n";
  return s;
};

SBTS_blqReq &operator<<(SBTS_blqReq& s, const SB_TRF& TRF)
{
  s << "Data should be submitted to the URL http://www.oso.chalmers.se/~loading/\n\n";
  s << "Name of station_________| |Longitude (deg) | Latitude (deg) | Height (m)\n";
  
  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) s << *W;

  return s;
};

SBTS_ssc1994 &operator>>(SBTS_ssc1994& s, SB_TRF& TRF)
{
  QString	Str;
  QString	a;
  int		i;
  //  int		len;
  bool		isOK;
  SBStation::SClassCoord	ClassCoord = SBStation::Class_Z;
  
  Str = s.readLine();
  QRegExp r("CLASS [A-Z]+ STATION");
  if ((i=r.match(Str))!=-1)
    {
      a = Str.mid(i+6,1);
      if (a=="A") ClassCoord = SBStation::Class_A;
      else if (a=="B") ClassCoord = SBStation::Class_B;
      else if (a=="C") ClassCoord = SBStation::Class_C;
      else if (a=="D") ClassCoord = SBStation::Class_D;
      else if (a=="E") ClassCoord = SBStation::Class_E;
      else if (a=="Z") ClassCoord = SBStation::Class_Z;
      else Log->write(SBLog::ERR, SBLog::STATION, "SBTS_ssc1994: cannot get class from [" + a + "]");
    }
  else
    Log->write(SBLog::ERR, SBLog::STATION, "SBTS_ssc1994: cannot match to class [" + Str + "]");
    
  if ((i=Str.find("AT EPOCH "))!=-1)
    {
      a = Str.mid(i+9,6);
      SBMJD	epoch(TRF.epoch());
      a.toDouble(&isOK);
      if (!isOK)
	Log->write(SBLog::ERR, SBLog::STATION, "SBTS_ssc1994: cannot get epoch from [" + a + "]");
      else
	{
	  epoch.setMJD(SBMJD::F_Year, a);
	  if (epoch!=TRF.epoch())
	    {
	      Log->write(SBLog::INF, SBLog::STATION, "SBTS_ssc1994: change TRF epoch from " + 
			 TRF.epoch().toString(SBMJD::F_Short) + " to " +
			 epoch.toString(SBMJD::F_Short));
	      TRF.setEpoch(epoch);
	    };
	};
    }
  else 
    Log->write(SBLog::ERR, SBLog::STATION, "SBTS_ssc1994: cannot find epoch in [" + Str + "]");
    
  SBStationID	ID;
  SBSite	*Site;
  SBStation	*Station;
  double	x, dx;
  double	y, dy;
  double	z, dz;
  Vector3	R, dR;
  while (!s.eof())
    {
      //          1         2         3         4         5         6         7         8         9         10        11
      //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
      //10402M004 ONSALA            GPS ONSA  5  7              3370658.732   711876.975  5349786.833  .006  .007  .005 
      //10402M004                                                    -.0150        .0151        .0061 .0014 .0016 .0013 

      Str= s.readLine();
      if (SBStationID::isValidStr( (a = Str.mid(0,9)) ))
	{
	  ID.setID(a);
	  if (Str.mid(10,8)=="        ") // this is a velocity
	    {
	      x = Str.mid(60,7).toDouble(&isOK);
	      if (!isOK)
		{
		  x=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong x-vel: [" + 
			     Str.mid(60,7) + "] in `" + Str + "'");
		};
	      y = Str.mid(73,7).toDouble(&isOK);
	      if (!isOK)
		{
		  y=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong y-vel: [" + 
			     Str.mid(73,7) + "] in `" + Str + "'");
		};
	      z = Str.mid(86,7).toDouble(&isOK);
	      if (!isOK)
		{
		  z=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong z-vel: [" + 
			     Str.mid(86,7) + "] in `" + Str + "'");
		};

	      dx = Str.mid(93,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dx=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong dx-vel: [" + 
			     Str.mid(93,6) + "] in `" + Str + "'");
		};
	      dy = Str.mid(99,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dy=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong dy-vel: [" + 
			     Str.mid(99,6) + "] in `" + Str + "'");
		};
	      dz = Str.mid(105,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dz=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong dz-vel: [" + 
			     Str.mid(105,6) + "] in `" + Str + "'");
		};

	      R [X_AXIS] =  x;
	      R [Y_AXIS] =  y;
	      R [Z_AXIS] =  z;
	      dR[X_AXIS] = dx;
	      dR[Y_AXIS] = dy;
	      dR[Z_AXIS] = dz;

	      Station = TRF[ID];
	      if (!Station) // station not found..
		{
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: station [" + ID.toString()
			     + "] not found in database; the entry is `" + Str + "'");
		  if ( !(Site = TRF[ID.domeMajor()]) ) // site not found..
		    {
		      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: site for [" + ID.toString()
				 + "] not found in database; the entry is `" + Str + "'");
		      Site = new SBSite;
		      Site -> setDomeMajor(ID.domeMajor());
		      TRF.inSort(Site);
		    };
		  Station = new SBStation(Site);
		  Station->setDomeMinor(ID.domeMinor());
		  Station->setPointType(ID.ptType());
		  Site->addStation(Station);
		};
	      Station->setV(R);
	      Station->setV_err(dR);
	      Station->addAttr(SBStation::VelocImported);
	      Station->setClassCoord(ClassCoord);
	    }
	  else // coordinates & other info
	    {
	      Technique	Tech=TECH_UNKN;
	      QString	CharID;
	      QString	StationName;
	      int	CDPNum;
	      StationName = Str.mid(10,16).simplifyWhiteSpace().copy();
	      
	      a = Str.mid(26,5);
	      if (a=="  GPS") Tech=TECH_GPS;
	      else if (a==" VLBI") Tech=TECH_VLBI;
	      else if (a=="  SLR") Tech=TECH_SLR;
	      else if (a=="  TIE") Tech=TECH_TIE;
	      else if (a=="DORIS") Tech=TECH_DORIS;
	      else if (a=="  LLR") Tech=TECH_LLR;
	      else Log->write(SBLog::WRN, SBLog::STATION, 
			      "SBTS_ssc1994: unknown tech type: [" + a + "] in `" + Str + "'");
	      CharID = Str.mid(32,4);
	      CDPNum = CharID.toInt(&isOK);
	      if (!isOK) CDPNum = 9999;

	      x = Str.mid(55,12).toDouble(&isOK);
	      if (!isOK)
		{
		  x=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong x-coo: [" + 
			     Str.mid(55,12) + "] in `" + Str + "'");
		};
	      y = Str.mid(68,12).toDouble(&isOK);
	      if (!isOK)
		{
		  y=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong y-coo: [" + 
			     Str.mid(68,12) + "] in `" + Str + "'");
		};
	      z = Str.mid(81,12).toDouble(&isOK);
	      if (!isOK)
		{
		  z=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong z-coo: [" + 
			     Str.mid(81,12) + "] in `" + Str + "'");
		};

	      dx = Str.mid(94,5).toDouble(&isOK);
	      if (!isOK)
		{
		  dx=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong dx-coo: [" + 
			     Str.mid(94,5) + "] in `" + Str + "'");
		};
	      dy = Str.mid(100,5).toDouble(&isOK);
	      if (!isOK)
		{
		  dy=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong dy-coo: [" + 
			     Str.mid(100,5) + "] in `" + Str + "'");
		};
	      dz = Str.mid(106,5).toDouble(&isOK);
	      if (!isOK)
		{
		  dz=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: wrong dz-coo: [" + 
			     Str.mid(106,5) + "] in `" + Str + "'");
		};
	      
	      R [X_AXIS] =  x;
	      R [Y_AXIS] =  y;
	      R [Z_AXIS] =  z;
	      dR[X_AXIS] = dx;
	      dR[Y_AXIS] = dy;
	      dR[Z_AXIS] = dz;
	      
	      Station = TRF[ID];
	      if (!Station) // station not found..
		{
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: station [" + ID.toString()
			     + "] not found in database; the entry is `" + Str + "'");
		  if ( !(Site = TRF[ID.domeMajor()]) ) // site not found..
		    {
		      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc1994: site for [" + ID.toString()
				 + "] not found in database; the entry is `" + Str + "'");
		      Site = new SBSite(Str.mid(10,16));
		      Site -> setDomeMajor(ID.domeMajor());
		      TRF.inSort(Site);
		    };
		  Station = new SBStation(Site, StationName);
		  Station->setDomeMinor(ID.domeMinor());
		  Station->setPointType(ID.ptType());
		  Site->addStation(Station);
		};
	      Station->setName(StationName);
	      Station->setComments("no comment");
	      //	      Station->setR(R);
	      //	      Station->setR_err(dR);
	      Station->setCoords(R, dR, v3Zero, v3Unit, TZero, "Initial data");
	      Station->setTech(Tech);
	      if (CDPNum!=9999)
		{
		  if (Station->CDP()!=CDPNum)
		    {
		      Log->write(SBLog::INF, SBLog::STATION, "SBTS_ssc1994: station [" +
				 ID.toString() + "] change CDP Number from `" + 
				 QString().setNum(Station->CDP()) + "' to `" + 
				 QString().setNum(CDPNum) + "'");
		      Station->setCDP(CDPNum);
		      Station->setCharID("    ");
		    };
		}
	      else Station->setCharID(CharID);
	      Station->addAttr(SBStation::OrigImported);
	      Station->setClassCoord(ClassCoord);
	    };
	}
      else 
	Log->write(SBLog::DBG, SBLog::STATION, "SBTS_ssc1994: not an ssc entry: `" + Str + "'");
    };
  
  return s;
};

SBTS_ssc2000 &operator>>(SBTS_ssc2000& s, SB_TRF& TRF)
{
  QString	Str;
  QString	a;
  int		i;
  //  int		len;
  bool		isOK;
  SBStation::SClassCoord	ClassCoord = SBStation::Class_A;
  
  Str = s.readLine();

  if ((i=Str.find("AT EPOCH "))!=-1)
    {
      a = Str.mid(i+9,6);
      SBMJD	epoch(TRF.epoch());
      a.toDouble(&isOK);
      if (!isOK)
	Log->write(SBLog::ERR, SBLog::STATION, "SBTS_ssc2000: cannot get epoch from `" + a + "'");
      else
	{
	  epoch.setMJD(SBMJD::F_Year, a);
	  if (epoch!=TRF.epoch())
	    {
	      Log->write(SBLog::INF, SBLog::STATION, "SBTS_ssc2000: change TRF epoch from " + 
			 TRF.epoch().toString(SBMJD::F_Short) + " to " +
			 epoch.toString(SBMJD::F_Short));
	      TRF.setEpoch(epoch);
	    };
	};
    }
  else 
    Log->write(SBLog::ERR, SBLog::STATION, "SBTS_ssc2000: cannot find epoch in `" + Str + "'");
    
  SBStationID	ID;
  SBSite	*Site;
  SBStation	*Station;
  double	x, dx;
  double	y, dy;
  double	z, dz;
  Vector3	R, dR;
  while (!s.eof())
    {
      //          1         2         3         4         5         6         7         8         9         10
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
      //40453M001 CARROLLTON        VLBI 7228   453520.717 -5300506.348  3507207.45917.56317.56717.560
      //40453M001                                    .0021        .0451        .00051.85731.85791.8569
      //40454M001 LEONARD           VLBI 7292  -522231.629 -5145676.885  3720152.352 2.191 2.192 2.192
      //40454M001                                   -.0124        .0017       -.0044 .2342 .2343 .2343
      //10003S001 TOULOUSE         DORIS TLSA  4628047.462   119670.346  4372787.806  .003  .003  .003
      //10002M006 GRASSE             GPS GRAS  4581691.012   556114.680  4389360.696  .002  .001  .002
      //10002S001 GRASSE             SLR 7835  4581691.641   556159.539  4389359.491  .002  .001  .002
      //12711S001 BOLOGNA           VLBI 7230  4461369.979   919596.826  4449559.199  .002  .001  .002
      //12711S001                                   -.0187        .0200        .0086 .0004 .0002 .0004
      //          1         2         3         4         5         6         7         8         9         10       11        12
      //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012
      //12711S001 BOLOGNA           VLBI 7230  4461369.985   919596.819  4449559.208  .002  .001  .002 2 AFTER 96:341
      //12711S001                                   -.0187        .0200        .0086 .0004 .0002 .0004
      //40408S002 FAIRBANKS         VLBI 7225 -2281547.431 -1453645.159  5756993.085 0.014 0.011 0.029  6 04:090:00000 04:365:00000
      //40408S002                                   -.0169       -.0047       0.0007 .0030 .0024 .0064


      Str= s.readLine();
      if (SBStationID::isValidStr( (a = Str.mid(0,9)) ))
	{
	  ID.setID(a);
	  if (Str.mid(10,8)=="        ") // this is a velocity
	    {
	      x = Str.mid(43,7).toDouble(&isOK);
	      if (!isOK)
		{
		  x=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong x-vel: [" + 
			     Str.mid(43,7) + "] in `" + Str + "'");
		};
	      y = Str.mid(56,7).toDouble(&isOK);
	      if (!isOK)
		{
		  y=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong y-vel: [" + 
			     Str.mid(56,7) + "] in `" + Str + "'");
		};
	      z = Str.mid(69,7).toDouble(&isOK);
	      if (!isOK)
		{
		  z=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong z-vel: [" + 
			     Str.mid(69,7) + "] in `" + Str + "'");
		};

	      dx = Str.mid(76,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dx=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong dx-vel: [" + 
			     Str.mid(76,6) + "] in `" + Str + "'");
		};
	      dy = Str.mid(82,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dy=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong dy-vel: [" + 
			     Str.mid(82,6) + "] in `" + Str + "'");
		};
	      dz = Str.mid(88,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dz=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong dz-vel: [" + 
			     Str.mid(88,6) + "] in `" + Str + "'");
		};

	      R [X_AXIS] =  x;
	      R [Y_AXIS] =  y;
	      R [Z_AXIS] =  z;
	      dR[X_AXIS] = dx;
	      dR[Y_AXIS] = dy;
	      dR[Z_AXIS] = dz;

	      Station = TRF[ID];
	      if (!Station) // station not found..
		{
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: station [" + ID.toString()
			     + "] not found in database; the entry is `" + Str + "'");
		  if ( !(Site = TRF[ID.domeMajor()]) ) // site not found..
		    {
		      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: site for [" + ID.toString()
				 + "] not found in database; the entry is `" + Str + "'");
		      Site = new SBSite;
		      Site -> setDomeMajor(ID.domeMajor());
		      TRF.inSort(Site);
		    };
		  Station = new SBStation(Site);
		  Station->setDomeMinor(ID.domeMinor());
		  Station->setPointType(ID.ptType());
		  Site->addStation(Station);
		};
	      Station->setV(R);
	      Station->setV_err(dR);
	      Station->addAttr(SBStation::VelocImported);
	      Station->setClassCoord(ClassCoord);
	    }
	  else // coordinates & other info
	    {
	      Technique	Tech=TECH_UNKN;
	      QString	CharID;
	      QString	StationName;
	      int	CDPNum;
	      StationName = Str.mid(10,16).simplifyWhiteSpace().copy();
	      
	      a = Str.mid(27,5);
	      if (a=="  GPS") Tech=TECH_GPS;
	      else if (a==" VLBI") Tech=TECH_VLBI;
	      else if (a=="  SLR") Tech=TECH_SLR;
	      else if (a=="  TIE") Tech=TECH_TIE;
	      else if (a=="DORIS") Tech=TECH_DORIS;
	      else if (a=="  LLR") Tech=TECH_LLR;
	      else Log->write(SBLog::WRN, SBLog::STATION, 
			      "SBTS_ssc2000: unknown tech type: [" + a + "] in `" + Str + "'");
	      CharID = Str.mid(33,4);
	      CDPNum = CharID.toInt(&isOK);
	      if (!isOK) CDPNum = 9999;

	      x = Str.mid(38,12).toDouble(&isOK);
	      if (!isOK)
		{
		  x=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong x-coo: [" + 
			     Str.mid(38,12) + "] in `" + Str + "'");
		};
	      y = Str.mid(51,12).toDouble(&isOK);
	      if (!isOK)
		{
		  y=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong y-coo: [" + 
			     Str.mid(51,12) + "] in `" + Str + "'");
		};
	      z = Str.mid(64,12).toDouble(&isOK);
	      if (!isOK)
		{
		  z=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong z-coo: [" + 
			     Str.mid(63,12) + "] in `" + Str + "'");
		};

	      dx = Str.mid(76,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dx=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong dx-coo: [" + 
			     Str.mid(76,6) + "] in `" + Str + "'");
		};
	      dy = Str.mid(82,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dy=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong dy-coo: [" + 
			     Str.mid(82,6) + "] in `" + Str + "'");
		};
	      dz = Str.mid(88,6).toDouble(&isOK);
	      if (!isOK)
		{
		  dz=0.0;
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: wrong dz-coo: [" + 
			     Str.mid(88,6) + "] in `" + Str + "'");
		};
	      
	      R [X_AXIS] =  x;
	      R [Y_AXIS] =  y;
	      R [Z_AXIS] =  z;
	      dR[X_AXIS] = dx;
	      dR[Y_AXIS] = dy;
	      dR[Z_AXIS] = dz;
	      
	      Station = TRF[ID];
	      if (!Station) // station not found..
		{
		  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: station [" + ID.toString()
			     + "] not found in database; the entry is `" + Str + "'");
		  if ( !(Site = TRF[ID.domeMajor()]) ) // site not found..
		    {
		      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: site for [" + ID.toString()
				 + "] not found in database; the entry is `" + Str + "'");
		      Site = new SBSite(Str.mid(10,16));
		      Site -> setDomeMajor(ID.domeMajor());
		      TRF.inSort(Site);
		    };
		  Station = new SBStation(Site, StationName);
		  Station->setDomeMinor(ID.domeMinor());
		  Station->setPointType(ID.ptType());
		  Site->addStation(Station);
		};
	      Station->setName(StationName);
	      Station->setComments("no comment");
	      //	      Station->setR(R);
	      //	      Station->setR_err(dR);

	      if (Str.length()>108 && Str.mid(97,5)=="AFTER")
		{
		  SBMJD		T_event;
		  T_event.setMJD(SBMJD::F_SINEX_S, Str.mid(103,6));
		  if (T_event.isGood())
		    {
		      Station->setCoords(R, dR, v3Zero, v3Unit, T_event, "ITRF2000 SSC record");
		      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: added new point for station " + 
				 Station->name() + ", [" + ID.toString() + "], Event: " + T_event.toString());
		    };
		}
	      else if (Str.length()>122 && Str.mid(98,12)!="00:000:00000")
		{
		  SBMJD		T_event;
		  T_event.setMJD(SBMJD::F_SINEX, Str.mid(98,12));
		  if (T_event.isGood())
		    {
		      Station->setCoords(R, dR, v3Zero, v3Unit, T_event, "ITRF2005 SSC record");
		      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_ssc2000: added new point for station " + 
				 Station->name() + ", [" + ID.toString() + "], Event: " + T_event.toString());
		    };
		}
	      else
		Station->setCoords(R, dR, v3Zero, v3Unit, TZero, "Initial data");
	      Station->setTech(Tech);
	      if (CDPNum!=9999)
		{
		  if (Station->CDP()!=CDPNum)
		    {
		      Log->write(SBLog::INF, SBLog::STATION, "SBTS_ssc2000: station [" +
				 ID.toString() + "] changed CDP Number from `" + 
				 QString().setNum(Station->CDP()) + "' to `" + 
				 QString().setNum(CDPNum) + "'");
		      Station->setCDP(CDPNum);
		      Station->setCharID("    ");
		    };
		}
	      else Station->setCharID(CharID);
	      Station->addAttr(SBStation::OrigImported);
	      Station->setClassCoord(ClassCoord);
	    };
	}
      else 
	Log->write(SBLog::DBG, SBLog::STATION, "SBTS_ssc2000: not an ssc entry: `" + Str + "'");
    };
  
  return s;
};

SBTS_blq &operator<<(SBTS_blq& s, const SB_TRF& TRF)
{
  s << "$$ Ocean loading displacements\n$$ CMIB: YES  (corr.tide mass imbalance)\n"
    << "$$\n$$ Abbreviations occuring in each record:\n$$ \"PTM\" = COMBINED SOLUTION:\n"
    << "$$       From CSR3: M2 S2 N2 K2 K1 O1 P1 Q1\n$$       From SCHW: MF MM SSA\n"
    << "$$\n$$ OCEAN MODELS:\n$$ \"SCHW\" = Schwiderski, 1979-1982\n"
    << "$$ \"CSR3\" = Austin Texas tide (Eanes, 1995) with LEPR as a starting solution.\n"
    << "$$ \"LEPR\" = \"PROV\" = \"Prov\" = LeProvost-Genco-Lyard-Vincent-Canceil (1994)\n"
    << "$$       Long-period tides not available.\n"
    << "$$       Interpolated partials like L2 have been ignored\n"
    << "$$       (they and many more can be restored by interpolating this table,\n"
    << "$$       using a decent harmonic development of the tide potential,\n"
    << "$$       like Tamura 1988).\n$$ \"MDL12\" = \"mdl12\" = Mediterranean tides, Scherneck,\n"
    << "$$       have been added where indicated (\"+mdl12/filename\").\n$$ METHODS:\n"
    << "$$ \"OLFG\"    - explicit convolution\n$$ \"OLFG_OT\" - ortho-tides\n"
    << "$$ \"OLMPP\"   - local refinement of OLFG\n"
    << "$$ \"OLM\"     - from maps obtained by fast convolution\n$$\n"
    << "$$ COLUMN ORDER:  M2  S2  N2  K2  K1  O1  P1  Q1  MF  MM SSA\n$$ ROW ORDER:\n"
    << "$$ AMPLITUDES (m)\n$$   RADIAL\n$$   TANGENTL    EW\n$$   TANGENTL    NS\n"
    << "$$ PHASES (deg)\n$$   RADIAL\n$$   TANGENTL    EW\n$$   TANGENTL    NS\n"
    << "$$\n$$ N.B.: Tangential displacements towards west / south\n$$ END HEADER\n";
  
  SBSiteIterator it(TRF);
  for (SBSite* W=it.toFirst(); it.current(); W=++it) 
    {
      if (W->oLoad().pickedUp()==SBOLoad::P_Domes)
	s << SBOLoadCarrier(*W);

      SBStationIterator it_st(W->stations());
      for (SBStation* Q=it_st.toFirst(); it_st.current(); Q=++it_st)
	if (Q->isAttr(SBStation::hasOwnOLoad))
	  s << SBOLoadCarrier(*Q);
    };

  s << "$$ END TABLE\n";
  return s;
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* class SBOLoadCarrier										*/
/*												*/
/*==============================================================================================*/
SBOLoadCarrier::SBOLoadCarrier() : SBNamed()
{
  OLoad.clear();
  Longitude = 0.0;
  Latitude  = 0.0;
  CharID = "";
  CDPNum = 9999;
  Id.setDomeMajor(99999);
  Id.setDomeMinor(0);
  Id.setPtType(SBStation::Pt_Antenna);
  isSuccessfulLoad = TRUE;
};

SBOLoadCarrier::SBOLoadCarrier(SBSite& Site) : SBNamed(Site)
{
  OLoad = Site.oLoad();
  Longitude = Site.longitude();
  Latitude  = Site.latitude();
  CharID = "";
  CDPNum = 9999;
  Id.setDomeMajor(Site.domeMajor());
  Id.setDomeMinor(0);
  Id.setPtType(SBStation::Pt_Antenna);
  isSuccessfulLoad = TRUE;
};

SBOLoadCarrier::SBOLoadCarrier(const SBStation& Station) : SBNamed(Station)
{
  if (Station.oLoad()) OLoad = *Station.oLoad();
  else OLoad.clear();
  Longitude = Station.longitude();
  Latitude  = Station.latitude();
  CharID = Station.charID();
  CDPNum = Station.CDP();
  Id = Station.id();
  isSuccessfulLoad = TRUE;
};

SBOLoadCarrier::SBOLoadCarrier(const SBOLoadCarrier& OLC)
  : SBNamed((const char*)OLC.Name)
{
  OLoad = OLC.OLoad;
  Longitude = OLC.Longitude;
  Latitude  = OLC.Latitude;
  CharID = (const char*)OLC.CharID;
  CDPNum = OLC.CDPNum;
  Id = OLC.Id;
  isSuccessfulLoad = OLC.isSuccessfulLoad;
};

SBOLoadCarrier& SBOLoadCarrier::operator= (const SBOLoadCarrier& OLC)
{
  SBNamed::operator=(SBNamed(OLC));
  OLoad = OLC.OLoad;
  Longitude = OLC.Longitude;
  Latitude  = OLC.Latitude;
  CharID = (const char*)OLC.CharID;
  CDPNum = OLC.CDPNum;
  Id = OLC.Id;
  isSuccessfulLoad = OLC.isSuccessfulLoad;
  return *this;
};

bool SBOLoadCarrier::operator==(const SBOLoadCarrier& OLC) const
{
  return  (Longitude == OLC.Longitude) && (Latitude  == OLC.Latitude) &&
    (Name == OLC.Name) && (CharID == OLC.CharID) && (CDPNum == OLC.CDPNum);
};

bool SBOLoadCarrier::operator!=(const SBOLoadCarrier& OLC) const
{
  return !operator==(OLC);
};

double SBOLoadCarrier::calcDistance(double lon, double lat) const
{
  return  6378136.0*sqrt(2.0*(1.0 - sin(Latitude)*sin(lat) - cos(Latitude)*cos(lat)*
			      (cos(Longitude)*cos(lon) + sin(Longitude)*sin(lon))
			      )
			 );
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOLoadCarrier's friends implementation						*/
/*												*/
/*==============================================================================================*/
SBTS_blq &operator>>(SBTS_blq& s, SBOLoadCarrier& OLC)
{
  QString	Str;
  QString	a;
  int		i;
  double	f;
  bool		isOK;
  OLC.isSuccessfulLoad = FALSE;

  //          1         2         3         4         5         6         7         8
  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
  //$$
  //  ALBUQUER  7884
  //$$ Complete CSR3 PTM
  //$$ Computed by OLFG, H.-G. Scherneck, Onsala Space Observatory 1997
  //$$ Albuquerque NM, SLR                  RADI TANG lon/lat:  253.5361   34.9642
  //  .00164 .00137 .00016 .00042 .00573 .00384 .00189 .00069 .00006 .00007 .00025
  //  .00085 .00019 .00018 .00005 .00232 .00151 .00076 .00028 .00006 .00004 .00030
  //  .00188 .00089 .00044 .00024 .00080 .00051 .00026 .00011 .00004 .00001 .00003
  //   137.5 -140.0 -176.4 -134.4   35.0   21.0   33.8   17.5 -155.0 -168.5  -34.7
  //   173.7  151.0  140.1  136.6 -136.5 -154.7 -137.9 -159.9  124.0 -163.4 -124.7
  //    92.6  108.8   78.3  108.6  152.2  148.5  151.5  152.7  -12.4   15.0 -118.8
  //$$
  //  RICHMOND 7219
  //$$ CSR3_PP ID: Nov   9, 1995 21:51 PTM
  //$$ Computed by H.G.Scherneck and H.J.P. Derks on gere.oso.chalmers.se, 1995
  //$$ 40499S001 RICHMOND, VLBI             RADI TANG lon/lat:  -80.3847   25.6138
  //  .00870 .00246 .00181 .00062 .00213 .00112 .00067 .00026 .00047 .00023 .00133
  //  .00384 .00082 .00090 .00020 .00078 .00077 .00027 .00016 .00002 .00002 .00018
  //  .00215 .00034 .00055 .00008 .00034 .00031 .00012 .00004 .00005 .00003 .00010
  //   163.2  179.8  150.6 -179.4    1.3   26.6    2.3   34.3  148.0  144.7 -144.6
  //   172.3  166.4  153.6  158.8   25.6   34.5   27.6   28.7    1.2 -140.6  -99.1
  //  -161.0 -101.4  171.6  -90.4   -3.3  -40.7   -7.5  -81.0  -94.5  144.1   80.2 
  //$$
  //  NYALES20  10317S003
  //  KOKEE  40424S007
  //$$ GOT99.2_R.Ray_CC_PP_PTME ID: Feb   3, 2000  8:49
  //$$ Computed by H.G.Scherneck on gere.oso.chalmers.se, 2000
  //$$ NY-ALESUND        VLBI 7331                    lon/lat:   11.8697   78.9291
  //  .00876 .00393 .00278 .00094 .00051 .00132 .00011 .00055 .00186 .00134 .00108
  //  .00259 .00084 .00070 .00022 .00058 .00052 .00019 .00013 .00018 .00005 .00005
  //  .00139 .00093 .00056 .00021 .00080 .00041 .00025 .00010 .00007 .00005 .00002
  //   174.9 -121.6  171.2 -113.7   50.3 -117.2   45.2 -162.4   31.2    9.3  180.0
  //   -16.7   50.5  -20.0   54.1  113.0   42.5  107.6   20.6 -176.4  172.9     .0
  //   -18.5   40.9  -31.6   42.4  135.0   63.8  131.9   13.6   -3.6  -21.6 -180.0
  //....
  //  10003M003 TOULOUSE


  Str="$$";
  while (Str.left(2)=="$$") 
    Str = s.readLine(); //ignore previous comments  

  if (Str.length()>2)
    OLC.Name = (const char*) Str.mid(2,8).simplifyWhiteSpace();
  else
    {
      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: not a station name: [" + Str + "]");
      return s;
    };

  if (Str.length()>11) //there is a CDP Number or Dome Major
    {
      a = Str.mid(2+OLC.Name.length(), Str.length()).simplifyWhiteSpace().copy();
      i = a.toInt(&isOK);
      if (!isOK)
	{
	  if (a.length()==4) 
	    OLC.CharID=(const char*)a;
	  else if (SBStationID::isValidStr(a.mid(0,9)))
	    OLC.Id.setID(a.mid(0,9));
	  else
	    Log->write(SBLog::WRN, SBLog::STATION, 
		       "SBTS_blq: not a CDP number, DOMES number or Char ID: ["  + a + "]");
	}
      else
	{
	  if (1000<=i && i<= 9999) OLC.CDPNum = i;
	  else if (10000<=i && i<= 99999) OLC.Id.setDomeMajor(i);
	  else 
	    {
	      a.sprintf("`%d'", i);
	      Log->write(SBLog::WRN, SBLog::STATION, 
			 "SBTS_blq: not a valid CDP or Dome number: " + a);
	    };
	};
    };
  // skip two comment lines
  Str = s.readLine();
  if (Str.left(2)!="$$")
    {
      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: wrong blq sequence: `" + Str + "'");
      return s;
    };
  OLC.OLoad.setCommentLine1(Str.mid(3, Str.length()-3));
  Str = s.readLine();
  if (Str.left(2)!="$$")
    {
      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: wrong blq sequence: `" + Str + "'");
      return s;
    };
  OLC.OLoad.setCommentLine2(Str.mid(3, Str.length()-3));
  Str = s.readLine();
  if (Str.left(2)!="$$")
    {
      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: wrong blq sequence: `" + Str + "'");
      return s;
    };

  //
  if ( SBStationID::isValidStr(Str.mid(3,9)) ) //it's a luck for us..
    {
      OLC.Id.setID(Str.mid(3,9));
      /*
	i = Str.find(",", 12);
	if (i==-1) i=39;
	OLC.SiteName = (const char*) Str.mid(12,i).simplifyWhiteSpace();
      */
    };
  
  if ((i=Str.find("lon/lat:"))==-1)
    {
      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: wrong \"lon/lat:\" signature: `"+Str+"'");
      return s;
    };
  
  a = Str.mid(i+8,10).stripWhiteSpace();
  f = a.toDouble(&isOK);
  if (!isOK)
    {
      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: cannot get longitude from `" + a + "'");
      return s;
    };
  if (f<0.0) f+=360.0;
  OLC.Longitude = f*DEG2RAD;

  a = Str.mid(i+19,10).stripWhiteSpace();
  f = a.toDouble(&isOK);
  if (!isOK)
    {
      Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: cannot get latitude from `" + a + "'");
      return s;
    };
  OLC.Latitude = f*DEG2RAD;
  
  //parsing amplitudes
  for (i=0; i<3; i++)
    {
      Str = s.readLine();
      if (Str.left(2)!="  ")
	{
	  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: wrong blq sequence: `" + Str + "'");
	  return s;
	};
      for (int j=0; j<11; j++)
	{
	  a = Str.mid(2 + 7*j, 6).stripWhiteSpace();
	  f = a.toDouble(&isOK);
	  if (!isOK)
	    {
	      Log->write(SBLog::WRN, SBLog::STATION, 
			 "SBTS_blq: cannot get amplitude from `" + a + "'");
	      return s;
	    };
	  OLC.OLoad.Amplitude[i][j] = f;
	};

    };
  
  //parsing phases
  for (i=0; i<3; i++)
    {
      Str = s.readLine();
      if (Str.left(2)!="  ")
	{
	  Log->write(SBLog::WRN, SBLog::STATION, "SBTS_blq: wrong blq sequence: `" + Str + "'");
	  return s;
	};
      for (int j=0; j<11; j++)
	{
	  a = Str.mid(2 + 7*j, 6).stripWhiteSpace();
	  f = a.toDouble(&isOK);
	  if (!isOK)
	    {
	      Log->write(SBLog::WRN, SBLog::STATION, 
			 "SBTS_blq: cannot get phase from `" + a + "'");
	      return s;
	    };
	  OLC.OLoad.Phase[i][j] = f;
	};

    };

  OLC.isSuccessfulLoad = TRUE;
  return s;
};

SBTS_blq &operator<<(SBTS_blq& s, const SBOLoadCarrier& OLC)
{
  QString Str;

  if (1000<OLC.CDPNum && OLC.CDPNum<9999) Str.sprintf("  %-8s %4d\n", 
						      OLC.Name.left(8).data(), OLC.CDPNum);
  else Str.sprintf("  %-8s %5d\n", OLC.Name.left(8).data(), OLC.Id.domeMajor());

  s << "$$\n" << (const char*) Str << "$$ " << (const char*) OLC.OLoad.commentLine1() << "\n"
    << "$$ "  << (const char*) OLC.OLoad.commentLine2() << "\n";
  
  s << (const char*) Str.sprintf("$$ %9s %-26s RADI TANG lon/lat: %9.4f  %8.4f\n", 
				 OLC.Id.toString().data(), OLC.Name.data(), 
				 (double)(RAD2DEG*OLC.Longitude), 
				 (double)(RAD2DEG*OLC.Latitude));
  for (int i=0; i<3; i++)
    {
      s << "  ";
      for (int j=0; j<11; j++)
	s << (const char*) Str.sprintf("%7.5f", OLC.OLoad.Amplitude[i][j]).mid(1,6) << " ";
      s << "\n";
    };

  for (int i=0; i<3; i++)
    {
      s << "  ";
      for (int j=0; j<11; j++)
	s << (const char*) Str.sprintf("%6.1f", OLC.OLoad.Phase[i][j]) << " ";
      s << "\n";
    };
  
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOLoadList's friends implementation							*/
/*												*/
/*==============================================================================================*/
SBTS_blq &operator>>(SBTS_blq& s, SBOLoadList& OLL)
{
  SBOLoadCarrier	*olc;
  while (!s.eof())
    {
      olc = new SBOLoadCarrier;
      s>>*olc;
      if (olc->IsSuccessfulLoad())
	{
	  OLL.inSort(olc);
	  Log->write(SBLog::DBG, SBLog::STATION, 
		     "SBTS_blq: OLoad `" + olc->name() + "' has been inserted");
	}
      else delete olc;
    };
  return s;
};
/*==============================================================================================*/

/*==============================================================================================*/

