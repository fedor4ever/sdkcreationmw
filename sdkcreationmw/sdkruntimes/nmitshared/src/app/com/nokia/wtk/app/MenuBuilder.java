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

import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyEditor;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

import javax.swing.AbstractButton;
import javax.swing.Action;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JSeparator;
import javax.swing.KeyStroke;
import javax.swing.MenuElement;
import javax.swing.MenuSelectionManager;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.xml.parsers.FactoryConfigurationError;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertyEditors;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.gui.RolloverJButton;

/**
 * A utility class for creating menus.
 *
 */
public class MenuBuilder implements MessageWriters {

    /**
     * Creates a MenuBulder instance.
     * @param target the target class with which to resolve resource references
     * @param menuResource the name of the resource which contains the menu definition
     * @param actions a set of the target actions with which to resolve command references
     * @param properties a set of the target properties with which to resolve property references
     */
    MenuBuilder(Class target, Map actions, PropertySet properties) {
        if ((target == null)  || (actions == null) || (properties == null)) {
            throw new IllegalArgumentException("MenuBuilder: invalid constructor arguments");
        }
        this.targetClass = target;
        this.targetCommands = actions;
        this.targetProperties = properties;


    }

    JMenuBar loadMenuBar(String menuResource) {
        // Create a menu resource stream:
        InputStream is =
            (menuResource != null) ? targetClass.getResourceAsStream(menuResource)
                                                : null;
        if (is == null) {
            throw new IllegalArgumentException("MenuBuilder.createMenuBar: invalid or missing menu resource");
        }

        try {
            // Load a menu from the resource stream:
            JMenuBar menubar = loadMenuBar(is);
            // Save MenuBuilder instance for later:
            return menubar;
        }
        catch (IOException ex) {
MessageWriters.DetailMsg.println("MenuBuilder.createMenuBar: " + ex);
//ex.printStackTrace(MessageWriters.DetailMsg);
            return null;
        }
    }

    static private void unbindTraverse(MenuBuilder mb, MenuElement me) {
        if (me == null) {
            return;
        }
        if (me instanceof JMenuItem) {
            JMenuItem mi = (JMenuItem) me;
            mb.unbindButtonGroup(mi);
            if (mi instanceof AbstractButton) {
                AbstractButton ab = (AbstractButton) mi;
                mb.unbindAction(ab);
                mb.unbindProperty(ab);
            }
        }
        MenuElement[] mes = me.getSubElements();
        for (int j = 0; j < mes.length; j++) {
            unbindTraverse(mb, mes[j]);
        }
    }

    /**
     * Releases a menubar, rendering the associated MenuBuilder object unusable.
     * Actions and items will be unbound, components released, Hashtables emptied.
     * Recommended to use when menu item is no longer needed.
     */
    public void dispose(JMenuBar menubar) {

        MenuElement[] mis = menubar.getSubElements();
        for (int j = 0; j < mis.length; j++) {
            unbindTraverse(this, mis[j]);
        }
        this.menuComponents.clear();
        this.targetCommands.clear();
        if (this.buttonGroups != null) {
            this.buttonGroups.clear();
        }
        if (this.targetProperties != null) {
            this.targetProperties = null;
        }
        menubar.removeAll();

    }



    /**
     * Fetches a menu component by its identifying name.
     * @param id the identifying name of the menu component.
     * @return the menu component or <code>null</code> if <code>id</code> is not defined.
     */
    public Component getMenuComponent(String id) {
        Component menuComponent = null;
        if ((menuComponents != null) && (id != null)) {
            menuComponent = (Component)menuComponents.get(id);
        }
        return menuComponent;
    }

    /**
     * Stores a menu component by its identifying name.
     * @param id the identifying name of the menu component.
     * @param menuComponent the menu component to save.
     */
    private void putMenuComponent(String id, Component menuComponent) {
        if (menuComponents == null) {
            menuComponents = new Hashtable(7);
        }
        if ((id != null) && (id.length() > 0) && (menuComponent != null)) {
            menuComponents.put(id, menuComponent);
        }
    }

    /**
     * Removes a menu component by its identifying name.
     * @param id the identifying name of the menu component.
     * @return the menu component or <code>null</code> if <code>id</code> is not defined.
     */
    private Component removeMenuComponent(String id) {
        Component menuComponent = null;
        if ((menuComponents != null) && (id != null)) {
            menuComponent = (Component)menuComponents.remove(id);
        }
        return menuComponent;
    }

    /**
     * Removes a menu component by its identifying name.
     * @param id the identifying name of the menu component.
     * @return the menu component or <code>null</code> if <code>id</code> is not defined.
     */
    private String getComponentId(Component c) {
        String id = null;
        if ((menuComponents != null) && (c != null)) {
            if (menuComponents.containsValue(c)) {
                for (Iterator i = menuComponents.keySet().iterator(); i.hasNext();) {
                    id = (String)i.next();
                    if (menuComponents.get(id) == c) {
                        break;  // found matching key
                    } else {
                        id = null;
                    }
                }
            }
        }
        return id;
    }

    /**
     * Sets the enabled state of an identified menu item.
     * @param locId the identifying name of the menu component.
     */
    public void setEnabled(String locId, boolean enabled) {
        if ((locId != null) && (menuComponents != null) && (menuComponents.get(locId) != null)) {
            Component c = (Component)menuComponents.get(locId);
            if (c instanceof JMenuItem) {
                ((JMenuItem)c).setEnabled(enabled);
            }
        }
    }

    /**
     * Creates a unique id with which to identify a new menu component.
     * @return a unique id for a new menu component.
     */
    private String createUniqueId() {
        // Generate sequenced numbers until a unique number is found:
        String id = null;
        for (int number = 1; id == null; number++) {
            id = "ID#" + number;
            if ((menuComponents != null) && menuComponents.containsKey(id)) {
                id = null;  // Try again
            }
        }
        return id;
    }

    /**
     * Adds a new menu item or separator.
     * @param locId the location id of the menu component which determines where to add the new component.
     *           The new menu component will be added immediately after the identified location.
     * @param item the menu item to add.
     *             if <code>null</code> then separator will be added.
     * @return a unique id for the new menu component or <code>null</code> if <code>locId</code> was not defined.
     */
    private String addMenuItem(String locId, JMenuItem item) {
        String newId = null;
        if ((locId != null) && (menuComponents != null) && (menuComponents.get(locId) != null)) {
            Component c = (Component)menuComponents.get(locId);
//SYSOUT.println("MenuBuilder.addMenuItem(\"" + locId + "\", " + item + ")");
            //TODO: Cannot yet insert at the JMenuBar level
            if (c != null) {
/*/
                JMenu menu = (JMenu)c;
                if (item != null) {
                    menu.add(item);
                    newId = createUniqueId();
                    putMenuComponent(newId, item);
                } else {
                    menu.addSeparator();
                    newId = createUniqueId();
                    putMenuComponent(newId, menu.getMenuComponent(menu.getItemCount()-1));
                }
            } else if ((c instanceof JMenuItem) || (c instanceof JSeparator)) {
//*/
                JMenu menu = (JMenu)((JPopupMenu)c.getParent()).getInvoker();
                Component[] menuComponents = menu.getMenuComponents();
                for (int i = 0; i < menuComponents.length; i++) {
                    if (menuComponents[i] == c) {
                        boolean insert = (i < (menuComponents.length-1));
                        if (item != null) {
                            if (insert) {
                                menu.insert(item, i+1);
                            } else {
                                menu.add(item);
                            }
                            newId = createUniqueId();
                            putMenuComponent(newId, item);
                        } else {
                            Component separator;
                            if (insert) {
                                menu.insertSeparator(i+1);
                                separator = menu.getMenuComponent(i+1);
                            } else {
                                menu.addSeparator();
                                separator = menu.getMenuComponent(menuComponents.length);
                            }
                            newId = createUniqueId();
                            putMenuComponent(newId, separator);
                        }
                        break;
                    }
                }
            }
        }
        return newId;
    }

    /**
     * Adds a new menu.
     * @param locId the location id of the menu component which determines where to add the new component.
     *           The new menu component will be added immediately after the identified location.
     * @param label the label to use for the new menu.
     * @param icon the icon to use for the new menu.
     * @return two unique ids for the new menu (index 0) and its "first entry" marker (index 1)
     *         or <code>null</code> if <code>locId</code> was not defined.
     */
    public String[] addMenu(String locId, String label, Icon icon) {
        String[] newIds = null;
        if ((locId != null) && (menuComponents != null) && (menuComponents.get(locId) != null)) {
            newIds = new String[2];
            JMenu menu = new JMenu(label);
            bindIcons(menu, icon);
            newIds[0] = addMenuItem(locId, menu);
            // Create a marker as the first menu entry:
            menu.add(Box.createVerticalStrut(0));
            newIds[1] = createUniqueId();
            putMenuComponent(newIds[1], menu.getMenuComponent(0));
        }
        return newIds;
    }

    /**
     * Adds a new menu item.
     * @param locId the location id of the menu component which determines where to add the new component.
     *           The new menu component will be added immediately after the identified location.
     * @param label the label to use for the new menu item.
     * @param icon the icon to use for the new menu item.
     * @param action the action to bind to the new menu item.
     * @param command the action command to send to the bound action when the menu item is selected.
     * @return a unique id for the new menu component or <code>null</code> if <code>locId</code> was not defined.
     */
    public String addMenuItem(String locId, String label, Icon icon, Action action, String command) {
        String newId = null;
        if ((locId != null) && (menuComponents != null) && (menuComponents.get(locId) != null)) {
            String[] labelComponents = parseLabelComponents(label);
            JMenuItem menuItem = new JMenuItem(labelComponents[0], icon);
            bindMnemonic(menuItem, labelComponents[1]);
            bindAccelerators(menuItem, labelComponents[2]);
            bindAction(menuItem, action, command);
            newId = addMenuItem(locId, menuItem);
        }
        return newId;
    }

    /**
     * Adds a new menu separator.
     * @param locId the location id of the menu component which determines where to add the new component.
     *           The new menu component will be added immediately after the identified location.
     * @return a unique id for the new menu separator or <code>null</code> if <code>locId</code> was not defined.
     */
    public String addSeparator(String locId) {
        String newId = null;
        if ((locId != null) && (menuComponents != null) && (menuComponents.get(locId) != null)) {
            newId = addMenuItem(locId, null);
        }
        return newId;
    }

    /**
     * Adds a new check box menu item.
     * @param locId the location id of the menu component which determines where to add the new component.
     *           The new menu component will be added immediately after the identified location.
     * @param label the label to use for the new menu item.
     * @param icon the icon to use for the new menu item.
     * @param property the name of a property to bind to the new menu item.
     * @param selected the text value of the property which represents the selected state
     *                 of the new menu item.
     * @param deselected the text value of the property which represents the deselected state
     *                 of the new menu item.
     * @return a unique id for the new menu component or <code>null</code> if <code>locId</code> was not defined.
     */
    public String addCheckBoxMenuItem(String locId, String label, Icon icon, String property, String selected, String deselected) {
        String newId = null;
        if ((locId != null) && (menuComponents != null) && (menuComponents.get(locId) != null)) {
            String[] labelComponents = parseLabelComponents(label);
            JCheckBoxMenuItem menuItem = new JCheckBoxMenuItem(labelComponents[0], icon);
            bindMnemonic(menuItem, labelComponents[1]);
            bindAccelerators(menuItem, labelComponents[2]);
            bindProperty(menuItem, property, selected, deselected);
            newId = addMenuItem(locId, menuItem);
        }
        return newId;
    }

    /**
     * Adds a new radio button menu item.
     * @param locId the location id of the menu component which determines where to add the new component.
     *              The new menu component will be added immediately after the identified location.
     * @param label the label to use for the new menu item.
     * @param icon the icon to use for the new menu item.
     * @param group the name of a ButtonGroup to bind to the new menu item to allow single-selection within a group.
     * @param property the name of a property to bind to the new menu item.
     *                 This presumes that the property exists in the set that was originally
     *                 registered with the MenuBuilder instance.
     * @param property the name of a property to bind to the new menu item.
     * @param selected the text value of the property which represents the selected state
     *                 of the new menu item.
     * @return a unique id for the new menu component or <code>null</code> if <code>locId</code> was not defined.
     */
    public String addRadioButtonMenuItem(String locId, String label, Icon icon, String group, String property, String selected) {
        String newId = null;
        if ((locId != null) && (menuComponents != null) && (menuComponents.get(locId) != null)) {
            String[] labelComponents = parseLabelComponents(label);
            JRadioButtonMenuItem menuItem = new JRadioButtonMenuItem(labelComponents[0], icon);
            bindButtonGroup(menuItem, group);
            bindMnemonic(menuItem, labelComponents[1]);
            bindAccelerators(menuItem, labelComponents[2]);
            bindProperty(menuItem, property, selected, null);
            newId = addMenuItem(locId, menuItem);
        }
        return newId;
    }

    /**
     * Removes a menu component.
     * @param id the location id of the menu component which determines the component to remove.
     */
    public void removeItem(String id) {
        Component c = removeMenuComponent(id);
        if (c != null) {
            JMenu menu = (JMenu)((JPopupMenu)c.getParent()).getInvoker();
            if (c instanceof JMenu) {
                Component[] children = ((JMenu)c).getMenuComponents();
                for (int i = 0; i < children.length; i++) {
                    removeItem(getComponentId(children[i]));
                }
            } else if (c instanceof JCheckBoxMenuItem) {
                ((JMenuItem)c).setAccelerator(null);
                unbindAction((AbstractButton)c);
                unbindProperty((JMenuItem)c);
            } else if (c instanceof JRadioButtonMenuItem) {
                ((JMenuItem)c).setAccelerator(null);
                unbindAction((AbstractButton)c);
                unbindProperty((JMenuItem)c);
                unbindButtonGroup((JMenuItem)c);
            } else if (c instanceof JMenuItem) {
                ((JMenuItem)c).setAccelerator(null);
                unbindAction((AbstractButton)c);
            }
            menu.remove(c);
        }
    }

    /**
     * Removes a range of menu components.
     * Two component identifiers are use to specify a range
     * of menu components within the same menu.  The range
     * excludes the first item and includes all items up to
     * and including the last item.
     * @param from the location id of the menu component just prior the first component to remove.
     * @param to the location id of the last component to remove.
     */
    public void removeItems(String from, String to) {
        Component cStart = getMenuComponent(from);
        Component cEnd = getMenuComponent(to);
        if ((cStart != null) && (cEnd != null)) {
            // Component parents must be the same:
            if (cStart.getParent() == cEnd.getParent()) {
                JMenu menu = (JMenu)((JPopupMenu)cStart.getParent()).getInvoker();
                Component[] sibling = menu.getMenuComponents();
                // Get the IDs of each item in the range:
                Vector items = new Vector();
                boolean started = false;
                for (int i = 0; i < sibling.length; i++) {
                    if (!started && (sibling[i] == cStart)) {
                        started = true;
                    } else if (started) {
                        items.add(getComponentId(sibling[i]));
                        if (sibling[i] == cEnd) {
                            break;
                        }
                    }
                }
                // Now remove each item in the range:
                for (int i = 0; i < items.size(); i++) {
                    removeItem((String)items.elementAt(i));
                }
            }
        }
    }

    /**
     * Parses a menu label into it component parts.
     * The format of a label string allows for a label,
     * a possible mnemonic character as defined by an
     * ampersand escape within the label, and a possible
     * list of keyboard accelerators as defined by a
     * semicolon-delimited list.
     * The label components will be returned in an array
     * of string values as follows:
     * <ul>
     * <li>[0]: the label component</li>
     * <li>[1]: the mnemonic component</li>
     * <li>[2]: the keyboard accelerators component</li>
     * </ul>
     * @param label the label string to parse.
     * @return the set of label components.
     */
    private String[] parseLabelComponents(String label) {
        String mnemonic = null;
        String accelerators = null;
        if ((label != null) && (label.length() > 0)) {
            int semicolon = label.indexOf(';');
            if (semicolon >= 0) {
                accelerators = label.substring(semicolon+1);
                label = label.substring(0, semicolon);
            }
            StringBuffer sbuf = new StringBuffer();
            for (int index = 0; index < label.length(); ) {
                int ampersand = label.indexOf(index, '&');
                if (ampersand >= 0) {
                    sbuf.append(label.substring(index, ampersand));
                    if ((ampersand+1) < label.length()) {
                        char nextChar = label.charAt(ampersand+1);
                        if (nextChar == '&') {
                            sbuf.append(nextChar);
                        } else {
                            mnemonic = label.substring((ampersand+1), (ampersand+2));
                        }
                        index = ampersand + 2;
                    } else {
                        index = ampersand + 1;
                    }
                } else {
                    sbuf.append(label.substring(index, label.length()));
                    index = label.length();
                }
            }
            label = sbuf.toString();
        }
        return new String[] {label, mnemonic, accelerators};
    }

    /**
     * Fetches an action by name.
     * @param name the name of the action.
     * @return the named action or <code>null</code> if not defined.
     */
    private Action getAction(String name) {
        Action action = null;
        if ((targetCommands != null) && (name != null)) {
            action = (Action)targetCommands.get(name);
        }
        return action;
    }

    /**
     * Creates an icon from a resource name.
     * @param name the name of the icon resource.
     * @return the icon or <code>null</code> if not found.
     */
    private Icon createIcon(String name) {
        Icon icon = null;
        if (name != null) {
            URL url = targetClass.getResource(name);
            if (url != null) {
                icon = new ImageIcon(url);
            }
        }
        return icon;
    }

    /**
     * Binds one or more icons to a menu item.
     * <p>
     * By default, a single icon will represent the default icon.
     * If multiple icons are specified, the icons will be applied in the following order:
     * <ol start="0">
     * <li>The default icon</li>
     * <li>The "selected" icon</li>
     * <li>The "rollover" icon</li>
     * <li>The "rollover-selected" icon</li>
     * <li>The "pressed" icon</li>
     * </ol>
     * @param menuItem the menu item to be bound.
     * @param iconObj either a the string identifying one or more icons
     *                or an <code>Icon</code> or <code>Icon[]</code> instance.
     *                If a string is to represent multiple icons it must use the
     *                semicolon character to delimit each icon description.
     */
//BTW: Only the default icon is used in menu items, so this effort has been wasted.
    private void bindIcons(AbstractButton menuItem, Object iconObj) {
        Icon[] icons = null;

        // Create Icons:
        if (iconObj instanceof String) {
            String[] strAry = StringUtils.tokenize((String)iconObj, ";", true);
            icons = new Icon[strAry.length];
            for (int i = 0; i < strAry.length; i++) {
                icons[i] = createIcon(strAry[i]);
            }
        } else if (iconObj instanceof String[]) {
            String[] strAry = (String[])iconObj;
            icons = new Icon[strAry.length];
            for (int i = 0; i < strAry.length; i++) {
                icons[i] = createIcon(strAry[i]);
            }
        } else if (iconObj instanceof Icon) {
            icons = new Icon[] {(Icon)iconObj};
        } else if (iconObj instanceof Icon[]) {
            icons = (Icon[])iconObj;
        }

        // Bind Icons:
        if (icons != null) {
            for (int i = 0; i < icons.length; i++) {
                if (icons[i] == null) {
                    continue;
                }
                switch (i) {
                case 0:
                    menuItem.setIcon(icons[i]);
                    break;
                case 1:
                    menuItem.setSelectedIcon(icons[i]);
                    break;
                case 2:
                    menuItem.setRolloverIcon(icons[i]);
                    break;
                case 3:
                    menuItem.setRolloverSelectedIcon(icons[i]);
                    break;
                case 4:
                    menuItem.setPressedIcon(icons[i]);
                    break;
                }
            }
        }
    }

    /**
     * Binds a key mnemonic to a menu item.
     * @param menuItem the menu item to be bound.
     * @param mnemonic the string identifying the mnemonic.
     */
    private void bindMnemonic(AbstractButton menuItem, String mnemonic) {
        if ((mnemonic != null) && (mnemonic.length() == 1)) {
            menuItem.setMnemonic(mnemonic.charAt(0));
            Action a = (Action)menuItem.getClientProperty(ActionProperty);
            if (a != null) {
                a.putValue(Action.MNEMONIC_KEY, new Integer(menuItem.getMnemonic()));
            }
        }
    }

    /**
     * Binds one or more keyboard shortcuts to a menu item.
     * @param menuItem the menu item to be bound.
     * @param accelerator the text description the accelerator.
     *        More than one accelerator may be specified by separating
     *        each description with a semicolon character.  Only the
     *        first accelerator description will be visible on the menu item.
     */
    private void bindAccelerators(AbstractButton menuItem, String accelerator) {
        String[] acceleratorSet = StringUtils.tokenize(accelerator, ";", true);
        for (int i = 0; i < acceleratorSet.length; i++) {
            KeyStroke keyStroke = PropertyEditors.decodeKeyStroke(acceleratorSet[i]);
            if (keyStroke != null) {
                if ((i == 0) && (menuItem instanceof JMenuItem)) {
                    // First item becomes the visible accelerator:
                    ((JMenuItem)menuItem).setAccelerator(keyStroke);

                    // Also assign accelerator to the action. This is used
                    // to display acelerators in popup menus
                    Action a = (Action)menuItem.getClientProperty(ActionProperty);
                    if (a != null) {
                        a.putValue(Action.ACCELERATOR_KEY, keyStroke);
                    }

                } else {
                    // Additional items become aliased accelerators:
                    final AbstractButton fButton = menuItem;
                        menuItem.registerKeyboardAction(new ActionListener() {
                                public void actionPerformed(ActionEvent e) {
                                    MenuSelectionManager.defaultManager().clearSelectedPath();
                                    fButton.doClick();
                                }
                        }, keyStroke, JComponent.WHEN_IN_FOCUSED_WINDOW);
                }
            }
        }
    }

    /**
     * Binds an action to a menu item.
     * @param menuItem the menu item to be bound.
     * @param action the action to bind.
     * @param command the action command.
     */
    private void bindAction(AbstractButton menuItem, Action action, String command) {
        if (action != null) {

            // Bind accelerator to the menu action
            if (action != null && (menuItem instanceof JMenuItem)) {
                KeyStroke accelerator = ((JMenuItem)menuItem).getAccelerator();
                action.putValue(Action.ACCELERATOR_KEY, accelerator);
                action.putValue(Action.MNEMONIC_KEY, new Integer(menuItem.getMnemonic()));
            }

            // Assign action command for button event:
            if (command != null) {
                menuItem.setActionCommand(command);
            }

            // Invoke action on button event:
            menuItem.addActionListener(action);
            menuItem.putClientProperty(ActionProperty, action);

            // Reflect current action property values:
            menuItem.setIcon((Icon)action.getValue(Action.SMALL_ICON));
            menuItem.setEnabled(action.isEnabled());

            // Reflect action property changes:
            final AbstractButton fmi = menuItem;
            PropertyChangeListener pcl = new PropertyChangeListener() {
                public void propertyChange(PropertyChangeEvent e) {
                    if (Action.SMALL_ICON.equals(e.getPropertyName())) {
                        fmi.setIcon((Icon)e.getNewValue());
                    } else if ("enabled".equals(e.getPropertyName())) {
                        Boolean enabled = (Boolean)e.getNewValue();
                        fmi.setEnabled(enabled.booleanValue());
                    } else if (Action.NAME.equals(e.getPropertyName())) {
                        fmi.setText((String)e.getNewValue());
                    } else if (Action.MNEMONIC_KEY.equals(e.getPropertyName())) {
                        fmi.setMnemonic(((Integer)e.getNewValue()).intValue());
                    } else if (Action.ACCELERATOR_KEY.equals(e.getPropertyName())) {
                        if (fmi instanceof JMenuItem) {
                            ((JMenuItem)fmi).setAccelerator((KeyStroke)e.getNewValue());
                        }
                    }
                }
            };
            action.addPropertyChangeListener(pcl);
            menuItem.putClientProperty(ActionPropertyChangeListenerProperty, pcl);
        } else {    // Action not defined
            menuItem.setEnabled(false);
        }
    }

    /**
     * Unbinds an action from a menu item.
     * @param menuItem the menu item to be unbound.
     */
    private void unbindAction(AbstractButton menuItem) {
        Action action = (Action)menuItem.getClientProperty(ActionProperty);
        if (action != null) {

            // Remove action binding:
            menuItem.removeActionListener(action);

            // Remove action property change listener binding:
            PropertyChangeListener pcl = (PropertyChangeListener)menuItem.getClientProperty(ActionPropertyChangeListenerProperty);
            if (pcl != null) {
                action.removePropertyChangeListener(pcl);
            }
        }
    }

    private void bindHelpIDResource(AbstractButton menuItem, String resource) {
        // No support for Java help anymore
    }

    /**
     * Binds a property to a menu item.
     * @param menuItem the menu item to be bound.
     * @param name the name of the property.
     * @param selected the property value (as text) representing the "selected" state.
     * @param deselected the property value (as text) representing the "deselected" state.
     */
    private void bindProperty(AbstractButton menuItem, final String name, String selected, String deselected) {
        final Property p = targetProperties.getProperty(name);
        if (p != null) {
//SYSOUT.println("MenuBuilder.bindProperty(..., \"" + name + "\", " + selected + ", " + deselected + ")");
            // Initialize default "selected","deselected" values:
            Object selectedValue = null;
            Object unselectedValue = null;
            PropertyEditor editor = PropertyEditors.getEditor(p.getType());
            if (editor != null) {
                if (selected != null) {
                    // Read "selected" value:
                    editor.setAsText(selected);
                    Object value = editor.getValue();
                    if (value != null) {
                        selectedValue = value;
                    }
                }
                if (deselected != null) {
                    // Read "deselected" value:
                    editor.setAsText(deselected);
                    Object value = editor.getValue();
                    if (value != null) {
                        unselectedValue = value;
                    }
                }
            }

            // Initialize "selected" state based on the current value
            // of the property. The "selected" value must be set. If not,
            // do nothing and complain about it.
            if (selectedValue != null) {
                menuItem.setSelected(selectedValue.equals(p.getValue()));
            } else {
                String msg = "MenuBuilder.bindProperty(..., \"" + name + "\", " + selected + ", " + deselected + ") ... SELECTED VALUE IS NULL.";
                SYSOUT.println(msg);
                DebugMsg.println(msg);
            }

            final AbstractButton fmi = menuItem;
            final Object fsv = selectedValue;
            final Object fuv = unselectedValue;
            final String shortName = name.substring(name.lastIndexOf('.')+1);

            PropertyChangeListener pcl = new PropertyChangeListener() {
                public void propertyChange(PropertyChangeEvent evt) {
                    if (fsv != null) {
                        // Only do setSelected(false) if "unselected" value
                        // is defined. Typically toggle buttons have both
                        // "selected" and "unselected" values, and radio
                        // buttons have only "selected" values.

                        boolean selectedNew = fsv.equals(evt.getNewValue());
                        if (selectedNew|| fuv != null) {
                            fmi.setSelected(selectedNew);
                        }

                        // If we did setSelected(false) on a radio button, it
                        // would produce undesirable behavior. Here is the
                        // scenario  of such behavior - DeviceManager selecting
                        // a new device which fails to initialize.
                        //
                        // Selection of new device is caused by
                        // "device.current-index" property changing its
                        // state. Now, suppose the new device fails to
                        // initialize, and the radio button is pointing to
                        // the bad device. In such case DeviceManager wants
                        // to switch the radio buttons back to point to the
                        // old device. Let 1 be the index of the new "bad"
                        // device, and 4 be the index of the old "good" one.
                        // Let's see what happens if DeviceManager set the
                        // value of "device.current-index" property to 4
                        // (the "1" button is currently selected)
                        //
                        // 1. The property value is changed from 4 back to 1
                        // 2. The "1" menu item gets notified first about
                        //    the change. Since the new value if "4", we do
                        //
                        //        fmi.setSelected(false)
                        //
                        // 3. The toggle button model invokes
                        //
                        //        getGroup().setSelected(this, false);
                        //
                        // 4. The button group does nothing if the second
                        //    parameter is false (see ButtonGroup.setSelected)
                        //    and the menu item remains selected.
                        // 5. The toggle button model fires the change event
                        // 6. The MenuBuilder change listener gets this
                        //    notification from the "1" button which is
                        //    still SELECTED! So it switches the property
                        //    back to "1"
                        // 7. This makes DeviceManager make another attempt
                        //    to load the bad device.
                        // 8. When the "4" button is finally notified that
                        //    the new value is "4", we do
                        //
                        //        fmi.setSelected(true)
                        //
                        // I've fixed this by avoiding fmi.setSelected(false)
                        // for radio buttons (assuming that those with null
                        // unselected values are radio buttons).
                        //
                    }
                }
            };
            p.addPropertyChangeListener(pcl);
            menuItem.putClientProperty(PropertyNameProperty, name);
            menuItem.putClientProperty(PropertyChangeListenerProperty, pcl);

            // Change the property value when the "selected" state changes:
            menuItem.addChangeListener(new ChangeListener() {
                public void stateChanged(ChangeEvent e) {
                    // if a value is not specified for this state, then
                    // do nothing. For example, unselected value is undefined
                    // for properties associated with radio buttons
                    Object value = (fmi.isSelected() ? fsv : fuv);
                    if (value != null) {
                        p.setValue(value);
                    }
                }
            });
        } else {    // Property not defined
            if (verbose) {
                String msg = "MenuBuilder.bindProperty(..., \"" + name + "\", " + selected + ", " + deselected + ") ... NOT FOUND.";
                SYSOUT.println(msg);
                DebugMsg.println(msg);
            }
            menuItem.setEnabled(false);
        }
    }

    /**
     * Unbinds a property from a menu item.
     * @param menuItem the menu item to be unbound.
     */
    private void unbindProperty(AbstractButton menuItem) {
        String name = (String)menuItem.getClientProperty(PropertyNameProperty);
        if (name != null) {
            PropertyChangeListener pcl = (PropertyChangeListener)
                    menuItem.getClientProperty(PropertyChangeListenerProperty);
            if (pcl != null) {
                Property p = targetProperties.getProperty(name);
                if (p != null) {
                    p.removePropertyChangeListener(pcl);
                }
            }
        }
    }

    /**
     * Binds a ButtonGroup to a menu item.
     * @param menuItem the menu item to be bound.
     * @param name the name of the ButtonGroup.
     * @param radioSelected whether the JMenuItem being added to the group is being selected.
     */
    private void bindButtonGroup(JMenuItem menuItem, String name) {
        ButtonGroup group = null;
        if (name != null) {
            if (buttonGroups == null) {
                buttonGroups = new Hashtable(4);
            }
            group = (ButtonGroup)buttonGroups.get(name);
            if (group == null) {
                group = new ButtonGroup();
                buttonGroups.put(name, group);
            }
            group.add(menuItem);
            menuItem.putClientProperty(ButtonGroupProperty, group);
        }
    }

    /**
     * Unbinds a ButtonGroup from a menu item.
     * @param menuItem the menu item to be unbound.
     */
    private void unbindButtonGroup(JMenuItem menuItem) {
        if (buttonGroups != null) {
            ButtonGroup group = (ButtonGroup)menuItem.getClientProperty(ButtonGroupProperty);
            if (group != null) {
                group.remove(menuItem);
            }
        }
    }
                // Create a handler class for SAX document events:
    private class Loader extends DefaultHandler {
                // Track element nodes as document is parsed.
                private JMenuBar rootNode = null;
                private JMenu currentNode = null;

                public void startElement(String uri, String localName, String qname,
                   Attributes attrs) throws SAXException {
                    if (MenuBarElementName.equals(qname)) {
                        // JMenuBar (Root) Element:
                        if (rootNode == null) {
                            rootNode = new JMenuBar();
                            putMenuComponent(attrs.getValue(IdAttributeName), rootNode);
                        } else {
                            //ERROR
                        }
                    } else if (MenuElementName.equals(qname)) {
                        // JMenu Element:
                        JMenu newNode = new JMenu(attrs.getValue(LabelAttributeName));
                        putMenuComponent(attrs.getValue(IdAttributeName), newNode);
                        bindIcons(newNode, attrs.getValue(IconAttributeName));
                        bindMnemonic(newNode, attrs.getValue(MnemonicAttributeName));
                        bindHelpIDResource(newNode, attrs.getValue(HelpResourceName));
                        if (currentNode != null) {
                            currentNode.add(newNode);
                            currentNode = newNode;
                        } else {    // Top-level Menu
                            if (rootNode != null) {
                                rootNode.add(newNode);
                            }
                            currentNode = newNode;
                        }
                    } else if (ActionMenuItemElementName.equals(qname)) {
                        // JMenuItem Element:
                        JMenuItem newItem = new JMenuItem(attrs.getValue(LabelAttributeName));
                        putMenuComponent(attrs.getValue(IdAttributeName), newItem);
                        bindMnemonic(newItem, attrs.getValue(MnemonicAttributeName));
                        bindAccelerators(newItem, attrs.getValue(AcceleratorAttributeName));
                        // Action must be defined for menu item to be enabled
                        bindAction(newItem,
                                   getAction(attrs.getValue(ActionAttributeName)),
                                   attrs.getValue(CommandAttributeName));
                        bindIcons(newItem, attrs.getValue(IconAttributeName));
                        bindHelpIDResource(newItem, attrs.getValue(HelpResourceName));
                        if (currentNode != null) {
                            currentNode.add(newItem);
                        } else if (rootNode != null) {
                            rootNode.add(newItem);
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    } else if (CheckBoxMenuItemElementName.equals(qname)) {
                        // JCheckBoxMenuItem Element:
                        JCheckBoxMenuItem newItem = new JCheckBoxMenuItem(attrs.getValue(LabelAttributeName));
                        putMenuComponent(attrs.getValue(IdAttributeName), newItem);
                        bindIcons(newItem, attrs.getValue(IconAttributeName));
                        bindMnemonic(newItem, attrs.getValue(MnemonicAttributeName));
                        bindHelpIDResource(newItem, attrs.getValue(HelpResourceName));
                        bindAccelerators(newItem, attrs.getValue(AcceleratorAttributeName));
                        // Property must be defined for menu item to be enabled
                        // Assume true and false values by default
                        String select = attrs.getValue(SelectedAttributeName);
                        String deselect = attrs.getValue(DeselectedAttributeName);
                        if (select == null) select = Boolean.TRUE.toString();
                        if (deselect == null) deselect = Boolean.FALSE.toString();
                        bindProperty(newItem,
                                     attrs.getValue(PropertyAttributeName),
                                     select,
                                     deselect);

                        if (currentNode != null) {
                            currentNode.add(newItem);
                        } else if (rootNode != null) {
                            rootNode.add(newItem);
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    } else if (RadioButtonMenuItemElementName.equals(qname)) {
                        // JRadioButtonMenuItem Element:
                        JRadioButtonMenuItem newItem = new JRadioButtonMenuItem(attrs.getValue(LabelAttributeName));
                        putMenuComponent(attrs.getValue(IdAttributeName), newItem);
                        bindIcons(newItem, attrs.getValue(IconAttributeName));
                        bindMnemonic(newItem, attrs.getValue(MnemonicAttributeName));
                        bindHelpIDResource(newItem, attrs.getValue(HelpResourceName));
                        bindAccelerators(newItem, attrs.getValue(AcceleratorAttributeName));
                        //TODO: Should missing group attribute be handled by default?
                        bindButtonGroup(newItem, attrs.getValue(GroupAttributeName));
                        // Property must be defined for menu item to be enabled
                        String select = attrs.getValue(SelectedAttributeName);
                        if (select == null) select = Boolean.TRUE.toString();
                        bindProperty(newItem,
                                     attrs.getValue(PropertyAttributeName),
                                     select,
                                     null);
                        if (currentNode != null) {
                            currentNode.add(newItem);
                        } else if (rootNode != null) {
                            rootNode.add(newItem);
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    } else if (ButtonMenuItemElementName.equals(qname)) {
                        // JButton Element:
                        if ((currentNode != null) || (rootNode != null)) {
                            final JButton button = new RolloverJButton(attrs.getValue(LabelAttributeName));
                            button.setMargin(new Insets(0,0,0,0));
                            button.setFocusPainted(false);
                            button.setContentAreaFilled(true);

                            button.setToolTipText(attrs.getValue(TooltipAttributeName));
                            putMenuComponent(attrs.getValue(IdAttributeName), button);

                            // Action must be defined for menu item to be enabled
                            bindAction(button,
                                       getAction(attrs.getValue(ActionAttributeName)),
                                       attrs.getValue(CommandAttributeName));
                            bindIcons(button, attrs.getValue(IconAttributeName));

                            // place it into a panel so that it does not
                            // get stretched vertically
                            JPanel bp = new JPanel(new FlowLayout(FlowLayout.RIGHT,0,0)) {
                                public Dimension getMinimumSize() {
                                    return button.getMinimumSize();
                                }
                                public Dimension getMaximumSize() {
                                    return button.getMaximumSize();
                                }
                                public Dimension getPreferredSize() {
                                    return button.getPreferredSize();
                                }
                                public boolean isMinimumSizeSet() {return true;}
                                public boolean isMaximumSizeSet() {return true;}
                                public boolean isPreferredSizeSet() {return true;}
                            };
                            bp.add(button);

                            if (currentNode != null) {
                                currentNode.add(bp);
                            } else {
                                rootNode.add(bp);
                            }
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    } else if (MenuSeparatorElementName.equals(qname)) {
                        // JSeparator Element:
                        if (currentNode != null) {
                            currentNode.addSeparator();
                            putMenuComponent(attrs.getValue(IdAttributeName), currentNode.getMenuComponent(currentNode.getItemCount()-1));
                        } else if (rootNode != null) {
                            //TODO: Fix: JSeparator acts like "glue" in this mode
                            Component c = new JSeparator(SwingConstants.VERTICAL);
                            putMenuComponent(attrs.getValue(IdAttributeName), c);
                            rootNode.add(c);
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    } else if (MenuBarGlueElementName.equals(qname)) {
                        // MenuBar Glue Element:
                        if ((currentNode == null) && (rootNode != null)) {
                            Component c = Box.createHorizontalGlue();
                            putMenuComponent(attrs.getValue(IdAttributeName), c);
                            rootNode.add(c);
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    } else if (MenuBarStrutElementName.equals(qname)) {
                        // MenuBar Strut Element:
                        if ((currentNode == null) && (rootNode != null)) {
                            String sizeAttr = attrs.getValue(SizeAttributeName);
                            if (sizeAttr != null) {
                                try {
                                    Component c = Box.createHorizontalStrut(Integer.parseInt(sizeAttr));
                                    putMenuComponent(attrs.getValue(IdAttributeName), c);
                                    rootNode.add(c);
                                }
                                catch (NumberFormatException ex) {}
                            }
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    } else if (MenuMarkElementName.equals(qname)) {
                        // Non-rendering Mark Element:
                        if (currentNode != null) {
                            Component c = Box.createVerticalStrut(0);
                            putMenuComponent(attrs.getValue(IdAttributeName), c);
                            currentNode.add(c);
                        } else if (rootNode != null) {
                            Component c = Box.createVerticalStrut(0);
                            putMenuComponent(attrs.getValue(IdAttributeName), c);
                            rootNode.add(c);
                        } else {
                            if (verbose) Thread.dumpStack(); //ERROR
                        }
                    }
//System.out.println(" ... currentNode=" + toString(currentNode) + ", rootNode=" + toString(rootNode));
                }

                public void endElement(String uri, String localName, String qname) throws SAXException {
                    Component currentNodeParent = (currentNode != null) ? currentNode.getParent() : null;
//System.out.println("MenuBuilder.Loader.loadMenu: endElement(\"" + qname + "\"): currentNode=" + toString(currentNode) + ", rootNode=" + toString(rootNode) + ", currentNodeParent=" + toString(currentNodeParent));
                    if (MenuElementName.equals(qname) && (currentNode != null)) {
                        if (currentNode.getParent() instanceof JMenu) {
                            currentNode = (JMenu)currentNode.getParent();
                        } else if (currentNode.getParent() instanceof JPopupMenu) {
                            currentNode = (JMenu)((JPopupMenu)currentNode.getParent()).getInvoker();
                        } else if (currentNode.getParent() instanceof JMenuBar) {
                            // End of Top-level Menu
                            currentNode = null;
                        }
                    }
//System.out.println(" ... currentNode=" + toString(currentNode) + ", rootNode=" + toString(rootNode));
                }

//private String toString(Object obj) { return (obj != null) ? obj.getClass().getName() : "<null>"; }

                // treat validation errors as fatal
                public void error (SAXParseException err) throws SAXParseException {
                    throw err;
                }

                // dump warnings too
                public void warning (SAXParseException err) throws SAXParseException {
                    throw err;
                }
            }   // End of Loader

    /**
     * Loads a menu bar definition from an XML resource stream.
     * @param in the input stream to load from
     * @return the menu bar
     */
    private JMenuBar loadMenuBar(InputStream in) throws IOException {
        JMenuBar menubar = null;
        try {

            XMLReader parser = SAXParserFactory.newInstance().newSAXParser().getXMLReader();
            Loader docHandler = new Loader();
            parser.setContentHandler(docHandler);
            parser.setErrorHandler(docHandler);
            parser.parse(new InputSource(in));
            menubar = docHandler.rootNode;
//SYSOUT.println("*** MenuBuilder.loadMenu: menubar=" + menubar);
        } catch (SAXParseException ex) {
            MessageWriters.DetailMsg.println("*** MenuBuilder.loadMenu: ParseException"
                                           + ": PublicId=" + ex.getPublicId()
                                           + ", SystemId=" + ex.getSystemId()
                                           + ", line=" + ex.getLineNumber()
                                           + ", column=" + ex.getColumnNumber()
                                           + ": " + ex.getMessage());
            //ex.printStackTrace(MessageWriters.DetailMsg);
        } catch (SAXException ex) {
            MessageWriters.DetailMsg.println("*** MenuBuilder.loadMenu: " + ex);
        } catch (FactoryConfigurationError ex) {
            MessageWriters.DetailMsg.println("*** " + ex);
        } catch (ParserConfigurationException ex) {
            MessageWriters.DetailMsg.println("*** " + ex);
        }

        return menubar;
    }

    // Element names:
    private final static String MenuBarElementName = "menu-bar";
    private final static String MenuBarStrutElementName = "strut";
    private final static String MenuBarGlueElementName = "glue";
    private final static String MenuElementName = "menu";
    private final static String MenuSeparatorElementName = "separator";
    private final static String MenuMarkElementName = "mark";
    private final static String ActionMenuItemElementName = "action";
    private final static String CheckBoxMenuItemElementName = "checkbox";
    private final static String RadioButtonMenuItemElementName = "radiobutton";
    private final static String ButtonMenuItemElementName = "button";

    // Attribute names:
    private final static String IdAttributeName = "id";
    private final static String LabelAttributeName = "label";
    private final static String IconAttributeName = "icon";
    private final static String TooltipAttributeName = "tooltip";
    private final static String MnemonicAttributeName = "mnemonic";
    private final static String AcceleratorAttributeName = "accelerator";
    private final static String ActionAttributeName = "action";
    private final static String CommandAttributeName = "command";
    private final static String PropertyAttributeName = "property";
    private final static String SelectedAttributeName = "selected";
    private final static String DeselectedAttributeName = "deselected";
    private final static String GroupAttributeName = "group";
    private final static String SizeAttributeName = "size";
    private final static String HelpResourceName = "help";

    private static final String ActionProperty = "menuItem.action";
    private static final String ActionPropertyChangeListenerProperty = "menuItem.action.propertyChangeListener";
    private static final String PropertyNameProperty = "menuItem.propertyName";
    private static final String PropertyChangeListenerProperty = "menuItem.propertyChangeListener";
    private static final String ButtonGroupProperty = "menuItem.buttonGroup";

    // Instance members:
    private transient Class targetClass;
    private transient Map targetCommands;
    private transient Map menuComponents;
    private transient Map buttonGroups;
    private transient PropertySet targetProperties;
}
