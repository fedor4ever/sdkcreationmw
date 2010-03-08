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
* Description:  Interface of AddressBook
*
*/


#ifndef MADDRESSBOOKENGINEOBSERVER_H
#define MADDRESSBOOKENGINEOBSERVER_H

// DATA TYPES
enum TAddressbookErrorCode
	{
	EConnectionNotReady = 1,
	ETooManyMatches,
	ESearchFailed
	};

// FORWARD DECLARATIONS
class CAddressBookContact;

// CLASS DECLARATION
class MAddressBookEngineObserver
	{
	public:
		
		/**
		* checks errors that can happen
    	* @param aCode Code of Error
    	*/
		virtual void ErrorL(TInt aCode) = 0;
		
		/**
		* sets conection's status as ready
    	*/
		virtual void ConnectionReady() = 0;
		
		/**
		* finishes searching of information
		* @param aContactArray - contact information that was found
		*/
		virtual void SearchFinishedL(RPointerArray<CAddressBookContact>& aContactArray) = 0;
	};

#endif // MADDRESSBOOKENGINEOBSERVER_H

// End of File

