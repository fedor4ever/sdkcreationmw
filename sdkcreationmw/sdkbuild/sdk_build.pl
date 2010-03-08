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
#! /usr/bin/perl
#
# Series 60 SDK build script.
#
# Reads the configuration from sdk_build.conf file located in the same
# directory as the build script. The comments in the configuration file
# descripe its format.
#
# The important feature of this script is that it's not only part of the
# nightly build, but also part of the SDK development environment.
#
#   First, it re-generates the diffs after developer has
#   modified a platform file. It uses the diff utility
#   provided by the Symbian build environment (as opposed
#   to taking it from the PATH, be it MKS Toolkit, cygwin
#   or whatever else you may have installed). That makes
#   sure that the diff is produced in a standard format.
#   Also, if the diff didn't really change (the script will
#   ignore non-essential changes in the diff, such as time
#   stamps in the header) the diff file in platform_mod
#   doesn't get updated.
#
#   Second, the developers can (and do) re-run this script in
#   their work area, and that will produce the same result as
#   when it runs on top of a clean build area. Maybe not 100%
#   the same, but accurate enough to provide, say 95% guarantee
#   that if the build compiles on the developer's machine, it
#   will do the same in the nightly build. That will help us
#   to avoid broken builds.
#
#   Third, the script has minimal dependency on the
#   environment (i.e. environment variables, where the
#   CodeWarrior/VisualStudio/Perl are installed, etc.)
#   A typical problem of the build scripts is that they
#   make too many assumptions about the environment, and
#   it's only a matter of time when one build script starts
#   conflicting with another. Most developers build multiple
#   products on their machines. I've been in the situations
#   when one development environment breaks another. Very
#   frustrating and inefficient.
#

use Cwd;
use File::Copy;
use File::Temp;
use File::Path;
use File::Find;
use FindBin;
use Env qw(JAVA_HOME EPOCROOT BUILD_ROOT MWCIncludes PATH);

push (@INC, $FindBin::RealBin);

require "sdk_env.pl";

MAIN:{
  ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime ();

# Log levels
$LOG_DEBUG = 2;
$LOG_VERBOSE = 1;
$LOG_NORMAL = 0;
$LOG_QUIET = -1;
$LOG_SILENT = -2;

# Build types
$BUILD_TYPE_CPP = 1;
$BUILD_TYPE_MIDP = 2;
$BUILD_TYPE_ALL = 3;

# Editable configuration
$LOGLEVEL = $LOG_NORMAL;
$DO_PATCH = 0;
$DO_DIFF = 0;
$BUILD_PLATFORM = 0;
$BUILD_JAVA = 0;
$BUILD_SDK = 0;
$LOG = 0;
$CLEAN = 0;
$REBUILD = 0;
$BUILD_TYPE = $BUILD_TYPE_ALL;
$CHECK_BUILD = 0;
$DO_UNPATCH = 0;
$FINALIZE_S60EX = 0;

# Non-edtable configuration
$FS = '/';  # File separator
$PS = ';';  # Path separator
@PLATFORMS = ("winscw");
@VARIANTS  = ("udeb", "urel");
@SDK_MODULES = (
  "NMIT",
 # "sdkcommonutils",
  "msgsimulation",
  "xrpcrt",
  "wsock",
  "scard",
#  "MIDP", later needs to be enabled 
  "emumenubar",
  "bluetoothdriver",
  #"Tools",
 # "AsyStub"
);

$PATCH_LIST_FNAME = "sdk_build.conf";
$PLATFORM = get_plat_name ();
$SDK_PROJ_NAME = "PublicSDK/sdkcreationmw"; 
$PATCH_DIR = "$SDK_PROJ_NAME/sdkplatformupdates/";
$BUILD_SCRIPTS_DIR = $BUILD_ROOT . "$SDK_PROJ_NAME/sdkbuild";
$S60_TOOLS_DIR = $BUILD_ROOT . "$SDK_PROJ_NAME/sdkruntimes";
$BACKUP_PREFIX = ".#";
$PATCH_EXT = "diff";
$DIED_MSG = "ERROR!";
$WARNING = "WARNING:";
$NOTE = "NOTE:";
$NULL_NAME = "NULL";

$patch_list_path = "$BUILD_ROOT$PATCH_DIR/$PATCH_LIST_FNAME";
$TMP_CONF = "$BUILD_SCRIPTS_DIR/__tmp.conf";

$DIFF_EXE =  "$BUILD_SCRIPTS_DIR/diff.exe";
$PATCH_EXE = "$BUILD_SCRIPTS_DIR/patch.exe";
$REDIFF_CMD = "$DIFF_EXE -N -c5";

$LOG_FILE = sprintf ("sdk_build-%d%02d%02d_%02d-%02d-%02d.log",
                     $year + 1900, $mon + 1, $mday, $hour, $min, $sec);

# Define Java build configuration
$JAVA_SRC_ROOT = "${BUILD_ROOT}PublicSDK/sdkcreationmw/sdkconnectivityfw/emuconnectmanager/epdt_java";
$JAVA_BUILD_DIR = "${JAVA_SRC_ROOT}";
$JAVA_DIST_DIR = "${JAVA_SRC_ROOT}/build/ecmt";
$JAVA_TARGET_DIR = "${BUILD_ROOT}epoc32/tools/ecmt";
@JAVA_CLEAN_DIRS = (
  "$JAVA_TARGET_DIR/config",
  "$JAVA_TARGET_DIR/language",
  "$JAVA_TARGET_DIR/lib",
  "$JAVA_TARGET_DIR/log",
  "$JAVA_TARGET_DIR/plugins"
);


# parse the command line
  parse_cmd_line (@ARGV);

  $BUILD_ROOT = $ENV{BUILD_ROOT};
  $EPOCROOT = $ENV{EPOCROOT};

  @bld_info = ();
  if ($LOG && !(open (LOG_FH, "> $LOG_FILE"))) {
    warn "Could'n open log file $LOG_FILE. Logging is turned OFF.\n";
    $LOG = 0;
  }

  # Make sure that the required executables are there
  if (!-e $DIFF_EXE) {
    err ($DIFF_EXE . " is missing");
    exit 1;
  }

  if (!-e $PATCH_EXE) {
    err ($PATCH_EXE . " is missing");
    exit 1;
  }

  msg ("Build script started at " . localtime());
  msg ("Cmd line: $0 @ARGV");

  open PATCH_LIST, $patch_list_path or die "$DIED_MSG Could not find $patch_list_path!";

  if ($DO_PATCH) {

    # IMPORTANT! SDK now maintains two startup-lists, which both are modified
    # versions of the original one, but patched differently. To achieve this
    # we have to first make a copy of the original list. The copy is made of
    # .orig, if such exists - if not (i.e. very first patch), then from then
    # original file.
    # Unfortunately there is no simple way to do this without hardcoding.
    #
    #$starter_path = "${BUILD_ROOT}sf/os/devicesrv/sysstatemgmt/starter/starterserver/data";
    #$starter_list = "starter.rss";
    #$starter_list2 = "starter_full.rss";

    #if (not -e "$starter_path/$starter_list2") {
     # dbg ("making a copy of the startup-list");
     # if (-e "$starter_path/$starter_list.orig") {
      #  copy_or_die ("$starter_path/$starter_list.orig", "$starter_path/$starter_list2");
     # } else {
      #  copy_or_die ("$starter_path/$starter_list", "$starter_path/$starter_list2");
     # }
   # }
    #else {
     # dbg ("full startup-list already exists");
   # }
    
   #  This starter is commented by Dhanvantri For Integration  
   # $starter_non_critical_path = "${BUILD_ROOT}sf/os/devicesrv/sysstatemgmt/starter/starterserver/data";
   # $starter_non_critical_list = "starter_non_critical_1.rss";
   # $starter_non_critical_list2 = "starter_non_critical_1_full.rss";

   # if (not -e "$starter_non_critical_path/$starter_non_critical_list2") {
   #   dbg ("making a copy of the startup-list");
   #   if (-e "$starter_non_critical_path/$starter_non_critical_list.orig") {
   #     copy_or_die ("$starter_non_critical_path/$starter_non_critical_list.orig", "$starter_non_critical_path/$starter_non_critical_list2");
    #  } else {
     #   copy_or_die ("$starter_non_critical_path/$starter_non_critical_list", "$starter_non_critical_path/$starter_non_critical_list2");
     # }
   # }
   # else {
    #  dbg ("full startup-list for non critical section already exists");
    #}
  }

  # Collect full path names for all pathes.
  @all_patches = find_all ("$BUILD_ROOT$PATCH_DIR");
  $lineno = 0;
  while (<PATCH_LIST>) {
    $lineno++;
    chomp;
    # Input lines might end in backslashes to indicate continuation
    if (s/\\$//) {
        $_ .= <PATCH_LIST>;
        redo unless eof;
    }

    s/\#.*$//;                 # Get rid of the comments
    next if (/^\s*$/);         # Skip empty lines
    my @fdata = split /:/;     # Split the input line
    foreach (@fdata) {         # Ignore spaces before and after delimiters
      s/^\s+//;
      s/\s+$//;
    }

    my $path = shift @fdata;
    next if ($path eq "");

    # The first field is a name of the patch file.
    # By default it's "diff"
    my $relative_path = $path;
    my $patch_file = get_patch_fname ($path, @all_patches);

    if ($patch_file eq "") {
      $patch_file = "$BUILD_ROOT" . "$PATCH_DIR" . "$path.$PATCH_EXT";
      dbg ("Looking for: $patch_file");
      if (not -e $patch_file) {
        # This means that there is an entry in the patch list file
        # but there is no actual patch file under patch dir.
        # If so skip the line. The exception is DO_DIFF case since do_diff
        # can create patch files.
        if ($DO_DIFF) {
          $patch_file = "";
        }
        else {
          msg("$WARNING $PATCH_LIST_FNAME:$lineno: Missing patch file for $path");
          next;
        }
      }
    }
    else {
      $patch_file .= ".$PATCH_EXT";
    }

    # This shouldn't happen: there is a patch file name but no name for file
    # to be patched. If so skip this line.
    next if ($path eq "");
    $path = "$BUILD_ROOT" . $path;

    if ($LOGLEVEL >= $LOG_VERBOSE) {
      if (-e "$patch_file") {
        dbg ("patch file: $patch_file");
      } else {
        dbg ("patch file does not exist: $patch_file");
      }
      dbg ("fdata: @fdata");
    }

    # Diff'ing files only. It will generate new "diff" files.
    if ($DO_DIFF) {
      chdir_or_die ($BUILD_ROOT);
      do_diff ($path, $patch_file);
      next;
    }

    # Patch platform files
    if ($DO_PATCH) {
      my $res_file = $patch_file;
      $res_file =~ s/\.$PATCH_EXT$//;

      # Make a backup copy of the current version of the patched file.
      if (-e $path) {
        my $backup_path = get_last_backup_path ($path);
        if (!run("$DIFF_EXE $res_file $path", $NULL_NAME)) {
          dbg("$path is identical to $res_file");
        }
        elsif (-e $backup_path &&
            !run("$DIFF_EXE $backup_path $path", $NULL_NAME)) {
          dbg("$path is identical to $backup_path");
        } else {
          # file has changed, create a backup copy
          my $backup_path = get_new_backup_path ($path);
          dbg ("backup to $backup_path");
          copy ("$path", "$backup_path") or
            msg("$WARNING failed to copy $path -> $backup_path");
        }
      }

      # If patch file does not exist, patch means a copy
      if (-e "$patch_file") {
        patch_files ($path, $patch_file);
      } else {
        copy_or_die ($res_file, $path);
        msg ("Copied $path");
      }
    }


    if ($DO_UNPATCH) {
      do_unpatch($path, $patch_file);
    }

    # Process build info
    if ($#fdata >= 0) {
      if ($BUILD_PLATFORM || $CHECK_BUILD) {
        process_bld_info ($path, @fdata);
      }
    }
  }

  close PATCH_LIST;

  # if temp conf-file was used (i.e. -t option given), remove tmp-file
  if ($patch_list_path == $TMP_CONF) {
    dbg("deleting $TMP_CONF");
    unlink $TMP_CONF;
  }

  # Build Java first
  if ($BUILD_JAVA) {
    build_java ();
  }

  # build SDK modules
  # NOTE: some of the hooks in the platform code depend on the headers
  # exported by, for example, MsgRelay module
  if ($BUILD_SDK) {
    foreach $m (@SDK_MODULES) {
      if ($m =~ m/^wsock$/i) {
        # hack to make EXPORT work reliably
        my $now = time;
        my $esk = "$S60_TOOLS_DIR/wsock/data/wsock.esk";
        dbg ("touching $esk");
        utime ($now, $now, $esk) or msg ("$WARNING couldn't touch $esk\n");
      }
      # the rest is a standard procedure for all modules
      build_series60_tools_component ($m); 
    }
  }

  if ($BUILD_PLATFORM) {
    dbg ("Build info: @bld_info");
    do_platform_build (@bld_info);
  }

  #rebuild Java Subsystem in S60 (System AMS) except for C++ SDK
  if($BUILD_PLATFORM && $BUILD_TYPE != $BUILD_TYPE_CPP){
  	dbg ("Building java platform:");
  	do_java_platform_build();
  }

  if ($CHECK_BUILD) {
    foreach $m (@SDK_MODULES) {
      check_series60_tools_component ($m);
    }
    dbg ("Build info: @bld_info");
    check_platform_build (@bld_info);
  }

  if($FINALIZE_S60EX && $BUILD_TYPE != $BUILD_TYPE_MIDP){
  	dbg ("Finalizing S60 C++ examples:");
  	do_s60ex_finalizing();
  	#TODO: Cleaning may not be required for 9.l check and add components in the below function
  	#do_s60_rndtools_cleaning();
  }
  
  
	msg ("Build script finished at " . localtime());
  close ($LOG_FH) if $LOG;
}

# =======================================================================
  sub usage
# =======================================================================
  {
    print "Usage: sdk_build [-p | --patch] [-d | --diff] [-u | --unpatch]\
                 [-b | --build] [-m | --module [mod]] [-a | --all]\
                 [-r | --rebuild] [-c | --clean] [-e | -- err] [-l | --log]\
                 [-s | --silent] [-q | --quiet] [-v | --verbose]\
                 [-f | --config <file>] [-h | --help]\
                 [-t | --target <file>]\
                 [--env] [--midp] [--cpp] [--udeb] [--urel] [--s60ex]\
Options:\
  -p, --patch         patch the platform files\
  -d, --diff          diff the platform files and update patches\
  -u, --unpatch       undo patches and return platform to it's original state\
  -b, --build         build patched platform modules (implies -p)\
  -m, --module [mod]  build SDK modules (or one module if specified)\
  -j, --java          build Java mofules (EcmtManager, ...)\
  -a, --all           build everything (implies -b, -p, -m and -j)\
  -r, --rebuild       do complete rebuild rather than incremental build\
  -c, --clean         delete the output and intermediate files\
  -e, --err           check build errors\
  -l, --log           create log file in the current directory\
  -q, --quiet         be quiet\
  -s, --silent        be completely silent\
  -v, --verbose       be verbose (repeatable)\
  -f, --config <file> use <file> instead of patches/$PATCH_LIST_FNAME\
  -t, --target <file> patch/diff/unpatch a single target: <file>\
  -h, --help          print this help message and exit\
  --midp              MIDP build (don't build C++ specific components)\
  --cpp               C++ build (don't build MIDP specific components)\
  --udeb              only build debug (UDEB) targets\
  --urel              only build release (UREL) targets\
  --s60ex             finalize S60 C++ example applications\
  --env               print (some) build environment and exit\n";
  }

# =======================================================================
  sub parse_cmd_line
# =======================================================================
  {
    if ($#_ < 0) {
      usage ();
      exit;
    } else {
      $CONF_FILE_GIVEN = 0;
      $TARGET_FILE_GIVEN = 0;
      my $n = $#_ + 1;  # total number of options
      my $u = 0;        # number of unknown options
      while(defined (@_[0])) {
        my $arg = shift;
        if ($arg eq "-v" || $arg eq "--verbose")  { $LOGLEVEL++; }
        elsif ($arg eq "--env") {
          $LOGLEVEL = $LOG_VERBOSE;
          dbg("Environment variables:\n  JAVA_HOME: $ENV{JAVA_HOME}\n  EPOCROOT: $ENV{EPOCROOT}\n  BUILD_ROOT: $ENV{BUILD_ROOT}\n  MWCIncludes: $ENV{MWCIncludes}\n  PATH: $ENV{PATH}\n  INCLUDE: $ENV{INCLUDE}\n  LIB: $ENV{LIB}\n");
          exit;
        }
        elsif ($arg eq "--midp") {
          if ($BUILD_TYPE == $BUILD_TYPE_CPP) {
            err ("--midp and --cpp options are mutually exclusive");
            exit 1;
          }
          $BUILD_TYPE = $BUILD_TYPE_MIDP;
        }
        elsif ($arg eq "--cpp" || $arg eq "--c++") {
          if ($BUILD_TYPE == $BUILD_TYPE_MIDP) {
            err ("--midp and --cpp options are mutually exclusive");
            exit 1;
          }
          $BUILD_TYPE = $BUILD_TYPE_CPP;
        }
        elsif ($arg eq "--udeb") { @VARIANTS  = ("udeb"); }
        elsif ($arg eq "--urel") { @VARIANTS  = ("urel"); }
        elsif ($arg eq "-m" || $arg eq "--module" ) {
          $BUILD_SDK = 1;
          if ($DO_DIFF) {
            err ("--diff and --module options are mutually exclusive");
            exit 1;
          }
          # optional module name
          if (defined(@_[0])) {
            my $mod = @_[0];
            if (!($mod =~ m/^-/)) {
              @SDK_MODULES = (shift);
            }
          }
        }
        elsif ($arg eq "-b" || $arg eq "--build" ) {
          $BUILD_PLATFORM = 1;
          if (!$DO_UNPATCH) {
          $DO_PATCH = 1;
          }
          if ($DO_DIFF) {
            err ("--diff and --build options are mutually exclusive");
            exit 1;
          }
        }
        elsif ($arg eq "-p" || $arg eq "--patch" ) {
          $DO_PATCH = 1;
          if ($DO_DIFF) {
            err ("--diff and --patch options are mutually exclusive");
            exit 1;
          }
          if ($DO_UNPATCH) {
            err ("--patch and --unpatch options are mutually exclusive");
            exit 1;
          }
        }
        elsif ($arg eq "-u" || $arg eq "--unpatch") {
          $DO_UNPATCH = 1;
          if ($DO_DIFF) {
            err ("--diff and --unpatch options are mutually exclusive");
            exit 1;
          }
          if ($BUILD_PLATFORM) {
            $DO_PATCH = 0;
          }
          if ($DO_PATCH) {
            err("--patch and --unpatch options are mutually exclusive");
            exit 1;
          }
        }
        elsif ($arg eq "-d" || $arg eq "--diff") {
          $DO_DIFF = 1;
          if ($BUILD_PLATFORM) {
            err ("--diff and --build options are mutually exclusive");
            exit 1;
          }
          if ($DO_PATCH) {
            err ("--diff and --patch options are mutually exclusive");
            exit 1;
          }
          if ($DO_UNPATCH) {
            err ("--diff and --unpatch options are mutually exclusive");
            exit 1;
          }
        }
        elsif ($arg eq "-l" || $arg eq "--log") { $LOG = 1; }
        elsif ($arg eq "-q" || $arg eq "--quiet") { $LOGLEVEL = $LOG_QUIET; }
        elsif ($arg eq "-s" || $arg eq "--silent") { $LOGLEVEL = $LOG_SILENT; }
        elsif ($arg eq "-j" || $arg eq "--java") { $BUILD_JAVA = 1; }
        elsif ($arg eq "-a" || $arg eq "--all") {
          $BUILD_PLATFORM = 1;
          $BUILD_JAVA = 1;
          $BUILD_SDK = 1;
          $DO_PATCH = 1;
          if ($DO_DIFF) {
            err ("--diff and --all options are mutually exclusive");
            exit 1;
          }
          if ($DO_UNPATCH) {
            err ("--all and --unpatch options are mutually exclusive");
            exit 1;
          }
        }
        elsif ($arg eq "-r" || $arg eq "--rebuild") { $REBUILD = 1; }
        elsif ($arg eq "-c" || $arg eq "--clean") { $CLEAN = 1; }
        elsif ($arg eq "-e" || $arg eq "--err") { $CHECK_BUILD = 1; }
        elsif ($arg eq "-h" || $arg eq "--help" ) { usage (); exit; }
        elsif ($arg eq "-f" || $arg eq "--config" ) {
          $CONF_FILE_GIVEN = 1;
          if ($TARGET_FILE_GIVEN) {
            err ("--config and --target options are mutually exclusive");
            exit 1;
          }
          if (defined(@_[0])) {
            $patch_list_path = shift;
          }
          else {
            err ("-f should follow the filename");
            exit 1;
          }
        }
        elsif ($arg eq "-t" || $arg eq "--target") {
          $TARGET_FILE_GIVEN = 1;
          if ($CONF_FILE_GIVEN) {
            err ("--config and --target options are mutually exclusive");
            exit 1;
          }
          if ($BUILD_PLATFORM) {
            err ("--target can only be used with --patch, --unpatch or --diff");
            exit 1;
          }
          if (defined(@_[0])) {
            # patch or diff target-file given. create a temporary config-file
            # note, makes sense only when doing just patch, unpatch or diff
            # operation.
            my $tmp_file = shift;
            system("echo $tmp_file > $TMP_CONF");
            $patch_list_path = $TMP_CONF;
          }
          else {
            err ("--target should follow the path to the patched file");
            exit 1;
          }
        }
        elsif ($arg eq "--s60ex") { $FINALIZE_S60EX = 1; }
        else {
          warn "unrecognized commang line option $arg\n";
          $u++;
        }
      }
      if ($n == $u) {
        # not a single valid option
        usage ();
        exit;
      }
      if ($CHECK_BUILD) {
        $BUILD_SDK = 0;
        $BUILD_PLATFORM = 0;
        $DO_PATCH = 0;
        $BUILD_JAVA = 0;
      }
    }
  }


# =======================================================================
# Builds Java part of the SDK.
# =======================================================================
  sub build_java
# =======================================================================
  {
    if ($REBUILD || $CLEAN) {
      chdir_or_die ($JAVA_SRC_ROOT);
      for $i (0 .. $#JAVA_CLEAN_DIRS) {
        $dir = $JAVA_CLEAN_DIRS[$i];
        dbg ("Deleting $dir");
        rmtree($dir);
      }

      chdir_or_die ($JAVA_BUILD_DIR);
      run_build_cmd ("ant clean");

    }

    if (!$CLEAN) {
      chdir_or_die ($JAVA_BUILD_DIR);
      my $epdt_target = "build";
      if ($BUILD_TYPE == $BUILD_TYPE_MIDP) {
        $epdt_target = "midp";
      } elsif ($BUILD_TYPE == $BUILD_TYPE_CPP) {
        $epdt_target = "cpp";
      }
      run_build_cmd ("ant " . $epdt_target);
      xcopy ($JAVA_DIST_DIR, $JAVA_TARGET_DIR);

      if ($BUILD_TYPE == $BUILD_TYPE_MIDP ||
      					$BUILD_TYPE == $BUILD_TYPE_ALL) {
				open(DUMMY_FH, ">","${JAVA_TARGET_DIR}\\MIDP.txt");

				if(!copy("${BUILD_ROOT}\\sdkcreationmw\\sdkcomponents\\Common_Patch\\epoc32\\data\\sdk_info.properties",
				"${BUILD_ROOT}\\epoc32\\data\\sdk_info.properties"))
				{
					msg("Couldn't copy file sdk_info.properties\n");
					msg($!);
				}


			}

    }
  }

# =======================================================================
  sub do_diff
# =======================================================================
  {
    my $path = shift;
    my $patch_file = shift;
    my $relative_path;
    my $relative_orig;

    ($relative_path = $path) =~ s/^([A-Za-z])\:$FS//g;
    $relative_orig = $relative_path . ".orig";
    if (! -e $relative_path) {
      msg ("$WARNING Missing $relative_path. No diff was done.");
      return;
    }

    if ($patch_file eq "") {
      # if don't have patch file yet. Pehaps we running diff for a first time.
      $patch_file = $PATCH_DIR . $relative_path . ".$PATCH_EXT";
      dbg ("patch_file is empty. New patch_file is $patch_file");
      my $patch_dir = get_dir_name ($patch_file);
      if (! -e $patch_dir) {
        mkpath ($patch_dir) or die "Couldn't create $patch_dir: $@";
      }
    }
    $patch_file =~ s/^([A-Za-z])\:$FS//g;
    my $result_file;
    ($result_file = $patch_file) =~ s/\.$PATCH_EXT$//;
    my $esc_path;
    ($esc_path = $relative_path) =~ s/([\\\/\.])/\\$1/g;
    my $ignore_match = "\"^[\\*-][\\*-][\\*-] $esc_path\"";
    my $tmp_file = $patch_dir . $BACKUP_PREFIX .
                      File::Temp::mktemp (get_fname ($patch_file) . "XXXXX");
    my $cmd = $REDIFF_CMD . " $relative_orig $relative_path";
    my $res = 0;
    my $derr = 1;

    if (-e "$relative_orig") {
      $res = 1;
      dbg ("working on $relative_path");
      run ($cmd, $tmp_file);
      if (-e $patch_file) {
        # Let's see if we got a different patch file that we already have.
        # We don't want to just replace patch file we have with new one
        # because source save might be too picky about changing time stamp
        # and if new and old patch file are equal we will keep the old one.
        $res = run ("$DIFF_EXE -N -I $ignore_match $tmp_file $patch_file",
                    $NULL_NAME);
      }

      if ($res == 2) {
        die "$DIED_MSG Couldn't diff $tmp_file and $patch_file\n";
      }
      elsif ($res == 1) {
          # New patch file is differerent so replace old patch file
          # with new one.
          msg ("Diffing $relative_path");
          chmod 666, $patch_file;
          rename "$tmp_file", "$patch_file";
      }
      else {
        unlink $tmp_file;
      }
    }

    # check, if resulted .diff-file is zero-size. If so, abort.
    # (either patch is obsolete or you manually messed things up)
    if (-e $patch_file && not -s "$patch_file") {
        die "$DIED_MSG $patch_file size is 0!\nCheck the patch!\n";
    }

    $derr = run ("$DIFF_EXE $relative_path $result_file", $NULL_NAME);
    dbg ("derr: $derr, res: $res");
    if ($res == 1 || $derr > 0) {
      # Patch files are different so the result file has to be updated.
      if (-e "$relative_orig") {
        # This is normal condition, no need to display this message
        # unless verbose output is on
        dbg ("Copying $relative_path to $result_file");
      } else {
        # Or we don't have .orig file, so we just want to copy plat. file
        # to patch dir. In that case issue a normal message
        msg ("Copying $relative_path to $result_file");
      }
      copy ($relative_path, $result_file);
      chmod 666, $result_file;
    }
    else {
      msg ("Checking $relative_path ...");
    }
  }


# =======================================================================
  sub patch_files
# =======================================================================
  {
    my $path = shift;
    my $patch_file = shift;

    # remove read-only bits from all the files in the dir of the file-to-be-patched
    my $tmp_dir = get_dir_name ($path);
    my $cmd = "attrib -r /s /d $tmp_dir*.*";
    dbg($cmd);
    system($cmd);

    my ($p_err, $d_err) = do_patch ($path, $patch_file);
    my $res_file;

    ($res_file = $patch_file) =~ s/\.$PATCH_EXT$//;

    if ($p_err == 0) {
      msg ("Patched $path");
      if ($d_err == 2) {
        die "$DIED_MSG PATCH RESULT: Diff failed while comparing $path and $res_file\n";
      }
      elsif ($d_err == 1) {
       msg ("$WARNING PATCH RESULT: $path does not match $res_file\n");
      }
      else {
        dbg ("PATCH RESULT: res $p_err, diff res: $d_err. Proceeding");
      }

      # additionally check, if .diff-file is zero-size. If so, print a note.
      # (either patch is obsolete or you manually messed things up)
      # note, that we still continue.
      if (not -s "$patch_file") {
          msg ("$NOTE $patch_file size is 0!\nCheck the patch!\n");
      }
    }
    elsif ($p_err == -1) {
      die "$DIED_MSG PATCH FAILED: Couldn't start the patch command!\n";
    }
    else {
      die "$DIED_MSG PATCH FAILED for $path!\n";
    }
  }


# =======================================================================
  sub chdir_to_build_specific_dir
# =======================================================================
  {
    my $dir;
    if ($BUILD_TYPE == $BUILD_TYPE_MIDP) {
      $dir = "midp";
    } elsif ($BUILD_TYPE == $BUILD_TYPE_CPP) {
      $dir = "cpp";
    } else {
      return;
    }

    if (-f "$dir/bld.inf") {
      chdir $dir;
    }
  }

# =======================================================================
  sub do_platform_build
# =======================================================================
  {
    # Build patched platform modules
    foreach $b (@bld_info) {
      my @modules = split /,/, $b;
      my $group_dir = shift @modules;
      chdir_or_die ("$BUILD_ROOT$group_dir");
      dbg ("Building in $BUILD_ROOT$group_dir");
      run ("cmd /c bldmake.bat bldfiles");

      #modules are optional
      if(@modules>0)
      {
	      foreach $m (@modules)
	      {
	        build_module ($m);
	      }
	    }
	    else
	    {
	    	build_module('');
	    }
    }
  }
 
# =======================================================================
  sub do_java_platform_build
# =======================================================================
  {
    # Build java system ams
  		my $java_group_dir = "sf/app/java/group";
      chdir_or_die ("$BUILD_ROOT$java_group_dir");
      dbg ("Building in $BUILD_ROOT$java_group_dir");
      run ("cmd /c bldmake.bat bldfiles");
      run ("cmd /c abld.bat export");
      run ("cmd /c pbuild build winscw udeb midp2ams");
      run ("cmd /c pbuild build winscw udeb wma");
      run ("cmd /c pbuild build winscw udeb multimedia11");
			
			my $s60ex_dir = $BUILD_ROOT . "$SDK_PROJ_NAME/sdkexamples/javaexamples";
			chdir_or_die ($s60ex_dir);
			
			run_build_cmd("ant IAPInfoMIDlet eSWTMIDlet SystemProperties");
			run_build_cmd("ant copy_documentation");
		  $cmd = "unzip -o \\\\filerblr\\SP\\AM\\DT_SDK\\SDK_BIS\\Repository\\Java_API\\IAPInfoMIDlet_doc.zip -d Z\:\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples\\dist\\examples\\IAPInfoMIDlet";
      print ($cmd."\n"); system ($cmd);
      $cmd = "unzip -o \\\\filerblr\\SP\\AM\\DT_SDK\\SDK_BIS\\Repository\\Java_API\\eSWTMIDlet_doc.zip -d Z\:\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples\\dist\\examples\\eSWTMIDlet";
      print ($cmd."\n"); system ($cmd);
      $cmd = "unzip -o \\\\filerblr\\SP\\AM\\DT_SDK\\SDK_BIS\\Repository\\Java_API\\SystemPropertiesMidlet_doc.zip -d Z\:\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples\\dist\\examples\\SystemProperties";
      print ($cmd."\n"); system ($cmd);
      system("copy /V Z\:\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples\\examples\\examples.html Z\:\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples\\dist\\examples");
      system("copy /V \\\\filerblr\\SP\\AM\\DT_SDK\\SDK_BIS\\Repository\\Java_API\\build.xml Z\:\\PublicSDK\\sdkcreationmw\\sdkexamples\\javaexamples\\dist\\examples\\IAPInfoMIDlet\\targets");
  }

# =======================================================================
  sub check_platform_build
# =======================================================================
  {
    foreach $b (@bld_info) {
      my @modules = split /,/, $b;
      my $group_dir = shift @modules;
      if (!chdir ("$BUILD_ROOT$group_dir")) {
        msg ("$DIED_MSG missing $BUILD_ROOT$group_dir!!!!!!!!");
        return;
      };
      #modules are optional
      if(@modules>0)
      {
	      foreach $m (@modules)
	      {
          check_module_build ($m);
	      }
	    }
	    else
	    {
        check_module_build ('');
	    }

    }
  }


# =======================================================================
  sub check_module_build
# =======================================================================
  {
    my $dir = getcwd;
    my $module = shift;
    dbg ("Checking $dir");
    foreach $t (@PLATFORMS) {
      if (!-e "ABLD.BAT") {
        run ("cmd /c bldmake.bat bldfiles");
      }
      run ("cmd /c abld.bat build -c $t $module");
    }
  }


# =======================================================================
  sub build_series60_tools_component
# =======================================================================
  {
    my $name = shift;
    my $dir = "$S60_TOOLS_DIR/$name/group";
    chdir_or_die ($dir);
    chdir_to_build_specific_dir ();
    dbg ("Building in " . getcwd);
    run ("cmd /c bldmake.bat bldfiles");
    run ("cmd /c abld.bat export");
    run ("cmd /c abld.bat makefile");
    foreach $bt (@VARIANTS) {
      if($REBUILD || $CLEAN) {
        run_build_cmd ( "cmd /c abld.bat clean $bt");
      }
      if (!$CLEAN) {
        run_build_cmd ( "cmd /c abld.bat -k build $bt");
      }
    }
  }


# =======================================================================
  sub check_series60_tools_component
# =======================================================================
  {
    my $name = shift;
    my $dir = "$S60_TOOLS_DIR/$name/group";
    if (!chdir ($dir)) {
      msg ("$DIED_MSG Missing $dir!!!!!!!!");
      return;
    };
    chdir_to_build_specific_dir ();
    dbg ("Checking " . getcwd);
    if (!-e "ABLD.BAT") {
      run ("cmd /c bldmake.bat bldfiles");
    }
    run ("cmd /c abld.bat build -c");
  }


# =======================================================================
  sub build_module
# =======================================================================
  {
    my $module = shift;

    foreach $t (@PLATFORMS) {
      if (!$CLEAN) 
      {
        run_build_cmd ( "cmd /c abld.bat makefile $t $module");
      }
      foreach $bt (@VARIANTS) 
      {
        if($REBUILD || $CLEAN) 
        {
          run_build_cmd ( "cmd /c abld.bat clean $t $bt $module");
        }
        if (!$CLEAN) 
        {
          	run_build_cmd ( "cmd /c abld.bat -k build $t $bt $module");
        }
      }
    }
  }


# =======================================================================
  sub do_patch
# =======================================================================
  {
    my $dest = shift;
    my $patch_file = shift;
    my $res_file;
    my $perr = 0;
    my $derr = 0;
    my $silent = ($LOGLEVEL > $LOG_NORMAL) ? "" : "-s";

    ($res_file = $patch_file) =~ s/\.diff$//;

    # Overwrite the destination file with the .orig file. If .orig file
    # is not found, assume that it's the very first patch and create the
    # .orig file
    if (-e "$dest.orig") {
      copy_or_die ("$dest.orig", "$dest");
    } else {
      dbg ("creating new orig file $dest.orig");
      copy_or_die ("$dest", "$dest.orig");
    }

    $perr = run ("$PATCH_EXE $silent -d $BUILD_ROOT -p0 -N --ignore-whitespace --no-backup-if-mismatch -i $patch_file $dest");
    dbg ("PATCH: error $perr");

    if ($perr == 0) {
      if ($LOGLEVEL >= $LOG_VERBOSE) {
        $derr = run ("$DIFF_EXE $dest $res_file");
      }
      else {
        $derr = run ("$DIFF_EXE $dest $res_file", $NULL_NAME);
      }
      dbg ("Diff for $dest and $res_file returned $derr");
    }
    return ($perr, $derr);
  }


# =======================================================================
  sub do_unpatch
# =======================================================================
  {
    my $dest = shift;
    my $patch_file = shift;
    my $backup_path = get_first_backup_path($dest);
    my $res_file = $patch_file;
    $res_file =~ s/\.$PATCH_EXT$//;

    # try to restore the original version
    if (-e "$dest.orig") {
      # there is a "orig file", so just rename "orig" back to $dest
      msg("restoring $dest from \".orig\"...");
      copy_or_die("$dest.orig", "$dest");
    }
    elsif (-e "$backup_path") {
        # there is no ".orig" file but there is backup file,
        # restore from there.
        msg("Restoring $dest from backup file.");
        copy_or_die("$backup_path", "$dest");
    }
    elsif (!-e $patch_file &&
           !run("$DIFF_EXE $dest $res_file", $NULL_NAME)) {
        # there are no .orig and backup files and the result
        # file ($res_file) is the same as in the platform ($dest).
        # This is most likely the file that was copied to the platform,
        # so remove it.
        msg("Removing current copy of $dest");
        unlink $dest;
    }

  }


# =======================================================================
  sub process_bld_info
# =======================================================================
  {
    my $file = shift;
    my $idx = 0;

    foreach $b (@_) {
      @bld_data = split /,/, $b;
      $bld_inf_path = shift @bld_data;

      die "$DIED_MSG Dir. name for bld.inf is empty. Line: $b\n" if $bld_inf_path eq "";

      # Get the index of the $bld_info element that starts with $bld_inf_path.
      $idx = bld_info_arr_idx ($bld_inf_path);

      if ($idx < 0) {
        # There is no such entry in the $bld_info, so just add entire line.
        push (@bld_info, $b);
      } else {
        # There is an entry that starts with $bld_inf_path, so just add
        # modules that are not there to the $idx-th line (element of $bld_info)
        @bld_data_idx = split /,/, $bld_info[$idx];
        shift @bld_data_idx;

        foreach $m (@bld_data) {
          $bld_info[$idx] =~ s/\,$m\,/\,/g;
          $bld_info[$idx] =~ s/\,$m$//g;
          $bld_info[$idx] .= ",$m";
        }
      }
    }
  }


# =======================================================================
  sub bld_info_arr_idx
# =======================================================================
  {
    my $val = shift;
    my $idx = 0;

    for ($idx = 0; $idx < $#bld_info + 1; $idx++) {
      last if ($bld_info[$idx] =~ m/^$val\,/);
    }

    if ($idx ==  $#bld_info + 1) {$idx = -1;}
    return $idx;
  }


# =======================================================================
  sub get_last_backup_path
# =======================================================================
  {
    my $orig_path = shift;
    my $new_ver = get_last_file_ver ($orig_path);
    my $dir = get_dir_name ($orig_path);
    my $fname = get_fname ($orig_path);
    my $backup_path = $dir . $BACKUP_PREFIX . $fname . ".$new_ver";
    return $backup_path;
  }

# =======================================================================
  sub get_first_backup_path
# =======================================================================
  {
    my $orig_path = shift;
    my $dir = get_dir_name ($orig_path);
    my $fname = get_fname ($orig_path);
    my $backup_path = $dir . $BACKUP_PREFIX . $fname . ".1";
    return $backup_path;
  }


# =======================================================================
  sub get_new_backup_path
# =======================================================================
  {
    my $orig_path = shift;
    my $new_ver = get_last_file_ver ($orig_path) + 1;
    my $dir = get_dir_name ($orig_path);
    my $fname = get_fname ($orig_path);
    my $backup_path = $dir . $BACKUP_PREFIX . $fname . ".$new_ver";
    return $backup_path;
  }


# =======================================================================
  sub get_last_file_ver
# =======================================================================
# Assupmtion: if file name is "foo.cpp" the n-th version of this file would be
# in the same directory and would have name "#foo.cpp.n"
  {
    my $path = shift;
    my $dir = get_dir_name ($path);
    my $fname = get_fname ($path);
    my $basename = get_basename ($fname);
    my $postfix = 0;

    if ($dir eq "") {$dir = "."}
    opendir (DIR, $dir);
    @flist = grep {/^$BACKUP_PREFIX$basename.*\..*\.[0-9]+$/} readdir (DIR);
    closedir DIR;

    if ($#flist + 1 > 0) {
      foreach $f (@flist) {
        my $p = unpack ("A*", get_ext ($f));
        $postfix = $p if $postfix < $p;
      }
    }
    return $postfix;
  }


# ========================================================================
  sub get_basename
# ========================================================================
  {
    my $orig_name = shift;
    my $basename;
    ($basename = $orig_name) =~ s/\..*$//g;
    return $basename;
  }


# ========================================================================
  sub get_ext
# ========================================================================
  {
    my $orig_name = shift;
    my $ext;
    ($ext = $orig_name) =~ s/^.*\.//g;
    return $ext;
  }


# =======================================================================
  sub get_fname
# =======================================================================
  {
    my $orig_name = shift;
    my $dir = get_dir_name ($orig_name);
    my $fname;
    ($fname = $orig_name) =~ s/\\/\//g;
    $fname =~ s/^$dir//g;
    return $fname;
  }


# =======================================================================
  sub get_dir_name
# =======================================================================
  {
    my $orig_name = shift;
    my $dir_name;
    ($dir_name = $orig_name) =~ s/\\/\//g;
    $dir_name =~ s/[^\/]+$//g;
    return $dir_name;
  }


# =======================================================================
  sub err
# =======================================================================/
  {
    my $m = "ERROR: @_[0]\n";
    print $m;
    print LOG_FH $m if $LOG;
    exit 1;
  }


# =======================================================================
  sub quiet
# =======================================================================/
  {
    if ($LOGLEVEL => $LOG_QUIET) {
      my $m = "*** @_[0]\n";
      print $m;
      print LOG_FH $m if $LOG;
    }
  }


# =======================================================================
  sub msg
# =======================================================================/
  {
    if ($LOGLEVEL >= $LOG_NORMAL) {
      my $m = "*** @_[0]\n";
      print $m;
      print LOG_FH $m if $LOG;
    }
  }


# =======================================================================
  sub dbg
# =======================================================================
  {
    if ($LOGLEVEL >= $LOG_VERBOSE) {
      my $dbg_msg = "=== @_[0]\n";
      print $dbg_msg;
      print LOG_FH $dbg_msg if $LOG;
    }
  }


# =======================================================================
  sub noisy
# =======================================================================
  {
    if ($LOGLEVEL > $LOG_VERBOSE) {
      my $m = "=== @_[0]\n";
      print $m;
      print LOG_FH $m if $LOG;
    }
  }


# =======================================================================
  sub run_build_cmd
# =======================================================================
  {
    my $cmd = shift;
    if ($LOGLEVEL >= $LOG_NORMAL) {
      my $dir = getcwd;
      msg ("***********************************************************");
      msg ("* DIR:   $dir");
      msg ("* BUILD: $cmd");
      msg ("***********************************************************");
    } else {
      msg ($cmd);
    }
    my $err = run ($cmd);
    dbg ("$cmd returned $err");
    return $err;
  }


# =======================================================================
  sub run
# =======================================================================
  {
    my $cmd = shift;
    my $out_name = shift;
    my $close_out = 0;
    my $out;

    dbg ("Running: $cmd");
    if (defined($out_name)) {
      if ($out_name eq $NULL_NAME) {
        # no output desired
        undef $out;
      } else {
        open OUT, "> $out_name" or
          die "$DIED_MSG $cmd couldn't write to output $out_name!!!\n";
        $out = OUT;
        $close_out = 1;
      }
    } elsif ($LOGLEVEL <= $LOG_SILENT) {
      # output supressed by log level
      undef $out;
    } else {
      $out = STDOUT;
    }

    open CHILD, "$cmd 2>&1|" or die "$DIED_MSG $cmd Err: $!";
    while (<CHILD>) {
      if (defined ($out)) {
        print $out $_;
        print LOG_FH $_ if $LOG;
      }
    }
    close CHILD;
    close $out if $close_out;
    noisy ("return code " . ($? >> 8));
    return $? >> 8;
  }


# =======================================================================
# Changes the current direcctory, dies on error
# =======================================================================
  sub chdir_or_die
# =======================================================================
  {
    my $dir = shift;
    dbg ("chdir $dir");
    chdir $dir or die "$DIED_MSG $dir: $!";
  }


# =======================================================================
# Copies a file, dies on error
# =======================================================================
  sub copy_or_die
# =======================================================================
  {
    my $src = shift;
    my $dest = shift;
    dbg ("copying $src -> $dest");
    #if the file exists, rename it to .orig_X - safer than overwriting
    #note: if we neither rename nor remove existing $dest, copy gives
    #fatal error
    if(-e $dest)
    {
    	my $i = 1;
    	while(-e "$dest.orig_$i")
    	{
    		$i++;
    	}
    	move($dest, "$dest.orig_$i");
    }
    copy ($src, $dest) or
      die "$DIED_MSG : failed to copy $src -> $dest: $!";
  }



# =======================================================================
#
# Recursive copy
#
# Copies a folder, its sub folders & files therein.
# Paramter 1: Source folder path.
# Paramter 2: Destination folder path.
#
# =======================================================================
  sub xcopy
# =======================================================================
  {
    my ($CopyFrom,$CopyTo) = @_;
    my ($src,$RelativePath,$dest);

    # Make sure that the source directory exists, create the top level
    # destination directory
    if (-d $CopyFrom) {
      eval { mkpath($CopyTo) };
      if ($@) {
        err("Couldn't create $_[0]: $@");
        exit(1);
      }
    } else {
      err("Directory $CopyFrom does not exist");
      return;
    }

    dbg("Copying $CopyFrom -> $CopyTo");

    # Traverse file tree (highest level directory first)
    find(sub{
      $src=$File::Find::name;
      $RelativePath=$src;
      $RelativePath=~s/^\Q$CopyFrom\E//;
      $dest=$CopyTo.$RelativePath;
      if (-f $src) {  # Copy file
        noisy ("$dest");
        if (!copy($src,$dest)) {
          err("Couldn't copy $src: $@");
        }
      } else {  # Make a duplicate directory if necessary
        unless($dest eq '..'||$dest eq '.') {
          unless(-e $dest) {
            noisy ("Creating $dest");
            mkdir($dest,0775);
          }
        }
      }
    },$CopyFrom);
  }


# ========================================================================
  sub get_patch_fname
# ========================================================================
  {
    my $patch_name = shift;
    my $res;

    foreach $f (@_) {
      if ($f =~ m/$patch_name$/i) {
        $res = $f;
        last;
      }
    }
    return $res;
  }


# ========================================================================
  sub find_all
# ========================================================================
  {
    my $base_dir = shift;
    my @flist = ();

    find (sub {
      if (-f $File::Find::name) {
        push (@flist, $File::Find::name);
      }
    }, $base_dir);

    return @flist
  }

# ========================================================================
  sub get_plat_name
# ========================================================================
  {
    return "3.0";
  }

# ========================================================================
  sub do_s60ex_finalizing
# ========================================================================
  {
    my $s60ex_dir = $BUILD_ROOT . "$SDK_PROJ_NAME/ExamplesCpp/build";
    chdir_or_die ($s60ex_dir);
    run ("cmd /c finalize.bat");
  }

# ========================================================================
sub do_s60_rndtools_cleaning
# ========================================================================
  {
  	# List of tools that needs to be make cleaned to remove "rnd tools" from emulator menu
    print "\n \n ======== Start Clean RnD Tools===========";
    @rndtools_cleandirs = qw ( s:/s60/tools/commontools/group s:/s60/tools/performancetools/group s:/s60/tools/nettools/ConnTest/group s:/ppd_sw/rd_sw/prism_rnd/group  s:/ppd_sw/rd_sw/btproxy_rnd/group  s:/ppd_sw/rd_sw/bappeaprofiler/group  s:/ppd_sw/rd_sw/analyzetool/group  s:/ppd_sw/rd_sw/hti/src/HtiAdmin/group  s:/ppd_sw/rd_sw/hti/src/HtiAutoStart/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiBtCommPlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiIPCommPlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiIsaCommPlugin/HtiIsaCommEcomPlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiIsaCommPlugin/HtiIsaCommServer/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiSerialCommPlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiTraceCommPlugin/HtiTraceCommEcomPlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiTraceCommPlugin/HtiTraceCommServer/group  s:/ppd_sw/rd_sw/hti/src/HtiCommPlugins/HtiUsbSerialCommPlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiDeviceReboot  s:/ppd_sw/rd_sw/hti/src/HtiFileHlp  s:/ppd_sw/rd_sw/hti/src/HtiFramework/group  s:/ppd_sw/rd_sw/hti/src/HtiWatchDog  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiAppServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiAudioServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiEchoServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiFtpServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiIpProxyServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiIsiMsgServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiKeyEventServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiMessagesServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiPIMServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiScreenshotServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiStifTfServicePlugin/group  s:/ppd_sw/rd_sw/hti/src/HtiServicePlugins/HtiSysInfoServicePlugin/group  s:/ppd_sw/cs_salo_domain/aiemailplugin/group s:/s60/mw/securityservices/iaupdate/IAD/group );

    # Make clean to remove "rnd tools" from emulator: suggestion given by pf team
    foreach my $rndtools_cleandirs ( @rndtools_cleandirs )
    {     
      print "\n\n=== Clean up of rnd tools: $rndtools_cleandirs\n\n ===";
      chdir $rndtools_cleandirs or print  " WARNING: Can chdir to $rndtools_cleandirs: $!";
    
      $cmd = "call bldmake bldfiles";
      system ($cmd); #==0 or print ($cmd."\n\n");

      $cmd = "call abld makefile";
      system ($cmd); #==0 or print ($cmd."\n\n");

      $cmd = "call abld reallyclean";
      system ($cmd); #==0 or print ($cmd."\n\n");
    }
  }
