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



#ifndef ECMTSEEKFILE_H
#define ECMTSEEKFILE_H

//  INCLUDES
#include "EcmtFile.h"

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*  @ingroup EcmtPlugins
*/
class REcmtSeekFile: public REcmtFile
    {
    public:  // Constructors and destructor

	    /**
	    * Constructor that gets the reference to filename
	    */
	    REcmtSeekFile( TDesC& aFileName );

	    /**
	    * Destructor.
	    */
	    ~REcmtSeekFile( );
	    
    public: // New functions
//		const TPtrC8 FindNextLine( const TDesC8& aFind );
			const TPtrC8 NextLine( );
			void ResetPosition( );
	  	void SkipLines( TInt aLines );
	  	TBool HasNext();
    
    private: // Private
    /**
    * C++ default constructor.
    */
	   	REcmtSeekFile();

    private:    // Data
    	TLex8				iLexer;
    };

#endif      
        
// End of File
