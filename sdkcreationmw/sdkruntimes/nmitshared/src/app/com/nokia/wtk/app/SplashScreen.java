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




package com.nokia.wtk.app;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
//import com.nokia.wap.sdk.Toolkit;

/**
 * Application splash screen.
 *
 */
class SplashScreen extends JWindow {

    private JLabel messageLabel = null;

    SplashScreen() {
        this(true);
    }

    public SplashScreen(boolean show) {

        final KeyListener keyListener = new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                switch (e.getKeyCode()) {
                case KeyEvent.VK_ESCAPE:
                    SplashScreen.this.dispose();
                    break;
                }
            }
        };

        // Provide a user-initiated way to dispose of window:
        addKeyListener(keyListener);

        // Capture Window Closure:
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) { dispose(); }
        });

        // Create the contents
        JPanel contentPanel = new JPanel(new BorderLayout());
        contentPanel.setBorder(BorderFactory.createLineBorder(Color.black, 1));
        contentPanel.setBackground(Color.white);
        getContentPane().add(contentPanel, BorderLayout.CENTER);

        JLabel identityLabel = new JLabel(
            AppEnvironment.getTitle() + "   v" +
                AppEnvironment.getVersion());
        identityLabel.setOpaque(false);

        // Identification Panel at Bottom:
        JPanel identityPanel = new JPanel();
        identityPanel.setLayout(new BoxLayout(identityPanel, BoxLayout.X_AXIS));
        identityPanel.setOpaque(false);
        identityPanel.add(new JLabel(
                AppEnvironment.getIcon(AppEnvironment.appCorporateLogoImage)));
        identityPanel.add(Box.createRigidArea(new Dimension(20,20)));
        identityPanel.add(Box.createGlue());
        identityPanel.add(identityLabel);
        identityPanel.add(Box.createRigidArea(new Dimension(20,20)));

        contentPanel.add(identityPanel, BorderLayout.NORTH);

        // Spash Image in Center:
        contentPanel.add(new JLabel(AppEnvironment.getIcon(AppEnvironment.splashImage)),
                                    BorderLayout.CENTER);

        // Message Panel at Bottom:
        JPanel messagePanel = new JPanel();
        messagePanel.setLayout(new BoxLayout(messagePanel, BoxLayout.X_AXIS));
        messagePanel.setOpaque(false);

        messageLabel = new JLabel(AppEnvironment.getString(AppEnvironment.splashMessage));
        messageLabel.setOpaque(false);

        messagePanel.add(Box.createRigidArea(new Dimension(30,30)));
        messagePanel.add(messageLabel);
        messagePanel.add(Box.createGlue());
        messagePanel.add(Box.createRigidArea(new Dimension(30,30)));

        contentPanel.add(messagePanel, BorderLayout.SOUTH);

        // show the frame
        pack();
        Dimension screenSize = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
        Dimension prefSize = getPreferredSize();
        setLocation((screenSize.width  - prefSize.width)/2,
                    (screenSize.height - prefSize.height)/2);

        if (show) show();
    }
}

