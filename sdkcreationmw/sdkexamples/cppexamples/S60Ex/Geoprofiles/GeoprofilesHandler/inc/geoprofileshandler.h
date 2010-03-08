/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Framework Function declarations for Geo-Profiles.
*
*/


#ifndef __GEOPROFILESHANDLERUI_H
#define __GEOPROFILESHANDLERUI_H

/**
 * Initialization Function.
 */
LOCAL_C void callImplementationL(); 

/**
 * Handles the fired event. This function acts as a starting point for all
 * event action
 */
void HandleEventL();

TInt GetProfileIdofFiredTriggerL();

void ActivateProfileL(  TInt aProfileId );

#endif // __GEOPROFILESHANDLERUI_H