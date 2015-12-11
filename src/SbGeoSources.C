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

#include "SbGeoSources.H"
#include <qmessagebox.h>
#include "SbSetup.H"
#include "SbGeoParameter.H"
#include "SbGeoProject.H"




/*==============================================================================================*/
/*												*/
/* class SBSource implementation								*/
/*												*/
/*==============================================================================================*/
SBSource::SBSource(const QString& Name_, double RA_, double DN_)
  : SBNamed(Name_), SBAttributed()
{
  RA		= RA_;
  dRA		= 0.1*HR2RAD/3600.0;
  DN		= DN_;
  dDN		= 0.1*DEG2RAD/3600.0;
  ICRF		= "ICRF J000000.0+000000";
  ObsName	= Name_;
  Status	= STATUS_UNKN;
  Type		= TYPE_UNKN;
  SpectrumClass	= SP_UNKN;
  IsHLink	= FALSE;
  RedShift	= 0.0;
  Flux1		= 0.0;
  Flux2		= 0.0;
  SpIdx		= 0.0;
  Mv		= 0.0;
  NObsApr	= 0;
  NSessApr	= 0;
  MeanMJD	= 0.0;
  FirstMJD	= 0.0;
  LastMJD	= 0.0;
  RA_DN		= 0.0;
  StructIdxX	= 0;
  StructIdxS	= 0;
  RAt		= RA;
  DNt		= DN;
  aList		= new QList<QString>;
  P_RA		= NULL;
  P_DN		= NULL;

  SINEXCharID	= "";
};

SBSource::SBSource(const SBSource& S)
  : SBNamed(S.Name), SBAttributed(S)
{
  RA		= S.RA;
  dRA		= S.dRA;
  DN		= S.DN;
  dDN		= S.dDN;
  ICRF		= S.ICRF;
  ObsName	= S.ObsName;
  Status	= S.Status;
  Type		= S.Type;
  SpectrumClass	= S.SpectrumClass;
  IsHLink	= S.IsHLink;
  RedShift	= S.RedShift;
  Flux1		= S.Flux1;
  Flux2		= S.Flux2;
  SpIdx		= S.SpIdx;
  Mv		= S.Mv;
  NObsApr	= S.NObsApr;
  NSessApr	= S.NSessApr;
  MeanMJD	= S.MeanMJD;
  FirstMJD	= S.FirstMJD;
  LastMJD	= S.LastMJD;
  RA_DN		= S.RA_DN;
  StructIdxX	= S.StructIdxX;
  StructIdxS	= S.StructIdxS;
  RAt		= RA;
  DNt		= DN;
  aList		= new QList<QString> (*S.aList);
  P_RA		= NULL;
  P_DN		= NULL;

  SINEXCharID	= S.SINEXCharID;
};

SBSource& SBSource::operator= (const SBSource& S)
{
  //  Name		= S.Name;
  SBAttributed::operator= (S);
  RA		= S.RA;
  dRA		= S.dRA;
  DN		= S.DN;
  dDN		= S.dDN;
  ICRF		= S.ICRF;
  ObsName	= S.ObsName;
  Status	= S.Status;
  Type		= S.Type;
  SpectrumClass	= S.SpectrumClass;
  IsHLink	= S.IsHLink;
  RedShift	= S.RedShift;
  Flux1		= S.Flux1;
  Flux2		= S.Flux2;
  SpIdx		= S.SpIdx;
  Mv		= S.Mv;
  NObsApr	= S.NObsApr;
  NSessApr	= S.NSessApr;
  MeanMJD	= S.MeanMJD;
  FirstMJD	= S.FirstMJD;
  LastMJD	= S.LastMJD;
  RA_DN		= S.RA_DN;
  StructIdxX	= S.StructIdxX;
  StructIdxS	= S.StructIdxS;
  RAt		= RA;
  DNt		= DN;
  if (aList) 
    delete aList;
  aList		= new QList<QString> (*S.aList);
  releaseParameters();
  P_RA		= NULL;
  P_DN		= NULL;

  SINEXCharID	= S.SINEXCharID;

  return *this;
};

bool SBSource::isValidICRFName(QString sICRF)
{
  bool		is = FALSE;
  int		v;

  //0123456789012345
  //JHHMMSS.s-DDMMSS
  if (sICRF.length()!=16) return FALSE;
  if (sICRF.mid(0,1)!="J") return FALSE;

  v = sICRF.mid(1, 2).toInt(&is);
  if (!is || (0>v) || (v>=24)) return FALSE;
  v = sICRF.mid(3, 2).toInt(&is);
  if (!is || (0>v) || (v>=60)) return FALSE;
  v = sICRF.mid(5, 2).toInt(&is);
  if (!is || (0>v) || (v>=60)) return FALSE;
  if (sICRF.mid(7,1)!=".") return FALSE;
  v = sICRF.mid(8, 1).toInt(&is);
  if (!is) return FALSE;
  if ((sICRF.mid(9,1)!="+")&&(sICRF.mid(9,1)!="-")) return FALSE;
  
  v = sICRF.mid(10, 2).toInt(&is);
  if (!is || (0>v) || (v>=90)) return FALSE;
  v = sICRF.mid(12, 2).toInt(&is);
  if (!is || (0>v) || (v>=60)) return FALSE;
  v = sICRF.mid(14, 2).toInt(&is);
  if (!is || (0>v) || (v>=60)) return FALSE;

  return TRUE;
};

void SBSource::createParameters()
{
  QString Prefix("So: " + name() + ": ");
  if (P_RA) 
    delete P_RA;
  P_RA = new SBParameter(Prefix + "RA");
  if (P_DN) 
    delete P_DN;
  P_DN = new SBParameter(Prefix + "DN");
};

void SBSource::releaseParameters()
{
  if (P_RA)
    {
      delete P_RA;
      P_RA = NULL;
    };
  if (P_DN)
    {
      delete P_DN;
      P_DN = NULL;
    };
};

void SBSource::updateParameters(SBRunManager* Mgr, const SBMJD& T, bool IsRealRun)
{
  RAt = RA;
  DNt = DN;
  if (Mgr)
    {
      Mgr->solution()->updateParameter(P_RA, T, IsRealRun);
      Mgr->solution()->updateParameter(P_DN, T, IsRealRun);
      RAt += P_RA->v();
      DNt += P_DN->v();
    };
};

void SBSource::getGlobalParameters4Report(SBRunManager* Mgr)
{
  RAt = RA;
  DNt = DN;
  if (Mgr)
    {
      Mgr->solution()->getGlobalParameter4Report(P_RA);
      Mgr->solution()->getGlobalParameter4Report(P_DN);
      RAt += P_RA->v();
      DNt += P_DN->v();
      dRA =  P_RA->e();
      dDN =  P_DN->e();
      NObsApr = P_DN->num();
      NSessApr= P_DN->numSes();
      MeanMJD  = P_DN->tEpoch();
      FirstMJD = P_DN->tStart();
      LastMJD  = P_DN->tFinis();
      RA_DN   = 0.0;
    };
};

void SBSource::getParameters4Report(SBRunManager* Mgr)
{
  if (Mgr)
    {
      updateParameters(Mgr, TZero, FALSE);
      dRA =  P_RA->e();
      dDN =  P_DN->e();
      NObsApr = P_DN->num();
      NSessApr= P_DN->numSes();
      MeanMJD  = P_DN->tEpoch();
      FirstMJD = P_DN->tStart();
      LastMJD  = P_DN->tFinis();
      RA_DN   = 0.0;
    };
};

QString SBSource::RA2str() const
{
  int		ra_h, ra_m;
  double	ra_s;
  QString	s;
  rad2hms(RAt, ra_h, ra_m, ra_s);
  return s.sprintf("%2d %2d %9.6f", ra_h, ra_m, ra_s);
};

QString SBSource::DN2str() const
{
  int		dn_d, dn_m;
  double	dn_s;
  QString	s;
  rad2dms(fabs(DNt), dn_d, dn_m, dn_s);
  return  s.sprintf("%c%2d %2d %8.5f", DNt<0.0?'-':' ', dn_d, dn_m, dn_s);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSource's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBSource& S)
{
  s << (const SBNamed&)S << (const SBAttributed&)S;
  return s  << S.RA << S.dRA << S.DN << S.dDN << S.ICRF << (uint)(S.Status)
	    << (uint)(S.Type) << (uint)(S.SpectrumClass) << (uint)(S.IsHLink) 
	    << S.RedShift << S.Flux1 << S.Flux2 << S.SpIdx << S.Mv << S.NObsApr 
	    << S.NSessApr << S.MeanMJD << S.FirstMJD << S.LastMJD << S.RA_DN 
	    << S.StructIdxX << S.StructIdxS;
};

QDataStream& operator>>(QDataStream& s, SBSource& S)
{
  s >> (SBNamed&)S >> (SBAttributed&)S;
  s  >> S.RA >> S.dRA >> S.DN >> S.dDN >> S.ICRF >> (uint&)(S.Status)
     >> (uint&)(S.Type) >> (uint&)(S.SpectrumClass) >> (uint&)(S.IsHLink) 
     >> S.RedShift >> S.Flux1 >> S.Flux2 >> S.SpIdx >> S.Mv >> S.NObsApr 
     >> S.NSessApr >> S.MeanMJD >> S.FirstMJD >> S.LastMJD >> S.RA_DN
     >> S.StructIdxX >> S.StructIdxS;
  S.RAt		= S.RA;
  S.DNt		= S.DN;
  S.SINEXCharID = "";
  return s;
};

SBTS_report& operator<<(SBTS_report& s, const SBSource& S)
{
  QString	Str;
  int		ra_h;
  int		ra_m;
  double	ra_s;
  int		dn_d;
  int		dn_m;
  double	dn_s;
  char		c_sig = ' ';

  rad2hms(S.ra_t(), ra_h, ra_m, ra_s);
  rad2dms(fabs(S.dn_t()), dn_d, dn_m, dn_s);
  if (S.dn_t()<0.0) c_sig='-';

  /* URL:
     ftp://hpiers.obspm.fr/iers/annreport/Call_2001AR/format_rsc
     I hav'n found any other descriptions of the format.

    Field           Contents  all fields separated by one blank (1)
    
     1                     IAU name
     2                     Alternate name
     3                     hours of right ascension (2)
     4                     minutes of right ascension (2)
     5                     seconds of right ascension (2)
     6                     degrees of declination (3)
     7                     minutes of declination (3)
     8                     seconds of declination (3)
     9                     uncertainty on right ascension (s)
    10                     uncertainty on declination (")
    11                     correlation : right ascension, declination
    12                     weighted mean observation date (MJD) (4)
    13                     date of first observation (MJD)
    14                     date of last observation (MJD)
    15                     number of sessions
    16                     number of delays
    17                     number of delay rates
    (5)
    
    
    Notes
    
    1 - All the 17 fields should be filled (with 0 when the information is
    lacking).
    
    2 - These three fields can be replaced by one field, giving the right
    ascensions in degrees and decimal fraction (from 0. to 360.)
    
    3 - These three fields can be replaced by one field, giving the declination
    in degrees and decimal fraction (from -90. to +90.).
    
    4 - The epoch of the catalog should be given in the general description of
    the analysis.
    
    5 - Any additional parameter(s) you feel appropriate to the description of
    the result.
  */


  s << " "  << (const char*)S.ICRF << "  " << (const char*)S.name() << "    ";
  
  Str.sprintf("%02d %02d %09.6f  %c%02d %02d %08.5f  %9.7f %8.6f %7.4f %7.1f %7.1f %7.1f %5d %6d %6d\n",
	      ra_h, ra_m, ra_s, 
	      c_sig, dn_d, dn_m, dn_s,
	      (double)(RAD2HR*3600.0*S.ra_err()),  (double)(RAD2DEG*3600.0*S.dn_err()),
	      S.corrRA_DN(),
	      (double)S.meanMJD(), (double)S.firstMJD(), (double)S.lastMJD(), 
	      S.nSessApr(), S.nObsApr(), 0
	      );
  s << (const char*)Str;
  return s;
};

SBTS_report4IVS& operator<<(SBTS_report4IVS& s, const SBSource& S)
{
  QString	Str;
  int		ra_h;
  int		ra_m;
  double	ra_s;
  int		dn_d;
  int		dn_m;
  double	dn_s;
  char		c_sig = ' ';

  rad2hms(S.ra_t(), ra_h, ra_m, ra_s);
  rad2dms(fabs(S.dn_t()), dn_d, dn_m, dn_s);
  if (S.dn_t()<0.0) c_sig='-';

  /* URL:
     http://rorf.usno.navy.mil/ICRF2/IVS/files/icrf2_f.i
     C
     C   FORMAT FOR ICRF-2 SOURCE POSITION CATALOGS
     C   AND TIME SERIES
     C
     c NEW
     6000   FORMAT (1X,A8,    ! IAU NAME
     &         1X,A8,         ! ALT NAME
     &         1X,2(I2.2,1X), ! RA HR MN
     &         F11.8,         ! RA SEC
     &         1X,A1,         ! DEC SIGN
     &         2(I2.2,1X),    ! DEC DG MN
     &         F10.7,1X,      ! DEC ARC SEC
     &         F10.8,1X,      ! RA ERROR SEC
     &         F9.7,1X,       ! DEC ERROR ARC SEC
     &         F7.4,          ! CCF
     &         3(1X,F7.1),    ! EPOCH MEAN FIRST LAST
     &         2X,I4,         ! # EXP
     &         2(1X,I6),      ! DELAY RATE OBS
     &         1X,A)          ! DATABASE NAME OR COMMENT
  */

  QString	Alias(S.name());
  if (S.aList && S.aList->count())
    Alias = *(S.aList)->first();
  
  Str.sprintf(" %8s %8s ", (const char*)Alias, (const char*)S.name() );
  s << (const char*)Str;

  //  Str.sprintf("%02d %02d %011.8f %c%02d %02d %010.7f %10.8f %9.7f %7.4f %7.1f %7.1f %7.1f %5d %6d %6d\n",
  Str.sprintf("%02d %02d %011.8f %c%02d %02d %010.7f %10.8f %9.7f %7.4f %7.1f %7.1f %7.1f %5d %6d %6d ",
	      ra_h, ra_m, ra_s, 
	      c_sig, dn_d, dn_m, dn_s,
	      (double)(RAD2HR*3600.0*S.ra_err()),  (double)(RAD2DEG*3600.0*S.dn_err()),
	      S.corrRA_DN(),
	      (double)S.meanMJD(), (double)S.firstMJD(), (double)S.lastMJD(), 
	      S.nSessApr(), S.nObsApr(), 0
	      );
  s << (const char*)Str;
  if (S.isAttr(SBSource::AnalysConstr))
    s << "C";
  else 
    s << " ";
  return s;
};

SBTS_rsc& operator<<(SBTS_rsc& s, const SBSource& S)
{
  QString	Str;
  QString	a;
  int		ra_h;
  int		ra_m;
  double	ra_s;
  int		dn_d;
  int		dn_m;
  double	dn_s;
  char		c_sig = ' ';

  rad2hms(S.ra_t(), ra_h, ra_m, ra_s);
  rad2dms(fabs(S.dn_t()), dn_d, dn_m, dn_s);
  if (S.dn_t()<0.0) c_sig='-';

  switch (S.Status)
    {
    case SBSource::STATUS_UNKN:
      a = "       ";
      break;
    case SBSource::STATUS_DEF:
      a = "D      ";
      break;
    case SBSource::STATUS_CAND:
      a = "C      ";
      break;
    case SBSource::STATUS_NEW:
      a = "N      ";
      break;
    case SBSource::STATUS_OTHER:
      a = "O      ";
      break;
    };
  if (S.isHLink()) a += "*";
  else a += " ";

  s << " "  << (const char*)S.ICRF << "   " << (const char*)S.name() << "  " << a << "  ";  

  if (S.corrRA_DN()!=0.0) a.sprintf("%6.3f", S.corrRA_DN());
  else a = "      ";

  Str.sprintf("%2d %2d %9.6f  %c%2d %2d %8.5f   %8.6f  %7.5f %s %7.1f %7.1f %7.1f %5d %6d\n",
	      ra_h, ra_m, ra_s, c_sig, dn_d, dn_m, dn_s, 
	      (double)(RAD2HR*3600.0*S.ra_err()),  (double)(RAD2DEG*3600.0*S.dn_err()),
	      (const char*)a,
	      (double)S.meanMJD(), (double)S.firstMJD(), (double)S.lastMJD(), 
	      S.nSessApr(), S.nObsApr()
	      );
  s << (const char*)Str;
  return s;
};

SBTS_rsc  &operator>>(SBTS_rsc& s, SBSource& S)
{
  QString	a;
  QString	sICRFName = "?";
  QString	sIERSName = "?";
  bool		isOK = FALSE;
  QString	Str = s.readLine();

  int		h;
  int		m;
  double	sec;
  int		signum;
  int		idx;

  S.ICRF = sICRFName;
  S.setName(sIERSName);
  
  /*
    I don't know how to parse that iers' files correctly; 
    alas, they usually invent a new format when make a new solution..
   */
  
  switch (Str.length())
    {
    case 0:
      Log->write(SBLog::DBG, SBLog::SOURCE, "SBTS_rsc: got an empty string");
      break;
    case 109: //COORDINATES OF THE OTHER SOURCES IN ICRF-Ext.1
      //          1         2         3         4         5         6         7         8         9        10
      //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678
      // ICRF J010838.7+013500   0106+013  2 1     1  8 38.771074    1 35  0.31714   0.000080  0.00213    1256  24538
      // ICRF J013741.2+330935   0134+329  4 4 *   1 37 41.299431   33  9 35.13299   0.000187  0.00663      19     64

      a = Str.mid(1, 4);
      if (a!="ICRF") break;
      a = Str.mid(6, 16);
      if (!SBSource::isValidICRFName(a)) break;
      sICRFName = "ICRF " + a;
      sIERSName = Str.mid(25, 8);
      
      a = Str.mid(35, 1);
      idx = a.toInt(&isOK);
      if (isOK && idx>-1)
	S.setStructIdxX(idx);
      a = Str.mid(37, 1);
      idx = a.toInt(&isOK);
      if (isOK && idx>-1)
	S.setStructIdxS(idx);

      if (Str.mid(39, 1)=="*") S.setIsHLink(TRUE);

      h = Str.mid(42, 2).toInt(&isOK);
      if (!isOK || (0>h) || (h>=24)) break;
      m = Str.mid(45, 2).toInt(&isOK);
      if (!isOK || (0>m) || (m>=60)) break;
      sec = Str.mid(48, 9).toDouble(&isOK);
      if (!isOK || (0.0>sec) || (sec>=60.0)) break;
      S.setRA(hms2rad(h,m,sec));

      signum = 1;
      if (Str.mid(59, 1)=="-") signum = -1;

      h = Str.mid(60, 2).toInt(&isOK); // in fact this is a degree
      if (!isOK || (0>h) || (h>=90)) break;
      m = Str.mid(63, 2).toInt(&isOK);
      if (!isOK || (0>m) || (m>=60)) break;
      sec = Str.mid(66, 8).toDouble(&isOK);
      if (!isOK || (0.0>sec) || (sec>=60.0)) break;
      S.setDN(signum*dms2rad(h,m,sec));

      sec = Str.mid(77, 8).toDouble(&isOK);
      if (!isOK || (0.0>sec)) break;
      S.setRA_err(hms2rad(0,0,sec));

      sec = Str.mid(86, 8).toDouble(&isOK);
      if (!isOK || (0.0>sec)) break;
      S.setDN_err(dms2rad(0,0,sec));

      h = Str.mid(95, 7).toInt(&isOK);
      if (isOK && (0<h)) S.setNSessApr(h);

      h = Str.mid(102, 7).toInt(&isOK);
      if (isOK && (0<h)) S.setNObsApr(h);

      if (s.assignedStatus() != SBSource::STATUS_UNKN)
	S.Status = s.assignedStatus();
      
      isOK = TRUE;
      break;
    case 141: // it was for the RSC(WGRF)95R01 and ICRF2
      if (Str.mid(0,1) == " ")
	{
	  //          1         2         3         4         5         6         7         8         9         10        11        12        13        14
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  // ICRF J113130.5-050019   1128-047  C         11 31 30.516713  - 5  0 19.65713   0.000031  0.00061  -.087 49408.6 49099.7 49547.8     4     48
	  a = Str.mid(1, 4);
	  if (a!="ICRF") break;
	  a = Str.mid(6, 16);
	  if (!SBSource::isValidICRFName(a)) break;
	  sICRFName = "ICRF " + a;
	  sIERSName = Str.mid(25, 8);
	  
	  a = Str.mid(35, 1);
	  if (a=="D") S.Status = SBSource::STATUS_DEF;
	  else if (a=="C") S.Status = SBSource::STATUS_CAND;
	  else if (a=="O") S.Status = SBSource::STATUS_OTHER;
	  else S.Status = SBSource::STATUS_UNKN;
	  
	  if (Str.mid(42, 1)=="*") S.setIsHLink(TRUE);
	  
	  h = Str.mid(45, 2).toInt(&isOK);
	  if (!isOK || (0>h) || (h>=24)) break;
	  m = Str.mid(48, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(51, 9).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setRA(hms2rad(h,m,sec));
	  
	  signum = 1;
	  if (Str.mid(62, 1)=="-") signum = -1;
	  
	  h = Str.mid(63, 2).toInt(&isOK); // in fact this is a degree
	  if (!isOK || (0>h) || (h>=90)) break;
	  m = Str.mid(66, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(69, 8).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setDN(signum*dms2rad(h,m,sec));

	  sec = Str.mid(80, 8).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setRA_err(hms2rad(0,0,sec));
      
	  sec = Str.mid(90, 7).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setDN_err(dms2rad(0,0,sec));
	  
	  sec = Str.mid(99, 5).toDouble(&isOK);
	  if (isOK && (sec!=0.0)) S.setCorrRA_DN(sec);
 
	  // alas...
	  h = Str.mid(105, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(111, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setMeanMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(113, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(119, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setFirstMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(121, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(127, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setLastMJD(SBMJD(h, 0.1L*(long double)m));
	    };

	  h = Str.mid(129, 5).toInt(&isOK);
	  if (isOK && (0<h)) S.setNSessApr(h);
	  
	  h = Str.mid(135, 6).toInt(&isOK);
	  if (isOK && (0<h)) S.setNObsApr(h);
	  
	  isOK = TRUE;
	}
      else if (Str.mid(0,1) == "I") // ICRF2 record
	{
	  //          1         2         3         4         5         6         7         8         9        10        11        12        13        14
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //ICRF J000435.6-473619  0002-478  D  00 04 35.65550384  -47 36 19.6037899  0.00001359 0.0002139   0.383  52501.0 49330.5 54670.7     28    129
	  a = Str.mid(0, 4);
	  if (a!="ICRF") break;
	  a = Str.mid(5, 16);
	  if (!SBSource::isValidICRFName(a)) break;
	  sICRFName = "ICRF " + a;
	  sIERSName = Str.mid(23, 8);
	  
	  a = Str.mid(33, 1);
	  if (a=="D") S.Status = SBSource::STATUS_DEF;
	  //	  else if (a=="C") S.Status = SBSource::STATUS_CAND;
	  //	  else if (a=="O") S.Status = SBSource::STATUS_OTHER;
	  //	  else S.Status = SBSource::STATUS_UNKN;
	  else S.Status = SBSource::STATUS_OTHER;
	  
	  //	  if (Str.mid(42, 1)=="*") S.setIsHLink(TRUE);
	  
	  h = Str.mid(36, 2).toInt(&isOK);
	  if (!isOK || (0>h) || (h>=24)) break;
	  m = Str.mid(39, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(42, 11).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setRA(hms2rad(h,m,sec));
	  
	  signum = 1;
	  if (Str.mid(55, 1)=="-") signum = -1;
	  
	  h = Str.mid(56, 2).toInt(&isOK); // in fact this is a degree
	  if (!isOK || (0>h) || (h>=90)) break;
	  m = Str.mid(59, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(62, 10).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setDN(signum*dms2rad(h,m,sec));

	  sec = Str.mid(74, 10).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setRA_err(hms2rad(0,0,sec));
      
	  sec = Str.mid(85, 9).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setDN_err(dms2rad(0,0,sec));
	  
	  sec = Str.mid(96, 6).toDouble(&isOK);
	  if (isOK && (sec!=0.0)) S.setCorrRA_DN(sec);
      
	  
	  // alas...
	  h = Str.mid(104, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(110, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setMeanMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(112, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(118, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setFirstMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(120, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(126, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setLastMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  
	  h = Str.mid(128, 6).toInt(&isOK);
	  if (isOK && (0<h)) S.setNSessApr(h);
	  
	  h = Str.mid(135, 6).toInt(&isOK);
	  if (isOK && (0<h)) S.setNObsApr(h);
	  
	  isOK = TRUE;
	};
      break;
    case 138:
    case 140:
      if (Str.mid(0,1) == " ") // ICRF.Ext1 records
	{
	  //---------------------------------------------------------------------------------------------------------------------------------------------
	  //  ICRF Designation      IERS Des.   Inf.  Right ascension    Declination        Uncertainty     Corr.  Mean    First   Last    Nb     Nb
	  //         (1)                (2)     (3)      J2000.0           J2000.0         R.A.     Dec.    Ra-Dc   MJD     MJD     MJD   sess.   del.
	  //                                   X S H    h  m  s           o  '  "         s         "              of observation span            (4) (5)
	  //---------------------------------------------------------------------------------------------------------------------------------------------
	  //          1         2         3         4         5         6         7         8         9        10        11        12        13        14
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  // ICRF J000435.6-473619   0002-478          0  4 35.655527  -47 36 19.60361   0.000045  0.00072   .185 49939.5 49330.5 50182.6     6     31
	  // ICRF J080839.6+495036   0804+499  2 1     8  8 39.666274   49 50 36.53046   0.000027  0.00026   .081 49582.5 47165.8 49924.8   155   9947 #
	  a = Str.mid(1, 4);
	  if (a!="ICRF") break;
	  a = Str.mid(6, 16);
	  if (!SBSource::isValidICRFName(a)) break;
	  sICRFName = "ICRF " + a;
	  sIERSName = Str.mid(25, 8);
	  
	  a = Str.mid(35, 1);
	  idx = a.toInt(&isOK);
	  if (isOK && idx>-1)
	    S.setStructIdxX(idx);
	  a = Str.mid(37, 1);
	  idx = a.toInt(&isOK);
	  if (isOK && idx>-1)
	    S.setStructIdxS(idx);
	  
	  if (Str.mid(39, 1)=="*") S.setIsHLink(TRUE);
	  
	  h = Str.mid(42, 2).toInt(&isOK);
	  if (!isOK || (0>h) || (h>=24)) break;
	  m = Str.mid(45, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(48, 9).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setRA(hms2rad(h,m,sec));
	  
	  signum = 1;
	  if (Str.mid(59, 1)=="-") signum = -1;
	  
	  h = Str.mid(60, 2).toInt(&isOK); // in fact this is a degree
	  if (!isOK || (0>h) || (h>=90)) break;
	  m = Str.mid(63, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(66, 8).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setDN(signum*dms2rad(h,m,sec));
	  
	  sec = Str.mid(77, 8).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setRA_err(hms2rad(0,0,sec));
	  
	  sec = Str.mid(87, 7).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setDN_err(dms2rad(0,0,sec));
	  
	  sec = Str.mid(95, 6).toDouble(&isOK);
	  if (isOK && (sec!=0.0)) S.setCorrRA_DN(sec);
	  
	  // alas...
	  h = Str.mid(102, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(108, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setMeanMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(110, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(116, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setFirstMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(118, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(124, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setLastMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  
	  h = Str.mid(125, 6).toInt(&isOK);
	  if (isOK && (0<h)) S.setNSessApr(h);
	  
	  h = Str.mid(131, 7).toInt(&isOK);
	  if (isOK && (0<h)) S.setNObsApr(h);
	  
	  if (s.assignedStatus() != SBSource::STATUS_UNKN)
	    S.Status = s.assignedStatus();
	  
	  if (Str.mid(139, 1)=="#")
	    S.addAttr(SBSource::StatusSuspected);
	  
	  isOK = TRUE;
	}
      else if (Str.mid(0,1) == "I") // ICRF2 record
	{
	  //          1         2         3         4         5         6         7         8         9        10        11        12        13        14
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //ICRF J000020.3-322101  2357-326  00 00 20.39994757  -32 21 01.2335157  0.00003337 0.0009246  -0.004  52306.7 52306.7 52306.7      1     40
	  a = Str.mid(0, 4);
	  if (a!="ICRF") break;
	  a = Str.mid(5, 16);
	  if (!SBSource::isValidICRFName(a)) break;
	  sICRFName = "ICRF " + a;
	  sIERSName = Str.mid(23, 8);
	  
	  // set status of VCS sources to "NEW":
	  S.Status = SBSource::STATUS_NEW;
	  
	  h = Str.mid(33, 2).toInt(&isOK);
	  if (!isOK || (0>h) || (h>=24)) break;
	  m = Str.mid(36, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(39, 11).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setRA(hms2rad(h,m,sec));
	  
	  signum = 1;
	  if (Str.mid(52, 1)=="-") signum = -1;
	  
	  h = Str.mid(53, 2).toInt(&isOK); // in fact this is a degree
	  if (!isOK || (0>h) || (h>=90)) break;
	  m = Str.mid(56, 2).toInt(&isOK);
	  if (!isOK || (0>m) || (m>=60)) break;
	  sec = Str.mid(59, 10).toDouble(&isOK);
	  if (!isOK || (0.0>sec) || (sec>=60.0)) break;
	  S.setDN(signum*dms2rad(h,m,sec));

	  sec = Str.mid(71, 10).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setRA_err(hms2rad(0,0,sec));
      
	  sec = Str.mid(82, 9).toDouble(&isOK);
	  if (!isOK || (0.0>sec)) break;
	  S.setDN_err(dms2rad(0,0,sec));
	  
	  sec = Str.mid(93, 6).toDouble(&isOK);
	  if (isOK && (sec!=0.0)) S.setCorrRA_DN(sec);
      	  
	  // alas...
	  h = Str.mid(101, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(107, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setMeanMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(109, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(115, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setFirstMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  h = Str.mid(117, 5).toInt(&isOK);
	  if (isOK)
	    {
	      m = Str.mid(123, 1).toInt(&isOK);
	      if (isOK && (0<=h)) S.setLastMJD(SBMJD(h, 0.1L*(long double)m));
	    };
	  
	  h = Str.mid(125, 6).toInt(&isOK);
	  if (isOK && (0<h)) S.setNSessApr(h);
	  
	  h = Str.mid(132, 6).toInt(&isOK);
	  if (isOK && (0<h)) S.setNObsApr(h);
	  
	  isOK = TRUE;
	};
      break;
    default:
      a.setNum(Str.length());
      Log->write(SBLog::DBG, SBLog::SOURCE, "SBTS_rsc: got unknown format, ignored; strlen=" + a + 
		 ", text: [" + Str + "]");
    };
  
  if (isOK)
    {
      S.ICRF = sICRFName;
      S.setName(sIERSName);
      S.addAttr(SBSource::OrigImported);
    }
  else if (sIERSName!="?")
    Log->write(SBLog::INF, SBLog::SOURCE, "SBTS_rsc: got a error parsing [" + sIERSName + "]");
    
  return s;
};

SBTS_car  &operator<<(SBTS_car& s, const SBSource& S)
{
  QString	Str;
  QString	a;

  switch (S.Type)
    {
    case SBSource::TYPE_UNKN:
      a = "  ";
      break;
    case SBSource::TYPE_QUASAR:
      a = " Q";
      break;
    case SBSource::TYPE_GALAXY:
      a = " G";
      break;
    case SBSource::TYPE_BL_LAC:
      a = " L";
      break;
    case SBSource::TYPE_BL_LAC_C:
      a = "?L";
      break;
    case SBSource::TYPE_OTHER:
      a = " A";
      break;
    };

  s << " "  << (const char*)(S.ICRF) << "    " << (const char*)(S.name()) 
    << "   " << (const char*)a << "   ";

  if (S.redShift()!=0.0) a.sprintf("%6.3f", S.redShift());
  else a = "      ";
  s << (const char*)a << "  ";

  if (S.flux1()>0.0) a.sprintf("%5.2f", S.flux1());
  else a = "     ";
  s << (const char*)a << "  ";

  if (S.flux2()>0.0) a.sprintf("%5.2f", S.flux2());
  else a = "     ";
  s << (const char*)a << "   ";

  if (S.spIdx()!=0.0) a.sprintf("%4.1f", S.spIdx());
  else a = "    ";
  s << (const char*)a << "  ";

  if (S.mv()>0.0) a.sprintf("%5.1f", S.mv());
  else a = "     ";
  s << (const char*)a << "  ";

  switch (S.SpectrumClass)
    {
    case SBSource::SP_UNKN:
    case SBSource::SP_OTHER:
      a = "  ";
      break;
    case SBSource::SP_SEYFERT:
      a = "S ";
      break;
    case SBSource::SP_SEYFERT1:
      a = "S1";
      break;
    case SBSource::SP_SEYFERT1_2:
      a = "S1";
      break;
    case SBSource::SP_SEYFERT2:
      a = "S2";
      break;
    case SBSource::SP_SEYFERT3:
      a = "S3";
      break;
    case SBSource::SP_HP:
      a = "HP";
      break;
    };
  s << (const char*)a << "\n";
  return s;
};

SBTS_car  &operator>>(SBTS_car& s, SBSource& S)
{
  QString	a;
  QString	sICRFName = "?";
  QString	sIERSName = "?";
  bool		isOK = FALSE;
  QString	Str = s.readLine();

  float		f_v;

  S.ICRF = sICRFName;
  S.setName(sIERSName);

  switch (Str.length())
    {
    case 0:
      Log->write(SBLog::DBG, SBLog::SOURCE, "SBTS_car: got an empty string");
      break;
    case 80:
      //          1         2         3         4         5         6         7         8
      //012345678901234567890123456789012345678901234567890123456789012345678901234567890
      // ICRF J001033.9+172418    0007+171    Q    1.601   1.19    .91*    .4   18.0    
      // ICRF J012031.6-270124    0118-272    L   > .557    .78   1.00     .3   15.6  HP

      a = Str.mid(1, 4);
      if (a!="ICRF") break;
      a = Str.mid(6, 16);
      if (!SBSource::isValidICRFName(a)) break;
      sICRFName = "ICRF " + a;
      sIERSName = Str.mid(26, 8);
      
      a = Str.mid(38, 1);
      if (a=="Q") S.Type = SBSource::TYPE_QUASAR;
      else if (a=="G") S.Type = SBSource::TYPE_GALAXY;
      else if (a=="L") 
	{
	  if (Str.mid(37, 1)=="?") S.Type = SBSource::TYPE_BL_LAC_C;
	  else S.Type = SBSource::TYPE_BL_LAC;
	}
      else if (a=="A") S.Type = SBSource::TYPE_OTHER;
      else S.Type = SBSource::TYPE_UNKN;

      f_v = Str.mid(43, 5).toFloat(&isOK);
      if (isOK) S.setRedShift(f_v);

      f_v = Str.mid(50, 5).toFloat(&isOK);
      if (isOK) S.setFlux1(f_v);

      f_v = Str.mid(57, 5).toFloat(&isOK);
      if (isOK) S.setFlux2(f_v);

      f_v = Str.mid(64, 5).toFloat(&isOK);
      if (isOK) S.setSpIdx(f_v);

      f_v = Str.mid(71, 5).toFloat(&isOK);
      if (isOK) S.setMv(f_v);

      a = Str.mid(78, 2);
      if (a == "  ") S.setSpectrum(SBSource::SP_OTHER);
      else if (a == "S ") S.setSpectrum(SBSource::SP_SEYFERT);
      else if (a == "S1") S.setSpectrum(SBSource::SP_SEYFERT1);
      else if (a == "S2") S.setSpectrum(SBSource::SP_SEYFERT2);
      else if (a == "S3") S.setSpectrum(SBSource::SP_SEYFERT3);
      else if (a == "HP") S.setSpectrum(SBSource::SP_HP);
      else S.setSpectrum(SBSource::SP_UNKN);
      
      isOK = TRUE;
      break;

    case 82: // icrf-Ext1.car format
      //          1         2         3         4         5         6         7         8
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789012
      // ICRF J001033.9+172418    0007+171    Q    1.601   1.19    .91*    .4   18.0    
      // ICRF J012031.6-270124    0118-272    L   > .557    .78   1.00     .3   15.6  HP

      // ICRF J001031.0+105829    0007+106    G     .089    .43    .23*         15.4  S1.2
      // ICRF J005905.5+000651    0056-001    G     .717   1.41   2.20    -.4   17.3  S1.5
 
      a = Str.mid(1, 4);
      if (a!="ICRF") break;
      a = Str.mid(6, 16);
      if (!SBSource::isValidICRFName(a)) break;
      sICRFName = "ICRF " + a;
      sIERSName = Str.mid(26, 8);
      
      a = Str.mid(38, 1);
      if (a=="Q") S.Type = SBSource::TYPE_QUASAR;
      else if (a=="G") S.Type = SBSource::TYPE_GALAXY;
      else if (a=="L") 
	{
	  if (Str.mid(37, 1)=="?") S.Type = SBSource::TYPE_BL_LAC_C;
	  else S.Type = SBSource::TYPE_BL_LAC;
	}
      else if (a=="A") S.Type = SBSource::TYPE_OTHER;
      else S.Type = SBSource::TYPE_UNKN;

      f_v = Str.mid(43, 5).toFloat(&isOK);
      if (isOK) S.setRedShift(f_v);

      f_v = Str.mid(50, 5).toFloat(&isOK);
      if (isOK) S.setFlux1(f_v);

      f_v = Str.mid(57, 5).toFloat(&isOK);
      if (isOK) S.setFlux2(f_v);

      f_v = Str.mid(64, 5).toFloat(&isOK);
      if (isOK) S.setSpIdx(f_v);

      f_v = Str.mid(71, 5).toFloat(&isOK);
      if (isOK) S.setMv(f_v);

      a = Str.mid(78, 4);
      if (a == "  ") S.setSpectrum(SBSource::SP_OTHER);
      else if (a == "S   ") S.setSpectrum(SBSource::SP_SEYFERT);
      else if (a == "S1  ") S.setSpectrum(SBSource::SP_SEYFERT1);
      else if (a == "S2  ") S.setSpectrum(SBSource::SP_SEYFERT2);
      else if (a == "S3  ") S.setSpectrum(SBSource::SP_SEYFERT3);
      else if (a == "HP  ") S.setSpectrum(SBSource::SP_HP);
      else if (a.mid(0,3)=="S1.") S.setSpectrum(SBSource::SP_SEYFERT1_2);
      else S.setSpectrum(SBSource::SP_UNKN);
      
      isOK = TRUE;
      break;
    default:
      a.setNum(Str.length());
      Log->write(SBLog::DBG, SBLog::SOURCE, "SBTS_car: got unknown format, ignored; strlen=" + a);
      Log->write(SBLog::DBG, SBLog::SOURCE, "SBTS_car: string: [" + Str + "]");
    };
  
  if (isOK)
    {
      S.ICRF = sICRFName;
      S.setName(sIERSName);
    }
  else if (sIERSName!="?")
    Log->write(SBLog::INF, SBLog::SOURCE, "SBTS_car: got a error parsing [" + sIERSName + "]");
    
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SB_CRF implementation									*/
/*												*/
/*==============================================================================================*/
//SB_CRF::SB_CRF() : SBCatalog() {Label.setType(SBSolutionName::TYPE_RSC);};
void SB_CRF::prepareAliases4Output()
{
  if (!count()) return;
  SBSource		*source;
  SBSourceIterator	it_s(*this);
  for (source=it_s.toFirst(); it_s.current(); source=++it_s)
    {
      if (source->aList) delete source->aList;
      source->aList = new QList<QString>;
      source->aList->setAutoDelete(TRUE);
    };
  
  QString		*Str;
  QDictIterator<QString> it_a(*Aliases);
  while ((Str=it_a.current()))
    {
      if((source = find(*Str)))
	source->aList->append(new QString(it_a.currentKey()));
      else
	Log->write(SBLog::WRN, SBLog::SOURCE, 
		   ClassName() + ": alias `" + *Str + "' not found in database..");
      ++it_a;
    };
  
};

void SB_CRF::releaseAliases4Output()
{
  if (!count()) return;
  SBSource		*source;
  SBSourceIterator	it(*this);
  for (source=it.toFirst(); it.current(); source=++it)
    {
      if (source->aList) delete source->aList;
      source->aList = NULL;
    };
};

void SB_CRF::collectObjAliases(QString S)
{
  if (!count()) return;
  SBSource *source = find(S);
  if (source)
    {
      if (source->aList) delete source->aList;
      source->aList = new QList<QString>;
      source->aList->setAutoDelete(TRUE);
      QString *Str;
      QDictIterator<QString> it(*Aliases);
      while ((Str=it.current()))
	{
	  if(*Str==source->name()) source->aList->append(new QString(it.currentKey()));
	  ++it;
	};
    };
};

void SB_CRF::releaseObjAliases(QString S)
{
  if (!count()) return;
  SBSource *source = find(S);
  if (source) source->clearAliasesList();
};

SBSource* SB_CRF::lookupAlias(const QString& Alias)
{
  SBSource*	Source=NULL;
  QString*	str= Aliases->find(Alias);
  if (str) Source=find(*str);
  return Source;
};

SBSource* SB_CRF::lookupNearest(double ra, double dn, double& MinR)
{
  SBSource*	Source=NULL;
  SBSource*	W;
  double	r;
  double	dr = 1.0e-7/3600.0/RAD2DEG; // 0.1 ms
  double	dx,dy;
  
  QString	tmp;
  QString	tmp2;
  MinR=M_PI;
  for (W=first(); W; W=next())
  {
    // exact match
    dx = ra - W->ra();
    dy = dn - W->dn();
    if (fabs(dx)<=dr && fabs(dy)<=dr)
      {
	MinR  = sqrt(dx*dx + dy*dy);
	Source= W;
      }
    else if (MinR> (r=2.0*asin(sqrt(2.0*(1.0 
			- sin(dn)*sin(W->dn()) 
			- cos(dn)*cos(W->dn())*
				(cos(ra)*cos(W->ra()) + sin(ra)*sin(W->ra()))))/2.0) )   )
      {
	MinR  = r;
	Source= W;
      };
  };
  if (Source)
    Log->write(SBLog::DBG, SBLog::SOURCE, ClassName() + ": found [" + Source->name()
	       + "] as nearest source; the distance is " + 
	       tmp.setNum((double)(3600.0*RAD2DEG*MinR), 'g', 8) + " arcsec");
  return Source;
};

void SB_CRF::prepare4Run()
{
  for (SBSource* W=first(); W; W=next())
    W->createParameters();
};

void SB_CRF::finisRun()
{
  for (SBSource* W=first(); W; W=next())
    W->releaseParameters();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SB_CRF's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SB_CRF& CRF)
{
  s << (SBCatalog&)CRF;
  return s;
};

QDataStream &operator>>(QDataStream& s, SB_CRF& CRF)
{
  s >> (SBCatalog&)CRF;
  return s;
};

SBTS_dico  &operator<<(SBTS_dico& s, SB_CRF& CRF)
{
  s << "\n			DICTIONARY OF ALIASES \n"
    << "-------------------------------------------------------------------------\n"
    << "ICRF Designation       IERS Des. GSFC/USNO   Other names\n"
    << "          (1)            (2)       (3)          (4,5)\n"
    << "-------------------------------------------------------------------------\n";

  CRF.prepareAliases4Output();  
  if (CRF.count())
    {
      SBSource		*source;
      SBSourceIterator	it(CRF);
      while ((source=it.current()))
	{
	  s << source->ICRF << "  " << source->name();
	  QString* w;
	  if (source->aList->count())
	    for (w=source->aList->first(); w; w=source->aList->next()) s << "  " << *w;
	  s << "\n";
	  ++it;
	};
    };
  CRF.releaseAliases4Output();

  return s;
};

SBTS_dico &operator>>(SBTS_dico& s, SB_CRF& CRF)
{
  QString	a;
  QString	sICRFName = "?";
  QString	sIERSName = "?";

  while (!s.eof())
    {
      s >> a;
      if(a=="ICRF")
	{
	  s >> a;
	  if (SBSource::isValidICRFName(a))
	    {
	      s >> sIERSName;
	      sICRFName = s.readLine();
	      if ((sICRFName.length()>11) && 
		  ( (a = sICRFName.mid(0,11).stripWhiteSpace().leftJustify(8, ' ', TRUE) ) 
		    != "        ") )
		{
		  //a = a.leftJustify(8);
		  if (!(CRF.Aliases->find(a)))
		    {
		      (CRF.Aliases)->insert((const char*)a, new QString((const char*)sIERSName));
		      if (!(CRF.find(sIERSName)))
			  Log->write(SBLog::WRN, SBLog::SOURCE, "SBTS_dico: source " + sIERSName 
				     + " aka " + a + " not found in database; alias inserted.");
		    }
		  else if ( *(CRF.Aliases->find(a)) != sIERSName)
		    Log->write(SBLog::ERR, SBLog::SOURCE, "SBTS_dico: alias `" + a + 
			       "' already defined as `" + *(CRF.Aliases->find(a)) + 
			       "'; the entry `" + sIERSName + "' ignored..");
		}
	    }
	  else
	    Log->write(SBLog::DBG, SBLog::SOURCE, "SBTS_dico: `" + a + "' not an ICRF name..");
	}
      else
	Log->write(SBLog::DBG, SBLog::SOURCE, "SBTS_dico: `" + a + 
		   "' not an dico entry at all..");
    };
  
  return s;
};

SBTS_report &operator<<(SBTS_report& s, const SB_CRF& CRF)
{
  s << "\n " << CRF.Label;
  s << ". Coordinates of the " << CRF.count() << " sources in ICRF\n"
    << " ----------------------------------------------------------------------" 
    << "----------------------------------------------------------------------\n"
    << "  ICRF Designation      IERS Des.   Right ascension    Declination      "
    << "Uncertainty       Corr.   Mean   First   Last      Nb     Nb    Nb\n"
    << "                                       J2000.0           J2000.0        R.A."
    << "     Dec.     Ra-Dc   MJD     MJD     MJD    sess.   del.   rat.\n"
    << "                                      h  m  s           o  '  \"        s"
    << "         \"                 of observation span\n"
    << " ----------------------------------------------------------------------" 
    << "----------------------------------------------------------------------\n";
  
  SBSourceIterator it(CRF);
  for (SBSource* W=it.toFirst(); it.current(); W=++it) s << *W;

  s << " ----------------------------------------------------------------------" 
    << "----------------------------------------------------------------------\n";

  return s;
};

SBTS_report4IVS &operator<<(SBTS_report4IVS& s, const SB_CRF& CRF)
{
  s << "# Coordinates of the " << CRF.count() << " sources in ICRF\n"
    << "#------------------------------------------------------------------------"
    << "------------------------------------------------------\n#_______NAME______"
    << " _______RA________ _______DEC_______ ________ERROR_______         ________EPOCH"
    << "_________    _#_  ____OBS_____\n#   IAU     B1950  HR MN    SEC       DG MN   ARC"
    << " SEC    RA SEC  DEC ARCSEC    CCF  MEAN    FIRST   LAST      EXP  DELAY   RATE\n"
    << "#--------------------------------------------------------------------------------"
    << "----------------------------------------------\n";
  SBSourceIterator it(CRF);
  for (SBSource* W=it.toFirst(); it.current(); W=++it) 
    if (W->isAttr(SBSource::Analysed))
      s << *W << "\n";
  
  s << "# EOF \n";
  
  return s;
};

SBTS_rsc &operator<<(SBTS_rsc& s, const SB_CRF& CRF)
{

  s << "\nNotes\n\n (1)    ICRF Designations, constructed from J2000.0 coordinates with the format"
    << "\n                ICRF JHHMMSS.s+DDMMSS   or   ICRF JHHMMSS.s-DDMMSS \n"
    << "        They follow the recommendations of the IAU Task Group on Designations.\n\n"
    << " (2)    IERS Designations, previously constructed from B1950 coordinates. \n"
    << "        The complete format, including acronym and epoch in addition to the\n"
    << "        coordinates, is\n"
    << "		 IERS BHHMM+DDd   or   IERS BHHMM-DDd\n\n"
    << " (3)	c: Category of the source: [D]efining, [C]andidate, [O]ther\n"
    << "        X: Structure index at X band\n"
    << "        S: Structure index at S band\n"
    << "        H: Asterisk indicates that the source serves to link the Hipparcos\n"
    << "             stellar reference frame to the ICRS.\n\n"
    << " (4)    Number of pairs of delay and delay rate observations.\n\n\n ";
  
  s << CRF.Label;
  s << ". Coordinates of all the " << CRF.count() << " sources in ICRF"
    << "\n ----------------------------------------------------------------------"
    << "-----------------------------------------------------------------------\n"
    << "  ICRF Designation       IERS Des.   Inf.     Right Ascension   Declination"
    << "        Uncertainty     Corr.   Mean   First    Last    Nb     Nb\n"
    << "         (1)                (2)      (3)         J2000.0           J2000.0"
    << "        R.A.      Dec.   RA-Dc   MJD     MJD     MJD    sess.  del.\n"
    << "                                   c  X S H    h  m  s           o  '  \""
    << "         s         \"               of observation span           (4)\n"
    << " ----------------------------------------------------------------------"
    << "-----------------------------------------------------------------------\n";
  
  SBSourceIterator it(CRF);
  for (SBSource* W=it.toFirst(); it.current(); W=++it) s << *W;
  return s;
};

SBTS_rsc &operator>>(SBTS_rsc& s, SB_CRF& CRF)
{
  SBSource	*source=NULL;
  SBSource	*W=NULL;
  while (!s.eof())
    {
      source = new SBSource();
      s>>*source;
      if (source->name()!="?") 
	{
	  if (!(W=CRF.find(source->name())))
	    {
	      CRF.inSort(source);
	      Log->write(SBLog::DBG, SBLog::SOURCE, 
			 "SBTS_rsc: source `" + source->name() + "' has been inserted");
	    }
	  else
	    {
	      if (W && source && W->ICRFName()!=source->ICRFName())
		Log->write(SBLog::ERR, SBLog::SOURCE, 
			   "SBTS_rsc: source `" + source->name() + "' has ICRF designation `" +
			   source->ICRFName() + "' that differ from database `" + W->ICRFName() + 
			   "' one; an entry ignored..");
	      else
		{
		  W->  setRA	   (source->ra());
		  W->  setDN       (source->dn());
		  W->  setRA_err   (source->ra_err());
		  W->  setDN_err   (source->dn_err());
		  W->  setStatus   (source->status());
		  W->  setNObsApr  (source->nObsApr());
		  W->  setNSessApr (source->nSessApr());
		  W->  setMeanMJD  (source->meanMJD());
		  W->  setFirstMJD (source->firstMJD());
		  W->  setLastMJD  (source->lastMJD());
		  W->  setCorrRA_DN(source->corrRA_DN());
		  W->  setIsHLink  (source->isHLink());
		  W->  addAttr     (SBSource::OrigImported);

		  Log->write(SBLog::DBG, SBLog::SOURCE, 
			     "SBTS_rsc: source `" + source->name() + "' has been updated");
		};
	      delete source;
	    };
	}
      else delete source;
    };  
  return s;
};

SBTS_car    &operator<<(SBTS_car& s, const SB_CRF& CRF)
{
  s << "		  PHYSICAL CHARACTERISTICS OF RADIO SOURCES\n\nNotes:\n"
    << "        (1)  Type of Object:\n"
    << "                 Q = quasar,	G = galaxy,	L = BL Lac,\n"
    << "                ?L = BL Lac candidate,		A = other\n"
    << "        (2)  Asterisk indicates that the reported value is the flux at 11 cm. \n"
    << "        (3)  Redshift (unitless), \">\" prefix means the value given is a lower\n"
    << "                limit.\n"
    << "        (4)  Spectral index from Preston et al.\n"
    << "        (5)  Classification of Spectrum:\n"
    << "   	         S = Seyfert spectrum, 		S1 = Seyfert 1 spectrum,\n"
    << "              	S2 = Seyfert 2 spectrum, 	S3 = Seyfert 3 or liner,\n"
    << "          	HP = high optical polarization (>3%)\n\n\n"
    << "---------------------------------------------------------------------------------\n"
    << "ICRF Designation          IERS Des.  Tp    Redsh.   Flux (Jy)      SP    mv   cl.\n"
    << "                                                   6cm    15cm                sp\n"
    << "                                     (1)    (2)           (3)     (4)         (5)\n"
    << "---------------------------------------------------------------------------------\n";
  SBSourceIterator it(CRF);
  for (SBSource* W=it.toFirst(); it.current(); W=++it) s << *W;
  s <<"---------------------------------------------------------------------------------\n";
  return s;
};

SBTS_car &operator>>(SBTS_car& s, SB_CRF& CRF)
{
  SBSource	*source=NULL;
  SBSource	*W=NULL;
  while (!s.eof())
    {
      source = new SBSource();
      s>>*source;
      if (source->name()!="?") 
	{
	  if (!(W=CRF.find(source->name())))
	    {
	      CRF.inSort(source); // why we have to keep these info?
	      //	      Log->write(SBLog::ERR, SBLog::SOURCE, 
	      //			 "SBTS_car: source `" + source->name() + 
	      //			 "' not found in database; the entry is ignored..");
	    }
	  else
	    {
	      if (W->ICRFName()!=source->ICRFName())
		Log->write(SBLog::ERR, SBLog::SOURCE, 
			   "SBTS_car: source `" + source->name() + "' has ICRF designation `" +
			   source->ICRFName() + "' that differ from database `" + W->ICRFName() + 
			   "' one; the entry is ignored..");
	      else
		{
		  W->  setType	   (source->type());
		  W->  setRedShift (source->redShift());
		  W->  setFlux1    (source->flux1());
		  W->  setFlux2    (source->flux2());
		  W->  setSpIdx    (source->spIdx());
		  W->  setMv       (source->mv());
		  W->  setSpectrum (source->spectrum());
		  Log->write(SBLog::DBG, SBLog::SOURCE, 
			     "SBTS_car: source `" + source->name() + "' has been updated");
		};
	      delete source;
	    };
	}
      else delete source;
    };  
  return s;
};
/*==============================================================================================*/
