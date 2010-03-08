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


#ifndef CHATSERVICESEARCHER_H
#define CHATSERVICESEARCHER_H

// INCLUDES
#include <e32base.h>
#include "ChatBtServiceSearcher.h"
#include "Log.h"

// CLASS DECLARATIONS
/**
* CChatServiceSearcher
* Searches for a chat service.
*/
class CChatServiceSearcher : public CChatBtServiceSearcher
    {
    public:     // Constructors and destructor

        /*
        * NewL()
        * Create a CChatServiceSearcher object
        * @param aLog the log to send output to
        * @return a pointer to the created instance of 
        * CChatServiceSearcher
        */
        static CChatServiceSearcher* NewL( MLog& aLog );

        /**
        * NewLC()
        * Create a CChatServiceSearcher object
        * @param aLog the log to send output to
        * @return a pointer to the created instance of 
        * CChatServiceSearcher
        */
        static CChatServiceSearcher* NewLC( MLog& aLog );

        /**
        * ~CMessageServiceSearcher()
        * Destroy the object and release all memory objects.
        */
        virtual ~CChatServiceSearcher();
       

    public: // New function

        /**
        * Port()
        * Retrieve the port on which the service is installed
        * @return the port number
        */
        TInt Port();

    protected:  // New functions

        /**
        * ServiceClass()
        * @return the service class uid.
        */
        const TUUID& ServiceClass() const;

        /**
        * FoundElementL()
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, CSdpAttrValue& aValue );

        /**
        * ProtocolList()
        * @return the attribute list.
        */
        RArray<TChatSdpAttributeParser::TChatSdpAttributeNode>& ProtocolList();
      
    private:    // Constructors

        /**
        * CChatServiceSearcher()
        * Constructs this object
        * @param aLog the log to send output to
        */
        CChatServiceSearcher( MLog& aLog );

        /**
        * ConstructL()
        * Performs second phase construction of this object
        */
        void ConstructL();

    private:    // data

        /** 
        * iServiceClass 
        * the service class UUID to search for 
        */
        TUUID iServiceClass;

        /** 
        * iPort
        * the port number that the remote service is installed 
        */
        TInt iPort;
        
        /**
        * iProtocolArray
        * the attribute list 
        */
        RArray<TChatSdpAttributeParser::TChatSdpAttributeNode> iProtocolArray;
    };

#endif // CHATSERVICESEARCHER_H

// End of File
