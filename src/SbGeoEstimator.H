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
 * Overview:	Estimator & company;
 *
 */


#ifndef SB_GEO_ESTIMATOR_H
#define SB_GEO_ESTIMATOR_H

#include <config.h>

#include "SbGeo.H"
#include "SbGeoProject.H"
#include "SbGeoParameter.H"

#include <qdatastream.h>
#include <qfile.h>
#include <qintdict.h>
#include <qlist.h>
#include <qstack.h>
#include <qstring.h>




/*==============================================================================================*/
/**Class SBEstimator, describes estimator.
 *
 *
 */
/*==============================================================================================*/
class SBEstimator
{
protected:
  class GroupList;
  /**Class SBEstimator::Group introduces a part of the information array.
   * It is assumed that elements of the information array are separated on 
   * the basis of the TTL value.
   */
  class Group : public SBParameterList, public SBMJD
  {
  protected:
    SBUpperMatrix	*R;			//!< diagonal matrix for the set of par-s.
    QList<SBMatrix>	*CrossCorr;		//!< the list of non-diagonal matrices;
    SBVector		*Z;			//!< z-vector corresponded to the set of par-s;
  public:
    Group(const SBMJD& =TZero);
    ~Group();
    /**Adds a parameter for the set.*/
    void addParameter(const SBParameter*);
    /**Deletes a parameter from the set.*/
    void delParameter(const SBParameter*);
    /**Prepares the set and aux data for running.*/
    void beforeRun(int, GroupList*);
    /**Finishes the set and aux data after running.*/
    void afterRun(int, GroupList*);
    /**Returns the TTL value.*/
    const SBMJD& ttl(){return *this;};
  };
  
  /**SBMJDList.
   * List of MJDs, ordered by date value.
   *
   */
  class GroupList : public QList<Group>
  {
  protected:
    QDict<Group>	*ValByDate;
    virtual int		compareItems(QCollection::Item p1, QCollection::Item p2)
      {return *(SBMJD*)p1<*(SBMJD*)p2?-1:*(SBMJD*)p1==*(SBMJD*)p2?0:1;};
  public:
    // constructors/destructors:
    //
    /**A constructor.
     * Creates a copy of the object.
     */
    GroupList() : QList<Group>()
      {
	ValByDate = new QDict<Group>(20);
	setAutoDelete(TRUE);
	ValByDate->setAutoDelete(FALSE);
      };
    /**A constructor.
     * Creates a copy of the object.
     * \param L_ is value to copy in.
     */
    GroupList(const GroupList& L_) : QList<Group>() {*this=L_;};
    GroupList& operator= (const GroupList&);
    /**A destructor.*/
    virtual ~GroupList(){if (ValByDate) delete ValByDate;};
    /**Refers to a class name (debug info).*/
    virtual QString	ClassName()	const {return "GroupList";};
    
    // interface:
    //
    
    // functions:
    //
    virtual void		inSort(const Group*);
    virtual bool		remove(const QString&);
    virtual bool		remove(const Group*);
    virtual Group*		find  (const SBMJD& t_) const
      {return ValByDate?ValByDate->find(t_.toString(SBMJD::F_INTERNAL)):NULL;};
    
  private: // disabled:
    virtual bool		remove(uint idx_)
      {return 
	 at(idx_)?ValByDate->remove(at(idx_)->toString(SBMJD::F_INTERNAL)) &&
	 QList<Group>::remove(idx_):FALSE;};
    virtual bool		remove()
      {return 
	 current()?ValByDate->remove(current()->toString(SBMJD::F_INTERNAL)) &&
	 QList<Group>::remove():FALSE;};
  public:
    
    
    // I/O:
    //
  };

  class SmoothCarrier
  {
  public:
    SBParameterList		*Ps;
    SBUpperMatrix		*Rp_s;
    SBMatrix			*Rpp_s;
    SBMatrix			*Rpx_s;
    SBMatrix			*Rpy_s;
    SBVector			*Zp_s;

    SmoothCarrier() {Ps=NULL; Rp_s=NULL; Rpp_s=NULL; Rpx_s=NULL; Rpy_s=NULL; Zp_s=NULL;};
    ~SmoothCarrier() 
      {
	if (Ps) delete Ps; if (Rp_s) delete Rp_s; if (Rpp_s) delete Rpp_s; 
	if (Rpx_s) delete Rpx_s; if (Rpy_s) delete Rpy_s; if (Zp_s) delete Zp_s;
      };
    void			load(QDataStream &s) 
      {
	Ps   = new SBParameterList;
	Ps->setAutoDelete(TRUE);
	Rp_s = new SBUpperMatrix;
	Rpp_s= new SBMatrix;
	Rpx_s= new SBMatrix;
	Rpy_s= new SBMatrix;
	Zp_s = new SBVector;
	s >> *Ps >> *Zp_s >> *Rp_s >> *Rpp_s >> *Rpx_s >> *Rpy_s;
      };
  };
  /*
   */
  
private:
  // general stuff:
  SBProject			*Prj;		//!< project (not an owner of);
  SBRunManager			*RunMgr;	//!< run manager (not an owner of);
  SBSolution			*Solution;
  //  GroupList			*Groups;	//!< groups of parameters;
  //  GroupList			*NewGroups;	//!< groups of parameters;
  //  SBParameterList		Pars;		//!< list of parameters;
  
  // Global parameters:
  unsigned int			NyAll;
  SBParameterList		*YsAll;		//!< list of all global parameters;
  SBVector			*YAll;
  SBVector			*ZyAll;
  SBUpperMatrix			*RyAll;
  SBSymMatrix			*PyAll;

  unsigned int			Ny;
  SBParameterList		*Ys;		//!< list of global parameters;
  SBVector			*Y;
  SBVector			*Zy;
  SBUpperMatrix			*Ry;
  SBSymMatrix			*Py;


  // Local parameters:
  unsigned int			Nx;
  SBParameterList		*Xs;		//!< list of local parameters;
  SBVector			*X;
  SBVector			*Zx;
  SBUpperMatrix			*Rx;
  SBMatrix			*Rxy;
  SBSymMatrix			*Px;
  SBMJD				TofBatch;

  // Stochastic parameters:
  unsigned int			Np;
  SBParameterList		*Ps;		//!< list of stochastic parameters;
  SBVector			*P;
  SBVector			*Zp;
  SBUpperMatrix			*Rp;
  SBMatrix			*Rpx;
  SBMatrix			*Rpy;
  SBSymMatrix			*Pp;
  SBMatrix			*SR;
  // 4smoothing:
  SBUpperMatrix			*Rp_s;
  SBMatrix			*Rpp_s;
  SBMatrix			*Rpx_s;
  SBMatrix			*Rpy_s;
  SBVector			*Zp_s;
  // 4propagating:
  SBMJD				TLastProcessed;
  unsigned int			NStochsCont;
  SBParameterList		*PsCont;
  SBVector			*ZpCont;
  SBUpperMatrix			*RpCont;
  SBMatrix			*RpyCont;
  SBParameterList		*PsAux; // auxiliary list of stoch. parameters
  SBVector			*PAux;	// vector of aux. stochs
  SBSymMatrix			*Pp_Aux;
  

  // Data:
  unsigned int			Nz;
  SBVector			*Z;
  SBMatrix			*Ap;
  SBMatrix			*Ax;
  SBMatrix			*Ay;
  SBVector			*U;

  // aux:
  QString			WorkDir;
  QString			BatchName;
  QFile				file4Stoch;
  QDataStream			stream4Stoch;
  FILE				*pipe4Stoch;
  unsigned int			NumOfStochs;
  QStack<QString>		Stack;
  
  bool				IsNeedRmTemp;

  // functions:
  void				moveGlobalInfo();
  void				moveGlobalInfo_Old();
  void				propagateStochasticPars(const SBMJD&, double);
  void				save4Smoothing() 
    {stream4Stoch << *Ps << *Zp_s << *Rp_s << *Rpp_s << *Rpx_s << *Rpy_s; NumOfStochs++;};
  void				dataUpdate();
  void				dataUpdateGlobalConstr();
  void				solveGlobals();
  void				solveLocals();
  void				solveStochs(const QString*, int);
  QString			workDir4Loc(const QString& BatchName_) {return WorkDir + BatchName_ + "/";}

  void				collectContStochs4NextBatch();
  void				mapContStochs4NewBatch(const SBMJD&);

public:
  // constructors/destructors:
  //
  SBEstimator(SBProject*, SBRunManager*, SBSolution*);
  virtual ~SBEstimator();
  virtual QString ClassName() const {return "SBEstimator";};
  
  // interface:
  //
  
  // functions:
  //
  virtual void		prepare4Run(SBParameterList*);
  virtual void		parameterListsAboutToChange();
  virtual void		parameterListsChanged();
  virtual void		prepare4Local(int, const SBMJD&);
  virtual void		finisRun();
  virtual void		finisLocal();
  virtual void		processObs(const SBMJD&, const SBMJD&, const SBVector&, const SBVector&);
  virtual void		addParameter(const SBParameter*, const SBMJD&);
  virtual void		delParameter(const SBParameter*, const SBMJD&);
  virtual void		rearrangeGroups();

  virtual void		prepare4GlobConstr(int);
  virtual void		processConstr(double, double);
  virtual void		processGlobalConstr(double, double);
  virtual void		finisGlobConstr();

  // I/O:
  //
};
/*==============================================================================================*/





#endif //SB_GEO_ESTIMATOR_H
