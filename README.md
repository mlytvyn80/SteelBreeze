
   # SteelBreeze

   ## About this file
   
    This file describes the SteelBreeze version: 	2.0.2;
               modified                      on:  Apr 20 1999;

   ## Introduction
   
   ## Availability
   
   ### Original archives
   
   

   1. FTP: You can download the software from the following ftp sites:
	ftp://ftp.mao.kiev.ua/pub/SteelBreeze,
	ftp://mao.gluk.apc.org/pub/SteelBreeze
   2. WWW: the SteelBreeze's home pages should looks like these
	http://www.mao.kiev.ua/SteelBreeze,
	http://www2.mao.kiev.ua/SteelBreeze
   3. Sourceforge page: http://steelbreeze.sourceforge.net/


   ## Installation

   See the file INSTALL for general instructions how to compile and install
the software using GNU's configure.

   This software requires Qt library. You can get the library and information
how to install it from the http://www.troll.no. Usually, it is assumed that the
environment variable `QTDIR' is set and pointing into root Qt directory. If it
is not set, you can tell to the configure script where it is located using 
`--with-qt`, `--with-qt-includes` and `--with-qt-libraries` options.
   This is a beta release and configure's default options are `--enable-debug`
(which enables debug info in the executable) and `--disable-optimization` (turns
off all optimization flags); these options are useful for developing. However,
if you want to make a faster executable and to strip debug information, you can 
configure it with the `--enable-optimization` `--disable-debug` options, like
this:

	`./configure [other options] --enable-optimization --disable-debug [..]`

   You can specify the `--prefix` option to change the default path to install
the software. By the default it is installed into `/usr/local`, but if you
have not permission to write to it, you must say another writable for you path
(for example, `--prefix=$HOME` (and append `:~/bin` to your $PATH)).



   ## Supported platforms

   Currently, SteelBreeze is known to build and execute on the following
platforms:

OS      | Version       | Arch/CPU      |  Compiler    |  GNU C Library
--------|---------------|---------------|--------------|--------------
FreeBSD | 2.2.2-RELEASE | PC/Pentium    |  gcc-2.7.2.1 |
Linux   | 2.0.35        | PC/Pentium    |  egcs-1.0.2  |  libc-5.4.38
--------|---------------|---------------|--------------|--------------
        | 2.2.6         | PC/Pentium II |  egcs-1.1.2  |  glibc-2.1
Solaris | 2.5.1         | SS-20/SPARC   |  egcs-1.1.2  |
--------|---------------|---------------|--------------|---------------

If you have experience on other platforms, please, let me know about it.


   ## Features


   ## Documentation


   ## Legal notes

   SteelBreeze is distributed under the GNU Public License. For details
read the 'COPYING' file and check the URL http://www.gnu.org.


