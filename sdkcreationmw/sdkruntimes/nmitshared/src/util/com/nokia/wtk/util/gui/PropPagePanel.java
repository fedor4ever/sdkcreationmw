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

/* java.awt */
import java.awt.Component;
import java.awt.LayoutManager;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.JPanel;

/**
 * A JPanel that implements {@link PropertyPage} interface.
 *
 */
public class PropPagePanel extends JPanel implements PropPage {

    private String label;
    private String tooltip;
    private Icon icon;

    /**
     * Creates <code>PropPagePanel</code> with specified layout manager.
     * The label, icon and other attributes can be set later.
     *
     * @param layout the layout manager for this panel
     */
    public PropPagePanel(LayoutManager layout) {
        super(layout);
    }

    /**
     * Creates <code>PropPagePanel</code> with specified label and no icon.
     *
     * @param label the label for this property set.
     */
    public PropPagePanel(String label) {
        this(label, null);
    }

    /**
     * Creates <code>PropPagePanel</code> with specified label and icon.
     *
     * @param label the label for this property set.
     * @param icon the icon for this property page.
     */
    public PropPagePanel(String label, Icon icon) {
        this.label = label;
        this.icon = icon;
    }

    /**
     * Creates <code>PropPagePanel</code> with specified label and
     * layout manager.
     *
     * @param layout the layout manager for this panel
     * @param label the label for this property set.
     */
    public PropPagePanel(LayoutManager layout, String label) {
        super(layout);
        this.label = label;
    }

    /**
     * Creates <code>PropPagePanel</code> with specified label, icon
     * and layout manager.
     *
     * @param layout the layout manager for this panel
     * @param label the label for this property set.
     * @param icon the icon for this property page.
     */
    public PropPagePanel(LayoutManager layout, String label, Icon icon) {
        super(layout);
        this.label = label;
        this.icon = icon;
    }

    /**
     * Creates <code>PropPagePanel</code> with specified label, tooltip
     * and layout manager.
     *
     * @param layout the layout manager for this panel
     * @param label the label for this property set.
     * @param tip the tooltip text for this page
     */
    public PropPagePanel(LayoutManager layout, String label, String tip) {
        super(layout);
        this.label = label;
        this.tooltip = tip;
    }

    /**
     * Gets the component that represents this property page on the screen.
     *
     * @return the component that represents this property page on the screen.
     */
    public Component getComponent() {
        return this;
    }

    /**
     * Gets the label of this property page. {@link #LABEL} property change
     * event is fired when this property changes.
     *
     * @return the label for this property page.
     */
    public String getLabel() {
        return label;
    }

    /**
     * Sets label for this property page. {@link #LABEL} property change
     * event is fired when this property changes.
     *
     * @param label the new label
     */
    public void setLabel(String label) {
        String oldLabel = this.label;
        this.label = label;
        firePropertyChange(LABEL_PROPERTY, oldLabel, label);
    }

    /**
     * Get the tooltip text for this property page. This method may return
     * <code>null</code> if property page does not have or does not
     * need a tooltip. {@link #TOOLTIP} property change event is fired
     * when this property changes.
     *
     * @return the tooltip text for this property page.
     */
    public String getToolTip() {
        return tooltip;
    }

    /**
     * Sets tooltip for this property page. {@link #TOOLTIP} property change
     * event is fired when this property changes.
     *
     * @param tooltip the new tooltip text
     */
    public void setToolTip(String tooltip) {
        String oldTooltip = this.tooltip;
        this.tooltip = tooltip;
        firePropertyChange(TOOLTIP_PROPERTY, oldTooltip, tooltip);
    }

    /**
     * Gets the icon for this property page. This method may return
     * <code>null</code> if property page does not have or does not
     * need an icon. {@link #ICON} property change event is fired
     * when this property changes.
     *
     * @return the icon for this property page, <code>null</code> if none
     */
    public Icon getIcon() {
        return icon;
    }

    /**
     * Sets icon for this property page. {@link #ICON} property change
     * event is fired when this property changes.
     *
     * @param icon the icon for this property page
     */
    public void setIcon(Icon icon) {
        Icon oldIcon = this.icon;
        this.icon = icon;
        firePropertyChange(ICON_PROPERTY, oldIcon, icon);
    }

    /**
     * Gets the help ID for this page, <code>null</code> if none.
     *
     * @return this implementation returns <code>null</code>
     */
    public String getHelpID() {
        return null;
    }
}
