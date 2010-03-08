/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef HEADER_SWI_INSTALL_TASK
#define HEADER_SWI_INSTALL_TASK

#include "SwiTask.h"
#include "ifeui.h"
#include "SWInstApi.h"
#include <ecom/implementationproxy.h>


using namespace Java::Installer::Ife;
typedef CIfeUi* (*NEWLP)();

const TUint32 KDefaultIAP = 0;

_LIT(KEmpty16,"");
_LIT8(KEmpty,"");
_LIT8(KDefaultJadMimeType, "text/vnd.sun.j2me.app-descriptor");
_LIT8(KDefaultJarMimeType, "application/java-archive");

//class CIfeUi;

class CSwiInstallTask : public CSwiTask
	{
public:
	CSwiInstallTask(
		CDriver* aDriver, 
		CPassiveLogger* aLogger,
		HBufC* aJad,
		HBufC* aJar
		);
		
	~CSwiInstallTask();

	void DoTaskL(void);
	void DoTaskCleanupL(void);

public:
	HBufC* 				iJad;//Jad File Name
	HBufC* 				iJar;//Jar File Name
private:
		RLibrary iIfeLib;
		Java::Installer::Ife::CIfeUi* iIfe;
		RFile iJadFile;
		RFs iFs;
		NEWLP iNewLp;
		TUid iInstallPkgUid;
		
		RSWInstSilentLauncher iInstaller;
		TInstallOptions iInstallOptions;
		TInstallOptionsPckg iInstallPckg;
		RTimer iTimer;

	

	};

#endif

