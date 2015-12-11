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
 * Overview:	Contains: auxilaries for SINEX I/O format
 *
 *
 */


#ifndef SB_GEO_SINEX_H
#define SB_GEO_SINEX_H

#include <config.h>

#include "SbGeo.H"
#include "SbGeoParameter.H"

#include <qdatastream.h>
#include <qintdict.h>
#include <qlist.h>
#include <qstring.h>



enum SINEX_SolutionContents
{
  SNX_SC_STATIONS	= 1<< 0,
  SNX_SC_ORBITS		= 1<< 1,
  SNX_SC_EOP		= 1<< 2,
  SNX_SC_TROPOSPHERE	= 1<< 3,
  SNX_SC_SOURCES	= 1<< 4
};



/*==============================================================================================*/
/**Class SBSINEXParameter
 *
 *
 */
/*==============================================================================================*/
class SBSINEXParameter : public SBParameter
{
public:
  /**Types of SINEX parameters*/
  enum SType		{ST_STAX, ST_STAY, ST_STAZ, ST_VELX, ST_VELY, ST_VELZ,
			 ST_XGC, ST_YGC, ST_ZGC, 
			 ST_RS_RA, ST_RS_DE, ST_RS_RAR, ST_RS_DER, ST_RS_PL, 
			 ST_LOD, ST_UT, ST_XPO, ST_YPO, ST_XPOR, ST_YPOR, 
			 ST_NUT_LN, ST_NUT_OB, ST_NUTRLN, ST_NUTROB,
			 ST_UNKN};
  
protected:
  double	APrioriValue;
  int		Idx;
  SType		Type;
  QString	SiteCode;
  QString	PointCode;
  QString	SolutionID;

  SBMJD		TValidEpoch;
  QString	ConstraintCode;
  
public:
  // constructors/destructors:
  //
  /**A constructor.*/
  SBSINEXParameter(const SBSINEXParameter&);
  SBSINEXParameter(SBParameter*);
  /**A destructor.*/
  virtual ~SBSINEXParameter(){};
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName() const {return "SBSINEXParameter";};
  
  // interface:
  //
  int			idx() const {return Idx;};
  SType			type() const {return Type;};
  const QString&	siteCode() const {return SiteCode;};
  const QString&	pointCode() const {return PointCode;};
  const QString&	solutionID() const {return SolutionID;};
  double		aPrioriValue() const {return APrioriValue;};
  double		totValue() const {return APrioriValue + v();};
  const SBMJD&		tValidEpoch() const {return TValidEpoch;};
  const QString&	constraintCode() const {return ConstraintCode;};

  //
  void			setIdx(int I_){Idx=I_;};
  void			setType(SType T_){Type=T_;};
  void			setSiteCode(const QString& S_){SiteCode=S_;};
  void			setPointCode(const QString&  P_){PointCode=P_;};
  void			setSolutionID(const QString& ID_){SolutionID=ID_;};
  void			setAPrioriValue(double Val_){APrioriValue=Val_;};
  void			setTValidEpoch(const SBMJD& T_){TValidEpoch=T_;};
  void			setConstraintCode(const QString& C_){ConstraintCode=C_;};


  // functions:
  //
  QString		parameterUnits();
  QString		parameterType();

  //  virtual QString	str4compare() const
  //    {return QString().sprintf("%09d:", idx()) + 
  //       siteCode() + ":" + pointCode() + ":" + solutionID() + ":" + name();};
  
  // I/O:
  //
  //  friend QDataStream &operator<<(QDataStream&, const SBParameter&);
  //  friend QDataStream &operator>>(QDataStream&, SBParameter&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBSINEXParameterList.
 * List of SINEX parameters, ordered by name.
 *
 */
/*==============================================================================================*/
class SBSINEXParameterList : public QList<SBSINEXParameter>
{
protected:
  virtual int		compareItems(QCollection::Item p1, QCollection::Item p2)
    {
      const SBSINEXParameter *P1 = (const SBSINEXParameter*)p1;
      const SBSINEXParameter *P2 = (const SBSINEXParameter*)p2;
      return P1->str4compare()<P2->str4compare()?-1:P1->str4compare()==P2->str4compare()?0:1;
    };
  //  virtual QDataStream&		write(QDataStream& s, QCollection::Item E) const
  //    {return s << *(const SBSINEXParameter*)E;};
  //  virtual QDataStream&		read(QDataStream& s, QCollection::Item &E)
  //    {E = new SBSINEXParameter; return s >> *(SBSINEXParameter*)E;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Class SBTropRecord
 *
 *
 */
/*==============================================================================================*/
class SBTropRecord : public SBMJD
{
protected:
  double	TotZenith;
  double	WetZenith;
  double	WetZenithErr;
  double	GradNorth;
  double	GradNorthErr;
  double	GradEast;
  double	GradEastErr;
  double	Temp;
  double	Pres;
  double	RH;
  int		NumPts;
  bool		IsCollecting;
  
  // aux:
  long double	TSum;
  double	aleph[3], beta[3], gamma[3], delta[3];
  int		NumProc;

public:
  // constructors/destructors:
  //
  /**A constructor.*/
  SBTropRecord();
  /**A constructor.*/
  SBTropRecord(const SBTropRecord&);
  /**A destructor.*/
  virtual ~SBTropRecord(){};
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName() const {return "SBTropRecord";};
  
  // interface:
  //
  double		totZenith()	const {return TotZenith;};
  double		wetZenith()	const {return WetZenith;};
  double		wetZenithErr()	const {return WetZenithErr;};
  double		gradNorth()	const {return GradNorth;};
  double		gradNorthErr()	const {return GradNorthErr;};
  double		gradEast()	const {return GradEast;};
  double		gradEastErr()	const {return GradEastErr;};
  double		temp()		const {return Temp;};
  double		pres()		const {return Pres;};
  double		rh()		const {return RH;};
  int			numPts()	const {return NumPts;};
  int			numProc()	const {return NumProc;};

  //
  void			setTotZenith(double z_)		{TotZenith=z_;};
  void			setWetZenith(double z_)		{WetZenith=z_;};
  void			setWetZenithErr(double z_)	{WetZenithErr=z_;};
  void			setGradNorth(double z_)		{GradNorth=z_;};
  void			setGradNorthErr(double z_)	{GradNorthErr=z_;};
  void			setGradEast(double z_)		{GradEast=z_;};
  void			setGradEastErr(double z_)	{GradEastErr=z_;};
  void			setTemp(double z_)		{Temp=z_;};
  void			setPres(double z_)		{Pres=z_;};
  void			setRH(double z_)		{RH=z_;};
  void			setNumPts(int n_)		{NumPts=n_;};

  SBMJD			realEpoch() const {return NumPts>0?TSum/NumPts:*this;};

  bool			operator==(const SBTropRecord& TR) const
    {
      return 
	SBMJD::operator==(TR) ||
	TotZenith	== TR.totZenith() ||
	WetZenith	== TR.wetZenith() ||
	WetZenithErr	== TR.wetZenithErr() ||
	GradNorth	== TR.gradNorth() ||
	GradNorthErr	== TR.gradNorthErr() ||
	GradEast	== TR.gradEast() ||
	GradEastErr	== TR.gradEastErr() ||
	NumPts		== TR.numPts();
    };

  bool			operator!=(const SBTropRecord& TR) const {return !(*this==TR);};

  // functions:
  //
  void			clear();
  void			collect(SBTropRecord*);
  void			processCollected();
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBTropRecordList.
 *
 *
 */
/*==============================================================================================*/
class SBTropRecordList : public QList<SBTropRecord>
{
protected:
  virtual int		compareItems(QCollection::Item p1, QCollection::Item p2)
    {
      const SBMJD *P1 = (const SBMJD*)p1;
      const SBMJD *P2 = (const SBMJD*)p2;
      return *P1<*P2?-1:*P1==*P2?0:1;
    };
};
/*==============================================================================================*/



class SBTS_SINEX;

void writeNormalEquationSystem(SBTS_SINEX&, 
			       SBParameterList*, SBVector*, SBUpperMatrix*,
			       SBParameterList*, SBVector*, SBUpperMatrix*, SBMatrix*,
			       const QString&, const QString&);




#endif //SB_GEO_SINEX_H
