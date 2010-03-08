#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
#!/usr/bin/perl
#
# $Id: sdk_env.pl,v 1.10 2006/02/22 03:09:50 slava Exp $
#
# Common Perl code shared by sdk_build.pl and sdk_dist.pl scripts.
# Used mostly to set the right environment.
#

use FindBin;
use Env qw(JAVA_HOME MWCIncludes PATH RVCTROOT ARMLMD_LICENSE_FILE);

$FS = '/';  # File separator
$PS = ';';  # Path separator

($BUILD_ROOT = $FindBin::RealBin) =~ s/\/[^\/]+\/[^\/]+\/[^\/]+$//g;
($BUILD_ROOT = $BUILD_ROOT . $FS) unless $BUILD_ROOT =~ m/$FS$/;
($EPOCROOT = $BUILD_ROOT) =~ s/^([A-Z]|[a-z])\://g;
$EPOCROOT =~ s/$FS$/\\/;

$ENV{BUILD_ROOT} = $BUILD_ROOT;

  # Find java (at least 1.4.x)
  if (!defined $ENV{JAVA_HOME}) {
    # Should we check Java version?
    $javac_dir = find_prog_or_die ("javac.exe");
    ($ENV{JAVA_HOME} = $javac_dir) =~ s/$FS[^$FS]+$//g;
  }
  $ENV{JAVA_HOME} =~ s/\\/$FS/g;
  $ENV{JAVA_HOME} =~ s/$FS*$//g;

  # Get the path to the Metrowerks tools
  $cw_tool_exe = "mwccsym2.exe";
  $cw_tool_dir = find_prog ($cw_tool_exe);
  $cw_bin_dir = "";

  # CW 3.1 requires that its bin directory is in the PATH
  # Otherwise some command line tools can't find lmgr8c.dll
  if (-d $cw_tool_dir) {
    ($cw_bin_dir = $cw_tool_dir) =~ s/$FS[^$FS]+$FS[^$FS]+$//g;
    $cw_bin_dir .= "/Bin";
  } else {
    $cw_bin_dir = find_prog_or_die ("IDE.exe");
    ($cw_tool_dir = $cw_bin_dir) =~ s/[^$FS]+$//g;
    $cw_tool_dir .= "/Symbian_Tools/Command_Line_Tools";
    if (-x "$cw_tool_dir/$cw_tool_exe") {
      die "ERROR: cound not find $cw_tool_exe in $cw_tool_dir\n";
    }
  }

  # Get paths to MSDEV, cl and link
  $msdev_dir = find_prog_or_die("msdev.exe");
  $cl_dir = find_prog_or_die("cl.exe");
  ($msdev_home = $cl_dir) =~ s/\\/$FS/g;
  $msdev_home =~ s/$FS[^$FS]+$//g;

  # Get path for MWCIncludes
  ($mwc_inc = $cw_tool_dir) =~ s/$FS[^$FS]+$FS[^$FS]+$//g;
  $mwc_inc = "+$mwc_inc$FS" . "Symbian_Support";

  # Get path to the perl
  ($perl_path = $^X) =~ s/\\/$FS/g;
  $perl_path =~ s/$FS[^$FS]+$//g;

  # Get path to the ant tool.
  # Ant is needed only for building Java component
  $ant_dir = "";

  if (!defined($ENV{ANT_HOME})) {
    $ant_dir = find_prog ("ant.bat");
    $ant_dir =~ s/\\/$FS/g;
    $ant_dir =~ s/$FS[^$FS]+$//g
  } else {
    $ant_dir = $ENV{ANT_HOME};
    $ant_dir =~ s/\\/$FS/g;
  }

  if ($ant_dir eq "") {
    warn "***** ACHTUNG! Couldn't find ANT! Java code won't be built! ****\n";
  }

  # Get path to the python tool.
  # Python is needed only for building Java AMS in the S60 platform
  $python_dir = find_prog ("python.exe");
  $python_dir =~ s/\\/$FS/g;
  $python_dir =~ s/$FS[^$FS]+$//g;    #I wonder if this ever matches

  if ($python_dir eq "") {
    warn "***** ACHTUNG! Couldn't find Python! Java Platform code won't be built! ****\n";
  }

  # RVCT environment
  $rvct_bin = find_prog ("armcc.exe");
  unless ( $rvct_bin ){
    if (defined($ENV{RVCTROOT})) {
      $rvct_bin = $ENV{RVCTROOT} . "${FS}bin$PS";
    } else {
      warn "***** RVCT compiler not found in $rvct_bin ****\n";
      $rvct_bin = "";
    }
  }
  # Allow the user to redefine license information
  if (!defined ($ENV{ARMLMD_LICENSE_FILE})) {
    # Default to US configuration
    $ENV{ARMLMD_LICENSE_FILE} = "8224@bslic02.americas.nokia.com";
  }

  # doxygen
  # doxygen is needed only, if s60 c++ example application finalize is done
  $doxy_bin = find_prog ("doxygen.exe");
  if ($doxy_bin eq "") {
    warn "***** NOTE! Couldn't find Doxygen! S60Ex document generation cannot be done ****\n";
  }

  # This one if for CodeWarrior
  $ENV{MWCIncludes} = $mwc_inc;

  # These are for Microsoft Visual C++
  $ENV{INCLUDE} = "$ENV{JAVA_HOME}$FS" . "include$PS" .
                  "$ENV{JAVA_HOME}$FS" . "include$FS" . "win32$PS" .
                  "$msdev_home$FS" .  "include$PS" .
                  "$msdev_home$FS" . "mfc$FS" . "include";
  $ENV{LIB} = "$msdev_home$FS" .  "lib$PS$msdev_home$FS" . "mfc$FS" . "lib";

  # These affect everything...
  $ENV{EPOCROOT} = $EPOCROOT;
  $ENV{PATH} = $rvct_bin . "$PS$ENV{JAVA_HOME}$FS" . "bin$PS" .
               "$ant_dir$FS" . "bin$PS" .
               "$python_dir$PS" .
               $BUILD_ROOT . "epoc32$FS" . "tools$PS" .
               $BUILD_ROOT . "epoc32$FS" . "gcc$FS" . "bin$PS" .
               "$cw_tool_dir$PS$cw_bin_dir$PS" .
               "$msdev_dir$PS$cl_dir$PS$perl_path$PS" .
               "$ENV{SystemRoot}$PS" . "$ENV{SystemRoot}$FS" . "system32$PS" .
               "$doxy_bin";

#  print "$ENV{JAVA_HOME}\n$ENV{EPOCROOT}\n$ENV{BUILD_ROOT}\n$ENV{MWCIncludes}\n$ENV{PATH}\n$ENV{RVCT21INC}\n$ENV{RVCT21LIB}\n$ENV{ARMROOT}\n";

# ======================================================================+
  sub find_prog_or_die
# =======================================================================
  {
    my $prog = @_[0];
    my $dir = find_prog($prog);
    die "ERROR: $prog not found in the PATH\n" unless $dir;
    return $dir;
  }

# =======================================================================
  sub find_prog
# =======================================================================
  {
    my $prog = @_[0];
    for $dir (split /;/, $ENV{PATH}) {
      if (-x "$dir/$prog") {
        $dir =~ s/\\/$FS/g;
        $dir =~ s/$FS*$//g;
        return $dir;
      }
    }
    return "";
  }

#
# Code style configuration for Emacs:
#
# Local Variables:
# perl-indent-level: 2
# perl-continued-statement-offset: 2
# perl-continued-brace-offset: -2
# indent-tabs-mode: nil
# End:
#
