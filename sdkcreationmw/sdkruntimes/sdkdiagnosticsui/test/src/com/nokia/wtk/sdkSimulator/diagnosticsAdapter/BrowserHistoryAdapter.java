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


/*
 * Created on Aug 20, 2004
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package com.nokia.wtk.sdkSimulator.diagnosticsAdapter;

// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceBrowserHistoryListener;


/**
 * <p>Title: BrowserHistoryAdapter</p>
 * <p>Description: Specifies the APIs bewteen the SDK and an adapter object for
 *  browser history events.
 *  For this simple implementation of the diagnostics adapter, the interface
 *  just extends the DeviceBrowserHistoryListener interface with no additions.
 *  However, this doesn't have to be the case: DeviceBrowserHistoryListener is
 *  the interface to the Diagnostics component itself, while BrowserHistoryAdapter
 *  is the interface between the SDK and the diagnostics adapter. The two interfaces
 *  may be different. </p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */
public interface BrowserHistoryAdapter extends DeviceBrowserHistoryListener {
}



