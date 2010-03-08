#
# Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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

!IF "$(PLATFORM)" == "WINS" || "$(PLATFORM)" == "WINSCW"
!ERROR "Not making SIS files for WINS or WINSCW platform!"
!ENDIF

!IFNDEF EPOCROOT
EPOCROOT = \
!ENDIF

PackageDir = $(EPOCROOT)Series60SDK_dev\Series60_tools\Ecmt\sis
PackageDir3 = $(EPOCROOT)S60\SimPsyConfigurator\Install
PackageDir4 = $(EPOCROOT)Adaptation\stubs\LocationSimulationPSY10\SimulationPSY\Install
TargetDir = $(EPOCROOT)Series60Tools

!IF "$(SDK)" != "MIDP"
PackageName1 = EcmtAgent_CPP
Package1     = $(PackageDir)\$(PackageName1)
Target1      = $(TargetDir)\$(PackageName1)
!ENDIF

!IF "$(SDK)" != "CPP"
PackageName2 = EcmtAgent_MIDP
Package2     = $(PackageDir)\$(PackageName2)
Target2      = $(TargetDir)\$(PackageName2)
!ENDIF

PackageName3 = SimPsyConfigurator
Package3     = $(PackageDir3)\$(PackageName3)
Target3      = $(TargetDir)\$(PackageName3)

PackageName4 = SimulationPsy_ARMV5
Package4     = $(PackageDir4)\$(PackageName4)
Target4      = $(TargetDir)\$(PackageName4)

# Targets

RELEASABLES:
!IF "$(CFG)" == "UREL"
!IF "$(SDK)" != "MIDP"
    @echo $(Package1).SIS
    @echo $(Target1).SIS
!ENDIF
!IF "$(SDK)" != "CPP"
    @echo $(Package2).SIS
    @echo $(Target2).SIS
!ENDIF
    @echo $(Package3).SIS
    @echo $(Target3).SIS
    @echo $(Package4).SIS
    @echo $(Target4).SIS
!ENDIF

CLEAN:
!IF "$(CFG)" == "UREL"
!IF "$(SDK)" != "MIDP"
   if exist $(Package1).SIS del $(Package1).SIS
   if exist $(Target1).SIS  del $(Target1).SIS
!ENDIF
!IF "$(SDK)" != "CPP"
   if exist $(Package2).SIS del $(Package2).SIS
   if exist $(Target2).SIS  del $(Target2).SIS
!ENDIF
   if exist $(Package3).SIS del $(Package3).SIS
   if exist $(Target3).SIS  del $(Target3).SIS
   if exist $(Package4).SIS del $(Package4).SIS
   if exist $(Target4).SIS  del $(Target4).SIS
!ENDIF

BLD:
!IF "$(CFG)" == "UREL"
   if not exist $(TargetDir) md $(TargetDir)
!IF "$(SDK)" != "MIDP"
   makesis $(Package1).pkg
   if exist $(Package1).pkg copy $(Package1).SIS $(Target1).SIS
!ENDIF
!IF "$(SDK)" != "CPP"
   makesis $(Package2).pkg
   if exist $(Package2).pkg copy $(Package2).SIS $(Target2).SIS
!ENDIF
   makesis $(Package3).pkg
   if exist $(Package3).pkg copy $(Package3).SIS $(Target3).SIS
   makesis $(Package4).pkg
   if exist $(Package4).pkg copy $(Package4).SIS $(Target4).SIS
!ENDIF

MAKMAKE FINAL FREEZE LIB CLEANLIB RESOURCE SAVESPACE :
   @rem donothing
