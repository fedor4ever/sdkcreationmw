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


package com.nokia.wtk.diagnostics.views;

import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Vector;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JPanel;
import javax.swing.Scrollable;
import javax.swing.SwingConstants;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.RightClickMenuSupport;
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.Utils;

/**
 * <p>Title: ViewsManager</p>
 * <p>Description: A class that encapsulates a visual Container component
 * that stacks other components with sliders to adjust the height of
 * the component. No </p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */

public class ViewsManager {

    /**
     * Interface that describes the object encapsulating the state of an
     * added component in this panel. Use the methods in here to dynamically
     * adjust the parameters of an item added to the ViewsManager
     */
    public static interface ViewState {
        /**
         * Tell the ViewState whether the usder should be able to resize
         * the component in the ViewState.
         * @param v if true, the ViewState will show appropriate resizing
         * controls, and inform the ViewsManager to allow resizing.
         * If false, no controls will be shown and the ViewState will not
         * allow resizing
         */
        public void setFlexible(boolean v);

        /**
         * Findf out whether the user can resize the component inside the
         * ViewState
         * @return whether the ViewState will allow resizing
         */
        public boolean isFlexible();

        /**
         * Return the ViewPanel associated with this state
         * @return ViewPanel the ViewPanel associated with this state
         */
        public ViewPanel getViewPanel();
    }

    static {
        AppEnvironment.addResources(
            "resources/ViewsManager.properties",
            ViewsManager.class);
    }

    private ViewsContainer jcomponent_;
    private Vector viewStates_ = new Vector();
    private ViewPanel [] viewPanels;

    static class ViewsPanelVisibleMenuItem
        extends JCheckBoxMenuItem
        implements PropertyChangeListener, ActionListener {

        private final Property visibleProperty_;

        ViewsPanelVisibleMenuItem(ViewPanel obj) {
            // make sure the action name is exactly what will be displayed
            // as the short name, i.e. the label
            super(obj.getViewTitle());

            visibleProperty_ =
                obj.getProperties().getProperty(ViewPanel.Visible_Property);
            setSelected(((Boolean) visibleProperty_.getValue()).booleanValue());
            visibleProperty_.addPropertyChangeListener(this);

            // this might be the stranges line of code I have ever written
            this.addActionListener(this);
        }
        public void actionPerformed(ActionEvent ae) {
            Boolean doB = Utils.getBoolean(this.isSelected());
            visibleProperty_.setValue(doB);
        }
        public void propertyChange(PropertyChangeEvent e) {
            boolean value = ((Boolean) e.getNewValue()).booleanValue();
            if (value != this.isSelected()) {
                ViewsPanelVisibleMenuItem.this.setSelected(value);
            }
        }
    }

    public ViewPanel [] getViewPanels() {
        if (viewPanels == null) {
            JComponent c = getComponent();
            int n = c.getComponentCount();
            viewPanels = new ViewPanel[n];
            for (int i=0; i<n; i++) {
                viewPanels[i] = (ViewPanel)c.getComponent(i);
            }
        }
        return viewPanels;
    }

    /**
     * Adds the component to the bottom of the stacked panels
     * @param obj The component to add.
     * @return a ViewState object with which to influence the shape and
     * flexibility of the added component obj. This panel will have its
     * flexibility property set to true
     */
    public ViewState addComponent(ViewPanel obj) {

        ViewPanelStateImpl sliderP = new ViewPanelStateImpl(obj, this);
        JCheckBoxMenuItem jcheck = new ViewsPanelVisibleMenuItem(obj);

        viewPanels = null;
        getJComponent().add(obj);
        getJComponent().getPodContainerMenuItem().add(jcheck);
        viewStates_.add(0, sliderP);
        obj.getSlider().addPropertyChangeListener(getDragListener());
        return sliderP;
    }

    /**
     * Remove a component by giving the ViewState
     * @param state the ViewState of the panel to remove
     */
    public void removeComponent(ViewState state) {
        ViewPanel p = state.getViewPanel();
        getJComponent().remove(p);
        p.getSlider().addPropertyChangeListener(getDragListener());
    }

    private PropertyChangeListener dragListener_;
    private boolean dragging_ = false;

    public boolean isDragging() {
        return dragging_;
    }
    public void setDragging(boolean dragging) {
        dragging_ = dragging;
    }

    private PropertyChangeListener getDragListener() {
        if (dragListener_ == null) {
            dragListener_ = new PropertyChangeListener() {

                public void propertyChange(PropertyChangeEvent pe) {
                    if (Slider
                        .DRAGGING_PROPERTY
                        .equals(pe.getPropertyName())) {
                        setDragging(
                            ((Boolean) pe.getNewValue()).booleanValue());
                    }
                }
            };
        }
        return dragListener_;
    }
    /**
      * Get the visisble component to add to the window. This is a very general
      * component to make programmers using this class think twice from directly
      * accessing it.
      * @return A Component that contains the all the added components,
      * managed properly, with sliders.
      */
    public JComponent getComponent() {
        return getJComponent();
    }

    /**
     * Get the visisble component to add to the window. For internal
     * implementation purposes only.
     * @return A Component that contains the all the added components,
     * managed properly, with sliders.
     */
    protected ViewsContainer getJComponent() {
        if (jcomponent_ == null) {
            jcomponent_ = new ViewsContainer(this);
        }
        return jcomponent_;
    }

    /**
     * Will return a menu with names of pods for setting/unsetting pod
     * visibility.
     * @return JMenu The menu. It has a proper title set, so can easily
     * be added to another menu to create a sub-menu.
     */
    public JMenu getPodContainerMenuItem() {
        return getJComponent().getPodContainerMenuItem();
    }

    static private class ViewsContainer
        extends JPanel
        implements Scrollable, RightClickMenuSupport {

        private JMenu views_;
        private final ViewsManager manager_;
        protected final int vgap = 0;
        ViewsContainer(ViewsManager manager) {
            manager_ = manager;
            setLayout(new DiagWinLayout(vgap));
//            setLayout
//              (new DiagWinLayout
//                (vgap, VerticalLayout.BOTH, VerticalLayout.CENTER, true));
            // new

        }
        public Dimension getPreferredScrollableViewportSize() {
            return this.getPreferredSize();
        }
        public boolean getScrollableTracksViewportWidth() {
            return true;
        }
        public boolean getScrollableTracksViewportHeight() {
            if (manager_.isDragging()) {
                return false;
            }
            int height = getHeight();
            double screenheight =
                Toolkit.getDefaultToolkit().getScreenSize().getHeight() - 100;
            return height < screenheight;
        }

        public int getScrollableUnitIncrement(
            Rectangle visibleRect,
            int orientation,
            int direction) {
            return 1;
        }
        public int getScrollableBlockIncrement(
            Rectangle visibleRect,
            int orientation,
            int direction) {
            if (orientation == SwingConstants.VERTICAL) {
                return visibleRect.height - 5;
            }
            return visibleRect.width - 5;
        }

        public Object[] getRightClickActions(MouseEvent ev) {
            return new Object[] { getPodContainerMenuItem()};
        }

        JMenu getPodContainerMenuItem() {
            if (views_ == null) {
                views_ =
                    new JMenu(
                        AppEnvironment.getString(Resource.MANAGER_VIEWS_LABEL));
            }
            return views_;
        }
    }
}
