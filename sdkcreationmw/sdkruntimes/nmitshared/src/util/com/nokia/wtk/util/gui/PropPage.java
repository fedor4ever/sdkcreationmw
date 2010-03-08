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

/* java.beans */
import java.beans.PropertyChangeListener;

/* java.awt */
import java.awt.Component;

/* javax.swing */
import javax.swing.Icon;

/**
 * A property page. Normally, these would be displayed within a tab
 * dialog but nothing prevents the application from displaying it
 * some other way.
 *
 */
public interface PropPage {

    /* properties */
    public static final String LABEL_PROPERTY   = "label";       // String
    public static final String ICON_PROPERTY    = "icon";        // Icon
    public static final String TOOLTIP_PROPERTY = "tooltip";     // String

    // don't change the following property name. We assume
    // that it's the same as the one used by JComponent
    public static final String ENABLED_PROPERTY = "enabled";     // Boolean

    /**
     * Gets the component that represents this property page on the screen.
     *
     * @return the component that represents this property page on the screen.
     */
    public Component getComponent();

    /**
     * Gets the label of this property page. {@link #LABEL} property change
     * event is fired when this property changes.
     *
     * @return the label for this property page.
     */
    public String getLabel();

    /**
     * Gets the icon for this property page. This method may return
     * <code>null</code> if property page does not have or does not
     * need an icon. {@link #ICON} property change event is fired
     * when this property changes.
     *
     * @return the icon for this property page, <code>null</code> if none
     */
    public Icon getIcon();

    /**
     * Get the tooltip text for this property page. This method may return
     * <code>null</code> if property page does not have or does not
     * need a tooltip. {@link #TOOLTIP} property change event is fired
     * when this property changes.
     *
     * @return the tooltip text for this property page.
     */
    public String getToolTip();

    /**
     * Determines whether this property page is enabled. {@link #TOOLTIP}
     * property change event is fired when this state changes.
     *
     * @return <code>true</code> is this property page is enabled.
     */
    public boolean isEnabled();

    /**
     * Gets the help ID for this page, <code>null</code> if none
     *
     * @return the help ID for this page, <code>null</code> if none
     */
    public String getHelpID();

    /**
     * Adds a {@link PropertyChangeListener} to the listener list.
     * The listener is registered for all properties.
     *
     * @param listener  the {@link PropertyChangeListener} to be added
     */
    public void addPropertyChangeListener(PropertyChangeListener listener);

    /**
     * Removes a {@link PropertyChangeListener} from the listener list.
     * This removes a PropertyChangeListener that was registered
     * for all properties.
     *
     * @param listener  the {@link PropertyChangeListener} to be removed
     */
    public void removePropertyChangeListener(PropertyChangeListener listener);
}
