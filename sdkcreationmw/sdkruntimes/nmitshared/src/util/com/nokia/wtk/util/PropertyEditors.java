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



package com.nokia.wtk.util;

/* java.lang.ref */
import java.lang.ref.Reference;
import java.lang.ref.WeakReference;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyEditor;
import java.beans.PropertyEditorManager;
import java.beans.PropertyEditorSupport;

/* java.util */
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/* java.awt */
import java.awt.Font;
import java.awt.Point;
import java.awt.Color;
import java.awt.Polygon;
import java.awt.Rectangle;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Toolkit;

/* java.awt.event */
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;

/* javax.swing */
import javax.swing.ButtonGroup;
import javax.swing.AbstractButton;
import javax.swing.ComboBoxEditor;
import javax.swing.DefaultComboBoxModel;
import javax.swing.ListSelectionModel;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenuItem;
import javax.swing.JRadioButton;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JPasswordField;
import javax.swing.JSlider;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

/* javax.swing.event */
import javax.swing.event.ChangeEvent;
import javax.swing.event.DocumentEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.DocumentListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

/* javax.swing.text */
import javax.swing.text.Document;
import javax.swing.text.JTextComponent;

/* com.nokia.wtk.util.gui */
import com.nokia.wtk.util.gui.GUI;

/**
 * A collection of PropertyEditors for use by PropertySet.
 * <p>
 * By default, PropertyEditors are provided for the following types:
 * <ul>
 * <li><code>java.lang.String[]</code></li>
 * <li><code>java.awt.Font</code></li>
 * <li><code>java.awt.Point</code></li>
 * <li><code>java.awt.Dimension</code></li>
 * <li><code>java.awt.Rectangle</code></li>
 * <li><code>java.awt.Polygon</code></li>
 * <li><code>java.io.File</code></li>
 * <li><code>javax.swing.KeyStroke</code></li>
 * <li><code>javax.swing.KeyStroke[]</code></li>
 * </ul>
 * <p>
 * All methods are static, so no instantiation is necessary.
 * Addition utility methods exist to aid in related text parsing
 * activities.
 *
 * @see Property
 * @see PropertySet
 * @see PropertyEditor
 *
 */
public class PropertyEditors {

    /**
     * Initializes base set of property editors.
     * @todo other editors for java.awt.Insets,java.util.Date,java.util.Locale
     */
    private synchronized static void initializeEditors() {
        if (editors == null) {
            editors = new Hashtable(17);
            editors.put(String[].class, StringArrayPropertyEditor.class);
            editors.put(int[].class, IntArrayPropertyEditor.class);
            editors.put(byte[].class, ByteArrayPropertyEditor.class);
            editors.put(Point.class, PointPropertyEditor.class);
            editors.put(Dimension.class, DimensionPropertyEditor.class);
            editors.put(Rectangle.class, RectanglePropertyEditor.class);
            editors.put(Color.class, ColorPropertyEditor.class);
            editors.put(Polygon.class, PolygonPropertyEditor.class);
            editors.put(Font.class, FontPropertyEditor.class);
            editors.put(KeyStroke.class, KeyStrokePropertyEditor.class);
            editors.put(KeyStroke[].class, KeyStrokeArrayPropertyEditor.class);
        }
    }

    /**
     * Locates and instantiates a PropertyEditor for a given type.
     *
     * @param type The Class object for the type to be edited
     * @return An editor object for the given target class.
     * The result is <code>null</code> if no suitable editor can be found.
     */
    public static PropertyEditor getEditor(Class type) {
        if (editors == null) initializeEditors();
        Class editorClass = (Class)editors.get(type);
        PropertyEditor editor = null;
        if (editorClass != null) {
            try { editor = (PropertyEditor)editorClass.newInstance(); }
            catch (Exception ex) {
                System.err.println("Couldn't instantiate type editor \"" + editorClass.getName() + "\" : " + ex);
                printStackTrace(ex);
            }
        }

        // Otherwise, look for globally-defined PropertyEditor for the property type:
        if (editor == null) {
            editor = PropertyEditorManager.findEditor(type);

            // For Java primitive "wrapper" types, pretend to be of primitive type:
            if (editor == null) {
                if (type == Boolean.class) {
                    editor = PropertyEditorManager.findEditor(Boolean.TYPE);
                } else if (type == Double.class) {
                    editor = PropertyEditorManager.findEditor(Double.TYPE);
                } else if (type == Float.class) {
                    editor = PropertyEditorManager.findEditor(Float.TYPE);
                } else if (type == Long.class) {
                    editor = PropertyEditorManager.findEditor(Long.TYPE);
                } else if (type == Integer.class) {
                    editor = PropertyEditorManager.findEditor(Integer.TYPE);
                } else if (type == Short.class) {
                    editor = PropertyEditorManager.findEditor(Short.TYPE);
                } else if (type == Byte.class) {
                    editor = PropertyEditorManager.findEditor(Byte.TYPE);
                }
            }
        }

        // update the editor table to avoid calling
        // PropertyEditorManager.findEditor too often, because
        // it takes up considerable amount of CPU time
        if (editor != null) {
            synchronized (PropertyEditors.class) {
                editors.put(type, editor.getClass());
            }
        }

        return editor;
    }

    /**
     * Registers a property editor for a given type.
     * @param type the type to register the editor for
     * @param editor the editor to register
     */
    public synchronized static void registerEditor(Class type, PropertyEditor editor) {
        if (editors == null) initializeEditors();
        editors.put(type, editor.getClass());
    }

    /**
     * Creates a check box component that can edit a boolean property.
     * @param ps the property set to edit
     * @param name the property name
     * @param label a label to use for the check box. Specifically,
     *              the string returns by the <tt>toString()</tt> method of
     *              this object is used as the text for the check box.
     * @return the check box component
     */
    public static JComponent createCheckBox(PropertySet ps, String name, Object label) {
        return createCheckBox(ps.getProperty(name), label);
    }

    /**
     * Creates a check box component that can edit a boolean property.
     * @param p the property to edit
     * @param label a label to use for the check box. Specifically,
     *              the string returns by the <tt>toString()</tt> method of
     *              this object is used as the text for the check box.
     * @return the check box component
     */
    public static JComponent createCheckBox(Property p, Object label) {
        return createCheckBox(p, label, false);
    }

    /**
     * Creates a check box component that can edit a boolean property.
     * @param p the property to edit
     * @param label a label String to use for the check box.
     * @return the check box component
     */
    public static JComponent createCheckBox(Property p, String label) {
        return createCheckBox(p, (Object)label, false);
    }

    /**
     * Creates a check box component that can edit a boolean property.
     *
     * @param p the property to edit
     * @param label a label to use for the check box. Specifically,
     *              the string returns by the <tt>toString()</tt> method of
     *              this object is used as the text for the check box.
     * @param invert invert the state of the check box relative to
     *                the property value. That is, if this parameter is
     *                <code>true</code>, the check box will be checked if
     *                the value of the property is <code>false</code> and
     *                unchecked if the value of the property is
     *                <code>true</code>
     * @return the check box component
     */
    public static JComponent createCheckBox(Property p,
                                            Object label,
                                            boolean invert) {
        if (p == null || p.getType() != Boolean.class) {
            return new DummyComponent(String.valueOf(label));
        }
        JCheckBox checkBox = new CheckBox(label, false);
        GUI.setMnemonicFromText(checkBox);
        initCheckBox(checkBox, p, invert);
        return checkBox;
    }

    private static class CheckBox extends JCheckBox {
        private Object label;
        CheckBox(Object obj, boolean selected) {
            super(String.valueOf(obj), selected);
            label = obj;
        }
        public void updateUI() {
            if (label != null) {
                setText(String.valueOf(label));
            }
            super.updateUI();
        }
    }

    /**
     * Initializes a check box button that can edit a boolean property.
     * This function is used to initialize both check boxes and check
     * box menu items.
     *
     * @param checkBox the check box to initialize
     * @param p the property to edit
     * @param invert invert the state of the check box relative to
     *                the property value. That is, if this parameter is
     *                <code>true</code>, the check box will be checked if
     *                the value of the property is <code>false</code> and
     *                unchecked if the value of the property is
     *                <code>true</code>
     */
    private static void initCheckBox(final AbstractButton checkBox,
                                     final Property p,
                                     final boolean invert) {
        try {
            Object v = p.getValue();
            boolean value = (v instanceof Boolean)
                          ? ((Boolean)v).booleanValue()
                          : false;
            checkBox.setSelected(invert ? (!value) : value);
            checkBox.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    boolean selected = checkBox.isSelected();
                    boolean value = (invert ? (!selected) : selected);
                    try {
                        p.setValue(Utils.getBoolean(value));
                    } catch (UnsupportedOperationException ex) {
                        Toolkit.getDefaultToolkit().beep();
                    } catch (Throwable x) {
                        printStackTrace(x);
                    }
                }
            });
            p.addPropertyChangeListener(new PropertyChangeListener() {
                public void propertyChange(PropertyChangeEvent evt) {
                    Boolean b = (Boolean)evt.getNewValue();
                    checkBox.setSelected(!Utils.getBoolean(invert).equals(b));
                }
            });
        } catch (Throwable x) {
            printStackTrace(x);
            checkBox.setSelected(false);
            checkBox.setEnabled(false);
        }
    }

    /**
     * Creates a check box menu item that can edit a boolean property.
     * @param ps the property set to edit
     * @param name the property name
     * @param label a label to use for the menu item
     * @return the check box menu item
     */
    public static JMenuItem createCheckBoxMenuItem(PropertySet ps,
                                                   String name,
                                                   String label) {
        return createCheckBoxMenuItem(ps.getProperty(name), label);
    }

    /**
     * Creates a check box menu item that can edit a boolean property.
     * @param p the property to edit
     * @param label a label to use for the menu item
     * @return the check box menu item
     */
    public static JMenuItem createCheckBoxMenuItem(Property p, String label) {
        return createCheckBoxMenuItem(p, label, false);
    }

    /**
     * Creates a check box menu item that can edit a boolean property.
     *
     * @param p the property to edit
     * @param label a label to use for the menu item
     * @param invert invert the state of the check box relative to
     *                the property value. That is, if this parameter is
     *                <code>true</code>, the check box will be checked if
     *                the value of the property is <code>false</code> and
     *                unchecked if the value of the property is
     *                <code>true</code>
     * @return the check box menu item
     */
    public static JMenuItem createCheckBoxMenuItem(Property p,
                                                   String label,
                                                   boolean invert) {
        if (p == null || p.getType() != Boolean.class) {
            JMenuItem dummy = new JMenuItem(label);
            dummy.setEnabled(false);
            return dummy;
        }
        JMenuItem checkBox = new JCheckBoxMenuItem(label, false);
        GUI.setMnemonicFromText(checkBox);
        initCheckBox(checkBox, p, invert);
        return checkBox;
    }

    /**
     * Creates a horizontal slider that can edit a property by choosing
     * one of a fixed set of values. The labels for the slider are produced
     * by calling toString method on the possible property values.
     *
     * @param p the property to edit
     * @param v the set of values to associate with the radio button
     *          components. The size of the array determines the number
     *          of ticks on the the slider.
     *
     * @return the set of radio button components
     */
    public static JComponent createHorizontalSlider(Property p, Object[] v) {
        return createHorizontalSlider(p, v, new String[v.length]);
    }

    /**
     * Creates a horizontal slider that can edit a property by choosing
     * one of a fixed set of values.
     *
     * @param p the property to edit
     * @param values the set of values to associate with the radio button
     *               components. The size of the array determines the number
     *               of ticks on the the slider.
     * @param labels the set of labels to use for the slider. If this
     *               parameter is <code>null</code>, slider will have no
     *               labels. If it's not <code>null</code>, the size of
     *               the array must match the size of <code>values</code>.
     *
     * @return the set of radio button components
     */
    public static JComponent createHorizontalSlider(Property p,
                                                    Object[] values,
                                                    String[] labels) {
        return createSlider(p, values, labels, JSlider.HORIZONTAL);
    }

    /**
     * Creates a vertical slider that can edit a property by choosing
     * one of a fixed set of values. The labels for the slider are produced
     * by calling toString method on the possible property values.
     *
     * @param p the property to edit
     * @param v the set of values to associate with the radio button
     *          components. The size of the array determines the number
     *          of ticks on the the slider.
     *
     * @return the set of radio button components
     */
    public static JComponent createVerticalSlider(Property p, Object[] v) {
        return createVerticalSlider(p, v, new String[v.length]);
    }

    /**
     * Creates a vertical slider that can edit a property by choosing
     * one of a fixed set of values.
     *
     * @param p the property to edit
     * @param values the set of values to associate with the radio button
     *               components. The size of the array determines the number
     *               of ticks on the the slider.
     * @param labels the set of labels to use for the slider. If this
     *               parameter is <code>null</code>, slider will have no
     *               labels. If it's not <code>null</code>, the size of
     *               the array must match the size of <code>values</code>.
     *
     * @return the set of radio button components
     */
    public static JComponent createVerticalSlider(Property p,
                                                  Object[] values,
                                                  String[] labels) {
        return createSlider(p, values, labels, JSlider.VERTICAL);
    }

    /**
     * Creates a slider that can edit a property by choosing one of
     * a fixed set of values.
     *
     * @param p the property to edit
     * @param values the set of values to associate with the radio button
     *               components. The size of the array determines the number
     *               of ticks on the the slider.
     * @param labels the set of labels to use for the slider. If this
     *               parameter is <code>null</code>, slider will have no
     *               labels. If it's not <code>null</code>, the size of
     *               the array must match the size of <code>values</code>.
     * @param orient the orientation of the slider
     *
     * @return the set of radio button components
     */
    public static JComponent createSlider(final Property p,
                                          final Object[] values,
                                          String[] labels,
                                          int orient) {
        if (labels != null && labels.length != values.length) {
            throw new IllegalArgumentException("labels/values mismatch");
        }
        if (p == null) {
            return new DummyComponent();
        }
        int val = ArrayUtils.indexOf(values, p.getValue());
        int max = values.length-1;
        if (val < 0) val = 0;
        if (val > max) val = max;
        // keep a reference to the property change listener to prevent
        // it from being garbage collected before the slider
        final JSlider slider = new JSlider(orient, 0, max, val) {
            private PropertyChangeListener pcl;
            {
                pcl = new PropertyChangeListener() {
                    public void propertyChange(PropertyChangeEvent evt) {
                        // Unfortunately, slider does not have an
                        // "undefined" state, so in case if the value
                        // is not one of our known values, we cannot
                        // do anything meaningful. When you cannot do
                        // something that makes sense, the best thing
                        // to do is to not do anything at all. Any
                        // objections?
                        Object value = evt.getNewValue();
                        int index = ArrayUtils.indexOf(values, value);
                        if (index >= 0) {
                            setValue(index);
                        }
                    }
                };
                addWeakPropertyChangeListener(p, pcl);
            }
        };
        slider.putClientProperty("JSlider.isFilled", Boolean.TRUE);
        slider.setPaintTicks(true);
        slider.setSnapToTicks(true);
        slider.setMinorTickSpacing(1);
        if (labels != null) {
            Dictionary labelTable = new Hashtable();
            for (int i=0; i<labels.length; i++) {
                String label = labels[i];
                if (label == null) label = String.valueOf(values[i]);
                labelTable.put(new Integer(i), new JLabel(label));
            }
            slider.setLabelTable(labelTable);
            slider.setPaintLabels(true);
        }

        slider.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
                if (!slider.getValueIsAdjusting()) {
                    p.setValue(values[slider.getValue()]);
                }
            }
        });

        return slider;
    }

    /**
     * Creates a set of radio button components that can edit a property
     * by choosing one of a fixed set of values.
     * @param p the property to edit
     * @param labels the set of labels to use for the radio button components.
     *               The size of the array determines the number of components
     *               created.
     * @param values the set of values to associate with the radio button
     *               components. The size of the array must match the size
     *               of <code>labels</code>.
     * @return the set of radio button components
     */
    public static JComponent[] createRadioButtonGroup(final Property p,
                                                      String[] labels,
                                                      final Object[] values) {
        if (labels.length != values.length) throw new IllegalArgumentException();
        if (p == null) {
            JComponent [] dummies = new JComponent[labels.length];
            for (int i=0; i<labels.length; i++) {
                dummies[i] = new DummyComponent(labels[i]);
            }
            return dummies;
        }
        final JRadioButton[] buttons = new JRadioButton[labels.length];
        final PropertyChangeListener pcl = new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent evt) {
                Object newval = evt.getNewValue();
                // Must iterate through all buttons because
                // new value may not match any of those supplied:
                for (int i = 0; i < buttons.length; i++) {
                    buttons[i].setSelected(Utils.equals(newval, values[i]));
                }
            }
        };

        ButtonGroup buttonGroup = new ButtonGroup();
        Object value = p.getValue();
        for (int i = 0; i < buttons.length; i++) {
            final Object buttonValue = values[i];
            boolean enabled = Utils.equals(value, buttonValue);
            // keep a reference to the property change listener to prevent
            // it from being garbage collected before the button
            final JRadioButton button = new JRadioButton(labels[i], enabled) {
                private PropertyChangeListener ref = pcl;
            };
            GUI.setMnemonicFromText(button);
            buttonGroup.add(buttons[i] = button);
            button.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (button.isSelected()) {
                        try { p.setValue(buttonValue); }
                        catch (Exception x) { printStackTrace(x); }
                    }
                }
            });
        }

        addWeakPropertyChangeListener(p, pcl);

        return buttons;
    }

    /**
     * Creates a set of radio menu items that can edit a property by choosing
     * one of a fixed set of values. The labels for the menu items will be
     * determined by calling toString() method of the value objects
     *
     * @param p the property to edit
     * @param values the set of values to associate with the radio button
     *               components. The size of the array determines the number
     *               of menu items created.
     *
     * @return the set of radio menu items
     */
    public static JMenuItem[] createRadioMenuItems(Property p, Object[] values) {
        String [] labels = new String[values.length];
        for (int i=0; i<values.length; i++) {
            labels[i] = String.valueOf(values[i]);
        }
        return createRadioMenuItems(p, labels, values);
    }

    /**
     * Creates a set of radio menu items that can edit a property by choosing
     * one of a fixed set of values.
     *
     * @param p the property to edit
     * @param labels the set of labels to use for the radio menu items.
     *               The size of the array determines the number of menu items
     *               created.
     * @param values the set of values to associate with the radio menu items.
     *               The size of the array must match the size of
     *               <code>labels</code>.
     * @return the set of radio menu items
     */
    public static JMenuItem[] createRadioMenuItems(final Property p,
                                                   final Object[] labels,
                                                   final Object[] values) {
        if (labels.length != values.length) throw new IllegalArgumentException();
        if (p == null) {
            JMenuItem [] dummies = new JMenuItem[labels.length];
            for (int i=0; i<labels.length; i++) {
                dummies[i] = new JMenuItem(String.valueOf(labels[i]));
                dummies[i].setEnabled(false);
            }
            return dummies;
        }
        final JMenuItem[] buttons = new JRadioButtonMenuItem[labels.length];
        final PropertyChangeListener pcl = new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent evt) {
                Object newval = evt.getNewValue();
                // Must iterate through all buttons because
                // new value may not match any of those supplied:
                for (int i = 0; i < buttons.length; i++) {
                    buttons[i].setSelected(Utils.equals(newval, values[i]));
                }
            }
        };

        ButtonGroup buttonGroup = new ButtonGroup();
        Object value = p.getValue();
        for (int i = 0; i < buttons.length; i++) {
            final Object buttonValue = values[i];
            boolean enabled = Utils.equals(value, buttonValue);
            final JMenuItem button = new RadioMenuItem(labels[i],pcl,enabled);
            GUI.setMnemonicFromText(button);
            buttonGroup.add(buttons[i] = button);
            button.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (button.isSelected()) {
                        try { p.setValue(buttonValue); }
                        catch (Exception x) { printStackTrace(x); }
                    }
                }
            });
        }

        addWeakPropertyChangeListener(p, pcl);
        return buttons;
    }

    private static class RadioMenuItem extends JRadioButtonMenuItem {
        // pcl field keeps a reference to the property change listener to
        // prevent it from being garbage collected before the menu item
        private PropertyChangeListener pcl;
        private Object label;
        RadioMenuItem(Object obj, PropertyChangeListener p, boolean enable) {
            super(String.valueOf(obj), enable);
            label = obj;
            pcl = p;
        }
        public void updateUI() {
            if (label != null) {
                setText(String.valueOf(label));
            }
            super.updateUI();
        }
    }

    /**
     * Creates a combo box component that can edit a property by choosing one
     * of a fixed set of values.
     * @param p the property to edit
     * @param labels the set of labels to use for the combo box.
     *               The size of the array determines the number of items
     *               in the combo box.
     * @param values the set of values to associate with the labels.
     *               The size of the array must match the size of
     *                <code>labels</code>.
     * @return the combo box component
     */
    public static JComponent createComboBox(final Property p,
                                            Object[] labels,
                                            final Object[] values) {
        if (p == null) return new DummyComponent();
        final JComboBox cb = new JComboBox(labels);

        // Select initial value
        cb.setSelectedIndex(ArrayUtils.indexOf(values, p.getValue()));

        // Listen for combo box changes
        cb.addItemListener(new ItemListener() {
            public void itemStateChanged(ItemEvent e) {
                if (e.getStateChange() == ItemEvent.SELECTED) {
                    int index = cb.getSelectedIndex();
                    try { p.setValue(values[index]); }
                    catch (Exception x) { printStackTrace(x); }
                }
            }
        });

        // Listen for property changes
        p.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                Object val = e.getNewValue();
                cb.setSelectedIndex(ArrayUtils.indexOf(values, val));
            }
        });

        return cb;
    }

    /**
     * Creates a list box component that can edit a property by choosing one
     * of a fixed set of values. Deselecting the item in the list box sets
     * the property to <code>null</code>.
     *
     * @param p the property to edit
     * @param values the set of values to associate with the labels.
     *               The size of the array determines the number of items
     *               in the combo box.
     * @return the combo box component
     */
    public static JComponent createListBox(Property p, Object[] values) {
        return createListBox(p, null, values);
    }

    /**
     * Creates a list box component that can edit a property by choosing one
     * of a fixed set of values. Deselecting the item in the list box sets
     * the property to <code>null</code>.
     *
     * @param p the property to edit
     * @param values the set of values to associate with the labels.
     *               The size of the array determines the number of items
     *               in the combo box.
     * @param lines  The preferrerd number of lines in the list box,
     *               <code>-1</code> to use the JList default.
     * @return the combo box component
     */
    public static JComponent createListBox(Property p,
                                           Object[] values,
                                           int lines) {
        return createListBox(p, null, values, lines);
    }

    /**
     * Creates a list box component that can edit a property by choosing one
     * of a fixed set of values. Deselecting the item in the list box sets
     * the property to <code>null</code>.
     *
     * @param p the property to edit
     * @param labels the set of labels to use for the list box.
     *               The size of the array determines the number of items
     *               in the combo box.
     * @param values the set of values to associate with the labels.
     *               The size of the array must match the size of
     *                <code>labels</code>.
     * @return the combo box component
     */
    public static JComponent createListBox(Property p,
                                           Object[] labels,
                                           Object[] values) {
        return createListBox(p, labels, values, 5);
    }

    /**
     * Creates a list box component that can edit a property by choosing one
     * of a fixed set of values. Deselecting the item in the list box sets
     * the property to <code>null</code>.
     *
     * @param p the property to edit
     * @param labels the set of labels to use for the list box.
     *               The size of the array determines the number of items
     *               in the combo box.
     * @param values the set of values to associate with the labels.
     *               The size of the array must match the size of
     *                <code>labels</code>.
     * @param lines  The preferrerd number of lines in the list box,
     *               <code>-1</code> to use the JList default.
     * @return the combo box component
     */
    public static JComponent createListBox(final Property p,
                                           Object[] labels,
                                           final Object[] values,
                                           int lines) {
        if (p == null) return new DummyComponent();
        if (labels == null) labels = values;
        final JList lb = new JList(labels);
        lb.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        if (lines >= 0) lb.setVisibleRowCount(lines);

        // Select initial value
        lb.setSelectedIndex(ArrayUtils.indexOf(values, p.getValue()));

        // Listen for combo box changes
        lb.addListSelectionListener(new ListSelectionListener() {
            public void valueChanged(ListSelectionEvent e) {
                if (!e.getValueIsAdjusting()) {
                    int index = lb.getSelectedIndex();
                    try {
                        Object value = ((index >= 0) ? values[index] : null);
                        p.setValue(value);
                    } catch (Exception x) {
                        printStackTrace(x);
                    }
                }
            }
        });

        // Listen for property changes
        p.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                int index = ArrayUtils.indexOf(values, e.getNewValue());
                if (index < 0) {
                    lb.clearSelection();
                } else {
                    lb.setSelectedIndex(index);
                    lb.ensureIndexIsVisible(index);
                }
            }
        });

        return lb;
    }

    /**
     * Creates a combo box component that can edit a property by choosing one
     * of a fixed set of values.
     * @param p the property to edit
     * @param history the property set containing the history of the
     *                property values
     * @return the editable combo box component
     */
    public static JComponent createEditableComboBox(final Property p, final PropertySet history) {

        if (p == null) return new DummyComponent();

        final boolean[] textModified = new boolean [] {true};
        final DocumentListener docListener = new DocumentListener () {
            private String lastValue = p.getValueAsText();
            public void insertUpdate(DocumentEvent e) { update(e.getDocument()); }
            public void removeUpdate(DocumentEvent e)  { update(e.getDocument()); }
            public void changedUpdate(DocumentEvent e)  { update(e.getDocument()); }
            private void update(Document doc) {
                try {
                    String text = doc.getText(0,doc.getLength());
                    if (!Utils.equals(text,lastValue) && !textModified[0]) {
                        lastValue = text;
                        // Mark this property change to prevent
                        // infinite recursion:
                        textModified[0] = true;
                        p.setValueAsText(text);
                    }
                }
                catch (IllegalArgumentException x) {}
                catch (Exception z) { printStackTrace(z); }
                finally { textModified[0] = false; }
            }
        };

        // create history list
        String [] h = (String[])history.getValue(PropUtils.HISTORY_DATA_PROP);
        if (h == null) h = new String[0];

        // create the combo box
        final DefaultComboBoxModel model = new DefaultComboBoxModel(h);
        JComboBox combo = new JComboBox(model) {
            private PropertyChangeListener propListener = null;
            private PropertyChangeListener histDataListener = null;
            public void updateUI() {
                ComboBoxEditor editor = this.getEditor();
                if (editor != null) {
                    Component c = editor.getEditorComponent();
                    if (c instanceof JTextComponent) {
                        JTextComponent tc = ((JTextComponent)c);
                        tc.getDocument().removeDocumentListener(docListener);
                    }
                }
                super.updateUI();
                editor = this.getEditor();
                if (editor != null) {
                    Component c = editor.getEditorComponent();
                    if (c instanceof JTextComponent) {
                        JTextComponent tc = ((JTextComponent)c);
                        // Force single-line document
                        Document doc = tc.getDocument();
                        if (!(doc instanceof SingleLineDocument)) {
                            doc = new SingleLineDocument(doc);
                            tc.setDocument(doc);
                        }
                        doc.addDocumentListener(docListener);
                    }
                }
            }
            // this method fixes a compilation problem
            private ComboBoxEditor getEditorPlease() {
                return this.getEditor();
            }
            public void addNotify() {

                // create listener for the property we are editing
                if (propListener == null) {
                    propListener = new PropertyChangeListener() {
                        public void propertyChange(PropertyChangeEvent evt) {
                            // Ignore changes from text widget:
                            if (!textModified[0]) {
                                getEditorPlease().setItem(p.getValueAsText());
                            }
                        }
                    };
                }

                // create listener for the history data
                if (histDataListener == null) {
                    histDataListener = new PropertyChangeListener() {
                        public void propertyChange(PropertyChangeEvent evt) {
                            String s = evt.getPropertyName();
                            if (PropUtils.HISTORY_DATA_PROP.equals(s)) {
                                String [] data = (String[])evt.getNewValue();
                                model.removeAllElements();
                                if (data != null) {
                                    for(int i=0; i<data.length; i++) {
                                        model.addElement(data[i]);
                                    }
                                }
                            }
                        }
                    };
                }

                // add listeners
                p.addPropertyChangeListener(propListener);
                history.addPropertyChangeListener(histDataListener);

                // let superclass handle it
                super.addNotify();
            }
            public void removeNotify() {
                if (propListener != null) {
                    p.removePropertyChangeListener(propListener);
                }
                if (histDataListener != null) {
                    history.removePropertyChangeListener(histDataListener);
                }
                super.addNotify();
            }
        };

        // this second updateUI() is necessary because the first one
        // (invoked from JComboBox constructor) does not exactly do what
        // we need. It appears (although that may depend on the compiler)
        // that docListener is null when updateUI() is invoked the first
        // time, i.e. references to the final objects on stack aren't
        // set up yet.
        combo.updateUI();
        combo.setEditable(true);
        combo.getEditor().setItem(p.getValueAsText());
        textModified[0] = false;

        return combo;
    }

    /**
     * Creates a text label component that reflects the current value of the
     * specified property.
     * @param ps the property set
     * @param name the property name
     * @return the text label component
     */
    public static JComponent createTextLabel(PropertySet ps, String name) {
        return createTextLabel(ps, name, SwingConstants.LEFT);
    }

    /**
     * Creates a text label component that reflects the current value of the
     * specified property.
     * @param ps the property set
     * @param name the property name
     * @param align one of the following constants
     *           defined in <code>SwingConstants</code>:
     *           <code>LEFT</code>,
     *           <code>CENTER</code>,
     *           <code>RIGHT</code>,
     *           <code>LEADING</code> or
     *           <code>TRAILING</code>.
     * @return the text label component
     */
    public static JComponent createTextLabel(PropertySet ps, String name, int align) {
        return createTextLabel(ps.getProperty(name), align);
    }

    /**
     * Creates a text label component that reflects the current value of the
     * specified property.
     * @param p the property
     * @return the text label component
     */
    public static JComponent createTextLabel(Property p) {
        return createTextLabel(p, SwingConstants.LEFT);
    }

    /**
     * Creates a text label component that reflects the current value of the
     * specified property.
     * @param p the property
     * @param align one of the following constants
     *           defined in <code>SwingConstants</code>:
     *           <code>LEFT</code>,
     *           <code>CENTER</code>,
     *           <code>RIGHT</code>,
     *           <code>LEADING</code> or
     *           <code>TRAILING</code>.
     * @return the text label component
     */
    public static JComponent createTextLabel(Property p, int align) {
        if (p != null) {
            return new LabelComponent(p, align);
        } else {
            return new DummyComponent();
        }
    }

    /**
     * A label that tracks changes in the property.
     * Used by <code>createTextLabel()</code>
     */
    private static class LabelComponent extends JLabel {
        private Property property;
        private PropertyChangeListener pcl = null;
        LabelComponent(Property p, int align) {
            property = p;
            setHorizontalAlignment(align);
            updateTextFromProperty();
        }
        public void addNotify() {
            super.addNotify();
            if (pcl == null) pcl = new PropertyListener();
            property.addPropertyChangeListener(pcl);
            updateTextFromProperty();
        }
        public void removeNotify() {
            super.removeNotify();
            if (pcl != null) {
                property.removePropertyChangeListener(pcl);
            }
        }
        private void updateTextFromProperty() {
            // If we set empty text, then the preferred height of the
            // label will become zero which will most likely screw up
            // the layout
            String text = property.getValueAsText();
            if (text == null || text.length() == 0) text = " ";
            setText(text);
        }
        private class PropertyListener implements PropertyChangeListener {
            public void propertyChange(PropertyChangeEvent e) {
                updateTextFromProperty();
            }
        }
    }

    /**
     * Creates a text field component that can edit a property as a single-line
     * string.
     * @param ps the property set to edit
     * @param name the property name
     * @param cols the number of columns to use to calculate the preferred width
     * @return the text field component
     */
    public static JComponent createTextField(PropertySet ps, String name, int cols) {
        return createTextField(ps.getProperty(name), cols);
    }

    /**
     * Creates a text field component that can edit a property as a single-line
     * string. A PropertyEditor must exist for the non-string property types.
     * @param p the property to edit
     * @param cols the number of columns to use to calculate the preferred width
     * @return the text field component
     */
    public static JComponent createTextField(Property p, int cols) {
        if (p != null) {
            return setupTextField(p, new JTextField(cols));
        } else {
            return new DummyComponent();
        }
    }

    /**
     * Creates a readonly text field component that shows the property value
     * as a single-line string. A PropertyEditor must exist for the non-string
     * property types.
     * @param ps the property set to edit
     * @param name the property name
     * @param cols the number of columns to use to calculate the preferred width
     * @return the text field component
     */
    public static JComponent createReadOnlyTextField(PropertySet ps, String name, int cols) {
        return createReadOnlyTextField(ps.getProperty(name), cols);
    }

    /**
     * Creates a readonly text field component that shows the property value
     * as a single-line string. A PropertyEditor must exist for the non-string
     * property types.
     * @param p the property to edit
     * @param cols the number of columns to use to calculate the preferred width
     * @return the text field component
     */
    public static JComponent createReadOnlyTextField(Property p, int cols) {
        if (p != null) {
            JTextField field = setupTextField(p, new JTextField(cols));
            field.setEditable(false);
            return field;
        } else {
            return new DummyComponent();
        }
    }

    /**
     * Creates a password field component that can edit a property
     * @param ps the property set to edit
     * @param name the property name
     * @param cols the number of columns to use to calculate the preferred width
     * @return the text field component
     */
    public static JComponent createPasswordField(PropertySet ps, String name, int cols) {
        return createPasswordField(ps.getProperty(name), cols);
    }

    /**
     * Creates a text field component that can edit a property as a single-line
     * string. A PropertyEditor must exist for the non-string property types.
     * @param p the property to edit
     * @param cols the number of columns to use to calculate the preferred width
     * @return the text field component
     */
    public static JComponent createPasswordField(Property p, int cols) {
        if (p != null) {
            return setupTextField(p, new JPasswordField(cols));
        } else {
            return new DummyComponent();
        }
    }

    /**
     * Creates a text area component that can edit a property as a multi-line
     * string.
     * @param p the property to edit
     * @param cols the number of columns to calculate the preferred width
     * @param rows the number of rows to calculate the preferred height
     * @return the text field component
     */
    public static JComponent createTextArea(Property p, int cols, int rows) {
        return createTextArea(p, cols, rows, false);
    }

    /**
     * Creates a text area component that can edit a property as a multi-line
     * string.
     * @param p the property to edit
     * @param cols the number of columns to calculate the preferred width
     * @param rows the number of rows to calculate the preferred height
     * @param wrap <code>true</code> to wrap the lines, <code>false</code>
     *             otherwise
     * @return the text field component
     */
    public static JComponent createTextArea(Property p, int cols, int rows,
                                            boolean wrap) {
        if (p != null) {
            JTextArea textEditor = new JTextArea(rows, cols);
            textEditor.setTabSize(2);
            textEditor.setLineWrap(wrap);
            textEditor.setWrapStyleWord(wrap);
            setupTextEditor(p, textEditor);
            return textEditor;
        } else {
            return new DummyComponent();
        }
    }

    /**
     * Sets up a single-line text component that can edit a property
     * @param p the property to edit
     * @param t the text field component
     * @return the text field passed in as a parameter
     */
    private static JTextField setupTextField(Property p, JTextField t) {
        t.setDocument(new SingleLineDocument(t.getDocument()));
        setupTextEditor(p, t);
        return t;
    }

    /**
     * Sets up a text component that can edit a property. A PropertyEditor
     * must exist for the non-string property types.
     *
     * @param p the property to edit
     * @param textEditor the text component
     */
    private static void setupTextEditor(final Property p,
                                        final JTextComponent textEditor) {

        final boolean[] textModified = new boolean [] {false};
        final boolean[] propertyUpdateDisabled = new boolean [] {false};
        if (p != null && p.getValue() != null) {
            textEditor.setText(p.getValueAsText());
        } else {
            textEditor.setText("");
        }
        textEditor.getDocument().addDocumentListener( new RunnableDocumentListener() {
            private boolean submitted = false;
            private String lastValue = textEditor.getText();
            private PropertyEditor editor = getEditor(p.getType());
            private TextComponentValidator validator = null;
            public void insertUpdate(DocumentEvent e) { doChange(); }
            public void removeUpdate(DocumentEvent e)  { doChange(); }
            public void changedUpdate(DocumentEvent e)  { doChange(); }
            private synchronized void doChange() {
                if (!propertyUpdateDisabled[0]) {
                    if (!submitted && !textModified[0]) {
                        submitted = true;
                        SwingUtilities.invokeLater(this);
                    }
                }
            }
            public void run() {
                submitted = false;
                String text = textEditor.getText();
                if (!Utils.equals(lastValue,text)) {
                    lastValue = text;
                    try {
                        // Mark this property change to prevent infinite
                        // recursion:
                        textModified[0] = true;
                        try {  p.setValueAsText(text); }
                        finally { textModified[0] = false; }

                        // success
                        if (validator != null) validator.uninstall();
                        return;
                    }
                    catch (IllegalArgumentException ex) {}
                    catch (Exception ex) { printStackTrace(ex); }

                    // we failed to set the property
                    if (editor != null) {
                        if (validator == null) {
                            validator = new TextComponentValidator( textEditor,
                            new PropertyEditors.TextValidator() {
                                private boolean beep = true;
                                public boolean validate(String text) {
                                    try {
                                        editor.setAsText(text);
                                        beep = true;
                                        return true;
                                    } catch (Exception x) {
                                        if (beep) {
                                            textEditor.getToolkit().beep();
                                            beep = false;
                                        }
                                        return false;
                                    }
                                }
                            });
                        }
                        validator.install();
                    }
                }
            }
        });

        p.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent evt) {
                // Ignore changes from text widget:
                if (!textModified[0]) {
                    if (p != null && p.getValue() != null) {
                        textEditor.setText(p.getValueAsText());
                    } else {
                        boolean savedFlag = propertyUpdateDisabled[0];
                        propertyUpdateDisabled[0] = true;
                        try {
                            textEditor.setText("");
                        } finally {
                            propertyUpdateDisabled[0] = savedFlag;
                        }
                    }
                }
            }
        });
    }

    /**
     * Creates two single-line text field components that can edit a property
     * of type {@link Dimension}. The first edit component edits the width
     * the second one edits the height.
     * @param p the property. Must be of type {@link Dimension}
     */
    public static JComponent [] createDimensionTextFields(Property p) {
        return createDimensionTextFields(p, 10);
    }

    /**
     * Creates two single-line text field components that can edit a property
     * of type {@link Dimension}. The first edit component edits the width
     * the second one edits the height.
     * @param p the property. Must be of type {@link Dimension}
     */
    public static JComponent [] createDimensionTextFields(final Property p, int cols) {

        // Handle the case when property is null (i.e. missing)
        if (p == null || !Dimension.class.isAssignableFrom(p.getType())) {
            JLabel w = new JLabel("width");
            JLabel h = new JLabel("height");
            w.setEnabled(false);
            h.setEnabled(false);
            return new JComponent[] { w, h };
        }

        Dimension dim = (Dimension)p.getValue();

        // Create standalone width property:
        final ListeningProperty width = new ListeningProperty("width",
            Integer.class, new Integer(dim.width), false) {
            public void propertyChange(PropertyChangeEvent e) {
                // Try no to create unnecessary Integer objects
                Dimension d = (Dimension)e.getNewValue();
                Integer value = (Integer)getValue();
                if (value.intValue() != d.width) {
                    setValue(new Integer(d.width));
                }
            }
        };

        // Create standalone height property:
        final ListeningProperty height = new ListeningProperty("height",
            Integer.class,new Integer(dim.height),false) {
            public void propertyChange(PropertyChangeEvent e) {
                // Try no to create unnecessary Integer objects
                Dimension d = (Dimension)e.getNewValue();
                Integer value = (Integer)getValue();
                if (value.intValue() != d.height) {
                    setValue(new Integer(d.height));
                }
            }
        };

        // Create the edit fields
        JComponent [] editFields = new JComponent[2];
        editFields[0] =  createTextField(width, cols);
        editFields[1] =  createTextField(height, cols);

        // Apply the changes in these two properties to the actual
        // Dimension property:
        width.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                int newWidth = ((Integer)e.getNewValue()).intValue();
                Dimension d = (Dimension)p.getValue();
                Dimension newValue = new Dimension(newWidth, d.height);
                p.setValue(newValue);
            }
        });

        height.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                int newHeight = ((Integer)e.getNewValue()).intValue();
                Dimension d = (Dimension)p.getValue();
                Dimension newValue = new Dimension(d.width, newHeight);
                p.setValue(newValue);
            }
        });

        // Propagate changes in the Dimension property
        addWeakPropertyChangeListener(p, width);
        addWeakPropertyChangeListener(p, height);
        return editFields;
    }

    /**
     * Creates a dummy component which is being returned from various
     * createXxx methods in case if property is null
     */
    private static class DummyComponent extends JLabel {
        DummyComponent() { this(null); }
        DummyComponent(String text) {
            super((text == null) ? "????" : GUI.unescape(text));
        }
    }

    /**
     * Parse an integer array from a string.
     * @param input the string to tokenize.
     * @param delim the set of delimiters to use.
     *              If null or empty, then default to comma and whitespace.
     * @param radix the radix to be used for parsing.
     * @return the integer array, which will be empty if a parsing error occured
     */
    private static int[] parseIntArray(String input, String delim, int radix) {
        if ((delim == null) || (delim.length() == 0)) {
            delim = ", \t\n\r\f";
        }
        String[] parms = StringUtils.tokenize(input, delim);
        int[] iArray = new int[parms.length];
        boolean okay = true;
        for (int i = 0; i < parms.length; i++) {
            try { iArray[i] = Integer.parseInt(parms[i], radix); }
            catch (NumberFormatException ex) { okay = false; break; }
        }
        return (okay) ? iArray : new int[0];
    }

    /**
     * Apply escape sequences to a string.
     * @param s the string to escape.
     * @param esc the set of characters that must be escaped.
     * @return the escaped string.
     */
    private static String escapeString(String s, String esc) {
        if (s != null) {
            int len = s.length();
            StringBuffer sbuf = null;
            if (esc == null) esc = "";
            for (int i = 0; i < len; i++) {
                char c = s.charAt(i);
                if ((c == '\\') || (c == '\0') || (esc.indexOf(c) >= 0)) {
                    if (sbuf == null) {
                        sbuf = new StringBuffer(len+1);
                        sbuf.append(s.substring(0,i));
                    }
                    sbuf.append('\\');
                    sbuf.append(c);
                } else if (sbuf != null) {
                    sbuf.append(c);
                }
            }
            return ((sbuf == null) ? s : sbuf.toString());
        } else {
            return "";
        }
    }

    /**
     * Remove escape sequences from a string.
     * @param s the string to unescape.
     * @return the unescaped string.
     */
    private static String unescapeString(String s) {
        if (s != null) {
            int len = s.length();
            StringBuffer sbuf = null;
            for (int i = 0; i < len; i++) {
                char c = s.charAt(i);
                if ((c == '\\') && ((i+1) < len)) {
                    if (sbuf == null) {
                        sbuf = new StringBuffer(len-1);
                        sbuf.append(s.substring(0,i));
                    }
                    char nextc = s.charAt(++i);
                    sbuf.append(nextc);
                } else if (sbuf != null) {
                    sbuf.append(c);
                }
            }
            return ((sbuf == null) ? s : sbuf.toString());
        } else {
            return "";
        }
    }

    /**
     * Encodes a <code>KeyStroke</code> property value as text.
     * @param keyStroke the <code>KeyStroke</code> value.
     * @return the text-encoded <code>KeyStroke</code> value.
     */
    public static String encodeKeyStroke(KeyStroke keyStroke) {

        StringBuffer sbuf = new StringBuffer();
        int modifiers = keyStroke.getModifiers();
        if ((modifiers & ActionEvent.CTRL_MASK) != 0) {
            sbuf.append("CTRL+");
        }
        if ((modifiers & ActionEvent.META_MASK) != 0) {
            sbuf.append("META+");
        }
        if ((modifiers & ActionEvent.ALT_MASK) != 0) {
            sbuf.append("ALT+");
        }
        if ((modifiers & ActionEvent.SHIFT_MASK) != 0) {
            sbuf.append("SHIFT+");
        }

        switch (keyStroke.getKeyCode()) {
        case KeyEvent.VK_CANCEL:            sbuf.append("BREAK");       break;
        case KeyEvent.VK_PAUSE:             sbuf.append("PAUSE");       break;
        case KeyEvent.VK_ENTER:             sbuf.append("ENTER");       break;
        case KeyEvent.VK_SPACE:             sbuf.append("SPACE");       break;
        case KeyEvent.VK_TAB:               sbuf.append("TAB");         break;
        case KeyEvent.VK_BACK_SPACE:        sbuf.append("BACKSPACE");   break;
        case KeyEvent.VK_DELETE:            sbuf.append("DEL");         break;
        case KeyEvent.VK_INSERT:            sbuf.append("INS");         break;
        case KeyEvent.VK_ESCAPE:            sbuf.append("ESC");         break;
        case KeyEvent.VK_HOME:              sbuf.append("HOME");        break;
        case KeyEvent.VK_END:               sbuf.append("END");         break;
        case KeyEvent.VK_PAGE_UP:           sbuf.append("PGUP");        break;
        case KeyEvent.VK_PAGE_DOWN:         sbuf.append("PGDN");        break;
        case KeyEvent.VK_UP:                sbuf.append("UP");          break;
        case KeyEvent.VK_DOWN:              sbuf.append("DOWN");        break;
        case KeyEvent.VK_LEFT:              sbuf.append("LEFT");        break;
        case KeyEvent.VK_RIGHT:             sbuf.append("RIGHT");       break;
        case KeyEvent.VK_COMMA:             sbuf.append("COMMA");       break;
        case KeyEvent.VK_PERIOD:            sbuf.append("DOT");         break;
        case KeyEvent.VK_SLASH:             sbuf.append("SLASH");       break;
        case KeyEvent.VK_BACK_SLASH:        sbuf.append("BACKSLASH");   break;
        case KeyEvent.VK_QUOTE:             sbuf.append("QUOTE");       break;
        case KeyEvent.VK_BACK_QUOTE:        sbuf.append("BACKQUOTE");   break;
        case KeyEvent.VK_SEMICOLON:         sbuf.append("SEMICOLON");   break;
        case KeyEvent.VK_MINUS:             sbuf.append("MINUS");       break;
        case KeyEvent.VK_EQUALS:            sbuf.append("EQUALS");      break;
        case KeyEvent.VK_OPEN_BRACKET:      sbuf.append("OPEN");        break;
        case KeyEvent.VK_CLOSE_BRACKET:     sbuf.append("CLOSE");       break;

        case KeyEvent.VK_NUMPAD0:           sbuf.append("NUMPAD+0");    break;
        case KeyEvent.VK_NUMPAD1:           sbuf.append("NUMPAD+1");    break;
        case KeyEvent.VK_NUMPAD2:           sbuf.append("NUMPAD+2");    break;
        case KeyEvent.VK_NUMPAD3:           sbuf.append("NUMPAD+3");    break;
        case KeyEvent.VK_NUMPAD4:           sbuf.append("NUMPAD+4");    break;
        case KeyEvent.VK_NUMPAD5:           sbuf.append("NUMPAD+5");    break;
        case KeyEvent.VK_NUMPAD6:           sbuf.append("NUMPAD+6");    break;
        case KeyEvent.VK_NUMPAD7:           sbuf.append("NUMPAD+7");    break;
        case KeyEvent.VK_NUMPAD8:           sbuf.append("NUMPAD+8");    break;
        case KeyEvent.VK_NUMPAD9:           sbuf.append("NUMPAD+9");    break;

        case KeyEvent.VK_DECIMAL:           sbuf.append("NUMPAD+DOT");  break;
        case KeyEvent.VK_ADD:               sbuf.append("NUMPAD+PLUS"); break;
        case KeyEvent.VK_SUBTRACT:          sbuf.append("NUMPAD+MINUS");break;
        case KeyEvent.VK_MULTIPLY:          sbuf.append("NUMPAD+STAR"); break;
        case KeyEvent.VK_DIVIDE:            sbuf.append("NUMPAD+SLASH");break;
        case KeyEvent.VK_CLEAR:             sbuf.append("NUMPAD+CLEAR");break;

        case KeyEvent.VK_F1:                sbuf.append("F1");          break;
        case KeyEvent.VK_F2:                sbuf.append("F2");          break;
        case KeyEvent.VK_F3:                sbuf.append("F3");          break;
        case KeyEvent.VK_F4:                sbuf.append("F4");          break;
        case KeyEvent.VK_F5:                sbuf.append("F5");          break;
        case KeyEvent.VK_F6:                sbuf.append("F6");          break;
        case KeyEvent.VK_F7:                sbuf.append("F7");          break;
        case KeyEvent.VK_F8:                sbuf.append("F8");          break;
        case KeyEvent.VK_F9:                sbuf.append("F9");          break;
        case KeyEvent.VK_F10:               sbuf.append("F10");         break;
        case KeyEvent.VK_F11:               sbuf.append("F11");         break;
        case KeyEvent.VK_F12:               sbuf.append("F12");         break;

        default:
            {   // Other regular keyboard characters:
                char keyChar = keyStroke.getKeyChar();
                if ((keyChar >= '0') && (keyChar <= '9')) {
                    // Decimal digits:
                    sbuf.append(keyChar);
                } else if ((keyChar >= 'A') && (keyChar <= 'Z')) {
                    // Upper-case ASCII letters:
                    sbuf.append(keyChar);
                } else if ((keyChar >= 'a') && (keyChar <= 'z')) {
                    // Lower-case ASCII letters:
                    sbuf.append((char)(keyChar - 'a' + 'A'));
                }
            }
            break;
        }

        return sbuf.toString();
    }

    /**
     * Decodes a text-encoded <code>KeyStroke</code> property value.
     * @param text the text-encoded <code>KeyStroke</code> property value.
     * @return the decoded <code>KeyStroke</code> value or <code>null</code>.
     */
    public static KeyStroke decodeKeyStroke(String text) throws IllegalArgumentException {
        if (text != null) {
            KeyStroke keyStroke = null;
            String[] keyparts = StringUtils.tokenize(text, "+");
            int modifiers = 0;
            boolean numpad = false;
            for (int i = 0; i < keyparts.length; i++) {
                if (keyparts[i].equalsIgnoreCase("SHIFT")) {
                    modifiers |= ActionEvent.SHIFT_MASK;
                } else if (keyparts[i].equalsIgnoreCase("CTRL")) {
                    modifiers |= ActionEvent.CTRL_MASK;
                } else if (keyparts[i].equalsIgnoreCase("META")) {
                    modifiers |= ActionEvent.META_MASK;
                } else if (keyparts[i].equalsIgnoreCase("ALT")) {
                    modifiers |= ActionEvent.ALT_MASK;
                } else if (keyparts[i].equalsIgnoreCase("NUMPAD")) {
                    numpad = true;
                } else if (keyparts[i].equalsIgnoreCase("ENTER")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("BREAK") || keyparts[i].equalsIgnoreCase("BRK")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_CANCEL, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("PAUSE")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_PAUSE, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("BACKSPACE") || keyparts[i].equalsIgnoreCase("BKSP")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("TAB")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_TAB, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("DELETE") || keyparts[i].equalsIgnoreCase("DEL")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("INSERT") || keyparts[i].equalsIgnoreCase("INS")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_INSERT, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("ESCAPE") || keyparts[i].equalsIgnoreCase("ESC")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("HOME")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_HOME, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("END")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_END, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("PAGEUP") || keyparts[i].equalsIgnoreCase("PGUP")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_PAGE_UP, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("PAGEDOWN") || keyparts[i].equalsIgnoreCase("PGDN")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_PAGE_DOWN, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("UP")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_UP, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("DOWN")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("LEFT")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("RIGHT")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("QUOTE")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_QUOTE, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("BACKQUOTE")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_BACK_QUOTE, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("COMMA")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_COMMA, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("SEMICOLON")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_SEMICOLON, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("EQUALS")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_EQUALS, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("PLUS")) {
                    if (numpad) {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_ADD, modifiers);
                    }
                    break;
                } else if (keyparts[i].equalsIgnoreCase("MINUS")) {
                    if (numpad) {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_SUBTRACT, modifiers);
                    } else {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_MINUS, modifiers);
                    }
                    break;
                } else if (keyparts[i].equalsIgnoreCase("STAR")) {
                    if (numpad) {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_MULTIPLY, modifiers);
                    }
                    break;
                } else if (keyparts[i].equalsIgnoreCase("SLASH")) {
                    if (numpad) {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_DIVIDE, modifiers);
                    } else {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_SLASH, modifiers);
                    }
                    break;
                } else if (keyparts[i].equalsIgnoreCase("BACKSLASH")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SLASH, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("OPEN") || keyparts[i].equals("[")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_OPEN_BRACKET, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("CLOSE") || keyparts[i].equals("]")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_CLOSE_BRACKET, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("DOT") || keyparts[i].equals(".")) {
                    if (numpad) {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_DECIMAL, modifiers);
                    } else {
                        keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_PERIOD, modifiers);
                    }
                    break;
                } else if (keyparts[i].equalsIgnoreCase("CLEAR") && numpad) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_CLEAR, modifiers);
                    break;

                } else if (keyparts[i].equalsIgnoreCase("SPACE") || keyparts[i].equalsIgnoreCase("SP")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_SPACE, modifiers);
                    break;

                } else if (keyparts[i].equalsIgnoreCase("COMMA")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_COMMA, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("PERIOD")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_PERIOD, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("SLASH")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_SLASH, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("BACKSLASH")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SLASH, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("SEMICOLON") || keyparts[i].equalsIgnoreCase("SEMI")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_SEMICOLON, modifiers);
                    break;
                } else if (keyparts[i].equalsIgnoreCase("EQUALS") || keyparts[i].equalsIgnoreCase("EQ")) {
                    keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_EQUALS, modifiers);
                    break;
                } else if ((keyparts[i].compareTo("0") >= 0) && (keyparts[i].compareTo("9") <= 0)) {
                    // Handle NumPad and Regular Digit Keys:
                    try {
                        int digit = Integer.valueOf(keyparts[i]).intValue();
                        if ((digit >= 0) && (digit <= 9)) {
                            if (numpad) {   // NumPad digit key
                                keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_NUMPAD0+digit, modifiers);
                            } else {        // Regular digit key
                                keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_0+digit, modifiers);
                            }
                        }
                    }
                    catch (NumberFormatException e) {}
                    break;
                } else if ((keyparts[i].toUpperCase().startsWith("F"))
                        && (keyparts[i].length() >= 2)
                        && (keyparts[i].length() <= 3)) {
                    // Parse Function Key:
                    try {
                        int number = Integer.valueOf(keyparts[i].substring(1)).intValue();
                        if ((number >= 1) && (number <= 12)) {
                            keyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_F1+number-1, modifiers);
                        }
                    }
                    catch (NumberFormatException e) {}
                    break;
                } else if (keyparts[i].length() == 1) {
                    if (modifiers != 0) {
                        keyStroke = KeyStroke.getKeyStroke((int)keyparts[i].charAt(0), modifiers);
                    } else {
                        keyStroke = KeyStroke.getKeyStroke(keyparts[i].charAt(0));
                    }
                    break;
                }
            }
            return keyStroke;
        }
        return null;
    }

    /**
     * Encodes a string array property value as text.
     * @param stringArray the string array value value.
     * @return the text-encoded string array value.
     */
    public static String encodeStringArray(String [] stringArray) {
        String text = null;
        if (stringArray != null) {
            StringBuffer sbuf = new StringBuffer();
            for (int i = 0; i < stringArray.length; i++) {
                if (i > 0) {
                    sbuf.append(',');
                }
                sbuf.append(escapeString(stringArray[i], ","));
            }
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded string array property value.
     * @param text the text-encoded string array property value.
     * @return the decoded string array value.
     */
    public static String [] decodeStringArray(String text) {
        String[] array = null;
        if (text != null) {
            List list = new ArrayList();
            int len = text.length();
            int from = 0;
            for (int to = 0; to < len; to++) {
                char c = text.charAt(to);
                switch (c) {
                case '\\':
                    if (to < (len-1)) to++;
                    break;
                case ',':
                    list.add(text.substring(from, to));
                    from = to + 1;
                    break;
                default:
                    break;
                }
            }

            if (from <= len) {
                list.add(text.substring(from));
            }

            array = (String[])list.toArray(new String[list.size()]);
            for (int i = 0; i < array.length; i++) {
                array[i] = unescapeString(array[i]);
            }
        }
        return array;
    }

    /**
     * Encodes an int array property value as text.
     * @param value the int array value value.
     * @return the text-encoded int array value.
     */
    public static String encodeIntArray(int [] value) {
        String text = null;
        if (value != null) {
            StringBuffer sbuf = new StringBuffer();
            for (int i = 0; i < value.length; i++) {
                if (i > 0) {
                    sbuf.append(',');
                }
                sbuf.append(value[i]);
            }
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded int array property value.
     * @param text the text-encoded int array property value.
     * @return the decoded int array value.
     */
    public static int [] decodeIntArray(String text) {
        int [] value = null;
        if (text != null && text.length() > 0) {
            String[] tokens = StringUtils.tokenize(text, ",");
            value = new int[tokens.length];
            for (int i = 0; i < tokens.length; i++) {
                try { value[i] = Integer.parseInt(tokens[i]); }
                catch (NumberFormatException ex) {
                    return null;
                }
            }
        }
        return value;
    }

    /**
     * Encodes a <code>KeyStroke</code> array property value as text.
     * @param keyStrokes the <code>KeyStroke</code> array value.
     * @return the text-encoded <code>KeyStroke</code> array value.
     */
    public static String encodeKeyStrokeArray(KeyStroke [] keyStrokes) {
        String text = null;
        if (keyStrokes != null) {
            StringBuffer sbuf = new StringBuffer();
            for (int i = 0; i < keyStrokes.length; i++) {
                if (i > 0) {
                    sbuf.append(',');
                }
                sbuf.append(escapeString(encodeKeyStroke(keyStrokes[i]), ","));
            }
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded <code>KeyStroke</code> array property value.
     * @param text the text-encoded <code>KeyStroke</code> array
     * @return the decoded <code>KeyStroke</code> value or <code>null</code>.
     */
    public static KeyStroke [] decodeKeyStrokeArray(String text) {
        KeyStroke[] keyStrokes = null;
        if (text != null) {
            List list = new ArrayList();
            int len = text.length();
            int from = 0;
            for (int to = 0; to < len; to++) {
                char c = text.charAt(to);
                switch (c) {
                case '\\':
                    if (to < (len-1)) to++;
                    break;
                case ',':
                    list.add(text.substring(from, to));
                    from = to + 1;
                    break;
                default:
                    break;
                }
            }

            if (from <= len) {
                list.add(text.substring(from));
            }

            int n = list.size();
            keyStrokes = new KeyStroke[n];
            if (n > 0) {
                int k = 0;
                for (int i=0; i<n; i++) {
                    String s = unescapeString((String)list.get(i)).trim();
                    KeyStroke ks = decodeKeyStroke(s);
                    if (ks != null) {
                        keyStrokes[k++] = ks;
                    } else {
                        Trace.Warning.println("cannot convert string \"" +
                            s + "\" into a keystroke");
                    }
                }

                // reallocate the array if necessary
                if (k < n) {
                    if (k == 0) {
                        keyStrokes = null;
                    } else {
                        KeyStroke[] newArray = new KeyStroke[k];
                        System.arraycopy(keyStrokes,0,newArray,0,k);
                        keyStrokes = newArray;
                    }
                }
            }
        }
        return keyStrokes;
    }

    /**
     * Encodes a <code>Point</code> property value as text.
     * @param value the <code>Point</code> value.
     * @return the text-encoded <code>Point</code> value.
     */
    public static String encodePoint(Point value) {
        String text = null;
        if (value != null) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(value.x);
            sbuf.append(',');
            sbuf.append(value.y);
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded <code>Point</code> property value.
     * @param text the text-encoded <code>Point</code> property value.
     * @return the decoded <code>Point</code> value or <code>null</code>.
     */
    public static Point decodePoint(String text) {
        Point value = null;
        if (text != null) {
            int[] v = parseIntArray(text, null, 10);
            if (v != null && v.length == 2) {
                value = new Point(v[0], v[1]);
            }
        }
        return value;
    }

    /**
     * Encodes a <code>Dimension</code> property value as text.
     * @param value the <code>Dimension</code> value.
     * @return the text-encoded <code>Dimension</code> value.
     */
    public static String encodeDimension(Dimension value) {
        String text = null;
        if (value != null) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(value.width);
            sbuf.append(',');
            sbuf.append(value.height);
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded <code>Dimension</code> property value.
     * @param text the text-encoded <code>Dimension</code> property value.
     * @return the decoded <code>Dimension</code> value or <code>null</code>.
     */
    public static Dimension decodeDimension(String text) {
        Dimension value = null;
        if (text != null) {
            int[] v = parseIntArray(text, null, 10);
            if (v != null && v.length == 2) {
                value = new Dimension(v[0], v[1]);
            }
        }
        return value;
    }

    /**
     * Encodes a <code>Rectangle</code> property value as text.
     * @param value the <code>Rectangle</code> value.
     * @return the text-encoded <code>Rectangle</code> value.
     */
    public static String encodeRectangle(Rectangle value) {
        String text = null;
        if (value != null) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(value.x);
            sbuf.append(',');
            sbuf.append(value.y);
            sbuf.append(',');
            sbuf.append(value.width);
            sbuf.append(',');
            sbuf.append(value.height);
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded <code>Rectangle</code> property value.
     * @param text the text-encoded <code>Rectangle</code> property value.
     * @return the decoded <code>Rectangle</code> value or <code>null</code>.
     */
    public static Rectangle decodeRectangle(String text) {
        Rectangle value = null;
        if (text != null) {
            int[] v = parseIntArray(text, null, 10);
            if (v != null) {
                switch (v.length) {
                case 2:
                    value = new Rectangle(v[0], v[1]);
                    break;
                case 4:
                    value = new Rectangle(v[0], v[1], v[2], v[3]);
                    break;
                }
            }
        }
        return value;
    }

    /**
     * Encodes a <code>Color</code> property value as text.
     * @param value the <code>Color</code> value.
     * @return the text-encoded <code>Color</code> value.
     */
    public static String encodeColor(Color value) {
        String text = null;
        if (value != null) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(value.getRed());
            sbuf.append(',');
            sbuf.append(value.getGreen());
            sbuf.append(',');
            sbuf.append(value.getBlue());
            sbuf.append(',');
            sbuf.append(value.getAlpha());
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded <code>Color</code> property value.
     * @param text the text-encoded <code>Color</code> property value.
     * @return the decoded <code>Color</code> value or <code>null</code>.
     */
    public static Color decodeColor(String text) {
        Color value = null;
        if (text != null) {
            int[] v = parseIntArray(text, null, 10);
            if (v != null) {
                switch (v.length) {
                case 3:
                    value = new Color(v[0], v[1], v[2]);
                    break;
                case 4:
                    value = new Color(v[0], v[1], v[2], v[3]);
                    break;
                }
            }
        }
        return value;
    }

    /**
     * Encodes a <code>Polygon</code> property value as text.
     * @param value the <code>Polygon</code> value.
     * @return the text-encoded <code>Polygon</code> value.
     */
    public static String encodePolygon(Polygon value) {
        String text = null;
        if (value != null) {
            // Polygon is represented by a semicolon-separated set of points.
            // Each point is represented by a comma-separated x,y coordinate pair.
            StringBuffer sbuf = new StringBuffer();
            for (int i = 0; i < value.npoints; i++) {
                if (i > 0) {
                    sbuf.append(';');
                }
                sbuf.append(value.xpoints[i]);
                sbuf.append(',');
                sbuf.append(value.ypoints[i]);
            }
            text = sbuf.toString();
        }
        return text;
    }

    /**
     * Decodes a text-encoded <code>Polygon</code> property value.
     * @param text the text-Polygon <code>Polygon</code> property value.
     * @return the decoded <code>Color</code> value or <code>null</code>.
     */
    public static Polygon decodePolygon(String text) {
        Polygon value = null;
        if (text != null) {
            // Polygon is represented by a semicolon-separated set of points.
            String[] pointTokens = StringUtils.tokenize(text, ";");
            if (pointTokens.length > 0) {
                value = new Polygon();
                for (int i = 0; i < pointTokens.length; i++) {
                    // Each point is represented by a comma-separated x,y
                    // coordinate pair.
                    int [] v = parseIntArray(pointTokens[i], null, 10);
                    if (v != null && v.length == 2) {
                        value.addPoint(v[0], v[1]);
                    } else {
                        return null;
                    }
                }
            }
        }
        return value;
    }

    /**
     * Prints stack trace if configured to do so.
     * @param x contains the stack trace to print
     */
    private static void printStackTrace(Throwable x) {
        if (Trace.isVerbose()) {
            Trace.Debug.printStackTrace(x);
        }
    }

    /**
     * Inner Class: A PropertyEditor for String array types.
     */
    private static class StringArrayPropertyEditor extends PropertyEditorSupport {
        public StringArrayPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof String[]) {
                text = encodeStringArray((String[])v);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            this.setValue(decodeStringArray(text));
        }
    }   // End of StringArrayPropertyEditor


    /**
     * Inner Class: A PropertyEditor for integer array types.
     */
    private static class IntArrayPropertyEditor extends PropertyEditorSupport {
        public IntArrayPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof int[]) {
                text = encodeIntArray((int[])v);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            this.setValue(decodeIntArray(text));
        }
    }   // End of IntArrayPropertyEditor

    /**
     * Inner Class: A PropertyEditor for byte array types.
     */
    private static class ByteArrayPropertyEditor extends PropertyEditorSupport {
        public ByteArrayPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof byte[]) {
                text = BASE64.encode((byte[])v);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            try { this.setValue(BASE64.decode(text)); }
            catch (Exception ex) { this.setValue(null); }
        }
    }   // End of ByteArrayPropertyEditor

    /**
     * Inner Class: A PropertyEditor for Font types.
     */
    private static class FontPropertyEditor extends PropertyEditorSupport {
        public FontPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof Font) {
                Font font = (Font)v;
                StringBuffer sbuf = new StringBuffer();
                sbuf.append(font.getName().replace(' ', '_'));
                sbuf.append('-');
                if (font.isBold() && font.isItalic()) {
                    sbuf.append("BoldItalic");
                } else if (font.isBold()) {
                    sbuf.append("Bold");
                } else if (font.isItalic()) {
                    sbuf.append("Italic");
                } else {
                    sbuf.append("Plain");
                }
                sbuf.append('-');
                sbuf.append(Integer.toString(font.getSize()));
                text = sbuf.toString();
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            String[] parms = StringUtils.tokenize(text, "-");
            if (parms.length == 3) {
                boolean okay = true;
                String fontName = parms[0].replace('_', ' ');
                int fontStyle = Font.PLAIN;
                int fontSize = 0;
                if ("bolditalic".equalsIgnoreCase(parms[1])) {
                    fontStyle = Font.BOLD + Font.ITALIC;
                } else if ("bold".equalsIgnoreCase(parms[1])) {
                    fontStyle = Font.BOLD;
                } else if ("italic".equalsIgnoreCase(parms[1])) {
                    fontStyle = Font.ITALIC;
                } else if ("plain".equalsIgnoreCase(parms[1])) {
                    fontStyle = Font.PLAIN;
                } else {
                    okay = false;
                }
                try {
                    fontSize = Integer.parseInt(parms[2]);
                    if (fontSize < 4) {
                        okay = false;
                    }
                }
                catch (NumberFormatException ex) { okay = false; }
                if (okay) {
                    this.setValue(new Font(fontName, fontStyle, fontSize));
                }
            }
        }
    }   // End of FontPropertyEditor


    /**
     * Inner Class: A PropertyEditor for KeyStroke types.
     */
    private static class KeyStrokePropertyEditor extends PropertyEditorSupport {
        public KeyStrokePropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof KeyStroke) {
                KeyStroke keyStroke = (KeyStroke)v;
                text = encodeKeyStroke(keyStroke);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            if (text != null) {
                KeyStroke keyStroke = decodeKeyStroke(text);

                if (keyStroke != null) {
                    this.setValue(keyStroke);
                }
            }
        }
    }   // End of KeyStrokePropertyEditor

    /**
     * Inner Class: A PropertyEditor for KeyStroke types.
     */
    private static class KeyStrokeArrayPropertyEditor extends PropertyEditorSupport {
        public KeyStrokeArrayPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof KeyStroke[]) {
                KeyStroke[] vArray = (KeyStroke[])v;
                text = encodeKeyStrokeArray(vArray);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            this.setValue(decodeKeyStrokeArray(text));
        }
    }   // End of KeyStrokeArrayPropertyEditor


    /**
     * Inner Class: A PropertyEditor for Point types.
     */
    private static class PointPropertyEditor extends PropertyEditorSupport {
        public PointPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof Point) {
                text = encodePoint((Point)v);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            this.setValue(decodePoint(text));
        }
    }   // End of PointPropertyEditor


    /**
     * Inner Class: A PropertyEditor for Dimension types.
     */
    private static class DimensionPropertyEditor extends PropertyEditorSupport {
        public DimensionPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof Dimension) {
                text = encodeDimension((Dimension)v);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            this.setValue(decodeDimension(text));
        }
    }   // End of DimensionPropertyEditor


    /**
     * Inner Class: A PropertyEditor for Rectangle types.
     */
    private static class RectanglePropertyEditor extends PropertyEditorSupport {
        public RectanglePropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof Rectangle) {
                text = encodeRectangle((Rectangle)v);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            this.setValue(decodeRectangle(text));
        }
    }   // End of RectanglePropertyEditor

    /**
     * Inner Class: A PropertyEditor for Color types.
     */
    private static class ColorPropertyEditor extends PropertyEditorSupport {
        public ColorPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof Color) {
                Color c = (Color)v;
                text = encodeColor(c);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            setValue(decodeColor(text));
        }
    }   // End of ColorPropertyEditor


    /**
     * Inner Class: A PropertyEditor for Polygon types.
     */
    private static class PolygonPropertyEditor extends PropertyEditorSupport {
        public PolygonPropertyEditor() {}

        // Gets the string representation of the property value.
        public String getAsText() {
            String text = null;
            Object v = this.getValue();
            if (v instanceof Polygon) {
                text = encodePolygon((Polygon)v);
            }
            return text;
        }

        // Sets the property value by parsing a given String.
        public void setAsText(String text) throws IllegalArgumentException {
            this.setValue(decodePolygon(text));
        }
    }   // End of PolygonPropertyEditor

    /**
     * A standalone property that implements {@link PropertyChangeListener}
     * interface.
     */
    private
    abstract
    static
    class       ListeningProperty
    extends     PropertySet.StandaloneProperty
    implements  PropertyChangeListener {
        ListeningProperty(String name, Class type, Object value, boolean archive) {
            super(name, type, value, archive);
        }
    }

    /**
     * An interface that combines Runnable and DocumentListener
     */
    private interface RunnableDocumentListener extends Runnable, DocumentListener {
    }

    //======================================================================
    //      W E A K    L I S T E N E R
    //======================================================================

    /**
     * Adds a property change listener so that the listener can be garbage
     * collected before the property.
     */
    private static void addWeakPropertyChangeListener(Property property,
                                                      PropertyChangeListener listener) {
        new WeakPropertyChangeListener(property,listener);
    }

    /**
     * A property change listener that allows the listener be garbage
     * collected before the property.
     */
    private static class WeakPropertyChangeListener implements PropertyChangeListener {
        private Property prop;
        private Reference ref;

        /**
         * NOTE: constructor register with the property
         */
        WeakPropertyChangeListener(Property property,
                                   PropertyChangeListener listener) {
            ref = new WeakReference(listener);
            prop = property;
            prop.addPropertyChangeListener(this);
        }

        /**
         * Property change event gets forwarded to the actual listener
         */
        public void propertyChange(PropertyChangeEvent e) {
            PropertyChangeListener listener = (PropertyChangeListener)ref.get();
            if (listener == null) {
                prop.removePropertyChangeListener(this);
            } else {
                listener.propertyChange(e);
            }
        }
    }

    //======================================================================
    //      I N P U T    V A L I D A T I O N
    //======================================================================

    /**
     * Interface for validating text input. The <code>validate()</code>
     * method is invoked by the framework on a separate thread, because
     * the process of validation may take significant time (consider, for
     * instance, a validator that attempts to connect to a host that happens
     * to be down). Here is an example of a validator that validates a
     * input that must represent a valid URL:
     * <blockquote>
     * <pre>
     *     public static class URLValidator implements PropertyEditors.TextValidator {
     *         public boolean validate(String text) {
     *             try { URL url = new URL(text); return true; }
     *             catch (IOException x) { return false; }
     *         }
     *     }
     * </pre>
     * </blockquote>
     */
    public interface TextValidator {

        /**
         * Validate the string.
         * @param text the string to validate
         */
        public boolean validate(String text);
    }

    /**
     * Convenience method to install a text validator.
     */
    public static TextComponentValidator
    installTextValidator(JTextComponent c, TextValidator v) {
        TextComponentValidator tcv = new TextComponentValidator(c, v);
        tcv.install();
        return tcv;
    }

    /**
     * Runs given work item on validation thread. Starts the thread if
     * necessary
     */
    private static void runOnValidationThread(Runnable workItem) {
        synchronized (textValidationLock) {
            if (!activeValidators.contains(workItem)) {
                activeValidators.add(workItem);
            }
            if (textValidationThread == null || !textValidationThread.isAlive()) {
                textValidationThread = new TextValidationThread();
            }
            textValidationLock.notify();
        }
    }

    /**
     * This is a per-component text validation context.
     */
    public static class TextComponentValidator {

        private Timer timer;
        private TextValidator validator;
        private JTextComponent component;
        private int validationDelay = 1000;
        private boolean textListenerInstalled = false;

        // inner objects to avoid implementing those interfaces directly
        private DocumentListener textListener = null;
        private Runnable updater = null;
        private ActionListener timerAction;

        private boolean textValid = true;
        private String text;

        private Color normalColor;
        private Color disabledColor;
        private Color errorColor;
        private Color disabledErrorColor;

        TextComponentValidator(JTextComponent component, TextValidator validator) {
            this.component = component;
            this.validator = validator;
            text = component.getText();

            // colors
            normalColor = component.getForeground();
            disabledColor = component.getDisabledTextColor();

            // should these two be configurable?
            errorColor = Color.red;
            disabledErrorColor = errorColor.darker();

            // setup timer
            timerAction = new TimerAction();
            timer = new Timer(validationDelay, timerAction);
            timer.setRepeats(false);
        }

        public TextValidator getValidator() {
            return validator;
        }

        public JTextComponent getTextComponent() {
            return component;
        }

        /**
         * Sets the minimum delay between the text changes and the time
         * when validation occurs
         */
        public void setValidationDelay(int delay) {
            if (validationDelay != delay) {
                validationDelay = delay;
                boolean wasRunning = false;
                if (timer.isRunning()) {
                    wasRunning = true;
                    timer.stop();
                }

                // change the delay and restart the timer
                timer.setDelay(validationDelay);
                timer.setInitialDelay(validationDelay);
                if (wasRunning) {
                    timer.start();
                }
            }
        }

        /**
         * Attaches to the target component and starts listening for
         * text change events. This method should be invoked on event
         * dispatch thread.
         */
        public void install() {
            if (!textListenerInstalled) {
                if (textListener == null) {
                    textListener = new TextChangeListener();
                }
                textListenerInstalled = true;
                component.getDocument().addDocumentListener(textListener);

                // run initial validation
                SwingUtilities.invokeLater(new InitialValidation());
           }
        }

        /**
         * Detaches from the target component. Effectively stops validation.
         * This method should be invoked on event dispatch thread.
         */
        public void uninstall() {
            if (textListenerInstalled) {
                textListenerInstalled = false;
                component.getDocument().removeDocumentListener(textListener);

                // reset text color back to the default
                textValid = true;
                updateTextColor();
            }
        }

        public boolean isInstalled() {
            return textListenerInstalled;
        }

        /**
         * Synchronized with Validator.run
         */
        private synchronized void updateTextColor() {
            component.setForeground(textValid ? normalColor : errorColor);
            component.setDisabledTextColor(textValid ? disabledColor : disabledErrorColor);
        }

        private void handleInput() {
            String s = component.getText();
            if (!text.equals(s)) {
                text = s;
                timer.restart();
            }
        }

        private class TextChangeListener implements DocumentListener {
            public void insertUpdate(DocumentEvent e) { handleInput(); }
            public void removeUpdate(DocumentEvent e)  { handleInput(); }
            public void changedUpdate(DocumentEvent e)  { handleInput(); }
        }

        private class InitialValidation implements Runnable {
            public void run() {
                if (textListenerInstalled) {
                    text = component.getText();
                    runOnValidationThread(new Validator(text));
                }
            }
        }

        private class Updater implements Runnable {
            public void run() {
                updateTextColor();
            }
        }

        private class Validator implements Runnable {
            private String text;
            Validator(String text) {
                this.text = text;
            }
            public void run() {
                boolean ok;
                try { ok = validator.validate(text); }
                catch (Throwable x) {
                    printStackTrace(x);
                    ok = false;
                }

                synchronized (TextComponentValidator.this) {
                    if (ok != textValid) {
                        if (updater == null) {
                            updater = new Updater();
                        }
                        SwingUtilities.invokeLater(updater);
                        textValid = ok;
                    }
                }
            }
        }

        private class TimerAction implements ActionListener {
            public void actionPerformed(ActionEvent e) {
                runOnValidationThread(new Validator(component.getText()));
            }
        }
    }

    /**
     * Thread that runs validators. Exits if idle for more than 30 seconds.
     */
    private static class TextValidationThread extends Thread {

        private long timeout = 30000; // should this be configurable?
        TextValidationThread() {
            super("TextValidator");
            start();
        }

        public void run() {
            try {
                if (Trace.isVerbose()) {
                    Trace.Debug.println(getName() + " thread started.");
                }
                long start = System.currentTimeMillis();
                while (true) {
                    Runnable workItem = null;
                    synchronized (textValidationLock) {
                        if (activeValidators.isEmpty()) {
                            try { textValidationLock.wait(timeout); }
                            catch (InterruptedException x) {}
                        }
                        if (activeValidators.isEmpty()) {

                            // if waiting for too long, stop the thread
                            long now = System.currentTimeMillis();
                            if (now >= (start + timeout)) {
                                if (textValidationThread == this) {
                                    textValidationThread = null;
                                }
                                break;
                            } else {
                                continue;
                            }
                        } else {
                            workItem = (Runnable)activeValidators.remove(0);
                        }
                    }

                    workItem.run();
                    start = System.currentTimeMillis(); // restart the timer
                }
            } finally {
                synchronized (textValidationLock) {
                    if (textValidationThread == this) {
                        textValidationThread = null;
                    }
                }
                if (Trace.isVerbose()) {
                    Trace.Debug.println(getName() + " thread exiting.");
                }
            }
        }
    }

    private static final Object textValidationLock = new Object();
    private static List activeValidators = new LinkedList();
    private static Thread textValidationThread = null;

    private static Map editors;
}   // End of PropertyEditors
