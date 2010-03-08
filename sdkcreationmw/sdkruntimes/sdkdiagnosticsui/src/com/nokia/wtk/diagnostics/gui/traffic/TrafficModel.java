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

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.List;

import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeModel;
import com.nokia.wtk.diagnostics.browser.AttributedContent;
import com.nokia.wtk.diagnostics.views.LogViewListener;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
import com.nokia.wtk.device.DeviceSocket;
import com.nokia.wtk.device.DeviceMessageFragment;
import com.nokia.wtk.device.DeviceBluetoothSend;
import com.nokia.wtk.device.DeviceBluetoothReceive;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.Trace;

/**
 *
 * This class has easy calls for adding incoming data to a (merged) model,
 * and also manages a TreeModel for use in JTrees. The result is that this
 * calss can be every efficient at the whole adding and managing, while
 * re-using as much of the tree classes from Swing as possible.
 */
class TrafficModel implements ContentModel, LogViewListener {

    private List model_;
    private DefaultTreeModel treeModel_;
    private MutableTreeNode root_;
    private Filter filter_;
    private PropertyChangeListener filterListener_;
    private int httpReqCount_;
    private int httpRespCount_;
    private int tcpSentCount_;
    private int tcpRecvdCount_;
    private int udpSentCount_;
    private int udpRecvdCount_;
    private int smsSentCount_;
    private int smsRecvdCount_;
    private int bluetoothSentCount_;
    private int bluetoothRecvdCount_;
    private int pushedCount_;
    private List trafficCountListeners_;
    private PropertySet properties;

    private boolean requestEnabled = true;
    private boolean responseEnabled = true;
    private boolean pushEnabled = true;
    private boolean smsSentEnabled = true;
    private boolean smsReceivedEnabled = true;
    private boolean bluetoothSentEnabled = true;
    private boolean bluetoothReceivedEnabled = true;
    private boolean tcpudpSentEnabled = true;
    private boolean tcpudpReceivedEnabled = true;

    private final static String FILTERS_PROPSET_NAME = "trafficfilters";

    TrafficModel() {
        filter_ = new Filter();
        filter_.addPropertyChangeListener(getFilterListener());
        properties = new PropertySet();
        properties.addProperties(FILTERS_PROPSET_NAME,filter_.getProperties());
    }

    /**
     * Returns the traffic model properties
     * @return the traffic model properties
     */
    PropertySet getProperties() {
        return properties;
    }

    /**
     * Appends an element and fires the corresponding change notification.
     * Convenience call since all the views do a lot of adding.
     * @param node the source element
     * (Either <code>DeviceContentRequest</code> or
     * <code>DeviceContentReceived</code>)
     */
    public synchronized void add(ContentNode node) {
        if (node != null) {
            getModel().add(node);
            // update associated counter
            // we add to counter even if not filtered out
            increaseCounter(node);
            reportFilteredOutTraffic(node);
            if (isTreeModelStarted() && (!getFilter().isFilteredOut(node))) {
                getDefaultTreeModel().insertNodeInto(
                    node,
                    getRoot(),
                    getRoot().getChildCount());
                    if (getDefaultTreeModel().getIndexOfChild (getRoot(), node) == 0) {
                      getDefaultTreeModel().nodeStructureChanged(getRoot());
                    }
            }
        }
    }

    /**
     * Gets the number of contained elements.
     * @return the number of contained elements.
     */
    public synchronized int getSize() {
        return getModel().size();
    }

    /**
     * Updates the specified node.
     * @param node the node to update
     */
    synchronized void updateNode(DefaultMutableTreeNode node) {
      // notify listeners that the node content has changed
      getDefaultTreeModel().nodeChanged(node);
    }


    /**
     * Inserts an element and fires the corresponding change notification in
     * the tree.
     * @param node the source element
     * @param index the index of the element to insert
     */
    public synchronized void insert(ContentNode node, int index) {
        if (node != null) {
            if (index == getSize()) {
                add((ContentNode) node);
                return;
            }
            getModel().add(index, node);
            // we add to counter even if not filtered out
            increaseCounter(node);
            reportFilteredOutTraffic(node);

            if (isTreeModelStarted() && (!getFilter().isFilteredOut(node))) {
                // find the non-filtered node that would come after the one
                // we inserted
                ContentNode nextNodeInFilter;
                int position = -1;
                int i = index + 1;
                int size = getSize();
                while ((position == -1) && (i < size)) {
                    nextNodeInFilter = (ContentNode) getModel().get(i);
                    if (!getFilter().isFilteredOut(nextNodeInFilter)) {
                        position = getRoot().getIndex(nextNodeInFilter);
                    }
                    i++;
                }
                if (position == -1) {
                    position = getRoot().getChildCount();
                }
                getDefaultTreeModel().insertNodeInto(node, getRoot(), position);
                if (getDefaultTreeModel().getIndexOfChild (getRoot(), node) == 0) {
                  getDefaultTreeModel().nodeStructureChanged(getRoot());
                }
            }
        }
    }

    /**
     * Gets the index of the specified content node
     * @param node the node for which to get the index
     * @return the index of the node, or <code>-1</code> if the node is not
     * present in this model
     */
    int indexOf(ContentNode node) {
        return getModel().indexOf(node);
    }

    /**
     * Removes an element from the model and propagates it to the tree
     * @param node the element to remove
     */
    public synchronized void remove(ContentNode node) {
        getModel().remove(node);
        // see comments in method 'remove(int firstIndex, int lastIndex)'. Same
        // comments apply here.
        if (isTreeModelStarted() && (!getFilter().isFilteredOut(node))) {
          if (node.getParent() != null) {
            getDefaultTreeModel().removeNodeFromParent(node);
          }
          //debug
          /*
          else {
            System.err.println ("in TrafficModel.remove(), node parent is null!");
          }
          */
        }
    }

    /**
     * Removes a number of elements from the model and propagates it to
     * the tree
     * @param firstIndex the index of the first element to remove (inclusive)
     * @param lastIndex the index of the last element to remove (inclusive)
     */
    void remove(int firstIndex, int lastIndex) {
        if (firstIndex > lastIndex) {
            throw new IllegalArgumentException("firstIndex > lastIndex");
        }

        List model = getModel();
        DefaultTreeModel tree = getDefaultTreeModel();

        for (int i=lastIndex; i>=firstIndex; i--) {
            ContentNode node = (ContentNode) model.remove(i);
            // we need to put back the second condition, if the node is filtered
            // out, we will get (and we do!) an IllegalArgumentException in the
            // next statement (removeNodeFromParent), complaining that the node
            // has no parent. In fact, the node was never put in the tree, or removed
            // from it at some point, if that traffic type was filtered out. Therefor,
            // we do need to check for that
            if (isTreeModelStarted() && (!getFilter().isFilteredOut(node))) {
               // when running the JSR75 TCK, we often get an IllegalArgumentException
               // here, complaining that the node has no parent (see comment above).
               // Even though checking that the node is not filtered out should be
               // sufficient, we will also check that the parent is not null.
              if (node.getParent() != null) {
                tree.removeNodeFromParent(node);
              }
              //debug
              /*
              else {
                System.err.println ("in TrafficModel.remove(), node parent is null!");
              }
              */
            }
        }
    }

    /**
     * Removes an element from the model and propagates it to the tree
     * @param index the index of the element to remove
     * @return the user object of the removed node or <code>null</code>
     * if non-existent.
     */
    public synchronized Object remove(int index) {
        ContentNode node = (ContentNode) getModel().remove(index);
        // see comments in method 'remove(int firstIndex, int lastIndex)'. Same
        // comments apply here.
        if (isTreeModelStarted() && (!getFilter().isFilteredOut(node))) {
          if (node.getParent() != null) {
            getDefaultTreeModel().removeNodeFromParent(node);
          }
          //debug
          /*
          else {
            System.err.println ("in TrafficModel.remove(), node parent is null!");
          }
          */
        }
        if (node != null) {
            return node.getUserObject();
        }
        return null;
    }

    /**
     * Replace a previous node with a new node. Will update viewing in filtered
     * tree. If oldNode is not found, nothing happens.
     * @param oldNode the node to replace
     * @param node the new node to replace oldNode with.
     */
    public synchronized void replace(ContentNode oldNode, ContentNode node) {
        int pos = getModel().indexOf(oldNode);
        if ((pos == -1) || (node == null)) {
            return;
        }
        remove(oldNode);
        insert(node, pos);
    }

    /**
     * Replace a previous node with a new node. Will update viewing in filtered
     * tree. If oldNode is not found, nothing happens.
     * @param pos the location to replace
     * @param node the new node to replace oldNode with.
     */
    public synchronized void replace(int pos, ContentNode node) {
        if (pos >= getSize()) {
            return;
        }
        ContentNode oldNode = (ContentNode) getModel().get(pos);
        if ((pos == -1) || (node == null)) {
            return;
        }
        remove(oldNode);
        insert(node, pos);
    }

    /**
     * Removes all entries from the model and propagates it to the tree.
     * Will clear root node from the tree and allow currently visible to
     * be garbage collected.
     */
    public synchronized void removeAll() {
        getModel().clear();
        resetCounters();
        resetRoot();    // this will filre to all listeners that the model
        // is empty, so it has to come _after_ the model is cleared
    }

    /**
     * Adds a traffic count listener to the list.
     * @param listener the listener object
     */
    void addTrafficCountListener (TrafficCountListener listener) {

      if (trafficCountListeners_ == null) {
        trafficCountListeners_ = new ArrayList();
      }
      trafficCountListeners_.add(listener);
    }


    /**
     * Removes a traffic count listener from the list.
     * @param listener the listener object
     */
    void removeTrafficCountListener (TrafficCountListener listener) {

      if (trafficCountListeners_ != null) {
        trafficCountListeners_ .remove(listener);
      }
    }

    /**
     * Registers with {@link InterViewCommunicator}.
     * @param comm the communicator to register with
     */
    void bindToCommunicator(InterViewCommunicator comm) {
        comm.addLogViewListener(this);
        setOneTimeLoggingEnabled(true);
    }

    /**
     * Unregisters from {@link InterViewCommunicator}.
     * @param comm the communicator to unregister from
     */
    void unbindFromCommunicator(InterViewCommunicator comm) {
        comm.removeLogViewListener(this);
        setOneTimeLoggingEnabled(false);
    }

    /**
     * Handles a notification that the log view has cleared.
     */
    public void logViewCleared () {
        setOneTimeLoggingEnabled(true);
    }

    /**
     * Enables or disables logging of all filtered traffic messages
     * @param enable <code>true</code> to enable, <code>false</code> to disable
     */
    private synchronized void setOneTimeLoggingEnabled(boolean enable) {
        requestEnabled = enable;
        responseEnabled = enable;
        pushEnabled = enable;
        smsSentEnabled = enable;
        smsReceivedEnabled = enable;
        bluetoothSentEnabled = enable;
        bluetoothReceivedEnabled = enable;
        tcpudpSentEnabled = enable;
        tcpudpReceivedEnabled = enable;
    }

    /**
     * Resets the root to an empty new node.
     */
    private void resetRoot() {
        root_ = null;
        getDefaultTreeModel().setRoot(getRoot());
    }

    private boolean isTreeModelStarted() {
        return treeModel_ != null;
    }

    private MutableTreeNode getRoot() {
        if (root_ == null) {
            root_ = new DefaultMutableTreeNode();
        }
        return root_;
    }

    private DefaultTreeModel getDefaultTreeModel() {
        if (treeModel_ == null) {
            treeModel_ = new DefaultTreeModel(getRoot());
            updateFiltered();
        }
        return treeModel_;
    }
    /**
     * Get the tree model that will visualize the data added/ removed from this
     * model. Will call getRoot() to get the root when creating the model.
     * @return TreeModel the treemodel
     */
    synchronized public TreeModel getTreeModel() {
        return getDefaultTreeModel();
    }

    private List getModel() {
        if (model_ == null) {
            model_ = new ArrayList(20);
        }
        return model_;
    }

    private void increaseCounter (ContentNode node) {

      if (node instanceof TrafficTreeNodes.ContentRequestNode) {
        httpReqCount_ += 1;
        notifyTrafficCountListeners();
      }
      else if (node instanceof TrafficTreeNodes.ContentReceivedNode) {
        httpRespCount_ += 1;
        notifyTrafficCountListeners();
      }

      else if (node instanceof TCPUDPTreeNodes.TCPUDPNode) {
        if (node.getUserObject() instanceof DeviceSocket) {
          DeviceSocket socket = (DeviceSocket)node.getUserObject();
          int type= socket.getType();
          if (type == DeviceSocket.TCP_SOCKET) {
            if (((TCPUDPTreeNodes.TCPUDPNode)node).getSent() == true) {
              tcpSentCount_ += ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            else {
              tcpRecvdCount_ += ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            notifyTrafficCountListeners();
          }
          else if (type == DeviceSocket.UDP_SOCKET) {
            if (((TCPUDPTreeNodes.TCPUDPNode)node).getSent() == true) {
              udpSentCount_ += ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            else {
              udpRecvdCount_ += ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            notifyTrafficCountListeners();
          }

        }
      }


      else if (node instanceof SMSTreeNodes.SMSNode) {
        if (node.getUserObject() instanceof DeviceMessageFragment) {
          DeviceMessageFragment fragment = (DeviceMessageFragment)node.getUserObject();
          String direction = fragment.getMessageType();
         // sms sent
          if (direction.equalsIgnoreCase(SMSTreeNodes.SMS_SUBMIT)) {
            smsSentCount_ += fragment.getMessageLength();
          }
          // sms received
          else {
            smsRecvdCount_ += fragment.getMessageLength();
          }
          notifyTrafficCountListeners();
        }
      }

      // bluetooth send
      else if (node instanceof BluetoothTreeNodes.BluetoothSendNode) {
        DeviceBluetoothSend btPacket = (DeviceBluetoothSend)node.getUserObject();
        bluetoothSentCount_ += btPacket.getDataTotalLength();
        notifyTrafficCountListeners();
      }
      // bluetooth received
      else if (node instanceof BluetoothTreeNodes.BluetoothReceiveNode) {
        DeviceBluetoothReceive btPacket = (DeviceBluetoothReceive)node.getUserObject();
        bluetoothRecvdCount_ += btPacket.getDataTotalLength();
        notifyTrafficCountListeners();
      }
      // message pushed
      else if (node instanceof PushTreeNodes.PushNode) {
        // do not count MMS sent, although it is currently
        // represented in a PushNode. The way we know
        // that this is an MMS sent is by looking
        // at the "location" field in the content. This was a quick hack
        // introduced a while back to have a way to indicate wheteher
        // an MMS message was received (pushed) or sent by the SDK.
        // Instead of creating a new data type for MMS traffic,
        // we use the AttributedContent type (already used for other traffic).
        // Since the "location" field has no use for pushed
        // traffic, we use it in MMS to indicate "sent" or "pushed".
        AttributedContent content = (AttributedContent) node.getUserObject();
        // get the traffic category (can be: SI, SL, CO, MMS, Multipart, DR)
        String category = ((PushTreeNodes.PushNode)node).getMessageCategory(content.getType());
        // use a boolean to mark if this message is counted or not
        boolean countIt = true;
        // if category is not MMS, always count the message
        // if category is MMS, look at the "location" field, that indicates "sent" or "pushed",
        // do not count if location == "sent".
        if (category.equals("MMS")) {
          String location = content.getLocation();
          if (location != null && location.equalsIgnoreCase("sent")) {
            countIt = false;
          }
        }
        if (countIt) {
          pushedCount_ += 1;
          notifyTrafficCountListeners();
        }
      }
    }


    private void decreaseCounter (ContentNode node) {

      if (node instanceof TrafficTreeNodes.ContentRequestNode) {
        httpReqCount_ -= 1;
        notifyTrafficCountListeners();
      }
      else if (node instanceof TrafficTreeNodes.ContentReceivedNode) {
        httpRespCount_ -= 1;
        notifyTrafficCountListeners();
      }

      else if (node instanceof TCPUDPTreeNodes.TCPUDPNode) {
        if (node.getUserObject() instanceof DeviceSocket) {
          DeviceSocket socket = (DeviceSocket)node.getUserObject();
          int type= socket.getType();
          if (type == DeviceSocket.TCP_SOCKET) {
            if (((TCPUDPTreeNodes.TCPUDPNode)node).getSent() == true) {
              tcpSentCount_ -= ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            else {
              tcpRecvdCount_ -= ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            notifyTrafficCountListeners();
          }
          else if (type == DeviceSocket.UDP_SOCKET) {
            if (((TCPUDPTreeNodes.TCPUDPNode)node).getSent() == true) {
              udpSentCount_ -= ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            else {
              udpRecvdCount_ -= ((TCPUDPTreeNodes.TCPUDPNode)node).getPayloadLength();
            }
            notifyTrafficCountListeners();
          }
        }
      }

      else if (node instanceof SMSTreeNodes.SMSNode) {
        if (node.getUserObject() instanceof DeviceMessageFragment) {
          DeviceMessageFragment fragment = (DeviceMessageFragment)node.getUserObject();
          String direction = fragment.getMessageType();
          // sms sent
          if (direction.equalsIgnoreCase(SMSTreeNodes.SMS_SUBMIT)) {
            smsSentCount_ -= fragment.getMessageLength();
          }
          // sms received
          else {
            smsRecvdCount_ -= fragment.getMessageLength();
          }
          notifyTrafficCountListeners();
        }
      }


      // bluetooth send
      else if (node instanceof BluetoothTreeNodes.BluetoothSendNode) {
        DeviceBluetoothSend btPacket = (DeviceBluetoothSend)node.getUserObject();
        bluetoothSentCount_ -= btPacket.getDataTotalLength();
       notifyTrafficCountListeners();
      }
      // bluetooth received
      else if (node instanceof BluetoothTreeNodes.BluetoothReceiveNode) {
        DeviceBluetoothReceive btPacket = (DeviceBluetoothReceive)node.getUserObject();
        bluetoothRecvdCount_ -= btPacket.getDataTotalLength();
        notifyTrafficCountListeners();
      }
      // message pushed
      else if (node instanceof PushTreeNodes.PushNode) {
        // do not count MMS sent, although it is currently
        // represented in a PushNode. The way we know
        // that this is an MMS sent is by looking
        // at the "location" field in the content. This was a quick hack
        // introduced a while back to have a way to indicate wheteher
        // an MMS message was received (pushed) or sent by the SDK.
        // Instead of creating a new data type for MMS traffic,
        // we use the AttributedContent type (already used for other traffic).
        // Since the "location" field has no use for pushed
        // traffic, we use it in MMS to indicate "sent" or "pushed".
        AttributedContent content = (AttributedContent) node.getUserObject();
        // get the traffic category (can be: SI, SL, CO, MMS, Multipart, DR)
        String category = ((PushTreeNodes.PushNode)node).getMessageCategory(content.getType());
        // use a boolean to mark if this message is counted or not
        boolean countIt = true;
        // if category is not MMS, always count the message
        // if category is MMS, look at the "location" field, that indicates "sent" or "pushed",
        // do not count if location == "sent".
        if (category.equals("MMS")) {
          String location = content.getLocation();
          if (location != null && location.equalsIgnoreCase("sent")) {
            countIt = false;
          }
        }
        if (countIt) {
          pushedCount_ -= 1;
          notifyTrafficCountListeners();
        }
      }
    }


    private void reportFilteredOutTraffic (ContentNode node) {

      // if traffic is disabled for the 1st time, send a log message

      String text1 = "Warning: Traffic View: Listing of ";
      String text2 = " traffic is set to off (see Monitor)";

      // http request
      if (node instanceof TrafficTreeNodes.ContentRequestNode) {
        if (requestEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.REQUESTS_CHOICE_LABEL) +
                                  text2);
            requestEnabled = false;
        }
      }

      // http received
      else if (node instanceof TrafficTreeNodes.ContentReceivedNode) {
        if (responseEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.RESPONSES_CHOICE_LABEL) +
                                  text2);
            responseEnabled = false;
        }
      } // end http

      // tcp/udp
      else if (node instanceof TCPUDPTreeNodes.TCPUDPNode) {
        if (((TCPUDPTreeNodes.TCPUDPNode)node).getSent()) {
          if (tcpudpSentEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.TCPUDP_SENT_CHOICE_LABEL) +
                                  text2);
            tcpudpSentEnabled = false;
          }
        }
        else {
          if (tcpudpReceivedEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.TCPUDP_RECEIVED_CHOICE_LABEL) +
                                  text2);
            tcpudpReceivedEnabled = false;
          }
        }
      } // end tcp/udp

      // sms
      else if (node instanceof SMSTreeNodes.SMSNode &&
        node.getUserObject() instanceof DeviceMessageFragment) {
        DeviceMessageFragment fragment = (DeviceMessageFragment)node.getUserObject();
        String direction = fragment.getMessageType();
        // sms sent
        if (direction.equalsIgnoreCase(SMSTreeNodes.SMS_SUBMIT)) {
          if (smsSentEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.SMS_SENT_CHOICE_LABEL) +
                                  text2);
            smsSentEnabled = false;
          }
        }
        // sms received
        else {
          if (smsReceivedEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.SMS_RECEIVED_CHOICE_LABEL) +
                                  text2);
            smsReceivedEnabled = false;
          }
        }
      } // end SMS


      // bluetooth send
      else if (node instanceof BluetoothTreeNodes.BluetoothSendNode) {
        if (getFilter().isFilteredOut(node)) {
          if (bluetoothSentEnabled) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.BLUETOOTH_SENT_CHOICE_LABEL) +
                                  text2);
            bluetoothSentEnabled = false;
          }
        }
        else {
          bluetoothSentEnabled = true;
        }
      }
      // bluetooth received
      else if (node instanceof BluetoothTreeNodes.BluetoothReceiveNode) {
        if (bluetoothReceivedEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.BLUETOOTH_RECEIVED_CHOICE_LABEL) +
                                  text2);
            bluetoothReceivedEnabled = false;
        }
      }   // end bluetooth

      // push
      else if (node instanceof PushTreeNodes.PushNode) {
        if (pushEnabled && getFilter().isFilteredOut(node)) {
            Trace.Warning.println(text1 +
                                  AppEnvironment.getString(Resource.PUSH_CHOICE_LABEL) +
                                  text2);
            pushEnabled = false;
        }
      }   // end push
    }

    /**
     * Resets all traffic counters to 0
     */
    synchronized private void resetCounters() {

      httpReqCount_ = 0;
      httpRespCount_ = 0;
      tcpSentCount_ = 0;
      tcpRecvdCount_ = 0;
      udpSentCount_ = 0;
      udpRecvdCount_ = 0;
      smsSentCount_ = 0;
      smsRecvdCount_ = 0;
      bluetoothSentCount_ = 0;
      bluetoothRecvdCount_ = 0;
      pushedCount_ = 0;

      notifyTrafficCountListeners();
    }


    /**
     * Notifies Traffic count listeners about a counter update
     */
    private void notifyTrafficCountListeners() {
      if (trafficCountListeners_ == null || trafficCountListeners_.size() < 1) {
        return;
      }
      TrafficCountListener listener = null;
      for (int i=0; i< trafficCountListeners_.size(); i++) {
          listener = (TrafficCountListener)trafficCountListeners_.get(i);
          listener.trafficCountUpdated();
      }
    }

    /**
     * Get HTTP requests count.
     * @return HTTP requests count
     */
    int getHttpRequestCount() {
      return httpReqCount_;
    }

    /**
     * Get HTTP responses count.
     * @return HTTP responses count
     */
    int getHttpResponseCount() {
      return httpRespCount_;
    }

    /**
     * Get TCP sent count.
     * @return TCP sent count
     */
    int getTcpSentCount() {
      return tcpSentCount_;
    }

    /**
     * Get TCP received count.
     * @return TCP received count
     */
    int getTcpReceivedCount() {
      return tcpRecvdCount_;
    }

    /**
     * Get UDP sent count.
     * @return UDP sent count
     */
    int getUdpSentCount() {
      return udpSentCount_;
    }

    /**
     * Get UDP received count.
     * @return UDP received count
     */
    int getUdpReceivedCount() {
      return udpRecvdCount_;
    }

    /**
     * Get SMS sent count.
     * @return SMS sent count
     */
    int getSmsSentCount() {
      return smsSentCount_;
    }

    /**
     * Get SMS received count.
     * @return SMS received count
     */
    int getSmsReceivedCount() {
      return smsRecvdCount_;
    }

   /**
     * Get Bluetooth sent count.
     * @return Bluetooth sent count
     */
    int getBluetoothSentCount() {
      return bluetoothSentCount_;
    }

    /**
     * Get Bluetooth received count.
     * @return Bluetooth received count
     */
    int getBluetoothReceivedCount() {
      return bluetoothRecvdCount_;
    }

   /**
     * Get Pushed messages count.
     * @return Pushed messages count
     */
    int getPushedCount() {
      return pushedCount_;
    }

    /**
     * Get currently set filter for examination.
     * @return the current filter
     */
    Filter getFilter() {
        return filter_;
    }

    private PropertyChangeListener getFilterListener() {
        if (filterListener_ == null) {
            filterListener_ = new PropertyChangeListener() {
                public void propertyChange(PropertyChangeEvent pe) {
                    updateFiltered();
                }
            };
        }

        return filterListener_;
    }

    private synchronized void updateFiltered() {
        // do not use resetRoot() as that will fire a root event.
        // Make a new roo manually instead
        root_ = null;

        int i = 0;

        for (Iterator e = getModel().iterator(); e.hasNext();) {
            ContentNode tr = (ContentNode) e.next();
            if (!getFilter().isFilteredOut(tr)) {
                getRoot().insert(tr, i);
                tr.setParent(getRoot());
                i++;
            }
        }
        getDefaultTreeModel().setRoot(getRoot());
    }
}
