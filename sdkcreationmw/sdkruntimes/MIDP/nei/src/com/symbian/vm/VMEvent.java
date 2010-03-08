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

package com.symbian.vm;

import java.util.EventObject;

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

public class VMEvent {

  //
  // Constants
  //

  /**
   * VM started event type
   */
  public static final int VM_STARTED = 0;

  /**
   * VM stoped event type
   */
  public static final int VM_STOPED = 1;

  //
  // Members
  //

  /**
   * Event type
   */
  private final int iType;

  //
  // Life cycle
  //

  /**
   * Constructor.
   *
   * @param aType event type
   */
  public VMEvent(int aType) {
    iType = aType;
  }

  //
  // Operations
  //

  /**
   * Get the event type
   *
   * @return event type
   */
  public int getType(){
    return iType;
  }

}