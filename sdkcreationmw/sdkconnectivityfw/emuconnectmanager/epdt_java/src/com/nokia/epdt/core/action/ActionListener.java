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
 * ActionListener
 *
 * The listener interface for receiving action events.
 * The class that is interested in processing an action event implements this interface,
 * and the object created with that class is registered with a component, using the component's
 * addActionListener method. When the action event occurs, that object's actionPerformed method is invoked.
 */
public interface ActionListener {
    public static final int CORE = 0;
    public static final int PLUGIN = 1;
    
    /**
     * Invoked when an action occurs.
     */
    public void actionPerformed(Action action);
    
}
