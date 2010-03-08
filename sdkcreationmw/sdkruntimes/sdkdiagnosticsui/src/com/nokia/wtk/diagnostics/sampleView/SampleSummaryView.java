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

import java.awt.BorderLayout;

import javax.swing.Icon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.event.ListDataListener;
import javax.swing.event.ListDataEvent;
import javax.swing.ListModel;
import javax.swing.SwingConstants;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.support.Misc;

/**
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class SampleSummaryView extends JPanel  {

  static {
    AppEnvironment.addResources("resources/SampleView.properties",
                                SampleSummaryView.class);
    }

    public SampleSummaryView(Browser br) {

      super();
      _imageCount = 0;
      setLayout(new BorderLayout());
      _viewTitle =
        new JLabel(_imageCount + TITLE_CONST,
                   AppEnvironment.getIcon(Resource.SAMPLE_SUMMARY_ICON),
                   SwingConstants.LEFT);
      add(_viewTitle, BorderLayout.CENTER);
      Misc.setOpaque(this, false);
      SampleModel sampleModel = SampleModel.getSampleModel(br);
      if (sampleModel != null) {
        _listModel = sampleModel.getDataModel();
        if (_listModel != null) {
          _listModel.addListDataListener (new ListDataListener() {
             public void contentsChanged (ListDataEvent event) {
               intervalAdded(event);
             }
             public void intervalAdded (ListDataEvent event) {
               _imageCount = _listModel.getSize();
               _viewTitle.setText (_imageCount + TITLE_CONST);
             }
             public void intervalRemoved(ListDataEvent event) {
               intervalAdded(event);
             }
           });
         }
       }
    }

    private static final String TITLE_CONST = " images in list";
    private JLabel _viewTitle = null;
    private int _imageCount = 0;
    private ListModel _listModel = null;
}
