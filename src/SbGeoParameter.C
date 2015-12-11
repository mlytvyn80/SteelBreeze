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

#include "SbGeoParameter.H"
#include "SbSetup.H"
#include "SbVector.H"
#include "SbGeoProject.H"

#include <qcstring.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qregexp.h>




/*==============================================================================================*/
/*												*/
/* class SBParameterCfg implementation								*/
/*												*/
/*==============================================================================================*/
SBParameterCfg::SBParameterCfg()
  : SBNamed("Some parameter"), ScaleName("lapot'")
{
  IsPropagated	= FALSE;
  IsConstrained	= FALSE;
  Type		= PT_NONE;
  StochasticType= ST_RANDWALK;
  ConvAPriori	= 1.0;
  StocAPriori	= 1.0;
  BreakNoise	= 1.0;
  Tau		= 1.0;
  WhiteNoise	= 1.0;
  Scale		= 1.0;
};

SBParameterCfg::SBParameterCfg(const QString& Name_, bool IsPropagated_, bool IsConstrained_, 
			       PType Type_, 
			       double ConvAPriori_, double StocAPriori_, 
			       double BreakNoise_, double Tau_, double WhiteNoise_, 
			       double Scale_, const QString& ScaleName_,
			       StochType StochasticType_)
  : SBNamed(Name_)
{
  IsPropagated	= IsPropagated_;
  IsConstrained	= IsConstrained_;
  Type		= Type_;
  StochasticType= StochasticType_;
  ConvAPriori	= ConvAPriori_;
  StocAPriori	= StocAPriori_;
  BreakNoise	= BreakNoise_;
  Tau		= Tau_;
  WhiteNoise	= WhiteNoise_;
  Scale		= Scale_;
  ScaleName	= ScaleName_.copy();
};

SBParameterCfg& SBParameterCfg::operator=(const SBParameterCfg& P)
{
  SBNamed::operator= (P);
  setPropagated(P.isPropagated());
  setConstrained(P.isConstrained());
  setType(P.type());
  setStochasticType(P.stochasticType());
  setConvAPriori(P.convAPriori());
  setStocAPriori(P.stocAPriori());
  setBreakNoise(P.breakNoise());
  setTau(P.tau());
  setWhiteNoise(P.whiteNoise());
  setScale(P.scale());
  setScaleName(P.scaleName());
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBParameterCfg's friends implementation						*/
/*												*/
/*==============================================================================================*/
/**\relates SBParameterCfg
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBParameterCfg& P)
{
  return s 
    << (const SBNamed&) P
    << (uint)P.IsPropagated << (uint)P.IsConstrained 
    << (uint)P.Type << (uint)P.StochasticType 
    << P.ConvAPriori << P.StocAPriori
    << P.BreakNoise << P.Tau << P.WhiteNoise << P.Scale << P.ScaleName;
};

/**\relates SBParameterCfg
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBParameterCfg& P)
{
  return s 
    >> (SBNamed&) P
    >> (uint&)P.IsPropagated >> (uint&)P.IsConstrained 
    >> (uint&)P.Type >> (uint&)P.StochasticType 
    >> P.ConvAPriori >> P.StocAPriori
    >> P.BreakNoise >> P.Tau >> P.WhiteNoise >> P.Scale >> P.ScaleName;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBParametersDesc implementation							*/
/*												*/
/*==============================================================================================*/
const int SBParametersDesc::Num(NUMBER_OF_PARAMETERS);
SBParametersDesc::SBParametersDesc()
{
  //	const QString& Name, bool IsPropagated, bool IsConstrained, PType Type, 
  //	double ConvAPriori, double StocAPriori, 
  //	double BreakNoise, double Tau, double WhiteNoise, 
  //	double Scale, const QString& ScaleName,
  //    StochType StochasticType

  // Station's clock offset:
  Pars[0] =SBParameterCfg("Clock Offset", FALSE, FALSE, SBParameterCfg::PT_LOC, 
			  1.e8, 1.e8, 1.e8, 1., 20.0, 1.0e12, "ps");
  // Station's clock trend:
  Pars[1] =SBParameterCfg("Clock trend", FALSE, FALSE, SBParameterCfg::PT_LOC, 
			  1.e8, 1.e8, 1.e8, 1.,  1.0, 1.0e12, "ps/s");
  // Station's clock, 2-nd order:
  Pars[2] =SBParameterCfg("Clock 2-nd order", FALSE, FALSE, SBParameterCfg::PT_LOC, 
			  1.e8, 1.e8, 1.e8, 1.,  1.0, 1.0e12, "ps/s/s");
  // Station's clock, 3-rd order:
  Pars[3] =SBParameterCfg("Clock 3-rd order", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1., 1., 1., 1., 1., 1., "ps/(s^3)");
  // Coordinates of a radio source:
  Pars[4] =SBParameterCfg("Source's coordinates", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e8, 1.e8, 1.e8, 1., 0.16,  1.e3*RAD2SEC, "mas");
  // Coordinates of a station:
  Pars[5] =SBParameterCfg("Station's coordinates", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e8, 1.e6, 1.e6, 1., 0.1, 1.e2, "cm");
  // Velocities of a station:
  Pars[6] =SBParameterCfg("Station's velocities", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e7, 1.e7, 1.e7, 1., 0.1, 1.e3, "mm/year");
  // Zenith delay:
  Pars[7] =SBParameterCfg("Zenith delay", FALSE, FALSE, SBParameterCfg::PT_LOC, 
			  10000., 10000., 10000., 1., 1.6, 1.e2, "cm");
  // Atmospheric gradient:
  Pars[8]=SBParameterCfg("Atmospheric gradient", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			 1.e4, 1.e4, 1.e4, 1., 0.06, 1.e2, "cm");
  // Axis offset:
  Pars[9] =SBParameterCfg("Axis offset", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e6, 1.e6, 1.e6, 1., 0.1, 1.e2, "cm");
  // Polus coordinates:
  Pars[10] =SBParameterCfg("Polus coordinates dX & dY", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			   1.e8, 1.e4, 1.e4, 1., 0.18,  1.e3*RAD2SEC, "mas");
  // Polus d(UT1-UTC):
  Pars[11]=SBParameterCfg("Polus coordinates d(UT1-UTC)", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e8, 1.e4, 1.e4, 1., 0.012, 1.e3*DAY2SEC, "ms");
  // Nutation in obliquity and longitude:
  Pars[12]=SBParameterCfg("Nutaion longitude & obliquity", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e8, 1.e4, 1.e4, 1., 0.18,  1.e3*RAD2SEC, "mas");
  // Just a test:
  Pars[13]=SBParameterCfg("Test purposes", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e8, 1.e4, 1.e4, 1., 0.18,  RAD2SEC, "arcsec");

  // Cables (not visible for user): 
  Pars[14]=SBParameterCfg("Cable Calibration multiplier", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e6, 1.e6, 1.e6, 1.0, 1.0,  1.0, "pisc");

  // aux.parameters:
  // 
  Pars[15]=SBParameterCfg("Love Number h2", FALSE, FALSE, SBParameterCfg::PT_NONE,
			  1.e4, 1.e3, 1.e3, 1.0, 1.0,  1.0, "pisc");
  //
  Pars[16]=SBParameterCfg("Love Number l2", FALSE, FALSE, SBParameterCfg::PT_NONE,
			  1.e4, 1.e3, 1.e3, 1.0, 1.0,  1.0, "pisc");
  //
  Pars[17]=SBParameterCfg("Tide Lag", FALSE, FALSE, SBParameterCfg::PT_NONE, 
			  1.e3, 1.e2, 1.e2, 1.0, 1.0,  RAD2SEC/60.0/60.0, "degree");
};

SBParametersDesc& SBParametersDesc::operator=(const SBParametersDesc& D)
{
  for (int i=0; i<D.num(); i++) setPar(i, D.par(i));
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBParametersDesc's friends implementation						*/
/*												*/
/*==============================================================================================*/
/**\relates SBParametersDesc
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBParametersDesc& D)
{
  for (int i=0; i<D.num(); i++) s << D.par(i);
  return s;
};

/**\relates SBParameterCfg
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBParametersDesc& D)
{
  SBParameterCfg P;
  for (int i=0; i<D.num(); i++) 
    {
      s >> P; 
      D.setPar(i, P);
    };
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDerivation implementation								*/
/*												*/
/*==============================================================================================*/
/** A constructor.
 * Creates a copy of object.
 */
SBDerivation::SBDerivation(const QString& Name_)
  : SBNamed(Name_), SBAttributed()
{
  D	 = 0.0;
  Num	 = 0;
  NumSes = 0;
  TStart = TZero;
  TFinis = TZero;
  TSum   = 0.0;

  Num_aux    = 0;
  TStart_aux = TZero;
  TFinis_aux = TZero;
  TSum_aux   = 0.0;
};

SBDerivation& SBDerivation::operator= (const SBDerivation& Der_)
{
  SBNamed::operator= (Der_);
  SBAttributed::operator= (Der_);
  
  D	= Der_.D;
  Num	= Der_.Num;
  NumSes= Der_.NumSes;
  TStart= Der_.TStart;
  TFinis= Der_.TFinis;
  TSum  = Der_.TSum;

  Num_aux   = Der_.Num_aux;
  TStart_aux= Der_.TStart_aux;
  TFinis_aux= Der_.TFinis_aux;
  TSum_aux  = Der_.TSum_aux;

  return *this;
};

void SBDerivation::setD(double D_, const SBMJD& T)
{
  D=D_;
  if (T!=TZero) // we use T == TZero in constraints and specials
    {
      Num++;
      if (TSum==0.0)
	TStart=TFinis=TSum=T;
      else 
	TSum+=(TFinis=T);

      Num_aux++;
      if (TSum_aux==0.0)
	TStart_aux=TFinis_aux=TSum_aux=T;
      else 
	TSum_aux+=(TFinis_aux=T);
    };
};

void SBDerivation::clearValues()
{
  D = 0.0;
  Num = 0;
  NumSes = 0;
  TStart = TZero;
  TFinis = TZero;
  TSum   = 0.0;
};

void SBDerivation::clearValues_aux()
{
  D = 0.0;
  Num_aux = 0;
  TStart_aux = TZero;
  TFinis_aux = TZero;
  TSum_aux   = 0.0;
  
  //  std::cout << name() << " cleared\n";
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDerivation's friends implementation							*/
/*												*/
/*==============================================================================================*/
/**\relates SBDerivation
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBDerivation& D)
{
  //  std::cout << "Writing: " << D.name() << ": " << D.NumSes << "\n";
  //  return s << (const SBNamed&)D << (const SBAttributed&)D 
  s << D.name().local8Bit() << (const SBAttributed&)D 
    << D.Num << D.NumSes << D.TStart << D.TFinis;
  s.writeRawBytes((const char*)&D.TSum, sizeof(long double));

  s << D.Num_aux << D.TStart_aux << D.TFinis_aux;
  s.writeRawBytes((const char*)&D.TSum_aux, sizeof(long double));
  return s;
};

/**\relates SBDerivation
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBDerivation& D)
{
  QCString	CName;
  s >> CName >> (SBAttributed&)D
    >> D.Num >> D.NumSes >> D.TStart >> D.TFinis;
  s.readRawBytes((char*)&D.TSum, sizeof(long double));
  s >> D.Num_aux >> D.TStart_aux >> D.TFinis_aux;
  s.readRawBytes((char*)&D.TSum_aux, sizeof(long double));

  D.setName(CName);
  //  std::cout << "Reading: " << D.name() << ": " << D.NumSes << "\n";
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBParameter implementation								*/
/*												*/
/*==============================================================================================*/
/** A constructor.
 * Creates a copy of object.
 */
SBParameter::SBParameter(const QString& Name_, double Scale_)
  : SBDerivation(Name_)
{
  V = S = E = 0.0;
  SType		= SBParameterCfg::ST_NONE;
  SigmaAPriori	= 1.0;
  Tau		= 0.0;
  PSD_RWN	= 1.0;
  PSD_BN	= 1.0;
  Scale		= Scale_;
  Prefix	= ' ';
};

SBParameter& SBParameter::operator= (const SBParameter& Par_)
{
  SBDerivation::operator= (Par_);
  SType		= Par_.SType;
  V		= Par_.V;
  S		= Par_.S;
  E		= Par_.E;
  SigmaAPriori	= Par_.SigmaAPriori;
  Tau		= Par_.Tau;
  PSD_RWN	= Par_.PSD_RWN;
  PSD_BN	= Par_.PSD_BN;
  Scale		= Par_.scale();
  Prefix	= Par_.prefix();
  return *this;
};

void SBParameter::tuneParameter(const SBParameterCfg& Cfg)
{
  Scale		= Cfg.scale();
  SigmaAPriori	= (Cfg.type()==SBParameterCfg::PT_STH?Cfg.stocAPriori():Cfg.convAPriori())/Scale;
  Tau		= Cfg.tau();
  PSD_RWN	= Cfg.whiteNoise()/Scale;
  PSD_BN	= Cfg.breakNoise()/Scale;
  SType		= Cfg.stochasticType();
  if (Cfg.isPropagated())
    addAttr(IsPropagated);
  else 
    delAttr(IsPropagated);
};

void SBParameter::update(const SBParameter* P_)
{
  if (P_)
    {
      V+= P_->s();
      S = P_->s();
      E = P_->e();
    }
  else
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot update from NULL");
};

void SBParameter::clearValues()
{
  SBDerivation::clearValues();
  V = S = E = 0.0;
};

double SBParameter::m (double dT) // dT = t_(i+1) - t_i,  in days 
{
  double M=0.0;
  switch (SType)
    {
    case SBParameterCfg::ST_UNKN:
      M = isAttr(IsBreak)?0.0:1.0;
      break;
    case SBParameterCfg::ST_NONE:
    case SBParameterCfg::ST_WHITENOISE:
      M = 0.0;
      break;
    case SBParameterCfg::ST_MARKOVPROCESS:
      M = exp( -24.0*dT/Tau ); // Tau in hours
      break;
    default:
    case SBParameterCfg::ST_RANDWALK:
      //M = isAttr(IsBreak)?0.0:1.0;
      M = 1.0;
      break;
    };
  return M;
};

double SBParameter::rw(double dT)
{
  double PSD = isAttr(IsBreak)?PSD_BN:PSD_RWN;
  double E = 0.0;
  switch (SType)
    {
    case SBParameterCfg::ST_UNKN:
      E = 24.0*dT*24.0*dT*PSD*PSD/2.0;
      break;
    case SBParameterCfg::ST_NONE:
    case SBParameterCfg::ST_WHITENOISE:
      E = 0.0;
      break;
    case SBParameterCfg::ST_MARKOVPROCESS:
      E = Tau/2.0*(1.0 - m(dT)*m(dT))*PSD*PSD; // Tau in hours
      break;
    default:
    case SBParameterCfg::ST_RANDWALK:
      E = 24.0*dT*PSD*PSD;
      //      E = 24.0*dT*24.0*dT*PSD*PSD/2.0;	???IRW???
      break;
    };

  if (isAttr(IsBreak))
    {
      Log->write(SBLog::INF, SBLog::ESTIMATOR, ClassName() + 
		 ": `Break Flag` detected for the parameter [" + name() + "]; processed");
      delAttr(IsBreak);
    };
  return E>0.0?1.0/sqrt(E):SigmaAPriori;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBParameter's friends implementation							*/
/*												*/
/*==============================================================================================*/
/**\relates SBParameter
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBParameter& P)
{
  return s << (const SBDerivation&)P << P.V << P.S << P.E << P.Scale;
};

/**\relates SBParameter
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBParameter& P)
{
  return s >> (SBDerivation&)P >> P.V >> P.S >> P.E >> P.Scale;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBParameterList implementation								*/
/*												*/
/*==============================================================================================*/
SBParameterList& SBParameterList::operator= (const SBParameterList& L_)
{
  setAutoDelete(FALSE);
  if (ParByName) delete ParByName;
  if (count()) clear();
  ParByName = new QDict<SBParameter>(L_.count()+20);
  ParByName->setAutoDelete(FALSE);
  if (L_.count())
    {
      QListIterator<SBParameter> it(L_);
      for ( ; it.current(); ++it )
	inSort(new SBParameter(*it.current()));
    };
  return *this;
};

void SBParameterList::append(const SBParameter* p)
{
  if (!p)
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": appending of NULL parameter: failed");
  else
    {
      if (ParByName->find(p->name()))
	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		   ": cannot append the parameter [" + p->name() + "], already exists in the list");
      else
	{
	  QList<SBParameter>::append(p);
	  
	  if (ParByName->size()<=ParByName->count())
	    ParByName->resize(ParByName->size() + 10);
	  ParByName->insert(p->name(), p);
	};
    };
};

void SBParameterList::inSort(const SBParameter* p)
{
  if (!p)
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": insert of NULL parameter; failed");
  else
    {
      if (ParByName->find(p->name()))
	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		   ": cannot insert the parameter [" + p->name() + "], already inserted");
      else
	{
	  QList<SBParameter>::inSort(p);
	  
	  if (ParByName->size()<=ParByName->count())
	    ParByName->resize(ParByName->size() + 10);
	  ParByName->insert(p->name(), p);
	};
    };
};

bool SBParameterList::remove(const QString& key)
{
  SBParameter* p;
  
  if ((p = ParByName->find(key)))
    return ParByName->remove(key) && QList<SBParameter>::remove(p);
  else
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": cannot delete the parameter: cannot find the key in the dictionary");
      return FALSE;
    };
};

void SBParameterList::updateSolution(const SBVector* X, const SBSymMatrix* P)
{
  if (!X)
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": updateSolution: the vector is NULL");
      return;
    };
  if (!P)
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": updateSolution: the matrix is NULL");
      return;
    };

  if (count()!=X->n())
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": updateSolution: the number of parameters and the dimension of the vector " + 
		 X->name() + " are different; the solution ignored");
      return;
    };
  if (count()!=P->n())
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": updateSolution: the number of parameters and the dimension of the matrix " + 
		 P->name() + " are different; the solution ignored");
      return;
    };
  
  if (! (count()*X->n()*P->n()) ) return;
  
  SBParameter	*p;
  unsigned int	 i;

  for (i=0, p=first(); i<X->n(); i++, p=next())
    {
      p->setS(X->at(i));
      p->setE(sqrt(P->at(i,i)));
    };
};

void SBParameterList::report()
{
  if (Log->isEligible(SBLog::DBG, SBLog::ESTIMATOR))
    {
      SBParameter	*p;
      QString		S;
      Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": Solution: reporting:");
      for (p=first(); p; p=next())
	{
	  S.sprintf(": %20.8f/%20.8f (%.8f) @ %8d pnt ", 
		    p->v()*p->scale(), p->s()*p->scale(), p->e()*p->scale(), p->num());
	  Log->write(SBLog::DBG, SBLog::ESTIMATOR, p->name() + S + "[" + 
		     p->tStart().toString(SBMJD::F_YYYYMMDDHHMMSSSS) + " : " + 
		     p->tFinis().toString(SBMJD::F_YYYYMMDDHHMMSSSS) + "] (" +
		     p->tEpoch().toString(SBMJD::F_YYYYMMDDHHMMSSSS) + ")");
	};
    };
};

void SBParameterList::clearPValues()
{
  for (SBParameter *p=first(); p; p=next()) 
    p->clearValues();
};

void SBParameterList::clearPValues_aux()
{
  for (SBParameter *p=first(); p; p=next()) 
    p->clearValues_aux();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBParameterList's friends implementation						*/
/*												*/
/*==============================================================================================*/
/**\relates SBParameterList
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBParameterList& P)
{
  return s << (const QList<SBParameter>&)P;
};

/**\relates SBParameterList
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBParameterList& P)
{
  if (P.count()) P.clear();
  s >> (QList<SBParameter>&)P;

  if (P.ParByName) delete P.ParByName;
  P.ParByName = new QDict<SBParameter>(P.count() + 20);
  P.ParByName->setAutoDelete(FALSE);
  if (P.count())
    for (SBParameter* p=P.first(); p; p=P.next())
      P.ParByName->insert(p->name(), p);
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStochParameter implementation							*/
/*												*/
/*==============================================================================================*/
SBStochParameter::SBStochParameter(const char* Name_, double Scale_)
  : SBParameter(Name_, Scale_), QList<SBParameter>()
{
  ParByTime = new QDict<SBParameter>(200);
  setAutoDelete(TRUE);
  ParByTime->setAutoDelete(FALSE);
};

SBStochParameter& SBStochParameter::operator= (SBStochParameter& Par_)
{
  SBParameter::operator= (Par_);

  if (ParByTime) delete ParByTime;
  setAutoDelete(TRUE);
  if (count()) clear();
  
  ParByTime = new QDict<SBParameter>(Par_.count() + 20);
  ParByTime->setAutoDelete(FALSE);

  SBParameter	*W=NULL;
  QString	l;
  if (Par_.count())
    for (SBParameter* p=Par_.first(); p; p=Par_.next())
      {
	l = p->timeStamp();
	QList<SBParameter>::inSort((W = new SBParameter (*p)));
	ParByTime->insert(l, W);
      };
  
  return *this;
};

SBStochParameter::~SBStochParameter()
{
  if (ParByTime) delete ParByTime;
};

void SBStochParameter::update(const SBParameter* p)
{
  SBParameter	*W=NULL;
  if (!p)
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": inserting NULL parameter; failed");
  else if (p->name() != name())
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": wrong stochastic parameter [" + 
	       p->name() + "], expecting [" + name() + "]");
  else if (p->num()) //there can be "artificial" estimations, they're've got a Num==0
    {
      QString l = p->timeStamp();
      if ((W=ParByTime->find(l)))
	{
	  W->update(p);
	}
      else
	{
	  QList<SBParameter>::inSort((W = new SBParameter (*p)));
	  W ->setV(p->s());
	  ParByTime->insert(l, W);
	};
      
      if (ParByTime->size()<=ParByTime->count())
	ParByTime->resize(ParByTime->size() + 50);
    };
};

void SBStochParameter::addPar(const SBParameter* p)
{
  SBParameter	*W=NULL;
  if (!p)
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": inserting NULL parameter; failed");
  else if (p->name() != name())
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": wrong stochastic parameter [" + 
	       p->name() + "], expecting [" + name() + "]");
  else if (p->num()) //there can be "artificial" estimations, they're've got a Num==0
    {
      QString l = p->timeStamp();
      if ((W=ParByTime->find(l)))
	{
	  Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": stochastic parameter [" + 
		     p->name() + "], already added");
	  *W = *p;
	}
      else
	{
	  QList<SBParameter>::inSort((W = new SBParameter (*p)));
	  ParByTime->insert(l, W);
	};
      
      if (ParByTime->size()<=ParByTime->count())
	ParByTime->resize(ParByTime->size() + 50);
    };
};

void SBStochParameter::report()
{
  QString	Str;
  SBParameter	*p;
  if (Log->isEligible(SBLog::DBG, SBLog::STOCHPARS))
    {
      for (p=first(); p; p=next())
	{
	  Str.sprintf(": %20.8f/%20.8f (%.8f) @ %8d pnt ", 
		    p->v()*p->scale(), p->s()*p->scale(), p->e()*p->scale(), p->num());
	  Log->write(SBLog::DBG, SBLog::STOCHPARS, p->name() + Str + "[" + 
		     p->tStart().toString(SBMJD::F_YYYYMMDDHHMMSSSS) + " : " + 
		     p->tFinis().toString(SBMJD::F_YYYYMMDDHHMMSSSS) + "] (" +
		     p->tEpoch().toString(SBMJD::F_YYYYMMDDHHMMSSSS) + ")");
	};
    }
  else
  if (Log->isEligible(SBLog::DBG, SBLog::ESTIMATOR))
    {
      if (count())
	{
	  // grr..
	  unsigned int n = 0;
	  double	fV=0.0;
	  double	fS=0.0;
	  double	fE=0.0;
	  SBMJD	Tst = first()->tStart();
	  SBMJD	Tfi = last() ->tFinis();
	  SBMJD	Tep = TZero;
	  for (p=first(); p; p=next())
	    {
	      if (Tep==TZero)
		Tep = p->tEpoch()*p->num();
	      else
		Tep += p->tEpoch()*p->num();
	      fV += p->v()*p->num();
	      fS += p->s()*p->num();
	      fE += p->e()*p->num();
	      n  += p->num();
	    };
	  Tep = Tep/n;
	  Str.sprintf(": %20.8f/%20.8f (%.8f) @ %8d pnt ", fV*Scale/n, fS*Scale/n, fE*Scale/n, n);
	  Log->write(SBLog::DBG, SBLog::ESTIMATOR, Name + Str + "[" + 
		     Tst.toString(SBMJD::F_YYYYMMDDHHMMSSSS) + " : " + 
		     Tfi.toString(SBMJD::F_YYYYMMDDHHMMSSSS) + "] (" +
		     Tep.toString(SBMJD::F_YYYYMMDDHHMMSSSS) + ")");
	}
      else
	Log->write(SBLog::WRN, SBLog::ESTIMATOR, Name + " HAS GOT 0 (ZERO) OBSERVATIONS, plz, remove me..");
    };
};

void SBStochParameter::dump2File(const QString& FileName)
{
  QFile		f(FileName);
  if (f.open(IO_WriteOnly))
    {
      QTextStream	s(&f);
      
      s << "# Dump of the parameter `" + name() + "'\n";
      s << "# First  column: Time, MJD\n";
      s << "# Second column: the value of the parameter\n";
      s << "# Third  column: the formal error of the solution\n";
      s << "# Fourth column: the number of obs. per solution\n";
      s << "# Fivth  column: Time, number of seconds from 1st of Jan, 1970\n";
      s << "#_________ ____________________  ____________________  ________ ________________\n";
      
      QString	Str;
      SBParameter	*p;
      for (p=first(); p; p=next())
	{
	  Str.sprintf("%12.6f  %20.8f  %20.8f  %8d ",
	    (double)p->tEpoch(),
	    p->v()*p->scale(), p->e()*p->scale(), p->num());
//	  s << p->tEpoch().toString(SBMJD::F_MJD)
	  s
	    << Str << " " << p->tEpoch().toString(SBMJD::F_UNIX) << "\n";
	};
      
      s.unsetDevice();
      f.close();
    }
  else
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
	       ": cannot write to file \"" + FileName + "\"");
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStochParameter's friends implementation						*/
/*												*/
/*==============================================================================================*/
/**\relates SBStochParameter
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBStochParameter& P)
{
  return s << (const SBParameter&)P << (const QList<SBParameter>&)P;
};

/**\relates SBStochParameter
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBStochParameter& P)
{
  if (P.count()) P.clear();
  
  s >> (SBParameter&)P >> (QList<SBParameter>&)P;

  if (P.ParByTime) delete P.ParByTime;
  P.ParByTime = new QDict<SBParameter>(P.count() + 20);
  P.ParByTime->setAutoDelete(FALSE);
  if (P.count())
    for (SBParameter* p=P.first(); p; p=P.next())
      P.ParByTime->insert(p->timeStamp(), p);

  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStochParameterList implementation							*/
/*												*/
/*==============================================================================================*/
void SBStochParameterList::inSort(const SBStochParameter* p)
{
  if (!p)
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": insert of NULL stoch parameter; failed");
  else
    {
      if (ParByName->find(p->name()))
	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		   ": cannot insert the stoch parameter [" + p->name() + "], already inserted");
      else
	{
	  QList<SBStochParameter>::inSort(p);
	  
	  if (ParByName->size()<=ParByName->count())
	    ParByName->resize(ParByName->size() + 10);
	  ParByName->insert(p->name(), p);
	};
    };
};

bool SBStochParameterList::remove(const QString& key)
{
  SBStochParameter* p;
  
  if ((p = ParByName->find(key)))
    return ParByName->remove(key) && QList<SBStochParameter>::remove(p);
  else
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": cannot delete the stoch parameter: cannot find the key in the dictionary");
      return FALSE;
    };
};

void SBStochParameterList::report()
{
  SBStochParameter	*p;
  Log->write(SBLog::DBG, SBLog::ESTIMATOR | SBLog::STOCHPARS, ClassName() + 
	     ": Stochasic Parameters Report:");
  for (p=first(); p; p=next())
    p->report();
  Log->write(SBLog::DBG, SBLog::ESTIMATOR | SBLog::STOCHPARS, ClassName() + ": End of Report");
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStochParameterList's friends implementation						*/
/*												*/
/*==============================================================================================*/
/**\relates SBStochParameterList
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBStochParameterList& P)
{
  return s << (const QList<SBStochParameter>&)P;
};

/**\relates SBParameterList
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBStochParameterList& P)
{
  if (P.count()) P.clear();
  s >> (QList<SBStochParameter>&)P;

  if (P.ParByName) delete P.ParByName;
  P.ParByName = new QDict<SBStochParameter>(P.count() + 20);
  P.ParByName->setAutoDelete(FALSE);
  if (P.count())
    for (SBStochParameter* p=P.first(); p; p=P.next())
      P.ParByName->insert(p->name(), p);
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStatistics implementation								*/
/*												*/
/*==============================================================================================*/
SBStatistics::SBStatistics(const QString& Name_)
  : SBNamed(Name_)
{
  WPFR	    = 0.0;
  WRMS	    = 0.0;
  SumW	    = 0.0;
  SumW2	    = 0.0;
  NumObs    = 0;
  NumPars   = 0;
  NumConstr = 0;
  TStart    = TZero;
};

void SBStatistics::clearStatistics()
{
  WPFR	    = 0.0;
  WRMS	    = 0.0;
  SumW	    = 0.0;
  SumW2	    = 0.0;
  NumObs    = 0;
  NumPars   = 0;
  NumConstr = 0;
  TStart    = TZero;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStatistics's friends implementation							*/
/*												*/
/*==============================================================================================*/
/**\relates SBSolution::SBStatistics
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBStatistics& S)
{
  return s << (const SBNamed&)S << S.WPFR << S.WRMS << S.SumW << S.SumW2 
	   << S.NumObs << S.NumPars << S.NumConstr << S.TStart;
};

/**\relates SBSolution::SBStatistics
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBStatistics& S)
{
  return s >> (SBNamed&)S >> S.WPFR >> S.WRMS >> S.SumW >> S.SumW2
	   >> S.NumObs >> S.NumPars >> S.NumConstr >> S.TStart;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSolution implementation								*/
/*												*/
/*==============================================================================================*/
const QString SBSolution::DirGlobal("Global");
const QString SBSolution::DirArc   ("Arc");
const QString SBSolution::DirLocal ("Local");
const QString SBSolution::DirStoch ("Stochastic");

SBSolution::SBSolution(SBProject* Prj_, bool isRealRun)
{
  Prj = Prj_;

  StatGeneral.setName("General statistics for the project " + Prj->name());
  StatByBatch.setAutoDelete(TRUE);
  StatByBatch.clear();
  StatisticsFile = SetUp->path2Sols(Prj->name()) + "Statistics.bin";

  Global.setAutoDelete(TRUE);
  Global.clear();
  Local.setAutoDelete(TRUE);
  Local.clear();
  Stochastic.setAutoDelete(TRUE);
  Stochastic.clear();
  Stat4CurrentBatch = NULL;

  if (isRealRun)
    {
      QDir	d(SetUp->path2Sols(Prj->name()));
      if (!d.exists())
	{
	  if (d.mkdir(SetUp->path2Sols(Prj->name())))
	    Log->write(SBLog::DBG, SBLog::CONFIG | SBLog::ESTIMATOR, ClassName() + 
		       ": the directory has been created: [" + SetUp->path2Sols(Prj->name()) + "]");
	  else
	    {
	      Log->write(SBLog::ERR, SBLog::CONFIG | SBLog::ESTIMATOR, ClassName() + 
			 ": cannot create the directory [" + SetUp->path2Sols(Prj->name()) + "]");
	      QMessageBox::critical(0, Version.selfName(),
				    "it's impossible to create the directory [" + 
				    SetUp->path2Sols(Prj->name()) + 
				    "]\nso, you have to exit and fix it by yourself..\n", "OK");
	      exit(1);
	    };
	};
      QFileInfo fi(SetUp->path2Sols(Prj->name()));
      if (!fi.isWritable())
	{
	  Log->write(SBLog::ERR, SBLog::CONFIG | SBLog::ESTIMATOR, ClassName() + 
		     ": the directory [" + SetUp->path2Sols(Prj->name()) + "] is not writable");
	  QMessageBox::critical(0, Version.selfName(), 
				"it's impossible to write into the directory [" + 
				SetUp->path2Sols(Prj->name()) + 
				"]\nso, you have to exit and fix it by yourself..\n", "OK");
	  exit(1);
	};
      loadStatistics();
      loadGlbPar();

      QString	Dir2Output = SetUp->path2Output() + Prj->name() + "/";
      if (!d.exists(Dir2Output) && !d.mkdir(Dir2Output))
	{
	  Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": can't create dir [" + Dir2Output + "]");
	  QMessageBox::critical(0, Version.selfName(), 
				QString("Can't create output dir\n " + Dir2Output +
					"\nfor the project [" + Prj->name() + "]"));
	  exit(1);
	};
    };
  
  IsModified = isRealRun;
  IsGlobalModified = FALSE;
  IsLocalModified = FALSE;
  IsStochasticModified = FALSE;
};

SBSolution::~SBSolution()
{
  if (IsModified)
    saveStatistics();
  if (IsGlobalModified) 
    saveGlbPar();
  StatByBatch.clear();
  Stat4CurrentBatch = NULL;
};

QString	SBSolution::path2GlbDir() const 
{
  return QString(SetUp->path2Sols(Prj->name()) + DirGlobal);
};

QString	SBSolution::path2LocDir() const 
{
  return QString(SetUp->path2Sols(Prj->name()) + DirLocal);
};

//QString SBSolution::path2ArcDir() const {return QString(SetUp->path2Sols(Prj->name()) + DirArc);};

QString	SBSolution::path2StcDir() const 
{
  return QString(SetUp->path2Sols(Prj->name()) + DirStoch);
};

bool SBSolution::checkPath(const QString& Path, const QString& File)
{
  bool		IsOK(FALSE);
  QDir		d(Path);
  QFileInfo	fi;
  
  if (!d.exists())
    {
      if (d.mkdir(Path))
	Log->write(SBLog::DBG, SBLog::CONFIG | SBLog::ESTIMATOR, ClassName() + 
		   ": the directory has been created: [" + Path + "]");
      else
	{
	  Log->write(SBLog::ERR, SBLog::CONFIG | SBLog::ESTIMATOR, ClassName() + 
		     ": cannot create the directory [" + Path + "]");
	  QMessageBox::critical(0, Version.selfName(),
				"it's impossible to create the directory [" + Path +
				"]\nso, you have to exit and fix it by yourself..\n", "OK");
	  exit(1);
	};
    };
  fi.setFile(Path);
  if (!fi.isWritable())
    {
      Log->write(SBLog::ERR, SBLog::CONFIG | SBLog::ESTIMATOR, ClassName() + 
		 ": the directory [" + Path + "] is unwritable");
      QMessageBox::critical(0, Version.selfName(), 
			    "it's impossible to write into the directory [" + Path + 
			    "]\nso, you have to exit and fix it by yourself..\n", "OK");
      exit(1);
    };

  fi.setFile(File);
  if (fi.exists()) 
    IsOK=TRUE;
  
  return IsOK;
};

// loading statistics:
void SBSolution::loadStatistics()
{
  if (checkPath(SetUp->path2Sols(Prj->name()), StatisticsFile))
    {
      QFile f(StatisticsFile);
      if (f.open(IO_ReadOnly))
	{
	  QDataStream s(&f);
	  StatByBatch.clear();
	  s >> StatGeneral >> StatByBatch;
	  if (f.status()!=IO_Ok)
	    {
	      Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
			 ": loading of the the [" + StatisticsFile + "] failed");
	      if (StatByBatch.count()) StatByBatch.clear();
	    };
	  f.close();
	  s.unsetDevice();
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + StatisticsFile + "] has been loaded");
	}
      else    
	Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": could not read the file [" + StatisticsFile + "]");
    };
  return;
};

// loading parameters:
bool SBSolution::loadGlbPar()
{
  bool			IsOK(FALSE);
  Global.clear();
  if (checkPath(path2GlbDir(), path2GlbFile()))
    {
      FILE		*p;
      QFile		File(path2GlbFile());
      QDataStream	Stream;

      if ((p=SetUp->fcList()->open4In(File.name(), File, Stream)))
	{
	  Stream >> Global;
	  IsOK=TRUE;
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been loaded");
	  
	  SetUp->fcList()->close(File, p, Stream);
	}
      else    
	Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": could not read the file [" + File.name() + "]");
    };
  return IsOK;
};

bool SBSolution::loadGlbCor(SBParameterList*& List, SBSymMatrix*& P)
{
  bool			IsOK(FALSE);
  if (checkPath(path2GlbDir(), path2GlbFileCorr()))
    {
      FILE		*p;
      QFile		File(path2GlbFileCorr());
      QDataStream	Stream;

      if ((p=SetUp->fcList()->open4In(File.name(), File, Stream)))
	{
	  if (P)
	    delete P;
	  P = new SBSymMatrix;
	  
	  if (List)
	    List->clear();
	  else
	    {
	      List = new SBParameterList;
	      List ->setAutoDelete(TRUE);
	    };
	  
	  Stream >> *List >> *P;
	  IsOK = TRUE;
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been loaded");
	  
	  SetUp->fcList()->close(File, p, Stream);
	}
      else    
	Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": could not read the file [" + File.name() + "]");
    };
  return IsOK;
};

//bool SBSolution::loadArcPar(const SBMJD& MeanDate)
//{
//  bool		IsOK(FALSE);
//  return IsOK;
//};

bool SBSolution::loadLocPar(const QString& BatchName)
{
  bool			IsOK(FALSE);
  Local.clear();
  if (checkPath(path2LocDir(), path2LocFile(BatchName)))
    {
      FILE		*p;
      QFile		File(path2LocFile(BatchName));
      QDataStream	Stream;

      if ((p=SetUp->fcList()->open4In(File.name(), File, Stream)))
	{
	  Stream >> Local;
	  IsOK = TRUE;
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been loaded");
	  
	  SetUp->fcList()->close(File, p, Stream);
	}
      else    
	Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": could not read the file [" + File.name() + "]");
    };
  return IsOK;
};

bool SBSolution::loadLocCor(const QString& BatchName, SBParameterList*& List, SBSymMatrix*& P)
{
  bool			IsOK(FALSE);
  if (checkPath(path2LocDir(), path2LocFileCorr(BatchName)))
    {
      FILE		*p;
      QFile		File(path2LocFileCorr(BatchName));
      QDataStream	Stream;

      if ((p=SetUp->fcList()->open4In(File.name(), File, Stream)))
	{
	  if (P)
	    delete P;
	  P = new SBSymMatrix;
	  
	  if (List)
	    List->clear();
	  else
	    {
	      List = new SBParameterList;
	      List ->setAutoDelete(TRUE);
	    };
	  
	  Stream >> *List >> *P;
	  IsOK = TRUE;
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been loaded");
	  SetUp->fcList()->close(File, p, Stream);
	}
      else    
	Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": could not read the file [" + File.name() + "]");
    };
  return IsOK;
};

bool SBSolution::loadStcPar(const QString& BatchName)
{
  bool			IsOK(FALSE);
  Stochastic.clear();
  if (checkPath(path2StcDir(), path2StcFile(BatchName)))
    {
      FILE		*p;
      QFile		File(path2StcFile(BatchName));
      QDataStream	Stream;

      if ((p=SetUp->fcList()->open4In(File.name(), File, Stream)))
	{
	  Stream >> Stochastic;
	  IsOK=TRUE;
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been loaded");
	  
	  SetUp->fcList()->close(File, p, Stream);
	}
      else
	Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": could not read the file [" + File.name() + "]");
    };
  return IsOK;
};

// saving statistics:
void SBSolution::saveStatistics()
{
  QFile f(StatisticsFile);
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << StatGeneral << StatByBatch;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		 ": the file [" + StatisticsFile + "] has been saved");
    }
  else
    Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
	       ": could not write the file [" + StatisticsFile + "]");
  return;
};

// saving parameters:
void SBSolution::saveGlbPar()
{
  FILE		*p;
  QFile		File(path2GlbFile());
  QDataStream	Stream;
  
  if ((p=SetUp->fcList()->open4Out(File.name(), File, Stream)))
    {
      Stream << Global;
      if (File.status()!=IO_Ok)
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": saving data to the the [" + File.name() + "] failed");
      else
	{
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been saved");
	  IsGlobalModified = FALSE;
	};
      
      SetUp->fcList()->close(File, p, Stream);
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
	       ": could not write the file [" + File.name() + "]");
  return;
};

void SBSolution::saveGlbCor(SBParameterList*List, const SBSymMatrix *P_GLB)
{
  FILE		*p;
  QFile		File(path2GlbFileCorr());
  QDataStream	Stream;

  if (!P_GLB)
    return;
  
  if ((p=SetUp->fcList()->open4Out(File.name(), File, Stream)))
    {
      Stream << *List << *P_GLB;
      if (File.status()!=IO_Ok)
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": saving data to the the [" + File.name() + "] failed");
      else
	Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": the file [" + File.name() + "] has been saved");
      SetUp->fcList()->close(File, p, Stream);
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
	       ": could not write the file [" + File.name() + "]");
  return;
};

//bool SBSolution::saveArcPar(const SBMJD&)
//{
//  bool		IsOK(FALSE);
//  return IsOK;
//};

void SBSolution::saveLocPar(const QString& BatchName)
{
  FILE		*p;
  QFile		File(path2LocFile(BatchName));
  QDataStream	Stream;
  
  if ((p=SetUp->fcList()->open4Out(File.name(), File, Stream)))
    {
      Stream << Local;
      if (File.status()!=IO_Ok)
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": saving data to the the [" + File.name() + "] failed");
      else
	{
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been saved");
	  IsLocalModified = FALSE;
	};
      
      SetUp->fcList()->close(File, p, Stream);
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
	       ": could not write the file [" + File.name() + "]");
  return;
};

void SBSolution::saveLocCor(const QString& BatchName, SBParameterList* List, const SBSymMatrix *P_LOC)
{
  FILE		*p;
  QFile		File(path2LocFileCorr(BatchName));
  QDataStream	Stream;
  
  if (!P_LOC)
    return;
  
  if ((p=SetUp->fcList()->open4Out(File.name(), File, Stream)))
    {
      Stream << *List << *P_LOC;
      if (File.status()!=IO_Ok)
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": saving data to the the [" + File.name() + "] failed");
      else
	Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": the file [" + File.name() + "] has been saved");
      SetUp->fcList()->close(File, p, Stream);
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
	       ": could not write the file [" + File.name() + "]");
  return;
};

void SBSolution::saveStcPar(const QString& BatchName)
{
  FILE		*p;
  QFile		File(path2StcFile(BatchName));
  QDataStream	Stream;
  
  if ((p=SetUp->fcList()->open4Out(File.name(), File, Stream)))
    {
      Stream << Stochastic;
      if (File.status()!=IO_Ok)
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": saving data to the the [" + File.name() + "] failed");
      else
	{
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been saved");
	  IsStochasticModified = FALSE;
	};
      
      SetUp->fcList()->close(File, p, Stream);
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
	       ": could not write the file [" + File.name() + "]");
  return;
};

void SBSolution::prepare4Batch(const QString& BatchName, bool IsRealRun)
{
  loadLocPar((CurrentBatch=BatchName));
  loadStcPar(CurrentBatch);
  
  Stat4CurrentBatch = new SBStatistics(CurrentBatch);
  if (StatByBatch.find(Stat4CurrentBatch) != -1)
    {
      delete Stat4CurrentBatch;
      Stat4CurrentBatch = StatByBatch.current();
      if (IsRealRun)
	Stat4CurrentBatch->clearStatistics();
    }
  else
    StatByBatch.prepend(Stat4CurrentBatch);
};

void SBSolution::submitGlobalParameters(SBParameterList* List, const SBSymMatrix* P)
{
  if (List && P && List->count())
    {
      unsigned int i;
      SBParameter* W;
      SBParameter* p;
      for (p=List->first(), i=0; p; p=List->next(), i++)
	{
	  if ((W=Global.find(p->name()))) // the parameter exist, update the info:
	    W->update(p);
	  else // new solution
	    {
	      W = new SBParameter(*p);
	      W ->setV(p->s());
	      Global.inSort(W);
	    };
	  p->setV(p->v() + p->s()); // for sinex
	};
      //      Global.sort();
      IsGlobalModified=IsModified=TRUE;
      saveGlbCor(List, P);
    };
};

void SBSolution::reportGlobalParameters()
{
  Global.report();
};

void SBSolution::submitLocalParameters(SBParameterList* List, const SBSymMatrix* P)
{
  if (List && P && List->count())
    {
      unsigned int i;
      SBParameter* W;
      SBParameter* p;
      for (p=List->first(), i=0; p; p=List->next(), i++)
	{
	  if ((W=Local.find(p->name()))) // the parameter exist, update the info:
	    W->update(p);
	  else // new solution
	    {
	      W = new SBParameter(*p);
	      W ->setV(p->s());
	      Local.inSort(W);
	    };
	};
      //      Local.sort();
      IsLocalModified=IsModified=TRUE;
    };
  if (IsLocalModified)
    {
      saveLocPar(CurrentBatch);
      saveLocCor(CurrentBatch, List, P);
    };
};

void SBSolution::reportLocalParameters()
{
  Local.report();
};

void SBSolution::submitStochasticParameters(SBParameterList* List, const SBSymMatrix* P)
{
  if (List && P && List->count())
    {
      unsigned int	i;
      SBStochParameter	*W;
      SBParameter	*p;
      for (p=List->first(), i=0; p; p=List->next(), i++)
	{
	  if ((W=Stochastic.find(p->name()))) // the parameter exist, update the info:
	    {
	      W->update(p);
	    }
	  else // new solution
	    {
	      W = new SBStochParameter(p->name(), p->scale());
	      W ->update(p);
	      Stochastic.inSort(W);
	    };
	};
      //      Stochastic.sort();
      IsStochasticModified=IsModified=TRUE;
    };
};

void SBSolution::reportStochasticParameters()
{
  Stochastic.report();
};

void SBSolution::updateParameter(SBParameter* P, const SBMJD& T, bool IsRealRun)
{
  if (P)
    {
      SBParameter	*w=NULL;
      SBStochParameter	*s=NULL;
      double		v=0.0, e=0.0;
      int		num=0;
      if ((w=Global.find(P->name())))
	{
	  v+=w->v(T);
	  e= w->e(T);
	  num = w->num();
	  if (!IsRealRun)
	    P->setTimes(*w);
	};
      if ((w=Local.find(P->name())))
	{
	  v+=w->v(T);
	  e= w->e(T);
	  num = w->num();
	  if (!IsRealRun)
	    P->setTimes(*w);
	};
      if ((s=Stochastic.find(P->name())))
	{
	  v+=s->v(T);
	  e= s->e(T);
	  num = s->num(T);
	};
      P->setV(v);
      P->setE(e);
      if (!IsRealRun)
	P->setNum(num);
    };
};

void SBSolution::getGlobalParameter4Report(SBParameter* P)
{
  if (P)
    {
      SBParameter	*w=NULL;
      if ((w=Global.find(P->name())))
	{
	  P->setV(w->v());
	  P->setE(w->e());
	  P->setTimes(*w);
	};
    };
};

void SBSolution::deleteSolution()
{
  QDir		d(SetUp->path2Sols(Prj->name()));
  QString	What;
  if (d.exists())
    {
      loadStatistics();
      for (unsigned int i=0; i<StatByBatch.count(); i++)
	{
	  What = path2LocFile(StatByBatch.at(i)->name());
	  if (d.exists(What) && !d.remove(What))
	    Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		       ": cannot delete the directory [" + What + "]");
	  What = path2LocFileCorr(StatByBatch.at(i)->name());
	  if (d.exists(What) && !d.remove(What))
	    Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		       ": cannot delete the directory [" + What + "]");
	  // Normal equations system:
	  What = path2LocFileNormEqs(StatByBatch.at(i)->name());
	  if (d.exists(What) && !d.remove(What))
	    Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		       ": cannot delete the directory [" + What + "]");
	  // stochasics pars:
	  What = path2StcFile(StatByBatch.at(i)->name());
	  if (d.exists(What) && !d.remove(What))
	    Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		       ": cannot delete the directory [" + What + "]");
	};

      What = path2LocDir();
      if (d.exists(What) && !d.rmdir(What))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the directory [" + What + "]");
      What = path2StcDir();
      if (d.exists(What) && !d.rmdir(What))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
 		   ": cannot delete the directory [" + What + "]");

      What = path2GlbFile();
      if (d.exists(What) && !d.remove(What))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the directory [" + What + "]");
      What = path2GlbFileCorr();
      if (d.exists(What) && !d.remove(What))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the directory [" + What + "]");
      What = path2GlbDir();
      if (d.exists(What) && !d.rmdir(What))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the directory [" + What + "]");

      StatGeneral.clearStatistics();
      StatByBatch.clear();
      if (d.exists(StatisticsFile) && !d.remove(StatisticsFile))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the directory [" + StatisticsFile + "]");

      What = SetUp->path2Sols(Prj->name());
      if (d.exists(What) && !d.rmdir(What))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the directory [" + What + "]");
    };
};

QString	SBSolution::guessParameterName(const QString& ParName)
{
  QString	GuessedName = ParName;
  QString	Str;

  if (ParName.contains("St: "))
    {
      SBStationID	ID;
      SBStationInfo	*SI = NULL;
      
      Str = ParName.mid(4, 9);
      if (ID.isValidStr(Str))
	{
	  ID.setID(Str);
	  Str=ParName.mid(15);
	  if ( (SI=Prj->stationList()->find(&ID)) )
	    {
	      if (Str.contains("Clock_0"))
		GuessedName = "Clocks variation at " + SI->aka() + " (" + ID.toString() + "), ps";
	      else if (Str.contains("Clock_1"))
		GuessedName = "Clock trend at " + SI->aka() + " (" + ID.toString() + "), ps/s";
	      else if (Str.contains("Clock_2"))
		GuessedName = "Clock_2 at " + SI->aka() + " (" + ID.toString() + "), ps/s/s";
	      else if (Str.contains("Zenith "))
		GuessedName = "Wet Zenith Delay at " + SI->aka() + " (" + ID.toString() + "), cm";
	      else if (Str.contains("Grad N "))
		GuessedName = "North Gradient at " + SI->aka() + " (" + ID.toString() + "), cm";
	      else if (Str.contains("Grad E "))
		GuessedName = "East Gradient at " + SI->aka() + " (" + ID.toString() + "), cm";
	      else if (Str.contains("AxisOff"))
		GuessedName = "Axiss Offset of " + SI->aka() + " (" + ID.toString() + "), cm";
	      else if (Str.contains("coord-X"))
		GuessedName = "Position of " + SI->aka() + " (" + ID.toString() + "), X-coordinates, cm";
	      else if (Str.contains("coord-Y"))
		GuessedName = "Position of " + SI->aka() + " (" + ID.toString() + "), Y-coordinates, cm";
	      else if (Str.contains("coord-Z"))
		GuessedName = "Position of " + SI->aka() + " (" + ID.toString() + "), Z-coordinates, cm";
	      else if (Str.contains("veloc-X"))
		GuessedName = "Velocity of " + SI->aka() + " (" + ID.toString() + "), X-vel., mm/yr";
	      else if (Str.contains("veloc-Y"))
		GuessedName = "Velocity of " + SI->aka() + " (" + ID.toString() + "), Y-vel., mm/yr";
	      else if (Str.contains("veloc-Z"))
		GuessedName = "Velocity of " + SI->aka() + " (" + ID.toString() + "), Z-vel., mm/yr";
	    };
	};
    }
  else if (ParName.contains("So: "))
    {
      Str = ParName.mid(14);
      if (Str.contains("RA"))
	GuessedName = "Right Ascension of " + ParName.mid(4, 8) + ", mas";
      else if (Str.contains("DN"))
	GuessedName = "Declination of " + ParName.mid(4, 8) + ", mas";
    }
  else if (ParName.contains("EOP: "))
    {
      Str = ParName.mid(5);
      if (Str.contains("X_p, "))
	GuessedName = "Polar Motion, X-coordinate, mas";
      if (Str.contains("Y_p, "))
	GuessedName = "Polar Motion, Y-coordinate, mas";
      if (Str.contains("UT, "))
	GuessedName = "Earth Rotation, d(UT1-UTC), ms";
      if (Str.contains("Psi, "))
	GuessedName = "Nutation in Longitude, mas";
      if (Str.contains("Eps, "))
	GuessedName = "Nutation in Obliquity, mas";
    };

  return GuessedName;
};

void SBSolution::dumpParameters()
{
  // it will be rewritten latter..
  //

  QString		Dir2Output = SetUp->path2Output() + Prj->name() + "/SolutionDump";
  QString		Dir4Global = Dir2Output + "/GlobalParameters";
  QString		Dir4Local  = Dir2Output + "/LocalParameters";
  QString		Dir4Stoch  = Dir2Output + "/StochasticParameters";
  QString		DirWork, Str;
  int			i;
  QFile			f;
  QTextStream		s;

  SBStochParameterList	LocalPars;
  LocalPars.clear();
  LocalPars.setAutoDelete(FALSE);

  checkPath(Dir2Output, Dir2Output);
  checkPath(Dir4Global, Dir4Global);
  checkPath(Dir4Local,  Dir4Local);
  checkPath(Dir4Stoch,  Dir4Stoch);

  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
	     ": dumping the solution of the project \"" + Prj->name() + "\"");
  
  
  SBStatistics		*Stat = NULL;
  SBStochParameter	*Ps   = NULL;

  // quick'n'dirty:
  QList<SBNList>	StochParNameList;
  QDict<SBNList>	StochParByName;
  StochParNameList.setAutoDelete(TRUE);
  StochParByName.setAutoDelete(FALSE);
  SBNList		*nl   = NULL;


  // stochastic parameters:
  bool		IsHasStochEOP = FALSE;
  bool		IsHasStochSta = FALSE;
  bool		IsHasStochSou = FALSE;
  bool		IsWritingPlt  = FALSE;
  SBMJD		TMax=TZero, TMin=TInf;

  if (StatByBatch.count())
    {
      for (Stat=StatByBatch.first(); Stat; Stat=StatByBatch.next())
	{
	  prepare4Batch(Stat->name());

	  // collect local parameters:
	  for (SBParameter *P=Local.first(); P; P=Local.next())
	    {
	      if ( !(Ps=LocalPars.find(P->name())) )
		LocalPars.inSort( (Ps = new SBStochParameter(P->name(), P->scale())) );
	      Ps->addPar(P);
	    };

	  // dump stochastic parameters:
	  DirWork = Dir4Stoch + "/" + Stat->name();
	  checkPath(DirWork, DirWork);
	  i=0;
	  for (Ps=Stochastic.first(); Ps; Ps=Stochastic.next())
	    {
	      if (Ps->count()) // at least one point
		{
		  // guess the name of parameter:
		  // stations:
		  if (Ps->name().contains("St: "))
		    {
		      Str = Ps->name().mid(4, 9);
		      if (SBStationID::isValidStr(Str))
			Str = Str + "_" + Ps->name().mid(15);
		      else
			Str = "Sochastic_" + QString().sprintf("%02d", i++);
		      IsHasStochSta = TRUE;
		    }
		  // sources:
		  else if (Ps->name().contains("So: "))
		    {
		      Str = Ps->name().mid(4, 8) + "_" + Ps->name().mid(14);
		      IsHasStochSou = TRUE;
		    }
		  // EOPs:
		  else if (Ps->name().contains("EOP: "))
		    {
		      Str = "EOP_" + Ps->name().mid(5);
		      IsHasStochEOP = TRUE;
		    }
		  else // unparsered name:
		    Str = "Sochastic_" + QString().sprintf("%02d", i++);
		  Str = Str.replace( QRegExp(" "), "_") + ".dat";
		  
		  if (Ps->first()->tStart()<TMin)
		    TMin = Ps->first()->tStart();
		  if (Ps->last()->tFinis()>TMax)
		    TMax = Ps->last()->tFinis();
		  
		  if ( !(nl=StochParByName.find(Ps->name())) )
		    {
		      nl = new SBNList;
		      nl->setAutoDelete(TRUE);
		      StochParNameList.append(nl);
		      StochParByName.insert(Ps->name(), nl);
		      nl->append(new SBNamed(guessParameterName(Ps->name())));
		    };
		  nl->append(new SBNamed(Stat->name() + "/" + Str));
		  Ps->dump2File(DirWork + "/" + Str);
		}
	      else
		Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
			   ": the parameter [" + Ps->name() + "] has got 0 points; nothing to dump");
	    };
	};
    };
  // create scripts for gnuplot:
  //
  DirWork = Dir2Output + "/stoch_pars.plt";
  f.setName(DirWork);
  double dt = TMax-TMin;
  if (dt>0.0)
    {
      TMax = TMax + 0.02*dt;
      TMin = TMin - 0.02*dt;
    };

  if (!checkPath(Dir2Output, DirWork) && f.open(IO_WriteOnly))
    {
      s.setDevice(&f);
      
      s << "# This file was automatically created by "<< Version.name() << "\n\n";
      s << "set term postscript landscape color\n#set term postscript eps color\n\n";
      s << "set data style points\n#set data style errorbars\nset pointsize 1.0\n";
      s << "set zeroaxis\n\n";
      s << "set output \"stochastic_pars.ps\"\n\n";
      s << "set xrange [" + TMin.toString(SBMJD::F_MJD) + ":" + TMax.toString(SBMJD::F_MJD) + "]\n";
      s << "set yrange [:]\n";
      s << "set xlabel \"Time, MJD\"\n#set xdata time\n#set timefmt \"%s\"\n\n";
      
      SBNamed *nm = NULL;
      for (nl=StochParNameList.first(); nl; nl=StochParNameList.next())
	{
	  int j=0;
      	  s << "\n\nset ylabel \"" << nl->at(0)->name() << "\"\n";
	  nl->remove((unsigned int)0);
	  nl->sort();
	  if ( (nm=nl->at(0)) )
	    {
	      s << "plot \\\n\t\"StochasticParameters/" << nm->name() << "\" notitle " 
		<< QString::number(++j);
	      for (nm = nl->next(); nm; nm=nl->next())
		{
		  j%=5;
		  s << ",\\\n\t\"StochasticParameters/" << nm->name() << "\" notitle "
		    << QString::number(++j);
		};
	    };
	  s << "\n\n";
	};
   
      s << "#EOF\n";
      s.unsetDevice();
      f.close();
    };

  // 
  StochParByName.clear();
  StochParNameList.clear();


  DirWork = Dir2Output + "/loc_pars.plt";  
  IsWritingPlt = FALSE;
  f.setName(DirWork);
  
  if (!checkPath(Dir2Output, DirWork))
    {
      if (f.open(IO_WriteOnly))
	{
	  s.setDevice(&f);
	  
	  s << "# This file was automatically created by "<< Version.name() << "\n\n";
	  s << "set term postscript landscape enhanced color solid \"Times-Roman\"\n";
	  s << "#set term postscript eps enhanced color solid \"Times-Roman\"\n\n";
	  s << "set data style points\nset data style errorbars\nset pointsize 1.0\n";
	  s << "set zeroaxis\nset bar small\n\n";
	  s << "set output \"local_pars.ps\"\n\n";
	  s << "set xdata time\nset timefmt \"%s\"\nset format x \"%b %d\\n%Y\"\n#set format y \"%.1f\"\n\n";
	  s << "set xlabel \"Time, UTC\"\n";
	  IsWritingPlt = TRUE;
	};
    };
  
  DirWork = Dir4Local + "/";
  // local parameters:
  i=0;
  for (Ps = LocalPars.first(); Ps; Ps = LocalPars.next())
    {
      // guess the name of parameter:
      // stations:
      if (Ps->name().contains("St: "))
	{
	  Str = Ps->name().mid(4, 9);
	  if (SBStationID::isValidStr(Str))
	    Str = Str + "_" + Ps->name().mid(15);
	  else
	    Str = "Local_" + QString().sprintf("%02d", i++);
	}
      // sources:
      else if (Ps->name().contains("So: "))
	{
	  Str = Ps->name().mid(4, 8) + "_" + Ps->name().mid(14);
	}
      // EOPs:
      else if (Ps->name().contains("EOP: "))
	{
	  Str = "EOP_" + Ps->name().mid(5);
	}
      else // unparsered name:
	Str = "Local_" + QString().sprintf("%02d", i++);
      
      Str = Str.replace( QRegExp(" "), "_") + ".dat";
      Ps->dump2File(DirWork + Str);
      if (IsWritingPlt)
	{
	  s << "\n\nset ylabel \"" << guessParameterName(Ps->name()) << "\"\n";
	  s << "#plot [][] \"LocalParameters/" << Str << "\" notitle 7\n";
	  s << "plot [][] \"LocalParameters/" << Str << "\" using 5:2:3 notitle lt -1 lw 0.2 pt 7\n\n";
	};
    };
  // create scripts for gnuplot:
  //
  if (IsWritingPlt)
    {
      s << "#EOF\n";
      s.unsetDevice();
      f.close();
    };
  LocalPars.clear();
  // 
  
  
  // global parameters:
  // does anybody need it?

  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
	     ": the solution of the project \"" + Prj->name() + "\" has been dumped");
};

void SBSolution::saveNormalEqs(SBParameterList* Ys, SBVector* Zy, SBUpperMatrix* Ry,
			       SBParameterList* Xs, SBVector* Zx, SBUpperMatrix* Rx, SBMatrix* Rxy, 
			       const QString& BatchName)
{
  FILE		*p;
  QFile		File(path2LocFileNormEqs(BatchName));
  QDataStream	Stream;

  if (!Ys || !Zy || !Ry || !Xs || !Zx || !Rx || !Rxy)
    {
      Log->write(SBLog::WRN, SBLog::ESTIMATOR, ClassName() + 
		 ": could not save normal equation system: nothing to save");
      return;
    };

  if ((p=SetUp->fcList()->open4Out(File.name(), File, Stream)))
    {
      Stream << *Ys << *Zy << *Ry << *Xs << *Zx << *Rx << *Rxy;
      if (File.status()!=IO_Ok)
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": saving data to the the [" + File.name() + "] failed");
      else
	Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": the file [" + File.name() + "] has been saved");
      SetUp->fcList()->close(File, p, Stream);
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
	       ": could not write the file [" + File.name() + "]");
  
};

void SBSolution::loadNormalEqs(SBParameterList*& Ys, SBVector*& Zy, SBUpperMatrix*& Ry,
			       SBParameterList*& Xs, SBVector*& Zx, SBUpperMatrix*& Rx, SBMatrix*& Rxy, 
			       const QString& BatchName)
{
  if (checkPath(path2LocDir(), path2LocFileNormEqs(BatchName)))
    {
      FILE		*p;
      QFile		File(path2LocFileNormEqs(BatchName));
      QDataStream	Stream;

      if ((p=SetUp->fcList()->open4In(File.name(), File, Stream)))
	{
	  // Y:
	  if (Ys)
	    Ys->clear();
	  else
	    {
	      Ys = new SBParameterList;
	      Ys ->setAutoDelete(TRUE);
	    };
	  if (Zy)
	    delete Zy;
	  Zy = new SBVector;
	  if (Ry)
	    delete Ry;
	  Ry = new SBUpperMatrix;

	  // X:
	  if (Xs)
	    Xs->clear();
	  else
	    {
	      Xs = new SBParameterList;
	      Xs ->setAutoDelete(TRUE);
	    };
	  if (Zx)
	    delete Zx;
	  Zx = new SBVector;
	  if (Rx)
	    delete Rx;
	  Rx = new SBUpperMatrix;
	  if (Rxy)
	    delete Rxy;
	  Rxy = new SBMatrix;
	  
	  Stream >> *Ys >> *Zy >> *Ry >> *Xs >> *Zx >> *Rx >> *Rxy;
	  Log->write(SBLog::DBG, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		     ": the file [" + File.name() + "] has been loaded");
	  SetUp->fcList()->close(File, p, Stream);
	}
      else    
	Log->write(SBLog::WRN, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": could not read the file [" + File.name() + "]");
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
