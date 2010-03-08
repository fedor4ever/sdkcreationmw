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


#ifndef HEADER_UEI_COMMANDS
#define HEADER_UEI_COMMANDS

#include <e32base.h>
#include "Driver.h"
#include "AgentEngine.h"
#include "SocketReader.h"
#include "SocketWriter.h"
	
const TInt KInstallCommand      = 0x00000001; 
const TInt KRemoveCommand       = 0x00000002; 
const TInt KStartCommand        = 0x00000003; 
const TInt KCloseCommand        = 0x00000004; 
const TInt KListCommand         = 0x00000005; 
const TInt KGetInfoCommand      = 0x00000006; 
const TInt KGetPortCommand      = 0x00000007; 
const TInt KHandshakeCommand    = 0x00000008; 

const TInt KTestCommand         = 0x74657374; // ascii "test" for test
const TInt KQuitCommand         = 0x71756974; // ascii "quit" for quit
const TInt KTimeout				= 0x10000000;	

class CTcpServerHandler;

/**
* Base class for SeiCommands. 
*
*
*/
class CSeiCommand : public CActive
	{
public:
    enum TOperation
        {
        ENone = 0,
        ERead = 1,
        EExecute = 2,
        EWrite = 3,
        EDone = 4,
        EError = 5
        };

    // synchronous
	static CSeiCommand* CreateCommandL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter,
        CTcpServerHandler* aHandler,
        TInt aCommandCode);

    ~CSeiCommand();

public:
    // CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

public:
    // async
	void ReadL(TRequestStatus& aStatus);
	void ExecuteL(TRequestStatus& aStatus);
    void WriteL(TRequestStatus& aStatus);
    virtual TBool isFinal();

protected:
    CSeiCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);
    // called from RunL 
	virtual void DoReadL() = 0;
	virtual void DoExecuteL() = 0;
    virtual void DoWriteL() = 0;

private:
    void Complete(TInt aResult);
    void Reschedule();

protected:
    CDriver*        iDriver;
    CSocketReader*  iReader;
    CSocketWriter*  iWriter;

private:
    TInt            iOperation;
    TRequestStatus* iRequestStatus;
    };


// //
// UEI commands

class CInstallCommand : public CSeiCommand
	{
public:
    static CInstallCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CInstallCommand();
protected:
    CInstallCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();

private:
    
    void DownloadJadL();
    void DownloadJarL();
    HBufC* NextFilePathL(const TDesC& aType);

    // Data needed for DoExecuteL
    HBufC* iJadPath;
    HBufC* iJarPath;
    TBool iTrusted;
    // Data needed for DoWriteL
    TUint32 iId;
    // Other temp data
    TInt64 	iDirID;
    TInt 	iInstallationResult;
	};

class CRemoveCommand : public CSeiCommand
	{
public:
    static CRemoveCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CRemoveCommand();
protected:
    CRemoveCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();
    TBool isFinal();
    

private:
    
    // parameters needed for DoExecute
    HBufC* iVendor;
    HBufC* iName;
    TBool iFinal;

    /** @link dependency */
    /*# CAgentEngine lnkCAgentEngine; */
	};

class CStartCommand : public CSeiCommand
	{
public:
    static CStartCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CStartCommand();
protected:
    CStartCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();

private:
    
    // data needed for DoExecute / DoWriteL
    TInt    iMsId;
    TInt    iMId;
    HBufC*  iCommandLineOptions;
    TBool   iDefer;

    /** @link dependency */
    /*# CAgentEngine lnkCAgentEngine; */
	};

class CCloseCommand : public CSeiCommand
	{
public:
    static CCloseCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CCloseCommand();
protected:
    CCloseCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();

private:    
    // Data needed for DoExecute
    TInt    iMsId;
    TInt    iMId; 

    /** @link dependency */
    /*# CAgentEngine lnkCAgentEngine; */
	};


class CListCommand : public CSeiCommand
	{
public:
    static CListCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CListCommand();
protected:
    CListCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();

private:    
    // data needed for DoExecute / DoWriteL
    RArray<TUint32> iResult;

    /** @link dependency */
    /*# CAgentEngine lnkCAgentEngine; */
	};


class CGetInfoCommand : public CSeiCommand
	{
public:
    static CGetInfoCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CGetInfoCommand();
protected:
    CGetInfoCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();

private:    
    // data needed for DoExecute / DoWriteL
    TUint32 iMsId;
    HBufC8* iInfo;
    
    /** @link dependency */
    /*# CAgentEngine lnkCAgentEngine; */
	};


class CGetPortCommand : public CSeiCommand
	{
public:
    static CGetPortCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CGetPortCommand();
protected:
    CGetPortCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();

private:
    
    // parameters needed for DoExecute
	TInt iFreePortsNum;
    RArray<TInt> iFreePorts;
	};

class CHandshakeCommand : public CSeiCommand
	{
public:
    static CHandshakeCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter,
        CTcpServerHandler* aHandler);

    ~CHandshakeCommand();
protected:
    CHandshakeCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter,
        CTcpServerHandler* aHandler);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();
private:
	TInt 					iResult;
	HBufC*					iPinCode;
	CTcpServerHandler* 		iHandler;
	};


// //
// DebugAgent internal commands

class CQuitCommand : public CSeiCommand
	{
public:
    static CQuitCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CQuitCommand();
protected:
    CQuitCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();
	};


// //
// Test / Dev commands

class CTestCommand : public CSeiCommand
	{
public:
    static CTestCommand* NewL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    ~CTestCommand();
protected:
    CTestCommand(CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter);

    void ConstructL();

	void DoReadL();
	void DoExecuteL();
    void DoWriteL();
	};


#endif