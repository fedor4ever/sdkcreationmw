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


// INCLUDES
#include "AddressBookContact.h"
#include <SenBaseAttribute.h>

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CAddressBookContact::CAddressBookContact()
// ----------------------------------------------------------------------------
//
CAddressBookContact::CAddressBookContact()
	{
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::NewL()
// Two-phased constructor
// ----------------------------------------------------------------------------
//
CAddressBookContact* CAddressBookContact::NewL()
	{
	CAddressBookContact* pNew = new (ELeave) CAddressBookContact();
	CleanupStack::PushL(pNew);
	pNew->ConstructL();
	CleanupStack::Pop(pNew);
	return pNew;
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::ConstructL()
// ----------------------------------------------------------------------------
//
void CAddressBookContact::ConstructL()
	{
	CSenDomFragment::BaseConstructL(KNsUri, KElementNameCard, KQualifiedNameCard);
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::FirstName()
// The function gets first name of the person who was found
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::FirstName()
	{
	return ChildValue(KElementNameN, KElementNameGiven);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::LastName()
// The function gets last name of the person who was found
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::LastName()
	{
	return ChildValue(KElementNameN, KElementNameFamily);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::Street()
// The function gets a street where the person who was found lives
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Street()
	{
	return ChildValue(KElementNameAdr, KElementNameStreet);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::PostCode()
// The function gets post code where the person who was found lives
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::PostCode()
	{
	return ChildValue(KElementNameAdr, KElementNamePCode);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::PhoneL()
// The function gets a phone of the person who was found
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::PhoneL()
	{
	return SelectedPhoneL(KElementNamePref);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::MobilePhoneL()
// The function gets mobile phone of the person who was found
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::MobilePhoneL()
	{
	return SelectedPhoneL(KElementNameCell);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::Email()
// The function gets email of the person who was found
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Email()
	{
	return ContentOf(KElementNameEmail);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::MiddleName()
// The function gets middle name of the person who was found
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::MiddleName()
	{
	return ChildValue(KElementNameN, KElementNameMiddle);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::ExtraAddress()
// The function gets additional address where the person who was found lives
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::ExtraAddress()
	{
	return ChildValue(KElementNameAdr, KElementNameExtAdr);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::Locality()
// The function gets locality where the person who was found lives
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Locality()
	{
	return ChildValue(KElementNameAdr, KElementNameLocality);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::Region()
// The function gets a region where the person who was found lives
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Region()
	{
	return ChildValue(KElementNameAdr, KElementNameRegion);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::Country()
// The function gets a country where the person who was found lives
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Country()
	{
	return ChildValue(KElementNameAdr, KElementNameCountry);
	}
	
// ----------------------------------------------------------------------------
// CAddressBookContact::FaxL()
// The function gets a fax of the person who was found
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::FaxL()
	{
	return SelectedPhoneL(KElementNameFax);
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::Title()
// The function gets a title of the person who was found 
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Title()
	{
	return ContentOf(KElementNameTitle);
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::Organization()
// The function gets a name of organization where the person who was found works
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Organization()
	{
	return ChildValue(KElementNameOrg, KElementNameOrgName);
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::Unit()
// The function gets a name of unit where the person who was found works
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::Unit()
	{
	return ChildValue(KElementNameOrg, KElementNameOrgUnit);
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::ChildValue(const TDesC8& aFirstElement, 
//								 const TDesC8& aSecondElement)
// The function binds two elements and returns their content
// ----------------------------------------------------------------------------
//								 
const TPtrC8 CAddressBookContact::ChildValue(const TDesC8& aFirstElement, 
								 const TDesC8& aSecondElement)
	{
	CSenElement* pElement = AsElement().Element(aFirstElement);
	if (pElement)
		{
		pElement = pElement->Element(aSecondElement);
		if (pElement)
			{
			return pElement->Content();
			}
		}
	return KNullDesC8();
	}

// ----------------------------------------------------------------------------
// CAddressBookContact::SelectedPhoneL(const TDesC8& aType)
// The function check a type of phone (mobile, general, fax) and returns its content
// ----------------------------------------------------------------------------
//
const TPtrC8 CAddressBookContact::SelectedPhoneL(const TDesC8& aType)
	{
	RPointerArray<CSenElement> elements = AsElement().ElementsL();
	CSenElement* pElement = NULL;
	for (TInt i=0; i<elements.Count(); i++)
		{
		pElement = elements[i];
		if (pElement->LocalName() == KElementNameTel)
			{
			const TDesC8* value = pElement->AttrValue(KAttrNameTypeUpperCase);
			if (!value)
				{
				// lower case type check just in case
				value = pElement->AttrValue(KAttrNameTypeLowerCase); 
				}
			if (value && *value == aType)
				{
				return pElement->Content();
				}
			}
		}
	return KNullDesC8();	
	}

// End of File 

