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


#ifndef _MSGSVR_H_
#define _MSGSVR_H_

#include <e32base.h>
#include <e32std.h>

#include "msgcommandconnection.h" // CMsgCommandListener, MMsgCommandObserver

class CMessage;
class CSmsFileHandler;
class CMsgSvrEng;

/**
* Main class of the NCI message server.
* Process commands notified by MMsgObserver
* 
*/
class CMsgSvr : 
    public MMsgCommandObserver,
    public CBase 
    {
    public:
       /**
       * Basic C++ Constructor
       */
	   CMsgSvr();
	   /**
       * Basic C++ Destructor
       */
       virtual ~CMsgSvr();
       /**
       * Second Phase Constructor ConstructL().
       */
	   void ConstructL();
       /**
       * Second Phase Constructor NewL(). 
       * @return CMsgSvr*
       */
	   static CMsgSvr* NewL();
       /**
       * Second Phase Constructor NewLC().
       * @return CMsgSvr*
       */
	   static CMsgSvr* NewLC();
       /**
       * Processes Send Command 
       * @param  aMsg constains message data to send
       */
	   void ProcessSendL(CMessage &aMsg) const;
       /**
       * Processes Receive Command 
       */
	   void ProcessReceiveL(CMessage &aMsg);
       /**
       * Writes text to console 
       */
       void WriteToConsole(const TDesC &aText);
    private:
       /**
       * Prints message to console
       * @param  aMsg constains message data print
       */
	   void PrintMessage(CMessage &aMsg) const;
       /**
       * Prints Horizontal line to console 
       */
       void HorizontalLine() const;
    private:
	   CMsgSvrEng* iSvrEngine;
	   CSmsFileHandler* iSmsFileHandler;
	   
};

#endif //_MSGSVR_H