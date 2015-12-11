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

#include "SbIObsVLBI.H"

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qstrlist.h>
#include <qtooltip.h>

#include "SbSetup.H"
#include "SbGeoDelay.H"
#include "SbVector.H"
#include "SbIMainWin.H"
#include "SbIPlot.H"
#include "SbIProject.H"




/*==============================================================================================*/
/*												*/
/* SBVLBISesInfoLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBVLBISesInfoLI::text(int col) const
{
  static const char* sOrig [ 4] = {"NGS", "VBD", "DBH", " "};
  TmpString = "NULL";
  double Scale  = 1.0;
  switch (SetUp->units())
    {
    default:
    case SBSetUp::U_SEC: Scale = 1.0e+12;
      break;
    case SBSetUp::U_METER: Scale = 100.0*SBDelay::CLight;
      break;
    case SBSetUp::U_INCH:  Scale = 25.4*SBDelay::CLight;
      break;
    };
  if (SI)
    switch (col)
      {
      case  0: TmpString = SI->tStart().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  1: TmpString = SI->name().copy();
	break;
      case  2: TmpString.sprintf("%d/%d ", SI->version(), SI->localVersion());
	break;
      case  3: TmpString = SI->networkID().copy();
	break;
      case  4: TmpString = SI->userFlag().copy();
	break;
      case  5: 
	int	h,m;
	double	s;
	rad2hms(SI->tInterval()*2.0*M_PI, h,m,s);
	TmpString.sprintf("%02d:%02d:%05.2f", h+24*(int)(floor(SI->tInterval())) ,m,s);
	break;
      case  6: TmpString.sprintf("%d", SI->numObs());
	break;
      case  7: TmpString.sprintf("%d", SI->numSta());
	break;
      case  8: TmpString.sprintf("%d", SI->numBas());
	break;
      case  9: TmpString.sprintf("%d", SI->numSou());
	break;
      case 10: TmpString = sOrig[SI->importedFrom()];
	break;
      case 11: TmpString = SI->correlatorName().copy();
	break;
      case 12: 
	if (SI->dWRMS()==0.0) 
	  TmpString = "0.0";
	else 
	  TmpString.sprintf("%12.4f", Scale*SI->dWRMS());
	break;
      case 13: 
	if (SI->rWRMS()==0.0) 
	  TmpString = "0.0";
	else 
	  TmpString.sprintf("%12.4f", Scale*SI->rWRMS());
	break;
      case 14: 
	if (Mode == BM_PROJECT)
	  TmpString.sprintf("%s", SI->isAttr(SBVLBISesInfo::ConstrEOP)?"EOP":" ");
	else
	  TmpString.sprintf("%s", SI->isAttr(SBVLBISesInfo::preProc)?"YES":" ");
	break;
      case 15:
	TmpString = "";
	if (SI->isAttr(SBVLBISesInfo::HasClockBreak))
	  TmpString+="b";
	if (SI->isAttr(SBVLBISesInfo::BadAllCables))
	  TmpString+="C";
	else if (SI->isAttr(SBVLBISesInfo::BadCable))
	  TmpString+="c";
	if (SI->isAttr(SBVLBISesInfo::BadAllMeteos))
	  TmpString+="M";
	else if (SI->isAttr(SBVLBISesInfo::BadMeteo))
	  TmpString+="m";
	if (SI->isAttr(SBVLBISesInfo::ArtAllMeteos))
	  TmpString+="A";
	else if (SI->isAttr(SBVLBISesInfo::ArtMeteo))
	  TmpString+="a";
	break;
      case 16: TmpString.sprintf("%s", SI->isAttr(SBVLBISesInfo::notValid)?"X":" ");
	break;
      };
  return TmpString;
};

QString SBVLBISesInfoLI::key(int col, bool) const
{
  static const char* sOrig [ 4] = {"1NGS", "2VBD", "0DBH", "3 "};
  TmpString = "NULL";
  if (SI)
    switch (col)
      {
      case  0: TmpString.sprintf("%f", (double)(SI->tStart()));
	break;
      case  1: TmpString = SI->name().copy();
	break;
      case  2: TmpString.sprintf("%03d", SI->version());
	break;
      case  3: TmpString = SI->networkID().copy();
	break;
      case  4: TmpString = SI->userFlag().copy();
	break;
      case  5: TmpString.sprintf("%012.9f", (double)(SI->tInterval()));
	break;
      case  6: TmpString.sprintf("%06d", SI->numObs());
	break;
      case  7: TmpString.sprintf("%06d", SI->numSta());
	break;
      case  8: TmpString.sprintf("%06d", SI->numBas());
	break;
      case  9: TmpString.sprintf("%06d", SI->numSou());
	break;
      case 10: TmpString = sOrig[SI->importedFrom()];
	break;
      case 11: TmpString = SI->correlatorName().copy();
	break;
      case 12: TmpString.sprintf("%024.16f", SetUp->scale()*SI->dWRMS());
	break;
      case 13: TmpString.sprintf("%.16e", SI->rWRMS());
	break;
      case 14: 
	if (Mode == BM_PROJECT)
	  TmpString.sprintf("%s", SI->isAttr(SBVLBISesInfo::ConstrEOP)?"EOP":" ");
	else
	  TmpString.sprintf("%s", SI->isAttr(SBVLBISesInfo::preProc)?"YES":" ");
	break;
      case 15:
	TmpString = "";
	if (SI->isAttr(SBVLBISesInfo::HasClockBreak))
	  TmpString+="b";
	if (SI->isAttr(SBVLBISesInfo::BadAllCables))
	  TmpString+="C";
	else if (SI->isAttr(SBVLBISesInfo::BadCable))
	  TmpString+="c";
	if (SI->isAttr(SBVLBISesInfo::BadAllMeteos))
	  TmpString+="M";
	else if (SI->isAttr(SBVLBISesInfo::BadMeteo))
	  TmpString+="m";
	if (SI->isAttr(SBVLBISesInfo::ArtAllMeteos))
	  TmpString+="A";
	else if (SI->isAttr(SBVLBISesInfo::ArtMeteo))
	  TmpString+="a";
	break;
      case 16: TmpString.sprintf("%s", SI->isAttr(SBVLBISesInfo::notValid)?"X":" ");
	break;
      };
  return TmpString;
};

/* till the next time...
const QPixmap * SBStationListItem::pixmap (int col) const
{
  QPixmap *pm=0;
  if (Station)
    switch (col)
      {
      case 0:
	pm = pmSomeUglyPixmap[SI->isAttr(SBVLBISesInfo::isSomethingElse)];
	break;
      case 2: pm=0;
	break;
      case 3: pm=0;
	break;
      case 4: pm=0;
	break;
      default: pm=0;
	break;
      };
  return pm;
};
*/
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISesPreProcLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBVLBISesPreProcLI::text(int col) const
{
  TmpString = "NULL";
  double Scale  = 1.0;
  switch (SetUp->units())
    {
    default:
    case SBSetUp::U_SEC: Scale = 1.0e+12;
      break;
    case SBSetUp::U_METER: Scale = 100.0*SBDelay::CLight;
      break;
    case SBSetUp::U_INCH:  Scale = 25.4*SBDelay::CLight;
      break;
    };
  if (SI)
    switch (col)
      {
      case  0: TmpString = SI->name().copy();
	break;
      case  1: TmpString.sprintf("%d", SI->numObs());
	break;
      case  2: 
	if (SI->dWRMS()==0.0) 
	  TmpString = "0.0";
	else 
	  TmpString.sprintf("%.2f", Scale*SI->dWRMS());
	break;
      case  3:
	TmpString = "";
	if (SI->isAttr(SBVLBISesInfo::HasClockBreak))
	  TmpString+="b";
	if (SI->isAttr(SBVLBISesInfo::BadAllCables))
	  TmpString+="C";
	else if (SI->isAttr(SBVLBISesInfo::BadCable))
	  TmpString+="c";
	if (SI->isAttr(SBVLBISesInfo::BadAllMeteos))
	  TmpString+="M";
	else if (SI->isAttr(SBVLBISesInfo::BadMeteo))
	  TmpString+="m";
	if (SI->isAttr(SBVLBISesInfo::ArtAllMeteos))
	  TmpString+="A";
	else if (SI->isAttr(SBVLBISesInfo::ArtMeteo))
	  TmpString+="a";
	break;
      case  4: TmpString.sprintf("%c", SI->isAttr(SBVLBISesInfo::preProc)?'Y':' ');
	break;
      };
  return TmpString;
};

QString SBVLBISesPreProcLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (SI)
    switch (col)
      {
      case  0: TmpString.sprintf("%f", (double)(SI->tStart()));
	break;
      case  1: TmpString.sprintf("%06d", SI->numObs());
	break;
      case  2: TmpString.sprintf("%024.16f", SetUp->scale()*SI->dWRMS());
	break;
      case  3:
	TmpString = "";
	if (SI->isAttr(SBVLBISesInfo::HasClockBreak))
	  TmpString+="b";
	if (SI->isAttr(SBVLBISesInfo::BadAllCables))
	  TmpString+="C";
	else if (SI->isAttr(SBVLBISesInfo::BadCable))
	  TmpString+="c";
	if (SI->isAttr(SBVLBISesInfo::BadAllMeteos))
	  TmpString+="M";
	else if (SI->isAttr(SBVLBISesInfo::BadMeteo))
	  TmpString+="m";
	if (SI->isAttr(SBVLBISesInfo::ArtAllMeteos))
	  TmpString+="A";
	else if (SI->isAttr(SBVLBISesInfo::ArtMeteo))
	  TmpString+="a";
	break;
      case  4: TmpString.sprintf("%s", SI->isAttr(SBVLBISesInfo::preProc)?"YES":" ");
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBIObsLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBVLBIObsLI::text(int col) const
{
  double Scale  = 1.0;
  switch (SetUp->units())
    {
    default:
    case SBSetUp::U_SEC: Scale = 1.0e+12;
      break;
    case SBSetUp::U_METER: Scale = 100.0*SBDelay::CLight;
      break;
    case SBSetUp::U_INCH:  Scale = 25.4*SBDelay::CLight;
      break;
    };

  TmpString = "NULL";
  if (Obs && Sess)
    switch (col)
      {
      case  0: TmpString.sprintf("%6d", Obs->idx());
	break;
      case  1: TmpString = (const char*)(" " + Obs->toString(SBMJD::F_HHMMSS) + " ");
	break;
      case  2: TmpString = Sess->stationInfo(Obs->station_1Idx())->aka().copy();
	break;
      case  3: TmpString = Sess->stationInfo(Obs->station_2Idx())->aka().copy();
	break;
      case  4: TmpString = Sess->sourceInfo(Obs->sourceIdx())->aka().copy();
	break;
      case  5: TmpString.sprintf(Scale*Obs->delayErr()<1000.0?"%10.6f":"%14.6g", Scale*Obs->delayErr());
	break;
      case  6: 
	if (Obs->delayRes()==0.0)
	  TmpString = "0.0";
	else
	  TmpString.sprintf("%10.6f", Scale*Obs->delayRes());
	break;
      case  7: TmpString.sprintf(1.0e12*Obs->rateErr()<1000.0?"%10.6f":"%14.6g", 1.0e12*Obs->rateErr());
	break;
      case  8: 
	if (Obs->rateRes()==0.0)
	  TmpString = "0.0";
	else 
	  TmpString.sprintf("%10.6f", 1.0e12*Obs->rateRes());
	break;
      case  9: TmpString.sprintf("%3d", Obs->ambiguityFactor());
	break;
      case 10: TmpString = Obs->qualCode().copy();
	break;
      case 11: TmpString.sprintf("%d:%d", Obs->dUFlag(), Obs->rUFlag());
	break;
      case 12: TmpString.sprintf("%3d", Obs->ionErrorFlag());
	break;
      case 13: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable1))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo1))
	  TmpString+="m";
	break;
      case 14: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable2))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo2))
	  TmpString+="m";
	break;
      case 15: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock1)?'B':' ');
	break;
      case 16: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock2)?'B':' ');
	break;
      case 17: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::notValid)?'X':' ');
	break;
      };
  return TmpString;
};

QString SBVLBIObsLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (Obs && Sess)
    switch (col)
      {
      case  0: TmpString.sprintf("%06d", Obs->idx());
	break;
      case  1: TmpString.sprintf("%12.6f", (double)Obs->mjd());
	break;
      case  2: TmpString = Sess->stationInfo(Obs->station_1Idx())->aka().copy();
	break;
      case  3: TmpString = Sess->stationInfo(Obs->station_2Idx())->aka().copy();
	break;
      case  4: TmpString = Sess->sourceInfo(Obs->sourceIdx())->aka().copy();
	break;
      case  5: TmpString.sprintf("%010.6f", 1.0e9*Obs->delayErr());
	break;
      case  6: TmpString.sprintf("%030.16f", 1000+Obs->delayRes());
	break;
      case  7: TmpString.sprintf("%010.6f", 1.0e12*Obs->rateErr());
	break;
      case  8: TmpString.sprintf("%010.6f", 1.0e12*Obs->rateRes());
	break;
      case  9: TmpString.sprintf("%03d", 100+Obs->ambiguityFactor());
	break;
      case 10: TmpString = Obs->qualCode().copy();
	break;
      case 11: TmpString.sprintf("%02d:%02d", 10+Obs->dUFlag(), 10+Obs->rUFlag());
	break;
      case 12: TmpString.sprintf("%03d", 10+Obs->ionErrorFlag());
	break;
      case 13: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable1))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo1))
	  TmpString+="m";
	break;
      case 14: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable2))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo2))
	  TmpString+="m";
	break;
      case 15: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock1)?'B':' ');
	break;
      case 16: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock2)?'B':' ');
	break;
      case 17: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::notValid)?'X':' ');
	break;
      };
  return TmpString;
};
/* till the next time...
const QPixmap * SBStationListItem::pixmap (int col) const
{
  QPixmap *pm=0;
  if (Station)
    switch (col)
      {
      case 0:
	pm = pmSrcStatus[Source->status()];
	break;
      case 2: pm=0;
	break;
      case 3: pm=0;
	break;
      case 4: pm=0;
	break;
      default: pm=0;
	break;
      };
  return pm;
};
*/
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStaInfoLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBStaInfoLI::text(int col) const
{
  TmpString = "NULL";
  if (StaInf)
    {
      if (Mode == BM_GENERAL)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%6d", StaInf->idx());
	      break;
	    case  1: TmpString = StaInf->aka().copy();
	      break;
	    case  2: TmpString = StaInf->toString().copy();//id
	      break;
	    case  3: TmpString.sprintf("%d", StaInf->num());
	      break;
	    case  4: TmpString.sprintf("%d", StaInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%.4f", SetUp->scale()*StaInf->wrms());
	      break;
	    case  6: 
	      if (StaInf->clockBreaks().count()>0)
		TmpString.sprintf("%d", StaInf->clockBreaks().count());
	      else 
		TmpString = " ";
	      break;
	    case  7: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalClocks)?'Y':' ');
	      break;
	    case  8: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalZenith)?'Y':' ');
	      break;
	    case  9: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::notValid)?'X':' ');
	      break;
	    case 10: TmpString.sprintf("%d", StaInf->cl_Order());
	      break;
	    case 11: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::refClock)?'R':' ');
	      break;
	    case 12: 
	      TmpString = "";
	      if (StaInf->isAttr(SBStationInfo::HasClockBreak))
		TmpString+="b";
	      if (StaInf->isAttr(SBStationInfo::BadCable))
		TmpString+="c";
	      if (StaInf->isAttr(SBStationInfo::InvCable))
		TmpString+="i";
	      if (StaInf->isAttr(SBStationInfo::BadMeteo))
		TmpString+="m";
	      if (StaInf->isAttr(SBStationInfo::ArtMeteo))
		TmpString+="a";
	      break;
	    case 13: TmpString.sprintf("%.1f", StaInf->cl_V(0));
	      break;
	    case 14: TmpString.sprintf("%.3f", StaInf->cl_E(0));
	      break;
	    case 15: TmpString.sprintf("%.1f", StaInf->cl_V(1));
	      break;
	    case 16: TmpString.sprintf("%.3f", StaInf->cl_E(1));
	      break;
	    case 17: TmpString.sprintf("%.2f", StaInf->zw()*100.0);
	      break;
	    case 18: TmpString.sprintf("%.3f", StaInf->errZw()*100.0);
	      break;
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%6d", StaInf->idx());
	      break;
	    case  1: TmpString = StaInf->aka().copy();
	      break;
	    case  2: TmpString = StaInf->toString().copy();//id
	      break;
	    case  3: TmpString.sprintf("%d", StaInf->num());
	      break;
	    case  4: TmpString.sprintf("%d", StaInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%.4f", SetUp->scale()*StaInf->wrms());
	      break;
	    case  6: 
	      if (StaInf->clockBreaks().count()>0)
		TmpString.sprintf("%d", StaInf->clockBreaks().count());
	      else 
		TmpString = " ";
	      break;
	    case  7: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalClocks)?'Y':' ');
	      break;
	    case  8: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalZenith)?'Y':' ');
	      break;
	    case  9: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::notValid)?'X':' ');
	      break;
	    case 10: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::EstCoo)?'Y':' ');
	      break;
	    case 11: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::ConstrCoo)?'*':' ');
	      break;
	    case 12: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::EstVel)?'Y':' ');
	      break;
	    case 13: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::ConstrVel)?'*':' ');
	      break;
	    };
	};
    }
  return TmpString;
};

QString SBStaInfoLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (StaInf) 
    {
      if (Mode == BM_GENERAL)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%06d", StaInf->idx());
	      break;
	    case  1: TmpString = StaInf->aka().copy();
	      break;
	    case  2: TmpString = StaInf->toString().copy();//id
	      break;
	    case  3: TmpString.sprintf("%012d", StaInf->num());
	      break;
	    case  4: TmpString.sprintf("%012d", StaInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%024.16f", SetUp->scale()*StaInf->wrms());
	      break;
	    case  6: TmpString.sprintf("%012d", StaInf->clockBreaks().count());
	      break;
	    case  7: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalClocks)?'Y':' ');
	      break;
	    case  8: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalZenith)?'Y':' ');
	      break;
	    case  9: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::notValid)?'X':' ');
	      break;
	    case 10: TmpString.sprintf("%d", StaInf->cl_Order());
	      break;
	    case 11: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::refClock)?'R':' ');
	      break;
	    case 12: 
	      TmpString = "";
	      if (StaInf->isAttr(SBStationInfo::HasClockBreak))
		TmpString+="b";
	      if (StaInf->isAttr(SBStationInfo::BadCable))
		TmpString+="c";
	      if (StaInf->isAttr(SBStationInfo::InvCable))
		TmpString+="i";
	      if (StaInf->isAttr(SBStationInfo::BadMeteo))
		TmpString+="m";
	      if (StaInf->isAttr(SBStationInfo::ArtMeteo))
		TmpString+="a";
	      break;
	    case 13: TmpString.sprintf("%024.16f", StaInf->cl_V(0));
	      break;
	    case 14: TmpString.sprintf("%024.16f", StaInf->cl_E(0));
	      break;
	    case 15: TmpString.sprintf("%024.16f", StaInf->cl_V(1));
	      break;
	    case 16: TmpString.sprintf("%024.16f", StaInf->cl_E(1));
	      break;
	    case 17: TmpString.sprintf("%024.16f", StaInf->zw());
	      break;
	    case 18: TmpString.sprintf("%024.16f", StaInf->errZw());
	      break;
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%06d", StaInf->idx());
	      break;
	    case  1: TmpString = StaInf->aka().copy();
	      break;
	    case  2: TmpString = StaInf->toString().copy();//id
	      break;
	    case  3: TmpString.sprintf("%012d", StaInf->num());
	      break;
	    case  4: TmpString.sprintf("%012d", StaInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%024.16f", SetUp->scale()*StaInf->wrms());
	      break;
	    case  6: TmpString.sprintf("%012d", StaInf->clockBreaks().count());
	      break;
	    case  7: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalClocks)?'Y':' ');
	      break;
	    case  8: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::UseLocalZenith)?'Y':' ');
	      break;
	    case  9: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::notValid)?'X':' ');
	      break;
	    case 10: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::EstCoo)?'Y':' ');
	      break;
	    case 11: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::ConstrCoo)?'*':' ');
	      break;
	    case 12: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::EstVel)?'Y':' ');
	      break;
	    case 13: TmpString.sprintf("%c", StaInf->isAttr(SBStationInfo::ConstrVel)?'*':' ');
	      break;
	    };
	};
    }
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBasInfoLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBBasInfoLI::text(int col) const
{
  TmpString = "NULL";
  if (BasInf)
    switch (col)
      {
      case  0: TmpString.sprintf("%6d", BasInf->idx());
	break;
      case  1: TmpString = BasInf->aka().copy();
	break;
      case  2: TmpString = BasInf->name().copy();//id
	break;
      case  3: TmpString.sprintf("%d", BasInf->num());
	break;
      case  4: TmpString.sprintf("%d", BasInf->procNum());
	break;
      case  5: TmpString.sprintf("%.4f", SetUp->scale()*BasInf->wrms());
	break;
      case  6: TmpString.sprintf("%c", BasInf->isAttr(SBBaseInfo::notValid)?'X':' ');
	break;
      };
  return TmpString;
};

QString SBBasInfoLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (BasInf)
    switch (col)
      {
      case  0: TmpString.sprintf("%06d", BasInf->idx());
	break;
      case  1: TmpString = BasInf->aka().copy();
	break;
      case  2: TmpString = BasInf->name().copy();//id
	break;
      case  3: TmpString.sprintf("%012d", BasInf->num());
	break;
      case  4: TmpString.sprintf("%012d", BasInf->procNum());
	break;
      case  5: TmpString.sprintf("%024.16f", SetUp->scale()*BasInf->wrms());
	break;
      case  6: TmpString.sprintf("%c", BasInf->isAttr(SBBaseInfo::notValid)?'X':' ');
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSouInfoLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBSouInfoLI::text(int col) const
{
  static const char* cStatus[] = {"D", "C", "O", "N", "?"};
  static const char* cType  [] = {"Q", "G", "L", "l", "O", "?"};
  TmpString = "NULL";
  if (SouInf)
    {
      if (Mode == BM_GENERAL)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%6d", SouInf->idx());
	      break;
	    case  1: TmpString = SouInf->aka().copy();
	      break;
	    case  2: TmpString = SouInf->name().copy();//id
	      break;
	    case  3: TmpString.sprintf("%d", SouInf->num());
	      break;
	    case  4: TmpString.sprintf("%d", SouInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%.4f", SetUp->scale()*SouInf->wrms());
	      break;
	    case  6: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::notValid)?'X':' ');
	      break;
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%6d", SouInf->idx());
	      break;
	    case  1: TmpString = SouInf->aka().copy();
	      break;
	    case  2: TmpString = SouInf->name().copy();//id
	      break;
	    case  3: TmpString.sprintf("%d", SouInf->num());
	      break;
	    case  4: TmpString.sprintf("%d", SouInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%.4f", SetUp->scale()*SouInf->wrms());
	      break;
	    case  6: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::notValid)?'X':' ');
	      break;
	    case  7: 
	      TmpString = cStatus[Status];
	      if (IsSuspected) TmpString += "!";
	      //	    TmpString = IsSuspected?"!":"";
	      break;
	    case  8: TmpString = cType[Type];
	      break;
	    case  9: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::EstCoo)?'Y':' ');
	      break;
	    case 10: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::ConstrCoo)?'*':' ');
	      break;
	    };
	};
    }
  return TmpString;
};

QString SBSouInfoLI::key(int col, bool) const
{
  static const char* cStatus[] = {"AD", "BC", "CO", "DN", "E?"};
  static const char* cType  [] = {"AQ", "BG", "CL", "Dl", "EA", "F?"};
  TmpString = "NULL";
  if (SouInf)
    {
      if (Mode == BM_GENERAL)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%06d", SouInf->idx());
	      break;
	    case  1: TmpString = SouInf->aka().copy();
	      break;
	    case  2: TmpString = SouInf->name().copy();//id
	      break;
	    case  3: TmpString.sprintf("%012d", SouInf->num());
	      break;
	    case  4: TmpString.sprintf("%012d", SouInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%024.16f", SetUp->scale()*SouInf->wrms());
	      break;
	    case  6: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::notValid)?'X':' ');
	      break;
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  switch (col)
	    {
	    case  0: TmpString.sprintf("%06d", SouInf->idx());
	      break;
	    case  1: TmpString = SouInf->aka().copy();
	      break;
	    case  2: TmpString = SouInf->name().copy();//id
	      break;
	    case  3: TmpString.sprintf("%012d", SouInf->num());
	      break;
	    case  4: TmpString.sprintf("%012d", SouInf->procNum());
	      break;
	    case  5: TmpString.sprintf("%024.16f", SetUp->scale()*SouInf->wrms());
	      break;
	    case  6: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::notValid)?'X':' ');
	      break;
	    case  7: 
	      TmpString = cStatus[Status]; 
	      if (IsSuspected) TmpString += "Z";
	      break;
	    case  8: TmpString = cType[Type];
	      break;
	    case  9: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::EstCoo)?'Y':' ');
	      break;
	    case 10: TmpString.sprintf("%c", SouInf->isAttr(SBSourceInfo::ConstrCoo)?'*':' ');
	      break;
	    };
	};
    }
  return TmpString;
};

//const QPixmap* SBSouInfoLI::pixmap (int col) const
//{
//  QPixmap *pm=0;
//  if (SouInf)
//    switch (col)
//      {
//      case 7:
//	pm = pmSrcStatus[Status];
//	break;
//      default: pm=0;
// 	break;
//      };
//  return pm;
//};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBHistoryLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBHistoryLI::text(int col) const
{
  TmpString = "NULL";
  if (HEntry)
    switch (col)
      {
      case  0: TmpString = SBMJD(HEntry->date()).toString(SBMJD::F_Short).copy();
	break;
      case  1: TmpString.sprintf("%3d", HEntry->version());
	break;
      case  2: TmpString = Type==DBH?"DBH":"LOC";
	break;
      case  3: TmpString.sprintf(" %s", (const char*)HEntry->text());
	break;
      };
  return TmpString;
};

QString SBHistoryLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (HEntry)
    switch (col)
      {
      case  0: TmpString.sprintf("%015.8f", HEntry->date());
	break;
      case  1: TmpString.sprintf("%03d", HEntry->version());
	break;
      case  2: TmpString = Type==DBH?"DBH":"LOC";
	break;
      case  3: TmpString.sprintf("%s", (const char*)HEntry->text());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBClockBreakLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBClockBreakLI::text(int col) const
{
  TmpString = "NULL";
  if (Br)
    switch (col)
      {
      case  0: TmpString = Br->toString(SBMJD::F_YYYYMMDDHHMMSSSS).copy();
	break;
      case  1: TmpString.sprintf("%.2f", (double)(Br->b()));
	break;
      };
  return TmpString;
};

QString SBClockBreakLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (Br)
    switch (col)
      {
      case  0: TmpString.sprintf("%f", (double)(*Br));
	break;
      case  1: TmpString.sprintf("%020.2f", (double)(Br->b()));
	break;
      };
  return TmpString;
};
/*==============================================================================================*/






/*==============================================================================================*/
/*												*/
/* SBBrowseSessions..										*/
/*												*/
/*==============================================================================================*/
SBBrowseSessions::SBBrowseSessions(SBVLBISesInfoList* SIList_, BrowseMode BrwsMode_, SBProject *Project_,
				   QWidget* parent, const char* name, WFlags f)
  : QWidget(parent, name, f)
{
  Project	= Project_;
  SesInfoList	= SIList_;
  BrwsMode	= BrwsMode_;
  Layout	= new QVBoxLayout(this, 20, 10);
  LVSessions	= new SBListView(this);
  
  if (BrwsMode!=BM_PREPROCESS)
    {
      LVSessions -> addColumn("Date");
      LVSessions -> setColumnAlignment( 0, AlignLeft);
      LVSessions -> addColumn("Name");
      LVSessions -> setColumnAlignment( 1, AlignLeft);
      LVSessions -> addColumn("Ver");
      LVSessions -> setColumnAlignment( 2, AlignRight);
      LVSessions -> addColumn("Network");
      LVSessions -> setColumnAlignment( 3, AlignLeft);
      LVSessions -> addColumn("UF");
      LVSessions -> setColumnAlignment( 4, AlignCenter);
      LVSessions -> addColumn("Interval");
      LVSessions -> setColumnAlignment( 5, AlignRight);
      LVSessions -> addColumn("NObs");
      LVSessions -> setColumnAlignment( 6, AlignRight);
      LVSessions -> addColumn("NSta");
      LVSessions -> setColumnAlignment( 7, AlignRight);
      LVSessions -> addColumn("NBas");
      LVSessions -> setColumnAlignment( 8, AlignRight);
      LVSessions -> addColumn("NSou");
      LVSessions -> setColumnAlignment( 9, AlignRight);
      LVSessions -> addColumn("Origin");
      LVSessions -> setColumnAlignment(10, AlignCenter);
      LVSessions -> addColumn("Corr");
      LVSessions -> setColumnAlignment(11, AlignCenter);
      LVSessions -> addColumn("wrms(D)");
      LVSessions -> setColumnAlignment(12, AlignRight);
      LVSessions -> addColumn("wrms(R)");
      LVSessions -> setColumnAlignment(13, AlignRight);

      if (BrwsMode==BM_PROJECT)
	{
	  LVSessions -> addColumn("Fix EOP");
	  LVSessions -> setColumnAlignment(14, AlignCenter);
	}
      else
	{
	  LVSessions -> addColumn("PP?");
	  LVSessions -> setColumnAlignment(14, AlignCenter);
	};
      LVSessions -> addColumn("Trbl");
      LVSessions -> setColumnAlignment(15, AlignRight);
      if (BrwsMode==BM_PROJECT)
	{
	  LVSessions -> addColumn("Omit");
	  LVSessions -> setColumnAlignment(16, AlignCenter);
	};
    }
  else
    {
      LVSessions -> addColumn("Name");
      LVSessions -> setColumnAlignment( 0, AlignLeft);
      LVSessions -> addColumn("NObs");
      LVSessions -> setColumnAlignment( 1, AlignRight);
      LVSessions -> addColumn("wrms(D)");
      LVSessions -> setColumnAlignment( 2, AlignRight);
      LVSessions -> addColumn("Trbl");
      LVSessions -> setColumnAlignment( 3, AlignRight);
      LVSessions -> addColumn("PP?");
      LVSessions -> setColumnAlignment( 4, AlignCenter);
    };

  updateList();

  LVSessions -> setAllColumnsShowFocus(TRUE);
  LVSessions -> setMinimumSize(LVSessions->sizeHint());
  LVSessions -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (LVSessions->firstChild())
    LVSessions -> setSelected(LVSessions->firstChild(), TRUE);
  LVSessions -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(LVSessions, 5);
  Layout -> activate();
};

void SBBrowseSessions::updateList()
{
  NumObs = NumPPSess = 0;
  LVSessions->clear();
  for (SBVLBISesInfo* W=SesInfoList->first(); W; W=SesInfoList->next())
    {
      if (BrwsMode==BM_PREPROCESS)
	(void) new SBVLBISesPreProcLI(LVSessions, W, BrwsMode);
      else if (BrwsMode==BM_PROJECT)
	{
	  if (!Project)
	    {
	      if (W->isAttr(SBVLBISesInfo::preProc)) // ??????????????????? 
		(void) new SBVLBISesInfoLI(LVSessions, W, BrwsMode);
	    }
	  else if (W->isAttr(SBVLBISesInfo::preProc) && !Project->sessionByName(W->name()))
	    (void) new SBVLBISesInfoLI(LVSessions, W, BrwsMode);
	}
      else 
	(void) new SBVLBISesInfoLI(LVSessions, W, BrwsMode);

      if (W->isAttr(SBVLBISesInfo::preProc)) 
	NumPPSess++;
      NumObs+=W->numObs();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseStations..										*/
/*												*/
/*==============================================================================================*/
SBBrowseStations::SBBrowseStations(SBStationInfoList* SIList_, BrowseMode Mode_, const SBMJD& T0_,
				   QWidget* parent, const char* name, WFlags f)
  : QWidget(parent, name, f)
{
  StaInfoList = SIList_;
  Mode	      = Mode_;
  T0	      = T0_;
  QBoxLayout	*Layout = new QVBoxLayout(this, 20, 10);
  
  LVStations = new SBListView(this);
  LVStations -> addColumn("Idx");
  LVStations -> setColumnAlignment( 0, AlignRight);
  LVStations -> addColumn("DBH Name");
  LVStations -> setColumnAlignment( 1, AlignLeft);
  LVStations -> addColumn("Station ID");
  LVStations -> setColumnAlignment( 2, AlignRight);
  LVStations -> addColumn("T.Obs");
  LVStations -> setColumnAlignment( 3, AlignRight);
  LVStations -> addColumn("P.Obs");
  LVStations -> setColumnAlignment( 4, AlignRight);
  LVStations -> addColumn("RMS " + SetUp->scaleName());
  LVStations -> setColumnAlignment( 5, AlignRight);
  LVStations -> addColumn("#B");
  LVStations -> setColumnAlignment( 6, AlignRight);
  LVStations -> addColumn("LC");
  LVStations -> setColumnAlignment( 7, AlignCenter);
  LVStations -> addColumn("LZ");
  LVStations -> setColumnAlignment( 8, AlignCenter);
  LVStations -> addColumn("Ign");
  LVStations -> setColumnAlignment( 9, AlignCenter);
  if (Mode == BM_GENERAL) // it's a session
    {
      LVStations -> addColumn("#C");
      LVStations -> setColumnAlignment(10, AlignRight);
      LVStations -> addColumn("R.Cl.");
      LVStations -> setColumnAlignment(11, AlignCenter);
      LVStations -> addColumn("Trbl");
      LVStations -> setColumnAlignment(12, AlignRight);
      LVStations -> addColumn("Clock0");
      LVStations -> setColumnAlignment(13, AlignRight);
      LVStations -> addColumn("E[C0]");
      LVStations -> setColumnAlignment(14, AlignRight);
      LVStations -> addColumn("Clock1");
      LVStations -> setColumnAlignment(15, AlignRight);
      LVStations -> addColumn("E[C1]");
      LVStations -> setColumnAlignment(16, AlignRight);
      LVStations -> addColumn("Zenith");
      LVStations -> setColumnAlignment(17, AlignRight);
      LVStations -> addColumn("E[Z]");
      LVStations -> setColumnAlignment(18, AlignRight);
    }
  else if (Mode == BM_PROJECT) // it's a project
    {
      LVStations -> addColumn("Est.Coord.");
      LVStations -> setColumnAlignment(10, AlignCenter);
      LVStations -> addColumn("Constr.Coord");
      LVStations -> setColumnAlignment(11, AlignCenter);
      LVStations -> addColumn("Est.Veloc.");
      LVStations -> setColumnAlignment(12, AlignCenter);
      LVStations -> addColumn("Constr.Veloc.");
      LVStations -> setColumnAlignment(13, AlignCenter);
    };

  updateList();
  
  LVStations -> setAllColumnsShowFocus(TRUE);
  LVStations -> setMinimumSize(LVStations->sizeHint());
  LVStations -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (LVStations->firstChild())
    LVStations -> setSelected(LVStations->firstChild(), TRUE);
  LVStations -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(LVStations, 5);
  Layout -> activate();
  connect(LVStations, 
	  SIGNAL(doubleClicked(QListViewItem*)), 
	  SLOT(editEntry(QListViewItem*)));
  connect(LVStations,
	  SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint&, int)), 
	  SLOT(toggleEntryMarkEnable(int, QListViewItem*, const QPoint&, int)));
  connect(LVStations,
	  SIGNAL(moveUponItem(QListViewItem*, int)), 
	  SLOT  (toggleEntryMoveEnable(QListViewItem*, int)));
};

void SBBrowseStations::updateList()
{
  LVStations->clear();
  for (SBStationInfo* E=StaInfoList->first(); E; E=StaInfoList->next())
    (void) new SBStaInfoLI(LVStations, E, Mode);
};

void SBBrowseStations::editEntry(QListViewItem* Item)
{
  emit widgetIsBusy(TRUE);
  SBStaParsEditor *SIEditor =  new SBStaParsEditor(((SBStaInfoLI*)Item)->stationInfo(), T0, this);
  connect (SIEditor, SIGNAL(staInfoModified(bool)), SLOT(stainfoModified(bool)));
  SIEditor->show();
};

void SBBrowseStations::stainfoModified(bool IsModified_)
{
  emit modified(IsModified_);
  emit widgetIsBusy(FALSE);
};

void SBBrowseStations::setStaInfoList(SBStationInfoList* SIList_)
{
  if (SIList_)
    {
      StaInfoList = SIList_;
      updateList();

      //  LVStations -> setMinimumSize(LVStations->sizeHint());
      if (LVStations->firstChild())
	LVStations -> setSelected(LVStations->firstChild(), TRUE);
    };
};

void SBBrowseStations::toggleEntryMarkEnable(int button, QListViewItem* Item, const QPoint&, int n)
{
  if (Item) // may be NULL
    {
      if (Mode == BM_GENERAL)
	{
	  if (n==7 && button==Qt::RightButton) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::UseLocalClocks);
	      Item->repaint();
	      emit modified(TRUE);
	      //emit refClockChanged(((SBStaInfoLI*)Item)->stationInfo());
	    };
	  if (n==8 && button==Qt::RightButton) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::UseLocalZenith);
	      Item->repaint();
	      emit modified(TRUE);
	      //emit refClockChanged(((SBStaInfoLI*)Item)->stationInfo());
	    };
	  /*
	    if (n==9) // explicit number of the column
	    {
	    ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::notValid);
	    Item->repaint();
	    emit modified(TRUE);
	    };
	  */
	  if (n==11 && button==Qt::RightButton) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::refClock);
	      Item->repaint();
	      emit modified(TRUE);
	      emit refClockChanged(((SBStaInfoLI*)Item)->stationInfo());
	    };
	  if (n==12) // explicit number of the column
	    {
	      if (button==Qt::RightButton)
		((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::ArtMeteo);
	      else if (button==Qt::LeftButton)
		((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::InvCable);
	      Item->repaint();
	      emit modified(TRUE);
	      //emit refClockChanged(((SBStaInfoLI*)Item)->stationInfo());
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  if (n==7 && button==Qt::RightButton) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::UseLocalClocks);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==8 && button==Qt::RightButton) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::UseLocalZenith);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  /*
	    if (n==9) // explicit number of the column
	    {
	    ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::notValid);
	    Item->repaint();
	    emit modified(TRUE);
	    };
	    if (n==10) // explicit number of the column
	    {
	    ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::EstCoo);
	    Item->repaint();
	    emit modified(TRUE);
	    };
	    if (n==11) // explicit number of the column
	    {
	    ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::ConstrCoo);
	    Item->repaint();
	    emit modified(TRUE);
	    };
	    if (n==12) // explicit number of the column
	    {
	    ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::EstVel);
	    Item->repaint();
	    emit modified(TRUE);
	    };
	    if (n==13) // explicit number of the column
	    {
	    ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::ConstrVel);
	    Item->repaint();
	    emit modified(TRUE);
	    };
	  */
	};
    };
};

void SBBrowseStations::toggleEntryMoveEnable(QListViewItem* Item, int n)
{
  if (Item) // may be NULL
    {
      if (Mode == BM_GENERAL)
	{
	  if (n==9) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::notValid);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  if (n==9) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::notValid);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==10) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::EstCoo);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==11) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::ConstrCoo);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==12) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::EstVel);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==13) // explicit number of the column
	    {
	      ((SBStaInfoLI*)Item)->stationInfo()->xorAttr(SBStationInfo::ConstrVel);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	};
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseSources..										*/
/*												*/
/*==============================================================================================*/
SBBrowseSources::SBBrowseSources(SBSourceInfoList* SIList_, BrowseMode Mode_, SB_CRF* CRF, 
				 QWidget* parent, const char* name, WFlags f)
  : QWidget(parent, name, f)
{
  SouInfoList = SIList_;
  Mode	      = Mode_;

  QBoxLayout	*Layout = new QVBoxLayout(this, 20, 10);
  //--
  LVSources = new SBListView(this);
  LVSources -> addColumn("Idx");
  LVSources -> setColumnAlignment( 0, AlignRight);
  LVSources -> addColumn("DBH Name");
  LVSources -> setColumnAlignment( 1, AlignLeft);
  LVSources -> addColumn("Source Name");
  LVSources -> setColumnAlignment( 2, AlignRight);
  LVSources -> addColumn("T.Obs");
  LVSources -> setColumnAlignment( 3, AlignRight);
  LVSources -> addColumn("P.Obs");
  LVSources -> setColumnAlignment( 4, AlignRight);
  LVSources -> addColumn("RMS, " + SetUp->scaleName());
  LVSources -> setColumnAlignment( 5, AlignRight);
  LVSources -> addColumn("Ign");
  LVSources -> setColumnAlignment( 6, AlignCenter);

  if (Mode == BM_GENERAL) // it's a session
    {
    }
  else if (Mode == BM_PROJECT) // it's a project
    {
      LVSources -> addColumn("Status");
      LVSources -> setColumnAlignment( 7, AlignCenter);
      LVSources -> addColumn("Type");
      LVSources -> setColumnAlignment( 8, AlignCenter);
      LVSources -> addColumn("Est.");
      LVSources -> setColumnAlignment( 9, AlignCenter);
      LVSources -> addColumn("Constr.");
      LVSources -> setColumnAlignment(10, AlignCenter);
    };

  updateList(CRF);
  
  LVSources -> setAllColumnsShowFocus(TRUE);
  LVSources -> setMinimumSize(LVSources->sizeHint());
  LVSources -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (LVSources->firstChild())
    LVSources -> setSelected(LVSources->firstChild(), TRUE);
  LVSources -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(LVSources, 5);
  Layout -> activate();

  //  connect(LVSources,
  //	  SIGNAL(rightButtonClicked(QListViewItem*, const QPoint&, int)),
  //	  SLOT(toggleEntryMarkEnable(QListViewItem*, const QPoint&, int)));

  connect(LVSources,
	  SIGNAL(moveUponItem(QListViewItem*, int)), 
	  SLOT  (toggleEntryMoveEnable(QListViewItem*, int)));
};

void SBBrowseSources::updateList(SB_CRF* CRF)
{
  LVSources->clear();
  for (SBSourceInfo* E=SouInfoList->first(); E; E=SouInfoList->next())
    {
      if (!CRF)
	(void) new SBSouInfoLI(LVSources, E, Mode);
      else
	{
	  SBSource *src = CRF->find(E->name());
	  if (src)
	    (void) new SBSouInfoLI(LVSources, E, Mode, src->status(), 
				   src->type(), src->isAttr(SBSource::StatusSuspected));
	  else
	    (void) new SBSouInfoLI(LVSources, E, Mode);
	};
    };
};

void SBBrowseSources::setSouInfoList(SBSourceInfoList* SIList_)
{
  SouInfoList = SIList_;
  LVSources->clear();
  
  updateList(NULL);
  
  //  LVSources -> setMinimumSize(LVSources->sizeHint());
  if (LVSources->firstChild())
    LVSources -> setSelected(LVSources->firstChild(), TRUE);
};

void SBBrowseSources::toggleEntryMarkEnable(QListViewItem* Item, const QPoint&, int n)
{
  if (Item) // may be NULL
    {
      if (Mode == BM_GENERAL)
	{
	  if (n==6) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::notValid);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  if (n==6) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::notValid);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==9) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::EstCoo);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==10) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::ConstrCoo);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	};      
    };
};

void SBBrowseSources::toggleEntryMoveEnable(QListViewItem* Item, int n)
{
  if (Item) // may be NULL
    {
      if (Mode == BM_GENERAL)
	{
	  if (n==6) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::notValid);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	}
      else if (Mode == BM_PROJECT)
	{
	  if (n==6) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::notValid);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==9) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::EstCoo);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	  if (n==10) // explicit number of the column
	    {
	      ((SBSouInfoLI*)Item)->sourceInfo()->xorAttr(SBSourceInfo::ConstrCoo);
	      Item->repaint();
	      emit modified(TRUE);
	    };
	};      
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseBases..										*/
/*												*/
/*==============================================================================================*/
SBBrowseBases::SBBrowseBases(SBBaseInfoList* BIList_, 
			     QWidget* parent, const char* name, WFlags f)
  : QWidget(parent, name, f)
{
  BasInfoList = BIList_;
  QBoxLayout	*Layout = new QVBoxLayout(this, 20, 10);
  //--
  LVBases = new QListView(this);
  LVBases -> addColumn("Idx");
  LVBases -> setColumnAlignment( 0, AlignRight);
  LVBases -> addColumn("DBH Name");
  LVBases -> setColumnAlignment( 1, AlignLeft);
  LVBases -> addColumn("Base Name");
  LVBases -> setColumnAlignment( 2, AlignRight);
  LVBases -> addColumn("T.Obs");
  LVBases -> setColumnAlignment( 3, AlignRight);
  LVBases -> addColumn("P.Obs");
  LVBases -> setColumnAlignment( 4, AlignRight);
  LVBases -> addColumn("RMS, " + SetUp->scaleName());
  LVBases -> setColumnAlignment( 5, AlignRight);
  LVBases -> addColumn("Ign");
  LVBases -> setColumnAlignment( 6, AlignCenter);

  updateList();
  
  LVBases -> setAllColumnsShowFocus(TRUE);
  LVBases -> setMinimumSize(LVBases->sizeHint());
  LVBases -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (LVBases->firstChild())
    LVBases -> setSelected(LVBases->firstChild(), TRUE);
  LVBases -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(LVBases, 5);
  Layout -> activate();
  connect(LVBases,
	  SIGNAL(rightButtonClicked(QListViewItem*, const QPoint&, int)), 
	  SLOT(toggleEntryMarkEnable(QListViewItem*, const QPoint&, int)));
};

void SBBrowseBases::updateList()
{
  LVBases->clear();
  
  for (SBBaseInfo* E=BasInfoList->first(); E; E=BasInfoList->next())
    (void) new SBBasInfoLI(LVBases, E);
};

void SBBrowseBases::setBasInfoList(SBBaseInfoList* BIList_)
{
  BasInfoList = BIList_;

  updateList();

  //  LVBases -> setMinimumSize(LVBases->sizeHint());
  if (LVBases->firstChild())
    LVBases -> setSelected(LVBases->firstChild(), TRUE);
};

void SBBrowseBases::toggleEntryMarkEnable(QListViewItem* Item, const QPoint&, int n)
{
  if (Item) // may be NULL
    if (n==6) // explicit number of the column
      {
	((SBBasInfoLI*)Item)->baseInfo()->xorAttr(SBBaseInfo::notValid);
	Item->repaint();
	emit modified(TRUE);
      };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseHistory..										*/
/*												*/
/*==============================================================================================*/
SBBrowseHistory::SBBrowseHistory(SBHistory* HistDBH_, SBHistory* HistLocal_, int V_,
				 QWidget* parent, const char* name, WFlags f)
  : QWidget(parent, name, f)
{
  HistDBH = HistDBH_;
  HistLocal = HistLocal_;
  Version = V_;
  QBoxLayout	*Layout = new QVBoxLayout(this, 20, 10);
  //--
  LVHistory = new QListView(this);
  LVHistory -> addColumn("Date");
  LVHistory -> setColumnAlignment( 0, AlignRight);
  LVHistory -> addColumn("Ver");
  LVHistory -> setColumnAlignment( 1, AlignRight);
  LVHistory -> addColumn("Origin");
  LVHistory -> setColumnAlignment( 2, AlignRight);
  LVHistory -> addColumn("Text");
  LVHistory -> setColumnAlignment( 3, AlignLeft);

  if (HistDBH)
    for (SBDated* E=HistDBH->first(); E; E=HistDBH->next())
      (void) new SBHistoryLI(LVHistory, SBHistoryLI::DBH, (SBHistory::SBHistoryEntry*)E);
  if (HistLocal)
    for (SBDated* E=HistLocal->first(); E; E=HistLocal->next())
      (void) new SBHistoryLI(LVHistory, SBHistoryLI::LOCAL, (SBHistory::SBHistoryEntry*)E);
  
  LVHistory -> setAllColumnsShowFocus(TRUE);
  LVHistory -> setMinimumHeight(LVHistory->sizeHint().height());
  LVHistory -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (LVHistory->firstChild())
    LVHistory -> setSelected(LVHistory->firstChild(), TRUE);
  LVHistory -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(LVHistory, 5);
  //--

  QBoxLayout *SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);

  EHistLine = new QLineEdit(this);
  QLabel *label	= new QLabel(EHistLine, "&Add:", this);
  label	-> setMinimumSize(label->sizeHint());
  EHistLine -> setMinimumHeight(EHistLine->sizeHint().height());
  EHistLine -> setMinimumWidth(10*EHistLine->fontMetrics().width("WWW") + 10);

  SubLayout->addWidget(label);
  SubLayout->addWidget(EHistLine, 10);

  QPushButton *button = new QPushButton("Submit", this);
  button->setMinimumSize(button->sizeHint());
  SubLayout-> addWidget(button);
  connect (button, SIGNAL(clicked()), SLOT(addHistoryLine()));

  button = new QPushButton("Delete", this);
  button->setMinimumSize(button->sizeHint());
  SubLayout-> addWidget(button);
  connect (button, SIGNAL(clicked()), SLOT(deleteHistoryLine()));

  button = new QPushButton("Ver++", this);
  button->setMinimumSize(button->sizeHint());
  SubLayout-> addWidget(button);
  connect (button, SIGNAL(clicked()), SLOT(incVersion()));

  Layout -> activate();
};

void SBBrowseHistory::setHistories(SBHistory* HistDBH_, SBHistory* HistLocal_, int V_)
{
  HistDBH = HistDBH_;
  HistLocal = HistLocal_;
  Version = V_;

  LVHistory->clear();
  
  if (HistDBH)
    for (SBDated* E=HistDBH->first(); E; E=HistDBH->next())
      (void) new SBHistoryLI(LVHistory, SBHistoryLI::DBH, (SBHistory::SBHistoryEntry*)E);
  if (HistLocal)
    for (SBDated* E=HistLocal->first(); E; E=HistLocal->next())
      (void) new SBHistoryLI(LVHistory, SBHistoryLI::LOCAL, (SBHistory::SBHistoryEntry*)E);
  
  //  LVHistory -> setMinimumHeight(LVHistory->sizeHint().height());
  if (LVHistory->firstChild())
    LVHistory -> setSelected(LVHistory->firstChild(), TRUE);
};

void SBBrowseHistory::deleteHistoryLine()
{
  SBHistoryLI* Item = (SBHistoryLI*)LVHistory->currentItem();
  if (Item && Item->type()==SBHistoryLI::LOCAL)
    {
      QListViewItem* NextItem = Item->itemBelow();
      if (!NextItem) NextItem = Item->itemAbove();
      QString Str = SBMJD(Item->hEntry()->date()).toString(SBMJD::F_Short).copy();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete history entry\n "
				"created on \"" + Str + "\"?\n", 
				"Yes, del them!", "No, let it live.."))
	{
	  if (HistLocal->remove(Item->hEntry()))
	    {
	      delete Item;
	      if (NextItem) LVHistory->setSelected(NextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
			 ": \"" + Str + "\", rest in peace..");
	      emit modified(TRUE);
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": \"" + Str + "\" not found in the history lists");
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": relax, \"" + Str + 
		   "\", it's just a joke.. ");
    };
};

void SBBrowseHistory::addHistoryLine()
{
  QString Text_=EHistLine->text();
  if (Text_.simplifyWhiteSpace()!="")
    {
      HistLocal->addHistoryLine(Version, Text_);
      (void) new SBHistoryLI(LVHistory, SBHistoryLI::LOCAL, 
			     (SBHistory::SBHistoryEntry*) HistLocal->last());
      EHistLine->setText("");
      emit modified(TRUE);
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISetViewBase..										*/
/*												*/
/*==============================================================================================*/
SBVLBISetViewBase::SBVLBISetViewBase(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  numOpenedWindows = 0;
  isModified = FALSE;

  if (!(Wvlbi=SetUp->loadVLBI())) Wvlbi = new SBVLBISet;
};

SBVLBISetViewBase::~SBVLBISetViewBase()
{
  if (Wvlbi) delete Wvlbi;
};

QWidget* SBVLBISetViewBase::VLBIList(QWidget* parent, BrowseMode Mode_, SBProject* Prj)
{
  brwsSes = new SBBrowseSessions(Wvlbi, Mode_, Prj, parent);
  connect(brwsSes->lvSessions(), 
	  SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntry(QListViewItem*)));
  connect(brwsSes->lvSessions(), 
	  SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntry(QListViewItem*)));
  return brwsSes;
};

QWidget* SBVLBISetViewBase::unitsGroup(QWidget* parent)
{
  const char* cUnit[]=
  {
    "Delays in ps", 
    "Delays in cm", 
    "Delays in in"
    //    QString("Delays in ps"), 
    //    QString("Delays in cm"), 
    //    QString("Delays in in")
  };
  QButtonGroup *Units = new QButtonGroup("Units", parent);
  QBoxLayout *aLayout = new QVBoxLayout(Units, 20, 5);
  for (int i=0; i<3; i++)
    {
      RBUnit[i]= new QRadioButton(cUnit[i], Units);
      RBUnit[i]->setMinimumSize(RBUnit[i]->sizeHint());
      aLayout->addWidget(RBUnit[i]);
    };
  RBUnit[SetUp->units()]->setChecked(TRUE);
  aLayout->activate();
  connect(Units, SIGNAL(clicked(int)), SLOT(unitsChanged(int)));
  return Units;
};

QWidget* SBVLBISetViewBase::infoGroup(QWidget* parent)
{
  QGroupBox *Info = new QGroupBox("Info", parent);
  QGridLayout *grid = new QGridLayout(Info, 3,2, 20,5);
  QLabel *label= new QLabel("Sessions:", Info);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,0);

  LNumSes= new QLabel(str.sprintf("%07d", Wvlbi->count()), Info);
  LNumSes->setMinimumSize(LNumSes->sizeHint());
  LNumSes->setAlignment(AlignRight | AlignVCenter);
  grid -> addWidget(LNumSes, 0,1);

  label= new QLabel("PreProcessed:", Info);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1,0);

  LNumPPS= new QLabel(str.sprintf("%07d", brwsSes->numPPSess()), Info);
  LNumPPS->setMinimumSize(LNumPPS->sizeHint());
  LNumPPS->setAlignment(AlignRight | AlignVCenter);
  grid -> addWidget(LNumPPS, 1,1);

  label	= new QLabel("Observations:", Info);
  label-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 2,0);

  LNumObs= new QLabel(str.sprintf("%07d", brwsSes->numObs()), Info);
  LNumObs->setMinimumSize(LNumObs->sizeHint());
  LNumObs->setAlignment(AlignRight | AlignVCenter);
  grid -> addWidget(LNumObs, 2,1);
  grid -> activate();
  drawInfo();
  return Info;
};

void SBVLBISetViewBase::unitsChanged(int n)
{
  if (0<=n && n<=2) SetUp->setUnits((SBSetUp::DUnits)n);
};

void SBVLBISetViewBase::accept()
{
  if (numOpenedWindows) return; //there are open windows dependent upon us..
  QDialog::accept();
  if (isModified) SetUp->saveVLBI(Wvlbi);
  delete this;
};

void SBVLBISetViewBase::reject() 
{
  if (numOpenedWindows) return;
  QDialog::reject();
  delete this;
};

void SBVLBISetViewBase::drawInfo()
{
  LNumSes->setText(str.sprintf("%d", Wvlbi->count()));
  LNumSes->setMinimumSize(LNumSes->sizeHint());

  LNumPPS->setText(str.sprintf("%d", brwsSes->numPPSess()));
  LNumPPS->setMinimumSize(LNumPPS->sizeHint());

  LNumObs->setText(str.sprintf("%u", Wvlbi->totalObs()));
  LNumObs->setMinimumSize(LNumObs->sizeHint());
};

void SBVLBISetViewBase::modified(bool isModified_)
{
  isModified |= isModified_;
};
void SBVLBISetViewBase::childWindowClosed()
{
  numOpenedWindows--;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISetView..										*/
/*												*/
/*==============================================================================================*/
SBVLBISetView::SBVLBISetView(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBVLBISetViewBase(parent, name, modal, f)
{
  init();
};

void SBVLBISetView::init()
{
  setCaption("VLBI Dataset");
  
  QGroupBox  *gbox   = new QGroupBox("VLBI Sessions", this);
  QBoxLayout *Layout = new QVBoxLayout(gbox, 20, 10);
  
  Layout->addWidget(VLBIList(gbox, BM_GENERAL, NULL), 5);
  QBoxLayout *SubLayout = new QHBoxLayout(10);
  Layout->addLayout(SubLayout);
  SubLayout->addWidget(unitsGroup(gbox));
  SubLayout->addWidget(optionsGroup(gbox));
  SubLayout->addWidget(infoGroup(gbox));
  Layout->activate();

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  QPushButton *MRecords	= new QPushButton("MRecords", this);
  QPushButton *Networks	= new QPushButton("Networks", this);
  QPushButton *ImportNGS= new QPushButton("Import NGS", this);
  QPushButton *ImportDBH= new QPushButton("Import MkIII", this);
  Delete		= new QPushButton("Delete", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);
  Ok->setDefault(TRUE);

  QSize	BtnSize;
  ImportDBH->setMinimumSize((BtnSize=ImportDBH->sizeHint()));
  Ok->setMinimumSize(BtnSize);
  ImportNGS->setMinimumSize(BtnSize);
  MRecords->setMinimumSize(BtnSize);
  Networks->setMinimumSize(BtnSize);
  Delete->setMinimumSize(BtnSize);
  Cancel->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(MRecords);
  SubLayout->addWidget(Networks);
  SubLayout->addWidget(ImportNGS);
  SubLayout->addWidget(ImportDBH);
  SubLayout->addWidget(Delete);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(MRecords, SIGNAL(clicked()), SLOT(browseMRecords()));
  connect(Networks, SIGNAL(clicked()), SLOT(editNetworks()));
  connect(ImportNGS, SIGNAL(clicked()), SLOT(importNGS()));
  connect(ImportDBH, SIGNAL(clicked()), SLOT(importDBH()));
  connect(Delete, SIGNAL(clicked()), SLOT(deleteEntry()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  //--

  connect(this, SIGNAL(stBarReset()),			MainWin,SLOT(stBarReset()));
  connect(this, SIGNAL(stBarSetTotalSteps(int)),	MainWin,SLOT(stBarSetTotalSteps(int)));
  connect(this, SIGNAL(stBarSetProgress(int)),		MainWin,SLOT(stBarSetProgress(int)));
  connect(this, SIGNAL(message(const QString&)),	MainWin,SLOT(setMessage(const QString&)));

  Layout->activate();
};

QWidget* SBVLBISetView::optionsGroup(QWidget* parent)
{
  QButtonGroup	*Options = new QButtonGroup("Import options", parent);
  QBoxLayout	*Layout  = new QHBoxLayout(Options, 20, 20);

  // radio buttons:
  QButtonGroup* bg = new QButtonGroup("Existed session action", Options);
  QBoxLayout *SubLayout = new QVBoxLayout(bg, 20, 5);

  RBIfExistedAskUser = new QRadioButton("Ask user", bg);
  RBIfExistedAskUser-> setMinimumSize(RBIfExistedAskUser->sizeHint());
  SubLayout->addWidget(RBIfExistedAskUser);

  RBIfExistedDrop = new QRadioButton("Ignore", bg);
  RBIfExistedDrop-> setMinimumSize(RBIfExistedDrop->sizeHint());
  SubLayout->addWidget(RBIfExistedDrop);

  RBIfExistedImport = new QRadioButton("Import anyway", bg);
  RBIfExistedImport-> setMinimumSize(RBIfExistedImport->sizeHint());
  SubLayout->addWidget(RBIfExistedImport);

  RBIfExistedDrop->setChecked(TRUE);
  SubLayout->activate();

  Layout->addWidget(bg);
  //


  QBoxLayout	*aLayout = new QVBoxLayout(Layout);

  CBDump = new QCheckBox ("Make MkIII dump", Options);
  CBDump->setMinimumSize(CBDump->sizeHint());
  aLayout->addWidget(CBDump);
  //  aLayout->addStretch(1);


  Layout->activate();
  return Options;
};

void SBVLBISetView::networksModified(bool isModified_, SBVLBISet::NetList* Nets_)
{
  numOpenedWindows--;
  if (isModified_) 
    {
      *Wvlbi->networks() = *Nets_;
      isModified = isModified_;
    }
  if (Nets_) delete Nets_;
};
  
// shunt:
void SBVLBISetView::masterRecordModified(bool isModified_)
{
  numOpenedWindows--;
  if (isModified_) 
    {
      isModified = isModified_;
    };
};
  
void SBVLBISetView::editEntry(QListViewItem* SesItem)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBVLBISetViewEdit selected");
  SBVLBISessionEditor *SE = new SBVLBISessionEditor((SBVLBISesInfoLI*)SesItem, Wvlbi);
  connect (SE, SIGNAL(sessionModified(bool)), SLOT(modified(bool)));
  connect (SE, SIGNAL(windowClosed()), SLOT(childWindowClosed()));
  numOpenedWindows++;
  SE->show();
};
  
void SBVLBISetView::deleteEntry()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationDeleteSite selected");
  
  bool IsBlockedSignals = Delete->signalsBlocked();
  Delete->blockSignals(TRUE);
  SBVLBISesInfoLI* Item = (SBVLBISesInfoLI*)brwsSes->lvSessions()->currentItem();
  if (Item)
    {
      QListViewItem* NextItem = Item->itemBelow();
      if (!NextItem) NextItem = Item->itemAbove();
      str = Item->sessionInfo()->name().copy();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete session entry\n \""
	 			+ str + "\"?\n", "Yes, del them!", "No, let it live.."))
 	{
	  delete Item;
	  Item = NULL;
 	  Wvlbi->removeSession(str);
	  if (NextItem) brwsSes->lvSessions()->setSelected(NextItem, TRUE);
	  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": " + str + ", rest in peace..");
	  isModified = TRUE;
	  drawInfo();
 	}
      else
 	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": relax, " + str + 
 		   ", it's just a joke.. ");
    };
  Delete->blockSignals(IsBlockedSignals);
};

void SBVLBISetView::import(SBVLBISession::ObsImportedFrom IType)
{
  if (numOpenedWindows) return; //there are opened windows dependent upon us..

  bool		IsNeedDump = CBDump->isChecked();
  int		ExistedSessMode;
  bool		IsLoaded   = FALSE;
  QFileInfo	FInfo;

  ExistedSessMode = 0;
  if (RBIfExistedImport->isChecked())
    ExistedSessMode = -1;
  else if (RBIfExistedDrop->isChecked())
    ExistedSessMode = 1;

  Wvlbi->loadMaster();
  qApp->processEvents();
  QStringList sl(QFileDialog::getOpenFileNames(0, SetUp->path2Compil()));
  numOpenedWindows++;
  if (sl.count())
    {
      emit stBarReset();
      emit stBarSetTotalSteps(sl.count());
      int i=1;
      for (QStringList::Iterator it=sl.begin(); it != sl.end(); ++it)
	{ 
	  bool isIgnore=FALSE;
	  emit message(QString("Processing: ") + *it);
	  qApp->processEvents();
	  FInfo.setFile(*it);
	  
	  if (FInfo.size()<100)
	    {
	      Log->write(SBLog::DBG, SBLog::INTERFACE | SBLog::IO | SBLog::DATA, 
			 ClassName() + ": file [" + *it + "] is suspiciously small; ignored");
	      isIgnore=TRUE;
	    };
	  if (FInfo.extension(FALSE)=="bad")
	    {
	      Log->write(SBLog::DBG, SBLog::INTERFACE | SBLog::IO | SBLog::DATA, 
			 ClassName() + ": file [" + *it + "] has \".bad\" extension; ignored");
	      isIgnore=TRUE;
	    };
	  if (FInfo.isSymLink())
	    {
	      FInfo.setFile(FInfo.readLink());
	      if (FInfo.extension(FALSE)=="bad")
		{
		  Log->write(SBLog::DBG, SBLog::INTERFACE | SBLog::IO | SBLog::DATA, 
			     ClassName() + ": file [" + *it + 
			     "] points to the file with \".bad\" extension; ignored");
		  isIgnore=TRUE;
		};
	    };
	  if (!isIgnore && Wvlbi->import(IType, *it, IsNeedDump, ExistedSessMode))
	    {
	      IsLoaded = TRUE;
	      drawInfo();
	    };
	  emit stBarSetProgress(i++);
	};
      drawInfo();
      emit stBarReset();
      qApp->processEvents();
    }
  Wvlbi->releaseMaster();
  if (IsLoaded)
    {
      isModified = TRUE;
      brwsSes->updateList();
    };
  numOpenedWindows--;
};

void SBVLBISetView::editNetworks()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBVLBISetNetEdit selected");
  SBVLBINetworkEditor *NE = new SBVLBINetworkEditor(Wvlbi);
  connect (NE, SIGNAL(networksModified(bool, SBVLBISet::NetList*)), 
	   SLOT(networksModified(bool, SBVLBISet::NetList*)));
  numOpenedWindows++;
  NE->show();
};

void SBVLBISetView::browseMRecords()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBVLBISetMRecBrws selected");
  SBMasterRecBrowser *MRB = new SBMasterRecBrowser(Wvlbi);
  connect (MRB, SIGNAL(masterRecordModified(bool)), SLOT(masterRecordModified(bool)));
  numOpenedWindows++;
  MRB->show();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISessionEditor..									*/
/*												*/
/*==============================================================================================*/
SBVLBISessionEditor::SBVLBISessionEditor(SBVLBISesInfoLI *SIItem_, SBVLBISet* Wvlbi_, 
					 QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  SIItem	= SIItem_;
  SI		= SIItem->sessionInfo();
  Wvlbi		= Wvlbi_;
  isModified	= FALSE;

  switch (SetUp->units())
    {
    default:
    case SBSetUp::U_SEC: 
      scale = 1.0e+12;
      ScaleName = "ps";
      break;
    case SBSetUp::U_METER: 
      scale = 100.0*SBDelay::CLight;
      ScaleName = "cm";
      break;
    case SBSetUp::U_INCH: 
      scale = 25.4*SBDelay::CLight;
      ScaleName = "in";
      break;
    };


  if (!(Sess=Wvlbi->loadSession(SI->name()))) // just to explain before geting SIGSEGV ;)
    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + ": got an empty session"); 
  
  // plots for bases:
  PC_Bases = new SBPlotCarrier(7, 4, "Plots for the session " + Sess->name() + ", splitted by bases");
  PC_Bases->setFile4SaveBaseName(Sess->name() + "_Bases");
  // split data by bases:
  if (Sess->baseList())
    for (SBBaseInfo* E=Sess->baseList()->first(); E; E=Sess->baseList()->next())
      if (!E->isAttr(SBBaseInfo::notValid))
	PC_Bases->createBranch(E->num(), E->aka()); 
  // names of columns:
  PC_Bases->columnNames()->append(new QString("Time of observation, MJD"));		// 0
  PC_Bases->setType(0, SBPlotCarrier::AT_MJD);
  PC_Bases->columnNames()->append(new QString("Time Delay (O-C), " + ScaleName));	// 1
  PC_Bases->columnNames()->append(new QString("Observed Delay, " + ScaleName));		// 2
  PC_Bases->columnNames()->append(new QString("Delay Rate (O-C), ps/sec"));		// 3
  PC_Bases->columnNames()->append(new QString("Observed Rate, ps/sec"));		// 4
  PC_Bases->columnNames()->append(new QString("Ionosphere Delay, " + ScaleName));	// 5
  PC_Bases->columnNames()->append(new QString("Ionosphere Rate, ps/sec"));		// 6
  
  // std vars:
  PC_Bases->columnNames()->append(new QString("Delay sigma, " + ScaleName));		// 7
  PC_Bases->columnNames()->append(new QString("Rate sigma, ps/sec"));			// 8
  PC_Bases->columnNames()->append(new QString("Ion. delay sigma, " + ScaleName));	// 9
  PC_Bases->columnNames()->append(new QString("Ion. rate sigma, ps/sec"));		//10
  
  PC_Bases->setStdVarIdx( 1,  7);
  PC_Bases->setStdVarIdx( 2,  7);
  PC_Bases->setStdVarIdx( 3,  8);
  PC_Bases->setStdVarIdx( 4,  8);
  PC_Bases->setStdVarIdx( 5,  9);
  PC_Bases->setStdVarIdx( 6, 10);
  
  //
  // plots for stations:
  PC_Stations = new SBPlotCarrier( 6, 0, "Plots for the session " + Sess->name() + ", splitted by stations");
  PC_Stations->setFile4SaveBaseName(Sess->name() + "_Stations");
  // split data by stations:
  if (Sess->stationList())
    for (SBStationInfo* E=Sess->stationList()->first(); E; E=Sess->stationList()->next())
      if (!E->isAttr(SBStationInfo::notValid))
	PC_Stations->createBranch(E->num(), E->aka()); 
  // names of columns:
  PC_Stations->columnNames()->append(new QString("Time of observation, MJD"));		// 0
  PC_Stations->setType(0, SBPlotCarrier::AT_MJD);
  PC_Stations->columnNames()->append(new QString("Cable calibration, " + ScaleName));	// 1
  PC_Stations->columnNames()->append(new QString("Metheo: temperature, C"));		// 2
  PC_Stations->columnNames()->append(new QString("Metheo: pressure, mb"));		// 3
  PC_Stations->columnNames()->append(new QString("Metheo: rel. humidity, %"));		// 4
  PC_Stations->columnNames()->append(new QString("Total ZDelay (th+est), " + ScaleName));// 5
  
  // std vars:
  // eof plots  

  
  setCaption("VLBI Session: " + Sess->name() + " (Ver: " + Str.setNum(Sess->version()) + ")");
  
  addTab(wObservs(), "Observations");
  addTab(wDataPlotsByBases(), "Plots (Bases)");
  addTab(wDataPlotsByStations(), "Plots (Stations)");
  addTab(brwsSta = new SBBrowseStations(Sess->stationList(), BM_GENERAL, Sess->tStart()), "Stations");
  addTab(brwsBas = new SBBrowseBases(Sess->baseList()), "Bases");
  addTab(brwsSou = new SBBrowseSources(Sess->sourceList(), BM_GENERAL, NULL, this), "Sources");
  addTab(wParameters(), "Attributes");
  addTab(brwsHist = new SBBrowseHistory(Sess->dbhHistory(), Sess->localHistory(), 
					Sess->localVersion()), "History");
  
  setCancelButton();
  setApplyButton();
  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
  connect(brwsHist, SIGNAL(modified(bool)), SLOT(histModified(bool)));
  connect(brwsHist, SIGNAL(versionInced()), SLOT(histVerInced()));

  connect(ObservLV,
	  SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint&, int)), 
	  SLOT(toggleEntryMarkEnable(int, QListViewItem*, const QPoint&, int)));

  connect(brwsSta, SIGNAL(modified(bool)), SLOT(modified(bool)));
  connect(brwsSta, SIGNAL(refClockChanged(SBStationInfo*)), SLOT(refClockChange(SBStationInfo*)));
  connect(brwsBas, SIGNAL(modified(bool)), SLOT(modified(bool)));
  connect(brwsSou, SIGNAL(modified(bool)), SLOT(modified(bool)));
};

SBVLBISessionEditor::~SBVLBISessionEditor()
{
  if (PC_Bases)
    {
      delete PC_Bases;
      PC_Bases=NULL;
    };

  if (PC_Stations)
    {
      delete PC_Stations;
      PC_Stations=NULL;
    };

  Sess->checkAttributres();
  SI->setAttributes(Sess->attributes());

  if (Sess && SI && isModified)
    {
      *SI=*Sess;
      Wvlbi->saveSession(Sess);
      SIItem->repaint();
    };
  if (Sess) delete Sess;
  emit sessionModified(isModified);
  emit windowClosed();
};
 
void SBVLBISessionEditor::makeApply()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

QWidget* SBVLBISessionEditor::wObservs()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);
  
  ObservLV = new QListView(W);
  ObservLV -> addColumn("Num");
  ObservLV -> setColumnAlignment( 0, AlignRight);
  ObservLV -> addColumn("Time");
  ObservLV -> setColumnAlignment( 1, AlignRight);
  ObservLV -> addColumn("Station#1");
  ObservLV -> setColumnAlignment( 2, AlignLeft);
  ObservLV -> addColumn("Station#2");
  ObservLV -> setColumnAlignment( 3, AlignLeft);
  ObservLV -> addColumn("Source");
  ObservLV -> setColumnAlignment( 4, AlignLeft);
  ObservLV -> addColumn("Delay err");
  ObservLV -> setColumnAlignment( 5, AlignRight);
  ObservLV -> addColumn("Delay (O-C)");
  ObservLV -> setColumnAlignment( 6, AlignRight);
  ObservLV -> addColumn("Rate err");
  ObservLV -> setColumnAlignment( 7, AlignRight);
  ObservLV -> addColumn("Rate (O-C)");
  ObservLV -> setColumnAlignment( 8, AlignRight);
  ObservLV -> addColumn("Ambig");
  ObservLV -> setColumnAlignment( 9, AlignRight);
  ObservLV -> addColumn("QCode");
  ObservLV -> setColumnAlignment(10, AlignCenter);
  ObservLV -> addColumn("UFs");
  ObservLV -> setColumnAlignment(11, AlignCenter);
  ObservLV -> addColumn("IonQ");
  ObservLV -> setColumnAlignment(12, AlignRight);
  ObservLV -> addColumn("?_1");
  ObservLV -> setColumnAlignment(13, AlignRight);
  ObservLV -> addColumn("?_2");
  ObservLV -> setColumnAlignment(14, AlignRight);
  ObservLV -> addColumn("Br_1");
  ObservLV -> setColumnAlignment(15, AlignCenter);
  ObservLV -> addColumn("Br_2");
  ObservLV -> setColumnAlignment(16, AlignCenter);
  ObservLV -> addColumn(" X ");
  ObservLV -> setColumnAlignment(17, AlignCenter);

  SBObsVLBIEntry	*E;
  int			i;
  for (i=0, E=Sess->first(); E; i++, E=Sess->next())
    {
      (void) new SBVLBIObsLI(ObservLV, Sess, E);
      
      SBStationInfo	*St1=Sess->stationInfo(E->station_1Idx());
      SBStationInfo	*St2=Sess->stationInfo(E->station_2Idx());
      SBPlotBranch	*pb = NULL;
      SBBaseInfo	*bi = NULL;
      if (St1 && St2)
	{
	  // fill plot carrier for bases:
	  if ((bi=Sess->baseInfo(St1->aka() + ":" + St2->aka())) && !bi->isAttr(SBBaseInfo::notValid))
	    if ((pb=PC_Bases->findBranch(bi->aka())))
	      {
		pb->data()->set(pb->Idx,  0,	    E->mjd());		//
		pb->data()->set(pb->Idx,  1,  scale*E->delayRes());	//
		pb->data()->set(pb->Idx,  2,  scale*E->delay());	//
		pb->data()->set(pb->Idx,  3, 1.0e12*E->rateRes());	// sec->ps
		pb->data()->set(pb->Idx,  4, 1.0e12*E->rate());		// sec->ps
		pb->data()->set(pb->Idx,  5,  scale*E->delayIon());	//
		pb->data()->set(pb->Idx,  6, 1.0e12*E->rateIon());	// sec->ps
		// std variations:
		pb->data()->set(pb->Idx,  7,  scale*E->delayErr());	//
		pb->data()->set(pb->Idx,  8, 1.0e12*E->rateErr());	// sec->ps
		pb->data()->set(pb->Idx,  9,  scale*E->delayIonErr());	//
		pb->data()->set(pb->Idx, 10, 1.0e12*E->rateIonErr());	// sec->ps
		if (! E->isAttr(SBObsVLBIEntry::processed))
		  pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_IGN);
		if (E->isAttr(SBObsVLBIEntry::breakClock1))
		  pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_BAR);
		if (E->isAttr(SBObsVLBIEntry::breakClock2))
		  pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_BAR);
		pb->Idx++;
	      }
	    else // pb == NULL
	      Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			 ": cannot find plot branch for base [" + bi->aka() + "]");
	  else if (!bi) // baseInfo failed
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": cannot find base [" + St1->aka() + ":" + St2->aka() + "] for the session " + 
		       Sess->name());

	  // fill plot carrier for stations:
	  if (!St1->isAttr(SBStationInfo::notValid))
	    {
	      if ((pb=PC_Stations->findBranch(St1->aka())))
		{
		  pb->data()->set(pb->Idx,  0,	    E->mjd());		//
		  pb->data()->set(pb->Idx,  1,  scale*E->cable_1());	//
		  pb->data()->set(pb->Idx,  2,        E->atmTemp_1());	//
		  pb->data()->set(pb->Idx,  3,        E->atmPress_1());	//
		  pb->data()->set(pb->Idx,  4,        E->atmHum_1());	//
		  pb->data()->set(pb->Idx,  5,  scale*E->zenithDelay_1());//
		  if (! E->isAttr(SBObsVLBIEntry::processed))
		    pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_IGN);
		  // std variations:
		  pb->Idx++;
		}
	      else // pb == NULL
		Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			   ": cannot find plot branch for the station [" + St1->aka() + "]");
	    }
	  
	  if (!St2->isAttr(SBStationInfo::notValid))
	    {
	      if ((pb=PC_Stations->findBranch(St2->aka())))
		{
		  pb->data()->set(pb->Idx,  0,	    E->mjd());		//
		  pb->data()->set(pb->Idx,  1,  scale*E->cable_2());	//
		  pb->data()->set(pb->Idx,  2,        E->atmTemp_2());	//
		  pb->data()->set(pb->Idx,  3,        E->atmPress_2());	//
		  pb->data()->set(pb->Idx,  4,        E->atmHum_2());	//
		  pb->data()->set(pb->Idx,  5,  scale*E->zenithDelay_2());//
		  if (! E->isAttr(SBObsVLBIEntry::processed))
		    pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_IGN);
		  // std variations:
		  pb->Idx++;
		}
	      else // pb == NULL
		Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			   ": cannot find plot branch for the station [" + St2->aka() + "]");
	    }
	  
	}
      else
	Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		   ": cannot find plot branch: St1/St2 is/are NULL"); 
    };
  
  ObservLV -> setAllColumnsShowFocus(TRUE);
  ObservLV -> setMinimumSize(ObservLV->sizeHint());
  ObservLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (ObservLV->firstChild())
    ObservLV -> setSelected(ObservLV->firstChild(), TRUE);
  ObservLV -> setFocusPolicy(QWidget::StrongFocus);
  Layout   -> addWidget(ObservLV,5);
  
  Layout -> activate();
  return W;
};

QWidget* SBVLBISessionEditor::wParameters()
{
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);
  QLabel	*label;
  QGridLayout	*grid;
  QGroupBox	*gbox;
  QBoxLayout	*subLayout;
  int		i;

  subLayout = new QVBoxLayout;
  Layout -> addLayout(subLayout);

  // attributes:
  gbox = new QGroupBox("Session's attributes", W);
  grid = new QGridLayout(gbox, 9,2, 20, 10);
  
  label	= new QLabel("DBH File Name:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,0);
  label	= new QLabel(Sess->name(), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,1);

  label	= new QLabel("Official Name/Code:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 1,0);
  label	= new QLabel(Sess->officialName() + "/" + Sess->code(), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 1,1);

  label	= new QLabel("Correlator Name:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 2,0);
  label	= new QLabel(Sess->correlatorName(), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 2,1);

  label	= new QLabel("Network ID:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 3,0);

  NetID = new QComboBox(FALSE, gbox);
  NetID -> setInsertionPolicy(QComboBox::AtBottom);
  i = 1;
  NetID->insertItem("UNDEF");
  if (Wvlbi->networks()->count())
    for (SBVLBISet::Network *n=Wvlbi->networks()->first(); n; n=Wvlbi->networks()->next(), i++)
      {
	NetID->insertItem(n->name());
	if (n->name()==Sess->networkID()) NetID->setCurrentItem(i);
      }
  else 
    NetID->insertItem("N/A");
  NetID	-> setMinimumSize(NetID->sizeHint());
  grid -> addWidget(NetID, 3,1);

  label	= new QLabel("Epoch of the first observation:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4,0);
  label	= new QLabel(Sess->tStart().toString(SBMJD::F_Short), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4,1);

  label	= new QLabel("Epoch of the last observation:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 5,0);
  label	= new QLabel(Sess->tFinis().toString(SBMJD::F_Short), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 5,1);

  label	= new QLabel("Mean epoch of observations:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 6,0);
  label	= new QLabel(Sess->tMean().toString(SBMJD::F_Short), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 6,1);

  label	= new QLabel("Version created on:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 7,0);
  label	= new QLabel(Sess->dateCreat().toString(SBMJD::F_Short), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 7,1);

  label	= new QLabel("Experiment description:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 8,0);
  Str = Sess->expDescr().stripWhiteSpace();
  label	= new QLabel(Str!=""?Str:QString(" "), gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 8,1);

  grid -> activate();
  subLayout -> addWidget(gbox);


  // parameters:
  gbox = new QGroupBox("Session's parameters", W);
  grid = new QGridLayout(gbox, 3,3, 20, 10);

  label	= new QLabel("User Flag:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,0);

  UFlag = new QComboBox(FALSE, gbox);
  UFlag -> setInsertionPolicy(QComboBox::AtBottom);

  const char* sUFlags[] = 
  {
    "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"
  };
  for (i=0; i< (int)(sizeof(sUFlags)/sizeof(const char*)); i++)
    {
      UFlag->insertItem(sUFlags[i]);
      if (sUFlags[i] == Sess->userFlag()) 
	UFlag->setCurrentItem(i);
    };

  UFlag	-> setMinimumSize(UFlag->sizeHint());
  grid -> addWidget(UFlag, 0,1);


  GrAmbig = new QLineEdit(gbox);
  label	= new QLabel(GrAmbig, "&Group Ambiguity spacing:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  GrAmbig -> setText(Str.sprintf("%7.3f", 1.0e9*Sess->grDelayAmbig()));
  GrAmbig -> setMinimumHeight(GrAmbig->sizeHint().height());
  GrAmbig -> setMinimumWidth(GrAmbig->fontMetrics().width(Str)+10);
  grid -> addWidget(label, 1,0);
  grid -> addWidget(GrAmbig, 1,1);

  label	= new QLabel("(nsec)", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 1,2);

  RefFreq = new QLineEdit(gbox);
  label	= new QLabel(RefFreq, "&Reference Frequency:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  RefFreq -> setText(Str.sprintf("%8.3f", Sess->refFreq()));
  RefFreq -> setMinimumHeight(RefFreq->sizeHint().height());
  RefFreq -> setMinimumWidth(RefFreq->fontMetrics().width(Str)+10);
  grid -> addWidget(label, 2,0);
  grid -> addWidget(RefFreq, 2,1);

  label	= new QLabel("(MHz)", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 2,2);
  grid -> activate();

  subLayout -> addWidget(gbox);
  subLayout -> addStretch(1);

  Layout -> addStretch(5);
  Layout -> activate();
  return W;
};

QWidget* SBVLBISessionEditor::wDataPlotsByBases()
{
  QWidget	*W	= new QWidget(this);
  SBPlot	*Plot   = new SBPlot(PC_Bases, W);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);

  Layout->addWidget(Plot, 5);
  Layout->activate();

  return W;
};

QWidget* SBVLBISessionEditor::wDataPlotsByStations()
{
  QWidget	*W	= new QWidget(this);
  SBPlot	*Plot   = new SBPlot(PC_Stations, W);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);

  Layout->addWidget(Plot, 5);
  Layout->activate();

  return W;
};

void SBVLBISessionEditor::acquireData()
{
  if (Wvlbi->networks()->count() && Sess->networkID() != (Str=NetID->currentText()))
    {
      Sess->setNetworkID(Str);
      isModified = TRUE;
    };
  if (Sess->userFlag() != (Str=UFlag->currentText()))
    {
      Sess->setUserFlag(Str);
      isModified = TRUE;
    };
};

void SBVLBISessionEditor::toggleEntryMarkEnable(int button, QListViewItem* Item, const QPoint&, int n)
{
  if (Item) // may be NULL
    {
      SBObsVLBIEntry	*Obs = ((SBVLBIObsLI*)Item)->obs();
      bool		IsNeedUpdate = TRUE;

      switch (n)
	{
	case  9:
	  if (button==Qt::RightButton)
	    Obs->incAmbigFactor();
	  else if (button==Qt::LeftButton)
	    Obs->decAmbigFactor();
	  break;
	case 15: 
	  ((SBVLBIObsLI*)Item)->obs()->xorAttr(SBObsVLBIEntry::breakClock1);
	  break;
	case 16: 
	  ((SBVLBIObsLI*)Item)->obs()->xorAttr(SBObsVLBIEntry::breakClock2);
	  break;
	case 17: 
	  ((SBVLBIObsLI*)Item)->obs()->xorAttr(SBObsVLBIEntry::notValid);
	  break;
	default: 
	  IsNeedUpdate = FALSE;
	  break;
	};

      if (IsNeedUpdate)
	{
	  Item->repaint();
	  isModified = TRUE;
	};
    };
};

void SBVLBISessionEditor::modified(bool isModified_)
{
  isModified |= isModified_;
};

void SBVLBISessionEditor::refClockChange(SBStationInfo*)
{
  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISetView::NetworkListItem implementation..						*/
/*												*/
/*==============================================================================================*/
QString SBVLBINetworkEditor::NetworkListItem::text(int col) const
{
  TmpString = "NULL";
  if (Network)
    switch (col)
      {
      case  0: TmpString = Network->name().copy();
	break;
      case  1: TmpString = Network->descr().copy();
	break;
      case  2:
	TmpString = " ";
	if (QList<QString>* keys=Network->keys())
	  for (QString *s=keys->first(); s; s=keys->next()) TmpString += s->copy()+",";
	TmpString=TmpString.left(TmpString.length()-1);
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISetView implementation..								*/
/*												*/
/*==============================================================================================*/
SBVLBINetworkEditor::SBVLBINetworkEditor(SBVLBISet* W_, QWidget* parent, 
					 const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  numOpenedWindows	= 0;
  isModified		= FALSE;
  Wvlbi			= W_;
  Nets			= new SBVLBISet::NetList;
  Nets->setAutoDelete(TRUE);
  *Nets			= *Wvlbi->networks();
  
  setCaption("VLBI Networks");
  
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;

  gbox	 = new QGroupBox("Networks/experiments", this);
  Layout = new QVBoxLayout(gbox, 20, 10);

  NetsLV = new QListView(gbox);
  NetsLV -> addColumn("Network ID");
  NetsLV -> setColumnAlignment( 0, AlignLeft);
  NetsLV -> addColumn("Network/Experiments description");
  NetsLV -> setColumnAlignment( 1, AlignLeft);
  NetsLV -> addColumn("Keys");
  NetsLV -> setColumnAlignment( 2, AlignLeft);


  for (SBVLBISet::Network* N=Nets->first(); N; N=Nets->next())
    (void) new NetworkListItem(NetsLV, N);
  
  NetsLV -> setAllColumnsShowFocus(TRUE);
  NetsLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (NetsLV->firstChild())
    NetsLV -> setSelected(NetsLV->firstChild(), TRUE);

  NetsLV -> setFocusPolicy(QWidget::StrongFocus);

  NetsLV -> setColumnWidthMode( 2, QListView::Manual);
  NetsLV -> setColumnWidth    ( 2, 2*NetsLV->columnWidth(1));
  NetsLV -> setMinimumHeight  (NetsLV->sizeHint().height());


  Layout -> addWidget(NetsLV,5);
  Layout -> activate();
  connect (NetsLV, SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntry()));
  connect (NetsLV, SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntry()));

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  Ok->setDefault(TRUE);
  QPushButton *New	= new QPushButton("New", this);
  QPushButton *Edit	= new QPushButton("Edit", this);
  QPushButton *Delete	= new QPushButton("Delete", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);

  QSize	BtnSize;
  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok->setMinimumSize(BtnSize);
  New->setMinimumSize(BtnSize);
  Edit->setMinimumSize(BtnSize);
  Delete->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(New);
  SubLayout->addWidget(Edit);
  SubLayout->addWidget(Delete);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(New, SIGNAL(clicked()), SLOT(addEntry()));
  connect(Edit, SIGNAL(clicked()), SLOT(editEntry()));
  connect(Delete, SIGNAL(clicked()), SLOT(deleteEntry()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  Layout->activate();
};

SBVLBINetworkEditor::~SBVLBINetworkEditor()
{
  emit networksModified(isModified, Nets);
};

void SBVLBINetworkEditor::accept()
{
  if (numOpenedWindows) return; //there are open windows dependent upon us..
  QDialog::accept();
  delete this;
};

void SBVLBINetworkEditor::reject() 
{
  if (numOpenedWindows) return;
  QDialog::reject();
  delete this;
};

void SBVLBINetworkEditor::addEntry()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBVLBISetNetEntryNew selected");
  SBVLBINetEntryEditor *NE = 
    new SBVLBINetEntryEditor(new SBVLBISet::Network, NULL);
  connect (NE, SIGNAL(networkModified(bool, SBVLBISet::Network*)), 
	   SLOT(entryModified(bool, SBVLBISet::Network*)));
  numOpenedWindows++;
  NE->show();
};

void SBVLBINetworkEditor::editEntry()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBVLBISetNetEntryEdit selected");
  SBVLBINetEntryEditor *NE = 
    new SBVLBINetEntryEditor(((NetworkListItem*)NetsLV->currentItem())->network(), 
			     NetsLV->currentItem());
  connect (NE, SIGNAL(networkModified(bool, SBVLBISet::Network*)), 
	   SLOT(entryModified(bool, SBVLBISet::Network*)));
  numOpenedWindows++;
  NE->show();
};

void SBVLBINetworkEditor::deleteEntry()
{
  QListViewItem *ni, *w;
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBVLBISetNetEntryDelete selected");
  if (NetsLV->childCount() &&
      !QMessageBox::warning(this, "Delete?", "Are you sure to delete entry\n \"" +
			    ((NetworkListItem*)(ni=NetsLV->currentItem()))->network()->name()
			    + "\"?\n", "Yes, del them!", "No, let it live.."))
    {
      if (!(w=ni->itemBelow())) w=ni->itemAbove();
      int i=Nets->find(((NetworkListItem*)ni)->network());
      delete ni;
      if (w) NetsLV->setSelected(w, TRUE);
      if (i!=-1) 
	{
	  Nets->remove();
	  isModified = TRUE;
	};
    };
};

void SBVLBINetworkEditor::entryModified(bool isModified_, SBVLBISet::Network* N_)
{
  int i;
  numOpenedWindows--;
  if (isModified_) 
    {
      isModified = isModified_;
      if ((i=Nets->find(N_))!=-1) *Nets->at(i)=*N_;
      else 
	{
	  SBVLBISet::Network *n;
	  Nets->inSort((n = new SBVLBISet::Network(*N_)));
	  (void) new NetworkListItem(NetsLV, n);
	};
    };
  delete N_;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBINetEntryEditor implementation..							*/
/*												*/
/*==============================================================================================*/
SBVLBINetEntryEditor::SBVLBINetEntryEditor(SBVLBISet::Network* N_, QListViewItem *LI_, 
					   QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  isModified	= FALSE;
  LI		= LI_;
  Network	= new SBVLBISet::Network;
  *Network	= *N_;

  setCaption("Network/Experiments");
  
  QPushButton	*AddButton, *ModifyButton, *DelButton, *OKButton, *CancelButton;
  QSize		Size;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout, *aLayout, *bLayout;
  QLabel	*label;
  
  Layout = new QVBoxLayout(this, 20, 10);
  SubLayout = new QHBoxLayout;
  Layout -> addLayout(SubLayout,2);

  gbox	 = new QGroupBox("Network", this);
  aLayout = new QVBoxLayout(gbox, 20, 10);
  bLayout = new QHBoxLayout;
  aLayout -> addLayout(bLayout);

  ID = new QLineEdit(gbox);
  label	= new QLabel(ID, "Network &ID:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  ID -> setText(Network->name());
  ID -> setMinimumHeight(ID->sizeHint().height());
  ID -> setMinimumWidth(ID->fontMetrics().width(Network->name())+10);
  bLayout->addWidget(label);
  bLayout->addWidget(ID, 4);

  Descr = new QMultiLineEdit(gbox);
  label	= new QLabel(Descr, "&Description:", gbox);
  Descr -> setText(Network->descr());
  label	-> setMinimumSize(label->sizeHint());
  //  Descr -> setMinimumSize(Descr->sizeHint());
  aLayout->addWidget(label);
  aLayout->addWidget(Descr, 10);

  aLayout -> activate();
  SubLayout->addWidget(gbox);


  gbox	 = new QGroupBox("Keys", this);
  aLayout = new QVBoxLayout(gbox, 20, 10);

  KeyList = new QListBox(gbox);
  for (QString *a=Network->keys()->first(); a; a=Network->keys()->next())
    KeyList->insertItem(a->data());
  connect (KeyList, SIGNAL(highlighted(int)), SLOT(showKey(int)));
  aLayout->addWidget(KeyList, 10);

  bLayout = new QHBoxLayout;
  aLayout -> addLayout(bLayout);
  Key = new QLineEdit(gbox);
  Key -> setText("QQ");
  label	= new QLabel(Key, "Add &Key:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  Key -> setMinimumSize(Key->sizeHint());
  bLayout->addWidget(label);
  bLayout->addWidget(Key, 4);
  
  bLayout = new QHBoxLayout;
  aLayout -> addLayout(bLayout);

  bLayout->addStretch(1);
  bLayout->addWidget(AddButton = new QPushButton("Add", gbox));
  bLayout->addWidget(ModifyButton = new QPushButton("Modify", gbox));
  bLayout->addWidget(DelButton = new QPushButton("Del", gbox));

  ModifyButton->setMinimumSize(Size = ModifyButton->sizeHint());
  AddButton->setMinimumSize(Size);
  DelButton->setMinimumSize(Size);

  aLayout -> activate();
  SubLayout->addWidget(gbox);
  
  SubLayout = new QHBoxLayout;
  Layout -> addLayout(SubLayout);

  SubLayout->addStretch(1);
  SubLayout->addWidget(OKButton	= new QPushButton("OK", this));
  SubLayout->addWidget(CancelButton = new QPushButton("Cancel", this));

  CancelButton->setMinimumSize(Size = CancelButton->sizeHint());
  OKButton->setMinimumSize(Size);

  OKButton->setDefault(TRUE);

  connect(AddButton, SIGNAL(clicked()), SLOT(addKey()));
  connect(ModifyButton, SIGNAL(clicked()), SLOT(modKey()));
  connect(DelButton, SIGNAL(clicked()), SLOT(delKey()));
  connect(OKButton, SIGNAL(clicked()), SLOT(accept()));
  connect(CancelButton, SIGNAL(clicked()), SLOT(reject()));

  Layout -> activate();
  if (KeyList->count()) KeyList->setCurrentItem(0);
  else Key->setText("");
};

SBVLBINetEntryEditor::~SBVLBINetEntryEditor()
{
  emit networkModified(isModified, Network);
};

void SBVLBINetEntryEditor::showKey(int idx)
{
  Key->setText(KeyList->text(idx));
};

void SBVLBINetEntryEditor::delKey()
{
  int i;
  if (KeyList->count() && (i=KeyList->currentItem())!=-1 &&
      Network->keys()->find(&(Str=KeyList->text(i))))
    {
      KeyList->removeItem(i);
      Network->keys()->remove(); 
      isModified=TRUE;
    };
};

void SBVLBINetEntryEditor::addKey()
{
  if (Network->keys()->find(&(Str=Key->text()))==-1)
    {
      Network->addKey(Str);
      KeyList->insertItem(Str);
      KeyList->setCurrentItem(KeyList->count()-1);
      isModified=TRUE;
    };
};

void SBVLBINetEntryEditor::modKey()
{
  int i;
  if (KeyList->count() && (i=KeyList->currentItem())!=-1 &&
      Network->keys()->find(&(Str=KeyList->text(i))))
    {
      Network->keys()->remove(); 
      Network->addKey((Str=Key->text()));
      KeyList->changeItem(Str, i);
      isModified=TRUE;
    };
};

void SBVLBINetEntryEditor::accept()
{
  if ((Str=ID->text())!=Network->name())
    {
      Network->setName(Str);
      isModified=TRUE;
    };
  if ((Str=Descr->text())!=Network->descr())
    {
      Network->setDescr(Str);
      isModified=TRUE;
    };
  QDialog::accept();
  delete this; 
  if (LI) LI->repaint();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMasterRecBrowser::SBMRListItem implementation..						*/
/*												*/
/*==============================================================================================*/
QString SBMasterRecBrowser::SBMRListItem::text(int col) const
{
  TmpString = "NULL";
  if (Rec)
    switch (col)
      {
      case  0: TmpString = Rec->name().copy();
	break;
      case  1: TmpString = Rec->sessionName().copy();
	break;
      case  2: TmpString = Rec->sessionCode().copy();
	break;
      case  3: TmpString = Rec->sessionDate().copy();
	break;
      case  4: TmpString = Rec->correlatorName().copy();
	break;
      case  5:
	TmpString = Rec->isAttr(SBMasterRecord::Exists)?"Y":" ";
	break;
      case  6:
	TmpString = MFName.copy();
	break;
      };
  return TmpString;
};

QString SBMasterRecBrowser::SBMRListItem::key(int col, bool) const
{
  TmpString = "NULL";
  if (Rec)
    switch (col)
      {
      case  0: TmpString = QString().sprintf("%4d", Rec->yr()<70?Rec->yr()+2000:Rec->yr()+1900) + 
		 Rec->name();
	break;
      case  1: TmpString = Rec->sessionName().copy();
	break;
      case  2: TmpString = Rec->sessionCode().copy();
	break;
      case  3: TmpString = Rec->sessionDate().copy();
	break;
      case  4: TmpString = Rec->correlatorName().copy();
	break;
      case  5:
	TmpString = Rec->isAttr(SBMasterRecord::Exists)?"0Y":"1Z";
	break;
      case  6:
	TmpString = MFName.copy();
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMasterRecBrowser implementation..								*/
/*												*/
/*==============================================================================================*/
SBMasterRecBrowser::SBMasterRecBrowser(SBVLBISet* W_, QWidget* parent, 
				       const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  isModified	= FALSE;
  VLBI		= W_;
  VLBI->loadMaster();
  Master	= VLBI->master();

  setCaption("Master File Records");
  
  QPushButton	*OKButton, *CancelButton;
  QSize		Size;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;
  
  gbox	 = new QGroupBox("Master File Records", this);
  Layout = new QVBoxLayout(gbox, 20, 10);
  
  // set up list box:
  MRecLV = new QListView(gbox);
  MRecLV -> addColumn("DBH Name");
  MRecLV -> setColumnAlignment( 0, AlignLeft);
  MRecLV -> addColumn("Offic.Name");
  MRecLV -> setColumnAlignment( 1, AlignLeft);
  MRecLV -> addColumn("Code");
  MRecLV -> setColumnAlignment( 2, AlignLeft);
  MRecLV -> addColumn("Date");
  MRecLV -> setColumnAlignment( 3, AlignLeft);
  MRecLV -> addColumn("Corr.");
  MRecLV -> setColumnAlignment( 4, AlignCenter);
  MRecLV -> addColumn("Exists");
  MRecLV -> setColumnAlignment( 5, AlignCenter);
  MRecLV -> addColumn("Master File");
  MRecLV -> setColumnAlignment( 6, AlignLeft);

  for (SBMasterFile *mf=Master->first(); mf; mf=Master->next())
    {
      for (SBMasterRecord *R=mf->first(); R; R=mf->next())
	{
	  if (VLBI->sessionInfo(R->name()))
	    R->addAttr(SBMasterRecord::Exists);
	  (void) new SBMRListItem(MRecLV, R, mf->name());
	};
    };

  MRecLV -> setAllColumnsShowFocus(TRUE);
  MRecLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  MRecLV -> setMinimumSize(MRecLV->sizeHint());
  MRecLV -> setFocusPolicy(QWidget::StrongFocus);
  if (Master->count() && MRecLV->firstChild())
    MRecLV -> setSelected(MRecLV->firstChild(), TRUE);
  Layout   -> addWidget(MRecLV, 10);

  Layout->activate();
  

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout->addWidget(gbox, 10);
  Layout->addLayout(SubLayout = new QHBoxLayout(10));
  
  SubLayout->addStretch(1);
  SubLayout->addWidget(OKButton = new QPushButton("OK", this));
  SubLayout->addWidget(CancelButton = new QPushButton("Cancel", this));

  CancelButton	-> setMinimumSize(Size=CancelButton->sizeHint());
  OKButton	-> setMinimumSize(Size);

  connect(OKButton,	SIGNAL(clicked()), SLOT(accept()));
  connect(CancelButton, SIGNAL(clicked()), SLOT(reject()));

  Layout->activate();
};

SBMasterRecBrowser::~SBMasterRecBrowser()
{
  VLBI->releaseMaster(); 
  emit masterRecordModified(isModified);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBClockBreakEditor..										*/
/*												*/
/*==============================================================================================*/
SBClockBreakEditor::SBClockBreakEditor(SBClockBreakLI* Bli_,
				       QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  Bli = Bli_;
  if (Bli)
    B = Bli->clockBreak();
  else 
    B = NULL;

  isModified = FALSE;

  setCaption("Clocks Break Editor");

  QLabel	*label;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;
  QGridLayout	*grid;
  
  int Yr=0, Mo=0, Dy=0, Hr=0, Mi=0;
  double Se=0.0, Val=0.0;

  if (B)
    {
      Yr = B->year();
      Mo = B->month();
      Dy = B->day();
      Hr = B->hour();
      Mi = B->min();
      Se = B->sec();
      Val= B->b();
    };
  
  QString	Str;

  gbox = new QGroupBox("Clock Break", this);
  grid = new QGridLayout(gbox, 3, 12,   20, 5);

  label= new QLabel("Epoch:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 0, Qt::AlignLeft | Qt::AlignVCenter);

  label= new QLabel("Value (ps):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);


  //
  label= new QLabel("YYYY", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 1);

  label= new QLabel("/", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 2);

  label= new QLabel("MM", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 3);

  label= new QLabel("/", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 4);

  label= new QLabel("DD", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 5);

  label= new QLabel("HH", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 7);

  label= new QLabel(":", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 8);

  label= new QLabel("MM", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 9);

  label= new QLabel(":", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1,10);

  label= new QLabel("SS.SS", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,11);


  //
  leYr = new QLineEdit(Str.sprintf("%04d", Yr), gbox);
  leYr->setFixedWidth(leYr->fontMetrics().width("YYYY")+10);
  grid ->addWidget(leYr, 1, 1);

  leMo = new QLineEdit(Str.sprintf("%02d", Mo), gbox);
  leMo->setFixedWidth(leMo->fontMetrics().width("MM")+10);
  grid ->addWidget(leMo, 1, 3);

  leDy = new QLineEdit(Str.sprintf("%02d", Dy), gbox);
  leDy->setFixedWidth(leDy->fontMetrics().width("MM")+10);
  grid ->addWidget(leDy, 1, 5);

  leHr = new QLineEdit(Str.sprintf("%02d", Hr), gbox);
  leHr->setFixedWidth(leHr->fontMetrics().width("MM")+10);
  grid ->addWidget(leHr, 1, 7);

  leMi = new QLineEdit(Str.sprintf("%02d", Mi), gbox);
  leMi->setFixedWidth(leMi->fontMetrics().width("MM")+10);
  grid ->addWidget(leMi, 1, 9);

  leSe = new QLineEdit(Str.sprintf("%5.2f", Se), gbox);
  leSe->setFixedWidth(leSe->fontMetrics().width("SS.SS")+10);
  grid ->addWidget(leSe, 1,11);


  //
  leVal = new QLineEdit(Str.sprintf("%.2f", Val), gbox);
  leVal->setMinimumSize(leVal->sizeHint());
  grid ->addMultiCellWidget(leVal, 2,2,  1, 5);

  grid->addColSpacing(6, 15);
  grid->activate();

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  Ok->setDefault(TRUE);
  QPushButton *Cancel	= new QPushButton("Cancel", this);

  QSize	BtnSize;

  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  Layout->activate();
};

void SBClockBreakEditor::accept()
{
  acquireData();
  QDialog::accept();
  if (Bli && isModified)
    Bli->repaint();
  emit clockBreakModified(isModified);
  delete this;
};

void SBClockBreakEditor::reject()
{
  emit clockBreakModified(FALSE);
  QDialog::reject(); 
  delete this;
};

void SBClockBreakEditor::acquireData()
{
  if (!Bli || !B)
    return;

  bool		isOK = TRUE;
  int		Yr = 0;
  int		Mo = 0;
  int		Dy = 0;
  int		Hr = 0;
  int		Mi = 0;
  double	Se = 0;

  if (isOK)
    Yr = leYr->text().toInt(&isOK);
  if (isOK)
    Mo = leMo->text().toInt(&isOK);
  if (isOK)
    Dy = leDy->text().toInt(&isOK);
  if (isOK)
    Hr = leHr->text().toInt(&isOK);
  if (isOK)
    Mi = leMi->text().toInt(&isOK);
  if (isOK)
    Se = leSe->text().toDouble(&isOK);

  SBMJD t(Yr, Mo, Dy, Hr, Mi, Se);
  if (isOK && t!=*B)
    {
      isModified = TRUE;
      *(SBMJD*)B = t;
    };

  if (isOK)
    Se = leVal->text().toDouble(&isOK);
  if (isOK && Se!=B->b())
    {
      isModified = TRUE;
      B->setB(Se);
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStaParsEditor..										*/
/*												*/
/*==============================================================================================*/
SBStaParsEditor::SBStaParsEditor(SBStationInfo* SI_, const SBMJD& T0_,
				 QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  SI = SI_;
  T0 = T0_;
  isModified = FALSE;

  static const QString AttrName[6] = 
  {
    QString("Ignore the station"),
    QString("Clocks of the station are the reference ones"),
    QString("The station has a bad cable calibration"),
    QString("The station has an inverted cable calibration"),
    QString("Metheo data on the station are wrong"),
    QString("Metheo data on the station are artificial or suspicious")
  };

  setCaption("Local Parameters for the station " + SI->aka());
  QBoxLayout	*Layout, *SubLayout;

  QGroupBox *gbox = new QGroupBox("List of Clock Break Events", this);
  Layout = new QVBoxLayout(gbox, 20, 10);

  ClockBreaksLV = new QListView(gbox);
  ClockBreaksLV -> addColumn("Epoch");
  ClockBreaksLV -> setColumnAlignment( 0, AlignLeft);
  ClockBreaksLV -> addColumn("Value (ps)");
  ClockBreaksLV -> setColumnAlignment( 1, AlignRight);

  for (SBParameterBreak* b=SI->clockBreaks().first(); b; b=SI->clockBreaks().next())
    (void) new SBClockBreakLI(ClockBreaksLV, b);
  
  ClockBreaksLV -> setAllColumnsShowFocus(TRUE);
  ClockBreaksLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (ClockBreaksLV->firstChild())
    ClockBreaksLV->setSelected(ClockBreaksLV->firstChild(), TRUE);

  ClockBreaksLV->setFocusPolicy(QWidget::StrongFocus);
  ClockBreaksLV->setMinimumHeight(ClockBreaksLV->sizeHint().height());
  Layout -> addWidget(ClockBreaksLV, 5);
  
  Layout -> activate();
  connect (ClockBreaksLV, SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntry()));
  connect (ClockBreaksLV, SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntry()));

  //
  QGroupBox *gboxAttr = new QGroupBox("Attributes of Station", this);
  Layout = new QVBoxLayout(gboxAttr, 20, 10);
  for (int i=0; i<6; i++)
    {
      cbAttrs[i] = new QCheckBox(AttrName[i], gboxAttr);
      cbAttrs[i]-> setMinimumSize(cbAttrs[i]->sizeHint());
      //      cbAttrs[i]-> setChecked(Cfg->isEphBody(i));
      Layout->addWidget(cbAttrs[i]);
    };
  cbAttrs[0]-> setChecked(SI->isAttr(SBStationInfo::notValid));
  cbAttrs[1]-> setChecked(SI->isAttr(SBStationInfo::refClock));
  cbAttrs[2]-> setChecked(SI->isAttr(SBStationInfo::BadCable));
  cbAttrs[3]-> setChecked(SI->isAttr(SBStationInfo::InvCable));
  cbAttrs[4]-> setChecked(SI->isAttr(SBStationInfo::BadMeteo));
  cbAttrs[5]-> setChecked(SI->isAttr(SBStationInfo::ArtMeteo));

  Layout -> activate();

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  Layout -> addWidget(gboxAttr, 10);

  QGridLayout	*grid = new QGridLayout(Layout, 3, 2);

  
  QLabel *label = new QLabel("Number of polinoms for a clock model:", this);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 0);

  sbOrder = new QSpinBox(3,10, 1, this);
  sbOrder->setValue(SI->cl_Order());
  sbOrder->setMinimumSize(sbOrder->sizeHint());
  grid ->addWidget(sbOrder, 0, 1);

  cbClocks = new QCheckBox("Use local model of stochastic parameters for Clocks", this);
  cbClocks-> setChecked(SI->isAttr(SBStationInfo::UseLocalClocks));
  cbClocks-> setMinimumSize(cbClocks->sizeHint());
  grid->addWidget(cbClocks, 1,0);
  
  cbZenith = new QCheckBox("Use local model of stochastic parameters for Clocks", this);
  cbZenith-> setChecked(SI->isAttr(SBStationInfo::UseLocalZenith));
  cbZenith-> setMinimumSize(cbZenith->sizeHint());
  grid->addWidget(cbZenith, 2,0);

  CPButton = new QPushButton("Edit Clocks", this);
  CPButton-> setMinimumSize(CPButton->sizeHint());
  grid->addWidget(CPButton, 1,1);

  ZPButton = new QPushButton("Edit Zenith", this);
  ZPButton-> setMinimumSize(ZPButton->sizeHint());
  grid->addWidget(ZPButton, 2,1);
  grid->setMargin(20);
  

  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  Ok->setDefault(TRUE);
  QPushButton *Add	= new QPushButton("Add", this);
  QPushButton *Edit	= new QPushButton("Edit", this);
  QPushButton *Delete	= new QPushButton("Delete", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);

  QSize	BtnSize;
  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok->setMinimumSize(BtnSize);
  Add->setMinimumSize(BtnSize);
  Edit->setMinimumSize(BtnSize);
  Delete->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Add);
  SubLayout->addWidget(Edit);
  SubLayout->addWidget(Delete);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(CPButton, SIGNAL(clicked()), SLOT(editClocksPar()));
  connect(ZPButton, SIGNAL(clicked()), SLOT(editZenithPar()));
  connect(Add,    SIGNAL(clicked()), SLOT(insertEntry()));
  connect(Edit,   SIGNAL(clicked()), SLOT(editEntry()));
  connect(Delete, SIGNAL(clicked()), SLOT(deleteEntry()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  Layout->activate();
};

SBStaParsEditor::~SBStaParsEditor()
{
  SI = NULL;
  emit staInfoModified(isModified);
};
 
void SBStaParsEditor::accept()
{
  acquireData();
  QDialog::accept();
  delete this;
};

void SBStaParsEditor::acquireData()
{
  int		l=0;

  // Attributes:
  if (cbAttrs[0]->isChecked() != SI->isAttr(SBStationInfo::notValid))
    {
      SI->xorAttr(SBStationInfo::notValid);
      isModified = TRUE;
    };
  if (cbAttrs[1]->isChecked() != SI->isAttr(SBStationInfo::refClock))
    {
      SI->xorAttr(SBStationInfo::refClock);
      isModified = TRUE;
    };
  if (cbAttrs[2]->isChecked() != SI->isAttr(SBStationInfo::BadCable))
    {
      SI->xorAttr(SBStationInfo::BadCable);
      isModified = TRUE;
    };
  if (cbAttrs[3]->isChecked() != SI->isAttr(SBStationInfo::InvCable))
    {
      SI->xorAttr(SBStationInfo::InvCable);
      isModified = TRUE;
    };
  if (cbAttrs[4]->isChecked() != SI->isAttr(SBStationInfo::BadMeteo))
    {
      SI->xorAttr(SBStationInfo::BadMeteo);
      isModified = TRUE;
    };
  if (cbAttrs[5]->isChecked() != SI->isAttr(SBStationInfo::ArtMeteo))
    {
      SI->xorAttr(SBStationInfo::ArtMeteo);
      isModified = TRUE;
    };

  // Peculiar parameters:
  if (cbClocks->isChecked() != SI->isAttr(SBStationInfo::UseLocalClocks))
    {
      SI->xorAttr(SBStationInfo::UseLocalClocks);
      isModified = TRUE;
    };
  if (cbZenith->isChecked() != SI->isAttr(SBStationInfo::UseLocalZenith))
    {
      SI->xorAttr(SBStationInfo::UseLocalZenith);
      isModified = TRUE;
    };
  if ((l=sbOrder->value()) != SI->cl_Order())
    {
      SI->setCl_Order(l);
      isModified = TRUE;
    };
};

void SBStaParsEditor::deleteEntry()
{
  SBClockBreakLI	*Bli;
  SBParameterBreak	*B;
  if ( (Bli = (SBClockBreakLI*)ClockBreaksLV->currentItem()) && (B = Bli->clockBreak()))
    {
      QListViewItem* NextItem = Bli->itemBelow();
      if (!NextItem) 
	NextItem = Bli->itemAbove();

      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete clock break happened on\n \""
				+ B->toString() + "\"?\n", "Yes, del' them!", "No, let it live.."))
	{
	  if (SI->clockBreaks().remove(B))
	    {
	      delete Bli;
	      if (NextItem) 
		ClockBreaksLV->setSelected(NextItem, TRUE);
	      isModified = TRUE;
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": `" + B->toString() + "' not found in the list of clock breaks");
	};
    };
};

void SBStaParsEditor::insertEntry()
{  
  SBParameterBreak	*B = new SBParameterBreak(T0, 0.0);
  SI->clockBreaks().append(B);
  SBClockBreakLI* Bli = new SBClockBreakLI(ClockBreaksLV, B);
  
  SBClockBreakEditor *CBEditor =  new SBClockBreakEditor(Bli, this);
  connect (CBEditor, SIGNAL(clockBreakModified(bool)), SLOT(modified(bool)));
  CBEditor->show();
};

void SBStaParsEditor::editEntry()
{  
  SBClockBreakLI	*Bli = (SBClockBreakLI*)ClockBreaksLV->currentItem();
  if (Bli)
    {
      SBClockBreakEditor *CBEditor =  new SBClockBreakEditor(Bli, this);
      connect (CBEditor, SIGNAL(clockBreakModified(bool)), SLOT(modified(bool)));
      CBEditor->show();
    };
};

void SBStaParsEditor::modified(bool IsModified_)
{  
  isModified = isModified || IsModified_;
  ClockBreaksLV->sort();
};

void SBStaParsEditor::editClocksPar()
{  
  SBParEditor *e =  new SBParEditor(&(SI->clocks()), "Clocks", 0, this);
  connect (e, SIGNAL(valueModified(bool)), SLOT(modified(bool)));
  e->show();
};

void SBStaParsEditor::editZenithPar()
{  
  SBParEditor *e =  new SBParEditor(&(SI->zenith()), "Zenith", 7, this);
  connect (e, SIGNAL(valueModified(bool)), SLOT(modified(bool)));
  e->show();
};
/*==============================================================================================*/



/*==============================================================================================*/
