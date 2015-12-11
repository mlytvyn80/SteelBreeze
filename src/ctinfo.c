/*
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 1998-2002 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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
 * Overview:	guess compile time info. 
 *
 * Now we use the follows constants:
 *
 * compiler's inner constants:
 *	SB_COMP_NAME: 		name of the compiler;
 *	SB_COMP_VERSION: 	version of the compiler;
 *	SB_COMP_DATE: 		date when the binnary has been assembled;
 *	SB_COMP_TIME: 		time when the binnary has been assembled;
 *
 * guessed values:
 *	SB_COMP_OS:		version of the operation system, result of the uname(2);
 *	SB_COMP_HOST_NAME:	host name (should not contain dots (`.'));
 *	SB_COMP_DOMAIN_NAME:	domain name (if it is not equal to `(none)' and
 *				contain at least one dot (`.'));
 *	SB_COMP_LOGIN_NAME:	user name (i.e., name of the login);
 *	SB_COMP_USER_NAME:	user name in real life (if we can recognize it);
 *
 * user defined constants (ARGV[1..3]):
 *	SB_VER_MAJOR:		major version of the software;
 *	SB_VER_MINOR:		minor version of the software;
 *	SB_VER_PATCHLEVEL:	teeny version of the software;
 *
 */

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

char* VerMajorDefault = "2";
char* VerMinorDefault = "0";
char* VerTiny_Default = "0";


int main (int ARGC, char *ARGV[])
{
    char		buff[256];
    char*		OSName=NULL;
    char*		HostName=NULL;
    char*		DomainName=NULL;
    char*		LoginName=NULL;
    char*		UserName=NULL;
    char*		VerMajor=NULL;
    char*		VerMinor=NULL;
    char*		VerTiny_=NULL;
    struct utsname 	UTS;
    struct passwd 	*pw=NULL;
    uid_t 		UID;
        
    if (ARGC<4) 
    {
	VerMajor = malloc(strlen(VerMajorDefault)+1);
	VerMinor = malloc(strlen(VerMinorDefault)+1);
	VerTiny_ = malloc(strlen(VerTiny_Default)+1);
	strcpy(VerMajor, VerMajorDefault);
	strcpy(VerMinor, VerMinorDefault);
	strcpy(VerTiny_, VerTiny_Default);
    }
    else
    {
	VerMajor = malloc(strlen(ARGV[1])+1);
	VerMinor = malloc(strlen(ARGV[2])+1);
	VerTiny_ = malloc(strlen(ARGV[3])+1);
	strcpy(VerMajor, ARGV[1]);
	strcpy(VerMinor, ARGV[2]);
	strcpy(VerTiny_, ARGV[3]);
    }

    /* guess OS name: */    
    if (uname(&UTS)==-1)
    {
	perror("uname: ");
	return -1;
    }
    OSName = malloc(strlen(UTS.sysname) + 1 + strlen(UTS.nodename) + 1 +
		    strlen(UTS.release) + 1 + strlen(UTS.version) + 1 + 
		    strlen(UTS.machine) + 1);
    sprintf(OSName,"%s %s %s %s %s", 
	    UTS.sysname, UTS.nodename, UTS.version, UTS.release, UTS.machine);

    /* guess host name: */
    if (gethostname(buff, sizeof(buff))==-1)
    {
	perror("gethostname: ");
	return -1;
    }
    HostName = malloc(strlen(buff)+1);
    strcpy(HostName, buff);

    /* If hostname contain at least one dot, assume this is F.Q.D.N. host name
    *  and do not ask about domainname:
    */
    if (!strstr(HostName, "."))
    {
	/* guess domain name: */
        if (getdomainname(buff, sizeof(buff))==-1)
        {
    	    perror("getdomainname: ");
	    return -1;
	}
        if (!strstr(buff, "(none)"))
	{
	    DomainName = malloc(strlen(buff)+1);
	    strcpy(DomainName, buff);
	}
    }
    /* guess login name: */
    UID = geteuid();
    pw = getpwuid(UID);
    if (!pw)
    {
	perror("getpwuid: ");
	return -1;
    }
    LoginName = malloc(strlen(pw->pw_name)+1);
    strcpy(LoginName, pw->pw_name);
    
    /* guess user name: */
    UserName = malloc(strlen(pw->pw_gecos)+1);
    if (!strtok(pw->pw_gecos, ",")) strcpy(UserName, pw->pw_gecos);
    else strcpy(UserName, strtok(pw->pw_gecos, ","));

    /* make output */

    printf("/*\n"
	   " * This file is part of SteelBreeze.\n"
	   " *\n"
	   " * SteelBreeze, the geodetic VLBI data analysing software.\n"
	   " * Copyright (C) 1998-2002 Sergei Bolotin, MAO NASU, Kiev, Ukraine.\n"
	   " *\n"
	   " * SteelBreeze is free software; you can redistribute it and/or modify\n"
	   " * it under the terms of the GNU General Public License as published by\n"
	   " * the Free Software Foundation; either version 2 of the License, or\n"
	   " * (at your option) any later version.\n"
	   " *\n"
	   " * SteelBreeze is distributed in the hope that it will be useful,\n"
	   " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	   " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	   " * GNU General Public License for more details.\n"
	   " *\n"
	   " * You should have received a copy of the GNU General Public License\n"
	   " * along with SteelBreeze; if not, write to the Free Software Foundation,\n"
	   " * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.\n"
	   " *\n"
	   " * Overview:	keeps determined compile time info.\n *\n */\n\n");
#ifdef __GNUC__
    printf("#define SB_COMP_NAME \"gcc\"\n");
    printf("#define SB_COMP_VERSION __VERSION__\n");
#else
    printf("#define SB_COMP_NAME \"unknown\"\n");
    printf("#define SB_COMP_VERSION \"unknown\"\n");
#endif
    printf("#define SB_COMP_DATE __DATE__\n");
    printf("#define SB_COMP_TIME __TIME__\n");

    if (OSName) 
    {
	printf("#define SB_COMP_OS \"%s\"\n", OSName);
	free(OSName);
    }
    if (DomainName) 
    {
	printf("#define SB_COMP_DOMAIN_NAME \"%s\"\n", DomainName);
	free(DomainName);
    }
    if (HostName) 
    {
	printf("#define SB_COMP_HOST_NAME \"%s\"\n", HostName);
	free(HostName);
    }
    if (LoginName) 
    {
	printf("#define SB_COMP_LOGIN_NAME \"%s\"\n", LoginName);
	free(LoginName);
    }
    if (UserName) 
    {
	printf("#define SB_COMP_USER_NAME \"%s\"\n", UserName);
	free(UserName);
    }
    if (VerMajor) 
    {
	printf("#define SB_VER_MAJOR %s\n", VerMajor);
	free(VerMajor);
    }
    if (VerMinor) 
    {
	printf("#define SB_VER_MINOR %s\n", VerMinor);
	free(VerMinor);
    }
    if (VerTiny_) 
    {
	printf("#define SB_VER_PATCHLEVEL %s\n", VerTiny_);
	free(VerTiny_);
    }

    return 0;
}
