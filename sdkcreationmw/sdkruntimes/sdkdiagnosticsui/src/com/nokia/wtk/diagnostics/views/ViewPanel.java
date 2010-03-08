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

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.Color;
import java.awt.Window;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.CardLayout;

import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ContainerAdapter;
import java.awt.event.ContainerEvent;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.Utils;
import com.nokia.wtk.util.gui.DisclosurePanel;
import com.nokia.wtk.util.gui.MenuButton;
import com.nokia.wtk.util.gui.RolloverJButton;

/**
 * <p>Description: An extension of JPanel that does the visual management
 * of tying together an Overview and an In-Depth view using progressive
 * disclosure. To create the JPanel, just supply an implementation of
 * the AbstractPanelComponentFactory that has all the elemnts. </p>
 * <p>This panel uses delayed instantiation, so the AbstractPanelFactory will
 * not be asked for any item until it needs to become visible.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */

public class ViewPanel extends JPanel implements Resource {

    static {
        AppEnvironment.addResources(
            "resources/ViewPanel.properties",
            ViewPanel.class);
    }

    private static final Color foreground__ =
        Color.decode(AppEnvironment.getString(FOREGROUND_COLOR));
    private static final int margin__ = AppEnvironment.getInt(COMPONENT_MARGIN);
    private static final int labelMargin__ =
        AppEnvironment.getInt(SMALL_COMPONENT_MARGIN);
    private static final Color titleForeground__ =
        Color.decode(AppEnvironment.getString(TITLE_FOREGROUND_COLOR));
    private static final Color viewOuterBorder__ =
        Color.decode(AppEnvironment.getString(OUTER_BORDER_COLOR));

    private final static Font titleFont__ =
        (AppEnvironment.getString(TITLE_FONT_ATTRIBUTES).indexOf("b") > -1
            ? LookFactory.getStandardBoldFont()
            : LookFactory.getStandardPlainFont());

    /** The size of the pod panel. */
    public static final String Size_Property = "size";
    /** The visibility state of the hidden panel in the pod panel. */
    public static final String Open_Property = "open";
    /** The visibility state of the whole pod panel. */
    public static final String Visible_Property = "visible";

    public static void clear(Component c, Color foreground) {
        if (c instanceof JComponent) {
            ((JComponent) c).setOpaque(false);
        }

        if (c instanceof Container) {
            Container cont = (Container) c;
            int num = cont.getComponentCount();

            for (int i = 0; i < num; i++) {
                clear(cont.getComponent(i), foreground);
            }
        }
        c.setForeground(foreground);
    }

    private abstract class Property
        extends PropertySet.AbstractProperty {

        Property(String path, Class type, boolean archive) {
            super(path, type, archive);
        }

        /**
         * Adds a PropertyChangeListener to the listener list.
         * @param l The PropertyChangeListener to be added
         */
        public void addPropertyChangeListener(PropertyChangeListener l) {
            getPropertyChangeSupport().addPropertyChangeListener(getName(),l);
        }

        /**
         * Removes a PropertyChangeListener from the listener list.
         * @param l The PropertyChangeListener to be removed
         */
        public void removePropertyChangeListener(PropertyChangeListener l) {
            getPropertyChangeSupport().removePropertyChangeListener(getName(),l);
        }

        /**
         * Fires a property change event
         */
        public void firePropertyChange(boolean oldValue, boolean newValue) {
            getPropertyChangeSupport().firePropertyChange(getName(),
                Utils.getBoolean(oldValue),Utils.getBoolean(newValue));
        }

        /**
         * Fires a property change event
         */
        public void firePropertyChange(Object oldValue, Object newValue) {
            getPropertyChangeSupport().firePropertyChange(getName(),
                oldValue,newValue);
        }
    }

    private static final String OPEN_STATE = "open";
    private static final String CLOSED_STATE = "closed";

    private final AbstractViewFactoryInterface viewFactory;
    private final Corner leftCornerClosed;
    private final Corner rightCornerClosed;
    private final Corner leftCornerOpen;
    private final Corner rightCornerOpen;
    private final JComponent bottomLine;

    private Slider slider;
    private JPanel topComponent;
    private CardLayout topComponentLayout;
    private JComponent openLine;
    private ButtonDisclosurePanel disclosurePanel;
    private JLabel topLabel;

    private PropertyChangeSupport propChange;
    private Property propSize;
    private Property propVisible;
    private Property propOpen;
    private PropertySet properties;

    /**
     * Make a new ViewPanel
     * @param fact the AbstractFactory to supply all the elements in the
     * ViewPanel.
     */
    public ViewPanel(AbstractViewFactoryInterface fact) {
        topComponentLayout = new CardLayout();

        leftCornerClosed =
            AppEnvironment.getString(BOTTOM_LEFT_IMAGE).length() != 0
                ? new Corner(BOTTOM_LEFT_IMAGE)
                : null;
        rightCornerClosed =
            AppEnvironment.getString(BOTTOM_RIGHT_IMAGE).length() != 0
                ? new Corner(BOTTOM_RIGHT_IMAGE)
                : null;
        leftCornerOpen =
            AppEnvironment.getString(BOTTOM_LEFT_IMAGE_OPEN).length() != 0
                ? new Corner(BOTTOM_LEFT_IMAGE_OPEN)
                : null;
        rightCornerOpen =
            AppEnvironment.getString(BOTTOM_RIGHT_IMAGE_OPEN).length() != 0
                ? new Corner(BOTTOM_RIGHT_IMAGE_OPEN)
                : null;

        viewFactory = fact;
        setLayout(new BorderLayout());

        JComponent comp = new JPanel(new BorderLayout());
        if (AppEnvironment.getString(TOP_LEFT_IMAGE) != null) {
            comp.add(new Corner(TOP_LEFT_IMAGE), BorderLayout.WEST);
        }
        if (AppEnvironment.getString(TOP_LINE_IMAGE).length() != 0) {
            comp.add(
                new TexturedArea(AppEnvironment.getIcon(TOP_LINE_IMAGE)),
                BorderLayout.CENTER);
        }
        if (AppEnvironment.getString(TOP_RIGHT_IMAGE).length() != 0) {
            comp.add(new Corner(TOP_RIGHT_IMAGE), BorderLayout.EAST);
        }
        if (comp.getComponentCount() > 0) {
            add(comp, BorderLayout.NORTH);
        }

        if (AppEnvironment.getString(SIDE_LEFT_IMAGE).length() != 0) {
            ImageIcon icon = AppEnvironment.getIcon(SIDE_LEFT_IMAGE);
            comp = new TexturedArea(icon);
            comp.setLayout(new BoxLayout(comp, BoxLayout.Y_AXIS));
            comp.add(Box.createHorizontalStrut(icon.getIconWidth()));
            add(comp, BorderLayout.WEST);
        }
        if (AppEnvironment.getString(SIDE_RIGHT_IMAGE).length() != 0) {
            ImageIcon icon = AppEnvironment.getIcon(SIDE_RIGHT_IMAGE);
            comp = new TexturedArea(icon);
            comp.setLayout(new BoxLayout(comp, BoxLayout.Y_AXIS));
            comp.add(Box.createHorizontalStrut(icon.getIconWidth()));
            add(comp, BorderLayout.EAST);
        }

        bottomLine = new JPanel(new BorderLayout());
        add(bottomLine, BorderLayout.SOUTH);

        comp = new JPanel(new BorderLayout());

        JComponent smallPod = viewFactory.getSmallPodComponent();
        comp.add(smallPod, BorderLayout.SOUTH);
        smallPod.setBorder(BorderFactory.createEmptyBorder(0,labelMargin__,0,0));
        add(comp, BorderLayout.CENTER);

        ButtonDisclosurePanel disc = getDisclosurePanel();
        comp.add(disc, BorderLayout.CENTER);
        clear(this, foreground__);

        String colorInt = AppEnvironment.getString(BACKGROUND_COLOR);
        disc.getHiddenPanel().setOpaque(false);
        disc.getTheButton().setOpaque(true);

        this.setOpaque(true);
        this.setBackground(Color.decode(colorInt));

        disc.setTopComponent(getTopComponent());

        getProperties().addPropertySet("view",fact.getViewProperties());
        this.addComponentListener(new ComponentAdapter() {
            private boolean visible_ = ViewPanel.this.isVisible();

            private void fire(ComponentEvent e) {
                boolean oldVisible = visible_;
                visible_ = e.getComponent().isVisible();
                propVisible.firePropertyChange(oldVisible, visible_);
            }
            public void componentHidden(ComponentEvent e) {
                fire(e);
            }
            public void componentShown(ComponentEvent e) {
                fire(e);
            }
        });

        // Handle the size change due to component removal.
        smallPod.addContainerListener(new ContainerAdapter() {
            private Dimension size_ = ViewPanel.this.getSize();

            private void fire(ContainerEvent e) {
                Dimension oldSize = size_;
                size_ = ViewPanel.this.getSize();
                propSize.firePropertyChange(oldSize, size_);
            }

            public void componentRemoved(ContainerEvent evt) {
                fire(evt);
            }
        });
    }

    /**
     * invokes revalidate() on the hidden component, on behalf of
     * someone else (used from MainWindow when the view preferred
     // size is modified following a resizing of the whole container)
     */
    public void revalidateFlexComponent() {
      JComponent flexComp = getDisclosurePanel().getHiddenPanel();
      if (flexComp != null && flexComp.isShowing()) {
        flexComp.revalidate();
      }
    }

    /**
     * Creates PropertyChangeSupport
     */
    protected synchronized PropertyChangeSupport getPropertyChangeSupport() {
        if (propChange == null) {
            propChange = new PropertyChangeSupport(this);
        }
        return propChange;
    }

    public Dimension getMinimumSize() {
        Dimension minSize = super.getMinimumSize();
        minSize.width = 200;
        return minSize;
    }

    public PropertySet getProperties() {
        if (properties == null) {
            properties = new PropertySet();
            // Pod Size:
            propSize =
                new Property(Size_Property, Dimension.class, true) {
                private Dimension size;
                {
                    size = (Dimension) getValue();
                    getDisclosurePanel()
                        .getHiddenPanel()
                        .addComponentListener(new ComponentAdapter() {
                        public void componentResized() {
                            Dimension oldSize = size;
                            Dimension newSize = (Dimension) getValue();
                            size = newSize;
                            firePropertyChange(oldSize, newSize);
                        }
                    });
                }

                public void setValue(Object value) {
                    getDisclosurePanel().getHiddenPanel().setPreferredSize(
                        (Dimension) value);
                }
                public Object getValue() {
                    Dimension d =
                        getDisclosurePanel()
                            .getHiddenPanel()
                            .getPreferredSize();
                    return d;
                }
            };
            // Pod Visible:
            propVisible = new Property(Visible_Property, Boolean.class, true) {
                public void setValue(Object value) {
                    setVisible(((Boolean) value).booleanValue());
                }
                public Object getValue() {
                    return Utils.getBoolean(isVisible());
                }
            };
            // Pod Open:
            propOpen = new Property(Open_Property, Boolean.class, true) {
                public void setValue(Object value) {
                    setBigPanelShowing(((Boolean) value).booleanValue());
                }
                public Object getValue() {
                    return Utils.getBoolean(isBigPanelShowing());
                }
            };
            propOpen.setValue(new Boolean(viewFactory.getDefaultViewOpen()));
            int width = viewFactory.getDefaultViewWidth();
            int height = viewFactory.getDefaultViewHeight();
            propSize.setValue(new Dimension(width, height));
            properties.addProperty(propSize);
            properties.addProperty(propVisible);
            properties.addProperty(propOpen);
        }
        return properties;
    }

    /**
     * Sets the whether the big panel should be shown or not
     * @param visible if true the big panel should show, if false the pod should show
     * summary state
     */
    public void setBigPanelShowing(boolean visible) {
        getDisclosurePanel().setHiddenPanelVisible(visible);
    }

    /**
     * Answers whether the big panel is showing
     * @return true if the hidden big panel is showing, false otherwise
     */
    public boolean isBigPanelShowing() {
        return this.disclosurePanel.isHiddenPanelVisible();
    }

    protected ButtonDisclosurePanel getDisclosurePanel() {
        if (disclosurePanel == null) {
            disclosurePanel = new ButtonDisclosurePanel();

            // This listener is defined locally because it is for one
            // time use only to delay instantiation of making the real
            // component, so should be cleaned up when no longer necessary
            PropertyChangeListener firstTimeShow =
                new PropertyChangeListener() {
                private boolean firstTime = true;
                public void propertyChange(PropertyChangeEvent pe) {

                    if (pe
                        .getPropertyName()
                        .equals(DisclosurePanel.ABOUT_TO_SHOW_PROPERTY)) {
                        DisclosurePanel pan = (DisclosurePanel) pe.getSource();
                        JComponent comp = viewFactory.getPodComponent().getPodContent();
                        if (firstTime) {
                            firstTime = false;
                            int defWidth = viewFactory.getDefaultViewWidth();
                            int defHeight = viewFactory.getDefaultViewHeight();
                            if (defWidth > 0 && defHeight > 0) {
                                Dimension prefSize = comp.getPreferredSize();
                                if (defWidth > 0) prefSize.width = defWidth;
                                if (defHeight > 0) prefSize.height = defHeight;
                                comp.setPreferredSize(prefSize);
                            }
                        }
                        comp.setBorder(
                            BorderFactory.createCompoundBorder(
                                BorderFactory.createLineBorder(
                                    viewOuterBorder__,
                                    AppEnvironment.getInt(OUTER_BORDER_WIDTH)),
                                LookFactory.lightLoweredBorder()));
                        pan.getHiddenPanel().setBorder(
                            BorderFactory.createEmptyBorder(0, margin__, 0, 0));
                        pan.getHiddenPanel().add(comp);
                        ViewPanel.this.getSlider().setComponent(
                            getDisclosurePanel().getHiddenPanel());

                        pan.removePropertyChangeListener(this);
                    }
                }
            };

            // Design: The Disclosure panel is basically the Model for this
            // MVC system. We listen to the state of the model and then act as
            // the view by setting the relevant parts visible and invisible
            // All 'set' Calls are thus basically controllers on our model
            // and should no influence the view directly, but set the model
            // properly
            disclosurePanel.addPropertyChangeListener(firstTimeShow);
            disclosurePanel.addPropertyChangeListener(
                DisclosurePanel.PANEL_VISIBLE_PROPERTY,
                new PropertyChangeListener() {
                    public void propertyChange(PropertyChangeEvent pe) {
                        boolean b = ((Boolean)pe.getNewValue()).booleanValue();
                        doBigPanelVisible(b);
                    }
                });

            clear(disclosurePanel, foreground__);

            String titleBackground =
                AppEnvironment.getString(TITLE_BACKGROUND_COLOR);
            JComponent theSwitch =
                ((ButtonDisclosurePanel) disclosurePanel).getTheButton();
            if ((titleBackground != null) && (titleBackground.length() > 0)) {
                Color titleColor = Color.decode(titleBackground);
                theSwitch.setBackground(titleColor);
            } else {
                Color podColor =
                    Color.decode(AppEnvironment.getString(BACKGROUND_COLOR));
                theSwitch.setBackground(podColor);
            }
        }
        return disclosurePanel;
    }

    /**
     * Return the slider used with this view panel. Developers can add
     * a listener to this slider to know when the user is dragging.
     * @return Slider the slider for this listener
     */
    public Slider getSlider() {
        if (slider == null) {
            slider = new Slider();
        }
        return slider;
    }

    /**
     * A special top component so we can set title color or make it
     * transparent
     */
    private JPanel getTopComponent() {
        if (topComponent == null) {
            topComponent = new JPanel(topComponentLayout);

            // The first component added to a CardLayout is the visible
            // component when the container is first displayed.
            topComponent.add(getLabel(), CLOSED_STATE);
            topComponent.add(getOpenTopLine(), OPEN_STATE);

            String titleBackground =
                AppEnvironment.getString(TITLE_BACKGROUND_COLOR);
            if ((titleBackground != null) && (titleBackground.length() > 0)) {
                Color titleColor = Color.decode(titleBackground);
                topComponent.setOpaque(true);
                topComponent.setBackground(titleColor);
            } else {
                topComponent.setOpaque(false);
            }
        }
        return topComponent;
    }

    /**
     * Gets the view title
     * @return the view title string
     */
    String getViewTitle() {
        return viewFactory.getViewTitle();
    }

    /**
     * Get the label for this pod when it is closed.
     * @return the label for this pod
     */
    private JLabel getLabel() {
        if (topLabel == null) {
            topLabel = new JLabel(getViewTitle());
            topLabel.setHorizontalTextPosition(SwingConstants.LEFT);
            topLabel.setFont(titleFont__);
            // Create an empty border of the same size asthe RolloverJButton
            // borders. This prevents the label from moving when the "pod"
            // switches between "open" and "closed" states
            topLabel.setBorder(BorderFactory.createEmptyBorder(2,2,2,2));
            clear(topLabel, titleForeground__);
        }
        return topLabel;
    }

    /**
     * Returns the top element to show when the panel is open
     * @return the lement to put in the top component when the big
     * panel is open
     */
    private JComponent getOpenTopLine() {
        if (openLine == null) {
            MenuButton mb = viewFactory.getPodComponent().getMenuButton();
            JComponent cp = viewFactory.getPodComponent().getControlsPanel();
            JPanel p = null;

            if ((cp != null) || (mb != null)) {
                p = new JPanel();
                p.setLayout(new BoxLayout(p, BoxLayout.X_AXIS));
                if (mb == null) {
                    JLabel l = new JLabel(getViewTitle());
                    l.setHorizontalTextPosition(SwingConstants.LEFT);
                    l.setFont(titleFont__);
                    p.add(l);
                } else {
                    mb.setForeground(titleForeground__);
                    mb.setFont(titleFont__);
                    mb.setBorder(
                        BorderFactory.createLineBorder(titleForeground__));
                    if (p == null) {
                        p = new JPanel(new BorderLayout());
                    }
                    p.add(mb);
                    mb.setText(getViewTitle(), getViewTitle());
                    RolloverJButton.transformToRollover(mb);
                }
                p.add(Box.createHorizontalGlue());
                if (cp != null) {
                    p.add(cp);
                }
                openLine = p;
                clear(openLine, titleForeground__);
            } else {
                openLine = getLabel();
            }
        }
        return openLine;
    }

    /**
     * Sets the whether the big panel should be shown or not
     * @param visible if true the big panel should show, if false the pod should show
     * summary state
     */
    private void doBigPanelVisible(boolean visible) {
        JComponent smallPod = viewFactory.getSmallPodComponent();
        boolean oldVisible = !smallPod.isVisible();
        smallPod.setVisible(!visible);
        topComponentLayout.show(getTopComponent(),
            visible ? OPEN_STATE: CLOSED_STATE);
        bottomLine.removeAll();
        if (visible) {
            if (leftCornerOpen != null) {
                bottomLine.add(leftCornerOpen, BorderLayout.WEST);
            }
            if (rightCornerOpen != null) {
                bottomLine.add(rightCornerOpen, BorderLayout.EAST);
            }
        } else {
            if (leftCornerClosed != null) {
                bottomLine.add(leftCornerClosed, BorderLayout.WEST);
            }
            if (rightCornerClosed != null) {
                bottomLine.add(rightCornerClosed, BorderLayout.EAST);
            }
        }

        bottomLine.add(getSlider(), BorderLayout.CENTER);
        propOpen.firePropertyChange(oldVisible, visible);
        Window win = SwingUtilities.getWindowAncestor(this);
        if (win != null) win.pack();
    }
    static class ButtonDisclosurePanel extends DisclosurePanel implements Resource {
        private final static ImageIcon openIcon__ =
            AppEnvironment.getString(BUTTON_ICON_OPEN).length() != 0
            ? AppEnvironment.getIcon(BUTTON_ICON_OPEN)
            : null;
        private final static ImageIcon closedIcon__ =
            AppEnvironment.getString(BUTTON_ICON_CLOSED).length() != 0
            ? AppEnvironment.getIcon(BUTTON_ICON_CLOSED)
            : null;
        ButtonDisclosurePanel() {
            setOpenIcon(openIcon__);
            setClosedIcon(closedIcon__);
        }

        JComponent getTheButton() {
            return getSwitch();
        }
    }

    static class Corner extends JLabel {

        Corner(String key) {
            super.setIcon(AppEnvironment.getIcon(key));
        }
    }
}
