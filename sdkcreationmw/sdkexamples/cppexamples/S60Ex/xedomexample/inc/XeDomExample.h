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



#ifndef XEDOMEXAMPLE_H
#define XEDOMEXAMPLE_H

#include <e32test.h>
#include <f32file.h>

//XmlEng headers
#include <xmlengdom.h>
#include <XmlEngErrors.h>

// Size of file path names used in this example
const TInt KFilePathSize = 128;

/**
 * The XML Engine DOM Example Application Class
 */

class CXEDomEx : public CBase
{

	public:

	/**
	 * High level Use cases demonstrated in this application
	 */
	enum TMenuItems
	{
		EQuit = 0,
		EParsing,
		ESave,
		EDocProcess,
		ENsProcess,
		EAttributes,
		EElements,
		EContainer
	};


    /**
     * Construction functions
     */
	virtual ~CXEDomEx();
	static CXEDomEx* NewLC();
	static CXEDomEx* NewL();
	
    /**
     * Function to start the application
     */
	void StartExampleL();


	/**
	 * Main example functions
	 */


	/**
	 * Demonstrates a simple parsing of an XML file and
	 * return the document in tree form. Demonstrates
	 * parsing a file as well as a buffer
	 */
	void ParseExampleL();

    /**
	 * Demonstrates the saving
	 * of an XML document onto
	 * a given file or buffer
	 */
	void SaveExampleL();

	/**
	 * Demonstrates the creation / buidling of a document from scratch.
	 * Also shows how to deal with the different nodes
	 * in the document and operations on a node list that can be done.
	 */
	void DocProcessingExampleL();

	/**
	 * This example demonstrates how namespaces can be dealt
	 * with in XML Engine DOM. It covers looking up for namespace
	 * URIs, prefixes, retrieving the default namespace uris,
	 * checking for NS URIs for a given prefix, creating a NS
	 * prefix & URI, addding a NS declaration onto an element etc.
	 */
	void NsProcessingExampleL();

	/**
	 * This example demonstrates how xml engine DOM deals
	 * with attributes. It shows checking an element whether
	 * it has an attribute, getting/setting attribute name/values,
	 * adding/removing attributes, creating/deleting an attribute
	 * node, copying attributes, attributes with namespaces etc.
	 */
	void AttributesExampleL();

	/**
	 * This example deals with element class of XML engine DOM
	 * It shows adding of new elements with/without namespaces,
	 * creating elements with auto prefixes, removal of element
	 * nodes, setting content text for elements (both raw and
	 * escaped), fetching elements by their name, removing NS
	 * declarations of an elements etc.
	 */
	void ElementsExampleL();


	/**
	 * This example demonstrates usage of internal (binary)&
	 * external (chunk) data containers provided by XML engine
	 * DOM. Creation of these containers, fetching the contents
	 * of these containers, setting the contens of these containers,
	 * specifying offsets,size in case of external containers,
	 * getting the list of data containers for the document
	 */
	void DataContainersExampleL();


	public:
    /**
	 * Sub example functions
	 *(Used by main example functions)
	 */
	 
	/**
     * Creating a new xml document from scratch
     */
	void CreateDocumentL();
	
	/**
     * Perform some sibling operations, e.g. move around the position of sibling nodes
     */	
	void SiblingOperationsL();
	
	/**
     * Shows how to use NodeList class
     */	
	void NodeListOperationsL();

	/**
     * Perform basic operation of attributes, i.e. getting/setting their values, removing etc
     */
    void AttributeOperationsL();
	
	/**
     * Shows how to add attributes in different ways
     */	
	void AddAttributesL();

	/**
     * Shows adding xml elements in different ways
     */
	void AddRemoveElementsL();
	
	/**
     * Shows different ways of changing element's text
     */
	void SetElementTextL();

	/**
     * Shows usage of internal binary containers within XML document tree
     */
	void BinaryContainersL();
	
	/**
     * Shows usage of external binary containers i.e filec ontainers,
     * chunk containers, within XML document tree
     */	
	void ExternalContainersL();
	
	/**
     * Drive letter for input testdata
     */	
	TBuf<4> iDrive;

	private:
	/**
	 * Internal support functions
	 */
	CXEDomEx(const TDesC& aTestName);
	void ConstructL();

	/**
	 * Retrieves the installation drive letter and stores for future use,
	 * required for filepath names for test input/output files.
	 */
    void CXEDomEx::GetDriveLetterL();
    
	/**
	 * Used to present the user with a list of options (ourPrompt),
	 * and get their selection. The user selection is validated against
	 * the choices (validChoices)
	 */
	TInt GetSelection(const TDesC& ourPrompt, const TDesC& validChoices);
	
	/**
	 * Displays the message "Press any key..." on screen and waits for any key press
	 * from user.
	 */	
	void PressAnyKey();
	
	/**
	 * Reads the file contents from given file and returns the contents in heap buffer
	 */	
	static HBufC8* ReadFileL(const TDesC& aFileName);

    // application console window
	RTest iTest;

	// DOM Implementation, needed for initializing XmlEngine library once
	RXmlEngDOMImplementation iDomImpl;
	
	// The DOM Parser, used for parsing documents
	RXmlEngDOMParser iDomParser;
};


#endif // #ifndef XEDOMEXAMPLE_H