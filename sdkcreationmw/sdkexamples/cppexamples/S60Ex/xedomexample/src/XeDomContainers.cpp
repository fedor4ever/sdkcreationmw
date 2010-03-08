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


/*************************************************************
* This example demonstrates usage of internal (binary)&
* external (chunk) data containers provided by XML engine
* DOM. Creation of these containers, fetching the contents
* of these containers, setting the contens of these containers,
* specifying offsets,size in case of external containers,
* getting the list of data containers for the document
**************************************************************/
EXPORT_C void CXEDomEx::DataContainersExampleL()
{
    // usage of binary containers - subexample
	BinaryContainersL();
	
	// usage external containers - subexample
	ExternalContainersL();
}


void CXEDomEx::BinaryContainersL()
{
	_LIT8(KElementName,"doc");
	_LIT8(KTarget,"test");
	_LIT8(KComment,"test comment");
	_LIT8(KValue,"test value");
	_LIT8(KNamespace,"testNamespace");
	_LIT8(KPrefix,"testPref");
	_LIT8(KStringName,"doc");
	_LIT8(KStringValue,"ATTR_value");
	_LIT8(KStringText,"test content");
	_LIT8(KCData,"test < test2 & \r\n> ;");

	RXmlEngDocument document;
	document.OpenL(iDomImpl);
	CleanupClosePushL(document);

	// Preparing a document before we deal with binary containers
	TXmlEngElement docEle = document.CreateDocumentElementL( KElementName,KNullDesC8 ,KNullDesC8 );
	TXmlEngTextNode textNode = document.CreateTextNodeL(KStringText);
	docEle.AppendChildL(textNode);
	TXmlEngCDATASection cdNode = document.CreateCDATASectionL(KCData);
	docEle.AppendChildL(cdNode);
	TXmlEngAttr atNode = document.CreateAttributeL(KStringName, KStringValue);
	docEle.AppendChildL(atNode);
	TXmlEngComment commentNode = document.CreateCommentL(KComment);
	docEle.AppendChildL(commentNode);
	TXmlEngProcessingInstruction piNode = document.CreateProcessingInstructionL(KTarget, KValue);
	docEle.AppendChildL(piNode);
	TXmlEngElement elementNode = document.CreateElementL(KElementName, KNamespace, KPrefix);
	docEle.AppendChildL(elementNode);

	// Binary data to be read
	TBuf<KFilePathSize> filepath1(iDrive);
	_LIT(KFilename1, "\\xedomexample\\input\\test.doc");
	filepath1.Append(KFilename1);	
	
	TBuf<KFilePathSize> filepath2(iDrive);
	_LIT(KFilename2, "\\xedomexample\\input\\test.jpg");
	filepath2.Append(KFilename2);	
		
	HBufC8* str1;
	str1=ReadFileL(filepath1);
	CleanupStack::PushL(str1);
	
	HBufC8* str2;
	str2=ReadFileL(filepath2);
	CleanupStack::PushL(str2);

	_LIT8(data, "this is some kind of binary data");
	_LIT8(data1, "this is another kind of binary data 123");
	_LIT8(cid, "123456789@123456789");
	
	// Creates new binary container and copies the content string into it.
	// Pointer to the container is stored in the document's
	// data container list that can be fetched using GetDataContainerList().
	TXmlEngBinaryContainer binData = document.CreateBinaryContainerL(cid,data);

	// Sets contents of binary container
	binData.SetContentsL(data1);

	// Get content of the container.
	TPtrC8 fetchedData = binData.Contents();
	if(fetchedData.Compare(data1))
	    User::Leave(KErrGeneral);

	// Check data size in bytes
	if(data1().Length() != binData.Size())
	    User::Leave(KErrGeneral);
	
	_LIT8(cid2, "123456789@123456788");
	TXmlEngBinaryContainer binData1 = document.CreateBinaryContainerL(cid2, *str1);
	binData1.SetContentsL(*str2);

    _LIT8(cid3, "123456789@123456787");
	TXmlEngBinaryContainer binData2 = document.CreateBinaryContainerL(cid3, *str2);
	binData2.SetContentsL(*str1);

    RArray<TXmlEngDataContainer> binaryContainers;
    document.GetDataContainerList(binaryContainers);
    CleanupClosePushL(binaryContainers);
    
    if(binaryContainers.Count() != 3)
        User::Leave(KErrGeneral);
	
	CleanupStack::PopAndDestroy(); //binaryContainers
	CleanupStack::PopAndDestroy(str2);
	CleanupStack::PopAndDestroy(str1);
	CleanupStack::PopAndDestroy(); //document
}



void CXEDomEx::ExternalContainersL()
{
	_LIT8(KElementName,"doc");
	_LIT8(KTarget,"test");
	_LIT8(KComment,"test comment");
	_LIT8(KValue,"test value");
	_LIT8(KNamespace,"testNamespace");
	_LIT8(KPrefix,"testPref");
	_LIT8(KStringName,"doc");
	_LIT8(KStringValue,"ATTR_value");
	_LIT8(KStringText,"test content");
	_LIT8(KCData,"test < test2 & \r\n> ;");

	RXmlEngDocument document;
	document.OpenL(iDomImpl);
	CleanupClosePushL(document);

	// Preparing an example document before we deal with binary containers
	TXmlEngElement docEle = document.CreateDocumentElementL( KElementName,KNullDesC8 ,KNullDesC8 );
	TXmlEngTextNode textNode = document.CreateTextNodeL(KStringText);
	docEle.AppendChildL(textNode);
	TXmlEngCDATASection cdNode = document.CreateCDATASectionL(KCData);
	docEle.AppendChildL(cdNode);
	TXmlEngAttr atNode = document.CreateAttributeL(KStringName, KStringValue);
	docEle.AppendChildL(atNode);
	TXmlEngComment commentNode = document.CreateCommentL(KComment);
	docEle.AppendChildL(commentNode);
	TXmlEngProcessingInstruction piNode = document.CreateProcessingInstructionL(KTarget, KValue);
	docEle.AppendChildL(piNode);
	TXmlEngElement elementNode = document.CreateElementL(KElementName, KNamespace, KPrefix);
	docEle.AppendChildL(elementNode);

	_LIT8(cid, "123456789@123456789");
	_LIT8(cid1, "11111111@123456789");
	_LIT8(cid2, "22222222@123456789");
	_LIT8(cid3, "33333333@123456789");
	_LIT(chunkName, "test chunk");
	const TInt size = 1000;
	const TInt maxSize = 5000;
	const TInt offset = 100;
	const TInt binarySize = 500;

    _LIT8(KText, "binary containers");
	TXmlEngElement el = document.CreateElementL(KText);
	document.DocumentElement().AppendChildL(el);
	
	RChunk chunk;
	chunk.CreateGlobal(chunkName, size, maxSize);
	CleanupClosePushL(chunk);

	// Creates new chunk container that stores reference to memory chunk
	TXmlEngChunkContainer binData = document.CreateChunkContainerL(cid, chunk,offset, binarySize);
	el.AppendChildL(binData);
	TXmlEngChunkContainer binData1 = document.CreateChunkContainerL(cid1, chunk,offset, binarySize);
	el.AppendChildL(binData1);
	TXmlEngChunkContainer binData2 = document.CreateChunkContainerL(cid2, chunk,offset, binarySize);
	el.AppendChildL(binData2);
	TXmlEngChunkContainer binData3 = document.CreateChunkContainerL(cid3, chunk,offset, binarySize);
	el.AppendChildL(binData3);

	RArray<TXmlEngDataContainer> list;
	CleanupClosePushL(list);

	// Retrieves an array of data containers owned by this document.
	document.GetDataContainerList(list);
	if(list.Count()!=4)
	    User::Leave(KErrGeneral);
	if(list[0].Cid().Compare(cid)!=0)
	    User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(); //document
	CleanupStack::PopAndDestroy(); //chunk
	CleanupStack::PopAndDestroy(); //list
}
