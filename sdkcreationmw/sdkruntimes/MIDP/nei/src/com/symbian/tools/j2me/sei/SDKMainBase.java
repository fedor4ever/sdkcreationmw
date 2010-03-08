/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei;


import com.symbian.tools.j2me.sei.SDKRuntimeProperties;

import com.symbian.utils.Debug;


/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

public abstract class SDKMainBase {
  

  /**
   * Initialize debug logs
   *
   * @param aArgs command line arguments
   */
  protected static void initDebugLogs(String[] aArgs){
    //init debug log folder
    Debug.init(SDKRuntimeProperties.SDK_HOME + java.io.File.separator + "logs");

    //ADD to history log file
    Debug.append("history",
                 "\r\n" +
                 new java.util.Date(System.currentTimeMillis()).toString()
                 + ":\r\n");
    Debug.append("history", aArgs);
    Debug.append("history", "\r\n");
  }


}