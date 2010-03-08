#
# Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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

use SDK_vars;
use SDK;
use strict;
use warnings;
use Getopt::Long;
use Data::Dumper;
use File::Basename;
use validate_build;


my %iniH;

my $valobj=new validate_build(0);
my $target=""; 
my $dump ="";
my $val ='';
my $fh;
my $phasedir = ".";
my $temp;

if(!(-d $phasedir))
{
	mkdir (${phasedir});
}

$val = shift @ARGV;
local *FILE = $valobj->open_log_file($phasedir);
parse_ini_file("sdk.ini", \%iniH);
$dump = $valobj->dump_sdk_ini(\%iniH);
$valobj->validate_sdk_ini_paths($dump,$target,\%iniH,\*FILE,"$iniH{S60_Root}\.*|$iniH{Sce_Root}\.*|Languages_Patch");
$valobj->check_for_sw_paths(\%iniH,\*FILE);
$valobj->check_for_build_directories(\%iniH,$val,\*FILE); 
$valobj->verify_time_server(\%iniH,\*FILE); 
$valobj->check_space_dir(\%iniH,\*FILE); 
$valobj->cm_access(\%iniH,\*FILE);
$valobj->build_id_server_access(\%iniH,\*FILE);
$valobj->perl_file_compilation(\%iniH,\*FILE);
$valobj->close_logfile(\*FILE);
$valobj->Notify_sms(\%iniH,$phasedir);
$valobj->Send_MailNotification(\%iniH,$phasedir);
