/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 2005 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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

#include "SbMathFilters.H"

#include <qfile.h>
#include <qmessagebox.h>





/*==============================================================================================*/
// aux function: returns VTPV
double makeHouseholder(SBUpperMatrix* R, SBVector* Z, SBMatrix* AObs, SBVector* ZObs, unsigned int NumOfData)
{
  unsigned int		Nx = R->n();
  unsigned int		i,j,l;
  double		s,ul,g;

  SBVector*		U = new SBVector(AObs->nRow(),"Aux Vector U");

  U->clear();                                                   // U-vector, + ul -- the first elementh of U
  for (l=0; l<Nx; l++)
    {
      s = R->at(l,l)*R->at(l,l);
      for (i=0; i<NumOfData; i++)
        s+=AObs->at(i,l)*AObs->at(i,l);
      if (s>0.0)
        {
          s = -sign(R->at(l,l))*sqrt(s);
          ul = R->at(l,l) - s;                                  // U_l
          for (i=0; i<NumOfData; i++) U->set(i, AObs->at(i,l)); // U_i (i=l+1, N)
          R->set(l,l, s);
          s = 1.0/(s*ul);                                       // betta
          for (j=l+1; j<Nx; j++)                                // |<-- processing "Ry/Ay"-block
            {                                                   // |
              g = ul*R->at(l,j);                                // |
              for (i=0; i<NumOfData; i++) g+=U->at(i)*AObs->at(i,j);    // |
              (*R)(l,j)+=s*g*ul;                                // |
              for (i=0; i<NumOfData; i++) (*AObs)(i,j)+=s*g*U->at(i);   // |
            };                                                  // |_________________________
          g = ul*Z->at(l);                                      // |--
          for (i=0; i<NumOfData; i++) g+=U->at(i)*ZObs->at(i);  // | dealing with the vectors Zy
          (*Z)(l)+=s*g*ul;                                      // |            and Z
          for (i=0; i<NumOfData; i++) (*ZObs)(i)+=s*g*U->at(i); // |--
        }
      else
        std::cerr << "got a zero during Householdering at l[" + QString().setNum(l) + "]\n";
    };
  
  s = 0.0;
  for (i=0; i<NumOfData; i++)
    s+= ZObs->at(i)*ZObs->at(i);
  return s;
};
/*==============================================================================================*/





/*==============================================================================================*/
/*												*/
/* SBFilterModel implementation									*/
/*												*/
/*==============================================================================================*/
SBFilterModel::SBFilterModel(unsigned int Idx_, unsigned int IdxSig_) : SBAttributed()
{
  T0		= 0.0;
  Idx		= Idx_;
  IdxSig	= IdxSig_;

  NumOfPolinoms = 0;
  NumOfHarmonics= 0;
  NumOfData	= 0;
  
  Periods	= NULL;
  Polinoms	= NULL;
  PolinomsSigma = NULL;
  Harmonics	= NULL;
  HarmonicsSigma= NULL;
  IsKeepTerm	= NULL;
  VTPV		= 0.0;
  SumW2		= 0.0;

  Series = NULL;
  IsOK   = FALSE;

  // spec.model:
  IsRunSM    = FALSE;
  NumOfSMTerms = 0;
  SMPolinoms = NULL;
  SMPolinomsSigma = NULL;
  SMPeriod = 1.0;
  IsKeepSM = FALSE;
  //
};

SBFilterModel::~SBFilterModel()
{
  if (Periods)
    {
      delete Periods;
      Periods = NULL;
    }
  if (Polinoms)
    {
      delete Polinoms;
      Polinoms = NULL;
    }
  if (PolinomsSigma)
    {
      delete PolinomsSigma;
      PolinomsSigma = NULL;
    }
  if (Harmonics)
    {
      delete Harmonics;
      Harmonics = NULL;
    }
  if (HarmonicsSigma)
    {
      delete HarmonicsSigma;
      HarmonicsSigma = NULL;
    }
  if (IsKeepTerm)
    {
      delete[] IsKeepTerm;
      IsKeepTerm = NULL;
    }
  // spec.model:
  if (SMPolinoms)
    {
      delete SMPolinoms;
      SMPolinoms = NULL;
    };
  if (SMPolinomsSigma)
    {
      delete SMPolinomsSigma;
      SMPolinomsSigma = NULL;
    };
  //
};

void SBFilterModel::setNumOfPolinoms(unsigned int n_)
{
  if (NumOfPolinoms != n_ || !Polinoms)
    {
      NumOfPolinoms = n_;
      if (Polinoms)
	delete Polinoms;
      Polinoms = new SBVector(NumOfPolinoms, "Estimated Polinoms");
      if (PolinomsSigma)
	delete PolinomsSigma;
      PolinomsSigma = new SBVector(NumOfPolinoms, "Estimated Std.Vars of Polinoms");
    };
  Polinoms->clear();
  PolinomsSigma->clear();
};

void SBFilterModel::setNumOfHarmonics(unsigned int n_) 
{
  if (NumOfHarmonics != n_ || !Periods)
    {
      NumOfHarmonics = n_;
      if (Periods)
	delete Periods;
      Periods = new SBVector(NumOfHarmonics, "Periods to estimate");
      if (Harmonics)
	delete Harmonics;
      Harmonics = new SBMatrix(NumOfHarmonics,2, "Estimated Harmonics (sin, cos)");
      if (HarmonicsSigma)
	delete HarmonicsSigma;
      HarmonicsSigma = new SBMatrix(NumOfHarmonics,2, "Estimated Std.Vars of Harmonics (sin, cos)");
      if (IsKeepTerm)
	delete[] IsKeepTerm;
      IsKeepTerm = new bool[NumOfHarmonics];
    };
  Periods->clear();
  Harmonics->clear();
  HarmonicsSigma->clear();
  for (unsigned int i=0; i<NumOfHarmonics; i++)
    *(IsKeepTerm+i) = FALSE; // remove the term
};

void SBFilterModel::setUpSpecModel(unsigned int n_, double P_, bool Is_)
{
  SMPeriod = P_;
  IsKeepSM = Is_;
  if (P_!=0.0 && n_>0)
    IsRunSM = TRUE;

  if (NumOfSMTerms != n_)
    {
      NumOfSMTerms = n_;
      if (SMPolinoms)
	delete SMPolinoms;
      SMPolinoms = new SBMatrix(NumOfSMTerms,2, "Estimated polinoms of SM (sin, cos)");
      if (SMPolinomsSigma)
	delete SMPolinomsSigma;
      SMPolinomsSigma = new SBMatrix(NumOfSMTerms,2, "Estimated Std.Vars of polinoms of SM (sin, cos)");
    };
  SMPolinoms->clear();
  SMPolinomsSigma->clear();
};


void SBFilterModel::setTermToKeep(unsigned int i, bool IsToKeep)
{
  if (i<NumOfHarmonics)
    *(IsKeepTerm+i) = IsToKeep;
  else
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": setTermToKeep: index (" + 
	       QString("").setNum(i) + ") out of ranges");
};


bool SBFilterModel::isCanRun()
{
  bool IsCanRun = TRUE;

  if (!NumOfPolinoms)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: NumOfPolinoms==0");
    };
  if (!Polinoms)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: Polinoms==NULL");
    };
  if (!PolinomsSigma)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: PolinomsSigma==NULL");
    };

  if (!NumOfHarmonics)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: NumOfHarmonics==0");
    };
  if (!Harmonics)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: Harmonics==NULL");
    };
  if (!HarmonicsSigma)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: HarmonicsSigma==NULL");
    };
  if (!Periods)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: Periods==NULL");
    };

  if (!DataAttribute)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: DataAttribute==NULL");
    };

  if (!Series)
    {
      IsCanRun = FALSE;
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot estimate model's parameters: Series==NULL");
    };

  return (IsOK=IsCanRun);
};

unsigned int SBFilterModel::eliminateBefore()
{
  SBDataSeriesEntry	*E=NULL;
  unsigned int		NumOfRmBefore = 0;

  // mark points if sig>3*mean_sigma:
  if (isAttr(IsOmit3SigmaBefore) && IdxSig && Series->count())
    {
      double		MeanSig = 0.0;
      unsigned int	i;

      for (E=Series->first(); E; E=Series->next())
	MeanSig += E->data()->at(IdxSig);
      MeanSig /= Series->count();

      i = 0;
      for (E=Series->first(); E; i++, E=Series->next())
	if (E->data()->at(IdxSig)>5.0*MeanSig)
	  {
	    DataAttribute->set(i, 0.0);
	    NumOfRmBefore++;
	  };
    };
  return NumOfRmBefore;
};

void SBFilterModel::analyseSeries()
{
  if (!IsOK)
    {
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot make analysis of the series: !IsOK");
      return;
    };

  // collect derivatives:
  SBVector		*ZObs = new SBVector(Series->count(), "Vector of Obs");
  SBMatrix		*AObs = new SBMatrix(Series->count(), numOfUnknowns(), "Matrix of Derivatives");
  SBDataSeriesEntry	*E=NULL;
  unsigned int		i,j;
  double		a, fSig, t;

  SumW2 = 0.0;
  i = j = 0;
  for (E=Series->first(); E; i++, E=Series->next())
    if (DataAttribute->at(i)>0.5)
      {
	fSig = IdxSig?E->data()->at(IdxSig):1.0;
	t    = E->t();
	a = 1.0;
	for (i=0; i<NumOfPolinoms; i++)
	  {
	    AObs->set(j, i, a/fSig);
	    a*=(t - T0)/365.25;
	  };
	for (i=0; i<NumOfHarmonics; i++)
	  {
	    AObs->set(j, NumOfPolinoms + 2*i,   cos(2.0*M_PI*(t-T0)/Periods->at(i))/fSig);
	    AObs->set(j, NumOfPolinoms + 2*i+1, sin(2.0*M_PI*(t-T0)/Periods->at(i))/fSig);
	  };
	if (IsRunSM)
	  {
	    a = 1.0;
	    for (i=0; i<NumOfSMTerms; i++)
	      {
		AObs->set(j, NumOfPolinoms + 2*NumOfHarmonics + 2*i,   a*cos(2.0*M_PI*(t-T0)/SMPeriod)/fSig);
		AObs->set(j, NumOfPolinoms + 2*NumOfHarmonics + 2*i+1, a*sin(2.0*M_PI*(t-T0)/SMPeriod)/fSig);
		a*=(t - T0)/365.25;
	      };
	  };
	
	ZObs->set(j, E->data()->at(Idx)/fSig);
	SumW2 += 1.0/fSig/fSig;
	j++;
      };
  NumOfData = j;
  
  // solve equations system:
  SBVector      *Z = new SBVector(numOfUnknowns(),  "Vector Z");
  SBUpperMatrix *R = new SBUpperMatrix(numOfUnknowns(),   "Matrix R");
  SBVector      *X = new SBVector(numOfUnknowns(), "Vector of Unknowns");
  SBSymMatrix   *P = new SBSymMatrix(numOfUnknowns(),   "Matrix P");

  for (i=0; i<numOfUnknowns(); i++)
    R->set(i,i, 1.0/1000000000.0);
 
  VTPV = makeHouseholder(R, Z, AObs, ZObs, NumOfData);
  Solve(*R, *X, *Z);
  RRT(*P, ~*R);
 
  // fill the vectors of parameters and std.vars:
  fSig = VTPV/(NumOfData - numOfUnknowns());
  for (i=0; i<NumOfPolinoms; i++)
    {
      Polinoms		-> set(i, X->at(i));
      PolinomsSigma	-> set(i, sqrt(fSig*P->at(i,i)));
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": data column #" + 
		 QString("").sprintf("(%d,%d): ", Idx, IdxSig) +
		 QString("").sprintf("Polinom order: %d; Value: %11.8f (%10.8f)", 
				     i, Polinoms->at(i), PolinomsSigma->at(i)));
    };
  for (i=0; i<NumOfHarmonics; i++)
    {
      Harmonics		-> set(i,0, X->at(NumOfPolinoms + 2*i));
      HarmonicsSigma	-> set(i,0, sqrt(fSig*P->at(NumOfPolinoms + 2*i, NumOfPolinoms + 2*i)));
      Harmonics		-> set(i,1, X->at(NumOfPolinoms + 2*i+1));
      HarmonicsSigma	-> set(i,1, sqrt(fSig*P->at(NumOfPolinoms + 2*i+1, NumOfPolinoms + 2*i+1)));
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": data column #" + 
		 QString("").sprintf("(%d,%d): ", Idx, IdxSig) +
		 //		 QString("").sprintf("Harmonics# %d (P=%10.4f); Cos: %11.8f (%10.8f), Sin: %11.8f (%10.8f)", 
		 QString("").sprintf("Harmonics# %d (P=%10.4f); Cos: %8.4f (%7.4f), Sin: %8.4f (%7.4f)", 
				     i, Periods->at(i),
				     Harmonics->at(i,0)*1000.0, HarmonicsSigma->at(i,0)*1000.0, 
				     Harmonics->at(i,1)*1000.0, HarmonicsSigma->at(i,1)*1000.0));
    };
  if (IsRunSM)
    for (i=0; i<NumOfSMTerms; i++)
      {
	SMPolinoms	-> set(i,0, X->at(NumOfPolinoms + 2*NumOfHarmonics + 2*i));
	SMPolinomsSigma	-> set(i,0, sqrt(fSig*P->at(NumOfPolinoms + 2*NumOfHarmonics + 2*i, 
						    NumOfPolinoms + 2*NumOfHarmonics + 2*i)));
	SMPolinoms	-> set(i,1, X->at(NumOfPolinoms + 2*NumOfHarmonics + 2*i+1));
	SMPolinomsSigma	-> set(i,1, sqrt(fSig*P->at(NumOfPolinoms + 2*NumOfHarmonics + 2*i+1, 
						    NumOfPolinoms + 2*NumOfHarmonics + 2*i+1)));
	Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": data column #" + 
		   QString("").sprintf("(%d,%d): ", Idx, IdxSig) +
		   //		   QString("").sprintf("SMTerm   # %d (P=%10.4f); Cos: %11.8f (%10.8f), Sin: %11.8f (%10.8f)", 
		   QString("").sprintf("SMTerm   # %d (P=%10.4f); Cos: %8.4g (%7.4g), Sin: %8.4g (%7.4g)", 
				       i, SMPeriod,
				       SMPolinoms->at(i,0)*1000.0, SMPolinomsSigma->at(i,0)*1000.0, 
				       SMPolinoms->at(i,1)*1000.0, SMPolinomsSigma->at(i,1)*1000.0));
      };
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": data column #" + 
	     QString("").sprintf("(%d,%d): ", Idx, IdxSig) +
	     QString("").sprintf("VarFactor: %.8f", sqrt(VTPV/SumW2)));

  // delete aux stuff:
  delete AObs;
  AObs = NULL;
  delete ZObs;
  ZObs = NULL;
  delete Z;
  Z = NULL;
  delete R;
  R = NULL;
  delete P;
  P = NULL;
  delete X;
  X = NULL;
};

double SBFilterModel::modelFull(double t)
{
  unsigned int	i;
  double	f = 0.0;
  double	a;
  
  a = 1.0;
  for (i=0; i<NumOfPolinoms; i++)
    {
      f += Polinoms->at(i)*a;
      a*=(t - T0)/365.25;
    };
  for (i=0; i<NumOfHarmonics; i++)
    {
      f += 
	Harmonics->at(i, 0)*cos(2.0*M_PI*(t-T0)/Periods->at(i)) +
	Harmonics->at(i, 1)*sin(2.0*M_PI*(t-T0)/Periods->at(i)) ;
    };
  if (IsRunSM)
    {
      a = 1.0;
      for (i=0; i<NumOfSMTerms; i++)
	{
	  f +=
	    SMPolinoms->at(i,0)*a*cos(2.0*M_PI*(t-T0)/SMPeriod) +
	    SMPolinoms->at(i,1)*a*sin(2.0*M_PI*(t-T0)/SMPeriod) ;
	  a*=(t - T0)/365.25;
	};
    };
  return f;
};

double SBFilterModel::modelKeep(double t)
{
  unsigned int	i;
  double	f = 0.0, a;
  
  /* do not add the trend:
     double	a;
     a = 1.0;
     for (i=0; i<NumOfPolinoms; i++)
     {
     f += Polinoms->at(i)*a;
     a*=(t - T0)/365.25;
     };
  */
  for (i=0; i<NumOfHarmonics; i++)
    if (*(IsKeepTerm+i))
      {
	f += 
	  Harmonics->at(i, 0)*cos(2.0*M_PI*(t-T0)/Periods->at(i)) +
	  Harmonics->at(i, 1)*sin(2.0*M_PI*(t-T0)/Periods->at(i)) ;
      };
  if (IsRunSM && IsKeepSM)
    {
      a = 1.0;
      for (i=0; i<NumOfSMTerms; i++)
	{
	  f +=
	    SMPolinoms->at(i,0)*a*cos(2.0*M_PI*(t-T0)/SMPeriod) +
	    SMPolinoms->at(i,1)*a*sin(2.0*M_PI*(t-T0)/SMPeriod) ;
	  a*=(t - T0)/365.25;
	};
    };
  return f;
};

unsigned int SBFilterModel::eliminateAfter()
{
  SBDataSeriesEntry	*E=NULL;
  unsigned int		NumOfRmAfter = 0;

  // mark points if (data-model)>3*vTPv:
  if (isAttr(IsOmit3SigmaAfter) && Series->count())
    {
      double		Sig = sqrt(VTPV/SumW2);
      unsigned int	i;

      i = 0;
      for (E=Series->first(); E; i++, E=Series->next())
	if (fabs(E->data()->at(Idx) - modelFull(E->t()))>5.0*Sig)
	  {
	    DataAttribute->set(i, 0.0);
	    NumOfRmAfter++;
	  };
    };
  return NumOfRmAfter;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBFilterGauss implementation									*/
/*												*/
/*==============================================================================================*/
SBFilterGauss::SBFilterGauss() : SBAttributed()
{
  Series = new SBDataSeries;
  SeriesMinusModel = new SBDataSeries;
  FWHM = 1.0;
  IsOK = FALSE;
  TStart = TFinis = 0.0;
  N = 0;
  NColOfData = 0;
  StdVar	= 0.0;

  ModelList.setAutoDelete(TRUE);
  ModelByIdx.setAutoDelete(FALSE);
  
  vAux = NULL;
  mAux = NULL;
  Sigmas = NULL;
  DataAttribute = NULL;
};

SBFilterGauss::~SBFilterGauss()
{
  if (Series)
    {
      delete Series;
      Series = NULL;
    };
  if (SeriesMinusModel)
    {
      delete SeriesMinusModel;
      SeriesMinusModel = NULL;
    };
  if (vAux)
    {
      delete vAux;
      vAux = NULL;
    };
  if (mAux)
    {
      delete mAux;
      mAux = NULL;
    };
  if (DataAttribute)
    {
      delete DataAttribute;
      DataAttribute = NULL;
    };
};

void SBFilterGauss::readDataFile(const QString& FileName)
{
  QFile f(FileName);
  f.open(IO_ReadOnly);
  QTextStream	s(&f);
  
  s >> *Series;

  f.close();
  s.unsetDevice();

  Log->write(SBLog::DBG, SBLog::DATA, 
	     ClassName() + ": the file [" + FileName + "] has been loaded");

  IsOK    = FALSE;
  
  // clear models:
  if (ModelList.count())
    rmModels();

  // checking the input:
  if (Series->isChecked() && Series->count()>2)
    {
      if ((N=Series->count())>2)
	{
	  if ((NColOfData=Series->numOfDataColumns()))
	    {
	      if (Series->dT()!=0.0)
		{
		  TStart = Series->first()->t();
		  TFinis = Series->last() ->t();

		  if (vAux && vAux->n()!=N)
		    {
		      delete vAux;
		      vAux = NULL;
		    };
		  if (!vAux)
		    vAux = new SBVector(N, "Aux Vector");

		  if (mAux && (mAux->nCol()!=NColOfData || mAux->nRow()!=N))
		    {
		      delete mAux;
		      mAux = NULL;
		    };
		  if (!mAux)
		    mAux = new SBMatrix(N, NColOfData, "Aux Matrix");

		  if (Sigmas)
		    {
		      delete Sigmas;
		      Sigmas = NULL;
		    };
		  Sigmas = new SBVector(NColOfData, "Sigmas");

		  if (DataAttribute)
		    delete DataAttribute;
		  DataAttribute = new SBVector(N, "Attributes");
		  for (unsigned int i=0; i<N; i++)
		    DataAttribute->set(i, 1.0);
		  
		  IsOK = TRUE;
		}
	      else
		Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": the file [" + FileName + 
			   "] contains data on the zero interval (dT==0.0)");
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": the file [" + FileName + 
		       "] does not contain any data column");
	}
      else
	Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": the file [" + FileName + 
		   "] does not contain enough points for smoothing (N<3)");
    }
  else
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the file [" + FileName + 
	       "] was not parsed properly");
  
  if (!IsOK)
    {
      Series->clear();
      TStart = TFinis = 0.0;
      N = 0;
      NColOfData = 0;
      if (vAux)
	{
	  delete vAux;
	  vAux = NULL;
	};
      if (mAux)
	{
	  delete mAux;
	  mAux = NULL;
	};
      if (Sigmas)
	{
	  delete Sigmas;
	  Sigmas = NULL;
	};
    };
};

double SBFilterGauss::interpolate(double t, unsigned int Idx, unsigned int SigIdx)
{
  double ret=0.0;
  double sig=0.0;

  // checking ranges:
  if (t<TStart - 2.0*Series->dT())
    return ret;
  if (TFinis + 2.0*Series->dT()<t)
    return ret;
  if (NColOfData<=Idx)
    return ret;
  if (NColOfData<=SigIdx)
    return ret;
  
  double		Wsum = 0.0;
  double		f, p;
  SBDataSeriesEntry	*E=NULL;
  unsigned int		i;
  double		a = FWHM/2.35482;

  if (isAttr(IsOmit3Sigma))
    {
      i = 0;
      for (E=Series->first(); E; i++, E=Series->next())
	if (3.0*Sigmas->at(Idx)>=fabs(mAux->at(i, Idx)))
	  {
	    p = SigIdx?E->data()->at(SigIdx):1.0;
	    if (p>0.0)
	      p = 1.0/p/p;
	    f = (t-E->t())/a;
	    f = p*exp(-f*f/2.0);
	    vAux->set(i, f);
	    Wsum += f;
	  };
      i = 0;
      for (E=Series->first(); E; i++, E=Series->next())
	if (3.0*Sigmas->at(Idx)>=fabs(mAux->at(i, Idx)))
	  {
	    ret += E->data()->at(Idx)*vAux->at(i);
	    //	    f = (SigIdx?E->data()->at(SigIdx):1.0)*vAux->at(i);
	    //	    sig += f*f;
	    f = SigIdx?E->data()->at(SigIdx):1.0;
	    sig += f*f*vAux->at(i);
	  };
    }
  else
    {
      i = 0;
      for (E=Series->first(); E; i++, E=Series->next())
	{
	  p = SigIdx?E->data()->at(SigIdx):1.0;
	  if (p>0.0)
	    p = 1.0/p/p;
	  f = (t-E->t())/a;
	  f = p*exp(-f*f/2.0);
	  vAux->set(i, f);
	  Wsum += f;
	};
      i = 0;
      for (E=Series->first(); E; i++, E=Series->next())
	{
	  ret += E->data()->at(Idx)*vAux->at(i);
	  //	  f = (SigIdx?E->data()->at(SigIdx):1.0)*vAux->at(i);
	  //	  sig += f*f;
	  f = SigIdx?E->data()->at(SigIdx):1.0;
	  sig += f*f*vAux->at(i);
	};
    };

  StdVar = sqrt(sig/Wsum);
  return ret/Wsum;
};

double SBFilterGauss::respFunc(double f)
{
  double a = FWHM/2.35482;
  double q = M_PI*f*a;
  return exp(-2.0*q*q);
};

void SBFilterGauss::calcSigma(unsigned int Idx, unsigned int SigIdx)
{
  double		f;
  SBDataSeriesEntry	*E=NULL;
  double		Sigma = 0.0;

  if (!Series->count())
    return;

  unsigned int	l;
  QListIterator<SBDataSeriesEntry> it(*Series);
  for (l=0; it.current(); l++, ++it)
    {
      E = it.current();
      f = interpolate(E->t(), Idx, SigIdx) - E->data()->at(Idx);
      mAux->set(l, Idx, f);
      Sigma += f*f;
    };
  Sigmas->set(Idx, sqrt(Sigma/Series->count()));
};

void SBFilterGauss::addModel(unsigned int Idx_, unsigned int IdxSig_)
{
  ModelList.append(new SBFilterModel(Idx_, IdxSig_));
};

void SBFilterGauss::prepareModels()
{
  SBDataSeriesEntry	*E=NULL;
  SBFilterModel		*model = NULL;
  unsigned int		num;
  unsigned int		i;
  double		f;

  SeriesMinusModel->clear();
  for (E=Series->first(); E; E=Series->next())
    SeriesMinusModel->append(new SBDataSeriesEntry(*E));
  
  for (i=0; i<N; i++)
    DataAttribute->set(i, 1.0);

  ModelByIdx.clear();
  ModelByIdx.resize(ModelList.count()+5);
  for (model=ModelList.first(); model; model=ModelList.next())
    {
      model->setT0(51544.5);
      model->setSeries(SeriesMinusModel);
      model->setDataAttribute(DataAttribute);
      //      model->setNumOfPolinoms(2);

      /* for AAM:*/
      if (!TRUE)
	{
	  model->setNumOfPolinoms(2);
	  model->setNumOfHarmonics(3);
	  model->periods()->set( 0,   365.25);
	  model->periods()->set( 1,   365.25/2.0);
	  model->periods()->set( 2,   365.25/3.0);
	}
      /**/
      /* This was used for Nutation:*/
      else
	{
	  model->setNumOfPolinoms(2);
	  if (!TRUE)
	    {
	      /**/
	      model->setNumOfHarmonics(5);
	      model->periods()->set( 0,   430.21);
	      model->periods()->set( 1,  6798.38);
	      model->periods()->set( 2,  3399.19);
	      model->periods()->set( 3,   365.26);
	      model->periods()->set( 4,   182.62);
	      model->setTermToKeep ( 0,   TRUE);
	      /**/
	      /*
		model->setNumOfHarmonics(22);
		model->periods()->set( 0,   430.21);
		model->periods()->set( 1,  6798.38);
		model->periods()->set( 2,  3399.19);
		model->periods()->set( 3,  1615.75);
		model->periods()->set( 4,  1305.48);
		model->periods()->set( 5,  1095.18);
		model->periods()->set( 6,   386.00);
		model->periods()->set( 7,   365.26);
		model->periods()->set( 8,   346.64);
		model->periods()->set( 9,   182.62);
		model->periods()->set(10,   121.75);
		model->periods()->set(11,    31.81);
		model->periods()->set(12,    27.55);
		model->periods()->set(13,    23.94);
		model->periods()->set(14,    14.77);
		model->periods()->set(15,    13.78);
		model->periods()->set(16,    13.66);
		model->periods()->set(17,     9.56);
		model->periods()->set(18,     9.13);
		model->periods()->set(19,     9.12);
		model->periods()->set(20,     7.10);
		model->periods()->set(21,     6.86);
	      */

	      
	      /*
		model->setNumOfHarmonics(5);
		model->periods()->set( 0,  6798.38);
		model->periods()->set( 1,  3399.19);
		model->periods()->set( 2,   365.26);
		model->periods()->set( 3,   182.62);
		model->periods()->set( 4,   430.21);
		model->setTermToKeep ( 4,   TRUE);
	      */
	    }
	  else
	    {
	      model->setNumOfHarmonics(25);
	      model->periods()->set( 0,   430.21);
	      /*	      
		6.8593928882     | 2  0  2  0  2|     -3.1046
		7.0957924441     | 0  0  2  2  2|     -3.8571
		
		9.1206803589     | 1  0  2  0  1|     -5.1613
		9.1329330603     | 1  0  2  0  2|    -30.1461
		9.5434387131     |-1  0  2  2  1|     -1.0204
		9.5568544398     |-1  0  2  2  2|     -5.9641
		
		13.6061104103    | 0  0  2  0  0|      2.5887
		13.6333959394    | 0  0  2  0  1|    -38.7298
		13.6607911246    | 0  0  2  0  2|   -227.6413
		13.7772749412    | 2  0  0  0  0|      2.9243
		
		14.7652944289    | 0  0  0  2  0|      6.3384
		23.9420798510    | 1  0  2 -2  2|      2.8593
		
		26.9849819935    |-1  0  2  0  1|      2.0441
		27.0925209685    |-1  0  2  0  2|     12.3457
		27.4433192769    |-1  0  0  0  1|     -5.7976
		27.5545498824    | 1  0  0  0  0|     71.1159
		27.6666858154    | 1  0  0  0  1|      6.3110
		
		31.6637756111    | 1  0  0 -2  1|     -1.2873
		31.8119411522    |-1  0  0  2  0|     15.6994
		31.9614998460    |-1  0  0  2  1|      1.5164
		
		91.3127282312    | 0  2  2 -2  2|     -1.5794
		121.7493351260   | 0  1  2 -2  2|    -51.6821
		173.3100379406   | 0  0 -2  2  0|      2.1783
		177.8437792705   | 0  0  2 -2  1|     12.8227
		
		182.6210952119   | 0  0  2 -2  2|  -1317.0906
		182.6298179210   | 0  2  0  0  0|      1.6707
		
		205.8922151547   |-2  0  0  2  0|     -4.7722
		346.6357876787   | 0 -1  0  0  1|     -1.2654
		
		365.2247466721   | 0 -1  2 -2  2|     21.5829
		365.2596358420   | 0  1  0  0  0|    147.5877
		
		385.9983321473   | 0  1  0  0  1|     -1.4053
		1095.1750518577  |-2  0  2  0  0|     -1.1024
		1305.4792009292  |-2  0  2  0  1|      4.5893
		3399.1917370372  | 0  0  0  0  2|    207.4554 
		6798.3834740744  | 0  0  0  0  1| -17206.4161
	      */
	      model->periods()->set( 1,  6798.3834740744);
	      model->periods()->set( 2,  3399.1917370372);
	      model->periods()->set( 3,  1305.4792009292);
	      model->periods()->set( 4,  1095.1750518577);
	      model->periods()->set( 5,   385.9983321473);
	      model->periods()->set( 6,   365.2596358420);
	      model->periods()->set( 7,   346.6357876787);
	      model->periods()->set( 8,   205.8922151547);
	      model->periods()->set( 9,   182.6210952119);
	      model->periods()->set(10,   177.8437792705);
	      model->periods()->set(11,   173.3100379406);
	      model->periods()->set(12,   121.7493351260);
	      model->periods()->set(13,    91.3127282312);
	      model->periods()->set(14,    31.8119411522);
	      model->periods()->set(15,    27.5545498824);
	      model->periods()->set(16,    27.0925209685);
	      model->periods()->set(17,    23.9420798510);
	      model->periods()->set(18,    14.7652944289);
	      model->periods()->set(19,    13.6607911246);
	      model->periods()->set(20,    13.6333959394);
	      model->periods()->set(21,     9.5568544398);
	      model->periods()->set(22,     9.1329330603);
	      model->periods()->set(23,     7.0957924441);
	      model->periods()->set(24,     6.8593928882);

              model->setTermToKeep ( 0,      TRUE);
	    };
	  
	};
      /**/
      /*
	model->setNumOfHarmonics(11);
	model->periods()->set( 0,  6798.38);
	model->periods()->set( 1,  3399.19);
	model->periods()->set( 2,  1615.75);
	model->periods()->set( 3,  1305.48);
	model->periods()->set( 4,  1095.18);
	model->periods()->set( 5,   430.21);//
	model->periods()->set( 6,   386.00);
	model->periods()->set( 7,   365.26);
	model->periods()->set( 8,   182.62);
	model->periods()->set( 9,    27.55);
	model->periods()->set(10,    13.66);
	model->setTermToKeep (5, TRUE);
      */

      model->addAttr(SBFilterModel::IsOmit3SigmaBefore);
//      model->addAttr(SBFilterModel::IsOmit3SigmaAfter);
      ModelByIdx.insert(model->idx(), model);
    };

  for (model=ModelList.first(); model; model=ModelList.next())
    if (!model->isCanRun())
      {
	Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": cannot run model for column #" +
		   QString("").setNum(model->idx()));
	return;
      };
  num = 0;
  for (model=ModelList.first(); model; model=ModelList.next())
    {
      num = model->eliminateBefore();
      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": `BeforeRun' elimination for the column #" +
		 QString("").setNum(model->idx()) + ": " + QString("").setNum(num) + " eliminated");
    };
  for (model=ModelList.first(); model; model=ModelList.next())
    model->analyseSeries();
  for (model=ModelList.first(); model; model=ModelList.next())
    {
      num = model->eliminateAfter();
      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": `AfterRun' elimination for the column #" +
		 QString("").setNum(model->idx()) + ": " + QString("").setNum(num) + " eliminated");
    };

  // remove full model from data series:
  for (model=ModelList.first(); model; model=ModelList.next())
    {
      i = 0;
      for (E=SeriesMinusModel->first(); E; i++, E=SeriesMinusModel->next())
	if (DataAttribute->at(i)>0.5)
	  {
	    f = E->data()->at(model->idx()) - model->modelFull(E->t());
	    E->data()->set(model->idx(), f);
	  };
    };
  
};

double SBFilterGauss::interpolateMinusModel(double t, unsigned int Idx, unsigned int SigIdx)
{
  double ret=0.0;
  double sig=0.0;

  // checking ranges:
  if (t<TStart - 2.0*SeriesMinusModel->dT())
    return ret;
  if (TFinis + 2.0*SeriesMinusModel->dT()<t)
    return ret;
  if (NColOfData<=Idx)
    return ret;
  if (NColOfData<=SigIdx)
    return ret;
  if (!ModelList.count())
    return ret;

  
  double		Wsum = 0.0;
  double		f, p;
  SBDataSeriesEntry	*E=NULL;
  unsigned int		i;
  double		a = FWHM/2.35482;

  i = 0;
  for (E=SeriesMinusModel->first(); E; i++, E=SeriesMinusModel->next())
    if (DataAttribute->at(i)>0.5)
      {
	p = SigIdx?E->data()->at(SigIdx):1.0;
	if (p>0.0)
	  p = 1.0/p/p;
	f = (t-E->t())/a;
	f = p*exp(-f*f/2.0);
	vAux->set(i, f);
	Wsum += f;
      };

  i = 0;
  for (E=SeriesMinusModel->first(); E; i++, E=SeriesMinusModel->next())
    if (DataAttribute->at(i)>0.5)
      {
	ret += E->data()->at(Idx)*vAux->at(i);
	f = (SigIdx?E->data()->at(SigIdx):1.0)*vAux->at(i);
	sig += f*f;
      };

  StdVar = sqrt(sig)/Wsum;
  
  ret /= Wsum;
  // add keeped terms to data series:
  ret += ModelByIdx.find(Idx)->modelKeep(t);
  return ret;
};

void SBFilterGauss::substractFilter(bool IsUseModel, int SMode)
{
  SBDataSeriesEntry	*E=NULL;
  SBDataSeries		*SeriesMinusFilter = new SBDataSeries;
  double		f=0.0;
  unsigned int		NumOfRealDataCol = NColOfData;

  if (SMode!=0)
    NumOfRealDataCol = NColOfData/2;

  for (E=Series->first(); E; E=Series->next())
    SeriesMinusFilter->append(new SBDataSeriesEntry(*E));
  SeriesMinusFilter->sort();

  // substract filter data:
  SBDataSeriesEntry	*Em=NULL;
  for (Em = SeriesMinusFilter->first(); Em; Em = SeriesMinusFilter->next())
    {
      for (unsigned int j=0; j<NumOfRealDataCol; j++)
	{
	  f = 0.0;
	  switch (SMode)
	    {
	    case 0:
	      f = IsUseModel?interpolateMinusModel(Em->t(),j,0):interpolate(Em->t(),j,0);
		(*Em->data())(j) -= f;
		break;
	    case 1:
	      f = IsUseModel?interpolateMinusModel(Em->t(), j, NumOfRealDataCol+j)
		:interpolate(Em->t(), j, NumOfRealDataCol+j);
		(*Em->data())(j) -= f;
		break;
	    case 2:
	      f = IsUseModel?interpolateMinusModel(Em->t(), 2*j, 2*j+1):interpolate(Em->t(), 2*j, 2*j+1);
		(*Em->data())(2*j) -= f;
		break;
	    };
	};
    };
  

  // make a copy:
  unsigned int		i;
  Em = SeriesMinusFilter->first();
  E  = Series->first();
  for (i=0; E && Em; i++)
    {
      if (E->t()==Em->t())
	*E->data() = *Em->data();
      else
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the arguments mismatch: " + 
		   QString().sprintf("idx=%d, OrigT=%.8f SubstrT=%.8f", 
				     i, E->t(), Em->t()));
      Em = SeriesMinusFilter->next();
      E  = Series->next();
    };

  delete SeriesMinusFilter;
};
/*==============================================================================================*/


/*==============================================================================================*/
