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

package com.symbian.io;

import java.io.IOException;
import java.io.DataInputStream;

public interface Loadable {

  /**
   * Load the object from the stream
   *
   * @param aDis data input stream
   * @throws IOException if some error occured
   */
  public void load(DataInputStream aDis) throws IOException;
}