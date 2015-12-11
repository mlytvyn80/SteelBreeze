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

#ifndef SB_GENERAL_H
#define SB_GENERAL_H

#include <config.h>
#include <iostream>

#include <qdatastream.h>
//#include <qdict.h>
#include <qlist.h>
#include <qstring.h>


extern bool IsNeedToStop;

class SBLog;
class SBVersion;
 
extern const QString SBCompilerName;
extern const QString SBCompilerVersion;
extern const QString SBCompilerHostName;
extern const QString SBCompilerDomainName;
extern const QString SBCompilerUserName;
extern const QString SBCompilerLoginName;
extern const QString SBCompilerDate;
extern const QString SBCompilerTime;
extern const QString SBCompilerOS;

extern SBLog		*Log;
extern const SBVersion	Version;


#ifndef HAVE_SINCOS
inline void sincos(double X, double *SINX, double *COSX)
{
  *SINX=sin(X);
  *COSX=cos(X);
};
#endif



/*==============================================================================================*/
/*												*/
/* class SBLog											*/
/*												*/
/*==============================================================================================*/
class SBLog
{
public:
  enum LogLevel 
  {
    ERR,				//<! Log level: error
    WRN, 				//<! Log level: normal
    INF, 				//<! Log level: info
    DBG					//<! Log level: debug
  };
  enum LogFacility
  {
    IO		= 1<< 0,		//<! Log facility: Input/Output
    DATA	= 1<< 1,		//<! Log facility: observation processing
    TIME	= 1<< 2,		//<! Log facility: time calculation
    PRECESSION	= 1<< 3,		//<! Log facility: precession
    NUTATION	= 1<< 4,		//<! Log facility: nutation
    R3		= 1<< 5,		//<! Log facility: matrix rotation(GST)
    PMOTION	= 1<< 6,		//<! Log facility: polar motion
    PINTERP	= 1<< 7,		//<! Log facility: EOP's interpolation
    TIDES	= 1<< 8,		//<! Log facility: site's tide displacement
    AXISOFF	= 1<< 9,		//<! Log facility: axiss offset
    PLATES	= 1<<10,		//<! Log facility: plate motion
    REFRACT	= 1<<11,		//<! Log facility: refraction delay
    GRAVDELAY	= 1<<12,		//<! Log facility: grav. delay
    DELAYALL	= 1<<13,		//<! Log facility: delay calculation
    EPHEM	= 1<<14,		//<! Log facility: ephemerides
    ESTIMATOR	= 1<<15,		//<! Log facility: estimator
    CONFIG   	= 1<<16,		//<! Log facility: config
    INTERFACE	= 1<<17,		//<! Log facility: GUI
    SOURCE   	= 1<<18,		//<! Log facility: source
    STATION	= 1<<19,		//<! Log facility: station
    MATRIX3D	= 1<<20,		//<! Log facility: 3d matrix/3d vector
    MATRIX	= 1<<21,		//<! Log facility: matrix/vector
    RUN   	= 1<<22,		//<! Log facility: project executing
    STOCHPARS	= 1<<23,		//<! Log facility: stochastic parameters
    SPLINE	= 1<<24,		//<! Log facility: spline interpolation
    PREPROC	= 1<<25,		//<! Log facility: preprocessing
    RES_07	= 1<<26,		//<! Log facility: reserved
    RES_08	= 1<<27,		//<! Log facility: reserved
    RES_09	= 1<<28,		//<! Log facility: reserved
    RES_10	= 1<<29,		//<! Log facility: reserved
    TEST	= 1<<30,		//<! Log facility: for test purposes
    TEMPORARY	= 1<<31			//<! Log facility: temporary
  };


protected:
  bool			IsStoreInFile;
  bool			IsTimeLabel;
  QString		FileName;
  QString		TmpStr;
  QList<QString>	Spool;
  uint			LogFacilities[4];
  uint			Capacity;
  virtual void MakeOutput(const QString & s, bool){std::cout << s << "\n";};
public:

  SBLog(int=100, bool=TRUE, const QString& = "stbreeze.log");
  virtual ~SBLog();
  
  virtual void startUp();
  virtual void clearSpool();
  virtual void write(LogLevel, uint, const QString &, bool=FALSE);
  virtual bool isEligible(LogLevel Level_, uint Facility_) {return (LogFacilities[Level_] & Facility_);};

  virtual QString ClassName     () const       {return "SBLog";};
  virtual void setLogFileName   (const QString &Tmp){FileName=Tmp;};
  virtual void setCapacity      (int Tmp)      {Capacity = Tmp;};
  virtual void setStoreInFile   (bool isTmp)   {IsStoreInFile = isTmp;};
  virtual void setTimeLabel     (bool isTmp)   {IsTimeLabel = isTmp;};
  virtual void setLogFacilities (LogLevel Lvl, uint Tmp) {LogFacilities[Lvl] = Tmp;};
  
  virtual const QString& fileName() const	{return FileName;};
  virtual int		 capacity() const	{return Capacity;};
  virtual bool		 isStoreInFile() const	{return IsStoreInFile;};
  virtual bool		 isTimeLabel() const	{return IsTimeLabel;};
  virtual bool		 isEligible(LogLevel Lvl, uint F) const {return LogFacilities[Lvl]&F;};
  virtual uint		 logFacilities (LogLevel Lvl) const {return LogFacilities[Lvl];};
  virtual void		 addLogFacility(LogLevel Lvl, uint Tmp) {LogFacilities[Lvl] |= Tmp;};
  virtual void		 rmLogFacility (LogLevel Lvl, uint Tmp) {LogFacilities[Lvl] &= ~Tmp;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVersion										*/
/*												*/
/*==============================================================================================*/
class SBVersion
{
protected:
  int			MajorVersion;
  int			MinorVersion;
  int			TeenyVersion;
  static QString	SelfName;
public:
  SBVersion(int Major_=2, int Minor_=0, int Teeny_=0)
    : MajorVersion(Major_), MinorVersion(Minor_), TeenyVersion(Teeny_) {}
  
  QString toString() const
    {return QString().sprintf("%1d.%d.%d", MajorVersion, MinorVersion, TeenyVersion);};

  const QString&	selfName() const {return SelfName;};
  QString		name() const 
    {return SelfName + QString().sprintf("-%1d.%d.%d", MajorVersion, MinorVersion, TeenyVersion);};

  friend QDataStream &operator<<(QDataStream& s, const SBVersion& v)
    {return s<<v.MajorVersion<<v.MinorVersion<<v.TeenyVersion;};
  friend QDataStream &operator>>(QDataStream& s, SBVersion& v)
    {return s>>v.MajorVersion>>v.MinorVersion>>v.TeenyVersion;};
  inline bool operator==(const SBVersion& v) const 
    {
      return 
	(MajorVersion==v.MajorVersion) && (MinorVersion==v.MinorVersion) && 
	(TeenyVersion==v.TeenyVersion);
    };
  inline bool operator!=(const SBVersion& v) const {return !(*this==v);};
  inline bool operator<(const SBVersion& v) const 
    {
      return 
	1000*MajorVersion + MinorVersion + 0.0001*TeenyVersion < 
	1000*v.MajorVersion + v.MinorVersion + 0.0001*v.TeenyVersion;
    };
  inline bool operator<=(const SBVersion& v) const 
    {
      return 
	1000*MajorVersion+MinorVersion+0.0001*TeenyVersion <=
	1000*v.MajorVersion+v.MinorVersion+0.0001*v.TeenyVersion;
    };
  inline bool operator>(const SBVersion& v) const 
    {
      return 
	1000*MajorVersion+MinorVersion+0.0001*TeenyVersion >
	1000*v.MajorVersion+v.MinorVersion+0.0001*v.TeenyVersion;
    };
  inline bool operator>=(const SBVersion& v) const 
    {
      return 
	1000*MajorVersion+MinorVersion+0.0001*TeenyVersion >=
	1000*v.MajorVersion+v.MinorVersion+0.0001*v.TeenyVersion;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Class SBAttributed.
 * It describes features of classes with attributes. Other classes have to inherit this one.
 * 
 */
/*==============================================================================================*/
class SBAttributed
{
private:
  uint		Attr;			//!< storage for 32 different attributes (is it enough?)
public:
  /**A constructor. */
  SBAttributed(){Attr=0;};
  /**A constructor. */
  SBAttributed(const SBAttributed& A){Attr=A.Attr;};
  SBAttributed& operator= (const SBAttributed& A) {Attr=A.Attr; return *this;};
  /**A destructor. */
  virtual ~SBAttributed(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBAttributed";};
  
  // interface:
  //
  /**Returns the attributes.*/
  inline uint attributes() const {return Attr;};
  /**Sets up the attributes.*/
  inline void setAttributes(uint A_){Attr = A_;};
  /**Adds the attribute to the storage.*/
  inline void addAttr(uint A_){Attr|=  A_;};
  /**Deletes the attribute from the storage.*/
  inline void delAttr(uint A_){Attr&= ~A_;};
  /**Toggles the attribute in the storage.*/
  inline void xorAttr(uint A_){Attr^= A_;};
  /**Returns TRUE if the attribute is set.*/
  inline bool isAttr (uint A_) const {return Attr & A_;};
  /**Removes all attributes.*/
  inline void clearAll() {Attr=0;};
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAttributed& A){return s<<(uint)A.Attr;};
  friend QDataStream &operator>>(QDataStream& s, SBAttributed& A)      {return s>>(uint&)A.Attr;};
};
/*==============================================================================================*/


// wrapping for geo*
// (functions placed in the "SbIMainWin.C" file)
// message in a status bar
extern void mainMessage(const QString&);
// reset progress bar 
extern void mainPrBarReset();
// set progress bar total steps
extern void mainPrBarSetTotalSteps(int);
// set progress bar progress
extern void mainPrBarSetProgress(int);
// set current status
extern void mainSetStatus(const QString&);
// set current operation
extern void mainSetOperation(const QString&);
// set the name of the TRF database
extern void mainSetTRFName(const QString&);
// set the name of the CRF database
extern void mainSetCRFName(const QString&);
// set the name of the EOP database
extern void mainSetEOPName(const QString&);
// set the name of the project
extern void mainSetPrjName(const QString&);
// set the number of current processed sessions
extern void mainSetProcSes(int, int);
// set the number of current processed observations
extern void mainSetProcObs(int, int);
// set the name of the current session
extern void mainSetSessName(const QString&);
// set the sizes of caches
extern void mainSetCacheSizes(int, int, int, int);


#endif //SB_GENERAL_H
