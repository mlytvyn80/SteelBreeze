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


#include "sys/utsname.h"


#include "SbGeoSINEX.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"

#include <qmessagebox.h>
#include <qfile.h>
//#include <qregexp.h>

#include "SbSetup.H"
#include "SbGeoAtmLoad.H"
#include "SbGeoDelay.H"
#include "SbGeoEop.H"
#include "SbGeoProject.H"
#include "SbGeoRefFrame.H"

#undef METEO_PARAMETERS_IN_TRO_FILE


/*==============================================================================================*/
/*												*/
/* class SBSINEXParameter implementation							*/
/*												*/
/*==============================================================================================*/
SBSINEXParameter::SBSINEXParameter(const SBSINEXParameter &P)
  : SBParameter(P)
{
  APrioriValue = 0.0;
  Idx = 0;
  Type = ST_UNKN;
  SiteCode = "";
  PointCode = "";
  SolutionID = "";
  TValidEpoch = TZero;
  ConstraintCode = "";
};

SBSINEXParameter::SBSINEXParameter(SBParameter *P)
  : SBParameter(*P)
{
  APrioriValue = 0.0;
  Idx = 0;
  Type = ST_UNKN;
  SiteCode = "";
  PointCode = "";
  SolutionID = "";
  TValidEpoch = TZero;
  ConstraintCode = "";
};

QString SBSINEXParameter::parameterUnits()
{
  QString	Ret("foot");
  switch (Type)
    {
    case ST_STAX: 
    case ST_STAY: 
    case ST_STAZ:
      Ret = "m";
      break;

    case ST_VELX:
    case ST_VELY:
    case ST_VELZ:
      Ret = "m/y";
      break;

    case ST_RS_RA:
    case ST_RS_DE:
      Ret = "rad";
      break;

    case ST_UT:
      Ret = "ms";
      break;

    case ST_XPO:
    case ST_YPO:
      Ret = "mas";
      break;

    case ST_NUT_LN:
    case ST_NUT_OB:
      Ret = "mas";
      break;


    default:
    case ST_UNKN:
      Ret = "foot";
    };

  return Ret;
};

QString SBSINEXParameter::parameterType()
{
  QString	Ret("UNKN");
  switch (Type)
    {
    case ST_STAX: Ret="STAX"; break;
    case ST_STAY: Ret="STAY"; break;
    case ST_STAZ: Ret="STAZ"; break;
    case ST_VELX: Ret="VELX"; break;
    case ST_VELY: Ret="VELY"; break;
    case ST_VELZ: Ret="VELZ"; break;

    case ST_RS_RA: Ret="RS_RA"; break;
    case ST_RS_DE: Ret="RS_DE"; break;

    case ST_UT:  Ret="UT"; break;
    case ST_XPO: Ret="XPO"; break;
    case ST_YPO: Ret="YPO"; break;
    case ST_NUT_LN: Ret="NUT_LN"; break;
    case ST_NUT_OB: Ret="NUT_OB"; break;

    default:
    case ST_UNKN: Ret = "UNKN";
    };

  return Ret;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBTropRecord implementation								*/
/*												*/
/*==============================================================================================*/
SBTropRecord::SBTropRecord() 
  : SBMJD(TZero)
{
  aleph[0] = aleph[1] = aleph[2] = 0.0;
  beta [0] = beta [1] = beta [2] = 0.0;
  gamma[0] = gamma[1] = gamma[2] = 0.0;
  delta[0] = delta[1] = delta[2] = 0.0;
  TSum = 0.0;
  Temp = 0.0;
  Pres = 0.0;
  RH   = 0.0;

  TotZenith	= 0.0;
  WetZenith	= 0.0;
  WetZenithErr	= 0.0;
  GradNorth	= 0.0;
  GradNorthErr	= 0.0;
  GradEast	= 0.0;
  GradEastErr	= 0.0;
  NumPts	= 0;
  NumProc	= 0;
  IsCollecting	= FALSE;
};

SBTropRecord::SBTropRecord(const SBTropRecord& TR)
  : SBMJD(TR)
{
  aleph[0] = aleph[1] = aleph[2] = 0.0;
  beta [0] = beta [1] = beta [2] = 0.0;
  gamma[0] = gamma[1] = gamma[2] = 0.0;
  delta[0] = delta[1] = delta[2] = 0.0;
  TSum = 0.0;
  NumProc = 0;

  TotZenith	= TR.TotZenith;
  WetZenith	= TR.WetZenith;
  WetZenithErr	= TR.WetZenithErr;
  GradNorth	= TR.GradNorth;
  GradNorthErr	= TR.GradNorthErr;
  GradEast	= TR.GradEast;
  GradEastErr	= TR.GradEastErr;
  Temp		= TR.Temp;
  Pres		= TR.Pres;
  RH		= TR.RH;
  NumPts	= TR.NumPts;
  IsCollecting	= TR.IsCollecting;
};

void SBTropRecord::clear()
{
  TotZenith	= 0.0;
  WetZenith	= 0.0;
  WetZenithErr	= 0.0;
  GradNorth	= 0.0;
  GradNorthErr	= 0.0;
  GradEast	= 0.0;
  GradEastErr	= 0.0;
  Temp		= 0.0;
  Pres		= 0.0;
  RH		= 0.0;
  NumPts	= 0;
  IsCollecting  = FALSE;

  aleph[0] = aleph[1] = aleph[2] = 0.0;
  beta [0] = beta [1] = beta [2] = 0.0;
  gamma[0] = gamma[1] = gamma[2] = 0.0;
  delta[0] = delta[1] = delta[2] = 0.0;
  TSum = 0.0;
  NumProc = 0;
};

void SBTropRecord::collect(SBTropRecord* Tr)
{
  if (!IsCollecting)
    {
      clear();
      IsCollecting = TRUE;
    };

  if (TSum == 0.0)
    TSum = Tr->NumPts**(SBMJD*)Tr;
  else
    TSum += Tr->NumPts**(SBMJD*)Tr;


  double p0,p1,p2;

  p0 = Tr->WetZenithErr*Tr->WetZenithErr;
  p1 = Tr->GradNorthErr*Tr->GradNorthErr;
  p2 = Tr->GradEastErr *Tr->GradEastErr;
  
  aleph[0] += 1.0/p0;	beta[0] += Tr->WetZenith/p0;	gamma[0] += Tr->WetZenith*Tr->WetZenith/p0;
  aleph[1] += 1.0/p1;	beta[1] += Tr->GradNorth/p1;	gamma[1] += Tr->GradNorth*Tr->GradNorth/p1;
  aleph[2] += 1.0/p2;	beta[2] += Tr->GradEast /p2;	gamma[2] += Tr->GradEast *Tr->GradEast /p2;

  delta[0] += p0;
  delta[1] += p1;
  delta[2] += p2;

  TotZenith+= Tr->TotZenith/p0;
  NumPts   += Tr->NumPts;
  Temp	   += Tr->Temp;
  Pres	   += Tr->Pres;
  RH	   += Tr->RH;
  NumProc++;
};

void SBTropRecord::processCollected()
{
  if (!IsCollecting || !NumPts)
    return;
  
  IsCollecting = FALSE;

  if (NumProc==1)
    {
      TotZenith     /= aleph[0];
      
      WetZenith    = beta[0]/aleph[0];
      WetZenithErr = sqrt(1.0/aleph[0]);
      
      GradNorth    = beta[1]/aleph[1];
      GradNorthErr = sqrt(1.0/aleph[1]);
      
      GradEast     = beta[2]/aleph[2];
      GradEastErr  = sqrt(1.0/aleph[2]);
    }
  else
    {
      TotZenith     /= aleph[0];
      
      WetZenith    = beta[0]/aleph[0];
      // dispersion:
      //      WetZenithErr = sqrt((gamma[0] - beta[0]*beta[0]/aleph[0])/aleph[0]);
      //      WetZenithErr = sqrt(delta[0]/NumProc);
      WetZenithErr = sqrt(delta[0]/NumPts);
      
      GradNorth    = beta[1]/aleph[1];
      // dispersion:
      //      GradNorthErr = sqrt((gamma[1] - beta[1]*beta[1]/aleph[1])/aleph[1]);
      //      GradNorthErr = sqrt(delta[1]/NumProc);
      GradNorthErr = sqrt(delta[1]/NumPts);
      
      GradEast     = beta[2]/aleph[2];
      // dispersion:
      //      GradEastErr  = sqrt((gamma[2] - beta[2]*beta[2]/aleph[2])/aleph[2]);
      //      GradEastErr  = sqrt(delta[2]/NumProc);
      GradEastErr  = sqrt(delta[2]/NumPts);
      Temp	   /= NumProc;
      Pres	   /= NumProc;
      RH	   /= NumProc;
    };
};
/*==============================================================================================*/







/*==============================================================================================*/
void collectListOfSINEXParameters(SBParameterList* LastGlb, SBSymMatrix* LastP, 
				  SBSINEXParameterList* SINEX_List, SBSymMatrix*& SINEX_P, 
				  SBRunManager* RunMgr)
{
  //SBParameterList Global = RunMgr->solution()->global();

  SB_TRF		*TRF = RunMgr->trf();
  SBStationID		id;
  SBStation		*Station = NULL;
  SBStationInfo		*SI = NULL;
  SBSINEXParameter	*S_Par = NULL;
  int			Idx=0;
  int			IdxSite=0;
  QString		SiteCode("");

  
  SINEX_List->clear();
  
  for (SBParameter *p=LastGlb->first(); p; p=LastGlb->next())
    {
      if (p->name().contains("St:"))
	{
	  id.setID(p->name().mid(4, 9));
	  if (id.isValidId() && (Station=TRF->find(id)))
	    {
	      S_Par = new SBSINEXParameter(p);
	      S_Par->setIdx(++Idx);

	      if (p->name().contains("coord-X"))
		{
		  S_Par->setType(SBSINEXParameter::ST_STAX);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staCoo());
		  S_Par->setAPrioriValue(Station->r(S_Par->tEpoch()).at(X_AXIS));
		}
	      else if (p->name().contains("coord-Y"))
		{
		  S_Par->setType(SBSINEXParameter::ST_STAY);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staCoo());
		  S_Par->setAPrioriValue(Station->r(S_Par->tEpoch()).at(Y_AXIS));
		}
	      else if (p->name().contains("coord-Z"))
		{
		  S_Par->setType(SBSINEXParameter::ST_STAZ);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staCoo());
		  S_Par->setAPrioriValue(Station->r(S_Par->tEpoch()).at(Z_AXIS));
		}
	      if (p->name().contains("veloc-X"))
		{
		  S_Par->setType(SBSINEXParameter::ST_VELX);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staVel());
		  S_Par->setAPrioriValue(Station->vModel(RunMgr).at(X_AXIS));
		}
	      else if (p->name().contains("veloc-Y"))
		{
		  S_Par->setType(SBSINEXParameter::ST_VELY);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staVel());
		  S_Par->setAPrioriValue(Station->vModel(RunMgr).at(Y_AXIS));
		}
	      else if (p->name().contains("veloc-Z"))
		{
		  S_Par->setType(SBSINEXParameter::ST_VELZ);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staVel());
		  S_Par->setAPrioriValue(Station->vModel(RunMgr).at(Z_AXIS));
		}

	      SiteCode = Station->charID4SINEX();
	      if (SiteCode=="")
		{
		  SiteCode.sprintf("%04X", ++IdxSite);
		  Station->setCharID(SiteCode);
		};
	      
	      S_Par->setSiteCode(Station->charID4SINEX());
	      S_Par->setPointCode(" A");
	      S_Par->setSolutionID("   1");
	      S_Par->setTValidEpoch(TRF->epoch());
	      
	      if (!(SI=RunMgr->prj()->stationList()->find(&id)))
		Log->write(SBLog::ERR, SBLog::ESTIMATOR, "SINEX I/O: cannot find station [" + 
			   Station->name() + "] in the project");
	      else
		{
		  if (S_Par->type()==SBSINEXParameter::ST_STAX ||
		      S_Par->type()==SBSINEXParameter::ST_STAY || 
		      S_Par->type()==SBSINEXParameter::ST_STAZ  )
		    {
		      if (SI->isAttr(SBStationInfo::ConstrCoo))
			S_Par->setConstraintCode("1");
		      else
			S_Par->setConstraintCode("2");
		    }
		  else if (S_Par->type()==SBSINEXParameter::ST_VELX ||
			   S_Par->type()==SBSINEXParameter::ST_VELY || 
			   S_Par->type()==SBSINEXParameter::ST_VELZ  )
		    {
		      if (SI->isAttr(SBStationInfo::ConstrVel))
			S_Par->setConstraintCode("1");
		      else
			S_Par->setConstraintCode("2");
		    };
		};

	      SINEX_List->append(S_Par);
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::ESTIMATOR, "SINEX I/O: cannot find a Station for the parameter [" + 
		       p->name() + "] in the TRF");
	};
    };

  int		N = SINEX_List->count();
  SINEX_P = new SBSymMatrix(N, "SINEX_P");

  int	*IdxS = new int[N];

  for (int i=0; i<N; i++)
    *(IdxS+i)=-1;

  for (int i=0; i<N; i++)
    {
      S_Par = SINEX_List->at(i);
      SBParameter *p = S_Par;
      if ( (*(IdxS+i)=LastGlb->idx(p))==-1 )
	Log->write(SBLog::ERR, SBLog::ESTIMATOR,
		   "SINEX I/O: cannot find the SINEX parameter [" + S_Par->name() + 
		   "] in the list of all last global parameters");
    };
  
  for (int i=0; i<N; i++)
    for (int j=i; j<N; j++)
      SINEX_P->set(i, j,  LastP->at(*(IdxS+i), *(IdxS+j)));
  
  delete[] IdxS;
};
/*==============================================================================================*/



/*==============================================================================================*/
// Special case of SINEX parameters:
// Only station's coordinates are reported, all of them forced the flag "2" in 
// the `constraint code', because the constraints were not applied in the NEQ 
// system.
void collectListOfSINEXParameters4NEQ(SBParameterList*		PList,  
				      SBSINEXParameterList*	SINEX_List,
				      SBRunManager*		RunMgr)
{
  SB_TRF		*TRF = RunMgr->trf();
  SBStationID		id;
  SBStation		*Station = NULL;
  int			IdxSite=0;

  SBSource		*Source = NULL;
  SB_CRF		*CRF = RunMgr->crf();
  int			IdxSrc =0;

  SBFrame		*Frame = RunMgr->frame();
  SBMJD			T;

  SBSINEXParameter	*S_Par = NULL;
  SBParameter		*p_aux = NULL;
  QString		SiteCode("");
  int			Idx=0;
  

  SINEX_List->clear();
  
  for (SBParameter *p=PList->first(); p; p=PList->next())
    {
      // stations:
      if (p->name().contains("St:"))
	{
	  id.setID(p->name().mid(4, 9));
	  if (id.isValidId() && (Station=TRF->find(id)))
	    {
	      S_Par = new SBSINEXParameter(p);
	      S_Par->setIdx(++Idx);

	      Station->calcDisplacement(RunMgr, S_Par->tEpoch(), FALSE);

	      if (p->name().contains("coord-X"))
		{
		  p_aux = Station->p_DRX();
		  S_Par->setType(SBSINEXParameter::ST_STAX);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staCoo());
		  S_Par->setAPrioriValue(Station->rt().at(X_AXIS)-p_aux->v());
		}
	      else if (p->name().contains("coord-Y"))
		{
		  p_aux = Station->p_DRY();
		  S_Par->setType(SBSINEXParameter::ST_STAY);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staCoo());
		  S_Par->setAPrioriValue(Station->rt().at(Y_AXIS)-p_aux->v());
		}
	      else if (p->name().contains("coord-Z"))
		{
		  p_aux = Station->p_DRZ();
		  S_Par->setType(SBSINEXParameter::ST_STAZ);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().staCoo());
		  S_Par->setAPrioriValue(Station->rt().at(Z_AXIS)-p_aux->v());
		};

	      SiteCode = Station->charID4SINEX();
	      if (SiteCode=="")
		{
		  SiteCode.sprintf("%04X", ++IdxSite);
		  Station->setCharID(SiteCode);
		};
	      
	      S_Par->setSiteCode(Station->charID4SINEX());
	      S_Par->setPointCode(" A");
	      S_Par->setSolutionID("   1");
	      //	      S_Par->setTValidEpoch(TRF->epoch());
	      S_Par->setTValidEpoch(S_Par->tEpoch());
	      
	      S_Par->setConstraintCode("2");

	      SINEX_List->append(S_Par);
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::ESTIMATOR, "SINEX I/O: cannot find a Station for the parameter [" + 
		       p->name() + "] in the TRF");
	};

      // sources:
      if (p->name().contains("So:"))
	{
	  if ((Source=CRF->find(p->name().mid(4, 8))))
	    {
	      S_Par = new SBSINEXParameter(p);
	      S_Par->setIdx(++Idx);

	      Source->updateParameters(RunMgr, S_Par->tEpoch(), FALSE);

	      if (p->name().contains("RA"))
		{
		  p_aux = Source->p_RA();
		  S_Par->setType(SBSINEXParameter::ST_RS_RA);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().srcCoo());
		  S_Par->setAPrioriValue(Source->ra_t()-p_aux->v());
		}
	      else if (p->name().contains("DN"))
		{
		  p_aux = Source->p_DN();
		  S_Par->setType(SBSINEXParameter::ST_RS_DE);
		  S_Par->tuneParameter(RunMgr->prj()->cfg().p().srcCoo());
		  S_Par->setAPrioriValue(Source->dn_t()-p_aux->v());
		};
	      
	      if (Source->sinexCharID()=="")
		Source->setSINEXCharID(QString().sprintf("%04X", ++IdxSrc));

	      S_Par->setSiteCode(Source->sinexCharID());
	      
	      S_Par->setPointCode("--");
	      S_Par->setSolutionID("----");
	      S_Par->setTValidEpoch(CRF->epoch());
	      S_Par->setTValidEpoch(S_Par->tEpoch());
	      
	      S_Par->setConstraintCode("2");

	      SINEX_List->append(S_Par);
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::ESTIMATOR, "SINEX I/O: cannot find a Station for the parameter [" + 
		       p->name() + "] in the TRF");
	};

      // EOPs:
      if (p->name().contains("EOP:"))
	{
	  S_Par = new SBSINEXParameter(p);
	  S_Par->setIdx(++Idx);
	  
	  T = p->tEpoch();
	  Frame->eop()->prepare4Date(T);
	  Frame->calc(T);

	  if (p->name().contains(Frame->eop()->p_Px()->name()))
	    {
	      p_aux = Frame->eop()->p_Px();
	      S_Par->setType(SBSINEXParameter::ST_XPO);
	      S_Par->tuneParameter(RunMgr->prj()->cfg().p().polusXY());
	      // here it's in `mas'
	      S_Par->setSigmaAPriori(S_Par->sigmaAPriori()*RAD2SEC*1000.0);
	      S_Par->setAPrioriValue((Frame->polar_X()-p_aux->v())*RAD2SEC*1000.0); 
	    }
	  else if (p->name().contains(Frame->eop()->p_Py()->name()))
	    {
	      p_aux = Frame->eop()->p_Py();
	      S_Par->setType(SBSINEXParameter::ST_YPO);
	      S_Par->tuneParameter(RunMgr->prj()->cfg().p().polusXY());
	      // here it's in `mas'
	      S_Par->setSigmaAPriori(S_Par->sigmaAPriori()*RAD2SEC*1000.0);
	      S_Par->setAPrioriValue((Frame->polar_Y()-p_aux->v())*RAD2SEC*1000.0); 
	    }
	  else if (p->name().contains(Frame->eop()->p_Put()->name()))
	    {
	      p_aux = Frame->eop()->p_Put();
	      S_Par->setType(SBSINEXParameter::ST_UT);
	      S_Par->tuneParameter(RunMgr->prj()->cfg().p().polusUT());
	      // here it's in `ms'
	      S_Par->setSigmaAPriori(S_Par->sigmaAPriori()*DAY2SEC*1000.0);
	      S_Par->setAPrioriValue((Frame->polar_UT1_UTC()-p_aux->v())*DAY2SEC*1000.0);
	    }
	  else if (p->name().contains(Frame->eop()->p_Npsi()->name()))
	    {
	      p_aux = Frame->eop()->p_Npsi();
	      S_Par->setType(SBSINEXParameter::ST_NUT_LN);
	      S_Par->tuneParameter(RunMgr->prj()->cfg().p().polusNut());
	      // here it's in `mas':
	      S_Par->setSigmaAPriori(S_Par->sigmaAPriori()*RAD2SEC*1000.0);
	      S_Par->setAPrioriValue((Frame->totNutationLongitude()-p_aux->v())*RAD2SEC*1000.0); 
	    }
	  else if (p->name().contains(Frame->eop()->p_Neps()->name()))
	    {
	      p_aux = Frame->eop()->p_Neps();
	      S_Par->setType(SBSINEXParameter::ST_NUT_OB);
	      S_Par->tuneParameter(RunMgr->prj()->cfg().p().polusNut());
	      // here it's in `mas':
	      S_Par->setSigmaAPriori(S_Par->sigmaAPriori()*RAD2SEC*1000.0);
	      S_Par->setAPrioriValue((Frame->totNutationObliquity()-p_aux->v())*RAD2SEC*1000.0);
	    };

	  S_Par->setSiteCode("----");
	  S_Par->setPointCode("--");
	  S_Par->setSolutionID("----");
	  S_Par->setTValidEpoch(S_Par->tEpoch());
	  
	  S_Par->setConstraintCode("2");
	  
	  SINEX_List->append(S_Par);
	};
    };
};
/*==============================================================================================*/




/*===============================================================================================*
 *
 *	SINEX output auxiliaries:
 *
 * ==============================================================================================*/
void sinex_Comments(QTextStream& s, const QString& Comments)
{
  s << "*" << Comments.left(79) << "\n";
};

void sinex_HeaderLine(QTextStream& s, double SINEX_VER, 
		      const SBMJD& CreationTime, const SBMJD& TStart, const SBMJD& TFinis,
		      int NumberOfPars, unsigned int SContents, bool IsNEQ)
{
  QString	Str;

  QString AgencyName("MAO");
  QString SolutionContents("S");
  QString ConstraintCode("1");

  if (IsNEQ)
    ConstraintCode= "2";

  SolutionContents.sprintf("%c%c%c%c%c", 
			   (SContents&SNX_SC_STATIONS)   ?'S':' ',
			   (SContents&SNX_SC_ORBITS)     ?'O':' ',
			   (SContents&SNX_SC_EOP)        ?'E':' ',
			   (SContents&SNX_SC_TROPOSPHERE)?'T':' ',
			   (SContents&SNX_SC_SOURCES)	 ?'C':' ');

  Str.sprintf(" %4.2f %-3s %s %-3s %s %s %s %05d %s %10s",
	      SINEX_VER, (const char*)AgencyName, (const char*)CreationTime.toString(SBMJD::F_SINEX),
	      (const char*)AgencyName, (const char*)TStart.toString(SBMJD::F_SINEX), 
	      (const char*)TFinis.toString(SBMJD::F_SINEX), "R", NumberOfPars,
	      (const char*)ConstraintCode, (const char*)SolutionContents);
  s << "%=SNX" << Str << "\n";
};

void sinex_TrailerLine(QTextStream& s)
{
  QString		Str("%ENDSNX");
  s << Str << "\n";
};

void sinex_FileReferenceBlock(QTextStream& s, 
			      const QString& DescriptionItem,
			      const QString& OutputItem,
			      const QString& InputItem)
{
  QString		Str;
  struct utsname	INFO;
  
  QString	ContactItem    (SBCompilerUserName + " <" + SBCompilerLoginName + "@" + 
				SBCompilerHostName + "." + SBCompilerDomainName + ">");
  QString	SoftwareItem   (Version.selfName() + " ver. " + Version.toString());
  QString	HardwareItem;

  if (uname (&INFO)!=-1)
    HardwareItem.sprintf("%s %s-%s %s.%s", INFO.machine,INFO.sysname,INFO.release,
			 INFO.nodename,INFO.domainname);
  else
    HardwareItem=SBCompilerOS;

  s << "+FILE/REFERENCE\n";
  Str.sprintf("%-18s %s", "DESCRIPTION",(const char*)DescriptionItem);
  s << " " << Str << "\n";
  Str.sprintf("%-18s %s", "OUTPUT",	(const char*)OutputItem);
  s << " " << Str << "\n";
  Str.sprintf("%-18s %s", "CONTACT",	(const char*)ContactItem);
  s << " " << Str << "\n";
  Str.sprintf("%-18s %s", "SOFTWARE",	(const char*)SoftwareItem);
  s << " " << Str << "\n";
  Str.sprintf("%-18s %s", "HARDWARE",	(const char*)HardwareItem);
  s << " " << Str << "\n";
  Str.sprintf("%-18s %s", "INPUT",	(const char*)InputItem);
  s << " " << Str << "\n";
  s << "-FILE/REFERENCE\n";
};

void sinex_FileCommentBlock(QTextStream& s, const QString& Comment)
{
  s << "+FILE/COMMENT\n";
  if (Comment.length()!=0)
    s << " " << Comment << "\n";
  s << "-FILE/COMMENT\n";
};

void sinex_InputHistoryBlock(QTextStream& s, double SINEX_VER, 
			     const SBMJD& CreationTime, const SBMJD& TStart, const SBMJD& TFinis,
			     int NumberOfPars, unsigned int SContents, bool IsNEQ)
{
  QString	Str;
  
  QString AgencyName("MAO");
  QString SolutionContents("S");
  QString ConstraintCode("1");

  if (IsNEQ)
    ConstraintCode= "2";

  SolutionContents.sprintf("%c%c%c%c%c", 
			   (SContents&SNX_SC_STATIONS)   ?'S':' ',
			   (SContents&SNX_SC_ORBITS)     ?'O':' ',
			   (SContents&SNX_SC_EOP)        ?'E':' ',
			   (SContents&SNX_SC_TROPOSPHERE)?'T':' ',
			   (SContents&SNX_SC_SOURCES)	 ?'C':' ');

  s << "+INPUT/HISTORY\n";
  sinex_Comments(s, "_Version_ Cre __Creation__ Own _Data_start_ _Data_end___ T Param S ___Type___");

  Str.sprintf(" %4.2f %-3s %s %-3s %s %s %s %05d %s %10s",
	      SINEX_VER, (const char*)AgencyName, (const char*)CreationTime.toString(SBMJD::F_SINEX),
	      (const char*)AgencyName, (const char*)TStart.toString(SBMJD::F_SINEX), 
	      (const char*)TFinis.toString(SBMJD::F_SINEX), "R", NumberOfPars,
	      (const char*)ConstraintCode, (const char*)SolutionContents);
  s << " =SNX" << Str << "\n";
  s << "-INPUT/HISTORY\n";
};

void sinex_InputFilesBlock(QTextStream& s, const SBMJD& CreationTime, 
			   const QString& FileName, const QString& FileDescr)
{
  QString	Str;
  QString AgencyName("MAO");

  s << "+INPUT/FILES\n";
  sinex_Comments(s, "Own __Creation__ __filename___________________ __________Description_________");
  
  Str.sprintf("%-3s %s %-29s %-32s",
	      (const char*)AgencyName, (const char*)CreationTime.toString(SBMJD::F_SINEX),
	      (const char*)FileName, (const char*)FileDescr);
  s << " " << Str << "\n";
  s << "-INPUT/FILES\n";
};

void sinex_InputAcknowledgementsBlock(QTextStream& s)
{
  QString	Str;
  QString AgencyName("MAO");
  QString AgencyDesc("Main Astronomical Observatory, NASU, Kiev");

  s << "+INPUT/ACKNOWLEDGEMENTS\n";
  Str.sprintf("%-3s %-75s",
	      (const char*)AgencyName, (const char*)AgencyDesc);
  s << " " << Str << "\n";
  s << "-INPUT/ACKNOWLEDGEMENTS\n";
};

void sinex_NutationDataBlock(QTextStream& s, const QString& NutCode, const QString& NutDescr)
{
  QString	Str;

  s << "+NUTATION/DATA\n";
  Str.sprintf("%-8s %-70s",
	      (const char*)NutCode, (const char*)NutDescr);
  s << " " << Str << "\n";
  s << "-NUTATION/DATA\n";
};

void sinex_PrecessionDataBlock(QTextStream& s, const QString& PrecCode, const QString& PrecDescr)
{
  QString	Str;

  s << "+PRECESSION/DATA\n";
  Str.sprintf("%-8s %-70s",
	      (const char*)PrecCode, (const char*)PrecDescr);
  s << " " << Str << "\n";
  s << "-PRECESSION/DATA\n";
};

void sinex_SourceIDBlock(QTextStream& s, SB_CRF* CRF, SBProject* Prj, 
			 SBParameterList* List,
			 SBSINEXParameterList* SINEX_List)
{
  QString	Str;
  QString	ICRFDes("----------------");
  SBSourceInfo	*SI  = NULL;
  unsigned int	Idx=0;
  
  QString	SrcSiteID;
  SBSINEXParameter	*S_Par = NULL, *SP_tmp=NULL;

  s << "+SOURCE/ID\n";
  sinex_Comments(s, "Code IVS name ICRF designator  Comments");
  for (SBSource *Src = CRF->first(); Src; Src = CRF->next())
    if (!(SI = Prj->sourceList()->find(Src)))
      Log->write(SBLog::ERR, SBLog::RUN, "sinex_SourceIDBlock: cannot find source [" + 
		 Src->name() + "] in the project");
    else if (!SI->isAttr(SBSourceInfo::notValid) && SI->isAttr(SBSourceInfo::EstCoo) &&
	     (!List || (List && List->count() && List->find(Src->p_RA()->name()))) )
      {
	if (!Src->ICRFName().contains("J000000.0+000000"))
	  ICRFDes = Src->ICRFName().mid(5);
	else 
	  ICRFDes = "----------------";
	
	if (SINEX_List)
	  {	    
	    SP_tmp = new SBSINEXParameter(Src->p_RA());
	    int i = SINEX_List->find(SP_tmp);
	    if (i!=-1 && (S_Par=SINEX_List->at(i)))
	      SrcSiteID = S_Par->siteCode();
	    if (SrcSiteID=="")
	      SrcSiteID="----";
	    delete SP_tmp;
	  }
	else
	  SrcSiteID.sprintf("%04X", ++Idx);

	Str.sprintf("%-4s %-8s %-16s %-48s",
		    (const char*)SrcSiteID, (const char*)Src->name(), (const char*)ICRFDes,
		    (!SINEX_List&&SI->isAttr(SBSourceInfo::ConstrCoo))?"Used on constraints":""
		    );
	s << " " << Str << "\n";
      };
  
  s << "-SOURCE/ID\n";
};

void sinex_SiteIDBlock(QTextStream& s, SB_TRF* TRF, SBProject* Prj, SBParameterList* List)
{
  QString	Str, SiteCode("");
  SBStationInfo	*SI  = NULL;
  SBStationID	ID;
  unsigned int	Idx=0;
  int		lon_d, lon_m, lat_d, lat_m;
  double	lon_f, lat_f;

  s << "+SITE/ID\n";
  sinex_Comments(s, " Here  the longitudes of sites are written in  notation  not conforming");
  sinex_Comments(s, " the ISO6709 standard, wich the description of SINEX-2.00 format refers");
  sinex_Comments(s, " to, because the format itself  does not assume  the sign at  the field");
  sinex_Comments(s, " of degree of longitude of a site ('I3').");
  sinex_Comments(s, "Code PT Domes____ T Station description___ Approx_lon_ Approx_lat_ App_h__");
  for (SBStation *Station = TRF->stations()->first(); Station; Station = TRF->stations()->next())
    {
      ID = Station->id();
      if (!(SI = Prj->stationList()->find(&ID)))
	Log->write(SBLog::ERR, SBLog::RUN, "sinex_SiteIDBlock: cannot find the station " + 
		   Station->name() + " [" + Station->id().toString() + "] in the project");
      else if (!SI->isAttr(SBStationInfo::notValid) && SI->isAttr(SBStationInfo::EstCoo) &&
	       (!List || (List && List->count() && List->find(Station->p_DRX()->name()))) )
	{
	  
	  rad2dmsl(Station->longitude(),		lon_d, lon_m, lon_f);
	  rad2dms (fabs(Station->latitude()),	lat_d, lat_m, lat_f);
	  if (Station->latitude()<0.0)
	    lat_d*=-1;
	  
	  SiteCode = Station->charID4SINEX();
	  if (SiteCode=="")
	    {
	      SiteCode.sprintf("%04X", Idx++);
	      Station->setCharID(SiteCode);
	    };
	  
	  Str.sprintf("%-4s %-2s %-9s %s %-22s %3d %2d %4.1f %3d %2d %4.1f %7.1f",
		      (const char*)SiteCode, " A",
		      (const char*)Station->id().toString(), "R", 
		      (const char*)Station->description().left(22), 
		      lon_d, lon_m, lon_f,
		      lat_d, lat_m, lat_f,
		      Station->height()
		      );
	  s << " " << Str << "\n";
	};
    };
  
  s << "-SITE/ID\n";
};

void sinex_SiteEccentricityBlock(QTextStream& s, SB_TRF* TRF, SBProject* Prj, SBVLBISet* VLBI, SBParameterList* List)
{
  QString	Str;
  SBStationID	id;
  SBMJD		T1, T2;
  SBVLBISession	*S=NULL;
  SBStationInfo	*StaInf=NULL;

  s << "+SITE/ECCENTRICITY\n";
  sinex_Comments(s, "Code PT SBIN T Data_Start__ Data_End____ typ Apr --> Benchmark (m)_______");
  for (SBStation *Station = TRF->stations()->first(); Station; Station = TRF->stations()->next())
    if (!List || (List && List->count() && List->find(Station->p_DRX()->name())))
      {
	SBEccentricity	*Ecc = NULL;
	SBStationEcc	*StatEcc = NULL;
	id.setID(Station->id());
	
	if ((StatEcc=TRF->ecc().eccByID()->find(id.toString())))
	  {
	    Ecc=StatEcc->eccs().first();
	    if (Ecc && Prj->tStart()<Ecc->tStart())
	      {
		bool		Have2Seek=TRUE;
		for (SBVLBISesInfo *SI=Prj->VLBIsessionList()->first(); Have2Seek && SI; 
		     SI=Prj->VLBIsessionList()->next())
		  if (SI->tFinis()<=Ecc->tStart())
		    {
		      if ((S = VLBI->loadSession(SI->name())))
			if (S->count() && (StaInf=S->stationList()->find(&id)) && 
			    !StaInf->isAttr(SBStationInfo::notValid))
			  {
			    Str.sprintf("%-4s %-2s %-4s %s 00:000:00000 %s XYZ   "
					"0.0000   0.0000   0.0000",
					(const char*)Station->charID4SINEX(), " A", "   1", "R", 
					(const char*)Ecc->tStart().toString(SBMJD::F_SINEX));
			    s << " " << Str << "\n";
			    Have2Seek = FALSE; // break the loop
			  };
		      if (S)
			{
			  delete S;
			  S = NULL;
			};
		    };
	      };
	    for (Ecc=StatEcc->eccs().first(); Ecc; Ecc=StatEcc->eccs().next())
	      {
		if ( (Prj->tStart()<=Ecc->tFinis() && Ecc->tFinis()<=Prj->tFinis()) || 
		     (Prj->tStart()<=Ecc->tStart() && Ecc->tStart()<=Prj->tFinis())  )
		  {
		    bool		Have2Seek=TRUE;
		    for (SBVLBISesInfo *SI=Prj->VLBIsessionList()->first(); Have2Seek && SI; 
			 SI=Prj->VLBIsessionList()->next())
		      if (Ecc->tStart()<=SI->tStart() && SI->tFinis()<=Ecc->tFinis())
			{
			  if ((S = VLBI->loadSession(SI->name())))
			    if (S->count() && (StaInf=S->stationList()->find(&id)) && 
				!StaInf->isAttr(SBStationInfo::notValid))
			      {
				T1 = Ecc->tStart()>=Prj->tStart()?Ecc->tStart():TZero;
				T2 = Ecc->tFinis()<=Prj->tFinis()?Ecc->tFinis():TZero;
				Str.sprintf("%-4s %-2s %-4s %s %s %s %-3s %8.4f %8.4f %8.4f",
					    (const char*)Station->charID4SINEX(), " A", "   1", "R",
					    (const char*)T1.toString(SBMJD::F_SINEX),
					    (const char*)T2.toString(SBMJD::F_SINEX),
					    Ecc->type()==SBEccentricity::ET_XYZ?"XYZ":"UNE",
					    Ecc->ecc().at(Ecc->type()==SBEccentricity::ET_XYZ?
							  X_AXIS:Z_AXIS), 
					    Ecc->ecc().at(Ecc->type()==SBEccentricity::ET_XYZ?
							  Y_AXIS:X_AXIS), 
					    Ecc->ecc().at(Ecc->type()==SBEccentricity::ET_XYZ?
							  Z_AXIS:Y_AXIS));
				s << " " << Str << "\n";
				Have2Seek = FALSE; // break the loop
			      };
			  if (S)
			    {
			      delete S;
			      S = NULL;
			    };
			};
		  }
		else if (Prj->tStart()>=Ecc->tStart() && Ecc->tFinis()>=Prj->tFinis())
		  {
		    Str.sprintf("%-4s %-2s %-4s %s 00:000:00000 00:000:00000 %-3s %8.4f %8.4f %8.4f",
				(const char*)Station->charID4SINEX(), " A", "   1", "R",
				Ecc->type()==SBEccentricity::ET_XYZ?"XYZ":"UNE",
				Ecc->ecc().at(Ecc->type()==SBEccentricity::ET_XYZ?X_AXIS:Z_AXIS), 
				Ecc->ecc().at(Ecc->type()==SBEccentricity::ET_XYZ?Y_AXIS:X_AXIS), 
				Ecc->ecc().at(Ecc->type()==SBEccentricity::ET_XYZ?Z_AXIS:Y_AXIS));
		    s << " " << Str << "\n";
		  };
	      };
	    Ecc=StatEcc->eccs().last();
	    if (Ecc && Prj->tFinis()>Ecc->tFinis())
	      {
		bool		Have2Seek=TRUE;
		for (SBVLBISesInfo *SI=Prj->VLBIsessionList()->first(); Have2Seek && SI; 
		     SI=Prj->VLBIsessionList()->next())
		  if (Ecc->tFinis()<=SI->tStart())
		    {
		      if ((S = VLBI->loadSession(SI->name())))
			if (S->count() && (StaInf=S->stationList()->find(&id)) && 
			    !StaInf->isAttr(SBStationInfo::notValid))
			  {
			    Str.sprintf("%-4s %-2s %-4s %s %s 00:000:00000 XYZ   "
					"0.0000   0.0000   0.0000",
					(const char*)Station->charID4SINEX(), " A", "   1", "R",
					(const char*)Ecc->tFinis().toString(SBMJD::F_SINEX));
			    s << " " << Str << "\n";
			    Have2Seek = FALSE; // break the loop
			  };
		      if (S)
			{
			  delete S;
			  S = NULL;
			};
		    };
	      };
	  }
	else // there is no any ECC-record, write a trivial string:
	  {
	    Str.sprintf("%-4s %-2s %-4s %s 00:000:00000 00:000:00000 XYZ   0.0000   0.0000   0.0000",
			(const char*)Station->charID4SINEX(), " A", "   1", "R");
	    s << " " << Str << "\n";
	  };
      };

  s << "-SITE/ECCENTRICITY\n";
};

void sinex_SolutionEpochsBlock(QTextStream& s, SB_TRF* TRF, SBParameterList* List)
{
  QString		Str;
  SBParameter		*P=NULL;

  s << "+SOLUTION/EPOCHS\n";
  sinex_Comments(s, "CODE PT SOLN T DATA_START__ DATA_END____ MEAN_EPOCH__");
  for (SBStation *Station = TRF->stations()->first(); Station; Station = TRF->stations()->next())
    {
      // get parameter:
      if (List && List->count())
	{
	  if (Station->p_DRX())
	    {
	      if (!(P=List->find(Station->p_DRX()->name())))
		{
		  if (Station->p_DVX())
		    P=List->find(Station->p_DVX()->name());
		}
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::RUN, "sinex_SolutionEpochBlock: the station " + 
		       Station->name() + " [" + Station->id().toString() + 
		       "] is not properly processed");
	    
	}
      else if (!(P=Station->p_DRX()))
	{
	  if (!(P=Station->p_DRY()))
	    if (!(P=Station->p_DRZ()))
	      if (!(P=Station->p_DVX()))
		if (!(P=Station->p_DVY()))
		  if (!(P=Station->p_DVZ())) // coords & velocs of the station were not estimated:
		    Log->write(SBLog::ERR, SBLog::RUN, "sinex_SolutionEpochBlock: the station " + 
			       Station->name() + " [" + Station->id().toString() + 
			       "] was not estimated");
	}
      // make report if the parameter was estimated:
      if (P && P->num() && P->tEpoch()!=TZero)
	{
	  Str.sprintf("%-4s %-2s %-4s %s %s %s %s",
		      (const char*)Station->charID4SINEX(), " A", "   1", "R",
		      (const char*)P->tStart().toString(SBMJD::F_SINEX),
		      (const char*)P->tFinis().toString(SBMJD::F_SINEX),
		      (const char*)P->tEpoch().toString(SBMJD::F_SINEX));
	  s << " " << Str << "\n";
	};
    };
  s << "-SOLUTION/EPOCHS\n";
};

void sinex_SolutionStatisticsBlock(QTextStream& s, SBStatistics* Statistics)
{
  QString		Str;

  s << "+SOLUTION/STATISTICS\n";
  sinex_Comments(s, "_STATISTICAL PARAMETER________ __VALUE(S)____________");
  Str.sprintf("%-30s %22.14f",
	      "NUMBER OF OBSERVATIONS",
	      (double)Statistics->numberOfObservations());
  s << " " << Str << "\n";
  Str.sprintf("%-30s %22.14f",
	      "NUMBER OF UNKNOWNS",
	      (double)Statistics->numPars());
  s << " " << Str << "\n";
  Str.sprintf("%-30s %22.14f",
	      "SQUARE SUM OF RESIDUALS (VTPV)",
	      (double)Statistics->wpfr());
  s << " " << Str << "\n";
  Str.sprintf("%-30s %22.14f",
	      "NUMBER OF DEGREES OF FREEDOM",
	      (double)Statistics->dof());
  s << " " << Str << "\n";
  Str.sprintf("%-30s %22.14f",
	      "VARIANCE FACTOR",
	      (double)Statistics->varFactor());
  s << " " << Str << "\n";
  Str.sprintf("%-30s %22.14f",
	      "WEIGHTED SQUARE SUM OF O-C",
	      (double)Statistics->wrms());
  s << " " << Str << "\n";
  s << "-SOLUTION/STATISTICS\n";
};

void sinex_SolutionEstimateBlock(QTextStream& s, SBSINEXParameterList *SINEX_List)
{
  QString		Str;

  s << "+SOLUTION/ESTIMATE\n";
  sinex_Comments(s, "Index TYPE__ CODE PT SBIN Ref_epoch___ Unit S Total_value__________ Formal_erro");

  for (SBSINEXParameter *p=SINEX_List->first(); p; p=SINEX_List->next())
    {
      Str.sprintf("%5d %-6s %-4s %-2s %-4s %s %-4s %s %21.14E %11.5E",
		  p->idx(),
		  (const char*)p->parameterType(),
		  (const char*)p->siteCode(),
		  (const char*)p->pointCode(),
		  (const char*)p->solutionID(),
		  (const char*)p->tValidEpoch().toString(SBMJD::F_SINEX),
		  (const char*)p->parameterUnits(),
		  (const char*)p->constraintCode(),
		  p->totValue(),
		  p->e());
      s << " " << Str << "\n";
    };

  s << "-SOLUTION/ESTIMATE\n";
};

void sinex_SolutionAprioriBlock(QTextStream& s, SBSINEXParameterList *SINEX_List)
{
  QString		Str;
  
  s << "+SOLUTION/APRIORI\n";
  sinex_Comments(s, "Index Type__ CODE PT SBIN Ref_epoch___ Unit S Apriori_value________ _Std._Dev._");

  for (SBSINEXParameter *p=SINEX_List->first(); p; p=SINEX_List->next())
    {
      Str.sprintf("%5d %-6s %-4s %-2s %-4s %s %-4s %s %21.14E %11.5E",
		  p->idx(),
		  (const char*)p->parameterType(),
		  (const char*)p->siteCode(),
		  (const char*)p->pointCode(),
		  (const char*)p->solutionID(),
		  (const char*)p->tValidEpoch().toString(SBMJD::F_SINEX),
		  (const char*)p->parameterUnits(),
		  (const char*)p->constraintCode(),
		  p->aPrioriValue(),
		  p->sigmaAPriori());
      s << " " << Str << "\n";
    };
  
  s << "-SOLUTION/APRIORI\n";
};

void sinex_SolutionMatrixEstimateBlock(QTextStream& s, bool IsEstimate, SBSymMatrix* P)
{
  QString		Str;
  int			N=P->nCol();
  int			i=0, j=0;
  
  s << (IsEstimate?"+SOLUTION/MATRIX_ESTIMATE U COVA\n":"+SOLUTION/MATRIX_APRIORI U COVA\n");
  sinex_Comments(s, "PARA1 PARA2 _____PARA2+0_________ _____PARA2+1_________ _____PARA2+2_________");

  for (i=0; i<N; i++)
    {
      j=i;
      while (j<N)
	{
	  Str.sprintf("%5d %5d",    i+1, j+1);
	  s << " " << Str;
	  if (j<N)
	    {
	      Str.sprintf("%21.14E",    P->at(i, j++));
	      s << " " << Str;
	    };
	  if (j<N)
	    {
	      Str.sprintf("%21.14E",    P->at(i, j++));
	      s << " " << Str;
	    };
	  if (j<N)
	    {
	      Str.sprintf("%21.14E",    P->at(i, j++));
	      s << " " << Str;
	    };
	  s << "\n";
	};
    };

  s << (IsEstimate?"-SOLUTION/MATRIX_ESTIMATE U COVA\n":"-SOLUTION/MATRIX_APRIORI U COVA\n");
};

void sinex_SolutionNormalEquationMatrixBlock(QTextStream& s, SBSymMatrix* N)
{
  QString		Str;
  int			n=N->nCol();
  int			i=0, j=0;
  
  s << "+SOLUTION/NORMAL_EQUATION_MATRIX U\n";
  sinex_Comments(s, "PARA1 PARA2 _____PARA2+0_________ _____PARA2+1_________ _____PARA2+2_________");

  for (i=0; i<n; i++)
    {
      j=i;
      while (j<n)
	{
	  Str.sprintf("%5d %5d",    i+1, j+1);
	  s << " " << Str;
	  if (j<n)
	    {
	      Str.sprintf("%21.14E",    N->at(i, j++));
	      s << " " << Str;
	    };
	  if (j<n)
	    {
	      Str.sprintf("%21.14E",    N->at(i, j++));
	      s << " " << Str;
	    };
	  if (j<n)
	    {
	      Str.sprintf("%21.14E",    N->at(i, j++));
	      s << " " << Str;
	    };
	  s << "\n";
	};
    };

  s << "-SOLUTION/NORMAL_EQUATION_MATRIX U\n";
};

void sinex_SolutionNormalEquationVectorBlock(QTextStream& s, SBSINEXParameterList *SINEX_List,
					     SBVector* B)
{
  QString		Str;
  int			i=0;

  s << "+SOLUTION/NORMAL_EQUATION_VECTOR\n";
  sinex_Comments(s, "Index Type__ CODE PT SBIN Ref_epoch___ Unit S RHS of Normal Eq.Sys.");

  for (SBSINEXParameter *p=SINEX_List->first(); p; p=SINEX_List->next(), i++)
    {
      Str.sprintf("%5d %-6s %-4s %-2s %-4s %s %-4s %s %21.14E",
		  p->idx(),
		  (const char*)p->parameterType(),
		  (const char*)p->siteCode(),
		  (const char*)p->pointCode(),
		  (const char*)p->solutionID(),
		  (const char*)p->tValidEpoch().toString(SBMJD::F_SINEX),
		  (const char*)p->parameterUnits(),
		  (const char*)p->constraintCode(),
		  B->at(i));
      s << " " << Str << "\n";
    };

  s << "-SOLUTION/NORMAL_EQUATION_VECTOR\n";
};


void sinex_tro_HeaderLine(QTextStream& s, double SINEX_TRO_VER, 
		      const SBMJD& CreationTime, const SBMJD& TStart, const SBMJD& TFinis)
{
  QString	Str;

  QString AgencyName("MAO");

  Str.sprintf(" %4.2f %-3s %s %-3s %s %s %s %s",
	      SINEX_TRO_VER, (const char*)AgencyName, (const char*)CreationTime.toString(SBMJD::F_SINEX),
	      (const char*)AgencyName, (const char*)TStart.toString(SBMJD::F_SINEX), 
	      (const char*)TFinis.toString(SBMJD::F_SINEX), "R", "MIX");
  s << "%=TRO" << Str << "\n";
};

void sinex_tro_TrailerLine(QTextStream& s)
{
  QString		Str("%ENDTRO");
  s << Str << "\n";
};

void sinex_tro_SiteIDBlock(QTextStream& s, SB_TRF* TRF, SBStationInfoList* Stations)
{
  SBStation	*Station=NULL;
  QString	Str, SiteCode("");
  int		lon_d, lon_m, lat_d, lat_m;
  double	lon_f, lat_f;

  s << "+SITE/ID\n";
  sinex_Comments(s, "Code PT Domes____ T Station description___ Approx_lon_ Approx_lat_ App_h__");

  for (SBStationInfo *SI = Stations->first(); SI; SI = Stations->next())
    {
      if (SI->procNum())
	{
	  SBStationID	id(*SI);
	  if ((Station = TRF->find(id)))
	    {
	      rad2dmsl(Station->longitude(),	lon_d, lon_m, lon_f);
	      rad2dms (fabs(Station->latitude()),	lat_d, lat_m, lat_f);
	      if (Station->latitude()<0.0)
		lat_d*=-1;
	      
//	      SiteCode = SI->aka().left(4);
	      SiteCode = Station->charID4SINEX();
	      if (SiteCode=="")
	        SiteCode = SI->aka().left(4);
	      Str.sprintf("%-4s %-2s %-9s %s %-22s %3d %2d %4.1f %3d %2d %4.1f %7.1f",
			  (const char*)SiteCode, " A",
			  (const char*)Station->id().toString(), "R", 
			  (const char*)Station->description().left(22), 
			  lon_d, lon_m, lon_f,
			  lat_d, lat_m, lat_f,
			  Station->height()
			  );
	      s << " " << Str << "\n";
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::STATION, "sinex_tro_SiteIDBlock: cannot find the station [" + 
		       SI->toString() + "] in the TRF database");
	};
    };
  s << "-SITE/ID\n";
};

void sinex_tro_StaCoordinatesBlock(QTextStream& s, SB_TRF* TRF, SBStationInfoList* Stations, 
				   SBRunManager *RunMgr, const SBMJD& T)
{
  SBStation	*Station=NULL;
  QString	Str, SiteCode("");
  Vector3	R(v3Zero);


  s << "+TROP/STA_COORDINATES\n";
  sinex_Comments(s, "Code PT SBIN T ___X-coord__ ___Y-coord__ ___Z-coord__ System AC_ID");

  for (SBStationInfo *SI = Stations->first(); SI; SI = Stations->next())
    {
      if (SI->procNum())
	{
	  SBStationID	id(*SI);
	  if ((Station = TRF->find(id)))
	    {
	      //SiteCode = SI->aka().left(4);
	      SiteCode = Station->charID4SINEX();
	      if (SiteCode=="")
	        SiteCode = SI->aka().left(4);
	      R = Station->r(T);
	      Station->updateParameters(RunMgr, T, FALSE);
	      R[X_AXIS] += Station->p_DRX()->v();
	      R[Y_AXIS] += Station->p_DRY()->v();
	      R[Z_AXIS] += Station->p_DRZ()->v();

	      Str.sprintf("%-4s %-2s %-4s %s %12.3f %12.3f %12.3f %6s %5s",
			  (const char*)SiteCode, " A", "   1", "R",
			  R.at(X_AXIS), R.at(Y_AXIS), R.at(Z_AXIS), 
			  "ITRF00", "MAO"
			  );
	      s << " " << Str << "\n";
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::STATION, "sinex_tro_SiteIDBlock: cannot find the station [" + 
		       SI->toString() + "] in the TRF database");
	};
    };
  s << "-TROP/STA_COORDINATES\n";
};

void sinex_tro_TropDescriptionBlock(QTextStream& s, SBRunManager *RunMgr, SBVLBISession& Session)
{
  const SBConfig *Cfg = &RunMgr->prj()->cfg();
  
  QString	Str;
  QString	DryZdName("N/A");
  QString	WetZdName("N/A");
  QString	DryMFName("N/A");
  QString	WetMFName("N/A");
  QString	ZDModel  ("Undefined");
  QString	GrModel  ("Undefined");

  switch (Cfg->refrDryMapping())
    {//                                      12345678901234567890123456789
    case SBConfig::RDM_Chao:	DryMFName = "Chao, 1974";		break;
    case SBConfig::RDM_CfA2p2:	DryMFName = "CfA-2.2, Davis et al., 1985";	break;
    case SBConfig::RDM_Ifadis:	DryMFName = "Ifadis, 1986";		break;
    case SBConfig::RDM_NMFH2:	DryMFName = "NMFh2, Niell, 1996";	break;
    default:
    case SBConfig::RDM_MTT:	DryMFName = "MTT, Herring, 1992";	break;
    case SBConfig::RDM_NONE:	DryMFName = "None";			break;
    };
  
  switch (Cfg->refrWetMapping())
    {
    case SBConfig::RWM_Chao:	WetMFName = "Chao, 1974";		break;
    case SBConfig::RWM_Ifadis:	WetMFName = "Ifadis, 1986";		break;
    case SBConfig::RDM_NMFW2:	WetMFName = "NMFw2, Niell, 1996";	break;
    default:
    case SBConfig::RWM_MTT:	WetMFName = "MTT, Herring, 1992";	break;
    case SBConfig::RWM_NONE:	WetMFName = "None";			break;
    };

  switch(Cfg->refrDryZenith())
    {
    default:
    case SBConfig::RDZ_Saastamoinen:	DryZdName = "Saastamoinen, 1972";	break;
    case SBConfig::RDZ_Hopfield:	DryZdName = "Hopfield, 1977";		break;
    case SBConfig::RDZ_NONE:		DryZdName = "None";			break;
    };
  switch(Cfg->refrWetZenith())
    {
    default:
    case SBConfig::RWZ_Saastamoinen:	WetZdName = "Saastamoinen, 1973";	break;
    case SBConfig::RWZ_Hopfield:	WetZdName = "Hopfield, 1977";		break;
    case SBConfig::RWZ_NONE:		WetZdName = "None";			break;
    };

  if (Cfg->p().zenith().type() == SBParameterCfg::PT_STH)
    {
      switch (Cfg->p().zenith().stochasticType())
	{
	case SBParameterCfg::ST_WHITENOISE:	
	  ZDModel.sprintf("White noise (%.2f %s^2/hr)",
			  Cfg->p().zenith().whiteNoise()*Cfg->p().zenith().whiteNoise(),
			  (const char*)Cfg->p().zenith().scaleName());
	  break;
	case SBParameterCfg::ST_MARKOVPROCESS: 
	  ZDModel.sprintf("Markov process (%.2f %s^2/hr)",
			  Cfg->p().zenith().whiteNoise()*Cfg->p().zenith().whiteNoise(),
			  (const char*)Cfg->p().zenith().scaleName());
	  break;
	case SBParameterCfg::ST_RANDWALK: 
	  ZDModel.sprintf("Random walk (%.2f %s^2/hr)",
			  Cfg->p().zenith().whiteNoise()*Cfg->p().zenith().whiteNoise(),
			  (const char*)Cfg->p().zenith().scaleName());
	  break;
	default:
	case SBParameterCfg::ST_UNKN:
	  break;
	};
    };

  if (Cfg->p().atmGrad().type() == SBParameterCfg::PT_STH)
    {
      switch (Cfg->p().atmGrad().stochasticType())
	{
	case SBParameterCfg::ST_WHITENOISE:	
	  GrModel.sprintf("White noise (%.2g %s^2/hr)",
			  Cfg->p().atmGrad().whiteNoise()*Cfg->p().atmGrad().whiteNoise(),
			  (const char*)Cfg->p().atmGrad().scaleName());
	  break;
	case SBParameterCfg::ST_MARKOVPROCESS: 
	  GrModel.sprintf("Markov process (%.2g %s^2/hr)",
			  Cfg->p().atmGrad().whiteNoise()*Cfg->p().atmGrad().whiteNoise(),
			  (const char*)Cfg->p().atmGrad().scaleName());
	  break;
	case SBParameterCfg::ST_RANDWALK: 
	  GrModel.sprintf("Random walk (%.2g %s^2/hr)",
			  Cfg->p().atmGrad().whiteNoise()*Cfg->p().atmGrad().whiteNoise(),
			  (const char*)Cfg->p().atmGrad().scaleName());
	  break;
	default:
	case SBParameterCfg::ST_UNKN:
	  break;
	};
    }
  else if (Cfg->p().atmGrad().type() == SBParameterCfg::PT_ARC)
    GrModel = "Offset";
  
  s << "+TROP/DESCRIPTION\n";
  
  Str.sprintf("%-22s %s",
	      "SOLUTION_FIELDS_1",
	      "TROTOT TROWET STDDEV GRTOTN STDDEV GRTOTE STDDEV"
	      );
  s << " " << Str << "\n";
  //  " SOLUTION_FIELDS_1             TROTOT TROWET STDDEV GRTOTN STDDEV GRTOTE STDDEV"

  sinex_Comments(s, "_______KEYWORD________ ___________GENERAL___________");
  Str.sprintf("%-22s %29s", "ELEVATION CUTOFF ANGLE", "0");
  s << " " << Str << "\n";

  Str.sprintf("%-22s %29s", "DOWNW. OF LOW OBSERV.", "NO");
  s << " " << Str << "\n";

  Str.sprintf("%-22s %29s", "TERR. REFERENCE FRAME", "ITRF2000");
  s << " " << Str << "\n";

  Str.sprintf("%-22s %29s", "TRF FIXED", "OTHER");
  s << " " << Str << "\n";

  Str.sprintf("%-22s %29s", "DRY MAPPING FUNCTION", (const char*)DryMFName);
  s << " " << Str << "\n";

  Str.sprintf("%-22s %29s", "WET MAPPING FUNCTION", (const char*)WetMFName);
  s << " " << Str << "\n";
  // here to add "alternatives" for wet mapping function:
  if (Cfg->isRollBack2NMF())
    {
      // check for `bad-meteo' flags and report:
      bool IsTmp = FALSE;
      for (SBStationInfo *SI = Session.stationList()->first(); SI; SI = Session.stationList()->next())
	if (SI->procNum() && (SI->isAttr(SBStationInfo::BadMeteo) || SI->isAttr(SBStationInfo::ArtMeteo)))
	  IsTmp = TRUE;
      
      if (IsTmp)
	{
	  sinex_Comments(s, "WARNING: Alternative mapping functions for the station(s): ");
	  for (SBStationInfo *SI = Session.stationList()->first(); SI; SI = Session.stationList()->next())
	    if (SI->procNum() && (SI->isAttr(SBStationInfo::BadMeteo) || SI->isAttr(SBStationInfo::ArtMeteo)))
	      {
		Str.sprintf("%4s %-22s %29s", (const char*)(SI->aka().left(4)), 
			    "DRY MAPPING FUNCTION", "NMFh2, Niell, 1996");
		sinex_Comments(s, Str);
		Str.sprintf("%4s %-22s %29s", (const char*)(SI->aka().left(4)), 
			    "WET MAPPING FUNCTION", "NMFw2, Niell, 1996");
		sinex_Comments(s, Str);
	      };
	};
    };

  sinex_Comments(s, "_______KEYWORD________ ________ZENITH DELAYS________");
  Str.sprintf("%-22s %29d", "SAMPLING ZENITH DELAYS", (int)Cfg->tropSamplInterval());
  s << " " << Str << "\n";
  Str.sprintf("%-22s %29s", "APRIORI HYDROST. DELAY", (const char*)DryZdName);
  s << " " << Str << "\n";
  Str.sprintf("%-22s %29s", "APRIORI WET DELAY", (const char*)WetZdName);
  s << " " << Str << "\n";
  Str.sprintf("%-22s %29s", "ZENITH DELAY MODEL", (const char*)ZDModel);
  s << " " << Str << "\n";
  sinex_Comments(s, "The output values are averages of stochastic estimations on sampling intervals");
  // here to add "alternatives" for parameter:
  Str.sprintf("%-22s %29s", "CONSTR. ZD OFFSET", "N/A");
  s << " " << Str << "\n";
  Str.sprintf("%-22s %29s", "CONSTR. ZD RATE", "N/A");
  s << " " << Str << "\n";

  sinex_Comments(s, "_______KEYWORD________ ____________GRADIENTS________");
  Str.sprintf("%-22s %29d", "SAMPLING GRADIENTS", (int)Cfg->tropSamplInterval());
  s << " " << Str << "\n";

  Str.sprintf("%-22s %29s", "APRIORI GRADIENTS", "N/A");
  s << " " << Str << "\n";
  Str.sprintf("%-22s %29s", "GRADIENT MODEL", (const char*)GrModel);
  s << " " << Str << "\n";
  sinex_Comments(s, "The output values are averages of stochastic estimations on sampling intervals");
  Str.sprintf("%-22s %29s", "CONSTR. GR OFFSET", "N/A");
  s << " " << Str << "\n";
  Str.sprintf("%-22s %29s", "CONSTR. GR RATE", "N/A");
  s << " " << Str << "\n";
  s << "-TROP/DESCRIPTION\n";
};

void sinex_tro_TropSolutionStation(QTextStream& s, SBRunManager* /*RunMgr*/, 
				   SBVLBISession& /*Session*/, SBStationInfo *SI, 
				   SBStation* Station, SBTropRecordList* Trops)
{
  QString	Str, SiteCode("");

  sinex_Comments(s, SI->aka());
  sinex_Comments(s, "____________________  TROTOT  TROWET  STDDEV  GRTOTN  STDDEV  GRTOTE  STDDEV");

  //SiteCode = SI->aka().left(4);
  SiteCode = Station->charID4SINEX();
  if (SiteCode=="")
    SiteCode = SI->aka().left(4);
  for (SBTropRecord *TropRec=Trops->first(); TropRec; TropRec=Trops->next())
    {
      if (TropRec->numPts())
	{
	  Str.sprintf("%s %s   %8.2f  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f   "
#ifdef METEO_PARAMETERS_IN_TRO_FILE
		      "%6.2f %6.2f %6.2f"
#endif
		      ,
		      (const char*)SiteCode, (const char*)(TropRec->toString(SBMJD::F_SINEX)),
		      TropRec->totZenith(), TropRec->wetZenith(), TropRec->wetZenithErr(),
		      TropRec->gradNorth(), TropRec->gradNorthErr(),
		      TropRec->gradEast(),  TropRec->gradEastErr()
#ifdef METEO_PARAMETERS_IN_TRO_FILE
		      ,TropRec->temp(),
		      TropRec->pres(), 
		      TropRec->rh()
#endif
		      );
	  /* for debug:
	    Str.sprintf("%s %s %s %4d  %8.2f  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f",
	    (const char*)SiteCode, 
	    (const char*)(TropRec->toString(SBMJD::F_Short)),
	    (const char*)(TropRec->realEpoch().toString(SBMJD::F_Short)),
	    TropRec->numPts(),
	    TropRec->totZenith(), TropRec->wetZenith(), TropRec->wetZenithErr(),
	    TropRec->gradNorth(), TropRec->gradNorthErr(),
	    TropRec->gradEast(),  TropRec->gradEastErr()
	    );
	  */
	  s << " " << Str << "\n";
	};
    };
  
};

void sinex_tro_TropSolutionBlock(QTextStream& s, 
				 SBRunManager* RunMgr, 
				 SBVLBISession& Session, 
				 QDict<SBTropRecordList>* TropByAka)
{
  SB_TRF*		TRF=RunMgr->trf();
  SBStationInfoList	*Stations = Session.stationList();
  SBStation		*Station=NULL;
  QString		Str, SiteCode("");
  SBTropRecordList	*Trops = NULL;

  s << "+TROP/SOLUTION\n";
  for (SBStationInfo *SI = Stations->first(); SI; SI = Stations->next())
    {
      if (SI->procNum() && TropByAka && (Trops=TropByAka->find(SI->aka())))
	{
	  SBStationID	id(*SI);
	  if ((Station = TRF->find(id)))
	    sinex_tro_TropSolutionStation(s, RunMgr, Session, SI, Station, Trops);
	  else
	    Log->write(SBLog::ERR, SBLog::STATION, "sinex_tro_SiteIDBlock: cannot find the station [" + 
		       SI->toString() + "] in the TRF database");
	};
    };
  s << "-TROP/SOLUTION\n";
};

void sinex_tro_MakeMeansTrops(double dt_, int StartSec_, 
			      SBTropRecordList* TropsOrig, 
			      SBTropRecordList* TropsMeans)
{
  SBTropRecord *TR=NULL;
  double	dt = dt_/DAY2SEC;
  SBMJD		T(*TropsOrig->first());
  SBMJD		T0(T);

  T0.setTime(StartSec_/86400.0); // 18:00

  int		Idx = -999, n = -888;
  
  for (SBTropRecord *TropRec=TropsOrig->first(); TropRec; TropRec=TropsOrig->next())
    {
      if (TropRec->numPts())
	{
	  if (Idx!=(n=(int)floor((*TropRec - T0 + dt/2.0)/dt)))
	    {
	      Idx=n;
	      TR = new SBTropRecord;
	      *(SBMJD*)TR = T0 + Idx*dt;
	      TropsMeans->append(TR);
	      //	      std::cout << "Collecting for the date " << TR->toString(SBMJD::F_Short) 
	      //			<< "; [" << SBMJD(T0 + (Idx-0.5)*dt).toString(SBMJD::F_TimeShort) 
	      //			<< "--" << SBMJD(T0 + (Idx+0.5)*dt).toString(SBMJD::F_TimeShort) 
	      //			<<"] (" << Idx << ")\n";
	    };
 	  TR->collect(TropRec);
	  //	  std::cout << "\t\t" << TropRec->toString(SBMJD::F_Short) << "\n";
	};
    };

  TropsMeans->sort();
  
  for (SBTropRecord *TropRec=TropsMeans->first(); TropRec; TropRec=TropsMeans->next())
    {
      TropRec->processCollected();
    };
};
/*==============================================================================================*/






/*==============================================================================================*/
/*												*/
/* class SB_TRF's friends implementation (continue)						*/
/*												*/
/*==============================================================================================*/
SBTS_SINEX &operator<<(SBTS_SINEX& s, SB_TRF&)
{
  //
  QString	DescriptionItem("Main Astronomical Observatory, NASU, Kiev");
  QString	OutputItem     ("Results of VLBI data analysis in SINEX format");
  QString	InputItem      ("A lot of files: VLBI data, initial TRF, CRF and EOP, etc.");
  //


  SBParameterList	*LastGlb	= NULL;
  SBSymMatrix		*LastP		= NULL;
  SBSINEXParameterList	*SINEX_List	= NULL;
  SBSymMatrix		*SINEX_P	= NULL;


  SBRunManager	*RunMgr = s.runMgr();
  SBMJD		CreationTime(SBMJD::currentMJD());


  if (!RunMgr)
    return (SBTS_SINEX&)(s << "%=not a valid SINEX file\n%END\n");

  QString		Str("");
  double		SINEX_VERSION = 2.00;


  RunMgr->solution()->loadGlbCor(LastGlb, LastP);
  if (LastGlb && LastP /*&& LastGlb->count()*LastP->n()*/)
    {
      SINEX_List = new SBSINEXParameterList;
      SINEX_List->setAutoDelete(TRUE);

      collectListOfSINEXParameters(LastGlb, LastP, SINEX_List, SINEX_P, RunMgr);
      
      // start of output
      sinex_HeaderLine(s, SINEX_VERSION, CreationTime, 
		       RunMgr->prj()->tStart(), RunMgr->prj()->tFinis(), 
		       SINEX_List->count(), SNX_SC_STATIONS, FALSE);
      
      
      sinex_FileReferenceBlock(s, DescriptionItem, OutputItem, InputItem);
      sinex_FileCommentBlock(s, "no comment");
      sinex_InputHistoryBlock(s, SINEX_VERSION, CreationTime, 
			      RunMgr->prj()->tStart(), RunMgr->prj()->tFinis(), 
			      SINEX_List->count(), SNX_SC_STATIONS, FALSE);
      
      sinex_InputFilesBlock(s, CreationTime, "mao2003a.trf", 
			    "Results of VLBI data analysis");
      sinex_InputAcknowledgementsBlock(s);  
      
      
      QString	NutCode("IAU1980"), NutDescr("General model");
      QString	PreCode("IAU1976"), PreDescr("General model");
      
      switch (RunMgr->prj()->cfg().nutModel())
	{
	default:
	case SBConfig::NM_IAU1980:
	  NutCode = "IAU1980";
	  NutDescr= QString("IAU-1980 nutation + ") + RunMgr->frame()->eop()->label().toString() +
	    " ; " + (RunMgr->prj()->cfg().sysTransform()==SBConfig::STR_Classic?
		     "Equinox-based transform.":"CEO-based transform.");
	  PreCode = "IAU1976";
	  PreDescr= "IAU-1976 precession model";
	  break;
	case SBConfig::NM_IAU1996:
	  NutCode = "IERS1996";
	  NutDescr= QString("IAU-1996 nutation + ") + RunMgr->frame()->eop()->label().toString() +
	    " ; " + (RunMgr->prj()->cfg().sysTransform()==SBConfig::STR_Classic?
		     "Equinox-based transform.":"CEO-based transform.");
	  PreCode = "IAU1976";
	  PreDescr= "IAU-1976 precession model";
	  break;
	case SBConfig::NM_IAU2000:
	  NutCode = "IAU2000a";
	  NutDescr= QString("IAU-2000 nutation + ") + RunMgr->frame()->eop()->label().toString() +
	    " ; " + (RunMgr->prj()->cfg().sysTransform()==SBConfig::STR_Classic?
		     "Equinox-based transform.":"CEO-based transform.");
	  PreCode = "IERS1996";
	  PreDescr= "IERS-1996 precession model";
	  break;
	};
      
      sinex_NutationDataBlock  (s, NutCode, NutDescr);
      sinex_PrecessionDataBlock(s, PreCode, PreDescr);

      sinex_SourceIDBlock(s, RunMgr->crf(), RunMgr->prj(), NULL, NULL);
      
      sinex_SiteIDBlock(s, RunMgr->trf(), RunMgr->prj(), NULL);
      
      sinex_SiteEccentricityBlock(s, RunMgr->trf(), RunMgr->prj(), RunMgr->vlbi(), NULL);
      
      sinex_SolutionEpochsBlock(s, RunMgr->trf(), NULL);
  
      sinex_SolutionStatisticsBlock(s, RunMgr->solution()->statGeneral());
      
      sinex_SolutionEstimateBlock(s, SINEX_List);

      sinex_SolutionAprioriBlock(s, SINEX_List);

      sinex_SolutionMatrixEstimateBlock(s, TRUE, SINEX_P);
      
      sinex_TrailerLine(s);
      // end of output
      
      
      if (LastGlb)
	delete LastGlb;
      if (LastP)
	delete LastP;

      if (SINEX_List)
	delete SINEX_List;
      if (SINEX_P)
	delete SINEX_P;
    }
  else
    {
      Log->write(SBLog::WRN, SBLog::RUN,  "SINEX I/O: nothing to output");
    };
  
  return s;
};

SBTS_SINEX &operator<<(SBTS_SINEX& s, SBVLBISession& Session)
{
  //
  QString	DescriptionItem("Main Astronomical Observatory, NASU, Kiev");
  QString	OutputItem     ("Tropospheric estimates from a VLBI session");
  QString	InputItem      ("VLBI data");
  //

  SBRunManager	*RunMgr = s.runMgr();
  const SB_TRF	*TRF	= RunMgr->trf();
  const SB_CRF	*CRF	= RunMgr->crf();
  SBMJD		CreationTime(SBMJD::currentMJD());

  if (!RunMgr)
    return (SBTS_SINEX&)(s << "%=not a valid SINEX_TRO file\n%END\n");
  
  InputItem = "The VLBI session " + Session.officialName() + " (" + Session.name() +
    QString().sprintf("_V%03d", Session.version()) + ")";


  // create the dict of stations
  SBTropRecordList		*Trops = NULL;
  SBTropRecord			*TropRec = NULL;
  QDict<SBTropRecordList>	TropByAka(Session.stationList()->count() + 5);
  QDict<SBTropRecordList>	TropMeansByAka(Session.stationList()->count() + 5);
  QDict<SBTropRecordList>	*TBA_work=NULL;

  TropByAka.setAutoDelete(TRUE);
  TropMeansByAka.setAutoDelete(TRUE);

  // fill it
  for (SBStationInfo *SI = Session.stationList()->first(); SI; SI = Session.stationList()->next())
    if (SI->procNum())
      {
	SBStationID	id(*SI);
	SBStation	*Station=NULL;
	if ((Station = TRF->find(id)))
	  {
	    if (!TropByAka.find(SI->aka()))
	      {
		Trops = new SBTropRecordList;
		Trops->setAutoDelete(TRUE);
		TropByAka.insert(SI->aka(), Trops);
	      }
	    else
	      Log->write(SBLog::ERR, SBLog::RUN, "TROP2SNX: Dict's misbehaviour");
	  };
      };
  //
  // station #1:
  SBStationInfo	*St_1Info = NULL;
  SBStation	*St_1	  = NULL;
  // station #2:
  SBStation	*St_2	  = NULL;
  SBStationInfo	*St_2Info = NULL;
  // source:
  SBSource	*Src	  = NULL;
  SBSourceInfo	*SrInfo   = NULL;
  bool   	IsArtMeteo= FALSE;
  
  for (SBObsVLBIEntry* Obs=Session.first(); Obs; Obs=Session.next()) 
    if (Obs->isAttr(SBObsVLBIEntry::processed))
      {
	if (!(St_1=Obs->station1(TRF, St_1Info)))
	  Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
		     "TROP2SNX: cannot find the station #1 [" + (St_1Info?St_1Info->toString():"NULL") + 
		     "] aka [" + (St_1Info?St_1Info->aka():"NULL") + "] in the catalogue");
	else if (!(St_2=Obs->station2(TRF, St_2Info)))
	  Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
		     "TROP2SNX: cannot find the station #2 [" + (St_2Info?St_2Info->toString():"NULL") + 
		     "] aka [" + (St_2Info?St_2Info->aka():"NULL") + "] in the catalogue");
	else if (!(Src=Obs->source(CRF, SrInfo)))
	  Log->write(SBLog::ERR, SBLog::SOURCE | SBLog::DELAYALL, 
		     "TROP2SNX: cannot find the source [" + (SrInfo?SrInfo->name():"NULL") + 
		     "] aka [" + (SrInfo?SrInfo->aka():"NULL") + "] in the catalogue");
	else 
	  {
	    // update parameters:
	    St_1->calcDisplacement(RunMgr, *Obs, FALSE);
	    St_2->calcDisplacement(RunMgr, *Obs, FALSE);
	    Src ->updateParameters(RunMgr, *Obs);
	    
	    // first station:
	    if ((Trops=TropByAka.find(St_1Info->aka())))
	      {
		int	j=-1;
		TropRec = new SBTropRecord;
		//
		*(SBMJD*)TropRec = *Obs;
		TropRec->setTotZenith(1000.0*SBDelay::CLight*Obs->zenithDelay_1());
		TropRec->setWetZenith(1000.0*(St_1->p_Zenith()->v() + St_1Info->zw()));
		TropRec->setGradNorth(1000.0*St_1->p_AtmGradN()->v());
		TropRec->setGradEast (1000.0*St_1->p_AtmGradE()->v());
		TropRec->setWetZenithErr(1000.0*St_1->p_Zenith()->e());
		TropRec->setGradNorthErr(1000.0*St_1->p_AtmGradN()->e());
		TropRec->setGradEastErr (1000.0*St_1->p_AtmGradE()->e());
		TropRec->setNumPts(St_1->p_Zenith()->num());
		//		TropRec->setTemp(273.15 + Obs->ambientT_1());
		IsArtMeteo = St_1Info->isAttr(SBStationInfo::BadMeteo) || St_1Info->isAttr(SBStationInfo::ArtMeteo);
		TropRec->setTemp(Obs->ambientT_1(IsArtMeteo));
		TropRec->setPres(Obs->ambientP_1(IsArtMeteo));
		TropRec->setRH  (Obs->ambientH_1(IsArtMeteo));
		if ( (j=Trops->find(TropRec)) == -1)
		  {
		    Trops->append(TropRec);
		  }
		else if ( *Trops->at(j) != *TropRec)
		  Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
			     "TROP2SNX: wrong order of trop-records for the station #1 [" + 
			     (St_1Info?St_1Info->toString():"NULL") + 
			     "] aka [" + (St_1Info?St_1Info->aka():"NULL") + "] in the trop-record-list");
		else
		  {
		    if (TropRec) 
		      delete TropRec;
		    TropRec = NULL;
		  };
	      }
	    else
	      Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
			 "TROP2SNX: cannot find the station #1 [" + (St_1Info?St_1Info->toString():"NULL") + 
			 "] aka [" + (St_1Info?St_1Info->aka():"NULL") + "] in the trop dict");

	    // second station:
	    if ((Trops=TropByAka.find(St_2Info->aka())))
	      {
		int	j=-1;
		TropRec = new SBTropRecord;
		//
		*(SBMJD*)TropRec = *Obs;
		TropRec->setTotZenith(1000.0*SBDelay::CLight*Obs->zenithDelay_2());
		TropRec->setWetZenith(1000.0*(St_2->p_Zenith()->v() + St_2Info->zw()));
		TropRec->setGradNorth(1000.0*St_2->p_AtmGradN()->v());
		TropRec->setGradEast (1000.0*St_2->p_AtmGradE()->v());
		TropRec->setWetZenithErr(1000.0*St_2->p_Zenith()->e());
		TropRec->setGradNorthErr(1000.0*St_2->p_AtmGradN()->e());
		TropRec->setGradEastErr (1000.0*St_2->p_AtmGradE()->e());
		TropRec->setNumPts(St_2->p_Zenith()->num());
		//		TropRec->setTemp(273.15 + Obs->ambientT_2());
		IsArtMeteo = St_1Info->isAttr(SBStationInfo::BadMeteo) || St_1Info->isAttr(SBStationInfo::ArtMeteo);
		TropRec->setTemp(Obs->ambientT_2(IsArtMeteo));
		TropRec->setPres(Obs->ambientP_2(IsArtMeteo));
		TropRec->setRH  (Obs->ambientH_2(IsArtMeteo));
		if ( (j=Trops->find(TropRec)) == -1)
		  {
		    Trops->append(TropRec);
		  }
		else if ( *Trops->at(j) != *TropRec)
		  Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
			     "TROP2SNX: wrong order of trop-records for the station #2 [" + 
			     (St_2Info?St_2Info->toString():"NULL") + 
			     "] aka [" + (St_2Info?St_2Info->aka():"NULL") + "] in the trop-record-list");
		else
		  {
		    if (TropRec) 
		      delete TropRec;
		    TropRec = NULL;
		  };
	      }
	    else
	      Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
			 "TROP2SNX: cannot find the station #2 [" + (St_2Info?St_2Info->toString():"NULL") + 
			 "] aka [" + (St_2Info?St_2Info->aka():"NULL") + "] in the trop dict");
	  };
      };

  QDictIterator<SBTropRecordList> it(TropByAka);
  while (it.current()) 
    {
      (it.current())->sort();
      ++it;
    };
  
  //  std::cout << "Processing session " << Session.name() << "\n";

  if (RunMgr->prj()->cfg().tropSamplInterval() < 60.0)
    TBA_work = &TropByAka;
  else
    {
      it.toFirst();
      while ((Trops=it.current()))
	{
	  SBTropRecordList* TropsMeans = new SBTropRecordList;
	  TropsMeans->setAutoDelete(TRUE);
	  TropMeansByAka.insert(it.currentKey(), TropsMeans);

	  //	  std::cout << "Processing station " << it.currentKey() << "\n";

	  sinex_tro_MakeMeansTrops(RunMgr->prj()->cfg().tropSamplInterval(), 
				   RunMgr->prj()->cfg().tropStartSec(),
				   Trops, TropsMeans);
	  ++it;
	};
      TBA_work = &TropMeansByAka;
    };
  
  // make an output
  double		SINEX_TRO_VER = 9.99;
  
  sinex_tro_HeaderLine(s, SINEX_TRO_VER, CreationTime, *Session.first(), *Session.last());
  sinex_FileReferenceBlock(s, DescriptionItem, OutputItem, InputItem);
  sinex_tro_SiteIDBlock(s, RunMgr->trf(), Session.stationList());
  sinex_tro_StaCoordinatesBlock(s, RunMgr->trf(), Session.stationList(), RunMgr, Session.tMean());
  sinex_tro_TropDescriptionBlock(s, RunMgr, Session);
  sinex_tro_TropSolutionBlock(s, RunMgr, Session, TBA_work);
  sinex_tro_TrailerLine(s);
  // end of output

  
  return s;
};

SBTS_SINEX &operator>>(SBTS_SINEX& s, SB_TRF& /*TRF*/)
{
  
  
  return s;
};



void writeNormalEquationSystem(SBTS_SINEX& s, 
			       SBParameterList* Ys, SBVector* Zy, SBUpperMatrix* Ry,
			       SBParameterList* Xs, SBVector* Zx, SBUpperMatrix* Rx, SBMatrix* Rxy,
			       const QString& BatchName, const QString& File4Output)
{
  //
  QString	DescriptionItem("Main Astronomical Observatory, NASU, Kiev");
  QString	OutputItem     ("Single session VLBI solution");
  QString	InputItem      ("VLBI database(s): " + BatchName);
  //

  SBRunManager	*RunMgr = s.runMgr();
  SBMJD		CreationTime(SBMJD::currentMJD());

  bool		IsRptStations = RunMgr->prj()->cfg().reportNormalEqsSTCs();
  bool		IsRptSources  = RunMgr->prj()->cfg().reportNormalEqsSRCs();
  bool		IsRptEOP      = RunMgr->prj()->cfg().reportNormalEqsEOPs();


  if (!RunMgr)
    {
      s << "%=not a valid SINEX file\n%END\n";
      Log->write(SBLog::ERR, SBLog::IO, "writeNormalEquationSystem: cannot write DSINEX file: RunMgr is NULL");
      return;
    };

  if (!Xs)
    {
      s << "%=not a valid SINEX file\n%END\n";
      Log->write(SBLog::ERR, SBLog::IO, "writeNormalEquationSystem: cannot write DSINEX file: Xs is NULL");
      return;
    };
  if (!Ys)
    {
      s << "%=not a valid SINEX file\n%END\n";
      Log->write(SBLog::ERR, SBLog::IO, 
		 "writeNormalEquationSystem: cannot write DSINEX file: Ys is NULL");
      return;
    };

  if (!Zy || !Ry || !Zx || !Rx || !Rxy)
    {
      s << "%=not a valid SINEX file\n%END\n";
      Log->write(SBLog::ERR, SBLog::IO, 
		 "writeNormalEquationSystem: cannot write DSINEX file: Zy or Ry or Zx or Rx or Rxy is/are NULL");
      return;
    };


  unsigned int	i, j;
  int		Ny  = Ys->count();
  int		Nx  = Xs->count();
  unsigned int	NOP = Ny+Nx;

  // fix time marks:
  if (Ny)
    for (SBParameter *p=Ys->first(); p; p=Ys->next())
      p->invertAux();
  
  
  if (!NOP)
    {
      s << "%=not a valid SINEX file\n%END\n";
      Log->write(SBLog::ERR, SBLog::IO, 
		 "writeNormalEquationSystem: cannot write DSINEX file: nothing is solved (NOP==0)");
      return;
    };
  
  
  // collect the list of all parameters:
  SBParameterList*	CommonList   = new SBParameterList;
  SBParameterList*	SelectedList = new SBParameterList;
  
  CommonList->setAutoDelete(FALSE);
  SelectedList->setAutoDelete(FALSE);
  CommonList->clear();
  SelectedList->clear();
  
  for (SBParameter* p=Xs->first(); p; p=Xs->next())
    {
      if (!CommonList->find(p->name()))
	CommonList->append(p);
      else
	Log->write(SBLog::ERR, SBLog::ESTIMATOR || SBLog::RUN, 
		   "writeNormalEquationSystem: CommonList/Xs misbehaviour");
    };
  for (SBParameter* p=Ys->first(); p; p=Ys->next())
    {
      if (!CommonList->find(p->name()))
	CommonList->append(p);
      else
	Log->write(SBLog::ERR, SBLog::ESTIMATOR || SBLog::RUN, 
		   "writeNormalEquationSystem: CommonList/Ys misbehaviour");
    };
  
  if (CommonList->count() != NOP)
    Log->write(SBLog::ERR, SBLog::IO, 
	       "writeNormalEquationSystem: Number of parameters in CommonList (" + 
	       QString().setNum(CommonList->count()) + ") is not equal to the sum of Xs and Ys (" + 
	       QString().setNum(NOP) + ")");
  
  // fill the matrices:
  SBUpperMatrix	*R_Total = new SBUpperMatrix(NOP, "R_Total");
  SBVector	*Z_Total = new SBVector (NOP, "Z_Total");
  for (int i=0; i<Nx; i++)
    {
      for (int j=i; j<Nx; j++)
	R_Total->set(i, j, Rx->at(i, j));
      for (int j=0; j<Ny; j++)
	R_Total->set(i, Nx+j, Rxy->at(i, j));
      Z_Total->set(i, Zx->at(i));
    };
  for (int i=0; i<Ny; i++)
    {
      for (int j=i; j<Ny; j++)
	R_Total->set(Nx+i, Nx+j, Ry->at(i, j));
      Z_Total->set(Nx+i, Zy->at(i));
    };

  //WWWWWW//
  // now, eleminate parameters which are not included in the report:
  SBParameterList*	IMList = new SBParameterList;
  IMList->setAutoDelete(FALSE);
  IMList->clear();
  for (SBParameter* p=CommonList->first(); p; p=CommonList->next())
    {
      if (!IMList->find(p->name()))
	{
	  IMList->append(p);
	  if (
	      (
	       (IsRptStations && p->name().contains("St: ") && p->name().contains("coord-")) ||
	       (IsRptSources && p->name().contains("So: ") && (p->name().contains("RA") || p->name().contains("DN"))) ||
	       (IsRptEOP && p->name().contains("EOP: "))
	       ) && p->num() && p->num_aux())
	    p->setPrefix('9');
	  else
	    p->setPrefix('0');
	}
      else
	Log->write(SBLog::ERR, SBLog::ESTIMATOR || SBLog::RUN, 
		   "writeNormalEquationSystem: CommonList/IMList misbehaviour");
    };
  IMList->sort();

  /*
    std::cout << "IsRptStations: " << IsRptStations << "\n";
    std::cout << "IsRptSources: "  << IsRptSources << "\n";
    std::cout << "IsRptEOP: "      << IsRptEOP << "\n";
    std::cout << "____________________________\n";
    for (SBParameter* p=IMList->first(); p; p=IMList->next())
      std::cout << p->name() << " (prefix: " <<  "\n";
    std::cout << "____________________________\n";
    */


  // get indexes of IMList items in CommonList:
  unsigned int	*IMIdx = new unsigned int[NOP];
  for (i=0; i<NOP; i++)
    *(IMIdx+i) = CommonList->idx(IMList->at(i));

  // ok, free it:
  if (CommonList)
    {
      delete CommonList;
      CommonList = NULL;
    };

  // fill IM matrix and vector:
  SBMatrix	*R_IM = new SBMatrix(NOP, NOP, "Intermadiate total R");
  SBVector	*Z_IM = new SBVector(NOP,      "Intermadiate total Z");
  for (i=0; i<NOP; i++)
    {
      Z_IM->set(i, Z_Total->at(*(IMIdx+i)));
      for (j=i; j<NOP; j++)
	R_IM->set(i,j,  R_Total->at(*(IMIdx+i), *(IMIdx+j)));
    };
  delete[] IMIdx;
  // del 'Total' matrix and vector:
  delete R_Total;
  R_Total = NULL;
  delete Z_Total;
  Z_Total = NULL;

  // make triangularization:
  SBVector	*U = new SBVector(NOP, "Aux Vector U");
  U->clear();
  double	ss, g;
  for (unsigned int l=0; l<NOP; l++)
    {
      ss = R_IM->at(l,l)*R_IM->at(l,l);
      for (i=l+1; i<NOP; i++) ss+=R_IM->at(i,l)*R_IM->at(i,l);
      if (ss>0.0)
        {
          ss = -sign(R_IM->at(l,l))*sqrt(ss);
          U->set(l, R_IM->at(l,l) - ss);
          for (i=l+1; i<NOP; i++)
	    U->set(i, R_IM->at(i,l));
          R_IM->set(l,l, ss);
          ss = 1.0/(ss*U->at(l));                               // betta
          for (j=l+1; j<NOP; j++)				// matrix R_IM
            {
              for (g=0.0,i=l; i<NOP; i++) g+=U->at(i)*R_IM->at(i,j);
              for (i=l; i<NOP; i++) (*R_IM)(i,j)+=ss*g*U->at(i);
            };
          for (g=0.0, i=l; i<NOP; i++) g+=U->at(i)*Z_IM->at(i);	// vector Z_IM
          for (i=l; i<NOP; i++) (*Z_IM)(i)+=ss*g*U->at(i);
        }
      else
        std::cerr << "got a zero during Householdering at l[" + QString().setNum(l) + "]\n";
    };
  delete U;
  // Warning: only upper right part of R_IM is valid.

  // select parameters for DSINEX output:
  for (SBParameter* p=IMList->first(); p; p=IMList->next())
    if (p->prefix()=='9') // parameters are marked for selection already
      {
	if (!SelectedList->find(p->name()))
	  SelectedList->append(p);
	else
	  Log->write(SBLog::ERR, SBLog::ESTIMATOR || SBLog::RUN, 
		     "writeNormalEquationSystem: IMList/SelectedList misbehaviour");
      };
  unsigned int		N = SelectedList->count();

  if (!N)
    {
      s << "%=not a valid SINEX file\n%END\n";
      Log->write(SBLog::INF, SBLog::IO, 
		 "writeNormalEquationSystem: cannot write DSINEX file: nothing to write (N=0); NOP=" + 
		 QString().setNum(NOP));
      delete   SelectedList;
      delete   IMList;
      delete   R_IM;
      delete   Z_IM;
      return;
    };
  
  Log->write(SBLog::DBG, SBLog::ESTIMATOR || SBLog::RUN || SBLog::IO, 
	     "writeNormalEquationSystem: Collected " + QString().setNum(N) + " DSINEX parameters from the " +
	     QString().setNum(NOP) + " toal ones");
  
  // fill R-matrix and Z-vector for selected list:
  SBUpperMatrix	*R_Selected = new SBUpperMatrix(N, "R_Selected");
  SBVector	*Z_Selected = new SBVector     (N, "Z_Selected");
  for (i=0; i<N; i++)
    {
      Z_Selected->set(i,  Z_IM->at(NOP-N+i));
      for (j=i; j<N; j++)
	R_Selected->set(i,j,  R_IM->at(NOP-N+i, NOP-N+j));
    };

  delete   R_IM;
  delete   Z_IM;
  delete   IMList;


  // Rescale:
  // scale matix:
  double *ReScaleFactor = new double[N];
  for (i=0; i<N; i++)
    {
      *(ReScaleFactor+i) = 1.0;
      SBParameter *p=SelectedList->at(i);
      if (p)
	{
	  if (p->name().contains("EOP: X_p"))
	    *(ReScaleFactor+i) = 1.0/RAD2MAS;
	  if (p->name().contains("EOP: Y_p"))
	    *(ReScaleFactor+i) = 1.0/RAD2MAS;
	  if (p->name().contains("EOP: UT"))
	    *(ReScaleFactor+i) = 1.0/(DAY2SEC*1000.0);
	  if (p->name().contains("EOP: Psi"))
	    *(ReScaleFactor+i) = 1.0/RAD2MAS;
	  if (p->name().contains("EOP: Eps"))
	    *(ReScaleFactor+i) = 1.0/RAD2MAS;
	}
      else 
	Log->write(SBLog::ERR, SBLog::IO, 
		   "writeNormalEquationSystem: Got NULL for parameter #" + 
		   QString().setNum(i) + " in the SelectedList");
    };
  for (i=0; i<N; i++)
    for (j=i; j<N; j++)
      (*R_Selected)(i,j)*=*(ReScaleFactor+j);
  delete[] ReScaleFactor;


  // calculate the Normal matrix and corresponding vector for selected parameters:
  SBSymMatrix	*N_SNX = new SBSymMatrix(N, "N_SNX");
  SBVector	*B_SNX = new SBVector   (N, "B_SNX");

  RTR(*N_SNX, *R_Selected);
  *B_SNX = matT_x_vec(*R_Selected, *Z_Selected);
  
  // del 'em:
  delete R_Selected;
  R_Selected = NULL;
  delete Z_Selected;
  Z_Selected = NULL;


  // process a report:
  
  SBMJD		TFirst=SelectedList->at(0)->tStart();
  SBMJD		TLast =SelectedList->at(0)->tFinis();

  if (TFirst==TZero)
    std::cerr << "tStart==TZero: " << SelectedList->at(0)->name() << "\n";
  if (TLast==TZero)
    std::cerr << "tFinis==TZero: " << SelectedList->at(0)->name() << "\n";

  for (SBParameter *p=SelectedList->first(); p; p=SelectedList->next())
    {
      if (TFirst>p->tStart())
	TFirst=p->tStart();
      if (TLast<p->tFinis())
	TLast=p->tFinis();
    };

  QString	NutCode("IAU1980"), NutDescr("General model");
  QString	PreCode("IAU1976"), PreDescr("General model");
  
  switch (RunMgr->prj()->cfg().nutModel())
    {
    default:
    case SBConfig::NM_IAU1980:
      NutCode = "IAU1980";
      NutDescr= QString("IAU-1980 nutation + ") + RunMgr->frame()->eop()->label().toString() +
	"; " + (RunMgr->prj()->cfg().sysTransform()==SBConfig::STR_Classic?
		 "Equinox-based transform.":"CEO-based transform.");
      PreCode = "IAU1976";
      PreDescr= "IAU-1976 precession model";
      break;
    case SBConfig::NM_IAU1996:
      NutCode = "IERS1996";
      NutDescr= QString("IAU-1996 nutation; ") + 
	(RunMgr->prj()->cfg().sysTransform()==SBConfig::STR_Classic?
	 "Equinox-based transform.":"CEO-based transform.");
      PreCode = "IAU1976";
      PreDescr= "IAU-1976 precession model";
      break;
    case SBConfig::NM_IAU2000:
      NutCode = "IAU2000a";
      NutDescr= QString("IAU-2000 nutation; ") + 
	(RunMgr->prj()->cfg().sysTransform()==SBConfig::STR_Classic?
	 "Equinox-based transform.":"CEO-based transform.");
      PreCode = "IERS1996";
      PreDescr= "in fact, it is IAU-2000 precession model, however, SINEX ver 2.00 does not reserve a code for it";
      break;
    };
  
  // create the dict of stations
  double		SINEX_VERSION = 2.00;

  
  SBSINEXParameterList	*SINEX_List = new SBSINEXParameterList;
  SINEX_List->setAutoDelete(TRUE);
  collectListOfSINEXParameters4NEQ(SelectedList, SINEX_List, RunMgr);

  // output:
  sinex_HeaderLine(s, SINEX_VERSION, CreationTime, TFirst, TLast, N, 
		   SNX_SC_STATIONS | (IsRptSources?SNX_SC_SOURCES:0) | SNX_SC_EOP, TRUE);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");


  sinex_FileReferenceBlock(s, DescriptionItem, OutputItem, InputItem);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");

  sinex_FileCommentBlock(s, "no comment");
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");

  sinex_InputHistoryBlock(s, SINEX_VERSION, CreationTime, TFirst, TLast, N, 
			  SNX_SC_STATIONS | SNX_SC_SOURCES | SNX_SC_EOP, TRUE);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");
  
  sinex_InputFilesBlock(s, CreationTime, File4Output,
			"Normal Equations System ");
  //                     __________Description_________
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");

  sinex_InputAcknowledgementsBlock(s);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");
  
  
  sinex_NutationDataBlock  (s, NutCode, NutDescr);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");

  sinex_PrecessionDataBlock(s, PreCode, PreDescr);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");
  
  if (IsRptSources)
    {
      sinex_SourceIDBlock(s, RunMgr->crf(), RunMgr->prj(), SelectedList, SINEX_List);
      sinex_Comments(s, "");
      sinex_Comments(s, " -----------------------------------------------------------------------------");
      sinex_Comments(s, "");
    };
  
  sinex_SiteIDBlock(s, RunMgr->trf(), RunMgr->prj(), SelectedList);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");

  // siteReceiver
  // siteAntenna
  
  sinex_SiteEccentricityBlock(s, RunMgr->trf(), RunMgr->prj(), RunMgr->vlbi(), SelectedList);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");
  
  sinex_SolutionEpochsBlock(s, RunMgr->trf(), SelectedList);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");
  
  sinex_SolutionStatisticsBlock(s, RunMgr->solution()->stat4CurrentBatch());
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");
  
  //  sinex_SolutionEstimateBlock(s, SINEX_List);
  
  sinex_SolutionAprioriBlock(s, SINEX_List);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");
  

  sinex_SolutionNormalEquationVectorBlock(s, SINEX_List, B_SNX);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");

  sinex_SolutionNormalEquationMatrixBlock(s, N_SNX);
  sinex_Comments(s, "");
  sinex_Comments(s, " -----------------------------------------------------------------------------");
  sinex_Comments(s, "");

  //  sinex_SolutionMatrixEstimateBlock(s, TRUE, SINEX_P);
  
  sinex_TrailerLine(s);
  // end of output



  if (SINEX_List)
    {
      delete SINEX_List;
      SINEX_List = NULL;
    };

  if (N_SNX)
    {
      delete N_SNX;
      N_SNX = NULL;
    };

  if (B_SNX)
    {
      delete B_SNX;
      B_SNX = NULL;
    };

  if (SelectedList)
    {
      delete SelectedList;
      SelectedList = NULL;
    };

};
/*==============================================================================================*/
