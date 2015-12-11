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

#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include "SbIMainWin.H"

//	saved actions for signals:
//	RT errors:
struct sigaction saveSIGFPE;
struct sigaction saveSIGILL;
struct sigaction saveSIGSEGV;
struct sigaction saveSIGABRT;
struct sigaction saveSIGBUS;

//	termination signals:
struct sigaction saveSIGINT;
struct sigaction saveSIGHUP;
struct sigaction saveSIGTERM;

//	default:
struct sigaction saveSIGDFL;

const char* options="dqa";

void handlerSIGs(int signum)
{
  QString Str("Signal handler: catched the \"");
  Str+=strsignal(signum);
  Str+="\" signal";

  QMessageBox::critical(0, Version.selfName() + "-" + Version.toString() + ": signal handler",
			QString("An internal error occurred.\n\n") + Str +
			".\n\nThe application will now exit.");

  std::cout << Str << "; saving the Log...\n";
  Log->write(SBLog::ERR, SBLog::INTERFACE, Str);
  delete Log;
  
  // special handling:
  switch(signum)
    {
      //run-time errors:
    case SIGFPE:  sigaction(signum, &saveSIGFPE, NULL); break;
    case SIGILL:  sigaction(signum, &saveSIGILL, NULL); break;
    case SIGSEGV: sigaction(signum, &saveSIGSEGV,NULL); break;
    case SIGABRT: sigaction(signum, &saveSIGABRT,NULL); break;
    case SIGBUS:  sigaction(signum, &saveSIGBUS, NULL); break;
      
      //interups:
    case SIGINT:  sigaction(signum, &saveSIGINT, NULL); break;
    case SIGHUP:  sigaction(signum, &saveSIGHUP, NULL); break;
    case SIGTERM: sigaction(signum, &saveSIGTERM,NULL); break;
    default: 
      std::cout << "got an unexpected signal, setting handler to default.\n"; 
      sigaction(signum, &saveSIGDFL, NULL);
      break;
    };

  std::cout << "Signal handler: processing default handler..\n";
  raise(signum);
};


/*==============================================================================================*/
/**\mainpage SteelBreeze Reference Documentation.
 * This is the main page of the SB docs.
 * (it is just a template.)
 *
 * \section intro Introduction.
 * Intro to VLBI, SB design, etc..
 *
 * \section install Installation.
 * How to install the software.
 *
 * \subsection step1 Step 1: Opening the box.
 * Getting and opening the box.
 *
 */
int main(int argc, char** argv)
{
  int			RC;
  // signal manipulations:
  struct sigaction	act;
  int			c;
  bool			isNeedIntHanflers=TRUE;

  saveSIGDFL.sa_handler=SIG_DFL;
  sigemptyset(&saveSIGDFL.sa_mask);
  saveSIGDFL.sa_flags = 0;
  
  // main body:
  QApplication::setDesktopSettingsAware(TRUE);
  QApplication Application(argc, argv);
  while ((c=getopt(argc, argv, options))!=-1)
    switch (c)
      {
      case 'd': // disable interupt handlers:
	isNeedIntHanflers=FALSE;
  	break;
      case 'q':
      case 'a':
      case '?':
	break;
      };
  if (isNeedIntHanflers)
    {
      act.sa_handler = handlerSIGs;
      sigemptyset(&act.sa_mask);
      act.sa_flags = 0;
      sigaction(SIGFPE, &act, &saveSIGFPE);
      sigaction(SIGILL, &act, &saveSIGILL);
      sigaction(SIGSEGV,&act, &saveSIGSEGV);
      sigaction(SIGABRT,&act, &saveSIGABRT);
      sigaction(SIGBUS, &act, &saveSIGBUS);
      sigaction(SIGINT, &act, &saveSIGINT);
      sigaction(SIGHUP, &act, &saveSIGHUP);
      sigaction(SIGTERM,&act, &saveSIGTERM);
    };
  
  //  try
  //    {
  Application.setMainWidget(MainWin=new SBMainWindow);
  Application.mainWidget()->show();
  RC = Application.exec();
  //    }
  //  catch (...)
  //    {
  //      std::cout << "Got an exeption\n";
  //    };
  delete MainWin;
  return RC;
};
/*==============================================================================================*/
