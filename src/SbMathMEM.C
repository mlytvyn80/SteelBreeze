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

#include "SbMathMEM.H"

#include <qfile.h>
#include <qmessagebox.h>


const std::complex<double> SBMEM::CZero = std::complex<double>(0.0, 0.0);
const std::complex<double> SBMEM::CI    = std::complex<double>(0.0, 1.0);


/*==============================================================================================*/
/*												*/
/* SBMEM implementation										*/
/*												*/
/*==============================================================================================*/
SBMEM::SBMEM() : SBAttributed()
{
  N = 0;
  M = 0;
  IsOK    = FALSE;
  IsReady = FALSE;
  IsMSet  = FALSE;
  X	= NULL;
  Z_r	= NULL;
  Z_i	= NULL;
  Am	= NULL;
  Am_1	= NULL;
  EFm	= NULL;
  EFm_1	= NULL;
  EBm	= NULL;
  EBm_1 = NULL;
  DT	= 0.0;
  Pm	= 0.0;
  Pm_1	= 0.0;
  Type	= Type_Recursion;
  MAuxCreated = M;
  ShiftR = 0.0;
  ShiftI = 0.0;
  TrendR = 0.0;
  TrendI = 0.0;
  SampleVar = 0.0;
};

SBMEM::~SBMEM()
{
  if (X)
    {
      delete X;
      X = NULL;
    };
  if (Z_r)
    {
      delete Z_r;
      Z_r = NULL;
    };
  if (Z_i)
    {
      delete Z_i;
      Z_i = NULL;
    };

  // aux:
  if (Am)
    {
      delete[] Am;
      Am = NULL;
    };
  if (EFm)
    {
      delete EFm;
      EFm = NULL;
    };
  if (EBm)
    {
      delete EBm;
      EBm = NULL;
    };
  if (Am_1)
    {
      delete[] Am_1;
      Am_1 = NULL;
    };
  if (EFm_1)
    {
      delete EFm_1;
      EFm_1 = NULL;
    };
  if (EBm_1)
    {
      delete EBm_1;
      EBm_1 = NULL;
    };
};

void SBMEM::readDataFile(const QString& FileName)
{
  SBDataSeries	Series;
 
  QFile f(FileName);
  f.open(IO_ReadOnly);
  QTextStream	s(&f);
  
  s >> Series;

  f.close();
  s.unsetDevice();

  Log->write(SBLog::DBG, SBLog::DATA, 
	     ClassName() + ": the file [" + FileName + "] has been loaded");

  IsOK    = FALSE;
  IsMSet  = FALSE;
  M = MAuxCreated = 0;
  ShiftR = 0.0;
  ShiftI = 0.0;
  TrendR = 0.0;
  TrendI = 0.0;

  // checking the input:
  if (Series.isChecked())
    {
      if (Series.isEquiDistant() || isAttr(ignoreNotEquidistant))
	{
	  if (Series.numOfDataColumns()==2 || (Series.numOfDataColumns()==1 && isAttr(fillMissingImag)))
	    {
	      if ((N=Series.count())>5) // ok, it's real data..
		{
		  DT = Series.dT();

		  if (X) delete X;
		  if (Z_r) delete Z_r;
		  if (Z_i) delete Z_i;

		  X   = new SBVector(N, "Argument");
		  Z_r = new SBVector(N, "Real part of data");
		  Z_i = new SBVector(N, "Imaginary part of data");
		  
		  QListIterator<SBDataSeriesEntry>	it(Series);
		  SBDataSeriesEntry			*E=NULL;
		  int					idx=0;

		  for(; (E=it.current()); ++it, idx++)
		    {
		      X  ->set(idx, E->t());
		      Z_r->set(idx, E->data()->at(0));
		      Z_i->set(idx, Series.numOfDataColumns()==2?E->data()->at(1):0.0);
		      //Z_i->set(idx, Series.numOfDataColumns()==2?E->data()->at(1):E->data()->at(0));
		    };
		  if (isAttr(removeShifts) && !isAttr(removeTrends))
		    removeShiftsFromData();
		  else if (isAttr(removeTrends))
		    removeTrendsFromData();
		  // calc sample variance:
		  SampleVar = 0.0;
		  if (isAttr(removeShifts) || isAttr(removeTrends))
		    {
		      for (unsigned int i=0; i<N; i++)
			SampleVar += Z_r->at(i)*Z_r->at(i) + Z_i->at(i)*Z_i->at(i);
		      SampleVar /= N - 1;
		    };
		  
		  IsOK=TRUE;
		}
	      else
		Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the file [" + FileName + 
			   "] has the data which are not cold be analysed (Num.of data rows "
			   + QString("_").setNum(N) + "<=5)");
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the file [" + FileName + 
		       "] has the data which are not cold be analysed (Num.of data columns "
		       + QString("_").setNum(Series.numOfDataColumns()) + "!=2)");
	}
      else
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the file [" + FileName + 
		   "] has the data which are not equidistant");
    }
  else
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the file [" + FileName + 
	       "] was not parsed properly");

  if (!IsOK)
    {
      N = 0;
      M = 0;
      DT = 0.0;
      if (X)
	{
	  delete X;
	  X = NULL;
	};
      if (Z_r)
	{
	  delete Z_r;
	  Z_r = NULL;
	};
      if (Z_i)
	{
	  delete Z_i;
	  Z_i = NULL;
	};
      ShiftR = 0.0;
      ShiftI = 0.0;
      TrendR = 0.0;
      TrendI = 0.0;
    };
};

unsigned int SBMEM::validM()
{
  unsigned int m=1;
  switch (Type)
    {
    case Type_Recursion:
      m = N-2;
      break;
    case Type_LSF:
    case Type_LSB:
      m = N/2-1;
      break;
    case Type_LSFnB:
      m = 2*N/3-1;
      break;
    };
  return m;
};

void SBMEM::createAuxVectors()
{
  if (IsMSet && M==MAuxCreated)
    {
      if (Am && EFm && EBm && Am_1 && EFm_1 && EBm_1)	// it's ok,nothing to do
	return;
      else				// explain before crashing:
	{
	  Log->write(SBLog::ERR, SBLog::DATA, ClassName() + 
		     ": misbehaviuor during createAuxVectors()");
	  return;
	};
    };
  
  if (Am)
    {
      delete[] Am;
      Am = NULL;
    };
  if (EFm)
    {
      delete EFm;
      EFm = NULL;
    };
  if (EBm)
    {
      delete EBm;
      EBm = NULL;
    };
  if (Am_1)
    {
      delete[] Am_1;
      Am_1 = NULL;
    };
  if (EFm_1)
    {
      delete EFm_1;
      EFm_1 = NULL;
    };
  if (EBm_1)
    {
      delete EBm_1;
      EBm_1 = NULL;
    };
  
  Am	= new std::complex<double>[M];
  Am_1	= new std::complex<double>[M];
  EFm	= new std::complex<double>[N];
  EFm_1	= new std::complex<double>[N];
  EBm	= new std::complex<double>[N];
  EBm_1	= new std::complex<double>[N];
  MAuxCreated = M;
};

void SBMEM::removeShiftsFromData()
{
  double	Ar, Ai;

  Ar = Ai = 0.0;
  for (unsigned int i=0; i<N; i++)
    {
      Ar += Z_r->at(i);
      Ai += Z_i->at(i);
    };
  ShiftR = Ar/N;
  ShiftI = Ai/N;
  for (unsigned int i=0; i<N; i++)
    {
      Z_r->set(i, Z_r->at(i) - ShiftR);
      Z_i->set(i, Z_i->at(i) - ShiftI);
    };  
};

void SBMEM::removeTrendsFromData()
{
  double	A, B, C;
  double	Dr, Di, Er, Ei;
  
  double	sig2; // replace in the case of real weights
  double	t0 = X->at(0);
  double	f;

  sig2 = 1.0; // sig_i*sig_i

  A = B = C = Dr = Di = Er = Ei = 0.0;
  for (unsigned int i=0; i<N; i++)
    {
      A += 1.0/sig2;
      B += (f=(X->at(i) - t0))/sig2;
      C += f*f/sig2;
      Dr+= Z_r->at(i)/sig2;
      Di+= Z_i->at(i)/sig2;
      Er+= Z_r->at(i)*f/sig2;
      Ei+= Z_i->at(i)*f/sig2;
    };
  TrendR = (A*Er - B*Dr)/(A*C - B*B);
  ShiftR = (Dr - B*TrendR)/A;
  TrendI = (A*Ei - B*Di)/(A*C - B*B);
  ShiftI = (Di - B*TrendI)/A;
  for (unsigned int i=0; i<N; i++)
    {
      Z_r->set(i, Z_r->at(i) - ShiftR - TrendR*(X->at(i) - t0));
      Z_i->set(i, Z_i->at(i) - ShiftI - TrendI*(X->at(i) - t0));
    };  
};

bool SBMEM::prepareSpectrum()
{
  if (IsReady && IsOK)
    return IsReady;

  if (!IsOK)
    return (IsReady=FALSE);
  
  if (M<2)
    {
      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": the order of the filter (" + 
		 QString("_").setNum(M) + ") is too small");
      return (IsReady=FALSE);
    };
  
  //
  // have to redesign here lately..
  createAuxVectors();
  
  switch (Type)
    {
    case Type_Recursion:
      calcA_Recusion();
      break;
    case Type_LSF:
      calcA_LSForward();
      break;
    case Type_LSB:
      calcA_LSBackward();
      break;
    case Type_LSFnB:
      calcA_LSFnB();
      break;
    };

  return IsReady;
};

void SBMEM::calcA_Recusion()
{
  unsigned int			i, m;
  double			f;
  std::complex<double>		c(0.0, 0.0);
  std::complex<double>		*Save=NULL;

  // init: m=0
  //
  for (i=1; i<N; i++)
    *(EFm+i) = std::complex<double>(Z_r->at(i), Z_i->at(i));
  for (i=0; i<N-1; i++)
    *(EBm+i) = std::complex<double>(Z_r->at(i), Z_i->at(i));
  
  for (f=0.0, i=0; i<N; i++)
    f+=mod2(Z_r->at(i),Z_i->at(i));
  Pm = f/N;
  
  // a^1_1:
  for (c=CZero,i=0; i<N-1; i++)
    c += std::conj(*(EBm+i))**(EFm+i+1);
  for (f=0.0, i=0; i<N-1; i++)
    f += mod2(*(EBm+i)) + mod2(*(EFm+i+1));
  *Am = 2.0*c/f;

  // P_1:
  Pm_1 = Pm;
  Pm = Pm_1*(1.0 - mod2(*Am));


  // iteration: m=1,M-1
  for (m=1; m<M; m++)
    {
      // swap pointers:
      // Am:
      Save	= Am;
      Am	= Am_1;
      Am_1	= Save;
      // EFm:
      Save	= EFm;
      EFm	= EFm_1;
      EFm_1	= Save;
      // EBm:
      Save	= EBm;
      EBm	= EBm_1;
      EBm_1	= Save;
      Pm_1	= Pm;
      
      // calc EFm and EBm, Eq.(8):
      for (i=1; i<N-m+1; i++)
	*(EFm+i) = *(EFm_1+i+1) - *(Am_1+m-1)**(EBm_1+i); 
      for (i=0; i<N-m; i++)
	*(EBm+i) = *(EBm_1+i)   - std::conj(*(Am_1+m-1))**(EFm_1+i+1);
      
      // calc Am, Eq.(10):
      for (c=CZero,i=0; i<N-1-m; i++)
	c+=std::conj(*(EBm+i))**(EFm+i+1);
      for (f=0.0,  i=0; i<N-1-m; i++)
	f+=mod2(*(EBm+i))+mod2(*(EFm+i+1));
      *(Am+m) = 2.0*c/f;
      // Eq.(6):
      for(i=0; i<m; i++)
	*(Am+i) = *(Am_1+i) - *(Am+m)*std::conj(*(Am_1+m-1-i));
      
      // calc Pm, Eq.(7):
      Pm = Pm_1*(1.0 - mod2(*(Am+m)));
    };
  IsReady=TRUE;
  /*
    std::cout << "Pm: " << Pm << "\n";
    for (m=0; m<M; m++) std::cout << m << ":  " << (Am+m)->real() << ",  " << (Am+m)->imag() << "\n";
    std::cout << "\n";
  */
};

void SBMEM::calcA_LSForward()
{
  SBMatrix		*A;
  SBUpperMatrix		*Rx;
  SBVector		*Z;
  SBVector		*Zx;
  SBVector		*X_;
  SBVector		*U;

  double		s,ul,g;
  unsigned int		Nx = 2*M;
  unsigned int		Nz = 2*(N-M);

  unsigned int		i, j, l;

  A  = new SBMatrix(Nz, Nx, "A");
  Rx = new SBUpperMatrix(Nx, "Rx-matrix for LSForward");
  Z  = new SBVector(Nz, "Z");
  Zx = new SBVector(Nx, "Zx");
  X_ = new SBVector(Nx, "X_");
  U  = new SBVector(Nz, "U");

  /*
  // tmp:
  SBMatrix		*Asaved = new SBMatrix(Nz, Nx, "Asaved");
  SBVector		*Zsaved = new SBVector(Nz, "Zsaved");
  */

  // fill a priori:
  for (i=0; i<Nx; i++)
    Rx->set(i,i, 1.0/10000000000.0);

  // fill A matrix and Z vector:
  for (i=0; i<N-M; i++)
    {
      for (j=0; j<M; j++)
	{
	  A->set(2*i,   2*j,     Z_r->at(M-1+i-j));
	  A->set(2*i,   2*j+1,  -Z_i->at(M-1+i-j));
	  A->set(2*i+1, 2*j,     Z_i->at(M-1+i-j));
	  A->set(2*i+1, 2*j+1,   Z_r->at(M-1+i-j));
	};
      Z->set(2*i,   Z_r->at(M+i));
      Z->set(2*i+1, Z_i->at(M+i));
    };

  /*
  // tmp:
  *Asaved = *A;
  *Zsaved = *Z;
  */
  
  // make Householder transform:
  for (l=0; l<Nx; l++)
    {
      s = Rx->at(l,l)*Rx->at(l,l);
      for (i=0; i<Nz; i++) 
	s+=A->at(i,l)*A->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Rx->at(l,l))*sqrt(s);
	  ul = Rx->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, A->at(i,l));		// U_i (i=l+1, N)
	  Rx->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Nx; j++)				// |<-- processing "Ry/Ay"-block
	    {							// |
	      g = ul*Rx->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*A->at(i,j);	// |
	      (*Rx)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*A)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  g = ul*Zx->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zy
	  (*Zx)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	};
    };
  
  s = 0.0;
  for (i=0; i<Nz; i++) 
    s+= Z->at(i)*Z->at(i);
  Pm = s/(N-M);

  // solve the system:
  Solve(*Rx, *X_, *Zx);
  //  RRT(*P, ~*R);

  /*
  // tmp:
  s = (*Asaved**X_ - *Zsaved).module();
  s *= s;
  Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": VTPV-RES2: " + QString("").sprintf("%.18g", Pm - s/(N-M)));
  Pm = s/(N-M);
  //
  */

  // fill the a's coeffs:
  for (j=0; j<M; j++)
    *(Am+j) = std::complex<double>(X_->at(2*j), X_->at(2*j+1));
  
  delete A;
  delete Rx;
  delete Z;
  delete Zx;
  delete X_;
  delete U;
  /*
  // tmp:
  delete Asaved;
  delete Zsaved;
  */
  IsReady=TRUE;
};

void SBMEM::calcA_LSBackward()
{
  SBMatrix		*A;
  SBUpperMatrix		*Rx;
  SBVector		*Z;
  SBVector		*Zx;
  SBVector		*X_;
  SBVector		*U;

  double		s,ul,g;
  unsigned int		Nx = 2*M;
  unsigned int		Nz = 2*(N-M);

  unsigned int		i, j, l;

  A  = new SBMatrix(Nz, Nx, "A");
  Rx = new SBUpperMatrix(Nx, "Rx-matrix for LSForward");
  Z  = new SBVector(Nz, "Z");
  Zx = new SBVector(Nx, "Zx");
  X_ = new SBVector(Nx, "X_");
  U  = new SBVector(Nz, "U");

  // fill a priori:
  for (i=0; i<Nx; i++)
    Rx->set(i,i, 1.0/10000000000.0);

  // fill A matrix and Z vector:
  for (i=0; i<N-M; i++)
    {
      for (j=0; j<M; j++)
	{
	  A->set(2*i,   2*j,     Z_r->at(1+i+j));
	  A->set(2*i,   2*j+1,   Z_i->at(1+i+j));
	  A->set(2*i+1, 2*j,    -Z_i->at(1+i+j));
	  A->set(2*i+1, 2*j+1,   Z_r->at(1+i+j));
	};
      Z->set(2*i,    Z_r->at(i));
      Z->set(2*i+1, -Z_i->at(i));
    };

  // make Householder transform:
  for (l=0; l<Nx; l++)
    {
      s = Rx->at(l,l)*Rx->at(l,l);
      for (i=0; i<Nz; i++) 
	s+=A->at(i,l)*A->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Rx->at(l,l))*sqrt(s);
	  ul = Rx->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, A->at(i,l));		// U_i (i=l+1, N)
	  Rx->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Nx; j++)				// |<-- processing "Ry/Ay"-block
	    {							// |
	      g = ul*Rx->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*A->at(i,j);	// |
	      (*Rx)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*A)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  g = ul*Zx->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zy
	  (*Zx)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	};
    };
  
  s = 0.0;
  for (i=0; i<Nz; i++) 
    s+= Z->at(i)*Z->at(i);
  Pm = s/(N-M);

  // solve the system:
  Solve(*Rx, *X_, *Zx);
  //  RRT(*P, ~*R);

  // fill the a's coeffs:
  for (j=0; j<M; j++)
    *(Am+j) = std::complex<double>(X_->at(2*j), X_->at(2*j+1));
  
  delete A;
  delete Rx;
  delete Z;
  delete Zx;
  delete X_;
  delete U;
  IsReady=TRUE;
};

void SBMEM::calcA_LSFnB()
{
  SBMatrix		*A;
  SBUpperMatrix		*Rx;
  SBVector		*Z;
  SBVector		*Zx;
  SBVector		*X_;
  SBVector		*U;

  double		s,ul,g;
  unsigned int		Nx = 2*M;
  unsigned int		Nz = 4*(N-M);

  unsigned int		i, j, l;

  A  = new SBMatrix(Nz, Nx, "A");
  Rx = new SBUpperMatrix(Nx, "Rx-matrix for LSForward");
  Z  = new SBVector(Nz, "Z");
  Zx = new SBVector(Nx, "Zx");
  X_ = new SBVector(Nx, "X_");
  U  = new SBVector(Nz, "U");

  // fill a priori:
  for (i=0; i<Nx; i++)
    Rx->set(i,i, 1.0/10000000000.0);

  // fill A matrix and Z vector:
  // forward part:
  for (i=0; i<N-M; i++)
    {
      for (j=0; j<M; j++)
	{
	  A->set(2*i,   2*j,     Z_r->at(M-1+i-j));
	  A->set(2*i,   2*j+1,  -Z_i->at(M-1+i-j));
	  A->set(2*i+1, 2*j,     Z_i->at(M-1+i-j));
	  A->set(2*i+1, 2*j+1,   Z_r->at(M-1+i-j));
	};
      Z->set(2*i,   Z_r->at(M+i));
      Z->set(2*i+1, Z_i->at(M+i));
    };
  // backward part:
  unsigned int Ns = 2*(N-M);
  for (i=0; i<N-M; i++)
    {
      for (j=0; j<M; j++)
	{
	  A->set(Ns + 2*i,   2*j,     Z_r->at(1+i+j));
	  A->set(Ns + 2*i,   2*j+1,   Z_i->at(1+i+j));
	  A->set(Ns + 2*i+1, 2*j,    -Z_i->at(1+i+j));
	  A->set(Ns + 2*i+1, 2*j+1,   Z_r->at(1+i+j));
	};
      Z->set(Ns + 2*i,    Z_r->at(i));
      Z->set(Ns + 2*i+1, -Z_i->at(i));
    };


  // make Householder transform:
  for (l=0; l<Nx; l++)
    {
      s = Rx->at(l,l)*Rx->at(l,l);
      for (i=0; i<Nz; i++) 
	s+=A->at(i,l)*A->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Rx->at(l,l))*sqrt(s);
	  ul = Rx->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, A->at(i,l));		// U_i (i=l+1, N)
	  Rx->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Nx; j++)				// |<-- processing "Ry/Ay"-block
	    {							// |
	      g = ul*Rx->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*A->at(i,j);	// |
	      (*Rx)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*A)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  g = ul*Zx->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zy
	  (*Zx)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	};
    };
  
  s = 0.0;
  for (i=0; i<Nz; i++) 
    s+= Z->at(i)*Z->at(i);
  Pm = s/(N-M)/2.0;

  // solve the system:
  Solve(*Rx, *X_, *Zx);
  //  RRT(*P, ~*R);

  // fill the a's coeffs:
  for (j=0; j<M; j++)
    *(Am+j) = std::complex<double>(X_->at(2*j), X_->at(2*j+1));
  
  delete A;
  delete Rx;
  delete Z;
  delete Zx;
  delete X_;
  delete U;
  IsReady=TRUE;
};

double SBMEM::psd(double f)
{
  double			PSD = 0.0;
  unsigned int			i;
  std::complex<double>		c(0.0, 0.0);

  if (fN()<fabs(f))
    return PSD;

  for (c=CZero,i=0; i<M; i++)
    c += *(Am+i)*std::exp(-2.0*M_PI*CI*f*(DT*(i+1)));
  PSD = Pm*DT/mod2(std::complex<double>(1.0,0.0) - c);
  
  return PSD;
};

/*==============================================================================================*/


/*==============================================================================================*/
