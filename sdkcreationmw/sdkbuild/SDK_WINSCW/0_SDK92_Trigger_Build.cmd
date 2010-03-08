@rem
@rem Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

@echo off

rem SDK build start up script.
rem Version:  1.2


setlocal
set log_dir="Logs"
Set Log_file=%log_dir%\S60_create_sdk
set Target="C++ Java Licensee Hybrid"
set Build_phase=all
set Tidy=false
set Debug=false
set Walker=false
set SMS=false
set San_Test=false
set validation=false
mkdir %log_dir%

rem parsing cmd line parameters and putting them into ini file
set _file=%0
perl -x %_file% %*

:BEGINPARAMS

rem Check if any parameters were given
if %1'==' (
	goto ENDPARAMS
)

rem Check if C++ has been specified
if /i %1 EQU /c (
	set Target=C++
	set Build_phase=cpp_publish.log
	shift
	goto BEGINPARAMS
)

rem Check if Java has been specified
if /i %1 EQU /j (
	set Target=Java
	set Build_phase=java_publish.log
	shift
	goto BEGINPARAMS
)

rem Check if Licensee has been specified
if /i %1 EQU /l (
	set Target=Licensee
	set Build_phase=licensee_package.log
	shift
	goto BEGINPARAMS
)

rem Check if Hybrid has been specified
if /i %1 EQU /h (
	set Target=Hybrid
	set Build_phase=sdk_publish.log
	shift
	goto BEGINPARAMS
)

rem Check if Debug has been requested
if /i %1 EQU /d (
	set Debug=true
	shift
	goto BEGINPARAMS
)

rem Check if ClassWalker run has been requested
if /i %1 EQU /w (
	set Walker=true
	shift
	goto BEGINPARAMS
)

rem Check if Tidy up has been requested
if /i %1 EQU /t (
	set Tidy=true
	shift
	goto BEGINPARAMS
)
rem Check if SMS sending has been requested
if /i %1 EQU /s (
	set SMS=true
	shift
	goto BEGINPARAMS
)

rem build environment validation
if /i %1 EQU /v (	
	set validation=true
	shift
	goto BEGINPARAMS
)
rem Check for nightly build
if /i %1==nb (
	set Build_phase=nb
	shift
	goto BEGINPARAMS
)

rem Check for Sanity Test
if /i %1==san_test (
	set San_Test=%1
	goto ENDPARAMS
)

rem Show help
if /i %1 EQU /? (
	echo ' 0_SDK_Trigger_Build.cmd [/c ^| /j ^| /l ^| /h] [/t] [/d] [/w] [/s] [nb [san_test]  ^| <phase>] [/o param_x=value ...]
	echo '	/c - Build C++ SDK
	echo '	/j - Build Java SDK
	echo '	/l - Build Licensee SDK
	echo '	/h - Build Hybrid SDK
	echo '	/d - Drop to perl debugger
	echo '	/t - Do Tidy up ie. clean up build history
	echo '	/w - Execute ClassWalker run at the end of build
	echo '	/s - Send SMS
	echo '	/o - Options, alternative settings of values from sdk.ini
	echo '	     /o argument must be the last one followed by any 
	echo '            number of "key=value" pairs
	echo ' 	/? - Show this help
	echo ' 	nb - Do nightly build, where target is all if not specified with /c, /j or /l
	echo '	phase - Build with specified target phase, file of the specified target is removed
	echo '  san_test - enable SDK sanity test '
	echo '  /v - Check build environment'
	goto END
)

rem Check if a certain phase has been given
if not %1'==' (
	set Build_phase=%1
	if %Debug% EQU true (
  	perl -d -mSDK_utils -e SDK_utils::remove_target %1
  ) else (
	  perl -mSDK_utils -e SDK_utils::remove_target %1
	)
	rem del phase\%1
	goto ENDPARAMS
)

:ENDPARAMS

echo Target: %Target%
echo Build phase: %Build_phase%

rem Do Tidy up ie. remove all files starting with _ from build script directory
if %Tidy% EQU true (
	echo on
	perl Create_SDK.pl "phase\tidy_up"
	@echo off
)

rem Run nightly build
if %Build_phase% EQU nb (
	echo on
	perl Create_SDK.pl "phase\tidy_up"
	@echo off
	if %Target% EQU C++ (
		set Build_phase=cpp_publish.log
	)
	if %Target% EQU Java (
		set Build_phase=java_publish.log
	)
	if %Target% EQU Licensee (
		set Build_phase=licensee_package.log
	)
	if %Target% EQU Hybrid (
		set Build_phase=sdk_publish.log
	)
	if %Target% EQU "C++ Java Licensee" (
		set Build_phase=all
	)
)

if %validation% EQU true (	
	perl validate_build_path.pl %Build_phase%		
)

echo Target: %Target%
echo Build phase: %Build_phase%

:BUILD
rem Start actual building
if %Debug% EQU true (
  echo on
  perl -d Create_SDK.pl "phase\%Build_phase%"
  @echo off
) else (
  echo on
  perl Create_SDK.pl "phase\%Build_phase%" > %Log_file%_x.log 2>&1
  @echo off
)
if %Build_phase% == sdk_publish.log (
  echo on
  if %Walker% EQU true (
    if %SMS% EQU true (
      perl post_build9_7.pl mail_sms_cw %Log_file%.log > post_build.log 2>&1
    ) else (
      perl post_build9_7.pl mail_cw %Log_file%.log > post_build.log 2>&1
    )
  ) else (
    if %SMS% EQU true (
      perl post_build9_7.pl mail_sms %Log_file%.log > post_build.log 2>&1
    ) else (
	echo perl post_build9_7.pl mail %Log_file%.log 
      perl post_build9_7.pl mail %Log_file%.log > post_build.log 2>&1
    )
  )
    @echo off
)
goto END

:PERL
#!perl

print "params were @ARGV\n";
our $file = "cmd.ini";	
system "del /s $file";
open(INI, "> $file") or die "cannot open \n";


our $option_encountered = 0;
#print "option_encountered=$option_encountered\n";
while ( defined (my $param= shift(@ARGV))  ){
#	print ">$param<\n";
#	print "option_encountered=$option_encountered\n";
	
	if ($option_encountered == 1){
		print "$param\n";
		print INI "#start\n" or warn "cannot write, $?, $!\n";
		print INI $param or die "cannot write\n";
		print INI "\n";
	}
	if ($param=~m/\/o/){
#		print "/o found\n";
		$option_encountered = 1;
	}
	
	
}
close INI;

__END__
:END

if %San_Test% EQU san_test (
#echo Copy phase to Nightly_Build Share
#perl publish_Phase.pl
#echo Testing 
K:
perl run_Test_Suite.pl -conf 92_sdk_test_hp.ini
)

@endlocal

