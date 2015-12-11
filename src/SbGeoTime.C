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

#include "SbGeoTime.H"
#include "SbGeoEop.H"
#include <qdatetime.h> 


/*==============================================================================================*/
/*												*/
/* SBMJD implementation										*/
/* (MJD calculation (and some fields) stolen from Qt)						*/
/*												*/
/*==============================================================================================*/
const char *SBMJD::monthNames[] = 
{
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const char *SBMJD::weekdayNames[] =
{
  "Mon","Tue","Wed","Thu","Fri","Sat","Sun"
};

const SBMJD TEphem(51544.5);	// J2000 1Jan 12h 0m 0.0s
const SBMJD TZero (1943, 9, 6);	// the birthday of Roger Waters
const SBMJD TInf  (2100, 1, 1);	// 
const SBMJD TUnix0(1970, 1, 1);	// Unix epoch

const double SBTime::fTT_TAI = 32.184/DAY2SEC;

SBMJD::SBMJD(int Year, int Month, int Day, int Hour, int Min, double Sec)
{
  setMJD(Year, Month, Day, Hour, Min, Sec);
};

SBMJD::SBMJD(short date[5])
{
  setMJD(date[0], 1, date[1], date[2], date[3], date[4]);
  //  Date+=date[1];
};

void SBMJD::setMJD(int Year, int Month, int Day, int Hour, int Min, double Sec, bool)
{
  Date = MJD_date(Year, Month, Day);
  Time = MJD_time(Hour, Min,   Sec);
  while (Time>=1.0) {Time--; Date++;};
  while (Time< 0.0) {Time++; Date--;};
};

int SBMJD::MJD_date(int y, int m, int d) 
{
  uint c, ya;
  if (y<=99)
    y += 1900;
  if (m>2)
    m -= 3;
  else 
    {
      m += 9;
      y--;
    };
  c = y;
  c /= 100;
  ya = y - 100*c;
  return -678882 + d + (146097*c)/4 + (1461*ya)/4 + (153*m+2)/5;
};

long double SBMJD::MJD_time(int Hour, int Min, double Sec)
{
  return (long double)Sec/DAY2SEC + (long double)Min/1440.0 + (long double)Hour/24.0;
};

void SBMJD::MJD_rev(int date_, long double time_, 
		    int& y, int& m, int& d, int& hr, int& mi, double& se)
{
  if (time_>1.0)
    {
      time_--;
      date_++;
    };

  uint	x;
  uint	j = date_ + 678882;
  y = (j*4 - 1)/146097;
  j = j*4 - 146097*y - 1;
  x = j/4;
  j = (x*4 + 3) / 1461;
  y = 100*y + j;
  x = (x*4) + 3 - 1461*j;
  x = (x + 4)/4;
  m = (5*x - 3)/153;
  x = 5*x - 3 - 153*m;
  d = (x + 5)/5;
  if (m < 10) 
    m += 3; 
  else 
    {
      m -= 9;
      y++;
    };

  long double tmp = time_*DAY2SEC;
  //  se = time_*DAY2SEC;
  hr = (int)(tmp/3600.0);
  //  hr = (int)(se/3600.0);
  mi = (int)((tmp - 3600.0*hr)/60.0);
  //  mi = (int)((se - 3600.0*hr)/60.0);
  se = tmp - 3600.0*hr - 60.0*mi;
  //  se -= 3600.0*hr + 60.0*mi;
};

SBMJD SBMJD::currentMJD()
{
  QDateTime d(QDate::currentDate(), QTime::currentTime());
  SBMJD C(d.date().year(), d.date().month(), d.date().day(), 
	  d.time().hour(), d.time().minute(), 
	  (double)(d.time().second() + d.time().msec()*0.001));
  return C;
};

int SBMJD::year() const
{
  int		y; 
  int		m; 
  int		d;
  int		hr; 
  int		mi; 
  double	se;
  MJD_rev (Date, Time,  y,m,d,hr,mi,se);
  return y;
};

int SBMJD::month() const
{
  int		y; 
  int		m; 
  int		d;
  int		hr; 
  int		mi; 
  double	se;
  MJD_rev (Date, Time,  y,m,d,hr,mi,se);
  return m;
};

int SBMJD::hour() const
{
  int		y; 
  int		m; 
  int		d;
  int		hr; 
  int		mi; 
  double	se;
  MJD_rev (Date, Time,  y,m,d,hr,mi,se);
  return hr;
};

int SBMJD::min() const
{
  int		y; 
  int		m; 
  int		d;
  int		hr; 
  int		mi; 
  double	se;
  MJD_rev (Date, Time,  y,m,d,hr,mi,se);
  return mi;
};

double SBMJD::sec() const
{
  int		y; 
  int		m; 
  int		d;
  int		hr; 
  int		mi; 
  double	se;
  MJD_rev (Date, Time,  y,m,d,hr,mi,se);
  return se;
};

QString	SBMJD::month2Str() const
{
  return monthNames[month()-1];
};

int SBMJD::day() const
{
  int		y; 
  int		m; 
  int		d;
  int		hr; 
  int		mi; 
  double	se;
  MJD_rev (Date, Time,  y,m,d,hr,mi,se);
  return d;
};

int SBMJD::dayOfYear() const
{
  return Date - MJD_date(year(), 1, 1) + 1;
};

int SBMJD::dayOfWeek() const
{
  int i = (Date - SBMJD(1997,1,6).date())%7 + 1;
  return i<=0?i+7:i;
};

QString	SBMJD::dayOfWeek2Str() const
{
  if (1<=dayOfWeek() && dayOfWeek()<=7)
    return weekdayNames[dayOfWeek()-1];
  else
    {
      std::cerr << "QString SBMJD::day_of_week2Str() const, : " << Date
	   << ", get_day_of_week():" << dayOfWeek() << "\n";
      exit(1);
    };
};

QString SBMJD::toString(Format F) const
{
  QString	s;
  int		y;
  int		m;
  int		d;
  int		hr;
  int		min;
  double	sec;
  QString	th;

  if (Date)
    {
      MJD_rev (Date, Time,  y,m,d,hr,min,sec);
      switch(F)
	{
	case F_Short:
	  if (sec>=59.999999)
	    MJD_rev (Date, Time+(60.0-sec)/DAY2SEC,  y,m,d,hr,min,sec);
	  s.sprintf("%02d %s, %04d; %02d:%02d:%09.6f", d, monthNames[m-1], y, hr,min,sec);
	  break;
	case F_Long:
	  switch (d%10)
	    {
	    case 1:  th="st"; break;
	    case 2:  th="nd"; break;
	    case 3:  th="rd"; break;
	    default: th="th";
	    };
	  s.sprintf("%s, the %d%s of %s, %04d; %02dhr %02dmin %09.6fsec",
		    (const char*)dayOfWeek2Str(), d, (const char*)th,
		    monthNames[m-1], y, hr,min,sec);
	  break;
	case F_DDMonYYYY:
	  s.sprintf("%02d %s, %04d", d, monthNames[m-1], y);
	  break;
	case F_Year:
	  s.sprintf("%7.2f", year() + dayOfYear()/365.25);
	  break;
	case F_YYMonDD:
	  s.sprintf("%02d%s%02d", y-1900, monthNames[m-1], d);
	  break;

	case F_DateShort:
	  s.sprintf("%04d %s %02d", y, monthNames[m-1], d);
	  break;
	case F_TimeShort:
	  if (sec>59.9) 
	    {
	      sec=0.0;
	      min++;
	    };
	  if (min==60)
	    {
	      hr++;
	      min=0;
	    };
	  s.sprintf("%02d:%02d:%04.1f", hr, min, sec);
	  break;
	case F_TimeVShort:
	  if (sec>58.0) min++;
	  if (min==60)
	    {
	      hr++;
	      min=0;
	    };
	  s.sprintf("%02d:%02d", hr, min);
	  break;

	case F_HHMMSS:
	  s.sprintf("%02d:%02d:%04.1f", hr, min, sec);
	  break;
	case F_YYYYMMDDSSSSSS:
	  s.sprintf("%04d%02d%02d%06d", y,m,d, (int)((hr*3600+min*60+sec)*10));
	  break;
	case F_YYYYMMDDHHMMSSSS:
	  s.sprintf("%04d/%02d/%02d %02d:%02d:%04.1f", y,m,d, hr, min, sec);
	  break;
	case F_INTERNAL: //MJDate:TimeInSeconds
	  s.sprintf("%06d:%010.4f", Date, (double)Time*86400.0);
	  break;
	case F_ECCDAT:
	  s.sprintf("%04d.%02d.%02d-%02d.%02d", y,m,d, hr,min);
	  break;
	case F_MJD:
	  s.sprintf("%10.4f", Date + (double)Time);
	  break;
	case F_UNIX:
	  //
	  s.sprintf("%.4f", 86400.0*(double)((*this-TUnix0)));
	  break;
	case F_SINEX:
	  if (*this==TZero)
	    s="00:000:00000";
	  else
	    {
	      sec = 86400.0*Time;
	      s.sprintf("%02d:%03d:%05d", y<2000?y-1900:y-2000, dayOfYear(), (int)round(sec));
	    };
	  break;
	case F_SINEX_S:
	  s.sprintf("%02d:%03d", y<2000?y-1900:y-2000, dayOfYear());
	  break;
	case F_YYYYMMDD:
	default:
	  s.sprintf("%04d %02d %02d", y,m,d);
	  break;
	};
      return s;
    }
  else 
    return "UNKNOWN";
};

void SBMJD::setMJD(Format F, QString s, bool isReset)
{
  int		y;
  int		m;
  int		d;
  int		hr;
  int		min;
  double	sec;
  bool		isOK;
  QString	tmp;
  
  if (isReset) {Date = 0; Time = 0.0;};
  if (s=="UNKNOWN") return;
  switch(F)
    {      
    case F_Short:
      if (s.length()<26)
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
	  ":setMJD(F_Short): wrong string length (<26); `" + s + "'");
	  return;
	};
      d = s.mid(0,2).toInt(&isOK);
      if (!isOK) return;

      m=0;
      tmp = s.mid(3,3);
      for (y=0; y<12; y++) if (monthNames[y]==tmp) m=y+1;
      if (!m) return;

      y = s.mid(8,4).toInt(&isOK);
      if (!isOK) return;

      hr = s.mid(14,2).toInt(&isOK);
      if (!isOK) return;

      min = s.mid(17,2).toInt(&isOK);
      if (!isOK) return;

      sec = s.mid(20,6).toDouble(&isOK);
      if (!isOK) return;

      setMJD(y,m,d, hr,min,sec);
      break;

    case F_Year:
      sec = s.toDouble(&isOK);
      if (!isOK) return;
      Date = MJD_date((int)floor(sec), 1, 1) + (int)(365.0*fmod(sec,1.0));
      Time = 0.0;
      break;

    case F_Long: // is there a sence to make parcing for it?
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + ": wrong format for parsering (F_Long)");
      break;

    case F_DDMonYYYY:
      if (s.length()<12)
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
		     ":setMJD(F_DDMonYYYY): wrong string length (<12); `" + s + "'");
	  return;
	};
      d = s.mid(0,2).toInt(&isOK);
      if (!isOK) return;
      m=0;
      tmp = s.mid(3,3);
      for (y=0; y<12; y++) if (tmp.find(monthNames[y], 0, FALSE)==0) m=y+1;
      //      for (y=0; y<12; y++) if (monthNames[y]==tmp) m=y+1;
      if (!m) return;
      y = s.mid(8,4).toInt(&isOK);
      if (!isOK) return;
      setMJD(y,m,d, 0,0,0.0);
      break;

    case F_YYMonDD:
      if (s.length()<7)
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
		     ":setMJD(F_YYMonDD): wrong string length (<7); `" + s + "'");
	  return;
	};
      d = s.mid(5,2).toInt(&isOK);
      if (!isOK) return;
      m=0;
      tmp = s.mid(2,3);
      for (y=0; y<12; y++) if (tmp.find(monthNames[y], 0, FALSE)==0) m=y+1;
      //      for (y=0; y<12; y++) if (monthNames[y]==tmp) m=y+1;
      if (!m) return;
      y = s.mid(0,2).toInt(&isOK);
      if (!isOK) return;
      setMJD(1900+y,m,d, 0,0,0.0);
      break;

    case F_YYYYMMDDHHMMSSSS:
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + 
		 ": unsupported format for parsering (F_YYYYMMDDHHMMSSSS)");
      break;
    case F_HHMMSS:
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + ": wrong format for parsering (F_HHMMSS)");
      break;
    case F_DateShort:
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + ": wrong format for parsering (F_DateShort)");
      break;
    case F_TimeShort:
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + ": wrong format for parsering (F_TimeShort)");
      break;
    case F_TimeVShort:
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + ": wrong format for parsering (F_TimeVShort)");
      break;
    case F_ECCDAT:
      //0123456789012345
      //1970.01.01-00:00
      if (s.length()<16)
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
		     ":setMJD(F_ECCDAT): wrong string length (<16); `" + s + "'");
	  return;
	};
      y = s.mid(0,4).toInt(&isOK);
      if (!isOK) return;

      m = s.mid(5,2).toInt(&isOK);
      if (!isOK) return;

      d = s.mid(8,2).toInt(&isOK);
      if (!isOK) return;

      hr = s.mid(11,2).toInt(&isOK);
      if (!isOK) return;

      min = s.mid(14,2).toInt(&isOK);
      if (!isOK) return;

      setMJD(y,m,d, hr,min, 0.0);
      break;

    case F_INTERNAL: //MJDate:TimeInSeconds
      //01234567890123456
      //052572:80913.0000
      if (s.length()<17)
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
		     ":setMJD(F_INTERNAL): wrong string length (<17); `" + s + "'");
	  return;
	};
      if (s.mid(6,1)!=":")
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
		     ":setMJD(F_INTERNAL): wrong format; `" + s + "'");
	  return;
	};
      y = s.mid(0,6).toInt(&isOK);
      if (!isOK) return;
      sec = s.mid(7,10).toDouble(&isOK);
      if (!isOK) return;
      Date = y;
      Time = sec/86400.0;
      break;
    case F_SINEX:
      //012345678901
      //95:120:86399
      if (s.length()<12)
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
		     ":setMJD(F_SINEX): wrong string length (<12); `" + s + "'");
	  return;
	};
      y = s.mid(0,2).toInt(&isOK);
      if (!isOK) return;
      d = s.mid(3,3).toInt(&isOK);
      if (!isOK) return;
      sec = s.mid(7,5).toDouble(&isOK);
      if (!isOK) return;
      setMJD(y<=50?y+2000:y+1900,1,d,  0,0,sec);
      break;
    case F_SINEX_S:
      //012345
      //95:120
      if (s.length()<6)
	{
	  Log->write(SBLog::WRN, SBLog::TIME, ClassName() + 
		     ":setMJD(F_SINEX_S): wrong string length (<6); `" + s + "'");
	  return;
	};
      y = s.mid(0,2).toInt(&isOK);
      if (!isOK) return;
      d = s.mid(3,3).toInt(&isOK);
      if (!isOK) return;
      setMJD(y<40?y+2000:y+1900,1,d,  0,0,0.0);
      break;
    case F_YYYYMMDD:
    default:
      if (s.length()<10)
	{
	  Log->write(SBLog::INF, SBLog::TIME, ClassName() + 
	  ":setMJD(F_YYYYMMDD): wrong string length (<10); `" + s + "'");
	  return;
	};
      y = s.mid(0,4).toInt(&isOK);
      if (!isOK) return;
      m = s.mid(5,2).toInt(&isOK);
      if (!isOK) return;
      d = s.mid(8,2).toInt(&isOK);
      if (!isOK) return;
      setMJD(y,m,d, 0,0,0.0);
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTime implementation									*/
/*												*/
/*==============================================================================================*/
SBTime::SBTime(SBEOP *EOP_) : SBMJD()
{
  Zero();
  EOP = EOP_;
};

SBTime::SBTime(const SBTime& T) : SBMJD(T)
{
  fTAI_UTC	= T.fTAI_UTC;
  fUT1_UTC	= T.fUT1_UTC;
  fUT1_UT1R	= T.fUT1_UT1R;
  
  FArgs[0]	= T.FArgs[0];
  FArgs[1]	= T.FArgs[1];
  FArgs[2]	= T.FArgs[2];
  FArgs[3]	= T.FArgs[3];
  FArgs[4]	= T.FArgs[4];
  FArgs[5]	= T.FArgs[5];
  EOP		= T.EOP;
};

void SBTime::Zero() 
{
  fTAI_UTC	= 0.0;
  fUT1_UTC	= 0.0;
  fUT1_UT1R	= 0.0;
  fTDB_TDT	= 0.0;
  
  FArgs[0]	= 0.0;
  FArgs[1]	= 0.0;
  FArgs[2]	= 0.0;
  FArgs[3]	= 0.0;
  FArgs[4]	= 0.0;
  FArgs[5]	= 0.0;
  Omega		= 0.0;
};

void SBTime::DefineTAI_UTC()
{
  if (date()<44239.0) fTAI_UTC = 18.0;
  else if (date()<44786.0) fTAI_UTC = 19.0;
  else if (date()<45151.0) fTAI_UTC = 20.0;
  else if (date()<45516.0) fTAI_UTC = 21.0;
  else if (date()<46247.0) fTAI_UTC = 22.0;
  else if (date()<47161.0) fTAI_UTC = 23.0;
  else if (date()<47892.0) fTAI_UTC = 24.0;
  else if (date()<48257.0) fTAI_UTC = 25.0;
  else if (date()<48804.0) fTAI_UTC = 26.0;
  else if (date()<49169.0) fTAI_UTC = 27.0;
  else if (date()<49534.0) fTAI_UTC = 28.0;
  else if (date()<50083.0) fTAI_UTC = 29.0; //Jan 1, 1996
  else if (date()<50630.0) fTAI_UTC = 30.0; //Jul 1, 1997
  else if (date()<51179.0) fTAI_UTC = 31.0; //Jan 1, 1999
  else if (date()<53736.0) fTAI_UTC = 32.0; //Jan 1, 2006
  else if (date()<54832.0) fTAI_UTC = 33.0; //Jan 1, 2009
  else 
    fTAI_UTC = 34.0;
  fTAI_UTC/=DAY2SEC;
};

void SBTime::FundamentalArgs()
{
  /*
    IERS Technical Note 21,
    IERS Conventions (1996)
    page 23 says:
    ...t is measured in Julian Centuries of 36525 days of 86400 seconds
    of Dynamical Time [I hope, "terrestrial"] since J2000.0...
   */
  double t  = (TT() - TEphem)/36525.0;
  //  double t  = (TDB() - TEphem)/36525.0;

  double t2 = t*t;
  double t3 = t2*t;
  double t4 = t2*t2;
 
  /* l, the Mean Anomaly of the Moon, arcsecs: */
  FArgs[0] = 134.96340251*3600.0 + 1717915923.2178*t + 31.8792*t2 + 0.051635*t3 - 0.00024470*t4;

  /* l', the Mean Anomaly of the Sun, arcsecs: */
  // Warning:	sign "-" have been changed to "+" for "0.000136" coeff at t^3
  //		according to D.D.McCarthy "Errata for the IERS Conventions (1996)", item 24.
  FArgs[1] = 357.52910918*3600.0 + 129596581.0481*t - 0.5532*t2 + 0.000136*t3 - 0.00001149*t4;
  // for comp. w/ old ver:
  //  FArgs[1] = 1287104.79305 + 129596581.0481*t - 0.5532*t2 + 0.000136*t3 - 0.00001149*t4;
  
  /* F, Mean Argument of Latitude of the Moon, arcsecs: */
  FArgs[2] = 93.27209062*3600.0 + 1739527262.8478*t - 12.7512*t2 - 0.001037*t3 + 0.00000417*t4;

  /* D, the Mean Elongation of the Moon from the Sun, arcsecs: */
  FArgs[3] = 297.85019547*3600.0 + 1602961601.2090*t - 6.3706*t2 + 0.006593*t3 - 0.00003169*t4;
  //  FArgs[3] = 1072260.70369 + 1602961601.2090*t - 6.3706*t2 + 0.006593*t3 - 0.00003169*t4;

  /* Omega, the Mean Longitude of the Ascending Node of the Moon, arcsecs: */
  //				     - 6962890.5431*t //<- IAU Nut 2000A value
  //  FArgs[4] = 125.04455501*3600.0 - 6962890.2665*t + 7.4722*t2 + 0.007702*t3 - 0.00005939*t4;
  FArgs[4] = 125.04455501*3600.0 - 6962890.5431*t + 7.4722*t2 + 0.007702*t3 - 0.00005939*t4;
  
  for (int i=0; i<5; i++) 
    {
      FArgs[i] = fmod(FArgs[i]*SEC2RAD, 2.0*M_PI); //rads
      if (FArgs[i]<0.0) FArgs[i]+=2.0*M_PI; //[0; 2Pi]
    };

  /* Earth Rotational Angle = Greenvich Mean Sidereal Time + Pi, rad: */
////////////////////////////////////////////////////////////////////////
//  t  = (UT1().date() - TEphem)/36525.0;
//  t2 = t*t;
//  double d = (24110.54841 + 8640184.812866*t + 0.093104*t2 - 6.2e-6*t*t2)/DAY2SEC; //days
//  double r = 1.002737909350795 + 5.9006e-11*t - 5.9e-15*t2;
//  FArgs[5] = 2.0*M_PI*fmod(d + r*UTC().time() + 0.5, 1.0); //rads
////////////////////////////////////////////////////////////////////////
  double r = 1.00273781191135448;
  FArgs[5] = 2.0*M_PI*(0.7790572732640 + fmod(r*(UTC() - TEphem), 1.0)) + M_PI; //rads
  if (FArgs[5] < 0.0     )    FArgs[5]+=2.0*M_PI; //[0; 2Pi]
  if (FArgs[5] > 2.0*M_PI)    FArgs[5]-=2.0*M_PI; //[0; 2Pi]

  Omega = FArgs[5] - M_PI; // GMST
};

void SBTime::setUTC(const SBMJD& UTC_)
{
  if (UTC_ == *this) return;
  if (UTC_.date() != date()) 
    {
      SBMJD::operator=(UTC_);
      DefineTAI_UTC();
    }
  else 
    SBMJD::operator=(UTC_);
  FundamentalArgs();
  fUT1_UTC = EOP?EOP->dUT1_UTC(*this):0.0;

  double g = (357.528 + 35999.050*(TT() - TEphem)/36525.0)*DEG2RAD;
  fTDB_TDT = (0.001658*sin(g + 0.0167*sin(g)))/DAY2SEC;


  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::TIME))
    {
      if (!EOP)
	Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": EOP is NULL");
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": UTC        : " +       toString());
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": UT1        : " + UT1().toString());
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": TAI        : " + TAI().toString());
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": TT         : " + TT() .toString());
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": TDB        : " + TDB().toString());
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": TAI-UTC    : " + rad2hmsStr(fTAI_UTC*M_PI*2.0));

      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": Fund.Arg[0]: " + 
		 rad2dmslStr(FArgs[0]) + QString().sprintf("; %20.16f (rad)", FArgs[0]));
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": Fund.Arg[1]: " + 
		 rad2dmslStr(FArgs[1]) + QString().sprintf("; %20.16f (rad)", FArgs[1]));
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": Fund.Arg[2]: " + 
		 rad2dmslStr(FArgs[2]) + QString().sprintf("; %20.16f (rad)", FArgs[2]));
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": Fund.Arg[3]: " + 
		 rad2dmslStr(FArgs[3]) + QString().sprintf("; %20.16f (rad)", FArgs[3]));
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": Fund.Arg[4]: " + 
		 rad2dmslStr(FArgs[4]) + QString().sprintf("; %20.16f (rad)", FArgs[4]));
      Log->write(SBLog::DBG, SBLog::TIME, ClassName() + ": Fund.Arg[5]: " + 
		 rad2dmslStr(FArgs[5]) + QString().sprintf("; %20.16f (rad)", FArgs[5]));
    };


};
/*==============================================================================================*/



/*==============================================================================================*/
