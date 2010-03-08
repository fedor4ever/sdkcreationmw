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


package com.nokia.epdt.plugins.s60.platsec;

/* java.util */
import java.util.List;
import java.util.Observer;
import java.util.ArrayList;
import java.util.Properties;
import java.util.Observable;

/* java.awt */
import java.awt.Color;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.Component;
import java.awt.Container;
import java.awt.CardLayout;
import java.awt.BorderLayout;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;

/* java.awt.event */
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.Action;
import javax.swing.KeyStroke;
import javax.swing.JCheckBox;
import javax.swing.ListModel;
import javax.swing.UIManager;
import javax.swing.JComponent;
import javax.swing.JScrollPane;
import javax.swing.ButtonModel;
import javax.swing.SwingUtilities;
import javax.swing.AbstractAction;
import javax.swing.ListCellRenderer;
import javax.swing.AbstractListModel;
import javax.swing.ListSelectionModel;

/* javax.swing.event */
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

/* javax.swing.border */
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;

/* com.nokia.wtk.util.gui */
import com.nokia.util.gui.GUI;
import com.nokia.wtk.util.gui.BorderPanel;

/* com.nokia.epdt.core.config.ini.epoc */
import com.nokia.epdt.plugins.s60.platsec.SecEpocIniConstants;

/**
 *
 * PlatformSecurityJPanel
 */
class PlatformSecurityJPanel extends JPanel
    implements ActionListener, SecEpocIniConstants {

    private static final int GAP = 6;
    private static final String EMPTY_SELECTION_KEY = "";
    private static String RESTART_NOTE =
    	"Emulator must be restarted in order to activate the changes.";
    private static String RESTART_NOTE_TITLE = "Preferences";
    private PlatformSecurityPlugin platformSecurityPlugin;
    private CapabilityListModel capabilities;
    private JPanel descriptionPanel;
    //private JCheckBox diagnosticsCheckBox;//requirement rejected, but let us keep it for a while
    private JCheckBox enforcementCheckBox;
    private JCheckBox apiCheckBox;
    private JList capabilitiesList;
    private int[] selectedIndices;

    /**
     * Creates PlatformSecurityJPanel
     *
     * @param plugin instance of plugin
     * @param observable probably the data model
     */
    PlatformSecurityJPanel(PlatformSecurityPlugin plugin) {
        super(new GridBagLayout());
        platformSecurityPlugin = plugin;
        capabilities = new CapabilityListModel();

        //requirement rejected, but let us keep it for a while
        //diagnosticsCheckBox = new JCheckBox("Enable security debug messages"); // Diagnostics Messages
        apiCheckBox = new JCheckBox("Panic|Leave on insecure APIs calls"); // Disable Insecure APIs
        enforcementCheckBox = new JCheckBox("Perform capability checks"); // Enforcement
        //diagnosticsCheckBox.addActionListener(this);
        apiCheckBox.addActionListener(this);
        enforcementCheckBox.addActionListener(this);

        // Initialize the layout
        BorderPanel bp = new BorderPanel(enforcementCheckBox);
        bp.getContentPanel().add(createCapabilityListPanel());

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.weightx = 1.0;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.NORTH;
        gbc.insets.left = bp.getBorder().getBorderInsets(bp).left +
            BorderPanel.COMPONENT_INSET_H;
        //add(diagnosticsCheckBox, gbc);
        add(apiCheckBox, gbc);

        gbc.weighty = 1.0;
        gbc.insets.left = 0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        add(bp, gbc);
        
        setBorder(new EmptyBorder(GAP,GAP,GAP,GAP));
    }

    private JPanel createCapabilityListPanel() {
        // Descriptions of the capabilities
        CardLayout descriptionLayout = new CardLayout2();
        descriptionPanel = new JPanel(descriptionLayout);
        descriptionPanel.add(GUI.createTextComponent(""),EMPTY_SELECTION_KEY);
        for (int i=0; i<capabilities.getSize(); i++) {
            Capability cap = capabilities.getCapability(i);
            JComponent text = GUI.createTextComponent(cap.getDescription());
            descriptionPanel.add(text, cap.getKey());
        }

        // List of capabilities and their description
        capabilitiesList = new JList(capabilities);
        CapabilityListRenderer renderer = new CapabilityListRenderer();
        capabilitiesList.setCellRenderer(renderer);
        capabilitiesList.addMouseListener(renderer);
        capabilitiesList.addFocusListener(renderer);
        capabilitiesList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
        capabilitiesList.getSelectionModel().addListSelectionListener(
            new CapabilitySelectionListener());

        // Select the first row after we have created the description panel
        // and registered the selection listener. That will update the text
        // in the description panel.
        capabilitiesList.setSelectedIndex(0);

        // Register additional keyboard actions (spacebar => toggle)
        Action toggle = new CapabilityToggleAction();
        Object key = toggle.getValue(Action.NAME); // action map key
        KeyStroke ks = KeyStroke.getKeyStroke(KeyEvent.VK_SPACE,0);
        capabilitiesList.getInputMap(JComponent.WHEN_FOCUSED).put(ks, key);
        capabilitiesList.getActionMap().put(key, toggle);

        // Construct the panel combining both description and the list
        JPanel capPanel = new JPanel(new BorderLayout(GAP,GAP));
        capPanel.add(new JScrollPane(capabilitiesList), BorderLayout.CENTER);
        capPanel.add(new JLabel("Granted capabilities for all applications:"), BorderLayout.NORTH);
        capPanel.add(descriptionPanel, BorderLayout.SOUTH);
        capPanel.setBorder(new EmptyBorder(0,GAP,GAP,GAP));
        return capPanel;
    }

    // ActionListener
    public void actionPerformed(ActionEvent e) {
        /*if (e.getSource() == diagnosticsCheckBox) {
            platformSecurityPlugin.setModelProperty(
                PLATSEC_DIAGNOSTICS,
                diagnosticsCheckBox.isSelected());
        } else*/
    	if (e.getSource() == enforcementCheckBox) {
            platformSecurityPlugin.setModelProperty(
                PLATSEC_ENFORCEMENT,
                enforcementCheckBox.isSelected());
            setCapabilityListEnabled(enforcementCheckBox.isSelected());
        } else if (e.getSource() == apiCheckBox) {
            platformSecurityPlugin.setModelProperty(
                PLATSEC_PROCESS_ISOLATION,
                apiCheckBox.isSelected());
        }
    }

    // Observer
    public void update() 
    {
        //diagnosticsCheckBox.setSelected(Boolean.valueOf(platformSecurityPlugin.getModelProperty(PLATSEC_DIAGNOSTICS)).booleanValue());
        apiCheckBox.setSelected(Boolean.valueOf(platformSecurityPlugin.getModelProperty(PLATSEC_PROCESS_ISOLATION)).booleanValue());

        boolean enforcement = Boolean.valueOf(platformSecurityPlugin.getModelProperty(PLATSEC_ENFORCEMENT)).booleanValue();
        enforcementCheckBox.setSelected(enforcement);
        setCapabilityListEnabled(enforcement);
    }
    
    public void showRestartWarning()
    {
        JOptionPane.showMessageDialog(this,
        		RESTART_NOTE, RESTART_NOTE_TITLE,
				JOptionPane.INFORMATION_MESSAGE);
    }
    

    /**
     * Enables or disables the capability list. Additionally, clears the
     * selection when the list gets disabled.
     * @param enabled <code>true</code> to enable the list,
     * 				  <code>false</code> to disable.
     */
    private void setCapabilityListEnabled(boolean enabled) {
        // We store las selected index, so that we can restore the last
        // selection after the list has been disabled and then enabled
        // again. When the list is disabled, we reset the selection.
        // For aesthetic reasons :)
        if (capabilitiesList.isEnabled() != enabled) {
            capabilitiesList.setEnabled(enabled);
            if (enabled) {
                if (selectedIndices != null) {
                    capabilitiesList.setSelectedIndices(selectedIndices);
                    selectedIndices = null;
                }
            } else {
                selectedIndices = capabilitiesList.getSelectedIndices();
                capabilitiesList.clearSelection();
            }
        }
    }

    //=========================================================================
    //   I N N E R    C L A S S E S
    //=========================================================================

    /**
     * Represents a single item in the capability list
     */
    private class Capability {
        private String key;
        private String name;
        private String description;

        Capability(String key, String propertyName) {
            this.key = key;
            Properties p = platformSecurityPlugin.getPluginProperties();
            name = p.getProperty(propertyName + ".name", key);
            description = p.getProperty(propertyName + ".description", "");
        }

        String getKey() {
            return key;
        }

        String getName() {
            return name;
        }

        String getDescription() {
            return description;
        }

        public String toString() {
            return name;
        }

        boolean getValue() {
            return platformSecurityPlugin.getModelProperty(key);
        }

        void setValue(boolean b) {
            if (b != getValue()) {
                platformSecurityPlugin.setModelProperty(key,b);
                capabilities.capabilityChanged(this);
            }
        }

        boolean toggleValue() {
            boolean b = !getValue();
            platformSecurityPlugin.setModelProperty(key,b);
            capabilities.capabilityChanged(this);
            return b;
        }
    }

    /**
     * The list data model
     */
    private class CapabilityListModel extends AbstractListModel {

        private Capability[] capabilities = new Capability[] {
            // The first parameter is a constant from SecEpocIniConstants,
            // the second one is a property prefix from platsec.properties
            // They are usually the same, but it's OK if they are not.
            new Capability(LOCAL_SERVICES, "LocalServices"),
            new Capability(READ_USER_DATA, "ReadUserData"),
            new Capability(WRITE_USER_DATA, "WriteUserData"),
            new Capability(USER_ENVIRONMENT, "UserEnvironment"),
            new Capability(NETWORK_SERVICES, "NetworkServices"),
            new Capability(LOCATION, "Location"),
            new Capability(MULTIMEDIA_DD, "MultiMediaDD"),
            new Capability(READ_DEVICE_DATA, "ReadDeviceData"),
            new Capability(WRITE_DEVICE_DATA, "WriteDeviceData"),
            new Capability(POWER_MGMT, "PowerMgmt"),
            new Capability(TRUSTED_UI, "TrustedUI"),
            new Capability(PROT_SERV, "ProtServ"),
            new Capability(NETWORK_CONTROL, "NetworkControl"),
            new Capability(SW_EVENT, "SwEvent"),
            new Capability(DRM, "DRM"),
            new Capability(SURROUNDINGS_DD, "SurroundingsDD"),
            new Capability(TCB, "TCB"),
            new Capability(COMM_DD, "CommDD"),
            new Capability(DISK_ADMIN, "DiskAdmin"),
            new Capability(ALL_FILES, "AllFiles")
        };

        /**
         * Returns the length of the list. Required by ListModel.
         * @return the length of the list
         */
        public int getSize() {
            return capabilities.length;
        }

        /**
         * Returns the value at the specified index. Required by ListModel.
         * @param index the requested index
         * @return the value at <code>index</code>
         */
        public Object getElementAt(int index) {
            return capabilities[index];
        }

        /**
         * Returns the {@link Capability} at the specified index.
         * @param index the requested index
         * @return the {@link Capability} at <code>index</code>
         */
        final Capability getCapability(int index) {
            return capabilities[index];
        }

        /**
         * Finds the index of the specified {@link Capability} in this model.
         * @param capability the {@link Capability} to find
         * @return the index of the specified {@link Capability},
         *   or <code>-1</code> if the capability is not found,
         *   or if it is <code>null</code>
         */
        final int indexOf(Capability capability) {
            int n = capabilities.length;
            for(int i = 0; i<n; i++) {
                if (capabilities[i] == capability) {
                    return i;
                }
            }
            return -1;
        }

        /**
         * Fires the constentsChanged event.
         * @param capability the capability that has changed.
         */
        final void capabilityChanged(Capability capability) {
            int index = indexOf(capability);
            fireContentsChanged(this, index, index);
        }
    }

    /**
     * Renders the capability object as a checkbox. Also tracks the current
     * "pressed" state of the list items.
     */
    private static class CapabilityListRenderer extends JCheckBox
        implements ListCellRenderer, FocusListener, MouseListener {

        private List repaintQueue = new ArrayList();
        private Capability pressed = null;
        private Border noFocusBorder;
        private Insets insets;
        private int iconWidth = -1;

        public CapabilityListRenderer() {
            noFocusBorder = new EmptyBorder(1, 1, 1, 1);
            setOpaque(true);
            setBorder(noFocusBorder);
            setBorderPainted(true);
        }

        public Component getListCellRendererComponent(JList list, Object value,
            int index, boolean isSelected, boolean cellHasFocus) {

            Capability cap = (Capability)value;
            boolean capabilityEnabled = cap.getValue();
            setText(cap.getName());

            setComponentOrientation(list.getComponentOrientation());
            setFont(list.getFont());
            setEnabled(list.isEnabled());

            if (list.isEnabled()) {
                if (capabilityEnabled) {
                    setForeground(isSelected ?
                                  list.getSelectionForeground() :
                                  list.getForeground());
                } else {
                    // Shouldn't this color be L&F-specific?
                    setForeground(Color.RED);
                }

                setBackground(isSelected ?
                              list.getSelectionBackground() :
                              list.getBackground());
            } else {
                setForeground(list.getForeground());
                setBackground(list.getBackground());
            }

            if (cellHasFocus) {
                setBorder(UIManager.getBorder("List.focusCellHighlightBorder"));
            } else {
                setBorder(noFocusBorder);
            }

            ButtonModel bm = getModel();
            bm.setRollover(isSelected);
            bm.setPressed(cap == pressed);
            bm.setArmed((cap == pressed) && isSelected);
            bm.setSelected(capabilityEnabled);
            return this;
        }

        private int getIconWidth() {
            if (iconWidth <= 0) {
                Icon checkBoxIcon = UIManager.getIcon("CheckBox.icon");
                if (checkBoxIcon != null) {
                    // Width of the checkbox icon (for current Look&Feel)
                    iconWidth = checkBoxIcon.getIconWidth();
                    if (iconWidth > 0) {
                        return iconWidth;
                    }
                }
                // Something is wrong, use default icon width
                iconWidth = 12;
            }
            return iconWidth;
        }

        private Capability getCapabilityForEvent(MouseEvent e) {
            JList list = (JList)e.getSource();
            if (e.getX() < (getIconWidth()+2+list.getInsets(insets).left)) {
                int index = list.locationToIndex(e.getPoint());
                if (index >= 0) {
                    ListModel model = list.getModel();
                    return (Capability)model.getElementAt(index);
                }
            }
            return null;
        }

        private void queueForRepaint(Capability cap) {
            if (cap != null && !repaintQueue.contains(cap)) {
                repaintQueue.add(cap);
            }
        }

        private void flushRepaintQueue(JList list) {
            int n = repaintQueue.size();
            for (int i=n-1; i>=0; i--) {
                repaintItem(list, (Capability)repaintQueue.remove(i));
            }
        }

        private void repaintItem(JList list, Capability cap) {
            CapabilityListModel model = (CapabilityListModel)list.getModel();
            int index = model.indexOf(cap);
            Rectangle cellRect = list.getCellBounds(index,index);
            if (cellRect != null) {
                list.repaint(cellRect);
            }
        }

        // MouseListener
        public void mousePressed(MouseEvent e) {
            if (SwingUtilities.isLeftMouseButton(e) ) {
                JList list = (JList)e.getSource();
                if (list.isEnabled()) {
                    Capability cap = getCapabilityForEvent(e);
                    if (cap != null && pressed != cap) {
                        queueForRepaint(pressed);
                        queueForRepaint(cap);
                        pressed = cap;
                        flushRepaintQueue(list);
                    }
                }
            }
        }

        public void mouseReleased(MouseEvent e) {
            if (SwingUtilities.isLeftMouseButton(e)) {
                JList list = (JList)e.getSource();
                if (list.isEnabled()) {
                    Capability cap = getCapabilityForEvent(e);
                    if (cap != null) {
                        if (pressed == cap) {
                            // Toggle the state
                            cap.toggleValue();
                            queueForRepaint(cap);
                        }
                    }
                    queueForRepaint(pressed);
                    pressed = null;
                    flushRepaintQueue((JList)e.getSource());
                }
            }
        }

        public void mouseEntered(MouseEvent e) {
            if (pressed != null) {
                Capability cap = getCapabilityForEvent(e);
                if (cap != null && cap == pressed) {
                    queueForRepaint(cap);
                    flushRepaintQueue((JList)e.getSource());
                }
            }
        }

        public void mouseExited(MouseEvent e) {
            if (pressed != null) {
                queueForRepaint(pressed);
                flushRepaintQueue((JList)e.getSource());
            }
        }

        public void mouseMoved(MouseEvent e) {}
        public void mouseDragged(MouseEvent e) {}
        public void mouseClicked(MouseEvent e) {}

        // FocusListener
        public void focusGained(FocusEvent e) {}
        public void focusLost(FocusEvent e) {
            if (pressed != null) {
                queueForRepaint(pressed);
                pressed = null;
                flushRepaintQueue((JList)e.getSource());
            }
        }

        // The following methods are overridden for performance reasons.
        // This is similar to what DefaultListCellRenderer does
        public void validate() {}
        public void revalidate() {}
        public void repaint(long tm, int x, int y, int width, int height) {}
        public void repaint(Rectangle r) {}
        public void firePropertyChange(String name, byte oldVal, byte newVal) {}
        public void firePropertyChange(String name, char oldVal, char newVal) {}
        public void firePropertyChange(String name, short oldVal, short newVal) {}
        public void firePropertyChange(String name, int oldVal, int newVal) {}
        public void firePropertyChange(String name, long oldVal, long newVal) {}
        public void firePropertyChange(String name, float oldVal, float newVal) {}
        public void firePropertyChange(String name, double oldVal, double newVal) {}
        public void firePropertyChange(String name, boolean oldVal, boolean newVal) {}
    }

    /**
     * Listens for selection events in the list and updates the description
     * of the currently selected capability.
     */
    private class CapabilitySelectionListener implements ListSelectionListener {
        public void valueChanged(ListSelectionEvent e) {
            if (!e.getValueIsAdjusting()) {
                ListSelectionModel m = capabilitiesList.getSelectionModel();
                String key = EMPTY_SELECTION_KEY;
                if (!m.isSelectionEmpty()) {
                    int min = m.getMinSelectionIndex();
                    int max = m.getMaxSelectionIndex();
                    if (min == max) {
                        // single selection
                        key = capabilities.getCapability(min).getKey();
                        capabilitiesList.ensureIndexIsVisible(min);
                        selectedIndices = new int[] {min};
                    }
                }
                CardLayout layout = (CardLayout)descriptionPanel.getLayout();
                layout.show(descriptionPanel, key);
            }
        }
    }

    /**
     * Action that toggles currently the "granted" flag for the currenttly
     * selected capabilities.
     */
    private static class CapabilityToggleAction extends AbstractAction {
        CapabilityToggleAction() {
            // The action name is used as a key in the list's action map
            super("toogle");
        }
        public void actionPerformed(ActionEvent e) {
            JList list = (JList)e.getSource();
            if (!list.isSelectionEmpty()) {
                int[] selected = list.getSelectedIndices();
                for (int i=0; i<selected.length; i++) {
                    ((Capability)list.getModel().getElementAt(selected[i])).
                        toggleValue();
                }
            }
        }
    }

    /**
     * Unlike the original {@link CardLayout}, this one resizes all components
     * when it performs layout, making sure that preferred size is actually
     * the maximum size of all components. The original {@link CardLayout}
     * works well for most components, but not for text components, because
     * the preferred height of wrappable text component depends on its width.
     */
    private static class CardLayout2 extends CardLayout {
        public void layoutContainer(Container parent) {
            synchronized (parent.getTreeLock()) {
                Insets insets = parent.getInsets();
                int hgap = getHgap();
                int vgap = getVgap();
                int width = parent.getWidth();
                int height = parent.getHeight();
                int n = parent.getComponentCount();
                for (int i=0; i<n; i++) {
                    Component c = parent.getComponent(i);
                    c.setBounds(hgap+insets.left, vgap+insets.top,
                                width-(hgap*2+insets.left+insets.right),
                                height-(vgap*2+insets.top+insets.bottom));
                }
                super.layoutContainer(parent);
            }
        }
    }
}
