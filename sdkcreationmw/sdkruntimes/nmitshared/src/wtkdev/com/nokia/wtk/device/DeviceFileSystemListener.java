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

import com.nokia.wtk.device.DeviceFile;

/**
 * This interface provides an abstraction for File System Event.
 *
 */

public interface DeviceFileSystemListener {

    /**
     * Called when a new filesystem, a directory, or a file has been created. 
     * 
     *
     * @param file      a unique reference to a DeviceFile that has been created.
     */
   public void fileAdded (DeviceFile file);

    /**
     * Called when a filesystem, a directory, or a file has been deleted. 
     *
     * @param file      a unique reference to a DeviceFile that has been deleted.
     */
   public void fileDeleted (DeviceFile file);

    /**
     * Called when a file has been updated (for example, the size of the file changed).
     * Reserved for future use.
     *
     * @param file      a unique reference to a DeviceFile that has been updated.
     */
   public void fileUpdated (DeviceFile file);

    /**
     * Called when a file has been moved.
     * Reserved for future use.
     *
     * @param file      a unique reference to a DeviceFile that has been moved.
     * @param oldParent a unique reference to a DeviceFile was the old parent 
     *                  directory before the move.
     */
   public void fileMoved(DeviceFile file, DeviceFile oldParent);


    /**
     * Called when a file has been renamed.
     * Reserved for future use.
     *
     * @param file      a unique reference to a DeviceFile that has been renamed.
     * @param oldName   the old name of the file 
     */
   public void fileRenamed(DeviceFile file, String oldName);


}

