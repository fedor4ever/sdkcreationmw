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
###############################################################################
#
#!/usr/bin/perl
#
# Test executor for Ecmt Server
#
###############################################################################

use strict;
use IO::Socket;
use IO::Select;
use vars qw($gEpocroot $gSock $gSel);

my $KMaxReadAttempts = 20;

if ( $#ARGV != 1 )
	{
	&Usage;
	exit( -1 );
	}

my $plat = shift;
my $cfg = shift;

$plat =~ tr /A-Z/a-z/;
$cfg =~ tr /A-Z/a-z/;

#
# Sanity checks
#
if ( ! ( $plat eq "wins" || $plat eq "winscw" ) )
	{
	print "Error: Plat must be either wins or winscw\n";
	exit( -1 );
	}

if ( ! ( $cfg eq "urel" || $cfg eq "udeb" ) )
	{
	print "Error: cfg must be either urel or udeb\n";
	exit( -1 );
	}

#
# Determine epocroot and launch emu
#
$gEpocroot = GetRoot();
$gEpocroot =~ s/[\/\\]$//; # Strip off any trailing slash (if present)
print "Epocroot is: $gEpocroot\\\n";
system( "start $gEpocroot\\epoc32\\release\\$plat\\$cfg\\epoc.exe" );

#
# Connect to emu
#
print "Connecting...\n";

my $gSock;
my $i;
for ( $i=0; $i<100 && !defined( $gSock ); $i++ )
	{
	# print "Attempt $i\n";
	$gSock = IO::Socket::INET->new( 
				Proto		=> "tcp",
				PeerAddr	=> "localhost",
				PeerPort	=> 49152 + 207
				);
	
	}

die "*** Failed to Connect\n" unless defined( $gSock );
						
print "Connected at attempt $i\n\n";

my $gSel = IO::Select->new($gSock);

#
# Run actual test
#

print "Running Test_1\n"; &Test_1;
print "Running Test_2\n"; &Test_2;
print "Running Test_3\n"; &Test_3;
print "Running Test_4\n"; &Test_4;
print "Running Test_5\n"; &Test_5;
print "Running Test_6\n"; &Test_6;

print "All test passed\n";

exit;





###############################################################################
#
# Test_1
#
###############################################################################

sub Test_1
	{
	#
	# test message
	#
	my $msg = "0x0000002b0x10202059BTCOM 5 HCI 0 IRDACOM 2";
	my $uid = 0x10202059;
	print $gSock $msg;

	my $reply = &ReadMsg( $uid );

	if ( $reply =~ /HCI/ )			#emu sends initial values once, so disregard them
		{
		$reply = &ReadMsg( $uid );
		}
		
	AssertEqual( $reply, "0x000000160x10202059OK" );

	die "*** FAILED $!" unless open( IN, "$gEpocroot\\epoc32\\$plat\\c\\system\\data\\bt.bt.esk" );
	undef $/;
	my $contents = <IN>;
	close IN;

	die "*** FAILED" if ( $contents !~ /hcidllfilename= (\w*).dll/ );
	die "*** FAILED" if ( $1 ne "hci" );
	die "*** FAILED" if ( $contents !~ /port= (\d*)/ );
	die "*** FAILED" if ( $1 ne "5" );

	die "*** FAILED $!" unless open( IN, "$gEpocroot\\epoc32\\$plat\\c\\system\\data\\ir.irda.esk" );
	undef $/;
	my $contents = <IN>;
	close IN;

	die "*** FAILED" if ( $contents !~ /irPhysicalComPort= (\d*)/ );
	die "*** FAILED" if ( $1 ne "2" );
	}

###############################################################################
#
# Test_2
#
###############################################################################

sub Test_2
	{
	#
	# test message
	#
	my $msg = "0x0000002b0x10202059BTCOM 2 HCI 1 IRDACOM 1";
	my $uid = 0x10202059;
	print $gSock $msg;

	my $reply = &ReadMsg( $uid );

	AssertEqual( $reply, "0x000000160x10202059OK" );

	die "*** FAILED $!" unless open( IN, "$gEpocroot\\epoc32\\$plat\\c\\system\\data\\bt.bt.esk" );
	undef $/;
	my $contents = <IN>;
	close IN;

	die "*** FAILED" if ( $contents !~ /hcidllfilename= (\w*).dll/ );
	die "*** FAILED" if ( $1 ne "hci_h4" );
	die "*** FAILED" if ( $contents !~ /port= (\d*)/ );
	die "*** FAILED" if ( $1 ne "2" );

	die "*** FAILED $!" unless open( IN, "$gEpocroot\\epoc32\\$plat\\c\\system\\data\\ir.irda.esk" );
	undef $/;
	my $contents = <IN>;
	close IN;

	die "*** FAILED" if ( $contents !~ /irPhysicalComPort= (\d*)/ );
	die "*** FAILED" if ( $1 ne "1" );

	}


###############################################################################
#
# Test_3
#
###############################################################################

sub Test_3
	{
	#
	# test message
	#
	my $msg = "0x0000003f0x1020205bMEMORY 64 JUSTINTIME 0 PANICS 0 DEBUGINFO 0";
	my $uid = 0x1020205b;

	print $gSock $msg;
	my $reply = &ReadMsg( $uid );

	AssertEqual( $reply, "0x000000160x1020205bOK" );

	die "*** FAILED $!" unless open( IN, "$gEpocroot\\epoc32\\data\\epoc.ini" );
	undef $/;
	my $contents = <IN>;
	close IN;

	die "*** FAILED" if ( $contents !~ /MegabytesOfFreeMemory (\d*)/ );
	die "*** FAILED" if ( $1 ne "64" );
	die "*** FAILED" if ( $contents !~ /JUSTINTIME (\d*)/ );
	die "*** FAILED" if ( $1 ne "0" );

	# ErrRd must not exist
	die "*** FAILED" if open( IN, "$gEpocroot\\Epoc32\\$plat\\c\\system\\bootdata\\ErrRd" );

	# epocwind.out must be read only
	die "*** FAILED" if open( IN, ">$ENV{TEMP}\\epocwind.out" );
	}

###############################################################################
#
# Test_4
#
###############################################################################

sub Test_4
	{
	#
	# test message
	#
	my $msg = "0x000000400x1020205bMEMORY 256 JUSTINTIME 1 PANICS 1 DEBUGINFO 1";
	my $uid = 0x1020205b;

	print $gSock $msg;
	my $reply = &ReadMsg( $uid );

	AssertEqual( $reply, "0x000000160x1020205bOK" );

	die "*** FAILED $!" unless open( IN, "$gEpocroot\\epoc32\\data\\epoc.ini" );
	undef $/;
	my $contents = <IN>;
	close IN;

	die "*** FAILED" if ( $contents !~ /MegabytesOfFreeMemory (\d*)/ );
	die "*** FAILED" if ( $1 ne "256" );
	die "*** FAILED" if ( $contents !~ /JUSTINTIME (\d*)/ );
	die "*** FAILED" if ( $1 ne "1" );

	# ErrRd must exist
	die "*** FAILED $!" unless open( IN, "$gEpocroot\\Epoc32\\$plat\\c\\system\\bootdata\\ErrRd" );
	close IN;

	# epocwind.out must not be read only
	die "*** FAILED $!" unless open( IN, ">$ENV{TEMP}\\epocwind.out" );
	close IN;
	}

###############################################################################
#
# Test_5
#
###############################################################################

sub Test_5
	{
	#
	# test message
	#
	my $msg = "0x0000008e0x1020205aADA 0 PRO 1 IP 1 ADD 0.0.0.0 MASK 0.0.0.0 GW 0.0.0.0 ";
	$msg .=	  "DNS 1 PRE 0.0.0.0 ALT 0.0.0.0 ";
	$msg .=   "PROXY 0 NAME 0.0.0.0 PORT 0 EXC 0.0.0.0";
	my $uid = 0x1020205a;

	print $gSock $msg;
	my $reply = &ReadMsg( $uid );

	AssertEqual( $reply, "0x000000160x1020205aOK" );

	system( "del $gEpocroot\\epoc32\\$plat\\c\\cedout.cfg" );

	system( "$gEpocroot\\epoc32\\release\\$plat\\$cfg\\ceddump.exe" );

	my $i;
	for( $i=0; $i < 10; $i++ )
		{
		last if open( IN, "$gEpocroot\\epoc32\\$plat\\c\\cedout.cfg" );
		}
	die "*** FAILED" if $i == 10;

	undef $/;
	my $contents = <IN>;
	close IN;

	$contents =~ /\[LANService\].ADD_SECTION(.*?)END_ADD/s;
	my $section = $1;
	die "*** FAILED" if ( $section !~ /IpAddrFromServer=TRUE/ );
	die "*** FAILED" if ( $section !~ /IpDNSAddrFromServer=TRUE/ );
	if ( $gEpocroot !~ /Nokia_7710_SDK_CW/ )
		{
		die "*** FAILED" if ( $section !~ /ConfigDaemonName=DhcpServ/ );
		die "*** FAILED" if ( $section !~ /ConfigDaemonManagerName=ConfigDaemonManager/ );
		}

	$contents =~ /\[Proxies\].ADD_SECTION(.*?)END_ADD/s;
	my $section = $1;
	die "*** FAILED" if ( $section !~ /UseProxyServer=FALSE/ );
	}

###############################################################################
#
# Test_6
#
###############################################################################

sub Test_6
	{
	#
	# test message
	#
	my $msg = "0x000000f10x1020205aADA 0 PRO 1 IP 0 ADD 123.123.123.123 MASK 255.255.255.128 GW 192.192.192.1 ";
	$msg .=	  "DNS 0 PRE 231.231.231.231 ALT 154.154.154.154 ";
	$msg .=   "PROXY 1 NAME www.jokuproxy.org PORT 8080 EXC www.dummyproxy.com/exception;www.otherproxy.org/various";
	my $uid = 0x1020205a;

	print $gSock $msg;
	my $reply = &ReadMsg( $uid );

	AssertEqual( $reply, "0x000000160x1020205aOK" );

	system( "del $gEpocroot\\epoc32\\$plat\\c\\cedout.cfg" );

	system( "$gEpocroot\\epoc32\\release\\$plat\\$cfg\\ceddump.exe" );

	my $i;
	for( $i=0; $i < 10; $i++ )
		{
		last if open( IN, "$gEpocroot\\epoc32\\$plat\\c\\cedout.cfg" );
		}
	die "*** FAILED" if $i == 10;

	undef $/;
	my $contents = <IN>;
	close IN;

	$contents =~ /\[LANService\].ADD_SECTION(.*?)END_ADD/s;
	my $section = $1;

	die "*** FAILED" if ( $section !~ /IpNetMask=255.255.255.128/ );
	die "*** FAILED" if ( $section !~ /IpGateway=192.192.192.1/ );
	die "*** FAILED" if ( $section !~ /IpAddrFromServer=FALSE/ );
	die "*** FAILED" if ( $section !~ /IpAddr=123.123.123.123/ );
	die "*** FAILED" if ( $section !~ /IpDNSAddrFromServer=FALSE/ );
	die "*** FAILED" if ( $section !~ /IpNameServer1=231.231.231.231/ );
	die "*** FAILED" if ( $section !~ /IpNameServer2=154.154.154.154/ );
	# Config daemon name must not be present in case of fixes IP
	if ( $gEpocroot !~ /Nokia_7710_SDK_CW/ )
		{
		die "*** FAILED" if ( $section =~ /ConfigDaemonName=/ );
		die "*** FAILED" if ( $section =~ /ConfigDaemonManagerName=/ );
		}

	$contents =~ /\[Proxies\].ADD_SECTION(.*?)END_ADD/s;
	my $section = $1;
	die "*** FAILED" if ( $section !~ /UseProxyServer=TRUE/ );
	die "*** FAILED" if ( $section !~ /ProxyServerName=www.jokuproxy.org/ );
	die "*** FAILED" if ( $section !~ /PortNumber=8080/ );
	die "*** FAILED" if ( $section !~ m#Exceptions=www.dummyproxy.com/exception;www.otherproxy.org/various# );
	}

###############################################################################
#
# Function: 	Reads next message from socket that has specified destination
#
# Precondition:	-
#
# Parameters:	- uid for which to read message
#
# Return value:	-
#
###############################################################################


sub ReadMsg
	{
	my $retries = 0;
	my $wantedUid = shift;

	# print "Waiting for message for $wantedUid\n";

	while ( 1 )
		{
		if ( $gSel->can_read(5) )
			{
			my ($hexLen, $hexUid, $len, $uid);
	
			read( $gSock, $hexLen, 10 );
			read( $gSock, $hexUid, 10 );
			$len = hex $hexLen;
			$uid = hex $hexUid;

			# print "hexLen = $hexLen\nhexUid = $hexUid\n";
			# print "len = $len\nuid = $uid\n";

			my $msgData;
			read( $gSock, $msgData, $len-20 );

			my $r = $hexLen.$hexUid.$msgData;
			if ( $uid == $wantedUid )
				{
				# print "Uids match, reply is $r\n";
				return $hexLen.$hexUid.$msgData;
				}
			else
				{
				# print "Uids DO NOT match, reply is $r\n";
				}
		
			$retries++;
			die "*** NO MESSAGES RECEIVED for $wantedUid" if ( $retries > $KMaxReadAttempts );
			}
		else
			{
			die "*** NO MESSAGES RECEIVED";
			}
		}
	}

###############################################################################
#
# Function: 	Checks that the first string is equal to the second, dies
#		otherwise
#
# Precondition:	-
#
# Parameters:	-
#
# Return value:	-
#
###############################################################################
sub AssertEqual
	{
	my $msg = shift;
	my $expected = shift;

	if ( $msg ne $expected )
		{
		print "*** FAILED***\n";
		print "Received: $msg\n";
		print "Expected: $expected\n";
		die;
		}
	}



###############################################################################
#
# Function: 	Print usage
#
# Precondition:	-
#
# Parameters:	-
#
# Return value:	-
#
###############################################################################

sub Usage
	{
	print "Usage: TestExecutor.pl <plat> <cfg>\n";
	print "where:\n\n";
	print "<plat> = wins or winscw\n";
	print "<cfg> = urel or udeb\n";
	}

###############################################################################
#
# Function: 	Returns path to epocroot. See also tlaunch.pm.
#
# Precondition:	-
#
# Parameters:	-
#
# Return value:	-
#
###############################################################################

sub GetRoot
	{
	my $epocroot = $ENV{EPOCROOT};
	if (defined($epocroot))	
		{
		return $epocroot;
		}

	my $defDevice = `devices -default`;
	$defDevice =~ s/Default device: //;
	my @info = `devices -info \@$defDevice`;
	# print "Info:\n";
	# print @info;
	foreach my $line (@info)
		{
		return $1 if ( $line =~ /Root is (.*)/ );
		}
	return undef;
	}
