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
 * Overview:	Interpolation by cubic splines;
 *
 */

#ifndef SB_MATH_SPLINE_H
#define SB_MATH_SPLINE_H

#include <config.h>

#include "SbGeneral.H"
#include "SbVector.H"


/*==============================================================================================*/
/*												*/
/* class SBSpline										*/
/*												*/
/*==============================================================================================*/
class SBSpline
{
protected:
  SBMatrix*	B;
  bool		IsOK;
  bool		IsReady;
  unsigned int	NDimension;	//!< the dimension of a vector to spline
  unsigned int	N;		//!< the number of points

  void		solveSpline(unsigned int);
  double	h(unsigned int i) const {return B->at(i, 0) - B->at(i-1, 0);};
public:
  // constructors/destructors:
  //
  SBSpline(unsigned int, unsigned int=1);
  /**A destructor.*/
  virtual ~SBSpline();
  
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBSpline";};

  // interface:
  //
  unsigned int	nDimension() const {return NDimension;};
  unsigned int	n()          const {return N;};
  SBMatrix*	b(){return B;};
  bool		isReady() const {return IsReady;};

  // functions:
  //
  virtual void prepare4Spline();
  virtual double spline(unsigned int, double);

  // I/O:
  //
};
/*==============================================================================================*/

#endif //SB_MATH_SPLINE_H
