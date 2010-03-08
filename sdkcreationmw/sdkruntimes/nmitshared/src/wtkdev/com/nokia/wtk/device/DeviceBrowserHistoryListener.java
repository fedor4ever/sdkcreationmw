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




package com.nokia.wtk.device;


/**
 * A listener for browser history events.
 * This interface will be implemented by the hosting application.
 *
 */
public interface DeviceBrowserHistoryListener {

    /**
     * Called when a new document URL has been pushed onto the history stack.
     * It is expected that the most recently received content, whose request URL
     * matches the URL parameter in this method, is the same content that was pushed.
     * @param url       the request URL of the item that was pushed onto the history stack.
     */
    public void historyPushed(String url);

    /**
     * Called when a document has been reloaded,
     * either explicitly or as a side-effect of history navigation.
     * It is expected that the most recently received content, whose request URL
     * matches the URL parameter in this method, is the same content that was reloaded.
     * @param url       the request URL of the item that was reloaded.
     */
    public void historyRefresh(String url);

    /**
     * Called when the top of the history stack has been popped.
     */
    public void historyPopped();

    /**
     * Called when history stack position index has been decremented without a 'pop'.
     */
    public void historyBack();

    /**
     * Called when history stack position index has been incremented without a 'push'.
     */
    public void historyForward();

    /**
     * Called when the history stack has been cleared.
     */
    public void historyCleared();

    /**
     * Called when the bottom-most item in the history stack has been removed.
     */
    public void historyTrimmed();

}   // End of DeviceBrowserHistoryListener
