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

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.AWTEventListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import javax.swing.Action;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.KeyStroke;
import javax.swing.MenuSelectionManager;
import javax.swing.SwingUtilities;

import com.nokia.wtk.util.WeakHashSet;

/**
 * Provides support for right mouse click menus.
 *
 */
class RightClickMenuSystem {

    /** Maps components to their popup menus */
    private static Set rightClickComponents  = null;
    private static MouseListener mouseListener = null;
    private static AWTEventListener rightClickMenuListener = null;
    private static Toolkit toolkit;

    /**
     * Creates <code>RightClickMenuSystem</code>
     */
    private RightClickMenuSystem() {}

    /**
     * Initializes the right-click menu system. From the moment this is
     * called, the system that supplied the Toolkit in the argument will
     * have a listener added to it checking for righ-clicks or other
     * triggers of the isPopupTrigger call, and create an approrpiate
     * PopupMenu based on the RightClickMenuSupport interface.
     */
    public static void startRightClickMenuSystem() {
        if (toolkit != null) {
            stopRightClickMenuSystem();
        }
        toolkit = java.awt.Toolkit.getDefaultToolkit();
        if (rightClickMenuListener == null) {
            rightClickMenuListener = new RightClickMenuListener();
            rightClickComponents  = new WeakHashSet(29);
        }
        toolkit.addAWTEventListener(rightClickMenuListener,
                                    AWTEvent.MOUSE_EVENT_MASK);
    }

    public static void stopRightClickMenuSystem() {
        toolkit.removeAWTEventListener(rightClickMenuListener);
        try {
            rightClickComponents.removeAll(rightClickComponents);
        } catch (UnsupportedOperationException u) {
        }
    }

    /**
     * Registers component with the right mouse click system
     */
    private static void registerComponent(Component c) {
        synchronized (rightClickComponents) {
            if (rightClickComponents.contains(c)) {
                return;
            } else {
                rightClickComponents.add(c);
                if (mouseListener == null) {
                    mouseListener = new RightMouseClickListener();
                }
            }
        }
        c.addMouseListener(mouseListener);
    }

    /**
     * Looks for {@link RightClickMenuSupport} components in the hierarchy
     * and if it finds any, return <code>true</code>
     */
    private static boolean findRightClick(Component deepest, MouseEvent e) {
        for(Component c=deepest; c != null; c=c.getParent()) {
            if (c instanceof RightClickMenuSupport) {
                RightClickMenuSupport rcms = (RightClickMenuSupport)c;
                Object[] actions = rcms.getRightClickActions(e);
                if (actions != null && actions.length > 0) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Returns the deepest component at the point specified by the
     * mouse event
     */
    private static Component getDeepestComponent(MouseEvent e) {
        Component source = e.getComponent();
        Component root = SwingUtilities.getRoot(source);
        Point p = SwingUtilities.convertPoint(source,e.getPoint(),root);
        return SwingUtilities.getDeepestComponentAt(root,p.x,p.y);
    }

    //=======================================================================
    //         I N N E R    C L A S S E S
    //=======================================================================

    /**
     * This class listens for ALL mouse events received by ALL components
     */
    static private class RightClickMenuListener implements AWTEventListener {
        final public void eventDispatched(AWTEvent event) {
            if (event instanceof MouseEvent) {
                MouseEvent e = (MouseEvent)event;
                if (e.isPopupTrigger()) {

                    // the original version of RightClickMenuSystem was
                    // showing the popup menu here. However, it didn't work
                    // on Unix for the following reasons:
                    //
                    // 1. popup trigger on Unix is MOUSE_PRESSED event
                    //    while on Windows it's MOUSE_RELEASED (see below
                    //    why this matters)
                    // 2. when we show a popup menu, it adds a mouse
                    //    listener to the invoking component which hides
                    //    the menu when it gets MOUSE_PRESSED event
                    // 3. this hook is invoked *before* the component
                    //    notifies its mouse listeners. Therefore, when
                    //    we show the menu and return from this call,
                    //    the component whould invoke the above mentioned
                    //    listener which on Unix (remember that popup
                    //    trigger on Unix is MOUSE_PRESSED) would hide
                    //    the menu before (or right after) it appears
                    //    on the screen.
                    //
                    // This is why all we do here is register a mouse
                    // listener which will show the popup menu when it
                    // receives appropriate event

                    Component comp = getDeepestComponent(e);
                    synchronized (rightClickComponents) {
                        if (rightClickComponents.contains(comp)) {
                            // we are already listenening for mouse events
                            // from this component, no need to scan the
                            // hierarchy
                            return;
                        }
                    }
                    if (findRightClick(comp, e)) {
                        registerComponent(comp);

                        // this is the first click on this component, and
                        // it so happened that the target of the event is
                        // one of the ancestors of the component we are
                        // interested in, meaning that the component we
                        // have just registered won't receive this event.
                        // Therefore, we need to post a copy of this event
                        // to the component in question
                        Object source = e.getSource();
                        if (comp != source) {

                            // translate the coordinates
                            int x = e.getX();
                            int y = e.getY();
                            Component c = comp;
                            while (c != null && c != source) {
                                x -= c.getX();
                                y -= c.getY();
                                c  = c.getParent();
                            }

                            // create and post a new event
                            MouseEvent repost = new MouseEvent(comp,
                                e.getID(), e.getWhen(), e.getModifiers(),
                                x, y, e.getClickCount(), e.isPopupTrigger());

                            Toolkit.getDefaultToolkit()
                                   .getSystemEventQueue()
                                   .postEvent(repost);
                        }
                    }
                }
            }
        }
    }

    // There's a memory leak in that the menu keeps some listeners attached
    // to longer-living actions after being used. Basically the menu needs
    // to unbind everything after use. 'After use' is when the menu becomes
    // invisible again. However, 'setVisible(false)' gets called before
    // the actual action is processed. So we have to unbind everything after
    // the menu has done its work: when the Swing thread is finished.
    // All these menus here are one-time use anyway.
    private static
    class NeatlyCleaningJPopupMenu extends JPopupMenu implements Runnable {

        /** list of menu items to get unbound after menu is used */
        private List unbindables = new ArrayList(5);

        public JMenuItem add(Action action) {
            JMenuItem mi = super.add(action);
            mi.setAccelerator((KeyStroke)action.getValue(Action.ACCELERATOR_KEY));
            unbindables.add(mi);
            return mi;
        }

        public void run() {
            if (!unbindables.isEmpty()) {
                for (Iterator i = unbindables.iterator(); i.hasNext(); ) {
                    ((JMenuItem)i.next()).setAction(null);
                }
                unbindables.clear();
            }
            removeAll();
            setInvoker(null);
        }

        public void setVisible(boolean b) {
            super.setVisible(b);
            if (!b && getComponentCount() > 0) {
                SwingUtilities.invokeLater(this);
            }
        }
    }

    /**
     * Listens to the mouse events from the components
     */
    private static class RightMouseClickListener extends MouseAdapter {

        // we don't want to guess which events are popup triggers on which
        // platform, so look at all three of them
        public void mouseClicked(MouseEvent e)  { processMouseEvent(e); }
        public void mousePressed(MouseEvent e)  { processMouseEvent(e); }
        public void mouseReleased(MouseEvent e) { processMouseEvent(e); }

        /** The mouse event processor */
        private void processMouseEvent(MouseEvent e) {
            if (e.isPopupTrigger()) {

                // hide current menu
                MenuSelectionManager.defaultManager().clearSelectedPath();
                JPopupMenu menu = new NeatlyCleaningJPopupMenu();
                // collect menu items from all the components in the heirarchy
                Component deepest = getDeepestComponent(e);
                for(Component c = deepest; c != null; c = c.getParent()) {
                    if (c instanceof RightClickMenuSupport) {
                        menu = buildMenu((RightClickMenuSupport)c, e, menu);
                    }
                }

                if (menu != null) {
                    Point pt = SwingUtilities.convertPoint(e.getComponent(),
                                                           e.getPoint(),
                                                           deepest);
                    pt = LookFactory.showMenuLocation(menu, deepest, pt.x, pt.y);
                    menu.show(deepest, pt.x, pt.y);
                }
            }
        }


        /**
         * Adds menu items from the specified {@link RightClickMenuSupport}
         * object to the menu.
         */
        private static JPopupMenu
        buildMenu(RightClickMenuSupport r, MouseEvent e, JPopupMenu menu) {
            Object[] actions = r.getRightClickActions(e);
            if (actions != null) {
                for (int i=0; i<actions.length; i++) {
                    Object action = actions[i];
                    if (action != null) {
                        if (menu == null) menu = new NeatlyCleaningJPopupMenu();
                        if (i==0 && menu.getComponentCount()>0) {
                            menu.addSeparator();
                        }
                        if (action instanceof JMenuItem) {
                            menu.add((JMenuItem)action);
                        } else if (action instanceof Action) {
                            menu.add((Action)action);
                        }
                    } else {
                        menu.addSeparator();
                    }
                }
            }
            return menu;
        }
    }

}
