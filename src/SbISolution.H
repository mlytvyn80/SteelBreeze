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

#ifndef SB_I_SOLUTION_H
#define SB_I_SOLUTION_H


#include <config.h>


#include "SbGeneral.H"
#include "SbGeoParameter.H"
#include "SbGeoObsVLBI.H"
#include "SbInterface.H"



/*==============================================================================================*/
/*												*/
/* SBParameterLI..										*/
/*												*/
/*==============================================================================================*/
class SBParameterLI : public QListViewItem
{
private:
  SBParameter		*P;

public:
  SBParameterLI(QListView* parent, SBParameter* P_) : QListViewItem(parent){P=P_?P_:NULL;};
  SBParameter* parameter(){return P;};
  virtual QString ClassName() const {return "SBParameterLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSolutionBatchLI..										*/
/*												*/
/*==============================================================================================*/
class SBSolutionBatchLI : public QListViewItem
{
private:
  SBStatistics		*S;
  double		Scale;
  
public:
  SBSolutionBatchLI(QListView* parent, SBStatistics* S_, double Scale_) 
    : QListViewItem(parent){S=S_?S_:NULL; Scale=Scale_;};
  SBStatistics* statistics(){return S;};
  virtual QString ClassName() const {return "SBSolutionBatchLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
};
/*==============================================================================================*/






class QLabel;

class SBPlotCarrier;
class SBPlot;
class SBProject;
class SBSolution;
/*==============================================================================================*/
/*												*/
/* SBSolutionBrowser..										*/
/*												*/
/*==============================================================================================*/
class SBSolutionBrowser : public SBTabDialog
{
  Q_OBJECT    
public:
  SBSolutionBrowser(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBSolutionBrowser();
  virtual QString ClassName() const {return "SBSolutionBrowser";};
  
  protected
  slots:
  void			modified(bool);
  void			batch4StochStChanged(QListViewItem*);
  void			batch4StochSoChanged(QListViewItem*);
  void			batch4StochEOPChanged(QListViewItem*);
  void			dumpSolution();
  void			updateCRF();
  void			updateTRF();
  
private:
  bool			isModified;
  SBProject		*Prj;
  SBSolution		*Solution;


  //-----
  /* functions */
  QWidget		*wGeneralInfo();
  QWidget		*wWRMSs();
  QWidget		*wGlobalPars();
  QWidget		*wLocalStPars();
  QWidget		*wLocalSoPars();
  QWidget		*wLocalEOPPars();
  QWidget		*wStochStPars();
  QWidget		*wStochSoPars();
  QWidget		*wStochEOPPars();

  void			lookupParameters();

  // General info:
  // General statistics:
  QLabel		*lGenVarF;
  QLabel		*lGenWRMS;
  QLabel		*lGenNumObs;
  QLabel		*lGenNumPars;
  QListView		*BatchesLV;



  // Global parameters:
  bool			IsGlobalPars;
  QListView		*GlobalParsLV;
  SBPlotCarrier		*PC_SessRMS;



  // Local parameters:
  // 4stations:
  bool			IsLocalSta;
  SBStochParameterList	LocalStPars;	// keeps a list of conventional parameters
  SBPlotCarrier		*PC_LocalStPars;
  QList<QString>	LocStationParameters;
  SBStationInfoList	LocStations;

  // 4sources:
  bool			IsLocalSou;
  SBStochParameterList	LocalSoPars;	// keeps a list of conventional parameters
  SBPlotCarrier		*PC_LocalSoPars;
  QList<QString>	LocSourceParameters;
  SBSourceInfoList	LocSources;

  // 4EOP:
  bool			IsLocalEOP;
  SBStochParameterList	LocalEOPPars;	// keeps a list of conventional parameters
  SBPlotCarrier		*PC_LocalEOPPars;



  // Stochastic parameters:
  // 4stations:
  bool			IsStochSta;
  SBStochParameterList	StochStPars;
  SBPlotCarrier		*StochasticsStationsPC;
  SBPlot		*StochasticsStationsPlot;

  // 4sources:
  bool			IsStochSou;
  SBStochParameterList	StochSoPars;
  SBPlotCarrier		*StochasticsSourcesPC;
  SBPlot		*StochasticsSourcesPlot;


  // 4EOP:
  bool			IsStochEOP;
  SBStochParameterList	StochEOPPars;
  SBPlotCarrier		*StochasticsEOPsPC;
  SBPlot		*StochasticsEOPsPlot;





  /* 4plotting:*/
  double		Scale;
  QString		ScaleName;
};
/*==============================================================================================*/



 


#endif //SB_I_SOLUTION_H
