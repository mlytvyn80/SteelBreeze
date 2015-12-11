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

#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <qapplication.h>
#include <qdatetime.h>
#include <qdir.h> 
#include <qfile.h> 
#include <qfileinfo.h> 
#include <qmessagebox.h>
#include <qregexp.h> 
#include <qstringlist.h>

#include "SbGeoEop.H"
#include "SbGeoRefFrame.H"
#include "SbGeoEstimator.H"
#include "SbGeoParameter.H"
#include "SbGeoProject.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"
#include "SbSetup.H"
#include "SbGeoSINEX.H"



/*==============================================================================================*/
/*												*/
/* class SBRunManager implementation (continue)							*/
/*												*/
/*==============================================================================================*/
void SBRunManager::makeReportMaps()
{
  static const QString GMT4CRFContent
    ("#!/bin/bash\n#\n# This file was created by " + Version.name() + "\n#\n#\n#\n#\n\n"
     "file=\"0_Map_CRF.ps\"\n\nRR=\"-180/180/-90/90\"\nStartLong=\"0\"\nScale=\"0.03i\"\nshift=\"-X2 -Y4\"\n"
     "color_wet=\"255/255/255\"\ncolor_dry=\"140/140/140\"\n\nPrj=\"i${StartLong}/${Scale}\"\n"
     "Prj=\"h${StartLong}/${Scale}\"\n#Prj=\"kf${StartLong}/${Scale}\"\n#Prj=\"ks${StartLong}/${Scale}\"\n"
     "#Prj=\"n${StartLong}/${Scale}\"\n#Prj=\"r${StartLong}/${Scale}\"\n#Prj=\"w${StartLong}/${Scale}\"\n"
     "\nBorder=\"a60f20g20/a20f10g10\"\n"
     "#ANOT_FONT AvantGarde-Book Bookman-Demi Helvetica-Narrow NewCenturySchlbk-Roman Palatino-Roman\n"
     "#ZapfChancery-MediumItalic\ngmtset DOTS_PR_INCH 600 \\\n"
     "ANOT_FONT ZapfChancery-MediumItalic \\\nBASEMAP_TYPE fancy \\\nPAGE_ORIENTATION landscape \\\n"
     "ANOT_FONT_SIZE 12\n\n\npsbasemap -R${RR} -J${Prj} -B${Border} -G${color_wet} ${shift} -K > $file\n"
     "psxy CRF.coo_def -R${RR} -J${Prj} -O -Sa0.45  -G0 -K >> $file\n"
     "psxy CRF.coo_oth -R${RR} -J${Prj} -O -Sc0.2   -G0    >> $file\n\n"
     );
  
  static const QString GMT4TRFContent
    ("#!/bin/bash\n#\n# This file was created by " + Version.name() + "\n#\n#\n#\n#\n\n"
     "file=\"0_Map_TRF.ps\"\nplateBoundaries=\"/usr/local/GMT/share/nuvel_1_plates\"\n\n"
     "RR=\"-180/180/-90/90\"\nRR2=\"-180/180/-80/80\"\nStartLong=\"0\"\n\n"
     "RR=\"-30/330/-90/90\"\nStartLong=\"150\"\n\n\nScale=\"0.03i\"\nshift=\"-X0 -Y4\"\n"
     "color_wet=\"255/255/255\"\ncolor_dry=\"200/200/200\"\n#color_dry=\"255/255/255\"\n"
     "\nPrj=\"h${StartLong}/${Scale}\"\n\n#ok\n#shift=\"-X1.2 -Y3\"\n#Scale=\"0.03i\"\n"
     "#Prj=\"j${StartLong}/${Scale}\"\n\n\nshift=\"-X1.2 -Y4.5\"\nScale=\"0.03i\"\n"
     "Prj=\"q${StartLong}/${Scale}\"\n\n\nBorder=\"a60f30g30/a30f30g30\"\n"
     "Border=\"a60f20g20/a20f20g10\"\n#ANOT_FONT AvantGarde-Book Bookman-Demi Helvetica-Narrow "
     "NewCenturySchlbk-Roman Palatino-Roman\n#ZapfChancery-MediumItalic\n"
     "gmtset DOTS_PR_INCH 600 ANOT_FONT ZapfChancery-MediumItalic BASEMAP_TYPE fancy ANOT_FONT_SIZE 10\n"
     "\n\n pscoast -R${RR} -J${Prj} -B${Border} -G${color_dry} -S${color_wet} -I1 -W -N1 -A2000 -Dc "
     "${shift} -K > $file\n#pscoast -R${RR} -J${Prj} -B${Border} -G${color_dry} -S${color_wet} -W "
     "-A1000 -Dc ${shift} -K > $file\n\n\nif [ -r $plateBoundaries ]\nthen\n"
     "    psxy $plateBoundaries -R${RR} -J${Prj} -O -M -W20/110/110/110 -K >> $file\n"
     "fi\n\n#psxy VEL   -R${RR2} -J${Prj} -O -SV0.02c/0.5c/0.1c   -G0 -K >> $file\n\n"
     "psxy TRF.vel_def    -R${RR} -J${Prj} -O -Sa0.6    -G0 -K >> $file\n"
     "psxy TRF.vel_oth    -R${RR} -J${Prj} -O -Sc0.15   -G0 -K >> $file\n\n"
     "psxy TRF.vel_def    -R${RR} -J${Prj} -O -SV0.02c/0.5c/0.07c   -G0 -K >> $file\n"
     "psxy TRF.vel_oth    -R${RR} -J${Prj} -O -SV0.02c/0.5c/0.07c   -G0 -K >> $file\n\n\n"
     "#psxy TRF.vel_plates -R${RR} -J${Prj} -O -SV0.02c/0.5c/0.07c   -G200/0/0 -K >> $file\n"
     "psxy TRF.coo_def    -R${RR} -J${Prj} -O -St0.4    -G0 -K >> $file\n"
     "psxy TRF.coo_oth    -R${RR} -J${Prj} -O -Sc0.15   -G0 -K >> $file\n\n\n\n\n\n"
     "psxy TRF.vel.p      -R${RR} -J${Prj} -O -Sc0.15             -G0 -X-12 -Y-8 -K >> $file\n"
     "psxy TRF.vel.p      -R${RR} -J${Prj} -O -SV0.02c/0.5c/0.07c -G0            -K >> $file\n"
     "psxy TRF.vel.p      -R${RR} -J${Prj} -O -Sl0.5/\"10cm/year\"  -G0 -X3 -Y-0.5    >> $file\n\n\n\n\n"
     );

  static const QString Gnuplot4CRFContent
    ("# This file was created by " + Version.name() + "\n#\n#\n#\n#\n\n"
     "set term postscript landscape\n#\n\nset output \"0_Map_CRF_difs.ps\"\n\n"
     "set xlabel \"Corrections to Right Ascension, mas\"\n"
     "set ylabel \"Corrections to Declination, mas\"\n\n"
     "plot '< awk \"{print \\$1, \\$3, \\$2, \\$4}\" CRF.dif' notitle with xyerrorbars\n\n\n"
     );
  

  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Creating scripts for GMT");


  QString	Dir2Output = SetUp->path2Output() + Prj->name() + "/Maps/";
  QString	Str;
  bool		IsInSolution;

  QFile		File4CRF_Def(Dir2Output + "CRF.coo_def");
  QFile		File4CRF_Oth(Dir2Output + "CRF.coo_oth");
  QFile		File4CRF_dif(Dir2Output + "CRF.dif");

  QTextStream	TS4CRF_Def;
  QTextStream	TS4CRF_Oth;
  QTextStream	TS4CRF_dif;


  QFile		File4TRF_CDef(Dir2Output + "TRF.coo_def");
  QFile		File4TRF_COth(Dir2Output + "TRF.coo_oth");
  QFile		File4TRF_VDef(Dir2Output + "TRF.vel_def");
  QFile		File4TRF_VOth(Dir2Output + "TRF.vel_oth");
  QFile		File4TRF_VPla(Dir2Output + "TRF.vel_plates");


  QTextStream	TS4TRF_CDef;
  QTextStream	TS4TRF_COth;
  QTextStream	TS4TRF_VDef;
  QTextStream	TS4TRF_VOth;
  QTextStream	TS4TRF_VPla;


  QDir		dir;
  if (!dir.exists(Dir2Output) && !dir.mkdir(Dir2Output))
    {
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": can't create dir [" + Dir2Output + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create output dir\n " + Dir2Output +
				    "\nfor the project [" + Prj->name() + "]"));
      return;
    };

  // makes output 4 CRF solution:
  File4CRF_Def.open(IO_WriteOnly|IO_Truncate);
  File4CRF_Oth.open(IO_WriteOnly|IO_Truncate);
  File4CRF_dif.open(IO_WriteOnly|IO_Truncate);
  TS4CRF_Def.setDevice(&File4CRF_Def);
  TS4CRF_Oth.setDevice(&File4CRF_Oth);
  TS4CRF_dif.setDevice(&File4CRF_dif);

  for (SBSource *Src = CRF->first(); Src; Src = CRF->next())
    {
      SBSourceInfo *SI;
      if (!(SI = Prj->sourceList()->find(Src)))
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find source [" + 
		   Src->name() + "] in the project");
      else
	{
	  if (!SI->isAttr(SBSourceInfo::notValid))
	    {
	      // coordinates:
	      IsInSolution = SI->isAttr(SBSourceInfo::EstCoo);

	      Src->getGlobalParameters4Report(this);
	      
	      Str.sprintf("%9.4f %9.4f 0.2 %8d  ", 
			  Src->ra_t()*RAD2DEG, Src->dn_t()*RAD2DEG, Src->p_DN()->num());
	      Str += Src->name();
	      if (!SI->isAttr(SBSourceInfo::EstCoo))
		Str += " Fixed ";
	      else if (SI->isAttr(SBSourceInfo::ConstrCoo))
		Str += " Constr";
	      else
		Str += "       ";

	      if (SI->isAttr(SBSourceInfo::ConstrCoo) || !SI->isAttr(SBSourceInfo::EstCoo))
		TS4CRF_Def << Str << "\n";
	      else
		TS4CRF_Oth << Str << "\n";
	      
	      Str.sprintf("%16.8f %16.8f %16.8f %16.8f   ", 
			  Src->p_RA()->v()*RAD2SEC*1000.0, Src->p_RA()->e()*RAD2SEC*1000.0, 
			  Src->p_DN()->v()*RAD2SEC*1000.0, Src->p_DN()->e()*RAD2SEC*1000.0);
	      Str += Src->name();
	      TS4CRF_dif << Str << "\n";
	    };
	};
    };

  TS4CRF_Def.unsetDevice();
  TS4CRF_Oth.unsetDevice();
  TS4CRF_dif.unsetDevice();
  File4CRF_Def.close();
  File4CRF_Oth.close();
  File4CRF_dif.close();



  // makes output 4 TRF solution:
  File4TRF_CDef.open(IO_WriteOnly|IO_Truncate);
  File4TRF_COth.open(IO_WriteOnly|IO_Truncate);
  File4TRF_VDef.open(IO_WriteOnly|IO_Truncate);
  File4TRF_VOth.open(IO_WriteOnly|IO_Truncate);
  File4TRF_VPla.open(IO_WriteOnly|IO_Truncate);

  TS4TRF_CDef.setDevice(&File4TRF_CDef);
  TS4TRF_COth.setDevice(&File4TRF_COth);
  TS4TRF_VDef.setDevice(&File4TRF_VDef);
  TS4TRF_VOth.setDevice(&File4TRF_VOth);
  TS4TRF_VPla.setDevice(&File4TRF_VPla);

  QString	ss, cc;
  double	ScaleVel = 40.0;
  for (SBStation *Sta = TRF->stations()->first(); Sta; Sta = TRF->stations()->next())
    {
      SBStationInfo	*SI;
      SBStationID	id;
      id.setID(Sta->id());
      if (!(SI = Prj->stationList()->find(&id)))
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find station [" + 
		   Sta->name() + "] in the project");
      else
	{
	  if (!SI->isAttr(SBStationInfo::notValid))
	    {
	      Sta->getGlobalParameters4Report(this);

	      Str.sprintf("%9.4f %9.4f ", Sta->longitude()*RAD2DEG, Sta->latitude()*RAD2DEG);
	      
	      if (SI->isAttr(SBStationInfo::EstVel))
		{
		  Vector3	VelPlates = RefPlates->velocity(*Sta);
		  Vector3	Vel = Sta->vt();
		  
		  Vel		= ~Sta->fmVEN()*Vel;
		  VelPlates	= ~Sta->fmVEN()*VelPlates;
		  
		  ss.sprintf("%16.4f %16.4f %16.4f ", atan2(Vel.at(EAST), Vel.at(NORT))*RAD2DEG, 
			     ScaleVel*Vel.module(), ScaleVel*Vel.at(VERT));
		  ss += "  [" + Sta->id().toString() + "]  " + SI->aka();
		  
		  cc.sprintf("%16.4f %16.4f %16.4f ", atan2(VelPlates.at(EAST), VelPlates.at(NORT))*RAD2DEG, 
			     ScaleVel*VelPlates.module(),  ScaleVel*VelPlates.at(VERT));
		  cc += "  [" + Sta->id().toString() + "]  " + SI->aka();

		  TS4TRF_VPla << Str + cc << "\n";

		  if (SI->isAttr(SBStationInfo::ConstrVel))
		    TS4TRF_VDef << Str + ss << "\n";
		  else 
		    TS4TRF_VOth << Str + ss << "\n";
		};
	      if (SI->isAttr(SBStationInfo::EstCoo))
		{
		  Str += " 0.2  [" + Sta->id().toString() + "]  " + SI->aka();

		  if (SI->isAttr(SBStationInfo::ConstrCoo) && !SI->isAttr(SBStationInfo::ConstrVel))
		    TS4TRF_CDef << Str << "\n";
		  else if (!SI->isAttr(SBStationInfo::EstVel))
		    TS4TRF_COth << Str << "\n";
		};
		
	    };
	};
    };


  TS4TRF_CDef.unsetDevice();
  TS4TRF_COth.unsetDevice();
  TS4TRF_VDef.unsetDevice();
  TS4TRF_VOth.unsetDevice();
  TS4TRF_VPla.unsetDevice();

  File4TRF_CDef.close();
  File4TRF_COth.close();
  File4TRF_VDef.close();
  File4TRF_VOth.close();  
  File4TRF_VPla.close();


  // auxiliary files:
  QFile		FileAux;
  QTextStream	TSAux;
  QFileInfo	fi;
  QString	FileAuxName;

  FileAuxName = Dir2Output + "TRF.vel.p";
  FileAux.setName(FileAuxName);
  FileAux.open(IO_WriteOnly|IO_Truncate);
  TSAux.setDevice(&FileAux);
  TSAux << "150.00000     0.00000    90.00000     " << QString::number(ScaleVel/10.0) 
	<< "   " << QString::number(ScaleVel/10.0) << " TST\n";
  TSAux.unsetDevice();
  FileAux.close();
  
  
  FileAuxName = Dir2Output + "2map_CRF";
  fi.setFile(FileAuxName);
  if (!fi.exists()) // create it
    {
      FileAux.setName(FileAuxName);
      FileAux.open(IO_WriteOnly);
      TSAux.setDevice(&FileAux);
      TSAux << GMT4CRFContent;
      TSAux.unsetDevice();
      FileAux.close();
      chmod((const char*)FileAuxName, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    };

  FileAuxName = Dir2Output + "2map_TRF";
  fi.setFile(FileAuxName);
  if (!fi.exists()) // create it
    {
      FileAux.setName(FileAuxName);
      FileAux.open(IO_WriteOnly);
      TSAux.setDevice(&FileAux);
      TSAux << GMT4TRFContent;
      TSAux.unsetDevice();
      FileAux.close();
      chmod((const char*)FileAuxName, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    };

  
  FileAuxName = Dir2Output + "2map_CRF_diffs";
  fi.setFile(FileAuxName);
  if (!fi.exists()) // create it
    {
      FileAux.setName(FileAuxName);
      FileAux.open(IO_WriteOnly);
      TSAux.setDevice(&FileAux);
      TSAux << Gnuplot4CRFContent;
      TSAux.unsetDevice();
      FileAux.close();
    };

  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": preview maps/pictures for the project `" + 
	     Prj->name() + "' were created in the directory " + Dir2Output);
};

void SBRunManager::makeReportCRFVariations()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing source coordinates variations");

  QString	Dir2Output = SetUp->path2Output() + Prj->name() + "/SrcCoordVars/";
  QString	Dir2Dat = SetUp->path2Output() + Prj->name() + "/SrcCoordVars/dat/";

  //
  QDir		dir;
  if (!dir.exists(Dir2Output) && !dir.mkdir(Dir2Output))
    {
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": can't create dir [" + Dir2Output + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create output dir\n " + Dir2Output +
				    "\nfor the project [" + Prj->name() + "]"));
      return;
    };

  if (!dir.exists(Dir2Dat) && !dir.mkdir(Dir2Dat))
    {
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": can't create dir [" + Dir2Dat + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create output dir\n " + Dir2Dat +
				    "\nfor the project [" + Prj->name() + "]"));
      return;
    };
  //

  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": starting the output of variations of the source coordinations for the project `" + 
	     Prj->name() + "'");

  SBSourceInfo		*SI=NULL;

  QFile			*f=NULL;
  QTextStream		*s=NULL;
  QDict<QFile>		FileNames;
  QDict<QTextStream>	TextStreams;
  QDict<int>		NumbersByKey;
  SBNList		IDList;

  FileNames.setAutoDelete(TRUE);
  TextStreams.setAutoDelete(TRUE);
  NumbersByKey.setAutoDelete(TRUE);
  IDList.setAutoDelete(TRUE);

  QString		key;
  QString		Str;
  
  // prepare files and streams for output:
  for (SBSource *Src = CRF->first(); Src; Src = CRF->next())
    {
      if (!(SI=Prj->sourceList()->find(Src)))
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find the source [" + 
		   Src->name() + "] in the project");
      else if (!SI->isAttr(SBSourceInfo::notValid) && SI->isAttr(SBSourceInfo::EstCoo))
	{
	  key = Src->name();
	  if (!FileNames.find(key))
	    {
	      IDList.append(new SBNamed(key));
	      FileNames.insert(key, new QFile(Dir2Dat + key + ".dat"));
	      TextStreams.insert(key, new QTextStream);
	      NumbersByKey.insert(key, new int(0));
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": got a duplicate source [" + 
		       Src->name() + "] in the dictionary of filenames");
	};
    };

  IDList.sort();

  for (SBNamed* n=IDList.first(); n; n=IDList.next())
    {
      key = n->name();
      if ( (f=FileNames.find(key)) && (s=TextStreams.find(key)))
	{
	  f->open(IO_WriteOnly);
	  s->setDevice(f);
	}
      else
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() 
		   + ": cannot find qfile or textstream correspondent to the source [" + 
		   key + "] in the dictionaries");
    };
  

  // get coordinates and write them:
  SBMJD			TMax=TZero, TMin=TInf;
  SBSource		*Src=NULL;
  SBParameter		*P  =NULL;
  SBStochParameter	*Ps =NULL;
  
  double		RA_0 = 0.0;
  double		DN_0 = 0.0;
  double		RA_v = 0.0;
  double		DN_v = 0.0;
  double		RA_e = 0.0;
  double		DN_e = 0.0;

  SBMJD			Epoch(TZero);
  bool			HasGlbPars=FALSE;
  bool			HasLocPars=FALSE;
  bool			HasSthPars=FALSE;
  bool			WasChecked=FALSE;
  int			*idx = NULL;
  
  
  //  for (SBStatistics *Stat=Solution->statByBatch().first(); Stat; Stat=Solution->statByBatch().next())
  for (SBStatistics *Stat=Solution->statByBatch().last(); Stat; Stat=Solution->statByBatch().prev())
    {
      Solution->prepare4Batch(Stat->name());

      if (!WasChecked)
	{
	  for (P=Solution->global().first(); P && !HasGlbPars; P=Solution->global().next())
	    if (P->name().contains("RA") && P->name().contains("So:"))
	      HasGlbPars = TRUE;
	  
	  for (P=Solution->local().first(); P && !HasLocPars; P=Solution->local().next())
	    if (P->name().contains("RA") && P->name().contains("So:"))
	      HasLocPars = TRUE;
	  
	  for (Ps=Solution->stochastic().first(); Ps && !HasSthPars; Ps=Solution->stochastic().next())
	    if (Ps->name().contains("RA") && Ps->name().contains("So:"))
	      HasSthPars = TRUE;
	  
	  WasChecked = TRUE;
	};

      HasGlbPars = TRUE;
      HasLocPars = TRUE;
      HasSthPars = FALSE;

      if (HasGlbPars || HasLocPars || HasSthPars)
	for (SBNamed* n=IDList.first(); n; n=IDList.next())
	  {
	    key = n->name();
	    Src = CRF->find(key);
	    SI  = Prj->sourceList()->find(Src);
	    if (Src && SI && (f=FileNames.find(key)) && (s=TextStreams.find(key)))
	      {
		RA_0 = Src->ra();
		DN_0 = Src->dn();
		RA_v = 0.0;
		DN_v = 0.0;
		RA_e = 0.0;
		DN_e = 0.0;

		if (HasSthPars)
		  {
		    if (HasGlbPars)
		      {
			if ( (P=Solution->global().find(Src->p_RA()->name())) )
			  {
			    RA_v = P->v()*P->scale();
			    RA_e = P->e()*P->scale();
			  }
			else 
			  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
				     + ": cannot find global parameter '" + Src->p_RA()->name() +
				     "' in the list of global parameters");
			if ( (P=Solution->global().find(Src->p_DN()->name())) )
			  {
			    DN_v = P->v()*P->scale();
			    DN_e = P->e()*P->scale();
			  }
			else 
			  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
				     + ": cannot find global parameter '" + Src->p_DN()->name() +
				     "' in the list of global parameters");
		      };
		    if (HasLocPars)
		      {
			if ( (P=Solution->local().find(Src->p_RA()->name())) )
			  {
			    RA_v += P->v()*P->scale();
			    RA_e  = P->e()*P->scale();
			  };
			if ( (P=Solution->local().find(Src->p_DN()->name())) )
			  {
			    DN_v += P->v()*P->scale();
			    DN_e  = P->e()*P->scale();
			  };
		      };

		    SBStochParameter	*PsRA =Solution->stochastic().find(Src->p_RA()->name());
		    SBStochParameter	*PsDN =Solution->stochastic().find(Src->p_DN()->name());
		    if (PsRA && PsDN)
		      {
			if (PsRA->first()->tStart()<TMin)
			  TMin = PsRA->first()->tStart();
			if (PsRA->last()->tFinis()>TMax)
			  TMax = PsRA->last()->tFinis();
			
			for (unsigned int i=0; i<PsRA->count(); i++)
			  if (PsDN->at(i)->num())
			    {
			      Epoch = PsRA->at(i)->tEpoch();
			      RA_v += PsRA->at(i)->v()*PsRA->scale();
			      DN_v += PsDN->at(i)->v()*PsDN->scale();
			      RA_e  = PsRA->at(i)->e()*PsRA->scale();
			      DN_e  = PsDN->at(i)->e()*PsDN->scale();
			      
			      Str.sprintf("%12.6f %12.6f  %12.6f %12.6f  %12.6f %12.6f  %5d  ",
					  RA_v, RA_e, RA_v*cos(DN_0), RA_e*cos(DN_0), 
					  DN_v, DN_e,
					  PsDN->at(i)->num() );
			      
			      *s << Epoch.toString(SBMJD::F_MJD)  << "  " 
				 << Epoch.toString(SBMJD::F_UNIX) << "  " 
				 << Str << Src->name() << "\n";
			      if ( (idx=NumbersByKey.find(key)) )
				(*idx)++;
			    };
		      };
		  }
		else if (HasLocPars)
		  {
		    if (HasGlbPars)
		      {
			if ( (P=Solution->global().find(Src->p_RA()->name())) )
			  {
			    RA_v = P->v()*P->scale();
			    RA_e = P->e()*P->scale();
			  }
			else 
			  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
				     + ": cannot find global parameter '" + Src->p_RA()->name() +
				     "' in the list of global parameters");
			if ( (P=Solution->global().find(Src->p_DN()->name())) )
			  {
			    DN_v = P->v()*P->scale();
			    DN_e = P->e()*P->scale();
			  }
			else 
			  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
				     + ": cannot find global parameter '" + Src->p_DN()->name() +
				     "' in the list of global parameters");
		      };
		    if ( (P=Solution->local().find(Src->p_RA()->name())) )
		      {
			RA_v += P->v()*P->scale();
			RA_e  = P->e()*P->scale();
		      };
		    if ( (P=Solution->local().find(Src->p_DN()->name())) )
		      {
			DN_v += P->v()*P->scale();
			DN_e  = P->e()*P->scale();
		      };
		    if (P && P->num())
		      {
			Epoch = P->tEpoch();
			if (Epoch<TMin)
			  TMin = Epoch;
			if (Epoch>TMax)
			  TMax = Epoch;
			
			Str.sprintf("%12.6f %12.6f  %12.6f %12.6f  %12.6f %12.6f  %5d  ",
				    RA_v, RA_e, RA_v*cos(DN_0), RA_e*cos(DN_0), 
				    DN_v, DN_e,
				    P->num() );
			
			*s << Epoch.toString(SBMJD::F_MJD)  << "  " 
			   << Epoch.toString(SBMJD::F_UNIX) << "  " 
			   << Str << Src->name() << "\n";
			if ( (idx=NumbersByKey.find(key)) )
			  (*idx)++;
		      };
		  }
		else if (HasGlbPars)
		  {
		    if ( (P=Solution->global().find(Src->p_RA()->name())) )
		      {
			RA_v = P->v()*P->scale();
			RA_e = P->e()*P->scale();
		      };
		    if ( (P=Solution->global().find(Src->p_DN()->name())) )
		      {
			DN_v = P->v()*P->scale();
			DN_e = P->e()*P->scale();
		      };
		    if (P && P->num())
		      {
			Epoch = P->tEpoch();
			if (Epoch<TMin)
			  TMin = Epoch;
			if (Epoch>TMax)
			  TMax = Epoch;

			Str.sprintf("%12.6f %12.6f  %12.6f %12.6f  %12.6f %12.6f  %5d ",
				    RA_v, RA_e, RA_v*cos(DN_0), RA_e*cos(DN_0), 
				    DN_v, DN_e,
				    P->num() );
			
			*s << Epoch.toString(SBMJD::F_MJD)  << "  " 
			   << Epoch.toString(SBMJD::F_UNIX) << "  " 
			   << Str << Src->name() << "\n";
			if ( (idx=NumbersByKey.find(key)) )
			  (*idx)++;
		      };
		  };
	      }
	    else
	      Log->write(SBLog::ERR, SBLog::RUN, ClassName() 
			 + ": cannot find qfile or textstream correspondent to the source [" + 
			 key + "] in the dictionaries");
	  };
    };
  

  // at last, close files and flushes streams:
  for (SBNamed* n=IDList.first(); n; n=IDList.next())
    {
      key = n->name();
      if ( (f=FileNames.find(key)) && (s=TextStreams.find(key)))
	{
	  s->unsetDevice();
	  f->close();
	}
      else
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() 
		   + ": cannot find qfile or textstream correspondent to the source [" + 
		   key + "] in the dictionaries");
    };

  // create the script for gnuplot:
  double dt = TMax-TMin;
  if (dt>0.0)
    {
      TMax = TMax + 0.02*dt;
      TMin = TMin - 0.02*dt;
    };
  QString	XRanges = (TMin<TMax)?
    "set xrange [\"" + TMin.toString(SBMJD::F_UNIX) + "\":\"" + TMax.toString(SBMJD::F_UNIX) + "\"]\n":
    "set xrange [*:*]\n";

  f = new QFile(Dir2Output + "src_coord_vars.plt");
  QFileInfo fi(f->name());
  if (/*!fi.exists() &&*/ f->open(IO_WriteOnly))
    {
      s = new QTextStream;
      s->setDevice(f);

      *s << "# This file was automatically created by " << Version.name() << "\n\n";
      *s << "set term postscript landscape enhanced color solid \"Times-Roman\"\n";
      *s << "#set term postscript eps enhanced color solid \"Times-Roman\"\n\n";
      *s << "#set data style points\nset data style errorbars\nset pointsize 1.0\n";
      *s << "set zeroaxis\nset bar small\n\n";
      *s << "set xdata time\nset timefmt \"%s\"\nset format x \"%b %d\\n%Y\"\n#set format y \"%.1f\"\n\n";
      *s << "set output \"src_coord_vars.ps\"\n\n";
      *s << XRanges;
      *s << "set yrange [*:*]\n";

      for (SBNamed* n=IDList.first(); n; n=IDList.next())
	{
	  Str = key = n->name();
	  idx = NumbersByKey.find(key);
	  if (idx && *idx>0)
	    {
	      if ( (Src=CRF->find(key)) && (SI=Prj->sourceList()->find(Src)) )
		if (SI->name() != SI->aka())
		  Str += " (" + SI->aka().stripWhiteSpace() + ")";
	      
	      *s << "\n#\n# Plots for the source " << Str << "\n";
	      *s << "#\nset xlabel \"Time, UTC\"\n";
	      *s << "set xdata time\nset timefmt \"%s\"\nset format x \"%b %d\\n%Y\"\n";
	      *s << XRanges;
	      
	      *s << "\nset ylabel \"Position Variations of " << Str << ", Right Ascension, mas\"\n";
	      *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:3:4 notitle lt -1 lw 0.2 pt 7\n";
	      
	      *s << "\nset ylabel \"Position Variations of " << Str << ", Declination, mas\"\n";
	      *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:7:8 notitle lt -1 lw 0.2 pt 7\n";
	      
	      *s << "\nset xrange [*:*]\nset xdata\nset format x\n";
	      *s << "set xlabel \"Position Variations of " << Str << ", RA*cos(Dec), mas\"\n";
	      *s << "set ylabel \"Position Variations of " << Str << ", Dec, mas\"\n";
	      *s << "plot \\\n\t\"dat/" << key << ".dat\" using 5:7:6:8 notitle with xyerrorbars 7\n\n\n";
	    };
	};
      
      *s << "#EOF\n";
      s->unsetDevice();
      delete s;
      f->close();
    };
  delete f;
  
  IDList.clear();
  FileNames.clear();
  TextStreams.clear();
  
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": variations of sources coordinates for the project `" + Prj->name() +
	     "' were created in the directory " + Dir2Output);
};

void SBRunManager::makeReportTRFVariations()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing station coordinates variations");

  QString	Dir2Output = SetUp->path2Output() + Prj->name() + "/CoordVars/";
  QString	Dir2Dat = SetUp->path2Output() + Prj->name() + "/CoordVars/dat/";

  //
  QDir		dir;
  if (!dir.exists(Dir2Output) && !dir.mkdir(Dir2Output))
    {
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": can't create dir [" + Dir2Output + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create output dir\n " + Dir2Output +
				    "\nfor the project [" + Prj->name() + "]"));
      return;
    };

  if (!dir.exists(Dir2Dat) && !dir.mkdir(Dir2Dat))
    {
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": can't create dir [" + Dir2Dat + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create output dir\n " + Dir2Dat +
				    "\nfor the project [" + Prj->name() + "]"));
      return;
    };
  //


  SBStationInfo		*SI=NULL;
  SBStationID		id;

  QFile			*f=NULL;
  QTextStream		*s=NULL;
  QDict<QFile>		FileNames;
  QDict<QTextStream>	TextStreams;
  SBNList		IDList;

  FileNames.setAutoDelete(TRUE);
  TextStreams.setAutoDelete(TRUE);
  IDList.setAutoDelete(TRUE);

  QString		key;
  QString		Str;
  
  // prepare files and streams for output:
  for (SBStation *Sta = TRF->stations()->first(); Sta; Sta = TRF->stations()->next())
    {
      id.setID(Sta->id());
      if (!(SI = Prj->stationList()->find(&id)))
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find station " + 
		   Sta->name() + " [" + id.toString() +"] in the project");
      else if (!SI->isAttr(SBStationInfo::notValid))
	{
	  key = id.toString();
	  if (!FileNames.find(key))
	    {
	      IDList.append(new SBNamed(key));
	      FileNames.insert(key, new QFile(Dir2Dat + key + ".dat"));
	      TextStreams.insert(key, new QTextStream);
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": got a duplicate station " + 
		       Sta->name() + " [" + id.toString() +"] in the dictionary of filenames");
	};
    };

  IDList.sort();

  for (SBNamed* n=IDList.first(); n; n=IDList.next())
    {
      key = n->name();
      if ( (f=FileNames.find(key)) && (s=TextStreams.find(key)))
	{
	  f->open(IO_WriteOnly);
	  s->setDevice(f);
	}
      else
	    Log->write(SBLog::ERR, SBLog::RUN, ClassName() 
		       + ": cannot find qfile or textstream correspondent to the stationID [" + 
		       key +"] in the dictionaries");
    };
  

  // get coordinates and write them:
  SBMJD			TMax=TZero, TMin=TInf;
  SBStation		*Sta=NULL;
  SBParameter		*P  =NULL;
  SBStochParameter	*Ps =NULL;
  Vector3		R0(v3Zero);
  Vector3		R(v3Zero), R_(v3Zero);
  Vector3		dR(v3Zero), dR_(v3Zero);
  Vector3		E(v3Zero), E_(v3Zero);
  Matrix3		toVEN;
  SBMJD			Epoch(TZero);
  bool			HasGlbPars=FALSE;
  bool			HasLocPars=FALSE;
  bool			HasSthPars=FALSE;
  bool			WasChecked=FALSE;

  for (SBStatistics *Stat=Solution->statByBatch().first(); Stat; Stat=Solution->statByBatch().next())
    {
      Solution->prepare4Batch(Stat->name());
      
      if (!WasChecked)
	{
	  for (P=Solution->global().first(); P && !HasGlbPars; P=Solution->global().next())
	    if (P->name().contains("coord-") && P->name().contains("St:"))
	      HasGlbPars = TRUE;

	  for (P=Solution->local().first(); P && !HasLocPars; P=Solution->local().next())
	    if (P->name().contains("coord-") && P->name().contains("St:"))
	      HasLocPars = TRUE;

	  for (Ps=Solution->stochastic().first(); Ps && !HasSthPars; Ps=Solution->stochastic().next())
	    if (Ps->name().contains("coord-") && Ps->name().contains("St:"))
	      HasSthPars = TRUE;

	  WasChecked = TRUE;
	};
      
      if (HasGlbPars || HasLocPars || HasSthPars)
	for (SBNamed* n=IDList.first(); n; n=IDList.next())
	  {
	    key = n->name();
	    id.setID(key);
	    SI = Prj->stationList()->find(&id);
	    Sta= TRF->find(id);
	    if ( Sta && SI && (f=FileNames.find(key)) && (s=TextStreams.find(key)))
	      {
		R = R0 = Sta->r_first();
		toVEN = ~Sta->fmVEN();
		if (HasSthPars)
		  {
		    if (HasGlbPars)
		      {
			if ( (P=Solution->global().find(Sta->p_DRX()->name())) )
			  {
			    R[X_AXIS] += P->v();
			    E[X_AXIS]  = P->e();
			  }
			else 
			  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
				     + ": cannot find global parameter '" + Sta->p_DRX()->name() +
				     "' in the list of global parameters");
			if ( (P=Solution->global().find(Sta->p_DRY()->name())) )
			  {
			    R[Y_AXIS] += P->v();
			    E[Y_AXIS]  = P->e();
			  }
			else 
			  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
				     + ": cannot find global parameter '" + Sta->p_DRY()->name() +
				     "' in the list of global parameters");
			if ( (P=Solution->global().find(Sta->p_DRZ()->name())) )
			  {
			    R[Z_AXIS] += P->v();
			    E[Z_AXIS]  = P->e();
			  }
			else 
			  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
				     + ": cannot find global parameter '" + Sta->p_DRZ()->name() +
				     "' in the list of global parameters");
		      };
		    if (HasLocPars)
		      {
			if ( (P=Solution->local().find(Sta->p_DRX()->name())) )
			  {
			    R[X_AXIS] += P->v();
			    E[X_AXIS]  = P->e();
			  }
			//		else 
			//		  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
			//		     + ": cannot find local parameter '" + Sta->p_DRX()->name() +
			//		     "' in the list of local parameters");
			if ( (P=Solution->local().find(Sta->p_DRY()->name())) )
			  {
			    R[Y_AXIS] += P->v();
			    E[Y_AXIS]  = P->e();
			  }
			//		else 
			//		  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
			//		     + ": cannot find local parameter '" + Sta->p_DRY()->name() +
			//		     "' in the list of local parameters");
			if ( (P=Solution->local().find(Sta->p_DRZ()->name())) )
			  {
			    R[Z_AXIS] += P->v();
			    E[Z_AXIS]  = P->e();
			  }
			//		else 
			//		  Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
			//		     + ": cannot find local parameter '" + Sta->p_DRZ()->name() +
			//		     "' in the list of local parameters");
		      };
		    SBStochParameter	*PsX =Solution->stochastic().find(Sta->p_DRX()->name());
		    SBStochParameter	*PsY =Solution->stochastic().find(Sta->p_DRY()->name());
		    SBStochParameter	*PsZ =Solution->stochastic().find(Sta->p_DRZ()->name());
		    if (PsX && PsY && PsZ)
		      {
			if (PsX->first()->tStart()<TMin)
			  TMin = PsX->first()->tStart();
			if (PsX->last()->tFinis()>TMax)
			  TMax = PsX->last()->tFinis();
			
			for (unsigned int i=0; i<PsX->count(); i++)
			  {
			    Epoch = PsX->at(i)->tEpoch();
			    R_[X_AXIS] = R[X_AXIS] + PsX->at(i)->v();
			    R_[Y_AXIS] = R[Y_AXIS] + PsY->at(i)->v();
			    R_[Z_AXIS] = R[Z_AXIS] + PsZ->at(i)->v();

			    E[X_AXIS]= PsX->at(i)->e();
			    E[Y_AXIS]= PsY->at(i)->e();
			    E[Z_AXIS]= PsZ->at(i)->e();
			    
			    E_[VERT] = 
			      sqrt(
				   toVEN.at(VERT,X_AXIS)*E.at(X_AXIS)*toVEN.at(VERT,X_AXIS)*E.at(X_AXIS) +
				   toVEN.at(VERT,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(VERT,Y_AXIS)*E.at(Y_AXIS) +
				   toVEN.at(VERT,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(VERT,Z_AXIS)*E.at(Z_AXIS)
				   );
			    E_[EAST] = 
			      sqrt(
				   toVEN.at(EAST,X_AXIS)*E.at(X_AXIS)*toVEN.at(EAST,X_AXIS)*E.at(X_AXIS) +
				   toVEN.at(EAST,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(EAST,Y_AXIS)*E.at(Y_AXIS) +
				   toVEN.at(EAST,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(EAST,Z_AXIS)*E.at(Z_AXIS)
				   );
			    E_[NORT] = 
			      sqrt(
				   toVEN.at(NORT,X_AXIS)*E.at(X_AXIS)*toVEN.at(NORT,X_AXIS)*E.at(X_AXIS) +
				   toVEN.at(NORT,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(NORT,Y_AXIS)*E.at(Y_AXIS) +
				   toVEN.at(NORT,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(NORT,Z_AXIS)*E.at(Z_AXIS)
				   );
			    
			    dR_ = dR = R_ - R0;
			    dR_ = toVEN*dR_;

			    dR *=100.0;
			    dR_*=100.0;
			    E  *=100.0;
			    E_ *=100.0;
			    Str.sprintf("%8.3f %7.3f  %8.3f %7.3f  %8.3f %7.3f    "
					"%8.3f %7.3f  %8.3f %7.3f  %8.3f %7.3f    ", 
					dR .at(X_AXIS), E .at(X_AXIS), 
					dR .at(Y_AXIS), E .at(Y_AXIS), 
					dR .at(Z_AXIS), E .at(Z_AXIS), 
					dR_.at(VERT),   E_.at(VERT), 
					dR_.at(EAST),   E_.at(EAST), 
					dR_.at(NORT),   E_.at(NORT) );
			    
			    *s << Epoch.toString(SBMJD::F_MJD)  << "  " 
			       << Epoch.toString(SBMJD::F_UNIX) << "  " 
			       << Str << Stat->name() << "\n";
			  };
		      };
		  }
		else if (HasLocPars)
		  {
		    if ( (P=Solution->local().find(Sta->p_DRX()->name())) )
		      {
			R[X_AXIS] += P->v();
			E[X_AXIS]  = P->e();
		      }
		    //	    else 
		    //	      Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
		    //		 + ": cannot find local parameter '" + Sta->p_DRX()->name() +
		    //		 "' in the list of local parameters");
		    if ( (P=Solution->local().find(Sta->p_DRY()->name())) )
		      {
			R[Y_AXIS] += P->v();
			E[Y_AXIS]  = P->e();
		      }
		    //	    else 
		    //	      Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
		    //		 + ": cannot find local parameter '" + Sta->p_DRY()->name() +
		    //		 "' in the list of local parameters");
		    if ( (P=Solution->local().find(Sta->p_DRZ()->name())) )
		      {
			R[Z_AXIS] += P->v();
			E[Z_AXIS]  = P->e();
		      }
		    //	    else 
		    //	      Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
		    //		 + ": cannot find local parameter '" + Sta->p_DRZ()->name() +
		    //		 "' in the list of local parameters");
		    
		    if (P)
		      {
			Epoch = P->tEpoch();
			if (Epoch<TMin)
			  TMin = Epoch;
			if (Epoch>TMax)
			  TMax = Epoch;

			E_[VERT] = 
			  sqrt(
			       toVEN.at(VERT,X_AXIS)*E.at(X_AXIS)*toVEN.at(VERT,X_AXIS)*E.at(X_AXIS) +
			       toVEN.at(VERT,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(VERT,Y_AXIS)*E.at(Y_AXIS) +
			       toVEN.at(VERT,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(VERT,Z_AXIS)*E.at(Z_AXIS)
			       );
			E_[EAST] = 
			  sqrt(
			       toVEN.at(EAST,X_AXIS)*E.at(X_AXIS)*toVEN.at(EAST,X_AXIS)*E.at(X_AXIS) +
			       toVEN.at(EAST,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(EAST,Y_AXIS)*E.at(Y_AXIS) +
			       toVEN.at(EAST,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(EAST,Z_AXIS)*E.at(Z_AXIS)
			       );
			E_[NORT] = 
			  sqrt(
			       toVEN.at(NORT,X_AXIS)*E.at(X_AXIS)*toVEN.at(NORT,X_AXIS)*E.at(X_AXIS) +
			       toVEN.at(NORT,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(NORT,Y_AXIS)*E.at(Y_AXIS) +
			       toVEN.at(NORT,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(NORT,Z_AXIS)*E.at(Z_AXIS)
			       );
			
			dR_ = dR = R - R0;
			dR_ = toVEN*dR_;

			dR *=100.0;
			dR_*=100.0;
			E  *=100.0;
			E_ *=100.0;
			Str.sprintf("%8.3f %7.3f  %8.3f %7.3f  %8.3f %7.3f    "
				    "%8.3f %7.3f  %8.3f %7.3f  %8.3f %7.3f    ", 
				    dR .at(X_AXIS), E .at(X_AXIS), 
				    dR .at(Y_AXIS), E .at(Y_AXIS), 
				    dR .at(Z_AXIS), E .at(Z_AXIS), 
				    dR_.at(VERT),   E_.at(VERT), 
				    dR_.at(EAST),   E_.at(EAST), 
				    dR_.at(NORT),   E_.at(NORT) );
			
			*s << Epoch.toString(SBMJD::F_MJD)  << "  " 
			   << Epoch.toString(SBMJD::F_UNIX) << "  " 
			   << Str << Stat->name() << "\n";
		      };
		  }
		else if (HasGlbPars)
		  {
		    if ( (P=Solution->global().find(Sta->p_DRX()->name())) )
		      {
			R[X_AXIS] += P->v();
			E[X_AXIS]  = P->e();
		      }
		    //	    else 
		    //	      Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
		    //		 + ": cannot find global parameter '" + Sta->p_DRX()->name() +
		    //		 "' in the list of global parameters");
		    if ( (P=Solution->global().find(Sta->p_DRY()->name())) )
		      {
			R[Y_AXIS] += P->v();
			E[Y_AXIS]  = P->e();
		      }
		    //	    else 
		    //	      Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
		    //		 + ": cannot find global parameter '" + Sta->p_DRY()->name() +
		    //		 "' in the list of global parameters");
		    if ( (P=Solution->global().find(Sta->p_DRZ()->name())) )
		      {
			R[Z_AXIS] += P->v();
			E[Z_AXIS]  = P->e();
		      }
		    //	    else 
		    //	      Log->write(SBLog::WRN, SBLog::RUN, ClassName() 
		    //		 + ": cannot find global parameter '" + Sta->p_DRZ()->name() +
		    //		 "' in the list of global parameters");

		    if (P)
		      {
			Epoch = P->tEpoch();
			if (Epoch<TMin)
			  TMin = Epoch;
			if (Epoch>TMax)
			  TMax = Epoch;

			E_[VERT] = 
			  sqrt(
			       toVEN.at(VERT,X_AXIS)*E.at(X_AXIS)*toVEN.at(VERT,X_AXIS)*E.at(X_AXIS) +
			       toVEN.at(VERT,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(VERT,Y_AXIS)*E.at(Y_AXIS) +
			       toVEN.at(VERT,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(VERT,Z_AXIS)*E.at(Z_AXIS)
			       );
			E_[EAST] = 
			  sqrt(
			       toVEN.at(EAST,X_AXIS)*E.at(X_AXIS)*toVEN.at(EAST,X_AXIS)*E.at(X_AXIS) +
			       toVEN.at(EAST,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(EAST,Y_AXIS)*E.at(Y_AXIS) +
			       toVEN.at(EAST,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(EAST,Z_AXIS)*E.at(Z_AXIS)
			       );
			E_[NORT] = 
			  sqrt(
			       toVEN.at(NORT,X_AXIS)*E.at(X_AXIS)*toVEN.at(NORT,X_AXIS)*E.at(X_AXIS) +
			       toVEN.at(NORT,Y_AXIS)*E.at(Y_AXIS)*toVEN.at(NORT,Y_AXIS)*E.at(Y_AXIS) +
			       toVEN.at(NORT,Z_AXIS)*E.at(Z_AXIS)*toVEN.at(NORT,Z_AXIS)*E.at(Z_AXIS)
			       );
			
			dR_ = dR = R - R0;
			dR_ = toVEN*dR_;
			
			dR *=100.0;
			dR_*=100.0;
			E  *=100.0;
			E_ *=100.0;
			Str.sprintf("%8.3f %7.3f  %8.3f %7.3f  %8.3f %7.3f    "
				    "%8.3f %7.3f  %8.3f %7.3f  %8.3f %7.3f    ", 
				    dR .at(X_AXIS), E .at(X_AXIS), 
				    dR .at(Y_AXIS), E .at(Y_AXIS), 
				    dR .at(Z_AXIS), E .at(Z_AXIS), 
				    dR_.at(VERT),   E_.at(VERT), 
				    dR_.at(EAST),   E_.at(EAST), 
				    dR_.at(NORT),   E_.at(NORT) );
			
			*s << Epoch.toString(SBMJD::F_MJD)  << "  " 
			   << Epoch.toString(SBMJD::F_UNIX) << "  " 
			   << Str << Stat->name() << "\n";
		      };
		  };
	      }
	    else
	      Log->write(SBLog::ERR, SBLog::RUN, ClassName() 
			 + ": cannot find qfile or textstream correspondent to the stationID [" + 
			 key + "] in the dictionaries");
	  };
    };
  

  // at last, close files and flushes streams:
  for (SBNamed* n=IDList.first(); n; n=IDList.next())
    {
      key = n->name();
      if ( (f=FileNames.find(key)) && (s=TextStreams.find(key)))
	{
	  s->unsetDevice();
	  f->close();
	}
      else
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() 
		   + ": cannot find qfile or textstream correspondent to the stationID [" + 
		   key +"] in the dictionaries");
    };

  // create the script for gnuplot:
  double dt = TMax-TMin;
  if (dt>0.0)
    {
      TMax = TMax + 0.02*dt;
      TMin = TMin - 0.02*dt;
    };
  QString	XRanges = (TMin<TMax)?
    "set xrange [\"" + TMin.toString(SBMJD::F_UNIX) + "\":\"" + TMax.toString(SBMJD::F_UNIX) + "\"]\n":
    "set xrange [*:*]\n";

  f = new QFile(Dir2Output + "coord_vars.plt");
  QFileInfo fi(f->name());
  if (!fi.exists() && f->open(IO_WriteOnly))
    {
      s = new QTextStream;
      s->setDevice(f);

      *s << "# This file was automatically created by "<< Version.name() << "\n\n";
      *s << "set term postscript landscape enhanced color solid \"Times-Roman\"\n";
      *s << "#set term postscript eps enhanced color solid \"Times-Roman\"\n\n";
      *s << "#set data style points\nset data style errorbars\nset pointsize 1.0\n";
      *s << "set zeroaxis\nset bar small\n\n";
      *s << "set xdata time\nset timefmt \"%s\"\nset format x \"%b %d\\n%Y\"\n#set format y \"%.1f\"\n\n";
      *s << "set output \"coord_vars.ps\"\n\n";
      *s << XRanges;
      *s << "set yrange [*:*]\n";

      for (SBNamed* n=IDList.first(); n; n=IDList.next())
	{
	  Str = key = n->name();
	  id.setID(key);
	  if (id.isValidId() && (SI = Prj->stationList()->find(&id)) )
	    Str = SI->aka() + " (" + key + ")";

	  *s << "\n#\n# Plots for the station " + Str;
	  if ( SI && (Sta=TRF->find(id)) )
	    *s << ", " << Sta->name() << " located at " << Sta->site()->name() << "\n";
	  else 
	    *s << "\n";
	  *s << "#\nset xlabel \"Time, UTC\"\n";
	  *s << "set xdata time\nset timefmt \"%s\"\nset format x \"%b %d\\n%Y\"\n";
	  *s << XRanges;
	  
	  *s << "\nset ylabel \"Position Variations of " << Str << ", X-coord, cm\"\n";
	  *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:3:4 notitle lt -1 lw 0.2 pt 7\n";

	  *s << "\nset ylabel \"Position Variations of " << Str << ", Y-coord, cm\"\n";
	  *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:5:6 notitle lt -1 lw 0.2 pt 7\n";

	  *s << "\nset ylabel \"Position Variations of " << Str << ", Z-coord, cm\"\n";
	  *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:7:8 notitle lt -1 lw 0.2 pt 7\n";
	  
	  *s << "\nset ylabel \"Position Variations of " << Str << ", Up-direction, cm\"\n";
	  *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:9:10 notitle lt -1 lw 0.2 pt 7\n";

	  *s << "\nset ylabel \"Position Variations of " << Str << ", East-direction, cm\"\n";
	  *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:11:12 notitle lt -1 lw 0.2 pt 7\n";

	  *s << "\nset ylabel \"Position Variations of " << Str << ", North-direction, cm\"\n";
	  *s << "plot \\\n\t\"dat/" << key << ".dat\" using 2:13:14 notitle lt -1 lw 0.2 pt 7\n";

	  *s << "\nset xrange [*:*]\nset xdata\nset format x\n";
	  *s << "set xlabel \"Position Variations of " << Str << ", East-direction, cm\"\n";
	  *s << "set ylabel \"Position Variations of " << Str << ", North-direction, cm\"\n";
	  *s << "plot \\\n\t\"dat/" << key << ".dat\" using 11:13:12:14 notitle with xyerrorbars 7\n\n\n";
	};
      
      *s << "#EOF\n";
      s->unsetDevice();
      delete s;
      f->close();
    };
  delete f;

  
  IDList.clear();
  FileNames.clear();
  TextStreams.clear();

  
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": variations of the station coordinations for the project `" + Prj->name() +
	     "' were created in the directory " + Dir2Output);
};

void SBRunManager::makeReportTRF()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing TRF in SINEX format");

  SBSite		*Site	= NULL;
  SBStation		*Station= NULL;
  SBStationInfo		*SI	= NULL;
  SBStationID		id;


  if (!VLBI)
    {
      mainSetOperation("Loading VLBI dataset");
      if (!(VLBI=SetUp->loadVLBI())) 
	VLBI=new SBVLBISet;
      IsVLBIOwner = TRUE;
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": loaded default VLBI dataset;");
    };

  
  for (Station = TRF->stations()->first(); Station; Station = TRF->stations()->next())
    Station->delAttr(SBStation::Analysed);
  
  for (Station = TRF->stations()->first(); Station; Station = TRF->stations()->next())
    {
      id.setID(Station->id());
      if (!(SI = Prj->stationList()->find(&id)))
	Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find station " + 
		   Station->name() + " [" + id.toString() + "] in the project");
      else if (!SI->isAttr(SBStationInfo::notValid) && 
	       (SI->isAttr(SBStationInfo::EstCoo) || SI->isAttr(SBStationInfo::EstVel)))
	{
	  Station->getGlobalParameters4Report(this); //????????????????????????? for what?
	  Station->addAttr(SBStation::Analysed);
	};
    };

  TRF->finisRun();

  for (uint i=0; i<TRF->count(); i++)
    for (uint j=0; j<(Site=TRF->at(i))->stations().count(); j++)
      {
	Station=Site->stations().at(j);
	if (!Station->isAttr(SBStation::Analysed))
	  {
	    Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": the station [" + 
		       Station->name() + "] was removed from the catalogue");
	    if (Site->delStation(Station))
	      j--;
	    else
	      Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot remove the station [" + 
			 Station->name() + "] from the catalogue");
	  }
      };
  for (uint i=0; i<TRF->count(); i++)
    if (!(Site=TRF->at(i))->stations().count())
      {
	if (TRF->remove(Site->domeMajor()))
	  i--;
	else
	  Log->write(SBLog::ERR, SBLog::STATION, ClassName() + ": cannot remove the site [" + 
		     Site->name() + "] from the catalogue");
      }
	
  TRF->prepare4Run();
  for (Station = TRF->stations()->first(); Station; Station = TRF->stations()->next())
    Station->getGlobalParameters4Report(this);



  SBSolutionName	label = TRF->label();
  label.setAcronym("GAOUA");
  label.setYear(2005);
  label.setSerial(1);
  TRF->setLabel(label);

  QFile		f;
  SBTS_SINEX	*s;
  // SINEX report:
  f.setName(SetUp->path2Output() + Prj->name() + "/" + Prj->name() + ".trf");
  if (f.open(IO_WriteOnly))
    {      
      s = new SBTS_SINEX;
      s->setDevice(&f);
      s->setRunMgr(this);
      *s << *TRF;
      f.close();
      s->unsetDevice();
      if (s) 
	delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + (QString)f.name() + 
		 " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());


  for (Station = TRF->stations()->first(); Station; Station = TRF->stations()->next())
    Station->getGlobalParameters4Report4Axel(this);
  

  // SSC&V report:
  SBTS_ssc2000	*ssc;
  f.setName(SetUp->path2Output() + Prj->name() + "/" + Prj->name() + ".ssc");
  if (f.open(IO_WriteOnly))
    {
      ssc = new SBTS_ssc2000;
      ssc->setDevice(&f);
      //      ssc->setRunMgr(this);
      *ssc << *TRF;
      f.close();
      ssc->unsetDevice();
      if (ssc) 
	delete ssc;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + (QString)f.name() + 
		 " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());
};

void SBRunManager::makeReportCRF()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing CRF in IERS/IVS formats");

  SBSource		*Src	= NULL;
  SBSourceInfo		*SI	= NULL;
  SBParameterList	*LastGlb= NULL;
  SBSymMatrix		*LastP	= NULL;
  
  Solution->loadGlbCor(LastGlb, LastP);
  
  for (Src = CRF->first(); Src; Src = CRF->next())
    Src->delAttr(SBSource::Analysed);
  
  
  for (Src = CRF->first(); Src; Src = CRF->next())
    if (!(SI = Prj->sourceList()->find(Src)))
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find source [" + 
		 Src->name() + "] in the project");
    else if (!SI->isAttr(SBSourceInfo::notValid) && SI->isAttr(SBSourceInfo::EstCoo))
      {
	// coordinates:
	//std::cout << SI->name() << ": " << Src->p_DN()->numSes();
	//std::cout << ". " << Src->nSessApr() << "\n";
	Src->getGlobalParameters4Report(this);
	Src->addAttr(SBSource::Analysed);
	if (SI->isAttr(SBSourceInfo::ConstrCoo))
	  Src->addAttr(SBSource::AnalysConstr);
	if (LastGlb && LastP)
	  {
	    int		idx_RA=0, idx_DN=0;
	    if ( (idx_RA=LastGlb->idx(Src->p_RA()))!=-1 && (idx_DN=LastGlb->idx(Src->p_DN()))!=-1)
	      Src->setCorrRA_DN(LastP->at(idx_RA, idx_DN)/
				sqrt(LastP->at(idx_RA, idx_RA)*LastP->at(idx_DN, idx_DN)));
	  };
      };

  
  for (uint i=0; i<CRF->count(); i++)
    if (!((SBSource*)(CRF->at(i)))->isAttr(SBSource::Analysed))
      {
	if (CRF->remove(i))
	  i--;
	else
	  Log->write(SBLog::ERR, SBLog::SOURCE, ClassName() + ": cannot remove the source [" + 
		     CRF->at(i)->name() + "] from the catalogue");
      }
	
  SBSolutionName	label = CRF->label();
  label.setAcronym("MAO");
  label.setYear(2007);
  label.setSerial(1);
  CRF->setLabel(label);

  QFile		f;
  QTextStream	*s;

  // The output for IERS:
  f.setName(SetUp->path2Output() + Prj->name() + "/" + Prj->name() + ".crf");
  if (f.open(IO_WriteOnly))
    {      
      s = new SBTS_report;
      s->setDevice(&f);
      *(SBTS_report*)s << *CRF;
      f.close();
      s->unsetDevice();
      if (s) delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + (QString)f.name() + 
		 " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());


  // The output for ICRF2 WG: catalog:
  CRF->prepareAliases4Output();
  f.setName(SetUp->path2Output() + Prj->name() + "/" + Prj->name() + ".cat");
  if (f.open(IO_WriteOnly))
    {      
      s = new SBTS_report4IVS;
      s->setDevice(&f);
      *(SBTS_report4IVS*)s << *CRF;
      f.close();
      s->unsetDevice();
      if (s) delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + (QString)f.name() + 
		 " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());
  CRF->releaseAliases4Output();

  if (LastGlb)
    delete LastGlb;
  if (LastP)
    delete LastP;
};

void SBRunManager::makeReportCRFVariations4IVS()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing source coordinates variations in IVS format");

  QString		SessionsName("");
  SBVLBISession		*Session=NULL;
  QList<SBVLBISession>	SessionList;
  SessionList.setAutoDelete(TRUE);
	  
  SBParameterList	*Locals = NULL;
  SBSymMatrix		*Px	= NULL;
  SBSource		*Src	= NULL;
  SBSourceInfo		*SI	= NULL;
  QFile			f;
  SBTS_report4IVS	s;
  SBSolutionName	label	= CRF->label();
  int			idx_RA=0, idx_DN=0;

  label.setAcronym("MAO");
  label.setYear(2007);
  label.setSerial(1);
  CRF->setLabel(label);

  if (!VLBI)
    {
      mainSetOperation("Loading VLBI dataset");
      if (!(VLBI=SetUp->loadVLBI())) VLBI=new SBVLBISet;
      IsVLBIOwner = TRUE;
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": loaded default VLBI dataset;");
    };
  
  
  CRF->prepareAliases4Output();
  f.setName(SetUp->path2Output() + Prj->name() + "/" + Prj->name() + ".ts");
  if (f.open(IO_WriteOnly))
    {      
      s.setDevice(&f);
      /*
	s << "#------------------------------------------------------------------------"
	<< "------------------------------------------------------\n#_______NAME______"
	<< " _______RA________ _______DEC_______ ________ERROR_______         ________EPOCH"
	<< "_________    _#_  ____OBS_____\n#   IAU     B1950  HR MN    SEC       DG MN   ARC"
	<< " SEC    RA SEC  DEC ARCSEC    CCF  MEAN    FIRST   LAST      EXP  DELAY   RATE\n"
	<< "#--------------------------------------------------------------------------------"
	<< "----------------------------------------------\n";
      */
      s << "#--------------------------------------------------------------------------------------------"
	<< "-------------------------------------------- - - -  -\n#_______NAME______ _______RA________ _"
	<< "______DEC_______ ________ERROR_______         ________EPOCH_________    _#_  ____OBS_____ "
	<< "SESSION(s)...\n#   IAU     B1950  HR MN    SEC       DG MN   ARC SEC    RA SEC  DEC ARCSEC    "
	<< "CCF  MEAN    FIRST   LAST      EXP  DELAY   RATE   NAME(s)..\n#-------------------------------"
	<< "----------------------------------------------------------------------------------------------"
	<< "------ - - -  -\n";
      
      QDict<QString>	SessPerSrc;
      SessPerSrc.setAutoDelete(TRUE);
      bool		IsNeedSesLoad;
      
      for (SBStatistics *Stat=Solution->statByBatch().last(); Stat; Stat=Solution->statByBatch().prev())
	{
	  SessionsName="";
	  if (Stat->name().find(",")==-1)
	    {
	      SessionsName=Stat->name();
	      IsNeedSesLoad = FALSE;
	    }
	  else
	    {
	      IsNeedSesLoad = TRUE;
	      QStringList L(QStringList::split(", ", Stat->name()));
	      if (!L.count())
		Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
			   ": got an empty list of sessions name for the batch [" + Stat->name() + "]");
	      else
		for (QStringList::Iterator it = L.begin(); it != L.end(); ++it )
		  if ((Session=VLBI->loadSession(*it)))
		    SessionList.append(Session);
	    };

	  Solution->prepare4Batch(Stat->name());
	  Solution->loadLocCor(Stat->name(), Locals, Px);

	  // remove "Analysed" flag from all sources:
	  for (Src = CRF->first(); Src; Src = CRF->next())
	    Src->delAttr(SBSource::Analysed);
	  
	  // check each source:
	  for (Src = CRF->first(); Src; Src = CRF->next())
	    if (!(SI = Prj->sourceList()->find(Src)))
	      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": cannot find source [" + 
			 Src->name() + "] in the project");
	    else if (!SI->isAttr(SBSourceInfo::notValid) && SI->isAttr(SBSourceInfo::EstCoo))
	      {
		idx_RA=-1;
		idx_DN=-1;
		if (Locals && Px && (idx_RA=Locals->idx(Src->p_RA()))!=-1 && 
		    (idx_DN=Locals->idx(Src->p_DN()))!=-1)
		  {
		    //OK, it is present in the current batch, get it:
		    Src->getParameters4Report(this);
		    Src->setCorrRA_DN(Px->at(idx_RA, idx_DN)/
				      sqrt(Px->at(idx_RA, idx_RA)*Px->at(idx_DN, idx_DN)));
		    if (Src->p_RA()->num()>4)
		      {
			Src->addAttr(SBSource::Analysed);

			if (IsNeedSesLoad)
			  {
			    SessionsName="";
			    for (Session=SessionList.first(); Session; Session=SessionList.next())
			      {
				SBSourceInfo *si = Session->sourceList()->find(Src);
				if (si && si->num())
				  SessionsName += Session->name() + ", ";
			      };
			    SessionsName=SessionsName.left(SessionsName.length()-2);
			  };
			SessPerSrc.insert(Src->name(), new QString(SessionsName));
		      };
		  };
	      };
	  
	  // make an output into the file:
	  for (Src=CRF->first(); Src; Src=CRF->next())
	    if (Src->isAttr(SBSource::Analysed))
	      s << *Src << " " << *SessPerSrc.find(Src->name()) << "\n";

	  if (Locals)
	    {
	      delete Locals;
	      Locals = NULL;
	    };
	  if (Px)
	    {
	      delete Px;
	      Px = NULL;
	    };

	  SessPerSrc.clear();
	  SessionList.clear();
	};
      
      s << "# EOF \n";
    };

  f.close();
  s.unsetDevice();
  CRF->releaseAliases4Output();
      
  if (IsVLBIOwner)
    {
      delete VLBI;
      VLBI=NULL;
    };
  
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": variations of sources coordinates for the project `" + Prj->name() +
	     "' were saved into the file " + (QString)f.name());
};

void SBRunManager::makeReportEOP()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing EOP in IVS and IERS formats");

  SBMJD			T;
  QString		Str;
  SBParameterList	*Locals = NULL;
  SBSymMatrix		*Px = NULL;

  bool			HasPars = FALSE;
  int			idx_1, idx_2;
  unsigned int		NObs;
  double		DT;

  SBParameter		*P    =NULL;
  SBParameter		*P_px =NULL;
  SBParameter		*P_py =NULL;
  SBParameter		*P_ut =NULL;
  SBParameter		*P_psi=NULL;
  SBParameter		*P_eps=NULL;
  SBMaster		*Master = NULL;
  SBMasterRecord	*rec = NULL;

  double		cor_xy, cor_xut, cor_yut, cor_nut;

  QFile			f_iers;
  QTextStream		s_iers;
  QFile			f_ivs;
  QTextStream		s_ivs;

  QString		SessionsCodes;
  QString		SessionsNames;
  QString		NutName1 = Prj->cfg().sysTransform()==SBConfig::STR_Classic?"dPsi":" dX ";
  QString		NutName2 = Prj->cfg().sysTransform()==SBConfig::STR_Classic?"dEps":" dY ";
  QString		NutName3 = Prj->cfg().sysTransform()==SBConfig::STR_Classic?"Psi-Eps":"CIP:X-Y";

  f_iers.setName(SetUp->path2Output() + Prj->name() + "/mao_r.eop");
  f_ivs.setName (SetUp->path2Output() + Prj->name() + "/" + Prj->name() + ".eops");

  if (!f_iers.open(IO_WriteOnly))
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + f_iers.name());
      return;
    };
  if (!f_ivs.open(IO_WriteOnly))
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + f_ivs.name());
      f_iers.close();
      return;
    };


  if (!VLBI)
    {
      mainSetOperation("Loading VLBI dataset");
      if (!(VLBI=SetUp->loadVLBI())) 
	VLBI=new SBVLBISet;
      IsVLBIOwner = TRUE;
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": loaded default VLBI dataset;");
    };

  VLBI->loadMaster();
  Master = VLBI->master();
  
  s_iers.setDevice(&f_iers);
  s_ivs .setDevice(&f_ivs );


  s_iers << "#\n#  EOP(GAOUA)\n#\n#  VLBI EARTH ORIENTATION DATA IN THE IERS FORMAT\n"
	 << "#  (see http://hpiers.obspm.fr/eoppc/series/operational/format.eop)\n#\n"

	 << "#-------------+--------------------------------------------------------+------------"
	 << "---------------------------------------------+------------------------------+------"
	 << "-----------------+---------------------------------------------------+--------------"
	 << "-----------------------------------+\n"

	 << "#             |                    EARTH ORIENTATION                   |            "
	 << "         UNCERTAINTIES                       |        CORRELATION           |     "
	 << "INDICATORS        |     RATES OF EARTH ORIENTATION AND NUTATION       |            "
	 << " UNCERTAINITIES OF RATES             |\n"
    
	 << "#-------------+--------------------------------------------------------+-------------"
	 << "--------------------------------------------+------------------------------+--------"
	 << "---------------+---------------------------------------------------+-----------------"
	 << "--------------------------------+\n"

	 << "# Mean  Epoch |  x-Pole     y-Pole    d(UT1-UTC)    " << NutName1 << "       " << NutName2 
	 << "    |  x-Pole    y-Pole"
	 << "   d(UT1-UTC)   " << NutName1 << "     " << NutName2 
	 << "     RMS  |   x-y   x-UT   y-UT  " << NutName3 << " |  Nb of  Span of "
	 << "  N/A |   x-rate    y-rate  dUT1-rate " << NutName1 << "-rate " << NutName2 
	 << "-rate |   x-rate    y-rate  dUT1-rate "
	 << NutName1 << "-rate " << NutName2 << "-rate\n"
    
	 << "# of Obs, MJD | (arcsec)   (arcsec)     (sec)     (arcsec)   (arcsec)  | (arcsec)  (arcsec)"
	 << "    (sec)   (arcsec)  (arcsec)   (ps) |                              |   Obs.  Obs. (d)    "
	 << "  |   (arcsec per day)   (sec/d)    (arcsec per day)  |   (arcsec per day)  (sec/day)   "
	 << "(arcsec per day) \n"

	 << "#    (1)      |    (2)        (3)         (4)        (5)        (6)    |    (7)       "
	 << "(8)       (9)      (10)      (11)     (12) |  (13)   (14)   (15)   (16)   |  (17)    "
	 << "(18)    (19) |   (20)      (21)      (22)      (23)      (24)    |   (25)      (26)      "
	 << "(27)      (28)      (29)  \n";


  s_ivs << "#\n#  EOP(GAOUA)\n#\n#  VLBI EARTH ORIENTATION DATA IN THE IVS EOP FORMAT Version 2.0\n"
	<< "#  (see http://giub.geod.uni-bonn.de/vlbi/IVS-AC/eop-format.txt)\n#\n"

	<< "#-------------+----------------------------------------------------+---------------"
	<< "--------------------------------------+---------------------------------+----------"
	<< "---------------------------------+-------------------------------------------------"
	<< "--+-------------------------------------------------+\n"

	<< "#             |                 EARTH ORIENTATION                  |               "
	<< "      UNCERTAINTIES                   |        CORRELATION              |     INDICATORS"
	<< "                            |     RATES OF EARTH ORIENTATION AND NUTATION       |       "
	<< "      UNCERTAINITIES OF RATES             |\n"

	<< "#-------------+----------------------------------------------------+---------------"
	<< "--------------------------------------+---------------------------------+----------"
	<< "---------------------------------+-------------------------------------------------"
	<< "--+-------------------------------------------------+\n"

	<< "# Mean  Epoch |  x-Pole     y-Pole    d(UT1-UTC)   " << NutName1 << "     " << NutName2 
	<< "   |  x-Pole    y-Pole"
	<< "  d(UT1-UTC)   " << NutName1 << "    "<< NutName2 
	<< "    RMS |   x-y     x-UT    y-UT  " << NutName3 << " |  Nb of       "
	<< "Session(s) code      Span of |  x-rate    y-rate      LOD    " << NutName1 << "-rate "
	<< NutName2 << "-rate |  "
	<< "x-rate    y-rate      LOD    " << NutName1 << "-rate " << NutName2 << "-rate\n"
    
	<< "# of Obs, MJD | (arcsec)   (arcsec)     (sec)      (mas)    (mas)  | (arcsec)  (arcsec)"
	<< "    (sec)    (mas)   (mas)   (ps) |                                 |   Obs.           "
	<< "                Obs. (hr)|  (arcsec per day)     (sec)     (mas  per day)    |  (arcsec per day)"
	<< "     (sec)     (mas  per day)   \n"

	<< "#    (1)      |    (2)        (3)         (4)       (5)      (6)   |    (7)       (8)  "
	<< "     (9)      (10)    (11)   (12) |   (13)    (14)    (15)    (16)  |   (17)           "
	<< " (18)              (19)  |   (20)      (21)      (22)      (23)      (24)    |   (25)  "
	<< "    (26)      (27)      (28)      (29)   \n";

  
  for (SBStatistics *Stat=Solution->statByBatch().last(); Stat; Stat=Solution->statByBatch().prev())
    {
      Solution->prepare4Batch(Stat->name());
      Solution->loadLocCor(Stat->name(), Locals, Px);
      NObs = 0;
      DT = 0.0;
      HasPars = FALSE;
      
      for (P=Solution->local().first(); P && !HasPars; P=Solution->local().next())
	if (P->name().contains("EOP: "))
	  HasPars = TRUE;
      if (HasPars)
	{
	  SessionsNames = Stat->name();
	  SessionsNames.replace(QRegExp(" "), "");
	  
	  P_px = Solution->local().find(Frame->eop()->p_Px()->name());
	  P_py = Solution->local().find(Frame->eop()->p_Py()->name());
	  P_ut = Solution->local().find(Frame->eop()->p_Put()->name());
	  P_psi= Solution->local().find(Frame->eop()->p_Npsi()->name());
	  P_eps= Solution->local().find(Frame->eop()->p_Neps()->name());
	  
	  T = P_ut->tEpoch();
	  Frame->eop()->prepare4Date(T);
	  Frame->calc(T);

	  cor_xy = cor_xut = cor_yut = cor_nut = 0.0;
	  if (Locals && Px)
	    {
	      if (P_px && P_py)
		if ( (idx_1=Locals->idx(P_px))!=-1 && (idx_2=Locals->idx(P_py))!=-1)
		  cor_xy = Px->at(idx_1, idx_2)/sqrt(Px->at(idx_1, idx_1)*Px->at(idx_2, idx_2));

	      if (P_px && P_ut)
		if ( (idx_1=Locals->idx(P_px))!=-1 && (idx_2=Locals->idx(P_ut))!=-1)
		  cor_xut = Px->at(idx_1, idx_2)/sqrt(Px->at(idx_1, idx_1)*Px->at(idx_2, idx_2));

	      if (P_py && P_ut)
		if ( (idx_1=Locals->idx(P_py))!=-1 && (idx_2=Locals->idx(P_ut))!=-1)
		  cor_yut = Px->at(idx_1, idx_2)/sqrt(Px->at(idx_1, idx_1)*Px->at(idx_2, idx_2));
	      
	      if (P_psi && P_eps)
		if ( (idx_1=Locals->idx(P_psi))!=-1 && (idx_2=Locals->idx(P_eps))!=-1)
		  cor_nut = Px->at(idx_1, idx_2)/sqrt(Px->at(idx_1, idx_1)*Px->at(idx_2, idx_2));
	    };

	  if (P_px)
	    {
	      NObs = P_px->num();
	      DT   = P_px->tInterval();
	    }
	  else if (P_py)
	    {
	      NObs = P_py->num();
	      DT   = P_py->tInterval();
	    }
	  else if (P_ut)
	    {
	      NObs = P_ut->num();
	      DT   = P_ut->tInterval();
	    }
	  else if (P_psi)
	    {
	      NObs = P_psi->num();
	      DT   = P_psi->tInterval();
	    }
	  else if (P_eps)
	    {
	      NObs = P_eps->num();
	      DT   = P_eps->tInterval();
	    };
	  
	  if (Master)
	    {
	      SessionsCodes = "";
	      idx_1 = 0;
	      while ( (idx_2=SessionsNames.find(",", idx_1))!=-1 )
		{
		  Str = SessionsNames.mid(idx_1, idx_2-idx_1);
		  if ((rec=Master->lookupMR(Str)) )
		    SessionsCodes += QString(rec->sessionCode()).replace(QRegExp(" "), "") + ",";
		  else 
		    SessionsCodes += "[" + Str + "],";
		  
		  idx_1 = idx_2 + 1;
		};

	      Str = SessionsNames.mid(idx_1);
	      if ((rec=Master->lookupMR(Str)) )
		SessionsCodes += rec->sessionCode();
	      else 
		SessionsCodes += "[" + Str + "]";
	      
	    }
	  else
	    SessionsCodes = SessionsNames;
	  
	  // IERS format:
	  // URL: http://hpiers.obspm.fr/eoppc/series/operational/format.eop
	  /*
	   *
	   *                        FORMAT
	   *
	   *
	   * Field   Contents
	   *(fields separated by blanks)
	   *
	   *
	   *  1     MJD of the measurement
	   *        
	   *     EARTH ORIENTATION
	   *  2     x of the pole (")
	   *  3     y of the pole (")
	   *  4     UT1-UTC (s)
	   *  
	   *     NUTATION
	   *  5     dpsi (")
	   *  6     deps (")
	   *  
	   *     UNCERTAINITY OF EARTH ORIENTATION AND NUTATION
	   *  7     uncertainty on x (")
	   *  8     uncertainty on y (")
	   *  9     uncertainty on UT (s)
	   * 10     uncertainty on dpsi (")
	   * 11     uncertainty on deps (")
	   * 12     rms residual of the least squares solution providing the EOP in ps
	   * 
	   *    CORRELATION
	   * 13     correlation coefficient : x, y
	   * 14     correlation coefficient : x, UT1
	   * 15     correlation coefficient : y, UT1
	   * 16     correlation coefficient : dpsi,deps
	   *   
	   *                         INDICATORS
	   *  ____________________________________________________________________
	   *        FOR VLBI AND SLR        |               FOR GPS
	   *                                |
	   * 17     number of observation   |   number of receivers in the solution
	   * 18     span of the observation |   number of receivers with "fixed" solution
	   * 19     not used                |   number of satellites (transmitters) in the solution
	   *	 
	   *    RATES OF EARTH ORIENTATION AND NUTATION
	   * 20     x rate of the pole ("/d)
	   * 21     y rate of the pole ("/d)
	   * 22     LOD (s/d)
	   * 23     dpsi rate ("/d)
	   * 24     desp rate ("/d)
	   *
	   *    UNCERTAINITY OF EARTH ORIENTATION AND NUTATION
	   * 25     uncertainty on x rate ("/d)
	   * 26     uncertainty on y rate ("/d)
	   * 27     uncertainty on LOD (s/d)
	   * 28     uncertainty on dpsi rate ("/d)
	   * 29     uncertainty on deps rate ("/d)
	   *
	   */
	  Str.sprintf(" %12.6f  %10.7f %10.7f %11.8f %10.7f %10.7f   %9.7f %9.7f %9.7f %9.7f %9.7f %5.1f  "
		      " %6.3f %6.3f %6.3f %6.3f   %6d %9.6f %4d  "
		      " %9.6f %9.6f %9.6f %9.6f %9.6f   %9.6f %9.6f %9.6f %9.6f %9.6f",
		      (double)T, 
		      Frame->polar_X()*RAD2SEC, Frame->polar_Y()*RAD2SEC,
		      Frame->polar_UT1_UTC()*DAY2SEC,
		      Frame->dNutation_Psi()*RAD2SEC, Frame->dNutation_Eps()*RAD2SEC,
		      P_px?P_px->e()*RAD2SEC:0.0,
		      P_py?P_py->e()*RAD2SEC:0.0,
		      P_ut?P_ut->e()*DAY2SEC:0.0,
		      P_psi?P_psi->e()*RAD2SEC:0.0,
		      P_eps?P_eps->e()*RAD2SEC:0.0,
		      Stat->WRMS_unit()*1.0e+12,
		      cor_xy, cor_xut, cor_yut, cor_nut,
		      NObs, DT, 0,
		      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
		      );
	  s_iers << Str << "\n";

	  
	  //
	  // IVS format:
	  // URL: http://giub.geod.uni-bonn.de/vlbi/IVS-AC/eop-format.txt
	  /*
	   *                             IVS EOP FORMAT Version 2.0
	   *                    Axel Nothnagel, IVS Analysis Coordinator
	   *                           nothnagel@uni-bonn.de
	   *
	   *Files are free format with fields separated by at least one blank.
	   *
	   * FIELD   CONTENTS
	   * 
	   *  1     MJD of the measurement (with at least 5 decimal digits)
	   *        
	   *     EARTH ORIENTATION
	   *
	   *  2     x of the pole (arcseconds)
	   *  3     y of the pole (arcseconds)
	   *  4     UT1-UTC (seconds)
	   *  
	   *     NUTATION
	   *
	   *  5     dpsi (milliarcseconds)
	   *  6     deps (milliarcseconds)
	   *  
	   *     UNCERTAINTY OF EARTH ORIENTATION AND NUTATION
	   *
	   *  7     uncertainty in x (arcseconds)
	   *  8     uncertainty in y (arcseconds)
	   *  9     uncertainty in UT1-UTC (seconds)
	   * 10     uncertainty in dpsi (milliarcseconds)
	   * 11     uncertainty in deps (milliarcseconds)
	   *
	   * 12     wrms residual delay of the session in ps (preferably integer)
	   * 
	   *    CORRELATIONS
	   *
	   * 13     correlation coefficient : x, y
	   * 14     correlation coefficient : x, UT1
	   * 15     correlation coefficient : y, UT1
	   * 16     correlation coefficient : dpsi,deps
	   *   
	   *    INDICATORS
	   *
	   * 17     number of observables    
	   * 18     6 character session code according to the 2nd
	   *        column of IVS master schedules,
	   *        e.g for 2002: ftp://gemini.gsfc.nasa.gov/pub/master/master02.txt
	   * 19     span of the observation (in hours)       
	   * 
	   *   RATES OF EARTH ORIENTATION AND NUTATION
	   *
	   * 20     x rate of the pole (arcseconds/day)
	   * 21     y rate of the pole (arcseconds/day)
	   * 22     excess length of day (LOD) (seconds), see comment below 
	   * 23     dpsi rate (milliarcseconds/day)
	   * 24     deps rate (milliarcseconds/day)
	   * 
	   *   UNCERTAINTY OF RATES OF EARTH ORIENTATION AND NUTATION
	   *
	   * 25     uncertainty in x rate (arcseconds/day)
	   * 26     uncertainty in y rate (arcseconds/day)
	   * 27     uncertainty in LOD (s)
	   * 28     uncertainty in dpsi rate (milliarcseconds/day)
	   * 29     uncertainty in deps rate (milliarcseconds/day)
	   *
	   *
	   *Comments:
	   *
	   *- For the EOP rates, days are in 86400 SI seconds.
	   *- The lines starting with characters "! # or *" are considered as comments.
	   *- Although it is inconsistent to have LOD in the group of rates due to the
	   *  toggling of the sign, it is maintained here for consistency with other
	   *  space techniques.
	   *  LOD = UT1(n) - UT1(n+1) with n in days.
	   *  LOD has the same magnitude but opposite sign of the UT1-UTC rate 
	   *  or the UT1-TAI rate, which are normally determined in VLBI analyses and
	   *  given in seconds/day.
	   *
	   *
	   *Last updated: 2002.02.25
	   *Axel Nothnagel
	   *
	   */

	  Str.sprintf(" %12.6f  %10.7f %10.7f %11.8f %8.4f %8.4f   %9.7f %9.7f %9.7f %7.4f %7.4f %5.1f  "
		      " %7.4f %7.4f %7.4f %7.4f   %6d %-27s %6.3f  "
		      " %9.6f %9.6f %9.6f %9.6f %9.6f   %9.6f %9.6f %9.6f %9.6f %9.6f",
		      (double)T, 
		      Frame->polar_X()*RAD2SEC, Frame->polar_Y()*RAD2SEC,
		      Frame->polar_UT1_UTC()*DAY2SEC,
		      Frame->dNutation_Psi()*RAD2SEC*1000.0, Frame->dNutation_Eps()*RAD2SEC*1000.0,
		      P_px?P_px->e()*RAD2SEC:0.0,
		      P_py?P_py->e()*RAD2SEC:0.0,
		      P_ut?P_ut->e()*DAY2SEC:0.0,
		      P_psi?P_psi->e()*RAD2SEC*1000.0:0.0,
		      P_eps?P_eps->e()*RAD2SEC*1000.0:0.0,
		      Stat->WRMS_unit()*1.0e+12,
		      cor_xy, cor_xut, cor_yut, cor_nut,
		      NObs, (const char*)SessionsCodes,
		      DT*24.0,
		      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
		      );
	  s_ivs << Str << "\n";

	  if (Locals)
	    {
	      delete Locals;
	      Locals = NULL;
	    };
	  if (Px)
	    {
	      delete Px;
	      Px = NULL;
	    };
	};
    };

  Str = "# END OF DATA";
  s_iers << Str << "\n";
  s_ivs  << Str << "\n";

  f_iers.close();
  s_iers.unsetDevice();
  f_ivs.close();
  s_ivs.unsetDevice();
  
  VLBI->releaseMaster();
  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + f_iers.name() + " has been saved");
  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + f_ivs.name() + " has been saved");
};

void SBRunManager::makeReportTroposphere()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing troposphere in SINEX/IVS format");

  int			idx_1, idx_2;
  QString		Str;

  SBMaster		*Master = NULL;
  SBMasterRecord	*rec = NULL;

  SBVLBISession		*Session = NULL;

  QFile			file;
  SBTS_SINEX		*stream;
  
  QString		SessionCode;
  QString		BatchName;

  if (!VLBI)
    {
      mainSetOperation("Loading VLBI dataset");
      if (!(VLBI=SetUp->loadVLBI())) 
	VLBI=new SBVLBISet;
      IsVLBIOwner = TRUE;
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": loaded default VLBI dataset;");
    };

  VLBI->loadMaster();
  Master = VLBI->master();
  
  for (SBStatistics *Stat=Solution->statByBatch().last(); Stat; Stat=Solution->statByBatch().prev())
    {
      Solution->prepare4Batch(Stat->name());
      
      BatchName = Stat->name() + ",";
      BatchName.replace(QRegExp(" "), "");

      idx_1 = 0;
      while ( (idx_2=BatchName.find(",", idx_1))!=-1 )
	{
	  Str = BatchName.mid(idx_1, idx_2-idx_1);
	  if (Master && (rec=Master->lookupMR(Str)) )
	    SessionCode = rec->sessionCode();
	  else 
	    SessionCode = "[" + Str + "]";
	  SessionCode.replace(QRegExp(" "), "");
	  idx_1 = idx_2 + 1;
	  
	  if ((Session=VLBI->loadSession(Str)))
	    {
	      // averages:
	      file.setName(SetUp->path2Output() + Prj->name() + "/mao" + SessionCode + ".tro");
	      if (file.open(IO_WriteOnly))
		{      
		  stream = new SBTS_SINEX;
		  stream->setDevice(&file);
		  stream->setRunMgr(this);
		  *stream << *Session;
		  file.close();
		  stream->unsetDevice();
		  if (stream) 
		    delete stream;
		  Log->write(SBLog::DBG, SBLog::IO || SBLog::RUN, ClassName() + ": file " + 
			     (QString)file.name() + " has been saved");
		}
	      else    
		Log->write(SBLog::ERR, SBLog::IO || SBLog::RUN, ClassName() + 
			   ": could not write " + (QString)file.name());

	      // tech. sescript.:
	      file.setName(SetUp->path2Output() + Prj->name() + "/mao" + SessionCode + ".tro.txt");
	      if (file.open(IO_WriteOnly))
		{      
		  stream = new SBTS_SINEX;
		  stream->setDevice(&file);
		  stream->setRunMgr(this);
		  *stream << "Technical description of the solution `mao" + SessionCode + ".tro'.\n\n";
		  *stream << "VLBI data analysis was performed according to IERS Conventions (2003).\n";
		  *stream << "Estimated parameters: clocks, tropospheric parameters, station coordinates\n";
		  *stream << "and EOP. The solution, in general, is similar to the mao2003 one, except for\n";
		  *stream << "number of processed sessions.\n\n";
		  *stream << "For comments or questions contact S.Bolotin <bolotin@mao.kiev.ua>.\n";

		  file.close();
		  stream->unsetDevice();
		  if (stream) 
		    delete stream;
		  Log->write(SBLog::DBG, SBLog::IO || SBLog::RUN, ClassName() + ": file " + 
			     (QString)file.name() + " has been saved");
		}
	      else    
		Log->write(SBLog::ERR, SBLog::IO || SBLog::RUN, ClassName() + 
			   ": could not write " + (QString)file.name());

	      // real estimations:
	      double TSI_saved = Prj->cfg().tropSamplInterval();
	      if (TSI_saved > 60.0)
		{
		  // make reports for stochastic parameters too (just for curiosity):
		  SBConfig Cfg_ = Prj->cfg();
		  Cfg_.setTropSamplInterval(0.0);
		  Prj->setCfg(Cfg_);
		  file.setName(SetUp->path2Output() + Prj->name() + "/mao" + SessionCode + ".tro.s");
		  if (file.open(IO_WriteOnly))
		    {      
		      stream = new SBTS_SINEX;
		      stream->setDevice(&file);
		      stream->setRunMgr(this);
		      *stream << *Session;
		      file.close();
		      stream->unsetDevice();
		      if (stream) 
			delete stream;
		      Log->write(SBLog::DBG, SBLog::IO || SBLog::RUN, ClassName() + ": file " + 
				 (QString)file.name() + " has been saved");
		    }
		  else    
		    Log->write(SBLog::ERR, SBLog::IO || SBLog::RUN, ClassName() + 
			       ": could not write " + (QString)file.name());

		  Cfg_.setTropSamplInterval(TSI_saved);
		  Prj->setCfg(Cfg_);
		};
	      
	      delete Session;
	    }
	  else    
	    Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": could not load a session for the [" + 
		       Str + "] part of the batch [" + Stat->name() + "]");
	};
    };

  VLBI->releaseMaster();
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": tropospheric report has been finished for project " + Prj->name());
};

void SBRunManager::makeReportTechDescription()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing technical description");
};

void SBRunManager::makeReportNormalEqs()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing normal equation system in SINEX format");

  
  SBParameterList	*Ys = NULL;
  SBVector		*Zy = NULL;
  SBUpperMatrix		*Ry = NULL;
  SBParameterList	*Xs = NULL;
  SBVector		*Zx = NULL;
  SBUpperMatrix		*Rx = NULL;
  SBMatrix		*Rxy= NULL;

  QString		Str;

  QFile			file;
  SBTS_SINEX		*stream;

  
  QString		SessionCode;
  QString		BatchName;

  if (!VLBI)
    {
      mainSetOperation("Loading VLBI dataset");
      if (!(VLBI=SetUp->loadVLBI())) 
	VLBI=new SBVLBISet;
      IsVLBIOwner = TRUE;
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": loaded default VLBI dataset;");
    };

  for (SBStatistics *Stat=Solution->statByBatch().last(); Stat; Stat=Solution->statByBatch().prev())
    {
      Solution->prepare4Batch((BatchName=Stat->name()));

      Solution->loadNormalEqs(Ys, Zy, Ry,   Xs, Zx, Rx, Rxy,  BatchName);

      file.setName(SetUp->path2Output() + Prj->name() + "/" + BatchName + "_mao05it1.snx");
      if (file.open(IO_WriteOnly))
	{      
	  stream = new SBTS_SINEX;
	  stream->setDevice(&file);
	  stream->setRunMgr(this);
	  writeNormalEquationSystem(*stream, Ys, Zy, Ry,   
				    Xs, Zx, Rx, Rxy,  BatchName, BatchName + "_mao05it1.snx");
	  file.close();
	  stream->unsetDevice();
	  if (stream) 
	    delete stream;
	  Log->write(SBLog::DBG, SBLog::IO || SBLog::RUN, ClassName() + ": file " + 
		     (QString)file.name() + " has been saved");
	}
      else    
	Log->write(SBLog::ERR, SBLog::IO || SBLog::RUN, ClassName() + 
		   ": could not write " + (QString)file.name());

      if (Ys)
	{
	  delete Ys;
	  Ys = NULL;
	};
      if (Zy)
	{
	  delete Zy;
	  Zy = NULL;
	};
      if (Ry)
	{
	  delete Ry;
	  Ry = NULL;
	};

      if (Xs)
	{
	  delete Xs;
	  Xs = NULL;
	};
      if (Zx)
	{
	  delete Zx;
	  Zx = NULL;
	};
      if (Rx)
	{
	  delete Rx;
	  Rx = NULL;
	};
      if (Rxy)
	{
	  delete Rxy;
	  Rxy = NULL;
	};
    };

  VLBI->releaseMaster();
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": normal equation systems output has been finished for project " + Prj->name());
};

void SBRunManager::makeReportSessionStatistics()
{
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": writing list of sessions and their statistics");

  SBVLBISession		*Session=NULL;
  QFile			f;
  QTextStream		s;
  QString		Str;
  int			i;
  

  if (!VLBI)
    {
      mainSetOperation("Loading VLBI dataset");
      if (!(VLBI=SetUp->loadVLBI())) 
	VLBI=new SBVLBISet;
      IsVLBIOwner = TRUE;
      Log->write(SBLog::WRN, SBLog::DATA | SBLog::IO, ClassName() + 
		 ": loaded default VLBI dataset;");
    };
  
  f.setName(SetUp->path2Output() + Prj->name() + "/" + Prj->name() + ".lst");
  if (f.open(IO_WriteOnly))
    {      
      s.setDevice(&f);
      s << "#  In this file the char `#' means comments.\n#\n#\n"
	<< "#  Each session is presented with three lines, one record and two\n"
	<< "#  comments. The record line is written in the format\n"
	<< "#  \"%-9s %-6s %-10s  %9.3f %9.3f  %3d %4d %6d\"\n"
	<< "#  and presents the following fields: Session, Code, Official_Name,\n"
	<< "#  T_First, T_Last, #Sta, #Src and #Obs. Here\n"
	<< "#     Session    = Session name,\n"
	<< "#     Code       = Session code according to a master file,\n"
	<< "#     Offic.Name = Session name according to a master file,\n"
	<< "#     T_First    = MJD of a first observation,\n"
	<< "#     T_Last     = MJD of a last observation,\n"
	<< "#     #Sta       = Number of stations,\n"
	<< "#     #Src       = Number of sources,\n"
	<< "#     #Obs       = Number of observations.\n#\n"
	<< "#  First line in the comments: a list of comma separated elemenths which\n"
	<< "#  are describe station statistics. Each elementh have a form\n#\n"
	<< "#       IVS_station_name(NTot, NProc)\n#  Where\n"
	<< "#     IVS_station_name = 8-char name of a station, taken from DBH file,\n"
	<< "#     NTot             = number of all observations for the station,\n"
	<< "#     NProc            = number of observations which were included in the\n"
	<< "#                        solution\n#\n"
	<< "#  The second line in the comments have the same form and describes radio sources.\n#\n"
	<< "#  To remove comments, pipe the data through\n"
	<< "#                           grep -v \"^#\"\n"
	<< "#  To get just a list of sessions, pipe it through\n"
	<< "#                           grep -v \"^#\" | awk \"{print \\$1}\"\n"
	<< "#  or anything similar.\n#\n#\n"
	<< "#Session_ _Code_ Offic.Name  _T_First_ _T_Last__ #Sta #Src _#Obs_\n#\n";

      for (SBVLBISesInfo *si=Prj->VLBIsessionList()->first(); si; si=Prj->VLBIsessionList()->next())
	if (!si->isAttr(SBVLBISesInfo::notValid) && (Session=VLBI->loadSession(si->name())))
	  {
	    Str.sprintf("%-9s %-6s %-10s  %9.3f %9.3f  %3d %4d %6d",
			(const char*)Session->name(),
			(const char*)Session->code(),
			(const char*)Session->officialName(),
			(double)Session->tStart(),
			(double)Session->tFinis(),
			Session->numSta(),
			Session->numSou(),
			Session->numObs()
			);
	    s << Str << "\n";

	    // Stations:
	    s << "#";
	    i = Session->stationList()->count();
	    for (SBStationInfo* SI=Session->stationList()->first(); SI; SI=Session->stationList()->next())
	      {
		s << " " << SI->aka() << "(" << SI->num() << "," << SI->procNum() << ")";
		if (Session->stationList()->at()<i-1)
		  s << ",";
	      };
	    s << "\n";

	    // Sources:
	    s << "#";
	    i = Session->sourceList()->count();
	    for (SBSourceInfo* SI=Session->sourceList()->first(); SI; SI=Session->sourceList()->next())
	      {
		s << " " << SI->aka() << "(" << SI->num() << "," << SI->procNum() << ")";
		if (Session->sourceList()->at()<i-1)
		  s << ",";
	      };
	    s << "\n";
	    
	    

	    delete Session;
	  };

      s << "# EOF\n";
    };
  
  f.close();
  s.unsetDevice();
  if (IsVLBIOwner)
    {
      delete VLBI;
      VLBI = NULL;
    };
  
  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + 
	     ": a list of sessions with their statistics for the project `" + Prj->name() +
	     "' were saved into the file " + (QString)f.name());
};



/*==============================================================================================*/
