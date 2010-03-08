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




package com.nokia.wtk.diagnostics.sampleView;

// javax.swing
import javax.swing.ListModel;
import javax.swing.DefaultListModel;

// com.nokia.wtk.app
import com.nokia.wtk.app.AppEnvironment;

// com.nokia.wtk.device
import com.nokia.wtk.device.DeviceSampleListener;
import com.nokia.wtk.device.DeviceSampleData;

// com.nokia.wtk.util
import com.nokia.wtk.util.MessageWriters;

// com.nokia.wtk.diagnostics.browser
import com.nokia.wtk.diagnostics.browser.Browser;

/**
 *
 * A class to provide the data model for the sample view...
 */
class SampleModel   {

  static {
    AppEnvironment.addResources("resources/SampleModel.properties",
                                SampleModel.class);
    }

    private SampleModel(Browser b) {
      _listModel = new DefaultListModel();
      _supports =  b.registerHostService (DeviceSampleListener.class,
                                          getSampleListener());
    }

    private DeviceSampleListener getSampleListener() {
      if (_sampleListener == null) {
        _sampleListener = new DeviceSampleListener() {
          public void sampleEventGenerated(DeviceSampleData event) {
            _listModel.addElement (event.getSampleContent());
          }
        };
      }
      return _sampleListener;
    }

    /**
     * Get the data model
     * @return the data model
     */
    ListModel getDataModel() {
      return _listModel;
    }

    /**
     * Get the sample model
     * @return SampleModel
     */
    static SampleModel getSampleModel(Browser b) {
      if (_sampleModel == null) {
        _sampleModel = new SampleModel(b);
      }
      return _sampleModel;
    }

    /**
     * Returns whether the sample functionality is supported
     * @return boolean true if supported, false otherwise
     */
    boolean isSampleSupported() {
      return _supports;
    }

    private DeviceSampleListener _sampleListener = null;;
    private final boolean _supports;
    private  DefaultListModel _listModel = null;
    private  static SampleModel _sampleModel = null;
}