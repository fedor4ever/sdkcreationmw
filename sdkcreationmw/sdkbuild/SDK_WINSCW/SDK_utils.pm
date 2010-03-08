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

package SDK_utils;

use Exporter;
use File::Path;
use SDK_vars;
use Cwd;
use XML::DOM;
use File::Basename;
use File::Copy;
use File::Path;

our ($VERSION, @ISA, @EXPORT);

$VERSION = 1.00;
@ISA = qw(Exporter);
@EXPORT = qw(apply_blacklist apply_Symbian_blacklist beautify_remove_list clean_previous_wa collect_apis collect_symbian_headers collect_symbian_libs dbgstrippi do_rmdir_all enter_sub exit_sub    
             fix_and_copy getBuildNo get_Symbian_OS_info initialize_gcc_part lowercase remove_target replace_strings  replace_strings_common replace_strings_RnD repl_str repl_str2 repl_str3 rm_old_dirs
             start_target strip_and_copy tidy_up record_time Collect_S60_apiset);

#Global vars
my $S_time;
my $E_time;
sub record_time
{
	my $action=shift @_;
	my $target= shift @_;
	my $phase_dir=shift @_;
	
	$target=~s/phase\\//;
	
	if($target eq "initialize.log")
	{
		#Build is started, clean and Print CSV header
		open (TF,">$phase_dir/phase_duration.csv") or print ("ERROR: Can not open $phase_dir/phase_duration.log, $!");
		print TF "Build_Phase,Duration,Start,Finish";
		close (TF);
	}
	open (TF,">>$phase_dir/phase_duration.csv") or print ("ERROR: Can not open $phase_dir/phase_duration.log, $!");
	if ($action=~m/start/i)
	{
		$S_time=time;
		
	}
	elsif($action=~m/end/i)
	{
		$E_time=time;
		my $D_time=$E_time-$S_time;
		printf TF "\n%s,%.2d:%.2d:%.2d,%s,%s",$target,(gmtime $D_time)[2,1,0],scalar localtime($S_time),scalar localtime($E_time);
	}
	close (TF);
}

sub start_target 
{
  my ($phase_target, $logs_dir) = @_;
  redirect($phase_target, $logs_dir);
  print "\nStart $phase_target  ", scalar localtime(), "\n";
}

sub redirect {
  my ($phase_target, $logs_dir) = @_;

  $_ = $phase_target;
  s/^(.*)\\(.*)/$1\\$2/;
  my $targetname = $2;
  # Check if previous log exists
  if ( -e "$phase_target" ) {
      my $i = 1;
      while ( -e "${logs_dir}\\$targetname.$i.log" ) {
          $i++;
      }
      rename "$phase_target", "${logs_dir}\\$targetname.$i.log";
  }

    $|=1;
  open(STDOUT, ">$phase_target");
  open(STDERR, ">&STDOUT");
}

# This routine is called from 0_start to move existing target log file into log folder
# under new name with latest index
sub remove_target {
  my ($phase_target) = $ARGV[0];
  $_ = $phase_target;
  s/^(.*).log/$1/;
  my $targetname = $1;
  if ( -e "phase\\$phase_target" ) {
      my $i = 1;
      while ( -e "logs\\$targetname.$i.log" ) {
          $i++;
      }
      rename "phase\\$phase_target", "logs\\$targetname.$i.log";
  }
}

# Check phase directory through for target files from previous run and move them with new name
# to log directory.
sub tidy_up {
  my ($phase_dir, $logs_dir) = @_;
  my $file;
  opendir (DIR, ${phase_dir}) or die "Couldn't open directory, $!";
  while ($file = readdir DIR)
  {
    next if ( $file eq "." or $file eq "..");
    $_ = $file;
    s/^(.*).log/$1/;
    my $filename = $1;
    # Check if previous log exists
    if ( -e "${phase_dir}\\$file" ) {
        my $i = 1;
        while ( -e "${logs_dir}\\$filename.$i.log" ) {
            $i++;
        }
        rename "${phase_dir}\\$file", "${logs_dir}\\$filename.$i.log";
    }
  }
  close DIR;
}

sub beautify_remove_list {
  my $file = shift;
  enter_sub($file);
#  print "Beautify_remove_list $file === ",  scalar localtime(), "\n";
  open(OLD, "< $file")        || die "can't open $file: $!";
  open(NEW, "> bl.txt")       || die "can't open bl.txt: $!";
  while (<OLD>) {
    chomp;
    s/^#.*//;           #Remove comments
    s/(?<= )#.*//;     #Remove comments
    s/\//\\/g;          # / <- \
        s/\s+$//;           #Remove trailing spaces
        s/^\\//;            #Remove \ at the beginning of the line
        s/\\$//;            #Remove \ at the end of the line
#       s/.*\\([^\\]+)/$1/g ;  #Leave only filename, remove the path
    print NEW "$_\n" if length;
  }
  close(OLD);
  close(NEW);
  rename("bl.txt", $file)       || die "can't rename bl.txt to $file: $!"; 
}

sub collect_symbian_headers {
  my ($api_file, $href_dirs_to_collect, $href_iniH, $sea_dir, $dest_dir) = @_;  
  enter_sub($api_file);
  
  #Find out the platform version for set of LIB directories to copy
  my $platform_version =  (exists $$href_dirs_to_collect{$$href_iniH{ Platform_Version }}) ? $$href_iniH{ Platform_Version } : "9.9";


  open (API, $api_file) or die "Couldn't open $api_file: $!";

  while (<API>) {
    chomp;
    s/\//\\/g;
    m/([^\\]+)$/;           #We extract subdir and file parts into $` and $1 respectively
    my $subdir = $`;        #This is done for xcopy to know which is directory name and which is filename
    my $file = $1;          #otherwise it will ask it every time!
    my $file_trunk = substr($file,0,rindex($_,'.'));
    my $cnt=0;

    if (/.\.(h|rh|hrh|inl|lag|loc|rsg|mbg|pal|pan)$/i ){
      foreach my $sub_root ( @sub_roots ){                                                  # defined in SDK_vars.pm
        (-e "$sea_dir\\include$sub_root$subdir$file")
        && (system ("xcopy $sea_dir\\include$sub_root$subdir$file $dest_dir\\epoc32\\include$sub_root$subdir /f /r /i /Y")==0)
        && $cnt++;
      }
      ($cnt > 0) or  print "ERROR!!! HEADER NOT COPIED IN EPOC32\\INCLUDE: $subdir$file\n"; 
    } else {
      print "Unknown file type: $subdir$file\n";
    }
  } 
  close( API );

=head
  #Generate real file list of include dir
  open (SDK_HDR_FILE_LIST, "> $S60_Root\\sdk_hdr_file_list.txt") or die "Can't create $S60_Root\\sdk_hdr_file_list.txt: $!";
  open( SDK_HEADER_FILES, "dir /s /a-d /b $dest_dir\\epoc32\\include |") or die "Cannot open $dest_dir\\epoc32\\include: $!";
  while (<SDK_HEADER_FILES>){
    print SDK_HDR_FILE_LIST;
    do {
      my $target_dir = $_;
      ($target_dir =~ s/osextensions\\//i)       #Remove osextensions level
      || ($target_dir =~ s/oem\\//i)             #Remove oem level
      || ($target_dir =~ s/middleware\\//i)
      || ($target_dir =~ s/applications\\//i);
      $target_dir =~ /(.+\\)/;                  #Take path\ -> $1
      system ("xcopy $_  $1 /f /r /i /Y");      #Copy middleware upwards
    } if (/applications\\/i || /middleware\\/i || /oem\\/i || /osextensions\\/i);                               
  }
  close( SDK_HEADER_FILES );
  close( SDK_HDR_FILE_LIST );

  #delete subroots
  for (my $i=1; $i < @sub_roots; ++$i){
    my $subdir = "$dest_dir\\epoc32\\include\\$sub_roots[$i]";
    print "Deleted dir  - $subdir\n" if do_rmdir_all($subdir);
  }

    #Copy the sdk_special directory on top of include. Note: a file does not need to be on a sheet now. 
    my $cmd = "xcopy $sea_dir\\sdk_special $dest_dir\\epoc32\\include\\ /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");
=cut

}

sub collect_symbian_libs {
  my ($api_file, $href_dirs_to_collect, $href_iniH, $sea_dir, $dest_dir) = @_;  
  enter_sub($api_file);
  
#  print "Collect_apis from $sea_dir === started ", scalar localtime(), "\n";

  #Find out the platform version for set of LIB directories to copy
  my $platform_version =  (exists $$href_dirs_to_collect{$$href_iniH{ Platform_Version }}) ? $$href_iniH{ Platform_Version } : "9.9";

  #Generate the list of gcce specific part of the lib names (=%gcc_part), eg. {000a0000}, {00010000}
  #by scanning the wanted lib directories for {wwwwwwww}. pattern
  my $files ="";
  my %gcc_part = ();
  $gcc_part{""}++;                                                                        #Initialize with empty key
  foreach my $target (keys %{ $$href_dirs_to_collect{ $platform_version}}){               #e.g. winscw armi thumb
    foreach my $u_dir ( @{$$href_dirs_to_collect{ $platform_version}{$target}}){          #$u_dir = udeb or urel , see lib_dirs_to_collect_from_platform above
      opendir (DIR, "$sea_dir\\release\\$target\\$u_dir"); 
      while ( defined (my $file = readdir (DIR))){
        if ($file =~ /(\{\w{8}\})\./) {                                                   #e.g 3GPMP4Lib{000a0000}.dso {00020067}
            $gcc_part{$1}++;
        }
      }
      close(DIR);
    }
  } 
    
  open ($LIBS, $api_file) or die "Couldn't open $api_file: $!";
  
  while (<$LIBS>) {
    chomp;
    s/\//\\/g;
    m/([^\\]+)$/;           #We extract subdir and file parts into $` and $1 respectively
    my $subdir = $`;        #This is done for xcopy to know which is directory name and which is filename
    my $file = $1;          #otherwise it will ask it every time!
    my $file_trunk = substr($file,0,rindex($_,'.'));
    my $cnt=0;
    if (/.\.lib$/i) {
      foreach my $target (keys %{ $$href_dirs_to_collect{ $platform_version}}){               #e.g. winscw armi thumb
        foreach my $u_dir ( @{$$href_dirs_to_collect{ $platform_version}{$target}}){          #$u_dir = udeb or urel , see lib_dirs_to_collect_from_platform above
          foreach my $gcce_ext (keys %gcc_part) {
            foreach my $extension (qw ( .lib .dso )){
              ($file) = "$sea_dir\\release\\$target\\$u_dir\\$subdir$file_trunk$gcce_ext$extension";     
                    (-e "$file")                                                                                    #Check if the file exists
                  && (system ("xcopy $file $dest_dir\\epoc32\\release\\$target\\$u_dir\\$subdir /f /r /i /Y")==0)
                  && $cnt++;
              }
            }
            }
        }
        ($cnt > 0) or print "ERROR!!! LIB NOT COPIED $file_trunk\n";                            #We expect the lib to be found at least in one location
    } elsif (/.\.dll$/i) {                                                                    #dll or something
      $file = "$sea_dir\\release\\winscw\\udeb\\$subdir$file";     
        (-e "$file")                                                                          #Check if the file exists
      && (system ("xcopy \"$file\" \"$dest_dir\\epoc32\\release\\winscw\\udeb\\$subdir\" /f /r /i /Y")==0)
      && $cnt++;
        ($cnt > 0) or print "ERROR!!! DLL NOT COPIED $file_trunk\n";                          #We expect the lib to be found at least in one location
    }   else {
      print "Unknown file type: $subdir$file\n";
    }
  } 
  close( $LIBS );
}

sub collect_apis {
  my ($api_file, $href_dirs_to_collect, $href_iniH, $sea_dir, $dest_dir) = @_;  
  enter_sub($api_file);

    #Find out the platform version for set of LIB directories to copy
    my $platform_version =  (exists $$href_dirs_to_collect{$$href_iniH{ Platform_Version }}) ? $$href_iniH{ Platform_Version } : "9.9";

  #Generate the list of gcce specific part of the lib names (=%gcc_part), eg. {000a0000}, {00010000}
  #by scanning the wanted lib directories for {wwwwwwww}. pattern
  my $files ="";
  my %gcc_part = ();
  $gcc_part{""}++;                                                                        #Initialize with empty key
  foreach my $target (keys %{ $$href_dirs_to_collect{ $platform_version}}){               #e.g. winscw armi thumb
    foreach my $u_dir ( @{$$href_dirs_to_collect{ $platform_version}{$target}}){          #$u_dir = udeb or urel , see lib_dirs_to_collect_from_platform above
      opendir (DIR, "$sea_dir\\release\\$target\\$u_dir"); 
      while ( defined (my $file = readdir (DIR))){
        if ($file =~ /(\{\w{8}\})\./) {                                                   #e.g 3GPMP4Lib{000a0000}.dso {00020067}
            $gcc_part{$1}++;
        }
      }
      close(DIR);
    }
  } 
    
  open (API, $api_file) or die "Couldn't open $api_file: $!";
  
  while (<API>) {
    chomp;
    s/\//\\/g;
    m/([^\\]+)$/;           #We extract subdir and file parts into $` and $1 respectively
    my $subdir = $`;        #This is done for xcopy to know which is directory name and which is filename
    my $file = $1;          #otherwise it will ask it every time!
    my $file_trunk = substr($file,0,rindex($_,'.'));
    my $cnt=0;
    if (/.\.lib$/i) {
      foreach my $target (keys %{ $$href_dirs_to_collect{ $platform_version}}){               #e.g. winscw armi thumb
        foreach my $u_dir ( @{$$href_dirs_to_collect{ $platform_version}{$target}}){          #$u_dir = udeb or urel , see lib_dirs_to_collect_from_platform above
          foreach my $gcce_ext (keys %gcc_part) {
            foreach my $extension (qw ( .lib .dso )){
              ($file) = "$sea_dir\\release\\$target\\$u_dir\\$subdir$file_trunk$gcce_ext$extension";     
                    (-e "$file")                                                                                    #Check if the file exists
                  && (system ("xcopy $file $dest_dir\\epoc32\\release\\$target\\$u_dir\\$subdir /f /r /i /Y")==0)
                  && $cnt++;
              }
            }
            }
        }
        ($cnt > 0) or print "ERROR!!! LIB NOT COPIED $file_trunk\n";                            #We expect the lib to be found at least in one location
    }   elsif (/.\.(h|rh|hrh|inl|lag|loc|rsg|mbg|pal|pan)$/i ){
      do {
        my $ret = 0;                                                                          #Fix_and_copy status
        foreach my $sub_root ( @sub_roots ){                                                  # defined in SDK_vars.pm
#          ((-e "$sea_dir\\include\\$sub_root$subdir$file") && ($ret = fix_and_copy( "$sea_dir\\include\\$sub_root$subdir$file", "$dest_dir\\epoc32\\include\\$sub_root$subdir$file")));
          ((-e "$sea_dir\\include$sub_root$subdir$file") && ($ret = fix_and_copy( "$sea_dir\\include$sub_root$subdir$file", "$dest_dir\\epoc32\\include$sub_root$subdir$file")));
        }
        $ret;                                                                                 #Return the fix_and_copy status (of the last attempt)
      }
      || print "ERROR!!! HEADER NOT COPIED IN EPOC32\\INCLUDE: $subdir$file\n"; 
    } elsif (/.\.dll$/i) {                                                                    #dll or something
      $file = "$sea_dir\\release\\winscw\\udeb\\$subdir$file";     
        (-e "$file")                                                                          #Check if the file exists
      && (system ("xcopy \"$file\" \"$dest_dir\\epoc32\\release\\winscw\\udeb\\$subdir\" /f /r /i /Y")==0)
      && $cnt++;
        ($cnt > 0) or print "ERROR!!! DLL NOT COPIED $file_trunk\n";                          #We expect the lib to be found at least in one location
    }   else {
      print "Unknown file type: $subdir$file\n";
    }
  } 
  close( API );
 my $vg_api_dir = "Y\:\\SDK_files\\epoc32_api\\epoc32\\include\\osextensions\\vg";
  print "Deleted dir  - $vg_api_dir\n" if do_rmdir_all($vg_api_dir);
    
  #Generate real file list of include dir
  open (SDK_HDR_FILE_LIST, "> $sdk_root\\sdk_hdr_file_list.txt") or die "Can't create $sdk_root\\sdk_hdr_file_list.txt: $!";
  open( SDK_HEADER_FILES, "dir /s /a-d /b $dest_dir\\epoc32\\include |") or die "Cannot open $dest_dir\\epoc32\\include: $!";
  while (<SDK_HEADER_FILES>){
    print SDK_HDR_FILE_LIST;
    do {
      my $target_dir = $_;
      ($target_dir =~ s/osextensions\\//i)       #Remove osextensions level
      || ($target_dir =~ s/oem\\//i)             #Remove oem level
      || ($target_dir =~ s/middleware\\//i)
      || ($target_dir =~ s/applications\\//i);
      $target_dir =~ /(.+\\)/;                  #Take path\ -> $1
      system ("xcopy $_  $1 /f /r /i /Y");      #Copy middleware upwards
    } if (/applications\\/i || /middleware\\/i || /oem\\/i || /osextensions\\/i);                               
  }
  close( SDK_HEADER_FILES );
  close( SDK_HDR_FILE_LIST );

  #delete subroots
  for (my $i=1; $i < @sub_roots; ++$i){
    my $subdir = "$dest_dir\\epoc32\\include\\$sub_roots[$i]";
    print "Deleted dir  - $subdir\n" if do_rmdir_all($subdir);
  }

    #Copy the sdk_special directory on top of include. Note: a file does not need to be on a sheet now. 
    my $cmd = "xcopy $sea_dir\\sdk_special $dest_dir\\epoc32\\include\\ /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");

}


sub lowercase {
  rename ($_, lc($_));
}


sub do_rmdir_all {
  my $subdir = shift;
  if (-d $subdir) {
        rmtree([$subdir]);      
  }
}

sub getBuildNo {

  my ($server_host, $PORTNO) = @_;
  enter_sub($server_host);
  
  my ($sock, $msg, $port, $ipaddr, $hishost,
    $MAXLEN, $TIMEOUT);
  my $ret = "555-313";
  $msg = "";
  $MAXLEN = 64;
  $TIMEOUT = 5;
  
  $server_host = shift;
  $msg = "get";
  $sock = IO::Socket::INET->new(Proto     => 'udp',
                                PeerPort  => $PORTNO,
                                PeerAddr  => $server_host)
      or do { warn "Creating socket: $!\n"; system ("color 0C"); return $ret;};
  $sock->send($msg) or do { warn "send: $!\n"; system ("color 0C"); return $ret;};
  
  eval {
    local $SIG{ALRM} = sub { die "Alarm time out\n" };
 
    alarm 5;
    $sock->recv($msg, $MAXLEN) or do { warn "recv: $!\n"; system ("color 0C"); return $ret;};
    alarm 0;
    1;
  } or do { warn "recv from $server_host timed out after $TIMEOUT seconds.\n"; system ("color 0D"); return $ret;};
  

  print "BuildNo ", $msg." acquired from $server_host\n" || $ret." NOT acquired from $server_host\n" ;
  $msg  || $ret;
}
sub dbgstrippi{
  return if ((-d) || m/\.(abw|acl|aif|bat|bmp|bsc|bsf|ccc|cfg|cmd|cmi|cnf|conf|cre|css|dat|dbz|der|dic|dpf|dtd|esk|gdr|h
                         |html|ibw|icl|ilk|ini|jar|lib|lnk|log|m3g|mbm|mbw|mcl|mif|mk|mp3|nme|o0000|o0001|o0010|odc|orig|pdb|pdr|pl|pm|pod|policy|properties
                         |r\d+|rng|rsy|rsc|rtf|script|sel|sis|skn|smil|snm|spd|sps|tab|tok|ttf|txt|wav|xml|xpf|xsd|xsl|zip)$/ix);
  system ("dbgstrip", "$_");
}

sub apply_Symbian_blacklist {
  my ($target_dir, $symbianlist ) = @_;
  my $curr_dir = cwd();
  return unless (chdir $target_dir);
  print ("Remove read-only attribute: attrib /s $target_dir\\*.* -r\n");
  system ("attrib /s $target_dir\\*.* -r");

  open (SYMBIAN, $symbianlist) or do {print ("Couldn't open $symbianlist: $!: $_\n" ); system ("color 04"); chdir $curr_dir; return; };
  print "Apply_symbian_list $symbianlist to $target_dir === ", scalar localtime(), "\n";
  while(<SYMBIAN>){
    chomp;
    if( /^\\epoc32/i ) {
      if(/^\\epoc32\\include/i ) {
        unless ( /\.(h|rh|hrh|inl)$/i ) {    #We leave these files in epoc32\include, i.e. we put them into SDK
          ((-e "$target_dir$_" ) && system ("del /q /s $target_dir$_")==0) or print "Failed to delete  $target_dir$_\n";
        }
      }
      else {
        ((-e "$target_dir$_" ) && system ("del /q /s $target_dir$_")==0) or print "Failed to delete  $target_dir$_\n";
      }
    }
  }
  close SYMBIAN;
  chdir $curr_dir;
  1; 
}


sub apply_blacklist {
  my ($root_dir, @lists ) = @_;
  my $curr_dir = cwd();
  return unless (chdir $root_dir);
  foreach (@lists) {
    print "Apply blacklist $_ to $root_dir === ", scalar localtime(), "\n";
    open (BL, $_) or do {print ("$!: $_\n" ); system ("color 40"); next; };
    my @bl = <BL>;
    close (BL);
    system ("attrib /s $root_dir\\*.* -r");

    ### Remove black list items
    print ("Removing black list items...\n");
    foreach (@bl) {
        chomp;
      s/^(DIR|FILE) //;                            #To these harmonize list formats
      s/^\\+//;
#       s/#.*//;            # Remove comments
        s/^#.*//;           # Remove comments
        s/(?<= )#.*//;     # Remove comments
      next unless length; # anything left?
      my $target_item = "$root_dir\\$_";
      #Delete directories 
      if( -d  ) {
        print "Deleted dir  - $target_item\n" if do_rmdir_all($target_item);
      } 
      #Notify that (upper)directory does not exist  (One level up from item)
      elsif  ( not(-d (substr($target_item,0,rindex($target_item,'\\'))))) {
        print "Could Not Delete $target_item\n";
      }
      # Delete files that exist or has a star in the name
      elsif (-e "$target_item" or /\*/ ) {
        system ("del /q /s \"$target_item\"");
      }
      #Try anyway 
      else {
        system ("del /q /s \"$target_item\"");
      }
    }
  }
  chdir $curr_dir;
  1; #Do not remove
}  

sub replace_strings {

  enter_sub();
#  print "replace_strings === started ", scalar localtime(), "\n";

  #sdk_info.properties
  my $TargetFile="\\sdk_wa\\epoc32\\data\\sdk_info.properties";

  print repl_str("sdk_swbuild",  "$iniH{ Platform_Build_ID }" , $TargetFile);
  print repl_str("sdk_emulator_title",  "$iniH{ SDK_Product_name }" , $TargetFile);
  print "Replacing strings === finished ", scalar localtime(), "\n";  
}
 
sub replace_strings_RnD {
  enter_sub();
  my $TargetFile;
  my $curr_dir = cwd();

  #SdkProductInfo.h
  my $TargetDir = "$iniH{S60_Root}\\PublicSDK\\sdkcreationmw\\sdkruntimes\\emumenubar\\group";
  chdir "$TargetDir" or warn "Chdir to $TargetDir failed: $!";

  my $file = "SdkProductInfo.h";
  open(SDKPRODUCTINFO, "< $file")     or die "can't open $file: $!";
  open(TMP, "> tmp.txt")       or die "can't open tmp.txt: $!";
  undef (local $/);
  $_ = <SDKPRODUCTINFO>;
  my $replaced = s/(?<=#define SDK_PRODUCT_VERSION \")(.+)(?=\")/$iniH{ Platform_Version }/m;
  print "$file: #define SDK_PRODUCT_VERSION $1 -> $iniH{ Platform_Version }\n" if $replaced;
  my $replaced = s/(?<=#define SDK_PRODUCT_NAME_CPP\s\s\")(.+)(?=\")/$iniH{ SDK_Short_name }/m;
  print "$file: #define SDK_PRODUCT_NAME_CPP $1 -> $iniH{ SDK_Short_name }\n" if $replaced;
  my $replaced = s/(?<=#define SDK_PRODUCT_NAME_MIDP\s\")(.+)(?=\")/$iniH{ SDK_Short_name }/m;
  print "$file: #define SDK_PRODUCT_NAME_MIDP $1 -> $iniH{ SDK_Short_name }\n" if $replaced;
  my $replaced = s/(?<=#define SDK_FULL_PRODUCT_NAME_CPP\s\")(.+)(?=\")/$iniH{ SDK_Product_name }/m;
  print "$file: #define SDK_FULL_PRODUCT_NAME_CPP $1 -> $iniH{ SDK_Product_name }\n" if $replaced;
  my $replaced = s/(?<=#define SDK_FULL_PRODUCT_NAME_MIDP\s\")(.+)(?=\")/$iniH{ SDK_Product_name }/m;
  print "$file: #define SDK_FULL_PRODUCT_NAME_MIDP $1 -> $iniH{ SDK_Product_name }\n" if $replaced;
  print TMP $_;
  close(SDKPRODUCTINFO);
  close(TMP);
  rename("tmp.txt", $file) or die "can't rename tmp.txt to $file: $!"; 
  
  chdir "$curr_dir"  or warn "Chdir to $curr_dir failed: $!";
}
sub  replace_strings_common {

  enter_sub();
  my $TargetFile;
  my $curr_dir = cwd();

  #Get a new build number if it not already specified
  if ( $iniH{ Build_number } ) {
    $iniH{ Build_ID } .= $iniH{ Build_number };
  }
  else {
     $iniH{Build_ID}.=getBuildNo($iniH{Build_ID_Server},1234);
  }

  my $TargetDir = "\\SDK_files\\epoc32_emu\\epoc32\\data";
  my $cmd = "xcopy $iniH{Sce_Root}\\sdk_wa\\epoc32\\data\\BuildInfo.txt $TargetDir\\  /f /r /i /Y";
  system ($cmd)==0 or print "$cmd\n";
  $TargetFile = "$TargetDir\\BuildInfo.txt";
  print repl_str("sdk_devfam",  "$iniH{ Platform_Abbreviation}" , $TargetFile);
  print repl_str("sdk_famrev",  "$iniH{ PLATFORM_VERSION }" , $TargetFile);
  #print repl_str("sdk_swbuild", "$iniH{ Platform_Build_ID }-$iniH{ Build_ID }" , $TargetFile) ,"\n";
    
 
    #configuration.rul
  open(CFR, "> \\SDK_installer\\script files\\configuration.rul") 
      or die "Can't open configuration.rul: $! Perhaps the directory \\SDK_installer\\script files does not exists.";

  print CFR <<CONFRUL;
#define     INSTALLPATH         "$iniH{ Target_Directory }"
#define     DEFAULTDEVICE       "$iniH{ Kit_ID }"
#define     DEVICEEXT           "$iniH{ Device_Name }"
#define     PRODUCTNAME         "$iniH{ SDK_Product_name }"
#define     SUBVERSION          "$iniH{ Target_Phase }"
#define     SHORTNAME           "$iniH{ SDK_Short_name }"
#define     SUPPORTURL          "$iniH{ Support_URL }"
#define     COMPANYNAME         "$iniH{ SDK_Company_Name }"
#define     STARTMENU_MAIN      "$iniH{ Start_Menu }"
#define     STARTMENU_SDK       "$iniH{ Start_Menu1 }"
#define     STARTMENU           STARTMENU_MAIN ^ "\\\\" ^ STARTMENU_SDK ^ "\\\\$iniH{ Start_Menu2 }"
#define     JAVAREQUIRED        "$iniH{ Required_Java }"
#define     PERLREQUIRED        "$iniH{ Required_Perl }"
#define     DIRECTXVER          "$iniH{ Required_DirectX_user }"
#define     DIRECTXREQUIRED     "$iniH{ Required_DirectX }"
CONFRUL

  close CFR;
  
  #epoc.inis
  chdir "$TargetDir" or warn "Chdir to $TargetDir failed: $!";

  $cmd = "xcopy $iniH{Sce_Root}\\sdk_wa\\epoc32\\data\\epoc_*.ini $TargetDir\\  /f /r /i /Y";
  system ($cmd)==0 or print "$cmd\n";

  foreach my $file ( <epoc_*.ini> ) {
    open(EPOCINI, "< $file")     or die "can't open $file: $!";
    open(TMP, "> tmp.txt")       or die "can't open tmp.txt: $!";
    undef (local $/);
    $_ = <EPOCINI>;
    my $replaced = s/^WindowTitle .*?$(.*)//m; #removed s from modifiers
    print TMP $_;
    close(EPOCINI);
    close(TMP);
    rename("tmp.txt", $file)       or die "can't rename tmp.txt to $file: $!"; 
    print "$file: WindowTitle .. -> \n" if $replaced;
  } 
  
  $cmd = "xcopy $iniH{Sce_Root}\\sdk_wa\\epoc32\\data\\epoc.ini $TargetDir\\  /f /r /i /Y";
  system ($cmd)==0 or print "$cmd\n";

  my $file = "epoc.ini";
  open(EPOCINI, "< $file")     or die "can't open $file: $!";
  open(TMP, "> tmp.txt")       or die "can't open tmp.txt: $!";
  undef (local $/);
  $_ = <EPOCINI>;
  my $found = m/^WindowTitle /sm;
  print TMP $_;
  print TMP "WindowTitle\n" unless $found;           #We add WindowTitle if we didn't find it
  close(EPOCINI);
  close(TMP);
  rename("tmp.txt", $file)       or die "can't rename tmp.txt to $file: $!"; 
  print "$TargetDir\\$file: .. -> WindowTitle \n" unless $found;

  chdir "\\";

}

sub clean_previous_wa {

  enter_sub();
  my ($dir) = @_;
  my $curr_dir = cwd();
  return unless (chdir $dir);
  return unless (-e "Repository_area");             #Hope we are in right directory!
  opendir (DIR, ".") or die "can't open . :$!";   #Remove subdirs
  while (defined (my $subdir = (readdir DIR))){
    next if $subdir =~ /(^\.\.?$|RECYCLER)/i;   #Skip . & ..
    next unless $subdir =~ /^sdk_/i;
        print "Deleted dir - $subdir\n" if do_rmdir_all($subdir);
  }
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
}

sub rm_old_dirs {
  my ($base_dir, $cnt) = @_;
  
  return unless ($cnt > 0 and -d $base_dir);
  chomp (my @dirs2 = `dir /ad /b /o-d $base_dir`);  #Subdir listining
  my @dirs = grep (!/\D/, @dirs2);               #Take those whose names contain digits only
  for (my $i = @dirs; $i > $cnt; --$i){
    my $dir = "$base_dir\\$dirs[$i-1]";
      print "Deleted dir  - $dir\n" if ((-e $dir) && do_rmdir_all($dir));
  }
}
sub fix_and_copy {
    my ($src, $dst) = @_;

    open (SRC, "< $src") or warn "can't open $src: $!", return 1;       #The file may not be in S60 platform
    (my $dst_path = $dst) =~ s/[^\\]+$//;                               #Take the destination path
    mkpath($dst_path) unless (-e $dst_path);                            #Make it
    open (DST, "> $dst") or die "can't open dest file $dst: $!";   
  
    while (<SRC>){
      if ( /\/\*/ ... /\*\//) {                                         #We want to be inside a comment
        s/\@(since|lib|released).*?(?=\*\/|$)//;                        #Remove references to S60 4.0 5.0 releases
     }
      s{#\s*endif\s+(//.*)}{$1\n#endif};                                #Swap comment before endif
      s/\s*$/\n/ if eof SRC;                                            #Remove spaces from last line 
      print DST;
    }
    close SRC;
    close DST;
    print "$src --> $dst\n";
    1;
}
sub strip_and_copy {
    my ($src, $dst) = @_;

    open (SRC, "< $src") or warn "can't open $src: $!", return 0;       #The file may not be in S60 platform
    (my $dst_path = $dst) =~ s/\\[^\\]+$//;                               #Take the destination path
    mkpath($dst_path) unless (-e $dst_path);                            #Make it
    open (DST, ">", $dst) or die "can't open dest file $dst: $!";   
  
    while (<SRC>){
      if ( /\/\*/ ... /\*\//) {                                         #We want to be inside a comment
        s/\@(since|lib|released).*?(?=\*\/|$)//;                        #Remove references to S60 4.0 5.0 releases
     }
      print DST;
    }
    close SRC;
    close DST;
    print "$src --> $dst\n";
    1;
}

sub repl_str {
        my ($s1, $s2, $file) = @_;
        open (FILE, "< $file") or  return "Could not open $file: $!";  #The file may not be in S60 platform
    open(TMP, "> $file.tmp") or die "can't open $file.tmp: $!";
    undef (local $/);
    $_ = <FILE>;
    my $res = "$file: ";
    $res .=  (s/\Q$s1\E/$s2/smg) ? "$s1 -> $s2\n" : "\n";
    print TMP $_;
    close(FILE);
    close(TMP);
    rename("$file.tmp", $file)    or die "can't rename $file.tmp to $file: $!"; 
    $res;
}

sub repl_str2 {
        my ($s1, $s2, $file) = @_;
        open (FILE, "< $file") or  return "Could not open $file: $!";  #The file may not be in S60 platform
    open(TMP, "> $file.tmp") or die "can't open $file.tmp: $!";
    my $res = "$file: ";
    while (<FILE>){
      $res .=  (s/$s1/$s2/g) ? "$s1 -> $s2\n" : "";
      print TMP $_;
    }
    close(FILE);
    close(TMP);
    rename("$file.tmp", $file)    or die "can't rename $file.tmp to $file: $!"; 
    $res;
}
sub repl_str3 {
        my ($s1, $s2, $file) = @_;
        open (FILE, "< $file") or  return "Could not open $file: $!";  #The file may not be in S60 platform
    open(TMP, "> $file.tmp") or die "can't open $file.tmp: $!";
    undef (local $/);
    $_ = <FILE>;
    my $res = "$file:\n";
    while (s/($s1)/$s2/m) {
      $res .= "$1 -> $s2\n"
    };
    print TMP $_;
    close(FILE);
    close(TMP);
    rename("$file.tmp", $file)    or die "can't rename $file.tmp to $file: $!"; 
    $res;
}


sub enter_sub { 
  my ($param) = shift;
  print "\n===>  ", (caller(1))[3] . "($param)  ",  scalar localtime(), "\n\n"
};
sub exit_sub { 
  print "\n<===  ", (caller(1))[3] . "()  ",  scalar localtime(), "\n\n"
};

sub initialize_gcc_part { 
  my ($sea_dir, $gcc_part, $platform_version) = @_;  
  enter_sub($sea_dir);
  
	#Find out the platform version for set of LIB directories to copy

	$$platform_version =  (exists $lib_dirs_to_collect_from_platform{$iniH{ Platform_Version }}) ? $iniH{ Platform_Version } : "9.9";

  #Generate the list of gcce specific part of the lib names (=%gcc_part), eg. {000a0000}, {00010000}
  #by scanning the wanted lib directories for {wwwwwwww}. pattern
  my $files ="";
  $$gcc_part{""}++;                                                                                    #Initialize with empty key
  foreach my $target (keys %{ $lib_dirs_to_collect_from_platform{ $$platform_version}}){               #e.g. winscw armi thumb
    foreach my $u_dir ( @{$lib_dirs_to_collect_from_platform{ $$platform_version}{$target}}){          #$u_dir = udeb or urel , see lib_dirs_to_collect_from_platform above
      opendir (DIR, "$sea_dir\\epoc32\\release\\$target\\$u_dir"); 
      while ( defined (my $file = readdir (DIR))){
      	if ($file =~ /(\{\w{8}\})\./) {                                                   #e.g 3GPMP4Lib{000a0000}.dso {00020067}
	        $$gcc_part{$1}++;
        }
      }
      close(DIR);
    }
  }
  1;
}

# This sub will collect the S60 Apis based on check in apis.xml 
sub Collect_S60_apiset
{
	my $S60_Root=shift @_;
	my $apis_xml=shift @_;
	my $api_dest_dir=shift @_;
	my $platform_version=shift @_;
	my $lib_dirs_to_collect_from_platform=shift @_;
	
	enter_sub($apis_xml);	
	#Parse it
	my $parser=new XML::DOM::Parser;
	my $doc=$parser->parsefile($apis_xml);
	#Get the root node , <api_dataset>
	my $root=$doc->getDocumentElement();
	my @apis_nodes=$root->getElementsByTagName("api");
	
	#Lib extentions to copy
	my @lib_exts=qw (lib dso dll);
	foreach my $api_node(@apis_nodes)
	{
		#Look for <release category="public"> only APIs
		my $release_node_list=$api_node->getElementsByTagName("release");
		my $release_node=$release_node_list->item(0);
		
			next if ($release_node->getAttribute("category") =~/platform/i);
						
		my $name_node_list=$api_node->getElementsByTagName("name");
		my $name_node=$name_node_list->item(0);
		print "\n ----------------------------------------------------";
		print "\n Processing:",$name_node->getFirstChild()->getData();
		
		#Get the Lib file
		my $libs_node_list=$api_node->getElementsByTagName("libs");
		if($libs_node_list->getLength()>0)
		{
			    my $libs_node=$libs_node_list->item(0);
				my @lib_node_list=$libs_node->getElementsByTagName("lib");
				foreach my $lib_node(@lib_node_list)
				{
					my $lib_file=$lib_node->getAttribute("name");
					print "\n Looking Lib:",$lib_file;
					my $lib_file_name="";
					if($lib_file=~m/(.*)\.(.*)/)
					{
						$lib_file_name=$1;
					}
					else
					{
						$lib_file_name=$lib_file;
					}
										
					#Collect .lib,.dso files from platform
					 foreach my $target (keys %{$lib_dirs_to_collect_from_platform{$platform_version}})
					 {               #e.g. winscw armi thumb
    					foreach my $u_dir ( @{$lib_dirs_to_collect_from_platform{$platform_version}{$target}})
    					{
    						foreach my $lib_ext (@lib_exts)
    						{
    							#Copy Lib
    							my $cp_cmd="xcopy $S60_Root\\epoc32\\release\\$target\\$u_dir\\$lib_file_name.$lib_ext $api_dest_dir\\epoc32\\release\\$target\\$u_dir\\";
	    						$cp_cmd=~s/\//\\/g;
	    						$cp_cmd.="   /C /F /R /I /E /Y";
	    						print "\n $cp_cmd";
	    						print "\n";
	    						system($cp_cmd);
	    						#Copy GCC part libname{00000}.dll
	    						$cp_cmd=~s/($lib_file_name)\.(.*?)/$1\{\*\}\.$2/;
	    						print "\n $cp_cmd";
	    						system($cp_cmd);
	    						print "\n";
	    						
    						}
    					}
					 }   
				}
		}
		
		#Get the header files
		my $sources_node_list=$api_node->getElementsByTagName("sources");
			next if ($sources_node_list->getLength()==0); 
		my $sources_node=$sources_node_list->item(0);
		
		
		my @file_node_list=$sources_node->getElementsByTagName("file");
		foreach my $file_node(@file_node_list)
			{
				
				my @path=$file_node->getElementsByTagName("path");
				my $src_api_file_path=$path[0]->getFirstChild()->getData();
				
				my @export=$file_node->getElementsByTagName("export");
				my $export_api_file_path=$export[0]->getFirstChild()->getData();
				
				#Copy the header
				print "\n cp: $S60_Root\\$src_api_file_path -->$api_dest_dir\\$export_api_file_path";
				mkpath(dirname("$api_dest_dir\\$export_api_file_path"))unless(-d "$api_dest_dir\\$export_api_file_path");
				copy("$S60_Root$src_api_file_path","$api_dest_dir\\$export_api_file_path") or print "\n ERROR: Cp failed $!";
				
					
			}
		print "\n ----------------------------------------------------";		
	}
}


sub get_Symbian_OS_info {
  my $readme_file = "$iniH{ S60_Root }\\readme.txt";
  open (FH, "< $readme_file") or return "No Symbian OS info, $readme_file not found";
  do { $line = <FH> } until $line =~ /^Symbian OS.+/ || do {$eof = eof};  #Read until Symbian OS -line is found
  $line =~ /_(.+)/;
  close FH;
  return "No Symbian info in  $readme_file" if $eof;
  return $1;
} 
1;

__END__

sub where_when { 
  (caller(1))[3] . "()  ===  ",  scalar localtime()
};
sub where_when { 
  print "\n", (caller(1))[3] . "()  ===  ",  scalar localtime(), "\n\n"
};
sub where_when { 
  my ($param) = shift;
  print "\n", (caller(1))[3] . "($param)  ===  ",  scalar localtime(), "\n\n"
};
