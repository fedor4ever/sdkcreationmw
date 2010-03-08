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





package com.nokia.wtk.diagnostics.gui.traffic;

import java.awt.BorderLayout;
import java.awt.Component;

import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.util.Iterator;
import java.util.SortedMap;


import java.util.ArrayList;
import java.io.UnsupportedEncodingException;
import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTree;
import javax.swing.LookAndFeel;
import javax.swing.border.EtchedBorder;
import javax.swing.tree.TreeCellRenderer;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.device.DeviceMessage;
import com.nokia.wtk.device.DeviceMessageFragment;
import com.nokia.wtk.diagnostics.views.support.ColorStyle;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.StringUtils;



/**
 * A model for collecting push information from devices
 */
class SMSTreeNodes implements Resource, MessageWriters {

    static {
        AppEnvironment.addResources(
            "resources/SMSTreeNodes.properties",
            SMSTreeNodes.class);
    }

    static private Icon smsSentIcon =
        AppEnvironment.getIcon(SMS_SENT_ICON);
    static private Icon smsReceivedIcon =
        AppEnvironment.getIcon(SMS_RECEIVED_ICON);
    static private Icon smsFragmentSentIcon =
        AppEnvironment.getIcon(SMS_FRAGMENT_SENT_ICON);
    static private Icon smsFragmentReceivedIcon =
        AppEnvironment.getIcon(SMS_FRAGMENT_RECEIVED_ICON);
    static private Icon contentDataIcon =
        AppEnvironment.getIcon(CONTENT_DATA_ICON);


/*****   SMS Message Nodes  *******/

    /*
     * Inner Class: A tree node for SMS messages.
     */
    static class SMSNode extends TopLevelContentNode {
        /**
         * Creates a new SMSFragmentNode.
         * @param message the received SMS message
         */
        SMSNode(DeviceMessage message) {

            int i;
            DeviceMessageFragment[] frags = message.getFragments();
            ArrayList dataList = new ArrayList();
            for (i=0; i<frags.length; i++) {
              dataList.add(frags[i].getData());
            }
            if (dataList.get(0) instanceof String) {
              String dataString = getDataString(dataList);
              add (new SMSDataNode(dataString, frags[0]));
            }
            else {
              byte[] dataBytes = getDataBytes(dataList);
              add (new SMSDataNode(dataBytes, frags[0]));
            }
            for (i = 0; i < frags.length; i++) {
                add(new SMSFragmentNode(frags[i]));
            }
            setUserObject(frags[0]);

        }

        private String getDataString (ArrayList dataList) {
          StringBuffer buf = new StringBuffer();
          int i;
          for (i=0; i<dataList.size(); i++) {
            buf.append (dataList.get(i));
          }
          return buf.toString();
        }

        private byte[] getDataBytes (ArrayList dataList) {
          int byteCount = 0;
          int i;
          int copyIndex = 0;
          byte[] smallArray = null;

          for (i=0; i< dataList.size(); i++) {
            byteCount += ((byte[])dataList.get(i)).length;
          }

          byte[] bigArray = new byte[byteCount];

          for (i=0; i< dataList.size(); i++) {
            smallArray = (byte[])dataList.get(i);
            System.arraycopy (smallArray, 0,
                              bigArray,
                              copyIndex,
                              smallArray.length);
            copyIndex += smallArray.length;
          }
          return bigArray;
        }


        private void summarizeFragsText(
            DeviceMessageFragment frag,
            StringBuffer sbuf) {
            sbuf.append("'");
            String s = frag.getData().toString();
            if (s.length() > 10) {
                sbuf.append(s.substring(0, 9)).append("...");
            } else {
                sbuf.append(s);
            }
            sbuf.append("'");

        }
        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            DeviceMessageFragment fragment =
                (DeviceMessageFragment) getUserObject();
            String direction = fragment.getMessageType();
            StringBuffer sbuf = new StringBuffer();

            if (asHTML) {
                sbuf.append("<font " + ColorStyle.BLACK + ">");
            }
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.RED + ">");
            }
            if (direction.equalsIgnoreCase(SMS_SUBMIT)) {
                sbuf.append("Sent: ");
            } else {
                sbuf.append("Received: ");
            }
            if (asHTML) {
                sbuf.append("</font>");
            }
            sbuf.append(SMS_MESSAGE_TEXT + " ");

            if (direction.equalsIgnoreCase(SMS_SUBMIT)) {
                sbuf.append(" to " + fragment.getDestination());
            } else {
                sbuf.append(" from " + fragment.getSourceAddress());
            }
            String port = fragment.getHeader(String.valueOf(PORT_NUMBER_SHORT));
            if (port != null) {
              sbuf.append (":" + port);
            }
            else {
              port = fragment.getHeader(String.valueOf(PORT_NUMBER_LONG));
              if (port != null) {
                sbuf.append (":" + port);
              }
            }

            Object data = fragment.getData();
            // There is no summary for binary messages
            if (data instanceof String) {
              String codingScheme = fragment.getHeader(String.valueOf(SMS_TP_DCS));
              if (!(codingScheme == null || codingScheme.equalsIgnoreCase("unicode"))) {
                sbuf.append(", ");
                if (asHTML) {
                  sbuf.append("<font face=monospace " + ColorStyle.BLACK + ">");
                }
                summarizeFragsText(fragment, sbuf);
                if (asHTML) {
                  sbuf.append("</font>");
                }
              }
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            DeviceMessageFragment fragment =
                (DeviceMessageFragment) getUserObject();
            if (fragment.getMessageType().equalsIgnoreCase(SMS_SUBMIT)) {
                return smsSentIcon;
            } else {
                return smsReceivedIcon;
            }
        }

        /* (non-Javadoc)
         * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#getSummary()
         */
        public String getSummary() {
            StringBuffer sbuf = new StringBuffer(SMS_MESSAGE_TEXT).append(" ");
            DeviceMessageFragment fragment =
                (DeviceMessageFragment) getUserObject();
            String direction = fragment.getMessageType();

            if (direction.equalsIgnoreCase(SMS_SUBMIT)) {
                sbuf.append(" to ").append(fragment.getDestination());
            } else {
                sbuf.append(" from ").append(
                    fragment.getSourceAddress());
            }
            String port = fragment.getHeader(String.valueOf(PORT_NUMBER_SHORT));
            if (port != null) {
              sbuf.append (":" + port);
            }
            else {
              port = fragment.getHeader(String.valueOf(PORT_NUMBER_LONG));
              if (port != null) {
                sbuf.append (":" + port);
              }
            }

            Object data = fragment.getData();
            // There is no summary for binary messages
            if (data instanceof String) {
              sbuf.append(", ");
              summarizeFragsText(fragment, sbuf);
            }

            return sbuf.toString();
        }

        /* (non-Javadoc)
         * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#isFiltered(com.nokia.wtk.diagnostics.gui.traffic.Filter)
         */
        boolean isFiltered(Filter f) {
            DeviceMessageFragment fragment =
                (DeviceMessageFragment) getUserObject();
            if (fragment.getMessageType().equalsIgnoreCase(SMS_SUBMIT)) {
                return !f.isSMSSentVisible();
            } else {
                return !f.isSMSReceivedVisible();
            }
        }
    } // End of SMSNode


    /**
     * Inner Class: A tree node for SMS data.
     */
     static class SMSDataNode extends ContentNode {

        /**
         * Creates a new SMSDataNode.
         * @param data the data in the message
         * @param fragment the first fragment in the message
         */
        protected SMSDataNode(Object data, DeviceMessageFragment fragment) {
            super(false);
            this.fragment = fragment;
            this.data = data;
            ContentNode dataNode;
            dataNode = new SMSDataDisplayNode(data,fragment);
            add(dataNode);

            // Allow UI to gain access to the content:
            setUserObject(this);
            dataNode.setUserObject(this);
        }

        /**
         * Gets the text representation of SMS data.
         * @param the data in the message
         */
        protected static String toString(DeviceMessageFragment fragment, Object data) {

            StringBuffer sbuf = new StringBuffer();
            String codingScheme = fragment.getHeader(String.valueOf(SMS_TP_DCS));
            int length = 0;
            if (codingScheme == null) {
              codingScheme = "unknown coding scheme";
            }
            if (data instanceof String) {
              if (codingScheme.equalsIgnoreCase("unicode")) {
                try {
                  byte[] bytes = ((String)data).getBytes ("iso-10646-ucs-2");
                  length = bytes.length;
                } catch (UnsupportedEncodingException e) {
                  System.err.println ("UnsupportedEncodingException caught, " + e.getMessage());
                  length = ((String) data).length();
                }
              }
              else {
                length = ((String) data).length();
              }
            } else {
                length = ((byte[]) data).length;
            }
            sbuf.append (String.valueOf(length));
            sbuf.append(" bytes");

            sbuf.append(", " + codingScheme);
            return sbuf.toString();
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append("Content Data: ");
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.GREEN + ">");
            }
            sbuf.append(toString(fragment, data));
            if (asHTML) {
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentDataIcon;
        }

        private DeviceMessageFragment fragment;
        private Object data;

    } // End of SMSDataNode

    /*
     * Inner Class: A tree node for fragment data display.
     */


    static class SMSDataDisplayNode
        extends ContentNode
        implements TreeCellRenderer {

        /**
         * Creates a new SMSDataDisplayNode.
         * @param fragment the fragment containing the data
         */
        private SMSDataDisplayNode(Object data, DeviceMessageFragment fragment) {
            super(false);
            this.data = data;
            this.fragment = fragment;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            if (asHTML) {
                return null; // Using a special TreeCellRenderer instead
            } else {
                return getFormattedText();
            }
        }

        public String toString() {
            return getFormattedText();
        }

        /**
         * Gets the renderer component for this node.
         * @return the renderer component
         */
        public Component getTreeCellRendererComponent(
            JTree tree,
            Object value,
            boolean selected,
            boolean expanded,
            boolean leaf,
            int row,
            boolean hasFocus) {
            // ASSUMPTION: DeviceContentReceived never changes
            if (panel == null) {
                panel = new JPanel(new BorderLayout());
                textArea = new JTextArea();
                panel.setBorder(
                    BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
                panel.add(textArea, BorderLayout.CENTER);
                textArea.setFont(LookFactory.getStandardCodeFont());
            }
            textArea.setText(getFormattedText());

            if (selected) {
                LookAndFeel.installColors(
                    textArea,
                    "Table.selectedForeground",
                    "Table.selectedBackground");
            } else {
                LookAndFeel.installColors(
                    textArea,
                    "Table.background",
                    "Table.foreground");
            }

            return panel;
        }

        /**
         * Gets the formatted text for the data.
         * @return the formatted text
         */

        private String getFormattedText() {
            String formattedText = null;
            if (data instanceof String) {
                String codingScheme = fragment.getHeader(String.valueOf(SMS_TP_DCS));
                if (codingScheme.equalsIgnoreCase("unicode")) {
                  try {
                    byte[] bytes = ((String)data).getBytes ("iso-10646-ucs-2");
                    formattedText = toHexString(bytes);
                  } catch (UnsupportedEncodingException e) {
                    System.err.println ("UnsupportedEncodingException caught, " + e.getMessage());
                    formattedText = (String)data;
                  }
                  return formattedText;
                }
                else {
                  return (String) data;
                }
            } else {
                //            String encoding = fragment.getEncoding();
                formattedText = toHexString((byte[]) data);
                return formattedText;
            }
        }

        /**
         * Returns the string representation of a byte array.
         * @param bytes the byte array
         * @return the string representation of a byte array.
         */
        private static String toHexString(byte[] bytes) {
            StringBuffer sbuf = new StringBuffer();
            for (int i = 0; i < bytes.length; i++) {
                if (i > 0)
                    sbuf.append(',');
                if ((i > 0) && ((i % 16) == 0))
                    sbuf.append('\n');
                sbuf.append(StringUtils.toHexString(bytes[i]));
            }
            return sbuf.toString();
        }

        //TODO: Future right-click menu to enable/disable custom formatter?
        private boolean allowCustomFormatters = true;
        private Object data;
        private DeviceMessageFragment fragment;
    } // End of SMSDataDisplayNode


/*****   SMS Fragment Nodes  *******/

    /*
     * Inner Class: A tree node for SMS fragments.
     */
    static class SMSFragmentNode extends ContentNode {

        /**
         * Creates a new SMSFragmentNode.
         * @param fragment the received fragment
         */
        SMSFragmentNode(DeviceMessageFragment fragment) {
            setUserObject(fragment);
            //this.data = fragment.getData();

            add(new HeaderListNode(fragment));
            add(new FragmentDataNode(fragment));
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            DeviceMessageFragment fragment =
                (DeviceMessageFragment) getUserObject();
            String direction = fragment.getMessageType();
            StringBuffer sbuf = new StringBuffer();
            if (getParent() instanceof SMSFragmentNode) {
                sbuf.append("Original Response: ");
            } else {
                if (asHTML) {
                    sbuf.append("<font " + ColorStyle.BLACK + ">");
                }
                if (asHTML) {
                    sbuf.append("<font " + ColorStyle.RED + ">");
                }
                if (direction.equalsIgnoreCase(SMS_SUBMIT)) {
                    sbuf.append("Sent: ");
                } else {
                    sbuf.append("Received: ");
                }
                if (asHTML) {
                    sbuf.append("</font>");
                }
                sbuf.append(SMS_FRAGMENT_TEXT + ", ");
                sbuf.append(fragment.getIndexInMessage()).append(" of ");
                sbuf.append(fragment.getMessageLength()).append(", ");

                if (direction.equalsIgnoreCase(SMS_SUBMIT)) {
                    sbuf.append(" to " + fragment.getDestination());
                } else {
                    sbuf.append(" from " + fragment.getSourceAddress());
                }

                if (asHTML) {
                    sbuf.append("</font>");
                }
            }
            return sbuf.toString();
        }
        public Icon getIcon() {
            DeviceMessageFragment fragment =
                (DeviceMessageFragment) getUserObject();
            if (fragment.getMessageType().equalsIgnoreCase(SMS_SUBMIT)) {
                return smsFragmentSentIcon;
            } else {
                return smsFragmentReceivedIcon;
            }
        }

    } // End of SMSFragmentNode

    /**
     * Inner Class: A tree node for AttributedContent data.
     */
    static class FragmentDataNode extends ContentNode {

        /**
         * Creates a new FragmentDataNode.
         * @param fragment the fragment containing the data
         */
        protected FragmentDataNode(DeviceMessageFragment fragment) {
            super(false);
            this.fragment = fragment;
            ContentNode dataNode;
            dataNode = new FragmentDataDisplayNode(fragment);
            add(dataNode);

            // Allow UI to gain access to the content:
            setUserObject(this);
            dataNode.setUserObject(this);
        }

        /**
         * Gets the text representation of fragment data.
         * @param fragment the fragment data
         */
        protected static String toString(DeviceMessageFragment fragment) {
            StringBuffer sbuf = new StringBuffer();
            int length = 0;
            String direction = fragment.getMessageType();
            Object data = fragment.getData();
            String codingScheme = fragment.getHeader(String.valueOf(SMS_TP_DCS));
            if (codingScheme == null) {
              codingScheme = "unknown coding scheme";
            }
            if (data instanceof String) {
              if (codingScheme.equalsIgnoreCase("unicode")) {
                try {
                  byte[] bytes = ((String)data).getBytes ("iso-10646-ucs-2");
                  length = bytes.length;
                } catch (UnsupportedEncodingException e) {
                  System.err.println ("UnsupportedEncodingException caught, " + e.getMessage());
                  length = ((String) data).length();
                }
              }
              else {
                length = ((String) data).length();
              }
            } else {
                length = ((byte[]) data).length;
            }
            sbuf.append (String.valueOf(length));
            sbuf.append(" bytes");

            sbuf.append(", " + codingScheme);

/*
            if (direction.equalsIgnoreCase(SMS_SUBMIT)) {
                sbuf.append(", to " + fragment.getDestination());
            } else {
                sbuf.append(", from " + fragment.getSourceAddress());
            }
*/
            return sbuf.toString();
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append("Content Data: ");
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.GREEN + ">");
            }
            sbuf.append(toString(fragment));
            if (asHTML) {
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentDataIcon;
        }

        private DeviceMessageFragment fragment;
    } // End of FragmentDataNode

    /*
     * Inner Class: A tree node for fragment data display.
     */

    static class FragmentDataDisplayNode
        extends ContentNode
        implements TreeCellRenderer {

        /**
         * Creates a new FragmentDataDisplayNode.
         * @param fragment the fragment containing the data
         */
        private FragmentDataDisplayNode(DeviceMessageFragment fragment) {
            super(false);
            this.fragment = fragment;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            if (asHTML) {
                return null; // Using a special TreeCellRenderer instead
            } else {
                return getFormattedText();
            }
        }

        public String toString() {
            return getFormattedText();
        }

        /**
         * Gets the renderer component for this node.
         * @return the renderer component
         */
        public Component getTreeCellRendererComponent(
            JTree tree,
            Object value,
            boolean selected,
            boolean expanded,
            boolean leaf,
            int row,
            boolean hasFocus) {
            // ASSUMPTION: DeviceContentReceived never changes
            if (panel == null) {
                panel = new JPanel(new BorderLayout());
                textArea = new JTextArea();
                panel.setBorder(
                    BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
                panel.add(textArea, BorderLayout.CENTER);
                textArea.setFont(LookFactory.getStandardCodeFont());
            }
            textArea.setText(getFormattedText());

            if (selected) {
                LookAndFeel.installColors(
                    textArea,
                    "Table.selectedForeground",
                    "Table.selectedBackground");
            } else {
                LookAndFeel.installColors(
                    textArea,
                    "Table.background",
                    "Table.foreground");
            }

            return panel;
        }

        /**
         * Gets the formatted text for the data.
         * @return the formatted text
         */

        private String getFormattedText() {
           Object data = fragment.getData();
           int i;
           String formattedText = null;
            if (data instanceof String) {
                String codingScheme = fragment.getHeader(String.valueOf(SMS_TP_DCS));
                if (codingScheme.equalsIgnoreCase("unicode")) {
                  try {
                    /*
                    SortedMap charsets = Charset.availableCharsets();
                    if (charsets != null) {
                      Iterator it = charsets.keySet().iterator();
                      while (it.hasNext()) {
                        System.err.println (it.next().toString());
                      }
                    }
                    */
                    byte[] bytes = ((String)data).getBytes ("iso-10646-ucs-2");
                    formattedText = toHexString(bytes);
                  } catch (UnsupportedEncodingException e) {
                    System.err.println ("UnsupportedEncodingException caught, " + e.getMessage());
                    formattedText = (String)data;
                  }
                  return formattedText;
                }
                else {
                  return (String) data;
                }
            } else {
                formattedText = null;
                //            String encoding = fragment.getEncoding();
                formattedText = toHexString((byte[]) fragment.getData());
                return formattedText;
            }
        }

        /**
         * Returns the string representation of a byte array.
         * @param bytes the byte array
         * @return the string representation of a byte array.
         */
        private static String toHexString(byte[] bytes) {
            StringBuffer sbuf = new StringBuffer();
            for (int i = 0; i < bytes.length; i++) {
                if (i > 0)
                    sbuf.append(',');
                if ((i > 0) && ((i % 16) == 0))
                    sbuf.append('\n');
                sbuf.append(StringUtils.toHexString(bytes[i]));
            }
            return sbuf.toString();
        }

        //temp method

    private static String intToHexString(int i) {

        String  hexStr;

        // Create hex string representation of integer
        hexStr = new String(Integer.toHexString(i));
        // Ensure characters are upper-case
        hexStr = hexStr.toUpperCase();
        // Pad with a leading "0" if there is an odd number of characters (to make whole bytes)
        if(((hexStr.length() / 2) * 2) != hexStr.length())
        {
            hexStr = "0" + hexStr;
        }
        return hexStr;
    }


        //TODO: Future right-click menu to enable/disable custom formatter?
        private boolean allowCustomFormatters = true;
        private DeviceMessageFragment fragment;
    } // End of fragmentDataDisplayNode

    static private JPanel panel;
    static private JTextArea textArea;

    public static final String SMS_SUBMIT = "SMS-SUBMIT";
    public static final String SMS_DELIVER = "SMS_DELIVER";
    public static final String SMS_FRAGMENT_TEXT = "SMS Fragment";
    public static final String SMS_MESSAGE_TEXT = "SMS";
    private static final int PORT_NUMBER_SHORT = 104;
    private static final int PORT_NUMBER_LONG = 105;
    private static final int SMS_TP_DCS = 8;

}
