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
import com.nokia.wtk.device.DeviceSocket;
import com.nokia.wtk.diagnostics.views.support.ColorStyle;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.StringUtils;

/**
 * A view for the model of tcp nodes
 */
class TCPUDPTreeNodes implements MessageWriters {

    static {
        AppEnvironment.addResources(
            "resources/TCPUDPTreeNodes.properties",
            TCPUDPTreeNodes.class);
    }

    static private Icon packetSentIcon =
        AppEnvironment.getIcon(Resource.TCPUDP_SENT_ICON);
    static private Icon packetReceivedIcon =
        AppEnvironment.getIcon(Resource.TCPUDP_RECEIVED_ICON);
    static private Icon contentDataIcon =
        AppEnvironment.getIcon(Resource.CONTENT_DATA_ICON);
    static final private String UDP_SOCKET =
        AppEnvironment.getString(Resource.UDP_STRING);
    static private final String TCP_SOCKET =
        AppEnvironment.getString(Resource.TCP_STRING);

    /*
     * Inner Class: A tree node for TCP/UDP
     */
    static class TCPUDPNode extends TopLevelContentNode {
        private boolean sent;
        private byte[] payload;

        /**
         * Creates a new TCPUDPNode.
         * @param fragment the received fragment
         */
        TCPUDPNode(DeviceSocket socket, byte[] payload, boolean sent) {
            setUserObject(socket);
            this.sent = sent;
            this.payload = payload;

            if (sent) {
               add(
                   new HeaderNode(
                       "Destination",
                       socket.getDestinationAddress()
                           + ":"
                           + socket.getDestinationPort()));
               add(
                   new HeaderNode(
                       "Source",
                       socket.getSourceAddress() + ":" + socket.getSourcePort()));
            } else {
               add(
                   new HeaderNode(
                       "Source",
                       socket.getDestinationAddress()
                           + ":"
                           + socket.getDestinationPort()));
               add(
                   new HeaderNode(
                       "Destination",
                       socket.getSourceAddress() + ":" + socket.getSourcePort()));
            }

            add(new PayloadDataNode(payload));
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            DeviceSocket socket = (DeviceSocket) getUserObject();
            StringBuffer sbuf = new StringBuffer();

            if (asHTML) {
                sbuf.append("<font " + ColorStyle.BLACK + ">");
            }
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.RED + ">");
            }
            if (sent) {
                sbuf.append("Sent: ");
            } else {
                sbuf.append("Received: ");
            }
            if (asHTML) {
                sbuf.append("</font>");
            }
            if (socket.getType() == DeviceSocket.TCP_SOCKET) {
                sbuf.append("TCP, ");
            } else if (socket.getType() == DeviceSocket.UDP_SOCKET) {
                sbuf.append("UDP, ");
            }

            if (payload.length >0) {
                sbuf.append(" ").append(payload.length).append(" bytes ");
            }

            if (sent) {
                sbuf.append(
                    "to "
                        + socket.getDestinationAddress()
                        + ":"
                        + socket.getDestinationPort());
            } else {
                sbuf.append(
                    "from "
                        + socket.getDestinationAddress()
                        + ":"
                        + socket.getDestinationPort());
            }

            if (asHTML) {
                sbuf.append("</font>");
            }

            return sbuf.toString();
        }

        public Icon getIcon() {
            if (sent) {
                return packetSentIcon;
            } else {
                return packetReceivedIcon;
            }
        }


        public boolean getSent() {
          return sent;
        }

        public int getPayloadLength() {
          return payload.length;
        }

        /* (non-Javadoc)
         * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#getSummary()
         */
        public String getSummary() {
            StringBuffer sbuf = new StringBuffer();
            DeviceSocket socket =
                (DeviceSocket) getUserObject();
            if (socket.getType() == DeviceSocket.TCP_SOCKET) {
                sbuf.append("TCP, ");
            } else if (socket.getType() == DeviceSocket.UDP_SOCKET) {
                sbuf.append("UDP, ");
            }
            if (sent) {
                sbuf.append(
                    "to "
                        + socket.getDestinationAddress()
                        + ":"
                        + socket.getDestinationPort());
            } else {
                sbuf.append(
                    "from "
                        + socket.getSourceAddress()
                        + ":"
                        + socket.getSourcePort());
            }

            return sbuf.toString();
        }

        /* (non-Javadoc)
         * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#isFiltered(com.nokia.wtk.diagnostics.gui.traffic.Filter)
         */
        boolean isFiltered(Filter f) {

            if (this.sent) {
                return !f.isTCPUDPSentVisible();
            } else {
                return !f.isTCPUDPReceivedVisible();
            }
        }
    } // End of SMSFragmentNode

    /**
     * Inner Class: A tree node for the payload data
     */
    static class PayloadDataNode extends ContentNode {

        /**
         * Creates a new FragmentDataNode.
         * @param fragment the fragment containing the data
         */
        protected PayloadDataNode(byte[] payload) {
            super(false);
            this.payload = payload;
            ContentNode dataNode;
            dataNode = new PayloadDataDisplayNode(payload);
            add(dataNode);

            // Allow UI to gain access to the content:
            setUserObject(this);
            dataNode.setUserObject(this);
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();

            sbuf.append("Content Data ");
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.GREEN + "><i>");
            }
            sbuf.append(" (").append(payload.length).append(" bytes)");
            if (asHTML) {
                sbuf.append("</i><font>");
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentDataIcon;
        }

        private byte[] payload;
    } // End of PayloadDataNode

    /*
     * Inner Class: A tree node for fragment data display.
     */

    static private JPanel panel;
    static private JTextArea textArea;
    static class PayloadDataDisplayNode
        extends ContentNode
        implements TreeCellRenderer {

        /**
         * Creates a new FragmentDataDisplayNode.
         * @param fragment the fragment containing the data
         */
        private PayloadDataDisplayNode(byte[] fragment) {
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
            String formattedText = null;
            //            String encoding = fragment.getEncoding();
            formattedText = toHexString((byte[]) fragment);
            return formattedText;

        }

        /**
         * Returns the string representation of a byte array.
         * @param bytes the byte array
         * @return the string representation of a byte array.
         */
        private static String toHexString(byte[] bytes) {
            String hex = StringUtils.hexDump(bytes);
            if (hex.endsWith("\n")) hex = hex.substring(0,hex.length()-1);
            return hex;
        }

        //TODO: Future right-click menu to enable/disable custom formatter?
        private boolean allowCustomFormatters = true;
        private byte[] fragment;
    } // End of DataDisplayNode

}
