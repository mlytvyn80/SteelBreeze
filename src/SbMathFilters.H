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
 * Overview:	data smoothing, interpolations, etc.;
 *
 */


#ifndef SB_MATH_FILTERS_H
#define SB_MATH_FILTERS_H

#include <config.h>
#include <complex>

#include <qintdict.h>

#include "SbGeneral.H"
#include "SbMathDataSeries.H"
#include "SbVector.H"




/*==============================================================================================*/
/*												*/
/* class SBFilterModel										*/
/*												*/
/*==============================================================================================*/
class SBFilterModel : public SBAttributed
{
public:
  enum Attributes
  {
    IsOmit3SigmaBefore		   	= 1<<0,		//!< ignore data with sig>3(mean_sigma)
    IsOmit3SigmaAfter		   	= 2<<0,		//!< test
  };

protected:
  double		T0;
  
  unsigned int		NumOfData;
  unsigned int		NumOfPolinoms;
  unsigned int		NumOfHarmonics;
  SBVector		*Periods;

  SBVector		*Polinoms;
  SBVector		*PolinomsSigma;
  SBMatrix		*Harmonics;
  SBMatrix		*HarmonicsSigma;
  bool			*IsKeepTerm;

  // spec.model:
  bool			IsRunSM;
  unsigned int		NumOfSMTerms;
  SBMatrix		*SMPolinoms;
  SBMatrix		*SMPolinomsSigma;
  double		SMPeriod;
  bool			IsKeepSM;
  //

  SBDataSeries		*Series;
  SBVector		*DataAttribute;
  unsigned int		Idx;
  unsigned int		IdxSig;
  bool			IsOK;
  double		VTPV;
  double		SumW2;

public:
  // constructors/destructors:
  //
  SBFilterModel(unsigned int=0, unsigned int=0);
  /**A destructor.*/
  virtual ~SBFilterModel();
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBFilterModel";};


  // interface:
  //
  double		t0() const {return T0;};
  unsigned int		numOfPolinoms() const {return NumOfPolinoms;};
  unsigned int		numOfHarmonics() const {return NumOfHarmonics;};
  unsigned int		numOfUnknowns() const 
  {return NumOfPolinoms + 2*NumOfHarmonics + (IsRunSM?(2*NumOfSMTerms):0);};
  
  SBVector*		periods() {return Periods;};
  SBVector*		polinoms() {return Polinoms;};
  SBVector*		polinomsSigma() {return PolinomsSigma;};
  SBVector*		dataAttribute() {return DataAttribute;};
  SBMatrix*		harmonics() {return Harmonics;};
  SBMatrix*		harmonicsSigma() {return HarmonicsSigma;};
  bool			isOK() const {return IsOK;};
  unsigned int		idx() const {return Idx;};
  unsigned int		idxSig() const {return IdxSig;};

  void			setT0(double t_) {T0 = t_;};
  void			setNumOfPolinoms(unsigned int);
  void			setNumOfHarmonics(unsigned int);
  void			setSeries(SBDataSeries *series) {Series = series;};
  void			setDataAttribute(SBVector *DA_) {DataAttribute = DA_;};

  // spec.model:
  void			setIsRunSM (bool Is_) {IsRunSM =Is_;};
  void			setUpSpecModel(unsigned int, double, bool);
  //


  
  // functions:
  //
  bool			isCanRun();
  void			analyseSeries();
  unsigned int		eliminateBefore();
  unsigned int		eliminateAfter();
  double		modelFull(double);
  double		modelKeep(double);
  void			setTermToKeep(unsigned int, bool);

  // I/O: 
  //

};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* class SBFilterGauss										*/
/*												*/
/*==============================================================================================*/
class SBFilterGauss : public SBAttributed
{
public:
  enum Attributes
  {
    IsOmit3Sigma		   	= 1<<0,		//!< test
  };


  
protected:
  SBDataSeries		*Series;
  SBDataSeries		*SeriesMinusModel;
  SBVector		*DataAttribute;
  bool			IsOK;
  
  double		FWHM;
  double		TStart;
  double		TFinis;
  unsigned int		NColOfData;
  unsigned int		N;

  SBVector*		vAux;
  SBVector*		Sigmas;
  SBMatrix*		mAux;


  double		StdVar;

  // for models:
  QList<SBFilterModel>		ModelList;
  QIntDict<SBFilterModel>	ModelByIdx;

public:
  // constructors/destructors:
  //
  SBFilterGauss();
  /**A destructor.*/
  virtual ~SBFilterGauss();
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBFilterGauss";};


  // interface:
  //
  SBDataSeries*		series() {return Series;};
  SBDataSeries*		seriesMinusModel() {return SeriesMinusModel;};
  double		fwhm() const {return FWHM;};
  unsigned int		nColOfData() const {return NColOfData;};
  unsigned int		n() const {return N;};
  bool			isOK() const {return IsOK;};
  double		tStart() const {return TStart;};
  double		tFinis() const {return TFinis;};

  void			setFWHM(double w_) {FWHM = w_;};

  QList<SBFilterModel>*	modelList() {return &ModelList;};
  QIntDict<SBFilterModel>*	modelByIdx(){return &ModelByIdx;};

  
  // functions:
  //
  void			readDataFile(const QString&);
  void			calcSigma(unsigned int, unsigned int);
  double		interpolate(double, unsigned int=0, unsigned int=0);
  double		stdVar() const {return StdVar;};
  double		respFunc(double);
  double		sigma(unsigned int Idx) const {return Sigmas?Sigmas->at(Idx):0.0;};

  void			addModel(unsigned int, unsigned int);
  void			prepareModels();
  void			rmModels() {ModelByIdx.clear(); ModelList.clear();};
  double		interpolateMinusModel(double, unsigned int=0, unsigned int=0);
  void			substractFilter(bool, int);

  // I/O: 
  //

};
/*==============================================================================================*/

#endif //SB_MATH_FILTERS_H
