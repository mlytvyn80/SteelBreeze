/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 1998-2004 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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

#include <qapplication.h>


#include "SbGeoObsVLBIStat.H"
#include "SbSetup.H"



/*==============================================================================================*/

/*==============================================================================================*/
/*												*/
/* class SBObsVLBIStatistics implementation							*/
/*												*/
/*==============================================================================================*/
SBObsVLBIStatistics::SBObsVLBIStatistics(const QString& Name_) : SBNamed(Name_)
{
  SourcesStatistics.setAutoDelete(TRUE); 
  StationsStatistics.setAutoDelete(TRUE);
  IsGlobal=FALSE;
};

void SBObsVLBIStatistics::collectStatistics(SBVLBISesInfoList* SessionList)
{
  SBVLBISet		*VLBI=NULL;
  SBVLBISession		*Session=NULL;

  GeneralStatistics.clear();
  SourcesStatistics.clear();


  mainSetOperation("Loading VLBI dataset");
  if (!(VLBI=SetUp->loadVLBI()))
    {
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": nothing to collect: VLBI==NULL");
      mainSetOperation("Waiting");
      return;
    };
  
  if (!SessionList)
    {
      SessionList = VLBI;
      IsGlobal    = TRUE;
    };

  if (!SessionList || !SessionList->count())
    {
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": nothing to collect: SessionList is NULL or empty");
      mainSetOperation("Waiting");
      return;
    };


  int		i=0;
  int		n=SessionList->count();
  
  mainSetOperation("Collecting statistics");
  mainPrBarSetTotalSteps(n);
  mainSetOperation("Processing");
  for (SBVLBISesInfo *si=SessionList->first(); si; si=SessionList->next())
    {
      if ( !si->isAttr(SBVLBISesInfo::notValid)  && (Session = VLBI->loadSession(si->name())) )
	{
	  mainSetSessName(Session->name());
	  
	  if (GeneralStatistics.tStartTotal()==TZero || GeneralStatistics.tStartTotal()>Session->tStart())
	    GeneralStatistics.setTStartTotal(Session->tStart());
	  if (GeneralStatistics.tFinisTotal()==TZero || GeneralStatistics.tFinisTotal()<Session->tFinis())
	    GeneralStatistics.setTFinisTotal(Session->tFinis());
	  
	  if (GeneralStatistics.tStartProc()==TZero || GeneralStatistics.tStartProc()>Session->tStart())
	    GeneralStatistics.setTStartProc(Session->tStart());
	  if (GeneralStatistics.tFinisProc()==TZero || GeneralStatistics.tFinisProc()<Session->tFinis())
	    GeneralStatistics.setTFinisProc(Session->tFinis());
	  
	  GeneralStatistics.incNumTotal(Session->numObs());
	  GeneralStatistics.incNumProc (Session->numObs()); //
	  
	  SBNamed		Named;
	  // sources:
	  SBObsVLBIStatSource	*SrcS=NULL;
	  for (SBSourceInfo *SrcI=Session->sourceList()->first(); SrcI; SrcI=Session->sourceList()->next())
	    {
	      Named.setName(SrcI->name());
	      if (!(SrcS=SourcesStatistics.find(&Named)))
		SourcesStatistics.inSort(SrcS = new SBObsVLBIStatSource(SrcI->name(), SrcI->aka()));
	      
	      SrcS->incNumTotal(SrcI->num());
	      SrcS->incNumProc (SrcI->procNum());
	      
	      if (SrcS->tStartTotal()==TZero || SrcS->tStartTotal()>Session->tStart())
		SrcS->setTStartTotal(Session->tStart());
	      if (SrcS->tFinisTotal()==TZero || SrcS->tFinisTotal()<Session->tFinis())
		SrcS->setTFinisTotal(Session->tFinis());

	      if (SrcI->procNum())
		{
		  if (SrcS->tStartProc()==TZero || SrcS->tStartProc()>Session->tStart())
		    SrcS->setTStartProc(Session->tStart());
		  if (SrcS->tFinisProc()==TZero || SrcS->tFinisProc()<Session->tFinis())
		    SrcS->setTFinisProc(Session->tFinis());
		};
	      
	      if (!(SrcS->statPerSession().find(Session)))
		{
		  SBObsVLBIStatRecord *SR = new SBObsVLBIStatRecord(Session->name());
		  SR->setTStartTotal(Session->tStart());
		  SR->setTFinisTotal(Session->tFinis());
		  SR->setNumTotal(SrcI->num());
		  SR->setNumProc (SrcI->procNum());
		  SR->setRMS     (SrcI->wrms());
		  if (SrcI->aka() != SrcS->aka())
		    SR->setAka(SrcI->aka());
		  if (SrcI->procNum())
		    {
		      SR->setTStartProc(Session->tStart());
		      SR->setTFinisProc(Session->tFinis());
		    };
		  SrcS->statPerSession().inSort(SR);
		}
	      else
		Log->write(SBLog::ERR, SBLog::DATA | SBLog::IO, ClassName() + 
			   ": Src: [" + SrcS->name() + "] duplicate session names: " + Session->name());
	    };

	  // stations:
	  SBObsVLBIStatStation	*StaS=NULL;
	  for (SBStationInfo *StaI=Session->stationList()->first(); StaI; StaI=Session->stationList()->next())
	    {
	      Named.setName(StaI->toString());
	      if (!(StaS=StationsStatistics.find(&Named)))
		StationsStatistics.inSort(StaS = new SBObsVLBIStatStation(StaI->toString(), StaI->aka()));
	      
	      StaS->incNumTotal(StaI->num());
	      StaS->incNumProc (StaI->procNum());
	      
	      if (StaS->tStartTotal()==TZero || StaS->tStartTotal()>Session->tStart())
		StaS->setTStartTotal(Session->tStart());
	      if (StaS->tFinisTotal()==TZero || StaS->tFinisTotal()<Session->tFinis())
		StaS->setTFinisTotal(Session->tFinis());

	      if (StaI->procNum())
		{
		  if (StaS->tStartProc()==TZero || StaS->tStartProc()>Session->tStart())
		    StaS->setTStartProc(Session->tStart());
		  if (StaS->tFinisProc()==TZero || StaS->tFinisProc()<Session->tFinis())
		    StaS->setTFinisProc(Session->tFinis());
		};
	      
	      if (!(StaS->statPerSession().find(Session)))
		{
		  SBObsVLBIStatRecord *SR = new SBObsVLBIStatRecord(Session->name());
		  SR->setTStartTotal(Session->tStart());
		  SR->setTFinisTotal(Session->tFinis());
		  SR->setNumTotal(StaI->num());
		  SR->setNumProc (StaI->procNum());
		  SR->setRMS     (StaI->wrms());
		  if (StaI->aka() != StaS->aka())
		    SR->setAka(StaI->aka());
		  if (StaI->procNum())
		    {
		      SR->setTStartProc(Session->tStart());
		      SR->setTFinisProc(Session->tFinis());
		    };
		  StaS->statPerSession().inSort(SR);
		}
	      else
		Log->write(SBLog::ERR, SBLog::DATA | SBLog::IO, ClassName() + 
			   ": Sta: [" + StaS->name() + "](" + StaS->aka() + ") duplicate session names: " + 
			   Session->name());
	    };


	  mainPrBarSetProgress(i++);
	  mainSetProcSes(i, n);
	  qApp->processEvents();
	  delete Session;
	};
    };
  mainSetOperation("Waiting");
  mainSetProcSes(0, 0);
  mainPrBarReset();
  mainMessage("Ready");
  mainSetSessName("<none>");
};
/*==============================================================================================*/








/*==============================================================================================*/
