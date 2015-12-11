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

#include "SbSetup.H"
#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include <unistd.h>
#include <errno.h>

#include "SbGeoAtmLoad.H"
#include "SbGeoEop.H"
#include "SbGeoSources.H"
#include "SbGeoStations.H"
#include "SbGeoObsVLBI.H"
#include "SbGeoObsVLBIStat.H"
#include "SbGeoProject.H"



const char*	SBSetUp::sInstitutions = "institutions.bin";
SBSetUp		*SetUp;
const SBInstitution InstitutionsDefault[59]= 
{
  SBInstitution("AIUB",  "Astronomical Institute, University of Bern"),
  SBInstitution("AER",   "Atmospheric and Environmental Research Ins."),
  SBInstitution("BIH",   "Bureau International de l'Heure"),
  SBInstitution("BIPM",  "Bureau International des Poids et Mesures"),
  SBInstitution("CERGA", "Centre d'Etudes et de Recherches Geodynamiques et Astronomiques"),
  SBInstitution("CCIR",  "International Radio Consultative Committee"),
  SBInstitution("CFA",   "Harvard-Smithsonian Center for Astrophysics"),
  SBInstitution("CGS",   "Centro di Geodesia Spatiale, Roma"),
  SBInstitution("CLG",   "Central Laboratory for Geodesy, Sofia"),
  SBInstitution("CODE",  "Centre for Orbit Determination in Europe"),
  SBInstitution("CSR",   "Center for Space Research, University of Texas"),
  SBInstitution("CRL",   "Communication Research Laboratory"),
  SBInstitution("DGFII", "Deutsches Geodatisches Forschunginstitut, Abt. 1"),
  SBInstitution("DMA",   "Defence Mapping Agency"),
  SBInstitution("DPMS",  "Dahlgren Polar Monitoring Service"),
  SBInstitution("DUT",   "Delft University of Technology"),
  SBInstitution("ECMWF", "European Centre for Medium-range Weather Forecasting"),
  SBInstitution("EMR",   "Energy, Mines and Resources Canada"),
  SBInstitution("ESOC",  "European Space Agency Operational Centre"),
  SBInstitution("FSG",   "Forschunseinrichtung Satellitengeodaesie"),
  SBInstitution("GAOUA", "Main Astronomical Observatory of the Ukrainian Acad. of Sciences"),
  SBInstitution("GFZ",   "GeoForschungsZentrum, Potsdam"),
  SBInstitution("GUIB",  "Geodetic Institute of the University of Bonn"),
  SBInstitution("GRGS",  "Groupe de Recherches de Geodesie Spatiale"),
  SBInstitution("GSFC",  "Goddard Space Flight Center"),
  SBInstitution("IAA",   "Institute of Applied Astronomy, St Petersburg"),
  SBInstitution("IAG",   "International Association of Geodesy"),
  SBInstitution("IARAS", "Institute of Astronomy, Russian Acad. of Sci."),
  SBInstitution("IAU",   "International Astronomical Union"),
  SBInstitution("IERS",  "International Earth Rotation Service"),
  SBInstitution("IGS",   "International GPS Geodynamics Service"),
  SBInstitution("ILS",   "International Latitude Service"),
  SBInstitution("ITRF",  "International Terrestrial Reference Frame"), // faked entry
  SBInstitution("IUGG",  "International Union of Geodesy and Geophysics"),
  SBInstitution("JHD",   "Japan Hydrographic Departament"),
  SBInstitution("JMA",   "Japan Meteorological Agency"),
  SBInstitution("JPL",   "Jet Propulsion Laboratory"),
  SBInstitution("MCC",   "Russian Mission Control Centre"),
  SBInstitution("MIT",   "Massachusetts Institute of Technology"),
  SBInstitution("NAL",   "National Aerospace Laboratory, Japan"),
  SBInstitution("NAOMZ", "National Astronomical Observatory, Mizusawa branch"),
  SBInstitution("NEOS",  "National Earth Orientation Service"),
  SBInstitution("NGS",   "National Geodetic Survey"),
  SBInstitution("NMC",   "National Meteorological Center"),
  SBInstitution("NOAA",  "National Oceanic and Atmospheric Administration"),
  SBInstitution("NRL",   "Naval Research Laboratory"),
  SBInstitution("NRCan", "Natural Resources, Canada"),
  SBInstitution("NSWC",  "Naval Surface Weapon Center"),
  SBInstitution("OCA",   "Observatoire de la Cote d'Azur"),
  SBInstitution("OPA",   "Observatoire de Paris"),
  SBInstitution("SAO",   "Smithsoinian Astrophysical Observatory"),
  SBInstitution("SHA",   "Shanghai Observatory"),
  SBInstitution("SIO",   "Scripps Institution of Oceanography"),
  SBInstitution("UKMO",  "U.K. Meteorological Office"),
  SBInstitution("UPAD",  "Universita di Padova and Telespazio S.P.A."),
  SBInstitution("USNO",  "US Naval Observatory"),
  SBInstitution("UTXMO", "Dept. of Astron. The University of Texas at Austin"),
  SBInstitution("WGRF",  "IAU Working Group on Reference Frames"),
  SBInstitution("UNKN",  "One of institutions/research centers")
};


/*==============================================================================================*/
/*												*/
/* class SBFileConv implementation								*/
/*												*/
/*==============================================================================================*/
/**Create compressed input.
 * Opens a pipe through uncompress command, opens corresponded QFile, returns opened pipe
 * or NULL if popen failed. 
 * \warning caller have to pclose returned value manually or call a SBFileConv::close.
 */
FILE* SBFileConv::open4In(const QString& FileName, QFile& file, QDataStream& ds)
{
  FILE* p=NULL;
  if(ComUncom=="NONE")
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + name() + " call to missed command");
      return NULL;
    }
  if ((p=popen(ComUncom + " \"" + FileName + "\"", "r"))) 
    {
      file.open(IO_ReadOnly, p);
      ds.setDevice(&file);
    }
  else Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + QString(strerror(errno)));
  return p;
};

FILE* SBFileConv::open4In(const QString& FileName, QFile& file, QTextStream& ts)
{
  FILE* p=NULL;
  if(ComUncom=="NONE")
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + name() + " call to missed command");
      return NULL;
    }
  if ((p=popen(ComUncom + " \"" + FileName + "\"", "r"))) 
    {
      file.open(IO_ReadOnly, p);
      ts.setDevice(&file);
    }
  else Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + QString(strerror(errno)));
  return p;
};

/**Create compressed output.
 * Opens a pipe through compress command, opens corresponded QFile, returns opened pipe
 * or NULL if popen failed. 
 * \warning caller have to pclose returned value manually or call a SBFileConv::close().
 */
FILE* SBFileConv::open4Out(const QString& FileName, QFile& file, QDataStream& ds)
{
  FILE* p=NULL;
  if(ComCompr=="NONE")
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + name() + " call to missed command");
      return NULL;
    }
  if ((p=popen(ComCompr + " \"" + FileName + "\"", "w"))) 
    {
      file.open(IO_WriteOnly|IO_Truncate, p);
      ds.setDevice(&file);
    }
  else Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + QString(strerror(errno)));
  return p;
};

FILE* SBFileConv::open4Out(const QString& FileName, QFile& file, QTextStream& ts)
{
  FILE* p=NULL;
  if(ComCompr=="NONE")
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + name() + " call to missed command");
      return NULL;
    }
  if ((p=popen(ComCompr + " \"" + FileName + "\"", "w"))) 
    {
      file.open(IO_WriteOnly|IO_Truncate, p);
      ts.setDevice(&file);
    }
  else Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": " + QString(strerror(errno)));
  return p;
};

/**Closes compressed in/output.
 * Closes a pipe and corresponded QFile object.
 */
void SBFileConv::close(QFile& file, FILE*& p, QDataStream& ds) 
{
  file.close();
  if (p)
    {
      pclose(p);
      p = NULL;
    };
  ds.unsetDevice();
};

void SBFileConv::close(QFile& file, FILE*& p, QTextStream& ts) 
{
  file.close();
  if (p)
    {
      pclose(p);
      p = NULL;
    };
  ts.unsetDevice();
};

bool SBFileConv::check()
{
  if (ComCompr=="NONE" || ComUncom=="NONE") return TRUE; //impossible to check
  QString	str("QWERTY1234567890qwerty"), s2, tmpNam;

  QFile		f;
  QDataStream	s;
  FILE*		pipe=NULL;
  const char*	sFileTemplate="/tmp/SteelBreeze_XXXXXX";
  char*		tmpl=strdup(sFileTemplate);

  //  if (!(pipe=open4Out((tmpNam=tempnam(NULL, "Vasia")), f, s))) return FALSE;
  if (!(pipe=open4Out((tmpNam=mktemp(tmpl)), f, s))) 
    {
      free(tmpl);
      return FALSE;
    };
  s<<str;
  close(f, pipe, s);
  free(tmpl);

  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file `" + tmpNam + "' created");
  
  if (!(pipe=open4In(tmpNam, f, s))) return FALSE;
  s>>s2;
  close(f, pipe, s);

  if (unlink(tmpNam)==-1) Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": file `" + tmpNam + 
				     "' cannot be deleted: " + QString(strerror(errno)));
  else Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file `" + tmpNam + "' deleted");
  return str==s2;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBFCList implementation								*/
/*												*/
/*==============================================================================================*/
SBFCList::SBFCList() : SBNList()
{
  setAutoDelete(TRUE);
  ExtDict = new QDict<SBFileConv>(20);
  ExtDict-> setAutoDelete(FALSE);
  Default = "NONE";
  DefaultFC = NULL;
};

SBFCList::SBFCList(const SBFCList& L) : SBNList(L)
{
  setAutoDelete(TRUE);
  if (ExtDict) delete ExtDict;
  ExtDict= new QDict<SBFileConv>(*(L.ExtDict));
  ExtDict -> setAutoDelete(FALSE);
  setDefault((Default=L.Default));
};

SBFCList& SBFCList::operator=(const SBFCList& L)
{
  if (ExtDict) delete ExtDict;
  ExtDict= new QDict<SBFileConv>(L.count()+5);
  ExtDict -> setAutoDelete(FALSE);
  clear();

  SBNIterator it(L);
  while(it.current())
    {
      insert(new SBFileConv((SBFileConv&)*it.current()));
      ++it;
    };
  setAutoDelete(TRUE);
  setDefault((Default=L.Default));
  return *this;
};

SBFCList::~SBFCList() 
{
  if(ExtDict) delete ExtDict;
  DefaultFC=NULL;
};

SBFileConv* SBFCList::guessFileType(const QString& FileName)
{
  int	i=FileName.findRev('.');
  return ExtDict->find(i!=-1?FileName.right(FileName.length()-i-1):QString(""));
};

FILE* SBFCList::open4In(const QString& FileName, QFile& file, QDataStream& ds)
{
  SBFileConv	*W=guessFileType(FileName);
  if (W) return W->open4In(FileName, file, ds);
  // unknown format/uncompressed data:
  file.setName(FileName);
  file.open(IO_ReadOnly);
  ds.setDevice(&file);
  return NULL;
};

FILE* SBFCList::open4In(const QString& FileName, QFile& file, QTextStream& ts)
{
  SBFileConv	*W=guessFileType(FileName);
  if (W) return W->open4In(FileName, file, ts);
  // unknown format/uncompressed data:
  file.setName(FileName);
  file.open(IO_ReadOnly);
  ts.setDevice(&file);
  return NULL;
};

FILE* SBFCList::open4Out(const QString& FileName, QFile& file, QDataStream& ds)
{
  SBFileConv	*W=guessFileType(FileName);
  if (W) return W->open4Out(FileName, file, ds);
  // unknown format/uncompressed data:
  file.setName(FileName);
  file.open(IO_WriteOnly);
  ds.setDevice(&file);
  return NULL;
};

FILE* SBFCList::open4Out(const QString& FileName, QFile& file, QTextStream& ts)
{
  SBFileConv	*W=guessFileType(FileName);
  if (W) return W->open4Out(FileName, file, ts);
  // unknown format/uncompressed data:
  file.setName(FileName);
  file.open(IO_WriteOnly);
  ts.setDevice(&file);
  return NULL;
};

FILE* SBFCList::open4OutDflt(const QString& FileName, QFile& file, QDataStream& ds)
{
  if (DefaultFC) return DefaultFC->open4Out(FileName, file, ds);
  file.setName(FileName);
  file.open(IO_WriteOnly);
  ds.setDevice(&file);
  return NULL;
};

FILE* SBFCList::open4OutDflt(const QString& FileName, QFile& file, QTextStream& ts)
{
  if (DefaultFC) return DefaultFC->open4Out(FileName, file, ts);
  file.setName(FileName);
  file.open(IO_WriteOnly);
  ts.setDevice(&file);
  return NULL;
};

bool SBFCList::insert(const SBFileConv* F)
{
  if (!F) 
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": FC is NULL");
      return FALSE;
    }
  if (ExtDict->find(F->ext()))
    {
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": FC: " + F->name() + ", extension [" + 
		 F->ext() + "] already exist");
      return FALSE;
    }
  SBNList::inSort(F); 
  ExtDict->insert(F->ext(), F);
  return TRUE;
};

void SBFCList::setDefault(const QString& Default_)
{
  SBNamed *W=new SBNamed(Default_);
  int i = Default_!="NONE"&&W?find(W):-1;
  DefaultFC=i!=-1?(SBFileConv*)at(i):NULL;
  if (DefaultFC && DefaultFC->comCompr()=="NONE") DefaultFC=NULL;
  Default=DefaultFC?(const char*)DefaultFC->name():"NONE";
  delete W;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBFCList's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBFCList& L)
{
  return s << (const SBNList&)L << L.Default;
};

QDataStream &operator>>(QDataStream& s, SBFCList& L)
{
  s >> (SBNList&)L >> L.Default;

  if (L.ExtDict) delete L.ExtDict;
  L.ExtDict = new QDict<SBFileConv>(L.count()+5);
  L.ExtDict-> setAutoDelete(FALSE); 
  for (SBNamed *W=L.first(); W; W=L.next()) 
    L.ExtDict->insert(((SBFileConv*)W)->ext(), (SBFileConv*)W);
  L.setDefault(L.Default);
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSetup implementation									*/
/*												*/
/*==============================================================================================*/
SBSetUp::SBSetUp()
{
  SetDefaults();
  HaveToSave = TRUE;
  SetUpFName = QDir::homeDirPath() + "/.SteelBreeze-new";
  Log->write(SBLog::INF, SBLog::CONFIG, ClassName() + ": inited");
};

SBSetUp::~SBSetUp()
{
  //  if (Config)
  //    {
  //      delete Config;
  //      Config = NULL;
  //    }
};

void SBSetUp::LoadConfig()
{
  QFileInfo	setup_info(SetUpFName);
  QFile		setup_file(SetUpFName);
  QDataStream	s;
  SBVersion	ver;

  if (setup_info.exists())
    {
      Log->write(SBLog::DBG, SBLog::CONFIG, ClassName() + 
		 ": opening config \"" + SetUpFName + "\"");
      
      setup_file.open(IO_ReadOnly);
      s.setDevice(&setup_file);
      s >> ver;
      s.unsetDevice();
      setup_file.close();
      if (ver==Version)
	{
	  Log->write(SBLog::INF, SBLog::CONFIG, ClassName() + 
		     ": config is up to date, trying get data");
	  setup_file.open(IO_ReadOnly);
	  s.setDevice(&setup_file);
	  s >> *this;
	  s.unsetDevice();
	  setup_file.close();
	  Log->write(SBLog::INF, SBLog::CONFIG, ClassName() + ": done");
	}
      else if(ver<Version)
	{
	  Log->write(SBLog::INF, SBLog::CONFIG, ClassName() 
		     + ": config is out of date, trying upgrade");
	  setup_file.open(IO_ReadOnly);
	  s.setDevice(&setup_file);
	  PeekUpOldConfig(s);
	  s.unsetDevice();
	  setup_file.close();
	  Log->write(SBLog::INF, SBLog::CONFIG, ClassName() + ": done");
	  SaveConfig();	      
	}
      else
	{
	  Log->write(SBLog::ERR, SBLog::CONFIG, ClassName() + ": config new then me");
	  if (QMessageBox::warning
	      (0, Version.selfName(),
	       "Agrr, the version of your config file is newer then the\n"
	       "software. So, you can press \"OK\" and the default config\n"
	       "will be used, or exit and upgrade the soft.. In any case,\n"
	       "I cannot work with data's structure designed in the future\n",
	       "OK", "Exit and fix it by yourself"))
	    { 
	      HaveToSave = FALSE;
	      //QApplication::exit(20);
	      exit(1);
	    }
	  else
	    {
	      SetDefaults();
	      Log->write(SBLog::WRN, SBLog::CONFIG, ClassName() + 
			 ": configuration sets to default");
	      SaveConfig();
	    };

	};
    }
  else 
    {
      Log->write(SBLog::WRN, SBLog::CONFIG, ClassName() + ": config not found");
      SetDefaults();
      Log->write(SBLog::WRN, SBLog::CONFIG, ClassName() + ": configuration sets to default");
      SaveConfig();
    };
};

void SBSetUp::CheckDir(const QString Dir, bool have2write)
{
  QDir		d(Dir);
  QFileInfo	fi;
  QString	Explain;

  if (!d.exists())
    {
      if (d.mkdir(Dir))
	Log->write(SBLog::WRN, SBLog::CONFIG, ClassName() + 
		   ": directory has been created: \"" + Dir + "\"");
      else
	{
	  Explain = "it's impossible to create the directory \"" + Dir + "\"\n";
	  Explain+= "so, you have to exit and fix it by yourself..\n";
	  QMessageBox::critical(0, Version.selfName(), Explain, "OK");
	  exit(1);
	};
    };
  if (have2write)
    {
      fi.setFile(Dir);
      if (!fi.isWritable())
	{
	  Explain = "it's impossible to create the directory \"" + Dir + "\"\n";
	  Explain+= "so, you have to exit and fix it by yourself..\n";
	  QMessageBox::critical(0, Version.selfName(), Explain, "OK");
	  exit(1);
	};
    };  
};

void SBSetUp::CheckConfig()
{
  //-- check existence of the directories:
  CheckDir(path2Home());
  CheckDir(path2Log());
  CheckDir(path2Data());
  CheckDir(path2Compil(), FALSE);
  CheckDir(path2Stuff());
  CheckDir(path2StuffAplo());
  CheckDir(path2StuffObsStat());
  CheckDir(path2StuffObsStat() + "Projects");
  CheckDir(path2UIDump());
  CheckDir(path2Output());
  CheckDir((SolutionsDir.left(1)=="/")?SolutionsDir:HomeDir + "/" + SolutionsDir);
  CheckDir(path2Proj());
  CheckDir(path2Work());
  CheckDir(path2Import(), FALSE);
  CheckDir(path2ImpEOP(), FALSE);
  CheckDir(path2ImpITRF(), FALSE);
  CheckDir(path2ImpICRF(), FALSE);
  CheckDir(path2ImpOLoad(), FALSE);
  CheckDir(path2ImpMFiles(), FALSE);
  CheckDir(path2ImpAplo(), FALSE);
  CheckDir(path2TimeSeries(), TRUE);
};

void SBSetUp::LoadStuff()
{
  QFile		f;
  QDataStream	s;

  //  Config = new SBConfig;
  f.setName(modelsFileName());
  if (f.exists())
    {
      f.open(IO_ReadOnly);
      s.setDevice(&f);
      s >> Config;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": general models' config has been loaded");
    }
  else
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": cannot load general models' config");
      Config = DefaultConfig;
      Log->write(SBLog::INF, SBLog::IO, ClassName() + 
		 ": general models' config has been set to default");
    };

  if (Institutions) 
    delete Institutions;
  Institutions = new SBInstitutionList;
  f.setName(InstitutionsFileName());
  if (f.exists())
    {
      f.open(IO_ReadOnly);
      s.setDevice(&f);
      s >> *Institutions;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": list of institutions has been loaded");
    }
  else
    {
      Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": cannot load a list of institutions");
      for (int i=0; (uint)i<sizeof(InstitutionsDefault)/sizeof(SBInstitution); i++)
	Institutions -> insert(InstitutionsDefault[i].name(), InstitutionsDefault[i].descr());
      Log->write(SBLog::INF, SBLog::IO, ClassName() + 
		 ": list of institutions has been set to default");
    };

  // check EOP series:
  //  SBEOP *W = new SBEOP (EOPs);
  //  W->checkSeries();
  //  delete W;

  // check VLBI database set:
  SBVLBISet *V = new SBVLBISet;
  f.setName(VLBIfileName());
  if (!f.exists())
    {
      f.open(IO_WriteOnly);
      s.setDevice(&f);
      s << *V;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + VLBIfileName() +
		 " has been touched");
    };
  delete V;

  // check aux file:
  f.setName(masterFileName());
  if (!f.exists())
    {
      SBMaster *M = new SBMaster;
      M->checkNewFiles(path2ImpMFiles());
      f.open(IO_WriteOnly);
      s.setDevice(&f);
      s << *M;
      f.close();
      s.unsetDevice();

      delete M;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + masterFileName() +
		 " has been touched");
    };
};
 
void SBSetUp::SaveStuff()
{
  QFile		f;
  QDataStream	s;

  if (Config.isChanged())
    {
      f.setName(modelsFileName());
      f.open(IO_WriteOnly);
      s.setDevice(&f);
      s << Config;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": general models' config has been saved");
    };

  if (Institutions->isChanged())
    {
      f.setName(InstitutionsFileName());
      f.open(IO_WriteOnly);
      s.setDevice(&f);
      s << *Institutions;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": list of institutions has been saved");
    };
};

void SBSetUp::SaveConfig()
{
  QFile		setup_file(SetUpFName);
  setup_file   .open(IO_WriteOnly);
  QDataStream	s(&setup_file);
  s << *this;
  setup_file.close();
  s.unsetDevice();
  Log->write(SBLog::INF, SBLog::CONFIG, ClassName() + ": config has been saved");
};

void SBSetUp::SetDefaults()
{
  // Version 2.0.1:
  /* directories: */
  HomeDir		= QDir::homeDirPath() + "/SteelBreeze";
  DataDir		= "Data";
  CompileDir		= "Compile";
  StuffDir		= "Stuff";
  OutputDir		= "Output";
  SolutionsDir		= "Solutions";
  ProjectDir		= "Projects";
  LogDir		= "Log";
  WorkDir		= "/tmp";
  ImportDir		= "Import";
  ImportEOPDir		= "Import/EOP";
  ImportITRFDir		= "Import/ITRF";
  ImportICRFDir		= "Import/ICRF";
  ImportOLoadDir	= "Import/OLoad";
  ImportMFilesDir	= "Import/master";
  ImportAploDir		= "Import/aplo";
  /* file names: */
  Stations		= "itrf94";
  Sources		= "icrf95";
  EOPs			= "eops97c04";
  Ephem			= "lede403";
  Project		= "unkn";

  // Version 2.0.2:
  /*        SBLogger:                      */
  LogStoreInFile	= TRUE;
  LogTimeLabel		= TRUE;
  LogFileName		= "SteelBreeze.log";
  LogFacilities[0]	= 0xFFFFFFFFL;
  LogFacilities[1]	= 0xFFFFFFFFL;
  LogFacilities[2]	= 0xFFFFFFFFL;
#ifdef DEBUG
  LogFacilities[3]	= 0xFFFFFFFFL;
#else
  LogFacilities[3]	= 0x00000000L;
#endif
  LogCapacity		= 400;
  /*     SBCentarlPanel:                   */
  CentralPanelPosition.setX(50);
  CentralPanelPosition.setY(20);
  CentralPanelSize.setWidth(700);
  CentralPanelSize.setHeight(560);
  DefaultFont=QApplication::font();
  GUI_Style=S_Default;
  /*     Units:                   */
  Units			= U_SEC;
  
  if (!FCL.count())
    {
      FCL.insert(new SBFileConv("GZIP", "gzip -c >", "gzip -dc", "gz"));
      FCL.insert(new SBFileConv("BZIP2", "bzip2 -c >", "bzip2 -dc", "bz2"));
      FCL.insert(new SBFileConv("Compress", "compress -c >", "compress -dc", "Z"));
    };

  if (Institutions) delete Institutions;
  Institutions = new SBInstitutionList;
  for (int i=0; (uint)i<sizeof(InstitutionsDefault)/sizeof(SBInstitution); i++)
    Institutions -> insert(InstitutionsDefault[i].name(), InstitutionsDefault[i].descr());
};

void SBSetUp::UpdateLogger()
{
  Log->setLogFileName(path2Log() + LogFileName);
  Log->setCapacity(LogCapacity);
  Log->setStoreInFile(LogStoreInFile);
  Log->setTimeLabel(LogTimeLabel);
  Log->setLogFacilities(SBLog::ERR, LogFacilities[0]);
  Log->setLogFacilities(SBLog::WRN, LogFacilities[1]);
  Log->setLogFacilities(SBLog::INF, LogFacilities[2]);
  Log->setLogFacilities(SBLog::DBG, LogFacilities[3]);
};

const QString SBSetUp::path2Data() const
{
  return (LogDir.left(1)=="/"?DataDir:HomeDir + "/" + DataDir) + "/";
};

const QString SBSetUp::path2Log() const
{
  return (LogDir.left(1)=="/"?LogDir:HomeDir + "/" + LogDir) + "/";
};

const QString SBSetUp::path2Compil() const
{
  return (CompileDir.left(1)=="/"?CompileDir:HomeDir + "/" + CompileDir) + "/";
};

const QString SBSetUp::path2Stuff() const
{
  return (StuffDir.left(1)=="/"?StuffDir:HomeDir + "/" + StuffDir) + "/";
};

const QString SBSetUp::path2UIDump() const
{
  return path2Stuff() + "UserInfoDump";
};

const QString SBSetUp::path2Output() const
{
  return (OutputDir.left(1)=="/"?OutputDir:HomeDir + "/" + OutputDir) + "/";
};

const QString SBSetUp::path2Sols(const QString& PrjName_) const
{    
  return ((SolutionsDir.left(1)=="/"?SolutionsDir:HomeDir + "/" + SolutionsDir) + "/" + PrjName_) + "/";
};

const QString SBSetUp::path2Proj() const
{
  return (ProjectDir.left(1)=="/"?ProjectDir:HomeDir + "/" + ProjectDir) + "/";
};

const QString SBSetUp::path2Work() const
{
  return (WorkDir.left(1)=="/"?WorkDir:HomeDir + "/" + WorkDir) + "/";
};

const QString SBSetUp::path2Import() const
{
  return (ImportDir.left(1)=="/"?ImportDir:HomeDir + "/" + ImportDir) + "/";
};

const QString SBSetUp::path2ImpEOP() const
{
  return (ImportEOPDir.left(1)=="/"?ImportEOPDir:HomeDir + "/" + ImportEOPDir) + "/";
};

const QString SBSetUp::path2ImpITRF() const
{
  return (ImportITRFDir.left(1)=="/"?ImportITRFDir:HomeDir + "/" + ImportITRFDir) + "/";
};

const QString SBSetUp::path2ImpICRF() const
{
  return (ImportICRFDir.left(1)=="/"?ImportICRFDir:HomeDir + "/" + ImportICRFDir) + "/";
};

const QString SBSetUp::path2ImpOLoad() const
{
  return (ImportOLoadDir.left(1)=="/"?ImportOLoadDir:HomeDir + "/" + ImportOLoadDir) + "/";
};

const QString SBSetUp::path2ImpMFiles() const
{
  return (ImportMFilesDir.left(1)=="/"?ImportMFilesDir:HomeDir + "/" + ImportMFilesDir) + "/";
};

const QString SBSetUp::path2ImpAplo() const
{
  return (ImportAploDir.left(1)=="/"?ImportAploDir:HomeDir + "/" + ImportAploDir) + "/";
};

SBSetUp::SBSetUp(const SBSetUp& SetUp_)
{
  *this = SetUp_;
};

const SBSetUp& SBSetUp::operator=(const SBSetUp& SetUp_)
{
  HomeDir		= SetUp_.HomeDir;
  DataDir		= SetUp_.DataDir;
  CompileDir		= SetUp_.CompileDir;
  StuffDir		= SetUp_.StuffDir;
  OutputDir		= SetUp_.OutputDir;
  SolutionsDir		= SetUp_.SolutionsDir;
  ProjectDir		= SetUp_.ProjectDir;
  LogDir		= SetUp_.LogDir;
  WorkDir		= SetUp_.WorkDir;
  ImportDir		= SetUp_.ImportDir;
  ImportEOPDir		= SetUp_.ImportEOPDir;
  ImportITRFDir		= SetUp_.ImportITRFDir;
  ImportICRFDir		= SetUp_.ImportICRFDir;
  ImportOLoadDir	= SetUp_.ImportOLoadDir;
  ImportMFilesDir	= SetUp_.ImportMFilesDir;
  ImportAploDir		= SetUp_.ImportAploDir;
  Stations		= SetUp_.Stations;
  Sources		= SetUp_.Sources;
  EOPs			= SetUp_.EOPs;
  Ephem			= SetUp_.Ephem;
  Project		= SetUp_.Project;
  LogStoreInFile	= SetUp_.LogStoreInFile;
  LogTimeLabel		= SetUp_.LogTimeLabel;
  LogFileName		= SetUp_.LogFileName;
  LogFacilities[0]	= SetUp_.LogFacilities[0];
  LogFacilities[1]	= SetUp_.LogFacilities[1];
  LogFacilities[2]	= SetUp_.LogFacilities[2];
  LogFacilities[3]	= SetUp_.LogFacilities[3];
  LogCapacity		= SetUp_.LogCapacity;
  CentralPanelPosition	= SetUp_.CentralPanelPosition;
  CentralPanelSize	= SetUp_.CentralPanelSize;
  DefaultFont		= SetUp_.DefaultFont;
  GUI_Style		= SetUp_.GUI_Style;
  Units			= SetUp_.Units;
  FCL			= SetUp_.FCL;
  return *this;
};

QDataStream &operator<<(QDataStream& s, const SBSetUp &SetUp_)
{
  s <<  Version
    <<  SetUp_.HomeDir
    <<  SetUp_.DataDir
    <<  SetUp_.CompileDir
    <<  SetUp_.StuffDir
    <<  SetUp_.OutputDir
    <<  SetUp_.SolutionsDir
    <<  SetUp_.ProjectDir
    <<  SetUp_.LogDir
    <<  SetUp_.WorkDir
    <<  SetUp_.ImportDir
    <<  SetUp_.ImportEOPDir
    <<  SetUp_.ImportITRFDir
    <<  SetUp_.ImportICRFDir
    <<  SetUp_.ImportOLoadDir
    <<  SetUp_.ImportMFilesDir
    <<  SetUp_.ImportAploDir
    <<  SetUp_.Stations
    <<  SetUp_.Sources
    <<  SetUp_.EOPs
    <<  SetUp_.Ephem
    <<  SetUp_.Project

    //------------
    <<  SetUp_.LogStoreInFile
    <<  SetUp_.LogTimeLabel
    <<  SetUp_.LogFileName
    <<  SetUp_.LogFacilities[0]
    <<  SetUp_.LogFacilities[1]
    <<  SetUp_.LogFacilities[2]
    <<  SetUp_.LogFacilities[3]
    <<  SetUp_.LogCapacity
    <<  SetUp_.CentralPanelPosition
    <<  SetUp_.CentralPanelSize
    <<  SetUp_.DefaultFont
    <<  (uint)SetUp_.GUI_Style
    <<  (uint)SetUp_.Units
    <<	SetUp_.FCL
    ;

  return  s;
};

QDataStream &operator>>(QDataStream& s, SBSetUp& SetUp_)
{
  SBVersion	v;
  
  s >>  v
    >>  SetUp_.HomeDir
    >>  SetUp_.DataDir
    >>  SetUp_.CompileDir
    >>  SetUp_.StuffDir
    >>  SetUp_.OutputDir
    >>  SetUp_.SolutionsDir
    >>  SetUp_.ProjectDir
    >>  SetUp_.LogDir
    >>  SetUp_.WorkDir
    >>  SetUp_.ImportDir
    >>  SetUp_.ImportEOPDir
    >>  SetUp_.ImportITRFDir
    >>  SetUp_.ImportICRFDir
    >>  SetUp_.ImportOLoadDir
    >>  SetUp_.ImportMFilesDir
    >>  SetUp_.ImportAploDir
    
    >>  SetUp_.Stations
    >>  SetUp_.Sources
    >>  SetUp_.EOPs
    >>  SetUp_.Ephem
    >>  SetUp_.Project

    //------------
    >>  SetUp_.LogStoreInFile
    >>  SetUp_.LogTimeLabel
    >>  SetUp_.LogFileName
    >>  SetUp_.LogFacilities[0]
    >>  SetUp_.LogFacilities[1]
    >>  SetUp_.LogFacilities[2]
    >>  SetUp_.LogFacilities[3]
    >>  SetUp_.LogCapacity
    >>  SetUp_.CentralPanelPosition
    >>  SetUp_.CentralPanelSize
    >>  SetUp_.DefaultFont
    >>  (uint&)SetUp_.GUI_Style
    >>  (uint&)SetUp_.Units
    >>	SetUp_.FCL
    ;
  
  return s;
};

void SBSetUp::PeekUpOldConfig(QDataStream& s)
{

  SetDefaults();
  SBVersion v;
  s >>  v;
  
  // Version 2.0.1 do know about:
  if (v>=SBVersion(2,0,1))
    {
      s >>  HomeDir;
      s >>  DataDir;
      s >>  CompileDir;
      s >>  StuffDir;
      s >>  OutputDir;
      s >>  SolutionsDir;
      s >>  ProjectDir;
      s >>  LogDir;
      s >>  WorkDir;
      s >>  ImportDir;
      s >>  ImportEOPDir;
      s >>  ImportITRFDir;
      s >>  ImportICRFDir;
      s >>  ImportOLoadDir;
      s >>  ImportMFilesDir;
      s >>  ImportAploDir;
  
      s >>  Stations;
      s >>  Sources;
      s >>  EOPs;
      s >>  Ephem;
      s >>  Project;
    };

  // Version 2.0.2 do know about:
  if (v>=SBVersion(2,0,2))
    {
      s >>  LogStoreInFile;
      s >>  LogTimeLabel;
      s >>  LogFileName;
      s >>  LogFacilities[0];
      s >>  LogFacilities[1];
      s >>  LogFacilities[2];
      s >>  LogFacilities[3];
      s >>  LogCapacity;
      s >>  CentralPanelPosition;
      s >>  CentralPanelSize;
      s >>  DefaultFont;
      s >>  (uint&)GUI_Style;
      s >>  (uint&)Units;
      s >>  FCL;
    };

};

SBEOP* SBSetUp::loadEOP(SBEOP* W)
{
  QFile f(EOPfileName());
  if (f.open(IO_ReadOnly))
    {
      QDataStream s(&f);
      if (!W) W=new SBEOP;
      s >> *W;
      if (f.status()!=IO_Ok)
	{
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": loading of the \"" + 
		     EOPfileName() + "\" failed");
	  if (W) delete W;
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + EOPfileName() + " has been loaded");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not read " + EOPfileName());
  return W;
};

void SBSetUp::saveEOP(SBEOP* W)
{
  QFile f(EOPfileName());
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << *W;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + EOPfileName() + " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + EOPfileName());
};

//
SB_CRF* SBSetUp::loadCRF(SB_CRF* W)
{
  QFile f(CRFfileName());
  if (f.open(IO_ReadOnly))
    {
      QDataStream s(&f);
      if (!W) W=new SB_CRF();
      s >> *W;
      if (f.status()!=IO_Ok)
	{
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": loading of the \"" + 
		     CRFfileName() + "\" failed");
	  if (W) delete W;
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName()+": file "+CRFfileName()+" has been loaded");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not read " + CRFfileName());
  return W;
};

void SBSetUp::saveCRF(SB_CRF* W)
{
  QFile f(CRFfileName());
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << *W;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName()+": file " + CRFfileName() + " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + CRFfileName());
};


SB_TRF* SBSetUp::loadTRF(SB_TRF* W)
{
  QFile f(TRFfileName());
  if (f.open(IO_ReadOnly))
    {
      QDataStream s(&f);
      if (!W) W=new SB_TRF;
      s >> *W;
      if (f.status()!=IO_Ok)
	{
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": loading of the \"" + 
		     TRFfileName() + "\" failed");
	  if (W) delete W;
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName()+": file "+TRFfileName()+" has been loaded");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not read " + TRFfileName());
  return W;
};

void SBSetUp::saveTRF(SB_TRF* W)
{
  QFile f(TRFfileName());
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << *W;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName()+": file "+TRFfileName()+" has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + TRFfileName());
};

SBVLBISet* SBSetUp::loadVLBI(SBVLBISet* W)
{
  QFile f(VLBIfileName());
  if (f.open(IO_ReadOnly))
    {
      QDataStream s(&f);
      if (!W) W=new SBVLBISet;
      s >> *W;
      if (f.status()!=IO_Ok)
	{
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": loading of the \"" + 
		     VLBIfileName() + "\" failed");
	  if (W) delete W;
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName()+": file "+VLBIfileName()+" has been loaded");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not read " + VLBIfileName());
  return W;
};

void SBSetUp::saveVLBI(SBVLBISet* W)
{
  QFile f(VLBIfileName());
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << *W;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName()+": file "+VLBIfileName()+" has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + VLBIfileName());
};

SBProject* SBSetUp::loadProject(SBProject* P)
{
  QFile f(ProjectFileName());
  if (f.open(IO_ReadOnly))
    {
      QDataStream s(&f);
      if (!P) P=new SBProject("");
      s >> *P;
      if (f.status()!=IO_Ok)
	{
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": loading of the \"" + 
		     ProjectFileName() + "\" failed");
	  if (P) delete P;
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file \"" + ProjectFileName() + 
		 "\" has been loaded");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not read " + ProjectFileName());
  return P;
};

void SBSetUp::saveProject(SBProject* P)
{
  QFile f(ProjectFileName());
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << *P;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file \"" + ProjectFileName() + 
		 "\" has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + ProjectFileName());
};

SBAploEphem* SBSetUp::loadAploEphem(SBAploEphem* Aplo)
{
  QFile f(AploFileName());
  if (f.open(IO_ReadOnly))
    {
      QDataStream s(&f);
      if (!Aplo) Aplo = new SBAploEphem;
      s >> *Aplo;
      if (f.status()!=IO_Ok)
	{
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": loading of the \"" + 
		     AploFileName() + "\" failed");
	  if (Aplo) delete Aplo;
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file \"" + AploFileName() + 
		 "\" has been loaded");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not read " + AploFileName());
  return Aplo;
};

void SBSetUp::saveAploEphem(SBAploEphem* Aplo)
{
  QFile f(AploFileName());
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << *Aplo;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file \"" + AploFileName() + 
		 "\" has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + AploFileName());
};

SBObsVLBIStatistics* SBSetUp::loadObsVLBIStatistics(bool IsGlb)
{
  SBObsVLBIStatistics	*Stats=NULL;
  QFile f(ObsStatFileName(IsGlb));
  if (f.open(IO_ReadOnly))
    {
      QDataStream s(&f);
      Stats = new SBObsVLBIStatistics("UNNAMED");
      s >> *Stats;
      if (f.status()!=IO_Ok)
	{
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": loading of the \"" + 
		     ObsStatFileName(IsGlb) + "\" failed");
	  if (Stats) delete Stats;
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file \"" + ObsStatFileName(IsGlb) + 
		 "\" has been loaded");
    }
  else    
    Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": could not read " + ObsStatFileName(IsGlb));
  return Stats;
};

void SBSetUp::saveObsVLBIStatistics(SBObsVLBIStatistics* Stats)
{
  QFile f(ObsStatFileName(Stats->isGlobal()));
  if (f.open(IO_WriteOnly))
    {
      QDataStream s(&f);
      s << *Stats;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file \"" + ObsStatFileName(Stats->isGlobal()) + 
		 "\" has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + ObsStatFileName(Stats->isGlobal()));
};
/*==============================================================================================*/
