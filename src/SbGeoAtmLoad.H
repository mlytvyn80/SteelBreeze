/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 2003 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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
 *
 * Overview:	Atmospheric pressure loading.
 *		Ephemirides URL: 
 *			http://gemini.gsfc.nasa.gov/aplo/aplo.shtml
 *
 */

#ifndef SB_GEO_ATM_LOAD_H
#define SB_GEO_ATM_LOAD_H

#include <config.h>

#include "SbGeoStations.H"



class SBAploEntry;
/*==============================================================================================*/
/*												*/
/* class SBAploDRec										*/
/*												*/
/*==============================================================================================*/
class SBAploDRec : public SBMJD
{
private:
  Vector3		Displacement;
  SBAploEntry		*Owner;

public:
  // constructors/destructors:
  //
  SBAploDRec(SBAploEntry *Owner_) 
    : SBMJD(), Displacement(v3Zero){Owner=Owner_;};
  virtual ~SBAploDRec(){Owner=NULL;};
  virtual QString ClassName() const {return "SBAploDRec";};
  
  // interface:
  //
  const Vector3&	displacement() const {return Displacement;};
  void			setDisplacement(const Vector3& D_) {Displacement=D_;};

  // functions:
  //

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploDRec& DRec)
    {return s << (const SBMJD&)DRec << DRec.Displacement;};
  friend QDataStream &operator>>(QDataStream& s, SBAploDRec& DRec)
    {return s >> (SBMJD&)DRec >> DRec.Displacement;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploEntry										*/
/*												*/
/*==============================================================================================*/
class SBAploEntry : public SBNamed, public QList<SBAploDRec>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBMJD*)item1) <  (*(const SBMJD*)item2)?-1:
	(*(const SBMJD*)item1) == (*(const SBMJD*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBAploDRec*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBAploDRec(this); return s >> *(SBAploDRec*)Entry;};

private:
  SBStationID		ID;

public:
  // constructors/destructors:
  //
  SBAploEntry(const QString&, const SBStationID&);
  virtual ~SBAploEntry();
  virtual QString	ClassName() const {return "SBAploEntry";};

  // interface:
  //
  const SBStationID&	id() const {return ID;};
  void			setID(const SBStationID& ID_) {ID = ID_;};

  // functions:
  //
  

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploEntry& E)
    {return s << (const SBNamed&)E << (const QList<SBAploDRec>&)E << E.ID;};
  friend QDataStream &operator>>(QDataStream& s, SBAploEntry& E)
    {return s >> (SBNamed&)E >> (QList<SBAploDRec>&)E >> E.ID;};
  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploHeader										*/
/*												*/
/*==============================================================================================*/
class SBAploHeader
{
private:
  SBMJD			TStart;
  SBMJD			TFinis;
  double		Interval;	// "Sampling interval in days"

public:
  // constructors/destructors:
  //
  SBAploHeader();
  virtual ~SBAploHeader() {};
  virtual QString	ClassName() const {return "SBAploHeader";};

  // interface:
  //
  const SBMJD&		tStart() const {return TStart;};
  void			setTStart(const SBMJD& T_) {TStart = T_;};
  const SBMJD&		tFinis() const {return TFinis;};
  void			setTFinis(const SBMJD& T_) {TFinis = T_;};
  double		interval() const {return Interval;};
  void			setInterval(double dt_) {Interval = dt_;};

  // functions:
  //
  QString		name() {return TStart.toString(SBMJD::F_INTERNAL) + ".bin";};

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploHeader& H)
    {return s  << H.TStart << H.TFinis << H.Interval;};
  friend QDataStream &operator>>(QDataStream& s, SBAploHeader& H)
    {return s >> H.TStart >> H.TFinis >> H.Interval;};
  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploChunk										*/
/*												*/
/*==============================================================================================*/
class SBAploChunk : public QList<SBAploEntry>, public SBAploHeader
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBNamed*)item1) <  (*(const SBNamed*)item2)?-1:
	(*(const SBNamed*)item1) == (*(const SBNamed*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBAploEntry*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBAploEntry("noname", SidZero); return s >> *(SBAploEntry*)Entry;};

  QDict<SBAploEntry>	*EntryByID;
  QIntDict<SBAploEntry>	*EntryByDomes;
  void			fillDict();

public:
  // constructors/destructors:
  //
  SBAploChunk();
  virtual ~SBAploChunk();
  virtual QString	ClassName() const {return "SBAploChunk";};

  // interface:
  //
  const SBAploHeader&	header() const {return (const SBAploHeader&)*this;};
  

  // functions:
  //
  void			import(QTextStream&, SB_TRF*, bool&);
  SBAploEntry*		entryByID(const SBStationID&);

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploChunk& C)
    {return s << (const QList<SBAploEntry>&)C << (const SBAploHeader&)C;};
  friend QDataStream &operator>>(QDataStream& s, SBAploChunk& C)
    {s >> (QList<SBAploEntry>&)C >> (SBAploHeader&)C; C.fillDict(); return s; };
  friend QTextStream &operator<<(QTextStream&, const SBAploChunk&);
  friend QTextStream &operator>>(QTextStream&, SBAploChunk&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploWave										*/
/*												*/
/*==============================================================================================*/
class SBAploWave : public SBNamed
{
protected:
  double	Phase;
  double	Freq;
  double	Accel;

public:
  // constructors/destructors:
  //
  SBAploWave(const QString& N_) : SBNamed(N_) {Phase = Freq = Accel = 0.0;};
  virtual ~SBAploWave(){};
  virtual QString	ClassName() const {return "SBAploWave";};

  // interface:
  //
  double	phase() const {return Phase;};
  double	freq () const {return Freq;};
  double	accel() const {return Accel;};
  void		setPhase(double d_) {Phase=d_;};
  void		setFreq (double d_) {Freq =d_;};
  void		setAccel(double d_) {Accel=d_;};

  // functions:
  //
  double	angle(const SBMJD T_) 
    {double dt=(T_-TEphem)*86400.0; return Phase + Freq*dt + 0.5*Accel*dt*dt;}

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploWave& W)
    {return s << (const SBNamed&)W << W.Phase << W.Freq << W.Accel;};
  friend QDataStream &operator>>(QDataStream& s, SBAploWave& W)
    {return s >> (SBNamed&)W >> W.Phase >> W.Freq >> W.Accel;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploWaveList										*/
/*												*/
/*==============================================================================================*/
class SBAploWaveList : public QList<SBAploWave>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBNamed*)item1) <  (*(const SBNamed*)item2)?-1:
	(*(const SBNamed*)item1) == (*(const SBNamed*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBAploWave*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBAploWave("noname"); return s >> *(SBAploWave*)Entry;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploWaveAmpls									*/
/*												*/
/*==============================================================================================*/
class SBAploWaveAmpls : public SBNamed
{
protected:
  double	CosU;
  double	CosE;
  double	CosN;
  double	SinU;
  double	SinE;
  double	SinN;

public:
  // constructors/destructors:
  //
  SBAploWaveAmpls(const QString& N_) : SBNamed(N_) {CosU=CosE=CosN=SinU=SinE=SinN=0.0;};
  virtual ~SBAploWaveAmpls(){};
  virtual QString	ClassName() const {return "SBAploWaveAmpls";};

  // interface:
  //
  double	cosU() const {return CosU;};
  double	cosE() const {return CosE;};
  double	cosN() const {return CosN;};
  double	sinU() const {return SinU;};
  double	sinE() const {return SinE;};
  double	sinN() const {return SinN;};

  void		setCosU (double d_) {CosU=d_;};
  void		setCosE (double d_) {CosE=d_;};
  void		setCosN (double d_) {CosN=d_;};
  void		setSinU (double d_) {SinU=d_;};
  void		setSinE (double d_) {SinE=d_;};
  void		setSinN (double d_) {SinN=d_;};

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploWaveAmpls& A)
    {return s << (const SBNamed&)A << A.CosU << A.CosE << A.CosN << A.SinU << A.SinE << A.SinN;};
  friend QDataStream &operator>>(QDataStream& s, SBAploWaveAmpls& A)
    {return s >> (SBNamed&)A >> A.CosU >> A.CosE >> A.CosN >> A.SinU >> A.SinE >> A.SinN;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploSiteAmpl										*/
/*												*/
/*==============================================================================================*/
class SBAploSiteAmpl : public SBNamed, public QList<SBAploWaveAmpls>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBNamed*)item1) <  (*(const SBNamed*)item2)?-1:
	(*(const SBNamed*)item1) == (*(const SBNamed*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBAploWaveAmpls*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBAploWaveAmpls("noname"); return s >> *(SBAploWaveAmpls*)Entry;};


  QDict<SBAploWaveAmpls>	WavesByName;
  SBStationID			ID;

public:
  // constructors/destructors:
  //
  SBAploSiteAmpl(const QString& N_, const SBStationID& ID_) : SBNamed(N_) 
    {ID=ID_; WavesByName.setAutoDelete(FALSE);};
  virtual ~SBAploSiteAmpl(){};
  virtual QString	ClassName() const {return "SBAploSiteAmpl";};

  // interface:
  //
  const SBStationID&	id() const {return ID;};
  void			setID(const SBStationID& ID_) {ID = ID_;};

  // functions:
  //
  void			fillDict();
  SBAploWaveAmpls*	waveByName(const QString& Name_) {return WavesByName.find(Name_);};

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploSiteAmpl& A)
    {return s << (const SBNamed&)A << (const QList<SBAploWaveAmpls>&)A<< A.ID;};
  friend QDataStream &operator>>(QDataStream& s, SBAploSiteAmpl& A)
    {s >> (SBNamed&)A >> (QList<SBAploWaveAmpls>&)A >> A.ID; A.fillDict(); return s;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploSiteAmplList									*/
/*												*/
/*==============================================================================================*/
class SBAploSiteAmplList : public QList<SBAploSiteAmpl>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBNamed*)item1) <  (*(const SBNamed*)item2)?-1:
	(*(const SBNamed*)item1) == (*(const SBNamed*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBAploSiteAmpl*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBAploSiteAmpl("noname", SidZero); return s >> *(SBAploSiteAmpl*)Entry;};
};
/*==============================================================================================*/



class SBStationInfo;
class SBSpline;
/*==============================================================================================*/
/*												*/
/* class SBAploEphem										*/
/*												*/
/*==============================================================================================*/
class SBAploEphem : public QList<SBAploHeader>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	((const SBAploHeader*)item1)->tStart() <  ((const SBAploHeader*)item2)->tStart()?-1:
	((const SBAploHeader*)item1)->tStart() == ((const SBAploHeader*)item2)->tStart()?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBAploHeader*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBAploHeader; return s >> *(SBAploHeader*)Entry;};

private:
  SBMJD			TStart;
  SBMJD			TFinis;
  SBMJD			TPreparedStart;
  SBMJD			TPreparedFinis;
  double		Interval;
  bool			IsOK;

  //
  QList<SBStationID>	Sites;
  QDict<SBStationID>	SiteByID;
  QDict<SBSpline>	SplineByID;
  QIntDict<SBSpline>	SplineByDomes;

  //Harmonic variations:
  SBAploWaveList		Waves;
  QDict<SBAploWave>		WaveByName;
  SBAploSiteAmplList		SiteAmplitudes;
  QIntDict<SBAploSiteAmpl>	SiteAmplitudeByDomes;
  

  //
  void			checkEphem(bool = TRUE);
  QString		ext() const {return ".gz";};

public:
  // constructors/destructors:
  //
  SBAploEphem();
  virtual ~SBAploEphem();
  virtual QString	ClassName() const {return "SBAploEphem";};

  // interface:
  //
  const SBMJD&		tStart() const {return TStart;};
  void			setTStart(const SBMJD& T_) {TStart = T_;};
  const SBMJD&		tFinis() const {return TFinis;};
  void			setTFinis(const SBMJD& T_) {TFinis = T_;};
  double		interval() const {return Interval;};

  // functions:
  //
  bool			isOK() const {return IsOK;};
  bool			importEphem(const QString&, SB_TRF*, bool&);
  void			importHPS(const QString&, SB_TRF*);
  void			saveChunk(SBAploChunk*);
  void			loadChunk(SBAploHeader*, SBAploChunk*&);
  
  void			registerStation(SBStation*);
  void			getReady(const SBMJD&, const SBMJD&);
  void			dismissed();
  Vector3		displacement(const SBStationID&, const SBMJD&);
  SBSpline*		spline4station(const SBStationID& ID) 
    {return SplineByID.find(ID.toString());};

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBAploEphem& E)
    {return s << (const QList<SBAploHeader>&)E << E.TStart << E.TFinis << E.Waves << E.SiteAmplitudes;};
  friend QDataStream &operator>>(QDataStream& s, SBAploEphem& E)
    {s>>(QList<SBAploHeader>&)E>>E.TStart>>E.TFinis>>E.Waves>>E.SiteAmplitudes; E.checkEphem(); return s;};
};
/*==============================================================================================*/



#endif //SB_GEO_ATM_LOAD_H
