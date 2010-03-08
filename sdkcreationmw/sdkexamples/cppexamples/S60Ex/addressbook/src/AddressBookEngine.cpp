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


// INCLUDES
#include "AddressBookEngine.h"
#include "AddressBookAppUi.h"
#include "AddressBookConstants.h"

#include <SenServiceManager.h>
#include <SenXmlServiceDescription.h>
#include <SenXmlUtils.h>
#include <SenIdentityProvider.h>

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CAddressBookEngine::CAddressBookEngine(MAddressBookEngineObserver& aObserver)
// constructor
// ----------------------------------------------------------------------------
//
CAddressBookEngine::CAddressBookEngine(MAddressBookEngineObserver& aObserver)
: iConnection(NULL),
  iSession(NULL),
  iXmlReader(NULL),
  iObserver(aObserver),
  iManager(NULL),
  iFaultDelegate(NULL),
  iErrorState(ENoError),
  iInitialized(EFalse)
	{
	}
	
// ---------------------------------------------------------------------------- 
// CAddressBookEngine::CAddressBookEngine()
// destructor
// ----------------------------------------------------------------------------
//
CAddressBookEngine::~CAddressBookEngine()
	{
	_LIT( KLogFileClosed, "CAddressBookEngine::~CAddressBookEngine() - Log file closed.");
	LOG(KLogFileClosed);
	iContacts.ResetAndDestroy();
	delete iXmlReader;
	delete iFaultDelegate;
	delete iManager;
	delete iConnection;
	delete iSession;
	delete iFaultDelegate;
	}

// ---------------------------------------------------------------------------- 
// CAddressBookEngine::NewL(MAddressBookEngineObserver& aObserver)
// Two-phased constructor
// ----------------------------------------------------------------------------
//
CAddressBookEngine* CAddressBookEngine::NewL(
									MAddressBookEngineObserver& aObserver)
	{
	CAddressBookEngine* newEngine = new (ELeave) CAddressBookEngine(aObserver);
	CleanupStack::PushL(newEngine);
	newEngine->ConstructL();
	CleanupStack::Pop(newEngine);
	return newEngine;
	}
	
// ----------------------------------------------------------------------------
// CAddressBookEngine::ConstructL()
// EPOC default constructor can leave
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::ConstructL()
	{
	LOG_OPEN();
	_LIT( KConstructL, "CAddressBookEngine::ConstructL() ***");
	LOG(KConstructL);
	CSenBaseFragment::BaseConstructL(KPpContract, KQueryResponseLocalName);
	iXmlReader = CSenXmlReader::NewL();
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::HandleMessageL(const TDesC8& aMessage)
// It is overridden from interface MSenServiceConsumer function
// for receiving messages from the SendL()(CSenServiceConnection)function call
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::HandleMessageL(const TDesC8& aMessage)
	{
	_LIT(KHandleMessageL, "CAddressBookEngine::HandleMessageL()");
	LOG( KHandleMessageL);
	LOG_ALL(aMessage);
	iContacts.ResetAndDestroy();
	SetReader(*iXmlReader);
	
	// ------------------------------------------------------------------------
	// Xmlparser calls CAddressBook::StartElementL every time a new element tag is found
	// and CAddressBookEngine::EndElementL every time a closing element tag is found.
	// ------------------------------------------------------------------------
	// 
	ParseL(aMessage);
	iObserver.SearchFinishedL(iContacts);
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::HandleErrorL(const TInt aErrorCode, const TDesC8& aError)	
// It is the overridden function from MSenServiceConsumer interface
// for receiving errors about the connection to be initialized or from the
// SendL() (CSenServiceConnection) function call.
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::HandleErrorL(const TInt aErrorCode, const TDesC8& aError)
	{
	_LIT( KHandleErrorL, "CAddressBookEngine::HandleErrorL()");
	LOG( KHandleErrorL);
	TBuf<24> code;
	code.AppendNum(aErrorCode);
	LOG(code);
	LOG_ALL(aError);
	iContacts.ResetAndDestroy();
	
	// ------------------------------------------------------------------------
	// Every error reported here is not nested XML error,
	// probably SOAP error, so show general error.
    // More sophisticated applications could resolve service
	// specific info about failure by inspecting soap fault
	// received in here.
	// ------------------------------------------------------------------------
	//
	iObserver.ErrorL(ESearchFailed);
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::SetStatus(const TInt aStatus)
// It is overridden function from MSenServiceConsumer interface
// for receiving status info about the connection to be initialized.
// It is called by CSenServiceConnectionImpl::RunL() that is called by active
// scheduler from CAddressBookAppUi::HandleCommandL(TInt aCommand) in case
// "EAddressBookCmdAppConnect"
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::SetStatus(const TInt aStatus)
	{
	_LIT( KSetStatus, "CAddressBookEngine::SetStatus()");
	LOG( KSetStatus);
	iConnectionState = aStatus;
	switch(iConnectionState)
		{
		case KSenConnectionStatusNew:
			_LIT( KSenConnStatNew, "KSenConnectionStatusNew");
			LOG( KSenConnStatNew);
			break;
		case KSenConnectionStatusReady:
			_LIT( KSenConnStatRdy, "KSenConnectionStatusReady");
			LOG( KSenConnStatRdy);
			iInitialized = ETrue;
			iObserver.ConnectionReady();
			break;
		case KSenConnectionStatusExpired:
			_LIT( KSenConnStatEx, "KSenConnectionStatusExpired");
			LOG( KSenConnStatEx);
			break;
		default:
			TBuf<32> num;
			num.AppendNum(iConnectionState);
			_LIT( KUknStat, "UNKNOWN STATUS: (%S)");
			LOG_FORMAT((KUknStat, num));
			break;
		}
	if(!iInitialized)
		{
		TInt leaveCode(KErrNone);
		TRAP(leaveCode, iObserver.ErrorL(EConnectionNotReady));
		leaveCode = 0; // not used
		}
	}

// ---------------------------------------------------------------------------
// CAddressBookEngine::ResolveResourceIdL()
// This helper function resolves the ResourceID from WSF using
// ServiceDescriptionL() getter method.
// return pointer to resolved ResourceID element,
// which content indicates whether it is encrypted or not; 
// content of encrypted ResourceID includes "EncryptedData" element.
//----------------------------------------------------------------------------
//
CSenElement* CAddressBookEngine::ResolveResourceIdL()
	{
	_LIT( KResolveResourceIdL, "ResolveResourceIdL()");
	LOG( KResolveResourceIdL);
	TInt parseLeaveCode(KErrNone);
	HBufC8* sd = NULL;
	CSenElement* resourceID = NULL;

	if(iInitialized && iConnection)  
		{
		TInt err0 = iConnection->ServiceDescriptionL(sd);
		if(err0 == KErrNone && sd)
			{
			_LIT( KServDescRet, "iConnection->ServiceDescriptionL(sd) returned:");
			LOG( KServDescRet);
			CleanupStack::PushL(sd);
			LOG_ALL((*sd));
			delete iSession;
			iSession = NULL;
			iSession = CSenXmlServiceDescription::NewL();
			iSession->SetReader(*iXmlReader);
			iXmlReader->SetContentHandler(*iSession);
			TRAP(parseLeaveCode, iSession->ParseL(*sd));
			CleanupStack::PopAndDestroy(sd); 
			if(parseLeaveCode!=KErrNone)
				{
				_LIT( KParServDescFailed, "Parsing of service description (session) failed!");
				LOG( KParServDescFailed);
				}
			else
				{
				_LIT8 (KResourceID, "ResourceID");
				resourceID = iSession->AsElement().Element(KResourceID);
				if(resourceID)
					{
					_LIT( KResID, "ResourceID:");
					LOG(KResID);
					LOG_ALL((resourceID->Content()));
					}
				else
					{
					_LIT( KResIdNotFound, "ResourceID not found from service description.");
					LOG( KResIdNotFound);
					}
				}
			}
		}
	return resourceID;
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::GenerateQueryRequestL(const TDesC& aSearchPattern)
// gets the XML query based on the search parameter passed
// SAMPLE XML Request is 
// 
// <ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\">
//	 <ab:QueryItem>
//		<ab:Select> 
//		        ab:Card[contains(ab:N ab:FAMILY,"%S") or contains(ab:N ab:GIVEN, "%S") or contains(ab:TEL, "%S")]
//		</ab:Select>
// 	 </ab:QueryItem>
//	 <ab:ResourceID>
//	      XYZ
//	 </ab:ResourceID>
// </ab:Query>
//
// The %S will be replaced with the value entered by user as search string
// ----------------------------------------------------------------------------
//
HBufC8* CAddressBookEngine::GenerateQueryRequestL(const TDesC& aSearchPattern)
	{
	_LIT( KGenQueryReqL, "CAddressBookEngine::GenerateQueryRequestL(const TDesC& aSearchPattern)");
	LOG( KGenQueryReqL);
 
 	// Create Query Tag
 	_LIT8 (KElement, "Query");
    CSenElement* element = CSenBaseElement::NewL(KElement);
	CleanupStack::PushL(element);
	element->SetNamespaceL(KQueryNS, KPpContract);
	
	// Create QueryItem element
	_LIT8 (KQueryItem, "ab:QueryItem");	
	CSenElement* queryItem = CSenBaseElement::NewL(KQueryItem);
	CleanupStack::PushL(queryItem);
	
	// Create Select element
	_LIT8 (KSelectElement, "ab:Select");
	CSenBaseElement* selectElement=CSenBaseElement::NewL(KSelectElement);	
	CleanupStack::PushL(selectElement);
	
	//Create temporary buffer and perform UTF8 conversion for parameter passed	
	HBufC8* tempBuffer = HBufC8::NewLC(512);  	
	TPtr8 ptrSelectQuery = tempBuffer->Des();	
	HBufC8* pattern = SenXmlUtils::ToUtf8LC(aSearchPattern); 
	//Replace %S with pattern passed in parameter
	ptrSelectQuery.Format(KSelectQuery, pattern, pattern, pattern);	
		
	selectElement->SetContentL(ptrSelectQuery);	
	// Add select element to QueryItem
	queryItem->AddElementL(*selectElement);
		
	//Add QueryItemto Query
	element->AddElementL(*queryItem);

	// Next phase is to create and add ResourceID element into the query.
	CSenElement* resourceID = ResolveResourceIdL();
	CSenBaseElement* resourceElement = NULL; 
	if(resourceID)
		{
		// (Encrypted)Resource was successfully resolved. Now check
		// whether plain ResourceID or EncryptedResourceID is
		// available and create according element into the query.
		if (resourceID->Content().Find(KEncrypted()) != KErrNotFound) 
			{
			_LIT( KEncResID, "EncryptedResourceID exists.");
			LOG(KEncResID);
			_LIT8 (KResourceElement, "ab:EncryptedResourceID");
			resourceElement = CSenBaseElement::NewL(KResourceElement);
			CleanupStack::PushL(resourceElement);	
			// copy the actual resource id content
			resourceElement->CopyFromL(*resourceID);
			}
		else
			{
			_LIT( KOnlyPlainResID, "Only plain ResourceID should be used (not encrypted).");
			LOG( KOnlyPlainResID);
			_LIT8 (KResourceElement, "ab:ResourceID");
			resourceElement = CSenBaseElement::NewL(KResourceElement);
			CleanupStack::PushL(resourceElement);	
			// Copy the actual resource id content
			resourceElement->CopyFromL(*resourceID);
			}
		}
	else
		{
		_LIT( KErrResID, "Error: could not resolve ResourceID!");
		LOG( KErrResID);
		_LIT8 (KResourceElement, "ab:ResourceID");
		resourceElement=CSenBaseElement::NewL(KResourceElement);	
		CleanupStack::PushL(resourceElement);	
		resourceElement->SetContentL(KResourceIDVal); // example content
		}
	element->AddElementL(*resourceElement);
	CleanupStack::Pop(resourceElement); 
	
	//Get the XML
	HBufC8* soapBuf = element->AsXmlL();

	// Log the XML request
    _LIT( KGenQuery, "Generated Query:");
    LOG( KGenQuery);
	LOG_ALL(( soapBuf->Des() ));
	
	//Destroy the objects pushed on cleanup stack
	CleanupStack::PopAndDestroy(pattern);
	CleanupStack::PopAndDestroy(tempBuffer);
	CleanupStack::Pop(selectElement);
	CleanupStack::Pop(queryItem);
	CleanupStack::PopAndDestroy(element);
	//return Pointer to Soap Request Buffer;
	return soapBuf;
 	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::SendSearchL(const TDesC& aSearchPattern)
// function is called after an user has selected Search from the GUI 
// and typed a request. Function creates a search pattern from the descriptor
// and send it to the WSP. The message from server is received through 
// HandleMessageL() or an error is received through HandleErrorL().
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::SendSearchL(const TDesC& aSearchPattern)
	{	
	_LIT( KSendSearchL, "CAddressBookEngine::SendSearchL()");
	LOG(KSendSearchL);
	iErrorState = ENoError;
	if (!iConnection)
		{
		_LIT( KConnNotReady, "Connection not ready.");
		LOG(KConnNotReady);
		iObserver.ErrorL(EConnectionNotReady);
		return;
		}
	
	//Get the xml query to be sent to service
	HBufC8* stSearch= GenerateQueryRequestL(aSearchPattern);
	CleanupStack::PushL(stSearch);

	//Send the query to service
	iConnection->SendL(*stSearch);
	
	CleanupStack::PopAndDestroy(stSearch);
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::StartElementL(const TDesC8& aNsUri,
//									const TDesC8& aLocalName,
//									const TDesC8& aQName,
//									const RAttributeArray& aAttrs)
// It is called from CAddressBookEngine::HandleMessageL (ParseL(aMessage))
// every time when XML parser finds a new element tag.
// 									
// Example server response:
// <ab:QueryResponse xmlns:ab="urn:nokia:test:addrbook:2004-09">
//    <ab:Status code="ab:OK"/>
// <ab:Data>
//	<ab:Card>
//		<ab:N>
//			<ab:FAMILY>Smith</ab:FAMILY>
//			<ab:GIVEN>John</ab:GIVEN>
//		</ab:N>
//		<ab:ADR>
//			<ab:EXTADR>Room 123</ab:EXTADDR>
//			<ab:STREET>1st ST 123</ab:STREET>
//			<ab:PCODE>7698532</ab:PCODE>
//		</ab:ADR>
//		<ab:TEL>1234</ab:TEL>
//		<ab:TEL type="CELL">2122345</ab:TEL>
//	</ab:Card>
// </ab:Data>
// </ab:QueryResponse>
//
// In this case we are interested in the <Status> and <Card> elements.
// When Status is found, we compare if attribute 'Code' is OK or not.
// SenXmlUtils::LocalName() only extracts out a possible XML prefix
// (i.e. 'ab:OK').
// When local name is 'Card', we have encountered a contact-object.
// Then we make a new CAddressBookContact and delegate the parsing to it.
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::StartElementL(	const TDesC8& aNsUri,
										const TDesC8& aLocalName,
										const TDesC8& aQName,
										const RAttributeArray& aAttrs)
	{
	_LIT( KStartElementL, "CAddressBookEngine::StartElementL() ***");
	_LIT( KNamespaceURI, "- namespace URI  (%S)");
	_LIT( KLocalname, "- localname      (%S)");
	_LIT( KQualName, "- qualified name (%S)");
	
	LOG( KStartElementL);
	LOG_FORMAT((KNamespaceURI, aNsUri));
	LOG_FORMAT((KLocalname, aLocalName));
	LOG_FORMAT((KQualName, aQName));

	if (aLocalName == KStatus)
		{
		
		if (SenXmlUtils::LocalName(SenXmlUtils::AttrValue(aAttrs, KCode)) != KOk)
			{
			iErrorState = ENestedError;
			delete iFaultDelegate;
			iFaultDelegate = NULL;
			iFaultDelegate = CSenDomFragment::NewL(aNsUri,
							aLocalName, aQName, aAttrs);
			DelegateParsingL(*iFaultDelegate);
			return;
			}
		}
	else if (aLocalName == KCard)
		{
		iErrorState = ENoError;

		CAddressBookContact* delegate = CAddressBookContact::NewL();
		CleanupStack::PushL(delegate);
		iContacts.Append(delegate);
		DelegateParsingL(*delegate);
		CleanupStack::Pop(delegate);
		return;
		}
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::EndElementL(const TDesC8& aNsUri,
//								  const TDesC8& aLocalName,
//								  const TDesC8& aQName)
// It is called from CAddressBookEngine::HandleMessageL (ParseL(aMessage))
// every time when XML parser finds a closing element tag.
//								  
// We are only interested about the </Status> element.
// When the </Status> is found, we check for errors found in response,
// and if so, notice the observer
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::EndElementL(	const TDesC8& aNsUri,
										const TDesC8& aLocalName,
										const TDesC8& aQName)
	{
	_LIT( KEndElementL, "CAddressBookEngine::EndElementL()");
	_LIT( KNamespaceURI, "- namespace URI  (%S)");
	_LIT( KLocalname, "- localname      (%S)");
	_LIT( KQualName, "- qualified name (%S)");	

	LOG( KEndElementL);
	LOG_FORMAT((KNamespaceURI, aNsUri));
	LOG_FORMAT((KLocalname, aLocalName));
	LOG_FORMAT((KQualName, aQName));

	if(aLocalName == KStatus)
		{
		if (iErrorState == ENestedError)
			{
			iErrorState = ENoError;
			CSenElement* statusElement = NULL;
			if (iFaultDelegate)
				{
					statusElement =
						iFaultDelegate->AsElement().Element(KStatus);
				}

			if(statusElement)
				{
				const TDesC8* errorCode = statusElement->AttrValue(KCode);
				iContacts.ResetAndDestroy();
				if (errorCode)
					{
					if (SenXmlUtils::LocalName(*errorCode) == KTooManyMatches)
						{
						iObserver.ErrorL(ETooManyMatches);
						}
					else
						{
						iObserver.ErrorL(ESearchFailed); // Default error
						}
					}
				}
			else
				{
				// could not find status element:
				iObserver.ErrorL(ESearchFailed); // Default error
				}
			}
		else if (iErrorState == ESoapError)
			{
			iErrorState = ENoError;
			delete iFaultDelegate;
			iFaultDelegate = NULL;
			iContacts.ResetAndDestroy();
			iObserver.ErrorL(ESearchFailed); // Default error
			}
		}
	else
		{
		CSenBaseFragment::EndElementL(aNsUri, aLocalName, aQName);
		}
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::RegisterIdentityProviderL()
// registers the IdentityProvider and associates the FN service to it. 
// Method creates the CSenServiceManager instance and uses it to register
// Identity Provider ALSO as Authentication Service (AS) Description.
// The actual Forum Nokia's Addressbook service is then associated to this
// newly registered IdentityProvider.
// ----------------------------------------------------------------------------
//
TInt CAddressBookEngine::RegisterIdentityProviderL()
	{
	_LIT( KRegIdProvL, "CAddressBookEngine::RegisterIdentityProviderL()");
	LOG( KRegIdProvL);
	TBuf<64> buf; // for logging

	TInt retVal(KErrNone);

	if (!iManager)
		{
		_LIT( KInsNewServMng, "Instantiating new service manager");
		LOG( KInsNewServMng);
		iManager = CSenServiceManager::NewL();
		}

	CSenIdentityProvider* idp = CSenIdentityProvider::NewLC(
		KASEndPoint, KASContract);

	// Set the Provider ID
	idp->SetProviderID(KASProviderID);

	// Associate Forum Nokia's Addressbook service ID (contract)
	// to this Identity Provider
	idp->SetServiceID(KPpContract);

	// Set Liberty ID-WSF framework ID
	idp->SetFrameworkIdL(KDefaultIdWsfFrameworkID);

	// ------------------------------------------------------------------------
	// The following username/password properties will be used for
	// authentication. Please note, that use advisory authentication
	// id "IMEI" would result device ID to be directly fetched from phone.
	// ------------------------------------------------------------------------
	//
	idp->SetUserInfoL(KTestAuthzID, KNullDesC8, KTestPassword);

	_LIT( KRegIdProv, "Registering identityprovider. User info provided.");
	LOG( KRegIdProv);
	retVal = iManager->RegisterIdentityProviderL(*idp);
	if(retVal != KErrNone)
		{
		// --------------------------------------------------------------------
		// An error occurred. However, the rest of registration calls will
		// be attempted in order to provide any possible data that WSF needs.
		// --------------------------------------------------------------------
		buf.AppendNum(retVal);
		_LIT( KErrRegIdProv, "Error registering Identity Provider: %S");
		LOG_FORMAT((KErrRegIdProv, buf));
		buf.Zero();
		}
	_LIT( KRegAutServDesc, "Registering autentication service description.");
	LOG( KRegAutServDesc);
	retVal = iManager->RegisterServiceDescriptionL(*idp);
	if(retVal != KErrNone)
		{
		// --------------------------------------------------------------------
		// An error occurred. However, the rest of registration calls will
		// be attempted in order to provide any possible data that WSF needs.
		// --------------------------------------------------------------------
		buf.AppendNum(retVal);
		_LIT( KErrRegServDesc, "Error registering Service Description: %S");
		LOG_FORMAT((KErrRegServDesc, buf));
		buf.Zero();
		}

// Note, that following API call could be used to associate more services
// to some Identity Provider, which ProviderID is known.
//
//	LIT( KAssocAddressbookServ, "Associating Addressbook service (WSP) to known Identity Provider.");
//	LOG( KAssocAddressbookServ);
//	retVal = iManager->AssociateServiceL(KPpContract, KASProviderID);
//
//	if(retVal != KErrNone) 
//		{
//		// --------------------------------------------------------------------
//		// An error occurred. However, the rest of registration calls will
//		// be attempted in order to provide any possible data that WSF needs.
//		// --------------------------------------------------------------------
//		buf.AppendNum(retVal);
//		_LIT( KErrAssFNServ, "Error associating the FN service to IDP %S");
//		LOG_FORMAT( KErrAssFNServ, buf);
//		buf.Zero();
//		}

	CleanupStack::PopAndDestroy(idp); 
	return retVal;
	}

// ----------------------------------------------------------------------------
// CAddressBookEngine::ConnectL()
// Makes a connection to an ID-WSF service.
// When ConnectL() is called the first time, authentication and
// discovery services are connected and the user is therefore
// authenticated and the service is connected.
// When ConnectL() is called again and the credentials received from
// authentication and discovery services are valid, there are no need to
// connect to them again. This means that SetStatus() will be called
// almost immediately with value CONNECTED, but in device GPRS or data
// connection is not opened yet (but instead it is opened when search
// occurs).
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::ConnectL()
	{
	_LIT (KConnectL, "CAddressBookEngine::ConnectL()");
	LOG( KConnectL);
	RegisterIdentityProviderL();

	CSenXmlServiceDescription* pattern = CSenXmlServiceDescription::NewLC();
	pattern->SetContractL(KPpContract());
	delete iConnection;
	iConnection = NULL;
	iConnection = CSenServiceConnection::NewL(*this, *pattern);
	CleanupStack::PopAndDestroy(pattern); 
	}
	
// ---------------------------------------------------------------------------- 
// CAddressBookEngine::Connected()
// sets a state of connection as KSenConnectionStatusReady
// ----------------------------------------------------------------------------
//
TBool CAddressBookEngine::Connected()
	{
	return (iConnectionState == KSenConnectionStatusReady);
	}

// ---------------------------------------------------------------------------- 
// CAddressBookEngine::GetContact(TInt aPosition, CAddressBookContact*& aContact)
// This function need to get a current contact (CAddressBookView::CurrentContact())
// or to open an item (CAddressBookView::OpenItemL())
// ----------------------------------------------------------------------------
// 
TInt CAddressBookEngine::GetContact(TInt aPosition, CAddressBookContact*& aContact)
	{
	_LIT( KGetContact, "CAddressBookEngine::GetContact()");
	LOG( KGetContact);
	TInt retVal(KErrNotFound);
	if (aPosition < iContacts.Count() && aPosition >= 0)
		{
		aContact = iContacts[aPosition];
		retVal = KErrNone;
		}
	return retVal;
	}
	
// ---------------------------------------------------------------------------- 
// CAddressBookEngine::Cancel()
// Close WebServices-connection if user canceled
// ----------------------------------------------------------------------------
//
void CAddressBookEngine::Cancel()
	{
	_LIT( KCancel, "CAddressBookEngine::Cancel()");
	LOG( KCancel);
	delete iConnection;
	iConnectionState = 0;	 // not connected
	iConnection = NULL;
	}

// End of File 

