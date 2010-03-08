/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application Engine class, CAddressBookEngine - 
*				 performs engine related operations
*
*/


#ifndef ADDRESSBOOKENGINE_H
#define ADDRESSBOOKENGINE_H

// INCLUDES
#include <flogger.h>

#include <SenServiceConnection.h>
#include <SenXmlServiceDescription.h>
#include <SenBaseFragment.h>
#include <SenXmlReader.h>

#include "AddressBookContact.h"
#include "MAddressBookEngineObserver.h"

#include <SenSoapMessage.h>
#include <SenElement.h>
#include <SenBaseElement.h>

// CONSTANTS
_LIT8(KTooManyMatches, "TooManyMatches");
_LIT8(KStatus, "Status");
_LIT8(KCode, "code");
_LIT8(KOk, "OK");
_LIT8(KCard, "Card");
		
_LIT8(KQueryNS, "ab"); // Namespace identifier
 
_LIT8(KResourceIDVal, "XYZ"); // Contents of Resource Tag
 
_LIT8(KEncrypted, "Encrypted"); // Identifier for encrypted resource ID's

// Query to be sent to service, contents of Select Tag
_LIT8(KSelectQuery, "/ab:Card[contains(ab:N/ab:FAMILY,'%S') or contains(ab:N/ab:GIVEN,'%S') or contains(ab:TEL,'%S')]");

// MACROS
// for logging only
_LIT(KLogDir, "AddressBook");
_LIT(KLogFile, "AddressBook.log");
_LIT(KLogOpeningLine, "CAddressBookEngine::ConstructL - log file opened.");

#define LOG_OPEN() \
		RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeOverwrite, KLogOpeningLine);
		
#define LOG(string) \
		RFileLogger::Write(KLogDir, KLogFile, EFileLoggingModeAppend, string);
		
#define LOG_FORMAT(params) \
		RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, params);
		
#define LOG_ALL(longstring) \
	{ \
	TInt ___iLogZ___ = 0; \
	TInt ___amountLogZ___ = 100; \
	while(___iLogZ___<longstring.Length()) \
		{ \
		if(( longstring.Length()-___iLogZ___) < 100) \
			___amountLogZ___ = longstring.Length()-___iLogZ___; \
		LOG(longstring.Mid(___iLogZ___,___amountLogZ___)); \
		___iLogZ___+=100; \
		} \
	} 

// DATA TYPES
enum TErrorState
	{
	ENoError = 0,
	ESoapError,
	ENestedError
	};

// FORWARD DECLARATIONS
class CSenServiceManager;

// CLASS DECLARATION

/**
* Application Engine class
* performs engine related operations
*/
class CAddressBookEngine : public CSenBaseFragment, public MSenServiceConsumer
	{
	public: // Constructors and destructor
	
		/**
		* Two-phased constructor
		*/
		static CAddressBookEngine* NewL(MAddressBookEngineObserver& aObserver);
		
		/**
     	* Destructor.
     	*/	
		~CAddressBookEngine();
		
		// New functions
		
		/**
		* This function is called after an user has selected Search from the GUI 
		* and typed a request. Function creates a search pattern from the descriptor
		* and send it to the WSP. The message from server is received through 
		* HandleMessageL() or an error is received through HandleErrorL().
		* @param aSearchPattern - descriptor on Searching note that an user has typed
		*/
		void SendSearchL(const TDesC& aSearchPattern);
		
		/**
		* Makes a connection to an ID-WSF service.
		* When ConnectL() is called the first time, authentication and
		* discovery services are connected and the user is therefore
		* authenticated and the service is connected.
		* When ConnectL() is called again and the credentials received from
		* authentication and discovery services are valid, there are no need to
		* connect to them again. This means that SetStatus() will be called
		* almost immediately with value CONNECTED, but in device GPRS or data
		* connection is not opened yet (but instead it is opened when search
		* occurs).
		*/
		void ConnectL();
		
		/** 
		* sets a state of connection as KSenConnectionStatusReady
		*/
		TBool Connected();
		
		/**
		* This function need to get a current contact (CAddressBookView::CurrentContact())
		* or to open an item (CAddressBookView::OpenItemL())
		* @param aPosition - Index of Item whose data is needed to get
		* @param aContact - gets Value of Item whose Index == aPosition
		* @return if there is such item, KErrorNotFound is returned
     	*         otherwise returns KIndexError.
		*/ 
		TInt GetContact(TInt aPosition, CAddressBookContact*& aContact);
		
		/**
		* Close WebServices-connection if user canceled
		*/
		void Cancel();
		
		/**
		* Gets the XML query based on the search parameter passed
		* Method also calls WSF ServiceDescriptionL() getter and
		* resolves either ResourceID or EncryptedResourceID element. Later
		* on, this resource id may be used when generating the actual query
		* which is sent to the service (WSP)
		* @param aSearchPattern - descriptor on Searching note that an user has typed
		*/
		HBufC8* GenerateQueryRequestL(const TDesC& aSearchPattern);

	protected: // Functions from base classes
	
		/** 
		* From CSenDomFragment, is called from CAddressBookEngine::HandleMessageL 
		* (ParseL(aMessage)) every time when XML parser finds a new element tag.
		* In this case we are interested in the <Status> and <Card> elements.
		* When Status is found, we compare if attribute 'Code' is OK or not.
		* SenXmlUtils::LocalName() only extracts out a possible XML prefix
		* (i.e. 'ab:OK').
		* When local name is 'Card', we have encountered a contact-object.
		* Then we make a new CAddressBookContact and delegate the parsing to it.
		*
		* Example server response:
		* <ab:QueryResponse xmlns:ab="urn:nokia:test:addrbook:2004-09">
		*    <ab:Status code="ab:OK"/>
		* <ab:Data>
		*	<ab:Card>
		*		<ab:N>
		*			<ab:FAMILY>Smith</ab:FAMILY>
		*			<ab:GIVEN>John</ab:GIVEN>
		*		</ab:N>
		*		<ab:ADR>
		*			<ab:EXTADR>Room 123</ab:EXTADDR>
		*			<ab:STREET>1st ST 123</ab:STREET>
		*			<ab:PCODE>7698532</ab:PCODE>
		*		</ab:ADR>
		*		<ab:TEL>1234</ab:TEL>
		*		<ab:TEL type="CELL">2122345</ab:TEL>
		*	</ab:Card>
		* </ab:Data>
		* </ab:QueryResponse>
		*
		* @param aNsUri - namespace URI
		* @param aLocalName - local name (in this case may be Status or Card)
		* @param aQName - qualified name
		* @param aAttrs - we are interested if value of attribute is KCode or not
		*/
		virtual void StartElementL(const TDesC8& aNsUri, 
								   const TDesC8& aLocalName,
								   const TDesC8& aQName,
								   const RAttributeArray& aAttrs);
								   
		/**
		* From CSenDomFragment, is called from CAddressBookEngine::HandleMessageL 
		* (ParseL(aMessage)) every time when XML parser finds a closing element tag.
		*								  
		* We are only interested about the </Status> element.
		* When the </Status> is found, we check for errors found in response,
		* and if so, notice the observer
		* @param aNsUri - namespace URI
		* @param aLocalName - local name (in this case we are interested only Status)
		* @param aQName - qualified name
		*/							   
		virtual void EndElementL(const TDesC8& aNsUri, 
								 const TDesC8& aLocalName,
								 const TDesC8& aQName);

	private: // Constructor
		
		/**
		* C++ default constructor.
		*/
		CAddressBookEngine(MAddressBookEngineObserver& aObserver);
		
		/**
    	* By default Symbian 2nd phase constructor is private.
     	*/
		void ConstructL();

		
		// New functions
		
		/**
		* Register IdentityProvider and associates Forum Nokia's Addressbook 
		* service to be known to trust in this IDP. Method creates instance
		* of the CSenServiceManager to achieve this Identity Provider (IDP)
		* registration. IDP is also registered as Authentication Service (AS)
		* Description, because in Liberty ID-WSF, they are the same service. 
		* @return status code or -1 if error appears
		*/
		TInt RegisterIdentityProviderL();	
		
		// Functions from base classes
	 		
		/**
		* From MSenServiceConsumer, handles receiving messages from the SendL()
		* (CSenServiceConnection)function call 
		* @param aMessage - descriptor on content of receiving Message 
		*/
		virtual void HandleMessageL(const TDesC8& aMessage);
		
		/**
		* From MSenServiceConsumer, handles receiving errors about the connection to be initialized 
		* or from the SendL()(CSenServiceConnection)function call 
		* @param aErrorCode - code of error
		* @param aError - descriptor on content of receiving Error 
		*/
		virtual void HandleErrorL(const TInt aErrorCode, 
								  const TDesC8& aError);
		
		/**
		* From MSenServiceConsumer, handles receiving status info about the connection
		* to be initialized. Is called by CSenServiceConnectionImpl::RunL() 
		* that is called by active scheduler from CAddressBookAppUi::HandleCommandL(TInt aCommand) 
		* in case "EAddressBookCmdAppConnect"
		* @param aStatus - may be 
		*      KSenConnectionStatusNew, 
		*      KSenConnectionStatusReady,
		*   or STATUS_EXRIRED 
		*/
		virtual void SetStatus(const TInt aStatus);

		/**
		* This helper function resolves the ResourceID from WSF using
		* ServiceDescriptionL() getter method.
		* @return pointer to resolved ResourceID element,
		*         which content indicates whether it is
		*         encrypted or not; content of encrypted
		*         ResourceID includes "EncryptedData" 
		*         element.
		*/
		virtual CSenElement* ResolveResourceIdL();

		
	private: // Data
		RPointerArray<CAddressBookContact> iContacts;
		TInt iConnectionState;
		CSenServiceConnection* iConnection;
		CSenXmlServiceDescription* iSession;

		CSenXmlReader* iXmlReader;

        MAddressBookEngineObserver& iObserver; 
		CSenServiceManager* iManager;
		CSenDomFragment* iFaultDelegate;
		TInt iErrorState;

		// turns to ETrue after first successfull init.
		TBool iInitialized;
};

#endif // ADDRESSBOOKENGINE_H

// End of File

