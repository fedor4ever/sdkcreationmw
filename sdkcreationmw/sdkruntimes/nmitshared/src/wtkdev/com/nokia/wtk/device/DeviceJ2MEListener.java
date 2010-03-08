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
 * This interface provides an abstraction for J2ME events.
 *
 */

public interface DeviceJ2MEListener {

  /* KVM memory management */  

    /**
     * Called when a the available memory in KVM changes. 
     * Note, that it is too expensive to fire this event 
     * every time, when there is a memory allocation or deallocation in the KVM. 
     * The device is resonsible for fireing this event in a reasonable time intervall
     * (e.g. in every 1 or 2 seconds).
     * 
     * @param file      a reference to the DeviceMemory, that gives a memory snapshot 
     *                  of the KVM.
     */
  public void KVMMemoryChanged (DeviceMemory memory);
}
