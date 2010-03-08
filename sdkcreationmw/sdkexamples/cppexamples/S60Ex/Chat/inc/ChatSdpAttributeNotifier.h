/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CHATSDPATTRIBUTENOTIFIER_H
#define CHATSDPATTRIBUTENOTIFIER_H

// FORWARD DECLARATIONS
class CSdpAttrValue;

// CLASS DECLARATIONS
/**
* MChatSdpAttributeNotifier
* An instance of MChatSdpAttributeNotifier is used to read selected SDP Attribute
* elements from an attribute value.
*/
class MChatSdpAttributeNotifier
    {
    public: // New functions

        /**
        * FoundElementL
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, CSdpAttrValue& aValue ) = 0;
    };

#endif // CHATSDPATTRIBUTENOTIFIER_H

// End of File
