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

#ifndef SB_GEO_TIME_H
#define SB_GEO_TIME_H

#include <config.h>
#include <math.h>

#include <qdatastream.h>
#include <qdict.h>
#include <qlist.h>


class SBMJD;
extern const SBMJD TEphem;
extern const SBMJD TZero;
extern const SBMJD TInf;


/*==============================================================================================*/
/*												*/
/* class SBMJD											*/
/*												*/
/*==============================================================================================*/
class SBMJD
{
public:
  enum Format		{F_YYYYMMDD, F_DDMonYYYY, F_Short, F_Long, F_Year, F_YYMonDD, 
			 F_HHMMSS, F_YYYYMMDDSSSSSS, F_INTERNAL, F_YYYYMMDDHHMMSSSS,
			 F_DateShort, F_TimeShort, F_TimeVShort, F_ECCDAT, F_MJD, F_UNIX, 
			 F_SINEX, F_SINEX_S};
  static int		MJD_date(int, int, int);
  static long double	MJD_time(int, int, double);
  static void		MJD_rev (int, long double, int&,int&,int&,int&,int&,double&);
  static SBMJD		currentMJD();

private:
  int			Date;
  long double		Time;

protected:
  static const char* monthNames[];
  static const char* weekdayNames[];
  
public:
  SBMJD()                    {Date=0; Time=0.0;};
  SBMJD(const SBMJD& MJD)    {Date=MJD.Date; Time=MJD.Time;};
  SBMJD(long double MJD)     {Date = (int)floorl(MJD); Time = MJD - (long double) Date;};
  SBMJD(int d, long double t){Date = d + (int)floorl(t); Time = t>=1.0?t - floorl(t):t;};
  SBMJD(int, int, int, int=0, int=0, double=0.0);
  SBMJD(short [5]);
  virtual ~SBMJD(){};
  virtual QString ClassName() const {return "SBMJD";};



  // interface:
  virtual void		setMJD  (int, int, int, int=0, int=0, double=0.0, bool=TRUE);
  virtual void		setMJD  (Format, QString, bool=TRUE);
  virtual void		setDate (int Date_) {Date = Date_;};
  virtual void		setTime (long double Time_) 
    {Time = Time_; while (Time>=1.0) {Time--; Date++;}; while (Time< 0.0) {Time++; Date--;};};


  virtual QString	toString(Format=F_Short) const;
  virtual bool		isGood  (){return Date>0 && 0.0<=Time && Time<1.0;};

  virtual operator long double() const {return (long double)Date + (long double)Time;};
  inline virtual SBMJD& operator= (const SBMJD&);
  inline virtual SBMJD& operator= (long double);

  inline virtual bool operator==(const SBMJD&) const;
  inline virtual bool operator!=(const SBMJD&) const;
  inline virtual bool operator< (const SBMJD&) const;
  inline virtual bool operator<=(const SBMJD&) const;
  inline virtual bool operator> (const SBMJD&) const;
  inline virtual bool operator> (double) const;
  inline virtual bool operator>=(const SBMJD&) const;

  inline virtual SBMJD& operator+=(const SBMJD& MJD);
  inline virtual SBMJD& operator-=(const SBMJD& MJD);

  inline friend SBMJD operator +(const SBMJD&, const SBMJD&);
  inline friend SBMJD operator +(const SBMJD&, long double);
  inline friend SBMJD operator +(const SBMJD&, double);
  inline friend SBMJD operator +(const SBMJD&, float);
  inline friend SBMJD operator +(long double, const SBMJD&);
  inline friend SBMJD operator +(double, const SBMJD&);
  inline friend SBMJD operator +(float, const SBMJD&);
  inline friend SBMJD operator -(const SBMJD&, const SBMJD&);
  inline friend SBMJD operator -(const SBMJD&, long double);
  inline friend SBMJD operator -(const SBMJD&, double);
  inline friend SBMJD operator -(long double, const SBMJD&);
  inline friend SBMJD operator -(int, const SBMJD&);

  inline friend QDataStream &operator<<(QDataStream&, const SBMJD&);
  inline friend QDataStream &operator>>(QDataStream&, SBMJD&);


  int		date() const {return Date;};
  long double	time() const {return Time;};
  /*-----------------------------------*/
  int		year ()		const;
  int		month()		const;
  int		hour()		const;
  int		min()		const;
  double	sec()		const;
  QString	month2Str()	const;
  int		day  ()		const;
  int		dayOfYear()	const;
  int		dayOfWeek()	const;
  QString	dayOfWeek2Str() const;
};
/*==============================================================================================*/


class SBEOP;
/*==============================================================================================*/
/*												*/
/* class SBTime											*/
/*												*/
/*==============================================================================================*/
class SBTime : public SBMJD
{
protected:
  double		fTAI_UTC;
  double		fUT1_UTC;
  double		fUT1_UT1R;
  double		fTDB_TDT;
  
  double		FArgs[6];
  double		Omega;

  virtual void		DefineTAI_UTC();
  virtual void		FundamentalArgs();
  virtual void		Zero();
  SBEOP			*EOP;
public:
  static  const double	fTT_TAI;
  virtual QString ClassName() const {return "SBTime";};
  
  SBTime(SBEOP* =NULL);
  SBTime(const SBTime&);
  virtual ~SBTime(){EOP=NULL;}; //SBTime is not an owner of SBEOP..

  void		setEOP(SBEOP* EOP_){EOP=EOP_;};
  SBEOP*	eop() {return EOP;};
    
  virtual double fArg(int i) const {return -1<i&&i<6?FArgs[i]:0.0;};
  virtual void   fArgs(double A[6]) const 
    {A[0]=FArgs[0]; A[1]=FArgs[1]; A[2]=FArgs[2]; A[3]=FArgs[3]; A[4]=FArgs[4]; A[5]=FArgs[5];};

  /* synonims: */
  virtual double fArg_l() const {return FArgs[0];};
  virtual double fArg_lPrime() const {return FArgs[1];};
  virtual double fArg_F() const {return FArgs[2];};
  virtual double fArg_D() const {return FArgs[3];};
  virtual double fArg_Omega() const {return FArgs[4];};
  virtual double fArg_theta() const {return FArgs[5];};
  virtual double omega() const {return Omega;};

  virtual void   setUTC(const SBMJD& UTC_);
  virtual double TAI_UTC() const {return fTAI_UTC;};
  virtual SBMJD  UTC() const {return *this;};
  virtual SBMJD  TAI() const {return *this + (SBMJD)fTAI_UTC;};
  virtual SBMJD  TT()  const {return *this + (SBMJD)(fTAI_UTC + fTT_TAI);};
  virtual SBMJD  UT1() const {return *this + (SBMJD)fUT1_UTC;};
  virtual SBMJD  TDB() const {return *this + (SBMJD)(fTAI_UTC + fTT_TAI + fTDB_TDT);};
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* class SBMJD inlines										*/
/*												*/
/*==============================================================================================*/
inline bool SBMJD::operator==(const SBMJD& MJD) const 
{
  return MJD.Date==Date && MJD.Time==Time;
};

inline bool SBMJD::operator!=(const SBMJD& MJD) const 
{
  return MJD.Date!=Date || MJD.Time!=Time;
};

inline bool SBMJD::operator<(const SBMJD& MJD) const 
{
  if (Date < MJD.Date) return TRUE;
  return Date == MJD.Date ? Time < MJD.Time : FALSE;
};

inline bool SBMJD::operator<=(const SBMJD& MJD) const 
{
  if (Date < MJD.Date) return TRUE;
  return Date == MJD.Date ? Time <= MJD.Time : FALSE;
};

inline bool SBMJD::operator>(const SBMJD& MJD) const 
{
  if (Date > MJD.Date) return TRUE;
  return Date == MJD.Date ? Time > MJD.Time : FALSE;
};

inline bool SBMJD::operator>(double d) const 
{
  return (*this)>SBMJD(d);
};

inline bool SBMJD::operator>=(const SBMJD& MJD) const 
{
  if (Date > MJD.Date) return TRUE;
  return Date == MJD.Date ? Time >= MJD.Time : FALSE;
};

inline SBMJD& SBMJD::operator=(const SBMJD& MJD)
{
  Date=MJD.Date; 
  Time=MJD.Time; 
  return *this;
};

inline SBMJD& SBMJD::operator= (long double MJD)
{
  Date = (int)floorl(MJD);
  Time = MJD - (long double) Date;
  return *this;
};

inline SBMJD& SBMJD::operator+=(const SBMJD& MJD)
{
  Date += MJD.Date; 
  Time += MJD.Time; 
  while (Time>=1.0) {Time--; Date++;};
  return *this;
};

inline SBMJD& SBMJD::operator-=(const SBMJD& MJD)
{
  Date -= MJD.Date; 
  Time -= MJD.Time; 
  while (Time<0.0){Time++; Date--;};
  return *this;
};

inline SBMJD operator +(const SBMJD& A, const SBMJD& B)
{
  return SBMJD(A)+=B;
};

inline SBMJD operator +(const SBMJD& A, long double b)
{
  return SBMJD(A)+=SBMJD(b);
};

inline SBMJD operator +(const SBMJD& A, double b)
{
  return SBMJD(A)+=SBMJD((long double)b);
};

inline SBMJD operator +(const SBMJD& A, float b)
{
  return SBMJD(A)+=SBMJD((long double)b);
};

inline SBMJD operator +(long double a, const SBMJD& B)
{
  return SBMJD(a)+=B;
};

inline SBMJD operator +(double a, const SBMJD& B)
{
  return SBMJD((long double)a)+=B;
};

inline SBMJD operator +(float a, const SBMJD& B)
{
  return SBMJD((long double)a)+=B;
};

inline SBMJD operator -(const SBMJD& A, const SBMJD& B)
{
  return SBMJD(A)-=B;
};

inline SBMJD operator -(const SBMJD& A, long double b)
{
  return SBMJD(A)-=SBMJD(b);
};

inline SBMJD operator -(const SBMJD& A, double b)
{
  return SBMJD(A)-=SBMJD((long double)b);
};

inline SBMJD operator -(long double a, const SBMJD& B)
{
  return SBMJD(a)-=B;
};

inline SBMJD operator -(int a, const SBMJD& B)
{
  return SBMJD((long double)a)-=B;
};

inline QDataStream &operator<<(QDataStream &s, const SBMJD &MJD)
{
  double f = (double)MJD.Time;
  return s << MJD.Date << f;
//  return s.writeRawBytes((const char*)&MJD.Time, sizeof(long double));
};

inline QDataStream &operator>>(QDataStream &s, SBMJD &MJD)
{
  double f=0.0;
  s >> MJD.Date >> f;
  MJD.Time=f;
  return s;
};
/*==============================================================================================*/



#endif //SB_GEO_TIME_H
