#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors: Bakkiaraj M
#
# Author: Bakkiaraj M
#Mail ID: bakkiaraj.m@nokia.com

# Description:
#sdk_apis_creator.pl tool will create XML file which will be used by SDK build tools to collect the APIs from platform
#
# Input is list of .h files list (Usually we will receive it from Nokia API Management team) and platform builds
# Output is apis_symbian.xml
# 

#This tool is mainly used to collect SDK (winscw) .h to .lib files.  
 
use XML::DOM;
use strict;
use Getopt::Long;
use File::Find;
use File::Basename;
use Storable;

#Global variables
$|=1;
my $out_dir;
my $sdk_h_list;
my $pf_yrwk;
my $help;
my $epoc_root;
my $epoc_root_reg;


my $lib_collect_dir;
my @lib_collect_dirs;

my $NM_EXE;

#This data structure holds class as key and holding lib filenames as value in the form of annoymous hash
#Key= Class name, Return = list of implemented libs
my %class_to_lib;

#Ex:
# CDscItem = {dscstore.lib=>,
#	          second.lib=>,
#}

#This data structure holds header file as key and defined classes as values in the form of annoymous hash
#Key = header file , Return = list of classes
my %h_to_class;
#Ex:
# a.h = {A=>1,
#	          B=>1,
#}

#This data structure holds header file name as key and list of libs for header as value in the form of annonymous hash 
my %libs_for_h;
#Ex:
#a.h = {a.lib=>1,
#       b.lib=>1,	
#}

my $i=0;

# Get the user inputs
sub Usage
{
	print "\n sdk_apis_creator.pl tool will create XML file which will be used by SDK build tools to collect the APIs from platform";
	print "\n Need to check in sym_apis.xml to Series60SDK_dev/SDKComponents/CPP_API in SDK Synergy for build tools to pick up the Symbian APIs";
	print "\n  --sdk_h_list = <header list in the form of txt>";
	print "\n  --out_dir= <DIR path>";
	print "\n  --pf_yrwk = MCL Platform year and week";
	print "\n  --epoc_root = <epoc mapped dir>/ platform dir , where i can find epoc32";
	print "\n  --help = print usage help";
	print "\n\n  c:\> perl sdk_apis_creator.pl --sdk_h_list=c:/sdk_symbian_headers.txt --out_dir=c:/ --pf_yrwk=200950 --epoc_root=S:";
	print "\n\n Author : Bakkiaraj M , bakkiaraj.m\@nokia.com";
}

sub reap_class_from_lib
{

	return 0 if ($_!~m/\.lib$/i);
	#Skip GCC part libs Ex: _34xx_sdp_camerasc{000a0000}.lib
	return 0 if ($_ =~m/\{.*\}/i);
		
	#With the output of NM, find all the class names from the lib
	my $cmd;
	my $cmd_out;
		
	$cmd="$NM_EXE  --demangle=gnu-v3 -g $File::Find::name   2>NUL";
	#print "\n Run NM: $cmd";
	eval
	{
		open (NM_OUT, "$cmd |");
	};
	if ($@)
	{
		print "\n ERROR:$File::Find::name, $@";
	}
	$i++;
	print "\n $i. Process Lib: $File::Find::name";
	while($cmd_out=<NM_OUT>)
	{
		#print $cmd_out;
		chomp($cmd_out);
		#Get the class name
		#Ex: 00000188 T COomMonitorPlugin::~COomMonitorPlugin()
		# COomMonitorPlugin is a class (or even a struct)
		if($cmd_out=~m/^\d+\sT\s(\w+)::/i)
		{
			if (! exists $class_to_lib{$1})
			{
				print "\n \t \t CLASS: $1";
				$class_to_lib{$1}->{"$File::Find::name"}=1;
			}
			next;
		}
		#Get the class name
		#Ex: 00000000 T AuthServer::CIdAndString::NewLC(unsigned long, TDesC16 const&)
		# CIdAndString is a class name 
		if($cmd_out=~m/^\d+\sT\s.*?::(\w+?)::/i)
		{
			if (! exists $class_to_lib{$1})
			{
				print "\n \t \t CLASS: $1";
				$class_to_lib{$1}->{"$File::Find::name"}=1;
			}
			next;
		}
		
		#Get the class name from vitual function decl.
		#Ex: 000002cc T non-virtual thunk to CHXSymbianAudioSession::Write(IHXBuffer const*)
		# CHXSymbianAudioSession is a class name 
		if($cmd_out=~m/^\d+\sT\snon-virtual\sthunk\sto\s(\w+)::/i)
		{
			if (! exists $class_to_lib{$1})
			{
				print "\n \t \t CLASS: $1";
				$class_to_lib{$1}->{"$File::Find::name"}=1;
			}
			next;
		}

		#Get the Orphan function name , A function with out associated to class.
		#Ex: 00000000 T CreateOOMWatcherThreadL()
		#CreateOOMWatcherThreadL is a orphan
		if($cmd_out=~m/^\d+\sT\s(\w+)\(.*\)$/i)
		{
			if (! exists $class_to_lib{$1})
			{
				print "\n \t \t Orphan Fun: $1";
				$class_to_lib{$1}->{"$File::Find::name"}=1;
			}
			next;
		}
		
	}
	close (NM_OUT);
}

sub reap_class_from_h
{
	
	my $h_file=shift @_;
	
	my $cmd_out;
	my $full_header_file;
	
	my $inside_class=0; #0 is no, 1 is yes
		
	my $class_curl_count=0;
		
	open (HF_OUT, "< $h_file") or print "\n ERROR: Can not open $h_file for reading, $!";
	$i++;
	print "\n $i. Process Header: $h_file";
	
	$full_header_file=do { local $/; <HF_OUT> };
	
	#strip c comments
	$full_header_file=~s#/\*[^*]*\*+([^/*][^*]*\*+)*/|("(\\.|[^"\\])*"|'(\\.|[^'\\])*'|.[^/"'\\]*)#defined $2 ? $2 : ""#gse;
	#strip c++ comments
	$full_header_file=~s#/\*[^*]*\*+([^/*][^*]*\*+)*/|//([^\\]|[^\n][\n]?)*?\n|("(\\.|[^"\\])*"|'(\\.|[^'\\])*'|.[^/"'\\]*)#defined $3 ? $3 : ""#gse;
	
	foreach $cmd_out( split (/\n/,$full_header_file) )	
	{
		#ignore include
		next if ($cmd_out=~m/^\s*#include/);
		next if ($cmd_out=~m/^\s*#ifdef/);
		next if ($cmd_out=~m/^\s*#endif/);
		next if ($cmd_out=~m/^\s*#ifndef/);		
		next if ($cmd_out=~m/^\s*#define/);
		
		#Get the class name from macro class def
		#NONSHARABLE_CLASS (CDscItem) : public CBase
		#CDscItem is class name
		if($cmd_out=~m/CLASS\s*\(\s*(\w+)\s*\)/i)
		{
			if(! exists $h_to_class{$h_file}->{"$1"})
			{
				print "\n \t \t CLASS: $1";
				$h_to_class{$h_file}->{"$1"}=1;
			}
			$inside_class=1;
		}
		
		#Get the class name from class def
		#Ex:class SysUtil
		#SysUtil is class name
		if($cmd_out=~m/class\s*(\w+)\s*$/i)
		{
			if(! exists $h_to_class{$h_file}->{"$1"})
			{
				print "\n \t \t CLASS: $1";
				$h_to_class{$h_file}->{"$1"}=1;
			}
			$inside_class=1;
		}
		#Get the class name from class def
		#Ex:class SysUtil : 
		# public abc, public c
		#SysUtil is class name
		if($cmd_out=~m/class\s*(\w+)\s*:\s*$/i)
		{
			if(! exists $h_to_class{$h_file}->{"$1"})
			{
				print "\n \t \t CLASS: $1";
				$h_to_class{$h_file}->{"$1"}=1;
			}
			$inside_class=1;
		}
		#Get the class name from class def
		#Ex:class SysUtil: public CBase
		#SysUtil is class name
		if($cmd_out=~m/class\s*(\w+)\s*:\s*\w+\s*\w+/i)
		{
			if(! exists $h_to_class{$h_file}->{"$1"})
			{
				print "\n \t \t CLASS: $1";
				$h_to_class{$h_file}->{"$1"}=1;
			}
			$inside_class=1;
		}

		#TODO: Need some changes to tweak the class boundary
		if($cmd_out=~m/\s*\{\s*/ && $inside_class)
		{
			$class_curl_count++;
		}
		if($cmd_out=~m/\s*\}\s*\;/ && $inside_class)
		{
			$class_curl_count--;
			if($class_curl_count == 0)
			{
				$inside_class=0;
			}
		}

	   	#Get the Orphan function name , A function with out associated to class
		#This function will be outside of class
		#Ex: [MACRO]  [static] <return-type> <function-name> ([arguments]);
		#Ex: IMPORT_C void CreateOOMWatcherThreadL();
		#CreateOOMWatcherThreadL is a function name
	

		if($cmd_out=~m/\s*IMPORT_C(.*)\s+(\w+)\s*\((.*)\)\s*;/i && !$inside_class)
		{
			if(! exists $h_to_class{$h_file}->{"$2"})
			{
				print "\n \t \t Orphan Fun: $2";
				$h_to_class{$h_file}->{"$2"}=1;
			}
		}
		
	}

	#Now Look up header to lib association
	
	#Get the class for headers
	foreach my $class (keys %{$h_to_class{$h_file}})
	{
		#print "\n \t \tCLASS: $class";
		#Look libs in class_to_lib and store in libs_for_h to get unique libs
		foreach my $lib_fn(keys %{$class_to_lib{"$class"}})
		{
			#print "\n $lib_fn";
			$libs_for_h{$h_file}->{$lib_fn}=1;
		}
	}
	
	close (HF_OUT);
}

sub write_sym_apis_xml
{
	my $parser=new XML::DOM::Parser;
	my $cmd;
    my $h_line;
	my $h_file;
    my $h_src_path;
    
    my $sym_header_name;
	#Variables for sym_apis.xml
	my $out_xml_doc;
	my $api_dataset_ele;
	my $out_xml_decl;
	my $full_apis_xml;
	my $doc;
	
	my $api_ele;
	my $name_ele;
	my $libs_ele;
	my $lib_ele;
	my $release_ele;
	my $sources_ele;
	my $file_ele;
	my $path_ele;
	my $export_ele;	
	
	my $export_path;
    my $sym_apis_xml='sym_apis.xml';
	my @cmd_out;
	my $epoc_root_reg=quotemeta($epoc_root);
	
	chdir $out_dir or die ("\n ERROR: Can not change directory to $out_dir, $!");
	unlink ($out_dir.'/'.$sym_apis_xml) if (-e $out_dir.'/'.$sym_apis_xml);
			
	$out_xml_doc=new XML::DOM::Document;
    $out_xml_decl=$out_xml_doc->createXMLDecl('1.0');
	$api_dataset_ele=$out_xml_doc->createElement('api_dataset');
    $api_dataset_ele->addText("MCL_$pf_yrwk"."_S60.52");
    
    chdir $epoc_root or die ("\n ERROR: Can not change directory to $epoc_root, $!");
	open (H_LIST,"< $sdk_h_list") or die "\n ERROR: Can not open $sdk_h_list";

	while($h_line=<H_LIST>)
	{		
		chomp($h_line);
		#Skip comments
		next if ($h_line=~m/^\s*#/);
		undef $export_path;
		#$h_line may have export path too
		if($h_line=~m/(.*)::(.*)/)
		{
			$h_file=$epoc_root."\\".$1;
			$export_path=$2;
		}
		else
		{
			$h_file=$epoc_root."\\".$h_line;
		}
		

		#Skip if the header is not there in source tree
		if (! -s $h_file)
		{
			print "\n ERROR: $h_file is MISSING";
			next;
		}
		
		$i++;				
		print "\n $i. [XML]: $h_file ";

        #Write name of the API				
		$sym_header_name=basename($h_file);
		$api_ele=$out_xml_doc->createElement('api');
		$api_ele->setAttribute('id',"dummy_for_sym_api");
		
		$name_ele=$out_xml_doc->createElement('name');
		$name_ele->addText("$sym_header_name");
		$api_ele->appendChild($name_ele);	
		
		#Write libs info
		$libs_ele= $out_xml_doc->createElement('libs');
		foreach my $sym_lib_file (keys %{$libs_for_h{$h_file}})
		{
				print "\n \t \t Lib:  $sym_lib_file"; 
				$lib_ele=$out_xml_doc->createElement('lib');
				$lib_ele->setAttribute('name',basename("$sym_lib_file"));
				
				$libs_ele->appendChild($lib_ele);
		}
		
		$api_ele->appendChild($libs_ele);
		
		#Write release 
		$release_ele= $out_xml_doc->createElement('release');
		$release_ele->setAttribute('category','public');
		$api_ele->appendChild($release_ele);
		
		#Write sources, path, export info for header
		$path_ele= $out_xml_doc->createElement('path');
		
		#$h_line may have export path too
		if($h_line=~m/(.*)::(.*)/)
		{
			$h_src_path=$1;
		}
		else
		{
			$h_src_path=$h_line;
		}
		$path_ele->addText("$h_src_path");
		
		if (!defined $export_path)
		{
			#Find $sym_header_name in platform epoc32\include 
			$cmd="dir /S /B  $epoc_root\\epoc32\\include\\$sym_header_name";
			@cmd_out=`$cmd`;
			$cmd_out[0]=~s/$epoc_root_reg//;
		    if (defined  $cmd_out[0])
		    {
	        	$export_path=$cmd_out[0];
	        	chomp($export_path);
		    }
		    else
		    {
		    	#Default export is epoc32\\include
		    	$export_path="\\epoc32\\include\\$sym_header_name";
		    }
		    print "\n \t \t Found Export Path: $export_path";	
		}
        else
        {
        	print "\n \t \t Defined Export Path: $export_path";	
        }
		$export_ele=$out_xml_doc->createElement('export');
		$export_ele->addText($export_path);
		
		$file_ele= $out_xml_doc->createElement('file');
		$file_ele->addText("$sym_header_name");
		$file_ele->appendChild($path_ele);
		$file_ele->appendChild($export_ele);
		
		$sources_ele= $out_xml_doc->createElement('sources');
		$sources_ele->appendChild($file_ele);
		$api_ele->appendChild($sources_ele);
				
		$api_dataset_ele->appendChild($api_ele);
		
	}
    close(H_LIST);

    
    print "\n ---------------------------------------------------------------";
    print "\n XML tags Beautify ";
    $full_apis_xml=$out_xml_decl->toString();
    $full_apis_xml.=$api_dataset_ele->toString();
	$full_apis_xml=~s/><api_dataset>/>\n<api_dataset>/g;
	$full_apis_xml=~s/(.)<api/$1\n\n<api/g;
	$full_apis_xml=~s/><name>/>\n\t<name>/g;
	$full_apis_xml=~s/><libs>/>\n\t<libs>/g;
	$full_apis_xml=~s/><lib/>\n\t\t<lib/g;
	$full_apis_xml=~s/><\/libs>/>\n\t<\/libs>/g;
	$full_apis_xml=~s/><release/>\n\t<release/g;
	$full_apis_xml=~s/><sources>/>\n\t<sources>/g;
	$full_apis_xml=~s/><file>/>\n\t\t<file>/g;
	$full_apis_xml=~s/(\w)<path>/$1\n\t\t\t<path>/g;
	$full_apis_xml=~s/><export>/>\n\t\t\t<export>/g;
	$full_apis_xml=~s/><\/file>/>\n\t\t<\/file>/g;
	$full_apis_xml=~s/><\/sources>/>\n\t<\/sources>/g;
	$full_apis_xml=~s/><\/api>/>\n<\/api>/g;
	
	print "\n $full_apis_xml";
	
    #Write in to sym_apis.xml file
    print "\n ---------------------------------------------------------------";
    print "\n XML writing, Write to $out_dir/$sym_apis_xml";
    open (SYM_APIS,"> $out_dir/$sym_apis_xml") or die ("\n ERROR: Can not open $out_dir/$sym_apis_xml for write");
    	print SYM_APIS $full_apis_xml;
    close (SYM_APIS);
    
    eval
	{
		$doc=$parser->parsefile("$out_dir/$sym_apis_xml");
	};
	if ($@)
	{
		print "\n ERROR: $out_dir/$sym_apis_xml is not a valid XML, $@";
		print "\n ERROR: Creating $out_dir/$sym_apis_xml is failed.";
	}
	#Clean up
	$out_xml_doc->dispose();
	   
}


# Main
my $tmp_path=$ENV{PATH};
#Remove CYGWIN, esim from path, if it is there..
$tmp_path=~s/;.*cygwin.*?;/;/;
$ENV{PATH}=$tmp_path;

#Get the options from user
GetOptions("sdk_h_list=s"=>\$sdk_h_list,
		   "out_dir=s"=>\$out_dir,
           "pf_yrwk=s"=>\$pf_yrwk,
           "help"=>\$help,
           "epoc_root=s"=>\$epoc_root);
           
$sdk_h_list="I:\\lib_find\\sdk_symbian_headers.txt";
$out_dir="I:\\lib_find";
$pf_yrwk="200950";
$epoc_root="S:";

#if (! defined $pf_yrwk || !defined $epoc_root || $help || ! defined $sdk_h_list || ! defined $out_dir)
#{
#	&Usage;
#	exit(0);
#}

print "\n Nokia SDK for Symbian - API info collector";
print "\n --------------------------------------------------------";
print "\n  SDK headers master list: $sdk_h_list";
print "\n  Output Dir: $out_dir";
print "\n  EPOC Root: $epoc_root";
print "\n  Platform Yr-Wk: $pf_yrwk";

#Set global variables
$NM_EXE=$epoc_root."\\epoc32\\gcc_mingw\\bin\\nm.exe";

$lib_collect_dir=$epoc_root."\\epoc32\\release\\armv5\\lib";
push(@lib_collect_dirs,$lib_collect_dir);
print "\n Libs dir: $lib_collect_dir";

$lib_collect_dir=$epoc_root."\\epoc32\\release\\armv5\\udeb";
push(@lib_collect_dirs,$lib_collect_dir);
print "\n Libs dir: $lib_collect_dir";

print "\n --------------------------------------------------------";

#Build the ClassLibDB for every lib in epoc_root
print "\n \n Building ClassLibDB";
print "\n --------------------------------------------------------";
$i=0;

find(\&reap_class_from_lib,@lib_collect_dirs);

#Process the sdk_symbian_headers list
print "\n \n Parse Headers";
print "\n --------------------------------------------------------";
my $h_line;
my $h_file;
$i=0;
open (H_LIST,"< $sdk_h_list") or die "\n ERROR: Can not open $sdk_h_list";
while($h_line=<H_LIST>)
{
	chomp($h_line);
	#Skip comments
	next if ($h_line=~m/^\s*#/);
	
	#$h_line may have export path too
	if($h_line=~m/(.*)::(.*)/)
	{
		$h_file=$epoc_root."\\".$1;
	}
	else
	{
		$h_file=$epoc_root."\\".$h_line;
	}
	#Skip if the header is not there in source tree
	if (! -s $h_file)
	{
		print "\n ERROR: $h_file is MISSING";
		next;
	}
	reap_class_from_h($h_file);
}
close (H_LIST);

#Write it as XML. Format should look like apis.xml
print "\n \n Write sym_apis.xml";
print "\n --------------------------------------------------------";
$i=0;
&write_sym_apis_xml();
