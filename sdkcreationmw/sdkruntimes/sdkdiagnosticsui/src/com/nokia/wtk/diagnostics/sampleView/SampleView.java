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

// java.awt
import java.awt.BorderLayout;
import java.awt.Component;

// javax.swing
import javax.swing.Icon;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.ListModel;
import javax.swing.ListCellRenderer;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.BorderFactory;
import javax.swing.SwingConstants;

// com.nokia.wtk.app
import com.nokia.wtk.app.AppEnvironment;

// com.nokia.wtk.util.gui
import com.nokia.wtk.util.gui.MenuButton;

// com.nokia.wtk.diagnostics.browser
import com.nokia.wtk.diagnostics.browser.Browser;

// com.nokia.wtk.diagnostics.views
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.support.Misc;

/**
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class SampleView extends JPanel implements ViewAndControlsInterface {

  static {
    AppEnvironment.addResources("resources/SampleView.properties",
                                SampleView.class);
    }

  // constructor
  public SampleView(Browser br) {

    setLayout (new BorderLayout());

    // set title in a panel
    JLabel viewTitle =
      new JLabel(AppEnvironment.getString(Resource.SAMPLE_DETAILED_TITLE),
                 DETAILED_ICON,
                 SwingConstants.LEFT);
    JPanel titlePanel = new JPanel ();
    titlePanel.add(viewTitle);

    // create list and put in a scroll panel
    _sampleModel = SampleModel.getSampleModel(br);
    _sampleList = new JList(_sampleModel.getDataModel());
    _sampleList.setCellRenderer(new SampleCellRenderer());
    add(titlePanel, BorderLayout.NORTH);
    add(new JScrollPane(_sampleList), BorderLayout.CENTER);

    Misc.setOpaque(this, false);
  }

  // implementation of ViewAndControlsInterface
  public MenuButton getMenuButton() {
      return null;
  }

  public JComponent getControlsPanel() {
    return null;
  }

  public JComponent getPodContent() {
    return this;
  }

  private final static Icon DETAILED_ICON =
    AppEnvironment.getIcon(Resource.SAMPLE_DETAILED_ICON);

  private SampleModel _sampleModel = null;
  private JList _sampleList = null;


  // Private class SampleCellRenderer
  // Displays an icon and a string for each object in the list.


   class SampleCellRenderer extends JLabel implements ListCellRenderer {



     // Set the text and icon in JLabel each time we're called.
     public Component getListCellRendererComponent(JList list,
                                                   Object value,
                                                   int index,
                                                   boolean isSelected,
                                                   boolean cellHasFocus) {

       setText("image " + (index + 1));
       if (value instanceof Icon) {
         setIcon((Icon) value);
       }
       return this;
     }
   } // end of SampleCellRenderer class

}
