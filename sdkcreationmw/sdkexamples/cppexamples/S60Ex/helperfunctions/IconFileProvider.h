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


#ifndef ICONFILEPROVIDER_H
#define ICONFILEPROVIDER_H

#include <akniconutils.h>

/**
*  CIconFileProvider class.
*  This class is used to provide a icon file handle to MAknIconFileProvider.
*  With this class it is possible to use icons from application's private
*  folder.
*/
class CIconFileProvider : public CBase,
                          public MAknIconFileProvider
    {

    public:

    	/**
        * Two-phased constructor.
        * @param aSession File server session.
        * @param aFilename Full path to the icon file
        */
    	static CIconFileProvider* NewL(RFs aSession, const TDesC& aFilename);

    	/**
        * Destructor.
        */
    	~CIconFileProvider();

    private: //from MAknIconFileProvider

    	/**
        * Returns an open file handle to the icon file.
        * @param aFile Icon file should be opened in this file handle, which
        *              is an empty file handle, when the AknIcon framework
        *              calls this method. The AknIcon framework takes care of
        *              closing the file handle after having used it.
        * @param aType Icon file type.
        */
    	void RetrieveIconFileHandleL( RFile& aFile, const TIconFileType aType );

    	/**
        * With this method, AknIcon framework informs that it does not use
        * this MAknIconFileProvider instance any more.
        */
        void Finished();

    private:

        /**
        * C++ default constructor.
        * @param aSession. File server session.
        */
    	CIconFileProvider(RFs aSession);

        /**
        * 2nd phase constructor.
        * @param aFilename. Full path to the icon file.
        */
        void ConstructL( const TDesC& aFilename);

    private: // Data

        /**
        * File server
        */
        RFs iSession;

        /**
        * Icon file name.
        *
        * Owned.
        */
        HBufC* iFilename;
    };

#endif // ICONFILEPROVIDER
