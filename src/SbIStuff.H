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

#ifndef SB_I_STUFF_H
#define SB_I_STUFF_H

#include <config.h>
#include <qdialog.h>
#include <qlistview.h>

#include "SbGeneral.H"
#include "SbInterface.H"
#include "SbGeoEop.H"





class QCheckBox;
class QComboBox;
class QLineEdit;
class QBoxLayout;
class QLabel;
class QRadioButton;
class QListBox;
/*==============================================================================================*/
/*												*/
/* SBEOPListItem..										*/
/*												*/
/*==============================================================================================*/
class SBEOPListItem : public QListViewItem
{
private:
  SBEOPEntry	*EOPEntry;
public:
  SBEOPListItem(QListView*, SBEOPEntry*);
  virtual QString ClassName() const {return "SBEOPListItem";};
  virtual QString text(int) const;
  QString key(int, bool) const;
  friend class SBStuffEOPs;
};
/*==============================================================================================*/


class SBPlotCarrier;
/*==============================================================================================*/
/*												*/
/* SBStuffEOPs..										*/
/*												*/
/*==============================================================================================*/
class SBStuffEOPs : public QDialog
{
  Q_OBJECT    
public:
  SBStuffEOPs(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBStuffEOPs();
  
  virtual QString ClassName() const {return "SBStuffEOPs";};
  
  private 
  slots:
  void modified(bool);
  void childWindowClosed();
  void accept();
  void reject();
  void import();
  void draw();
  void showSeries();
  void updateEOP();
  
protected:
  virtual void		closeEvent(QCloseEvent *){delete this;};
  
private:
  SBEOP		*Weop;
  QString	str;
  
  QListView	*EOPsLV;
  QLabel	*EOPTStart;
  QLabel	*EOPTFinis;
  QLabel	*EOPNum;
  QLineEdit	*EOPEpoch;
  QLineEdit	*EOPLabel;
  
  QCheckBox	*EOPHiTide;
  QCheckBox	*EOPLoTide;
  
  /* transpars 4 consistency: */
  QLineEdit	*EOPXp;
  QLineEdit	*EOPXps;
  QLineEdit	*EOPYp;
  QLineEdit	*EOPYps;
  QLineEdit	*EOPUT;
  QLineEdit	*EOPUTs;
  QLineEdit	*EOPPsi;
  QLineEdit	*EOPPsis;
  QLineEdit	*EOPEps;
  QLineEdit	*EOPEpss;

  bool		isModified;
  int		numOpenedWindows;
  SBPlotCarrier	*PlotCarrier;
  
  QWidget*	createInfoGroup(QWidget*);
  QWidget*	createOptionsGroup(QWidget*);
};
/*==============================================================================================*/



class SBEphem;
class SBConfig;
/*==============================================================================================*/
/*												*/
/* SBStuffEphem..										*/
/*												*/
/*==============================================================================================*/
class SBStuffEphem : public QDialog
{
  Q_OBJECT    
public:
  SBStuffEphem(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual QString ClassName() const {return "SBStuffEphem";};
  ~SBStuffEphem();
  
  signals:
  void stBarReset();
  void stBarSetTotalSteps(int);
  void stBarSetProgress(int);
  void message(const QString&);
  
  private 
  slots:
  void import();
  void updateEphem();
  void accept();
  void reject();
  
protected:
  virtual void		closeEvent(QCloseEvent *){delete this;};
  
private:
  SBEphem	*Weph;
  QString	str;
  
  QLabel	*LStatus;
  QLabel	*LTStart;
  QLabel	*LTFinis;
  QLabel	*LDt;
  QLabel	*LNCoef;
  QLabel	*LNRecs;
  QLabel	*LAU;
  QLabel	*LCLight;
  QLabel	*LEMRat;
  
  QLabel	*LBodyMass[11];
};
/*==============================================================================================*/


class SBAploHeader;
/*==============================================================================================*/
/*												*/
/* SBAploHeaderLI..										*/
/*												*/
/*==============================================================================================*/
class SBAploHeaderLI : public QListViewItem
{
private:
  SBAploHeader		*Header;
public:
  SBAploHeaderLI(QListView* parent, SBAploHeader* H_) : QListViewItem(parent){Header=H_;};
  virtual ~SBAploHeaderLI(){};
  virtual QString	ClassName() const {return "SBAploHeaderLI";};
  virtual QString	text(int) const;
  QString		key(int, bool) const;
  SBAploHeader*		header() {return Header;};
};
/*==============================================================================================*/



class SBAploEntry;
/*==============================================================================================*/
/*												*/
/* SBAploHeaderLI..										*/
/*												*/
/*==============================================================================================*/
class SBAploEntryLI : public QListViewItem
{
private:
  SBAploEntry		*Entry;
public:
  SBAploEntryLI(QListView* parent, SBAploEntry* E_) : QListViewItem(parent){Entry=E_;};
  virtual ~SBAploEntryLI(){};
  virtual QString	ClassName() const {return "SBAploEntry";};
  virtual QString	text(int) const;
  QString		key(int, bool) const;
  SBAploEntry*		entry() {return Entry;};
};
/*==============================================================================================*/



class SBAploEphem;
class SBAploChunk;
/*==============================================================================================*/
/*												*/
/* SBStuffAplo..										*/
/*												*/
/*==============================================================================================*/
class SBStuffAplo : public QDialog
{
  Q_OBJECT    
public:
  SBStuffAplo(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual QString ClassName() const {return "SBStuffAplo";};
  ~SBStuffAplo();
  
  signals:
  void stBarReset();
  void stBarSetTotalSteps(int);
  void stBarSetProgress(int);
  void message(const QString&);
  
  private 
  slots:
  void import();
  void importHPS();
  void updateEphem();
  void accept();
  void reject();
  void headerChanged(QListViewItem*);
  void draw();
  
protected:
  virtual void		closeEvent(QCloseEvent *){delete this;};
  void			blockInput();
  void			restoreInput();

  
private:
  SBAploEphem		*AploEphem;
  SBAploChunk		*Chunk;
  bool			isModified;
  bool			isTRFModified;


  QListView		*HeadersLV;
  QListView		*EntriesLV;
  
  QLabel		*LStatus;
  QLabel		*LTStart;
  QLabel		*LTFinis;
  QLabel		*LNumChunks;

  QPushButton		*Ok;
  QPushButton		*Draw;
  QPushButton		*Import;
  QPushButton		*ImportHPS;
  QPushButton		*Cancel;

  bool			IsBlockedOkButton;
  bool			IsBlockedDrawButton;
  bool			IsBlockedImportButton;
  bool			IsBlockedImportHPSButton;
  bool			IsBlockedCancelButton;

  //  SBPlotCarrier		*PlotCarrier;
};
/*==============================================================================================*/



#endif //SB_I_STUFF_H
