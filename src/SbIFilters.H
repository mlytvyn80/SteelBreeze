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

#ifndef SB_I_FILTERS_H
#define SB_I_FILTERS_H


#include <config.h>


#include "SbGeneral.H"
#include "SbMathFilters.H"
#include "SbInterface.H"



class QCheckBox;
class QDoubleValidator;
class QIntValidator;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QTabWidget;

class SBPlotCarrier;
class SBPlot;
/*==============================================================================================*/
/*												*/
/* SBFilteringGauss..										*/
/*												*/
/*==============================================================================================*/
class SBFilteringGauss : public QDialog
{
  Q_OBJECT    
public:
  SBFilteringGauss(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBFilteringGauss();
  virtual QString ClassName() const {return "SBFilteringGauss";};
  
  protected 
  slots:

  void			accept();
  void			reject();
  void	 		readDataFile();
  void			makeAnalysis();

  void			wrote2File();
  void			changeNInterpolate(const QString&);
  void			changeFWHM(const QString&);
  void			changeStep(const QString&);
  void			changeEpoch(const QString&);
  void			changeByNum(int);
  void			setSigmasMode(int);
  void			setFilterType(int);
  void			calcSigmas();
  void			makeAnalysisWith3Sigma();
  void			substractFilter();
  
  
private:
  enum SIGMASMODE {SIG_MODE_0=0, SIG_MODE_1=1, SIG_MODE_2=2};
  enum FILTERTYPE {FILT_GAUSS=0, FILT_GAUSS_APRIORY=1};
  // 
  QString		Name;
  unsigned int		NInterpolate;
  double		Step;
  unsigned int		NumOfCols;
  unsigned int		NumOfDataCols;
  unsigned int		NperTF;
  double		FWHM;
  double		Epoch;
  SBFilterGauss		*Filter;
  bool			IsByNum;
  bool			IsFilterSubstracted;
  SIGMASMODE		SMode;
  FILTERTYPE		FType;

  // Widgets:
  QTabWidget		*Tab;


  // options:
  QLabel		*lImportedFileName;
  QLabel		*lNumOfPts;
  QLabel		*lNumOfCols;
  QLabel		*lDT;
  QLabel		*lDTMinMax;
  QLabel		*lInterval;

  QRadioButton		*rbFilterType[1];
  QRadioButton		*rbSigmasMode[3];

  QLineEdit		*leNInterpolate;
  QLineEdit		*leStep;
  QLineEdit		*leFWHM;
  QLineEdit		*leEpoch;


  // buttons:
  QPushButton		*Save;
  QPushButton		*GetData;
  QPushButton		*Analyse;
  QPushButton		*Ok;
  
  //-----
  /* functions */
  QWidget		*wOptions();
  QWidget		*wBrowseData();
  QWidget		*wBrowseTF();
  void			collectData();


  // Data:
  SBPlotCarrier		*PCData;
  SBPlot		*PlotData;
  void			redrawDataPlot();
  void			redrawDataPlot_ExpMode();

  // Transfer function:
  SBPlotCarrier		*PCTF;
  SBPlot		*PlotTF;
  void			redrawTFPlot();

  // shortcuts:
  QAccel		*accel;
};
/*==============================================================================================*/



 


#endif //SB_I_FILTERS_H
