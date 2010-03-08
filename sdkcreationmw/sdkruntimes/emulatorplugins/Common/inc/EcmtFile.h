/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for file manipulation
*
*/



#ifndef ECMTFILE_H
#define ECMTFILE_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KMaxWin32Path = 512;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*  @ingroup EcmtPlugins
*/
class REcmtFile
    {
    public:  // Constructors and destructor

	    /**
	    * Constructor that gets the reference to filename
	    */
	    REcmtFile( TDesC& aFileName );

	    /**
	    * Destructor.
	    */
	    ~REcmtFile( );
	    
    public: // New functions

	  TBool Read( );
		TBool Write( );
		void Close( );
		const TPtrC8 FindLine( const TDesC8& aFind );
		TBool ReplaceLine( const TDesC8& aOld, const TDesC8& aNew );
		TBool DeleteLine( const TDesC8& aOld );
		TBool InsertLine( const TDesC8& aLine );
		inline TBool IsGood() { return iGood; }
		TBool Create( );
		TBool Delete( );
		TBool Exists( );
		TBool ReadOnly( );
		TBool SetReadOnly( );
		TBool ResetReadOnly( );	
    
    private: // Private
    /**
    * C++ default constructor.
    */
	   	REcmtFile();


    protected:    // Data
    	int					iGood;
    	unsigned char*		iData;
    	void*				iMemHandle;
    	unsigned long		iLen, iMaxLen;
    	void*				iHandle;
    	TDesC&				iName;
    };

#endif      
        
// End of File
