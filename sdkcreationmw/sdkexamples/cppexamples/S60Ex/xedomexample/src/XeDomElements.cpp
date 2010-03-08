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


/********************************************************
* This example deals with element class of XML engine DOM
* It shows adding of new elements with/without namespaces,
* creating elements with auto prefixes, removal of element
* nodes, setting content text for elements (both raw and
* escaped), fetching elements by their name, removing NS
* declarations of an elements etc.
*********************************************************/
EXPORT_C void CXEDomEx::ElementsExampleL()
{
    // some examples of adding and removing elements
	AddRemoveElementsL();

    // accessing element text - subexample	
    SetElementTextL();
}


void CXEDomEx::AddRemoveElementsL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\input\\XML_nsprocessing.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);

	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document);
	TXmlEngElement element = document.DocumentElement().FirstChild().AsElement();

	_LIT8(str,"element");
	_LIT8(str2,"wrong");
	_LIT8(pref,"testns");
	_LIT8(uri, "http://test.com");

	// Adds child element with no namespace
	element.AddNewElementL(str);

	// Performs lookup for the namespace declaration for specified namespace URI and
	// adds new child element with found namespace.
	element.AddNewElementWithNsL(str,uri);

	element = element.NextSibling().AsElement();

	// Performs lookup for the namespace declaration for specified prefix and
	// adds new child element with found namespace.
	element.AddNewElementUsePrefixL(str2,pref);

	_LIT8(str4,"element3");
	element = element.NextSibling().NextSibling().AsElement();

	// Copies a list of elements.Elements are appended to the element's children list.
	element.CopyChildrenL(document.DocumentElement().FirstChild().AsElement());

	_LIT8(uri3, "http://test.test.pl");
	element = element.NextSibling().AsElement();
	
	// Creates new child element; if there is no a prefix binding for new element's namespace,
	// a namespace decaration is created with generated prefix at specified element.
	element.AddNewElementAutoPrefixL(str4,uri3,document.DocumentElement());

	//Save the document to verify manually
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\output\\XML_addelements_output.xml");
	filepath2.Append(KFilename2);	
	document.SaveL(filepath2);

	// To demonstrate removing of an element
	TXmlEngNode node1 = document.DocumentElement();
	TXmlEngNode node2 = node1.FirstChild().Unlink();
	node2 = node1.LastChild().SubstituteForL(node2);
	node2.Remove();
	node1.FirstChild().Remove();
	
	CleanupStack::PopAndDestroy(); //document
}

void CXEDomEx::SetElementTextL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\input\\XML_setelement_text.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);

	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document);
	TXmlEngElement element = document.DocumentElement().FirstChild().AsElement();

	_LIT8(str4,"test");
	if(element.Text().Compare(str4) != 0)
	{
        User::Leave(KErrGeneral); // or some appropriate error
	}

	_LIT8(str,"new text");
	_LIT8(str2," new added text");
	_LIT8(str3,"test&test1;test");

	// Sets text contents for the element. Any child nodes are removed
	element.SetTextL(str);

	// Adds text as a child of the element
	element.AddTextL(str2);

	element = element.NextSibling().AsElement();

	// Sets text content of the element from escaped string.
	element.SetEscapedTextL(str3);

	//Save the document to verify manually
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\output\\XML_addelements_output.xml");
	filepath2.Append(KFilename2);	
	document.SaveL(filepath2);

    CleanupStack::PopAndDestroy(); //document
}

