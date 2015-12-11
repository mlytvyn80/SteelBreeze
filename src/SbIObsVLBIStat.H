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

#ifndef SB_I_OBS_VLBI_STAT_H
#define SB_I_OBS_VLBI_STAT_H


#include <config.h>


#include "SbGeneral.H"
#include "SbGeoObsVLBIStat.H"
#include "SbInterface.H"




/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatSrcLI..										*/
/*												*/
/*==============================================================================================*/
class SBObsVLBIStatSrcLI : public QListViewItem
{
private:
  SBObsVLBIStatSource	*SS;
public:
  SBObsVLBIStatSrcLI(QListView* parent, SBObsVLBIStatSource* SS_) 
    : QListViewItem(parent) {SS=SS_;};
  virtual QString ClassName() const {return "SBObsVLBIStatSrcLI";};
  SBObsVLBIStatSource* sourceStatistics() {return SS;};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatStaLI..										*/
/*												*/
/*==============================================================================================*/
class SBObsVLBIStatStaLI : public QListViewItem
{
private:
  SBObsVLBIStatStation	*SS;
public:
  SBObsVLBIStatStaLI(QListView* parent, SBObsVLBIStatStation* SS_)
    : QListViewItem(parent) {SS=SS_;};
  virtual QString ClassName() const {return "SBObsVLBIStatStaLI";};
  SBObsVLBIStatStation* stationStatistics() {return SS;};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatRecordLI..									*/
/*												*/
/*==============================================================================================*/
class SBObsVLBIStatRecordLI : public QListViewItem
{
private:
  SBObsVLBIStatRecord	*SR;
public:
  SBObsVLBIStatRecordLI(QListView* parent, SBObsVLBIStatRecord* SR_) 
    : QListViewItem(parent) {SR=SR_;};
  virtual QString ClassName() const {return "SBObsVLBIStatSrcPerSessionLI";};
  SBObsVLBIStatRecord* statRecord() {return SR;};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatBrowser..									*/
/*												*/
/*==============================================================================================*/
class SBObsVLBIStatBrowser : public SBTabDialog
{
  Q_OBJECT    
public:
  SBObsVLBIStatBrowser(bool=FALSE, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBObsVLBIStatBrowser();
  virtual QString ClassName() const {return "SBObsVLBIStatBrowser";};
  
  protected
  slots:
  void editSrcEntry(QListViewItem*);
  void editStaEntry(QListViewItem*);
  
private:
  SBObsVLBIStatistics	*Statistics;
  
  
  // stations:
  QWidget		*wStationStatistics();
  QListView		*StaLV;

  // sources:
  QWidget		*wSourceStatistics();
  QListView		*SrcLV;


  /* functions */
  //  virtual void		acquireData();
  //  virtual void		browseData();

  //protected:
  //  virtual void closeEvent(QCloseEvent *e){if(numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatSrc..										*/
/*												*/
/*==============================================================================================*/
class SBObsVLBIStatSrc : public QDialog
{
  Q_OBJECT    
public:
  SBObsVLBIStatSrc(SBObsVLBIStatSource*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBObsVLBIStatSrc();
  virtual QString ClassName() const {return "SBObsVLBIStatSrc";};
  //  virtual void closeEvent(QCloseEvent *e){if (numOpenedWindows) e->ignore(); else delete this;};
  
  protected
  slots:
  void		accept();
  void		reject(){QDialog::reject(); delete this;};

private:
  SBObsVLBIStatSource	*SrcStat;
  QListView		*SessionLV;

  virtual void		acquireData();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatSta..										*/
/*												*/
/*==============================================================================================*/
class SBObsVLBIStatSta : public QDialog
{
  Q_OBJECT    
public:
  SBObsVLBIStatSta(SBObsVLBIStatStation*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBObsVLBIStatSta();
  virtual QString ClassName() const {return "SBObsVLBIStatSta";};
  //  virtual void closeEvent(QCloseEvent *e){if (numOpenedWindows) e->ignore(); else delete this;};
  
  protected
  slots:
  void		accept();
  void		reject(){QDialog::reject(); delete this;};

private:
  SBObsVLBIStatStation	*StaStat;
  QListView		*SessionLV;

  virtual void		acquireData();
};
/*==============================================================================================*/

 


#endif //SB_I_OBS_VLBI_STAT_H
