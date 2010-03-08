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
use TinyMake ':all';

use strict;
use log_collector;
use SDK;
use SDK_vars;
use Remote_utils;

my $phasedir = "phase";
my $logsdir = "logs";

file "${phasedir}\\collect_sdk_logs" => sub {
  collect_sdk_logs(@ARGV[1]);
  #Copy the phase logs to  Build output area
  my $phase_base=$iniH{'StartDir'};
  $phase_base=~s/\//\\/g;  #Convert unix path to windows
  my $cmd ="xcopy $phase_base\\phase $iniH{Output_Base}\\$iniH{Release}\\$iniH{Platform_Version_Major}.$iniH{Platform_Version_Minor}\\$iniH{Start_time}\\SDK_team_only\\Build_Log  /F /R /I /E /Y";
  print "\n EXEC: $cmd";
  system ($cmd)==0 or print "\n ERROR: $cmd has failed. $!";
  sh "touch $target";
};



file "mail" => ["${phasedir}\\collect_sdk_logs"], sub {
  form_mail();
  Notify_mail("$sdk_zip is ready!"); 
};


make @ARGV;
