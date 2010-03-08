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


/*****************************************************************
* Demonstrates the creation / buidling of a document from scratch.
* Also shows how to deal with the different nodes
* in the document and how operations of a node list
******************************************************************/
EXPORT_C void CXEDomEx::DocProcessingExampleL()
{
	//creating a new document - sub example
	CreateDocumentL();
	
	// Perform some sibling operations
	SiblingOperationsL();
	
	// How to use node list
	NodeListOperationsL();
}

void CXEDomEx::CreateDocumentL()
{
	_LIT8(name,"doc");
	_LIT8(uri,"http://test.com");
	_LIT8(arg1,"1.0");
	_LIT8(arg2,"http://test2.org");
	_LIT8(elename,"test");
	
	RXmlEngDocument document;
	document.OpenL(iDomImpl);
	CleanupClosePushL(document);
	
	//Create a new document
	TXmlEngElement docEle = document.CreateDocumentElementL(name); // Creates and returns a document element
	document.DocumentElement().SetDefaultNamespaceL(uri); // Sets the default namespace of the document
	document.SetStandalone(TRUE); // Sets the standalone status of the document
	document.SetXmlVersionL(arg1); // Sets the XML version
	document.SetDocumentUriL(arg2);	// Sets the document URI

	TXmlEngElement ele= document.CreateElementL(elename); // Creates an element node
	docEle.AppendChildL(ele); // And make the element node a child to the document

	//Save the document to verify manually
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\output\\XML_createdoc_output.xml");
	filepath.Append(KFilename);	
	document.SaveL(filepath);
    
    CleanupStack::PopAndDestroy(); //document
}


void CXEDomEx::SiblingOperationsL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\input\\XML_sibling_fns.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);

	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document);

	// Retrieves the doc. element
	TXmlEngElement element = document.DocumentElement();
    TXmlEngNode node;
    
	// Sets the first child as last
	element.FirstChild().SetAsLastSibling();

	// Sets the previous child of last child as first one
	element.LastChild().PreviousSibling().SetAsFirstSibling();

	//Moves the last but one child before the child next to first child
	node = element.FirstChild().NextSibling();
	element.LastChild().PreviousSibling().MoveBeforeSibling(node);

	//Moves the next to next sibling of first child after last but one sibling
	node = element.LastChild().PreviousSibling();
	element.FirstChild().NextSibling().NextSibling().MoveAfterSibling(node);

	//Save the document to verify manually
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\output\\XML_check_sibling_fns.xml");
	filepath2.Append(KFilename2);	
	document.SaveL(filepath2);
	
	CleanupStack::PopAndDestroy(); //document
}


void CXEDomEx::NodeListOperationsL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename, "\\xedomexample\\input\\XML_nodetree_modif.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);

	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document);

	TXmlEngNode node;
	RXmlEngNodeList<TXmlEngElement> nodeList;
	node = document.DocumentElement();
	
	// Gets all the chidren elements of current node ( root element)
	node.AsElement().GetChildElements(nodeList);

	// Gets the no.of nodes in the list
	if(nodeList.Count()!=4)
	    User::Leave(KErrGeneral);
	CleanupClosePushL(nodeList);
	
	TXmlEngElement element;

	// Returns next node
	element = nodeList.Next();
	
	// Check the name of first element
	_LIT8(name,"elem1");
	if(element.Name().Compare(name))
	    User::Leave(KErrGeneral);

	nodeList.Next();
	nodeList.Next();

	// Checks whether there is a node
	if (nodeList.HasNext())
	{
	    element = nodeList.Next();
	    _LIT8(name1,"elem4");
	    if(element.Name().Compare(name1))
	        User::Leave(KErrGeneral);
	}
	
	CleanupStack::PopAndDestroy(); //nodeList
	CleanupStack::PopAndDestroy(); //document
}
