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

#ifndef SB_GEO_OBS_VLBI_STAT_H
#define SB_GEO_OBS_VLBI_STAT_H

#include <config.h>
#include <qintdict.h>

#include "SbGeoObsVLBI.H"
#include "SbGeoParameter.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"





/*==============================================================================================*/
/**
 *
 */
/*==============================================================================================*/
class SBObsVLBIStatRecord : public SBNamed, public SBAttributed
{
private:
  SBMJD			TStartTotal;
  SBMJD			TFinisTotal;
  SBMJD			TStartProc;
  SBMJD			TFinisProc;
  unsigned int		NumTotal;
  unsigned int		NumProc;
  QString		Aka;
  double		RMS;

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of object.
   */
  SBObsVLBIStatRecord(const QString& Name_="UNNAMED") : SBNamed(Name_), SBAttributed() {clear(); Aka=""; RMS=0.0;};
  /**A destructor. */
  virtual ~SBObsVLBIStatRecord(){};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBObsVLBIStatRecord";};

  // interface:
  //
  const SBMJD&		tStartTotal() const {return TStartTotal;};
  const SBMJD&		tFinisTotal() const {return TFinisTotal;};
  const SBMJD&		tStartProc()  const {return TStartProc;};
  const SBMJD&		tFinisProc()  const {return TFinisProc;};
  unsigned int		numTotal()    const {return NumTotal;};
  unsigned int		numProc()     const {return NumProc;};
  const QString		aka()	      const {return Aka;};
  double		rms()	      const {return RMS;};

  void			setTStartTotal(const SBMJD& T_) {TStartTotal=T_;};
  void			setTFinisTotal(const SBMJD& T_) {TFinisTotal=T_;};
  void			setTStartProc (const SBMJD& T_) {TStartProc=T_;};
  void			setTFinisProc (const SBMJD& T_) {TFinisProc=T_;};
  void			setNumTotal   (int N_) {NumTotal=N_;};
  void			setNumProc    (int N_) {NumProc =N_;};
  void			setAka	      (const QString& Str_) {Aka=Str_.copy();};
  void			setRMS	      (double RMS_) {RMS=RMS_;};

  void			incNumTotal   (int N_) {NumTotal+=N_;};
  void			incNumProc    (int N_) {NumProc +=N_;};

  // functions:
  //
  void			clear(){TStartTotal=TFinisTotal=TStartProc=TFinisProc=TZero; NumTotal=NumProc=0; RMS=0.0;};
  
  // I/O:
  //
  inline friend QDataStream &operator<<(QDataStream&, const SBObsVLBIStatRecord&);
  inline friend QDataStream &operator>>(QDataStream&, SBObsVLBIStatRecord&);
};
/**\relates SBObsVLBIStatRecord
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBObsVLBIStatRecord& SR)
{return s<<(const SBNamed&)SR<<(const SBAttributed&)SR<<SR.TStartTotal<<SR.TFinisTotal
	 <<SR.TStartProc<<SR.TFinisProc<<SR.NumTotal<<SR.NumProc<<SR.Aka<<SR.RMS;};

/**\relates SBObsVLBIStatRecord
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBObsVLBIStatRecord& SR)
{return s>>(SBNamed&)SR>>(SBAttributed&)SR>>SR.TStartTotal>>SR.TFinisTotal>>SR.TStartProc
	 >>SR.TFinisProc>>SR.NumTotal>>SR.NumProc>>SR.Aka>>SR.RMS;};
/*==============================================================================================*/



/*==============================================================================================*/
/**
 *
 */
/*==============================================================================================*/
class SBObsVLBIStatList : public QList<SBObsVLBIStatRecord>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return  *(SBNamed*)item1<*(SBNamed*)item2?-1:*(SBNamed*)item1==*(SBNamed*)item2?0:1;};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBObsVLBIStatRecord*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {return s >> *(SBObsVLBIStatRecord*)(Entry=new SBObsVLBIStatRecord("UNNAMED"));};
public:
  virtual QString ClassName() const {return "SBObsVLBIStatList";};
  virtual SBObsVLBIStatRecord* find(const SBNamed* item) 
    {
      QList<SBObsVLBIStatRecord>::find((const SBObsVLBIStatRecord*)item);
      return QList<SBObsVLBIStatRecord>::current();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/**
 *
 */
/*==============================================================================================*/
class SBObsVLBIStatSource : public SBObsVLBIStatRecord
{
private:
  SBObsVLBIStatList		StatPerSession;
  QString			Aka;

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of object.
   */
  SBObsVLBIStatSource(const QString& Name_, const QString& Aka_="unkn") : SBObsVLBIStatRecord(Name_)
    {Aka=Aka_.copy(); StatPerSession.setAutoDelete(TRUE);};
  /**A destructor. */
  virtual ~SBObsVLBIStatSource(){};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBObsVLBIStatSource";};

  // interface:
  //
  inline const QString&		aka() const {return Aka;};
  inline void			setAka(const QString& Aka_) {Aka=Aka_.copy();};
  inline SBObsVLBIStatList&	statPerSession() {return StatPerSession;};

  // functions:
  //

  // I/O:
  //
  inline friend QDataStream &operator<<(QDataStream&, const SBObsVLBIStatSource&);
  inline friend QDataStream &operator>>(QDataStream&, SBObsVLBIStatSource&);
};
/**\relates SBObsVLBIStatSource
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBObsVLBIStatSource& SS)
{return s<<(const SBObsVLBIStatRecord&)SS<<SS.Aka<<SS.StatPerSession;};

/**\relates SBObsVLBIStatSource
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBObsVLBIStatSource& SS)
{return s>>(SBObsVLBIStatRecord&)SS>>SS.Aka>>SS.StatPerSession;};
/*==============================================================================================*/



/*==============================================================================================*/
/**
 *
 */
/*==============================================================================================*/
class SBObsVLBIStatSrcList : public QList<SBObsVLBIStatSource>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return  *(SBNamed*)item1<*(SBNamed*)item2?-1:*(SBNamed*)item1==*(SBNamed*)item2?0:1;};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBObsVLBIStatSource*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {return s >> *(SBObsVLBIStatSource*)(Entry=new SBObsVLBIStatSource("UNNAMED"));};
public:
  virtual QString ClassName() const {return "SBObsVLBIStatList";};
  virtual SBObsVLBIStatSource* find(const SBNamed* item) 
    {
      QList<SBObsVLBIStatSource>::find((const SBObsVLBIStatSource*)item);
      return QList<SBObsVLBIStatSource>::current();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/**
 *
 */
/*==============================================================================================*/
class SBObsVLBIStatStation : public SBObsVLBIStatRecord
{
private:
  SBObsVLBIStatList		StatPerSession;
  QString			Aka;

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of object.
   */
  SBObsVLBIStatStation(const QString& Name_, const QString& Aka_="unkn") : SBObsVLBIStatRecord(Name_)
    {Aka=Aka_.copy(); StatPerSession.setAutoDelete(TRUE);};
  /**A destructor. */
  virtual ~SBObsVLBIStatStation(){};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBObsVLBIStatStation";};

  // interface:
  //
  inline const QString&		aka() const {return Aka;};
  inline void			setAka(const QString& Aka_) {Aka=Aka_.copy();};
  inline SBObsVLBIStatList&	statPerSession() {return StatPerSession;};

  // functions:
  //

  // I/O:
  //
  inline friend QDataStream &operator<<(QDataStream&, const SBObsVLBIStatStation&);
  inline friend QDataStream &operator>>(QDataStream&, SBObsVLBIStatStation&);
};
/**\relates SBObsVLBIStatStation
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBObsVLBIStatStation& SS)
{return s<<(const SBObsVLBIStatRecord&)SS<<SS.Aka<<SS.StatPerSession;};

/**\relates SBObsVLBIStatStation
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBObsVLBIStatStation& SS)
{return s>>(SBObsVLBIStatRecord&)SS>>SS.Aka>>SS.StatPerSession;};
/*==============================================================================================*/



/*==============================================================================================*/
/**
 *
 */
/*==============================================================================================*/
class SBObsVLBIStatStaList : public QList<SBObsVLBIStatStation>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return  *(SBNamed*)item1<*(SBNamed*)item2?-1:*(SBNamed*)item1==*(SBNamed*)item2?0:1;};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBObsVLBIStatStation*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {return s >> *(SBObsVLBIStatStation*)(Entry=new SBObsVLBIStatStation("UNNAMED"));};
public:
  virtual QString ClassName() const {return "SBObsVLBIStatList";};
  virtual SBObsVLBIStatStation* find(const SBNamed* item) 
    {
      QList<SBObsVLBIStatStation>::find((const SBObsVLBIStatStation*)item);
      return QList<SBObsVLBIStatStation>::current();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/**
 *
 */
/*==============================================================================================*/
class SBObsVLBIStatistics : public SBNamed
{
private:
  SBObsVLBIStatRecord		GeneralStatistics;
  SBObsVLBIStatSrcList		SourcesStatistics;
  SBObsVLBIStatStaList		StationsStatistics;
  bool				IsGlobal;

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of object.
   */
  SBObsVLBIStatistics(const QString&);
		     //    {SourcesStatistics.setAutoDelete(TRUE); StationsStatistics.setAutoDelete(TRUE); IsGlobal=FALSE;};
  /**A destructor. */
  virtual ~SBObsVLBIStatistics(){};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBObsVLBIStatistics";};

  // interface:
  //
  SBObsVLBIStatRecord&		generalStatistics() {return GeneralStatistics;};
  SBObsVLBIStatSrcList&		sourcesStatistics() {return SourcesStatistics;};
  SBObsVLBIStatStaList&		stationsStatistics(){return StationsStatistics;};
  bool				isGlobal() const {return IsGlobal;};
  void				seIsGlobal(bool Is_) {IsGlobal=Is_;};

  // functions:
  //
  void				collectStatistics(SBVLBISesInfoList* =NULL);


  // I/O:
  //
  inline friend QDataStream &operator<<(QDataStream&, const SBObsVLBIStatistics&);
  inline friend QDataStream &operator>>(QDataStream&, SBObsVLBIStatistics&);
};
/**\relates SBObsVLBIStatistics
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBObsVLBIStatistics& S)
{return s<<(const SBNamed&)S<<S.GeneralStatistics<<S.SourcesStatistics<<S.StationsStatistics;};

/**\relates SBObsVLBIStatistics
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBObsVLBIStatistics& S)
{return s>>(SBNamed&)S>>S.GeneralStatistics>>S.SourcesStatistics>>S.StationsStatistics;};
/*==============================================================================================*/





















#endif //SB_GEO_OBS_VLBI_STAT_H
