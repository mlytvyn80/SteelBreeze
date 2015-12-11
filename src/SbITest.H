
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

#ifndef SB_I_TEST_H
#define SB_I_TEST_H

#include <config.h>
#include <qdialog.h>

#include "SbGeneral.H"
#include "SbInterface.H"
#include "SbVector3.H"



class QLineEdit;
class QLabel;
class SBMJD;
/*==============================================================================================*/
/*												*/
/* SBTest..											*/
/*												*/
/*==============================================================================================*/
class SBTest : public QDialog
{
  Q_OBJECT    
public:
  SBTest(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTest();
  
  virtual QString ClassName() const {return "SBTest";};
  
  protected
  slots:
  void accept();
  virtual void recalc();
  void reject();
  
protected:
  virtual void		closeEvent(QCloseEvent *){delete this;};
  
  QString		str;
  SBMJD			*Date;
  SBMJD			*TStart;
  SBMJD			*TFinis;
  
  
  QLabel		*LMJD;
  QLabel		*LDate;
  QLabel		*LRange;
  QLineEdit		*EYear;
  QLineEdit		*EMonth;
  QLineEdit		*EDay;
  QLineEdit		*EHour;
  QLineEdit		*EMin;
  QLineEdit		*ESec;
  bool			IsDateModified;
  
  virtual QWidget	*createWidget4Test(QWidget*);
  virtual QWidget	*auxWidgetRt(QWidget*) {return NULL;};
  virtual void		createWidgets();
  virtual SBMJD*	tStart(){return TStart;};
  virtual SBMJD*	tFinis(){return TFinis;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestMatrix..										*/
/*												*/
/*==============================================================================================*/
class SBTestMatrix : public SBTest
{
  Q_OBJECT    
public:
  SBTestMatrix(QWidget* parent=0, const char* name=0, bool modal=FALSE, WFlags f=0)
    : SBTest(parent, name, modal, f){Mat=NULL;};
  virtual ~SBTestMatrix(){};
  
  virtual QString ClassName() const {return "SBTestMatrix";};
  
protected:
  Matrix3		*Mat;
  QString		str;
  QLabel		*LMat[3][3];
  
  virtual QWidget	*createMatrixWidget(QWidget*);
  void			displayValues();
};
/*==============================================================================================*/



class SBPrecession;
class SBPrec_IAU1976;
class SBTime;
/*==============================================================================================*/
/*												*/
/* SBTestPrecession..										*/
/*												*/
/*==============================================================================================*/
class SBTestPrecession : public SBTestMatrix
{
  Q_OBJECT    
public:
  SBTestPrecession(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestPrecession();
  
  virtual QString ClassName() const {return "SBTestPrecession";};
  
  private 
  slots:
  virtual void recalc();
private:
  QString		str;
  SBPrec_IAU1976	*Pre;
  //  SBPrecession	*Pre;
  SBTime		*Time;
  QLabel		*LZeta;
  QLabel		*LTheta;
  QLabel		*LZ;
  QLabel		*LTT;
  
  virtual QWidget	*createWidget4Test(QWidget*);
  void			displayValues();
};
/*==============================================================================================*/


class SBNutation;
class SBEOP;
/*==============================================================================================*/
/*												*/
/* SBTestNutation..										*/
/*												*/
/*==============================================================================================*/
class SBTestNutation : public SBTestMatrix
{
  Q_OBJECT    
public:
  SBTestNutation(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestNutation();
  
  virtual QString ClassName() const {return "SBTestNutation";};
  
  private 
  slots:
  virtual void recalc();
private:
  QString		str;
  SBNutation		*Nut;
  SBTime		*Time;
  SBEOP			*Weop;

  QLabel		*LThDeltaPsi;
  QLabel		*LThDeltaEps;
  QLabel		*LDeltaPsi;
  QLabel		*LDeltaEps;
  QLabel		*LMeanEps;
  QLabel		*LTrueEps;
  QLabel		*LEqEquinox;
  QLabel		*LGeodesicNutation;
  QLabel		*LTT;
  
  virtual QWidget	*createWidget4Test(QWidget*);
  void			displayValues();
};
/*==============================================================================================*/



class SBFrame;
/*==============================================================================================*/
/*												*/
/* SBTestFrame..										*/
/*												*/
/*==============================================================================================*/
class SBTestFrame : public SBTestMatrix
{
  Q_OBJECT    
public:
  SBTestFrame(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestFrame();
  
  virtual QString ClassName() const {return "SBTestFrame";};
  
  private 
  slots:
  virtual void recalc();
private:
  QString		str;
  SBFrame		*Frame;

  //Time:
  QLabel		*LTAI;
  QLabel		*LTT;
  QLabel		*LUT1;
  
  //FARGs:
  QLabel		*LArg0;
  QLabel		*LArg1;
  QLabel		*LArg2;
  QLabel		*LArg3;
  QLabel		*LArg4;
  QLabel		*LArg5;

  //GST:
  QLabel		*LGMST;
  QLabel		*LGST;
  
  //Diurnals:
  QLabel		*LDiurnXP;
  QLabel		*LDiurnYP;
  QLabel		*LDiurnUT;

  virtual QWidget	*createWidget4Test(QWidget*);
  void			displayValues();
};
/*==============================================================================================*/



class SBEphem;
/*==============================================================================================*/
/*												*/
/* SBTestEphem..										*/
/*												*/
/*==============================================================================================*/
class SBTestEphem : public SBTest
{
  Q_OBJECT    
public:
  SBTestEphem(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestEphem();
  
  virtual QString ClassName() const {return "SBTestEphem";};
  
  private 
  slots:
  virtual void recalc();
  void originChanged(int);
  void unitChanged(int);
  void frameTypeChanged(int);
  void frameChanged(int);

private:
  enum			EUnits {U_KM=0, U_AU=1, U_U=2};
  enum			EFrameType {FT_ORTH=0, FT_SPH=1};
  enum			EFrame {F_CRS=0, F_TRS=1};
  double		Scale[3];
  EUnits		Units;
  EFrame		Frame;
  EFrameType		FrameType;
  SBEphem		*Weph;
  SBFrame		*Wref;
  const Vector3		*Origin;
  const Matrix3		*RF;
  Vector3		v0;
  Matrix3		m0;

  QLabel		*LCoo[11][3];
  QLabel		*LCLabel[3];
  
  virtual QWidget	*createWidget4Test(QWidget*);
  void			displayCoo();
};
/*==============================================================================================*/


class SBPlot;
class SBPlotCarrier;
class SBMatrix;
class SBSpline;
class SBNutThGeneral;
/*==============================================================================================*/
/*												*/
/* SBTestEOP..											*/
/*												*/
/*==============================================================================================*/
class SBTestEOP : public SBTest
{
  Q_OBJECT    
public:
  SBTestEOP(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestEOP();
  
  virtual QString ClassName() const {return "SBTestEOP";};
  
  private 
  slots:
  virtual void recalc();

private:
  SBEOP			*Weop;
  SBTime		*Time;
  SBNutThGeneral	*Theory;
  unsigned int		NumOfPts;
  unsigned int		NumOfSplines;
  double		Step;
  QLineEdit		*ENumPts;
  QLineEdit		*EStep;
  QLabel		*LNumOfSplines;

  SBPlotCarrier		*PlotCarrier;
  SBPlot		*Plot;

  void			fillData4Plotting();


  virtual QWidget	*createWidget4Test(QWidget*);
  virtual QWidget	*auxWidgetRt(QWidget*);
};
/*==============================================================================================*/



class SBOTideEOPGeneral;
/*==============================================================================================*/
/*												*/
/* SBTestDiurnEOP..										*/
/*												*/
/*==============================================================================================*/
class SBTestDiurnEOP : public SBTest
{
  Q_OBJECT    
public:
  SBTestDiurnEOP(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestDiurnEOP();
  
  virtual QString ClassName() const {return "SBTestDiurnEOP";};
  
  private 
  slots:
  virtual void recalc();

private:
  SBTime		*Time;
  // available theoretical models:
  SBOTideEOPGeneral	*deopRay1994;
  SBOTideEOPGeneral	*deopRay2003;
  SBOTideEOPGeneral	*deopJPL92;
  SBOTideEOPGeneral	*deopHD1994;
  SBOTideEOPGeneral	*deopGSFC95;
  SBOTideEOPGeneral	*deopGipson1995;
  //aux:
  SBOTideEOPGeneral	*deopAtm;
  SBOTideEOPGeneral	*deopNonTide;
  SBOTideEOPGeneral	*deopGrav;
  
  SBOTideEOPGeneral	*dEOPs[6+3];

  unsigned int		NumOfModels;
  unsigned int		NumOfPts;
  double		Step;

  QLineEdit		*ENumPts;
  QLineEdit		*EStep;


  SBPlotCarrier		*PlotCarrier;
  SBPlot		*Plot;

  void			fillData4Plotting();


  virtual QWidget	*createWidget4Test(QWidget*);
  virtual QWidget	*auxWidgetRt(QWidget*);
};
/*==============================================================================================*/


class SBSolidTideLd;
class SB_TRF;
class SBStation;
/*==============================================================================================*/
/*												*/
/* SBTestStationLI..										*/
/*												*/
/*==============================================================================================*/
class SBTestStationLI : public QListViewItem
{
private:
  SBStation	*Station;
public:
  SBTestStationLI(QListView* parent_, SBStation* Station_) 
    : QListViewItem(parent_) {Station = Station_;};
  ~SBTestStationLI(){};
  virtual QString ClassName() const {return "SBTestStationLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  SBStation*	station() {return Station;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestSolidTides..										*/
/*												*/
/*==============================================================================================*/
class SBTestSolidTides : public SBTest
{
  Q_OBJECT    
public:
  SBTestSolidTides(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestSolidTides();
  
  virtual QString ClassName() const {return "SBTestSolidTides";};
  
  private 
  slots:
  virtual void		recalc();
  virtual void		stationChange(QListViewItem*);

private:
  SBFrame		*Frame;
  SBEphem		*Ephem;
  SBSolidTideLd		*SolidTide;
  SB_TRF		*TRF;
  SBStation		*Station;
  Vector3		Vectors[7];
  QString		VectorNames[7];

  unsigned int		NumOfPts;
  unsigned int		NumOfVecotors;
  double		Step;


  // widgets:
  QLineEdit		*ENumPts;
  QLineEdit		*EStep;

  QListView		*StLV;


  SBPlotCarrier		*PlotCarrier;
  SBPlot		*Plot;

  void			fillData4Plotting();

  virtual QWidget	*createWidget4Test(QWidget*);
  virtual QWidget	*auxWidgetRt(QWidget*);
};
/*==============================================================================================*/


class SBOceanTideLd;
/*==============================================================================================*/
/*												*/
/* SBTestOceanTides..										*/
/*												*/
/*==============================================================================================*/
class SBTestOceanTides : public SBTest
{
  Q_OBJECT    
public:
  SBTestOceanTides(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestOceanTides();
  
  virtual QString ClassName() const {return "SBTestOceanTides";};
  
  private 
  slots:
  virtual void		recalc();
  virtual void		stationChange(QListViewItem*);

private:
  SBFrame		*Frame;
  SBOceanTideLd		*OceanTide;
  SB_TRF		*TRF;
  SBStation		*Station;
  Vector3		Vectors[1];
  QString		VectorNames[1];

  unsigned int		NumOfPts;
  unsigned int		NumOfVecotors;
  double		Step;


  // widgets:
  QLineEdit		*ENumPts;
  QLineEdit		*EStep;

  QListView		*StLV;


  SBPlotCarrier		*PlotCarrier;
  SBPlot		*Plot;

  void			fillData4Plotting();

  virtual QWidget	*createWidget4Test(QWidget*);
  virtual QWidget	*auxWidgetRt(QWidget*);
};
/*==============================================================================================*/



class SBPolarTideLd;
/*==============================================================================================*/
/*												*/
/* SBTestPolarTides..										*/
/*												*/
/*==============================================================================================*/
class SBTestPolarTides : public SBTest
{
  Q_OBJECT    
public:
  SBTestPolarTides(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestPolarTides();
  
  virtual QString ClassName() const {return "SBTestPolarTides";};
  
  private 
  slots:
  virtual void		recalc();
  virtual void		stationChange(QListViewItem*);

private:
  SBFrame		*Frame;
  SBPolarTideLd		*PolarTide;
  SB_TRF		*TRF;
  SBStation		*Station;
  Vector3		Vectors[1];
  QString		VectorNames[1];

  unsigned int		NumOfPts;
  unsigned int		NumOfVecotors;
  double		Step;


  // widgets:
  QLineEdit		*ENumPts;
  QLineEdit		*EStep;

  QListView		*StLV;


  SBPlotCarrier		*PlotCarrier;
  SBPlot		*Plot;

  void			fillData4Plotting();

  virtual QWidget	*createWidget4Test(QWidget*);
  virtual QWidget	*auxWidgetRt(QWidget*);
};
/*==============================================================================================*/



class SBAploEphem;
/*==============================================================================================*/
/*												*/
/* SBTestPolarTides..										*/
/*												*/
/*==============================================================================================*/
class SBTestAPLoad : public SBTest
{
  Q_OBJECT    
public:
  SBTestAPLoad(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBTestAPLoad();
  
  virtual QString ClassName() const {return "SBTestAPLoad";};
  
  private 
  slots:
  virtual void		recalc();
  virtual void		stationChange(QListViewItem*);

private:
  //  SBFrame		*Frame;
  SBAploEphem		*AploEphem;
  SB_TRF		*TRF;
  SBStation		*Station;

  unsigned int		NumOfPts;
  double		Step;


  // widgets:
  QLineEdit		*ENumPts;
  QLineEdit		*EStep;

  QListView		*StLV;


  SBPlotCarrier		*PlotCarrier;
  SBPlot		*Plot;

  void			fillData4Plotting();

  virtual QWidget	*createWidget4Test(QWidget*);
  virtual QWidget	*auxWidgetRt(QWidget*);
};
/*==============================================================================================*/




#endif //SB_I_TEST_H
