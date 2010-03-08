/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.epdt.core.action;

/**
 * ActionEventQueue
 */
public interface ActionEventQueue {
    public boolean put(Action action);
    public Action getAction() throws InterruptedException;
    public int size();
}
