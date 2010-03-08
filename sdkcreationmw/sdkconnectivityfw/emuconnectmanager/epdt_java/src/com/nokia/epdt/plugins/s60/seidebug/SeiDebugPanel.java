/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MIDP debugging
 *
*/


package com.nokia.epdt.plugins.s60.seidebug;

/* java.util */
import java.util.List;
import java.util.ArrayList;

/* java.awt */
import java.awt.Font;
import java.awt.Component;
import java.awt.BorderLayout;

/* javax.swing */
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JComponent;

/* javax.swing.border */
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.border.CompoundBorder;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.PropertyEditors;

/* com.nokia.wtk.util.gui */
import com.nokia.wtk.util.gui.GUI;

/* com.symbian.tools.j2me.sei.preferences */
import com.symbian.tools.j2me.sei.preferences.KDPPreferences;

/**
 * UI panel for MIDP debug preferences
 */
class SeiDebugPanel extends JPanel implements UIConstants {

    private static final int GAP = 6;
    private static final Border EMPTY = new EmptyBorder(0,GAP,GAP,GAP);

    /**
     * Creates <tt>SeiDebugJPanel</tt>
     * @param ps properties to edit
     */
     SeiDebugPanel(PropertySet ps) {
         super(new BorderLayout());
         List l = new ArrayList();
         List r = new ArrayList();
         add(GUI.createRowPanel(new Component[] {
             createEmulatorPanel(ps, l, r), null,
             createDevicePanel(ps, l, r), null,
         }), BorderLayout.CENTER);
         GUI.maximizeWidth((JComponent[])l.toArray(new JComponent[l.size()]));
         GUI.maximizeWidth((JComponent[])r.toArray(new JComponent[r.size()]));
         setBorder(new EmptyBorder(GAP,GAP,GAP,GAP));
     }

     private JComponent createEmulatorPanel(PropertySet ps, List l, List r) {
         JComponent portLabel = new JLabel(EMULATOR_PORT_LABEL);
         JComponent portField = PropertyEditors.createTextField(ps,
             PROP_EMULATOR_PORT, 10);
         l.add(portLabel);
         r.add(portField);

         JComponent panel = GUI.createRowPanel(new Component[] {
             PropertyEditors.createCheckBox(ps, PROP_ATTACH_TO_EMULATOR,
             EMULATOR_ATTACH_ENABLE), null,
             GUI.createTextComponent(EMULATOR_ATTACH_TEXT), null,
             portLabel, portField
         });
         setTitledBorder(panel, KDPPreferences.EMULATOR_SESSION_NAME);
         return panel;
     }

     private JComponent createDevicePanel(PropertySet ps, List l, List r) {
         JComponent panel = GUI.createTextComponent(DEVICE_TEXT);
         setTitledBorder(panel, KDPPreferences.DEVICE_SESSION_NAME);
         return panel;
     }

     private static void setTitledBorder(JComponent panel, String title) {
         TitledBorder b = new TitledBorder(title);
         b.setTitleFont(b.getTitleFont().deriveFont(Font.BOLD));
         panel.setBorder(new CompoundBorder(b,EMPTY));
     }
}
