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

package Remote_utils;
use SDK_vars;
use SDK_utils;
use ApiRef;
use File::Path;

use XML::Writer;
use IO::File;
use XML::Simple;

use HTTP::Response;
use HTTP::Request::Common qw(POST);
use LWP::UserAgent;
use Mail::Sendmail qw(sendmail %mailcfg);
use Cwd;

our ($VERSION, @ISA, @EXPORT);

$VERSION = 1.00;
@ISA = qw(Exporter);
@EXPORT = qw(copy_platform reconfigure fetch_IF_sheets create_api_and_black_lists Notify_plain_mail Notify_mail beatnik fetch_capas Notify_sms);

sub copy_platform {
  my ($href) = @_;
  enter_sub();

  my $curr_dir = cwd;
  system ("subst");
  chdir "$$href{S60_Root}\\";
  my $cmd = "xcopy $iniH{Getenv_server}\\Builds\\Tools\\getenv\\getenv.pl $$href{S60_Root} /F /R /Y";
  system ($cmd)==0 or die "$cmd failed!\n";
  foreach my $pl_src_path ( split (/ /, $iniH{Platform_Build}) ){
    $cmd = "perl getenv.pl -start -k -path $pl_src_path -grace -verbose";
    print "$cmd\n"; system ($cmd);
  }
  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
};

sub reconfigure {
  my ($href) = @_;
 	my $four_part_name;
  enter_sub($$href{SCM_Project});

  $|=1;

  my $pwd = `getPwd Synergy` or print "\n ERROR: Can not Execute getPwd Synergy";
  my $addr = `ccm start -r build_mgr -q -nogui -m -pw $pwd`;
  print "\n CCM ADDRESS: $addr \n ";
  $ENV{CCM_ADDR} = $addr if $addr;
  die "Logon failed!!!" if (!$addr and $$href{'Reconfigure'} =~ /(TRUE|YES)/i);
  warn "Logon failed", return unless $addr;
  #Print project info
  my ($proj_name, $proj_version) = split "-", $$href{SCM_Project};
  warn "Project not found", return unless $proj_version;
  chomp(my $project_object = `ccm query -type project -name $proj_name -version $proj_version -u -f \"%objectname\"`);

  system "ccm query -type project -name $proj_name -version $proj_version -u -f \"%objectname  %release\"";
  print "Reconfigure $$href{'Reconfigure'}\n";
 	if ($$href{'Reconfigure'} =~ /(TRUE|YES)/i) {
    #Print the reconf time to the log
    print "Reconf_time : ";
    system "ccm attr -s reconf_time \"$project_object\"";
 		#Force Maintain workarea on
 		my $cmd = "ccm wa -wa -relative -recurse -p \"$project_object\" -path \"$iniH{SCM_WA_Path}\"";
  	print ("\n$cmd\n\n"); system ($cmd);
	  #First Reconcile
	  system "ccm reconc -ignore_uncontrolled -recurse -update_wa $project_object";
	  #Print rp
	  system "ccm rp /sh i $project_object";
	  #and tasks
	  #then reconfigure
	  system "ccm reconfigure /recurse /project $project_object";
	  #Print rp
	  system "ccm rp /sh i $project_object";
	  #Print the baseline
	  $cmd = "ccm rp -sh b \"$project_object\" -u";
	  print "$cmd\n";
	  system($cmd);
	  $four_part_name="$project_object";
	  print "Fout Part Project Name: $four_part_name\n";
	  print "\n\nConflicts Start\n";
	  $cmd = "ccm conflicts -recurse -nowrap -tasks $four_part_name";
	  print "$cmd\n";
	  system($cmd);
	  $cmd = "ccm conflicts -recurse -nowrap $four_part_name";
	  print "$cmd\n";
	  system($cmd);
	  print "\n\nConflicts End\n";
	  #and tasks
	  $cmd = "ccm rp -sh all_tasks \"$project_object\" -f \"%displayname %release  %completion_date  %owner %task_synopsis\"";
	  print "$cmd\n";
    system ($cmd);

    #Print the task according to  Print_tasks_since  specification in sdk.ini, or print yesterday anyway
    my @tasks_since = split " ", $iniH{Print_tasks_since} || ("-1:0:0:0");
    for (my $i=0; $i < @tasks_since -1; ++$i){
      $cmd = "ccm query \"(is_task_in_rp_of('$project_object') or is_task_in_folder_of(is_folder_in_rp_of('$project_object'))) and completion_date >= time('$tasks_since[$i]') and completion_date < time('$tasks_since[$i+1]')\" -f \"%displayname  %task_synopsis\"";
     	print "\n$cmd\n\nTasks completed between [$tasks_since[$i] - $tasks_since[$i+1]):\n";
      system ($cmd);
      print "\nEnd of recent tasks.\n";
    }
    $cmd = "ccm query \"(is_task_in_rp_of('$project_object') or is_task_in_folder_of(is_folder_in_rp_of('$project_object'))) and completion_date > time('$tasks_since[@tasks_since-1]')\" -f \"%displayname  %task_synopsis\"";
   	print "\n$cmd\n\nRecently completed tasks (since $tasks_since[@tasks_since-1]):\n";
    system ($cmd);
    print "\nEnd of recent tasks.\n";

    #Print the reconf time to the log
    print "Reconf_time : ";
    system "ccm attr -s reconf_time \"$project_object\"";
	}
  #Grab the work area path
  chomp($wa_path = `ccm attr /s wa_path \"$project_object\"`);
  $$href{SCM_Workarea} = $wa_path if ($wa_path and !$$href{SCM_Workarea});
 	print "\nSCM WorkArea: $$href{SCM_Workarea}\n";
  system "ccm stop";
  exit_sub();
};

sub fetch_IF_sheets 
{

  my ($sheet_source_dir, $IFs_dir, $iniH) = @_;
          
  #Check if repository directory is ready to be used, ie. it exists and is not empty, otherwise use document browser
  ($sheet_source_dir, my $doc_browser_url) = check_sheet_repository( $sheet_source_dir, $href );

  enter_sub($sheet_source_dir || $doc_browser_url);
  my $curr_dir = cwd;
  

  
	mkpath($IFs_dir) unless (-e $IFs_dir);             #Make it
	#All_If_sheets_S60.zip will be downloaded to this location and then copied to $IFs_dir.
	mkpath($iniH{'Sheets_Local_Repo'}) unless (-d $iniH{'Sheets_Local_Repo'});
	
 
  chdir $iniH{'Sheets_Local_Repo'} or print "\n ERROR: Can not chdir to $iniH{'Sheets_Local_Repo'}, $!";

  unless ( $sheet_source_dir )
  {#If source directory does not exists, get the stuff from server
	print "\n Download IFs to $iniH{'Sheets_Local_Repo'}, URL: $doc_browser_url";
    #Get password from registry, not from user!
    open( PWD, "getPwd Sheets |") or die "Cannot open getPwd: $!";
    chomp(my $pwd = <PWD>);
    close( PWD );

		$ua = LWP::UserAgent->new;
		$ua->cookie_jar( {} );
		my $uid = getlogin || getpwuid($<);
		my $req = HTTP::Request->new(GET => "$doc_browser_url/All_If_sheets_S60.zip");
		$req->authorization_basic($uid, $pwd);
		$res = $ua->request($req);
		print "\n WARNING: Fetching $doc_browser_url All_If_sheets_S60.zip FAILED!!!\n Local copy from $iniH{'Sheets_Local_Repo'} will be used." unless ($res->is_success);

		if ($res->title =~ /Web Single Sign-On Login/) 
		{
		    $res = $ua->post($res->request->uri->as_string,
		                     [
		                       SMENC    => 'Base64',
		                       USER     => $uid,
		                       PASSWORD => $pwd,
		                       submit   => "Authenticate",
		                     ]);
		    # We need to follow the redirect from the login page
		    $res = $ua->request($req, "All_If_sheets_S60.zip");    #The name for the output file needs to be specified somehow. Next line works also.

		    print "\n WARNING: *** WebSSO post failed: ,Local copy from $iniH{'Sheets_Local_Repo'} will be used. ", $res->status_line if $res->is_error;
		    print "\n WARNING: WebSSO Login failed (bad password?), Local copy from $iniH{'Sheets_Local_Repo'} will be used."          if $res->as_string =~ /authenticate via WebSSO/;
		}

    my $req2 = HTTP::Request->new(GET => "$doc_browser_url/Other/tools_if_sheets_compare.zip");

    $res2 = $ua->request($req2, "tools_if_sheets_compare.zip");
    print "\n WARNING: Fetching  tools_if_sheets_compare.zip failed, Local copy from $iniH{'Sheets_Local_Repo'} will be used." unless ($res2->is_success);
  }
  elsif(-d $sheet_source_dir) 
  {
    my $cmd = "xcopy $sheet_source_dir\\*.zip  /F /R /Y";                        #Copy All_If_sheets_S60.zip and tools
    system ($cmd)==0 or print ($cmd."\n");
    $cmd = "xcopy $sheet_source_dir\\*.xls  /F /R /Y";                        #(First) sheet patches
    system ($cmd)==0 or print ($cmd."\n");
  }
  else 
  {
    print "Nothing to fetch!\n";
  }
  
  #Now copy from local $iniH{'Sheets_Local_Repo'} to  $IFs_dir, So if IFSHEETS site is down, it will pick from local copy
  chdir $IFs_dir or print "\n ERROR: Can not chdir to $iniH{'Sheets_Local_Repo'}, $!";
  
  print "\n\n Copy IFs from $iniH{'Sheets_Local_Repo'} to $IFs_dir\n";
  $cmd = "xcopy $iniH{'Sheets_Local_Repo'}\\*.zip /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");
  
  # Copy (own) patch sheets
  print "\n\n Copy IFs patches from $iniH{ Sheet_Patches } to $IFs_dir\n";
  $cmd = "xcopy $iniH{ Sheet_Patches }\\*.xls /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");

  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
  exit_sub();
}

sub check_sheet_repository {
  my ($zip_dir, $iniH) = @_;
  my $doc_browser_url = "$iniH{Doc_Browser_Url}";

  (-e $zip_dir) || return ("", $doc_browser_url);
  opendir(my $DIR, $zip_dir) || do {print "can't opendir $zip_dir: $!\n"; return ("", $doc_browser_url)};
  my @zips = grep { /\.(zip|xls)$/i } readdir($DIR);
  closedir $DIR;

  if (@zips){              #We found some zips or xls files -> use that directory
    return ($zip_dir, "");
  }
  return ("", $doc_browser_url);
}

sub create_api_and_black_lists {

#  @_ = qw( 3.0 IFs sheets result \\Repository_area\\CPP_API \\Repository_area\\Common_Blacklist) unless @_;
  my ($rel, $IFs_dir, $sheets_sub_dir, $result_dir, $cpp_api_dir, $black_list_dir, $org_dir) = @_;

  enter_sub();
  my $curr_dir = cwd;
  return unless (chdir $IFs_dir);

  my $all_zip = "All_If_sheets_S60.zip";
  $all_zip = "All_If_sheets_S60_2.zip" unless (-e $all_zip);         #Try the produced one if we are not supposed to use the server

  my $cmd = "unzip -o $all_zip -d $sheets_sub_dir";
  print ($cmd."\n"); system ($cmd);

  $cmd = "xcopy *.xls $sheets_sub_dir\\ /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");


  $cmd = "unzip -o tools_if_sheets_compare.zip";
  print ($cmd."\n"); system ($cmd);

#  print "result dir: $result_dir\n";
  mkpath $result_dir unless (-e $result_dir);
  $cmd = "perl gather_sdk_info.pl -p S60 -r $rel -d $sheets_sub_dir -t $result_dir";
  print ($cmd."\n"); system ($cmd);

	#Put them into one file if sheets were specified for S60, sdk_apis.txt
	if ($iniH{S60_API} eq 'sheets'){
    $cmd = "type $result_dir\\_sdk_*.txt > $result_dir\\sdk_apis.txt";
    print ($cmd."\n"); system ($cmd);
  } else {                                 #or if metaxmls are used
    $cmd = "type $result_dir\\_sdk_*symbian*.txt > $result_dir\\sdk_apis_symbian.txt";
    print ($cmd."\n"); system ($cmd);
    $cmd = "type $result_dir\\_sdk_*nokia*.txt > $result_dir\\sdk_apis_nokia.txt";
    print ($cmd."\n"); system ($cmd);
  }

  $cmd = "move /y $result_dir\\_black_list.txt $result_dir\\black_list.txt";        #Renaming!
  print ($cmd."\n"); system ($cmd);
  $cmd = "xcopy $result_dir\\black_list.txt $black_list_dir\\ /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");

  $cmd = "zip -j All_If_sheets_S60_2.zip $sheets_sub_dir\\*.*";
  print ($cmd."\n"); system ($cmd);
	system ("del All_If_sheets_S60.zip") if (-e "All_If_sheets_S60.zip");


  $cmd = "xcopy $result_dir\\sdk_*.txt $cpp_api_dir\\ /F /R /Y";
  system ($cmd)==0 or print ($cmd."\n");

  chdir $curr_dir or die "Couldn't chdir to $curr_dir: $!";
	print "create_api_and_black_lists finished === ", scalar localtime(), "\n";
}



sub Notify_mail {

  my ($msg) = @_;

	print "Notifying $msg\n\n";
	$mailcfg{smtp} = [qw(mgw.nokia.com)];
	$mailcfg{from} = "$iniH{Mail_From}";

	%mail = ( To      => "$iniH{Mail_To}",
	          Cc      => "$iniH{Mail_Cc}",
						Subject => "$msg",
            'content-type' => 'text/html; charset="iso-8859-1"',
	         );

  $mail{body} = <<END_OF_HEADER;
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
    "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <title> $msg </title>
<style type="text/css">
  #container
  {
    padding: 1em;
    width: 90%;
    margin: 10px auto;
    line-height: 110%;
  }

  body {
    color: blue;
    background: white;
    margin-left: 1em;
    margin-right: 1em;
    font-family: arial, helvetica, sans-serif;
  }
  pre {
    font-family: monospace;
  }
  .tasks {
    border: none;
    font-family: monospace;
    font-size: 70%;
    color: green;
  }
  .components {
    border: none;
    font-family: monospace;
    font-size: 80%;
  }
  .errors {
    border: none;
    font-family: monospace;
    font-size: 70%;
    color: red;
  }
  .platform_patches {
    border: none;
    font-family: monospace;
    font-size: 70%;
    color: maroon;
  }
  .error_reports {
    border: none;
    font-family: monospace;
    font-size: 70%;
    color: magenta;
  }
  .navbar {
    position: relative;
    margin: 0;
    padding: 1em;
    font-size: 70%;
  }
  A:link, A:visited { text-decoration: none; }
  A:hover { background: cyan; }
</style>
</head>
END_OF_HEADER

  $mail{body} .= <<END_OF_BODY;
<div id="container">
$body
</div>
</body>
</html>
END_OF_BODY

# Create the mail report in build output directory
    my $report_fn="$iniH{Output_Base}\\$iniH{Release}\\$iniH{Platform_Version_Major}.$iniH{Platform_Version_Minor}\\$iniH{Start_time}\\SDK_team_only\\Build_Log\\Build_Report.html";
    open (BR, ">$report_fn") or print "\n ERROR: Can not create $report_fn";
    print BR $mail{body};
    close(BR);
    
    #Send out mail.
	sendmail(%mail) or die $Mail::Sendmail::error;

	print "$msg. Log says:\n", $Mail::Sendmail::log;
}
#This Sub will take sub, meg as input and send a plain mail.

sub Notify_plain_mail {
    
    enter_sub();
    my ($sub, $msg) = @_;

	print "\n\n Notifying $sub\n\n";
	$mailcfg{smtp} = [qw(mgw.nokia.com)];
	$mailcfg{from} = "$iniH{Mail_From}";

	%mail = ( To      => "$iniH{Mail_To}",
	          Cc      => "$iniH{Mail_Cc}",
						Subject => "$sub",
            Message=>"$msg"
	         );
    print "\n Sending Mail  to $iniH{Mail_To}, $iniH{Mail_Cc}";
	sendmail(%mail) or die $Mail::Sendmail::error;
	print "\n Mail Log: $Mail::Sendmail::log" if ($!);
	print "\n Mail has been sent.";
	exit_sub();
}

sub Notify_sms {
  my ($sms_receivers, $body) = @_;
	my $ua = LWP::UserAgent->new();
	my $res = $ua->request(POST 'http://smsgw.nokia.com/smsgw/systemSender',
	            [ APPLICATION_ID => 'SDK_Creation',
	              gsmnum => $sms_receivers,
	              message=> $body,
	            ]);

  print "$msg.\n";
  # Check the outcome of the response
  if ($res->is_success) {
      print "Success: $res->content";
  }
  else {
      print $res->status_line, "\n";
  }
}

sub beatnik {

  my ($source) = @_;

  enter_sub($source);

  #Delete the possible old directory

  unless (-d $source ) {                #Try to find the package in GRACE
                                        #We need to extract the files from a big rosetta/ariane zip, may change in the future
    my $pl_version = "$iniH{Platform_Version_Major}_$iniH{Platform_Version_Minor}";
#    my $zip_file = "$iniH{Grace_server}\\S60RnD\\S60_$pl_version\\S60_${pl_version}_$iniH{Platform_Year}$iniH{Platform_Week}_RnD_rosetta\\S60_${pl_version}_$iniH{Platform_Year}$iniH{Platform_Week}_RnD_rosetta_epoc32.zip";
    my $zip_file = "$iniH{Grace_server}\\S60RnD\\S60_$pl_version\\S60_${pl_version}_$iniH{Platform_Year}$iniH{Platform_Week}_RnD_ariane\\S60_${pl_version}_$iniH{Platform_Year}$iniH{Platform_Week}_RnD_ariane_epoc32.zip";
    my @dlls = qw(
    		BAEMmfAudioController.dll
    		BAEMmfCodec.dll
    		BAEMmfMidiController.dll
    		BAEMmfStructuredFormat.dll
        mobilebae.dll
    );
    my @rsc_files = qw(
    		BAEMmfAudioController.rsc
    		BAEMmfCodec.rsc
    		BAEMmfMidiController.rsc
    		BAEMmfStructuredFormat.rsc
    );
    my
    $beatnik_files = join " ", map {"epoc32\\release\\winscw\\udeb\\$_"} @dlls;
    $beatnik_files .= " ";
    $beatnik_files .= join " ", map {"epoc32\\release\\winscw\\udeb\\z\\resource\\plugins\\$_"} @rsc_files;
    $beatnik_files .= " epoc32\\release\\winscw\\udeb\\z\\resource\\ensb.dat";

    #Unzip the files onto repository
    #NOTE: unzip can create only one directory level!
    my $cmd = "unzip -o -C $zip_file $beatnik_files -d $source";
    print "$cmd\n";
    system("$cmd");
  }

  #Finally copy the files onto repository area
}
sub fetch_capas {

  my ($source) = @_;

  enter_sub($source);

  #Delete the possible old repository_area directory

  unless (-d $source ) {                #Try to find the package in GRACE
                                        #We need to extract the files from a big rosetta zip, may change in the future
    $SOS = get_Symbian_OS_info();
    die "No Symbian info found" unless ($SOS =~ /[\d_]/);                            #Should contain only digits or underscore

    my ($sos, $symb_year_week) = split "_", $SOS;
    my ($symb_major, $symb_minor) = split "", $sos;
    my $capa_file_trunk = "GT_${symb_major}.${symb_minor}";
    my $zip_file = "$iniH{Grace_server}\\SymbianOS\\__SymbianOS_${sos}\\SymbianOS_${sos}_$symb_year_week\\MCL_${sos}_${symb_year_week}_src_common_other_sources_part1.zip";

    #Unzip the files onto repository
    #NOTE: unzip can create only one directory level!
    my $cmd = "unzip -o -j -C $zip_file src\\common\\developerlibrary\\doc_source\\reference\\capability\\$capa_file_trunk\\${capa_file_trunk}.xml.txt -d $source";
    print "$cmd\n";
    system("$cmd");

    mkpath("$source\\epoc32\\data\\capabilities") unless (-e "$source\\epoc32\\data\\capabilities");             #Make the directory for move
    $cmd = "move /y $source\\${capa_file_trunk}.xml.txt $source\\epoc32\\data\\capabilities\\${capa_file_trunk}.xml";        #Renaming!
    print ($cmd."\n"); system ($cmd);

  }

  #Finally copy the files onto repository area. We create the proper directory (epoc32\data\capabilities) only here; perhaps should be in unzip already

}
__END__
package SubHandlers;
sub api {
}
sub header {
  print $Remote_utils::OUT_PUT "@_[3]\n"; #The tag name (=header name) is under index 3
}
sub lib {
  print $Remote_utils::OUT_PUT "@_[3]\n"; #The tag name (=lib name) is under index 3
}
sub api_ {
}
