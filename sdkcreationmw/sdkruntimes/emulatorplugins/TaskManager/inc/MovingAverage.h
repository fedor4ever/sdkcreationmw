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
* Description:  Class for calculating moving average
*
*/



#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Template class for calculating moving average.
*  @ingroup EcmtPlugins
*/
template <class T>
class CMovingAverage : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
		static CMovingAverage<T>* NewL( TInt aLen );

        /**
        * Destructor.
        */
		virtual ~CMovingAverage();

    public: // New functions
    	/**
    	* Insert a value into the moving average calculation.
    	* @param iSample value to insert.
    	*/

		void Insert(T iSample);

		/**
		* Returns the current moving average value.
		*/
		T Value();
        
    private:

        /**
        * C++ default constructor.
        */
        CMovingAverage();
        
        /**
        * Constructor that sets the length of moving average buffer.
        */
        CMovingAverage( TInt aLen );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:
    	/// Array that stores the samples
		T* iValues;
		/// Current insert position.
		TInt iCurpos;
		/// Length of buffer.
		TInt iLen;
		/// Sum of entries currently in buffer.
		T iSum;
		/// True if at least one full buffer of samples has been
		/// inserted.
		TBool iReady;
    };

#endif
            
// End of File
