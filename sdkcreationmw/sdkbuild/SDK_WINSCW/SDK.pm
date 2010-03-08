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

package SDK;
use SDK_vars;
use SDK_utils;
use Remote_utils;
use File::Path;
use IO::Socket;
use Cwd;
use File::Find;
use SDK_checksum;
use ApiRef;
use Data::Dumper;
use Sys::Hostname;
use JarSegregator;

#use Mail::Sender;

our ($VERSION, @ISA, @EXPORT);

$VERSION = 1.00;
@ISA = qw(Exporter);
@EXPORT = qw(initialize   dump_sdk_ini validate_sdk_ini_paths copy_api_platform copy_platform_patch copy_sdk_source_components 
             copy_sdk_binary_components integrate build_examples_java create_sdk_api sce_prepare   
             get_CPP_Symbian_Ex create_api_ref create_sdk_common create_sdk_files publish_sdk run_installer rm_old_dirs unzip_some_source_zips);

sub initialize {
  my ($start_time, $href) = @_;
  system ("perl -v");
  my $curr_dir = cwd;
  my $s60_root_dir = "$$href{S60_Root_Path}\\$start_time";
  
  #Put the start time to sdk.ini
  my $TargetFile = "sdk.ini";
  system ("perl -pi.old -e\"s/(Start_time\\s+=).*/Start_time                   = $start_time/;\"  $TargetFile");
  print <<KORVAUS;
$TargetFile: Start_time    -> $start_time
KORVAUS
  $TargetFile .= ".old";
  system ("del /F $TargetFile");
  $$href{Start_time} = $start_time;

  mkpath($s60_root_dir) unless (-e $s60_root_dir);             #Make it
  chdir  ("$s60_root_dir") or die "Couldn't chdir    $s60_root_dir: $!";
  system ("subst $$href{S60_Root} /d");
  print "$s60_root_dir\n";
  system ("subst $$href{S60_Root} $s60_root_dir");
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
}
# Dump the contect of sdk.ini (which is loaded in %iniH)

# Takes array ref of dump of %iniH and validate the paths. 
# 
sub dump_sdk_ini($)
{
	enter_sub();
    my $iniref=shift @_;
	$Data::Dumper::Sortkeys = \&my_filter;
	$Data::Dumper::Terse    = 1;
	$Data::Dumper::Indent   = 1;

	my $foo = { %$iniref };
	my @out;

	@out= split(/\n/,Dumper[$iniref]);
	#print "\n in dump";
	#print @out;
	sub my_filter
	{
		my ($hash) = @_;

		# return an array ref containing the hash keys to dump
		# in the order that you want them to be dumped
		return [
			# Sort the keys of %$foo in reverse numeric order
			$hash eq $foo ? ( sort { $b <=> $a } keys %$hash ) :

			  # Only dump the odd number keys of %$bar
			  #  $hash eq $bar ? (grep {$_ % 2} keys %$hash) :
			  # Sort keys in default order for all other hashes
			  ( sort keys %$hash )
		];
	}
	exit_sub();
	return \@out;
}

#Takes array ref and validate paths and notify by mail.
#Second input is $target or phase
#Third is %iniH hash ref
#Forth input is ignore pattern, inititally you can ignore Z: errors
sub validate_sdk_ini_paths($$$$)
{
	enter_sub();
	my $sdk_ini_array_ref=shift @_;
	my $phase_name=shift @_;
	my $iniH_ref=shift @_;
	my $ignore_pat=shift @_;
	
	my $line;
	my $val;
	my $err_level=0;
	my $MAX_RETRY=3;
	my $MAX_TIME_WAIT=10;#Todo: Change to long time
	my $i;
	my @err_paths;
	# Get Valid paths and check for its presense
	print "\n \nValidating SDK.ini paths\n";
	print "\n Ignore following pattern: $ignore_pat\n";
	foreach $line (@$sdk_ini_array_ref)
	{
		next if($line=~m/\s*#/); #Skip the # commented
		#Skip some known invalid keys
		next if($line=~m/Target_Directory/);
		#Skip ignore patterns
		if (length ($ignore_pat)>0 && $line=~m/$ignore_pat/o)
		{
			print "\n SKIP: $line";
			next;
		}
		
		if($line=~m/\s+'(.*)'\s+=>\s+'(.*)'/)
		{
			$val=$2;
			if($val=~m/^[A-Z]:|^\\\\\\\\(.*)\\+(.*)/i) #Should be valid drive or windows share
			{
				#Strip multiple \\ to \
				$val=~s/\\\\|\//\\/g;
				if (! -d $val)
				{
					$err_level=0;
					#Ohh , Got error, Take a nap and come back
					for ($i=0;$i<$MAX_RETRY;$i++)
					{
						print "\n ".($i+1)."Retry to access : $val";
						sleep ($MAX_TIME_WAIT);
						if( -d $val)
						{
							$err_level=0;
							last;
						}
						else
						{
							$err_level++;
						}	
					}
										
					if($err_level>=$MAX_RETRY) #Tried at max
					{
						print "\n ERROR: $val is not found. Key: $line";
						push (@err_paths,"\n ERROR: $val is not found. Key: $line");
					}
					
				}
				else
				{
					print "\n OK: $val";
				}
			}
		}		
	}

	if(@err_paths) #There are some errors, Looks like.
	{
		
		$,="\n";
		my $sub="$$iniH_ref{Platform_Name} Build has ERRORS";
		my $msg= <<END_OF_MSG;
Hi ,
$$iniH_ref{Platform_Name} Build has errors due to following paths are not accessible.

Build Phase: $phase_name

Invalid Paths
--------------
@err_paths

Please refer the Log files for more details.

Log Path: $init_dir\\phase

Machine Name: $ENV{COMPUTERNAME}
		    
***** AUTO GENERATE MESSAGE****
END_OF_MSG

		Notify_plain_mail($sub,$msg);
	}
	else
	{
		print ("\n All the paths is SDK.ini is valid!!\n");
	}
	exit_sub();
}
sub unzip_some_source_zips {
  print "unzipping some source zips...Jee\n" ;
};

sub platform_patch_duplicates {
  my ($patch_dir) = @_;
  enter_sub($patch_dir);
  my %files = ();
#  my $patch_dir = shift || "y:\\Repository_area\\Platform_Patch";
#  print "Checking duplicates from $patch_dir\n";
  opendir(DIR, $patch_dir) or die "can't opendir $path: $!";
  @zfiles = grep { /\.zip$/i } readdir(DIR);
  closedir(DIR);
#  for my $zip (<$patch_dir\\*.zip>){
  foreach (@zfiles) {
    my $zip = "$patch_dir\\$_";
#    print "Checking zip-file $zip\n";
    for ( `tools\\unzip -l $zip` ) {                      #Get the filelisting of the zip
      next if /\/$/;                                      #Skip the path lines. They end with /
      next unless (s/.+\d   \b//);                        #Devour anything before path
      chomp;
      $zip =~ s[.+\\][];                                  #Discard the path (of the zip)
      push @{$files{$_}}, $zip;
    }
  }
  
  foreach my $file (keys %files){
    print "\n$file: ", join ", ", @{$files{$file}} if (@{$files{$file}} > 1);  #Print if there are duplicates
  }
  print "\n";
  exit_sub();
}

sub copy_platform_patch {
  enter_sub($iniH{Platform_Patch});
  if (-e $iniH{Platform_Patch}){
    platform_patch_duplicates("$iniH{Platform_Patch}");
    my $cmd = "xcopy $iniH{Platform_Patch} $iniH{S60_Root}\\sce\\Repository_area\\Platform_Patch\\ /F /R /Y";
    system ($cmd)==0 or print ($cmd."\n\n");
    $cmd = "unzip -o $iniH{S60_Root}\\sce\\Repository_area\\Platform_Patch\\*.zip -d $iniH{S60_Root}";                        
    print ($cmd."\n"); system ($cmd);
  }
  exit_sub();
} 

sub copy_api_platform {
  enter_sub();
  return if not $iniH{Api_Build} or $iniH{Api_Build} eq $iniH{Platform_Build};
  my $temp_dir = "$iniH{S60_Root}\\temp2\\";
  print "Copying api platform from $iniH{Api_Build} -> $temp_dir === \n" ;
  my $cmd = "xcopy $iniH{Api_Build}\\*epoc32.zip $temp_dir /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");
  $cmd = "xcopy $iniH{Api_Build}\\*winscw.zip $temp_dir /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");
  $cmd = "xcopy $iniH{Api_Build}\\*armv5.zip $temp_dir /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");

  $cmd = "unzip -o -qq $temp_dir\\*.zip -d $iniH{S60_Root}\\apis";
  print ($cmd."\n"); system ($cmd);
  print "Deleted dir  - $temp_dir === ", scalar localtime(), "\n" if ((-e $temp_dir) && do_rmdir_all($temp_dir));
} 

sub copy_sdk_source_components {
  my ($href) = @_;
  print "Copying the sdk components from $$href{SCM_Workarea} -> $$href{S60_Root}\n";
  die "Workarea $$href{SCM_Workarea} not found!!!" unless (-e $$href{SCM_Workarea});
  die "Sdk root not found!!!" unless (-e "$$href{S60_Root}\\");

  #Clean the destination first
  chomp (my @dirs = `dir /ad /b /o-d $$href{SCM_Workarea}`);  #Subdir listining
  for (my $i = 0; $i < @dirs; ++$i){
    my $dir = "$$href{S60_Root}\\$dirs[$i-1]";
    print "Deleted dir  - $dir\n" if ((-e $dir) && do_rmdir_all($dir));
  }
  
  $cmd = "xcopy $$href{SCM_Workarea} $$href{S60_Root}\\ /F /R /I /E /Y";
  system ($cmd)==0 or print ($cmd."\n");
};

sub copy_sdk_binary_components {
  my ($href) = @_;
  print "Copying the sdk binary components from $$href{SCM_Workarea}\\PublicSDK\\sdkcreationmw\\SDKComponents-> $$href{Sce_Root}\\Repository_area === ", scalar localtime(), "\n"; 
  my $dir = "$$href{S60_Root}\\PublicSDK\\sdkcreationmw\\SDKComponents";
  print "Deleted dir  - $dir\n" if ((-e $dir) && do_rmdir_all($dir));
  $cmd = "xcopy $$href{SCM_Workarea}\\PublicSDK\\sdkcreationmw\\SDKComponents $dir\\ /F /R /I /E /Y";
  system ($cmd)==0 or print ($cmd."\n");
  $dir = "$$href{Sce_Root}\\Repository_area";
  print "Deleted dir  - $dir\n" if ((-e $dir) && do_rmdir_all($dir));
  $cmd = "xcopy $$href{S60_Root}\\PublicSDK\\sdkcreationmw\\SDKComponents $dir\\ /F /R /I /E /Y";
  system ($cmd)==0 or print ($cmd."\n");
}

sub integrate {
  my ($href) = @_;
#  my ($href, $type) = @_;
  print "Integrating emulator started === ", scalar localtime(), "\n";
  replace_strings_RnD();
  my $curr_dir = cwd;
  chdir "$$href{S60_Root}\\PublicSDK\\sdkcreationmw\\sdkbuild" || (warn "Chdir to $$href{S60_Root}\\PublicSDK\\sdkcreationmw\\sdkbuild failed. $!" and return);
  
  #Export S60_Root_Drive environment variable with S60_Root, It will be used in the sdk_build.pl script
  $ENV{"S60_Root_Drive"}=$$href{S60_Root};
  my $cmd = "perl sdk_build.pl -a -l --s60ex ";
  system ($cmd); # or print ($cmd."\n");
  #And check the errors
  $cmd = "perl sdk_build.pl -e ";
  system ($cmd);# or print ($cmd."\n");
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
}

sub build_examples_java {
  enter_sub();
  my ($href) = @_;
  my $curr_dir = cwd;
  chdir "$$href{S60_Root}\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples" || (warn "Chdir to $$href{S60_Root}\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples failed. $!" and return);
  my $cmd = "ant buildS60";
  system ($cmd)==0 or print ($cmd."\n");
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
}

sub create_sdk_api {
  enter_sub();
  my ($href) = @_;
  my $curr_dir = cwd; #org_dir
#  my $repository_area = "$$href{S60_Root}\\sce\\Repository_area";
  chdir "$$href{S60_Root}\\" or die "Cannot chdir to $$href{S60_Root}\\: $!";

  unless ($$href{Common_Blacklist} && $$href{CPP_API}) {
#    create_api_and_black_lists($$href{IFs_Platform_Version}, "$$href{Sce_Root}\\IFs", "sheets", "results", "$repository_area\\CPP_API", "$repository_area\\Common_Blacklist", $curr_dir);
    create_api_and_black_lists($$href{IFs_Platform_Version}, "$$href{S60_Root}\\sce\\IFs", "sheets", "$$href{S60_Root}\\sce\\IFs\\results", "$repository_area\\CPP_API", "$repository_area\\Common_Blacklist", $curr_dir);
  } else {
    $cmd = "xcopy $$href{CPP_API} $repository_area\\CPP_API\\sdk_apis_nokia.txt /F /R /Y";
    system ($cmd)==0 or print ($cmd."\n") ;
    $cmd = "xcopy $$href{Common_Blacklist} $repository_area\\Common_Blacklist\\black_list.txt /F /R /Y";
    system ($cmd)==0 or print ($cmd."\n") ;
  }
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
  #Beautify the blacklists, i.e. remove comments etc.
  beautify_remove_list($blacklist);
  exit_sub();
}

sub sce_prepare {
  my ($href) = @_;
  my $repository = $href;
  my $curr_dir = cwd;
  my $sce_root_dir = "$$href{S60_Root}\\sce";
  my $jsrobj=new JarSegregator;
  my $target_jar="extract_jars";

  enter_sub($sce_root_dir);
  system("subst");
  unless (-e $sce_root_dir) {
     mkpath "$sce_root_dir";
  }
  chdir "$sce_root_dir" or die "Couldn't chdir from $curr_dir to $sce_root_dir: $!";
  #Unsubst Sce_root
  `subst $$href{Sce_Root} /d`;
  print "sce_root_dir: $sce_root_dir\n";
  print ("\n Run: subst $$href{Sce_Root} $sce_root_dir");
  system ("subst $$href{Sce_Root} $sce_root_dir");
  chdir ("$$href{Sce_Root}") or die "Couldn't chdir to $$href{Sce_Root} $!";

  #Delete sdk_wa
  print "Deleted dir  - sdk_wa\n" if ((-e "sdk_wa") && do_rmdir_all("sdk_wa"));
  
  # Copy common components from repositories to the local Repository_area
  print "Copy common_comp from repositories to the Repository_area $repository_area started === ",  scalar localtime(), "\n";
  foreach $dir (keys %common_comp) {
    next unless $$repository{$dir};  #Skip empty components
    print "Deleted dir  - $copy_pattern $repository_area\\$dir\n" if ((-e "$copy_pattern $repository_area\\$dir") && do_rmdir_all("$copy_pattern $repository_area\\$dir"));
    print "common_comp: $dir\n";
    foreach my $copy_pattern (@{$common_comp{$dir}}){
      $cmd = "xcopy $$repository{$dir}$copy_pattern $repository_area\\$dir\\ /f /r /i /Y".(($copy_pattern =~ /\./)  ? "" : " /E");
      system ($cmd)==0 or print "$cmd\n";
    } 
  }
  
  print "\n------------------------ Jar Segregation (Start)---------------------------\n";
  	$target_jar = "$repository_area\\Java_API";
  	$jsrobj->jar_segregation($target_jar,\%{$href});
  print "\n------------------------ Jar Segregation (End)---------------------------\n";

  #Beautify the blacklists, i.e. remove comments etc.
  map (beautify_remove_list($_), grep {-e} @remove_lists);

  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
  exit_sub();
}

#sub create_api_ref {
#  my $parser4apiref = new XML::Parser(Style=>'Subs', Pkg=>'SH4ApiReference', Handlers => {Char => \&SH4ApiReference::baretext}, ErrorContext => 2);
##  S60_api_reference("$init_dir\\apirefgen\\apidescr\\apidescr.xml", \$parser4apiref, $iniH{S60_Root}, "$init_dir\\$iniH{PTBase}\\apis");
#  S60_api_reference("$repository_area\\CPP_API\\apidescr.xml", \$parser4apiref, $iniH{S60_Root}, "$iniH{ApisDir}");
#
#  #Copy the results to the repository_area
#  $cmd = "xcopy $init_dir\\$iniH{PTBase}\\plugins $repository_area\\Java_Doc_Eclipse_Plugin\\ /f /r /i /e /Y";  
#  system ($cmd)==0 or print ($cmd."\n");
#}

sub create_sdk_common {
  my $curr_dir = cwd();
  enter_sub("$iniH{Sce_Root}\\ ...");
#  print "Create sdk common $iniH{Sce_Root}\\ ...\n"; 
  chdir "$iniH{Sce_Root}\\" or warn "Couldn't chdir to $iniH{Sce_Root} - $!";

  #Copy emulator stuff to \\sdk_wa
  print "Copy emulator and tools from $iniH{S60_Root} to SDK work area \\sdk_wa === ", scalar localtime(), "\n";
#  map { system ("xcopy $iniH{S60_Root}\\epoc32\\$_ \\sdk_wa\\epoc32\\$_ /f /r /i /s /Y /EXCLUDE:$init_dir\\tools\\Emu_dirs.txt" )} @emu_src_dirs;
#  map { system ("xcopy $iniH{S60_Root}\\epoc32\\$_ \\sdk_wa\\epoc32\\$_ /f /r /i /s /Y " )} @tools_src_dirs;

  map { my $cmd = "xcopy $iniH{S60_Root}\\epoc32\\$_ \\sdk_wa\\epoc32\\$_ /f /r /i /s /Y /EXCLUDE:$init_dir\\tools\\exclude_from_emu.txt"; print "\n CMD: $cmd\n"; system ($cmd)==0 or print "ERROR: $cmd is failed\n";} @emu_src_dirs;
  map { my $cmd ="xcopy $iniH{S60_Root}\\epoc32\\$_ \\sdk_wa\\epoc32\\$_ /f /r /i /s /Y "; print "\n CMD: $cmd\n"; system ($cmd)==0 or print "ERROR: $cmd is failed\n";} @tools_src_dirs;
  
  # Copy common comp to  (sdk_wa)
  foreach my $comp (keys %common_comp2) {
    next unless $common_comp2{$comp};
    print "common_comp2: $comp\n";
    $cmd = "xcopy $repository_area\\$comp $common_comp2{$comp}\\ /f /r /i /e /Y";  
    system ($cmd)==0 or print ($cmd."\n");
  } 

  #Beautify the blacklists, i.e. remove comments etc.
  map (beautify_remove_list($_), grep {-e} @remove_lists);
   
  # Remove Symbian_List.TXT, Series60 blacklist.TXT and graylist items from SDK_wa
#?  apply_Symbian_blacklist("\\sdk_wa", $symbianlist);
  apply_blacklist("\\sdk_wa", ($blacklist, $common_graylist, $cpp_graylist, $java_graylist));
#  apply_blacklist("\\sdk_wa", ($blacklist, $common_graylist, $cpp_graylist));
  
  # Copy the paches after blacklist is applied
  # Copy common comp to  (sdk_wa)
  foreach my $comp (keys %common_comp3) {
    next unless $common_comp3{$comp};
    print "common_comp3: $comp\n";
    $cmd = "xcopy $repository_area\\$comp $common_comp3{$comp}\\ /f /r /i /e /Y";  
    system ($cmd)==0 or print ($cmd."\n");
  } 

#  print "Copy common patch to $common_comp3{Common_Patch} === ", scalar localtime(), "\n";
#  $cmd = "xcopy $repository_area\\Common_Patch $common_comp3{Common_Patch}\\ /f /r /i /e /Y";
#  system ( $cmd ) if ($common_comp3{Common_Patch});
  
#  print "Copy common tools to $final_dest_dirsH2{Common_Tools} === ", scalar localtime(), "\n";
#  $cmd = "xcopy $repository_area\\Common_Tools $final_dest_dirsH2{Common_Tools}\\ /f /r /i /e /Y";
#  system ( $cmd ) if ($final_dest_dirsH2{Common_Tools});
  
#Lisennoijapaketissa ei vaihdeta rekisteröintikomponenttia
  #Rename Ecmtmenubar2.dll, 
#  (-e "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR2.DLL") 
#  && (system ("del /q /s \\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR2.DLL")==0);
#  && (system ( "rename \\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR2.DLL ECMTMENUBAR.DLL" )==0) 
#  and print "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR2.DLL >-> ECMTMENUBAR.DLL\n";
  #Rename Ekern2.dll
#  (-e "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern2.dll")
#  && (system ("del /q /s \\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern.dll")==0)
#  && system ("rename \\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern2.dll Ekern.dll")==0 
#  and print "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern2.dll >-> Ekern.dll\n";

#muuten vaihdetaan
  #Rename Ecmtmenubar2.dll, 
  (-e "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR2.DLL") 
  && (system ("del /q /s \\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR.DLL")==0)
  && (system ( "rename \\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR2.DLL ECMTMENUBAR.DLL" )==0) 
  and print "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\ECMTMENUBAR2.DLL >-> ECMTMENUBAR.DLL\n";
  #Rename Ekern2.dll
  (-e "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern2.dll")
  && (system ("del /q /s \\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern.dll")==0)
  && system ("rename \\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern2.dll Ekern.dll")==0 
  and print "\\sdk_wa\\epoc32\\release\\winscw\\udeb\\Ekern2.dll >-> Ekern.dll\n";
 
  
  #Strip debug info on SDK_wa
  my @dgbstrip_these_dirs =  grep {( -e )} map {"\\sdk_wa\\$_"}  @dirs_to_dbgstrip;            #Add the sdk_wa prefix first
  print "Stripping dlls ", join " ", @dgbstrip_these_dirs, " === ", scalar localtime(), "\n";
  find \&dbgstrippi, @dgbstrip_these_dirs;
  
  replace_strings();
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
}

sub create_sdk_files {
  enter_sub();

  #Delete SDK_files
  my $target_dir = "$iniH{Sce_Root}\\SDK_files";
  print "Deleted dir  - $target_dir\n" if ((-e $target_dir) && do_rmdir_all($target_dir));

  my $curr_dir = cwd();
  chdir "$iniH{Sce_Root}\\" or die "Couldn't chdir to $iniH{Sce_Root}\\: $!";

  print "Copy components to final destination === ", scalar localtime(), "\n";
  foreach $dir (keys %final_dest_dirsH) {
    next unless ($final_dest_dirsH{$dir});
    print "Comp: $dir\n";
    my $cmd = "xcopy $repository_area\\$dir $final_dest_dirsH{$dir}\\ /F /R /I /E /Y";
    system ($cmd)==0 or print "$cmd\n";
  }
  
  # Ensure Nokia_EULA.txt in Synergy:S60_SDK_Docs\hybrid\External is copied to SDK_files\docs
  #        as S60_SDK_Docs\hybrid\External\Nokia_EULA.txt was removed to ensure only one
  #        EULA exists in the SDK build system
  my $cmd = "copy /V \"$iniH{Doc_External}\\Nokia_EULA.txt\" \"$target_dir\\docs\\\"";
  print ($cmd."\n"); system ($cmd);
  
  print "\n Copying SDK_wa to \\SDK_files === started ", scalar localtime(), "\n";
  #We take into toolchain everything but emulator dirs and ecmt
  $cmd = "xcopy $iniH{Sce_Root}\\sdk_wa $iniH{Sce_Root}\\SDK_files\\epoc32_toolchain_cpp\\ /f /r /i /e /Y /EXCLUDE:$init_dir\\tools\\Emu_dirs.txt ";
  system ($cmd)==0 or print "$cmd\n";
  map { system ("xcopy $iniH{Sce_Root}\\sdk_wa\\epoc32\\$_ $iniH{Sce_Root}\\SDK_files\\epoc32_emu\\epoc32\\$_ /f /r /i /s /Y " )} @emu_src_dirs;
  map { system ("xcopy $iniH{Sce_Root}\\sdk_wa\\epoc32\\$_ $iniH{Sce_Root}\\SDK_files\\epoc32_toolchain_common\\epoc32\\$_ /f /r /i /s /Y " )} @ecmt_dirs;

  print "\n Copy midpODDUtility.sis and Disclaimer\n ";
  $cmd = "copy /v $iniH{S60_Root}\\Series60SDK_dev\\Series60_tools\\Ecmt\\midpODDUtility.sis $iniH{Sce_Root}\\SDK_files\\epoc32_toolchain_common\\epoc32\\tools\\ecmt\\";
  system ($cmd)==0 or print "$cmd\n";
  $cmd = "xcopy /V /C /I /F /R $iniH{S60_Root}\\PublicSDK\\sdkcreationmw\\sdkconnectivityfw\\series60_tools\\MIDP\\Disclaimer $iniH{Sce_Root}\\SDK_files\\epoc32_emu\\epoc32\\release\\winscw\\udeb\\z\\data\\disclaimer";
  system ($cmd)==0 or print "$cmd\n";
    
  #Copy language zips
  $cmd = "xcopy $iniH{Sce_Root}\\sdk_lang $iniH{Sce_Root}\\SDK_files\\epoc32_toolchain_common\\ /f /r /i /e /Y ";
  system ($cmd)==0 or print "$cmd\n";

  if (-e "$repository_area\\CPP_API\\sdk_apis.txt")
  {
    collect_apis( "$repository_area\\CPP_API\\sdk_apis.txt", \%lib_dirs_to_collect_from_platform, \%iniH, "$iniH{S60_Root}\\epoc32", "$iniH{Sce_Root}\\SDK_files\\epoc32_api");
  } 
  else 
  {
  	
    my $sea_dir = "$iniH{S60_Root}";
    my $sdk_dest_dir = "$iniH{S60_Root}\\sce\\SDK_files\\epoc32_api";
    #my $parser4coll = new XML::Parser(Style=>'Subs', Pkg=>'SH4ApiCollection', ErrorContext => 2);
    my $apis_xml="$sea_dir\\PublicSDK\\sdkcreationmw\\SDKComponents\\CPP_API\\apis.xml";
    
    initialize_gcc_part($sea_dir, \%gcc_part, \$platform_version);
    #collect_S60_apis("$repository_area\\CPP_API\\apidescr.xml", \$parser4coll, $sea_dir, $sdk_dest_dir, \%gcc_part, $platform_version);
    
    Collect_S60_apiset($sea_dir,$apis_xml,$sdk_dest_dir,$platform_version,\%lib_dirs_to_collect_from_platform);
    collect_apis( "$repository_area\\CPP_API\\sdk_apis_symbian.txt", \%lib_dirs_to_collect_from_platform, \%iniH, "$iniH{S60_Root}\\epoc32", "$iniH{Sce_Root}\\SDK_files\\epoc32_api");  
  }


  print "Filenames to lowercase on \\sdk_files\\epoc32_toolchain_common === started ", scalar localtime(), "\n";
  find \&lowercase, qw(\\SDK_files\\epoc32_api \\sdk_files\\epoc32_toolchain_common \\sdk_files\\epoc32_emu);  
  print "Filenames to lowercase === finished ", scalar localtime(), "\n";

  
#  apply_blacklist(("\\SDK_Java\\SDK_files\\bin"), ($java_graylist));
  
  #Copy run-time JVM directory back to sdk (it is removed by java graylist) 
  my $cmd = "xcopy $iniH{S60_Root}\\epoc32\\release\\WINSCW\\udeb\\Z\\resource\\ive\\lib\\jclcldc11\\ext\\odc\\*.* $iniH{Sce_Root}\\SDK_files\\epoc32_emu\\epoc32\\release\\WINSCW\\udeb\\Z\\resource\\ive\\lib\\jclcldc11\\ext\\odc\\ /F /R /I /Y /EXCLUDE:$init_dir\\tools\\Emu_dirs.txt";
  system ($cmd)==0 or print "$cmd\n";

  # Copy patches to final destination area
  #The copying is done in 2 phases to ensure the proper order of the copy (patches last; hash table does not preserve the order)
  foreach $dir (keys %final_dest_dirsH2) {
    next unless ($final_dest_dirsH2{$dir});
    print "Comp: $dir\n";
    my $cmd = "xcopy \"$repository_area\\$dir\" \"$final_dest_dirsH2{$dir}\\\" /F /R /I /E /Y";
    system ($cmd)==0 or print "$cmd\n";
  }
   
  # Added by gvn
  my $openc_plugin_dir = "$iniH{Sce_Root}\\sce\\SDK_files\\OpenC_Plugin";
  my $cmd = "unzip -o $repository_area\\CPP_OpenC_Plugin\\glib.zip -d $openc_plugin_dir";
  print ($cmd."\n"); system ($cmd);
  print "\n\n Open C plugin directory: $openc_plugin_dir\\glib.zip \n\n";
  if ( -f "$openc_plugin_dir\\glib.zip" ) {
  	unlink "$openc_plugin_dir\\glib.zip";
  } 

  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
}

sub run_installer {
  enter_sub();
  my $curr_dir = cwd();
  my $cmd;
  print "Building installer === started ", scalar localtime(), "\n";
  chdir "$iniH{Sce_Root}\\" or print "Couldn't chdir to $iniH{Sce_Root} - $!", return;
  
  replace_strings_common();

  my $result_dir = "$iniH{Sce_Root}\\SDK_installer\\Media";
  print "Deleted dir  - $result_dir\n" if ((-e $result_dir) && do_rmdir_all($result_dir));
  $cmd = "\"C:\\Program Files\\Macrovision\\IS 12 StandaloneBuild\\IsSABld.exe\" -r $iniH{ Release } -p $iniH{Sce_Root}\\SDK_installer\\installer.ism";
  print  ( $cmd."\n" );
  system ( $cmd );

  #Zip them
  print "\nZipping bundle === started ", scalar localtime(), "\n";

  chdir ("$iniH{Sce_Root}\\SDK_installer\\Media\\$iniH{ Release }\\Disk Images\\Disk1") or die "Couldn't chdir to $iniH{Sce_Root}\\SDK_installer\\Media\\$iniH{ Release }\\Disk Images\\Disk1: $!";

  my $output_dir =  "\\Output";
  print "Deleted dir  - $output_dir\n" if ((-e $output_dir) && do_rmdir_all($output_dir));
   mkpath($output_dir);#Unzip doesn't create deep direcories
  
  # Copy external files to the directory which is to be zipped
  foreach $dir (keys %root_levelH3) {
    next unless ($root_levelH3{$dir});
    print "Comp: $dir\n";
    $cmd = "xcopy \"$repository_area\\$dir\" \"$root_levelH3{$dir}\\\" /F /R /I /E /Y";
    print ($cmd."\n"); system ($cmd);
  }
  # zip_name Ex: Symbian ^ 2-SDK-Rel200850-2676.zip
  my $zip_name = "$iniH{Platform_Abbreviation} $iniH{Edition_number}-SDK-$iniH{Platform_Build_ID}$iniH{Build_ID}";
  del $zip_name if (-e "$zip_name.zip");
  $cmd = "zip -r  ".'"'."\\Output\\$zip_name.zip".'"'."  *.*";  
  print ($cmd."\n"); system ($cmd);                                                

  #Copy IF sheets to output directory
  $cmd = "xcopy $iniH{Sce_Root}\\$IFs\\*.zip $iniH{Sce_Root}\\Output\\SDK_team_only /F /R /I /Y";
  print "\n EXEC: $cmd";
  system ($cmd)==0 || print $cmd."\n";
  #print "Deleted dir  - $IFs\n" if ((-e $IFs) && do_rmdir_all($IFs));
  
  #Copy mataxml and api info to output directory
  $cmd = "xcopy $iniH{Sce_Root}\\Repository_area\\CPP_API\\*.* $iniH{Sce_Root}\\Output\\SDK_team_only /F /R /I /Y";
  print "\n EXEC: $cmd";
  system ($cmd)==0 || print $cmd."\n";
 
  #Calculate checksums
  print "\n Calculating CRC for $iniH{Sce_Root}\\SDK_files/";
  calc_checksums("crc","$iniH{Sce_Root}\\SDK_files/", "$iniH{Sce_Root}\\Output\\$zip_name");
  print "\n CRC is calculated.";
    
#  print "Building SDK installer === finished ", scalar localtime(), "\n";
  print "$iniH{ SDK_Product_name } $iniH{ Platform_Version } $iniH{Platform_Build_ID}-$iniH{ Build_ID} Hybrid SDK created.\n";  
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
  exit_sub();
} 

sub publish_sdk {
  enter_sub();
  my $curr_dir = cwd();
  chdir "$iniH{Sce_Root}\\" or warn "Couldn't chdir to $iniH{Sce_Root} - $!", return;
  my $cmd ="xcopy $iniH{Sce_Root}\\Output $iniH{Output_Base}\\$iniH{Release}\\$iniH{Platform_Version_Major}.$iniH{Platform_Version_Minor}\\$iniH{Start_time}\\  /f /r /i /e /Y";
  
  rm_old_dirs("$iniH{Output_Base_substed}\\$iniH{Release}\\$iniH{Platform_Version_Major}.$iniH{Platform_Version_Minor}", $iniH{Nbr_of_nbs_to_keep});
  do {system ($cmd)==0 or print "$cmd\n"} if $iniH{Output_Base};
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";

  #system ($cmd)==0 or print "$cmd\n";
}
1;

