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




/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */

public class MIDletSuiteAMSArguments extends Arguments{

  //
  // Constants
  //

  /**
   * System property
   */
  protected static final String PROPERTY_ARG = "-D";




  //
  // Life cycle
  //

  public MIDletSuiteAMSArguments() {
  }


  //
  // Operations
  //

  /**
   * Set system property
   *
   * @param aKey property key
   * @param aValue property name
   */
  public void setSystemProperty(String aKey, String aValue)
  {
    iArguments.append(" ").append(PROPERTY_ARG).append(aKey)
        .append("=").append(aValue);
  }



}