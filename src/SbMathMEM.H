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
 *
 * Overview:	MEM manipulations;
 *
 */


#ifndef SB_MATH_MEM_H
#define SB_MATH_MEM_H

#include <config.h>
#include <complex>

#include "SbGeneral.H"
#include "SbMathDataSeries.H"
#include "SbVector.H"



/*==============================================================================================*/
/*												*/
/* class SBMEM											*/
/*												*/
/*==============================================================================================*/
class SBMEM : public SBAttributed
{
public:
  enum MEMType {Type_Recursion=0, Type_LSF=1, Type_LSB=2, Type_LSFnB=3};

  enum Attributes
  {
    ignoreNotEquidistant   	= 1<<0,		//!< assume data are equidistant
    fillMissingImag		= 1<<1,		//!< fill missing imag-column with zero
    removeShifts		= 1<<2,		//!< remove shifts from data
    removeTrends		= 1<<3,		//!< remove shifts+trends from data
  };


  
protected:
  SBVector*		X;		//!< the argument
  SBVector*		Z_r;		//!< real part of data
  SBVector*		Z_i;		//!< imagenary part of data

  std::complex<double>	*Am;
  std::complex<double>	*Am_1;
  std::complex<double>	*EFm;
  std::complex<double>	*EFm_1;
  std::complex<double>	*EBm;
  std::complex<double>	*EBm_1;
  double		Pm;
  double		Pm_1;


  double		DT;		//!< sample interval
  bool			IsOK;
  bool			IsReady;
  double		ShiftR;
  double		ShiftI;
  double		TrendR;
  double		TrendI;

  bool			IsMSet;

  unsigned int		M;		//!< the length of the filter
  unsigned int		N;		//!< the number of points
  MEMType		Type;		//!< the method of a's calculation

  unsigned int		MAuxCreated;
  double		SampleVar;


  static const std::complex<double>	CZero;
  static const std::complex<double>	CI;


  void			calcA_Recusion();
  void			calcA_LSForward();
  void			calcA_LSBackward();
  void			calcA_LSFnB();
  void			createAuxVectors();
  void			removeShiftsFromData();
  void			removeTrendsFromData();

  // helpers:
  inline double	mod2(double r, double i) const {return r*r+i*i;};
  inline double	mod2(std::complex<double> c) const {return c.real()*c.real()+c.imag()*c.imag();};

public:
  // constructors/destructors:
  //
  SBMEM();
  /**A destructor.*/
  virtual ~SBMEM();

  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBMEM";};

  // interface:
  //
  inline unsigned int		m()	const	{return M;};
  inline unsigned int		n()	const	{return N;};
  inline SBVector*		x()		{return X;};
  inline SBVector*		z_r()		{return Z_r;};
  inline SBVector*		z_i()		{return Z_i;};
  inline bool			isReady() const {return IsReady;};
  inline bool			isOK()	const {return IsOK;};
  inline MEMType		type()	const {return Type;};
  inline double			dt()	const {return DT;};
  inline double			shiftR()const {return ShiftR;};
  inline double			trendR()const {return TrendR;};
  inline double			shiftI()const {return ShiftI;};
  inline double			trendI()const {return TrendI;};
  inline double			sampleVar()const {return SampleVar;};



  inline void			setM(unsigned int M_) 
  {if (M!=M_) IsReady=FALSE; M=std::min(validM(),M_); IsMSet=TRUE;};
  inline void			setType(MEMType T_) {if (Type!=T_) IsReady=FALSE; Type=T_;};
  unsigned int			validM();
  inline std::complex<double>	am(unsigned int i) {return (Am&&IsMSet&&i<M)?*(Am+i):CZero;};
  

  // functions:
  //
  void				readDataFile(const QString&);
  bool				prepareSpectrum();
  inline double			fN() const {return IsOK&&DT>0.0?0.5/DT:0.0;};
  inline double			pm() const {return Pm;};
  double			psd(double);
  inline double			interval() const {return IsOK?X->at(N-1)-X->at(0):0.0;};
  inline double			fpe() const 
  {return IsOK?((isAttr(removeShifts)||isAttr(removeTrends))?Pm*(N+M+1)/(N-M-1):Pm*(N+M)/(N-M)):0.0;};

  // I/O: 
  //
};
/*==============================================================================================*/

#endif //SB_MATH_MEM_H
