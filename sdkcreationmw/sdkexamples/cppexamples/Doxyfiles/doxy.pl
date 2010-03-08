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
#!/usr/local/bin/perl
#
# doxy.pl
#
# THis script generates doxygen formatted documentation from source code
#
# ***********************************************************************
#
#  Change history:
#
#                 13.06.2005 This could be v.1.0
#
#
#
# ***********************************************************************
use strict;
use Tk;
use Tk::NoteBook;
use Tk::DirTree;
use Tk::DialogBox;
use Cwd;
use File::Copy;
my ($TOP_GIF);
$TOP_GIF='top.gif';


#
# finds doc-dirs where is Mainpage.dox
#
sub find_doc {

	my ($rdir_list, $cf, $path, $last_dir, $dir);
	$rdir_list = shift;
	$cf = shift;

	$path = cwd();
	$last_dir=$path;
	$last_dir=~s/.*\///;

	if ($last_dir=~m/^$$cf{'document_dir'}$/) {

		$path=~s/\/$$cf{'document_dir'}.*//;

		if (-s $$cf{'mainpage'}) {
			push(@$rdir_list, $path);
		}
	}

    	foreach $dir (<*>) {
		if (-d $dir) {
			chdir $dir;
			&find_doc($rdir_list, $cf);
			chdir "..";
		}
    	}
}

#
# rips class hierarchy from hierarchy.html
#
sub rip_hierarchy {
	my ($rlines, $line, $l_count, $page);

	$rlines = shift;
	$page = shift;

	$l_count = 0;
	open (HIERARCHY, $page);
	while ($line = <HIERARCHY>) {
		chomp($line);
		if ($line=~m/\<\/ul\>/) {
			if ($l_count==1) {
			push (@$rlines, $line."\n");
			}
			$l_count--;
		}
		if ($line=~m/\<ul\>/) {
			$l_count++;
		}
		if ($l_count > 0) {
			$line=~s/\<h1\>.*\<\/h1\>//;
			push (@$rlines, $line."\n");
		}
		#if ($line=~m/\<h1\>.*\<ul\>/) {
		#	$line=~s/\<h1\>.*\<\/h1\>//;
		#	push (@$rlines, $line."\n");
		#	$l_count++;
		#}
	}
	close (HIERARCHY);
}

#
# rips class hierarchy from annotated.html
#
sub rip_annotated {
	my ($rlines, $line, $page);

	$rlines = shift;
	$page = shift;

	open (ANNOTATED, $page);
	#$line = "<h1>Class Hierarchy</h1>This inheritance list is sorted roughly, but not completely, alphabetically:<ul>\n";
	$line = "This inheritance list is sorted roughly, but not completely, alphabetically:<ul>\n";
	push (@$rlines, $line);
	while ($line = <ANNOTATED>) {
		chomp($line);
		if ($line=~m/.*<tr.*indexkey.*indexvalue.*tr>/) {
			$line=~s/^.*<tr>.*<a//;
			$line=~s/<\/td>.*$//;
			$line=~s/^/<li><a/;
			push (@$rlines, $line."\n");
		}
	}
	close (ANNOTATED);
}

#
# finds right place from Mainpage.dox and inserts
# ripped lines
#
sub find_replace {
	my ($rlines, $infile, $tmpfile, $cf);
	$rlines = shift;
	$cf = shift;

	$infile = $$cf{'document_dir'}.'/'.$$cf{'mainpage'};
	$tmpfile = $infile.'.tmp';

	open (IN_F, $infile) or die "Cannot open input file $infile : $!";
	open (TMP_F, "> $tmpfile") or die "Cannot open tmp file $tmpfile : $!";
	while (<IN_F>) {
		unless ((/\s*\<h1\>.*\<ul\>/) ||
			(/\s*\<li\>/) ||
			(/\s*\<ul\>/) ||
			(/\s*\<\/ul\>/) ||
			(/\s*\*\//))
		{
			print TMP_F $_;
		}
	}
	print TMP_F @$rlines;
	print TMP_F ' */';
	close (IN_F);
	close (TMP_F);
	unlink $infile or die "Cannot unlink file : $!";
	rename $tmpfile, $infile or die "Cannot rename file : $!";
}

#
# Decides from what file class hierarchy should be taken
# routes hierarchy lines to replace-function
#
sub update_mainpage {
	my ($cf, @lines, $hierarchy, $annotated);
	$cf = shift;
	@lines = ();

	$hierarchy = $$cf{'document_dir'}.'/hierarchy.html';
	$annotated = $$cf{'document_dir'}.'/annotated.html';

	if (-s $hierarchy) {
		&rip_hierarchy(\@lines, $hierarchy);
	}
	elsif (-s $annotated) {
		&rip_annotated(\@lines, $annotated);
	}

	if (-s $$cf{'document_dir'}.'/'.$$cf{'mainpage'}) {
		&find_replace(\@lines, $cf);
	}
}

#
# runs doxygen in each dir from where Mainpage.dox is found
# updates class hierarchy in Mainpage.dox
#
sub do_doxy {
	my ($dir, $list, $cf, $doxy_com, $path);
	$list = shift;
	$cf = shift;
	$path = shift() .'/'."$TOP_GIF";

	$doxy_com = $$cf{'doxy_command'}.' '.$$cf{'doxyfile'};

	foreach $dir (@$list) {
		chdir $dir;
		copy($path, $$cf{'document_dir'}.'/');
		system($doxy_com) == 0
			or die "Cannot execute command $doxy_com : $?";
#		&update_mainpage($cf);
#		system($doxy_com) == 0
#			or die "Cannot execute command $doxy_com : $?";
	}
}

#
# checks that configuration file item exist and is defined
#
sub check_conf_item {
	my $cf = shift;
	my $prop = shift;

	die "$prop not found from configuration file." if !exists $$cf{$prop};
	die "$prop not defined in configuration file." if !defined $$cf{$prop};
}

#
# checks configuration file items
#
sub check_configuration {
	my $cf = shift;
	check_conf_item ($cf, 'mainpage');
	check_conf_item ($cf, 'document_dir');
	check_conf_item ($cf, 'doxyfile');
	check_conf_item ($cf, 'doxy_command');
	check_conf_item ($cf, 'root_path');
	check_conf_item ($cf, 'footer');
	check_conf_item ($cf, 'header');
	check_conf_item ($cf, 'stylesheet');
}

#
# loads configuration file and validates it
#
sub conf {
	my $cf = shift;
	my $c_file = shift;

	open (CONFF, $c_file) or die "Cannot open configuration file : $!";
	while (<CONFF>) {
		chomp;
		s/#.*//;
		s/^\s+//;
		s/\s+$//;
		next unless length;
		my ($var, $value) = split (/\s*=\s*/, $_, 2);
		$$cf{$var} = $value;
	}
	close(CONFF);
	check_configuration($cf);
}

#
# GUI : opens file browse window
#
sub gui_browse_file {
	my $mp = shift;
	my $top = shift;
	my $ext = shift;
	my $typelabel = shift;
	my $type = shift;
	my $initialfile = shift;
	my $title = shift;
	my $fi;

	$fi = $$top->getOpenFile(	-defaultextension => "$ext",
					-filetypes =>
					  [["$typelabel", "$type"],
					   ['All files', '*']
					  ],
					-initialdir => Cwd::cwd(),
					-initialfile => "$initialfile",
					-title => "$title"
					);
	$fi=~s/^.*\///;
	$$mp = $fi;
}

#
# GUI : opens directory tree browser
#
sub gui_browse_root_path {
	my $mp = shift;
	my $top = shift;
	my $fi = 0;

	my $dir = Cwd::cwd();

	my $popup = $$top->Toplevel(-title=>'Browse');

	$popup->Scrolled('DirTree',
		  -scrollbars => 'osoe',
		  -width => 50,
		  -height => 30,
		  -selectmode => 'browse',
		  -exportselection => 1,
		  -browsecmd => sub{$dir=shift;},
		  -command   => sub{$fi=1;}
		 )->pack(-fill => "both", -expand => 1);

	$popup->waitVariable(\$fi);
	if ($fi==1) {
		$dir=~s/\//\\/g;
		$$mp = $dir;
	}
	$popup->destroy();
}

#
# GUI : Returns constant text when pressing doxycommand default button
#
sub gui_browse_doxycommand {
	my $mp = shift;
	my $top = shift;
	$$mp = "doxygen";
}

#
# GUI : Returns constant text when pressing docdir default button
#
sub gui_browse_docdir {
	my $mp = shift;
	my $top = shift;
	$$mp = "doc";
}

#
# GUI : Opens configuration file browse window
#
sub gui_browse_open_conf {
	my $cf = shift;
	my $top = shift;
	my $bk = shift;

	my $fi;

	$fi = $$top->getOpenFile(	-defaultextension => ".conf",
					-filetypes =>
					  [['Doxy configurationfile', '.conf'],
					   ['All files', '*']
					  ],
					-initialdir => Cwd::cwd(),
					-initialfile => '',
					-title => 'Select doxy.pl configuration file'
					);
	&conf($cf, $fi);
	$fi=~s/^.*\///;
	$$bk->pageconfigure("tab1", -label => $fi);
}

#
# GUI : saves configuration file
#
sub gui_save_conf {
	my $cf = shift;
	my $file = shift;
	my $key;

	open (SAVE_FILE, "> $file")
		or die "Cannot open file $file to save : $!";

	foreach $key (keys %$cf) {
		print SAVE_FILE "$key=$$cf{$key}\n";
	}

	close (SAVE_FILE);
}

#
# GUI : opens file save dialog for configuration file
#
sub gui_browse_save_conf {
	my $cf = shift;
	my $top = shift;
	my $bk = shift;
	my $fi;
	my $le = $$bk->pagecget("tab1", -label);

	$fi = $$top->getSaveFile(	-defaultextension => ".conf",
					-filetypes =>
					  [['Doxy configuration', '.conf'],
					   ['All files', '*']
					  ],
					-initialdir => Cwd::cwd(),
					-initialfile => $le,
					-title => 'Save doxy configuration file'
					);
	&gui_save_conf($cf, $fi);
	$fi=~s/^.*\///;
	$$bk->pageconfigure("tab1", -label => $fi);
}

#
# GUI : creates menu resource
#
sub gui_create_menu {
	my $top = shift;
	my $cf = shift;
	my $bk = shift;

	my $menu =
    		[
     		 [Cascade => "~File", -tearoff => 0, -menuitems =>
      		  [
       		   [Button => "~Open", -command => [\&gui_browse_open_conf, \%$cf, \$$top, \$$bk]],
       		   [Button => "~Save", -command => [\&gui_browse_save_conf, \%$cf, \$$top, \$$bk]],
       		   [Button => "~Quit", -command => sub{exit();}],
      		  ]
     		 ],
    		];

	my $menub = $$top->Menu(-menuitems => $menu);
	$$top->configure(-menu => $menub);
}

#
# GUI : handles what happens when pressing run button
#
sub gui_run {
	my $cf = shift;
	&check_configuration($cf);
	&update_doxyfile($cf);
	my (@dir_list, $path);
	$path = cwd();
	chdir $$cf{root_path};
	&find_doc(\@dir_list, $cf);
	&do_doxy(\@dir_list, $cf, $path);
	chdir $path;
}

#
# GUI : creates dialog inside tab folder
#
sub gui_create_tab {
	my $book = shift;
	my $top = shift;
	my $configuration = shift;
	my $tab = shift;
	my $label = shift;

	my $conf_tab;

	$conf_tab = $$book->add( $tab, -label=>$label );

	my $frame = $conf_tab->Frame()->pack(-anchor=>'n');

	$frame->Label(-text=>'')->grid(-column=>0,-row=>0,-sticky=>'ew');
	$frame->Label(-text=>'')->grid(-column=>1,-row=>0,-sticky=>'ew');
	$frame->Label(-text=>'')->grid(-column=>2,-row=>0,-sticky=>'ew');


	$frame->Label(-text=>'Mainpage:',-anchor=>'w')->grid(-column=>0,-row=>1,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{mainpage})->grid(-column=>1,-row=>1,-sticky=>'ew');
    	$frame->Button(	-text     => "Browse",
			-command  => [\&gui_browse_file, \$$configuration{mainpage}, \$$top, '.dox','Doxygen mainpage', '.dox', 'Mainpage.dox', 'Select mainpage filename'],
			-width    => 5)->
				grid(-column=>2,-row=>1,-sticky=>'ew', -padx=>5, -pady=>5);


	$frame->Label(-text=>'Doxyfile:',-anchor=>'w')->grid(-column=>0,-row=>2,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{doxyfile})->grid(-column=>1,-row=>2,-sticky=>'ew');
    	$frame->Button(	-text     => "Browse",
			-command  => [\&gui_browse_file, \$$configuration{doxyfile}, \$$top,'','Doxygen configurationfile','Doxyfile','Doxyfile', 'Select doxygen configuration filename'],
			-width    => 5)->
				grid(-column=>2,-row=>2,-sticky=>'ew', -padx=>5, -pady=>5);

	$frame->Label(-text=>'Doxy command:',-anchor=>'w')->grid(-column=>0,-row=>3,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{doxy_command})->grid(-column=>1,-row=>3,-sticky=>'ew');
    	$frame->Button(	-text     => "Default",
			-command  => [\&gui_browse_doxycommand, \$$configuration{doxy_command}, \$$top],
			-width    => 5)->
				grid(-column=>2,-row=>3,-sticky=>'ew', -padx=>5, -pady=>5);

	$frame->Label(-text=>'Document dir:',-anchor=>'w')->grid(-column=>0,-row=>4,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{document_dir})->grid(-column=>1,-row=>4,-sticky=>'ew');
    	$frame->Button(	-text     => "Default",
			-command  => [\&gui_browse_docdir, \$$configuration{document_dir}, \$$top],
			-width    => 5)->
				grid(-column=>2,-row=>4,-sticky=>'ew', -padx=>5, -pady=>5);

	$frame->Label(-text=>'Header file:',-anchor=>'w')->grid(-column=>0,-row=>5,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{header})->grid(-column=>1,-row=>5,-sticky=>'ew');
    	$frame->Button(	-text     => "Browse",
			-command  => [\&gui_browse_file, \$$configuration{header}, \$$top, '.html', 'Header file', '.html', 'header.html', 'Select header filename'],
			-width    => 5)->
				grid(-column=>2,-row=>5,-sticky=>'ew', -padx=>5, -pady=>5);

	$frame->Label(-text=>'Footer file:',-anchor=>'w')->grid(-column=>0,-row=>6,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{footer})->grid(-column=>1,-row=>6,-sticky=>'ew');
    	$frame->Button(	-text     => "Browse",
			-command  => [\&gui_browse_file, \$$configuration{footer}, \$$top, '.html', 'Footer file', '.html', 'footer.html', 'Select footer filename'],
			-width    => 5)->
				grid(-column=>2,-row=>6,-sticky=>'ew', -padx=>5, -pady=>5);

	$frame->Label(-text=>'Stylesheet:',-anchor=>'w')->grid(-column=>0,-row=>7,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{stylesheet})->grid(-column=>1,-row=>7,-sticky=>'ew');
    	$frame->Button(	-text     => "Browse",
			-command  => [\&gui_browse_file, \$$configuration{stylesheet}, \$$top,'.css','Stylesheet', '.css', 'stylesheet.css','Select stylesheet filename'],
			-width    => 5)->
				grid(-column=>2,-row=>7,-sticky=>'ew', -padx=>5, -pady=>5);

	$frame->Label(-text=>'Root path:',-anchor=>'w')->grid(-column=>0,-row=>8,-sticky=>'ew');
	$frame->Entry(-textvariable=>\$$configuration{root_path})->grid(-column=>1,-row=>8,-sticky=>'ew');
    	$frame->Button(	-text     => "Browse",
			-command  => [\&gui_browse_root_path, \$$configuration{root_path}, \$$top],
			-width    => 5)->
				grid(-column=>2,-row=>8,-sticky=>'ew', -padx=>5, -pady=>5);

    	$frame->Button(	-text     => "Run",
			-command  => [\&gui_run, \%$configuration],
			-width    => 5)->
				grid(-column=>1,-row=>9,-sticky=>'ew', -padx=>5, -pady=>5);

}

#
# GUI : Creates GUI
#
sub gui {
	my ($mw, @dir_list, $book, $tab);
	my (%configuration);

	$mw = new MainWindow;
	$mw->geometry( "450x400" );

	&gui_create_menu(\$mw, \%configuration, \$book);

	$book = $mw->NoteBook()->pack( -fill=>'both', -expand=>1 );

	&gui_create_tab(\$book, \$mw, \%configuration, "tab1", "Configuration");

	MainLoop;
}

#
# Updates property in doxygen configuration file
#
sub update_doxyfile_item {
	my $fi = shift;
	my $file = shift;
	my $doxfile = shift;

	my $find = "($fi)(\\s*)(=)(\\s*)(.*)";

	$file=~s/\\/\\\\/g;
	my $replace = "\\1\\2\\3\\4$file";

	system ("perl -p -i.ftr -e \"s/$find/$replace/\" $doxfile") == 0
		or die "Cannot update $fi to $doxfile : $?";
}

#
# List of properties to update in doxygen configuration
#
sub update_doxyfile {
	my $cf = shift;
	update_doxyfile_item("HTML_HEADER", $$cf{header}, $$cf{doxyfile});
	update_doxyfile_item("HTML_FOOTER", $$cf{footer}, $$cf{doxyfile});
	update_doxyfile_item("HTML_STYLESHEET", $$cf{stylesheet}, $$cf{doxyfile});
	update_doxyfile_item("HTML_OUTPUT", $$cf{document_dir}, $$cf{doxyfile});
	#update_doxyfile_item("INCLUDE_PATH", $$cf{include_path}, $$cf{doxyfile});
}

#
# commandline mode
#
sub batch {
	my ($c_file) = shift;
	if (-s $c_file) {
		my (%conf);
		&conf(\%conf, $c_file);
		&update_doxyfile(\%conf);
		my (@dir_list, $path);
		$path = cwd();
		chdir $conf{root_path};
		&find_doc(\@dir_list, \%conf);
		&do_doxy(\@dir_list, \%conf, $path);
		chdir $path;
	}
	else {
		die "Configuration file \"$c_file\" not found : $!";
	}
}

#
# Main program :
#
# if no arguments -> run gui
#
if (0 == scalar(@ARGV)) {
	gui();
	exit();
}
#
# with arguments -> run cmdline version
#
batch(@ARGV[0]);