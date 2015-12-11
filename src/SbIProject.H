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

#ifndef SB_I_PROJECT_H
#define SB_I_PROJECT_H


#include <config.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qtabdialog.h>


#include "SbGeoProject.H"
#include "SbGeoParameter.H"
#include "SbIObsVLBI.H"


/*==============================================================================================*/
/*												*/
/* SBProjectCreate..										*/
/*												*/
/*==============================================================================================*/
class SBProjectCreate : public SBVLBISetViewBase
{
  Q_OBJECT
public:
  SBProjectCreate(SBProject* =NULL, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBProjectCreate(){};
  
  virtual QString ClassName() const {return "SBProjectCreate";};
  
  signals:
  void stBarReset();
  void stBarSetTotalSteps(int);
  void stBarSetProgress(int);
  void message(const QString&);
  void sessionsAdded();

  private 
  slots:
  
  void selChanged(QListViewItem*, bool);
  void editEntry(QListViewItem*);
  void createProject();
  void selectAll();
  void unselectAll();
  
private:
  SBProject		*Project;

  int			NumObsSel;
  int			NumSessSel;
  int			NumPPSessSel;
  bool			IsProcessing;

  QLineEdit		*EPrjName;

  virtual void		init();
  virtual void		drawInfo();
  virtual QWidget	*optionsGroup(QWidget*);
};
/*==============================================================================================*/


class SBVLBISet;
class SBProject;
class QMultiLineEdit;
/*==============================================================================================*/
/*												*/
/* SBProjectEdit..										*/
/*												*/
/*==============================================================================================*/
class SBProjectEdit : public SBTabDialog
{
  Q_OBJECT
public:
  SBProjectEdit(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBProjectEdit();
  
  virtual QString ClassName() const {return "SBProjectEdit";};
  
  signals:
  void stBarReset();
  void stBarSetTotalSteps(int);
  void stBarSetProgress(int);
  void message(const QString&);
  

  private 
  slots:
  void modified(bool);
  void commentsChanged();
  void childWindowClosed();

  virtual void done(int);
  virtual void makeApply();

  virtual void delSession();
  virtual void addSession();
  
  void sessionListChanged();

  void editEntry(QListViewItem*);
  void toggleEntryMarkEnable(QListViewItem*, const QPoint&, int);
  void histModified(bool B){if (B) isModified=TRUE;};
  void histVerInced();
  
private:
  virtual void		acquireData();

  SBVLBISet		*Wvlbi;
  SBProject		*Project;
  bool			isModified;
  bool			isCommentsChanged;
  int			numOpenedWindows;
  QString		Str;

  QWidget		*comments();
  QMultiLineEdit	*mleComments;
  SBBrowseSessions	*brwsSes;
  SBBrowseStations	*brwsSta;
  SBBrowseSources	*brwsSou;
  SBBrowseBases		*brwsBas;
  SBBrowseHistory	*brwsHist;

  SB_CRF		*CRF;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBProjectSel..										*/
/*												*/
/*==============================================================================================*/
class SBProjectSel : public QDialog
{
  Q_OBJECT
public:
  SBProjectSel(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBProjectSel(){};
  
  virtual QString ClassName() const {return "SBProjectSel";};
  
  private 
  slots:
  void accept();
  void reject(){QDialog::reject(); delete this;};
  void remove();
  
private:
  class ProjectListItem : public QListViewItem
  {
  private:
    QString		Name;
    SBMJD		TStart;
    SBMJD		TFinis;
    double		Interval;
    int			NumObs;
    int			NumSes;
    int			NumSta;
    int			NumBas;
    int			NumSou;
    int			Ver;
  public:
    ProjectListItem(QListView*, SBProject*);
    virtual QString text(int) const;
    virtual QString	key(int, bool) const;
    const QString&	name(){return Name;};
  };

  QString		Str;
  QListView		*lvProject;
protected:
  virtual void closeEvent(QCloseEvent*){delete this;};
};
/*==============================================================================================*/



class SBConfig;
class QRadioButton;
class QCheckBox;
class QLineEdit;
class QGroupBox;
class QButtonGroup;
/*==============================================================================================*/
/*												*/
/* SBModelEditor..										*/
/*												*/
/*==============================================================================================*/
class SBModelEditor : public SBTabDialog
{
  Q_OBJECT    
public:
  SBModelEditor(SBConfig*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBModelEditor();
  virtual QString ClassName() const {return "SBModelEditor";};
  
  protected
  slots:
  virtual void	makeApply();
  virtual void	makeDefault();
  void		solidTideIERS92(bool);
  
private:
  SBConfig		*Cfg;
  SBProject		*Project;
  bool			isModified;
  bool			isGlobal;
  QString		Str;
    
  /* functions */
  virtual void		acquireData();
  
  /* System transformation: */
  QWidget		*wSysTransform();
  QRadioButton		*rbSysTransform[2];
  /* Alternative DB: */
  QCheckBox		*cbUseAltStations;
  QCheckBox		*cbUseAltSources;
  QCheckBox		*cbUseAltEOPs;
  QLineEdit		*leAltStations;
  QLineEdit		*leAltSources;
  QLineEdit		*leAltEOPs;


  /* Light Propagation: */
  QWidget		*wLightPropagation();
  QRadioButton		*rbEphem[3];
  QCheckBox		*cbIsHO;
  QLineEdit		*leHOThreshold;
  QCheckBox		*cbBody[11];

  /* Solid tide: */
  QWidget		*wSolidTide();
  QGroupBox		*gbSTIERS96;
  QButtonGroup		*bgSTOrder;
  QRadioButton		*rbSTide[2];
  QCheckBox		*cbPTide;
  QRadioButton		*rbDegree[2];
  QCheckBox		*cbAnelastic;
  QCheckBox		*cbLatDepend;

  /* EOPs: */
  QWidget		*wEOPs();
  QRadioButton		*rbEOPDiurn[6];
  QRadioButton		*rbEOPDiurnAtm[2];
  QRadioButton		*rbEOPDiurnNonTidal[2];
  QLineEdit		*leEOPNInterp;
  QCheckBox		*cbEOP_IAU2K_XY;
  QCheckBox		*cbEOPResrerved_1;

  /* Nutation: */
  QWidget		*wNutation();
  QRadioButton		*rbNutModel[3];
  QRadioButton		*rbNutEquinox[3];
  QCheckBox		*cbNutGeod;

  /* Station Motion: */
  QWidget		*wPlateMotion();
  QRadioButton		*rbTPMModel[3];
  QCheckBox		*cbStatVel;
  QCheckBox		*cbRB2Plates;
  QCheckBox		*cbAplo;

  /* Refraction: */
  QWidget		*wRefraction();
  QRadioButton		*rbDryZenith[3];
  QRadioButton		*rbWetZenith[3];
  QRadioButton		*rbDryMap[6];
  QRadioButton		*rbWetMap[6];
  QCheckBox		*cbAxissAlt;
  QCheckBox		*cbRB2NMF;

  //Data processing:
  QWidget		*wDataProcessing();
  QLineEdit		*eDelayQThreshold;
  QLineEdit		*eRateQThreshold;
  QCheckBox		*cbUseDelay;
  QCheckBox		*cbUseRate;
  QCheckBox		*cbUseBadIon;
  QCheckBox		*cbGenerateDSINEXes;


  //Solution reports:
  QWidget		*wSolutionReports();
  QCheckBox		*cbReportMaps;
  QCheckBox		*cbReportTRF_Changes;
  QCheckBox		*cbReportCRF_Changes;
  QCheckBox		*cbReportCRF;
  QCheckBox		*cbReportTRF_SINEX;
  QCheckBox		*cbReportEOP;
  QCheckBox		*cbReportTechDesc;
  QCheckBox		*cbReportNormalEqs;

  QCheckBox		*cbReportNormalEqsSta;
  QCheckBox		*cbReportNormalEqsSou;
  QCheckBox		*cbReportNormalEqsEOP;

  QCheckBox		*cbReportTroposhere;
  QLineEdit		*leTropSamplInterval;
  QLineEdit		*leTropStartSec;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBParametersEditor..										*/
/*												*/
/*==============================================================================================*/
class SBParametersEditor : public SBTabDialog
{
  Q_OBJECT    
public:
  SBParametersEditor(SBParametersDesc* =NULL, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBParametersEditor();
  virtual QString ClassName() const {return "SBParametersEditor";};
  
  protected
  slots:
  virtual void makeDefault();
  virtual void makeApply();
  //  void accept();
  //  void reject(){if(numOpenedWindows) return; QDialog::reject(); delete this;};
  void ParameterModified(bool);

  void editParameter(int);
  void editPar__0() {emit editParameter( 0);};
  void editPar__1() {emit editParameter( 1);};
  void editPar__2() {emit editParameter( 2);};
  void editPar__3() {emit editParameter( 3);};
  void editPar__4() {emit editParameter( 4);};
  void editPar__5() {emit editParameter( 5);};
  void editPar__6() {emit editParameter( 6);};
  void editPar__7() {emit editParameter( 7);};
  void editPar__8() {emit editParameter( 8);};
  void editPar__9() {emit editParameter( 9);};
  void editPar_10() {emit editParameter(10);};
  void editPar_11() {emit editParameter(11);};
  void editPar_12() {emit editParameter(12);};
  void editPar_13() {emit editParameter(13);};
  void editPar_14() {emit editParameter(14);};
  void editPar_15() {emit editParameter(15);};
  void editPar_16() {emit editParameter(16);};
  void editPar_17() {emit editParameter(17);};

  
private:
  SBParametersDesc	*P;
  SBProject		*Project;
  bool			isModified;
  bool			isGlobal;
  int			numOpenedWindows;

  QRadioButton		*RB[NUMBER_OF_PARAMETERS][5];

  // stations:
  QWidget		*wStationParameters();

  // sources:
  QWidget		*wSourceParameters();

  // EOPs:
  QWidget		*wEOPParameters();

  // other:
  QWidget		*wOtherParameters();

  // tests:
  QWidget		*wTestParameters();


  
  /* functions */
  virtual void		acquireData();
  virtual void		browseData();


protected:
  virtual void closeEvent(QCloseEvent *e){if(numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBParEditor..										*/
/*												*/
/*==============================================================================================*/
class SBParEditor : public QDialog
{
  Q_OBJECT    
public:
  SBParEditor(SBParameterCfg*, const QString&, int,
	      QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBParEditor();
  virtual QString ClassName() const {return "SBParEditor";};
  
  signals:
  void valueModified(bool);
  
  protected
  slots:
  void makeDefault();
  void accept();
  void reject(){QDialog::reject(); emit valueModified(FALSE); delete this;};
  
private:
  SBParameterCfg*	P;
  bool			isModified;
  bool			isConstrained;
  int			Idx;
  QString		ParName;

  QLineEdit*		ConvApriori;
  QLineEdit*		StocApriori;
  QLineEdit*		RuledWNoise;
  QLineEdit*		BreaksNoise;
  QLineEdit*		Tau;
  QCheckBox*		Propagate;
  QRadioButton*		Type[5];
  

  /* functions */
  virtual void		acquireData();
  virtual void		browseData();
};
/*==============================================================================================*/




#endif //SB_I_PROJECT_H
