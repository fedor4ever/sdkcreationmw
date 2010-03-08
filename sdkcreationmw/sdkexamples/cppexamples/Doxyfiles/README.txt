GENERAL
=======

  doxy.pl is a perl script which generates doxygen documentation from all mainpages 
  which are found recursively from root_path subfolders. 
  It accepts configuration filename as an argumnet.
  
PREREQUISITIES
==============

  install doxygen
  install active state perl
  configure configuration file (e.g. doxy.conf)
  run doxy.pl from directory where it is located: perl doxy.pl doxy.conf

TECHNIQUES
==========
  - reads configuration from file
  - updates footer-file path and name to doxyfile and creates .ftr backupfile.
  - finds recursively all doc-folders from root_path subfolders where mainpage is.
  - updates class hierarchy to mainpage from hierarchy.html if hierarchy.html is
    not found it uses annotated.html 

CONFIGURATION FILE
===================

  doxy.pl needs configuration file to work.
  Configuration file is a ASCII-text file, which consits of "key=value" pairs:
  Example:

  document_dir=doc
  mainpage=Mainpage.dox
  doxyfile=F:\ex\doxyfiles\Doxyfile
  doxy_command=doxygen
  root_path=F:\ex\Series60Ex_doced_11042005
  footer=F:\ex\doxyfiles\footer.html

  document_dir: in which directory doxygen creates documentation
  mainpage    : file from where mainpage tag is found e.g. file to be a base of index.html
  doxyfile    : path and filename to doxygen configuration file
  doxy_command: command which is used to run doxygen. Gets doxyfile as an argument.
  root_path   : path from where start to look for mainpage recursively.
  footer      : path and filename of the footer file
  header      : path and filename of the header file
  stylesheet  : path and filename of style sheet


TIPS
====

- You can use doxy.pl with several configuration files.

- with root_path configuration you can set scope to pointing it to desired folder

- (win) You can also create several shortcuts to doxy.bat at your desktop with
  diffent configurations:

    Edit shortcut properties:
    Set      Target: [path_to_doxy.bat] [configuration_file] > [log_file] 2>&1
    e.g.     Target:  F:\ex\doxyfiles\doxy.bat doxy.conf > doxy.log 2>&1
    You can set "Run: Minimized" 
    Check that "Start in:" points to your "doxyfiles" folder !

- if there is something wrong with footer: find HTML_FOOTER tag from Doxyfile and
  check that it points to footer-file. (doxy.pl should update that automatically)








