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

/* java.io */
import java.io.Writer;

/* java.awt */
import java.awt.Dimension;
import java.awt.BorderLayout;

/* java.awt.event */
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/* javax.swing */
import javax.swing.Timer;
import javax.swing.JPanel;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JProgressBar;
import javax.swing.SwingUtilities;

/* com.nokia.wap.sdk.util.gui */
import com.nokia.wtk.util.gui.ComponentBox;

/**
 * A class to encapsulate a region to deliver status messages that is
 * unobtrusive and continually visible. The status bar has three distinct
 * areas: a large one in the middle, and two smaller ones on each side,
 * left and right. Suggested use is to send important messages to the
 * middle large area using <code>setStatusText</code>.
 * The other two components consist of a progress indicator and as user-supplied
 * third component.
 */
public
class StatusBar
extends JPanel {

    private static int MIN_TICKS = 10;
    private static int MAX_TICKS = 30;

    private JProgressBar progress;      // Progress Indicator
    private JPanel progressPanel = new JPanel(new BorderLayout());
    private JLabel centerLabel;         // Long Center-Aligned Status Area
    private JComponent thirdComponent;  // user-supplied
    private Writer statusWriter = null;
    ProgressIndicator indicator;
    String indicatorAlignment = BorderLayout.WEST;


    private void init() {
        // force L&F independent border for progress bar
        progress = new JProgressBar(0, MAX_TICKS);
        progress.setBorderPainted(false);
        progress.setPreferredSize(new Dimension(100,5));
        progressPanel.setBorder(LookFactory.loweredBorder());
        progressPanel.add(progress, BorderLayout.CENTER);

        // just a default
        thirdComponent = new JLabel("      ", JLabel.LEFT);

        centerLabel = new JLabel(" ", JLabel.LEFT);
        centerLabel.setBorder(LookFactory.loweredBorder());

        indicator = new ProgressIndicator();

        setBorder(LookFactory.emptyBorder());
    }

    /**
     * Creates a status bar where the indicator is on the left, the status text
     * is center, and the settable component is on the right.
     */
    public StatusBar() {
        super(new BorderLayout(2,0));

        init();
        add(progressPanel, BorderLayout.WEST);
        add(centerLabel, BorderLayout.CENTER);
        add(ComponentBox.createRigidComponent(thirdComponent),BorderLayout.EAST);
    }

    /**
     * Creates a status bar, where the status text is in the center but the user
     * can set the position of the other two elements.
     * @param indicatorAlignment if the parameter equals BorderLayout.EAST, the
     * progress indicator will be set on the east of the center status text, and
     * west in all other cases.
     */
    public StatusBar(String indicatorAlignment) {
        super(new BorderLayout(2,0));

        init();
        if (BorderLayout.EAST.equals(indicatorAlignment)) {
            add(ComponentBox.createRigidComponent(thirdComponent), BorderLayout.WEST);
            add(progressPanel, BorderLayout.EAST);
        } else {
            add(progressPanel, BorderLayout.WEST);
            add(ComponentBox.createRigidComponent(thirdComponent), BorderLayout.EAST);
        }
        add(centerLabel, BorderLayout.CENTER);
        this.indicatorAlignment = indicatorAlignment;
    }

    /**
     * Returns progress indicator
     */
    public ProgressUI.Indicator getProgressIndicator() {
        return indicator;
    }

    /**
     * Cancel progress indication
     */
    public void cancelProgressIndicator() {
        indicator.cancel();
    }

    /**
     * Returns current status text.
     */
    public String getStatusText() {
        return centerLabel.getText();
    }

    /**
     * Set Status Field text.
     * @param s A string to display in the application center status field.
     */
    public void setStatusText(String s) {
        centerLabel.setText(s);
    }

    /**
     * Set the third component.
     * @param s A string to display in the application right-most
     *          status field.
     */
    public void setThirdComponent(JComponent c) {
        thirdComponent = c;
        if (BorderLayout.EAST.equals(this.indicatorAlignment)) {
            add(ComponentBox.createRigidComponent(thirdComponent),BorderLayout.WEST);
        } else {
            add(ComponentBox.createRigidComponent(thirdComponent),BorderLayout.EAST);
        }
    }

    /**
     * Return the third component
     */
    public JComponent getThirdComponent() {
        return thirdComponent;
    }

    /**
     * Return the Writer for showing status messages
     */
    public Writer getStatusWriter() {
        if (statusWriter == null) {
            statusWriter = new StatusWriter();
        }
        return statusWriter;
    }


    //=======================================================================
    //          W R I T E R S
    //=======================================================================

    private class StatusWriter extends Writer {
        public void write(char cbuf[], int off, int len) {
            if ((cbuf != null) && (len > 0)) {

                // Remove any trailing '\r' or '\n' characters:
                int i=len-1;
                while (i>=0 && (cbuf[off+i] == '\r' || cbuf[off+i] == '\n')) {
                    i--;
                }

                if (i >= 0) {
                    // Put text into status area during event-dispatch thread:
                    final String s = new String(cbuf, off, i+1);
                    SwingUtilities.invokeLater(new Runnable() {
                        public void run() {
                            StatusBar.this.setStatusText(s);
                        }
                    });
                }
            }
        }
        public void flush() {}
        public void close() {}
    };

    //=======================================================================
    //          P R O G R E S S    I N D I C A T O R
    //=======================================================================

    /**
     * Progress indicator
     */
    private class ProgressIndicator implements ProgressUI.Indicator,
                                               ActionListener {
        private Timer timer;

        ProgressIndicator() {
            timer = new Timer(100, this);
        }

        public void startProgress() {
            if (!timer.isRunning()) {
                timer.start();
            }
        }

        public void stopProgress() {
            // not doing anything here
        }

        void cancel() {
            timer.stop();
            progress.setValue(0);
        }

        private boolean canStop() {
            return (!AppEnvironment.getAppEnvironment().getProgressUI().isActive());
        }

        /** Timer callback */
        public void actionPerformed(ActionEvent e) {

            // show at least 10 ticks before stopping
            if (progress.getValue() >= MIN_TICKS) {
                if (canStop()) {
                    cancel();
                    return;
                }

                // rollover at 30 ticks
                if (progress.getValue() >= MAX_TICKS) {
                    progress.setValue(0);
                } else {
                    progress.setValue(progress.getValue()+1);
                }
            } else {
                progress.setValue(progress.getValue()+1);
            }
        }
    }
}
