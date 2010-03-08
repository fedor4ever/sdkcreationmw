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




package com.nokia.wtk.device;


import java.beans.PropertyEditor;
import javax.swing.Icon;
import javax.swing.JPanel;
import javax.swing.AbstractButton;
import javax.swing.JComboBox;
import javax.swing.text.JTextComponent;


/**
 * A service interface for interactively modifying device settings.
 * This interface must be implemented by a device simulator component,
 * if it is to support modifyable configuration settings.
 *
 */
public interface DeviceSettings {

    /**
     * Requests a set of property pages for use in modifying device settings.
     * These property pages will subsequently be embedded within a presentation
     * dialog for user interaction. The property pages will operate on a cloned
     * copy of device settings to allow changes to be applied or cancelled at will.
     * Device implementations should create a fresh set of property pages for
     * each call of this method, because the properties will not be the same
     * from one call to the next.
     *
     * @param pages     the container for the property pages
     * @param props     a cloned copy of the current device properties
     */
    public void getPropertyPages(PropertyPages pages, DeviceProperty.Set props);


    /**
     * A container of property pages for use within a device settings dialog.
     * This interface will be implemented by the hosting application.
     */
    public interface PropertyPages {

        /**
         * Creates and adds a new <code>PropertyPage</code> instance.
         * @param label     the property page label.
         * @param icon      the property page icon or <code>null</code>.
         * @param descr     the property page descriptive text or <code>null</code>.
         * @return          the newly-created property page component.
         */
        public PropertyPage addPage(String label, Icon icon, String descr);

    }   // End of DeviceSettings.PropertyPages


    /**
     * A single property page for use within a device settings dialog.
     * This interface will be implemented by the hosting application.
     * Instances of this interface should be created with the method
     * {@link DeviceSettings.PropertyPages#addPage}.
     */
    //TODO: Online help provisions from within a property page?
    public interface PropertyPage extends PropertyPages {

        /**
         * Returns the label of this property page.
         * @return the label for this page.
         */
        public String getLabel();

        /**
         * Returns the icon of this property page, if one exists.
         * @return the icon for this page or <code>null</code> if not applicable.
         */
        public Icon getIcon();

        /**
         * Returns the descriptive text for this property page.
         * This may be displayed directly or used as tooltip text if applicable.
         * @return the descriptive text or <code>null</code> if not applicable.
         */
        public String getDescription();

        /**
         * Returns the graphical component that contains the elements of this page.
         * @return the graphical container for this page.
         */
        public JPanel getPanel();

        /**
         * Returns the set of utility methods used to create property pages.
         * @return the graphical utilities instance.
         */
        public GUI getUtils();

    }   // End of DeviceSettings.PropertyPage


    /**
     * A set of utility methods used to create property pages.
     * This interface will be implemented by the hosting application.
     * It will not be registered with the device, but will be passed
     * as a method argument when requesting the set of property pages.
     */
    public interface GUI {

        /**
         * Gets the preferred amount of space between components.
         * @return          the preferred component gap
         */
        public int getComponentSpacing();

        /**
         * Creates a check box component that can edit a boolean property.
         * The component will be pre-configured to maintain synchronization
         * of its changing state and changing property value.
         * @param p         the property to edit
         * @param label     a label to use for the check box
         * @return          the check box component
         *                  or <code>null</code> if property is not defined
         */
        public AbstractButton createCheckBox(DeviceProperty p, String label);

        /**
         * Creates a set of radio button components that can edit a property
         * by choosing one of a fixed set of values.
         * The component will be pre-configured to maintain synchronization
         * of its changing state and changing property value.
         * @param p         the property to edit.
         * @param labels    the set of labels to use for the radio button components.
         *                  The size of the array determines the number of components created.
         * @param values    the set of values to associate with the radio button components.
         *                  The size of the array must match the size of <code>labels</code>.
         * @return          the set of radio button components.
         */
        public AbstractButton[] createRadioButtonGroup(DeviceProperty   p,
                                                       String[]         labels,
                                                       Object[]         values);

        /**
         * Creates a combo box component that can edit a property by choosing one
         * of a fixed set of values.
         * The component will be pre-configured to maintain synchronization
         * of its changing state and changing property value.
         * @param p the property to edit.
         * @param labels    the set of labels to use for the combo box.
         *                  The size of the array determines the number of items
         *                  in the combo box.
         * @param values    the set of values to associate with the labels.
         *                  The size of the array must match the size of <code>labels</code>.
         * @return          the combo box component.
         */
        public JComboBox createComboBox(DeviceProperty  p,
                                        String[]        labels,
                                        Object[]        values);

        /**
         * Creates a text field component that can edit a property as a single-line string.
         * In general, this component is only useful when editing scalar property
         * types (e.g. String, Integer, Float, File, URL).
         * The component will be pre-configured to maintain synchronization
         * of its changing state and changing property value.
         * @param p         the property to edit.
         * @param editor    an optional editor to use for the property
         * @param cols      the number of columns to use to calculate the preferred width
         * @return          the text field component.
         */
        public JTextComponent createTextField(DeviceProperty    p,
                                              PropertyEditor    editor,
                                              int               cols);

        /**
         * Creates a text area component that can edit a property as a multi-line string.
         * In general, this component is only useful when editing scalar property
         * types (e.g. String, Integer, Float, File, URL).
         * The component will be pre-configured to maintain synchronization
         * of its changing state and changing property value.
         * @param p         the property to edit
         * @param editor    an optional editor to use for the property
         * @param cols      the number of columns to use to calculate the preferred width
         * @param rows      the number of rows to use to calculate the preferred height
         * @return          the text field component.
         */
        public JTextComponent createTextArea(DeviceProperty     p,
                                             PropertyEditor     editor,
                                             int                cols,
                                             int                rows);

    }   // End of DeviceSettings.GUI

}   // End of DeviceSettings
