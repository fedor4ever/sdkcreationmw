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
package log_collector;
use SDK;
use SDK_vars;
use Remote_utils;
use SDK_utils;

our ($VERSION, @ISA, @EXPORT);
our ($log_path, $body, $sdk_zip, $sdk_dir, $log_file);

$VERSION = 1.00;
@ISA = qw(Exporter);
@EXPORT = qw(collect_sdk_logs collect_cw_logs form_mail $log_path $sdk_zip $sdk_dir $log_file);

sub collect_sdk_logs {
  my ($full_log_file) = @_ or qw( S60_create_sdk.log );

  parse_ini_file("sdk.ini", \%iniH);
  if (-f "cmd.ini"){
		parse_ini_file("cmd.ini", \%iniH);
	}

  gather_sdk_logs($full_log_file);
  
   
  $message2 = "SDK build/package    $iniH{S60_Root_Path}\\$iniH{Start_time} \n\n";
  
  open (INPUT, $full_log_file)       or die "can't open $full_log_file: $!";
  
  while (<INPUT>){
  
    $synergy_wa = "$_" if /^Synergy WorkArea:/;  #Sync with reconfigure
    
    if ( /^Starting reconfigure process/ ... /^Reconfigure complete/) { #Reconfigure stuff is between these
      $reconf_log .= $_ if /replaces/;
    }
    
    if ( /^Recently completed tasks \(since/ ... /^End of recent tasks./) { # Tasks are between these
      $tasks .= $_;
  
      # Check if task name contains error report name in form [aaaa-bbbb]
      if ( /[[](.)*[-](.)*[]]/ ) {
        s/^(.*)\[(.*)$/$2/m;
        s/\]//;
        $error_reports .= $_;
      }
    }
    if ( /^Conflicts Start/ ... /^Conflicts End/) { # conflicts are between these
      $conflicts .= $_;
    }
    
    #  Platform patches
    if ( /^===>  SDK::copy_platform_patch/ ... /^<===  SDK::copy_platform_patch/) { #Patches are between these
      push @platform_patches, $_;
    }
    if ( /^===>  SDK::platform_patch_duplicates/ ... /^<===  SDK::platform_patch_duplicates/) { #Patch duplicates are between these
      if ( not /===/) {
        $platform_patches_duplicates .= $_;
      }
    }
  
    # MIDLets
    if ( /Building MIDLETS/ ... /Done building MIDLETS/) { #MIDlets built between these
    
      if ( /CURRENT MIDLET IS / ... /preverify/) {
        if (/CURRENT MIDLET IS /) {
          $midlet = $_;
          $midlet =~ s/\[echo\] CURRENT MIDLET IS //;
          $midlets .= $midlet;
        }
        if (/error/) {
          $midlet_errs .= "$midlet:";
          $midlet_errs .= "$_ <br>";
        }
      }
      if ( /checkbuild/ ... /Done building MIDLETS/) {
        if ( /WARNING/) {
          $midlet_checkbuild = $_;
          $midlet_checkbuild =~ s/\[exec\] //;
          $midlet_errs .= $midlet_checkbuild;
        }
      }
    }
  
    if ( / Cmd line\: sdk_build.pl .*\-e/ ... /^\*\*\* Build script finished at /) { #RnD stuff is build between these
      $em_int_errors .= $_ ;
    }
    $create_errors .= $_ if /^ERROR!/;
    #Modified to match Symbian SDK name
    $edition_zip_name = quotemeta('Nokia 9.2');
    #Grab what was copied
    if ($_=~m/-> (\Q$iniH{Output_Base}\E.+$edition_zip_name-SDK.+)/i)
    {

	    print "\n *** BAKKI - Match: $1";
	     push (@copied , $1);
     }
  	#...and the components
    if (m"^.{6}DIR:   $iniH{S60_Root}/.+/"i){
      s/\/(Plugins|group|generic\/base).*$//i;
      m".+/(.+)$";
      $components{$1}++;
    }
  }
  close INPUT;
  
  #If we didn't find any platform patches, say something anyway
  $platform_patches = join "", grep {/ -> /} @platform_patches;  #Take only the zips 
  $platform_patches = "None" unless $platform_patches;
  $platform_patches_duplicates = "None" unless $platform_patches_duplicates;
  $create_errors = "None" unless $create_errors;
  $tasks = "None" unless $tasks;
  $conflicts = "None" unless $conflicts;
  $error_reports = "None" unless $error_reports;
  $midlets = "None" unless $midlets;
  $em_int_errors = "None" unless $em_int_errors;
  $components = "None" unless $components;
  
  my @copied_zips = grep {/\.zip/} @copied;
  print "\n ***** BAKKI:  $copied_zips[$#copied_zips] ";

  $copied_zips[$#copied_zips] =~ /(.+?)\\([^\\]+)$/;
  
  $sdk_dir = $1;
  $sdk_zip = $2 or "SDK nb log ";
  print "\n *** SDK_zip: $sdk_zip";

  $full_log_file =~  /(.+?\\)([^\\]+)$/;
  $log_path = $1 or "Logs";
  my $full_log_file2 = "$1$iniH{Start_time}_$2";
  
  unless ($full_log_file2 =~ /_\d+\./) { 
    $full_log_file2 =~ s/(.+)(?=\.)/$1_1/; 
  }
  
  while (-e "$full_log_file2") {
      next if ($full_log_file2 =~ s/(?<=_)(\d+)(?=\.)/1+$1/e);  #Increment the number by one
      $full_log_file2 =~ s/(.+)(?=\.)/$1_1/;
      last;
  }
  
  $full_log_file2 =~  /(.+?\\)([^\\]+)$/;
  $log_file = $2;
  
  #Put the components in scalar variable
  $comps = join "\n", sort (keys %components);
  
 
  my $cmd = "copy \"$full_log_file\"  \"$full_log_file2\"";

  print ((system ($cmd)==0) ?  "$cmd  , copied\n" : "$cmd failed!\n");
  
  my $cmd = "xcopy \"$full_log_file2\" \"$iniH{Output_Base}\\logs\\\" /f/r/y"; 
  do {print ($cmd."\n"); system ($cmd);} if $iniH{Output_Base};
}

sub collect_cw_logs {
  open (INPUT, "$iniH{S60_Root}\\cw\\report_winscw.tsv")       or die "can't open $iniH{S60_Root}\\cw\\report_winscw.tsv: $!";

  while (<INPUT>){
    if ( /^MISSING HEADERS/ ... /^MISSING LIBS/) { #Missing headers
      $cw_headers .= "$_<br/>" unless /MISSING LIBS/;
    }
    if ( /^MISSING LIBS/ ... /^Total cases tested/) { #Missing libs
      $cw_libs .= "$_<br/>" unless /Total cases tested/;
    }
    if ( /^Total cases tested/ ... eof()) { #Summary
      $cw_summary .= "$_<br/>";
    }
  }
  close INPUT;
}

sub get_Symbian_rel_info {
  my $readme_file = "$iniH{ S60_Root }\\readme.txt";
  open (FH, "< $readme_file") or return "No Symbian info, $readme_file not found";
  do { $line = <FH> }  until $line =~ /^Symbian OS.+/ || do {$eof = eof};  #Read until Symbian OS -line is found
  $line .= do {local $/ = ""; <FH>;} unless $eof;
  close FH;
  return "No Symbian info in  $readme_file" if $eof;
  return $line;
} 

sub gather_sdk_logs {
    my ($full_log_file) = @_;

    $path = "phase";
    $IDX = 'mtime';

    # Find files that have log as extension but the extension is not
    # preceded by .<number>. since those are previous versions of the log.
    opendir(DIR, $path) or die "can't opendir $path: $!";
    @files = readdir(DIR);
    closedir(DIR);

    foreach (@files) {
        ($time{$_}) = (stat("$path\\$_"))[9];
    }

    # Sort files according to their modification time and reverse the list
    # so that oldest file is listed first.
    @sfiles = sort { $time{$b} <=> $time{$a} } keys %time;
    @sfiles = reverse @sfiles;
    print @sfiles;

    open (LFH, ">$full_log_file");
    foreach (@sfiles) {
        open(FH, "<$path\\$_");
        while (<FH>) {
            print LFH $_;
        }
        close FH;
    }
    close LFH;
}

sub form_mail {
   $body = <<END_HTML;
  <h2>$sdk_zip nb build note</h2>
  
  <ul class="navbar">
    $links
  </ul>
  
  <p class="navbar">
    <a href="#tasks">Tasks</a>
    &nbsp; | &nbsp; 
    <a href="#conflicts">conflicts</a>
    &nbsp; | &nbsp; 
    <a href="#platform">Platform patches</a>
    &nbsp; | &nbsp; 
    <a href="#midlets">MIDLets</a>
    &nbsp; | &nbsp; 
    <a href="#symbianinfo">Symbian info</a>
    &nbsp; | &nbsp; 
    <a href="#integration">Integrated components</a>
    &nbsp; | &nbsp; 
    <a href="#emulator">Emulator integration errors</a>
    &nbsp; | &nbsp; 
    <a href="#creation">Create errors</a>
    &nbsp; | &nbsp; 
    <a href="#reports">Corrected error reports</a>
END_HTML

  # Check if classwalker should be shown or not
  if ($cw_headers or $cw_libs or $cw_summary) {
    $body .= <<END_HTML;
    &nbsp; | &nbsp; 
    <a href="#cw">Classwalker results</a>
END_HTML
  }

  $body .= <<END_HTML;
  </p>
  <p><a name="tasks">Tasks</a></p>
  <pre class="tasks">
    $tasks
  </pre>
  
  <p><a name="conflicts">conflicts</a></p>
  <pre class="conflicts">
    $conflicts
  </pre>
  
  <p><a name="platform">Platform patches</a></p>
  <pre class="platform_patches">
    $platform_patches
  </pre>
  <p>Duplicate patches:</p>
  <pre class="platform_patches">
    $platform_patches_duplicates
  </pre>
END_HTML

  if ("$midlets") {
  $body .= <<END_HTML;
  
  <p><a name="midlets">MIDLets</a></p>
  <div class="components">
      $midlets
  </div>
  <div class="errors">
      $midlet_errs
  </div>
END_HTML
  }

  my $symbian_info =   get_Symbian_rel_info();

  $body .= <<END_HTML;
  
  <p><a name="symbianinfo"></a>Symbian info</p>
  <div class="components">
    $symbian_info
  </div>
  
  <p><a name="integration">Integrated components</a></p>
  <div class="components">
    $comps
  </div>
  
  <p><a name="emulator">Emulator integration errors</a></p>
  <pre class="errors">
  $em_int_errors
  </pre>
  
  <p><a name="creation">Create errors</a></p>
  <pre class="errors">
  $create_errors
  </pre>
  
  <p><a name="reports">Corrected error reports</a></p>
  <pre class="error_reports">
  $error_reports
  </pre>
END_HTML

  # Check if classwalker should be shown or not
  if ($cw_headers or $cw_libs or $cw_summary) {
    $body .= <<END_HTML;
    <p><a name="cw">Classwalker results</a></p>
    <div class="cw_headers">
    $cw_headers
    </div>
    <div class="cw_libs">
    $cw_libs
    </div>
    <div class="cw_summary">
    $cw_summary
    </div>
END_HTML
  }

  rm_old_dirs("$iniH{S60_Root_Path}", $iniH{Nbr_of_envs_to_keep});
  
  $cmd = "zip -u -j $iniH{Start_time}_sce.zip *_tm.pl $0 *.pm *.cmd sdk.ini $full_log_file2";
  system ($cmd);
  $cmd = "xcopy $iniH{Start_time}_sce.zip \"$iniH{Output_Base}\\Hybrid\\$iniH{Platform_Version_Major}.$iniH{Platform_Version_Minor}\\$iniH{Start_time}\\SDK_team_only\\\" /f/r/y"; 
  do {print ($cmd."\n"); system ($cmd);} if $iniH{Output_Base};
}
1;
