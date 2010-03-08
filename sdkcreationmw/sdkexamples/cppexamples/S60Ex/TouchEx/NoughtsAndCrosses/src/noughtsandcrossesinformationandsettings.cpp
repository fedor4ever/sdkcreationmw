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



// INCLUDE FILES
#include <s32file.h>

#include "noughtsandcrossesinformationandsettings.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::CNoughtsAndCrossesInformationAndSettings()
// Default constructor.
// ----------------------------------------------------------------------------
//
CNoughtsAndCrossesInformationAndSettings::CNoughtsAndCrossesInformationAndSettings() : iPlayFirst (true),
	iPlayTypeCross (true), iHumanWins (0), iHumanLosses (0)
	{
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::SetPlayerName (const TPlayerName& aPlayerName)
// ----------------------------------------------------------------------------
//
void CNoughtsAndCrossesInformationAndSettings::SetPlayerName (const TPlayerName& aPlayerName)
	{
	iPlayerName = aPlayerName;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::PlayerName()
// Return the players name
// ----------------------------------------------------------------------------
//
CNoughtsAndCrossesInformationAndSettings::TPlayerName& CNoughtsAndCrossesInformationAndSettings::PlayerName()
	{
	return iPlayerName;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::SetHumanPlayFirst(TBool aPlayFirst)
// ----------------------------------------------------------------------------
//
void CNoughtsAndCrossesInformationAndSettings::SetHumanPlayFirst (TBool aPlayFirst)
	{
	iPlayFirst = aPlayFirst;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::HumanPlayFirst()
// Return ETrue if human play first
// ----------------------------------------------------------------------------
//
TBool& CNoughtsAndCrossesInformationAndSettings::HumanPlayFirst()
	{
	return iPlayFirst;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::SetHumanTypeCross(TBool aPlayTypeCross)
// ----------------------------------------------------------------------------
//
void CNoughtsAndCrossesInformationAndSettings::SetHumanTypeCross (TBool aPlayTypeCross)
	{
	iPlayTypeCross = aPlayTypeCross;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::HumanTypeCross()
// Return ETrue if human type is cross
// ----------------------------------------------------------------------------
//
TInt& CNoughtsAndCrossesInformationAndSettings::HumanTypeCross()
	{
	return iPlayTypeCross;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::AddHumanWins()
// ----------------------------------------------------------------------------
//
void CNoughtsAndCrossesInformationAndSettings::AddHumanWins()
	{
	++iHumanWins;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::HumanWins()
// Return human wins
// ----------------------------------------------------------------------------
//
TInt CNoughtsAndCrossesInformationAndSettings::HumanWins() const
	{
	return iHumanWins;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::AddHumanLosses()
// ----------------------------------------------------------------------------
//
void CNoughtsAndCrossesInformationAndSettings::AddHumanLosses()
	{
	++iHumanLosses;
	}

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::HumanLosses()
// Return human losses
// ----------------------------------------------------------------------------
//
TInt CNoughtsAndCrossesInformationAndSettings::HumanLosses() const
	{
	return iHumanLosses;
	}
	
// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::ExternalizeL (RWriteStream& aStream) const
// Write the player settings to stream.
// ----------------------------------------------------------------------------
//
void CNoughtsAndCrossesInformationAndSettings::ExternalizeL (RWriteStream& aStream) const
	{
	aStream << iPlayerName;
	aStream.WriteInt32L (iPlayFirst);
	aStream.WriteInt32L (iPlayTypeCross);
	aStream.WriteInt32L (iHumanWins);
	aStream.WriteInt32L (iHumanLosses);
	}
		
// ----------------------------------------------------------------------------
// CNoughtsAndCrossesInformationAndSettings::InternalizeL (RReadStream& aStream)
// Read the contents of aStream.
// ----------------------------------------------------------------------------
//
void CNoughtsAndCrossesInformationAndSettings::InternalizeL (RReadStream& aStream)
	{
	aStream >> iPlayerName;
	iPlayFirst = aStream.ReadInt32L();
	iPlayTypeCross = aStream.ReadInt32L();
	iHumanWins = aStream.ReadInt32L();
	iHumanLosses = aStream.ReadInt32L();
	}

// End of File
