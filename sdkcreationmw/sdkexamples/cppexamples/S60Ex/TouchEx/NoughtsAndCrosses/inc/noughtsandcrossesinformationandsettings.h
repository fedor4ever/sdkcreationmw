/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __NOUGHTSANDCROSSESINFORMATIONANDSETTINGS_H__
#define __NOUGHTSANDCROSSESINFORMATIONANDSETTINGS_H__

// INCLUDES
#include <s32std.h>


// CONSTANTS
const TInt KUsernameBufSize = 20;

// CLASS DECLARATION
class CNoughtsAndCrossesInformationAndSettings : public CBase
	{
	public:
		typedef TBuf<KUsernameBufSize> TPlayerName;

	public:
		/**
		 * Constructor
		 */
		CNoughtsAndCrossesInformationAndSettings();

		/**
		 * Set player name 
		 */
		void SetPlayerName (const TPlayerName& aPlayerName);
		/**
		 * PlayerName()
		 * Return player name
		 */
		TPlayerName& PlayerName();
		/**
		 * SetHumanPlayFirst(TBool aPlayFirst)
		 */
		void SetHumanPlayFirst (TBool aPlayFirst);
		/**
		 * HumanPlayFirst()
		 * Return ETrue if human play first
		 */
		TBool& HumanPlayFirst();
		/**
		 * SetHumanTypeCross(TBool aPlayTypeCross)
		 */
		void SetHumanTypeCross (TBool aPlayTypeCross);
		/**
		 * HumanTypeCross()
		 * Return ETrue if human type is cross
		 */
		TBool& HumanTypeCross();
		/**
		 * AddHumanWins()
		 */
		void AddHumanWins();
		/**
		 * HumanWins()
		 * Return human wins
		 */
		TInt HumanWins() const ;
		/**
		 * AddHumanLosses()
		 */
		void AddHumanLosses();
		/**
		 * HumanLosses()
		 * Return human losses
		 */
		TInt HumanLosses() const;
		/**
		 * ExternalizeL(RWriteStream& aStream)
		 * Write the player settings to stream
		 */
		void ExternalizeL (RWriteStream& aStream) const;
	    /**
	     * InternalizeL(RReadStream& aStream)
		 * Read the contents of aStream.
		 */	
		void InternalizeL (RReadStream& aStream);

	private:
		/**
		 * Make a private copy constructor, so that the compiler will not create
		 * a default one. This way a copy of this object cannot be created accidentally.
		 */
		CNoughtsAndCrossesInformationAndSettings (
			const CNoughtsAndCrossesInformationAndSettings& aCopy);
		/**
		 * Define the assignment operator to be private, so that the compiler will not create
		 * a default one. We do not want assingments to be made between two objects of this class.
		 */
		CNoughtsAndCrossesInformationAndSettings& operator= (
			const CNoughtsAndCrossesInformationAndSettings& aOther);

	private:
		TPlayerName iPlayerName;
		TBool iPlayFirst;
		TBool iPlayTypeCross;
		TInt iHumanWins;
		TInt iHumanLosses;
	};

#endif // __NOUGHTSANDCROSSESINFORMATIONANDSETTINGS_H__

// End of File

