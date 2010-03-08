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


#include "XeDomExample.h"


/**************************************************
* Demonstrates a simple parsing of an XML file and
* return the document in tree form. Demonstrates
* parsing a file as well as a buffer
***************************************************/

EXPORT_C void CXEDomEx::ParseExampleL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename,"\\xedomexample\\input\\XML_parsing_001.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);
	
	// The API for parsing a file, returns a handle to document
	document = iDomParser.ParseFileL(srcFileName); 	                                                   
	if(document.NotNull())
	{
	    // We have got the parsed dom tree,it can be used to access tree contents
	    // using document APIs.
	    TPtrC8 name = document.DocumentElement().Name();
	    
	    // finally closed by calling Close()
		document.Close();
	}


	// Parse a buffer
	_LIT8(docW, "<doc>xmlns=\"http://test.pl\"><test>Test Content</test></doc>" );
	RXmlEngDocument document2;
	TPtrC8 ptrW(docW);

    // The API for parsing a buffer, returns a handle to document
	document2 = iDomParser.ParseL(ptrW);
	if(document2.NotNull())
	{
	    // Use the document tree and close it finally
		document2.Close();
	}

}

/*************************
* Demonstrates the saving
* of an XML document onto
* a given file or buffer
**************************/

EXPORT_C void CXEDomEx::SaveExampleL()
{
	RXmlEngDocument document;
	TBuf<KFilePathSize> filepath(iDrive);
	_LIT(KFilename,"\\xedomexample\\input\\XML_parsing_001.xml");
	filepath.Append(KFilename);
	TFileName srcFileName(filepath);
		
	// Need to have a document tree for saving it
    // One way is get a parsed document
	document = iDomParser.ParseFileL(srcFileName);
	CleanupClosePushL(document); 
	// The document can be modified now ...
	
    //Save it to a file
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\output\\XML_save_file.xml");
	filepath2.Append(KFilename2);
	document.SaveL(filepath2); // The API to save (serialize) the document onto a file

	//Save to a buffer
	_LIT8(encoding,"UTF-8");
	TXmlEngSerializationOptions options(0, encoding);
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	
	// The API to save (serialize) the document onto a buffer
	// It can take different serialization options
	document.SaveL( buffer, document, options); 
	                                               
	CleanupStack::PopAndDestroy(); //buffer
	CleanupStack::PopAndDestroy(); //document
}
