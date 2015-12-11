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

#ifndef SB_PLOT_H
#define SB_PLOT_H


#include <config.h>

//#include <qcanvas.h>

#include <qdialog.h>
#include <qframe.h>
#include <qscrollview.h>

#include "SbGeneral.H"
#include "SbVector.H"


/*==============================================================================================*/
/*												*/
/* SBPlotBranch											*/
/*												*/
/*==============================================================================================*/
class SBPlotBranch : public SBNamed
{
  SBMatrix		*Data;
  unsigned int		NR;		//<! Number of rows
  bool			IsBrowsable;

public:
  SBPlotBranch(unsigned int, unsigned int, unsigned int, const QString&);
  ~SBPlotBranch();
  
  unsigned int		Idx;
  unsigned int		nR()  const {return NR;};
  SBMatrix*		data() {return Data;};
  void			setIsBrowsable(bool Is_) {IsBrowsable = Is_;};
  bool			isBrowsable() const {return IsBrowsable;};
  void			setDataAttr(unsigned int, unsigned int);
  unsigned int		dataAttr(unsigned int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlotCarrier										*/
/*												*/
/*==============================================================================================*/
class SBPlotBranchList : public QList<SBPlotBranch>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      if (*(SBNamed*)item1==*(SBNamed*)item2) return 0; 
      return (*(SBNamed*)item1 < *(SBNamed*)item2)?-1:1;
    };
public:
  virtual SBPlotBranch* find(const SBNamed* n)
    {
      QList<SBPlotBranch>::find((const SBPlotBranch*)n);
      return QList<SBPlotBranch>::current();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlotCarrier										*/
/*												*/
/*==============================================================================================*/
class SBPlotCarrier : public SBNamed
{
public:

  enum AxisType {AT_DATA=0, AT_MJD=1};
  
  enum DataAttr 
  {
    DA_IGN	=	1<<0,
    DA_BAR	=	1<<1
  };
  
  
private:
  SBPlotBranchList	Branches;

  // branch format descriptor:
  unsigned int		NCV;		//<! Number of values columns 
  unsigned int		NCE;		//<! Number of std errors columns
  QList<QString>*	ColumnNames;	//<! List of column names
  int*			DatTypes;	//<! Type of a column
  unsigned int*		DatStdVarIdx;	//<! Index of a std. var. column
  unsigned int*		DatAttr;	//<! Index of a std. var. column
  QString		File4SaveBaseName;

  bool			IsOK;

public:
  SBPlotCarrier(unsigned int, unsigned int, const QString&);
  ~SBPlotCarrier();
  void			createBranch(unsigned int, const QString&);
  void			setType(unsigned int NC_, AxisType Type_)
    {if (NC_<NCV) *(DatTypes+NC_) = Type_;};
  AxisType		type(unsigned int NC_) const 
    {return (NC_<NCV)? (AxisType)(*(DatTypes+NC_)):AT_DATA;};
  void			setStdVarIdx(unsigned int NC_, unsigned int StdErr_) 
    {if (NC_<NCV && StdErr_<NCV+NCE) *(DatStdVarIdx+NC_)=StdErr_;};
  unsigned int		stdVarIdx(unsigned int NC_) const
    {return (NC_<NCV)? *(DatStdVarIdx+NC_):0;};
  void			setAttr(unsigned int NC_, int Attr_) 
    {if (NC_<NCV) *(DatAttr+NC_)=Attr_;};

  SBPlotBranchList*	branches() {return &Branches;};
  SBPlotBranch*		findBranch(const QString& Name_){SBNamed N_(Name_); return Branches.find(&N_);};
  QList<QString>*	columnNames() {return ColumnNames;};
  bool			selfCheck();
  bool			isOK() const {return IsOK;};
  // max number of columns:
  unsigned int		numOfCols() const {return NCV+NCE+1;};
  unsigned int		nCV() const {return NCV;};
  unsigned int		nCE() const {return NCE;};
  
  const QString&	file4SaveBaseName() const {return File4SaveBaseName;};
  void			setFile4SaveBaseName(const QString&);
  void			setAbsFile4SaveBaseName(const QString&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlotArea											*/
/*												*/
/*==============================================================================================*/
class SBPlotArea : public QWidget
{
  Q_OBJECT

  // areas:
  int			Width;
  int			Height;
  int			UpMarg;
  int			RtMarg;
  // labels & tics:
  int			XLabelWidth;
  int			LabelsHeight;
  int			YLabelWidth;
  int			YLabelHeight;
  int			XTicsWidth;
  int			YTicsWidth;
  // frame area:
  int			XFrameBegin;
  int			XFrameEnd;
  int			YFrameBegin;
  int			YFrameEnd;
  // data area:
  int			XDataBegin;
  int			XDataEnd;
  int			YDataBegin;
  int			YDataEnd;
  // Title:
  int			TitleWidth;
  // X Tics:
  int			NumOfXTics;
  int			XStepP;
  double		XTicsStep;
  double		XTicsBias;
  bool			isXTicsBiased;
  bool			isXTicsMJD;
  int			XTicsMJD;
  // Y Tics:
  int			NumOfYTics;
  int			YStepP;
  double		YTicsStep;
  double		YTicsBias;
  bool			isYTicsBiased;

  // labels:
  QString		*XLabel;
  QString		*YLabel;
  static QString	XLabel4Unknown;
  static QString	YLabel4Unknown;

  // 
  unsigned int		Xcol;
  unsigned int		Ycol;
  double		MaxX;
  double		MaxY;
  double		MinX;
  double		MinY;

  // transform:
  double		f_Ax;
  double		f_Bx;
  double		f_Ay;
  double		f_By;
  
  // pens:
  QPen			*FramePen;
  QPen			*BranchPens;
  QPen			*ZeroPen;
  QPen			*BarPen;
  int			BPSaturation;
  int			BPValue;
  
  // draw mode:
  bool			IsPlotPoints;
  bool			IsPlotLines;
  bool			IsPlotErrBars;
  bool			IsPlotImpulses;
  
  // options:
  bool			IsLimitsOnVisible;
  bool			IsAllData;
  bool			IsStdVar;

  // browsables:
  SBPlotCarrier		*PlotCarrier;

  // misc
  bool			IsHasHaveZero;

public:
  SBPlotArea(SBPlotCarrier*, QWidget* parent=0, const char* name=0, WFlags f=0);
  virtual ~SBPlotArea();
  
  virtual QString	ClassName() const {return "SBPlotArea";};
  unsigned int		xCol() const {return Xcol;};
  unsigned int		yCol() const {return Ycol;};

  void			setXCol(unsigned int);
  void			setYCol(unsigned int);
  void			setBPSaturation(int);
  void			setBPValue(int);
  void			setIsPlotPoints(bool Is_) {IsPlotPoints=Is_;};
  void			setIsPlotLines(bool Is_)  {IsPlotLines=Is_;};
  void			setIsPlotErrBars(bool Is_){IsPlotErrBars=Is_;};
  void			setIsPlotImpulses(bool Is_){IsPlotImpulses=Is_;};

  // modes:
  void			setIsHasHaveZero(bool Is_){IsHasHaveZero=Is_;};
  

  void			setIsAllData(bool Is_){IsAllData=Is_;};
  void			setIsLimitsOnVisible(bool Is_){IsLimitsOnVisible=Is_;};
  void			setIsStdVar(bool Is_){IsStdVar=Is_;};

  void			output4Print(QPainter*, int, int);
  void			output4Files();
  void			dataChanged() {initBranchPens();};

protected:
  void			showData(QPainter*);
  void			drawFrames(QPainter*);
  void			drawYTics(QPainter*);
  void			drawXTics(QPainter*);
  void			drawXmjdTics(QPainter*);
  void			drawData(QPainter*);
  void			getLimits();
  void			calcTransforms();
  void			defineAreas(QPainter*);
  int			calcX(double X_) 
    {double f = f_Ax + f_Bx*X_; if (f<0.0) return 0; if (f>Width) return Width-1; return (int)f;};
  int			calcY(double Y_) 
    {double f = f_Ay + f_By*Y_; if (f<0.0) return 0; if (f>Height) return Height-1; return (int)f;};
  void			setBranchColors();
  void			initBranchPens();

  virtual void		paintEvent(QPaintEvent*);
  virtual void		resizeEvent(QResizeEvent*);  
};
/*==============================================================================================*/



class QComboBox;
class QListBox;
class QCheckBox;
class QSlider;
class QPushButton;
class QListBoxItem;
/*==============================================================================================*/
/*												*/
/* SBPlot											*/
/*												*/
/*==============================================================================================*/
class SBPlot : public QWidget
{
  Q_OBJECT
  
public:

  enum PlotMode
  {
    PM_HAS_HAVE_ZERO	=	1<<0,
    PM_IMPULSE		=	1<<1,
    PM_WO_BRANCH_NAMES	=	1<<2,
    PM_WO_AXIS_NAMES	=	1<<3,
    PM_WO_DOTS		=	1<<4,
    PM_LINES		=	1<<5,
  };


private:
  static const double	ScaleFactor;
  QScrollView		*Plotter;
  SBPlotArea		*Area;
  QFrame		*Controls;
  double		scaleX;
  double		scaleY;

  unsigned int		Modes;
  SBPlotCarrier		*PlotCarrier;

  // controls:
  QComboBox		*CBXAxis;
  QComboBox		*CBYAxis;
  QListBox		*LBBranches;
  
  QCheckBox		*CBPoints;
  QCheckBox		*CBLines;
  QCheckBox		*CBErrBars;
  QCheckBox		*CBImpulses;
  
  QCheckBox		*CBAllData;
  QCheckBox		*CBRangeVisible;
  QCheckBox		*CBWStdVar;
  
  QPushButton		*PBZommIn;
  QPushButton		*PBZommOut;
  QPushButton		*PBZommXIn;
  QPushButton		*PBZommXOut;
  QPushButton		*PBZommYIn;
  QPushButton		*PBZommYOut;

public:
  SBPlot(SBPlotCarrier*, QWidget* =0, unsigned int =0, const char* =0);
  virtual ~SBPlot();
  void			dataChanged();

protected:
  virtual QString	ClassName() const {return "SBPlot";};
  virtual void		resizeEvent(QResizeEvent*);
  QWidget*		control();
  void			fillAxisNames();
  void			fillBranchesNames();
  
  
  protected
  slots:
  
  void			xAxisChanged(int);
  void			yAxisChanged(int);
  void			colorSChanged(int);
  void			colorVChanged(int);
  void			branchChanged();
  void			DMPointsChanged(bool);
  void			DMLinesChanged(bool);
  void			DMErrBarsChanged(bool);
  void			DMImpulsesChanged(bool);
  
  void			ODataAllChanged(bool);
  void			OVisRang(bool);
  void			OWStdVar(bool);

  // zooming:
  void			zoomIn();
  void			zoomOut();
  void			zoomXIn();
  void			zoomXOut();
  void			zoomYIn();
  void			zoomYOut();
  void			zoomNormalView();

  // saving:
  void			save2File();
  void			save2PS();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlotDialog.										*/
/*												*/
/*==============================================================================================*/
class SBPlotDialog : public QDialog
{
  Q_OBJECT    
public:
  SBPlotDialog(SBPlotCarrier*, bool = FALSE, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBPlotDialog();
  
  virtual QString ClassName() const {return "SBPlotDialog";};


  signals:
  void windowClosed();
  
  private 
  slots:
  void accept();
  
protected:
  virtual void		closeEvent(QCloseEvent *){delete this;};
  
private:
  bool			IsNeedDeletePC;
  SBPlotCarrier		*Carrier;
  SBPlot		*Plot;
};
/*==============================================================================================*/




#endif //SB_PLOT_H
