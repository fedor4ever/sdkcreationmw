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

package SDK_lang;
use SDK_vars;
use SDK_utils;
use File::Find;
use File::Path;
use File::Basename;
use File::Copy;
use Cwd;

our ($VERSION, @ISA, @EXPORT);

$VERSION = 1.00;
@ISA = qw(Exporter);
@EXPORT = qw(copy_language_zips create_lang_package);

sub copy_language_zips {
  my ($href) = @_;
  enter_sub("Copying the language zips from $$href{Platform_Languages} -> $$href{S60_Root}\\Languages");
  my $curr_dir = cwd();
  my $lang_dir = "$$href{S60_Root}\\Languages";
	print "Deleted dir  - $lang_dir\n" if ((-e $lang_dir) && do_rmdir_all($lang_dir));

  foreach my $pl_lang_path ( split (/ /, $iniH{Platform_Languages}) ){
  	if($_ =~ m/japan/g) # Added by dinesh since japanese is not required in the N97 SDK
		{
			print "Skipping copying of $_ file\n";
			next;
		}
	  map { my $cmd ="xcopy $pl_lang_path\\$_ $$href{S60_Root}\\Languages\\$_ /f /r /i /s /Y "; system ($cmd)==0 or print "$cmd\n";} qw(*_delta_*.zip);
  }

  #Copy the possible patches
  if (-e $$href{Languages_Patch}) {
    my $cmd = "xcopy $$href{Languages_Patch}\\*.zip $lang_dir\\ /f /r /i /s /Y";
    system ($cmd)==0 or print "$cmd\n";
  }
};

sub create_lang_package {
  local ($S60_Root, $lang_dir, $graylist, $blacklist, $dest_dir) = @_;
  enter_sub($lang_dir);
  my $curr_dir = cwd;
  local $emu_dirs = join " ", map { "epoc32\\$_\\*" } @emu_src_dirs;
  return unless (chdir $lang_dir);

  local $lang_wa_dir = "$iniH{Sce_Root}\\lang_wa";
	print "Deleted dir  - $lang_wa_dir\n" if ((-e $lang_wa_dir) && do_rmdir_all($lang_wa_dir));
    mkpath($lang_wa_dir);

  #Copy language zips to wa
  foreach (<*.zip>){
    if (/delta_/){
    	if($_ =~ m/japan/g) # Added by dinesh since japanese is not required in the N97 SDK
			{
				print "Skipping copying of $_ file\n";
				next;
			}
      $cmd = "xcopy $_ $lang_wa_dir /F /I /Y";
      system ($cmd)==0 or print "$cmd\n";
    }
  }

  return unless (chdir "$lang_wa_dir");
  print "\ngvn: lang_wa_dir is $lang_wa_dir\n";

  print "Rename language zips === started ", scalar localtime(), "\n";
  foreach (<*.zip>){
  	print "\ngvn: ZIP PROCESSED is: $_\n";
    SWITCH: {
        /china/     && do {
                            system ("rename $_ zh_CN.zip")==0 and print "$_ >-> zh_CN.zip\n";
                            $cmd = "copy /V $iniH{S60_Root}\\s60\\misc\\release\\S60LocFiles\\data\\31.zip $lang_dir\\package_31.zip";
        										print "$cmd\n"; system ($cmd);
                            last SWITCH;
                       };
        /japan/     && do {
                            system ("rename $_ ja_JP.zip")==0 and print "$_ >-> ja_JP.zip\n";
                            $cmd = "copy /V $iniH{S60_Root}\\s60\\misc\\release\\S60LocFiles\\data\\32.zip $lang_dir\\package_32.zip";
        										print "$cmd\n"; system ($cmd);
                            last SWITCH;
                       };
        /western/   && do {
                            system ("rename $_ en_US.zip")==0 and print "$_ >-> en_US.zip\n";
                            $cmd = "copy /V $iniH{S60_Root}\\s60\\misc\\release\\S60LocFiles\\data\\01.zip $lang_dir\\package_01.zip";
        										print "$cmd\n"; system ($cmd);
                            last SWITCH;
                       };
        /arabic/    && do {
                            system ("rename $_ ar_SA.zip")==0 and print "$_ >-> ar_SA.zip\n";
                            $cmd = "copy /V $iniH{S60_Root}\\s60\\misc\\release\\S60LocFiles\\data\\37.zip $lang_dir\\package_37.zip";
        										print "$cmd\n"; system ($cmd);
                            last SWITCH;
                       };
        /urdu/     && do {
                            system ("rename $_ ur_PK.zip")==0 and print "$_ >-> ur_PK.zip\n";
                            $cmd = "copy /V $iniH{S60_Root}\\s60\\misc\\release\\S60LocFiles\\data\\94.zip $lang_dir\\package_94.zip";
        										print "$cmd\n"; system ($cmd);
                            last SWITCH;
                       };
        /hindi/    && do {
                            system ("rename $_ hi_IN.zip")==0 and print "$_ >-> hi_IN.zip\n";
                            $cmd = "copy /V $iniH{S60_Root}\\s60\\misc\\release\\S60LocFiles\\data\\58.zip $lang_dir\\package_58.zip";
        										print "$cmd\n"; system ($cmd);
                            last SWITCH;
                       };

    }
  }
  #Creating localization zip by adding all the package_*
  $cmd = "zip -r $lang_dir\\localisation.zip $lang_dir\\package_*.zip";
  print "$cmd\n"; system ($cmd);


	create_lang_zips(\%sdk_lang_codes, $lang_dir, $lang_wa_dir, $dest_dir);
	create_lang_zips(\%extra_lang_codes, $lang_dir, $lang_wa_dir, "$iniH{Sce_Root}\\sdk_lang_extra",\%lang_names);
	
	#Now Create language packages.zip in sdk_lang_extra
	create_lang_package_zip(\%sdk_lang_codes,$lang_wa_dir,$S60_Root,$graylist, $blacklist);
	create_lang_package_zip(\%extra_lang_codes,$lang_wa_dir,$S60_Root,$graylist, $blacklist,\%lang_names);
	
	#Now create Forum Nokia specific extra language package zips
	print "\n Create FORUM NOKIA standard extra language zips";
	create_FN_extralang_zip(\%extra_lang_codes,\%lang_names);
	
    mkpath("$lang_wa_dir\\undo2zip");
  chdir "$lang_wa_dir\\undo2zip" or print ("$!: $lang_wa_dir\\undo2zip\n" );
  $cmd = "zip -r $dest_dir\\$iniH{ 'Platform_Build_ID' }-undo.zip *";  				#Zip them again to Result directory (prefixed by build id)
  print ($cmd."\n"); system ($cmd);

  #Put English as a default
  my $en_US_zip = glob("$dest_dir\\*-en_US.zip");
	my $cmd = "unzip -o $en_US_zip -d \\sdk_wa";
  print ( $cmd."\n" );
  system ( $cmd );
  foreach my $package (glob("$dest_dir\\*package*.zip")) {          		#Extract the packages (resources) on top of the sdk
		my $cmd = "unzip -o $package -d \\sdk_wa";
	  print ( $cmd."\n" );
	  system ( $cmd );
		$cmd = "move $package $iniH{Sce_Root}\\sdk_lang_extra";                        	#Move them to extra folder
	  print ( $cmd."\n" );
	  system ( $cmd );
  }
  chdir $curr_dir;

#Remove the work area
	$subdir = "$lang_wa_dir";
	#print "Deleted dir  - $subdir\n" if do_rmdir_all($subdir);
}


#This sub will create Rel-$relid-package-$lang_key.zip (Rel200802-package_ja_JP.zip , Rel200802-package_ur_PK.zip)
sub create_lang_package_zip
{
	
	my $lang_code_href=shift @_;
	my $lang_wa_dir=shift @_;
	my $S60_Root=shift @_;
	my $graylist = shift @_;
	my $blacklist = shift @_;
	my $language_names=shift @_;
	
	my $assemble_area=$lang_wa_dir."\\PACKAGES";
	my $loc_resource_base=$S60_Root."\\epoc32\\RELEASE\\WINSCW\\udeb\\Z";
	my @loc_resource_dirs=qw (private resource);
	
	
	my $lang_extra_dest="$iniH{Sce_Root}\\sdk_lang_extra";
	my $lang_code_val; 
	
	my $cmd;
	my @out;
	my $sstring;
	my $lang_code_val_rx;
	
	my $lang_rs_src;
	my $lang_rs_dest;
	my $lang_rs_dir_name;
	my $S60_Root_Rx=quotemeta($S60_Root);
	my $res;
	
	#Create work area,
	mkpath($assemble_area);
	foreach my $lang_code_name ( keys %$lang_code_href)
	{   
		#Initialize
		$cmd="";
		@out=();
		$sstring="";  
		$lang_code_val_rx="";                         
    	print "\nCreating $lang_code_name package=== started ", scalar localtime(), "\n\n";
	    	foreach $lang_code_val(@{$lang_code_href->{$lang_code_name}})#$lang_code val is an array, so expand it with @
	    	{
	    		$sstring.=' '.$lang_code_val; #Add for dir
	    		$sstring.='  *.r'.$lang_code_val; #Add for files
	    		$lang_code_val_rx.="|$lang_code_val\$";
	    	} 
    	$lang_code_val_rx=~s/^\|//; #Knock off first |
    	$cmd="dir $sstring  /S /L /B";
    	print "\n Search string for $lang_code_name => $cmd";

    	#Now go to  $loc_resource_base
    	chdir $loc_resource_base or print "\n ERROR: Can not chdir to $loc_resource_base";
    	
    	print "\n EXEC: $cmd in ",cwd();
	    	eval
	    	{
	    		@out=`$cmd`;
	    	};
	    	if ($@)
	    	{
	    		print "\n ERROR: $cmd failed in $loc_resource_base, $!";
	    	}
    	
    	#Got the list fo files, Now need to copy them to $assemble_src\\$lang_code_name
    	mkpath($assemble_area."\\".$lang_code_name);
	    	for $lang_rs_src(@out)
	    	{
	    		#avoid unwanted file
	    		chomp($lang_rs_src);
	    		 next if ($lang_rs_src!~m/$lang_code_val_rx/i);
	    		     		
	    		if (! -d $lang_rs_src)
	    		{
	    			$lang_rs_dir_name=dirname($lang_rs_src)  #if it is a dir , we need to cp as it is
	    		}
	    		else
	    		{
	    			$lang_rs_dir_name=$lang_rs_src;
	    		}
	    		$lang_rs_dir_name=~s/$S60_Root_Rx//io; # Strip the $S60_Root
	    		$lang_rs_dest=$assemble_area."\\"."$lang_code_name"."\\".$lang_rs_dir_name;
	    		
	    		#Make dest path
	    		mkpath($lang_rs_dest);
	    		#Copy it
	    		$cmd="xcopy $lang_rs_src $lang_rs_dest /F /R /I /S /Y";
	    		
	    		$res=system($cmd);
	    		if($res !=0)
	    		{
	    			  print "\n ERROR: $cmd FAILED, $res";
	    		}
	   			    		
	    	}
	    
	    chdir $assemble_area."\\".$lang_code_name;
    	#Rm read only
    	print "\n Remove read only attribute in ",cwd();
    	system ("attrib /s *.* -r");   
    	#Apply gray / balck
    	apply_blacklist($assemble_area."\\".$lang_code_name, ($blacklist, $common_graylist) ); 
    	#dbgstrip
    	print "\n Start DBGSTRIPPI in $assemble_area\\$lang_code_name";
    	find \&dbgstrippi, "$assemble_area\\$lang_code_name";
    	#lower case
    	print "\n Start DBGSTRIPPI in $assemble_area\\$lang_code_name";
    	find \&lowercase, "$assemble_area\\$lang_code_name";
    	
    	chdir $assemble_area."\\".$lang_code_name;
    	#Zip it
    	print "\n Create lang ZIP package for $lang_code_name in ",cwd();
    	if(defined $language_names)
    	{
	    	#Need to create FN specific extra lang zip
	    	$cmd = "zip -r $lang_extra_dest\\$iniH{'Platform_Abbreviation'}_$iniH{'Edition_number'}_Edition_$$language_names{$lang_code_name}_plug_in.zip *"; 
    	}
    	else
    	{
    		$cmd="zip -r  $lang_extra_dest\\$iniH{'Platform_Build_ID'}-package_$lang_code_name.zip *";
    	}
    	print "\n EXEC: $cmd";
    	$res=system($cmd);
    	if($res !=0)
    	{
    		  print "\n ERROR: $cmd FAILED, $res";
    	}
    	
	}
}

#This sub will create the FN specific extra language packages by doing,
#For example, S60_5th_Edition_Arabic_v1_0_en.zip=S60_5th_Edition_Arabic_plug_in.zip+S60_5th_Edition_Arabic_Switcher.zip+ReadMe_Arabic.txt
sub create_FN_extralang_zip
{
	my $lang_codes=shift @_;
	my $language_names=shift @_;
	my $lang_extra_dest="$iniH{Sce_Root}\\sdk_lang_extra";
	my $readme_src=	"$iniH{Sce_Root}\\Repository_area\\sdk_lang_extra_readme";
	my $pre_pwd = cwd;
	
	print "\n Start Create FORUM NOKIA standard extra language zips";
	my $cmd="";
	my $fn_zip_name="";
	my $includes="";
	chdir $lang_extra_dest or die "\n ERROR: Can not chdir to $lang_extra_dest";
	foreach my $lang(keys %$lang_codes)
	{
		#Ex: S60_5th_Edition_Arabic_v1_0_en.zip
		$fn_zip_name="$iniH{'Platform_Abbreviation'}_$iniH{'Edition_number'}_Edition_$$language_names{$lang}_v1_0_en.zip";
		#Copy Read me
		copy("$readme_src\\ReadMe_$$language_names{$lang}.txt","ReadMe_$$language_names{$lang}.txt");
		#Ex: S60_5th_Edition_Arabic_plug_in.zip + S60_5th_Edition_Arabic_Switcher.zip+ReadMe_Arabic.txt
		$includes="$iniH{'Platform_Abbreviation'}_$iniH{'Edition_number'}_Edition_$$language_names{$lang}_Switcher.zip  $iniH{'Platform_Abbreviation'}_$iniH{'Edition_number'}_Edition_$$language_names{$lang}_plug_in.zip ReadMe_$$language_names{$lang}.txt";
		
		$cmd = "zip $lang_extra_dest\\$fn_zip_name  $includes"; 
		print $cmd."\n";  system ( $cmd );	
	}
	chdir $pre_pwd or die ("\n ERROR: Can not chdir to $pre_pwd");
}
sub create_lang_zips 
{
	my ( $lang_codes, $lang_dir, $lang_wa_dir, $dest_dir,$language_names) = @_;
  enter_sub($lang_dir);
  my $curr_dir = cwd;

  my @localisation_zips = glob("$lang_dir\\*localisation.zip");
  print "Deleted dir  - $dest_dir\n" if do_rmdir_all($dest_dir);
  mkpath($dest_dir);

  foreach my $lang ( keys %$lang_codes ){                                #Handle all languages, see SDK_vars.pm for %lang_codes
    print "\nCreating $lang === started ", scalar localtime(), "\n\n";
    chdir "$lang_wa_dir" or die "chdir for $lang_wa_dir failed: $!";
    my ($lang_emu_wa, $lang_resource_wa) = ("$lang_wa_dir\\$lang\\emu","$lang_wa_dir\\$lang\\resource");

     mkpath ($lang_emu_wa);#Create the directory for $lang_emu_wa
    if (-e "$lang.zip" ){                                               		#We have delta for this language
    	my $cmd = "unzip -o $lang.zip $emu_dirs -d $lang_emu_wa";
      print ( $cmd."\n" );
      system ( $cmd );                                                  		#Unzip delta to $lang directory
    }

    #Apply the language patches
    #It extracts the languages patches resides on filerblr to $lang_wa_dir
    if (-e "$iniH{S60_Root}\\Languages\\Patch_$lang.zip") 
    {
      $cmd = "unzip -o $iniH{S60_Root}\\Languages\\Patch_$lang.zip $emu_dirs -d $lang_emu_wa";
      system ($cmd)==0 or print "$cmd\n";
    }

    foreach my $lang_code ( @{$$lang_codes{$lang}} ){
      #Get the localization packages from the localisation zip
      for my $localisation_zip ( @localisation_zips ){
        $cmd = "unzip -o -j -C $localisation_zip $lang_dir\\package_${lang_code}.zip -d $lang_wa_dir";
        print ( $cmd."\n" ); system ( $cmd );
        #$cmd = "unzip -o -C package_${lang_code}.zip $emu_dirs -d $lang_resource_wa";
        $cmd = "unzip -o -C $iniH{S60_Root}\\s60\\misc\\release\\S60LocFiles\\data\\${lang_code}.zip $emu_dirs -d $lang_resource_wa";
        do {print "$cmd\n"; system ($cmd)} if (-e "${lang_code}.zip");            #Unzip resource to $lang_resource_wa directory
      }
    }
	
	if (chdir ("$lang_wa_dir\\$lang")) {                                  #If the dir exists => we had the delta for this language
      system ("attrib /s *.* -r");                                        #Remove read-only attributes

      apply_blacklist($lang_emu_wa, ($blacklist, $common_graylist) );  				#Apply graylist
      apply_blacklist($lang_resource_wa, ($blacklist, $common_graylist) );  	#Apply graylist

      print ("Dgbstrippi started on ", cwd(), " === ", scalar localtime(), "\n");
      find \&dbgstrippi, ".";

      print "Filenames to lowercase started on ", cwd()," === ", scalar localtime(), "\n";
      find \&lowercase, qw(.);
      print "Filenames to lowercase === finished ", scalar localtime(), "\n";

      # zap_files_from_lang_zips("$lang_wa_dir\\$lang");  	 #Remove files which are supposed to be patched, ie. do not overwrite patches
      # create_101f876d_ini("$lang");                        #Perhaps not needed anymore, this puts the initial input language on, I presume
    }

	  mkpath("$lang_wa_dir\\$lang") unless (-e "$lang_wa_dir\\$lang");
	  chdir ("$lang_wa_dir\\$lang");
    #Perhaps not needed anymore, the zips already contain this
    create_language_txt($lang_emu_wa, $lang, $lang_codes);

    create_lang_properties_file ( $lang_emu_wa, $lang );             	                  #This must go to the zip. Do not zap!
		#Zip the work areas
    chdir "$lang_emu_wa" or die "chdir for $lang_emu_wa failed: $!";
    if(defined $language_names)
    {
    	#Need to create FN specific extra lang zip
    	$cmd = "zip -r $dest_dir\\$iniH{'Platform_Abbreviation'}_$iniH{'Edition_number'}_Edition_$$language_names{$lang}_Switcher.zip *"; 
    }
    else
    {
    	$cmd = "zip -r $dest_dir\\$iniH{ 'Platform_Build_ID' }-$lang.zip *";          			#Zip them again to Result directory (prefixed by build id)
    }
    print $cmd."\n";  system ( $cmd );

  }
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";

  print "Create undo zip === started ", scalar localtime(), "\n";
  my $undo_dir = "$lang_wa_dir\\undo2zip";
  foreach (<$dest_dir\\*.zip>){
    next if /package/i;                         					#Do not consider packages
		my $cmd = "unzip -l $_";
    print ( $cmd."\n" );
    for ( `$cmd` ) {																			#Get the filelisting of the zip
			next if /\/$/;                            					#Skip the path lines. They end with /
	    next unless (s/.+\d   //);                					#Devour anything before path
	    chomp;
	    s[/][\\]g;
	    next unless ( -e "$iniH{Sce_Root}\\sdk_wa\\$_");    #The file is not in SDK
	    m/(.*\\)/;       																		#Take the directory part -> $1
	    $cmd = "xcopy \"$iniH{Sce_Root}\\sdk_wa\\$_\" \"$lang_wa_dir\\undo2zip\\$1\" /F /I /Y";
	    system ($cmd)==0 or print "$cmd\n";                 #Copy the file from sdk_wa to be "undozipped"
	  }
	}
}


sub create_101f876d_ini {

#Unicode file!
  my $lang = shift;
  my $file = "$lang_wa_dir\\$lang\\epoc32\\release\\winscw\\udeb\\z\\private\\10202be9\\101f876d.txt";
  open(INI, "<:encoding(utf16le)", $file) or die "can't open $file: $!";
  open(TMP, ">:raw:encoding(utf16le)", "tmp.txt") or die "can't open tmp.txt: $!";
  while (<INI>){
    s/0x5 int \p{IsDigit}+/0x5 int $$lang_codes{$lang}[0]/;
    print TMP $_;
  }
  close(INI);
  close(TMP);
  rename("tmp.txt", $file)     or die "can't rename tmp.txt to $file: $!";
  print <<KORV5;
$file: 0x5 int 0 -> 0x5 int $$lang_codes{$lang}[0]
KORV5
}

sub create_lang_properties_file {

  my ($lang_wa_dir, $lang) = @_;
  my @array = split(/_/, $lang);
  enter_sub($lang);
  print "Create language.properties $array[0] === started in $lang_wa_dir ", scalar localtime(), "\n";
  my $dest_dir = "$lang_wa_dir\\epoc32\\data";
   mkpath($dest_dir);
  open (LPF, ">$dest_dir\\language.properties") or die "Can't create $dest_dir\\language.properties: $!";
  print LPF <<LANG_PROP;
#language.properties
language=$array[0]
country=$array[1]
version=$iniH{ 'Platform_Build_ID' }
LANG_PROP
  close LPF;
  print "Create language.properties $array[0] === finished ", scalar localtime(), "\n";
}

sub create_language_txt {

  my ($lang_wa_dir, $lang, $lang_codes) = @_;

  enter_sub();
  #Check if already exists
  if (-e "$lang_wa_dir\\epoc32\\release\\winscw\\udeb\\z\\resource\\bootdata\\languages.txt") {print "languages.txt already exists!\n"; return};
  #Unicode file!
  my $src_file = "$iniH{S60_Root}\\epoc32\\release\\winscw\\udeb\\z\\resource\\bootdata\\languages.txt";
  open(LANG_SRC, "<:encoding(utf16le)", $src_file) or warn "can't open $src_file: $!" and return;
  open(TMP, ">:raw:encoding(utf16le)", "languages.txt") or die "can't open languages.txt: $!";
  while (<LANG_SRC>){
    my $cnt=0;
    $cnt = s/\p{IsDigit}+,d/$$lang_codes{$lang}[0],d/;
    print "$src_file:  $& --> $$lang_codes{$lang}[0],d\n" if ($cnt);
    print TMP $_;
  }
  close(LANG_SRC);
  close(TMP);

  #Move the languages.txt to proper place
  $cmd = "xcopy languages.txt $lang_wa_dir\\epoc32\\release\\winscw\\udeb\\z\\resource\\bootdata\\ /F /R /I /Y";
  system ($cmd)==0 or print "$cmd\n";
  unlink ("languages.txt");
  exit_sub();
}

#Remove files that we are going to patch
#I.e. prevent the patches being overwritten
sub zap_files_from_lang_zips {
  my $lang = shift;
  enter_sub($lang);
  foreach $patch_dir (qw(Common_Patch CPP_Patch Java_Patch)) {   #These directories are included
    next unless (-e "$repository_area\\$patch_dir");
    #We create a file (=FH) containing the filenames
    open( PATCH_FILES, "dir /s /a-d /b $repository_area\\$patch_dir |" ) or die "Cannot open dir $repository_area\\$patch_dir: $!";
    while (<PATCH_FILES>){
      chomp;
      my $patch_path = "$repository_area\\$patch_dir\\";
      s/\Q$patch_path\E/$lang\\/;
      $target_item = "$_";
      next unless ( -e );    #The file is not in language zip
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
        system ("del /q /s $target_item")==0 or print " $target_item\n";
      }
      else {
        system ("del /q /s $target_item")==0 or print " $target_item\n";
      }
   }
    close( PATCH_FILES );
  }
  print "zap_files_from_lang_zips $lang  === finished ", scalar localtime(), "\n";
  exit_sub();
  1;
}
