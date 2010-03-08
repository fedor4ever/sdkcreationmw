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

package SDK_vars;
use strict;
no strict "subs";
use Exporter;
use Cwd;

our ($VERSION, @ISA, @EXPORT, %iniH, %common_comp, %common_comp2, %common_comp3, %lib_dirs_to_collect_from_platform, 
     $sdk_root, $repository_area, @remove_lists, $blacklist, $common_graylist, $cpp_graylist, $java_graylist, 
     $symbianlist, %sdk_lang_codes, %extra_lang_codes, $IFs, @ecmt_dirs,@emu_src_dirs, @tools_src_dirs, @java_dirs, @dirs_to_dbgstrip, $body, 
     %final_dest_dirsH, %Java_final_dest_dirsH, %final_dest_dirsH2,  
     %root_levelH3, $sms_receivers, @sub_roots, $init_dir, @RnD_dirs,%lang_names );

$VERSION = 1.00;
@ISA = qw(Exporter);
@EXPORT = qw(parse_ini_file  
             %iniH %common_comp %common_comp2 %common_comp3 %lib_dirs_to_collect_from_platform $sdk_root 
             $repository_area @remove_lists $blacklist $common_graylist $cpp_graylist $java_graylist %sdk_lang_codes %extra_lang_codes
             $IFs $symbianlist @ecmt_dirs @emu_src_dirs @tools_src_dirs @java_dirs @dirs_to_dbgstrip $body 
             %final_dest_dirsH %Java_final_dest_dirsH %final_dest_dirsH2 
             %root_levelH3 $sms_receivers @sub_roots $init_dir @RnD_dirs %lang_names %lib_collect_dirs );

#These will be executed every time
($init_dir = Cwd::cwd())=~ tr [/][\\];

initialize_start_dir($init_dir);


parse_ini_file("sdk.ini", \%iniH);
if (-f "cmd.ini"){
	parse_ini_file("cmd.ini", \%iniH);
}
set_sdk_name();

my $tmp_path=$init_dir."\\tools;".$ENV{PATH};
$ENV{PATH}=$tmp_path;


if ($iniH{Output_Base_substed}){
  `subst $iniH{Output_Base_substed} /d` ;
  system ("subst $iniH{Output_Base_substed} $iniH{Output_Base}");
}

my $substs = `subst`;
print $substs;
print "\nSubstitutions vs Start_time or Sdk_root mismatch!!!\n$substs\n$iniH{S60_Root} $iniH{Start_time} \n\n" unless ($substs =~ /$iniH{S60_Root}\\: =>.+$iniH{Start_time}/);

#Global variables
$repository_area = "$iniH{S60_Root}\\sce\\Repository_area";
$blacklist       = "$repository_area\\Common_Blacklist\\black_list.txt";
$common_graylist = "$repository_area\\Common_Graylist\\common_graylist.txt";
$cpp_graylist    = "$repository_area\\CPP_Graylist\\graylistcpp.txt";
$java_graylist   = "$repository_area\\Java_Graylist\\Java_graylist.txt";
$IFs             = "\\IFs";
@remove_lists = ($blacklist, $common_graylist, $cpp_graylist, $java_graylist);

#The set of sub root directories including include itself (=empty)!
@sub_roots = qw ( \\ \\middleware\\ \\osextensions\\ \\applications\\);
# Following line commented by ngv to ensure more apis are included


#Copy components from repositories to repository_area
#If it contains . then only that pattern is copied. Otherwise all subdirectories are copied as well.
#NB. Complemantary localisation files are copied only if they are in *_localisation.zip. 

#  Platform_Patch          => [qw(\\*S60_3_1_200614_SP1.zip)],
#  Languages_Patch         => [qw(\\*.zip)],
#	 Common_Tools            => [""],

%common_comp = (
  Beatnik                  => [""],
  Capas                    => [""],
  Common_Blacklist         => [qw(\\black_list.txt)],
  Common_Graylist          => [qw(\\common_graylist.txt)],
	Common_Patch             => [""],
	Common_S60Tools          => [""],
  SDK_Installer            => [""],
  Doc_Browser              => [""],
  Doc_General              => [""],
  Doc_Carbide_Plugin       => [""],
  Doc_External             => [""],
  Doc_Licensee             => [""],
  Doc_Eclipse_Plugin       => [""],
  Doc_NetBeans_Plugin      => [qw(\\*.nbm)],
  CPP_Doc_APIRef           => [""],
  CPP_Carbide_Manifest     => [""],
  CPP_CarbideVS_Descriptor => [""],
  CPP_API                  => [""],   
  CPP_Examples             => [""],
  CPP_Graylist             => [qw(\\graylistcpp.txt)],
  CPP_Patch                => [""],
  CPP_Commonfilestools     => [""],
  CPP_Symbian_Ex           => [""],
  #CPP_S60Tools             => [""],
  CPP_Tools                => [""],
  CPP_Tools_build          => [""],
  Java_Examples            => [qw(\\examples)],
  Java_Graylist            => [qw(\\Java_graylist.txt)],
  Java_Patch               => [""],
  Java_SDK                 => [qw(\\emulator.exe \\prefs.exe \\preverify2.exe \\preverify.exe)],
  #Java_S60Tools            => [""],
  Java_Toolchain           => [qw(\\nei.jar)],
  Java_Tools_build         => [""],
  Java_NUMC                => [""],
  Widget_Examples          => [""],
  Vista_GCCEPatch_Plugin   => [""],
  CPP_OpenC_Plugin         => [qw(\\glib.zip)],
  Sdk_Lang_Extra_Readme    => [""],
  OOMMonitorAPI            => [""],
);                  
          
%lib_dirs_to_collect_from_platform = (
  '2.8'   => {
    armi   => [qw(urel)],
    thumb  => [qw(urel)],
    winscw => [qw(udeb)]
  },
  '3.0'   => {
    armi   => [qw(urel)],
    armv5  => [qw(lib urel udeb)],
    thumb  => [qw(urel)],
    winscw => [qw(udeb)]
  },
  '9.9'   => {
    armv5  => [qw(lib urel udeb)],
    winscw => [qw(udeb)]
  },
);

%sdk_lang_codes = (
  zh_CN => [qw( 31 159)],
  en_US => [qw( 01 10)],
#  ja_JP => [qw( 32 160)], /Commented to remove the japanese lang from N97 SDK
);
%extra_lang_codes = (
##  tr_TR => [qw( 14 )],
##  th_TH => [qw( 33 161)],
##  ar_SA => [qw( 37 )],
##  fa_IR => [qw( 50 )],
##  el_GR => [qw( 54 )],
##  he_IL => [qw( 57 )],
##  hi_IN => [qw( 58 )],
##  uk_UA => [qw( 93 )],
##  ur_PK => [qw( 94 )],
);
#Update this hash to create FN specific extra lang zips
%lang_names=(
ar_SA=>"Arabic",
ur_PK=>"Urdu"
);
$body = "";
$sms_receivers = "";

#Blacklist and dbgstrip applied to these
%common_comp2 = (
	Beatnik                 => "\\sdk_wa\\",
);
#Patches et al. These are copied at phase 2 ie. after blacklist but before dbgstrip
%common_comp3 = (
  Common_Patch          => "\\sdk_wa\\",
);

%final_dest_dirsH = (
  CPP_CarbideVS_Descriptor 	=> "\\SDK_files\\CarbideVS_Descriptor\\",
  CPP_Carbide_Manifest  		=> "\\SDK_files\\carbide_manifest\\epoc32\\kit\\",
  CPP_Doc_APIRef            => "\\SDK_files\\docs\\",
  Doc_Carbide_Plugin		    => "\\SDK_files\\carbide_docplugin\\epoc32\\kit\\",
  Doc_General               => "\\SDK_files\\docs\\",
  Doc_Browser               => "\\SDK_files\\docs\\",
  Doc_Licensee              => "\\SDK_files\\docs\\Terms\\",
  Doc_Eclipse_Plugin        => "\\SDK_files\\Eclipse_Plugin\\",
  SDK_Installer         		=> "\\installer\\",
  CPP_Symbian_Ex            => "\\SDK_files\\Examples\\Symbian\\",
  CPP_Examples              => "\\SDK_files\\Examples\\CPP\\",
  CPP_Commonfilestools  		=> "\\SDK_files\\Commonfilestools\\",
  Java_API                  => "\\SDK_files\\lib\\",
  #Java_Doc_Examples			    => "\\SDK_files\\S60examples\\",
  Java_Examples             => "\\SDK_files\\Examples\\MIDP\\",
  Java_Patch                => "\\SDK_files\\",
  SDK_Installer             => "\\SDK_installer\\",
  Java_NUMC                 => "\\SDK_files\\Java_NUMC\\",
  Java_SDK                  => "\\SDK_files\\bin\\",
  Java_Toolchain            => "\\SDK_files\\epoc32_toolchain_common\\epoc32\\tools\\ecmt\\",
  Widget_Examples           => "\\SDK_files\\Examples\\Widget\\",
  Vista_GCCEPatch_Plugin    => "\\SDK_files\\Vista_GCCEPatch_Plugin\\",
);
                      

%final_dest_dirsH2 = (
  Capas                 => "\\SDK_files\\epoc32_emu\\",
  CPP_Patch             => "\\SDK_files\\",                      #These may go to epoc32_emu eventually     
  #CPP_S60Tools          => "\\SDK_files\\S60Tools\\",
  CPP_Tools             => "\\SDK_files\\",
  CPP_Tools_build       => "\\SDK_files\\S60Tools\\Ecmt\\",
	Common_S60Tools       => "\\SDK_files\\S60Tools\\",
  Java_Patch            => "\\SDK_files\\",
  #Java_S60Tools         => "\\SDK_Java\\SDK_files\\S60tools\\",
  Java_Tools_build      => "\\SDK_files\\S60tools\\Ecmt",
  CPP_OpenC_Plugin      => "\\SDK_files\\OpenC_Plugin\\",
);


%root_levelH3 = (
  Doc_External     			 => ".\\",
  #Doc_NetBeans_Plugin    => ".\\",
);

#These are copied from SDK platform -> sdk_wa 
@emu_src_dirs = qw (data  release\\winscw\\udeb winscw);
@ecmt_dirs = qw (tools\\ecmt);

#These are copied from SDK platform -> sdk_wa                 
@tools_src_dirs = qw (release\\winc\\urel\\*.dll
                      release\\winc\\urel\\*.exe 
                      release\\winc\\urel\\*.fsy 
                      tools  gcc  cshlpcmp_template);

#These dirs are copied from sdk_wa\epoc32 -> Java sdk
@java_dirs = qw (data  release\\winscw  tools\\ecmt tools\\java tools\\SipServer winscw);

@RnD_dirs = qw ( data
                 cshlpcmp_template
                 gcc
                 include
                 release\\winscw\\udeb
                 release\\winc\\urel\\*.dll
                 release\\winc\\urel\\*.exe 
                 release\\winc\\urel\\*.fsy 
                 tools
                 winscw
               );

#These directories are stripped on work_area
@dirs_to_dbgstrip = qw (epoc32\release\winscw\udeb
                        epoc32\release\winscw\urel
                        epoc32\tools);



sub parse_ini_file {
  my $regex_var = qr{
  	 \$		          # dollar sign
  	 (\{)?		      # $1: optional opening curly
  	 ([a-zA-Z_]\w*)	# $2: capturing variable name
  	 (?(1)\})		    # $3: if there's the opening curly... match closing curly
  }x;
  my $regex_sub = qr{
  	 sub	          # 
  	 \s*            # potential blanko(s)
  	 (\{)?		      # $1: optional opening curly
  	  \s*
  	 (.+)	          # $2: capturing variable name
  	 (?(1)\})		    # $3: if there's the opening curly... match closing curly
  }x;

  my ($ini_file, $href) = @_;  
  open(INI, "<$ini_file") or die "Cannot read $ini_file: $!\n";

  while( <INI> ) {
  	chomp;
  	s/\s#.*//;            # Remove comments
  	s/\s+$//;             # and traling spaces
  	s/^\[.*\]// ;         # and sections
    next unless length;   # anything left?
  	my ($key, $value ) = split /\s*=\s?/;                   #Don't comsume more than 1 blanko after =
  	while ($value =~/$regex_var/ && defined ($$href{$2})){  #interpolate only if the variable is defined earlier in the inifile
  	  $value =~ s/$regex_var/$$href{$2}/;
    }

   	if ($value =~ s/$regex_sub/$2/){                #Evaluate if the value starts with 'sub{'
   	  $value =~ s/\$/esc_dollar_/g;                 #\$ -> esc_dollar
  	  $value = eval $value;
   	  $value =~ s/esc_dollar_/\$/g;
  	}
  	$$href{ $key } = $value;
  }
  close INI;
}

sub set_sdk_name {

	my %Phases = ('a' => 'Alpha',
				  'b' => 'Beta',
				  'f' => '1.0',
				  'h' => 'Herald',);
	
  # Following section was added by ngv on 08/08/2008 due to changes in SDK naming
	$iniH{ Kit_ID } = "$iniH{Kit_ID}_$iniH{Start_Menu2}";
	$iniH{ SDK_Short_name } = "$iniH{SDK_Short_name} $iniH{Start_Menu2}";
	$iniH{ SDK_Product_name } = "$iniH{ SDK_Product_name } $iniH{Start_Menu2}";
	$iniH{ Build_ID } = "-";		
	$iniH{ Start_Menu2 } = "$iniH{Start_Menu2}";

	if ( $iniH{Support_pack} ) {
		$iniH{SDK_Product_name} = "$iniH{SDK_Product_name}, $iniH{Support_pack}";
		$iniH{sdkApiRefFullName} = "$iniH{SDK_Product_name}";
	}
}

sub initialize_start_dir {

  
  my $curr_dir = Cwd::cwd();
  $curr_dir =~  s/\//\\\//g;     # forward slashes in dir name
 
  
  print "curr_dir = $curr_dir\n";
  #Put the curr_dir to sdk.ini
  my $TargetFile = "sdk.ini";
  system ("perl -pi.old -e\"s/(StartDir\\s+=).*/StartDir                  = $curr_dir/;\"  $TargetFile");

  $TargetFile .= ".old";
  system ("del /F $TargetFile");
}
1;
