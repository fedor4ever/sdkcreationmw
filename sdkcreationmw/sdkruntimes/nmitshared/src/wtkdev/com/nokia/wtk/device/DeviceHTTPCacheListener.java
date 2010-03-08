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
 * A listener for HTTP Cache events.
 * This interface will be implemented by the hosting application.
 *
 */
public interface DeviceHTTPCacheListener {

    /**
     * Called when a new item has been added to the content cache.
     * @param url       the URL of the item that was added
     */
    public void cacheItemAdded(String url);

    /**
     * Called when an existing item has been replaced in the content cache.
     * @param url       the URL of the item that was changed
     */
    public void cacheItemChanged(String url);

    /**
     * Called when an item has been removed from the content cache.
     * @param url       the URL of the item that was removed
     */
    public void cacheItemRemoved(String url);

    /**
     * Called when all cached content has been deleted.
     */
    public void cacheCleared();

}   // End of DeviceHTTPCacheListener
