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
 * Overview:	Contains: derivation, estimated parameter, 
 *		`solution' - a collection of parameters.
 *
 */


#ifndef SB_GEO_PARAMETER_H
#define SB_GEO_PARAMETER_H

#include <config.h>

#include "SbGeo.H"

#include <qdatastream.h>
#include <qintdict.h>
#include <qlist.h>
#include <qstring.h>




/*==============================================================================================*/
/**Class SBParameterCfg, the Parameter's configuration.
 * It just describes the config for each estimated parameter.
 *
 */
/*==============================================================================================*/
class SBParameterCfg : public SBNamed
{
public:
  enum PType		{PT_NONE=0, PT_GLB=1, PT_ARC=2, PT_LOC=3, PT_STH=4};
  enum StochType	{ST_NONE=0, ST_WHITENOISE=1, ST_MARKOVPROCESS=2, ST_RANDWALK=3, ST_UNKN=4};

private:
  /**Some Comment: */
  bool			IsPropagated;		//!< Propagate the parameter in time.
  bool			IsConstrained;		//!< The parameter needs constrains.
  PType			Type;
  StochType		StochasticType;
  double		ConvAPriori;		//!< a priori std. deviation (conventional pars)
  double		StocAPriori;		//!< a priori std. deviation (stochastic pars)
  double		BreakNoise;		//!< psd of noise process used for "breaks"
  double		Tau;			//!< time of relaxation
  double		WhiteNoise;		//!< psd of ruled white noise
  double		Scale;			//!< Value of the scale factor.
  QString		ScaleName;		//!< Name of the scale factor.
  
public:
  // constructors/destructors:
  //
  /**A constructor.*/
  SBParameterCfg();
  /**A constructor.*/
  SBParameterCfg(const QString&, bool, bool, PType, double, double, double, double, 
		 double, double, const QString&, StochType=ST_RANDWALK);
  /**A constructor.*/
  SBParameterCfg(const SBParameterCfg& P_) : SBNamed(P_.name()) {*this = P_;};
  /**A destructor.*/
  virtual ~SBParameterCfg(){};
  SBParameterCfg& operator= (const SBParameterCfg&);
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBParameterCfg";};
  
  // interface:
  //
  virtual bool		isPropagated()	const {return IsPropagated;};
  virtual bool		isConstrained() const {return IsConstrained;};
  virtual PType		type()		const {return Type;};
  virtual StochType	stochasticType()const {return StochasticType;};
  virtual double	convAPriori()	const {return ConvAPriori;};
  virtual double	stocAPriori()	const {return StocAPriori;};
  virtual double	breakNoise()	const {return BreakNoise;};
  virtual double	tau()		const {return Tau;};
  virtual double	whiteNoise()	const {return WhiteNoise;};
  virtual double	scale()		const {return Scale;};
  virtual const QString&scaleName()	const {return ScaleName;};

  virtual void		setPropagated(bool Is_){IsPropagated = Is_;};
  virtual void		setConstrained(bool Is_){IsConstrained = Is_;};
  virtual void		setType(PType T_){Type = T_;};
  virtual void		setStochasticType(StochType T_){StochasticType = T_;};
  virtual void		setConvAPriori(double d_){ConvAPriori = d_;};
  virtual void		setStocAPriori(double d_){StocAPriori = d_;};
  virtual void		setBreakNoise(double d_){BreakNoise = d_;};
  virtual void		setTau(double d_){Tau = d_;};
  virtual void		setWhiteNoise(double d_){WhiteNoise = d_;};
  virtual void		setScale(double d_){Scale = d_;};
  virtual void		setScaleName(const QString& _){ScaleName=_.copy();};

  // functions:
  //

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBParameterCfg&);
  friend QDataStream &operator>>(QDataStream&, SBParameterCfg&);
};
/*==============================================================================================*/




/*==============================================================================================*/
/**Class ParametersDesc.
 * Just an array (and some other structures) of SBParameterCfgs, describes 
 * estimated parameters
 */
/*==============================================================================================*/
class SBParametersDesc
{
private:
#define NUMBER_OF_PARAMETERS	20
  static const int	Num;				//!< The number of elemenths in the array.
  SBParameterCfg	Pars[NUMBER_OF_PARAMETERS];	//!< The array of the parameters.
public:
  // constructors/destructors:
  //
  /**A constructor.*/
  SBParametersDesc();
  /**A constructor.
   * Creates a copy of object.
   * \param D_ the Parameters' descriptor.
   */
  SBParametersDesc(const SBParametersDesc& D){*this = D;};
  /**A destructor.*/
  virtual ~SBParametersDesc(){};
  SBParametersDesc& operator= (const SBParametersDesc&);
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBParametersDesc";};
  
  // interface:
  //
  /**Returns Clock at station (zero order).*/
  virtual const SBParameterCfg&	clock0() const {return Pars[0];};
  /**Sets Clock at station (zero order).*/
  virtual void  setClock0(const SBParameterCfg& P){Pars[0]=P;};
  virtual void	setClock0Type(SBParameterCfg::PType T_) {Pars[0].setType(T_);};
  /**Returns Clock at station (first order).*/
  virtual const SBParameterCfg&	clock1() const {return Pars[1];};
  /**Sets Clock at station (first order).*/
  virtual void  setClock1(const SBParameterCfg& P){Pars[1]=P;};
  virtual void	setClock1Type(SBParameterCfg::PType T_) {Pars[1].setType(T_);};
  /**Returns Clock at station (second order).*/
  virtual const SBParameterCfg&	clock2() const {return Pars[2];};
  /**Sets Clock at station (second order).*/
  virtual void  setClock2(const SBParameterCfg& P){Pars[2]=P;};
  virtual void	setClock2Type(SBParameterCfg::PType T_) {Pars[2].setType(T_);};
  /**Returns Clock at station (third order).*/
  virtual const SBParameterCfg&	clock3() const {return Pars[3];};
  /**Sets Clock at station (third order).*/
  virtual void  setClock3(const SBParameterCfg& P){Pars[3]=P;};
  virtual void	setClock3Type(SBParameterCfg::PType T_) {Pars[3].setType(T_);};
  /**Returns Coordinates of a radio source.*/
  virtual const SBParameterCfg&	srcCoo() const {return Pars[4];};
  /**Sets Coordinates of a radio source.*/
  virtual void  setSrcCoo(const SBParameterCfg& P){Pars[4]=P;};
  /**Returns Coordinates of a station.*/
  virtual const SBParameterCfg&	staCoo() const {return Pars[5];};
  /**Sets Coordinates of a station.*/
  virtual void  setStaCoo(const SBParameterCfg& P){Pars[5]=P;};
  /**Returns Velocities of a station.*/
  virtual const SBParameterCfg&	staVel() const {return Pars[6];};
  /**Sets Velocities of a station.*/
  virtual void  setStaVel(const SBParameterCfg& P){Pars[6]=P;};

  /**Returns Zenith delay.*/
  virtual const SBParameterCfg&	zenith() const {return Pars[7];};
  /**Sets Zenith delay.*/
  virtual void  setZenith(const SBParameterCfg& P){Pars[7]=P;};
  virtual void	setZenithType(SBParameterCfg::PType T_) {Pars[7].setType(T_);};
  /**Returns atmospheric gradient parameter.*/
  virtual const SBParameterCfg&	atmGrad() const {return Pars[8];};
  /**Sets atmospheric parameter.*/
  virtual void  setAtmGrad(const SBParameterCfg& P){Pars[8]=P;};
  /**Returns Axis offset.*/
  virtual const SBParameterCfg&	axisOffset() const {return Pars[9];};
  /**Sets Axis offset.*/
  virtual void  setAxisOffset(const SBParameterCfg& P){Pars[9]=P;};
  /**Returns Polus coordinates.*/
  virtual const SBParameterCfg&	polusXY() const {return Pars[10];};
  /**Sets Polus coordinates.*/
  virtual void  setPolusXY(const SBParameterCfg& P){Pars[10]=P;};
  /**Returns Polus d(UT1-UTC).*/
  virtual const SBParameterCfg&	polusUT() const {return Pars[11];};
  /**Sets Polus d(UT1-UTC).*/
  virtual void  setPolusUT(const SBParameterCfg& P){Pars[11]=P;};
  /**Returns Nutation in obliquity and longitude.*/
  virtual const SBParameterCfg&	polusNut() const {return Pars[12];};
  /**Sets Nutation in obliquity and longitude.*/
  virtual void  setPolusNut(const SBParameterCfg& P){Pars[12]=P;};
  /**Returns test parameter.*/
  virtual const SBParameterCfg&	test() const {return Pars[13];};
  /**Sets test parameter.*/
  virtual void  setTest(const SBParameterCfg& P){Pars[13]=P;};
  //--
  /**Returns cable correction.*/
  virtual const SBParameterCfg&	cable() const {return Pars[14];};
  /**Sets cable correction.*/
  virtual void  setCable(const SBParameterCfg& P){Pars[14]=P;};
  virtual void	setCableType(SBParameterCfg::PType T_) {Pars[14].setType(T_);};
  //--
  /**Returns love number h2.*/
  virtual const SBParameterCfg&	love_h2() const {return Pars[15];};
  /**Sets love number h2.*/
  virtual void  setLove_h2(const SBParameterCfg& P){Pars[15]=P;};
  virtual void	setLove_h2Type(SBParameterCfg::PType T_) {Pars[15].setType(T_);};
  /**Returns love number l2.*/
  virtual const SBParameterCfg&	love_l2() const {return Pars[16];};
  /**Sets love number l2.*/
  virtual void  setLove_l2(const SBParameterCfg& P){Pars[16]=P;};
  virtual void	setLove_l2Type(SBParameterCfg::PType T_) {Pars[16].setType(T_);};
  /**Returns love number h2.*/
  virtual const SBParameterCfg&	tideLag() const {return Pars[17];};
  /**Sets Tide Lag.*/
  virtual void  setTideLag(const SBParameterCfg& P){Pars[17]=P;};
  virtual void	setTideLagType(SBParameterCfg::PType T_) {Pars[17].setType(T_);};



  //
  /**Returns i-th parameter.*/
  virtual const SBParameterCfg&	par(int i) const {return Pars[0<=i&&i<Num?i:0];};
    /**Sets i-th parameter.*/
  virtual void	setPar(int i, const SBParameterCfg& P) {Pars[0<=i&&i<Num?i:0]=P;};
  /**Returns total number of the parameters.*/
  static int num() {return Num;};
  /**Returns currently used number of the parameters.*/
  static int n() {return 18;};
  
  // functions:
  //
  virtual void	setType(int i, SBParameterCfg::PType T_){Pars[0<=i&&i<Num?i:0].setType(T_);};
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBParametersDesc&);
  friend QDataStream &operator>>(QDataStream&, SBParametersDesc&);
};
/*==============================================================================================*/





class SBVector;
class SBSymMatrix;
class SBUpperMatrix;
class SBMatrix;

/*==============================================================================================*/
/**SBDerivation.
 * Keeps basic info about derivation.
 *
 */
/*==============================================================================================*/
class SBDerivation : public SBNamed, public SBAttributed
{
public:
  enum Attributes
  {
    IsBreak		= (1<<0),	//!< there is a break in the parameter's behaviour
    IsPropagated	= (1<<1),	//!< the parameter's info have to be propagated in future
    IsInRun		= (1<<2),	//!< 
  };
  
protected:
  double	D;			//!< value of a derivation;
  int		Num;			//!< number of points per derivation;
  int		NumSes;			//!< number of sessions per derivation;
  SBMJD		TStart;			//!< MJD of the first measurement;
  SBMJD		TFinis;			//!< MJD of the last measurement;
  long double	TSum;			//!< the sum of the epochs of observation;
  
  // for DSINEX purposes:
  int		Num_aux;		//!< number of points per derivation;
  SBMJD		TStart_aux;		//!< MJD of the first measurement;
  SBMJD		TFinis_aux;		//!< MJD of the last measurement;
  long double	TSum_aux;		//!< the sum of the epochs of observation;
  
public:
  // constructors/destructors:
  //
  /** A constructor. */
  SBDerivation(const QString&);
  /** A constructor. */
  SBDerivation(const SBDerivation& Der_) : SBNamed(Der_), SBAttributed(Der_) {*this=Der_;};
  /** A destructor. */
  virtual ~SBDerivation(){};
  SBDerivation& operator= (const SBDerivation&);
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBDerivation";};
  
  // interface:
  //
  /**Returns the value of the derivation.*/
  virtual double	d(const SBMJD& =TZero) const {return D;};
  /**Returns the number of observations.*/
  virtual int		num(const SBMJD& =TZero) const {return Num;};
  /**Returns the number of sessions.*/
  virtual int		numSes(const SBMJD& =TZero) const {return NumSes;};
  /**Returns date of the first observation.*/
  virtual SBMJD		tStart(const SBMJD& =TZero) const {return TStart;};
  /**Returns date of the last observation.*/
  virtual SBMJD		tFinis(const SBMJD& =TZero) const {return TFinis;};
  /**Returns date of the mean observation.*/
  virtual SBMJD		tEpoch(const SBMJD& =TZero) const {return Num?SBMJD(TSum/Num):TZero;};
  /**Returns interval of the observations.*/
  virtual double	tInterval(const SBMJD& =TZero) const {return (double)(TFinis-TStart);};
  
  /**Sets up the value of the derivation.*/
  virtual void		setD(double, const SBMJD&);
  /**Sets up the value of the number of observations.*/
  virtual void		setNum(int Num_) {Num=Num_;};
  virtual void		setNumSes(int Num_) {NumSes=Num_;};
  virtual void		addNumSes(int Num_) {NumSes+=Num_;};
  virtual void		setTimes(const SBDerivation& Der)
    {TStart=Der.TStart; TFinis=Der.TFinis; TSum=Der.TSum; Num=Der.Num; NumSes=Der.NumSes;};
  
  
  
  /**Returns the number of observations.*/
  virtual int		num_aux(const SBMJD& =TZero) const {return Num_aux;};
  /**Returns date of the first observation.*/
  virtual SBMJD		tStart_aux(const SBMJD& =TZero) const {return TStart_aux;};
  /**Returns date of the last observation.*/
  virtual SBMJD		tFinis_aux(const SBMJD& =TZero) const {return TFinis_aux;};
  /**Returns date of the mean observation.*/
  virtual SBMJD		tEpoch_aux(const SBMJD& =TZero) const{return Num_aux?SBMJD(TSum_aux/Num_aux):TZero;};
  /**Returns interval of the observations.*/
  virtual double	tInterval_aux(const SBMJD& =TZero) const {return (double)(TFinis_aux-TStart_aux);};
  
  // functions:
  //
  virtual void		clearValues();
  virtual void		clearValues_aux();
  virtual void		invertAux()
    {Num = Num_aux; TStart = TStart_aux; TFinis = TFinis_aux; TSum = TSum_aux;};

  virtual void		zero() {D=0.0;};

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBDerivation&);
  friend QDataStream &operator>>(QDataStream&, SBDerivation&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBParameter.
 * Keeps info about solution.
 *
 */
/*==============================================================================================*/
class SBParameter : public SBDerivation
{
protected:
  double	V;			//!< value of the parameter;
  double	S;			//!< value of the solution;
  double	E;			//!< formal error;
  double	Scale;			//!< scale factor;

  //
  SBParameterCfg::StochType	SType;
  double	SigmaAPriori;		//!< a priori std. deviation (to init the Inform. matrix)
  double	Tau;			//!< time of relaxation (for stochastic parameters)
  double	PSD_RWN;		//!< PSD of Ruled White Noise
  double	PSD_BN;			//!< PSD of White Noise used in break

  char		Prefix;

public:
  // constructors/destructors:
  //
  /**A constructor.*/
  SBParameter(const QString& = "Unnamed parameter", double =1.0);
  /**A constructor.*/
  SBParameter(const SBParameter& P_) : SBDerivation(P_) {*this=P_;};
  /**A destructor.*/
  virtual ~SBParameter(){};
  SBParameter& operator= (const SBParameter&);
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName() const {return "SBParameter";};
  
  // interface:
  //
  /**Returns value of the parameter.*/
  virtual double	v(const SBMJD& =TZero) const {return V;};
  /**Returns value of the last solution.*/
  virtual double	s(const SBMJD& =TZero) const {return S;};
  /**Returns value of the formal error.*/
  virtual double	e(const SBMJD& =TZero) const {return E;};
  /**Returns value of the scale factor.*/
  virtual double	scale() const {return Scale;};
  //
  virtual double	sigmaAPriori() const {return SigmaAPriori;};
  virtual double	tau()          const {return Tau;};
  virtual double	psd_RWN()      const {return PSD_RWN;};
  virtual double	psd_BN()       const {return PSD_BN;};
  virtual SBParameterCfg::StochType sType() const {return SType;};
  virtual char		prefix()       const {return Prefix;};

  /**Sets the value of the parameter.*/
  virtual void		setV(double d_) {V=d_;};
  /**Sets the value of the last udjustment.*/
  virtual void		setS(double d_) {S=d_;};
  /**Sets the value of the formal error.*/
  virtual void		setE(double d_) {E=d_;};
  /**Sets the value of the scale factor.*/
  virtual void		setScale(double s_) {Scale=s_;};
  //
  virtual void		setSigmaAPriori(double d_) {SigmaAPriori=d_;};
  virtual void		setTau(double d_)          {Tau=d_;};
  virtual void		setPSD_RWN(double d_)      {PSD_RWN=d_;};
  virtual void		setPSD_BN(double d_)       {PSD_BN=d_;};
  virtual void		setSType(SBParameterCfg::StochType ST_)	{SType = ST_;};
  virtual void		setPrefix(char c_)	   {Prefix = c_;};


  // functions:
  //
  virtual void		tuneParameter(const SBParameterCfg&);
  virtual void		update(const SBParameter*);
  QString		timeStamp() const 
    {return tEpoch().toString(SBMJD::F_INTERNAL).copy();};
  virtual double	m (double);
  virtual double	rw(double);
  virtual void		clearValues();
  virtual QString	str4compare() const
    {return Prefix + (isAttr(SBDerivation::IsPropagated)?"z":"a") + name();};
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBParameter&);
  friend QDataStream &operator>>(QDataStream&, SBParameter&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBParameterList.
 * List of parameters, ordered by name. Not an owner of inserted parameters.
 *
 */
/*==============================================================================================*/
class SBParameterList : public QList<SBParameter>
{
protected:
  QDict<SBParameter>	*ParByName;
  virtual int		compareItems(QCollection::Item p1, QCollection::Item p2)
    {
      const SBParameter *P1 = (const SBParameter*)p1;
      const SBParameter *P2 = (const SBParameter*)p2;
      return P1->str4compare()<P2->str4compare()?-1:P1->str4compare()==P2->str4compare()?0:1;
    };
  virtual QDataStream&		write(QDataStream& s, QCollection::Item E) const
    {return s << *(const SBParameter*)E;};
  virtual QDataStream&		read(QDataStream& s, QCollection::Item &E)
    {E = new SBParameter; return s >> *(SBParameter*)E;};

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of the object.
   */
  SBParameterList(): QList<SBParameter>()
    {
      ParByName=new QDict<SBParameter>(20);
      setAutoDelete(FALSE);
      ParByName->setAutoDelete(FALSE);
    };
  /**A constructor.
   * Creates a copy of the object.
   * \param L_ is value to copy in.
   */
  SBParameterList(const SBParameterList& L_) : QList<SBParameter>() {*this=L_;};
  SBParameterList& operator= (const SBParameterList&);
  /**A destructor.*/
  virtual ~SBParameterList(){if (ParByName) delete ParByName;};
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName()	const {return "SBParameterList";};
  
  // interface:
  //
  
  // functions:
  //
  virtual void		append(const SBParameter*);
  virtual void		inSort(const SBParameter*);
  virtual bool		remove(const QString&);
  virtual SBParameter*	find  (const QString& p_) const
    {return ParByName?ParByName->find(p_):NULL;};
  /**Returns an index of the parameter in the list*/
  virtual int		idx(const SBParameter*	p_) {return QList<SBParameter>::find(p_);};
  virtual int		idx(const QString&	p_) {return QList<SBParameter>::find(find(p_));};
  virtual void		clear() {ParByName->clear(); QList<SBParameter>::clear();};
  virtual void		updateSolution(const SBVector*, const SBSymMatrix*);
  virtual void		report();
  virtual void		clearPValues();
  virtual void		clearPValues_aux();

private: // disabled:
  virtual bool		remove(uint idx_)
    {return 
       at(idx_)?ParByName->remove(at(idx_)->name()) && QList<SBParameter>::remove(idx_):FALSE;};
  virtual bool		remove()
    {return 
       current()?ParByName->remove(current()->name()) && QList<SBParameter>::remove():FALSE;};
public:
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBParameterList&);
  friend QDataStream &operator>>(QDataStream&, SBParameterList&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBStochParameter.
 * Keeps info about variable in time solution.
 *
 */
/*==============================================================================================*/
class SBStochParameter : public SBParameter, public QList<SBParameter>
{
private:
  QDict<SBParameter>*	ParByTime;
  
  virtual int		compareItems(QCollection::Item p1, QCollection::Item p2)
    {
      return 
	((SBParameter*)p1)->tEpoch()< ((SBParameter*)p2)->tEpoch()?-1:
	((SBParameter*)p1)->tEpoch()==((SBParameter*)p2)->tEpoch()?0:1;
    };
  virtual QDataStream&	write(QDataStream& s, QCollection::Item E) const
    {return s << *(const SBParameter*)E;};
  virtual QDataStream&	read(QDataStream& s, QCollection::Item &E)
    {E = new SBParameter; return s >> *(SBParameter*)E;};

  virtual bool		remove(uint idx_)
    {return 
       at(idx_)?ParByTime->remove(at(idx_)->timeStamp())&&QList<SBParameter>::remove(idx_):FALSE;};
  virtual bool		remove()
    {return 
       current()?ParByTime->remove(current()->timeStamp())&&QList<SBParameter>::remove():FALSE;};

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of an object.
   */
  SBStochParameter(const char* ="Unnamed stochastic parameter", double =1.0);
  /**A destructor.*/
  virtual ~SBStochParameter();
  SBStochParameter& operator= (SBStochParameter&);
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBStochParameter";};
  
  // interface:
  //
  /**Returns the number of observations.*/
  virtual int		num(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->num():0;};
  /**Returns date of the first observation.*/
  virtual SBMJD		tStart(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->tStart():TZero;};
  /**Returns date of the last observation.*/
  virtual SBMJD		tFinis(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->tFinis():TZero;};
  /**Returns date of the mean observation.*/
  virtual SBMJD		tEpoch(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->tEpoch():TZero;};
  /**Returns interval of the observations.*/
  virtual double	tInterval(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->tInterval():0;};
  /**Returns value of the parameter.*/
  virtual double	v(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->v():0;};
  /**Returns value of the last solution.*/
  virtual double	s(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->s():0;};
  /**Returns value of the formal error.*/
  virtual double	e(const SBMJD& t) const 
    {SBParameter* W; return (W=find(t))?W->e():0;};

  // functions:
  //
  //  virtual void		inSort(const SBParameter*);
  virtual SBParameter*	find  (const SBMJD& t_) const
    {return ParByTime?ParByTime->find(t_.toString(SBMJD::F_INTERNAL)):NULL;};
  virtual void		clear() {ParByTime->clear(); QList<SBParameter>::clear();};
  
  virtual void		update(const SBParameter*);
  virtual void		addPar(const SBParameter*);
  virtual void		report();
  virtual void		dump2File(const QString&);

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBStochParameter&);
  friend QDataStream &operator>>(QDataStream&, SBStochParameter&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBStochParameterList.
 * List of stochastic parameters, ordered by name.
 *
 */
/*==============================================================================================*/
class SBStochParameterList : public QList<SBStochParameter>
{
protected:
  QDict<SBStochParameter>	*ParByName;
  virtual int			compareItems(QCollection::Item p1, QCollection::Item p2)
    {return cmp(*(const SBNamed*)p1, *(const SBNamed*)p2);};

  virtual QDataStream&		write(QDataStream& s, QCollection::Item E) const
    {return s << *(const SBStochParameter*)E;};
  virtual QDataStream&		read(QDataStream& s, QCollection::Item &E)
    {E = new SBStochParameter; return s >> *(SBStochParameter*)E;};

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of the object.
   */
  SBStochParameterList() : QList<SBStochParameter>()
    {
      ParByName=new QDict<SBStochParameter>(20);
      ParByName->setAutoDelete(FALSE);
      setAutoDelete(TRUE);
    };
  /**A destructor.*/
  virtual ~SBStochParameterList(){if (ParByName) delete ParByName;};
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName()	const {return "SBStochParameterList";};
  
  // interface:
  //
  
  // functions:
  //
  virtual void		inSort(const SBStochParameter*);
  virtual bool		remove(const QString&);
  virtual SBStochParameter*	find  (const QString& p_) const
    {return ParByName?ParByName->find(p_):NULL;};
  /**Returns an index of the parameter in the list*/
  virtual int		idx(const SBStochParameter* p_) {return QList<SBStochParameter>::find(p_);};
  virtual int		idx(const QString&	p_) {return QList<SBStochParameter>::find(find(p_));};
  virtual void		clear() {ParByName->clear(); QList<SBStochParameter>::clear();};
  virtual void		report();

private: // disabled:
  virtual bool		remove(uint idx_)
    {return 
       at(idx_)?ParByName->remove(at(idx_)->name()) && QList<SBStochParameter>::remove(idx_):FALSE;};
  virtual bool		remove()
    {return 
       current()?ParByName->remove(current()->name()) && QList<SBStochParameter>::remove():FALSE;};
public:
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBStochParameterList&);
  friend QDataStream &operator>>(QDataStream&, SBStochParameterList&);
};
/*==============================================================================================*/




/*==============================================================================================*/
/**Class SBStatistics, describes statistics info about a solution.
 */
/*==============================================================================================*/
class SBStatistics : public SBNamed
{
private:
  // general stuff:
  double			WPFR;		//!< weighted post fit residuals (= V^(T)PV )
  double			WRMS;		//!< weighted root means squares (= sum(O_C^2))
  double			SumW;		//!< sum of weights
  double			SumW2;		//!< sum of squares of weights
  unsigned int			NumObs;		//!< number of observations
  unsigned int			NumPars;	//!< number of parameters
  unsigned int			NumConstr;	//!< number of constraints
  SBMJD				TStart;		//!< epoch of first observation

public:
  // constructors/destructors:
  //
  SBStatistics(const QString& ="Unknown");
  ~SBStatistics(){};
  QString	ClassName() const {return "SBStatistics";};
  
  // interface:
  //
  /**Returns weighted post fit residuals*/
  double	wpfr() const {return WPFR;};
  /**Returns weighted root means squares*/
  double	wrms() const {return WRMS;};
  /**Returns the number of observations*/
  unsigned int	numObs() const {return NumObs;};
  /**Returns the number of parameters*/
  unsigned int	numPars() const {return NumPars;};
  /**Returns the number of constraints*/
  unsigned int	numConstr() const {return NumConstr;};
  /**Returns the epoch of first observation*/
  const SBMJD&	tStart() const {return TStart;};
  /**Sets up post fit residuals*/
  void		setWPFR(double d) {WPFR=d;};
  /**Sets up weighted root means squares*/
  void		setWRMS(double d) {WRMS=d;};
  /**Increments weighted post fit residuals*/
  void		addWPFR(double d) {WPFR+=d;};
  /**Increments weighted root means squares*/
  void		addWRMS(double O_C, double W) {WRMS+=O_C*O_C*W*W; SumW+=W; SumW2+=W*W;};

  /**Sets up the number of observations*/
  void		setNumObs(unsigned int n) {NumObs=n;};
  /**Sets up the number of parameters*/
  void		setNumPars(unsigned int n) {NumPars=n;};
  /**Sets up the number of constraints*/
  void		setNumConstr(unsigned int n) {NumConstr=n;};
  /**Increments the number of observations*/
  void		addNumObs(unsigned int n) {NumObs+=n;};
  /**Increments the number of parameters*/
  void		addNumPars(unsigned int n) {NumPars+=n;};
  /**Increments the number of constraints*/
  void		addNumConstr(unsigned int n) {NumConstr+=n;};

  void		setTStart(const SBMJD& T_) {TStart = T_;};

  // functions:
  //
  void		clearStatistics();
  /**Returns `degree of freedom'*/
  unsigned int	numberOfObservations() const {return NumObs-NumConstr;};

  unsigned int	dof()         const {return numberOfObservations() - NumPars;};
  double	varFactor()   const {return dof()>0?sqrt(WPFR/dof()):0.0;};


  // has the same units as observables..
  /**Returns WRMS per observations (per unit of weight); dimension the same as obs*/
  double	WRMS_unit()      const     {return SumW2>0.0?sqrt(WRMS/SumW2):0.0;};
  /**Returns WPFR per observations (per unit of weight)*/
  double	VarFactor_unit() const     {return SumW2>0.0?sqrt(WPFR/SumW2):0.0;};

  // I/O
  friend QDataStream &operator<<(QDataStream&, const SBStatistics&);
  friend QDataStream &operator>>(QDataStream&, SBStatistics&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStatisticsList									*/
/*												*/
/*==============================================================================================*/
class SBStatisticsList : public QList<SBStatistics>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      if ( *(SBNamed*)item1 == *(SBNamed*)item2 ) return 0;
      return *(SBNamed*)item1 < *(SBNamed*)item2?-1:1;
      //      if ( ((SBStatistics*)item1)->tStart() == ((SBStatistics*)item2)->tStart() ) return 0;
      //      return ((SBStatistics*)item1)->tStart() < ((SBStatistics*)item2)->tStart()?-1:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBStatistics*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBStatistics; return s >> *(SBStatistics*)Entry;};
};
/*==============================================================================================*/



class SBProject;
/*==============================================================================================*/
/**Class SBSolution, describes solution.
 * The solution is the set of global parameters (common for all data), local parameters (wich
 * are depended on date of observation) and a stochastic parameters (date and time dependant).
 */
/*==============================================================================================*/
class SBSolution
{
private:
  // general stuff:
  SBProject*			Prj;		//!< project (not an owner of);
  SBStatistics			StatGeneral;	//!< general statistics information
  SBStatisticsList		StatByBatch;	//!< list of statistics info per a batch
  bool				IsModified;
  SBStatistics			*Stat4CurrentBatch;
  QString			StatisticsFile;

  // global parameters:
  SBParameterList		Global;		//!< list of global parameters;
  bool				IsGlobalModified;
  //  // arc parameters:
  //  SBParameterList		Arc;		//!< list of arc parameters (depend on session);
  // local parameters:
  SBParameterList		Local;		//!< list of local parameters;
  bool				IsLocalModified;
  QString			CurrentBatch;

  //stochastic parameters:
  SBStochParameterList		Stochastic;	//!< list of stochastic parameters;
  bool				IsStochasticModified;

  static const QString		DirGlobal;
  static const QString		DirArc;
  static const QString		DirLocal;
  static const QString		DirStoch;

  void		loadStatistics();
  bool		loadGlbPar();
  bool		loadLocPar(const QString&);
  bool		loadStcPar(const QString&);
  //  bool		loadArcPar(const SBMJD&);

  void		saveStatistics();
  void		saveGlbPar();
  void		saveGlbCor(SBParameterList*, const SBSymMatrix*);
  void		saveLocPar(const QString&);
  void		saveLocCor(const QString&, SBParameterList*, const SBSymMatrix*);
  void		saveStcPar(const QString&);
  //  bool		saveArcPar(const SBMJD&);

  bool		checkPath(const QString&, const QString&);

  QString	path2GlbDir() const;
  QString	path2LocDir() const;
  QString	path2StcDir() const;
  //  QString	path2ArcDir() const;

  QString	path2GlbFile() const 
  {return QString(path2GlbDir() + "/parameters.bin.gz");};
  //  {return QString(path2GlbDir() + "/parameters.bin");};
  QString	path2GlbFileCorr() const 
  {return QString(path2GlbDir() + "/correlations.bin.gz");};
  //  {return QString(path2GlbDir() + "/correlations.bin");};
  QString	path2LocFile(const QString& name_) const 
  {return QString(path2LocDir() + "/" + name_ + ".par.bin.gz");};
  //  {return QString(path2LocDir() + "/" + name_ + ".par.bin");};
  QString	path2LocFileCorr(const QString& name_) const 
  {return QString(path2LocDir() + "/" + name_ + ".cor.bin.gz");};
  //  {return QString(path2LocDir() + "/" + name_ + ".cor.bin");};
  QString	path2StcFile(const QString& name_) const
  {return QString(path2StcDir() + "/" + name_ + ".par.bin.gz");};
  //  {return QString(path2StcDir() + "/" + name_ + ".par.bin");};
  //
  //  QString	path2ArcFile(const SBMJD& T) const
  //    {return QString(path2ArcDir() + "/" + T.toString(SBMJD::F_YYYYMMDD)+ ".bin");};
  QString	path2LocFileNormEqs(const QString& name_) const 
  {return QString(path2LocDir() + "/" + name_ + ".neq.bin.gz");};
  //  {return QString(path2LocDir() + "/" + name_ + ".neq.bin");};
  
  

public:
  // constructors/destructors:
  //
  SBSolution(SBProject* =NULL, bool = TRUE);
  ~SBSolution();
  QString		ClassName() const {return "SBSolution";};
  
  // interface:
  //
  /**Returns project.*/
  SBProject*		project() const {return Prj;};
  /**Returns general statistics.*/
  SBStatistics*		statGeneral() {return &StatGeneral;};
  /**Returns the list of statistics per batch.*/
  SBStatisticsList&	statByBatch() {return StatByBatch;};
  /**Returns the list of global parameters.*/
  SBParameterList&	global() {return Global;};
  /**Returns the list of curent local parameters.*/
  SBParameterList&	local()  {return Local;};
  /**Returns the list of curent stochastic parameters.*/
  SBStochParameterList&	stochastic()  {return Stochastic;};
  
  /**Sets up the project.*/
  void			setProject(SBProject* const P_) {Prj=P_;};

  // functions:
  //
  void			prepare4Batch(const QString&, bool=FALSE);
  void			submitGlobalParameters(SBParameterList*, const SBSymMatrix*);
  void			reportGlobalParameters();
  void			submitLocalParameters(SBParameterList*, const SBSymMatrix*);
  void			reportLocalParameters();
  void			submitStochasticParameters(SBParameterList*, const SBSymMatrix*);
  void			saveStochasticParameters(){saveStcPar(CurrentBatch);};
  void			reportStochasticParameters();
  void			dumpParameters();

  bool			loadGlbCor(SBParameterList*&, SBSymMatrix*&);
  bool			loadLocCor(const QString&, SBParameterList*&, SBSymMatrix*&);


  SBStatistics*		stat4CurrentBatch() {return Stat4CurrentBatch;};
  void			updateParameter(SBParameter*, const SBMJD&, bool=TRUE);
  void			getGlobalParameter4Report(SBParameter*);

  void			deleteSolution();
  bool			isLocalModified() const {return IsLocalModified;};
  void			setIsLocalModified(bool IsModified_) {IsLocalModified = IsModified_;};
  QString		guessParameterName(const QString&);
  void			saveNormalEqs(SBParameterList*, SBVector*, SBUpperMatrix*,
				      SBParameterList*, SBVector*, SBUpperMatrix*, SBMatrix*,
				      const QString&);
  void			loadNormalEqs(SBParameterList*&, SBVector*&, SBUpperMatrix*&,
				      SBParameterList*&, SBVector*&, SBUpperMatrix*&, SBMatrix*&,
				      const QString&);
};
/*==============================================================================================*/



#endif //SB_GEO_PARAMETER_H
