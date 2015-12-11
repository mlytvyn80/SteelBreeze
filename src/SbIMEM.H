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

#ifndef SB_I_MEM_H
#define SB_I_MEM_H


#include <config.h>


#include "SbGeneral.H"
#include "SbMathMEM.H"
#include "SbInterface.H"



/*==============================================================================================*/
/*												*/
/* SBMEMECoeffLI..										*/
/*												*/
/*==============================================================================================*/
class SBMEMECoeffLI : public QListViewItem
{
private:
  std::complex<double>		C;
  unsigned int			Idx;

public:
  SBMEMECoeffLI(QListView* parent, unsigned int Idx_, std::complex<double> C_) 
    : QListViewItem(parent){Idx=Idx_; C=C_;};
  std::complex<double>	c()  {return C;};
  unsigned int		idx(){return Idx;};
  virtual QString ClassName() const {return "SBMEMECoeffLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/




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
/* SBSpectrumAnalyserMEM..									*/
/*												*/
/*==============================================================================================*/
class SBSpectrumAnalyserMEM : public QDialog
{
  Q_OBJECT    
public:
  SBSpectrumAnalyserMEM(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBSpectrumAnalyserMEM();
  virtual QString ClassName() const {return "SBSpectrumAnalyserMEM";};
  
  protected 
  slots:

  void			accept();
  void			reject();
  void	 		readDataFile();
  void			makeAnalysis();
  void			scan4FPE();
  void			resetLimits();

  void			changeFilterOrder(int);

  void			setMEMAssumeEquiDistant(bool);
  void			setMEMFillMissingImag(bool);
  void			setMEMRemoveShifts(bool);
  void			setMEMRemoveTrends(bool);
  void			setMEMType(int);
  void			changeNumOfSpPts(const QString&);
  void			changeFStart(const QString&);
  void			changeFFinis(const QString&);
  void			changePStart(const QString&);
  void			changePFinis(const QString&);
  void			setRangeFreq(bool);
  void			changeFPE_Mstart(const QString&);
  void			changeFPE_Mfinis(const QString&);
  void			changeLine2Check(const QString&);
  void			setScalingCPD2CPY(bool);
  void			browseLimits();
  void			checkLine();
  
private:
  // 
  SBMEM			*MESA;
  SBMEM::MEMType	MEMType;
  unsigned int		M;
  unsigned int		Mmin;
  unsigned int		Mmax;
  QString		Name;
  bool			IsAutoCalc;
  unsigned int		NSp;
  bool			IsPlotFreq;
  double		FStart;
  double		FFinis;
  double		PStart;
  double		PFinis;
  double		FLine;
  int			FPE_Mstart;
  int			FPE_Mfinis;
  bool			IsScanning;
  bool			IsScanStop;
  int			MinFPEOrder;
  double		MinFPE;
  bool			ScaleCPD2CPY;
  

  QString		Str;

  // Widgets:
  QTabWidget		*Tab;
  QSpinBox		*sbMval;


  // options:
  QLabel		*lImportedFileName;
  QLabel		*lNumOfPts;
  QLabel		*lDT;
  QLabel		*lShiftR;
  QLabel		*lTrendR;
  QLabel		*lShiftI;
  QLabel		*lTrendI;
  QLabel		*lSampleVar;

  QRadioButton		*rbMEMType[4];
  QCheckBox		*cbAssumeEquiDistant;
  QCheckBox		*cbFillMissingImag;
  QCheckBox		*cbRmShifts;
  QCheckBox		*cbRmTrends;
  QLineEdit		*leNumOfSpPts;

  QLineEdit		*leFStart;
  QLineEdit		*leFFinis;
  QLineEdit		*lePStart;
  QLineEdit		*lePFinis;
  QLineEdit		*leFPEMstart;
  QLineEdit		*leFPEMfinis;
  QLineEdit		*leLine2Check;


  QDoubleValidator	*dvFStart;
  QDoubleValidator	*dvFFinis;
  QDoubleValidator	*dvFLine;
  QDoubleValidator	*dvPStart;
  QDoubleValidator	*dvPFinis;
  QIntValidator		*ivMscan;

  QLabel		*lMinFPE;
  QLabel		*lMinFPEOrder;
  QCheckBox		*cbScaleCPD2CPY;

  
  // buttons:
  QPushButton		*ResetLimits;
  QPushButton		*GetData;
  QPushButton		*Analyse;
  QPushButton		*Ok;
  QPushButton		*Scan;
  
  //-----
  /* functions */
  QWidget		*wOptions();
  QWidget		*wBrowseData();
  QWidget		*wBrowseSpectrum();
  QWidget		*wMEMCoeffList();
  void			collectData();


  // Data:
  SBPlotCarrier		*PCData;
  SBPlot		*PlotData;
  void			redrawDataPlot();

  // Spectrum:
  SBPlotCarrier		*PCSpectrum;
  SBPlot		*PlotSpectrum;
  void			redrawSpectrumPlot();

  // FPE scan:
  SBPlotCarrier		*PCFPE;
  SBPlot		*PlotFPE;
  void			redrawFPEPlot();

  // shortcuts:
  QAccel		*accel;
};
/*==============================================================================================*/



 


#endif //SB_I_MEM_H
