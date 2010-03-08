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

use TinyMake ':all';
use strict;
use Cwd;
use SDK_vars;
use SDK;
use SDK_utils;
use Remote_utils;
use SDK_lang;
use ApiRef;
use File::Path;

my $phasedir = "phase";
my $logsdir = "logs";
my ($sec,$min,$hour,$mday,$mon,$year) =  localtime();
my $start_time = sprintf ("%02d%02d%02d%02d%02d", ($year + 1900)%100, $mon + 1, $mday, $hour, $min);
our $given_target = $ARGV[0] || "${phasedir}\\_all";
$|=1; #Immediate flush
mkpath (${phasedir});
mkpath (${logsdir});

file "${phasedir}\\initialize.log" => sub {
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  initialize($start_time, \%iniH);
   record_time("end",$target,$phasedir);
    sh "touch $target";
};
file "${phasedir}\\sdk_installer.log" => ["${phasedir}\\sdk_files.log"], sub {
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  run_installer();
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\sdk_publish.log" => ["${phasedir}\\sdk_installer.log"], sub {
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  publish_sdk();
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\sdk_files.log" => ["${phasedir}\\sce.log","${phasedir}\\sdk_api.log","${phasedir}\\api_ref.log","${phasedir}\\language_packages.log","${phasedir}\\sdk_common.log",
  "${phasedir}\\_copy_api_platform"], sub { 
	  record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  create_sdk_files();
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\sdk_common.log" => ["${phasedir}\\sce.log","${phasedir}\\sdk_api.log"], sub {
       record_time("start",$target,$phasedir);	
  start_target($target);
  create_sdk_common(\%iniH); 
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\language_packages.log" => ["${phasedir}\\copy_lang_zips.log","${phasedir}\\s60_platform.log","${phasedir}\\sdk_api.log","${phasedir}\\sdk_common.log"], sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  create_lang_package($iniH{S60_Root}, "$iniH{S60_Root}\\Languages", $common_graylist, $blacklist, "\\sdk_lang\\epoc32\\tools\\ecmt\\language");
   record_time("end",$target,$phasedir);
  sh "touch $target"; 
};

file "${phasedir}\\sdk_api.log" => ["${phasedir}\\fetch_IF_sheets.log","${phasedir}\\sce.log"], sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  create_sdk_api(\%iniH);
   record_time("end",$target,$phasedir);
  sh "touch $target";
};


file "${phasedir}\\S60_api_description.log" => ["${phasedir}\\s60_all.log"], sub { 
	record_time("start",$target,$phasedir);
  start_target($target);
  create_apidescr_xml( \%iniH, "$iniH{S60_Root}\\sf", "$repository_area\\CPP_API\\apidescr.xml");
  record_time("end",$target,$phasedir);
  sh "touch $target";
};

file "${phasedir}\\sce.log" => ["${phasedir}\\s60_platform.log","${phasedir}\\synergy_work_area.log","${phasedir}\\sdk_binary_components.log","${phasedir}\\sdk_platform.log", 
  "${phasedir}\\examples_Java.log", "${phasedir}\\fetch_capas.log","${phasedir}\\beatnik.log"], sub { 
	  record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  sce_prepare(\%iniH);
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\sdk_platform.log" => ["${phasedir}\\s60_platform.log","${phasedir}\\synergy_work_area.log"], sub {
       record_time("start",$target,$phasedir);	
  start_target($target, ${logsdir});
  integrate(\%iniH);
   record_time("end",$target,$phasedir);
  sh "touch $target"; 
};

file "${phasedir}\\examples_Java.log" => sub {
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  build_examples_java(\%iniH);
   record_time("end",$target,$phasedir);
  sh "touch $target"; 
}; 

file "${phasedir}\\synergy_work_area.log" => sub {
       record_time("start",$target,$phasedir);	
  start_target($target, ${logsdir});
  reconfigure(\%iniH);
  copy_sdk_source_components(\%iniH); 
  start_target("${phasedir}\\sdk_binary_components.log");
  sh "touch ${phasedir}\\sdk_binary_components.log";
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\s60_platform.log" => ["${phasedir}\\s60_all.log","${phasedir}\\s60_recovery.log","${phasedir}\\copy_lang_zips.log"], sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  copy_platform_patch();
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\s60_all.log" => ["${phasedir}\\initialize.log"], sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  copy_platform(\%iniH);
  start_target("${phasedir}\\s60_recovery.log");
  sh "touch ${phasedir}\\s60_recovery.log";
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\s60_recovery.log" =>  sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  unzip_some_source_zips();
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\copy_lang_zips.log" =>  sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  copy_language_zips(\%iniH);
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\copy_api_platform.log" =>  sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  copy_api_platform(\%iniH);
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\sdk_binary_components.log" => ["${phasedir}\\synergy_work_area.log"], sub { 
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  copy_sdk_binary_components(\%iniH);
   #Todo: Add Path validator code here
  my $dump =dump_sdk_ini(\%iniH);
  validate_sdk_ini_paths($dump,$target,\%iniH,"Languages_Patch");
   record_time("end",$target,$phasedir);
  sh "touch $target";
};

file "${phasedir}\\beatnik.log" => sub {
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  beatnik($iniH{Beatnik});
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\fetch_capas.log" => sub {
	record_time("start",$target,$phasedir);
  start_target($target);
  fetch_capas($iniH{Capas});
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\fetch_IF_sheets.log" => ["${phasedir}\\initialize.log"], sub {
	record_time("start",$target,$phasedir);
  start_target($target, ${logsdir});
  fetch_IF_sheets($iniH{CPP_API}, "$iniH{S60_Root}\\sce\\IFs", \%iniH);
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\tidy_up" => sub { 
	
  tidy_up(${phasedir}, ${logsdir});

};
file "${phasedir}\\sdk.ini" => sub { 
	record_time("start",$target,$phasedir);
 

  my $dump = dump_sdk_ini(\%iniH);
  local $,="\n";
  print @$dump;
   record_time("end",$target,$phasedir);

};


# This is for testing purposes only....
file "phase\\dummy" => sub { 
	record_time("start",$target,$phasedir);
	 start_target($target, ${logsdir});
  print "Dummy phase entered !\n";
  
  print  "changed value: $iniH{StartDir}\n";
    copy_and_interpolate("$init_dir\\$::iniH{PTBase}\\templates\\apiref32.properties", "$init_dir\\$::iniH{PTBase}\\conf");
    copy_and_interpolate("$init_dir\\$::iniH{PTBase}\\templates\\DoxySettings.cfg", "$init_dir\\$::iniH{PTBase}\\doxygen");
    
	 record_time("end",$target,$phasedir);   
  sh "touch $target";
};



make @ARGV;



__END__
file "${phasedir}\\_cpp_installer" => ["${phasedir}\\_sdk_cpp"], sub {
  start_target($target);
  run_cpp_installer();
  sh "touch $target";
};
file "${phasedir}\\_cpp_publish" => ["${phasedir}\\_cpp_installer"], sub {
  start_target($target);
  publish_cpp();
  sh "touch $target";
};
file "${phasedir}\\_java_installer" => ["${phasedir}\\_sdk_java"], sub {
  start_target($target);
  run_java_installer();
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
file "${phasedir}\\_java_publish" => ["${phasedir}\\_java_installer"], sub {
  start_target($target);
  publish_java();
   record_time("end",$target,$phasedir);
  sh "touch $target";
};
