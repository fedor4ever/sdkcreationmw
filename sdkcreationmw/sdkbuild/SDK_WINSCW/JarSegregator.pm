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

package JarSegregator;

use File::Path;
use File::Copy;
use Cwd;
use XML::DOM;

#Create new object
sub new 
{
	my $class=shift @_;
	my $self={}; # New blank object
	bless ($self, $class);
	return $self;
}

#internal function 
# =======================================================================
#
# Recursive copy
#
# Copies a folder, its sub folders & files therein.
# Paramter 1: Source folder path.
# Paramter 2: Destination folder path.
# return 0 for success
# =======================================================================
sub CopyTree
# =======================================================================
{
use File::Path;
use File::Find;
use Cwd 'abs_path';

    my $CopyFrom = shift @_;
    my $CopyTo = shift @_;
  	if(-e $CopyFrom)
  	{
  		$CopyFrom = abs_path($CopyFrom);
  		$CopyTo = abs_path($CopyTo);
  	}
    my ($src,$RelativePath,$dest);

    # Make sure that the source directory exists, create the top level
    # destination directory
    if (-d $CopyFrom) 
    {
      eval { mkpath($CopyTo) };
      if ($@) 
      {
        print("WARN: Couldn't create $_[0]: $@");
        #return -1;
      }
    } else {
      print ("WARN: Directory $CopyFrom does not exist");
      #return -2;
    }

    print("\n CopyTree: Copying $CopyFrom -> $CopyTo");

    # Traverse file tree (highest level directory first)
    find(sub{
      $src=$File::Find::name;
      $RelativePath=$src;
      $RelativePath=~s/^\Q$CopyFrom\E//;
      $dest=$CopyTo.$RelativePath;
      if (-f $src) 
      {  # Copy file
        if (!copy($src,$dest)) 
        {
          	print ("ERROR: Couldn't copy $src: $@");
        }
      } else {  # Make a duplicate directory if necessary
        unless($dest eq '..'||$dest eq '.') {
          unless(-e $dest) {              
#              mkdir($dest,0775);
              mkpath($dest);
          }
        }
      }
    },$CopyFrom);
    return 0;
}

#This subroutine prepares the environment
#First parameter is the log file handle (optional)
#Second parameter is the j9 file name taken from sdk.ini file
#Third parameter is the j9 path name taken from sdk.ini file
#Fourth parameter is the temporary work area for this jars
sub Prepare_env
{	
	my $self=shift @_;	# class name passed by default
	my $j9filename = shift @_;
	my $j9path = shift @_;
	my $jsr_wa = shift @_;
	my $cmd;
	my @out;
	my $filename="";
	print "\n\n--------------Preparing J9 Extraction Env--------------\n\n";
	$filename = "$j9filename";
	# if directory exists it deletes it	
	if(-d $filename)
	{
		rmtree($filename);
	}
	#copies the j9 file from the platform to the temp work area
	if(-f "$j9path\\$filename.zip")
	{	
		copy ("$j9path\\$filename.zip", "$jsr_wa\\$filename.zip");
	}	
	eval{
		$cmd = "unzip -o $jsr_wa\\$filename.zip -d $jsr_wa\\$filename/"; 	
		@out = `$cmd`;
	};
	if ($@)
		{
			print "\nERROR: $cmd is failed: $@";
			return 1;
		}
		else
		{
			print @out;	
		}
}

#This subroutine segregates the jars using the jsr_specification.xml
#First parameter is the log file handle (optional)
#Second parameter is the temporary work area for this jars
#Third parameter is the hash reference to the information retrieved from jsr_spec.xml
#Fourth parameter is the j9 file name taken from sdk.ini file
sub segregation
{
	my $self=shift @_;	# class name passed by default
	my $temp_path = shift @_;
	my ($href) = shift @_;
	my $j9filename = shift @_;
	my $api="";
	my $packagename="";
	my @packagename="";
	my $packagepath="";		
	print  "\n\n--------------Segregating the JSRs --------------\n\n";
	mkpath ("$temp_path");	
	foreach $api(keys (%$href))
	{	
		$packagelist = %$href->{$api}->{'package'};		
		(@packagenames) = split(/\,/,$packagelist);			
		#copies the packages from the source path to the temp work area
		foreach $packagenames(@packagenames)
		{				
			$packagenames =~ s/\./\\/g;
			$packagepath = $packagenames;		
			$ch = getcwd;			
			chdir "$temp_path" or die ("\nERROR: Can not chdir to $temp_path. $!");
			mkpath ("$api\\$packagepath");
			chdir "$ch" or die ("\nERROR: Can not chdir to $ch. $!");
			if(($api=~m/midp20/g) or ($api=~m/midp21/g) or (($api=~m/jsr139/g)))
			{				
				if($packagepath =~ m/javax\\microedition\\io/gi)
				{
					print "Package path : $packagepath\n";
					system ("copy $j9filename\\$packagepath $temp_path\\$api\\$packagepath");
					next;
				}
			}
			$cmd = CopyTree("$j9filename\\$packagepath","$temp_path\\$api\\$packagepath"); 
			if($cmd!=0)
			{
				print "\n ERROR: $j9filename\\$packagepath is NOT copied to $temp_path\\$api\\$packagepath. $cmd";				
			}
			print $cmd;
		}	
	}	
}

#This subroutine generates the manifest file for each jsr mentioned in jsr_specification.xml
#First parameter is the log file handle (optional)
#Second parameter is the temporary work area for this jars
#Third parameter is the hash reference to the information retrieved from jsr_spec.xml

sub manifest_file_process
{
	my $self=shift @_;	# class name passed by default
	my $temp_path = shift @_;
	my ($href) = shift @_;
	my $api;
	my $manifilename = "manifest.mf";
	my @manifestinfo ="";
	open OUT_TEMP,">Temp_log.txt";
	print "\n\n--------------Manifest file process--------------\n\n";
	open OUTMANIFEST,">$manifilename";
	foreach $api(keys (%$href))	
	{
		foreach $manifest_info(@{%$href->{$api}->{'manifest_info'}})
		{
			$manifest_info =~ s/^\n//;
			#$manifest_info =~ s/\n$//;
			if($manifest_info =~ m/API-Type/)
			{
				push (@manifestinfo,$manifest_info);
				print OUT_TEMP $manifest_info;
				print OUTMANIFEST @manifestinfo;
				@manifestinfo="";
				print OUTMANIFEST "\n";
				close OUTMANIFEST;
				mkpath ("$temp_path\\$api\\META-INF");
				$cmd = copy ("$manifilename", "$temp_path\\$api\\META-INF\\$manifilename");
				if($cmd!=0)
				{
					print "\nCopied $manifilename to $temp_path\\$api\\META-INF\\$manifilename";
				}
				open OUTMANIFEST,">$manifilename";
			}
			else{
				push (@manifestinfo,$manifest_info);
				print OUT_TEMP $manifest_info;
			}
		}
	}
	close OUTMANIFEST;
	close OUT_TEMP;
	$cmd = unlink "$manifilename" or die ("\nERROR: Can not delete $manifilename, $!"); 
	print $cmd;
}

#This subroutine generates the jars for each jsr and copies to target directory
#First parameter is the log file handle (optional)
#Second parameter is the temporary work area for this jars
#Third parameter is the target directory of the jar files to be copied
sub jar_files
{
	my $self=shift @_;	# class name passed by default
	my $temp_path = shift @_;
	my $target_jar = shift @_;
	my $dirname="";
	my @file="";
	my $file="";
	my $api_name="";
	my $cwd="";
	print "\n\n--------------Creating Jars --------------\n\n";
	if(-d "$target_jar")
	{		
		rmtree($target_jar);
	}
	mkpath ("$target_jar");
	$dirname="$temp_path\\";			
	opendir DIR, $dirname or print "\n WARNING: Can not open $dirname, $!";
	@file = readdir DIR;
	shift @file;
	shift @file;
	foreach $file(@file)
	{		
		$cwd = getcwd;		
		$api_name = $file;
		chdir "$temp_path\\$api_name" or die ("\nERROR: Can not chdir to $temp_path\\$api_name. $!");
		eval{
			$cmd = "jar cvfM $file.jar ./";
			print "\n INFO: EXE $cmd \n";
			@cmd = `$cmd`;
		};
		if ($@)
		{
			print "\nERROR: $cmd is failed: $@";
			return 1;
		}
		print @cmd;		
		$cmd = copy ("$file.jar","$target_jar");		
		if($cmd!=0)
		{
			print "\nCopied $file.jar to $target_jar\n" ;		
		}
		@cmd = unlink "$file.jar" or die ("\nERROR: Can not delete $file.jar, $!");
		print @cmd;
		chdir $cwd or die ("\nERROR: Can not chdir to $cwd. $!");
	}	
	closedir DIR;
}

#This subroutine parses the jsr_specification.xml and stores in hash ref
#First parameter is the log file handle (optional)
#Second parameter is the jsr_specification file name
#Third parameter is the hash ref for the jsr information
sub parse_xml_file
{
	my $self=shift @_;	# class name passed by default
	my $file = shift @_;
	my ($href) = shift @_;
	my $parser = new XML::DOM::Parser;
	my $doc = $parser->parsefile($file);
	my $apiname;
	my @symbol_nodes;
	my $symbol_node;
	my $symbol;
	my @manifest_info;
	
	foreach my $jsr ($doc->getElementsByTagName("jsr")) {
	  @symbol_nodes = $jsr->getElementsByTagName("api-name");
	  $symbol_node  = $symbol_nodes[0];
	  $symbol       = $symbol_node->getFirstChild->getData();  
	  print "\n".$symbol."\n";
	  $apiname = $symbol;	  
	  
	  @symbol_nodes = $jsr->getElementsByTagName("package");
	  $symbol_node  = $symbol_nodes[0];
	  $symbol       = $symbol_node->getFirstChild->getData();
		$symbol =~ s/\s+/\,/g;				
		print $symbol;
		
		$href->{$apiname}->{'package'} = $symbol;		
		print $href->{$apiname}->{'package'};
		print "\n";
		
	  @symbol_nodes = $jsr->getElementsByTagName("manifest_info");
	  $symbol_node  = $symbol_nodes[0];
	  $symbol       = $symbol_node->getFirstChild->getData();
		@symbol = split(/\n/,$symbol);
		foreach $symbol(@symbol)
		{
			$symbol =~ s/^\s+//g;		
			$symbol =~ s/\s+$//g;
			push @{$href->{$apiname}->{'manifest_info'}}, "$symbol\n";
		}
		print @{$href->{$apiname}->{'manifest_info'}};
		print "\n";
	}
}


# This subroutine is called from the build script from jsr_jar_segregation
# Second Parameter is the target destination directory
# Third Parameter is the hash reference for sdk.ini file
# Fourth Parameter is the log file directory as "phase"
sub jar_segregation
{	
	my $self=shift @_;	# class name passed by default	
	my $target_jar=shift @_;
	my ($href) = shift @_;

  my $j9filename =$$href{Jsr_j9filename};	
  my $j9path=$$href{j9_filepath};
  my $synergypath=$$href{jsr_spec_cm_path};
  my $xml_file=$$href{jsr_specfile};
  my $jsr_wa = $$href{jsr_wa};	
  
  my $temp_path ="$jsr_wa\\new_jars";
  my %jsr_info="";  
  if(!(-d $jsr_wa))
  {
  	mkpath ("$jsr_wa");
  }
	$ch = getcwd;	
	$self->parse_xml_file($xml_file,\%jsr_info);
	chdir "$jsr_wa"  or die ("\nERROR: Can not chdir to $jsr_wa. $!");
	$self->Prepare_env($j9filename,$j9path,$jsr_wa);
	$self->segregation($temp_path,\%jsr_info,$j9filename);
	$self->manifest_file_process($temp_path,\%jsr_info);
	$self->jar_files($temp_path,$target_jar);	
	chdir "$ch"  or die ("\nERROR: Can not chdir to $ch. $!");
}

return 1;
