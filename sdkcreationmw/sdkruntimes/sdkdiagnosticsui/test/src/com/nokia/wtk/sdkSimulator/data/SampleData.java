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


package com.nokia.wtk.sdkSimulator.data;

import java.io.Serializable;
import com.nokia.wtk.device.DeviceSampleData;

/**
 * <p>Title: SampleData</p>
 * <p>Description: A class that contains the data content for
 * sample events/p>
 * <p>Company: Nokia</p>
 */
public class SampleData implements Serializable, DeviceSampleData {

  /**
   * Constructor
   * @param data the sample data
   */
  public SampleData (Object data) {
    this.data = data;
  }

  /**
   * Returns the data content.
   * @return the data content.
   */
  public Object getSampleContent() {
    return data;
  }

  /**
   * the data content of the sample event
   */
  private Object data = null;
}
