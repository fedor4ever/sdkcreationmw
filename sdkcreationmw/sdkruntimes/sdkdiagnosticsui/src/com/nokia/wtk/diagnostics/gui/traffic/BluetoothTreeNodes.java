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
import com.nokia.wtk.device.DeviceBluetooth;
import com.nokia.wtk.device.DeviceBluetoothSend;
import com.nokia.wtk.device.DeviceBluetoothReceive;
import com.nokia.wtk.diagnostics.views.support.ColorStyle;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.StringUtils;



/**
 * A model for collecting bluetooth information from devices
 */
class BluetoothTreeNodes implements Resource, MessageWriters {

  static {
    AppEnvironment.addResources(
            "resources/BluetoothTreeNodes.properties",
            BluetoothTreeNodes.class);
    }

  static private Icon bluetoothSentIcon =
    AppEnvironment.getIcon(BLUETOOTH_SENT_ICON);
  static private Icon bluetoothReceivedIcon =
    AppEnvironment.getIcon(BLUETOOTH_RECEIVED_ICON);
  static private Icon contentDataIcon =
    AppEnvironment.getIcon(CONTENT_DATA_ICON);


/*****   Bluetooth Packet Nodes  *******/

  /*
   * Inner Class: A tree node for Bluetooth send packets.
   */
  static class BluetoothSendNode extends TopLevelContentNode {

  /**
   * Creates a new BluetoothSendNode.
   * @param packet the bluetooth packet sent
   */
    BluetoothSendNode(DeviceBluetoothSend packet) {
      setUserObject(packet);
      if (packet.getBroadcastFlag() == DeviceBluetooth.NO_BROADCAST ) {
        add (new BluetoothDestinationNode(packet.getToAddress()));
      }
      add (new BluetoothPacketBoundaryNode(packet.getPacketBoundaryFlag(), true));
      add (new BluetoothBroadcastNode(packet.getBroadcastFlag(), true));
      add (new BluetoothDataNode(packet));
    }

    /**
     * Gets the text representation of this node.
     * @return the text representation of this node
     */
    public String getAsText(boolean asHTML) {
      DeviceBluetoothSend packet =
        (DeviceBluetoothSend) getUserObject();
      StringBuffer sbuf = new StringBuffer();
      int broadcastFlag = packet.getBroadcastFlag();

      if (asHTML) {
        sbuf.append("<font " + ColorStyle.BLACK + ">");
      }
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.PURPLE + ">");
      }
      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_SENT_TXT));
      }
      else {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_SENT_BR_TXT));
      }
      sbuf.append(" ");

      if (asHTML) {
        sbuf.append("</font>");
      }
      sbuf.append(packet.getDataTotalLength());
      sbuf.append(" bytes");
      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(" to ");
        sbuf.append(packet.getToAddress());
      }
      return sbuf.toString();
    }


    public Icon getIcon() {
      return bluetoothSentIcon;
    }

    /* (non-Javadoc)
     * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#getSummary()
     */
    public String getSummary() {
      DeviceBluetoothSend packet =
        (DeviceBluetoothSend) getUserObject();
      StringBuffer sbuf = new StringBuffer();
      int broadcastFlag = packet.getBroadcastFlag();

      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_SENT_TXT));
      }
      else {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_SENT_BR_TXT));
      }
      sbuf.append(" ");
      sbuf.append(packet.getDataTotalLength());
      sbuf.append(" bytes");
      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(" to ");
        sbuf.append(packet.getToAddress());
      }
      return sbuf.toString();
    }

    /* (non-Javadoc)
     * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#isFiltered(com.nokia.wtk.diagnostics.gui.traffic.Filter)
     */
    boolean isFiltered(Filter f) {
      return !f.isBluetoothSentVisible();
    }
  } // End of BluetoothSend node


  /*
   * Inner Class: A tree node for Bluetooth receive packets.
   */
  static class BluetoothReceiveNode extends TopLevelContentNode {

    /**
    * Creates a new BluetoothReceiveNode.
    * @param packet the bluetooth packet received
    */
    BluetoothReceiveNode(DeviceBluetoothReceive packet) {
      setUserObject(packet);
      if (packet.getBroadcastFlag() == DeviceBluetooth.NO_BROADCAST ) {
        add (new BluetoothSourceNode(packet.getFromAddress()));
      }
      add (new BluetoothPacketBoundaryNode(packet.getPacketBoundaryFlag(), false));
      add (new BluetoothBroadcastNode(packet.getBroadcastFlag(), false));
      add (new BluetoothDataNode(packet));
    }

    /**
     * Gets the text representation of this node.
     * @return the text representation of this node
     */
    public String getAsText(boolean asHTML) {
      DeviceBluetoothReceive packet =
        (DeviceBluetoothReceive) getUserObject();
      StringBuffer sbuf = new StringBuffer();
      int broadcastFlag = packet.getBroadcastFlag();

      if (asHTML) {
        sbuf.append("<font " + ColorStyle.BLACK + ">");
      }
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.DARKBLUE + ">");
      }
      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_RECEIVED_TXT));
      }
      else {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_RECEIVED_BR_TXT));
      }
      sbuf.append(" ");

      if (asHTML) {
        sbuf.append("</font>");
      }
      sbuf.append(packet.getDataTotalLength());
      sbuf.append(" bytes");
      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(" from ");
        sbuf.append(packet.getFromAddress());
      }
      return sbuf.toString();
    }


    public Icon getIcon() {
      return bluetoothReceivedIcon;
    }


    /* (non-Javadoc)
     * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#getSummary()
     */
    public String getSummary() {
      DeviceBluetoothReceive packet =
        (DeviceBluetoothReceive) getUserObject();
      StringBuffer sbuf = new StringBuffer();
      int broadcastFlag = packet.getBroadcastFlag();

      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_RECEIVED_TXT));
      }
      else {
        sbuf.append(AppEnvironment.getString(TRAFFIC_SUMMARY_BLUETOOTH_RECEIVED_BR_TXT));
      }
      sbuf.append(" ");
      sbuf.append(packet.getDataTotalLength());
      if (broadcastFlag == DeviceBluetooth.NO_BROADCAST) {
        sbuf.append(" from ");
        sbuf.append(packet.getFromAddress());
      }
      return sbuf.toString();
    }

    /* (non-Javadoc)
     * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#isFiltered(com.nokia.wtk.diagnostics.gui.traffic.Filter)
     */
    boolean isFiltered(Filter f) {
      return !f.isBluetoothReceivedVisible();
    }
  } // End of BluetoothReceive node


  /**
   * Inner Class: A tree node for Bluetooth data.
   */
  static class BluetoothDataNode extends ContentNode {

    /**
     * Creates a new BluetoothDataNode.
     * @param data the data in the message
     * @param the Bluetooth packet object
     */
    protected BluetoothDataNode(DeviceBluetooth packet) {
      super(false);
      this.packet = packet;
      ContentNode dataNode;
      dataNode =
        new BluetoothDataDisplayNode(packet.getData(),packet.getDataTotalLength());
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
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.BLACK + ">");
      }
      if (asHTML) {
        if (packet instanceof DeviceBluetoothSend) {
          sbuf.append("<font " + ColorStyle.PURPLE + ">");
        }
        else {
          sbuf.append("<font " + ColorStyle.DARKBLUE + ">");
        }
      }
      sbuf.append("Content Data: ");
      if (asHTML) {
        sbuf.append("</font>");
      }
      sbuf.append(String.valueOf(packet.getDataTotalLength()));
      sbuf.append(" bytes");
      return sbuf.toString();
    }

    public Icon getIcon() {
        return contentDataIcon;
    }

    private DeviceBluetooth packet;

  } // End of BlutoothDataNode


  /*
   * Inner Class: A tree node for data display.
   */

  static class BluetoothDataDisplayNode
    extends ContentNode
    implements TreeCellRenderer {

    /**
     * Creates a new BluetoothDataDisplayNode.
     * @param fragment the fragment containing the data
     */
    private BluetoothDataDisplayNode(byte[] data, int dataLength) {
      super(false);
      this.data = data;
      this.dataLength = dataLength;
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
    public Component getTreeCellRendererComponent(JTree tree,
                                                  Object value,
                                                  boolean selected,
                                                  boolean expanded,
                                                  boolean leaf,
                                                  int row,
                                                  boolean hasFocus) {
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
        LookAndFeel.installColors(textArea,
                                  "Table.selectedForeground",
                                  "Table.selectedBackground");
      }
      else {
        LookAndFeel.installColors(textArea,
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
      formattedText = toHexString(data);
      return formattedText;
    }

    /**
     * Returns the string representation of a byte array.
     * @param bytes the byte array
     * @return the string representation of a byte array.
     */
    private static String toHexString(byte[] bytes) {
      StringBuffer sbuf = new StringBuffer();
      for (int i = 0; i < bytes.length; i++) {
        if (i > 0) {
          sbuf.append(',');
        }
        if ((i > 0) && ((i % 16) == 0)) {
          sbuf.append('\n');
        }
        sbuf.append(StringUtils.toHexString(bytes[i]));
      }
      return sbuf.toString();
    }

    private byte[] data;
    private int dataLength;

  } // End of BluetoothDataDisplayNode

  /*
   * Inner Class: A tree node for display of destination address.
   */
  static class BluetoothDestinationNode extends ContentNode {
   /**
    * Creates a new BluetoothDestinationNode.
    * @param flag the broadcast flag value
    */
    BluetoothDestinationNode(String destination) {
      super(true);
      this.destination = destination;
    }

    /**
     * Gets the text representation of this node.
     * @return the text representation of this node
     */
    public String getAsText(boolean asHTML) {
      StringBuffer sbuf = new StringBuffer();
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.BLACK + ">");
      }
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.PURPLE + ">");
      }
      sbuf.append("Destination address: ");
      if (asHTML) {
        sbuf.append("</font>");
      }
      sbuf.append (destination);
      return sbuf.toString();
    }

    private String destination;

  } // End of BluetoothDestinationNode


  /*
   * Inner Class: A tree node for display of source address.
   */
  static class BluetoothSourceNode extends ContentNode {
   /**
    * Creates a new BluetoothSourceNode.
    * @param flag the broadcast flag value
    */
    BluetoothSourceNode(String source) {
      super(true);
      this.source = source;
    }

    /**
     * Gets the text representation of this node.
     * @return the text representation of this node
     */
    public String getAsText(boolean asHTML) {
      StringBuffer sbuf = new StringBuffer();
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.BLACK + ">");
      }
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.DARKBLUE + ">");
      }
      sbuf.append("Source address: ");
      if (asHTML) {
        sbuf.append("</font>");
      }
      sbuf.append (source);
      return sbuf.toString();
    }

    private String source;

  } // End of BluetoothSourceNode


/*****   Bluetooth Broadcast flag node  *******/

  /*
   * Inner Class: A tree node for display of broadcast flag value.
   */
  static class BluetoothBroadcastNode extends ContentNode {
   /**
    * Creates a new BluetoothBroadcastNode.
    * @param flag the broadcast flag value
    */
    BluetoothBroadcastNode(int flagValue, boolean send) {
      super(true);
      this.flagValue = flagValue;
      this.send= send;
    }

    /**
     * Gets the text representation of this node.
     * @return the text representation of this node
     */
    public String getAsText(boolean asHTML) {
      String byteText = Integer.toBinaryString(flagValue);
      if (byteText == null) {
        byteText = "null";
      }
      else {
        if (byteText.length() < 2 ) {
          byteText = "0" + byteText;
        }
      }
      String descriptionText = getBroadcastAsDescriptionText(flagValue, send);
      StringBuffer sbuf = new StringBuffer();
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.BLACK + ">");
      }
      if (asHTML) {
        if (send) {
          sbuf.append("<font " + ColorStyle.PURPLE + ">");
        }
        else {
          sbuf.append("<font " + ColorStyle.DARKBLUE + ">");
        }
      }

      sbuf.append("Broadcast flag: ");
      if (asHTML) {
        sbuf.append("</font>");
      }
      sbuf.append (byteText);
      sbuf.append (" ");
      sbuf.append (descriptionText);

      return sbuf.toString();
    }


    /**
     * Gets the meaningful description of this flag's value.
     * @param value the flag value
     * @param send true if this is a send packet, false if it's a receive packet
     * @return the description of this flag's value
     */
    private String getBroadcastAsDescriptionText(int value, boolean send) {
      if (value == DeviceBluetooth.BROADCAST_RESERVED) {
        return RESERVED_TEXT;
      }
      if (send == false) {
        switch (value) {
          case DeviceBluetooth.POINT_TO_POINT:
            return P2P_BROADCAST_TEXT;
          case DeviceBluetooth.SLAVE_BROADCAST:
            return SLAVE_BROADCAST_TEXT;
          case DeviceBluetooth.SLAVE_PARK_BROADCAST:
            return SLAVE_PARK_BROADCAST_TEXT;
          default:
            return INVALID_TEXT;
        }
      }
      else {
        switch (value) {
          case DeviceBluetooth.NO_BROADCAST:
            return NO_BROADCAST_TEXT;
          case DeviceBluetooth.ACTIVE_BROADCAST:
            return ACTIVE_BROADCAST_TEXT;
          case DeviceBluetooth.PICONET_BROADCAST:
            return PICONET_BROADCAST_TEXT;
          default:
            return INVALID_TEXT;
        }
      }
    }

    private int flagValue;
    private boolean send;

  } // End of BluetoothBroadcastNode


/*****   Bluetooth Packet Boundary flag node  *******/

  /*
   * Inner Class: A tree node for display of broadcast flag value.
   */
  static class BluetoothPacketBoundaryNode extends ContentNode {
   /**
    * Creates a new BluetoothPacketBoundaryNode.
    * @param flag the packet boundary flag value
    */
    BluetoothPacketBoundaryNode(int flagValue, boolean send) {
      super(true);
      this.flagValue = flagValue;
      this.send= send;
    }

    /**
     * Gets the text representation of this node.
     * @return the text representation of this node
     */
    public String getAsText(boolean asHTML) {
      String byteText = Integer.toBinaryString(flagValue);
      if (byteText == null) {
        byteText = "null";
      }
      else {
        if (byteText.length() < 2 ) {
          byteText = "0" + byteText;
        }
      }
      String descriptionText = getPacketBoundaryAsDescriptionText(flagValue);
      StringBuffer sbuf = new StringBuffer();
      if (asHTML) {
        sbuf.append("<font " + ColorStyle.BLACK + ">");
      }
      if (asHTML) {
        if (send) {
          sbuf.append("<font " + ColorStyle.PURPLE + ">");
        }
        else {
          sbuf.append("<font " + ColorStyle.DARKBLUE + ">");
        }
      }
      sbuf.append("Packet boundary flag: ");
      if (asHTML) {
        sbuf.append("</font>");
      }
      sbuf.append (byteText);
      sbuf.append (" ");
      sbuf.append (descriptionText);

      return sbuf.toString();
    }

    /**
     * Gets the meaningful description of this flag's value.
     * @return the description of this flag's value
     */
    private String getPacketBoundaryAsDescriptionText(int value) {
      switch (value) {
        case DeviceBluetooth.CONTINUING_PACKET:
          return CONTINUING_PACKET_TEXT;
        case DeviceBluetooth.FIRST_PACKET:
          return FIRST_PACKET_TEXT;
        case DeviceBluetooth.PACKET_RESERVED_00:
          return RESERVED_TEXT;
        case DeviceBluetooth.PACKET_RESERVED_11:
          return RESERVED_TEXT;
        default:
          return INVALID_TEXT;
      }
    }

    private int flagValue;
    private boolean send;

  } // End of BluetoothPacketBoundaryNode

  static private JPanel panel;
  static private JTextArea textArea;

  /** Common Broadcast flag values */
  private static final String RESERVED_BROADCAST_VALUE =   "11";


  /** Send Broadcast flag description text */
  private static final String NO_BROADCAST_TEXT =          "[No Broadcast]";
  private static final String ACTIVE_BROADCAST_TEXT =      "[Active Broadcast]";
  private static final String PICONET_BROADCAST_TEXT =     "[Piconet Broadcast]";

  /** Receive Broadcast flag description text */
  private static final String P2P_BROADCAST_TEXT =         "[Point-to-Point]";
  private static final String SLAVE_BROADCAST_TEXT =       "[Slave]";
  private static final String SLAVE_PARK_BROADCAST_TEXT =  "[Slave in park mode]";


  /** Packet-boundary flag description text */
  private static final String CONTINUING_PACKET_TEXT =     "[Continuing packet]";
  private static final String FIRST_PACKET_TEXT =          "[First packet]";

  /** Common flag text */
  private static final String RESERVED_TEXT =              "[Reserved]";
  private static final String INVALID_TEXT =               "[Invalid]";

}
