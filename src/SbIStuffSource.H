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
 * Overview:	GUI for handling RSC (source catalogue) and source data;
 *
 */

#ifndef SB_I_STUFF_SOURCE_H
#define SB_I_STUFF_SOURCE_H
 

#include <config.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qtabdialog.h>
#include <qwizard.h>


#include "SbGeneral.H"
#include "SbInterface.H"
#include "SbGeoSources.H"

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QBoxLayout;
class QLabel;
class QRadioButton;



/*==============================================================================================*/
/*												*/
/* SBSourceListItem..										*/
/*												*/
/*==============================================================================================*/
class SBSourceListItem : public QListViewItem
{
private:
  SBSource	*Source;
  bool		isFaked;
public:
  SBSourceListItem(QListView*, SBSource* =0);
  virtual ~SBSourceListItem();
  virtual QString ClassName() const {return "SBSourceListItem";};
  virtual QString text(int) const;
  QString key(int, bool) const;
  const QPixmap * pixmap (int) const;
  friend class SBStuffSources;
  friend class SBSourceEditor;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBAliasListItem..										*/
/*												*/
/*==============================================================================================*/
class SBAliasListItem : public QListViewItem
{
private:
  QString	Alias;
  QString	Reference;
  bool		isFaked;
public:
  SBAliasListItem(QListView*, const char* =0, const char* =0);
  virtual QString ClassName() const {return "SBAliasListItem";};
  virtual QString text(int) const;
  friend class SBStuffSources;
  friend class SBStuffStations;
  friend class SBAliasEditor;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffSources..										*/
/*												*/
/*==============================================================================================*/
class SBStuffSources : public QDialog
{
  Q_OBJECT    
public:
  SBStuffSources(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBStuffSources();
  
  virtual QString ClassName() const {return "SBStuffSources";};
  
  private 
  slots:
  void importData();
  void exportData();
  void accept();
  void reject();
  void reloadCRF();
  void editEntryA(QListViewItem*);
  void insertEntryA();
  void deleteEntryA();
  void editEntryS(QListViewItem*);
  void insertEntryS();
  void deleteEntryS();
  
  void CRFModified(bool);
  
private:
  SB_CRF	*Wcrf;
  QString	str;
  
  
  QListView	*SourcesLV;
  QListView	*AliasesLV;
  QLineEdit	*CRFEpoch;
  QLineEdit	*CRFLabel;
  QLineEdit	*CRFComment;
  QLabel	*NumSA;
  bool		isModifiedCRF;
  int		numOpenedWindows;
  
  void drawInfo();
  
protected:
  virtual void closeEvent(QCloseEvent *e) {if(numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSourceImport..										*/
/*												*/
/*==============================================================================================*/
class SBSourceImport : public QWizard
{
  Q_OBJECT    
public:
  SBSourceImport(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual QString	ClassName() const {return "SBSourceImport";};
  
private:
  enum		FileFormat  {F_UNKN=0, F_ICRF95=1, F_ICRF99=2, F_ICRF2009=3};
  enum		FileType    {T_RSC=0, T_CAR=1, T_DICO=2};
  enum		ImportMode  {REPLACE=0, APP_ONLY=1, APP_UPD=2, UPD_ONLY=3, UPD_COO_ONLY=4};
  
  signals:
  void sourcesImported();
  
  private 
  slots:
  void		accept();
  void		formatChanged(int);
  void		modeChanged(int);

  void selectRSCfile1995 ()	{selectFile1995(T_RSC);};
  void selectCARfile1995 ()	{selectFile1995(T_CAR);};
  void selectDICOfile1995()    	{selectFile1995(T_DICO);};

  void setRSCstate1995 (bool T)	{setState1995(T_RSC, T);};
  void setCARstate1995 (bool T)	{setState1995(T_CAR, T);};
  void setDICOstate1995(bool T)	{setState1995(T_DICO,T);};

  void fnRSCChanged1995 (const QString& FN_){fileNameChanged1995(T_RSC,  FN_);};
  void fnCARChanged1995 (const QString& FN_){fileNameChanged1995(T_CAR,  FN_);};
  void fnDICOChanged1995(const QString& FN_){fileNameChanged1995(T_DICO, FN_);};


  void selectRSCDfile1999()	{selectFile1999(0);};
  void selectRSCCfile1999()	{selectFile1999(1);};
  void selectRSCOfile1999()	{selectFile1999(2);};
  void selectRSCNfile1999()	{selectFile1999(3);};
  void selectCARfile1999 ()	{selectFile1999(4);};
  void selectDICOfile1999()    	{selectFile1999(5);};

  void setRSCDstate1999(bool T)	{setState1999(0, T);};
  void setRSCCstate1999(bool T)	{setState1999(1, T);};
  void setRSCOstate1999(bool T)	{setState1999(2, T);};
  void setRSCNstate1999(bool T)	{setState1999(3, T);};
  void setCARstate1999 (bool T)	{setState1999(4, T);};
  void setDICOstate1999(bool T)	{setState1999(5, T);};

  void fnRSCDChanged1999(const QString& FN_){fileNameChanged1999(0, FN_);};
  void fnRSCCChanged1999(const QString& FN_){fileNameChanged1999(1, FN_);};
  void fnRSCOChanged1999(const QString& FN_){fileNameChanged1999(2, FN_);};
  void fnRSCNChanged1999(const QString& FN_){fileNameChanged1999(3, FN_);};
  void fnCARChanged1999 (const QString& FN_){fileNameChanged1999(4, FN_);};
  void fnDICOChanged1999(const QString& FN_){fileNameChanged1999(5, FN_);};

  // ICRF2:
  void selectRSCNonVCSFile2009(){selectFile2009(0);};
  void selectRSCVCSFile2009()	{selectFile2009(1);};
  void selectCARFile2009 ()	{selectFile2009(2);};
  void selectDICOFile2009()    	{selectFile2009(3);};

  void setRSCNonVCSState2009(bool T)	{setState2009(0, T);};
  void setRSCVCSState2009(bool T)	{setState2009(1, T);};
  void setCARState2009 (bool T)		{setState2009(2, T);};
  void setDICOState2009(bool T)		{setState2009(3, T);};

  void fnRSCNonVCSChanged2009(const QString& FN_){fileNameChanged2009(0, FN_);};
  void fnRSCVCSChanged2009   (const QString& FN_){fileNameChanged2009(1, FN_);};
  void fnCARChanged2009      (const QString& FN_){fileNameChanged2009(2, FN_);};
  void fnDICOChanged2009     (const QString& FN_){fileNameChanged2009(3, FN_);};


protected:
  virtual void	closeEvent(QCloseEvent *){delete this;};

  void		setupPage1();
  void		setupPage2_1();
  void		setupPage2_2();
  void		setupPage2_3();
  void		setupPage3();
  bool		checkFileExists(const QString&);
    
  
private:
  FileFormat	FFormat;

  QWidget	*page1;
  QWidget	*page2_1;
  QWidget	*page2_2;
  QWidget	*page2_3;
  QWidget	*page3;

  // format icrf95:
  QString	sFileName1995[3];
  QCheckBox	*cbFileName1995[3];
  QLineEdit	*leFileName1995[3];
  QPushButton	*bFile1995[3];
  void		checkPage2_1State();

  // format icrf99:
  QString	sFileName1999[6];
  QCheckBox	*cbFileName1999[6];
  QLineEdit	*leFileName1999[6];
  QPushButton	*bFile1999[6];
  void		checkPage2_2State();

  // format icrf2009:
  QString	sFileName2009[4];
  QCheckBox	*cbFileName2009[4];
  QLineEdit	*leFileName2009[4];
  QPushButton	*bFile2009[4];
  void		checkPage2_3State();



  //
  ImportMode	Mode;

  void		selectFile1995(FileType);
  void		setState1995(FileType, bool);
  void		fileNameChanged1995(FileType, const QString&);

  void		selectFile1999(int);
  void		setState1999(int, bool);
  void		fileNameChanged1999(int, const QString&);

  void		selectFile2009(int);
  void		setState2009(int, bool);
  void		fileNameChanged2009(int, const QString&);

  void		importICRF();

  void		loadFile1995(SB_CRF*, QString, FileType, SBSource::SStatus=SBSource::STATUS_UNKN);
  void		loadFile1999(SB_CRF*, QString, FileType);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSourceEditor..										*/
/*												*/
/*==============================================================================================*/
class SBSourceEditor : public SBTabDialog
{
  Q_OBJECT    
public:
  SBSourceEditor(SBSourceListItem*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  SBSourceEditor(QListView*, SB_CRF*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBSourceEditor();
  virtual QString ClassName() const {return "SBSourceEditor";};
  
 signals:
  void SourceModified(bool);
  
  protected
    slots:
  virtual void makeApply();
  
 private:
  virtual void		acquireData();
  void			init();
  QWidget*		wCoordinates();
  QWidget*		wStats();
  QWidget*		wPhysChars();

  QListView*		SourcesLV;
  SBSourceListItem*	SrcItem;
  SBSource*		Source;
  SB_CRF*		Wcrf;
  bool			isModified;
  bool			isNew;

  /**/
  QLineEdit*		EName;
  QLineEdit*		EICRF;
  QComboBox*		CBAlias;
  QGroupBox*		NameGBox;
  
  /**/
  QLineEdit*		ERAhr;
  QLineEdit*		ERAmin;
  QLineEdit*		ERAsec;
  QLineEdit*		ERAunc;
  QLineEdit*		EDNdg;
  QLineEdit*		EDNmin;
  QLineEdit*		EDNsec;
  QLineEdit*		EDNunc;
  QLineEdit*		ECorr;
  QCheckBox*		CBHipp;
  QRadioButton*		RBStatus[5];
  
  /**/
  QLineEdit*		ENofObs;
  QLineEdit*		ENofSes;
  QLineEdit*		EFMJD;
  QLineEdit*		EMMJD;
  QLineEdit*		ELMJD;
  QCheckBox*		CDOrigin[4];
  QCheckBox*		CDAttr [13];
  
  /**/
  QRadioButton*		RBType[6];
  QRadioButton*		RBSpClass[8];
  QLineEdit*		EMv;
  QLineEdit*		EFlux1;
  QLineEdit*		EFlux2;
  QLineEdit*		ESpIdx;
  QLineEdit*		ERedShift;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBAliasEditor..										*/
/*												*/
/*==============================================================================================*/
class SBAliasEditor : public QDialog
{
  Q_OBJECT    
    public:
  SBAliasEditor(SBAliasListItem*, SBCatalog*, QWidget* =0, 
		const char* =0, bool =FALSE, WFlags =0);
  SBAliasEditor(QListView*, SBCatalog*, QWidget* =0, 
		const char* =0, bool =FALSE, WFlags =0);
  ~SBAliasEditor();
  virtual QString ClassName() const {return "SBAliasEditor";};
  
 signals:
  void AliasModified(bool);
  
  protected
    slots:
  void accept();
  void reject();
  
 protected:
  virtual void		closeEvent(QCloseEvent *){delete this;};

 private:
  virtual void		acquireData();
  void			init();
  
  QListView*		AliasesLV;
  SBAliasListItem*	AkaItem;

  bool			isModified;
  bool			isNew;
  QString		Alias;
  QString		Reference;
  QString		OldReference;
  SBCatalog*		Wcrf;

  /**/
  QLineEdit*		EAlias;
  QComboBox*		CBReference;
};
/*==============================================================================================*/


 




#endif //SB_I_STUFF_SOURCE_H
