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
#################################################################################
#
# Nokia/NMP/TWS S60 SDK TF
#
# Series 60 & Series 90 SDK example apps builder
# 
# [TBD]
#
# Mike Smyslov
# 15.07.2003
#
  $version = "buildS60ex.pl v0.22";
#
# CHANGES
#
# 0.21 -> 0.22: Smart handling of creating .sis files for ARMV5 and GCCE targets:
#               .sis file is created from the .pkg file corresponding to target,
#               both UREL armv5 and gcce are assumed; .pkg file is either
#               <appname>_armv5.pkg or <appname>_gcce.pkg
#               
# 0.20 -> 0.21: GCCE udeb / urel made valid targets
#
# 0.19 -> 0.20: .sis creation removed for armv5_udeb target		
#
# 0.18 -> 0.19: Separate targets armv5_udeb & armv5_urel;
#		separate targets winscw_udeb & wincw urel
#
# 0.17 -> 0.18: More correct parsing of example apps grouped in subdirs (openglex).
#
# 0.16 -> 0.17: Target "armv5" added".
#
#################################################################################


# we will measure walking time in seconds
$starttime = (time); 			

# current time and date
chomp($time = `time /T`);
chomp($date = `date /T`);


# Target token to control build process' target
$target = $ARGV[0] || die "\nError!\nUsage: build.pl target sdk_location\n\n";

chomp($target);
$target =~ tr/A-Z/a-z/;

# targets: wins, winscw, armv5, gcce, thumb, clean, reallyclean

if ( ($target ne "wins") && 
     ($target ne "winscw_urel") && 
     ($target ne "winscw_udeb") && 
     ($target ne "thumb") && 
     ($target ne "armv5_udeb") && 
     ($target ne "armv5_urel") && 
     ($target ne "gcce_udeb") && 
     ($target ne "gcce_urel") && 
     ($target ne "clean") && 
     ($target ne "reallyclean") )
	{
	print "\nError!\nTarget wins|winscw_urel|winscw_udeb|thumb|armv5_urel|armv5_udeb|gcce_urel|gcce_udeb|clean|reallyclean\n";
	exit;
	}

$path = $ARGV[1] || die "\nError!\nUsage: build.pl target [SDK location]\n\n";

# LOGFILE
$curd = `cd`;
chomp($curd);
$log = $curd."\\build_S60ex_".$target."_.txt";


# write script's starting information to logfile and console

print "\nStarting to build SDK example apps ($target)\n";
open( LOG, ">$log") || die "Cannot open file $log!";
print LOG "Starting to build SDK example apps ($target).\n";
print LOG "Path to example apps: $path\n";
print "Path to example apps: $path\n";




# try to follow the path to example applications
$dir_exist = chdir $path;

if (not $dir_exist)
	{
	print "Error: could not enter target path!\n";
	print LOG "\nError: could not enter target path!\n";
	close LOG;
	exit;
	}

# find all applications
@apps = `dir /S /B bld.inf`;


# the number of found applications
$appCount = @apps;


# write the number of found applications to logfile and console

if ($appCount eq 0)
	{
	print "Error: could not find Series 60 example applications!\n";
	print LOG "\nError: could not find Series 60 example applications!";
	close LOG;
	exit;
	}


print "Build started: $date $time\n";
print LOG "Build started: $date $time\n";
print "$appCount applications found.\n\n";
print LOG "$appCount applications found.\n\n\n\n\n\n\n\n\n\n";
close LOG;



# set default target device prior to building 
# NOTE: this has been dropped since SDK builds 
# suppose to carry variable device mappings
#system("devices -setdefault \@Series60_v20:com.nokia.series60");



while( @apps )	# build all found applications
{ 
	chomp($app = shift @apps);			# parse application name from path

							# debug # print "\napp=$app\n";
	$app =~ /(.*)\\bld\.inf$/i;
	$appPath = $1;

	$appPathLC = $appPath;
	$appPathLC =~ tr/A-Z/a-z/;

	next if ($appPathLC =~ m /tsrc/);		# skip /tsrc/ dirs from handling
							# debug # print "appPath = $appPath\n";
	$shortpath = $appPath;
	$shortpath =~ s/\Q$path//;			# remove common dir prefix from $path

							# debug# print "\nshortpath = $shortpath\n";

	$shortpath =~/\\(.*)/;				# remove leading "\"
	$shortpath = $1;
	$shortpath =~/(.*)(\\group)/i;			# remove trailing "\group"
	$appName = $1;
							# debug # print "\nappName = $appName\n";
	# build application
	build($appPath, $appName);
}



# walking time calculation in seconds
$endtime = (time);			
$runtime = $endtime - $starttime;



# write ending information of building process to logfile and console

chomp($time = `time /T`);
chomp($date = `date /T`);
open( LOG, ">>$log") || die "Cannot open file $log!";
print LOG '_' x 80;
print LOG "\nBuild ended: $date $time ($runtime seconds spent)\n";
print "\nBuild ended: $date $time ($runtime seconds spent)\n";
close LOG;

exit;






# Subroutine for build commands
#
sub build($appPath, $appName)
{
	# change dir
	chdir $appPath;
	
	print "$appName";

	# write error messages to logfile 
	open( STDERR, ">>$log") || die "Cannot open file $log!";

	$heading = $appName."       (".$appPath.")";
	$spc_count = length($heading);

	print STDERR '-' x $spc_count . "\n";	
	print STDERR $heading."\n";
	print STDERR '-' x $spc_count . "\n\n";




	if ( ($target ne "clean") && ($target ne "reallyclean") )  # makefiles needs to be generated
	{							   # only for build process, not for cleaning
	# generate makefiles and abld.bat from bld.inf
	#
	print STDERR "---> bldmake -v bldfiles:\n";
	$res = `bldmake -v bldfiles`;
	print STDERR "$res\n\n";
	print ".";
	}




	if ($target eq "winscw_urel")						     # WINSCW
	{									     
	# build winscw
	#
	print STDERR "---> abld build winscw urel (before freeze):\n";
	$res = `abld build winscw urel`;
	print STDERR "$res\n\n";
	print ".";

	# freeze exports winscw
	#
	print STDERR "---> abld freeze winscw:\n";
	$res = `abld freeze winscw`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build winscw urel (after freeze):\n";
	$res = `abld build winscw urel`;
	print STDERR "$res\n\n";
	print ".";
	
	# generate Codewarrior IDE project file
	#
	print STDERR "---> abld makefile cw_ide:\n";
	$res = `abld makefile cw_ide`;
	print STDERR "$res\n\n";
	print ".";
	}


	

	if ($target eq "winscw_udeb")						     # WINSCW only UREL branch for S90 SDK
	{									     
	# build winscw
	#
	print STDERR "---> abld build winscw udeb (before freeze):\n";
	$res = `abld build winscw udeb`;
	print STDERR "$res\n\n";
	print ".";

	# freeze exports winscw
	#
	print STDERR "---> abld freeze winscw:\n";
	$res = `abld freeze winscw`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build winscw udeb (after freeze):\n";
	$res = `abld build winscw udeb`;
	print STDERR "$res\n\n";
	print ".";
	
	# generate Codewarrior IDE project file
	#
	print STDERR "---> abld makefile cw_ide:\n";
	$res = `abld makefile cw_ide`;
	print STDERR "$res\n\n";
	print ".";
	}




	if ($target eq "wins")						     	     # WINS
	{									     
	# build wins
	#
	print STDERR "---> abld build wins (before freeze):\n";
	$res = `abld build wins`;
	print STDERR "$res\n\n";
	print ".";

	# freeze exports wins
	#
	print STDERR "---> abld freeze wins:\n";
	$res = `abld freeze wins`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze wins
	#
	print STDERR "---> abld build wins (after freeze):\n";
	$res = `abld build wins`;
	print STDERR "$res\n\n";
	print ".";

	# generate MSVC project file
	#
	print STDERR "---> abld makefile vc6:\n";
	$res = `abld makefile vc6`;
	print STDERR "$res\n\n";
	print ".";
	}
	



	if ($target eq "armi")						     	     # ARMI UREL
	{									     
	# build armi urel
	#
	print STDERR "---> abld build armi urel (before freeze):\n";
	$res = `abld build armi urel`;
	print STDERR "$res\n\n";
	print ".";

	#  freeze exports armi
	#
	print STDERR "---> abld freeze armi:\n";
	$res = `abld freeze armi`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build armi urel (after freeze):\n";
	$res = `abld build armi urel`;
	print STDERR "$res\n\n";
	print ".";
	}





	if ($target eq "thumb")						     	     # THUMB UREL
	{									     
	# build thumb urel
	#
	print STDERR "---> abld build thumb urel (before freeze):\n";
	$res = `abld build thumb urel`;
	print STDERR "$res\n\n";
	print ".";

	#  freeze exports thumb
	#
	print STDERR "---> abld freeze thumb:\n";
	$res = `abld freeze thumb`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build thumb urel (after freeze):\n";
	$res = `abld build thumb urel`;
	print STDERR "$res\n\n";
	print ".";

	# make sis
	#
	print STDERR "---> makesis";
	chdir "..\\sis";
	$res = `cd`;

	if (not ($res =~ m /sis/))
	{
		chdir "..\\..\\sis";
	}

	$res = `cd`;
	$pkgPath = $res;

	$res = `dir /b *.pkg`;
	chomp ($res);

	if ($res eq "")
	{
		print STDERR " failed: .pkg file is not found!\n\n";
	}
	else 
	{
		$pkgName = $res;
		chomp($pkgName);

		print STDERR " $pkgName:";

		checkHardcodedPath($pkgName);

		$res = `makesis $pkgName`;
		print STDERR "\n$res\n\n";
	}
	}





	if ($target eq "armv5_urel")					     	     # ARMV5 UREL
	{									     
	# build armv5 urel
	#
	print STDERR "---> abld build armv5 urel (before freeze):\n";
	$res = `abld build armv5 urel`;
	print STDERR "$res\n\n";
	print ".";

	#  freeze exports armv5
	#
	print STDERR "---> abld freeze armv5:\n";
	$res = `abld freeze armv5`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build armv5 urel (after freeze):\n";
	$res = `abld build armv5 urel`;
	print STDERR "$res\n\n";
	print ".";

	# make sis
	#
	print STDERR "---> makesis";
	chdir "..\\sis";
	$res = `cd`;

	if (not ($res =~ m /sis/))
	{
		chdir "..\\..\\sis";
	}

	$res = `cd`;
	$pkgPath = $res;

	$res = `dir /b *_armv5.pkg`;
	chomp ($res);

	if ($res eq "")
	{
		print STDERR " failed: .pkg file is not found!\n\n";
	}
	else 
	{
		$pkgName = $res;
		chomp($pkgName);

		print STDERR " $pkgName:";

		checkHardcodedPath($pkgName);

		$res = `makesis $pkgName`;
		print STDERR "\n$res\n\n";
	}
	}






	if ($target eq "armv5_udeb")					     	     # ARMV5 UDEB
	{									     
	# build armv5 udeb
	#
	print STDERR "---> abld build armv5 udeb (before freeze):\n";
	$res = `abld build armv5 udeb`;
	print STDERR "$res\n\n";
	print ".";

	#  freeze exports armv5
	#
	print STDERR "---> abld freeze armv5:\n";
	$res = `abld freeze armv5`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build armv5 udeb (after freeze):\n";
	$res = `abld build armv5 udeb`;
	print STDERR "$res\n\n";
	print ".";
	}
































	if ($target eq "gcce_urel")					     	     # GCCE UREL
	{									     
	# build armv5 urel
	#
	print STDERR "---> abld build gcce urel (before freeze):\n";
	$res = `abld build gcce urel`;
	print STDERR "$res\n\n";
	print ".";

	#  freeze exports gcce
	#
	print STDERR "---> abld freeze gcce:\n";
	$res = `abld freeze gcce`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build gcce urel (after freeze):\n";
	$res = `abld build gcce urel`;
	print STDERR "$res\n\n";
	print ".";

	# make sis
	#
	print STDERR "---> makesis";
	chdir "..\\sis";
	$res = `cd`;

	if (not ($res =~ m /sis/))
	{
		chdir "..\\..\\sis";
	}

	$res = `cd`;
	$pkgPath = $res;

	$res = `dir /b *_gcce.pkg`;
	chomp ($res);

	if ($res eq "")
	{
		print STDERR " failed: .pkg file is not found!\n\n";
	}
	else 
	{


		$pkgName = $res;
		chomp($pkgName);

		print STDERR " $pkgName:";

		checkHardcodedPath($pkgName);

		$res = `makesis $pkgName`;
		print STDERR "\n$res\n\n";
	}
	}






	if ($target eq "gcce_udeb")					     	     # GCCE UDEB
	{									     
	# build gcce udeb
	#
	print STDERR "---> abld build gcce udeb (before freeze):\n";
	$res = `abld build gcce udeb`;
	print STDERR "$res\n\n";
	print ".";

	#  freeze exports gcce
	#
	print STDERR "---> abld freeze gcce:\n";
	$res = `abld freeze gcce`;
	print STDERR "$res\n\n";
	print ".";

	# build app after freeze
	#
	print STDERR "---> abld build gcce udeb (after freeze):\n";
	$res = `abld build gcce udeb`;
	print STDERR "$res\n\n";
	print ".";
	}






















	if ($target eq "clean")						     # CLEAN
	{									     
	# clean
	#
	print STDERR "---> abld clean:\n";
	$res = `abld clean`;
	print STDERR "$res\n\n";
	print ".";
	}




	if ($target eq "reallyclean")					     # REALLYCLEAN
	{									     
	# reallyclean
	#
	print STDERR "---> abld reallyclean:\n";
	$res = `abld reallyclean`;
	print STDERR "$res\n\n";
	print ".";
	}



	# close logfile 						
	print STDERR "\n\n\n\n\n\n";
	close STDERR;
	print "\n";
}





sub checkHardcodedPath($pkgName)
{
	$pkg=$pkgName;

	unless (open FILE, "<$pkg")
	{ 
	print "\nError: can't open $pkg\n\n"; 
	return;
	}

	my @pkgFileLines = <FILE>;

	foreach my $line (@pkgFileLines)
	{
		$line =~ s#;.*?\n##gs; # remove comment lines
		$line =~ tr/A-Z/a-z/;

		if ($line =~ m /c:\\/)
		{
			print STDERR " Warning: hardcoded installation path!";
			print " Warning: hardcoded installation path!";
			last;
		}
	}
	close FILE;
}