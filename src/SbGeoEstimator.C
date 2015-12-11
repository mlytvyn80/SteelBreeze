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

#include "math.h"
#include "SbGeoEstimator.H"
#include "SbSetup.H"

#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>



/*==============================================================================================*/
/*												*/
/* class SBEstimator::Group implementation							*/
/*												*/
/*==============================================================================================*/
SBEstimator::Group::Group(const SBMJD& TTL_) : SBParameterList(), SBMJD(TTL_)
{
  setAutoDelete(TRUE);
};

SBEstimator::Group::~Group()
{
};

void SBEstimator::Group::addParameter(const SBParameter* P) 
{
  SBParameterList::inSort(P);
};

void SBEstimator::Group::delParameter(const SBParameter* P) 
{
  SBParameterList::remove(P->name());
};

void SBEstimator::Group::beforeRun(int NumInGroups, GroupList* groups)
{
  int N = count();
  Z = new SBVector(N, "Z");
  R = new SBUpperMatrix(N, "R");
  CrossCorr = new QList<SBMatrix>;
  CrossCorr->setAutoDelete(TRUE);
  for (unsigned int i=NumInGroups+1; i<groups->count(); i++)
    CrossCorr->append(new SBMatrix(N, groups->at(i)->count(), "CrossCorr"));
};

void SBEstimator::Group::afterRun(int /*NumGroups*/, GroupList* /*groups*/)
{
  if (Z) delete Z;
  if (R) delete R;
  if (CrossCorr) delete CrossCorr;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBEstimator::GroupList implementation							*/
/*												*/
/*==============================================================================================*/
SBEstimator::GroupList& SBEstimator::GroupList::operator= (const SBEstimator::GroupList& L_)
{
  setAutoDelete(TRUE);
  if (count()) clear();
  
  if (ValByDate) delete ValByDate;
  ValByDate = new QDict<Group>(L_.count()+20);
  ValByDate->setAutoDelete(FALSE);

  if (L_.count())
    {
      QListIterator<Group> it(L_);
      for ( ; it.current(); ++it ) 
	inSort(new Group(*it.current()));
    };
  
  return *this;
};

void SBEstimator::GroupList::inSort(const Group* d)
{
  if (!d)
    Log->write(SBLog::ERR, SBLog::TIME, ClassName() + ": insert of empty value; failed");
  else
    {
      QString l = d->toString(SBMJD::F_INTERNAL);
      if (ValByDate->find(l))
	Log->write(SBLog::ERR, SBLog::TIME, ClassName() + 
		   ": cannot insert the group whith the key [" + l +  
		   "], the group already exist");
      else
	{
	  QList<Group>::inSort(d);
	  
	  if (ValByDate->size()<=ValByDate->count())
	    ValByDate->resize(ValByDate->size() + 10);
	  ValByDate->insert(l, d);
	};
    };
};

bool SBEstimator::GroupList::remove(const QString& key)
{
  Group* d;
  
  if ((d = ValByDate->find(key)))
    return ValByDate->remove(key) && QList<Group>::remove(d);
  else
    {
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + 
		 ": cannot delete the value: cannot find the key in the dictionary");
      return FALSE;
    };
};

bool SBEstimator::GroupList::remove(const Group* d)
{
  if (d)
    return ValByDate->remove(d->toString(SBMJD::F_INTERNAL)) && QList<Group>::remove(d);
  else
    {
      Log->write(SBLog::ERR, SBLog::TIME, ClassName() + 
		 ": cannot delete the value: the value is NULL");
      return FALSE;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEstimator implementation								*/
/*												*/
/*==============================================================================================*/
SBEstimator::SBEstimator(SBProject* Prj_, SBRunManager* RunMgr_, SBSolution* Solution_)
{
  IsNeedRmTemp  = TRUE;
  TLastProcessed= TZero;

  Prj = Prj_;
  RunMgr = RunMgr_;
  Solution = Solution_;
  //  Groups = new GroupList;
  //  NewGroups = new GroupList;

  // check work dir:
  WorkDir = SetUp->path2Work() + Prj->name() + "/"; // should we replace spaces/comas/etc?
  QDir		dir;
  if (!dir.exists(WorkDir) && !dir.mkdir(WorkDir))
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": can't create dir [" + WorkDir + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create working dir\n " + WorkDir +
				    "\nfor the project [" + Prj->name() + "]"));
      // exit(1); ?
    };

  pipe4Stoch = NULL;

  NyAll = 0;
  YsAll = NULL;
  YAll  = NULL;
  ZyAll = NULL;
  RyAll = NULL;
  PyAll = NULL;

  Ny	= 0;
  Ys	= NULL;
  Y	= NULL;
  Zy	= NULL;
  Ry	= NULL;
  Py	= NULL;
  
  Nx	= 0;
  Xs	= NULL;
  X	= NULL;
  Zx	= NULL;
  Rx	= NULL;
  Rxy	= NULL;
  Px	= NULL;

  Np	= 0;
  Ps	= NULL;
  P	= NULL;
  Zp	= NULL;
  Rp	= NULL;
  Rpx	= NULL;
  Rpy	= NULL;
  Pp	= NULL;

  SR	= NULL;
  Rp_s	= NULL;
  Rpp_s = NULL;
  Rpx_s = NULL;
  Rpy_s = NULL;
  Zp_s	= NULL;

  NStochsCont = 0;
  PsCont= NULL;
  ZpCont= NULL;
  RpCont= NULL;
  RpyCont=NULL;

  PsAux  =NULL;
  PAux   =NULL;
  Pp_Aux =NULL;


  Nz	= 0;
  Z	= NULL;
  Ap	= NULL;
  Ax	= NULL;
  Ay	= NULL;
  U	= NULL;
};

SBEstimator::~SBEstimator()
{
  // remove work dir:
  QDir d;
  if (IsNeedRmTemp)
    {
      if (d.exists(WorkDir) && !d.rmdir(WorkDir))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the directory [" + WorkDir + "]");
    };
  
  //  if (Groups) delete Groups;
  //  if (NewGroups) delete NewGroups;
};

void SBEstimator::parameterListsAboutToChange()
{
  // move info from [R_y Z_y] to the global informaton array
  //  if (Ny) moveGlobalInfo();

};

void SBEstimator::parameterListsChanged()
{
  /*
    if (Ry) delete Ry;
    if (Zy) delete Zy;
    
    Ny = RunMgr->globalList()->count();
    Ry = new SBUpperMatrix(Ny, "Ry");
    Zy = new SBVector(Ny);
    Ys = RunMgr->globalList();
    for (unsigned int i=0; i<Ny; i++)
    Ry->set(i,i, 1.0/Ys->at(i)->stdDev());
  */
};

void SBEstimator::addParameter(const SBParameter* /*Par_*/, const SBMJD& /*TTL_*/)
{
  //  Group		*g=NULL;
  //  if ((g=NewGroups->find(TTL_))) g->addParameter(Par_);
  //  else //create group
  //    {
  //      NewGroups->inSort(g = new Group(TTL_));
  //      g->addParameter(Par_);
  //      Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
  //		 ": created the group with TTL <" + TTL_.toString(SBMJD::F_Short) + ">");
  //    };
};

void SBEstimator::delParameter(const SBParameter* /*Par_*/, const SBMJD& /*TTL_*/)
{
  //  Group		*g=NULL;
  //  if ((g=NewGroups->find(TTL_)))
  //    {
  //      g->delParameter(Par_);
  //      if (!g->count()) // rm the empty group
  //	{
  //	  NewGroups->remove(g);
  //	  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
  //		     ": deleted the group with TTL <" + TTL_.toString(SBMJD::F_Short) + ">");
  //	};
  //    }
  //  else //oops!
  //    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
  //	       ": cannot delete the parameter [" + Par_->name() + "] from the group with TTL <" +
  //	       TTL_.toString(SBMJD::F_Short) + ">");
};

void SBEstimator::rearrangeGroups()
{
  //  unsigned int	i;
  //  if (NewGroups && NewGroups->count())
  //    {
  //      for (i=0; i<NewGroups->count(); i++) NewGroups->at(i)->beforeRun(i, NewGroups);
  //    };
  //
  //  if (Groups) // not the first observations
  //    {
  //      
  //    };
};

void SBEstimator::prepare4Run(SBParameterList* AllGlobalList_)
{
  Stack.clear();
  Stack.setAutoDelete(TRUE);

  YsAll = AllGlobalList_;
  NyAll = YsAll->count();
  ZyAll = new SBVector(NyAll, "ZyAll");
  RyAll = new SBUpperMatrix(NyAll, "RyAll");
  for (unsigned int i=0; i<NyAll; i++)
    RyAll->set(i,i, 1.0/YsAll->at(i)->sigmaAPriori());
  YsAll->clearPValues();

  PsCont = new SBParameterList;
  PsCont->setAutoDelete(FALSE);
};

void SBEstimator::prepare4Local(int Nz_, const SBMJD& T_)
{
  TofBatch = T_;

  // global parameters:
  Ny = RunMgr->globalList()->count();
  Ys = RunMgr->globalList();
  Ry = new SBUpperMatrix(Ny, "Ry");
  Zy = new SBVector(Ny, "Zy");
  for (unsigned int i=0; i<Ny; i++)
    Ry->set(i,i, 1.0/Ys->at(i)->sigmaAPriori());
  //  Ys->clearPValues(); //keeping it
  Ys->clearPValues_aux();

  // local parameters:
  Nx = RunMgr->localList()->count();
  Xs = RunMgr->localList();
  Rx = new SBUpperMatrix(Nx, "Rx");
  Rxy= new SBMatrix(Nx, Ny, "Rxy");
  Zx = new SBVector(Nx, "Zx");
  for (unsigned int i=0; i<Nx; i++)
    Rx->set(i,i, 1.0/Xs->at(i)->sigmaAPriori());
  Xs->clearPValues();

  // stochastic parameters:
  Np = RunMgr->stochList()->count();
  Ps = RunMgr->stochList();
  Rp = new SBUpperMatrix(Np, "Rp");
  Rpy= new SBMatrix(Np, Ny, "Rpy");
  Rpx= new SBMatrix(Np, Nx, "Rpx");
  Zp = new SBVector(Np, "Zp");
  for (unsigned int i=0; i<Np; i++)
    Rp->set(i,i, 1.0/Ps->at(i)->sigmaAPriori());
  Ps->clearPValues();
  
  if (NStochsCont)
    mapContStochs4NewBatch(T_);

  // 4smoothing:
  Rp_s = new SBUpperMatrix(Np, "Rp_s");
  Rpp_s= new SBMatrix(Np, Np, "Rpp_s");
  Rpx_s= new SBMatrix(Np, Nx, "Rpx_s");
  Rpy_s= new SBMatrix(Np, Ny, "Rpy_s");
  SR   = new SBMatrix(2*Np, 2*Np + Nx + Ny + 1, "SR");
  Zp_s = new SBVector(Np, "Zp_s");

  // data:
  Nz = Nz_ + 8;
  Z   = new SBVector(Nz, "Z");
  Ap  = new SBMatrix(Nz, Np, "Ap");
  Ax  = new SBMatrix(Nz, Nx, "Ax");
  Ay  = new SBMatrix(Nz, Ny, "Ay");
  U   = new SBVector(Nz + 1, "U");
  Nz  = 0; // from here Nz is a count of observations

  // check work dir (continue):
  //  WorkDir4Loc = WorkDir + RunMgr->listOfCurrentSessNames() + "/";
  BatchName = RunMgr->listOfCurrentSessNames();
  QDir dir;
  if (!dir.exists(workDir4Loc(BatchName)) && !dir.mkdir(workDir4Loc(BatchName)))
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": can't create dir [" + 
		 workDir4Loc(BatchName) + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create working dir\n " + workDir4Loc(BatchName) +
				    "\nfor the project [" + Prj->name() + "]"));
      // exit(1); ?
    };
  NumOfStochs = 0;

  // opening the stream for saving the stochastic parameters
  if (Np)
    {
      file4Stoch.setName(workDir4Loc(BatchName) + "stochastics.bin.gz");
      //      if (!file4Stoch.open(IO_WriteOnly))
      //	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
      //		   ": cannot open the file [" + file4Stoch.name() + "]");
      pipe4Stoch = SetUp->fcList()->open4Out(file4Stoch.name(), file4Stoch, stream4Stoch);
      //      stream4Stoch.setDevice(&file4Stoch); // ready for writing...
    };
};

void SBEstimator::finisLocal()
{
  dataUpdate();
  // saving the intermediate results:
  // stochastic parameters:
  if (Np) 
    {
      stream4Stoch << *Zp << *Rp << *Rpx << *Rpy << *Ps; // save one copy of the list
      SetUp->fcList()->close(file4Stoch, pipe4Stoch, stream4Stoch);
      //      file4Stoch.close();
      //      stream4Stoch.unsetDevice();
    };

  // check for continuous stochastic parameters:
  NStochsCont = 0;
  for (unsigned int i=0; i<Np; i++)
    if (Ps->at(i)->isAttr(SBDerivation::IsPropagated))
      NStochsCont++;

  if (NStochsCont)
    collectContStochs4NextBatch();
  
  if (Prj->cfg().isGenerateDSINEXes())
    Solution->saveNormalEqs(Ys, Zy, Ry,  Xs, Zx, Rx, Rxy,  BatchName);

  // global parameters:
  if (Ny)
    moveGlobalInfo();

  /*
    GlobStack.push(new QString(BatchName));
    QFile	file(workDir4Loc(BatchName) + "globals.bin");
    if (!file.open(IO_WriteOnly))
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot open the file [" + file.name() + "]");
    QDataStream s(&file);
    s << *Ys << *Zy << *Ry << *Rxy;
    file.close();
    s.unsetDevice();
  */

  // local parameters:
  Stack.push(new QString(BatchName));
  QFile			file(workDir4Loc(BatchName) + "locals.bin.gz");
  QDataStream		s;
  FILE			*pipe = SetUp->fcList()->open4Out(file.name(), file, s);
  //  if (!file.open(IO_WriteOnly))
  //    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot open the file [" + file.name() + "]");
  s << *Ys << *Xs << *Zx << *Rx << *Rxy << NumOfStochs << TofBatch;
  SetUp->fcList()->close(file, pipe, s);
  //  file.close();
  //  s.unsetDevice();


  // freeing the memory:
  // 4smoothing:
  if (SR)    {delete SR;    SR   = NULL;};
  if (Rp_s)  {delete Rp_s;  Rp_s = NULL;};
  if (Rpp_s) {delete Rpp_s; Rpp_s= NULL;};
  if (Rpx_s) {delete Rpx_s; Rpx_s= NULL;};
  if (Rpy_s) {delete Rpy_s; Rpy_s= NULL;};
  if (Zp_s)  {delete Zp_s;  Zp_s = NULL;};
  // stochastic parameters:
  if (Rp)    {delete Rp;    Rp   = NULL;};
  if (Rpy)   {delete Rpy;   Rpy  = NULL;};
  if (Rpx)   {delete Rpx;   Rpx  = NULL;};
  if (Zp)    {delete Zp;    Zp   = NULL;};
  if (Ps)     Ps = NULL; // not an owner
  Np = 0;

  // local parameters:
  if (Rx)    {delete Rx;    Rx   = NULL;};
  if (Rxy)   {delete Rxy;   Rxy  = NULL;};
  if (Zx)    {delete Zx;    Zx   = NULL;};
  if (Xs)     Xs = NULL; // not an owner
  Nx = 0;

  // global parameters:
  if (Ry)    {delete Ry;    Ry   = NULL;};
  if (Zy)    {delete Zy;    Zy   = NULL;};
  if (Ys)     Ys = NULL; // not an owner
  Ny = 0;

  // data:
  if (Z)     {delete Z;     Z    = NULL;};
  if (Ap)    {delete Ap;    Ap   = NULL;};
  if (Ax)    {delete Ax;    Ax   = NULL;};
  if (Ay)    {delete Ay;    Ay   = NULL;};
  if (U)     {delete U;     U    = NULL;};
  Nz  = 0;
};

void SBEstimator::finisRun()
{
  if (PsCont)
    {
      delete PsCont;
      PsCont = NULL;
    };

  solveGlobals();

  // global parameters (special case):
  if (PyAll) {delete PyAll; PyAll = NULL;};
  if (RyAll) {delete RyAll; RyAll = NULL;};
  if (ZyAll) {delete ZyAll; ZyAll = NULL;};
  if (YAll)  {delete YAll;  YAll  = NULL;};
  if (YsAll)  YsAll = NULL; // not an owner
  NyAll = 0;

  Stack.clear();
};

void SBEstimator::solveGlobals()
{
  YAll  = new SBVector(NyAll, "YAll");
  PyAll = new SBSymMatrix(NyAll, "PyAll");

  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": solving global parameters");
  
  // solve the parameters:
  //     RyAll * YAll = ZyAll
  Solve(*RyAll, *YAll, *ZyAll);
  RRT(*PyAll, ~*RyAll);
  YsAll->updateSolution(YAll, PyAll);

  // submit to the SBSolution:
  Solution->submitGlobalParameters(YsAll, PyAll);
  Solution->reportGlobalParameters();
  
  // and local parameters:

  PsCont = new SBParameterList;
  PsCont->setAutoDelete(TRUE);
  
  solveLocals();

  if (PsCont)
    {
      delete PsCont;
      PsCont = NULL;
    }

  if (PsAux)
    {
      delete PsAux;
      PsAux = NULL;
    }

  delete YAll;  YAll=NULL;
  delete PyAll; PyAll=NULL;
};

void SBEstimator::solveLocals()
{
  while (!Stack.isEmpty())
    {
      QString	*wd=Stack.pop();
      QDir	dir;

      Solution->prepare4Batch(*wd);

      Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
		 ": solving local parameters in the directory [" + workDir4Loc(*wd) + "]");

      // read the list of parameters, vectors and matrices:
      QFile			file(workDir4Loc(*wd) + "locals.bin.gz");
      QDataStream		s;
      FILE			*pipe = SetUp->fcList()->open4In(file.name(), file, s);
      //      if (!file.open(IO_ReadOnly))
      //	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
      //        ": cannot open the file [" + file.name() + "]");
      Ys = new SBParameterList;
      Ys->setAutoDelete(TRUE);
      Xs = new SBParameterList;
      Xs->setAutoDelete(TRUE);
      Rx = new SBUpperMatrix;
      Rxy= new SBMatrix;
      Zx = new SBVector;
      s >> *Ys >> *Xs >> *Zx >> *Rx >> *Rxy >> NumOfStochs >> TofBatch;
      SetUp->fcList()->close(file, pipe, s);
      //      file.close();
      //      s.unsetDevice();

      // rm the temporary file:
      if (IsNeedRmTemp)
	{
	  if (dir.exists(file.name()) && !dir.remove(file.name()))
	    Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		       ": cannot delete the file [" + file.name() + "]");
	};
      
      Nx = Xs->count();
      X  = new SBVector(Nx, "X");
      Px = new SBSymMatrix(Nx, "Px");
      SBUpperMatrix *RxInv = new SBUpperMatrix(Nx, "Inverse of Rx");

      Ny = Ys->count();
      Y  = new SBVector(Ny, "Y");
      Py = new SBSymMatrix(Ny, "Py");
      
      // mapping of YAll into Y:
      SBParameter	*Par;
      int		*Idx = new int[Ny], idx;	// an indixes of elements of Ys into YsAll
      unsigned int	i, j;
      for (i = 0; i<Ny; i++)
	if ((idx = YsAll->idx( (Par=Ys->at(i)) ))!=-1)
	  *(Idx+i) = idx;
	else
	  Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		     ": cannot find the global parameter [" + Par->name() + 
		     "] in the list of all global parameters");
      for (i=0; i<Ny; i++)
	{
	  Y->set(i, YAll->at(*(Idx+i)));
	  for (j=i; j<Ny; j++)
	    Py->set(i, j, PyAll->at(*(Idx+i), *(Idx+j)));
	};
           
      // solve the parameters:
      //     Rx * X = Zx - Rxy*Y
      Solve(*Rx, *X, *Zx - *Rxy**Y);
      RRT(*Px, (*RxInv=~*Rx));
      if (Ny)
	//     Px +      Rx^(-T) * (Rxy * Py * Rxy^T) * Rx^(-T)
	*Px += QuadraticForm(QuadraticForm(*Py, *Rxy), *RxInv);
      Xs->updateSolution(X, Px);
      
      // submit to the SBSolution:
      Solution->submitLocalParameters(Xs, Px);
      Solution->reportLocalParameters();
      Solution->stat4CurrentBatch()->setTStart(TofBatch);


      NStochsCont = 0;
      if (NumOfStochs)
	solveStochs(wd, NumOfStochs);

      // rm the temporary direcotry:
      if (IsNeedRmTemp)
	{
	  if (dir.exists(workDir4Loc(*wd)) && !dir.rmdir(workDir4Loc(*wd)))
	    Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		       ": cannot delete the directory [" + workDir4Loc(*wd) + "]");
	};

      delete[] Idx;
      if (wd)	 delete wd;
      if (Xs)	{delete Xs;	Xs   = NULL;};
      if (Rx)	{delete Rx;	Rx   = NULL;};
      if (Rxy)	{delete Rxy;	Rxy  = NULL;};
      if (Zx)	{delete Zx;	Zx   = NULL;};
      if (X)	{delete X;	X    = NULL;};
      if (Px)	{delete Px;	Px   = NULL;};
      if (RxInv){delete RxInv;  RxInv= NULL;};
      if (Ys)	{delete Ys;	Ys   = NULL;};
      if (Y)	{delete Y;	Y    = NULL;};
      if (Py)	{delete Py;	Py   = NULL;};
    };
};

void SBEstimator::solveStochs(const QString* wd, int Num)
{
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
	     ": solving stochastic parameters in the directory [" + workDir4Loc(*wd) + "]");

  unsigned int			i;
  int				*Idx=NULL;
  SmoothCarrier			*Carrier;
  QStack<SmoothCarrier>		StochStack;

  StochStack.clear();
  StochStack.setAutoDelete(TRUE);

  file4Stoch.setName(workDir4Loc(*wd) + "stochastics.bin.gz");
  pipe4Stoch = SetUp->fcList()->open4In(file4Stoch.name(), file4Stoch, stream4Stoch);
  //  if (!file4Stoch.open(IO_ReadOnly))
  //    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
  //	       ": cannot open the file [" + file4Stoch.name() + "]");
  //  stream4Stoch.setDevice(&file4Stoch);

  for (int l=0; l<Num; l++)
    {
      Carrier = new SmoothCarrier;
      Carrier->load(stream4Stoch);
      StochStack.push(Carrier);
    };

  Rp = new SBUpperMatrix;
  Rpy= new SBMatrix;
  Rpx= new SBMatrix;
  Zp = new SBVector;
  Ps = new SBParameterList;
  Ps->setAutoDelete(TRUE);

  stream4Stoch >> *Zp >> *Rp >> *Rpx >> *Rpy >> *Ps;
  SetUp->fcList()->close(file4Stoch, pipe4Stoch, stream4Stoch);
  //  file4Stoch.close();
  //  stream4Stoch.unsetDevice();

  // rm the temporary file:
  QDir dir;
  if (IsNeedRmTemp)
    {
      if (dir.exists(file4Stoch.name()) && !dir.remove(file4Stoch.name()))
	Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
		   ": cannot delete the file [" + file4Stoch.name() + "]");
    };
  
  Np = Ps->count();
  P  = new SBVector(Np, "P");
  Pp = new SBSymMatrix(Np, "Pp");
  SBUpperMatrix *RpInv = new SBUpperMatrix(Np, "Inverse of Rp");

  // calc filtered estimation (the last in the batch):
  // solve the parameters:

  //     Rp * P = Zp - Rpx*X - Rpy*Y
  Solve(*Rp, *P, *Zp - *Rpx**X - *Rpy**Y);
  
  RRT(*Pp, (*RpInv=~*Rp));
  if (Nx)
    //     Pp +      Rp^(-1) * (Rpx * Px * Rpx^T) * Rp^(-T)
    *Pp += QuadraticForm(QuadraticForm(*Px, *Rpx), *RpInv);
  if (Ny)
    //     Pp +      Rp^(-1) * (Rpy * Py * Rpy^T) * Rp^(-T)
    *Pp += QuadraticForm(QuadraticForm(*Py, *Rpy), *RpInv);

  // and overlay them by `cont's:
  if (PsAux && PsAux->count())
    {
      if (PAux && Pp_Aux)
	{
	  PsCont->clear();

	  SBUpperMatrix *Rp_sCont = new SBUpperMatrix;
	  SBMatrix      *Rpp_sCont= new SBMatrix;
	  SBMatrix      *Rpy_sCont= new SBMatrix;
	  SBVector      *Zp_sCont = new SBVector;

	  QFile f(workDir4Loc(*wd) + "stochastics_aux.bin");
	  if (!f.open(IO_ReadOnly))
	    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot open the file [" + f.name() + 
		       "] for reading");
	  else
	    {
	      QDataStream data_stream(&f);
	      data_stream >> *PsCont >> *Zp_sCont >> *Rp_sCont >> *Rpp_sCont >> *Rpy_sCont;
	      data_stream.unsetDevice();
	      f.close();

	      if (IsNeedRmTemp)
		if (dir.exists(f.name()) && !dir.remove(f.name()))
		  Log->write(SBLog::ERR, SBLog::IO | SBLog::ESTIMATOR, ClassName() + 
			     ": cannot delete the file [" + f.name() + "]");
	    };

	  if (PsCont->count())
	    {
	      unsigned int NumTmp = PsCont->count();
	      int *Idx_POld = new int[NumTmp];
	      int *Idx_PNew = new int[NumTmp];

	      for (i=0; i<NumTmp; i++)
		{
		  *(Idx_POld+i) = PsAux->idx(PsCont->at(i));
		  *(Idx_PNew+i) = Ps->idx(PsCont->at(i));
		};

	      SBVector      *P_N = new SBVector(NumTmp, "P_N");
	      SBVector      *P_j = new SBVector(NumTmp, "P_j");

	      SBUpperMatrix *Rp_jInv = new SBUpperMatrix(NumTmp, "Inverse of Rp_sCont");
	      SBSymMatrix   *Pp_j    = new SBSymMatrix  (NumTmp, "Pp_j");
	      SBSymMatrix   *Pp_N    = new SBSymMatrix  (NumTmp, "Pp_N");

	      for (i=0; i<NumTmp; i++)
		{
		  P_N->set(i, PAux->at(*(Idx_POld+i)));
		  for (unsigned int j=i; j<NumTmp; j++)
		    Pp_N->set(i,j,  Pp_Aux->at(*(Idx_POld+i), *(Idx_POld+j)));
		};

	      // solve the parameters:
	      //
	      //         Rp_ *  P     =  Zp_s     -  Rpp_s*PPrev    -    Rpy_s*Y
	      Solve(*Rp_sCont, *P_j,    *Zp_sCont - *Rpp_sCont**P_N - *Rpy_sCont**YAll);
	      

	      // here to add a calculations for covariance matrix:
	      //  Pp_j = (Rp*)^(-1) * (Rp*)^(-T)
	      RRT(*Pp_j, (*Rp_jInv=~*Rp_sCont));
	      //     Pp +      (Rp*)^(-1) * ((Rpp*) * Pp_prev * (Rpp*)^T) * (Rp*)^(-T)
	      *Pp_j += QuadraticForm(QuadraticForm(*Pp_N, *Rpp_sCont), *Rp_jInv);

	      //	      if (Ny)
	      //		//     Pp +      (Rp*)^(-1) * ((Rpy*) * Py * (Rpy*)^T) * (Rp*)^(-T)
	      //		*Pp += QuadraticForm(QuadraticForm(*Py, *Rpy_s), *RpInv);
	      
	      for (i=0; i<NumTmp; i++)
		{
 		  P->set(*(Idx_PNew+i), P_j->at(i));
		  for (unsigned int j=i; j<NumTmp; j++)
		    Pp->set(*(Idx_PNew+i), *(Idx_PNew+j), Pp_j->at(i,j));
		};


	      delete Pp_N;
	      delete Pp_j;
	      delete Rp_jInv;
	      delete P_j;
	      delete P_N;
	      delete[] Idx_POld;
	      delete[] Idx_PNew;
	    };

	  delete Rp_sCont;
	  delete Rpp_sCont;
	  delete Rpy_sCont;
	  delete Zp_sCont;
	}
      else Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		      ": wrong order of continiuous Stochastic parameters");
    };

  // make solution and submit it to the SBSolution:
  Ps->updateSolution(P, Pp);
  Solution->submitStochasticParameters(Ps, Pp);

  // check for `cont' flag:
  NStochsCont = 0;
  for (i=0; i<Np; i++)
    if (Ps->at(i)->isAttr(SBDerivation::IsPropagated))
      NStochsCont++;

  if (NStochsCont)
    {
      if (!PsAux)
	{
	  PsAux = new SBParameterList;
	  PsAux->setAutoDelete(TRUE);
	}
      else 
	PsAux->clear();

      for (i=0; i<Np; i++)
	PsAux->append(new SBParameter(*Ps->at(i)));
      
      if (PAux)
	delete PAux;
      PAux = new SBVector(Np, "PAux");
      
      if (Pp_Aux)
	delete Pp_Aux;
      Pp_Aux = new SBSymMatrix(Np, "Pp_Aux");
    };
  //........


  if (Ps) {delete Ps; Ps = NULL;};
  SBVector	*Pprev = new SBVector(Np, "P_(n-1)");
  *Pprev = *P;

  SBSymMatrix	*Pp_prev = new SBSymMatrix(Np, "Pp_(prev)");
  *Pp_prev = *Pp;

  while (!StochStack.isEmpty())
    {
      Carrier = StochStack.pop();
      Ps    = Carrier->Ps;
      Rp_s  = Carrier->Rp_s;
      Rpp_s = Carrier->Rpp_s;
      Rpx_s = Carrier->Rpx_s;
      Rpy_s = Carrier->Rpy_s;
      Zp_s  = Carrier->Zp_s;

      // solve the parameters:
      //
      //     Rp_s * P = Zp_s - Rpp_s*PPrev - Rpx_s*X - Rpy_s*Y
      Solve(*Rp_s, *P, *Zp_s - *Rpp_s**Pprev - *Rpx_s**X - *Rpy_s**Y);
      
      SBMatrix QQQ = Rp_s->T();
      // here to add a calculations for covariance matrix:
      //  Pp = (Rp*)^(-1) * (Rp*)^(-T)
      RRT(*Pp, (*RpInv=~*Rp_s));
      //     Pp +      (Rp*)^(-1) * ((Rpp*) * Pp_prev * (Rpp*)^T) * (Rp*)^(-T)
      *Pp += QuadraticForm(QuadraticForm(*Pp_prev, *Rpp_s), *RpInv);
      if (Nx)
	//     Pp +      (Rp*)^(-1) * ((Rpx*) * Px * (Rpx*)^T) * (Rp*)^(-T)
	*Pp += QuadraticForm(QuadraticForm(*Px, *Rpx_s), *RpInv);
      if (Ny)
	//     Pp +      (Rp*)^(-1) * ((Rpy*) * Py * (Rpy*)^T) * (Rp*)^(-T)
	*Pp += QuadraticForm(QuadraticForm(*Py, *Rpy_s), *RpInv);
      
      Ps->updateSolution(P, Pp);
            
      // submit to the SBSolution:
      Solution->submitStochasticParameters(Ps, Pp);
      
      *Pprev = *P;
      *Pp_prev = *Pp;
      if (Carrier) delete Carrier;
    };


  // keep last calculated `cont' stochs:
  if (NStochsCont)
    {
      *PAux	= *P;
      *Pp_Aux	= *Pp;
    };
  //..............

  Solution->saveStochasticParameters(); // special case
  Solution->reportStochasticParameters();

  if (Idx)  {delete[] Idx; Idx  = NULL;};
  if (Rp)   {delete Rp;    Rp   = NULL;};
  if (Rpy)  {delete Rpy;   Rpy  = NULL;};
  if (Rpx)  {delete Rpx;   Rpx  = NULL;};
  if (Zp)   {delete Zp;    Zp   = NULL;};
  //  if (Ps)   {delete Ps;    Ps   = NULL;};
  if (P)    {delete P;	   P    = NULL;};
  if (Pp)   {delete Pp;	   Pp   = NULL;};
  if (Pprev){delete Pprev; Pprev= NULL;};
  if (Pp_prev){delete Pp_prev; Pp_prev= NULL;};
  if (RpInv){delete RpInv; RpInv= NULL;};
  StochStack.clear();
};

void SBEstimator::processObs(const SBMJD& T, const SBMJD& Tnext, const SBVector& O_C, const SBVector& Sigma)
{
  unsigned int	i;
  SBDerivation	*d=NULL;
  double	weight=0.0;
  
  // global parameters:
  for (i=0, d=Ys->first(); i<Ny; d=Ys->next(), i++)
    {
      Ay->set(Nz,i,  d->d()/Sigma.at(0));
      d->zero();
    };

  // local parameters:
  for (i=0, d=Xs->first(); i<Nx; d=Xs->next(), i++)
    {
      Ax->set(Nz,i,  d->d()/Sigma.at(0));
      d->zero();
    };

  // stochastic parameters:
  for (i=0, d=Ps->first(); i<Np; d=Ps->next(), i++)
    {
      Ap->set(Nz,i,  d->d()/Sigma.at(0));
      d->zero();
    };

  // vector Z:
  Z->set(Nz, O_C.at(0)/Sigma.at(0));
  Nz++;


  weight= 1.0/Sigma.at(0);

  Solution->statGeneral()->addWRMS(O_C.at(0), weight);

  if (Solution->stat4CurrentBatch())
    Solution->stat4CurrentBatch()->addWRMS(O_C.at(0), weight);
  
  if (Np && T<Tnext)
    {
      dataUpdate();
      propagateStochasticPars(T, Tnext-T);
      save4Smoothing();
      Ps->clearPValues();
    };

  TLastProcessed = T;
};

void SBEstimator::processConstr(double O_C, double Sigma)
{
  unsigned int	i;
  SBDerivation	*d=NULL;
  
  // global parameters (it goes to `all' list):
  for (i=0, d=Ys->first(); i<Ny; d=Ys->next(), i++)
    {
      Ay->set(Nz,i,  d->d()/Sigma);
      d->zero();
    };

  // local parameters:
  if (Xs)
    for (i=0, d=Xs->first(); i<Nx; d=Xs->next(), i++)
      {
	Ax->set(Nz,i,  d->d()/Sigma);
	d->zero();
      };
  
  // stochastic parameters:
  if (Ps)
    for (i=0, d=Ps->first(); i<Np; d=Ps->next(), i++)
      {
	Ap->set(Nz,i,  d->d()/Sigma);
	d->zero();
      };
  
  // vector Z:
  Z->set(Nz, O_C/Sigma);
  Nz++;

  //?  Solution->statGeneral()->addWRMS(O_C, 1.0/Sigma);
  Solution->statGeneral()->addNumConstr(Nz);

  if (Solution->stat4CurrentBatch())
    {
      //?      Solution->stat4CurrentBatch()->addWRMS(O_C, 1.0/Sigma);
      Solution->stat4CurrentBatch()->addNumConstr(Nz);
    };
  dataUpdate();
};

void SBEstimator::processGlobalConstr(double O_C, double Sigma)
{
  unsigned int	i;
  SBDerivation	*d=NULL;
  
  // global parameters (it goes to `all' list):
  for (i=0, d=YsAll->first(); i<NyAll; d=YsAll->next(), i++)
    {
      Ay->set(Nz,i,  d->d()/Sigma);
      d->zero();
    };

  // local parameters:
  if (Xs)
    for (i=0, d=Xs->first(); i<Nx; d=Xs->next(), i++)
      {
	Ax->set(Nz,i,  d->d()/Sigma);
	d->zero();
      };
  
  // stochastic parameters:
  if (Ps)
    for (i=0, d=Ps->first(); i<Np; d=Ps->next(), i++)
      {
	Ap->set(Nz,i,  d->d()/Sigma);
	d->zero();
      };
  
  // vector Z:
  Z->set(Nz, O_C/Sigma);
  Nz++;

  //?  Solution->statGeneral()->addWRMS(O_C, 1.0/Sigma);
  Solution->statGeneral()->addNumConstr(Nz);

  if (Solution->stat4CurrentBatch())
    {
      //?      Solution->stat4CurrentBatch()->addWRMS(O_C, 1.0/Sigma);
      Solution->stat4CurrentBatch()->addNumConstr(Nz);
    };
  dataUpdateGlobalConstr();
};

void SBEstimator::prepare4GlobConstr(int Nz_) // in need for global pars only
{
  // data:
  Nz = Nz_ + 8;
  Z   = new SBVector(Nz, "Z");
  Ay  = new SBMatrix(Nz, NyAll, "Ay");
  U   = new SBVector(Nz + 1, "U");
  Nz  = 0; // from here Nz is a count of observations

  NumOfStochs = 0;
};

void SBEstimator::finisGlobConstr()
{
  // data:
  if (Z)     {delete Z;     Z    = NULL;};
  //  if (Ap)    {delete Ap;    Ap   = NULL;};
  //  if (Ax)    {delete Ax;    Ax   = NULL;};
  if (Ay)    {delete Ay;    Ay   = NULL;};
  if (U)     {delete U;     U    = NULL;};
  Nz  = 0;
};

void SBEstimator::propagateStochasticPars(const SBMJD& /*T*/, double dT)
{
  // propagate the stochastic parameters from an epoch "T" to a new one "T+dT":
  unsigned int	i,j,l;
  unsigned int	n=2*Np, m=2*Np+Nx+Ny;
  double	s,g;
  
  SR->clear();
  U->clear();

  // fill the matrix SR:
  for (i=0; i<Np; i++)
    {
      SBParameter      *p  = Ps->at( i);
      double		mm = p ->m (dT);
      double		rwn= p ->rw(dT);

      SR->set(i,       i,   -mm*rwn);
      SR->set(i,    Np+i,       rwn);

      SR->set(Np+i,    m, Zp->at(i));
      
      for (j=i; j<Np; j++) SR->set(Np+i,      j,  Rp ->at(i, j));
      for (j=0; j<Nx; j++) SR->set(Np+i, n   +j,  Rpx->at(i, j));
      for (j=0; j<Ny; j++) SR->set(Np+i, n+Nx+j,  Rpy->at(i, j));
    };

  // make a Householder transformation
  // this should be optimized...
  for (l=0; l<n; l++)
    {
      for (i=l, s=0.0; i<n; i++) s+=SR->at(i,l)*SR->at(i,l);
      if (s>0.0)
	{
	  s = -sign(SR->at(l,l))*sqrt(s);
	  U->set(l, SR->at(l,l) - s);				// U_l
	  for (i=l+1; i<n; i++) U->set(i, SR->at(i,l));		// U_i (i=l+1, N)
	  SR->set(l,l, s);
	  s = 1.0/(s*U->at(l));					// betta
	  for (j=l+1; j<m+1; j++)				// |-------------------------
	    {							// |
	      for (i=l,g=0.0; i<n; i++) g+=U->at(i)*SR->at(i,j);// |
	      for (i=l; i<n; i++) (*SR)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	};
    };

  // fill the matrices for smoothing and filtering:
  for (i=0; i<Np; i++)
    {
      for (j=i; j<Np; j++)
	{
	  Rp_s ->set(i, j,	SR->at(   i,     j));
	  Rp   ->set(i, j,	SR->at(Np+i,  Np+j));
	};

      for (j=0; j<Np; j++)
	Rpp_s->set(i, j,	SR->at(i, Np  +j));
      
      for (j=0; j<Nx; j++)
	{
	  Rpx_s->set(i, j,	SR->at(   i, n+j));
	  Rpx  ->set(i, j,	SR->at(Np+i, n+j));
	};
      
      for (j=0; j<Ny; j++)
	{
	  Rpy_s->set(i, j,	SR->at(   i, n+Nx+j));
	  Rpy  ->set(i, j,	SR->at(Np+i, n+Nx+j));
	};

      Zp  ->set(i, SR->at(Np+i, m));
      Zp_s->set(i, SR->at(   i, m));
    };
};

void SBEstimator::dataUpdate()
{
  // make a Householder transformation
  unsigned int	i,j,l;
  double	s,ul,g;
  
  
  // Pass one: dealing with the "p":
  U->clear();							// U-vector, + ul -- the first elementh of U
  for (l=0; l<Np; l++)
    {
      s = Rp->at(l,l)*Rp->at(l,l);
      for (i=0; i<Nz; i++) s+=Ap->at(i,l)*Ap->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Rp->at(l,l))*sqrt(s);
	  ul = Rp->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, Ap->at(i,l));		// U_i (i=l+1, N)
	  Rp->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Np; j++)				// |<-- processing "Rp/Ap"-block
	    {							// |
	      g = ul*Rp->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ap->at(i,j);	// |
	      (*Rp)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ap)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  for (j=0; j<Nx; j++)					// |<-- processing "Rpx/Ax"-block
	    {							// |
	      g = ul*Rpx->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ax->at(i,j);	// |
	      (*Rpx)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ax)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  for (j=0; j<Ny; j++)					// |<-- processing "Rpy/Ay"-block
	    {							// |
	      g = ul*Rpy->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ay->at(i,j);	// |
	      (*Rpy)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ay)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________

	  g = ul*Zp->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zp
	  (*Zp)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	};
    };


  // Pass two: dealing with the "x":
  U->clear();							// U-vector, + ul -- the first elementh of U
  for (l=0; l<Nx; l++)
    {
      s = Rx->at(l,l)*Rx->at(l,l);
      for (i=0; i<Nz; i++) s+=Ax->at(i,l)*Ax->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Rx->at(l,l))*sqrt(s);
	  ul = Rx->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, Ax->at(i,l));		// U_i (i=l+1, N)
	  Rx->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Nx; j++)				// |<-- processing "Rx/Ax"-block
	    {							// |
	      g = ul*Rx->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ax->at(i,j);	// |
	      (*Rx)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ax)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  for (j=0; j<Ny; j++)					// |<-- processing "Rxy/Ay"-block
	    {							// |
	      g = ul*Rxy->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ay->at(i,j);	// |
	      (*Rxy)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ay)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________

	  g = ul*Zx->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zx
	  (*Zx)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	};
    };


  // Pass three: dealing with the "y":
  U->clear();							// U-vector, + ul -- the first elementh of U
  for (l=0; l<Ny; l++)
    {
      s = Ry->at(l,l)*Ry->at(l,l);
      for (i=0; i<Nz; i++) s+=Ay->at(i,l)*Ay->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Ry->at(l,l))*sqrt(s);
	  ul = Ry->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, Ay->at(i,l));		// U_i (i=l+1, N)
	  Ry->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Ny; j++)				// |<-- processing "Ry/Ay"-block
	    {							// |
	      g = ul*Ry->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ay->at(i,j);	// |
	      (*Ry)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ay)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________

	  g = ul*Zy->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zy
	  (*Zy)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	}
      else
	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		   ": got a zero during pass three at l[" + QString().setNum(l) + "]");
    };
  
  s = 0.0;
  for (i=0; i<Nz; i++) 
    s+= Z->at(i)*Z->at(i);

  Solution->statGeneral()->addWPFR(s);
  Solution->statGeneral()->addNumObs(Nz);
  if (Solution->stat4CurrentBatch())
    {
      Solution->stat4CurrentBatch()->addWPFR(s);
      Solution->stat4CurrentBatch()->addNumObs(Nz);
    };
  
  Nz = 0;
};

void SBEstimator::dataUpdateGlobalConstr()
{
  // make a Householder transformation
  unsigned int	i,j,l;
  double	s,ul,g;
  
  
  // Pass one: dealing with the "p":
  U->clear();							// U-vector, + ul -- the first elementh of U
  for (l=0; l<Np; l++)
    {
      s = Rp->at(l,l)*Rp->at(l,l);
      for (i=0; i<Nz; i++) s+=Ap->at(i,l)*Ap->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Rp->at(l,l))*sqrt(s);
	  ul = Rp->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, Ap->at(i,l));		// U_i (i=l+1, N)
	  Rp->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Np; j++)				// |<-- processing "Rp/Ap"-block
	    {							// |
	      g = ul*Rp->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ap->at(i,j);	// |
	      (*Rp)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ap)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  for (j=0; j<Nx; j++)					// |<-- processing "Rpx/Ax"-block
	    {							// |
	      g = ul*Rpx->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ax->at(i,j);	// |
	      (*Rpx)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ax)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  for (j=0; j<NyAll; j++)				// |<-- processing "Rpy/Ay"-block
	    {							// |
	      g = ul*Rpy->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ay->at(i,j);	// |
	      (*Rpy)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ay)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________

	  g = ul*Zp->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zp
	  (*Zp)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	};
    };


  // Pass two: dealing with the "x":
  U->clear();							// U-vector, + ul -- the first elementh of U
  for (l=0; l<Nx; l++)
    {
      s = Rx->at(l,l)*Rx->at(l,l);
      for (i=0; i<Nz; i++) s+=Ax->at(i,l)*Ax->at(i,l);
      if (s>0.0)
	{
	  s = -sign(Rx->at(l,l))*sqrt(s);
	  ul = Rx->at(l,l) - s;					// U_l
	  for (i=0; i<Nz; i++) U->set(i, Ax->at(i,l));		// U_i (i=l+1, N)
	  Rx->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<Nx; j++)				// |<-- processing "Rx/Ax"-block
	    {							// |
	      g = ul*Rx->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ax->at(i,j);	// |
	      (*Rx)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ax)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  for (j=0; j<NyAll; j++)				// |<-- processing "Rxy/Ay"-block
	    {							// |
	      g = ul*Rxy->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ay->at(i,j);	// |
	      (*Rxy)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ay)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________

	  g = ul*Zx->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zx
	  (*Zx)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	};
    };


  // Pass three: dealing with the "y":
  U->clear();							// U-vector, + ul -- the first elementh of U
  for (l=0; l<NyAll; l++)
    {
      s = RyAll->at(l,l)*RyAll->at(l,l);
      for (i=0; i<Nz; i++) s+=Ay->at(i,l)*Ay->at(i,l);
      if (s>0.0)
	{
	  s = -sign(RyAll->at(l,l))*sqrt(s);
	  ul = RyAll->at(l,l) - s;				// U_l
	  for (i=0; i<Nz; i++) U->set(i, Ay->at(i,l));		// U_i (i=l+1, N)
	  RyAll->set(l,l, s);
	  s = 1.0/(s*ul);					// betta
	  for (j=l+1; j<NyAll; j++)				// |<-- processing "Ry/Ay"-block
	    {							// |
	      g = ul*RyAll->at(l,j);				// |
	      for (i=0; i<Nz; i++) g+=U->at(i)*Ay->at(i,j);	// |
	      (*RyAll)(l,j)+=s*g*ul;				// |
	      for (i=0; i<Nz; i++) (*Ay)(i,j)+=s*g*U->at(i);	// |
	    };							// |_________________________
	  
	  g = ul*ZyAll->at(l);					// |--
	  for (i=0; i<Nz; i++) g+=U->at(i)*Z->at(i);		// | dealing with the vectors Zy
	  (*ZyAll)(l)+=s*g*ul;					// |		and Z
	  for (i=0; i<Nz; i++) (*Z)(i)+=s*g*U->at(i);		// |--
	}
      else
	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		   ": got a zero during pass three at l[" + QString().setNum(l) + "]");
    };
  
  s = 0.0;
  for (i=0; i<Nz; i++) 
    s+= Z->at(i)*Z->at(i);

  Solution->statGeneral()->addWPFR(s);
  Solution->statGeneral()->addNumObs(Nz);
  if (Solution->stat4CurrentBatch())
    {
      Solution->stat4CurrentBatch()->addWPFR(s);
      Solution->stat4CurrentBatch()->addNumObs(Nz);
    };
  
  Nz = 0;
};

#include <qapplication.h>
void SBEstimator::moveGlobalInfo()
{
  
  SBParameter	*Par;
  int		idx;
  unsigned int	i,j,l;
  int		*Idx = new int[Ny+1];		// an indexes of elements of Ys into YsAll
  int		*IdxRev = new int[NyAll];	// 
  double	s,ul,g;
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": starting transfer global info");
  qApp->processEvents();

  for (i = 0; i<Ny; i++)
    if ((idx = YsAll->idx( (Par=Ys->at(i)) ))!=-1)
      *(Idx+i) = idx;
    else
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": cannot find the global parameter [" + Par->name() + 
		 "] in the list of all global parameters");
  *(Idx+Ny)=NyAll+1;
  j = 0;
  for (i = 0; i<NyAll; i++)
    {
      if (*(Idx+j)==(int)i)
	j++;
      *(IdxRev+i) = j - 1;
      if (j>Ny)
	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": ERROR IN IDXes ORDER.");
    };
  
  SBMatrix	*R_= new SBMatrix(Ny, NyAll, "R_");	// this is Ry
  SBVector	*Z_= new SBVector(Ny, "Z_");		// this is Zy
  SBVector	*U_= new SBVector(Ny, "U_");
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": transfer global info: mapping");
  qApp->processEvents();

  *Z_ = *Zy;
  for (i=0; i<Ny; i++)
    {
      //      Z_->set(i, Zy->at(i));
      for (j=i; j<Ny; j++)
	R_->set(i, *(Idx+j),  Ry->at(i,j));
    };
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": transfer global info: Householdering");
  qApp->processEvents();

  // Householdering:
  for (l=0; l<NyAll; l++)
    {
      s = 0.0;
      for (i=0; (int)i<=*(IdxRev+l); i++) 
	s+=R_->at(i,l)*R_->at(i,l);
      if (s>0.0)
	{
	  s += RyAll->at(l,l)*RyAll->at(l,l);
	  
	  s = -sign(RyAll->at(l,l))*sqrt(s);
	  ul = RyAll->at(l,l) - s;						// U_l
	  for (i=0; (int)i<=*(IdxRev+l); i++) U_->set(i, R_->at(i,l));		// U_i (i=l+1, N)
	  RyAll->set(l,l, s);
	  s = 1.0/(s*ul);							// betta  !add betta!
	  for (j=l+1; j<NyAll; j++)
	    {
	      g = ul*RyAll->at(l,j);
	      for (i=0; (int)i<=*(IdxRev+l); i++) 
		g+=U_->at(i)*R_->at(i,j);
	      (*RyAll)(l,j)+=s*g*ul;
	      for (i=0; (int)i<=*(IdxRev+l); i++) 
		(*R_)(i,j)+=s*g*U_->at(i);
	    };
	  g = ul*ZyAll->at(l);					// |--
	  for (i=0; (int)i<=*(IdxRev+l); i++)				// |
	    g+=U_->at(i)*Z_->at(i);					// | dealing with the vectors ZyAll
	  (*ZyAll)(l)+=s*g*ul;					// |		and Zy
	  for (i=0; (int)i<=*(IdxRev+l); i++)				// |
	    (*Z_)(i)+=s*g*U_->at(i);				// |--
	};
    };
  
  // here need to add Z_ to the vector E:
  // Z_->module(); ???
  
  s = 0.0;
  for (i=0; i<Ny; i++) 
    s+= Z_->at(i)*Z_->at(i);

  Solution->statGeneral()->addWPFR(s);
  if (Solution->stat4CurrentBatch())
    Solution->stat4CurrentBatch()->addWPFR(s);
  
  delete[] Idx;
  delete[] IdxRev;
  delete R_;
  delete Z_;
  delete U_;

  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": transfer of global info finished");
};

void SBEstimator::moveGlobalInfo_Old()
{
  
  SBParameter	*Par;
  int		idx;
  unsigned int	i,j,l;
  int		*Idx = new int[Ny];	// an indexes of elements of Ys into YsAll
  double	s,ul,g;
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": starting transfer global info");
  qApp->processEvents();

  for (i = 0; i<Ny; i++)
    if ((idx = YsAll->idx( (Par=Ys->at(i)) ))!=-1)
      *(Idx+i) = idx;
    else
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": cannot find the global parameter [" + Par->name() + 
		 "] in the list of all global parameters");
  
  SBUpperMatrix	*R_= new SBUpperMatrix(NyAll, "R_");	// this is Ry
  SBVector	*Z_= new SBVector(NyAll, "Z_");		// this is Zy
  SBVector	*U_= new SBVector(NyAll, "U_");
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": transfer global info: mapping");
  qApp->processEvents();

  for (i=0; i<Ny; i++)
    {
      Z_->set( *(Idx+i), Zy->at(i));
      for (j=i; j<Ny; j++)
	R_->set(*(Idx+i),*(Idx+j),  Ry->at(i,j));
    };
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": transfer global info: Householdering");
  qApp->processEvents();

  // Householdering:
  for (l=0; l<NyAll; l++)
    {
      s = RyAll->at(l,l)*RyAll->at(l,l);
      for (i=0; i<=l; i++) s+=R_->at(i,l)*R_->at(i,l);
      if (s>0.0)
	{
	  s = -sign(RyAll->at(l,l))*sqrt(s);
	  ul = RyAll->at(l,l) - s;				// U_l
	  for (i=0; i<=l; i++) U_->set(i, R_->at(i,l));		// U_i (i=l+1, N)
	  RyAll->set(l,l, s);
	  s = 1.0/(s*ul);					// betta  !add betta!
	  for (j=l+1; j<NyAll; j++)
	    {
	      g = ul*RyAll->at(l,j);
	      for (i=0; i<=l; i++) g+=U_->at(i)*R_->at(i,j);
	      (*RyAll)(l,j)+=s*g*ul;
	      for (i=0; i<=l; i++) (*R_)(i,j)+=s*g*U_->at(i);
	    };
	  g = ul*ZyAll->at(l);					// |--
	  for (i=0; i<=l; i++) g+=U_->at(i)*Z_->at(i);		// | dealing with the vectors ZyAll
	  (*ZyAll)(l)+=s*g*ul;					// |		and Zy
	  for (i=0; i<=l; i++) (*Z_)(i)+=s*g*U_->at(i);		// |--
	};
    };

  // here need to add Z_ to the vector E:
  // Z_->module(); ???

  s = 0.0;
  for (i=0; i<NyAll; i++) 
    s+= Z_->at(i)*Z_->at(i);

  Solution->statGeneral()->addWPFR(s);
  if (Solution->stat4CurrentBatch())
    Solution->stat4CurrentBatch()->addWPFR(s);
  
  delete[] Idx;
  delete R_;
  delete Z_;
  delete U_;

  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + ": transfer of global info finished");
};

void SBEstimator::collectContStochs4NextBatch()
{
  unsigned int	i,j;
  int		*Idx_Y = new int[Ny];		// an indexes of elements

  // delete them if they're exist still
  if (ZpCont)
    delete ZpCont;
  if (RpCont)
    delete RpCont;
  if (RpyCont)
    delete RpyCont;
  
  PsCont ->clear();

  ZpCont = new SBVector(Np, "ZpCont");
  RpCont = new SBUpperMatrix(Np, "RpCont");
  RpyCont= new SBMatrix(Np, NyAll, "RpyCont");

  // HAVE TO MAKE AN REARANGING OF PARAMETERS ORDER!



  // make copy of parameter lists:
  //  std::cout << "parameters: \n";
  for (i=0; i<Np; i++)
    //    {
    //      std::cout << Ps->at(i)->name() << ": " << QString().setNum(i) << "\n"; 
    PsCont->append(Ps->at(i));
  //    };
  //  std::cout << "\n";

  SBParameter	*Par=NULL;
  int		*idx_y=Idx_Y;
  for (i=0; i<Ny; i++)
    if ( (*idx_y++=YsAll->idx((Par=Ys->at(i))))==-1 )
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": cannot find the global parameter [" + Par->name() + 
		 "] in the list of all global parameters");
  
  // make a copy of matrices & vector:
  *ZpCont = *Zp;
  *RpCont = *Rp;

  for (i=0; i<Np; i++)
    for (j=0; j<Ny; j++)
      RpyCont->set(i, *(Idx_Y+j),   Rpy->at( i, j) );

  delete[] Idx_Y;
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
	     ": stochastic parameters have been saved for next batch propagation");
};

void SBEstimator::mapContStochs4NewBatch(const SBMJD& Tnew)
{
  if (!PsCont)
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": the list of `continuous' stochastic parameters is NULL");
      return;
    };
  if (!ZpCont)
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": the vector Zp of `continuous' stochastic parameters is NULL");
      return;
    };
  if (!RpCont)
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": the matrix Rp of `continuous' stochastic parameters is NULL");
      return;
    };
  if (!RpyCont)
    {
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": the matrix Rpy of `continuous' stochastic parameters is NULL");
      return;
    };

  // now, propagate the stochastic parameters from an epoch "TLastProcessed" to a new one "Tnew":
  unsigned int	i,j,l;
  double	s,g;
  double	dT = Tnew - TLastProcessed;

  SBParameterList *PsTmp = new SBParameterList; // PsCont AND Ps
  PsTmp->setAutoDelete(FALSE);

  for (i=0; i<PsCont->count(); i++)
    if (Ps->idx(PsCont->at(i)) != -1 && PsCont->at(i)->isAttr(SBDerivation::IsPropagated))
      PsTmp->append(PsCont->at(i));
  
  unsigned int NumTmp = PsTmp->count();
  int		*Idx_POld = new int[NumTmp];
  int		*Idx_PNew = new int[NumTmp];
  int		*Idx_Y = new int[Ny];		// an indexes of elements
  for (i=0; i<NumTmp; i++)
    {
      *(Idx_POld+i) = PsCont->idx(PsTmp->at(i));
      *(Idx_PNew+i) = Ps    ->idx(PsTmp->at(i));
    };  
  
  SBParameter	*Par=NULL;
  int		*idx_y=Idx_Y;
  for (i=0; i<Ny; i++)
    if ( (*idx_y++=YsAll->idx((Par=Ys->at(i))))==-1 )
      Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + 
		 ": cannot find the global parameter [" + Par->name() + 
		 "] in the list of all global parameters");

  unsigned int	n = 2*NumTmp, m = 2*NumTmp + NyAll;

  // for tieing parameters between batches:
  SBUpperMatrix *Rp_sCont = new SBUpperMatrix(NumTmp, "Rp_sCont");
  SBMatrix      *Rpp_sCont= new SBMatrix(NumTmp, NumTmp, "Rpp_sCont");
  SBMatrix      *Rpy_sCont= new SBMatrix(NumTmp, NyAll, "Rpy_sCont");
  SBVector      *Zp_sCont = new SBVector(NumTmp, "Zp_sCont");

  SBMatrix	*SRCont = new SBMatrix(n, m + 1, "SRCont");
  SBVector	*UCont  = new SBVector(n + 1, "UCont");

  // fill the matrix SRCont:
  for (i=0; i<NumTmp; i++)
    {
      SBParameter      *p  = PsTmp->at(i);
      double		mm = p ->m (dT);
      double		rwn= p ->rw(dT);

      SRCont->set(i,        i,   -mm*rwn);
      SRCont->set(i, NumTmp+i,       rwn);

      SRCont->set(NumTmp+i, m, ZpCont->at(*(Idx_POld+i)));

      for (j=i; j<NumTmp; j++) 
	SRCont->set(NumTmp+i,  j,  RpCont ->at(*(Idx_POld+i), *(Idx_POld+j)));

      for (j=0; j<NyAll;  j++)
	SRCont->set(NumTmp+i, n+j, RpyCont->at(*(Idx_POld+i), j));
    };

  // make a Householder transformation
  // this should be optimized too..
  for (l=0; l<n; l++)
    {
      for (i=l, s=0.0; i<n; i++) s+=SRCont->at(i,l)*SRCont->at(i,l);
      if (s>0.0)
	{
	  s = -sign(SRCont->at(l,l))*sqrt(s);
	  UCont->set(l, SRCont->at(l,l) - s);				// U_l
	  for (i=l+1; i<n; i++) UCont->set(i, SRCont->at(i,l));		// U_i (i=l+1, N)
	  SRCont->set(l,l, s);
	  s = 1.0/(s*UCont->at(l));					// betta
	  for (j=l+1; j<m+1; j++)					// |-------------------------
	    {								// |
	      for (i=l,g=0.0; i<n; i++) g+=UCont->at(i)*SRCont->at(i,j);// |
	      for (i=l; i<n; i++) (*SRCont)(i,j)+=s*g*UCont->at(i);	// |
	    };								// |_________________________
	};
    };

  // fill the matrices for smoothing and filtering:
  for (i=0; i<NumTmp; i++)
    {      
      for (j=i; j<NumTmp; j++)
	{
	  Rp_sCont->set(           i,             j,	SRCont->at(       i,        j));
	  Rp      ->set(*(Idx_PNew+i), *(Idx_PNew+j),	SRCont->at(NumTmp+i, NumTmp+j));
	};

      for (j=0; j<NumTmp; j++)
	Rpp_sCont->set(i, j,				SRCont->at(i,        NumTmp+j));

      for (j=0; j<NyAll; j++)
	Rpy_sCont->set(           i,  j,	SRCont->at(       i,  n+j));

      for (j=0; j<Ny; j++)
	Rpy      ->set(*(Idx_PNew+i), j,	SRCont->at(NumTmp+i,  n+*(Idx_Y+j)));
      
      Zp      ->set(*(Idx_PNew+i), SRCont->at(NumTmp+i, m));
      Zp_sCont->set(i, SRCont->at(   i, m));
    };

  // save for smoothing:
  // here BatchName is the name of previous batch still
  QFile f(workDir4Loc(BatchName) + "stochastics_aux.bin");
  if (!f.open(IO_WriteOnly))
    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot open the file [" + f.name() + 
	       "] for writting");
  QDataStream data_stream(&f);
  data_stream << *PsTmp << *Zp_sCont << *Rp_sCont << *Rpp_sCont << *Rpy_sCont;
  data_stream.unsetDevice();
  f.close();

  delete PsTmp;
  delete Rp_sCont;
  delete Rpp_sCont;
  delete Rpy_sCont;
  delete Zp_sCont;
  delete SRCont;
  delete UCont;

  delete[] Idx_Y;
  delete[] Idx_PNew;
  delete[] Idx_POld;
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR, ClassName() + 
	     ": `continuous' stochastic parameters have been propagated for new batch");
};
/*==============================================================================================*/



/*==============================================================================================*/
