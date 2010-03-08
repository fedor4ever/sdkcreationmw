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



package com.nokia.wtk.util.gui;

/* java.util */
import java.util.Map;
import java.util.WeakHashMap;
import java.util.Collections;

/* java.awt */
import java.awt.Insets;

/* java.awt.event */
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.HierarchyEvent;
import java.awt.event.HierarchyListener;
import java.awt.event.MouseMotionAdapter;
import java.awt.event.MouseMotionListener;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.Action;
import javax.swing.JButton;
import javax.swing.ButtonModel;

/* javax.swing.border */
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * This class encodes a version of a simple JButton whose border is only visible
 * when the user mouse's over it. Not a toggle-button.
 **/
public class RolloverJButton extends JButton {

    private static JButton pressedButton = null;
    private static Map rolloverMap = Collections.synchronizedMap(new WeakHashMap());

    private static Border normalBorder =
        new CompoundBorder(
        new LightBevelBorder(LightBevelBorder.RAISED),
        new EmptyBorder(1,1,1,1));

    private static Border pressedBorder =
        new CompoundBorder(
        new LightBevelBorder(LightBevelBorder.LOWERED),
        new EmptyBorder(1,1,1,1));

    private static
    class       Rollover
    extends     MouseAdapter
    implements  ChangeListener, HierarchyListener {

        private boolean mouseIn = false;
        private JButton bt;
        private ButtonModel bm;
        private MouseMotionListener motionDetector;

        Rollover(JButton button) {
            bt = button;
            bm = bt.getModel();

            bm.addChangeListener(this);
            bt.addMouseListener(this);
            bt.addHierarchyListener(this);
            if (!(bt instanceof RolloverJButton)) {
                motionDetector = new MotionDetector();
                bt.addMouseMotionListener(motionDetector);
            }
        }

        void deactivate() {
            bm.removeChangeListener(this);
            bt.removeMouseListener(this);
            bt.removeHierarchyListener(this);
            if (motionDetector != null) {
                bt.removeMouseMotionListener(motionDetector);
                motionDetector = null;
            }
        }

        public void hierarchyChanged(HierarchyEvent e) {
            // remove the border when button disappears from the screen
            if (!bt.isShowing()) {
                mouseIn = false;
                bm.setArmed(false);
                bm.setPressed(false);
                bt.setBorder(normalBorder);
                bt.setBorderPainted(false);
                if (pressedButton == bt) {
                    pressedButton = null;
                }
            }
        }
        public void mouseEntered(MouseEvent e) {
            mouseIn = true;
            stateChanged();
        }
        public void mouseExited(MouseEvent e) {
            mouseIn = false;
            stateChanged();
        }
        public void stateChanged(ChangeEvent e) {
            stateChanged();
        }
        private void stateChanged() {
            boolean pressed = bm.isPressed();
            if (pressed) {
                if (pressedButton != bt) {
                    // "unpress" the currently pressed button in case if
                    // it missed a notification
                    if (pressedButton != null) {
                        pressedButton.getModel().setPressed(false);
                    }
                    pressedButton = bt;
                }
            } else if (pressedButton == bt) {
                pressedButton = null;
            }
            bt.setBorder((pressed && mouseIn) ? pressedBorder : normalBorder);
            bt.setBorderPainted(mouseIn && bt.isEnabled() &&
                            (pressedButton == bt ||
                             pressedButton == null));
        }

        /**
         * This is a one-time mouse motion listener. It's purpose is to detect
         * whether the mouse is inside a "transformed" rollover button. Prior
         * to Java 1.5 there was no way to detect the current mouse position,
         * other than to listen to the mouse events.
         */
        private class MotionDetector extends MouseMotionAdapter {
            public void mouseDragged(MouseEvent e) {
                motionDetected();
            }
            public void mouseMoved(MouseEvent e) {
                motionDetected();
            }
            private void motionDetected() {
                if (!mouseIn) {
                    mouseIn = true;
                    stateChanged();
                }
                bt.removeMouseMotionListener(this);
                motionDetector = null;
            }
        }
    }

    public RolloverJButton() {
        init(this);
    }

    public RolloverJButton(Action action) {
        super(action);
        init(this);
    }

    public RolloverJButton(String text) {
        super(text);
        init(this);
    }

    public RolloverJButton(Icon icon) {
        super(icon);
        init(this);
    }

    public RolloverJButton(String text, Icon icon) {
        super(text, icon);
        init(this);
    }

    private static void init(JButton bt) {
        bt.setBorder(normalBorder);
        bt.setMargin(new Insets(0,0,0,0));
        bt.setBorderPainted(false);
        bt.setFocusPainted(false);
        bt.setDefaultCapable(false);
        bt.setRequestFocusEnabled(false);

        Rollover rollover = new Rollover(bt);
        if (!(bt instanceof RolloverJButton)) {
            rolloverMap.put(bt, rollover);
        }
    }

    /**
     * Takes a JButton and makes it a RolloverJButton. Not suitable
     * for toggle buttons.
     *
     * @param button The JButton to transform and make a rollover JButton.
     * @return the same JButton with some parameters set and a changeListener
     * added to the model.
     */
    public static JButton transformToRollover(JButton button) {
        if (!(button instanceof RolloverJButton)) init(button);
        return button;
    }

    /**
     * Cancels the rollover effect for the specified button.
     *
     * @param button the button to cancel the effect for.
     * @return <code>true</code> if transformToRollover was previously
     *   invoked on this button, <code>false</code> otherwise
     */
    public static boolean cancelRollover(JButton button) {
        Rollover rollover = (Rollover)rolloverMap.remove(button);
        if (rollover != null) {
            rollover.deactivate();
            if (pressedButton == button) {
                pressedButton = null;
            }
            return true;
        } else {
            return false;
        }
    }
}
