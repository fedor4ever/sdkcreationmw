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


package com.nokia.wtk.sdkSimulator.diagnosticsAdapter;

import java.io.Serializable;

/**
 * <p>Title: PropertyListener</p>
 * <p>Description: an interface to listen to changes in SDK properties</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */

public interface PropertyListener {
  public void propertyChanged( String property, Serializable value );
}
