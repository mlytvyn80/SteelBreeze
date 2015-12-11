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
 * Overview:    Class SBEOPEntry: a record of the Eearth rotation
 *				  parameters.
 *		Class SBEOP:	  a collection of the EOP records.
 *				  Performs storing the EOP solution,
 *				  import/export operations, calculation
 *				  of the interpolated values.
 *
 */

#ifndef SB_GEO_EOP_H
#define SB_GEO_EOP_H

#include <config.h>
#include <stdlib.h>
#include <math.h>

#include <qlist.h>

#include "SbGeneral.H"
#include "SbGeo.H"
#include "SbGeoTime.H"




/*==============================================================================================*/
/*												*/
/* class SBEOPEntry										*/
/*												*/
/*==============================================================================================*/
class SBEOPEntry : public SBMJD
{
  friend class SBEOP;
private:
  double		Xp;		//!< x, arc seconds
  double		Yp;		//!< y, arc seconds
  double		UT;		//!< UT1-UTC, seconds
  double		LOD;		//!< LOD, seconds
  double		Psi;		//!< dPsi, arc seconds
  double		Eps;		//!< dEps, arc seconds

public:
  SBEOPEntry() : SBMJD() {Xp=Yp=UT=LOD=Psi=Eps=0.0;};
  SBEOPEntry(const SBEOPEntry& eop) : SBMJD(eop)
    {Xp=eop.Xp;Yp=eop.Yp;UT=eop.UT;LOD=eop.LOD;Psi=eop.Psi;Eps=eop.Eps;};
  SBEOPEntry(double date_, double Xp_=0.0, double Yp_=0.0, double UT_=0.0, 
	     double LOD_=0.0, double Psi_=0.0, double Eps_=0.0) : SBMJD((long double)date_)
    {Xp=Xp_; Yp=Yp_; UT=UT_; LOD=LOD_; Psi=Psi_; Eps=Eps_;};
  
  virtual ~SBEOPEntry(){};
  virtual QString ClassName() const {return "SBEOPEntry";};
  
  /**Returns tabulated value of x, arc seconds.*/
  inline double xp()  const {return Xp;};
  /**Returns tabulated value of y, arc seconds.*/
  inline double yp()  const {return Yp;};
  /**Returns tabulated value of UT1-UTC, seconds.*/
  inline double ut()  const {return UT;};
  /**Returns tabulated value of LOD, seconds.*/
  inline double lod()  const {return LOD;};
  /**Returns tabulated value of dPsi, arc seconds.*/
  inline double psi() const {return Psi;};
  /**Returns tabulated value of dEps, arc seconds.*/
  inline double eps() const {return Eps;};
  
  
  inline virtual SBEOPEntry& operator= (const SBEOPEntry& eop)
    {(SBMJD&)(*this)=eop;Xp=eop.Xp;Yp=eop.Yp;UT=eop.UT;LOD=eop.LOD;Psi=eop.Psi;Eps=eop.Eps; return *this;};
  inline friend QDataStream &operator<<(QDataStream& s, const SBEOPEntry& eop)
    {return s << (const SBMJD&)eop << eop.Xp << eop.Yp << eop.UT << eop.LOD << eop.Psi << eop.Eps;};
  inline friend QDataStream &operator>>(QDataStream& s, SBEOPEntry& eop)
    {return s >> (SBMJD&)eop >> eop.Xp >> eop.Yp >> eop.UT >> eop.LOD >> eop.Psi >> eop.Eps;};
  
  friend QTextStream &operator<<(QTextStream& s, const SBEOPEntry& eop);
  friend QTextStream &operator>>(QTextStream& s, SBEOPEntry& eop);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Class SBEOP.
 * This class manages the EOP collection.
 *
 * \todo
 * This class (and its dependencies) should be rewritten. All things has a weak
 * design.
 *
 * \test
 *
 * \bug
 * The entry to the bug-list from SBEOP class. 
 * This entry will be removed after class redesigning.
 *
 */
/*==============================================================================================*/
class SBParameter;
class SBRunManager;
class SBSpline;
class SBMatrix;

class SBEOP : public QList<SBEOPEntry>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return (*(SBMJD*)item1==*(SBMJD*)item2)?0:((*(SBMJD*)item1<*(SBMJD*)item2)?-1:1);};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBEOPEntry*)Entry;};
  
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBEOPEntry; return s >> *(SBEOPEntry*)Entry;};
  
private:
  SBSolutionName       	Label;		/* catalog's name */
  SBMJD			Epoch;		/* catalog's epoch */
  SBMJD			TStart;
  SBMJD			TFinis;
  
  /* linear transformation for consistency... */
  double		Xp;		/* shift */
  double		Xps;		/* drift */
  double		Yp;		/* shift */
  double		Yps;		/* drift */
  double		UT;		/* shift */
  double		UTs;		/* drift */
  double		Psi;		/* shift */
  double		Psis;		/* drift */
  double		Eps;		/* shift */
  double		Epss;		/* drift */
  
  bool			UTHasHiTide;
  bool			UTHasLoTide;
  
  // interpolation:
  unsigned int		PtNum;
  SBMatrix		*Cache;
  SBSpline		*Spliner;
  unsigned int		Idx;
  bool			IsOK;
  SBMJD			T_4Prepare;
  
  // interpolated values:
  SBMJD			T_4Spline;
  double		EOP_X;
  double		EOP_Y;
  double		EOP_UT;
  double		EOP_LOD;
  double		EOP_Psi;
  double		EOP_Eps;
  bool			HasLeapSecond;
  SBMJD			T_LeapSecond;
  int			SLSign;
  
  // estimated parameters:
  SBRunManager		*Mgr;
  SBParameter*		P_Px;		//!< estimation of the X-polar coordinate;
  SBParameter*		P_Py;		//!< estimation of the Y-polar coordinate;
  SBParameter*		P_Put;		//!< estimation of the UT;
  SBParameter*		P_Npsi;		//!< estimation of the Nutation angle Psi;
  SBParameter*		P_Neps;		//!< estimation of the Nutation angle Eps;

  // tmps:
  SBParameter*		P_Xp_m2;
  SBParameter*		P_Yp_m2;
  SBParameter*		P_Xp_m1;
  SBParameter*		P_Yp_m1;
  SBParameter*		P_Xp_p1;
  SBParameter*		P_Yp_p1;
  SBParameter*		P_Xp_p2;
  SBParameter*		P_Yp_p2;

  SBParameter*		P_Xp_m3;
  SBParameter*		P_Yp_m3;
  SBParameter*		P_Xp_p3;
  SBParameter*		P_Yp_p3;
  SBParameter*		P_Xp_m4;
  SBParameter*		P_Yp_m4;
  SBParameter*		P_Xp_p4;
  SBParameter*		P_Yp_p4;

  SBParameter*		P_UT_c1;
  SBParameter*		P_UT_s1;
  SBParameter*		P_UT_c2;
  SBParameter*		P_UT_s2;

  SBParameter*		P_UT_c3;
  SBParameter*		P_UT_s3;
  SBParameter*		P_UT_c4;
  SBParameter*		P_UT_s4;

  // 4test:
  SBParameter*		P_Xp_m5;
  SBParameter*		P_Yp_m5;
  SBParameter*		P_Xp_p5;
  SBParameter*		P_Yp_p5;
  SBParameter*		P_UT_c5;
  SBParameter*		P_UT_s5;

  SBParameter*		P_Xp_m6;
  SBParameter*		P_Yp_m6;
  SBParameter*		P_Xp_p6;
  SBParameter*		P_Yp_p6;
  SBParameter*		P_UT_c6;
  SBParameter*		P_UT_s6;

  SBParameter*		P_Xp_m7;
  SBParameter*		P_Yp_m7;
  SBParameter*		P_Xp_p7;
  SBParameter*		P_Yp_p7;
  SBParameter*		P_UT_c7;
  SBParameter*		P_UT_s7;

  SBParameter*		P_Xp_m8;
  SBParameter*		P_Yp_m8;
  SBParameter*		P_Xp_p8;
  SBParameter*		P_Yp_p8;
  SBParameter*		P_UT_c8;
  SBParameter*		P_UT_s8;

  SBParameter*		P_Xp_m9;
  SBParameter*		P_Yp_m9;
  SBParameter*		P_Xp_p9;
  SBParameter*		P_Yp_p9;
  SBParameter*		P_UT_c9;
  SBParameter*		P_UT_s9;

  SBParameter*		P_Xp_m10;
  SBParameter*		P_Yp_m10;
  SBParameter*		P_Xp_p10;
  SBParameter*		P_Yp_p10;
  SBParameter*		P_UT_c10;
  SBParameter*		P_UT_s10;


  virtual void		checkRanges();
  
public:
  SBEOP();
  virtual ~SBEOP();
  virtual QString ClassName() const {return "SBEOP";};
  
  
  /*  interface:  */
  void			setLabel    (const SBSolutionName& Label_) {Label=Label_;};
  void			setEpoch    (const SBMJD& Epoch_)	   {Epoch=Epoch_;};
  void			setPtNum    (unsigned int);
  void			setMgr      (SBRunManager* Mgr_) {Mgr=Mgr_;};
  
  SBMatrix*		cache4Interpolation() {return Cache;};

  const SBSolutionName& label	    () const {return Label;};
  SBMJD			epoch       () const {return Epoch;};
  SBMJD			tStart	    () const {return TStart;};
  SBMJD			tFinis      () const {return TFinis;};
  unsigned int		ptNum	    () const {return PtNum;};
  SBRunManager*		mgr	    () const {return Mgr;};
  
  double		xp	    () const {return Xp;};
  double		xps         () const {return Xps;};
  double		yp	    () const {return Yp;};
  double		yps         () const {return Yps;};
  double		ut	    () const {return UT;};
  double		uts         () const {return UTs;};
  double		psi	    () const {return Psi;};
  double		psis        () const {return Psis;};
  double		eps	    () const {return Eps;};
  double		epss        () const {return Epss;};
  bool			utHasHiTide () const {return UTHasHiTide;};
  bool			utHasLoTide () const {return UTHasLoTide;};
  inline SBParameter*	p_Px()		{return P_Px;};
  inline SBParameter*	p_Py()		{return P_Py;};
  inline SBParameter*	p_Put()		{return P_Put;};
  inline SBParameter*	p_Npsi()	{return P_Npsi;};
  inline SBParameter*	p_Neps()	{return P_Neps;};

  // tmps:
  inline SBParameter*	p_Xp_m2()		{return P_Xp_m2;};
  inline SBParameter*	p_Yp_m2()		{return P_Yp_m2;};
  inline SBParameter*	p_Xp_m1()		{return P_Xp_m1;};
  inline SBParameter*	p_Yp_m1()		{return P_Yp_m1;};
  inline SBParameter*	p_Xp_p2()		{return P_Xp_p2;};
  inline SBParameter*	p_Yp_p2()		{return P_Yp_p2;};
  inline SBParameter*	p_Xp_p1()		{return P_Xp_p1;};
  inline SBParameter*	p_Yp_p1()		{return P_Yp_p1;};


  inline SBParameter*	p_Xp_m3()		{return P_Xp_m3;};
  inline SBParameter*	p_Yp_m3()		{return P_Yp_m3;};
  inline SBParameter*	p_Xp_p3()		{return P_Xp_p3;};
  inline SBParameter*	p_Yp_p3()		{return P_Yp_p3;};
  inline SBParameter*	p_Xp_m4()		{return P_Xp_m4;};
  inline SBParameter*	p_Yp_m4()		{return P_Yp_m4;};
  inline SBParameter*	p_Xp_p4()		{return P_Xp_p4;};
  inline SBParameter*	p_Yp_p4()		{return P_Yp_p4;};

  inline SBParameter*	p_UT_c1()		{return P_UT_c1;};
  inline SBParameter*	p_UT_s1()		{return P_UT_s1;};
  inline SBParameter*	p_UT_c2()		{return P_UT_c2;};
  inline SBParameter*	p_UT_s2()		{return P_UT_s2;};
  inline SBParameter*	p_UT_c3()		{return P_UT_c3;};
  inline SBParameter*	p_UT_s3()		{return P_UT_s3;};
  inline SBParameter*	p_UT_c4()		{return P_UT_c4;};
  inline SBParameter*	p_UT_s4()		{return P_UT_s4;};

  // 4test:
  inline SBParameter*	p_Xp_m5()		{return P_Xp_m5;};
  inline SBParameter*	p_Yp_m5()		{return P_Yp_m5;};
  inline SBParameter*	p_Xp_p5()		{return P_Xp_p5;};
  inline SBParameter*	p_Yp_p5()		{return P_Yp_p5;};
  inline SBParameter*	p_UT_c5()		{return P_UT_c5;};
  inline SBParameter*	p_UT_s5()		{return P_UT_s5;};

  inline SBParameter*	p_Xp_m6()		{return P_Xp_m6;};
  inline SBParameter*	p_Yp_m6()		{return P_Yp_m6;};
  inline SBParameter*	p_Xp_p6()		{return P_Xp_p6;};
  inline SBParameter*	p_Yp_p6()		{return P_Yp_p6;};
  inline SBParameter*	p_UT_c6()		{return P_UT_c6;};
  inline SBParameter*	p_UT_s6()		{return P_UT_s6;};

  inline SBParameter*	p_Xp_m7()		{return P_Xp_m7;};
  inline SBParameter*	p_Yp_m7()		{return P_Yp_m7;};
  inline SBParameter*	p_Xp_p7()		{return P_Xp_p7;};
  inline SBParameter*	p_Yp_p7()		{return P_Yp_p7;};
  inline SBParameter*	p_UT_c7()		{return P_UT_c7;};
  inline SBParameter*	p_UT_s7()		{return P_UT_s7;};

  inline SBParameter*	p_Xp_m8()		{return P_Xp_m8;};
  inline SBParameter*	p_Yp_m8()		{return P_Yp_m8;};
  inline SBParameter*	p_Xp_p8()		{return P_Xp_p8;};
  inline SBParameter*	p_Yp_p8()		{return P_Yp_p8;};
  inline SBParameter*	p_UT_c8()		{return P_UT_c8;};
  inline SBParameter*	p_UT_s8()		{return P_UT_s8;};

  inline SBParameter*	p_Xp_m9()		{return P_Xp_m9;};
  inline SBParameter*	p_Yp_m9()		{return P_Yp_m9;};
  inline SBParameter*	p_Xp_p9()		{return P_Xp_p9;};
  inline SBParameter*	p_Yp_p9()		{return P_Yp_p9;};
  inline SBParameter*	p_UT_c9()		{return P_UT_c9;};
  inline SBParameter*	p_UT_s9()		{return P_UT_s9;};

  inline SBParameter*	p_Xp_m10()		{return P_Xp_m10;};
  inline SBParameter*	p_Yp_m10()		{return P_Yp_m10;};
  inline SBParameter*	p_Xp_p10()		{return P_Xp_p10;};
  inline SBParameter*	p_Yp_p10()		{return P_Yp_p10;};
  inline SBParameter*	p_UT_c10()		{return P_UT_c10;};
  inline SBParameter*	p_UT_s10()		{return P_UT_s10;};
  //



  //  inline SBParameter*	p_Oy()		{return P_Oy;};
  //  inline SBParameter*	p_Oz()		{return P_Oz;};

  
  void	setXp		(double Xp_  ) {Xp  = Xp_ ;};
  void	setXps		(double Xps_ ) {Xps = Xps_;};
  void	setYp		(double Yp_  ) {Yp  = Yp_ ;};
  void	setYps		(double Yps_ ) {Yps = Yps_;};
  void	setUt		(double UT_  ) {UT  = UT_ ;};
  void	setUts		(double UTs_ ) {UTs = UTs_;};
  void	setPsi		(double Psi_ ) {Psi = Psi_ ;};
  void	setPsis		(double Psis_) {Psis= Psis_;};
  void	setEps		(double Eps_ ) {Eps = Eps_ ;};
  void	setEpss		(double Epss_) {Epss= Epss_;};
  void	setUtHasHiTide	(bool UTHasHiTide_) {UTHasHiTide=UTHasHiTide_;};
  void	setUtHasLoTide	(bool UTHasLoTide_) {UTHasLoTide=UTHasLoTide_;};
  
  
  
  /*  functions:  */
  virtual void		importSeries(QString);
  void			interpolateEOPs(const SBMJD&);
  void			prepare4Date(const SBMJD&);
  const SBMJD&		t_4Prepare() const {return T_4Prepare;};
  void			createParameters();
  void			releaseParameters();
  void			updateParameters(SBRunManager*, const SBMJD&);
 
  /* calc EOPs: */
  /**returns interpolated value of dXp in rads. */
  virtual double	dX(const SBMJD& T) const;
  /**returns interpolated value of dYp in rads. */
  virtual double	dY(const SBMJD& T) const;
  /**returns interpolated value of d(UT1-UTC) in days. */
  virtual double	dUT1_UTC(const SBMJD& T) const;
  /**returns interpolated value of LOD in days. */
  virtual double	dLOD(const SBMJD& T) const;
  /**returns interpolated value of dPsi in rads. */
  virtual double	dPsi(const SBMJD& T) const;
  /**returns interpolated value of dEps in rads. */
  virtual double	dEps(const SBMJD& T) const;
  
  
  /*  i/o:  */
  friend QDataStream &operator<<(QDataStream&, const SBEOP&);
  friend QDataStream &operator>>(QDataStream&, SBEOP&);
  friend QTextStream &operator<<(QTextStream&, const SBEOP&);
  friend QTextStream &operator>>(QTextStream&, SBEOP&);
};
/*==============================================================================================*/









#endif //SB_GEO_EOP_H
