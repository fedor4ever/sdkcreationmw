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

#ifndef SMSFILEHANDLER_H
#define SMSFILEHANDLER_H

//sms messaging
#include <Gsmumsg.h>		// CSmsMessage
#include <gsmubuf.h>		// CSmsBuffer
#include <gsmuset.h> 		// For TSmsUserDataSettings
#include <smsuaddr.h> 		// For TSmsServiceCenterAddress

#include "message.h"		//CMessage
#include "MsgRelay.h"		//CMessageWriter

_LIT(KSmsInboxLocation, "c:\\smsout\\"); //NCI inbox is Emulator outbox
_LIT(KSmsFileFilter, "*.sms");

_LIT(KSmsOutboxPath, "c:\\smsin\\");	 //NCI outbox is Emulator inbox
_LIT(KMessageNamePart, "message");
_LIT(KMessageNameExt, ".sms");

/**
*   CSmsFileHandler class for handling SmsMessage writing to and reading
*   from a file.
*/
class CSmsFileHandler : public CMessageWriter
{

protected:
	/**
	* Basic C++ Constructor
    */
	CSmsFileHandler();

	/**
	* Symbian 2nd phase constructor
    */
	void ConstructL();

	/**
	* This protected method creates unique file name for the SMS-message into
	* the specified directory.
	*
	* @param RFs &aFs is a reference to the File server session to allow file
	*        deleting if aLeaveFile parameter is EFalse,
	* @param const TDesC &aDir reference to the directory wherein the will be
	*        created.
	* @param const TDesC &aStaticPart reference to the static part of
	*        a filename
	* @param TFileName &aFileName holds the refrence to the unique file name.
	*		 This value holds the filename after function returns
	* @param const TDesC &aFileExt holds the reference to the file extension
	* @param TBool aLeaveFile holds the switch if the unique filename will be
	*        actually created by the method.
	*
    */
	void CreateUniqueFileNameL(RFs &aFs,
		const TDesC &aDir,
		const TDesC &aStaticPart,
		TFileName &aFileName,
		const TDesC &aFileExt,
		TBool aLeaveFile = ETrue);

	/**
	* This protected method Creates CSmsMessage from the CMessage class
	*
	* @param CMessage &msg hold a reference to the CMessage class that holds
	*		 the Address, Type, Timestamp and Payload data
	*
	* @return Method returns a pointer to the CSmsMessage object that is
	*         initialized according to the msg parameter data.
	*
	*/
	CSmsMessage *CreateMessageL(CMessage &msg);

	/**
	* This protected method isBinary checks if the CMessage type is
	* BINARY_MESSAGE
	*
	* @param CMessage &msg hold a reference to the CMessage class that holds
	*		 the Address, Type, Timestamp and Payload data
	*
	* @return Method returns TBool value ETrue if type is binary othervice
	*		  EFalse is returned.
	*/
	TBool isBinary(CMessage& msg);

	/**
	* This protected method is filename-scanner scan-once type of method used
	* for getting the sms-file from the msgout directory. File to be get should
	* be the oldest message. E.g. message0.sms, message1.sms, ... and so on.
	*
	* @param TFileName &fileName is a reference to the filename data type.
	*
	* @return Method returns KErrNone if file is found, Other vice file leaves
	*         with on of system wide error codes. Usually it is KErrNotFound,
	* 		  meaning that the there where no sms messages in msgout directory.
	*/
	TInt GetFileL(TFileName &fileName);

public:
	/**
	* Symbian 2nd phase constructor
    */
	static CSmsFileHandler *NewL();
	/**
	* Symbian 2nd phase constructor
    */
	static CSmsFileHandler *NewLC();

	/**
	* Basic C++ destructor
    */
	~CSmsFileHandler();

	/**
	* This public method Writes CMessage class data to the filebased sms-message
	* in c:\msgin directory.
	*
	* @param CMessage &msg holds Address, type, timestamp and payload data for
	*        the CSmsMessage. If msg equals NULL method leaves with KErrNotReady.
	* @param TMessageType aType hold information about the message type to be
	*        created. This value must be ESmsMessage. Other vice method leaves
	*        with KErrNotSupported.
	*
	*/
	void WriteMessageL(CMessage &msg, TMessageType aType);

	/**
	* This public method is reads a filebased sms-message from the c:\msgout
	* directory to the CMessage object
	*
	* @param CMessage &msg is a reference to the message object and hold the address,
	*        type, timestamp and payload data if the method success.
	*
	* This method leaved with KErrNotFound if there are no messages in outbox. On
	* other errors this method will panic.
	*
	*/
	void ReadMessageL(CMessage &msg);
private:
	RFs iFs;
};

#endif SMSFILEHANDLER_H