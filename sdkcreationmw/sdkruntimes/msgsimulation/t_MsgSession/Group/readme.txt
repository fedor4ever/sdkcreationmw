-----------------------------------------------------------------------------

    readme.txt - T_MsgSession application
    
-----------------------------------------------------------------------------

This project implements T_MsgSession Symbian OS application (t_msgsession.app).
Installation package file (t_msgsession.sis) is also provided to install
T_MsgSession application in Communicator (\System\Apps\T_MsgSession directory).
Application can be started from "Extras".


Compile using following commands in DOS-prompt:

    bldmake bldfiles
    abld build wins udeb
    abld build thumb urel


Create installation package (SIS-file):

    makesis t_msgsession.pkg


Debug using Microsoft Developer Studio:

    makmake t_msgsession vc6
    msdev t_msgsession.dsw


Start emulator with "Go" (F5). When the emulator is started for
the first time, Microsoft Developer Studio prompts executable
for debug session, type "\epoc32\release\wins\udeb\epoc.exe".
Start T_MsgSession application from "Extras".


