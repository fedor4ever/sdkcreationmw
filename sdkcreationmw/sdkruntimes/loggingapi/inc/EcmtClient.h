/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef ECMTCLIENT_H
#define ECMTCLIENT_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEcmtServerListener;
class MEcmtServerObserver;

// CONSTANTS
/**
* Formatted message maximum length
*/
const TInt KFormatMaxSize = 256;

/**
*  A handle to a session with Ecmt server.
*  This class provides methods for sending textual data to the 
*  System Output Diagnostic view on the connected PC.
*  @ingroup EcmtClient
*/
class REcmt : public RSessionBase
    {
    public: // New functions
    
    	/**
    	* Constructor.
    	*/
    	IMPORT_C REcmt();
    	
    	/**
    	* Destructor.
    	*/
    	IMPORT_C ~REcmt();
    	
    	/**
    	* Connects the client process to the Ecmt server. To end the session, 
    	* use the Close() method.
    	* @return KErrNone if succesfull otherwise one of the system-wide error codes.
    	*/
    	IMPORT_C TInt Connect( );
    	
    	/**
    	* Closes the session and frees any allocated resources.
    	*/
    	IMPORT_C void Close( );
    	
		/**
		* This member is internal and not intended for use.
      * This member leaves immediately with KErrNotSupported.
      * @deprecated 
		*/
      IMPORT_C void SetServerObserverL( MEcmtServerObserver* aObserver );

    	/**
    	* Writes text data to the System Output Diagnostic view on the connected PC.
    	* @param aText Text to be written.
    	*/
    	IMPORT_C void Write( const TDesC16& aText );

    	/**
    	* Writes text data to the System Output Diagnostic view on the connected PC.
    	* The text can include formatting directives (\%d, \%c, ...) which are then filled
    	* with the following parameters. The maximum length of the message after formatting
    	* is defined by KFormatMazSize.
    	* @param aText Text to be written.
    	* @param ... Values used for formatting.
    	*/
		IMPORT_C void WriteFormat(TRefByValue<const TDesC16> aFmt,...);

    	/**
		* This member is internal and not intended for use.
      *
      * Sends a binary message to the specified UID (plugin, core).
    	*/
    	IMPORT_C void Send( const TUint uid, const TDesC8& aText );

		/**
		* This member is internal and not intended for use.
      *
      * Sets this client as a recipient for messages.
		*/
      IMPORT_C void SetServerObserverL( TUid aUid, MEcmtServerObserver* aObserver );

    	/**
    	* Writes text data to the System Output Diagnostic view on the connected PC.
	* Unlike Write(), doesn't require Connect() before or Close() after calling this.
    	* @param aText Text to be written.
    	*/
    	IMPORT_C static void SWrite( const TDesC16& aText );

    	/**
    	* Writes text data to the System Output Diagnostic view on the connected PC.
    	* The text can include formatting directives (\%d, \%c, ...) which are then filled
    	* with the following parameters. The maximum length of the message after formatting
    	* is defined by KFormatMazSize.
	* Unlike WriteFormat(), doesn't require Connect() before or Close() after calling this.
    	* @param aText Text to be written.
    	* @param ... Values used for formatting.
    	*/
	IMPORT_C static void SWriteFormat(TRefByValue<const TDesC16> aFmt,...);

	/**
	 * Closes the REcmt session used by SWrite() and
	 * SWriteFormat(). If those funcions are called, also SClose()
	 * must be called in order to avoid memory allocation errors.
	 */
	IMPORT_C static void SClose();

	private:
	static REcmt* GetServer();

      /**
       * Requests a message.
       */
    	void RequestServerNotification( TDes8& aDes, TRequestStatus& aStatus );

      /**
       * Cancels the message request.
       */
    	void CancelRequestServerNotification( );
	
   	private:
		MEcmtServerObserver*	iObserver;
		CEcmtServerListener* 	iServerListener;
		friend class CEcmtServerListener;
    };
#endif
// End of File
