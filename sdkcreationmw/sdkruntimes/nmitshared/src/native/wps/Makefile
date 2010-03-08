#
# -*- Makefile -*-
#
# $Id: Makefile,v 1.8 2001/02/06 00:29:09 slava Exp $
#
# Makefile for building client WPS JNI wrapper dll's
#
# Environment:
#	JAVA_HOME - points to the top of the JDK tree
#	WPSC_HOME - points to the top of the WPS client source tree
#
# Targets:
#	all     - builds the libraries (typically same as 'release')
#	debug	- buld debug versions of the libraries
#	release	- buld release versions of the libraries
#	clean	- remove intermediate files (but not the libraries)
#	toolkit	- copy release libraries to the toolkit results area

CHECK_TOOLKIT_RESULTS = \
    if [ "${TOOLKIT_RESULTS}" = "" ] ; then \
      TOOLKIT_RESULTS="${TOOLKIT_HOME}/results" ; export TOOLKIT_RESULTS; \
    fi

INVOKE_MAKE= \
  @if [ "`uname`" = "SunOS" ] ; then \
    $(CHECK_TOOLKIT_RESULTS) ; \
    make -f Makefile.common OS="solaris" PLATFORM="unix" CPU=`uname -p` $@ ; \
  elif [ "`uname -s`" = "Linux" -a "`uname -m | grep i.86`" != "" ] ; then \
    $(CHECK_TOOLKIT_RESULTS) ; \
    make -f Makefile.common OS="linux" PLATFORM="unix" CPU="i386" $@; \
  elif [ "`uname | grep -i ^win`" != "" ] ; then \
    make -f Makefile.win32 OS="win32" PLATFORM="win32" CPU="i386" $@; \
  else \
    echo "Platform not supported" ; \
  fi


all:
	$(INVOKE_MAKE)

debug:
	$(INVOKE_MAKE)

release:
	$(INVOKE_MAKE)

clean:
	$(INVOKE_MAKE)

install:
	$(INVOKE_MAKE)

toolkit:
	$(INVOKE_MAKE)

debug_toolkit:
	$(INVOKE_MAKE)
