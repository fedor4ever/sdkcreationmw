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
* Description:  CAddressBookContact class - 
*				 responsible for work with personal card of an employee, 
*				 who was found
*
*/


#ifndef ADDRESSBOOKCONTACT_H
#define ADDRESSBOOKCONTACT_H

// INCLUDES
#include <SenDomFragment.h>

// CONSTANTS
_LIT8(KElementNameCard,		"Card");
_LIT8(KQualifiedNameCard,	"ab:Card");
_LIT8(KNsUri,				"urn:nokia:test:addrbook:2004-09");

_LIT8(KElementNameN,		"N");
_LIT8(KElementNameGiven,	"GIVEN");
_LIT8(KElementNameFamily,	"FAMILY");
_LIT8(KElementNameAdr,		"ADR");
_LIT8(KElementNameStreet,	"STREET");
_LIT8(KElementNamePCode,	"PCODE");
_LIT8(KElementNamePref,		"PREF");
_LIT8(KElementNameCell,		"CELL");
_LIT8(KElementNameEmail,	"EMAIL");
_LIT8(KElementNameCountry,	"COUNTRY");
_LIT8(KElementNameMiddle,	"MIDDLE");
_LIT8(KElementNameExtAdr,	"EXTADR");
_LIT8(KElementNameLocality, "LOCALITY");
_LIT8(KElementNameRegion,	"REGION");
_LIT8(KElementNameFax,		"FAX");
_LIT8(KElementNameTitle,	"TITLE");
_LIT8(KElementNameOrg,		"ORG");
_LIT8(KElementNameOrgName,	"ORGNAME");
_LIT8(KElementNameOrgUnit,	"ORGUNIT");
_LIT8(KElementNameTel,		"TEL");

_LIT8(KAttrNameTypeUpperCase, "TYPE");
_LIT8(KAttrNameTypeLowerCase, "type");

// CLASS DECLARATION

/**
* CAddressBookContact class, responsible for work with personal card of an employee, who was found
*/
class CAddressBookContact : public CSenDomFragment
	{
	public: // Constructors
		
		/**
		* Two-phased constructor
		*/	
		static CAddressBookContact* NewL();
		
		// New functions
	
		/**
		* The function gets first name of the person who was found
		* @return first name
		*/		
		const TPtrC8 FirstName();
		
		/**
		* The function gets last name of the person who was found
		* @return last name
		*/
		const TPtrC8 LastName();
		
		/**
		* The function gets street where the person who was found lives
		* @return street
		*/
		const TPtrC8 Street();
		
		/**
		* The function gets post code where the person who was found lives
		* @return post code
		*/
		const TPtrC8 PostCode();
		
		/**
		* The function gets phone of the person who was found 
		* @return phone
		*/
		const TPtrC8 PhoneL();
		
		/**
		* The function gets mobile phone of the person who was found 
		* @return mobile phone
		*/
		const TPtrC8 MobilePhoneL();
	
		/**
		* The function gets email of the person who was found 
		* @return email
		*/
		const TPtrC8 Email();
		
		/**
		* The function gets middle name of the person who was found 
		* @return middle name
		*/
		const TPtrC8 MiddleName();
		
		/**
		* The function gets additional address where the person who was found lives
		* @return extra address
		*/
		const TPtrC8 ExtraAddress();
		
		/**
		* The function gets locality where the person who was found lives
		* @return locality
		*/
		const TPtrC8 Locality();
		
		/**
		* The function gets region where the person who was found lives
		* @return region
		*/
		const TPtrC8 Region();
		
		/**
		* The function gets country where the person who was found lives
		* @return country
		*/
		const TPtrC8 Country();
		
		/**
		* The function gets fax of the person who was found 
		* @return fax
		*/
		const TPtrC8 FaxL();
		
		/**
		* The function gets title of the person who was found 
		* @return title
		*/
		const TPtrC8 Title();
		
		/**
		* The function gets organization where the person who was found works
		* @return organization
		*/
		const TPtrC8 Organization();
		
		/**
		* The function gets unit where the person who was found works
		* @return unit
		*/
		const TPtrC8 Unit();
		
	private: // Constructors
	
		/**
		* C++ default constructor
		*/
		CAddressBookContact();
		
		/**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
		// New functions
				
		/**
		* The function binds two elements
		* @param aFirstElement
		* @param aSecondElement
		* @return content of two elements
		*/
		const TPtrC8 ChildValue(const TDesC8& aFirstElement, 
								const TDesC8& aSecondElement);
		
		/**
		* The function check a type of phone
		* @param aType - mobile, general, fax
		* @return phone content
		*/
		const TPtrC8 SelectedPhoneL(const TDesC8& aType);
	};

#endif // ADDRESSBOOKCONTACT_H

// End of File

