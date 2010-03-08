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

import java.util.Map;
import java.util.Hashtable;
import javax.swing.SwingUtilities;

import com.nokia.wtk.diagnostics.browser.AttributedContent;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.ContentLog;
import com.nokia.wtk.diagnostics.browser.PushMessages;
import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceMessage;
import com.nokia.wtk.device.DeviceMessageFragment;
import com.nokia.wtk.device.DeviceMessageListener;
import com.nokia.wtk.device.DeviceBluetoothListener;
import com.nokia.wtk.device.DeviceBluetoothSend;
import com.nokia.wtk.device.DeviceBluetoothReceive;
import com.nokia.wtk.device.DeviceSocket;
import com.nokia.wtk.device.DeviceSocketListener;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
import com.nokia.wtk.util.MessageWriters;

/**
 * A central class that maintains the amalgamated traffic/push model. If
 * views call this as soon as they are instantiated, there will be a good
 * model availabel from the start
 *
 * (Justification: with what little info we get about the received content
 * from wtkdev (no timestamps, for example) it is very hard to make the
 * amalgamated traffic model after the phone has been started for a while,
 * because there is no algorithm to intersperse the push and traffic nodes
 * properly after the fact. So this model has to be created as soon as a
 * summary or extended view is created.
 */
class CentralTrafficSynchronizer implements MessageWriters {

    private Browser b__;
    private Hashtable pushKeyToNode_ = new Hashtable();

    private TrafficModel tmodel__;

    // Max number of content entries
    private Map trafficKeyToNode_ = new Hashtable();

    // streaming:  hashtable that holds associations between transaction IDs and
    // tree nodes
    private Map transIdToNode_ = new Hashtable();

    private boolean supportsSMS_;
    private boolean supportsTCPUDP_;
    private boolean supportsBluetooth_;

    CentralTrafficSynchronizer(Browser b) {
        b__ = b;
        tmodel__ = new TrafficModel();

        b.getContentLog().addContentListListener(new ContentListener());
        b.getPushMessages().addLoggedMessageListener(new PushListener());

        // Register directly for SMS fragments and messages, bypass browser
        // model
        supportsSMS_ = b.registerHostService(
            DeviceMessageListener.class,
            new SMSListener());

        // Register directly for Bluetooth events, bypass browser
        // model
        supportsBluetooth_ = b.registerHostService(
            DeviceBluetoothListener.class,
            new BluetoothListener());

        supportsTCPUDP_ = b.registerHostService(
            DeviceSocketListener.class,
            new SocketListener());
    }

    /**
     * Provides access to the Browser object associated with this synchronizer
     * @return the Browser object
     */
    Browser getBrowser() {
        return b__;
    }

    /**
     * Returns the {@link TrafficModel} maintained by this object
     * @return the {@link TrafficModel} maintained by this object
     */
    TrafficModel getTrafficModel() {
        return tmodel__;
    }

    /**
     * Called when all content has been removed from the underlying device
     * traffic model. Currently clears everything, both traffic and push
     */
    private void contentAllRemoved() {
        tmodel__.removeAll();
        trafficKeyToNode_.clear();
        transIdToNode_.clear();
        pushKeyToNode_.clear();
    }

    /**
     * Called when any content has been received. All new content from loader
     * is added in chronological, i.e. we are ignoring the key parameter as
     * positioning -- everything gets attached to the end of the tree -- and
     * use it only for identification of the node.
     *
     * @param content the content to be added.
     * @param key the key associated with the item to be added.
     */
    private void contentReceived(AttributedContent content, String key) {
        if (content != null) {
            // new for streaming
            // get the transaction ID.

            long transactionId = content.getTransactionId();

            // If the value is invalid (-1),just add a new node, and assume
            // that the content is sent all in one chunk, and we do not have
            // to store the transaction ID.
            if (transactionId == DeviceContentReceived.INVALID_TRANS_ID) {
                // key should never be null here. See ContentLog class
                // that fires these notifications
                if (key != null) {
                    ContentNode node = new TrafficTreeNodes.ContentReceivedNode(content,null);
                    trafficKeyToNode_.put(key, node);
                    tmodel__.add(node);
                } else {
                    assertFailed("no key and no id!");
                }
            }

            // we do have a transaction ID.
            // check if the transaction already exists
            else {

                TrafficTreeNodes.ContentReceivedNode node;
                Long id = new Long(transactionId);
                if (key != null) {

                    // create a new top level content node
                    node = new TrafficTreeNodes.ContentReceivedNode(content,id);
                    if (transIdToNode_.put(id, node) != null) {
                        assertFailed("duplicate id");
                    }
                    if (trafficKeyToNode_.put(key, node) != null) {
                        // This should never happen unless someone rewrites
                        // ContentLog.addReceivedContent() method
                        assertFailed("duplicate key");
                    }
                    tmodel__.add(node);

                } else {

                    // This must be an update of an existing node
                    node = (TrafficTreeNodes.ContentReceivedNode)transIdToNode_.get(id);
                    if (node != null) {
                        node.contentChanged();
                        tmodel__.updateNode(node);
                    } else {
                        assertFailed("update of non-existent node");
                    }
                }
            }
        }
    }

    /**
     * This method is always invoked on the GUI thread
     * @param elem the content
     * @param key the content key
     */
    private void contentAdded(Object elem, String key) {
        if (elem instanceof DeviceContentRequest) {
            contentRequested((DeviceContentRequest)elem, key);
        } else if (elem instanceof AttributedContent) {
            contentReceived((AttributedContent)elem, key);
        } else if (elem == null) {
            assertFailed("null element");
        } else {
            assertFailed(elem.getClass().getName());
        }
    }

    /**
     * Called when content has been removed from the underlying device traffic
     * model. Usually because the model was getting too big.
     * @param key     the key associated with the content
     */
    synchronized private void contentRemoved(String key) {
      if (key != null) {
        ContentNode node = (ContentNode) trafficKeyToNode_.get(key);
        if (node != null) { // a node may not exist because the user cleared
          removeFromTrafficModel(node);
          trafficKeyToNode_.remove(key);
          if (node instanceof TrafficTreeNodes.ContentReceivedNode) {
              TrafficTreeNodes.ContentReceivedNode contentReceivedNode =
                  (TrafficTreeNodes.ContentReceivedNode)node;
              Long transId = contentReceivedNode.getTransactionId();
              if (transId != null) {
                  transIdToNode_.remove(transId);
              }
          }
        }
      }
    }

    /**
     * Called when any content has been requested.
     * @param request the content that was requested.
     * @param key the key associated with the item to be added.
     * all new content from loader is added in
     * chronological, i.e. we are ignoring the key parameter as positioning
     * -- everything gets attached to the end of the tree -- and use it only
     * for identification of the node.
     */
    private void contentRequested(DeviceContentRequest request, String key) {
        if (request != null) {
            ContentNode node = new TrafficTreeNodes.ContentRequestNode(request);
            trafficKeyToNode_.put(key, node);
            tmodel__.add(node);
        }
    }

    /**
     * Called when all content has changed in the device push model.
     * @param newContent the content that has changed
     * @param key the key associated with the content
     */
    synchronized private void pushContentChanged(
        AttributedContent newContent,
        String key) {

        ContentNode node = new PushTreeNodes.PushNode(newContent);
        ContentNode oldNode = (ContentNode) pushKeyToNode_.get(key);
        pushKeyToNode_.put(key, node);

        if (oldNode != null) {
            tmodel__.replace(oldNode, node);
        }
    }

    /**
     * Called when any push content has been received.
     * @param content the received content
     * @param key the key associated with the content
     */
    synchronized private void pushContentReceived(AttributedContent content, String key) {
        if (content != null) {
            ContentNode node = new PushTreeNodes.PushNode(content);
            pushKeyToNode_.put(key, node);
            tmodel__.add(node);
        }
    }

    /**
     * Called when content has been removed from the underlying device push
     * model. Usually because the model was getting too big.
     * @param key the key associated with the content
     */
    synchronized private void pushContentRemoved(String key) {
        ContentNode node = (ContentNode)pushKeyToNode_.get(key);

        if (node != null) { // a node may not exist because the user cleared
            removeFromTrafficModel(node);
            pushKeyToNode_.remove(key);
        }
    }

    /**
     * Removes the content node from the traffic model. Also removes all
     * other traffic nodes that were added before this node. Otherwise,
     * the nodes like TCP/UDP don't get removed from the screen when the
     * traffic view overflows.
     *
     * @param node the node to remove.
     */
    private void removeFromTrafficModel(ContentNode node) {
      // double check that the node is not null and it exists in the model.
      if (node != null) {
        int index = tmodel__.indexOf(node);
        if (index > -1) {
          tmodel__.remove(0, index);
        }
      }
    }

    /**
     * Return whether SMS nodes will appear in this traffic model
     * @return boolean
     */
    boolean isSMSSupported() {
        return supportsSMS_;
    }


    /**
     * Return whether Bluetooth nodes will appear in this traffic model
     * @return boolean
     */
    boolean isBluetoothSupported() {
        return supportsBluetooth_;
    }

    /**
     * Return whether TCP/UDP nodes will appear in this traffic model
     * @return boolean
     */
    boolean isTCPUDPSupported() {
        return supportsTCPUDP_;
    }

    /**
     * This method is called when something unexpected happens
     * @param msg the error message
     */
    private static void assertFailed(String msg) {
        if (DetailMsg.isEnabled()) {
            DetailMsg.printStackTrace(new IllegalStateException(msg));
        }
    }

    //=======================================================================
    //             C O N T E N T
    //=======================================================================

    private class ContentListener implements ContentLog.ContentListListener {

        // inner classes used for invokeLater()

        class RemoveAllContentRunnable implements Runnable {
            public void run() {
                contentAllRemoved();
            }
        }

        class RemoveContentRunnable implements Runnable {
            private String myKey;

            RemoveContentRunnable(String key) {
                myKey = key;
            }

            public void run() {
                CentralTrafficSynchronizer.this.contentRemoved(myKey);
            }
        }

        /**
         * Called when all content has been removed from all list models.
         * Usually means a device switch has taken place and log was
         * cleared.
         */
        public void allContentRemoved() {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                contentAllRemoved();
            } else {
                RemoveAllContentRunnable removeAllContent =
                    new RemoveAllContentRunnable();
                SwingUtilities.invokeLater(removeAllContent);
            }
        }

        // the contentAdded() method now takes an additional argument:
        // elem, which is the actual content. Previously, the content was
        // retrieved from the key, but to support HTTP streaming, we now have
        // the case where the content is just an additional chunk to an
        // existing response. In this case, the key is null. So, to make
        // things easier, we pass the content object itself.
        public void contentAdded(int type, Object elem, String key) {
            if (type != ContentLog.CONTENT_REQUESTS_AND_RESPONSES) {
                // We only maintain one tree here
                return;
            }
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                CentralTrafficSynchronizer.this.contentAdded(elem, key);
            } else {
                SwingUtilities.invokeLater(new AddContentRunnable(type,elem,key));
            }
        }

        /**
         * Called when log has become to big internally
         * @param type in which log tree the content was removed
         * @param key the key associated with the element to be removed
         */
        public void contentRemoved(int type, String key) {
            if (type != ContentLog.CONTENT_REQUESTS_AND_RESPONSES) {
                // We only maintain one tree here
                return;
            }
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                CentralTrafficSynchronizer.this.contentRemoved(key);
            } else {
                RemoveContentRunnable removeContent = new RemoveContentRunnable(
                    key);
                SwingUtilities.invokeLater(removeContent);
            }
        }

    }

    class AddContentRunnable implements Runnable {
        private Object myElem;
        private String myKey;

        AddContentRunnable(int type, Object elem, String key) {
            myElem = elem;
            myKey = key;
        }

        public void run() {
            contentAdded(myElem, myKey);
        }
    }

    //=======================================================================
    //             P U S H
    //=======================================================================

    private class PushListener implements PushMessages.LoggedMessageListener {

        // inner classes used for invokeLater()

        class RemoveAllMessagesRunnable implements Runnable {
            public void run() {
                contentAllRemoved();
            }
        }

        class AddMessageRunnable implements Runnable {
            private String myKey;

            AddMessageRunnable(String key) {
                myKey = key;
            }

            public void run() {
                AttributedContent elem =
                    b__.getPushMessages().getLoggedMessageAt(myKey);
                pushContentReceived(elem, myKey);
            }
        }

        class ChangeMessageRunnable implements Runnable {
            private String myKey;

            ChangeMessageRunnable(String key) {
                myKey = key;
            }

            public void run() {
                AttributedContent elem =
                    b__.getPushMessages().getLoggedMessageAt(myKey);
                pushContentChanged(elem, myKey);
            }
        }

        class RemoveMessageRunnable implements Runnable {
            private String myKey;

            RemoveMessageRunnable(String key) {
                myKey = key;
            }

            public void run() {
                pushContentRemoved(myKey);
            }
        }

        /**
         * Called when all messages have been removed from the log.
         * Means a device switch and the log is being cleared.
         */
        public void allMessagesRemoved() {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                contentAllRemoved();
            } else {
                RemoveAllMessagesRunnable removeAllMessages =
                    new RemoveAllMessagesRunnable();
                SwingUtilities.invokeLater(removeAllMessages);
            }
        }

        /**
         * Called when a message has been added to the log.
         * @param key     the key associated with the message
         */
        public void messageAdded(String key) {
            AttributedContent elem =
                b__.getPushMessages().getLoggedMessageAt(key);
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                pushContentReceived(elem, key);
            } else {
                AddMessageRunnable addMessage =
                    new AddMessageRunnable(key);
                SwingUtilities.invokeLater(addMessage);
            }
        }

        /**
         * Called when a message state has been changed.
         * @param key     the key associated with the message
         */
        public void messageChanged(String key) {
            AttributedContent elem =
                b__.getPushMessages().getLoggedMessageAt(key);
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                pushContentChanged(elem, key);
            } else {
                ChangeMessageRunnable changeMessage =
                    new ChangeMessageRunnable(key);
                SwingUtilities.invokeLater(changeMessage);
            }
        }

        /**
         * Called when a message has been removed from the log.
         * Means log has become to big internally
         * @param key     the key associated with the message
         */
        public void messageRemoved(String key) {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                pushContentRemoved(key);
            } else {
                RemoveMessageRunnable removeMessage =
                    new RemoveMessageRunnable(key);
                SwingUtilities.invokeLater(removeMessage);
            }
        }
    }

    //=======================================================================
    //             S M S
    //=======================================================================

    private class SMSListener implements DeviceMessageListener {

        // inner classes for invokeLater()

        private class SMSMessageReceivedRunnable implements Runnable {
            private DeviceMessage myMessage;

            SMSMessageReceivedRunnable(DeviceMessage message) {
                myMessage = message;
            }

            public void run() {
                tmodel__.add(new SMSTreeNodes.SMSNode(myMessage));
            }
        }

        private class SMSMessageSentRunnable implements Runnable {
            private DeviceMessage myMessage;

            SMSMessageSentRunnable(DeviceMessage message) {
                myMessage = message;
            }

            public void run() {
                tmodel__.add(new SMSTreeNodes.SMSNode(myMessage));
            }
        }

        public void fragmentReceived(DeviceMessageFragment fragment) {
        }

        public void fragmentSent(DeviceMessageFragment fragment) {
        }

        public void messageReceived(DeviceMessage message) {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                tmodel__.add(new SMSTreeNodes.SMSNode(message));
            } else {
                SMSMessageReceivedRunnable SMSReceived =
                    new SMSMessageReceivedRunnable(message);
                SwingUtilities.invokeLater(SMSReceived);
            }
        }

        public void messageSent(DeviceMessage message) {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                tmodel__.add(new SMSTreeNodes.SMSNode(message));
            } else {
                SMSMessageSentRunnable SMSSent =
                    new SMSMessageSentRunnable(message);
                SwingUtilities.invokeLater(SMSSent);
            }
        }

    }

    //=======================================================================
    //           Blootooth
    //=======================================================================

    private class BluetoothListener implements DeviceBluetoothListener {

        // inner classes for invokeLater()

        private class BluetoothReceivedRunnable implements Runnable {
            private DeviceBluetoothReceive btData;

            BluetoothReceivedRunnable(DeviceBluetoothReceive data) {
                btData = data;
            }

            public void run() {
                tmodel__.add(new BluetoothTreeNodes.BluetoothReceiveNode(btData));
            }
        }

        private class BluetoothSentRunnable implements Runnable {
            private DeviceBluetoothSend btData;

            BluetoothSentRunnable(DeviceBluetoothSend data) {
                btData = data;
            }

            public void run() {
                tmodel__.add(new BluetoothTreeNodes.BluetoothSendNode(btData));
            }
        }

        public void bluetoothReceived(DeviceBluetoothReceive btData) {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                tmodel__.add(new BluetoothTreeNodes.BluetoothReceiveNode(btData));
            } else {
                BluetoothReceivedRunnable btReceived =
                    new BluetoothReceivedRunnable(btData);
                SwingUtilities.invokeLater(btReceived);
            }
        }

        public void bluetoothSent(DeviceBluetoothSend btData) {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
                tmodel__.add(new BluetoothTreeNodes.BluetoothSendNode(btData));
            } else {
                BluetoothSentRunnable btSent =
                    new BluetoothSentRunnable(btData);
                SwingUtilities.invokeLater(btSent);
            }
        }
    }

    //=======================================================================
    //             S O C K E T S
    //=======================================================================

    private class SocketListener implements DeviceSocketListener {

        // inner classes for invokeLater()

        class TCPUDPSentRunnable implements Runnable {
            private DeviceSocket mySocket;
            private byte[] myPayload;

            TCPUDPSentRunnable(DeviceSocket socket, byte[] payload) {
                mySocket = socket;
                myPayload = payload;
            }

            public void run() {
                tmodel__.add(
                    new TCPUDPTreeNodes.TCPUDPNode(mySocket, myPayload, true));
            }
        }

        class TCPUDPReceivedRunnable implements Runnable {
            private DeviceSocket mySocket;
            private byte[] myPayload;

            TCPUDPReceivedRunnable(DeviceSocket socket, byte[] payload) {
                mySocket = socket;
                myPayload = payload;
            }

            public void run() {
                tmodel__.add(
                    new TCPUDPTreeNodes.TCPUDPNode(mySocket, myPayload, false));
            }
        }

        public void socketCreated(DeviceSocket socket) {}

        public void socketConnected(DeviceSocket socket) {}

        public void socketAccepted(
            DeviceSocket listeningSocket,
            DeviceSocket acceptedSocket) {
        }

        public void socketListening(DeviceSocket socket) {}

        public void socketClosed(DeviceSocket socket) {}

        public void messageSent(DeviceSocket socket, byte[] payload) {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
              tmodel__.add(
                new TCPUDPTreeNodes.TCPUDPNode(socket, payload, true));
            }
            else {
              TCPUDPSentRunnable sentRunnable =
                    new TCPUDPSentRunnable(socket, payload);
                SwingUtilities.invokeLater(sentRunnable);
            }
        }

        public void messageReceived(DeviceSocket socket, byte[] payload) {
            // invoke immediately if the current thread is the UI dispatch
            // thread, otherwise use invokeLater().
            if (SwingUtilities.isEventDispatchThread()) {
              tmodel__.add(
                new TCPUDPTreeNodes.TCPUDPNode(socket, payload, false));
            }
            else {
              TCPUDPReceivedRunnable receivedRunnable =
                    new TCPUDPReceivedRunnable(socket, payload);
                SwingUtilities.invokeLater(receivedRunnable);
            }
        }
    }
}
