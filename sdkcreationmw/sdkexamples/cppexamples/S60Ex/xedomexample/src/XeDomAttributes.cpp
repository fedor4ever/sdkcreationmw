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


/**********************************************************
* This example demonstrates how xml engine DOM deals
* with attributes. It shows checking an element whether
* it has an attribute, getting/setting attribute name/values,
* adding/removing attributes, creating/deleting an attribute
* node, copying attributes, attributes with namespaces etc.
***********************************************************/

EXPORT_C void CXEDomEx::AttributesExampleL()
{
    // some basic attribute operations - subexample
	AttributeOperationsL();
	
	// different ways of adding attributes - subexample
	AddAttributesL();
}

void CXEDomEx::AttributeOperationsL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\input\\XML_check_attrs.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);

	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document); 
	TXmlEngElement element = document.DocumentElement().FirstChild().AsElement();

	_LIT8(str,"attr2");
	_LIT8(str2,"wrong");
	_LIT8(str3,"tewt");
	_LIT8(str4,"attr3");
	
	//Check whether this element has the given attribute
	if(!(element.HasAttributeL(str,KNullDesC8)))
	{
	    User::Leave(KErrGeneral); // or some appropriate error
	}

	//Gets the attribute node for the element
	TXmlEngAttr attr = element.AttributeNodeL(str,KNullDesC8);
	if(attr.Value().Compare(str3) != 0)
	{
	    User::Leave(KErrGeneral); // or some appropriate error
	}

	//Modify Attributes
	element = document.DocumentElement().FirstChild().AsElement();

	// Removes the attribute from the element
	element.RemoveAttributeL(str4,KNullDesC8);

	// Sets value of attribute; attribute is created if there is no such attribute yet
	element.SetAttributeL(str,str2);
	
	// Creates new attribute,
	TXmlEngAttr attr1 = document.CreateAttributeL(str4);

	// Sets new value of the attribute. Provided new value will be escaped as needed.
	attr1.SetValueL(str4);

	// Links attribute into tree (by setting the attribute node for the element)
	element.SetAttributeNodeL(attr1);

	//Save the document to verify manually
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\output\\XML_modifattrs_output.xml");
	filepath2.Append(KFilename2);	
	document.SaveL(filepath2);

	CleanupStack::PopAndDestroy(); //document
}

void CXEDomEx::AddAttributesL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\input\\XML_nsprocessing.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);

	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document); 	
	TXmlEngElement element = document.DocumentElement().FirstChild().AsElement();

	_LIT8(str,"attr2");
	_LIT8(str2,"wrong");
	_LIT8(pref,"testns");
	_LIT8(uri, "http://test.com");

	// Creates new attribute node
	element.AddNewAttributeL(str,str2);

	// Creates new attributes using namespace in the scope, which has specified URI
	element.AddNewAttributeWithNsL(str,str2,uri);

	element = element.NextSibling().AsElement();

	// Creates new attribute node using namespace of its parent element (this element)
	element.AddNewAttributeSameNsL(str,str2);

	//Creates new attribute using namespace, which is bound to the specified prefix
	element.AddNewAttributeUsePrefixL(str2,str,pref);

	_LIT8(str4,"attr3");
	_LIT8(pref2,"testns2");
	_LIT8(uri2, "http://test.com.pl");
	element.AddNewAttributeL(str4,str2,uri2,pref2);

	element = element.NextSibling().NextSibling().AsElement();

	// Resets element's attributes
	element.RemoveAttributes();

	// Copies attributes from another element
	element.CopyAttributesL(document.DocumentElement().FirstChild().AsElement());

	element = element.NextSibling().AsElement();
	TXmlEngNamespace elementNs = element.FindOrCreateNsDeclL(uri2,pref2);
	element.AddNewAttributeL(str4,str,elementNs);

	//Save the document to verify manually
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\output\\XML_addattrs_output.xml");
	filepath2.Append(KFilename2);	
	document.SaveL(filepath2);

	CleanupStack::PopAndDestroy(); //document
}
