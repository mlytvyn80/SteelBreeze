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
 *
 * Overview:	describes interface of classes related to LE/DE Ephemerides.
 *		see ftp://navigator.jpl.nasa.gov/pub/ephem/export/README
 */

#ifndef SB_GEO_EPHEM_H
#define SB_GEO_EPHEM_H

#include <config.h>
#include "SbGeneral.H"
#include "SbVector3.H"
#include "SbGeo.H"
#include "SbGeoTime.H"



/*==============================================================================================*/
/**Celestial body.
 *
 * 
 */
/*==============================================================================================*/
class SBCelestBody : public SBNamed, public SBAttributed
{
  friend class SBEphem;
public:
  enum Attributes
  {
    IsEligible		= (1<<0),	//!< take this body into account
    IsGravDelay		= (1<<1),	//!< calc gravitation delay for this body
  };

protected:
  Vector3	R;		//!< coordinate
  Vector3	V;		//!< velocity
  Vector3	A;		//!< acceleration
  double	GM;		//!< G*M, [m**3/s**2];
  SBMJD		TCurrent;	//!< ready for this date;
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBCelestBody(const QString& Name_="Unnamed CB", double GM_=0.0) 
    : SBNamed(Name_), SBAttributed(), TCurrent(TZero) {setAttributes(IsEligible); GM=GM_;};
  /** A destructor. */
  virtual ~SBCelestBody(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBCelestBody";};
  virtual const Vector3& r() const {return R;};
  virtual const Vector3& v() const {return V;};
  virtual const Vector3& a() const {return A;};
  virtual	double gm() const {return GM;};
  virtual void setGM(double GM_){GM=GM_;};
  virtual const SBMJD&	tCurrent() const {return TCurrent;};
  virtual void setTCurrent(const SBMJD& TC_){TCurrent = TC_;};
};
/*==============================================================================================*/


class SBConfig;
/*==============================================================================================*/
/**Ephemerides (general).
 * Describes genereal interface to ephemerides.
 * 
 */
/*==============================================================================================*/
class SBEphem : public SBNamed
{
protected:
  const SBConfig	*Cfg;
  SBCelestBody		Mercury;	//!< Mercury
  SBCelestBody		Venus;		//!< Venus
  SBCelestBody		Earth;		//!< Earth
  SBCelestBody		Mars;		//!< Mars
  SBCelestBody		Jupiter;	//!< Jupiter
  SBCelestBody		Saturn;		//!< Saturn
  SBCelestBody		Uran;		//!< Uran
  SBCelestBody		Neptun;		//!< Neptun
  SBCelestBody		Pluto;		//!< Pluto
  SBCelestBody		Moon;		//!< Moon
  SBCelestBody		Sun;		//!< Sun
  SBCelestBody		GeoMoon;	//!< Geocentric Moon
  SBCelestBody		*Solary[11];
  SBMJD			TStart;		//!< time of the first entry of ephemerides
  SBMJD			TFinis;		//!< time of the last entry of ephemerides
  SBMJD			TBlStart;	//!< time of block's begin;
  SBMJD			TBlFinis;	//!< time of block's end;
  FILE*			EphemFile;	//!< file containing data;
  bool			IsOK;
  double		*buff;		//!< buffer for a record;
  double		DT;		//!< a record's interval;
  unsigned int		NRecCoef;	//!< number of coefficients per record;
  int			NumRecs;	//!< number of records;
  long			FileOffset;	//!< file offset;
  double		AU;		//!< number of meters per astronomical unit
  double		CLight;		//!< Speed of light (m/s)
  double		EMRat;		//!< Earth-Moon mass ratio
  struct Entry
  {
    int		Offset;			//!< starting location in each data record of the coeff.
    int		NCoef;			//!< number of chebychev coefficients per component
    int		NSets;			//!< num. of complete sets of coeff. in each data record
  };

  const Entry		*Loc;
  static const Entry	LocAbstract[13];

  /**A constructor. */
  SBEphem(const SBConfig*, const QString& ="LE/DEXXX general interface");
  /**Allocates buffer for one record, then calls openFile(). */
  virtual void		initData();
  /**Addition check of data. */
  virtual void		checkData(){IsOK=TRUE;};
  /**Opens binary file and checks it. */
  virtual void		openFile();
  /**Calculates file offset for a corresponding time. */
  virtual long		calcOffset(const SBMJD &T)
    {return TStart<=T&&T<=TFinis?(long)((T - TStart)/dt()):-1L;};
  /**Makes interpolation of a vector state for specified body. */
  virtual void		interp(double, int, int, int, SBCelestBody&);

public:
  /**A destructor. */
  virtual ~SBEphem();
  /**Refers to a class name (debug info) */
  virtual QString	ClassName() const {return "SBEphem";};
  //
  /**Returns a file name. */
  virtual QString	fileName() const {return "dumb.bin";};
  /**Calcs states of the celestial bodies for corresponded date. */
  virtual void		calc(const SBMJD&);
  /**Calcs state of the i-th celestial body for corresponded date. */
  virtual void		calcI(int, const SBMJD&);
  /**Makes data imortation from the ASCII file. */
  virtual bool		import(const QString&);
  //
  /**Returns time interval of a record. */
  virtual double	dt() const {return DT;};
  /**Returns number of meters per Astronomical unit used in this Ephemerides. */
  virtual double	au() const {return AU;};
  /**Returns speed of light used in this Ephemerides. */
  virtual double	cLight() const {return CLight;};
  /**Returns Earth-Moon mass ratio used in this Ephemerides. */
  virtual double	emRat() const {return EMRat;};

  /**Returns Mercury state. */
  const SBCelestBody&	mercury() {return Mercury;};
  /**Returns Venus state. */
  const SBCelestBody&	venus() {return Venus;};
  /**Returns Earth (not E-M baricenter) state. */
  const SBCelestBody&	earth() {return Earth;};
  /**Returns Mars state. */
  const SBCelestBody&	mars() {return Mars;};
  /**Returns Jupiter state. */
  const SBCelestBody&	jupiter() {return Jupiter;};
  /**Returns Saturn state. */
  const SBCelestBody&	saturn() {return Saturn;};
  /**Returns Uranus state. */
  const SBCelestBody&	uran() {return Uran;};
  /**Returns Neptune state. */
  const SBCelestBody&	neptun() {return Neptun;};
  /**Returns Pluto state. */
  const SBCelestBody&	pluto() {return Pluto;};
  /**Returns Moon state (relative to the Solar System Barycenter). */
  const SBCelestBody&	moon() {return Moon;};
  /**Returns Moon state (relative to the Geocenter). */
  const SBCelestBody&	geoMoon() {return GeoMoon;};
  /**Returns Sun state. */
  const SBCelestBody&	sun() {return Sun;};
  /**Returns i-th body state relative to the SSB
   * i: 0(Mercury), 1(Venus), 2(Earth), 3(Mars), 4(Jupiter), 5(Saturn), 
   * 6(Uranus), 7(Neptune), 8(Pluto), 9(Moon), 10(Sun).
   */
  const SBCelestBody&	body(int i) {return *Solary[(-1<i&&i<11)?i:10];};

  /**Returns number of coefficients (+2 time stamps). */
  virtual unsigned int	nRecCoef() const {return NRecCoef;};
  /**Returns the first available MJD. */
  const SBMJD&		tStart(){return	TStart;};
  /**Returns the last available MJD. */
  const SBMJD&		tFinis(){return TFinis;};
  /**Returns TRUE if data is OK. */
  bool			isOK(){return IsOK;};
  /**Returns number of records stored in the file. */
  int			numRecs(){return NumRecs;};
};
/*==============================================================================================*/


//well, we intently separated algorithm for LE/DE ephemerides into different classes to 
//ensure that writers will check forthcoming models by the hand.. 
//
/*==============================================================================================*/
/**Ephemerides LE/DE200.
 * LE/DE200 model implementation.
 * from nav's README (ftp://nav.jpl.nasa.gov/pub/ephem/export/README):
 *"DE200 : (includes nutations but not librations)
 *
 *      JED 2305424.5 (1599 DEC 09)  to  JED 2513360.5  (2169 MAR 31)
 *
 *      This ephemeris has been the basis of the Astronomical Almanac since 
 *      1984.  It is based upon the dynamical equator and equinox of J2000 
 *      (see Standish, 1982 and Standish, 1990)."
 */
/*==============================================================================================*/
class SBEphemDE200 : public SBEphem
{
protected:
  static const Entry	LocDE200[13];
public:
  /**A constructor. */
  SBEphemDE200(const SBConfig*);
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName()	const {return "SBEphemDE200";};
  /**Returns LE/DE200 file name.*/
  virtual QString	fileName()	const {return "lede200.bin";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ephemerides LE/DE403.
 * LE/DE403 model implementation.
 * from nav's README (ftp://nav.jpl.nasa.gov/pub/ephem/export/README):
 *"DE403 : (includes nutations but not librations)
 *
 *      JED 2433264.5 (1949 DEC 14)  to  JED 2469808.5 (2050 JAN 02)
 *
 *      Kept available since it has been a standard for the IERS and has
 *      been used by others in the past."
 */
/*==============================================================================================*/
class SBEphemDE403 : public SBEphem
{
  friend class SBEphemDE405;
protected:
  static const Entry	LocDE403[13];
public:
  /**A constructor. */
  SBEphemDE403(const SBConfig*);
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName()	const {return "SBEphemDE403";};
  /**Returns LE/DE403 file name.*/
  virtual QString	fileName()	const {return "lede403.bin";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ephemerides LE/DE405.
 * LE/DE405 model implementation.
 * from nav's README (ftp://nav.jpl.nasa.gov/pub/ephem/export/README):
 *"DE405 : (includes both nutations and librations)
 *
 *      JED 2305424.50  (1599 DEC 09)  to  JED 2525008.50  (2201 FEB 20)
 *
 *      Our latest ephemeris; it is based upon the International Celestial
 *      Reference Frame (ICRF).  (DE200 is within 0.01 arcseconds of the frame
 *      of the ICRF).  DE405 was created in May-June 1997."
 */
/*==============================================================================================*/
class SBEphemDE405 : public SBEphem
{
public:
  /**A constructor. */
  SBEphemDE405(const SBConfig*);
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName()	const {return "SBEphemDE405";};
  /**Returns LE/DE403 file name.*/
  virtual QString	fileName()	const {return "lede405.bin";};
};
/*==============================================================================================*/









/*==============================================================================================*/

#endif //SB_GEO_EPHEM_H
