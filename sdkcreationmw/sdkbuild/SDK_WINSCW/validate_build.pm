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

package validate_build;
use Data::Dumper;
use IO::File;
use LWP::UserAgent;
use Mail::Sendmail qw(sendmail %mailcfg);
use Cwd;
use IO::Socket;

#Create new object
sub new 
{
	my $class=shift;
	my $self={err_cnt=>shift};
	bless ($self, $class);	
	return $self;
}

#returns the instance variable error count of the class
sub get_error_count
{	
	my $self = shift @_;
	return $self->{err_cnt};	
}

#sets the instance variable error count 
sub set_error_count
{	
	my $self = shift @_;	
	my $value = shift @_;	
	$self->{err_cnt} = $value;
}

#This subroutine opens a log file for accessing through the class
sub open_log_file
{
	my $self=shift @_;	# class name passed by default	
	my $phasedir=shift @_;	
	my $logfile;
	my ($sec,$min,$hour,$mday,$mon,$year) =  localtime();
	my $start_time = sprintf ("%02d:%02d : %02d %02d %02d",$hour,$min,$mday,$mon + 1,($year + 1900)%100);
	
	$logfile = "$phasedir\\buildenvcheck.log";	
	open (FILE,">$logfile") or die "Couldn't create a build env check log file $!";	
	print FILE "\nINFO : Validation Starts at $start_time\n";
	return \*FILE;
}

#This subroutine closes the log file
sub close_logfile
{
	my $self=shift @_;	# class name passed by default	
	my $fh = shift @_;
	my ($sec,$min,$hour,$mday,$mon,$year) =  localtime();
	my $end_time = sprintf ("%02d:%02d : %02d %02d %02d",$hour,$min,$mday,$mon + 1,($year + 1900)%100);
	
	print $fh "\nINFO : Validation Ends at : $end_time\n";
	close $fh;
}

#This subroutine increments the error count
sub incr_error_count()
{
	my $self = shift @_;
	my $err_cnt;
	$err_cnt = $self->get_error_count();
	$err_cnt = $err_cnt +1;
	$self->set_error_count($err_cnt);
}

# This subroutine checks whether the http is accessing
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub verify_IF_Sheets_path
{	
	my $self = shift @_;
	my $url = shift @_;
	my $fh = shift @_;	
  my $ua = LWP::UserAgent->new;
  my $uid;
  my $req;
  my $pwd;
  my $res;
  $ua->cookie_jar( {} );
  
  #getting the password from the already existing path not from the user
  open( PWD, "getPwd Sheets |") or die "Cannot open getPwd: $!";
  chomp($pwd = <PWD>);
  close( PWD );
    
  $ua->env_proxy();
  $uid = getlogin || getpwuid($<);  
  $req = HTTP::Request->new(GET => "$url");
  #Authenticate the url with the userid and password
	$req->authorization_basic($uid, $pwd);  
	
	$res = $ua->request($req);
  
	print $fh "\nINFO : Title : ".$res->title;
	
  if ($res->title =~ /Web Single Sign-On Login/) {
  			#proceed further from the login page
		    $res = $ua->post($res->request->uri->as_string,
		                     [
		                       SMENC    => 'Base64',
		                       USER     => $uid,
		                       PASSWORD => $pwd,
		                       submit   => "Authenticate",
		                     ]);		    
		    $res = $ua->request($req);
		    print $fh "\nINFO : Status :".$res->status_line;
		    print $fh "\nINFO : Title :".$res->title;
		}
	
	if ($res->is_success) 
	{
	 print $fh "\nINFO : $url \t http response is ok";
	}
	else 
	{
		#if http is not accessible then error count is updated for analyzing
  	$self->incr_error_count();
	 	print $fh "\nERROR : $url \t http response is not ok";
	}
}

# This subroutine dumps the sdk.ini to a array ref
sub dump_sdk_ini($) 
{	
	my $self=shift @_;
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
	return \@out;
}
# This subroutine executes the command for validating the software versions
# The second parameter is the command for getting the software version
# The third parameter is the hash ref for parsed sdk.ini
# The fourth paramter is the software type to be checked
# The fifth parameter is the file handle for build log file
sub software_version($$$$)
{
	my $cmd;	
	my $self = shift @_;
	my $version = shift @_;
	my $href = shift @_;
	my $sw_check = shift @_;
	my $fh= shift @_;
		
	eval{
		$cmd = `$version 2>&1`;
		};
	  if ($@)
		{
			print "\nERROR: $cmd is failed: $@";
			return 1;
		}		
		if(($cmd =~ $$href{$sw_check}))
		{
			print $fh "INFO: $sw_check version is fine\n";
		}
		else
		{
			#if any mismatch of version then error count is updated for analyzing
			$self->incr_error_count();
			print $fh "ERROR: $sw_check version is not proper\n";
		}	
}

#Takes array ref and validate paths and notify by mail.
#Second input is $target or phase
#Third is %iniH hash ref
#Forth input is ignore pattern, inititally you can ignore Z: errors
sub validate_sdk_ini_paths($$$$) 
{	
	my $self=shift @_;
	my $sdk_ini_array_ref=shift @_;
	my $phase_name=shift @_;
	my $iniH_ref=shift @_;
	my $fh = shift @_;
	my $ignore_pat=shift @_;
	
	my $line;
	my $val;
	my $err_level=0;
	my $MAX_RETRY=3;
	my $MAX_TIME_WAIT=10;
	my $i;
	my @err_paths;
	
	# Get Valid paths and check for its presense
	print $fh "\nINFO: Validating SDK.ini paths\n";
	print $fh "\nINFO: Ignore following pattern: $ignore_pat\n";
	foreach $line (@$sdk_ini_array_ref)
	{
		next if($line=~m/\s*#/); #Skip the # commented
		#Skip some known invalid keys
		next if($line=~m/Target_Directory/);
		#Skip ignore patterns
		if (length ($ignore_pat)>0 && $line=~m/$ignore_pat/o)
		{
			print $fh "\nSKIP: $line";
			next;
		}		
		if($line=~m/\s+'(.*)'\s+=>\s+'(.*)'/)
		{
			#	checking for Document_url 
			if($val =~ /^https/i)
			{				
				 $self->verify_IF_Sheets_path($val,$fh); 
			}
			$val=$2;
			if($val=~m/^[A-Z]:|^\\\\\\\\(.*)\\+(.*)/i) #Should be valid drive or windows share
			{
				#Strip multiple \\ to \
				$val=~s/\\\\|\//\\/g;
				if (! -d $val)
				{
					$err_level=0;
					if($line =~ m/S60_Root_Path/g)
				  {
				  	next;
				  }
					#Ohh , Got error, Take a nap and come back
					for ($i=0;$i<$MAX_RETRY;$i++)
					{
						
						print $fh "\n ".($i+1)."Retry to access : $val";
						sleep ($MAX_TIME_WAIT);
						if( -d $val)
						{
							$err_level=0;
							last;
						}
						else
						{							
							#if path is not accessible then error count is updated for analyzing
							$self->incr_error_count();
							$err_level++;
						}	
					}										
					if($err_level>=$MAX_RETRY) #Tried at max
					{
#						print $fh "\nERROR: $val is not found. Key: $line";
						push (@err_paths,"\nERROR: $val is not found. Key: $line");
						push (@msgerr_paths,"\nMSGERROR: $val is not found. Key: $line");
					}
					
				}
				else
				{
					print $fh "\nINFO: $val";
				}
			}
		}		
	}

	if(@err_paths) #There are some errors, Looks like.
	{
		$,="\n";		
		my $msg= <<END_OF_MSG;
@err_paths
END_OF_MSG
		print $fh "$msg\n";
	}
	else
	{
		print $fh "\nINFO : All the paths is SDK.ini is valid\n";
	}	
}

# This subroutine checks the software versions 
# It refers the versions from the sdk.ini file
# The second parameter is the hash reference of the parsed sdk.ini file
# The third parameter is file handle for build log file

sub check_for_sw_paths
{
	my $self=shift @_;	# class name passed by default	
	my $href = shift @_;
	my $fh = shift @_;
	my $sw_check ='';	
	my @software_check_list='';		
	
		
	@software_check_list = split(/\,/,$$href{Software_list});
		
	foreach $sw_check(@software_check_list)
	{
		if($sw_check eq "Java"){ # The order should match to the sdk.ini
			$self->software_version("javac -version",$href,$sw_check,$fh);
		}
		if($sw_check eq "Perl"){
			$self->software_version("perl -v",$href,$sw_check,$fh);
		}
		if($sw_check eq "Ant")
		{
			$self->software_version("ant -version",$href,$sw_check,$fh);
		}
		if($sw_check eq "Rvct"){	
			$self->software_version("armcc",$href,$sw_check,$fh);
		}
		if($sw_check eq "Doxygen"){
			my $doxy_path="$$href{StartDir}\\$$href{PTBase}\\doxygen\\doxygen.exe";
			$doxy_path=~s/\/+/\\/g;
			$self->software_version("$doxy_path -h",$href,$sw_check,$fh);
		}
		if($sw_check eq "CarbideIDE"){		
			$self->software_version("mwldsym2 -version",$href,$sw_check,$fh);
		}
	}		
}

#This subroutine verifies whether the time server is properly mapped or not
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub verify_time_server
{
	my $self=shift @_;	# class name passed by default
	my $href = shift @_;	
	my $fh = shift @_;	
	
	eval{
		$cmd = `net time /query`;		
	};
	if ($@)
	{
		print "\nERROR: net time command is failed: $@";
		return 1;
	}
	if($cmd =~ $$href{TimeServer})
	{
		print $fh "\nINFO : time server is correct";
	}
	else
	{
		print $fh "\nERROR: time server is incorrect\n";
	}
}


# This subroutine checks the build directories present in the system
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub check_for_build_directories
{
	my $self=shift @_;	# class name passed by default
	my $href = shift @_;
	my $val = shift @_;
	my $fh = shift @_;	
	my $cmd;
	my $cmd_values;		
	my @dir_value=($$href{Sce_Root},$$href{S60_Root});
		
	if($val ne "all")	
	{
		eval{
		$cmd = `subst`;	
		};
		if ($@)
		{
			print "\nERROR: subst command is failed: $@";
			return 1;
		}	
		$cmd_values = shift @dir_value;
		$self->verify_dir($cmd,$cmd_values,$fh);
		$cmd_values = shift @dir_value;
		$self->verify_dir($cmd,$cmd_values,$fh);
	}
	$self->verify_network_dir($href,$fh);
}

# This subroutine verifies the existing of the net directories 
# if doesn't exist it maps to network drive
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub verify_network_dir
{	
	my $self=shift @_;	# class name passed by default
	my $href = shift @_;	
	my $fh = shift @_; 
  my $drive;
  my $dir;
  my %drivehash =("$$href{TestMapDrive}","$$href{TestMapDirectory}","$$href{FilerblrDrive}","$$href{FilerblrMapDirectory}");
    
  eval{
		$cmd = `net use`;		
	};
	if ($@)
	{
		print "\nERROR: net use command is failed: $@";
		return 1;
	}
  
  foreach $drive(keys %drivehash)
  {   	
  	$dir = $drivehash{$drive};
		if($cmd =~ $drive)
		{	
			print $fh "\nINFO : $drive build drive exists\n";
		}
		else
		{		
		  print $fh "\nWARN : Attempting to map $drive\n";		
			eval{
				$netcmd = `net use $drive $dir /USER:$$href{Logonname}`;
			};
			if ($@)
			{
				print "\nERROR: net use command is failed: $@";
				return 1;
			}
			if($netcmd =~ m/(.*)error(.*)/g)
			{
				$self->incr_error_count();
				print $fh "\nERROR: can't create the $drive drive $netcmd\n";
			}
			else
			{
				print $fh "\nINFO: $drive Drive mapped successfully\n";
			}
		}
	}
}

sub verify_dir
{	
	my $self=shift @_;	# class name passed by default
	my $cmd = shift @_;
	my $cmd_values = shift @_;
	my $fh = shift @_;

	if($cmd =~ $cmd_values)
	{	
		print $fh "\nINFO : $cmd_values build drive exists\n";
	}
	else
	{
		$self->incr_error_count();
		print $fh "\nERROR :$cmd_values build drive does not exist\n";
	}
}

# This subroutine checks the space of the disk against the minimum 
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub check_space_dir
{	
	my $self=shift @_;	# class name passed by default
	my $href = shift @_;
	my $fh = shift @_;
	my $mindiskfreespace = $$href{MinDiskSpace};
	my $DiskFree;
	my @cmd;
	my $cmd;
	
	eval{
	@cmd = `dir`;
	};
	if ($@)
	{
		print "\nERROR: dir command is failed: $@";
		return 1;
	}
	foreach $cmd(@cmd)
	{		
		if($cmd =~ m/.*\)\s+(.*)bytes\sfree/g)
	  {	  	
	  	$_ = $1;	  	
	  	s/\,//g;
	  	$DiskFree = $_;	  	
	  }
	}
	$DiskFree = $DiskFree/(1024*1024*1024); # To Convert in terms of GB
	print $fh "\nINFO: ".$DiskFree."\tGB Free Space\n";	
	
	if($DiskFree < $mindiskfreespace)
	{
		#if low disk space then error count is updated for analyzing
		$self->incr_error_count();
		print $fh "ERROR: Disk Free Space is very low $DiskFree GB\n";		
	}
	elsif (($DiskFree > $mindiskfreespace) and ($DiskFree < $mindiskfreespace + 5)) # 5GB Has to be updated
	{
		print $fh "WARN : Disk Free space is low\n";
	}
	else
	{
		print $fh "INFO : Disk Free space is Fine\n";
	}
}

#This subroutine checks the compilation of the perl files in the current dir
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub perl_file_compilation
{
	my $self=shift @_;	# class name passed by default
	my $href = shift @_;
	my $fh = shift @_;	
	my $outtext ="";
	
	eval{
	@cmd = `dir /B *.pl *.pm`;
	};
	if ($@)
	{
		print "\nERROR: dir command is failed: $@";
		return 1;
	}	
	
	foreach $perlfile(@cmd)
	{
		chomp($perlfile);
		eval{
			$outtext = `perl -c $perlfile 2\>\&1`;
			};
			if ($@)
			{
				print "\nERROR: perl command is failed: $@";
				return 1;
			}
		if($outtext =~ m/error|failed/gi)
		{
			$self->incr_error_count();
			print $fh "\nERROR: $perlfile file compilation error";
		}
		else
		{
			print $fh "\nINFO: $perlfile file compilation is fine";
		}
	} 
}
#This subroutine checks the synergy access and existence of project
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub cm_access
{
	my $self=shift @_;	# class name passed by default
	my $href = shift @_;
	my $fh = shift @_;	
	
	
	eval{
		
		my $pwd = `getPwd Synergy`;
	  my $addr = `ccm start -r build_mgr -q -nogui -m -pw $pwd`;   
	  $ENV{CCM_ADDR} = $addr if $addr;
	  if (!$addr and $$href{'Reconfigure'} =~ /(TRUE|YES)/i)
	  {
	  	print $fh "ERROR : Synergy Logon failed\n";
	  	#if access problem then error count is updated for analyzing
			$self->incr_error_count();
	  	die "Logon failed!!!" 
	  }
	  warn "Logon failed", return unless $addr;
	  print $fh "INFO : Synergy logon success :$addr\n";  
	  #Print project info
	  my ($proj_name, $proj_version) = split "-", $$href{SCM_Project};
	  warn "Project not found", return unless $proj_version;
	  print $fh "\nINFO : Synergy $proj_name\-$proj_version exists\n";	  
	};
	if ($@)
	{
		print "\nERROR: CMSynergy command is failed: $@";
		return 1;
	}
	if(-d $$href{SCM_Workarea})
	{
		print $fh "\nINFO: Synergy Project Work area Directory exists\n";		
	}
	else
	{
		$self->incr_error_count();
		print $fh "\nERROR: Synergy project work area directory does not exists\n";
	}
	system("ccm stop");
}

# This subroutine checks whether the build service is running 
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub build_id_server_access
{
	my $self=shift @_;	# class name passed by default
	my $href = shift @_;
	my $fh = shift @_;	
	
	my $server_host = $$href{Build_id_service_host};	
	my $PORTNO = $$href{Build_id_service_portno};  
  my ($sock, $msg, $port, $ipaddr, $hishost,$MAXLEN, $TIMEOUT);
  my $ret = "8888"; # Dummy build id
  
     
  $msg = "";
  $MAXLEN = 64;
  $TIMEOUT = 5;
  
  $sock = IO::Socket::INET->new(Proto     => 'udp',
                                PeerPort  => $PORTNO,
                                PeerAddr  => $server_host)
      or do { warn "Creating socket: $!\n"; system ("color 0C"); return $ret;};
  
  #Check server is alive or not
  $sock->send("ALIVE") or do {warn "send: $!\n"; return $ret;};
  $sock->recv($msg, $MAXLEN) or do { warn "recv: $!\n";print $fh "\nERROR: BuildID server is incorrect\n";return $ret;};
#  print "\n $msg";
  chomp($msg);
	if($msg!~m/alive/i)
  {
  	#if build id service down then error count is updated for analyzing
  	$self->incr_error_count();
  	print $fh "\nERROR: Build ID server is down";  	
  }
  else
  {
  	print $fh "\nINFO: Build Service on $server_host is up\n";
  }  
}

# sends a mail notification after the paths and environment are fine
# The second parameter is the hash ref for parsed sdk.ini
# The third parameter is the file handle for build log file
sub Send_MailNotification
{ 
	my $self=shift @_;	# class name passed by default 
	my $href = shift @_;	
	my $phasedir = shift @_;
	
  my $sub;
  my $machine_name;
  my @msg;
  my $err_cnt;
  my $path;
 
  $platform = "S60 ".$$href{Edition}. " SDK"; # for displaying N97 SDK Edition
		
	$machine_name = $ENV{COMPUTERNAME};
	$err_cnt = $self->get_error_count();
	#check the error count before sending a mail
	if($err_cnt != 0)
	{		
		$sub = "ERROR";
	}
	else
	{	
		print "\nBuild Environment Checks is Fine : $platform in $machine_name\n";		
		return 0;
	}
	$sub = "$sub Build Environment Checks : $platform in $machine_name\n";
	
	open LOG_FILE,"$phasedir\\buildenvcheck.log";
	while($line = <LOG_FILE>)
	{
		if($line =~m/.*ERROR.*/g)
		{
			push @msg,$line;
		}
	}
	close LOG_FILE;
	
	$path = "\\\\$machine_name$$href{validatelogpath}buildenvcheck.log";
		
	push @msg,"\n Check for the validation log file in the path :$path\n";
	print $sub;
	print @msg;
	
	$mailcfg{smtp} = [qw(mgw.nokia.com)];
	$mailcfg{from} = "$$href{Mail_From}";
	%mail = ( To      => "$$href{Mail_To}",	          
						Cc      => "$$href{Mail_Cc}",
						Subject => "$sub",
            Message=>"@msg"
	         );
  print "\n Sending Mail  to $$href{Mail_To}\n";
	sendmail(%mail) or die $Mail::Sendmail::error;	
}

sub Notify_sms
{   
   my $self=shift @_;	# class name passed by default 
	 my $href = shift @_;	
	 my $phasedir = shift @_;	 
	 my $sms_out="";
	
   my $sms_gw='smsgw.nokia.com';
   
   $platform = "S60 ".$$href{Edition}. " SDK"; # for displaying N97 SDK Edition
		
	 $machine_name = $ENV{COMPUTERNAME};
	 $err_cnt = $self->get_error_count();	 
	 #check the error count before sending a mail
	 if($err_cnt != 0)
	 {		
		$sub = "ERROR";
	 }
	 else
	 {	
		print "\nBuild Environment Checks is Fine : $platform in $machine_name\n";		
		return 0;
	 }	   
   $sub = "$sub Build Env:in $machine_name\n";
   
   $sms_out= $self->analyze_log($phasedir);  
     
   $mailcfg{smtp} = [qw(mgw.nokia.com)];
   $mailcfg{from} = "$$href{SMS_From}";
   
   print $sub;
   print @$sms_out;
   
   my $sms_list = "$$href{SMS_To}";
   my @sms_numbers=split(/,/,$sms_list);
   my $mail_to="";
   map{$mail_to.="$_".'@'.$sms_gw.','} @sms_numbers; # Add @smsgw.nokia.com to all numbers 
   $mail_to=~s/,$//; #strip last ,
	%mail = ( To      => "$mail_to",
						Cc      => "",
						Subject => "$sub",
						Message => "@$sms_out"
	         );
  print "\n Sending SMS  to $$href{SMS_To} by $mail_to ";
	sendmail(%mail) or die $Mail::Sendmail::error;
	return 1;
}

sub analyze_log
{
		my $self=shift @_;	# class name passed by default
		my $phasedir = shift @_;
		my @sms_report= "";
		open LOG_FILE,"$phasedir\\buildenvcheck.log";		
		$temp="Path";
		$err_msg="";
		while($line = <LOG_FILE>)
		{
			if($line =~m/.*ERROR.*/)
			{				
				$line =~ s/ERROR\:\s+//g;
				push @err_msg,$line;
				if($line =~ m/Key/g)
				{
					$access_msg="AccessPath:";
				}
				if($line =~ m/version/g)
				{
					$version_msg="Ver:";
				}
				if($line =~ m/Disk/g)
				{
					$disk_msg="DFSpace:";
				}
			}			
		}
		close LOG_FILE;		
		foreach $err_msg(@err_msg)
		{
			$_ = $err_msg;
			s/\n//g;
			if($_ =~ m/Key/g)
			{
				s/.*Key\:\s+//g;
				s/\'(.*)\'\s\=\>(.*)/$1,/g;
				s/\_//g;
				s/\n//g;
				$access_msg=$access_msg.$_;
			}
			elsif($_ =~ m/version/g)
			{
				s/(.*)\sversion\sis\snot\sproper/$1,/g;
				s/\n//g;
				$version_msg=$version_msg.$_;				
			}
			elsif($_ =~ m/Disk/g)
			{
				s/(.*)low\s(.*)\sGB/$2GB/g;				
				$temp_msg = sprintf "%.1f", $2;				
				$disk_msg = $disk_msg.$temp_msg."GB";				
				push @msg,$disk_msg;
			}
			elsif($_ =~ m/time/g)
			{
				 push @msg,"TimeServer Incorrect";
			}
			elsif($_ =~ m/BuildID/g)
			{
				push @msg,"BuildID Server";
			}
			elsif($_ =~ m/Synergy/g)
			{
				push @msg,"Synergy Error";
			}
			elsif($_ =~ m/compilation/g)
			{
				push @msg,"Compilation Error";
			}
		}
		chop($version_msg);
		chop($access_msg);		
		push @msg, $access_msg;		
		push @msg, $version_msg;
		push @sms_report,@msg;
		return (\@sms_report);
}
return 1;
