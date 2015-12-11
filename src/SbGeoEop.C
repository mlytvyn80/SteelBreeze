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

#include "SbGeoEop.H"

#include <qdir.h>
#include <qfile.h>
#include <qmessagebox.h>

#include "SbSetup.H"
#include "SbGeoParameter.H"
#include "SbGeoProject.H"
#include "SbMathSpline.H"
#include "SbVector.H"

#include "SbGeoRefFrame.H"




/*==============================================================================================*/
/*												*/
/* class SBEOPEntry implementation								*/
/*												*/
/*==============================================================================================*/
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEOPEntry frends implementation							*/
/*												*/
/*==============================================================================================*/
QTextStream &operator<<(QTextStream& s, const SBEOPEntry& eop)
{
  QString	str;
  /*
    0        1         2         3         4         5         6         7         8         9
    123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
      JAN   1  51544  .043251  .377681  .3555151    .0009068    -.05082  -.00225
      FEB  27  51601  .066985  .363501  .3053352    .0007036    -.05015  -.00508
  */
  str.sprintf("%2d  %5d %8.6f %8.6f %9.7f   %9.7f   %8.5f% 8.5f", 
	      eop.day(), eop.date(), 
	      eop.Xp, eop.Yp, eop.UT, eop.LOD, eop.Psi, eop.Eps);
  return s << "  " << eop.month2Str().upper() << "  " << str << "\n";
};

QTextStream &operator>>(QTextStream& s, SBEOPEntry& eop)
{
  QString	str;
  int		i;
  s >> str >> i;
  if (!i)
    {
      eop = 0.0;
      s.readLine();
      return s;
    }
  else
    {
      s >> i;
      eop = (double)i;
      if (eop.date()<35000.0) eop=0.0;
      s >> eop.Xp >> eop.Yp >> eop.UT >> eop.LOD >> eop.Psi >> eop.Eps;
      return s;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEOP implementation									*/
/*												*/
/*==============================================================================================*/
SBEOP::SBEOP()
  : QList<SBEOPEntry>()
{
  setAutoDelete(TRUE);

  Label.setAttr(SBSolutionName::TYPE_EOP, "UNKN", 1980, TECH_UNKN);
  Epoch = TZero;
  TStart= TZero;
  TFinis= TZero;
  
  Xp	= 0.0;
  Xps	= 0.0;
  Yp	= 0.0;
  Yps	= 0.0;
  UT	= 0.0;
  UTs	= 0.0;
  Psi	= 0.0;
  Psis	= 0.0;
  Eps	= 0.0;
  Epss	= 0.0;
  UTHasHiTide = FALSE;
  UTHasLoTide = FALSE;

  Cache  = NULL;
  Spliner= NULL;
  PtNum  = 8;
  IsOK   = FALSE;
  Idx	 = 0;
  
  T_4Prepare = TZero;
  T_4Spline  = TZero;
  EOP_X	= 0.0;
  EOP_Y	= 0.0;
  EOP_UT= 0.0;
  EOP_LOD= 0.0;
  EOP_Psi= 0.0;
  EOP_Eps= 0.0;
  HasLeapSecond = FALSE;
  T_LeapSecond  = TZero;
  SLSign = 0;

  Mgr   = NULL;
  P_Px	= NULL;
  P_Py	= NULL;
  P_Put	= NULL;
  P_Npsi= NULL;
  P_Neps= NULL;

  // tmps:
  P_Xp_m2	= NULL;
  P_Yp_m2	= NULL;
  P_Xp_m1	= NULL;
  P_Yp_m1	= NULL;
  P_Xp_p1	= NULL;
  P_Yp_p1	= NULL;
  P_Xp_p2	= NULL;
  P_Yp_p2	= NULL;

  P_Xp_m3	= NULL;
  P_Yp_m3	= NULL;
  P_Xp_p3	= NULL;
  P_Yp_p3	= NULL;

  P_Xp_m4	= NULL;
  P_Yp_m4	= NULL;
  P_Xp_p4	= NULL;
  P_Yp_p4	= NULL;

  P_UT_c1	= NULL;
  P_UT_s1	= NULL;
  P_UT_c2	= NULL;
  P_UT_s2	= NULL;
  P_UT_c3	= NULL;
  P_UT_s3	= NULL;
  P_UT_c4	= NULL;
  P_UT_s4	= NULL;

  // 4test:
  P_Xp_m5	= NULL;
  P_Yp_m5	= NULL;
  P_Xp_p5	= NULL;
  P_Yp_p5	= NULL;
  P_UT_c5	= NULL;
  P_UT_s5	= NULL;

  P_Xp_m6	= NULL;
  P_Yp_m6	= NULL;
  P_Xp_p6	= NULL;
  P_Yp_p6	= NULL;
  P_UT_c6	= NULL;
  P_UT_s6	= NULL;

  P_Xp_m7	= NULL;
  P_Yp_m7	= NULL;
  P_Xp_p7	= NULL;
  P_Yp_p7	= NULL;
  P_UT_c7	= NULL;
  P_UT_s7	= NULL;

  P_Xp_m8	= NULL;
  P_Yp_m8	= NULL;
  P_Xp_p8	= NULL;
  P_Yp_p8	= NULL;
  P_UT_c8	= NULL;
  P_UT_s8	= NULL;

  P_Xp_m9	= NULL;
  P_Yp_m9	= NULL;
  P_Xp_p9	= NULL;
  P_Yp_p9	= NULL;
  P_UT_c9	= NULL;
  P_UT_s9	= NULL;

  P_Xp_m10	= NULL;
  P_Yp_m10	= NULL;
  P_Xp_p10	= NULL;
  P_Yp_p10	= NULL;
  P_UT_c10	= NULL;
  P_UT_s10	= NULL;
};

SBEOP::~SBEOP()
{
  if (Cache) 
    {
      delete Cache;
      Cache = NULL;
    };
  if (Spliner) 
    {
      delete Spliner;
      Spliner = NULL;
    };
};

void SBEOP::checkRanges()
{
  if (count())
    {
      TFinis= *getLast();
      SBMJD		t = TStart= *first();
      unsigned int	idx=0;
      for (SBEOPEntry *E=next(); E; E=next(), idx++)
	{
	  if (*E<=t)
	    Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": The series [" + Label.toString() +
		       "] has wrong set of parameters; for idx [" + QString().setNum(idx) + 
		       "] the argument [" + E->toString(SBMJD::F_DateShort) + 
		       "] less or equal to previous one [" + t.toString(SBMJD::F_DateShort) + "]");
	  t = *E;
	};
    };
};

void SBEOP::importSeries(QString File)
{
  QFile			f(File);
  f.open(IO_ReadOnly);
  QTextStream		s(&f);
  QString		Str("");
  bool			IsDefinedFormat=FALSE;
  enum KnownEOPFormats
    {
      F_Unknown, F_09ColumnsOld, F_09Columns, F_16Columns,
    };
  KnownEOPFormats	Format = F_Unknown;
  bool			IsOK;
  double		d;
  
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": EOP import from the file [" + File + "]");

  // guess format of the file:
  while (!IsDefinedFormat && !s.atEnd())
    {
      s >> Str;
      if (Str.find("FORMAT(3(I4),I7,2(F11.6),2(F12.7),2(F11.6),2(F11.6),2(F11.7),2F12.6)")!=-1)
	{
	  Format = F_16Columns;
	  IsDefinedFormat = TRUE;
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Detected new IERS EOP C04 format");
	}
      else if (Str.find("FORMAT(3(I4),I7,2(F11.6),2(F12.7),2(F12.6),2(F11.6),2(F12.7),2F12.6)")!=-1)
	{
	  Format = F_16Columns;
	  IsDefinedFormat = TRUE;
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		     ": Detected new IERS EOP C04 format (with missprinted FORMAT statement)");
	}
      else if (Str.find("FORMAT(2X,A4,I3,2X,I5,2F9.6,F10.7,2X,F10.7,2X,2F9.5)")!=-1)
	{
	  Format = F_09Columns;
	  IsDefinedFormat = TRUE;
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Detected old IERS EOP C04 format");
	}
      else if (Str.find("FORMAT(2X,A4,I3,2X,I5,2F9.5,F10.6,2X,F10.6,2X,2F9.5)")!=-1)
	{
	  Format = F_09ColumnsOld;
	  IsDefinedFormat = TRUE;
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Detected very old IERS EOP C04 format");
	}
      else if (Str.find("FORMAT(")!=-1)
	{
	  Format = F_Unknown;
	  IsDefinedFormat = TRUE;
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Got unknown EOP format: [" + Str + "]");
	};
    };

  if (s.atEnd())
    Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": Nothing left to read: reached end of the file");
  else if (!IsDefinedFormat)
    Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": Nothing left to read: undefined format of the EOP file");
  else if (Format == F_Unknown)
    Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": Nothing to read: unknown format of the EOP file");
  else
    {
      while (!s.atEnd())
	{
	  SBEOPEntry E;
	  IsOK = FALSE;
	  Str = s.readLine();
	  switch (Format)
	    {
	    case F_Unknown:
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": trying to parce unknown EOP format");
	      break;
	    case F_09ColumnsOld:
	    case F_09Columns:
	      if (Str.length()>=76)
		{ //FORMAT(2X,A4,I3,2X,I5,2F9.6,F10.7,2X,F10.7,2X,2F9.5)
		  //          1         2         3         4         5         6         7         8
		  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
		  //  JAN   1  53371 0.149183 0.238009-0.5039024   0.0004989   -0.05451 -0.00194
		  d = Str.mid(11, 5).toDouble(&IsOK);
		  if (IsOK && d>0.0)
		    {
		      E = d;
		      d = Str.mid(16, 9).toDouble(&IsOK);
		      if (IsOK)
			{
			  E.Xp = d;
			  d = Str.mid(25, 9).toDouble(&IsOK);
			  if (IsOK)
			    {
			      E.Yp = d;
			      d = Str.mid(34,10).toDouble(&IsOK);
			      if (IsOK)
				{
				  E.UT = d;
				  d = Str.mid(46,10).toDouble(&IsOK);
				  if (IsOK)
				    {
				      E.LOD = d;
				      d = Str.mid(58, 9).toDouble(&IsOK);
				      if (IsOK)
					{
					  E.Psi = d;
					  d = Str.mid(67, 9).toDouble(&IsOK);
					  if (IsOK)
					    E.Eps = d;
					};
				    };
				};
			    };
			};
		    };
		};
	      break;
	    case F_16Columns:
	      if (Str.length()>=155)
		{ //FORMAT(3(I4),I7,2(F11.6),2(F12.7),2(F12.6),2(F11.6),2(F12.7),2F12.6)
		  //          1         2         3         4         5         6         7         8         9
		  //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
		  //2006   1   1  53736   0.052611   0.383708   0.3387854   0.0001443  -0.056072  -0.002101 [skipped]
		  d = Str.mid(12, 7).toDouble(&IsOK);
		  if (IsOK && d>0.0)
		    {
		      E = d;
		      d = Str.mid(19,11).toDouble(&IsOK);
		      if (IsOK)
			{
			  E.Xp = d;
			  d = Str.mid(30,11).toDouble(&IsOK);
			  if (IsOK)
			    {
			      E.Yp = d;
			      d = Str.mid(41,12).toDouble(&IsOK);
			      if (IsOK)
				{
				  E.UT = d;
				  d = Str.mid(53,12).toDouble(&IsOK);
				  if (IsOK)
				    {
				      E.LOD = d;
				      d = Str.mid(65,11).toDouble(&IsOK);
				      if (IsOK)
					{
					  E.Psi = d;
					  d = Str.mid(76,11).toDouble(&IsOK);
					  if (IsOK)
					    E.Eps = d;
					};
				    };
				};
			    };
			};
		    };
		};
	      break;
	    };
	  if (IsOK)
	    append(new SBEOPEntry(E));
	};
    };

  f.close();
  s.unsetDevice();

  sort();
  checkRanges();
};

void SBEOP::setPtNum(unsigned int Num_)
{
  IsOK		= FALSE; 
  Idx		= 0; 
  T_4Prepare	= TZero; 
  PtNum		= Num_; 
  if (Cache)
    Cache->resize(PtNum, 7);
  if (Spliner)
    {
      delete Spliner;
      Spliner = new SBSpline(PtNum, 6);
    };
};

void SBEOP::prepare4Date(const SBMJD& T)
{
  if (IsOK && T==T_4Prepare) // it's already prepared
    return;

  if (T<TStart || TFinis<T)
    {
      Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": Arg(UTC): " + T.toString() +
		 " is out of range [" + TStart.toString(SBMJD::F_DateShort) + " : " + 
		 TFinis.toString(SBMJD::F_DateShort) + "]");
      return;
    };

  if (!Cache)
    {
      Cache = new SBMatrix(PtNum, 7, "EOP Interpolation Cache");
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": Cache Matrix has been inited");
    };
  if (!Spliner)
    {
      Spliner = new SBSpline(PtNum, 6);
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": Spline Interpolator has been inited");
    };

  SBMJD		t(T);	// temporary
  SBEOPEntry	*E;
  unsigned int	idx = Idx;

  // alas, that couple QDict/QList does not work here...
  E = at(idx);
  if (t<T_4Prepare)		// look up backward
    while (t<=*E)
      {
	E = prev();
	idx--;
      }
  else			// look up foreward
    {
      while (*E<t)
	{
	  E = next();
	  idx++;
	};
      Idx--;
      E = prev();
    };
  // now, (E <= t <= next())
  Idx = idx;
  Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": got the index [" + QString().setNum(Idx) +
	     "] for the epoch [" + t.toString(SBMJD::F_Short) + "], next to the tabulated point [" +
	     E->toString(SBMJD::F_Short) + "]");
  
  if (idx < PtNum/2) 
    idx = 0;
  else if (idx + PtNum/2 > count()-1) 
    idx = count() - PtNum;
  else 
    idx = idx - PtNum/2;

  unsigned int i=0;
  for (E = at(idx); i<PtNum; E = next(), i++) // prepare the Cache
    {
      Cache->set(i, 0, *E);
      Cache->set(i, 1, E->xp());
      Cache->set(i, 2, E->yp());
      Cache->set(i, 3, E->ut());
      Cache->set(i, 4, E->lod());
      Cache->set(i, 5, E->psi());
      Cache->set(i, 6, E->eps());
    };

  // check for a "leap second"
  HasLeapSecond = FALSE;
  SLSign	= 0;
  double f=Cache->at(0, 3); // first UT1-UTC
  for (i=1; i<PtNum; i++)
    {
      if (Cache->at(i, 3)-f > 0.6) //got it
	{
	  if (HasLeapSecond)
	    {
	      Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + 
			 ": WARNING: too many \"second leaps\" for the epoch [" + T_LeapSecond.toString() + 
			 "]; the results will be unpredictable");
	    }
	  else
	    {
	      HasLeapSecond = TRUE;
	      T_LeapSecond = (long double) Cache->at(i, 0);
	      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + 
			 ": got a \"second leap\" for the epoch [" + T_LeapSecond.toString() + "]");
	    };
	};
      f = Cache->at(i, 3);
    };
  if (HasLeapSecond)
    {
      if (T>=T_LeapSecond)
	{
	  SLSign = 1;
	  for (i=0; i<PtNum; i++) // correct UT1-UTC for a leap
	    if (Cache->at(i, 0) < T_LeapSecond)
	      (*Cache)(i, 3) += 1.0;
	}
      else
	{
	  SLSign = -1;
	  for (i=0; i<PtNum; i++) // correct UT1-UTC for a leap
	    if (Cache->at(i, 0) >= T_LeapSecond)
	      (*Cache)(i, 3) -= 1.0;
	};
    };
  
  
  for (i=0; i<PtNum; i++) // fill matrix for splines
    {
      Spliner->b()->set(i, 0, Cache->at(i, 0));
      Spliner->b()->set(i, 1, Cache->at(i, 1));
      Spliner->b()->set(i, 2, Cache->at(i, 2));
      Spliner->b()->set(i, 3, Cache->at(i, 3));
      Spliner->b()->set(i, 4, Cache->at(i, 4));
      Spliner->b()->set(i, 5, Cache->at(i, 5));
      Spliner->b()->set(i, 6, Cache->at(i, 6));
    };
  
  Spliner->prepare4Spline();
  
  T_4Prepare= T;
  IsOK = TRUE;
};

void SBEOP::interpolateEOPs(const SBMJD& T)
{
  if (!IsOK)
    {
      Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + 
		 ": ::interpolateEOPs called but is not READY; IGNORED");
      EOP_X  = 0.0;
      EOP_Y  = 0.0;
      EOP_UT = 0.0;
      EOP_LOD= 0.0;
      EOP_Psi= 0.0;
      EOP_Eps= 0.0;
      return;
    };
  
  if (Cache->at(0,0)> (double)T + 1.0 || (double)T - 1.0 > Cache->at(PtNum-1, 0))
    {
      Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": Arg(UTC): " + T.toString() +
		 " is out of PREPARED range [" + SBMJD(Cache->at(0,0)).toString(SBMJD::F_DateShort) + 
		 " : " + SBMJD(Cache->at(PtNum-1, 0)).toString(SBMJD::F_DateShort) + "]; IGNORED");
      EOP_X  = 0.0;
      EOP_Y  = 0.0;
      EOP_UT = 0.0;
      EOP_LOD= 0.0;
      EOP_Psi= 0.0;
      EOP_Eps= 0.0;
      return;
    };
  
  if (T == T_4Spline)	// already calculated
    return;

  SBConfig *Cfg = Mgr?&Mgr->prj()->cfg():&Config;
  bool IsUseNutation = Cfg->nutModel()==SBConfig::NM_IAU1980?TRUE:Cfg->isEOP_IAU2K_XY();
  
  // make an interpolation and precalculation:
  // they are in rads and days:
  EOP_X  = Spliner->spline(0, T) *SEC2RAD;
  EOP_Y  = Spliner->spline(1, T) *SEC2RAD;
  EOP_UT = Spliner->spline(2, T) /DAY2SEC;
  EOP_LOD= Spliner->spline(3, T) /DAY2SEC;

  if (IsUseNutation)
    {
      EOP_Psi= Spliner->spline(4, T) *SEC2RAD;
      EOP_Eps= Spliner->spline(5, T) *SEC2RAD;
    }
  else
    {
      EOP_Psi= 0.0;
      EOP_Eps= 0.0;
    };
  
  if (HasLeapSecond)
    {
      if (T>T_LeapSecond && SLSign <0)
	EOP_UT -= 1.0/DAY2SEC;
      if (T_LeapSecond>T && SLSign >0)
	EOP_UT += 1.0/DAY2SEC;
    };
  
  updateParameters(Mgr, T);
  if (P_Px)   EOP_X   += P_Px->v();
  if (P_Py)   EOP_Y   += P_Py->v();
  if (P_Put)  EOP_UT  += P_Put->v();
  if (P_Npsi) EOP_Psi += P_Npsi->v();
  if (P_Neps) EOP_Eps += P_Neps->v();

  T_4Spline = T;
  // report:
  if (Log->isEligible(SBLog::DBG, SBLog::PINTERP))
    {
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": Arg(UTC): " + T.toString());
      
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": x-coord (arcsec): " + 
		 QString().sprintf("%12.8f; w/estimation: %12.8f", 
				   EOP_X/SEC2RAD, P_Px?P_Px->v()/SEC2RAD:0.0));
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": y-coord (arcsec): " + 
		 QString().sprintf("%12.8f; w/estimation: %12.8f", 
				   EOP_Y/SEC2RAD, P_Py?P_Py->v()/SEC2RAD:0.0));
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": UT1-UTC    (sec): " + 
		 QString().sprintf("%12.8f; w/estimation: %12.8f", 
				   EOP_UT*DAY2SEC, P_Put?P_Put->v()*DAY2SEC:0.0));
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": LOD        (sec): " + 
		 QString().sprintf("%12.8f;", EOP_LOD*DAY2SEC));
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": Psi     (arcsec): " + 
		 QString().sprintf("%12.8f; w/estimation: %12.8f", 
				   EOP_Psi/SEC2RAD, P_Npsi?P_Npsi->v()/SEC2RAD:0.0));
      Log->write(SBLog::DBG, SBLog::PINTERP, ClassName() + ": Epsilon (arcsec): " + 
		 QString().sprintf("%12.8f; w/estimation: %12.8f", 
				   EOP_Eps/SEC2RAD, P_Neps?P_Neps->v()/SEC2RAD:0.0));
    };
};

double SBEOP::dX(const SBMJD& T) const 
{
  if (T!=T_4Spline)
    Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": wrong arg [" + T.toString() + 
	       "] in calling for dX(T); expected [" + T_4Spline.toString() + "]");

  double	add=0.0;
  double	Phi=0.0;

  if (P_Xp_m2 && Mgr && Mgr->frame())
    {
      Phi = Mgr->frame()->time().omega();
      add = 
	P_Xp_m2 ->v()*cos( -2.0*Phi) + P_Yp_m2 ->v()*sin( -2.0*Phi) +
	P_Xp_m1 ->v()*cos(     -Phi) + P_Yp_m1 ->v()*sin(     -Phi) +
	P_Xp_p1 ->v()*cos(      Phi) + P_Yp_p1 ->v()*sin(      Phi) +
	P_Xp_p2 ->v()*cos(  2.0*Phi) + P_Yp_p2 ->v()*sin(  2.0*Phi) + 

	P_Xp_m3 ->v()*cos( -3.0*Phi) + P_Yp_m3 ->v()*sin( -3.0*Phi) +
	P_Xp_p3 ->v()*cos(  3.0*Phi) + P_Yp_p3 ->v()*sin(  3.0*Phi) +

	P_Xp_m4 ->v()*cos( -4.0*Phi) + P_Yp_m4 ->v()*sin( -4.0*Phi) +
	P_Xp_p4 ->v()*cos(  4.0*Phi) + P_Yp_p4 ->v()*sin(  4.0*Phi) +

	// 4test:
	P_Xp_m5 ->v()*cos( -5.0*Phi) + P_Yp_m5 ->v()*sin( -5.0*Phi) +
	P_Xp_p5 ->v()*cos(  5.0*Phi) + P_Yp_p5 ->v()*sin(  5.0*Phi) +
	P_Xp_m6 ->v()*cos( -6.0*Phi) + P_Yp_m6 ->v()*sin( -6.0*Phi) +
	P_Xp_p6 ->v()*cos(  6.0*Phi) + P_Yp_p6 ->v()*sin(  6.0*Phi) +

	P_Xp_m7 ->v()*cos( -7.0*Phi) + P_Yp_m7 ->v()*sin( -7.0*Phi) +
	P_Xp_p7 ->v()*cos(  7.0*Phi) + P_Yp_p7 ->v()*sin(  7.0*Phi) +
	P_Xp_m8 ->v()*cos( -8.0*Phi) + P_Yp_m8 ->v()*sin( -8.0*Phi) +
	P_Xp_p8 ->v()*cos(  8.0*Phi) + P_Yp_p8 ->v()*sin(  8.0*Phi) +

	P_Xp_m9 ->v()*cos( -9.0*Phi) + P_Yp_m9 ->v()*sin( -9.0*Phi) +
	P_Xp_p9 ->v()*cos(  9.0*Phi) + P_Yp_p9 ->v()*sin(  9.0*Phi) +
	P_Xp_m10->v()*cos(-10.0*Phi) + P_Yp_m10->v()*sin(-10.0*Phi) +
	P_Xp_p10->v()*cos( 10.0*Phi) + P_Yp_p10->v()*sin( 10.0*Phi) 
	;
    };

  return EOP_X + add;
};

double SBEOP::dY(const SBMJD& T) const 
{
  if (T!=T_4Spline)
    Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": wrong arg [" + T.toString() + 
	       "] in calling for dY(T); expected [" + T_4Spline.toString() + "]");
  double	add=0.0;
  double	Phi=0.0;

  if (P_Xp_m2 && Mgr && Mgr->frame())
    {
      Phi = Mgr->frame()->time().omega();
      add = 
	- P_Xp_m2 ->v()*sin( -2.0*Phi) + P_Yp_m2 ->v()*cos( -2.0*Phi)
	- P_Xp_m1 ->v()*sin(     -Phi) + P_Yp_m1 ->v()*cos(     -Phi)
	- P_Xp_p1 ->v()*sin(      Phi) + P_Yp_p1 ->v()*cos(      Phi)
	- P_Xp_p2 ->v()*sin(  2.0*Phi) + P_Yp_p2 ->v()*cos(  2.0*Phi)

	- P_Xp_m3 ->v()*sin( -3.0*Phi) + P_Yp_m3 ->v()*cos( -3.0*Phi)
	- P_Xp_p3 ->v()*sin(  3.0*Phi) + P_Yp_p3 ->v()*cos(  3.0*Phi)

	- P_Xp_m4 ->v()*sin( -4.0*Phi) + P_Yp_m4 ->v()*cos( -4.0*Phi)
	- P_Xp_p4 ->v()*sin(  4.0*Phi) + P_Yp_p4 ->v()*cos(  4.0*Phi)

	// 4test:
	- P_Xp_m5 ->v()*sin( -5.0*Phi) + P_Yp_m5 ->v()*cos( -5.0*Phi)
	- P_Xp_p5 ->v()*sin(  5.0*Phi) + P_Yp_p5 ->v()*cos(  5.0*Phi)
	- P_Xp_m6 ->v()*sin( -6.0*Phi) + P_Yp_m6 ->v()*cos( -6.0*Phi)
	- P_Xp_p6 ->v()*sin(  6.0*Phi) + P_Yp_p6 ->v()*cos(  6.0*Phi)

	- P_Xp_m7 ->v()*sin( -7.0*Phi) + P_Yp_m7 ->v()*cos( -7.0*Phi)
	- P_Xp_p7 ->v()*sin(  7.0*Phi) + P_Yp_p7 ->v()*cos(  7.0*Phi)
	- P_Xp_m8 ->v()*sin( -8.0*Phi) + P_Yp_m8 ->v()*cos( -8.0*Phi)
	- P_Xp_p8 ->v()*sin(  8.0*Phi) + P_Yp_p8 ->v()*cos(  8.0*Phi)

	- P_Xp_m9 ->v()*sin( -9.0*Phi) + P_Yp_m9 ->v()*cos( -9.0*Phi)
	- P_Xp_p9 ->v()*sin(  9.0*Phi) + P_Yp_p9 ->v()*cos(  9.0*Phi)
	- P_Xp_m10->v()*sin(-10.0*Phi) + P_Yp_m10->v()*cos(-10.0*Phi)
	- P_Xp_p10->v()*sin( 10.0*Phi) + P_Yp_p10->v()*cos( 10.0*Phi)
	;
    };

  return EOP_Y + add;
};

double SBEOP::dUT1_UTC(const SBMJD& T) const 
{
  if (T!=T_4Spline)
    Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": wrong arg [" + T.toString() + 
	       "] in calling for dUT1_UTC(T); expected [" + T_4Spline.toString() + "]");

  double	add=0.0;
  double	Phi=0.0;

  if (P_UT_c1 && Mgr && Mgr->frame())
    {
      Phi = Mgr->frame()->time().omega();
      add = 
	P_UT_c1 ->v()*cos(     Phi) + P_UT_s1 ->v()*sin(     Phi) +
	P_UT_c2 ->v()*cos( 2.0*Phi) + P_UT_s2 ->v()*sin( 2.0*Phi) +
	P_UT_c3 ->v()*cos( 3.0*Phi) + P_UT_s3 ->v()*sin( 3.0*Phi) +
	P_UT_c4 ->v()*cos( 4.0*Phi) + P_UT_s4 ->v()*sin( 4.0*Phi) +

	// 4test:
	P_UT_c5 ->v()*cos( 5.0*Phi) + P_UT_s5 ->v()*sin( 5.0*Phi) +
	P_UT_c6 ->v()*cos( 6.0*Phi) + P_UT_s6 ->v()*sin( 6.0*Phi) +

	P_UT_c7 ->v()*cos( 7.0*Phi) + P_UT_s7 ->v()*sin( 7.0*Phi) +
	P_UT_c8 ->v()*cos( 8.0*Phi) + P_UT_s8 ->v()*sin( 8.0*Phi) +

	P_UT_c9 ->v()*cos( 9.0*Phi) + P_UT_s9 ->v()*sin( 9.0*Phi) +
	P_UT_c10->v()*cos(10.0*Phi) + P_UT_s10->v()*sin(10.0*Phi)
	;
    };

  return EOP_UT + add;
};

double SBEOP::dLOD(const SBMJD& T) const  
{
  if (T!=T_4Spline)
    Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": wrong arg [" + T.toString() + 
	       "] in calling for dLOD(T); expected [" + T_4Spline.toString() + "]");
  return EOP_LOD;
};

double SBEOP::dPsi(const SBMJD& T) const  
{
  if (T!=T_4Spline)
    Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": wrong arg [" + T.toString() + 
	       "] in calling for dPsi(T); expected [" + T_4Spline.toString() + "]");
  return EOP_Psi;
};

double SBEOP::dEps(const SBMJD& T) const  
{
  if (T!=T_4Spline)
    Log->write(SBLog::WRN, SBLog::PINTERP, ClassName() + ": wrong arg [" + T.toString() + 
	       "] in calling for dEps(T); expected [" + T_4Spline.toString() + "]");
  return EOP_Eps;
};


void SBEOP::createParameters()
{
  QString Prefix("EOP: ");
  if (P_Px) delete P_Px;
  P_Px = new SBParameter(Prefix +   "X_p, mas");
  if (P_Py) delete P_Py;
  P_Py = new SBParameter(Prefix +   "Y_p, mas");
  if (P_Put) delete P_Put;
  P_Put = new SBParameter(Prefix +  "UT,  ms ");
  if (P_Npsi) delete P_Npsi;
  P_Npsi = new SBParameter(Prefix + "Psi, mas");
  if (P_Npsi) delete P_Neps;
  P_Neps = new SBParameter(Prefix + "Eps, mas");

  if (P_Xp_m2) delete P_Xp_m2;
  P_Xp_m2 = new SBParameter(Prefix + "P_Xp_m2, mas");
  if (P_Yp_m2) delete P_Yp_m2;
  P_Yp_m2 = new SBParameter(Prefix + "P_Yp_m2, mas");

  if (P_Xp_m1) delete P_Xp_m1;
  P_Xp_m1 = new SBParameter(Prefix + "P_Xp_m1, mas");
  if (P_Yp_m1) delete P_Yp_m1;
  P_Yp_m1 = new SBParameter(Prefix + "P_Yp_m1, mas");


  if (P_Xp_p1) delete P_Xp_p1;
  P_Xp_p1 = new SBParameter(Prefix + "P_Xp_p1, mas");
  if (P_Yp_p1) delete P_Yp_p1;
  P_Yp_p1 = new SBParameter(Prefix + "P_Yp_p1, mas");

  if (P_Xp_p2) delete P_Xp_p2;
  P_Xp_p2 = new SBParameter(Prefix + "P_Xp_p2, mas");
  if (P_Yp_p2) delete P_Yp_p2;
  P_Yp_p2 = new SBParameter(Prefix + "P_Yp_p2, mas");


  if (P_Xp_m3) delete P_Xp_m3;
  P_Xp_m3 = new SBParameter(Prefix + "P_Xp_m3, mas");
  if (P_Yp_m3) delete P_Yp_m3;
  P_Yp_m3 = new SBParameter(Prefix + "P_Yp_m3, mas");

  if (P_Xp_p3) delete P_Xp_p3;
  P_Xp_p3 = new SBParameter(Prefix + "P_Xp_p3, mas");
  if (P_Yp_p3) delete P_Yp_p3;
  P_Yp_p3 = new SBParameter(Prefix + "P_Yp_p3, mas");

  if (P_Xp_m4) delete P_Xp_m4;
  P_Xp_m4 = new SBParameter(Prefix + "P_Xp_m4, mas");
  if (P_Yp_m4) delete P_Yp_m4;
  P_Yp_m4 = new SBParameter(Prefix + "P_Yp_m4, mas");

  if (P_Xp_p4) delete P_Xp_p4;
  P_Xp_p4 = new SBParameter(Prefix + "P_Xp_p4, mas");
  if (P_Yp_p4) delete P_Yp_p4;
  P_Yp_p4 = new SBParameter(Prefix + "P_Yp_p4, mas");


  if (P_UT_c1) delete P_UT_c1;
  P_UT_c1 = new SBParameter(Prefix + "P_UT_c1, ms");
  if (P_UT_s1) delete P_UT_s1;
  P_UT_s1 = new SBParameter(Prefix + "P_UT_s1, ms");
  if (P_UT_c2) delete P_UT_c2;
  P_UT_c2 = new SBParameter(Prefix + "P_UT_c2, ms");
  if (P_UT_s2) delete P_UT_s2;
  P_UT_s2 = new SBParameter(Prefix + "P_UT_s2, ms");

  if (P_UT_c3) delete P_UT_c3;
  P_UT_c3 = new SBParameter(Prefix + "P_UT_c3, ms");
  if (P_UT_s3) delete P_UT_s3;
  P_UT_s3 = new SBParameter(Prefix + "P_UT_s3, ms");
  if (P_UT_c4) delete P_UT_c4;
  P_UT_c4 = new SBParameter(Prefix + "P_UT_c4, ms");
  if (P_UT_s4) delete P_UT_s4;
  P_UT_s4 = new SBParameter(Prefix + "P_UT_s4, ms");

  // 4test:
  if (P_Xp_m5) delete P_Xp_m5;   P_Xp_m5 = new SBParameter(Prefix + "P_Xp_m5, mas"); 
  if (P_Yp_m5) delete P_Yp_m5;   P_Yp_m5 = new SBParameter(Prefix + "P_Yp_m5, mas");
  if (P_Xp_p5) delete P_Xp_p5;   P_Xp_p5 = new SBParameter(Prefix + "P_Xp_p5, mas");
  if (P_Yp_p5) delete P_Yp_p5;   P_Yp_p5 = new SBParameter(Prefix + "P_Yp_p5, mas");
  if (P_UT_c5) delete P_UT_c5;   P_UT_c5 = new SBParameter(Prefix + "P_UT_c5, ms");
  if (P_UT_s5) delete P_UT_s5;   P_UT_s5 = new SBParameter(Prefix + "P_UT_s5, ms");

  if (P_Xp_m6) delete P_Xp_m6;   P_Xp_m6 = new SBParameter(Prefix + "P_Xp_m6, mas");
  if (P_Yp_m6) delete P_Yp_m6;   P_Yp_m6 = new SBParameter(Prefix + "P_Yp_m6, mas");
  if (P_Xp_p6) delete P_Xp_p6;   P_Xp_p6 = new SBParameter(Prefix + "P_Xp_p6, mas");
  if (P_Yp_p6) delete P_Yp_p6;   P_Yp_p6 = new SBParameter(Prefix + "P_Yp_p6, mas");
  if (P_UT_c6) delete P_UT_c6;   P_UT_c6 = new SBParameter(Prefix + "P_UT_c6, ms");
  if (P_UT_s6) delete P_UT_s6;   P_UT_s6 = new SBParameter(Prefix + "P_UT_s6, ms");


  if (P_Xp_m7) delete P_Xp_m7;   P_Xp_m7 = new SBParameter(Prefix + "P_Xp_m7, mas"); 
  if (P_Yp_m7) delete P_Yp_m7;   P_Yp_m7 = new SBParameter(Prefix + "P_Yp_m7, mas");
  if (P_Xp_p7) delete P_Xp_p7;   P_Xp_p7 = new SBParameter(Prefix + "P_Xp_p7, mas");
  if (P_Yp_p7) delete P_Yp_p7;   P_Yp_p7 = new SBParameter(Prefix + "P_Yp_p7, mas");
  if (P_UT_c7) delete P_UT_c7;   P_UT_c7 = new SBParameter(Prefix + "P_UT_c7, ms");
  if (P_UT_s7) delete P_UT_s7;   P_UT_s7 = new SBParameter(Prefix + "P_UT_s7, ms");

  if (P_Xp_m8) delete P_Xp_m8;   P_Xp_m8 = new SBParameter(Prefix + "P_Xp_m8, mas");
  if (P_Yp_m8) delete P_Yp_m8;   P_Yp_m8 = new SBParameter(Prefix + "P_Yp_m8, mas");
  if (P_Xp_p8) delete P_Xp_p8;   P_Xp_p8 = new SBParameter(Prefix + "P_Xp_p8, mas");
  if (P_Yp_p8) delete P_Yp_p8;   P_Yp_p8 = new SBParameter(Prefix + "P_Yp_p8, mas");
  if (P_UT_c8) delete P_UT_c8;   P_UT_c8 = new SBParameter(Prefix + "P_UT_c8, ms");
  if (P_UT_s8) delete P_UT_s8;   P_UT_s8 = new SBParameter(Prefix + "P_UT_s8, ms");

  if (P_Xp_m9) delete P_Xp_m9;   P_Xp_m9 = new SBParameter(Prefix + "P_Xp_m9, mas"); 
  if (P_Yp_m9) delete P_Yp_m9;   P_Yp_m9 = new SBParameter(Prefix + "P_Yp_m9, mas");
  if (P_Xp_p9) delete P_Xp_p9;   P_Xp_p9 = new SBParameter(Prefix + "P_Xp_p9, mas");
  if (P_Yp_p9) delete P_Yp_p9;   P_Yp_p9 = new SBParameter(Prefix + "P_Yp_p9, mas");
  if (P_UT_c9) delete P_UT_c9;   P_UT_c9 = new SBParameter(Prefix + "P_UT_c9, ms");
  if (P_UT_s9) delete P_UT_s9;   P_UT_s9 = new SBParameter(Prefix + "P_UT_s9, ms");

  if (P_Xp_m10) delete P_Xp_m10; P_Xp_m10 = new SBParameter(Prefix + "P_Xp_m10, mas");
  if (P_Yp_m10) delete P_Yp_m10; P_Yp_m10 = new SBParameter(Prefix + "P_Yp_m10, mas");
  if (P_Xp_p10) delete P_Xp_p10; P_Xp_p10 = new SBParameter(Prefix + "P_Xp_p10, mas");
  if (P_Yp_p10) delete P_Yp_p10; P_Yp_p10 = new SBParameter(Prefix + "P_Yp_p10, mas");
  if (P_UT_c10) delete P_UT_c10; P_UT_c10 = new SBParameter(Prefix + "P_UT_c10, ms");
  if (P_UT_s10) delete P_UT_s10; P_UT_s10 = new SBParameter(Prefix + "P_UT_s10, ms");
};

void SBEOP::releaseParameters()
{
  if (P_Px)
    {
      delete P_Px;
      P_Px = NULL;
    };
  if (P_Py)
    {
      delete P_Py;
      P_Py = NULL;
    };
  if (P_Put)
    {
      delete P_Put;
      P_Put = NULL;
    };
  if (P_Npsi)
    {
      delete P_Npsi;
      P_Npsi = NULL;
    };
  if (P_Neps)
    {
      delete P_Neps;
      P_Neps = NULL;
    };

  // tmps:
  if (P_Xp_m2)
    {
      delete P_Xp_m2;
      P_Xp_m2 = NULL;
    };
  if (P_Yp_m2)
    {
      delete P_Yp_m2;
      P_Yp_m2 = NULL;
    };
  if (P_Xp_m1)
    {
      delete P_Xp_m1;
      P_Xp_m1 = NULL;
    };
  if (P_Yp_m1)
    {
      delete P_Yp_m1;
      P_Yp_m1 = NULL;
    };

  if (P_Xp_p1)
    {
      delete P_Xp_p1;
      P_Xp_p1 = NULL;
    };
  if (P_Yp_p1)
    {
      delete P_Yp_p1;
      P_Yp_p1 = NULL;
    };
  if (P_Xp_p2)
    {
      delete P_Xp_p2;
      P_Xp_p2 = NULL;
    };
  if (P_Yp_p2)
    {
      delete P_Yp_p2;
      P_Yp_p2 = NULL;
    };


  if (P_Xp_m3)
    {
      delete P_Xp_m3;
      P_Xp_m3 = NULL;
    };
  if (P_Yp_m3)
    {
      delete P_Yp_m3;
      P_Yp_m3 = NULL;
    };
  if (P_Xp_p3)
    {
      delete P_Xp_p3;
      P_Xp_p3 = NULL;
    };
  if (P_Yp_p3)
    {
      delete P_Yp_p3;
      P_Yp_p3 = NULL;
    };


  if (P_Xp_m4)
    {
      delete P_Xp_m4;
      P_Xp_m4 = NULL;
    };
  if (P_Yp_m4)
    {
      delete P_Yp_m4;
      P_Yp_m4 = NULL;
    };
  if (P_Xp_p4)
    {
      delete P_Xp_p4;
      P_Xp_p4 = NULL;
    };
  if (P_Yp_p4)
    {
      delete P_Yp_p4;
      P_Yp_p4 = NULL;
    };


  if (P_UT_c1)
    {
      delete P_UT_c1;
      P_UT_c1 = NULL;
    };
  if (P_UT_s1)
    {
      delete P_UT_s1;
      P_UT_s1 = NULL;
    };

  if (P_UT_c2)
    {
      delete P_UT_c2;
      P_UT_c2 = NULL;
    };
  if (P_UT_s2)
    {
      delete P_UT_s2;
      P_UT_s2 = NULL;
    };

  if (P_UT_c3)
    {
      delete P_UT_c3;
      P_UT_c3 = NULL;
    };
  if (P_UT_s3)
    {
      delete P_UT_s3;
      P_UT_s3 = NULL;
    };
  if (P_UT_c4)
    {
      delete P_UT_c4;
      P_UT_c4 = NULL;
    };
  if (P_UT_s4)
    {
      delete P_UT_s4;
      P_UT_s4 = NULL;
    };

  // 4test:
  if (P_Xp_m5)  {delete P_Xp_m5;  P_Xp_m5 = NULL;};
  if (P_Yp_m5)  {delete P_Yp_m5;  P_Yp_m5 = NULL;};
  if (P_Xp_p5)  {delete P_Xp_p5;  P_Xp_p5 = NULL;};
  if (P_Yp_p5)  {delete P_Yp_p5;  P_Yp_p5 = NULL;};
  if (P_UT_c5)  {delete P_UT_c5;  P_UT_c5 = NULL;};
  if (P_UT_s5)  {delete P_UT_s5;  P_UT_s5 = NULL;};

  if (P_Xp_m6)  {delete P_Xp_m6;  P_Xp_m6 = NULL;};
  if (P_Yp_m6)  {delete P_Yp_m6;  P_Yp_m6 = NULL;};
  if (P_Xp_p6)  {delete P_Xp_p6;  P_Xp_p6 = NULL;};
  if (P_Yp_p6)  {delete P_Yp_p6;  P_Yp_p6 = NULL;};
  if (P_UT_c6)  {delete P_UT_c6;  P_UT_c6 = NULL;};
  if (P_UT_s6)  {delete P_UT_s6;  P_UT_s6 = NULL;};

  if (P_Xp_m7)  {delete P_Xp_m7;  P_Xp_m7 = NULL;};
  if (P_Yp_m7)  {delete P_Yp_m7;  P_Yp_m7 = NULL;};
  if (P_Xp_p7)  {delete P_Xp_p7;  P_Xp_p7 = NULL;};
  if (P_Yp_p7)  {delete P_Yp_p7;  P_Yp_p7 = NULL;};
  if (P_UT_c7)  {delete P_UT_c7;  P_UT_c7 = NULL;};
  if (P_UT_s7)  {delete P_UT_s7;  P_UT_s7 = NULL;};

  if (P_Xp_m8)  {delete P_Xp_m8;  P_Xp_m8 = NULL;};
  if (P_Yp_m8)  {delete P_Yp_m8;  P_Yp_m8 = NULL;};
  if (P_Xp_p8)  {delete P_Xp_p8;  P_Xp_p8 = NULL;};
  if (P_Yp_p8)  {delete P_Yp_p8;  P_Yp_p8 = NULL;};
  if (P_UT_c8)  {delete P_UT_c8;  P_UT_c8 = NULL;};
  if (P_UT_s8)  {delete P_UT_s8;  P_UT_s8 = NULL;};

  if (P_Xp_m9)  {delete P_Xp_m9;  P_Xp_m9 = NULL;};
  if (P_Yp_m9)  {delete P_Yp_m9;  P_Yp_m9 = NULL;};
  if (P_Xp_p9)  {delete P_Xp_p9;  P_Xp_p9 = NULL;};
  if (P_Yp_p9)  {delete P_Yp_p9;  P_Yp_p9 = NULL;};
  if (P_UT_c9)  {delete P_UT_c9;  P_UT_c9 = NULL;};
  if (P_UT_s9)  {delete P_UT_s9;  P_UT_s9 = NULL;};

  if (P_Xp_m10) {delete P_Xp_m10; P_Xp_m10 = NULL;};
  if (P_Yp_m10) {delete P_Yp_m10; P_Yp_m10 = NULL;};
  if (P_Xp_p10) {delete P_Xp_p10; P_Xp_p10 = NULL;};
  if (P_Yp_p10) {delete P_Yp_p10; P_Yp_p10 = NULL;};
  if (P_UT_c10) {delete P_UT_c10; P_UT_c10 = NULL;};
  if (P_UT_s10) {delete P_UT_s10; P_UT_s10 = NULL;};
};

void SBEOP::updateParameters(SBRunManager* Mgr, const SBMJD& T)
{
  if (Mgr)
    {
      Mgr->solution()->updateParameter(P_Px,   T);
      Mgr->solution()->updateParameter(P_Py,   T);
      Mgr->solution()->updateParameter(P_Put,  T);
      Mgr->solution()->updateParameter(P_Npsi, T);
      Mgr->solution()->updateParameter(P_Neps, T);

      // tmps:
      Mgr->solution()->updateParameter(P_Xp_m2, T);
      Mgr->solution()->updateParameter(P_Yp_m2, T);
      Mgr->solution()->updateParameter(P_Xp_m1, T);
      Mgr->solution()->updateParameter(P_Yp_m1, T);
      Mgr->solution()->updateParameter(P_Xp_p1, T);
      Mgr->solution()->updateParameter(P_Yp_p1, T);
      Mgr->solution()->updateParameter(P_Xp_p2, T);
      Mgr->solution()->updateParameter(P_Yp_p2, T);

      Mgr->solution()->updateParameter(P_Xp_m3, T);
      Mgr->solution()->updateParameter(P_Yp_m3, T);
      Mgr->solution()->updateParameter(P_Xp_p3, T);
      Mgr->solution()->updateParameter(P_Yp_p3, T);

      Mgr->solution()->updateParameter(P_Xp_m4, T);
      Mgr->solution()->updateParameter(P_Yp_m4, T);
      Mgr->solution()->updateParameter(P_Xp_p4, T);
      Mgr->solution()->updateParameter(P_Yp_p4, T);

      Mgr->solution()->updateParameter(P_UT_c1, T);
      Mgr->solution()->updateParameter(P_UT_s1, T);
      Mgr->solution()->updateParameter(P_UT_c2, T);
      Mgr->solution()->updateParameter(P_UT_s2, T);
      Mgr->solution()->updateParameter(P_UT_c3, T);
      Mgr->solution()->updateParameter(P_UT_s3, T);
      Mgr->solution()->updateParameter(P_UT_c4, T);
      Mgr->solution()->updateParameter(P_UT_s4, T);

      // 4test:
      Mgr->solution()->updateParameter(P_Xp_m5, T);
      Mgr->solution()->updateParameter(P_Yp_m5, T);
      Mgr->solution()->updateParameter(P_Xp_p5, T);
      Mgr->solution()->updateParameter(P_Yp_p5, T);
      Mgr->solution()->updateParameter(P_UT_c5, T);
      Mgr->solution()->updateParameter(P_UT_s5, T);

      Mgr->solution()->updateParameter(P_Xp_m6, T);
      Mgr->solution()->updateParameter(P_Yp_m6, T);
      Mgr->solution()->updateParameter(P_Xp_p6, T);
      Mgr->solution()->updateParameter(P_Yp_p6, T);
      Mgr->solution()->updateParameter(P_UT_c6, T);
      Mgr->solution()->updateParameter(P_UT_s6, T);

      Mgr->solution()->updateParameter(P_Xp_m7, T);
      Mgr->solution()->updateParameter(P_Yp_m7, T);
      Mgr->solution()->updateParameter(P_Xp_p7, T);
      Mgr->solution()->updateParameter(P_Yp_p7, T);
      Mgr->solution()->updateParameter(P_UT_c7, T);
      Mgr->solution()->updateParameter(P_UT_s7, T);

      Mgr->solution()->updateParameter(P_Xp_m8, T);
      Mgr->solution()->updateParameter(P_Yp_m8, T);
      Mgr->solution()->updateParameter(P_Xp_p8, T);
      Mgr->solution()->updateParameter(P_Yp_p8, T);
      Mgr->solution()->updateParameter(P_UT_c8, T);
      Mgr->solution()->updateParameter(P_UT_s8, T);

      Mgr->solution()->updateParameter(P_Xp_m9, T);
      Mgr->solution()->updateParameter(P_Yp_m9, T);
      Mgr->solution()->updateParameter(P_Xp_p9, T);
      Mgr->solution()->updateParameter(P_Yp_p9, T);
      Mgr->solution()->updateParameter(P_UT_c9, T);
      Mgr->solution()->updateParameter(P_UT_s9, T);

      Mgr->solution()->updateParameter(P_Xp_m10, T);
      Mgr->solution()->updateParameter(P_Yp_m10, T);
      Mgr->solution()->updateParameter(P_Xp_p10, T);
      Mgr->solution()->updateParameter(P_Yp_p10, T);
      Mgr->solution()->updateParameter(P_UT_c10, T);
      Mgr->solution()->updateParameter(P_UT_s10, T);
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEOP friends implementation								*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBEOP& EOP)
{
  return s << (const QList<SBEOPEntry>&)EOP
	   << EOP.Label
	   << EOP.Epoch
	   << EOP.Xp  << EOP.Xps << EOP.Yp   << EOP.Yps << EOP.UT << EOP.UTs << EOP.Psi 
	   << EOP.Psis<< EOP.Eps << EOP.Epss 
	   << (Q_UINT8)EOP.UTHasHiTide << (Q_UINT8)EOP.UTHasLoTide;
};

QDataStream &operator>>(QDataStream& s, SBEOP& EOP)
{
  s >> (QList<SBEOPEntry>&)EOP 
    >> EOP.Label
    >> EOP.Epoch
    >> EOP.Xp   >> EOP.Xps >> EOP.Yp   >> EOP.Yps >> EOP.UT >> EOP.UTs >> EOP.Psi 
    >> EOP.Psis >> EOP.Eps >> EOP.Epss 
    >> (Q_UINT8&)EOP.UTHasHiTide >> (Q_UINT8&)EOP.UTHasLoTide;
  EOP.checkRanges();
  return s;
};

QTextStream &operator<<(QTextStream& s, const SBEOP&)
{
  return s;
};

QTextStream &operator>>(QTextStream& s, SBEOP&)
{
  return s;
};
/*==============================================================================================*/
