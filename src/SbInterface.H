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

#ifndef SB_INTERFACE_H
#define SB_INTERFACE_H


#include <config.h>
#include <qmultilinedit.h>
#include <qtabdialog.h>
#include <qlistview.h>

#include "SbGeneral.H"


extern QString TmpString;
class QPixmap;
extern QPixmap* pmSrcStatus[5];
extern QPixmap* pmSrcHLink;




/*==============================================================================================*/
/*												*/
/* SBLogger											*/
/*												*/
/*==============================================================================================*/
class SBLogger : public SBLog, public QMultiLineEdit
{
public:
  SBLogger(QWidget*, int=100, bool=TRUE, const QString& ="stbreeze.log", const char* =0);
  ~SBLogger(){};
protected:
  virtual QString ClassName() const {return "SBLogger";};
  virtual void MakeOutput(const QString &, bool);
  virtual void ClearSpool();
};
/*==============================================================================================*/


 
/*==============================================================================================*/
/*												*/
/* SBTabDialog..										*/
/*												*/
/*==============================================================================================*/
class SBTabDialog : public QTabDialog
{
  Q_OBJECT
public:
  SBTabDialog(QWidget* parent_, const char* name_, bool modal_, WFlags f) :
    QTabDialog(parent_, name_, modal_, f){};
  SBTabDialog() : QTabDialog(){};
  
  protected
  slots:
  virtual void makeApply(){acquireData();};
  virtual void makeDefaults(){};
  
  /* want to be deleted when OK pressed: */
  virtual void		done(int r)
    {
      QDialog::done(r);
      if (r==QDialog::Accepted)	acquireData();
      delete this;
    };
protected:
  /* want to be deleted when got a close event: */
  virtual void		closeEvent(QCloseEvent *){delete this;};
  virtual void		acquireData(){};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBListView..											*/
/*												*/
/*==============================================================================================*/
class SBListView : public QListView
{
  Q_OBJECT
  
  private 
  slots:
  void		movingStarted(QListViewItem*, const QPoint&, int);
  
private:
  bool			IsMoving;
  int			MCol;
  QListViewItem		*MItem;

protected:
  void		contentsMouseMoveEvent(QMouseEvent*);
  void		contentsMouseReleaseEvent(QMouseEvent*);
  
  
public:
  SBListView(QWidget* =0, const char* =0);
  virtual void setSelected (QListViewItem *, bool);
  
  signals:
  void	moveUponItem(QListViewItem*, int);
  void	selChanged4Item(QListViewItem*, bool);
};
/*==============================================================================================*/




#endif //SB_INTERFACE_H
