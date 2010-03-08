/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include "XeDomExample.h"


/************************************************************
* This example demonstrates how namespaces can be dealt
* with in XML Engine DOM. It covers looking up for namespace
* URIs, prefixes, retrieving the default namespace uris,
* checking for NS URIs for a given prefix, creating a NS
* prefix & URI, addding a NS declaration onto an element etc.
**************************************************************/

EXPORT_C void CXEDomEx::NsProcessingExampleL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\input\\XML_nsprocessing.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);

	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document);
	TXmlEngElement element = document.DocumentElement().FirstChild().AsElement();

	_LIT8(uri, "http://test.com");
	_LIT8(uriD, "http://test.pl");
	_LIT8(pref, "testns");

	//Looks up NS URI for given prefix
	if(element.LookupNamespaceUriL(pref).Compare(uri))
	{
		User::Leave(KErrGeneral); // or some appropriate error
	}

	// Looks up for prefix for given URI
	if(element.LookupPrefixL(uri).Compare(pref))
	{
		User::Leave(KErrGeneral); // or some appropriate error
	}

	//Gets the default namespace URI
	if(element.DefaultNamespaceL().Uri().Compare(uriD))
	{
		User::Leave(KErrGeneral); // or some appropriate error
	}


	//Checks whether an element has a NS declaration for the given prefix
	if(!document.DocumentElement().AsElement().HasNsDeclarationForPrefixL(pref))
	{
		User::Leave(KErrGeneral); // or some appropriate error
	}

	_LIT8(prefix2,"wrong");
	_LIT8(uri2,"http://wrong.com");
	
	//Creates an NS declaration for the given element after searching
	//for its existence.
	element.FindOrCreateNsDeclL(uri2,prefix2);
	

	_LIT8(prefix3,"newns");
	_LIT8(uri3,"http://newns.com");
	element = element.NextSibling().AsElement();

	//Adds a namespace declaration for the element
	element.AddNamespaceDeclarationL(uri3,prefix3);

	//Makes the element not to have a default namespace
	element.SetNoDefaultNamespaceL();

	//Save the document to verify manually
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\output\\XML_nsprocessing_output.xml");
	filepath2.Append(KFilename2);	
	document.SaveL(filepath2);

    CleanupStack::PopAndDestroy(); //document
}

