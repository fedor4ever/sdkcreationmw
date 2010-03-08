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

package ifsheets;
############################################
# 13.6.2005: 
#  - Added support for the Removed column in the black list.
#  - Added support for "For Adaptation" And "Extended SDK" columns
#    in the readInfo*2 functions (just to read the data into the hash).
#  - currently there is NO support to get extended SDK /for adaptation API lists
#    based on some release.
############################################


##########################################
# Libraries used
##########################################
use strict;
use Win32::OLE qw(in with);
use Win32::OLE::Const 'Microsoft Excel';
use Exporter ();     # Needed to make constants available to caller.
use excel_support;    # opening/closing workbook, etc.. functions.

##########################################
# Defining all the "global" variables, that we are using.
##########################################
use vars qw(
            @ISA
            @EXPORT
            );

@ISA = qw(Exporter); # Inherit from Exporter module (import method needed).

# Define all the functions defined by this module
use subs qw(
	    isSupported
	    addToHash
	    isRowWanted
	    setRowValue
	    getError
	    checkRel
	    checkApiRelRoadMap
	    checkFileRelRoadMap
	    readInfo
	    checkConflicts
	    readInfo2
	    readCRepInfo2
	    readBlackList
	    );

##########################################
# Define the exports from this module that can be used
# Outside
##########################################
@EXPORT = qw(
             &getExcelApp
	     &quitExcelApp
	     &isSupported
	     &openWorkbook
	     &getWorkSheet
	     &readInfo
	     &checkConflicts
	     &readInfo2
	     &readCRepInfo2
	     &readBlackList
             );


##########################################
# Global variables inside this module (constants
##########################################
# For 2.0 template version there are 32 columns
my $globalMaxColCount = 32;

# How many empty lines we are allowed to encounter before we 
# stop handling specific sheet. 
my $globalMaxEmptyRowCount = 10;

my $currFileName = "";

##########################################
# Error constants
##########################################
my $TEMPLATE_SYNTAX_ERROR = 0;
my $API_DESC_MISSING_ERROR = 10;
my $API_REL_MISSING_ERROR = 11;
my $API_REL_SYNTAX_ERROR = 12;
my $API_REL_LOGICAL_ERROR = 13;
my $API_HEADERS_IN_ROW_ERROR = 14;
my $API_ADAPTATION_ERROR = 15;
my $DLL_SYNTAX_ERROR = 20;
my $LIB_SYNTAX_ERROR = 30;
my $FILE_MORE_THAN_ONE_FILE_ERROR = 40;
my $FILE_COLUMN_SYNTAX_ERROR = 41;
my $FILE_REL_SYNTAX_ERROR = 42;
my $FILE_REL_LOGICAL_ERROR = 43;



###########################################################################
# Functions checks if the template version is supported or not.
#
# We only support template version 1.5,1.5.1 and 1.6
# From 1.5 onwards the template version is located in the "Instructions"-sheet
#
# Params: refToWorkbook,refToTemplateVersion
#
# Return: 0 not supported
#         1 is supported
#
#         Also the found template version is stored to the refToTemplateVersion
###########################################################################
sub isSupported
{
    my $refToWb = shift;
    my $refToTemplateVersion = shift;

    # Get the instructions sheet, since it contains the template version.
    my $refToWorksheet = getWorkSheet $refToWb,"Instructions";

    # The instructions sheet => then try the main page. In 1.8 the instructions
    # page was removed.
    if(! defined($refToWorksheet))
    {
	$refToWorksheet = getWorkSheet $refToWb,"Main";
    }

    # No main or instructions sheet => not supported.
    if(! defined($refToWorksheet))
    {
	return 0;
    }

    # the template version in in cell C2. (both tabs the place is same)
    my $templVersion  = $$refToWorksheet->Cells( 2, 3  )->{ 'Value' };
    
    if(defined($refToTemplateVersion))
    {
	$$refToTemplateVersion = $templVersion;
    }

    # Support version 1.5 and 1.5.1
    if( defined($templVersion) && $templVersion =~ /^\s*1\.5(\.1)*$/)
    {
	return 1;
    }
    # Support version 1.6
    if( defined($templVersion) && $templVersion =~ /^\s*1\.6$/)
    {
	return 1;
    }
    # Support version 1.7 and 1.7.*
    if( defined($templVersion) && $templVersion =~ /^\s*1\.7(\.[0-9]+)*$/)
    {
	return 1;
    }
    # Support version 1.8 and 1.8.*
    if( defined($templVersion) && $templVersion =~ /^\s*1\.8(\.[0-9]+)*$/)
    {
	return 1;
    }
    # Support version 1.9 and 1.9.*
    if( defined($templVersion) && $templVersion =~ /^\s*1\.9(\.[0-9]+)*$/)
    {
	return 1;
    }
    # Support version 2.0 and 2.0.*
    if( defined($templVersion) && $templVersion =~ /^\s*2\.0(\.[0-9]+)*$/)
    {
	return 1;
    }

    return 0;
}

###########################################################################
# Function defines what is the next api ordinal to be used in the 
# hash return by readInfo2 and readCRepInfo2 methods.
# there.
#
# Params: refToHash
#
# Return: next ordinal
##########################################################################
sub sortNumerically
{
    $a <=> $b;
}
sub getNextOrdinal
{
    my $refToHash = shift;


    my @ordinalsInOrder =  sort sortNumerically ( keys(%$refToHash));
    my $highestValue = pop @ordinalsInOrder;
    $highestValue++;

    return $highestValue;
}

###########################################################################
# Function adds the information in the str into the has, if the information is not
# there.
# It also separates command separated items before adding them
#
# Params: refToHash, text added.
#
# Return: -
###########################################################################
sub addToHash
{
    my $refToHash = shift;
    my $str = shift;
    
    # No need to do anything, if there is nothing to do.
    if(! defined($str) || $str =~ /^\s*$/)
    {
	return;
    } 

    # The items can be separated by "," and there could be additional
    # space before of after the separator.
    my @items = split /\s*[,\n ]+\s*/,$str;

    my $item;
    
    # Add each item to the hash. This is only done, if the item does not
    # exist there before.
    foreach $item (@items)
    {
	# all spaces and newlines taken away before addition
	$item =~ s/\s*//g;
	chomp($item);

	
	# Add if not added earlier (avoiding duplicate entries)
	if( !defined( $$refToHash{$item}) && $item !~ /^\s*N\/A\s*$/i && $item !~ /^\s*\-\s*$/)
	{
	    # It does not matter what is the value, since we are only interested in the
	    # key.
	    $$refToHash{$item} = 1; 
	}
    }
}
###########################################################################
# Function expects to find the fromStr and obsoletedStr in format "Sx0 <rel>, Sx0 <rel>", where x is 6 0r 9.
# It tries to determinate, if the range matches to the given release for the given platform
# Possible combinations "", "S60 <rel>","S90 <rel>","S60 <rel>, S90 <Rel>"
#
# Params: plat, rel, isAPIrow,fromStr, obsoletedStr
#
# Return:
#   1 is part of wanted platform
#   2 has been removed in this/prev releases
#   3 is coming in future releases
#   4 no release for the API in given str
# Note: 0 is used outside of this function.
###########################################################################
sub isRowWanted
{
    my $plat = shift;
    my $rel = shift;
    my $isApiRow = shift;
    my $fromStr = shift;
    my $obsoletedStr = shift;
    
    #######################################################
    # First check the obsoleted field. If that matches, it really does not matter
    # what is the from string.
    #######################################################
    if(defined($obsoletedStr) && $obsoletedStr =~ /$plat/i)
    {
	# the value was defined and contains the platform wanted.
	# Now extract the release for the wanted platform.
	if($obsoletedStr =~ /$plat\s+([0-9.]+)/i)
	{
	    my $obsoletedRelInSheet = $1;
	
	    # check, if the line has been obsoleted, if yes then return false.
	    if($rel >= $obsoletedRelInSheet)
	    {
		return 2;
	    }
	}
    }

    #######################################################
    # If the row was not obsoleted, then check the from-value.
    #######################################################

    # If there is no value in the field the item is handled differently depending
    # if the row is API row or header row.
    if( ! defined($fromStr) || $fromStr =~ /^\s*$/ ||  $fromStr !~ /$plat/i)
    {
	return 4;
    }
    
    # The platform was specified in the field. Now find out the number
    if($fromStr =~ /$plat\s+([0-9.]+)/i)
    {
	my $platRelInSheet = $1;
	if( $rel >= $platRelInSheet)
	{
	    # belongs to the release
	    return 1;
	}
	else
	{
	    # does not belong to the release
	    return 3;
	}
    }

    # We should only come here, if the information was somehow wrongly specified.
    # (for instance numbers in the release was not a number, but something else)
    return 4;
}


###########################################################################
# function sets the given rel value, if the value was defined
#
# Params: referenceToVariable, value
#
###########################################################################
sub setRelValue
{
    my $refToVar = shift;
    my $value = shift;

    if(defined($value) && $value !~ /^\s*$/)
    {
	$$refToVar = $value;
    }
}

###########################################################################
# Function generates an error item
#
# Params: errNbr, ... <other information>
#
# Return: reference to an error item list
###########################################################################
sub getError
{
    my $errNbr = shift;
    my $refToDetailsList = \@_;
    
    my @errItem;
    push(@errItem,$errNbr);
    push(@errItem,$currFileName);
    push(@errItem,@$refToDetailsList);
   
    return \@errItem;
}


###########################################################################
# Checks the syntax of the given release field
#
# Params: checked rel value
#
# Return:  0 OK
#         >0 NOK
###########################################################################
sub checkRel
{
    my $checkedRel = shift;

    # No value defined => return OK
    if($checkedRel == -1)
    {
	return 0;
    }

    # Only S60 or S90 release specified
    if($checkedRel =~ /^\s*S[69]0 [0-9]+\.[0-9]+\s*$/i)
    {
	return 0;
    }
    # Both S60 or S90 release specified
    if($checkedRel =~ /^\s*S[69]0 [0-9]+\.[0-9]+\s*\,\s*S[69]0 [0-9]+\.[0-9]+\s*$/i)
    {
	return 0;
    }
    
    # Otherwise error
    return 1;
}


###########################################################################
# Function checks that the information in the files for given release are OK.
#
# Params: refToErrList, checkedPlatform, apiName, sdkRel,oemRel,internalRel,privateRel
#
# Return:  0 no errors found
#          > 0 amount of  errors found
###########################################################################
sub checkApiRelRoadMap
{
    my $refToErrorList = shift;
    my $checkedPlatform = shift;
    my $apiName = shift;
    my $apiSdkRel = shift;
    my $apiOemRel = shift;
    my $apiInternalRel = shift;
    my $apiPrivateRel = shift;
    my $apiDeprecatedRel = shift;
    my $apiObsoletedRel = shift;
    
    my $sdk = -1;
    my $oem = -1;
    my $int = -1;
    my $priv = -1;
    my $dep = -1;
    my $obs = -1;

    my $errorsBeforeCheck = scalar(@$refToErrorList);
    
    if($apiSdkRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$sdk = $1;
    }
    if($apiOemRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$oem = $1;
    }
    if($apiInternalRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$int = $1;
    }
    if($apiPrivateRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$priv = $1;
    }
    if($apiDeprecatedRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$dep = $1;
    }
    if($apiObsoletedRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$obs = $1;
    }
    
    if($sdk != -1)
    {
	if($sdk < $oem || $sdk < $int || $sdk <  $priv)
	{
	    push(@$refToErrorList,getError($API_REL_LOGICAL_ERROR,
					   $apiName, "SDK release"));
	}
    }
    if($oem != -1)
    {
	if($oem < $int || $oem <  $priv)
	{
	    push(@$refToErrorList,getError($API_REL_LOGICAL_ERROR,
					   $apiName, "Oem release"));
	}
    }
    if($int != -1)
    {
	if($int <  $priv)
	{
	    push(@$refToErrorList,getError($API_REL_LOGICAL_ERROR,
					   $apiName, "Internal release"));
	}
    }

    if($dep != -1)
    {
	if($dep < $sdk || $dep < $oem || $dep < $int || $dep <  $priv)
	{
	    push(@$refToErrorList,getError($API_REL_LOGICAL_ERROR,
					   $apiName, "Deprecated release value."));
	}
    }
    if($obs != -1)
    {
	if($obs < $sdk || $obs < $oem || $obs < $int || $obs <  $priv || $obs < $dep)
	{
	    push(@$refToErrorList,getError($API_REL_LOGICAL_ERROR,
					   $apiName, "Obsoleted release value."));
	}
    }

    return  scalar(@$refToErrorList) - $errorsBeforeCheck;
}

###########################################################################
# Function checks that the information in the files for given release are OK.
#
# Params: refToErrList, checkedPlatform, apiName, fileName,apiSdkRel,apiOemRel,apiInternalRel,apiPrivateRel, 
#         apiDeprecatedRel,apiObsoletedeRel,fileSdkRel,fileOemRel,fileInternalRel,filePrivateRel, 
#         fileDeprecatedRel,fileObsoletedeRel
#
# Return:  0 no errors found
#          > 0 amount of  errors found
###########################################################################
sub checkFileRelRoadMap
{
    my $refToErrorList = shift;
    my $checkedPlatform = shift;
    my $apiName = shift;
    my $fileName = shift;
    my $apiSdkRel = shift;
    my $apiOemRel = shift;
    my $apiInternalRel = shift;
    my $apiPrivateRel = shift;
    my $apiDeprecatedRel = shift;
    my $apiObsoletedRel = shift;
    my $fileSdkRel = shift;
    my $fileOemRel = shift;
    my $fileInternalRel = shift;
    my $filePrivateRel = shift;
    my $fileDeprecatedRel = shift;
    my $fileObsoletedRel = shift;
    
    my $apiSdk = -1;
    my $apiOem = -1;
    my $apiInt = -1;
    my $apiPriv = -1;
    my $apiDep = -1;
    my $apiObs = -1;
    my $fileSdk = -1;
    my $fileOem = -1;
    my $fileInt = -1;
    my $filePriv = -1;
    my $fileDep = -1;
    my $fileObs = -1;

    my $errorsBeforeCheck = scalar(@$refToErrorList);

    #
    # Parse the wanted platform release from all the fields.
    #
    if($apiSdkRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$apiSdk = $1;
    }
    if($apiOemRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$apiOem = $1;
    }
    if($apiInternalRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$apiInt = $1;
    }
    if($apiPrivateRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$apiPriv = $1;
    }
    if($apiDeprecatedRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$apiDep = $1;
    }
    if($apiObsoletedRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$apiObs = $1;
    }
    if($fileSdkRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$fileSdk = $1;
    }
    if($fileOemRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$fileOem = $1;
    }
    if($fileInternalRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$fileInt = $1;
    }
    if($filePrivateRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$filePriv = $1;
    }
    if($fileDeprecatedRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$fileDep = $1;
    }
    if($fileObsoletedRel =~ /$checkedPlatform\s+([0-9]+\.[0-9]+)/)
    {
	$fileObs = $1;
    }


    # --------------------------------------------------------
    # Actual error checking
    # --------------------------------------------------------


    #
    # Check that the file row releases make sence in logical way
    #
    my $currErrAmount = scalar(@$refToErrorList);
    if($fileSdk != -1)
    {
	if($fileSdk < $fileOem || $fileSdk < $fileInt || $fileSdk <  $filePriv)
	{
	    push(@$refToErrorList,getError($FILE_REL_LOGICAL_ERROR,
					   "$apiName :: $fileName", "SDK release \"$checkedPlatform $fileSdk\""));
	}
    }
    if($fileOem != -1)
    {
	if($fileOem < $fileInt || $fileOem <  $filePriv)
	{
	    push(@$refToErrorList,getError($FILE_REL_LOGICAL_ERROR,
					   "$apiName :: $fileName", "Oem release \"$checkedPlatform $fileOem\""));
	}
    }
    if($fileInt != -1)
    {
	if($fileInt <  $filePriv)
	{
	    push(@$refToErrorList,getError($FILE_REL_LOGICAL_ERROR,
					   "$apiName :: $fileName", "Internal release \"$checkedPlatform $fileInt\""));
	}
    }

    if($fileDep != -1)
    {
	if($fileDep < $fileSdk || $fileDep < $fileOem || $fileDep < $fileInt || $fileDep <  $filePriv)
	{
	    push(@$refToErrorList,getError($FILE_REL_LOGICAL_ERROR,
					   "$apiName :: $fileName", "Deprecated release \"$checkedPlatform $fileDep\""));
	}
    }
    if($fileObs != -1)
    {
	if($fileObs < $fileSdk || $fileObs < $fileOem || $fileObs < $fileInt || $fileObs <  $filePriv || $fileObs < $fileDep)
	{
	    push(@$refToErrorList,getError($FILE_REL_LOGICAL_ERROR,
					   "$apiName :: $fileName", "Obsoleted release \"$checkedPlatform $fileObs\""));
	}
    }

    #
    # Check the release information for the file compared to the release information defined for the API
    #  (these are only checked, if there was no syntax errors in the release fields for the file).
    #
    if($currErrAmount == scalar(@$refToErrorList))
    {
	#
	# Compare the SDK information in the file against the API rel information
	#
	if($fileSdk != -1)
	{
	    # If API is not SDK => error, since file was defined as SDK
	    if($apiSdk == -1)
	    {
		push(@$refToErrorList,
		     getError($FILE_REL_LOGICAL_ERROR,
			      "$apiName :: $fileName", "File defined as SDK, but API is not a SDK API."));	       
	
	    }
	    else
	    {
		if($fileSdk < $apiSdk)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as SDK from \"$checkedPlatform $fileSdk\", ",
				  "but API defined as SDK only from \"$checkedPlatform $apiSdk\" onwards."));
		    
		}
		if($apiObs != -1 && $fileSdk > $apiObs)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as SDK from \"$checkedPlatform $fileSdk\",",
				  "but API was defined as obsoleted \"$checkedPlatform $apiObs\" onwards."));
		    
		}
	    }
	}
	#
	# Compare the Oem information in the file against the API rel information
	#
	if($fileOem != -1)
	{
	    # If API is not Oem => error, since file was defined as Oem
	    if($apiOem == -1)
	    {
		push(@$refToErrorList,
		     getError($FILE_REL_LOGICAL_ERROR,
			      "$apiName :: $fileName", "File defined as Oem, but API is not an Oem API."));	
	    }
	    else
	    {
		if($fileOem < $apiOem)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Oem from \"$checkedPlatform $fileOem\",",
				  "but API defined as OEM only from \"$checkedPlatform $apiOem\" onwards."));
		    
		}
		if($apiSdk != -1 && $fileOem >= $apiSdk)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Oem from \"$checkedPlatform $fileOem\", " ,
				  "but API was defined as SDK from \"$checkedPlatform $apiSdk\" onwards."));
	
		}
		if($apiObs != -1 && $fileOem > $apiObs)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Oem from \"$checkedPlatform $fileOem\", ",
				  "but API was defined as obsoleted \"$checkedPlatform $apiObs\" onwards."));
		}
	    }
	}
	#
	# Compare the Internal information in the file against the API rel information
	#
	if($fileInt != -1)
	{
	    # If API is not Internal => error, since file was defined as such
	    if($apiInt == -1)
	    {
		push(@$refToErrorList,
		     getError($FILE_REL_LOGICAL_ERROR,
			      "$apiName :: $fileName", 
			      "File defined as Internal, but API is not an Internal API."));	
	    }
	    else
	    {
		if($fileInt < $apiInt)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Internal from \"$checkedPlatform $fileInt\", " ,
				  "but API defined as Internal only from \"$checkedPlatform $apiInt\" onwards."));
		}
		if($apiSdk != -1 && $fileInt >= $apiSdk )
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Internal from \"$checkedPlatform $fileInt\", " ,
				  "but API was defined as SDK from \"$checkedPlatform $apiSdk\" onwards."));
	
		}
		if($apiOem != -1 && $fileInt >= $apiOem)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Internal from \"$checkedPlatform $fileInt\", " ,
				  "but API was defined as Oem from \"$checkedPlatform $apiOem\" onwards."));
	
		}
		if($apiObs != -1 && $fileInt > $apiObs)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Internal from \"$checkedPlatform $fileInt\", " ,
				  "but API was defined as obsoleted \"$checkedPlatform $apiObs\" onwards."));
		}
	    }
	}
	#
	# Compare the Internal information in the file against the API rel information
	#
	if($filePriv != -1)
	{
	    # If API is not Private => error, since file was defined as such
	    if($apiPriv == -1)
	    {
		push(@$refToErrorList,
		     getError($FILE_REL_LOGICAL_ERROR,
			      "$apiName :: $fileName", 
			      "File defined as Private, but API is not an Private API."));
	    }
	    else
	    {
		if($filePriv < $apiPriv)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Private from \"$checkedPlatform $filePriv\", " ,
				  "but API defined as Private only from \"$checkedPlatform $apiPriv\" onwards."));
		}
		if($apiSdk != -1 && $filePriv >= $apiSdk)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Private from \"$checkedPlatform $filePriv\", " ,
				  "but API was defined as SDK from \"$checkedPlatform $apiSdk\" onwards."));
	
		}
		if($apiOem != -1 && $filePriv >= $apiOem)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Private from \"$checkedPlatform $filePriv\", " ,
				  "but API was defined as Oem from \"$checkedPlatform $apiOem\" onwards."));
		}
		if($apiInt != -1 && $filePriv >= $apiInt)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Private from \"$checkedPlatform $filePriv\", " ,
				  "but API was defined as Internal from \"$checkedPlatform $apiInt\" onwards."));
		}
		if($apiObs != -1 && $filePriv > $apiObs)
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_LOGICAL_ERROR,
				  "$apiName :: $fileName", 
				  "File defined as Private from \"$checkedPlatform $filePriv\", " ,
				  "but API was defined as obsoleted \"$checkedPlatform $apiObs\" onwards."));
		}
	    }
	}
    }
}

################################################
# Function adds the lib to the hash, if it contains valid data
################################################
sub addLibToHash
{
    my $refToLibHash = shift;
    my $lib = shift;

    if($lib !~ /^\s*N\/A\s*$/i && $lib !~ /^\s*\-\s*$/ && 
       $lib !~ /\s*\(*\s*ECOM\s*\)*\s*$/i && $lib !~ /^\s*tbd\s*$/i)
    {
	addToHash $refToLibHash,$lib;
    }
}

################################################
# Function adds the dll to the hash, if it contains valid data
################################################
sub addDllToHash
{
    my $refToDllHash = shift;
    my $dll = shift;

    if($dll !~ /^\s*N\/A\s*$/i && $dll !~ /^\s*\-\s*$/ && 
       $dll !~ /\s*\(*\s*ECOM\s*\)*\s*$/i && $dll !~ /^\s*tbd\s*$/i)
    {
	addToHash $refToDllHash,$dll;
    }
}
###########################################################################
# Function that reads specified platform release information from the given worksheet
#  The information is put to the hashed passed as parameters
##  		if($dll !~ /^\s*N\/A\s*$/i && $dll !~ /^\s*\-\s*$/ && 
#  		   $dll !~ /\s*\(*\s*ECOM\s*\)*\s*$/i && $dll !~ /^\s*tbd\s*$/i)
#  		{
#  		    addToHash $refToDllHash,$dll;
#  		}

# Params: referenceToWorksheet,wantedPlatform, wantedRelease, reftoHeaderHash,refToDllHash, refToLibHash,
#         reftoNonSdkHeaders, [refToNonSdkLibsHash]
#
# Return: -
###########################################################################
sub readInfo     
{
    my $refToWorksheet = shift;
    my $plat = shift;
    my $rel = shift;
    my $reftoHeaderHash = shift;  # SDK headers
    my $refToDllHash = shift;     # SDK dll's
    my $refToLibHash = shift;     # SDK Libs
    my $refToNonSdkHeaderHash = shift;
    my $refToFutureHeaderHash = shift; # Files that are coming in future releases 
    my $refToRemovedHeaderHash = shift; # Files that are removed in previous releases 
    my $refToNonSdkLibHash = shift;    # optional, migth not exists
    my $refToFutureLibHash = shift;    
    my $refToRemovedLibHash = shift;   
    
    # check that sheet really exists (it could be missing)
    if(not defined($refToWorksheet))
    {
	return;
    }
    # handle optionality
    if(! defined($refToNonSdkLibHash))
    {
	my %hash;
	$refToNonSdkLibHash = \%hash;
    }
    # ----------------------------------------------------------------  
    # look to header information
    # ----------------------------------------------------------------  
    # API Name
    # Header file(s)    (or "File(s)" in template version 1.6->)
    # Purpose	
    # Responsible
    # SDK
    # Oem               (or "Partner" in template version 1.6->1.8 or "Domain" from 1.9->)
    # Internal
    # Private   (migth be missing, as is the case in the symbian sheet)	
    # Adaptation
    # Specification
    # DLL
    # Import Lib
    # Other
    # Grouping           (or "Subsystem" in template version 1.6->)
    # Clients
    # Deprecated
    # Obsoleted          (or "Removed" in template version 1.6->)
    # Partner(s)         (Only from 1.6->)
    # Partner Contact(s) (Only from 1.6->)
    # Owner
    # Note
    my $apiNameIndex = getIndexByColName $refToWorksheet,"API Name",$globalMaxColCount;
    my $fileNameIndex = getIndexByColName $refToWorksheet,"Header file(s)",$globalMaxColCount;
    if( !defined($fileNameIndex) || $fileNameIndex < 1)
    {
	$fileNameIndex = getIndexByColName $refToWorksheet,"File(s)",$globalMaxColCount;
    }
    my $purposeIndex = getIndexByColName $refToWorksheet,"Purpose",$globalMaxColCount;
    my $sdkIndex = getIndexByColName $refToWorksheet,"SDK",$globalMaxColCount;
    my $oemIndex = getIndexByColName $refToWorksheet,"Oem",$globalMaxColCount;
    if( !defined($oemIndex) || $oemIndex < 1)
    {
	$oemIndex = getIndexByColName $refToWorksheet,"Partner",$globalMaxColCount;
    }
    if( !defined($oemIndex) || $oemIndex < 1)
    {
	$oemIndex = getIndexByColName $refToWorksheet,"Domain",$globalMaxColCount;
    }
    my $internalIndex = getIndexByColName $refToWorksheet,"Internal",$globalMaxColCount;
    my $privateIndex =  getIndexByColName $refToWorksheet,"Private",$globalMaxColCount;

    my $dllIndex = getIndexByColName $refToWorksheet,"DLL",$globalMaxColCount;
    my $libIndex = getIndexByColName $refToWorksheet,"Import Lib",$globalMaxColCount;
    my $obsoletedIndex =  getIndexByColName $refToWorksheet,"Obsoleted",$globalMaxColCount;
    if( !defined($obsoletedIndex) || $obsoletedIndex < 1)
    {
	$obsoletedIndex = getIndexByColName $refToWorksheet,"Removed",$globalMaxColCount;
    }

    my $row = 1;
    my $isAPIRow = 0;
    my $isRowWanted = 0;  # is the row part of given SDK release
    my $isAPIWanted = 0;  # is the api part of given SDK release
    my $emptyRowCount = 0;

    ######################################################################
    # Loop row by row until we have found $globalMaxEmptyRowCount of rows
    # empty one after each other.
    ######################################################################
    for( $row = 2;; $row++ ) 
    {	         
	#------------------------------------------------------------
	# Read the needed rows from the row
	#------------------------------------------------------------
	my $apiName   = $$refToWorksheet->Cells( $row, $apiNameIndex  )->{ 'Value' };
        my $fileName  = $$refToWorksheet->Cells( $row, $fileNameIndex  )->{ 'Value' };
        my $purpose   = $$refToWorksheet->Cells( $row, $purposeIndex  )->{ 'Value' };
        my $sdk       = $$refToWorksheet->Cells( $row, $sdkIndex  )->{ 'Value' };
        my $oem       = $$refToWorksheet->Cells( $row, $oemIndex  )->{ 'Value' };
        my $internal  = $$refToWorksheet->Cells( $row, $internalIndex  )->{ 'Value' };
        my $private;
	if($privateIndex > 0)
	{
	    $private = $$refToWorksheet->Cells( $row, $privateIndex  )->{ 'Value' };
	}
	my $dll       = $$refToWorksheet->Cells( $row, $dllIndex  )->{ 'Value' };
	my $lib       = $$refToWorksheet->Cells( $row, $libIndex  )->{ 'Value' };
	my $obsoleted = $$refToWorksheet->Cells( $row, $obsoletedIndex  )->{ 'Value' };
	
	if($apiName =~ /^\s*Comment:/ || $purpose =~ /removed by nokia/i)
	{
	    next;
	}
	
	#------------------------------------------------------------
	# We only allow $globalMaxEmptyRowCount empty rows, before we stop
	# handling the specific sheet
	#------------------------------------------------------------
	if(! (defined($apiName) || defined($fileName)|| defined($dll) ||
	      defined($lib) || defined($obsoleted)))
	{
	    $emptyRowCount++;
	    if($emptyRowCount > $globalMaxEmptyRowCount)
	    {
		last;
	    }
	}
	else
	{
	    # Was not empty line, reset counter.
	    $emptyRowCount = 0;
	}
	
	#------------------------------------------------------------
	# Check, if is an api line and is the API wanted
	#------------------------------------------------------------
        $isAPIRow = 0;
        if(defined($apiName) and $apiName !~ /^\s*$/)
        {
	    $isAPIRow = 1;
	}
	# Check, if the line is wanted
        $isRowWanted = isRowWanted  $plat,$rel,$isAPIRow,$sdk,$obsoleted;
	
	# Case where the api/file was not wanted 
	if($isRowWanted == 4)
	{
	    my $isRowWanted2 = isRowWanted  $plat,$rel,$isAPIRow,$oem,$obsoleted;
	    if($isRowWanted2 == 4)
	    {
		$isRowWanted2 = isRowWanted  $plat,$rel,$isAPIRow,$internal,$obsoleted;
	    }
# Do not check the private field, since those are not in epoc32/include
#  	    if($isRowWanted2 == 4 && $privateIndex > 0)
#  	    {
#  		$isRowWanted2 = isRowWanted  $plat,$rel,$isAPIRow,$private,$obsoleted;
#  	    }
	    
	    if($isRowWanted2 == 1)
	    {
		# means that the row is in the release, but non-sdk
		$isRowWanted = 4;
	    }
	    else
	    {
		$isRowWanted = $isRowWanted2;
	    }
	} # if($isRowWanted == 4)
	
        # ----------------------------------------------------------------  
        # Handle the API line
        # ----------------------------------------------------------------
        if($isAPIRow == 1 || ($isAPIRow == 0 && $isRowWanted != 1))
        {
	    # Store the information in the separate variable, since we
	    # need the information, when handling headers (reason is that
	    # the headers can override the API information, but normally the 
	    # API information is directly the headers information)
	    $isAPIWanted = $isRowWanted if $isAPIRow == 1;
	    if($isAPIWanted == 1 && $isRowWanted != 2 && $isRowWanted != 3)
	    {
		# NOTE: in the following addings, we do not add  "N/A", "TBD","(ECOM)"
		#       "ECOM", "-" values
		#
		# Add the information to the hashes. They migth not all contain
		# information, but the called function handles the case.
		# (for instance there should be no headers defined)
		
		if($fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*DIR:/i)
		{
		    addToHash $reftoHeaderHash,$fileName;
		}
		addDllToHash $refToDllHash,$dll;
		addLibToHash $refToLibHash,$lib;
	    }
	    elsif($isAPIWanted == 2 || $isRowWanted == 2)
	    {
		if($fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*DIR:/i)
		{
		    addToHash $refToRemovedHeaderHash,$fileName;		
		}
		addLibToHash $refToRemovedLibHash,$lib;
	    }
	    elsif($isAPIWanted == 3 || $isRowWanted == 3)
	    {
		if($fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*DIR:/i)
		{
		    addToHash $refToFutureHeaderHash,$fileName;		
		}
		addLibToHash $refToFutureLibHash,$lib;
	    }
	    elsif($isAPIWanted != 4)
	    {
		if($fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*DIR:/i)
		{
		    addToHash $refToNonSdkHeaderHash,$fileName;		
		}
		addLibToHash $refToNonSdkLibHash,$lib;
	    }
	}
        # ----------------------------------------------------------------  
        # Handle the header line, if the API is wanted and row is wanted
        # ----------------------------------------------------------------
        elsif($isRowWanted == 1)
        {
	    if($isAPIWanted == 1)
	    {
		# Add the information to the hashes. They migth not all contain
		# information, but the called function handles the case.
		if($fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*DIR:/i)
		{
		    addToHash $reftoHeaderHash,$fileName;
		}
		addDllToHash $refToDllHash,$dll;
		addLibToHash $refToLibHash,$lib;	    
	    }
	    # If the api row was empty => private or no release specified => do not include it anywhere.
	    elsif($isAPIWanted != 4)
	    {
		if($fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*DIR:/i)
		{
		    addToHash $refToNonSdkHeaderHash,$fileName;
		}
		addLibToHash $refToNonSdkLibHash,$lib;
	    }
	}
        # ----------------------------------------------------------------  
        # Handle the header line, row is not wanted
        # ----------------------------------------------------------------
#  	else
#  	{
#  	    if($fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*DIR:/i)
#  	    {
#  		addToHash $refToNonSdkHeaderHash,$fileName;
#  	    }
#  	    addLibToHash $refToNonSdkLibHash,$lib;	    
#	}
    }
}


###########################################################################
# Function that reads all the information in the given interface sheet and checks 
#  for conlicts/inconsistencies.
# The following things are currently checked:
#   - The 1 row is according to the template (at least the most important fields)
#   - API row
#        *  always contains Description
#        * The SDK/OEM/Internal/Private/deprecated/obsoleted column values are in correct syntax
#        * API row has to contain at least 1 categorization
#        * There are no headers defined in the API row
#   - The lib field 
#        * Does not contain "dll" definitions
#        * No "TBD" values
#   - The dll field 
#        * Does not contain "lib" definitions
#        * No "TBD" values
#   - The header row
#        * exactly one file for each row
#        * only allowed values (no TBD for example)
#
# Params: referenceToWorksheet, Sheet name, refToConflictList
#
# Return: -
#
# The syntax of refToConflictList  is:
#
#  errorTypes:
#     0  = incorrect template format, details is the description as text
#
#   (
#    (errorType,<details, format depends on the errprType (see above))
#   (
###########################################################################
sub checkConflicts
{
    my $refToWorksheet = shift;
    my $sheetName = shift;
    my $refToErrorList = shift;

    # check that sheet really exists (it could be missing)
    if(not defined($refToWorksheet))
    {
	return;
    }

    $sheetName =~ /([a-zA-Z0-9_ \.]+\.xls)/i;
    $currFileName = $1;
    # ----------------------------------------------------------------  
    # look to header information
    # ----------------------------------------------------------------  
    # API Name
    # Header file(s)    (or "File(s)" in template version 1.6->)
    # Purpose	
    # Responsible
    # SDK
    # Oem               (or "Partner" in template version 1.6->1.8 or "Domain" from 1.9->)
    # Internal
    # Private   (migth be missing, as is the case in the symbian sheet)	
    # Adaptation
    # Specification
    # DLL
    # Import Lib
    # Other
    # Grouping           (or "Subsystem" in template version 1.6->)
    # Clients
    # Deprecated
    # Obsoleted          (or "Removed" in template version 1.6->)
    # Partner(s)         (Only from 1.6->)
    # Partner Contact(s) (Only from 1.6->)
    # Owner
    # Note
    my $apiNameIndex = getIndexByColName $refToWorksheet,"API Name",$globalMaxColCount;
    my $fileNameIndex = getIndexByColName $refToWorksheet,"Header file(s)",$globalMaxColCount;
    if( !defined($fileNameIndex) || $fileNameIndex < 1)
    {
	$fileNameIndex = getIndexByColName $refToWorksheet,"File(s)",$globalMaxColCount;
    }
    my $purposeIndex = getIndexByColName $refToWorksheet,"Purpose",$globalMaxColCount;
    my $sdkIndex = getIndexByColName $refToWorksheet,"SDK",$globalMaxColCount;
    my $oemIndex = getIndexByColName $refToWorksheet,"Oem",$globalMaxColCount;
    if( !defined($oemIndex) || $oemIndex < 1)
    {
	$oemIndex = getIndexByColName $refToWorksheet,"Partner",$globalMaxColCount;
    }
    if( !defined($oemIndex) || $oemIndex < 1)
    {
	$oemIndex = getIndexByColName $refToWorksheet,"Domain",$globalMaxColCount;
    }
    my $internalIndex = getIndexByColName $refToWorksheet,"Internal",$globalMaxColCount;
    my $privateIndex =  getIndexByColName $refToWorksheet,"Private",$globalMaxColCount;
    my $adaptIndex = getIndexByColName $refToWorksheet,"Adaptation",$globalMaxColCount;
    my $dllIndex = getIndexByColName $refToWorksheet,"DLL",$globalMaxColCount;
    my $libIndex = getIndexByColName $refToWorksheet,"Import Lib",$globalMaxColCount;
    my $deprecatedIndex =  getIndexByColName $refToWorksheet,"Deprecated",$globalMaxColCount;
    my $obsoletedIndex =  getIndexByColName $refToWorksheet,"Obsoleted",$globalMaxColCount;
    if( !defined($obsoletedIndex) || $obsoletedIndex < 1)
    {
	$obsoletedIndex = getIndexByColName $refToWorksheet,"Removed",$globalMaxColCount;
    }

    #############################
    # Check, that column names are correct ones
    #############################
    if($apiNameIndex < 0 || $fileNameIndex < 0 || $sdkIndex < 0 || 
       $oemIndex < 0 ||  $internalIndex < 0)
    {
	push(@$refToErrorList,
	     getError($TEMPLATE_SYNTAX_ERROR,
		      "The column names are not according to the template."));
	return;
    }

    my $row = 1;
    my $isAPIRow = 0;
    my $isRowWanted = 0;
    my $isAPIWanted = 0;
    my $emptyRowCount = 0;

    my $isConflict = 0;
    my $currApiName = "";
    my $apiSdkRel = -1;
    my $apiOemRel = -1;
    my $apiInternalRel = -1;
    my $apiPrivateRel = -1;
    my $apiDeprecatedRel = -1;
    my $apiObsoletedRel = -1;

    my $currApiRelInfoOK = 0;

    ######################################################################
    # Loop row by row until we have found $globalMaxEmptyRowCount of rows
    # empty one after each other.
    ######################################################################
    for( $row = 2;; $row++ ) 
    {	         
	$isConflict = 0;

	#------------------------------------------------------------
	# Read the needed rows from the row
	#------------------------------------------------------------
	my $apiName   = $$refToWorksheet->Cells( $row, $apiNameIndex  )->{ 'Value' };
        my $fileName    = $$refToWorksheet->Cells( $row, $fileNameIndex  )->{ 'Value' };
        my $purpose   = $$refToWorksheet->Cells( $row, $purposeIndex  )->{ 'Value' };
        my $sdk       = $$refToWorksheet->Cells( $row, $sdkIndex  )->{ 'Value' };
	my $oem       = $$refToWorksheet->Cells( $row, $oemIndex  )->{ 'Value' };
	my $internal  = $$refToWorksheet->Cells( $row, $internalIndex  )->{ 'Value' };
	my $adapt     = $$refToWorksheet->Cells( $row, $adaptIndex  )->{ 'Value' };
	my $dll       = $$refToWorksheet->Cells( $row, $dllIndex  )->{ 'Value' };
	my $lib       = $$refToWorksheet->Cells( $row, $libIndex  )->{ 'Value' };
	my $deprecated = $$refToWorksheet->Cells( $row, $deprecatedIndex  )->{ 'Value' };
	my $obsoleted = $$refToWorksheet->Cells( $row, $obsoletedIndex  )->{ 'Value' };

	my $private;
	if($privateIndex > 0)
	{
	    $private   = $$refToWorksheet->Cells( $row, $privateIndex  )->{ 'Value' };
	}
	#------------------------------------------------------------
	# We only allow $globalMaxEmptyRowCount empty rows, before we stop
	# handling the specific sheet
	#------------------------------------------------------------
	if(! (defined($apiName) || defined($fileName)|| defined($dll) ||
	      defined($lib) || defined($obsoleted)))
	{
	    $emptyRowCount++;
	    if($emptyRowCount > $globalMaxEmptyRowCount)
	    {
		last;
	    }
	}
	else
	{
	    # Was not empty line, reset counter.
	    $emptyRowCount = 0;
	}

	#------------------------------------------------------------
	# Handle API row
	#------------------------------------------------------------
        $isAPIRow = 0;
        if(defined($apiName) and $apiName !~ /^\s*$/)
        {
	    $isAPIRow = 1;
	    $currApiRelInfoOK = 1;
	    $currApiName = $apiName;
	    # Reset the release values and set new values to them.
	    $apiSdkRel = -1;
	    $apiOemRel = -1;
	    $apiInternalRel = -1;
	    $apiPrivateRel = -1;
	    $apiDeprecatedRel = -1;
	    $apiObsoletedRel = -1;
	    setRelValue(\$apiSdkRel,$sdk);
	    setRelValue(\$apiOemRel,$oem);
	    setRelValue(\$apiInternalRel,$internal);
	    setRelValue(\$apiDeprecatedRel,$deprecated);
	    setRelValue(\$apiObsoletedRel,$obsoleted);

	    if($privateIndex > -1)
	    {
		setRelValue(\$apiPrivateRel,$private);
	    }

	    if($apiName =~ /^\s*Comment:/ || $purpose =~ /removed by nokia/i)
	    {
		next;
	    }

	    ################################################
            # Error checking
	    ################################################

	    #
	    # API description is needed
	    #
	    if(!defined($purpose) || $purpose =~ /^\s*$/)
	    {
		push(@$refToErrorList,getError($API_DESC_MISSING_ERROR,
					       $currApiName));
	
	    }

	    #
	    # Adaptation field has to be set for API
	    #
	    if($purpose !~ /is a collection api/i && defined($adapt))
	    {
		if( $adapt !~ /^\s*No\s*$/ && $adapt !~ /^\s*Yes\s*$/)
		{
		    push(@$refToErrorList,getError($API_ADAPTATION_ERROR,
						   $currApiName,"Value was: \"$adapt\""));
		}
	    }
	    #
	    # Check that release information is in correct format
	    #
	    my $currErrAmount = scalar(@$refToErrorList);
	    if(checkRel($apiSdkRel))
	    {
		push(@$refToErrorList,getError($API_REL_SYNTAX_ERROR,
					       $currApiName,
					       "SDK release value \"$apiSdkRel\""));
	    }
	    if(checkRel($apiOemRel))
	    {
		push(@$refToErrorList,getError($API_REL_SYNTAX_ERROR,
					       $currApiName,
					       "OEM release value \"$apiOemRel\""));
	    }   

	    if(checkRel($apiInternalRel))
	    {
		push(@$refToErrorList,getError($API_REL_SYNTAX_ERROR,
					       $currApiName,
					       "Internal release value \"$apiInternalRel\""));
	    }

	    if($privateIndex > -1 && checkRel($apiPrivateRel))
	    {
		push(@$refToErrorList,getError($API_REL_SYNTAX_ERROR,
					       $currApiName,
					       "Private release value \"$apiPrivateRel\""));
	    }

	    if(checkRel($apiDeprecatedRel))
	    {
		push(@$refToErrorList,getError($API_REL_SYNTAX_ERROR,
					       $currApiName,
					       "Deprecated release value \"$apiDeprecatedRel\""));
	    }
	    if(checkRel($apiObsoletedRel))
	    {
		push(@$refToErrorList,getError($API_REL_SYNTAX_ERROR,
					       $currApiName,
					       "Obsoleted release value \"$apiObsoletedRel\""));
	    }

	    #
	    # Check the release information for the api
	    #  (these are only checked, if there was no syntax errors in the release fields.
            #
	    if($currErrAmount == scalar(@$refToErrorList))
	    {
		# 
		# Check that there is at least one release for the API.
		# 
		if($apiSdkRel == -1 && $apiOemRel == -1 && 
		   $apiInternalRel == -1 && $apiPrivateRel == -1 && $purpose !~ /is a collection api/i)
		{
		    push(@$refToErrorList,getError($API_REL_MISSING_ERROR,
						   $currApiName));
		    $currApiRelInfoOK = 0;
		}
		elsif($apiSdkRel == -1 && $apiOemRel == -1 && 
		      $apiInternalRel == -1 && $apiPrivateRel == -1)
		{
		    if(checkApiRelRoadMap($refToErrorList,"S60",$currApiName,$apiSdkRel,$apiOemRel,
					$apiInternalRel,$apiPrivateRel,$apiDeprecatedRel,$apiObsoletedRel))
		    {
			$currApiRelInfoOK = 0;
		    }
		    if(checkApiRelRoadMap($refToErrorList,"S90",$currApiName,$apiSdkRel,$apiOemRel,
					$apiInternalRel,$apiPrivateRel,$apiDeprecatedRel,$apiObsoletedRel))
		    {
			$currApiRelInfoOK = 0;
		    }
		}
	    }
	    else
	    {
		$currApiRelInfoOK = 0;
	    }

	    #
	    # Check that there are no headers defined in the API row
	    #
	    if($fileName !~ /^\s*$/)
	    {
		push(@$refToErrorList,getError($API_HEADERS_IN_ROW_ERROR,
					       $currApiName));
	    }
	}


	#------------------------------------------------------------
	# Check the LIB and DLL fields.
	#------------------------------------------------------------
	if(defined($dll) && $dll !~ /^\s*$/ && $dll !~ /\s*\(*\s*ECOM\s*\)*\s*$/i && $dll !~ /^\s*tbd\s*$/i)
	{
	    if( $dll =~ /\.lib/i)
	    {
		push(@$refToErrorList,getError($DLL_SYNTAX_ERROR,
					       $currApiName,
					       "DLL column contains LIBs."));
	    }
	    elsif( $dll !~ /\.[dll|agt|nif|prt|csy|mdl]/i && $dll !~ /^\s*N\/A\s*$/i && $dll !~ /^\s*\-\s*$/i )
	    {
		push(@$refToErrorList,getError($DLL_SYNTAX_ERROR,
					       $currApiName,
					       "DLL column contains illegal value(s):", 
					       " \"$dll\""));
	    }
	}
	if(defined($lib) && $lib !~ /^\s*$/ && $lib !~ /\s*\(*\s*ECOM\s*\)*\s*$/i && $lib !~ /^\s*tbd\s*$/i)
	{
	    if( $lib =~ /\.dll/i)
	    {
		push(@$refToErrorList,getError($LIB_SYNTAX_ERROR,
					       $currApiName, 
					       "LIB column contains DLLs."));
	    }
	    elsif( $lib !~ /\.lib/i && $lib !~ /^\s*N\/A\s*$/i && $lib !~ /^\s*\-s*$/i )
	    {
		push(@$refToErrorList,getError($LIB_SYNTAX_ERROR,
					       $currApiName, 
					       "LIB column contains illegal values.",
					       "\"$lib\""));
	    }
	}

	#------------------------------------------------------------
	# Check file definition rows (only if the api row was ok) 
	#------------------------------------------------------------
	if($isAPIRow == 0  && defined($fileName) && $fileName !~ /^\s*$/)
	{
	    # Set the release values for header information
	    my $hdrSdkRel = -1;
	    my $hdrOemRel = -1;
	    my $hdrInternalRel = -1;
	    my $hdrPrivateRel = -1;
	    my $hdrDeprecatedRel = -1;
	    my $hdrObsoletedRel = -1;
	    setRelValue(\$hdrSdkRel,$sdk);
	    setRelValue(\$hdrOemRel,$oem);
	    setRelValue(\$hdrInternalRel,$internal);
	    setRelValue(\$hdrDeprecatedRel,$deprecated);
	    setRelValue(\$hdrObsoletedRel,$obsoleted);

	    if($privateIndex > -1)
	    {
		setRelValue(\$hdrPrivateRel,$private);
	    }

	    ################################################
	    # Error checking
	    ################################################
	    #
	    # Check that only allowed values are used
	    if( $fileName !~ /^\s*DIR:/i  && $fileName !~ /^\s*FILE:/i &&
		$fileName !~ /^\s*[a-zA-Z0-9\\\/_-]+\.[h|inl|rh|hrh|loc|pan]/i && 
		$fileName !~ /^\s*tbd\s*$/i)
	    {
		push(@$refToErrorList,
		     getError($FILE_COLUMN_SYNTAX_ERROR,
			      $currApiName,"\"$fileName\""));
	    }
	    # 
            # Check that there is only 1 file in the row.
	    #
	    elsif($fileName =~ /(\.[h|inl|rh|hrh|loc|pan])/ig)
	    {
		my @list;
		@list = ($fileName =~ /(\.[h|inl|rh|hrh|loc|pan])/ig);
		if(scalar(@list) > 1)
		{
		    push(@$refToErrorList,
			 getError($FILE_MORE_THAN_ONE_FILE_ERROR,
				  $currApiName, "\"$fileName\""));
		}
	    }

	    # 
	    # Check that the release information is correctly defined
	    # (only checked, if API row was OK from that point 
	    if($currApiRelInfoOK == 1)
	    {
		#
		# Check that release information is in correct format
		#
		my $currErrAmount = scalar(@$refToErrorList);
		if(checkRel($hdrSdkRel))
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_SYNTAX_ERROR,
				  "$currApiName :: $fileName",
				  "SDK release value \"$hdrSdkRel\""));
		}
		if(checkRel($hdrOemRel))
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_SYNTAX_ERROR,
				  "$currApiName :: $fileName", "OEM release value \"$hdrOemRel\""));
		}
		if(checkRel($hdrInternalRel))
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_SYNTAX_ERROR,
				  "$currApiName :: $fileName", "Internal release value \"$hdrInternalRel\""));
		}

		if($privateIndex > -1 && checkRel($hdrPrivateRel))
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_SYNTAX_ERROR,
				  "$currApiName :: $fileName", "Private release value \"$hdrPrivateRel\""));
		}

		if(checkRel($hdrDeprecatedRel))
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_SYNTAX_ERROR,
				  "$currApiName :: $fileName", "Deprecated release value \"$hdrDeprecatedRel\""));
		}
		if(checkRel($hdrObsoletedRel))
		{
		    push(@$refToErrorList,
			 getError($FILE_REL_SYNTAX_ERROR,
				  "$currApiName :: $fileName", "Obsoleted release value \"$hdrObsoletedRel\""));
		}		


		#
		# Check the release information for the file
		#  (these are only checked, if there was no syntax errors in the release fields.
		#
		if($currErrAmount == scalar(@$refToErrorList))
		{
		    checkFileRelRoadMap($refToErrorList,"S60",$currApiName,$fileName,$apiSdkRel,$apiOemRel,
					 $apiInternalRel,$apiPrivateRel,$apiDeprecatedRel,$apiObsoletedRel,
					 $hdrSdkRel,$hdrOemRel,$hdrInternalRel,$hdrPrivateRel,
					 $hdrDeprecatedRel,$hdrObsoletedRel);
		    checkFileRelRoadMap($refToErrorList,"S90",$currApiName,$fileName,$apiSdkRel,$apiOemRel,
					 $apiInternalRel,$apiPrivateRel,$apiDeprecatedRel,$apiObsoletedRel,
					 $hdrSdkRel,$hdrOemRel,$hdrInternalRel,$hdrPrivateRel,
					 $hdrDeprecatedRel,$hdrObsoletedRel);
		}
	    }

	}
    }
}


#################################
# Extracs from the given string S60 and S90 releases
# and puts then into the hash, if releases were found.
#################################
sub addToTypeHash
{
    my $refToHash = shift;
    my $str = shift;

    if($str=~ /S60\s+([0-9.]+)/i)
    {
	$$refToHash{"s60"} = $1;
    }
    if($str=~ /S90\s+([0-9.]+)/i)
    {
	$$refToHash{"s90"} = $1;
    }
}
###########################################################################
# Function that reads certain columns from the interface sheet to the given hash.
# The function is intended to be called for the c++ API definitions.
# For the cenrep/P&S APIs use the "readCrepInfo2"-function
#
# The format of the returned hash is:
#  {
#    <ordinal>  => 
#                  {
#                  "api_name" => <APIName>
#                  "api_type" => "1"    # 1 means c++, 2 means cenrep/P&S apis 
#                  "owner_entity" => <API owner entity>
#                  "files" => {<file1> => 1 ,<file2> => 1,...}
#                  "purpose"      => <purpose text>
#                  "sdk"          => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "partner"      => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "internal"     => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "private"      => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "adapt"      => "0 = no adaptation, 1 = adaptation"
#                  "specification" => <spec name(s)>
#                  "dll"       => {<dll1> => 1,<dll2> => 1,...}
#                  "lib"       => {<dll1> => 1,<dll2> => 1,...}
#                  "subsystem" => <subsystem name>
#                  "deprecated"   => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                          my $refToHash = shift;          }
#                  "removed"      => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "partners"         => <namesStr>
#                  "partner_contacts" => <namesStr>
#                  "base"      => "0 = not base service, 1 = is base service"
#                  "clients"          => <namesStr>
#                  "clients_org"      => <namesStr>
#                  "owner"        => <API owner name>
#                  "for_adapt_since"   => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "for_adapt_deprecated"   => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "for_adapt_removed"      => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "extended_sdk_since"   => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "extended_sdk_deprecated"   => { 
#                                    "s60" => <releaseNbr>
#                          my $refToHash = shift;          }
#                  "extended_sdmy $refToHash = shift;k_removed"      => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  }
# etc....
#  }
# Note the that refToHash is assumed to be an empty hash.
#
# Note 1: The S60/s90 keys are not defined, if there was no information
#         in the type for the API.
#
# Params: referenceToWorksheet,refToHash,defOwnerEntity,defOwner
#
# Return: -
###########################################################################
sub readInfo2     
{
    my $refToWorksheet = shift;
    my $refToHash = shift;
    my $defOwnerEntity = shift;
    my $defOwner = shift;


    # check that sheet really exists (it could be missing)
    if(not defined($refToWorksheet))
    {
	return;
    }
    
    # getNextOrdinal
    my $nextOrdinal = getNextOrdinal $refToHash;

    # ----------------------------------------------------------------  
    # look to header information
    # ----------------------------------------------------------------  
    # API Name
    # Header file(s)    (or "File(s)" in template version 1.6->)
    # Purpose	
    # Responsible
    # SDK
    # Oem               (or "Partner" in template version 1.6->)
    # Internal
    # Private   (migth be missing, as is the case in the symbian sheet)	
    # Adaptation
    # Specification
    # DLL
    # Import Lib
    # Other
    # Grouping           (or "Subsystem" in template version 1.6->)
    # Clients
    # Deprecated
    # Obsoleted          (or "Removed" in template version 1.6->)
    # Partner(s)         (Only from 1.6->)
    # Partner Contact(s) (Only from 1.6->)
    # Owner
    # Note
    # For Adaptation Since (only from 2.0->)
    # For Adaptation Deprecated (only from 2.0->)
    # For Adaptation Removed (only from 2.0->)
    # Extended SDK Since (only from 2.0->)
    # Extended SDK Depricated (only from 2.0->)
    # Extended SDK Removed (only from 2.0->)
    my $ownerNameEntityIndex = getIndexByColName $refToWorksheet,"Owner Entity",$globalMaxColCount;
    my $apiNameIndex = getIndexByColName $refToWorksheet,"API Name",$globalMaxColCount;
    my $fileNameIndex = getIndexByColName $refToWorksheet,"Header file(s)",$globalMaxColCount;
    if( !defined($fileNameIndex) || $fileNameIndex < 1)
    {
	$fileNameIndex = getIndexByColName $refToWorksheet,"File(s)",$globalMaxColCount;
    }
    my $purposeIndex = getIndexByColName $refToWorksheet,"Purpose",$globalMaxColCount;
    my $sdkIndex = getIndexByColName $refToWorksheet,"SDK",$globalMaxColCount;
    my $oemIndex = getIndexByColName $refToWorksheet,"Oem",$globalMaxColCount;
    if( !defined($oemIndex) || $oemIndex < 1)
    {
	$oemIndex = getIndexByColName $refToWorksheet,"Partner",$globalMaxColCount;
    }
    if( !defined($oemIndex) || $oemIndex < 1)
    {
	$oemIndex = getIndexByColName $refToWorksheet,"Domain",$globalMaxColCount;
    }
    my $internalIndex = getIndexByColName $refToWorksheet,"Internal",$globalMaxColCount;
    my $privateIndex =  getIndexByColName $refToWorksheet,"Private",$globalMaxColCount;
    my $adaptIndex = getIndexByColName $refToWorksheet,"Adaptation",$globalMaxColCount;
    my $specificationIndex = getIndexByColName $refToWorksheet,"Specification",$globalMaxColCount;
    my $dllIndex = getIndexByColName $refToWorksheet,"DLL",$globalMaxColCount;
    my $libIndex = getIndexByColName $refToWorksheet,"Import Lib",$globalMaxColCount;
    my $ssNameIndex = getIndexByColName $refToWorksheet,"Subsystem",$globalMaxColCount;
    if( !defined($ssNameIndex) || $ssNameIndex < 1)
    {
	$ssNameIndex = getIndexByColName $refToWorksheet,"Grouping",$globalMaxColCount;
    }
    my $deprecatedIndex =  getIndexByColName $refToWorksheet,"Deprecated",$globalMaxColCount;
    my $removedIndex =  getIndexByColName $refToWorksheet,"Obsoleted",$globalMaxColCount;
    if( !defined($removedIndex) || $removedIndex < 1)
    {
	$removedIndex = getIndexByColName $refToWorksheet,"Removed",$globalMaxColCount;
    }
    my $partnerIndex = getIndexByColName $refToWorksheet,"Partner(s)",$globalMaxColCount;
    my $partnerContactIndex = getIndexByColName $refToWorksheet,
                                              "Partner Contact(s)",$globalMaxColCount;
    my $baseServiceIndex = getIndexByColName $refToWorksheet,"Base Service",$globalMaxColCount;
    my $clientIndex = getIndexByColName $refToWorksheet,"Clients",$globalMaxColCount;
    my $clientOrgIndex = getIndexByColName $refToWorksheet,"Client Org",$globalMaxColCount;
    my $ownerNameIndex = getIndexByColName $refToWorksheet,"Owner",$globalMaxColCount;

    my $forAdaptationSinceIndex = getIndexByColName $refToWorksheet,"For Adaptation Since",$globalMaxColCount;
    my $forAdaptationDeprecatedIndex = getIndexByColName $refToWorksheet,"For Adaptation Deprecated",$globalMaxColCount;
    my $forAdaptationRemovedIndex = getIndexByColName $refToWorksheet,"For Adaptation Removed",$globalMaxColCount;
    my $extendedSDKSinceIndex = getIndexByColName $refToWorksheet,"Extended SDK Since",$globalMaxColCount;
    my $extendedSDKDeprecatedIndex = getIndexByColName $refToWorksheet,"Extended SDK Depricated",$globalMaxColCount;
    my $extendedSDKRemovedIndex = getIndexByColName $refToWorksheet,"Extended SDK Removed",$globalMaxColCount;

    my $row = 1;
    my $emptyRowCount = 0;

    ######################################################################
    # Loop row by row until we have found $globalMaxEmptyRowCount of rows
    # empty one after each other.
    ######################################################################
    my $currApiName;
    my $currApiOrdinal;
    for( $row = 2;; $row++ ) 
    {	         
	#------------------------------------------------------------
	# Read the needed rows from the row
	#------------------------------------------------------------
	my $ownerEntity =  $defOwnerEntity;
	if($ownerNameEntityIndex > 0)
	{
	    $ownerEntity = $$refToWorksheet->Cells( $row, $ownerNameEntityIndex  )->{ 'Value' };
	}
	my $apiName   = $$refToWorksheet->Cells( $row, $apiNameIndex  )->{ 'Value' };
        my $fileName  = $$refToWorksheet->Cells( $row, $fileNameIndex  )->{ 'Value' };
        my $purpose   = $$refToWorksheet->Cells( $row, $purposeIndex  )->{ 'Value' };
        my $sdk       = $$refToWorksheet->Cells( $row, $sdkIndex  )->{ 'Value' };
	my $oem       = $$refToWorksheet->Cells( $row, $oemIndex  )->{ 'Value' };
	my $internal  = $$refToWorksheet->Cells( $row, $internalIndex  )->{ 'Value' };
	my $private  = "";
	if($privateIndex > 0)
	{
	    $private = $$refToWorksheet->Cells( $row, $privateIndex  )->{ 'Value' };
	}
	my $adapt     = $$refToWorksheet->Cells( $row, $adaptIndex  )->{ 'Value' };
	my $specification = $$refToWorksheet->Cells( $row, $specificationIndex   )->{ 'Value' };
	my $dll       = $$refToWorksheet->Cells( $row, $dllIndex  )->{ 'Value' };
	my $lib       = $$refToWorksheet->Cells( $row, $libIndex  )->{ 'Value' };
	my $ssName    = $$refToWorksheet->Cells( $row, $ssNameIndex  )->{ 'Value' };
	my $deprecated = $$refToWorksheet->Cells( $row, $deprecatedIndex  )->{ 'Value' };
	my $removed    = $$refToWorksheet->Cells( $row, $removedIndex  )->{ 'Value' };
	my $partner = "";
	$partner = $$refToWorksheet->Cells( $row, $partnerIndex  )->{ 'Value' } if $partnerIndex > 0;
	my $partnerContact = "";
	$partnerContact = $$refToWorksheet->Cells( $row, $partnerContactIndex  )->{ 'Value' } if $partnerContactIndex > 0;
	my $baseService = "";
	$baseService = $$refToWorksheet->Cells( $row, $baseServiceIndex  )->{ 'Value' } if $baseServiceIndex > 0;
	my $client = "";
	$client = $$refToWorksheet->Cells( $row, $clientIndex  )->{ 'Value' } if $clientIndex > 0;
	my $clientOrg = "";
	$clientOrg = $$refToWorksheet->Cells( $row, $clientOrgIndex  )->{ 'Value' } if $clientOrgIndex > 0;
	my $ownerName    = $$refToWorksheet->Cells( $row, $ownerNameIndex  )->{ 'Value' };

	my $forAdaptationSince = "";
	my $forAdaptationDeprecated = "";
	my $forAdaptationRemoved = "";
	my $extendedSDKSince = "";
	my $extendedSDKDeprecated = "";
	my $extendedSDKRemoved = "";
	# All the 6 columsn came at the same time => if 1 is present => all are present.
	# We just check one, since this makes the code a little bit less 
	if( $forAdaptationSinceIndex > 0)
	{
	    $forAdaptationSince = $$refToWorksheet->Cells( $row, $forAdaptationSinceIndex  )->{ 'Value' }  ;
	    $forAdaptationDeprecated = $$refToWorksheet->Cells( $row, $forAdaptationDeprecatedIndex  )->{ 'Value' };
	    $forAdaptationRemoved = $$refToWorksheet->Cells( $row, $forAdaptationRemovedIndex  )->{ 'Value' };
	    $extendedSDKSince = $$refToWorksheet->Cells( $row, $extendedSDKSinceIndex  )->{ 'Value' };
	    $extendedSDKDeprecated = $$refToWorksheet->Cells( $row, $extendedSDKDeprecatedIndex  )->{ 'Value' };
	    $extendedSDKRemoved = $$refToWorksheet->Cells( $row, $extendedSDKRemovedIndex  )->{ 'Value' };
	}

	#------------------------------------------------------------
	# We only allow $globalMaxEmptyRowCount empty rows, before we stop
	# handling the specific sheet
	#------------------------------------------------------------
	if(! (defined($apiName) || defined($fileName)|| defined($dll) ||
	      defined($lib)))
	{
	    $emptyRowCount++;
	    if($emptyRowCount > $globalMaxEmptyRowCount)
	    {
		last;
	    }
	}
	else
	{
	    # Was not empty line, reset counter.
	    $emptyRowCount = 0;
	}

	#------------------------------------------------------------
	# Check, if is an comment line. If so, then ignore line
	#------------------------------------------------------------
        if(defined($apiName) and $apiName =~ /^\s*Comment:/)
        {
            next;
        }

	#------------------------------------------------------------
	# Check, if is an api line
	#------------------------------------------------------------
        if(defined($apiName) and $apiName !~ /^\s*$/)
        {
	    $currApiName = $apiName;
	    $currApiOrdinal = $nextOrdinal;
	    $nextOrdinal++;

            my %newHash;
            $$refToHash{$currApiOrdinal} =  \%newHash;

	    $newHash{"api_name"} = $currApiName;
	    $newHash{"api_type"} = 1;
            my %hash1;
            my %hash2;
            my %hash3;
            my %hash4;
            my %hash5;
            my %hash6;
            my %hash7;
            my %hash8;
            my %hash9;
            my %hash10;
            my %hash11;
            my %hash12;
            my %hash13;
            my %hash14;
            my %hash15;
            $newHash{"owner_entity"}     = $ownerEntity;
            $newHash{"files"} = \%hash1;
	    $newHash{"purpose"} = $purpose;
            $newHash{"sdk"} = \%hash4;
	    addToTypeHash(\%hash4,$sdk);
            $newHash{"partner"} = \%hash5;
	    addToTypeHash(\%hash5,$oem);
            $newHash{"internal"} = \%hash6;
	    addToTypeHash(\%hash6,$internal);
            $newHash{"private"} = \%hash7;
	    addToTypeHash(\%hash7,$private);
	    $newHash{"adapt"} = 0;
	    $newHash{"adapt"} = 1 if $adapt =~ /yes/i;
	    $newHash{"specification"} =  $specification;
            $newHash{"dll"} = \%hash2;
            $newHash{"lib"} = \%hash3;
            $newHash{"subsystem"} = $ssName;
            $newHash{"depracated"} = \%hash8;
	    addToTypeHash(\%hash8,$deprecated);
            $newHash{"removed"} = \%hash9;
	    addToTypeHash(\%hash9,$removed);
	    $newHash{"partners"} =  $partner;
	    $newHash{"partner_contacts"} =  $partnerContact;
	    $newHash{"base"} =  0;
	    $newHash{"base"} =  1 if $baseService =~/yes/i;
	    $newHash{"clients"} =  $client;
	    $newHash{"clients_org"} =  $clientOrg;
            $newHash{"owner"}     = $ownerName;
	    $newHash{"owner"}     = $defOwner if $ownerName =~ /^\s*$/;

	    # For adaptation
            $newHash{"for_adapt_since"} = \%hash10;
	    addToTypeHash(\%hash10,$forAdaptationSince);
            $newHash{"for_adapt_deprecated"} = \%hash11;
	    addToTypeHash(\%hash11,$forAdaptationDeprecated);
            $newHash{"for_adapt_removed"} = \%hash12;
	    addToTypeHash(\%hash12,$forAdaptationRemoved);

	    # extended sdk
            $newHash{"extended_sdk_since"} = \%hash13;
	    addToTypeHash(\%hash13,$extendedSDKSince);
            $newHash{"extended_sdk_deprecated"} = \%hash14;
	    addToTypeHash(\%hash14,$extendedSDKDeprecated);
            $newHash{"extended_sdk_removed"} = \%hash15;
	    addToTypeHash(\%hash15,$extendedSDKRemoved);
	}
	#------------------------------------------------------------
	# Handle the files column. (ignore some special cases)
	#------------------------------------------------------------
        if(defined($fileName) && $fileName !~ /^\s*DIR:/i  && 
           $fileName !~ /^\s*FILE:/i && $fileName !~ /^\s*$/ && 
	   $fileName !~ /^\s*tbd\s*$/i && $fileName !~ /^\s*n\/?a\s*$/i)
        {
           my $refToApiHash = $$refToHash{$currApiOrdinal};
           my $refToFilesHash = $$refToApiHash{"files"};
           $$refToFilesHash{$fileName} = 1;
        }
	#------------------------------------------------------------
	# Handle the dlls column. (ignore some special cases)
	#------------------------------------------------------------
        if(defined($dll) && $dll !~ /^\s*$/ && $dll !~ /^\s*ECOM\s*$/i && 
	   $dll !~ /^\s*tbd\s*$/i && $dll !~ /^\s*n\/?a\s*$/i)
        {
           my $refToApiHash = $$refToHash{$currApiOrdinal};
           my $refToDllHash = $$refToApiHash{"dll"};

           addToHash($refToDllHash,$dll);
        }
	#------------------------------------------------------------
	# Handle the lib column. (ignore some special cases)
	#------------------------------------------------------------
        if(defined($lib) && $lib !~ /^\s*$/ && $lib !~ /^\s*ECOM\s*$/i && 
	   $lib !~ /^\s*tbd\s*$/i && $lib !~ /^\s*n\/?a\s*$/i)
        {
           my $refToApiHash = $$refToHash{$currApiOrdinal};
           my $refToLibHash = $$refToApiHash{"lib"};

           addToHash($refToLibHash,$lib);
        }
    }
}


###########################################################################
# Function that reads certain columns from the interface sheet to the given hash.
# The function is intended to be called for the cenrep/P&S API definitions.
# For the c++ APIs use the "readInfo2"-function
#
# The format of the returned hash is:
#  {
#  <ordinal>  => {
#  
#                  "api_name" => <apiName>    # 1 means c++, 2 means cenrep/P&S apis 
#                  "api_type" => "2"    # 1 means c++, 2 means cenrep/P&S apis 
#                  "owner_entity" => <API owner entity>
#                  "uid" => <uid>
#                  "type" => "cenrep/ps" 
#                  "keys" => {<key1> => 1 ,<key12> => 1,...}
#                  "purpose"      => <purpose text>
#                  "sdk"          => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "partner"      => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "internal"     => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "private"      => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "relFeatuers" => <relatedFeature(s)>
#                  "subsystem" => <subsystem name>
#                  "deprecated"   => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "removed"      => { 
#                                    "s60" => <releaseNbr>
#                                    "s90" => <releaseNbr>  # Note 1:
#                                    }
#                  "partners"         => <namesStr>
#                  "partner_contacts" => <namesStr>
#                  "clients"          => <namesStr>
#                  "clients_org"      => <namesStr>
#                  "owner"        => <API owner name>
#                  "for_adapt_since"   => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "for_adapt_deprecated"   => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "for_adapt_removed"      => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "extended_sdk_since"   => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "extended_sdk_deprecated"   => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  "extended_sdk_removed"      => { 
#                                    "s60" => <releaseNbr>
#                                    }
#                  }
# etc....
#  }
# Note the that refToHash is assumed to be an empty hash.
#
# Note 1: The S60/s90 keys are not defined, if there was no information
#         in the type for the API.
#
# Params: referenceToWorksheet,refToHash,defOwnerEntity,defOwner
#
# Return: -
###########################################################################
sub readCRepInfo2
{
    my $refToWorksheet = shift;
    my $refToHash = shift;
    my $defOwnerEntity = shift;
    my $defOwner = shift;

    # check that sheet really exists (it could be missing)
    if(not defined($refToWorksheet))
    {
	return;
    }
    # ----------------------------------------------------------------  
    # getNextOrdinal
    # ----------------------------------------------------------------  
    my $nextOrdinal = getNextOrdinal $refToHash;


    # ----------------------------------------------------------------  
    # look to header information
    # ----------------------------------------------------------------  
    # API Name
    # Uid Name
    # Key Name
    # Purpose	
    # Type
    # SDK
    # Domain               
    # Internal
    # Private
    # Subsystem
    # Clients
    # Client Org
    # Deprecated
    # Removed
    # Related Features
    # Partner(s)         (Only from 1.6->)
    # Partner Contact(s) (Only from 1.6->)
    # Owner
    # Note
    # For Adaptation Since (only from 2.0->)
    # For Adaptation Deprecated (only from 2.0->)
    # For Adaptation Removed (only from 2.0->)
    # Extended SDK Since (only from 2.0->)
    # Extended SDK Depricated (only from 2.0->)
    # Extended SDK Removed (only from 2.0->)

    my $ownerNameEntityIndex = getIndexByColName $refToWorksheet,"Owner Entity",$globalMaxColCount;
    my $apiNameIndex = getIndexByColName $refToWorksheet,"API Name",$globalMaxColCount;
    my $uidNameIndex = getIndexByColName $refToWorksheet,"Uid Name",$globalMaxColCount;
    my $keyNameIndex = getIndexByColName $refToWorksheet,"Key Name",$globalMaxColCount;
    my $keyTypeIndex = getIndexByColName $refToWorksheet,"Type",$globalMaxColCount;
    my $purposeIndex = getIndexByColName $refToWorksheet,"Purpose",$globalMaxColCount;
    my $sdkIndex = getIndexByColName $refToWorksheet,"SDK",$globalMaxColCount;
    my $oemIndex = getIndexByColName $refToWorksheet,"Domain",$globalMaxColCount;
    if( !defined($oemIndex) || $oemIndex < 1)
    {
	$oemIndex = getIndexByColName $refToWorksheet,"Partner",$globalMaxColCount;
    }
    my $internalIndex = getIndexByColName $refToWorksheet,"Internal",$globalMaxColCount;
    my $privateIndex =  getIndexByColName $refToWorksheet,"Private",$globalMaxColCount;
    my $ssNameIndex = getIndexByColName $refToWorksheet,"Subsystem",$globalMaxColCount;
    if( !defined($ssNameIndex) || $ssNameIndex < 1)
    {
	$ssNameIndex = getIndexByColName $refToWorksheet,"Subsystem full",$globalMaxColCount;
    }
    my $deprecatedIndex =  getIndexByColName $refToWorksheet,"Deprecated",$globalMaxColCount;
    my $removedIndex =  getIndexByColName $refToWorksheet,"Removed",$globalMaxColCount;
    my $partnerIndex = getIndexByColName $refToWorksheet,"Partner(s)",$globalMaxColCount;
    my $partnerContactIndex = getIndexByColName $refToWorksheet,
                                              "Partner Contact(s)",$globalMaxColCount;
    my $clientIndex = getIndexByColName $refToWorksheet,"Clients",$globalMaxColCount;
    my $clientOrgIndex = getIndexByColName $refToWorksheet,"Client Org",$globalMaxColCount;
    my $ownerNameIndex = getIndexByColName $refToWorksheet,"Owner",$globalMaxColCount;

    my $forAdaptationSinceIndex = getIndexByColName $refToWorksheet,"For Adaptation Since",$globalMaxColCount;
    my $forAdaptationDeprecatedIndex = getIndexByColName $refToWorksheet,"For Adaptation Deprecated",$globalMaxColCount;
    my $forAdaptationRemovedIndex = getIndexByColName $refToWorksheet,"For Adaptation Removed",$globalMaxColCount;
    my $extendedSDKSinceIndex = getIndexByColName $refToWorksheet,"Extended SDK Since",$globalMaxColCount;
    my $extendedSDKDeprecatedIndex = getIndexByColName $refToWorksheet,"Extended SDK Depricated",$globalMaxColCount;
    my $extendedSDKRemovedIndex = getIndexByColName $refToWorksheet,"Extended SDK Removed",$globalMaxColCount;

    my $row = 1;
    my $emptyRowCount = 0;

    ######################################################################
    # Loop row by row until we have found $globalMaxEmptyRowCount of rows
    # empty one after each other.
    ######################################################################
    my $currApiName;
    my $currApiOrdinal;
    for( $row = 2;; $row++ ) 
    {	         
	#------------------------------------------------------------
	# Read the needed rows from the row
	#------------------------------------------------------------
	my $ownerEntity =  $defOwnerEntity;
	if($ownerNameEntityIndex > 0)
	{
	    $ownerEntity = $$refToWorksheet->Cells( $row, $ownerNameEntityIndex  )->{ 'Value' };
	}
	my $apiName   = $$refToWorksheet->Cells( $row, $apiNameIndex  )->{ 'Value' };
	my $uidName   = $$refToWorksheet->Cells( $row, $uidNameIndex  )->{ 'Value' };
        my $keyName  = $$refToWorksheet->Cells( $row, $keyNameIndex  )->{ 'Value' };
        my $keyType  = $$refToWorksheet->Cells( $row, $keyTypeIndex  )->{ 'Value' };
        my $purpose   = $$refToWorksheet->Cells( $row, $purposeIndex  )->{ 'Value' };
        my $sdk       = $$refToWorksheet->Cells( $row, $sdkIndex  )->{ 'Value' };
	my $oem       = $$refToWorksheet->Cells( $row, $oemIndex  )->{ 'Value' };
	my $internal  = $$refToWorksheet->Cells( $row, $internalIndex  )->{ 'Value' };
	my $private = $$refToWorksheet->Cells( $row, $privateIndex  )->{ 'Value' };
	my $ssName    = $$refToWorksheet->Cells( $row, $ssNameIndex  )->{ 'Value' };
	my $deprecated = $$refToWorksheet->Cells( $row, $deprecatedIndex  )->{ 'Value' };
	my $removed    = $$refToWorksheet->Cells( $row, $removedIndex  )->{ 'Value' };
	my $partner = "";
	$partner = $$refToWorksheet->Cells( $row, $partnerIndex  )->{ 'Value' } if $partnerIndex > 0;
	my $partnerContact = "";
	$partnerContact = $$refToWorksheet->Cells( $row, $partnerContactIndex  )->{ 'Value' } if $partnerContactIndex > 0;
	my $client = "";
	$client = $$refToWorksheet->Cells( $row, $clientIndex  )->{ 'Value' } if $clientIndex > 0;
	my $clientOrg = "";
	$clientOrg = $$refToWorksheet->Cells( $row, $clientOrgIndex  )->{ 'Value' } if $clientOrgIndex > 0;
	my $ownerName    = $$refToWorksheet->Cells( $row, $ownerNameIndex  )->{ 'Value' };

	my $forAdaptationSince = "";
	my $forAdaptationDeprecated = "";
	my $forAdaptationRemoved = "";
	my $extendedSDKSince = "";
	my $extendedSDKDeprecated = "";
	my $extendedSDKRemoved = "";
	# All the 6 columsn came at the same time => if 1 is present => all are present.
	# We just check one, since this makes the code a little bit less 
	if( $forAdaptationSinceIndex > 0)
	{
	    $forAdaptationSince = $$refToWorksheet->Cells( $row, $forAdaptationSinceIndex  )->{ 'Value' }  ;
	    $forAdaptationDeprecated = $$refToWorksheet->Cells( $row, $forAdaptationDeprecatedIndex  )->{ 'Value' };
	    $forAdaptationRemoved = $$refToWorksheet->Cells( $row, $forAdaptationRemovedIndex  )->{ 'Value' };
	    $extendedSDKSince = $$refToWorksheet->Cells( $row, $extendedSDKSinceIndex  )->{ 'Value' };
	    $extendedSDKDeprecated = $$refToWorksheet->Cells( $row, $extendedSDKDeprecatedIndex  )->{ 'Value' };
	    $extendedSDKRemoved = $$refToWorksheet->Cells( $row, $extendedSDKRemovedIndex  )->{ 'Value' };
	}

	#------------------------------------------------------------
	# We only allow $globalMaxEmptyRowCount empty rows, before we stop
	# handling the specific sheet
	#------------------------------------------------------------
	if(! (defined($apiName) || defined($keyName)))
	{
	    $emptyRowCount++;
	    if($emptyRowCount > $globalMaxEmptyRowCount)
	    {
		last;
	    }
	}
	else
	{
	    # Was not empty line, reset counter.
	    $emptyRowCount = 0;
	}

	#------------------------------------------------------------
	# Check, if is an comment line. If so, then ignore line
	#------------------------------------------------------------
        if(defined($apiName) and $apiName =~ /^\s*Comment:/)
        {
            next;
        }

	#------------------------------------------------------------
	# Check, if is an api line
	#------------------------------------------------------------
        if(defined($apiName) and $apiName !~ /^\s*$/)
        {
	    $currApiName = $apiName;
	    $currApiOrdinal = $nextOrdinal;
	    $nextOrdinal++;

            my %newHash;
            $$refToHash{$currApiOrdinal} =  \%newHash;

	    $newHash{"api_name"} = $currApiName;
	    $newHash{"api_type"} = "2";
            my %hash1;
            my %hash2;
            my %hash3;
            my %hash4;
            my %hash5;
            my %hash6;
            my %hash7;
            my %hash8;
            my %hash9;
            my %hash10;
            my %hash11;
            my %hash12;
            my %hash13;
            my %hash14;
            my %hash15;
            $newHash{"owner_entity"}     = $ownerEntity;
            $newHash{"uid"} = $uidName;
            $newHash{"type"} = "ps";
            $newHash{"type"} = "cenrep" if $keyType =~ /centrep/i;
            $newHash{"keys"} = \%hash1;
	    $newHash{"purpose"} = $purpose;
            $newHash{"sdk"} = \%hash4;
	    addToTypeHash(\%hash4,$sdk);
            $newHash{"partner"} = \%hash5;
	    addToTypeHash(\%hash5,$oem);
            $newHash{"internal"} = \%hash6;
	    addToTypeHash(\%hash6,$internal);
            $newHash{"private"} = \%hash7;
	    addToTypeHash(\%hash7,$private);
            $newHash{"subsystem"} = $ssName;
            $newHash{"depracated"} = \%hash8;
	    addToTypeHash(\%hash8,$deprecated);
            $newHash{"removed"} = \%hash9;
	    addToTypeHash(\%hash9,$removed);
	    $newHash{"partners"} =  $partner;
	    $newHash{"partner_contacts"} =  $partnerContact;
	    $newHash{"clients"} =  $client;
	    $newHash{"clients_org"} =  $clientOrg;
            $newHash{"owner"}     = $ownerName;
	    $newHash{"owner"}     = $defOwner if $ownerName =~ /^\s*$/;

	    # For adaptation
            $newHash{"for_adapt_since"} = \%hash10;
	    addToTypeHash(\%hash10,$forAdaptationSince);
            $newHash{"for_adapt_deprecated"} = \%hash11;
	    addToTypeHash(\%hash11,$forAdaptationDeprecated);
            $newHash{"for_adapt_removed"} = \%hash12;
	    addToTypeHash(\%hash12,$forAdaptationRemoved);

	    # extended sdk
            $newHash{"extended_sdk_since"} = \%hash13;
	    addToTypeHash(\%hash13,$extendedSDKSince);
            $newHash{"extended_sdk_deprecated"} = \%hash14;
	    addToTypeHash(\%hash14,$extendedSDKDeprecated);
            $newHash{"extended_sdk_removed"} = \%hash15;
	    addToTypeHash(\%hash15,$extendedSDKRemoved);
	}
	#------------------------------------------------------------
	# Handle the files column. (ignore some special cases)
	#------------------------------------------------------------
        if(defined($keyName) && $keyName !~ /^\s*$/ && 
	   $keyName !~ /^\s*tbd\s*$/i && $keyName !~ /^\s*n\/?a\s*$/i)
        {
           my $refToApiHash = $$refToHash{$currApiOrdinal};
           my $refToKeysHash = $$refToApiHash{"keys"};
           $$refToKeysHash{$keyName} = 1;
        }
    }
}

###########################################################################
# Function assumes that black list sheet as been given. All files that
# belong to the given platform and releases black list are put into the hash
# format of the hash:
#
# {
#  file1 => 1,
#  file2 => 1,
#  etc...
# }
sub readBlackList
{
    my $refToWorksheet = shift;
    my $plat = shift;
    my $rel = shift;
    my $refToHash = shift;

    # check that sheet really exists (it could be missing)
    if(not defined($refToWorksheet))
    {
	return;
    }

    # ----------------------------------------------------------------  
    # look to header information
    # ----------------------------------------------------------------  
    # file
    # Since
    my $fileNameIndex = getIndexByColName $refToWorksheet,"File/Dir",$globalMaxColCount;
    my $sinceIndex = getIndexByColName $refToWorksheet,"Since",$globalMaxColCount;
    my $removedIndex = getIndexByColName $refToWorksheet,"Removed",$globalMaxColCount;

    my $row = 1;
    my $emptyRowCount = 0;

    ######################################################################
    # Loop row by row until we have found $globalMaxEmptyRowCount of rows
    # empty one after each other.
    ######################################################################
    for( $row = 2;; $row++ ) 
    {	         
	#------------------------------------------------------------
	# Read the needed rows from the row
	#------------------------------------------------------------
        my $fileName    = $$refToWorksheet->Cells( $row, $fileNameIndex  )->{ 'Value' };
        my $since       = $$refToWorksheet->Cells( $row, $sinceIndex  )->{ 'Value' };

        my $removed     = "";
	if($removedIndex >= 0)
	{
	    $removed    = $$refToWorksheet->Cells( $row, $removedIndex  )->{ 'Value' };
	}

	#------------------------------------------------------------
	# We only allow $globalMaxEmptyRowCount empty rows, before we stop
	# handling the specific sheet
	#------------------------------------------------------------
	if(! (defined($fileName)|| defined($since)))
	{
	    $emptyRowCount++;
	    if($emptyRowCount > $globalMaxEmptyRowCount)
	    {
		last;
	    }
	}
	else
	{
	    # Was not empty line, reset counter.
	    $emptyRowCount = 0;
	}

	#------------------------------------------------------------
	# Check, if line is wanted
	#------------------------------------------------------------
	if(isRowWanted($plat,$rel,1,$since,$removed) == 1)
	{
	    addToHash $refToHash,$fileName;
	}
    }    
}
1;

