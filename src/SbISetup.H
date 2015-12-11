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

#ifndef SB_I_SETUP_H
#define SB_I_SETUP_H

#include <config.h>

#include "SbGeneral.H"
#include "SbInterface.H"
#include <qlistview.h>


class SBFileConv;
/*==============================================================================================*/
/*												*/
/* SBFileConvLI..										*/
/*												*/
/*==============================================================================================*/
class SBFileConvLI : public QListViewItem
{
private:
  SBFileConv		*Entry;
public:
  SBFileConvLI(QListView* parent, SBFileConv* Entry_) : QListViewItem(parent) 
    {if (Entry_) Entry=Entry_; else Entry=NULL;};
  virtual QString ClassName() const {return "SBFileConvLI";};
  SBFileConv*		entry(){return Entry;};
  virtual QString	text(int) const;
};
/*==============================================================================================*/



class QLineEdit;
class QCheckBox;
class QSpinBox;
class SBSetUp;
class QComboBox;
class SBInstitution;
class SBInstitutionList;
class QRadioButton;
/*==============================================================================================*/
/**SBSetupDialog.
 * Allows users to configure the software (general thingth, not related to geodynamics at all).
 *
 */
/*==============================================================================================*/
class SBSetupDialog : public SBTabDialog
{
  Q_OBJECT    
public:
  SBSetupDialog(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBSetupDialog();
  
  virtual QString ClassName() const {return "SBSetupDialog";};
  
  private 
  slots:
  virtual void		setData();
  virtual void		makeApply();
  virtual void		makeDefaults();
  void			addPacker();
  void			delPacker();
  void			chkPacker();
  void			addInst();
  void			delInst();
  
private:
  class SBInstLI : public QListViewItem
  {
  private:
    SBInstitution	*Inst;
  public:
    SBInstLI(QListView* parent, SBInstitution *I_)
      : QListViewItem(parent) {if (I_) Inst=I_; else Inst=NULL;};
    SBInstitution*	inst(){return Inst;};
    virtual QString	text(int) const;
  };

  virtual void		acquireData();
protected:
  SBSetUp*		setup;
  
  /* pathes			*/
  QWidget		*pathes();
  QLineEdit		*PathLE[19];
  
  /* logger                   */
  QWidget		*logger();
  QCheckBox		*cbLogStoreInFile;
  QCheckBox		*cbLogTimeLabel;
  QLineEdit		*eLogFileName;
  QSpinBox		*sLogCapacity;
  QCheckBox		*cbLLevel[4][32];

  /* packer                   */
  QWidget		*packer();
  QListView		*FConv;
  QLineEdit		*FCName;
  QLineEdit		*FCComp;
  QLineEdit		*FCExtr;
  QLineEdit		*FCExt;
  QComboBox		*FCDefault;

  /* misc                   */
  QWidget		*misc();
  QRadioButton		*rbUnitNames[3];

  /* institutions                   */
  QWidget		*institutions();
  SBInstitutionList	*InstList;
  QListView		*lvInst;
  QLineEdit		*leAcronym;
  QLineEdit		*leDescr;
};
/*==============================================================================================*/



#endif //SB_I_SETUP_H
