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

#ifndef SB_GEO_OBS_VLBI_H
#define SB_GEO_OBS_VLBI_H

#include <config.h>
#include <qintdict.h>

#include "SbGeoObs.H"
#include "SbGeoParameter.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"



/*==============================================================================================*/
/*												*/
/* class SBDS_dat										*/
/*												*/
/* a data stream for i/o *.dat files								*/
/*==============================================================================================*/
class SBDS_dat : public QDataStream
{};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDS_aux										*/
/*												*/
/* a data stream for i/o *.aux files								*/
/*==============================================================================================*/
class SBDS_aux : public QDataStream
{};
/*==============================================================================================*/



class SBTS_ngs;
class SBDS_dbh;
/*==============================================================================================*/
/**Provides basic session's objects information for SBVLBISession.
 * Keeps necessary information on objects (stations, sources, etc) to make a reasonably
 * quick/usefull session management.
 */
/*==============================================================================================*/
class SBObjectObsInfo : public SBAttributed
{
private:
  short int			Idx;		//!< session's local index 
  unsigned int			Num;		//!< number of obs per object
  unsigned int			ProcNum;	//!< number of processed observations per object
  QString			Aka;		//!< an alias (how it's called in datafile)
  double			SWRMS2;		//!< sum of WRMS^2 per object
  double			SWeight;	//!< sum of weights per object
public:
  /**A constructor.
   * Creates a copy of object.
   * \param Idx_ session index of the object (default is 0);
   * \param Aka_ session alias of the object (default is "unkn").
   */
  SBObjectObsInfo(int Idx_=0, const QString& Aka_="unkn") 
    : SBAttributed() {Idx=Idx_;Num=ProcNum=0;Aka=Aka_.copy(); SWRMS2=SWeight=0.0;};
  /**A destructor. */
  virtual ~SBObjectObsInfo(){};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBObjectObsInfo";};

  /**Returns local index.*/
  inline short int idx() const {return Idx;};
  /**Returns number of observations.*/
  inline unsigned int num() const {return Num;};
  /**Returns number of processed observations.*/
  inline unsigned int procNum() const {return ProcNum;};
  /**Returns alias.*/
  inline const QString& aka() const {return Aka;};
  inline double wrms() const {return SWeight>0.0?sqrt(SWRMS2/SWeight):0.0;};

  /**Sets up index.*/
  inline void setIdx(int Idx_) {Idx=(short int)Idx_;};
  /**Sets up number of observations per object.*/
  inline void setNum(unsigned int Num_) {Num=Num_;};
  /**Sets up number of processed observations per object.*/
  inline void setProcNum(unsigned int ProcNum_) {ProcNum=ProcNum_;};
  /**Sets up alias.*/
  inline void setAka(const QString& Aka_) {Aka=Aka_.copy();};
  /**Increments number of observation by one.*/
  inline void incNum() {Num++;};
  /**Increments number of observation by one, adds RMS^2 to sum of (RMS^2).*/
  inline void addWRMS(double o_c_, double w_) {SWRMS2+=o_c_*o_c_*w_*w_; SWeight+=w_*w_; ProcNum++;};
  inline void clearRMSs() {SWRMS2=SWeight=0.0; ProcNum=0;};

  /* I/O: */
  inline friend QDataStream &operator<<(QDataStream&, const SBObjectObsInfo&);
  inline friend QDataStream &operator>>(QDataStream&, SBObjectObsInfo&);
};
/**\relates SBObjectObsInfo
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBObjectObsInfo& Oi)
{return s<<(const SBAttributed&)Oi<<Oi.Idx<<Oi.Num<<Oi.ProcNum<<Oi.Aka<<Oi.SWRMS2<<Oi.SWeight;};

/**\relates SBObjectObsInfo
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBObjectObsInfo& Oi)
{return s>>(SBAttributed&)Oi>>Oi.Idx>>Oi.Num>>Oi.ProcNum>>Oi.Aka>>Oi.SWRMS2>>Oi.SWeight;};
/*==============================================================================================*/



/*==============================================================================================*/
/**class SBParameterBreak, for describing a moment and a value of a break in a parameter.
 * 
 */
/*==============================================================================================*/
class SBParameterBreak : public SBMJD
{
private:
  double		B;
public:
  // constructors/destructors:
  //
  /**A constructor.*/
  inline SBParameterBreak() 
    : 
    SBMJD(TZero)
  {B=0.0;};
  /**A constructor.*/
  SBParameterBreak(const SBMJD& T_, double B_) : SBMJD(T_) {B=B_;};
  /**A destructor.*/
  virtual ~SBParameterBreak(){};
  SBParameterBreak& operator= (const SBParameterBreak& Br_) 
    {SBMJD::operator=(Br_); B=Br_.B; return *this;};
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName() const {return "SBParameterBreak";};

  // interface:
  //
  double		b() const {return B;};
  void			setB(double B_) {B=B_;};

  // functions:
  //
  double		bt(const SBMJD& T_) const {return *this>=T_?B:0.0;};

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBParameterBreak& B)
    {return s << (const SBMJD&)B << B.B;};
  friend QDataStream &operator>>(QDataStream& s, SBParameterBreak& B)
    {return s >> (SBMJD&)B >> B.B;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**class SBClockBreaksList, uses in the class SBStationInfo to list known breaks in
 * clocks of a station.
 */
/*==============================================================================================*/
class SBClockBreaksList : public QList<SBParameterBreak>
{
protected:
  virtual int		compareItems(QCollection::Item p1, QCollection::Item p2)
    {return *(const SBMJD*)p1<*(const SBMJD*)p2?-1:(*(const SBMJD*)p1==*(const SBMJD*)p2?0:1);};
  virtual QDataStream&	write(QDataStream& s, QCollection::Item B) const
    {return s << *(const SBParameterBreak*)B;};
  virtual QDataStream&	read(QDataStream& s, QCollection::Item &B)
    {B = new SBParameterBreak; return s >> *(SBParameterBreak*)B;};
  
public:
  /**A constructor.
   * Creates a copy of an object.
   */
  SBClockBreaksList() : QList<SBParameterBreak>() {setAutoDelete(TRUE);};
  /**A destructor.*/
  virtual ~SBClockBreaksList() {};
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBClockBreaksList";};
  
  // interface:
  //
  
  // functions:
  //
  double		bt(const SBMJD& T_)
    {double b=0.0; for (SBParameterBreak *br=first(); br; br=next()) b+=br->bt(T_); return b;};
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBClockBreaksList& L)
    {return s << (const QList<SBParameterBreak>&)L;};
  friend QDataStream &operator>>(QDataStream& s, SBClockBreaksList& L)
    {return s >> (QList<SBParameterBreak>&)L;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Provides station's info to the SBVLBISession.
 * Keeps station's information (SBStationID).
 */
/*==============================================================================================*/
class SBStationInfo : public SBStationID, public SBObjectObsInfo
{
public:
  /** Station's attributes.
   *  Describes station's behaviour related to the session.
   */
  enum Attributes
  {
    notValid   		= 1<<0,		//!< omit this station
    refClock		= 1<<1,		//!< station's clock is a reference
    BadCable		= 1<<2,		//!< station has wrong cable correction
    BadMeteo		= 1<<3,		//!< station has wrong meteo data
    EstCoo		= 1<<4,		//!< make an estimation of coordinates
    EstVel		= 1<<5,		//!< make an estimation of velocities
    ConstrCoo		= 1<<6,		//!< use station's coordinates to fix TRF
    ConstrVel		= 1<<7,		//!< use station's velocities to fix veloc.field
    ArtMeteo		= 1<<8,		//!< use artificial meteo paramaters
    HasClockBreak	= 1<<9,		//!< there is a break in clocks
    UseLocalClocks	= 1<<10,	//!< use local model of clocks
    UseLocalZenith	= 1<<11,	//!< use local model of zenith
    InvCable		= 1<<12,	//!< reserved
  };
  
private:
  double		Cl_V[10];	//!< clock model
  double		Cl_E[10];	//!< std.vars of clock model
  int			Cl_Order;	//!< actual number of used Cl_*s
  double		Zw;		//!< wet zenith delay
  double		ErrZw;		//!< std var. of wet zenith delay
  SBClockBreaksList	ClockBreaks;	//!< a list of epochs and values of clock breaks
  SBParameterCfg	Clocks;		//!< a descriptor of stoch.parameter for clocks variations
  SBParameterCfg	Zenith;		//!< a descriptor of stoch.parameter for wet z.delay variations

public:
  /**A constructor.
   * Creates a copy of object.
   * \param ID_  station ID (Domes Number);
   * \param Idx_ session index of the station (default is 0);
   * \param Aka_ session alias of the station (default is "unkn").
   */
  SBStationInfo(const SBStationID& ID_, int Idx_=0, const QString& Aka_="unkn");
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBStationInfo";};

  // interface:
  //
  double		cl_V(int i) const {return 0<=i&&i<10?Cl_V[i]:0.0;};
  double		cl_E(int i) const {return 0<=i&&i<10?Cl_E[i]:0.0;};
  int			cl_Order() const {return Cl_Order;};
  double		zw() const {return Zw;};
  double		errZw() const {return ErrZw;};
  SBClockBreaksList&	clockBreaks() {return ClockBreaks;};
  SBParameterCfg&	clocks() {return Clocks;};
  SBParameterCfg&	zenith() {return Zenith;};
  
  void			setCl_V(int i, double C_) {if (0<=i&&i<10) Cl_V[i] = C_;};
  void			setCl_E(int i, double E_) {if (0<=i&&i<10) Cl_E[i] = E_;};
  void			setCl_Order(int i) {if (2<i&&i<=10) Cl_Order=i;};
  void			clearClocks();
  void			setZw(double Z_) {Zw = Z_;};
  void			setErrZw(double E_) {ErrZw = E_;};

  // functions:
  //
  void			dumpUserInfo(QTextStream&);
  bool			restoreUserInfo(QTextStream&);
  double		clocksValue(double);
  
  /* I/O: */
  /**\relates SBStationInfo
   * Saves to the data stream.
   */
  friend QDataStream &operator<<(QDataStream&, const SBStationInfo&);
  /**\relates SBStationInfo
   * Loads from the data stream.
   */
  friend QDataStream &operator>>(QDataStream&, SBStationInfo&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**List of station infos.
 *
 */
/*==============================================================================================*/
class SBStationInfoList : public QList<SBStationInfo>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return *(SBStationInfo*)item1<*(SBStationInfo*)item2?-1:
	*(SBStationInfo*)item1==*(SBStationInfo*)item2?0:1;
    };

  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBStationInfo*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {return s >> *(SBStationInfo*)(Entry=new SBStationInfo(SBStationID()));};
public:
  virtual QString ClassName() const {return "SBStationInfoList";};
  virtual SBStationInfo* find(const SBStationID* id)
    {
      QList<SBStationInfo>::find((const SBStationInfo*)id);
      return QList<SBStationInfo>::current();
    };
  void		dumpUserInfo(QTextStream& ts)
    {if (count()) for (SBStationInfo *SI=first(); SI; SI=next()) SI->dumpUserInfo(ts);};
  bool		restoreUserInfo(QTextStream&);
  virtual void	clearRMSs() {if (count()) for (SBStationInfo *SI=first(); SI; SI=next()) SI->clearRMSs();};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Provides bases's info to the SBVLBISession.
 * Keeps bases's information.
 */
/*==============================================================================================*/
class SBBaseInfo : public SBNamed, public SBObjectObsInfo
{
public:
  /** Bases's attributes.
   *  Describes base's behaviour related to the session.
   */
  enum Attributes
  {
    notValid   		= 1<<0,		//!< omit this base
    Reserved_02 	= 1<<1,		//!< reserved
    Reserved_03		= 1<<2,		//!< reserved
    Reserved_04		= 1<<3		//!< reserved
  };
  
public:
  /**A constructor.
   * Creates a copy of object.
   * \param Name_  base's name;
   * \param Idx_ session index of the base (default is 0);
   * \param Aka_ session alias of the base (default is "unkn").
   */
  SBBaseInfo(const QString& Name_1, const QString& Name_2, int Idx_=0, 
	     const QString& Aka_1="unkn1", const QString& Aka_2="unkn2")
    : SBNamed(Name_1 + ":" + Name_2), SBObjectObsInfo(Idx_, Aka_1 + ":" + Aka_2){};
  SBBaseInfo(const SBBaseInfo& BI, int Idx_)
    : SBNamed(BI.name()), SBObjectObsInfo(Idx_, BI.aka()){};
  
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBBaseInfo";};

  // functions:
  //
  void			dumpUserInfo(QTextStream&);
  bool			restoreUserInfo(QTextStream&);

  /* I/O: */
  inline friend QDataStream &operator<<(QDataStream&, const SBBaseInfo&);
  inline friend QDataStream &operator>>(QDataStream&, SBBaseInfo&);
};

/**\relates SBBaseInfo
 * Saves to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBBaseInfo& B)
{return s<<(const SBNamed&)B<<(const SBObjectObsInfo&)B;};

/**\relates SBBaseInfo
 * Loads from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBBaseInfo& B)
{return s>>(SBNamed&)B>>(SBObjectObsInfo&)B;};
/*==============================================================================================*/



/*==============================================================================================*/
/**List of bases infos.
 *
 */
/*==============================================================================================*/
class SBBaseInfoList : public QList<SBBaseInfo>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return  *(SBBaseInfo*)item1<*(SBBaseInfo*)item2?-1:*(SBBaseInfo*)item1==*(SBBaseInfo*)item2?0:1;};
  //    {return  *(SBNamed*)item1<*(SBNamed*)item2?-1:*(SBNamed*)item1==*(SBNamed*)item2?0:1;};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBBaseInfo*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {return s >> *(SBBaseInfo*)(Entry=new SBBaseInfo("Q","Q"));};
public:
  virtual QString ClassName() const {return "SBBaseInfoList";};
  virtual SBBaseInfo* find(const SBNamed* item)
    {
      QList<SBBaseInfo>::find((const SBBaseInfo*)item);
      return QList<SBBaseInfo>::current();
    };
  void		dumpUserInfo(QTextStream& ts)
    {if (count()) for (SBBaseInfo *BI=first(); BI; BI=next()) BI->dumpUserInfo(ts);};
  bool		restoreUserInfo(QTextStream&);
  virtual void	clearRMSs() {if (count()) for (SBBaseInfo *BI=first(); BI; BI=next()) BI->clearRMSs();};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Provides source's info to the SBVLBISession.
 * Keeps sources's information.
 */
/*==============================================================================================*/
class SBSourceInfo : public SBNamed, public SBObjectObsInfo
{
public:
  /** Source's attributes.
   *  Describes source's behaviour related to session.
   */
  enum Attributes
  {
    notValid   		= 1<<0,		//!< omit this source
    EstCoo		= 1<<1,		//!< make an estimation of coordinates
    ConstrCoo		= 1<<2,		//!< use source's coordinates to fix CRF
    Reserved_03 	= 1<<3,		//!< reserved
    Reserved_04		= 1<<4,		//!< reserved
    Reserved_05		= 1<<5		//!< reserved
  };
public:
  /**A constructor.
   * Creates a copy of object.
   * \param Name_  the name of the source;
   * \param Idx_ session index of the source (default is 0);
   * \param Aka_ session alias of the source (default is "unkn").
   */
  SBSourceInfo(const QString& Name_, int Idx_=0, const QString& Aka_="unkn") 
    : SBNamed(Name_), SBObjectObsInfo(Idx_, Aka_) {addAttr(EstCoo); /*addAttr(ConstrCoo);*/};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBSourceInfo";};

  // functions:
  //
  void			dumpUserInfo(QTextStream&);
  bool			restoreUserInfo(QTextStream&);

  /* I/O: */

  inline friend QDataStream &operator<<(QDataStream&, const SBSourceInfo&);
  inline friend QDataStream &operator>>(QDataStream&, SBSourceInfo&);
};

/**\relates SBSourceInfo
 * Saves object to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBSourceInfo& Sinfo)
{return s<<(const SBNamed&)Sinfo<<(const SBObjectObsInfo&)Sinfo;};

/**\relates SBSourceInfo
 * Loads object from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBSourceInfo& Sinfo)
{return s>>(SBNamed&)Sinfo>>(SBObjectObsInfo&)Sinfo;};
/*==============================================================================================*/



/*==============================================================================================*/
/**List of source infos.
 *
 */
/*==============================================================================================*/
class SBSourceInfoList : public QList<SBSourceInfo>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return  *(SBSourceInfo*)item1<*(SBSourceInfo*)item2?-1:
	*(SBSourceInfo*)item1==*(SBSourceInfo*)item2?0:1;};
  //    {return  *(SBNamed*)item1<*(SBNamed*)item2?-1:*(SBNamed*)item1==*(SBNamed*)item2?0:1;};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBSourceInfo*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {return s >> *(SBSourceInfo*)(Entry=new SBSourceInfo("Q"));};
public:
  virtual QString ClassName() const {return "SBSourceInfoList";};
  virtual int idx(const SBNamed* item) {return QList<SBSourceInfo>::find((const SBSourceInfo*)item);};
  virtual SBSourceInfo* find(const SBNamed* item) 
    {
      QList<SBSourceInfo>::find((const SBSourceInfo*)item); 
      return QList<SBSourceInfo>::current();
    };
  void		dumpUserInfo(QTextStream& ts)
    {if (count()) for (SBSourceInfo *SI=first(); SI; SI=next()) SI->dumpUserInfo(ts);};
  bool		restoreUserInfo(QTextStream&);
  virtual void	clearRMSs() {if (count()) for (SBSourceInfo *SI=first(); SI; SI=next()) SI->clearRMSs();};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**History list.
 * Keeps sorted by date list of history entries SBHistory::SBHistoryEntry.
 */
/*==============================================================================================*/
class SBHistory : public SBDList
{
public:
  /**History entry.
   * Consist of date of creation, version and history text line. 
   * The idea is stolen from MkIII DBH format.
   */
  class SBHistoryEntry : public SBDated
  {
    int			Version;	//!< Datafile's version when the entry was entered.
    QString		Text;		//!< Text of the history line.
  public:
    /**A constructor.
     * Creates a copy of object.
     * \param Version_ version;
     * \param MJD_ date of history;
     * \param Text_ text.
     */
    SBHistoryEntry(int Version_=0, const SBMJD& MJD_=TZero, const QString& Text_="") 
      : SBDated(MJD_){Version=Version_; Text=Text_.copy();};
    /**Returns history's text. */
    const QString&	text() 	 {return Text;};
    /**Returns history's version. */
    int			version(){return Version;};
    friend QDataStream &operator<<(QDataStream& s, const SBHistoryEntry& E)
      {return s<<(const SBDated&)E<<E.Version<<E.Text;};
    friend QDataStream &operator>>(QDataStream& s, SBHistoryEntry& E)
      {return s>>(SBDated&)E>>E.Version>>E.Text;};
  };
  
protected:
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBHistoryEntry*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBHistoryEntry; return s >> *(SBHistoryEntry*)Entry;};
  
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBHistory() {setAutoDelete(TRUE);};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBHistory";};

  // interface:
  /**Adds a new history line.
   * \param Version_ version;
   * \param Text_ text;
   * \param MJD_ date of creation, MJD (default is current system time).
   */
  void addHistoryLine(int Version_, const QString& Text_, const SBMJD& MJD_=SBMJD::currentMJD())
    {inSort(new SBHistoryEntry(Version_, MJD_, Text_));};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**One record of the VLBI observation.
 * 
 */
/*==============================================================================================*/
class SBObsVLBIEntry : public SBObservation
{
  friend class SBVLBISession;
public:
  /**Observation's attributes.
   */
  enum Attributes
  {
    //notValid 		= 1<<0,		//!< omit the entry;
    breakClock1 	= 1<<1,		//!< there is a clock break at Station #1;
    breakClock2		= 1<<2,		//!< there is a clock break at Station #2;
    BadMeteo1		= 1<<3,		//!< there is strange thing about meteo data @1
    BadMeteo2		= 1<<4,		//!< there is strange thing about meteo data @2
    BadCable1		= 1<<5,		//!< there is strange thing about cable correction @1
    BadCable2		= 1<<6,		//!< there is strange thing about cable correction @2
    processed		= 1<<7		//!< observation has been processed
  };
  
private:
  // ----------- OBSERVATION (i.e., an input) -----------
  //  int		Idx;			//!< index
  short int	Station_1;		//!< Station#1
  short int	Station_2;		//!< Station#2
  short int	Source;			//!< Source
  double	Delay;			//!< observed delay, sec
  double	DelayErr;		//!< formal error for observed delay
  double	Rate;			//!< observed delayrate, sec/sec
  double	RateErr;		//!< formal error for observed delay rate
  char		QualCode[2];		//!< quality code
  int		DUFlag;			//!< delay unweight flag
  int		RUFlag;			//!< rate unweight flag
  float		Correlation;		//!< correlation coefficient
  double	FringePhase;		//!< total fringe phase (deg)
  float		Cable_1;		//!< cable calib correction (one-way) for site 1, sec
  float		Cable_2;		//!< cable calib correction (one-way) for site 2, sec
  float		AtmTemp_1;		//!< ambient atmospheric temp at site 1, C
  float		AtmTemp_2;		//!< ambient atmospheric temp at site 2, C
  float		AtmPress_1;		//!< ambient atmospheric pressure at site 1, mb
  float		AtmPress_2;		//!< ambient atmospheric pressure at site 2, mb
  float		AtmHum_1;		//!< ambient atmospheric humidity at site 1
  float		AtmHum_2;		//!< ambient atmospheric humidity at site 2
  //
  double	DelayIon;		//!< delay ionosphere correction, sec
  double	DelayIonErr;		//!< delay ionosphere correction formal error
  double	RateIon;		//!< delay rate ionosphere correction, sec/sec
  double	RateIonErr;		//!< delay rate ionosphere correction formal error
  //
  double	RefFreq;		//!< Reference frequency (MHz)
  double	GrDelayAmbig;		//!< Group delay ambiguity spacing (sec)

  int		IonErrorFlag;		//!< ionosphere error flag
  int		AmbiguityFactor;	//!< group delay ambiguity factor
  //
  // aux data:
  double	DelayRes;		//!< delay residual
  double	RateRes;		//!< delay rate residual
  // alternates: (if observed meteo are incorrect)
  float		AltAtmTemp_1;		//!< alternate ambient atmospheric temp at site 1, C
  float		AltAtmTemp_2;		//!< alternate ambient atmospheric temp at site 2, C
  float		AltAtmPress_1;		//!< alternate ambient atmospheric pressure at site 1, mb
  float		AltAtmPress_2;		//!< alternate ambient atmospheric pressure at site 2, mb
  float		AltAtmHum_1;		//!< alternate ambient atmospheric humidity at site 1
  float		AltAtmHum_2;		//!< alternate ambient atmospheric humidity at site 2
  // calcables:
  double	ZenithDelay_1;		//!< calculated+est zenith delay at site 1 (just for curiosity)
  double	ZenithDelay_2;		//!< calculated+est zenith delay at site 2 (just for curiosity)

  // vars:
  SBVLBISession*	Owner;		//!< an owner of the observation;
    
  // for estimator's interface:
  static SBVector	VLBI_Obs2SRIF;
  static SBVector	VLBI_Sig2SRIF;

public:
  // constructors/destructors:
  //
  SBObsVLBIEntry(SBVLBISession*);
  SBObsVLBIEntry(const SBObsVLBIEntry&);
  SBObsVLBIEntry& operator= (const SBObsVLBIEntry&);
  virtual ~SBObsVLBIEntry(){};
  virtual QString ClassName() const {return "SBObsVLBIEntry";};

  // interface:
  //
  //  inline int		idx()		const {return Idx;};
  inline short int	station_1Idx()	const {return Station_1;};
  inline short int	station_2Idx()	const {return Station_2;};
  inline short int	sourceIdx()	const {return Source;};
  inline double		delay()		const {return Delay;};
  inline double		delayErr()	const {return DelayErr;};
  inline double		rate()		const {return Rate;};
  inline double		rateErr()	const {return RateErr;};
  inline QString	qualCode()	const {return QString().sprintf("%c%c", QualCode[0], QualCode[1]);};
	 int		quality()	const;
  inline int		dUFlag()	const {return DUFlag;};
  inline int		rUFlag()	const {return RUFlag;};
  inline float		correlation()	const {return Correlation;};
  inline double		fringePhase()	const {return FringePhase;};
  inline float		cable_1()	const {return Cable_1;};
  inline float		cable_2()	const {return Cable_2;};
  inline float		atmTemp_1()	const {return AtmTemp_1;};
  inline float		atmTemp_2()	const {return AtmTemp_2;};
  inline float		atmPress_1()	const {return AtmPress_1;};
  inline float		atmPress_2()	const {return AtmPress_2;};
  inline float		atmHum_1()	const {return AtmHum_1;};
  inline float		atmHum_2()	const {return AtmHum_2;};
  inline double		delayIon()	const {return DelayIon;};
  inline double		delayIonErr()	const {return DelayIonErr;};
  inline double		rateIon()	const {return RateIon;};
  inline double		rateIonErr()	const {return RateIonErr;};
  inline double		refFreq()	const {return RefFreq;};
  inline double		grDelayAmbig()	const {return GrDelayAmbig;};
  inline int		ionErrorFlag()	const {return IonErrorFlag;};
  inline int		ambiguityFactor() const {return AmbiguityFactor;};
  // 
  inline double		delayRes()	const {return DelayRes;};
  inline double		rateRes()	const {return RateRes;};
  inline float		altAtmTemp_1()	const {return AltAtmTemp_1;};
  inline float		altAtmTemp_2()	const {return AltAtmTemp_2;};
  inline float		altAtmPress_1()	const {return AltAtmPress_1;};
  inline float		altAtmPress_2()	const {return AltAtmPress_2;};
  inline float		altAtmHum_1()	const {return AltAtmHum_1;};
  inline float		altAtmHum_2()	const {return AltAtmHum_2;};
  inline double		zenithDelay_1()	const {return ZenithDelay_1;};
  inline double		zenithDelay_2()	const {return ZenithDelay_2;};
  SBVLBISession*	owner()		const {return Owner;};

  void			setAltAtmTemp_1(double t_) {AltAtmTemp_1 = t_;};
  void			setAltAtmTemp_2(double t_) {AltAtmTemp_2 = t_;};
  void			setAltAtmPress_1(double p_) {AltAtmPress_1 = p_;};
  void			setAltAtmPress_2(double p_) {AltAtmPress_2 = p_;};
  void			setAltAtmHum_1(double h_) {AltAtmHum_1 = h_;};
  void			setAltAtmHum_2(double h_) {AltAtmHum_2 = h_;};
  void			setAmbiguityFactor(int j) {AmbiguityFactor=j;};

  void			incAmbigFactor(int j=1) {AmbiguityFactor+=j; DelayRes+=j*GrDelayAmbig;};
  void			decAmbigFactor(int j=1) {AmbiguityFactor-=j; DelayRes-=j*GrDelayAmbig;};

  //
  inline virtual bool operator==(const SBObsVLBIEntry& O) const;
  virtual bool operator!=(const SBObsVLBIEntry& O) const {return !(O==*this);};  
  inline virtual bool operator<(const SBObsVLBIEntry& O) const;
  virtual int cmp(const SBObservation& O) const 
    {
      return *this<(const SBObsVLBIEntry&)O?-1:*this==(const SBObsVLBIEntry&)O?0:1;
    };

  // functions:
  //
  virtual bool		isEligible(SBRunManager*);
  virtual bool		check() const
    {return Station_1>-1&&Station_2>-1&&Source>-1&&*(SBMJD*)this!=TZero;};
  virtual void		process(SBRunManager*);
  //  virtual SBVector	O_C(SBRunManager*);
  virtual const SBVector& O_C  ();
  virtual const SBVector& Sigma();

  inline float		ambientT_1(bool IsArtMeteo) const {return isAttr(BadMeteo1)||IsArtMeteo?AltAtmTemp_1:AtmTemp_1;};
  inline float		ambientT_2(bool IsArtMeteo) const {return isAttr(BadMeteo2)||IsArtMeteo?AltAtmTemp_2:AtmTemp_2;};
  inline float		ambientP_1(bool IsArtMeteo, double scale = 1.0) const {return isAttr(BadMeteo1)||IsArtMeteo?AltAtmPress_1*scale:AtmPress_1;};
  inline float		ambientP_2(bool IsArtMeteo, double scale = 1.0) const {return isAttr(BadMeteo2)||IsArtMeteo?AltAtmPress_2*scale:AtmPress_2;};
  inline float		ambientH_1(bool IsArtMeteo) const {return isAttr(BadMeteo1)?AltAtmHum_1||IsArtMeteo:AtmHum_1;};
  inline float		ambientH_2(bool IsArtMeteo) const {return isAttr(BadMeteo2)?AltAtmHum_2||IsArtMeteo:AtmHum_2;};

  inline SBStationInfo* station1Info() const;
  inline SBStationInfo* station2Info() const;
  inline SBSourceInfo*  sourceInfo  () const;

  inline SBStation*	station1(const SB_TRF*, SBStationInfo*&) const;
  inline SBStation*	station2(const SB_TRF*, SBStationInfo*&) const;
  inline SBSource*	source  (const SB_CRF*, SBSourceInfo*&) const;

  

  // I/O:
  //
  friend SBDS_dat &operator<<(SBDS_dat&, const SBObsVLBIEntry&);
  friend SBDS_dat &operator>>(SBDS_dat&, SBObsVLBIEntry&);
  friend SBDS_aux &operator<<(SBDS_aux&, const SBObsVLBIEntry&);
  friend SBDS_aux &operator>>(SBDS_aux&, SBObsVLBIEntry&);
  friend SBDS_dat &operator<<(SBDS_dat&, const SBVLBISession&);
  friend SBDS_dat &operator>>(SBDS_dat&, SBVLBISession&);
  friend SBDS_aux &operator<<(SBDS_aux&, const SBVLBISession&);
  friend SBDS_aux &operator>>(SBDS_aux&, SBVLBISession&);

  // [import/export]:
  friend SBTS_ngs &operator>>(SBTS_ngs&, SBVLBISession&);
  friend SBDS_dbh &operator>>(SBDS_dbh&, SBVLBISession&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISesInfo										*/
/*												*/
/*==============================================================================================*/
class SBVLBISesInfo : public SBNamed, public SBAttributed
{
  friend class SBVLBISet;
public:
  enum Attributes
  {
    preProc		= 1<<0,		//!< session is preprocessed
    notValid	 	= 1<<1,		//!< omit this session from the project
    BadCable		= 1<<2,		//!< has at least one station without cable correction
    BadAllCables	= 1<<3,		//!< has all stations without cable correction
    BadMeteo		= 1<<4,		//!< has at least one station without meteo data
    BadAllMeteos	= 1<<5,		//!< has all stations without meteo data
    HasClockBreak	= 1<<6,		//!< there is/are `clock break(s)'
    ConstrEOP		= 1<<7,
    ArtMeteo		= 1<<8,		//!< has at least one station with artificial meteo data
    ArtAllMeteos	= 1<<9 		//!< has all stations with artificial meteo data
  };
  enum ObsImportedFrom	{IF_NGS=0, IF_VBD=1, IF_DBH =2, IF_UNKN=3};
  enum ObsDelayType	{DT_GR =0, DT_PH =1, DT_UNKN=2};
  
protected:
  enum File_Type	{FT_DAT=0, FT_AUX=1, FT_UNKN=2};
  
  ObsImportedFrom	ImportedFrom;	//!< origin of the session;
  SBMJD			DateCreat;	//!< date of MkIII creation (just for curiosity);
  SBMJD			TStart;		//!< first epoch of the observations;
  SBMJD			TFinis;		//!< last epoch of the observations;
  SBMJD			TMean;		//!< mean epoch of the observations;
  QString		SubKey;		//!< aux key;
  QString		NetworkID;	//!< Network ID (a key);
  QString		ExpDescr;	//!< Experiment description;
  QString		UserFlag;	//!< Flag assigned by user
  QString		OfficialName;	//!< Official Name (from MasterFile)
  QString		Code;		//!< Code (from MasterFile)
  QString		CorrelatorName;	//!< Correlator Name (from MasterFile)
  ObsDelayType		DelayType;	//!< type of delay;
  int			NumSta;
  int			NumBas;
  int			NumSou;
  int			NumObs;
  double		DelayRMS2;
  double		RateRMS2;
  double		DelaySumW2;
  double		RateSumW2;
  int			Version;	//!< version;
  int			LocalVersion;
  
  QString		fileName(File_Type=FT_DAT);
  void			checkPath();
  virtual inline void calcTMean	(){TMean = (TStart + TFinis)/2.0;};


public:
  SBVLBISesInfo(const QString& ="70JAN01ZZ");
  SBVLBISesInfo(const SBVLBISesInfo&);
  SBVLBISesInfo& operator= (const SBVLBISesInfo&);
  
  virtual QString ClassName() const {return "SBVLBISesInfo";};
  
  // interface:
  ObsImportedFrom	importedFrom()	const {return ImportedFrom;};
  const SBMJD&		dateCreat()	const {return DateCreat;};
  const SBMJD&		tStart()	const {return TStart;};
  const SBMJD&		tFinis()	const {return TFinis;};
  const SBMJD&		tMean()		const {return TMean;};
  double		tInterval()	const {return TFinis - TStart;};
  int			version()	const {return Version;};
  int			localVersion()  const {return LocalVersion;};
  const QString&	subKey()	const {return SubKey;};
  const QString&	networkID()	const {return NetworkID;};
  const QString&	expDescr()	const {return ExpDescr;};
  const QString&	userFlag()	const {return UserFlag;};
  const QString&	officialName()	const {return OfficialName;};
  const QString&	code()		const {return Code;};
  const QString&	correlatorName()const {return CorrelatorName;};
  ObsDelayType		delayType()	const {return DelayType;};
  virtual int		numSta()	const {return NumSta;};
  virtual int		numBas()	const {return NumBas;};
  virtual int		numSou()	const {return NumSou;};
  virtual int		numObs()	const {return NumObs;};
  double		delayRMS2()	const {return DelayRMS2;};
  double		rateRMS2()	const {return RateRMS2;};
  double		delaySumW2()	const {return DelaySumW2;};
  double		rateSumW2()	const {return RateSumW2;};
  
  void setImportedFrom	(ObsImportedFrom ImportedFrom_) {ImportedFrom = ImportedFrom_;};
  void setTStart	(SBMJD		TStart_)	{TStart	  = TStart_;};
  void setTFinis	(SBMJD		TFinis_)	{TFinis	  = TFinis_;};
  void setTMean		(SBMJD		TMean_)		{TMean	  = TMean_;};
  void setDateCreat	(SBMJD		DateCreat_)	{DateCreat= DateCreat_;};
  void setVersion	(int		V_)		{Version=V_;};
  void setSubKey	(QString	SubKey_)	{SubKey   = SubKey_.copy();};
  void setNetworkID	(QString	NetworkID_)	{NetworkID= NetworkID_.copy();};
  void setExpDescr	(QString	ExpDescr_ )	{ExpDescr = ExpDescr_.copy();};
  void setUserFlag	(QString	UserFlag_ )	{UserFlag = UserFlag_.copy();};
  void setOfficialName	(QString	OfficialName_ )	{OfficialName = OfficialName_.copy();};
  void setCode		(QString	Code_ )		{Code	  = Code_.copy();};
  void setCorrelatorName(QString	CName_ )	{CorrelatorName = CName_.copy();};
  void setDelayType	(ObsDelayType	DelayType_)	{DelayType= DelayType_;};
  void setDelayRMS2	(double		DelayRMS_)	{DelayRMS2 = DelayRMS_;};
  void setRateRMS2	(double		RateRMS_)	{RateRMS2  = RateRMS_;};
  void setDelaySumW2	(double		d_)		{DelaySumW2= d_;};
  void setRateSumW2	(double		d_)		{RateSumW2 = d_;};
  void addDelayRMS	(double	o_c_, double w_)	{DelayRMS2+= o_c_*o_c_*w_*w_;  DelaySumW2+=w_*w_;};
  void addRateRMS	(double	o_c_, double w_)	{RateRMS2 += o_c_*o_c_*w_*w_;  RateSumW2 +=w_*w_;};

  virtual void clearRMSs() {DelayRMS2=RateRMS2=DelaySumW2=RateSumW2=0.0;};
  
  virtual bool operator==(const SBVLBISesInfo& I) const {return I==name();};

  /* functions: */
  // only local version can be changed:
  virtual void		incVersion(){LocalVersion++;};

  double		dWRMS()	const {return DelaySumW2>0.0?sqrt(DelayRMS2/DelaySumW2):0.0;};
  double		rWRMS()	const {return RateSumW2 >0.0?sqrt(RateRMS2 /RateSumW2 ):0.0;};

  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBVLBISesInfo&);
  friend QDataStream &operator>>(QDataStream&, SBVLBISesInfo&);
  
  friend SBDS_dat &operator<<(SBDS_dat&, const SBVLBISesInfo&);
  friend SBDS_dat &operator>>(SBDS_dat&, SBVLBISesInfo&);
  friend SBDS_aux &operator<<(SBDS_aux&, const SBVLBISesInfo&);
  friend SBDS_aux &operator>>(SBDS_aux&, SBVLBISesInfo&);
 };
/*==============================================================================================*/



/*==============================================================================================*/
/**List of session infos ordered by date of a first observation.
 * 
 */
/*==============================================================================================*/
class SBVLBISesInfoList : public QList<SBVLBISesInfo>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    //    {return *(SBNamed*)item1<*(SBNamed*)item2?-1:*(SBNamed*)item1==*(SBNamed*)item2?0:1;};
    {
      SBVLBISesInfo *s1 = (SBVLBISesInfo*)item1, *s2 = (SBVLBISesInfo*)item2;
      return s1->tStart()<s2->tStart()?-1:
	s1->tStart()==s2->tStart()?(s1->tFinis()<s2->tFinis()?-1:(s1->tFinis()==s2->tFinis()?
								  (cmp(*s1,*s2)):1)):1;};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const 
    {return s << *(SBVLBISesInfo*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry) 
    {return s >> *(SBVLBISesInfo*)(Entry=new SBVLBISesInfo);};
};
typedef QListIterator<SBVLBISesInfo>	SBVLBISesInfoIterator;
/*==============================================================================================*/



typedef QList<SBObsVLBIEntry>		SBObsVLBIList;
typedef QListIterator<SBObsVLBIEntry>	SBObsVLBIListIterator;
/*==============================================================================================*/
/**A session of VLBI observation.
 * This is a list of SBObsVLBIEntry objects, sorted by date. Keeps information on stations and
 * sources participated in the observations.
 */
/*==============================================================================================*/
class SBVLBISession : public SBObsVLBIList, public SBVLBISesInfo
{
  friend class SBVLBISet;
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
  {return *(SBObservation*)item1<*(SBObservation*)item2?-1:
      *(SBObservation*)item1==*(SBObservation*)item2?0:1;};
  
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const 
  {return (QDataStream&)((SBDS_dat&)s << *(SBObsVLBIEntry*)Entry);};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
  {return (QDataStream&)((SBDS_dat&)s >> *(SBObsVLBIEntry*)(Entry=new SBObsVLBIEntry(this)));};
  
  // data:
  SBHistory			DBHHistory;	//!< DBH History
  SBHistory			LocalHistory;	//!< local History
  
  SBStationInfoList		StationList;
  QIntDict<SBStationInfo>	*StationByIdx;
  QDict<SBStationInfo>		*StationByAka;
  
  SBSourceInfoList		SourceList;
  QIntDict<SBSourceInfo>	*SourceByIdx;
  QDict<SBSourceInfo>		*SourceByAka;
  
  SBBaseInfoList		BaseList;
  QIntDict<SBBaseInfo>		*BaseByIdx;
  QDict<SBBaseInfo>		*BaseByAka;
  
  double			RefFreq;	//!< Reference frequency (MHz)
  double			GrDelayAmbig;	//!< Group delay ambiguity spacing (sec)
  QTextStream			*Dump;
  
  virtual void			calcTMean();
  SBObsVLBIEntry*		parseObsDumpString(const QString&, SBMJD&, 
						   SBStationInfo*&, SBStationInfo*&, SBSourceInfo*&);
  
public:
  // constructors/destructors:
  //
  SBVLBISession();
  virtual ~SBVLBISession();
  virtual QString ClassName() const {return "SBVLBISession";};
  
  // interface:
  //
  double		refFreq()	const {return RefFreq;};
  double		grDelayAmbig()	const {return GrDelayAmbig;};
  virtual int		numSta()	const {return StationList.count();};
  virtual int		numBas()	const {return BaseList.count();};
  virtual int		numSou()	const {return SourceList.count();};
  virtual int		numObs()	const {return count();};
  
  // if supplied version greater then local, then upgrade local version value:
  void			addHistoryLine(const QString& T_, int V_=-1)
  {LocalHistory.addHistoryLine(V_==-1?localVersion():(LocalVersion=std::max(V_,LocalVersion)), T_);};
  SBHistory*		dbhHistory()	{return &DBHHistory;};
  SBHistory*		localHistory()	{return &LocalHistory;}; 
  SBObsVLBIEntry*	first()		{return SBObsVLBIList::first();};
  SBObsVLBIEntry*	next()		{return SBObsVLBIList::next();};
  
  void			setRefFreq     (double RefFreq_)     {RefFreq=RefFreq_;};
  void			setGrDelayAmbig(double GrDelayAmbig_){GrDelayAmbig=GrDelayAmbig_;};
  
  virtual bool operator==(const SBVLBISession& S) const {return SBVLBISesInfo::operator==(S);};
  
  // functions:
  //
  void			fillDicts();
  void			checkArtMeteo();
  void			checkAttributres();
  SBStationInfoList*	stationList(){return &StationList;};
  SBBaseInfoList*	baseList()   {return &BaseList;}
  SBSourceInfoList*	sourceList() {return &SourceList;}
  
  SBStationInfo*	stationInfo(short int   Idx_) const {return StationByIdx->find((long)Idx_);};
  SBStationInfo*	stationInfo(const char* Aka_) const {return StationByAka->find(Aka_);};
  SBBaseInfo*		baseInfo   (short int   Idx_) const {return BaseByIdx->find((long)Idx_);};
  SBBaseInfo*		baseInfo   (const char* Aka_) const {return BaseByAka->find(Aka_);};
  SBSourceInfo*		sourceInfo (short int   Idx_) const {return SourceByIdx->find((long)Idx_);};
  SBSourceInfo*		sourceInfo (const char* Aka_) const {return SourceByAka->find(Aka_);};
  void			setDump(QTextStream* Dump_){Dump=Dump_;};
  QTextStream*		dump(){return Dump;};
  virtual void		clearRMSs()
    { 
      SBVLBISesInfo::clearRMSs(); 
      StationList.clearRMSs(); 
      BaseList.clearRMSs(); 
      SourceList.clearRMSs();
    };
  void			dumpUserInfo(const QString&);
  bool			restoreUserInfo(const QString&);

  
  // I/O:
  //
  friend SBDS_dat &operator<<(SBDS_dat&, const SBVLBISession&);
  friend SBDS_dat &operator>>(SBDS_dat&, SBVLBISession&);
  friend SBDS_aux &operator<<(SBDS_aux&, SBVLBISession&);
  friend SBDS_aux &operator>>(SBDS_aux&, SBVLBISession&);
 
  // [export/import]:
  //
  friend SBTS_ngs &operator<<(SBTS_ngs&, const SBVLBISession&);
  friend SBTS_ngs &operator>>(SBTS_ngs&, SBVLBISession&);
  friend SBDS_dbh &operator<<(SBDS_dbh&, const SBVLBISession&);
  friend SBDS_dbh &operator>>(SBDS_dbh&, SBVLBISession&);

  friend SBTS_SINEX	&operator<<(SBTS_SINEX&, SBVLBISession&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMasterRecord										*/
/*												*/
/*==============================================================================================*/
class SBMasterRecord : public SBNamed, public SBAttributed
{
public:
  enum Attributes
  {
    Available		= 1<<0,		//!< session is present in the database
    Intensive	 	= 1<<1,		//!< session is INT
    Exists		= 1<<2,		//!< the session is present in the current database
    Reserved_01		= 1<<3		//!< reserved #1
  };
  
protected:
  int			Yr;		//!< Two last digits of a year of observation;
  QString		SessionName;	//!< name of an entry as it appeared in master file;
  QString		SessionCode;	//!< 'The official six (6) letter code name for the experiment';
  QString		SessionDate;	//!< session date;
  QString		CorrelatorName;	//!< correlator;
  QString		DBC;		//!< 'The two letter database code';

public:
  /**A constructor.
   */
  SBMasterRecord();
  SBMasterRecord(int, const QString&, const QString&, const QString&, const QString&, const QString&);
  SBMasterRecord(const SBMasterRecord& R) : SBNamed("77MBR44AA"), SBAttributed() {*this=R;};
  SBMasterRecord& operator= (const SBMasterRecord&);
  /**A destructor. */
  virtual ~SBMasterRecord(){};
  
  virtual QString ClassName() const {return "SBMasterRecord";};
  
  // interface:
  int			yr()		const {return Yr;};
  const QString&	sessionName()	const {return SessionName;};
  const QString&	sessionCode()	const {return SessionCode;};
  const QString&	sessionDate()	const {return SessionDate;};
  const QString&	correlatorName()const {return CorrelatorName;};
  const QString&	dbc()		const {return DBC;};
  void setYr(int Yr_){Yr = Yr_;};
  void setSessionName(const QString& SessionName_) {SessionName = SessionName_.copy();};
  void setSessionCode(const QString& SessionCode_) {SessionCode = SessionCode_.copy();};
  void setSessionDate(const QString& SessionDate_) {SessionDate = SessionDate_.copy();};
  void setCorrelatorName(const QString& CorrelatorName_) {CorrelatorName = CorrelatorName_.copy();};
  void setDBC        (const QString& DBC_)        {DBC = DBC_.copy();};

  /* functions */

  /* I/O: */
  friend QDataStream &operator<<(QDataStream& s, const SBMasterRecord& R)
    {return s<<(const SBNamed&)R<<(const SBAttributed&)R
	     <<R.Yr<<R.SessionName<<R.SessionCode<<R.SessionDate<<R.CorrelatorName<<R.DBC;};
  friend QDataStream &operator>>(QDataStream& s, SBMasterRecord& R)
    {return s>>(SBNamed&)R>>(SBAttributed&)R
	     >>R.Yr>>R.SessionName>>R.SessionCode>>R.SessionDate>>R.CorrelatorName>>R.DBC;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMasterFile										*/
/*												*/
/*==============================================================================================*/
class SBMasterFile : public SBNamed, public SBAttributed, public QList<SBMasterRecord>
{
public:
  enum Attributes
  {
    Intensive	 	= 1<<0,		//!< session is INT
    Reserved_1		= 1<<1		//!< reserved #1
  };
  
protected:
  int			Yr;		//!< Two last digits of a year of observation;
  SBMJD			TModified;

  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return cmp(*(SBMasterRecord*)item1, *(SBMasterRecord*)item2);};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const 
    {return s << *(SBMasterRecord*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry) 
    {return s >> *(SBMasterRecord*)(Entry=new SBMasterRecord);};
  
public:
  /**A constructor.
   */
  SBMasterFile();
  SBMasterFile(int, const QString&, const SBMJD&);
  SBMasterFile(const SBMasterFile& F) 
    : SBNamed("vasia.txt"), SBAttributed(), QList<SBMasterRecord>(){setAutoDelete(TRUE); *this=F;};
  SBMasterFile& operator= (const SBMasterFile&);
  /**A destructor. */
  virtual ~SBMasterFile(){};
  
  virtual QString ClassName() const {return "SBMasterFile";};
  
  // interface:
  int			yr()		const {return Yr;};
  const SBMJD&		tModified()	const {return TModified;};
  void			setYr(int Yr_)        {Yr = Yr_;};
  void			setTModified(const SBMJD& TModified_){TModified = TModified_;};

  /* functions */

  /* I/O: */
  friend QDataStream &operator<<(QDataStream& s, const SBMasterFile& F)
    {return s << (const SBNamed&)F << (const SBAttributed&)F << (const QList<SBMasterRecord>&)F 
	      << F.Yr << F.TModified;};
  friend QDataStream &operator>>(QDataStream& s, SBMasterFile& F)
    {return s >> (SBNamed&)F >> (SBAttributed&)F >> (QList<SBMasterRecord>&)F >> F.Yr >> F.TModified;};
  friend QTextStream &operator>>(QTextStream&, SBMasterFile&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMaster										*/
/*												*/
/*==============================================================================================*/
class SBMaster : public QList<SBMasterFile>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return cmp(*(SBMasterFile*)item1, *(SBMasterFile*)item2);};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const 
    {return s << *(SBMasterFile*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry) 
    {return s >> *(SBMasterFile*)(Entry=new SBMasterFile);};

  QDict<SBMasterRecord>		*RecordByName;
  QDict<SBMasterFile>		*FileByName;
  void				mapRecords();
  void				mapFiles();
  
public:
  /**A constructor.
   */
  SBMaster();
  /**A destructor. */
  virtual ~SBMaster();
  
  virtual QString ClassName() const {return "SBMaster";};
  
  // interface:
  /* functions */
  SBMasterRecord*	lookupMR(const QString&);
  void			importMF(const QString&);
  void			checkNewFiles(const QString&);

  /* I/O: */
  friend QDataStream &operator<<(QDataStream& s, const SBMaster& M)
    {return s << (const QList<SBMasterFile>&)M;};
  friend QDataStream &operator>>(QDataStream& s, SBMaster& M)
    {s >> (QList<SBMasterFile>&)M; M.mapFiles(); M.mapRecords(); return s;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**List of sessions.
 * 
 */
/*==============================================================================================*/
class SBVLBISet : public SBVLBISesInfoList
{
public:
  class Network : public SBNamed
  {
    class QStringList : public QList<QString>
    {
      virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
	{
	  if (*(QString*)item1==*(QString*)item2) return 0; 
	  return (*(QString*)item1 < (*(QString*)item2).data())?-1:1;
	};
      virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
	{return s << *(const QString*)Entry;};
      virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
	{return s >> *(QString*)(Entry=new QString);};
    };
    
    QString		Description;
    QStringList 	Keys;
  public:
    Network(const QString& ="noname", const QString& ="undescribed", QList<QString>* =NULL);
    Network(Network& N_) : SBNamed() {Keys.setAutoDelete(TRUE); *this=N_;};
    ~Network(){Keys.clear();};
    Network& operator=(Network&);
    const QString&		descr(){return Description;};
    void			setDescr(const QString& Description_){Description=Description_;};
    QList<QString>*		keys(){return &Keys;};
    int				lookup(const QString&);
    void			addKey(const QString& Key){Keys.inSort(new QString(Key));};
    friend QDataStream &	operator<<(QDataStream &s, const Network& N)
      {return s << (const SBNamed&)N << N.Description << N.Keys;};
    friend QDataStream &	operator>>(QDataStream &s, Network& N)
      {return s >> (SBNamed&)N >> N.Description >> N.Keys;};
  };
  class NetList : public QList<Network>
  {
    virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
      {
	if (*(SBNamed*)item1==*(SBNamed*)item2) return 0; 
	return (*(SBNamed*)item1 < *(SBNamed*)item2)?-1:1;
      };
    virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
      {return s << *(Network*)Entry;};
    virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
      {Entry = new Network; return s >> *(Network*)Entry;};
  public:
    QString		lookup(const QString&);
    NetList&		operator=(NetList&);
  };
  
protected:
  QDict<SBVLBISesInfo>		*SessionByName;
  NetList			Networks;
  SBMaster			*Master;
  void				fillDicts();
  unsigned int			TotalNumObs;

public:
  SBVLBISet();
  ~SBVLBISet();

  virtual QString ClassName() const {return "SBVLBISet";};
  
  // interface:
  unsigned int		totalObs() const {return TotalNumObs;};
  NetList*		networks(){return &Networks;};
  SBMaster*		master() {return Master;};

  /* functions: */
  SBVLBISesInfo*	import(SBVLBISession::ObsImportedFrom, const QString&, bool, int);
  void			loadMaster();
  void			releaseMaster(){if (Master) {delete Master; Master=NULL;};};


  //* returns SBVLBISesInfo* coresponds to the session ID. */
  SBVLBISesInfo*	sessionInfo(const QString& Name_) const {return SessionByName->find(Name_);};
  SBVLBISession*	loadSession(const QString&, SBVLBISession* =NULL) const;
  SBVLBISession*	saveSession(SBVLBISession*) const;
  void			removeSession(const QString&);
  //  void			dumpUserInfo(void(*hook)(int));
  void			dumpUserInfo();
  void			restoreUserInfo();

  
  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBVLBISet&);
  friend QDataStream &operator>>(QDataStream&, SBVLBISet&);
};
/*==============================================================================================*/


/**\relates SBObsVLBIEntry
 * Compares two instances of SBObsVLBIEntry.
 */
inline bool SBObsVLBIEntry::operator==(const SBObsVLBIEntry& O) const
{
  return SBObservation::operator==(O) && Owner->name()==O.owner()->name();
};

/**\relates SBObsVLBIEntry
 * Compares two instances of SBObsVLBIEntry.
 */
inline bool SBObsVLBIEntry::operator<(const SBObsVLBIEntry& O) const
{
  return
    SBMJD::operator< (O)?TRUE:
    (
     SBMJD::operator==(O)?
     (
      tech()<O.tech()?TRUE:
      (
       tech()==O.tech()?
       (
	Owner->name()<O.owner()->name()?TRUE:
	(
	 Owner->name()==O.owner()->name()?idx()<O.idx():FALSE
	 )
	):FALSE
       )
      ):FALSE
     );
};

/**\relates SBObsVLBIEntry
 * Returns Station Info for station#1.
 */
inline SBStationInfo* SBObsVLBIEntry::station1Info() const 
{
  return owner()?owner()->stationInfo(Station_1):NULL;
};

/**\relates SBObsVLBIEntry
 * Returns Station corresponded to station#1.
 */
inline SBStation* SBObsVLBIEntry::station1(const SB_TRF* TRF, SBStationInfo*& StInfo) const
{
  if (!(StInfo=station1Info()))
    {
      Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, ClassName() + 
		 ": cannot find the station_1 #" + QString().setNum(Station_1) + " in the session's dict");
      return NULL;
    };
  return TRF?TRF->find(*StInfo):NULL;
};

/**\relates SBObsVLBIEntry
 * Returns Station Info for station#2.
 */
inline SBStationInfo* SBObsVLBIEntry::station2Info() const 
{
  return owner()?owner()->stationInfo(Station_2):NULL;
};

/**\relates SBObsVLBIEntry
 * Returns Station corresponded to station#2.
 */
inline SBStation* SBObsVLBIEntry::station2(const SB_TRF* TRF, SBStationInfo*& StInfo) const
{
  if (!(StInfo=station2Info()))
    {
      Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, ClassName() + 
		 ": cannot find the station_2 #" + QString().setNum(Station_1) + " in the session's dict");
      return NULL;
    };
  return TRF?TRF->find(*StInfo):NULL;
};

/**\relates SBObsVLBIEntry
 * Returns Source Info for source.
 */
inline SBSourceInfo*  SBObsVLBIEntry::sourceInfo() const 
{
  return owner()?owner()->sourceInfo(Source):NULL;
};

/**\relates SBObsVLBIEntry
 * Returns Source.
 */
inline SBSource* SBObsVLBIEntry::source(const SB_CRF* CRF, SBSourceInfo*& SrInfo) const
{
  if (!(SrInfo=sourceInfo()))
    {
      Log->write(SBLog::ERR, SBLog::SOURCE | SBLog::DELAYALL, ClassName() + 
		 ": cannot find the source #" + QString().setNum(Source) + " in the session's dict");
      return NULL;
    };
  return CRF?CRF->find(SrInfo->name()):NULL;
};





#endif //SB_GEO_OBS_VLBI_H
