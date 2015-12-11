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

#include "SbMathSpline.H"

/*==============================================================================================*/
/*												*/
/* SBSpline implementation									*/
/*												*/
/*==============================================================================================*/
SBSpline::SBSpline(unsigned int N_, unsigned int M_)
{
  B = NULL;
  N = 0;
  NDimension=1;
  IsOK    = FALSE;
  IsReady = FALSE;
  if (N_<4)
    Log->write(SBLog::ERR, SBLog::SPLINE, ClassName() + ": number of rows less then 4; ignored");
  if (M_<1)
    Log->write(SBLog::ERR, SBLog::SPLINE, ClassName() + ": nothing to spline (NDimension=0); ignored");
  else
    {
      B = new SBMatrix((N=N_), 1+(NDimension=M_)*2, "The body of splines");
      IsOK = TRUE;
    };
};

SBSpline::~SBSpline()
{
  if (B)
    {
      delete B;
      B = NULL;
    };
};

void SBSpline::prepare4Spline() 
{
  if (NDimension>0 && N>3)
    {
      // check the order of arg:
      double f = B->at(0,0);
      for (unsigned int i=1; i<N; i++)
	{
	  if (f>=B->at(i, 0))
	    {
	      Log->write(SBLog::ERR, SBLog::SPLINE, ClassName() + 
			 ": the argument [" + QString().setNum(i)+ "] has a wrong order, ignored");
	      for (unsigned int k=0; k<N; k++)
		Log->write(SBLog::ERR, SBLog::SPLINE, ClassName() + QString().sprintf(": [%04d] = %.6f", k, B->at(k,0)));
	      return;
	    };
	  f = B->at(i, 0);
	};
      for (unsigned int i=0; i<NDimension; i++)
	solveSpline(i);
      IsReady = TRUE;
    };
};

void SBSpline::solveSpline(unsigned int nCol)
{
  SBVector	Aleph(N, "Aleph");
  SBVector	Beta (N, "Beta");
  double	f;
  unsigned int	nF = 1+nCol, nM=1+NDimension+nCol;

  f = h(1) + h(2);
  Aleph(2) = -0.5*h(2)/f;
  Beta (2) = 3.0*(B->at(0, nF)/h(1) - B->at(1, nF)*(1.0/h(1) + 1.0/h(2)) + B->at(2, nF)/h(2))/f;
  
  for (unsigned int i=2; i<N-2; i++)
    {
      f = Aleph(i)*h(i) + 2.0*(h(i) + h(i+1));
      Aleph(i+1) = -h(i+1)/f;
      Beta (i+1) = (6.0*(B->at(i-1, nF)/h(i) - B->at(i, nF)*(1.0/h(i) + 1.0/h(i+1)) + B->at(i+1, nF)/h(i+1))
		    - Beta(i)*h(i))/f;
    };

  (*B)(N-1, nM) = 0.0;
  (*B)(N-2, nM) = (6.0*(B->at(N-3, nF)/h(N-2) - B->at(N-2, nF)*(1.0/h(N-2) + 1.0/h(N-1)) + 
			B->at(N-1, nF)/h(N-1)) - h(N-2)*Beta(N-2)) /
    (h(N-2)*Aleph(N-2) + 2.0*(h(N-2) + h(N-1)));

  for (unsigned int i=N-3; i>0; i--)
      (*B)(i, nM) = Aleph.at(i+1)*B->at(i+1, nM) + Beta.at(i+1);

  (*B)(0, nM) = 0.0;
};

double SBSpline::spline(unsigned int nCol, double x)
{
  if (!IsReady)
    {
      Log->write(SBLog::ERR, SBLog::SPLINE, ClassName() + ": the spline is not ready");
      return 0.0;
    };
  if (nCol>=NDimension)
    {
      Log->write(SBLog::ERR, SBLog::SPLINE, ClassName() + ": the nCol [" + QString().setNum(nCol) + 
		 "] out of range [0.." + QString().setNum(NDimension-1) + "]");
      return 0.0;
    };
  unsigned int	nF = 1+nCol, nM=1+NDimension+nCol, Idx = 0;
  
  // get the idx:
  if (x<=B->at(1, 0)) 
    Idx = 1;
  else if (B->at(N-1, 0)<=x)
    Idx = N-1;
  else
    {
      unsigned int i=2;
      while (i<N && B->at(i-1, 0)<=x) i++;
      Idx = i-1;
    };
  
  double f1 = B->at(Idx, 0) - x, f2 = x - B->at(Idx-1, 0), h2=h(Idx)*h(Idx);
  return (B->at(Idx-1, nM)*f1*f1*f1/6.0 + B->at(Idx, nM)*f2*f2*f2/6.0 +
	  (B->at(Idx-1, nF) - B->at(Idx-1, nM)*h2/6.0)*f1 +
	  (B->at(Idx,   nF) - B->at(Idx,   nM)*h2/6.0)*f2)/h(Idx);
};
/*==============================================================================================*/
