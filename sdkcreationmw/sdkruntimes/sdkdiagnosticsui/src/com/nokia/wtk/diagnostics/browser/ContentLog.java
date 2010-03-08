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





package com.nokia.wtk.diagnostics.browser;


import java.util.Map;
import java.util.Hashtable;
import java.util.EventListener;
import javax.swing.event.EventListenerList;

import com.nokia.wtk.device.DeviceHTTPContentListener;
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceContentReceived;


/**
 * A log of content requests and responses, including cached responses.
 *
 */
public class ContentLog {

    /**
     * Creates a new <code>ContentLog</code>.
     * @param browser the BRowser instance
     */
    ContentLog(Browser browser) {
        this.browser = browser;
        this.requestedContent = new Hashtable();
        this.receivedContent = new Hashtable();
        this.aggregatedContent = new Hashtable();
        this.transactions = new Hashtable();
        setDeviceHTTPContentListenerSupported(browser.registerHostService(
            DeviceHTTPContentListener.class, getDeviceHTTPContentListener()));
    }

    /**
     * Sets the <code>PushMessages</code> instance.
     * @param pushMessages the <code>PushMessages</code> instance.
     */
    void setPushMessages(PushMessages pushMessages) {
        this.pushMessages = pushMessages;
    }

    /**
     * Gets the <code>PushMessages</code> instance.
     * @return the <code>PushMessages</code> instance.
     */
    private PushMessages getPushMessages() {
        return this.pushMessages;
    }

    /**
     * Gets the <code>DeviceHTTPContentListener</code> instance.
     * @return the <code>DeviceHTTPContentListener</code> instance.
     */
    private DeviceHTTPContentListener getDeviceHTTPContentListener() {
        if (this.deviceHTTPContentListener == null) {
            this.deviceHTTPContentListener = new DeviceHTTPContentListener() {

                public void contentRequested(DeviceContentRequest request) {
                    addRequestedContent(request);
                }

                public void contentReceived(DeviceContentReceived content, boolean cacheHit) {
                  addReceivedContent(content, cacheHit);
                }

                public void contentPushed(DeviceContentReceived content) {
                    getPushMessages().addLoggedMessage(content);
                }
            };
        }
        return this.deviceHTTPContentListener;
    }

    /**
     * Sets the support status for the <code>DeviceHTTPContentListener</code> interface.
     * @param supported if <code>true</code>, then the current device supports
     *                  the <code>DeviceHTTPContentListener</code> interface
     */
    private void setDeviceHTTPContentListenerSupported(boolean supported) {
        this.deviceHTTPContentListenerSupported = supported;
    }

    /**
     * Gets the support status for the <code>DeviceHTTPContentListener</code> interface.
     * @return <code>true</code> if the current device supports
     *         the <code>DeviceHTTPContentListener</code> interface
     */
    public boolean isDeviceHTTPContentListenerSupported() {
        return this.deviceHTTPContentListenerSupported;
    }

    /**
     * Removes all content.
     */
    public void removeAll() {
        synchronized (this) {
            this.requestedContent.clear();
            this.receivedContent.clear();
            this.aggregatedContent.clear();
        }
        // Event Notification:
        fireAllContentRemoved();
    }

    /**
     * Called when any content has been requested.
     * @param request   the content that was requested.
     */
    private void addRequestedContent(DeviceContentRequest request) {
        synchronized (this) {
          if (this.aggregatedContent.size() >= AGGREGATED_BUFFER_LIMIT) {
            String itemToRemove = String.valueOf(firstItem);
            Object item = this.aggregatedContent.remove(itemToRemove);
            // Event Notification:
            fireContentRemoved(CONTENT_REQUESTS_AND_RESPONSES, itemToRemove);
            if (item instanceof DeviceContentRequest) {
              this.requestedContent.remove(itemToRemove);
              // Event Notification:
              fireContentRemoved(CONTENT_REQUESTS, itemToRemove);
            } else {
              this.receivedContent.remove(itemToRemove);
              // Event Notification:
              fireContentRemoved(CONTENT_RESPONSES, itemToRemove);
            }
            firstItem++;
          }

          String itemToAdd = String.valueOf(currentItem);

          this.requestedContent.put(itemToAdd, request);
          this.aggregatedContent.put(itemToAdd,request);
          // Event Notification:
          fireContentAdded(CONTENT_REQUESTS, request, itemToAdd);
          fireContentAdded(CONTENT_REQUESTS_AND_RESPONSES, request, itemToAdd);
          currentItem++;
        }
    }

    /**
     * Called when any content has been received as a response to a request.
     * This method should be called for all request responses, including
     * error responses and cached responses.
     * @param content   the content that was received.
     * @param cacheHit  if <code>true</code> then content was retrieved from the cache.
     */
    private void addReceivedContent(DeviceContentReceived content, boolean cacheHit) {
        synchronized (this) {

          boolean newResponse = false;
          Long transId = new Long(content.getTransactionId());

          // determine whether this item has to be added as a new content
          // node, and to the tables. Only new content nodes are added to the
          // cross-reference tables and counted for the purpose of checking if
          // the aggregated content has got too big.
          // If the item transaction id is equal to -1 or this is a new
          // transaction id (does not exist in our list), then this is a new response.
          // If the transaction id already exists in out table, this is an additional
          // chunk of data to an existing response. Just update the content of the
          // data node, do not add to table.

          if (transId.longValue() == DeviceContentReceived.INVALID_TRANS_ID ||
              !(transactions.containsKey(transId))) {
            newResponse = true;
          }

          // if we need to add this content and the buffer size
          // is over the limit, first remove the first item in
          // the buffer. Do it only if 'newResponse' is true (see previous
          // comment): if we don't add, we don't need to remove...
          if (this.aggregatedContent.size() >= AGGREGATED_BUFFER_LIMIT &&
              newResponse == true) {

            String itemToRemove = String.valueOf(firstItem);
            Object item = this.aggregatedContent.remove(itemToRemove);
            if (item instanceof DeviceContentReceived) {
              long firstTransId = ((DeviceContentReceived)item).getTransactionId();
              if (firstTransId != DeviceContentReceived.INVALID_TRANS_ID) {
                transactions.remove(new Long(firstTransId));
              }
            }
            // Event Notification:
            fireContentRemoved(CONTENT_REQUESTS_AND_RESPONSES, itemToRemove);
            if (item instanceof DeviceContentRequest) {
              this.requestedContent.remove(itemToRemove);
              // Event Notification:
              fireContentRemoved(CONTENT_REQUESTS, itemToRemove);
            } else {
              this.receivedContent.remove(itemToRemove);
              // Event Notification:
              fireContentRemoved(CONTENT_RESPONSES, itemToRemove);
            }
            firstItem++;
          }



          // if this is a continuation of an existing response,
          // specify a null key to this item, since we haven't
          // added it to our tables, and the key will not be used
          // by other notified objects
          String itemToAdd = null;
          AttributedContent transactionContent = null;

          // if this is a new transaction, then specify a key and add the
          // object to the tables with that key.
          // Also increment the current item number.
          if (newResponse) {
            itemToAdd = String.valueOf(currentItem);
            // construct an AttributedContent object from the 'content' received
            transactionContent = new AttributedContent(content);

            // find out if this response is from the cache
            DeviceContentRequest request =
              findMostRecentContentRequest(content.getLocation());
            if (request != null) {
                transactionContent.setAttribute(ORIGINAL_REQUEST, request);
            }
            if (cacheHit) {
                transactionContent.setAttribute(IS_CACHED, Boolean.TRUE);
            }
            transactionContent.setAttribute(TRANSACTION_ID, transId);
            this.receivedContent.put(itemToAdd, transactionContent);
            this.aggregatedContent.put(itemToAdd, transactionContent);
            this.transactions.put(transId, transactionContent);
            currentItem++;
          } else {
              transactionContent = (AttributedContent)transactions.get(transId);
              if (transactionContent != null) {
                transactionContent.addDataChunk(content.getData());
                BrowserHistory history = browser.getBrowserHistory();
                int historySize = history.getHistorySize();
                for (int i=0; i<historySize; i++) {
                    AttributedContent document = history.getDocumentAt(i);
                    if (transId.equals(document.getAttribute(TRANSACTION_ID))) {
                        history.fireHistoryUpdated(i);
                        break;
                    }
                  }
              }
          }

          // Event Notification:
          fireContentAdded(CONTENT_RESPONSES, transactionContent, itemToAdd);
          fireContentAdded(CONTENT_REQUESTS_AND_RESPONSES, transactionContent, itemToAdd);
        }
     }

    /**
     * Checks whether two URLs refer to the same content.
     * The URL parameters have already been lower-cased
     * and stripped of any URL reference parts.
     * @param url1 the first URL
     * @param url2 the second URL
     * @return <code>null</code> if the URL strings refer to the same content.
     */
    private boolean isContentReferenceMatch(String url1, String url2) {
        // For file URIs, strip away URI prefix variants:
        if (url1.startsWith("file:") && url2.startsWith("file:")) {

            url1 = url1.substring(5);   // strip away "file:"
            // Remove leading slashes:
            while ((url1.length() > 0) && (url1.charAt(0) == '/')) {
                url1 = url1.substring(1);
            }
            url2 = url2.substring(5);   // strip away "file:"
            // Remove leading slashes:
            while ((url2.length() > 0) && (url2.charAt(0) == '/')) {
                url2 = url2.substring(1);
            }
            //TODO: Could go the extra step of canonicalization, but is there a need?
        }
        // Try for a straight match:
        boolean match = url1.equals(url2);
        if (!match) {
            // Try again after stripping away any argument fields:
            int argIndex = url1.indexOf('?');
            if (argIndex >= 0) {
                url1 = url1.substring(0, argIndex);
            }
            argIndex = url2.indexOf('?');
            if (argIndex >= 0) {
                url2 = url2.substring(0, argIndex);
            }
            match = url1.equals(url2);
        }
        if (!match) {
            /* at this point, the mismatch may be caused by the fact that one of the URLs
               is a partial URL, meaning it contains only the absolute path, without
               the scheme and the host name. So, we strip the scheme and host name and compare
               the rest
            */
            if (url1.startsWith("http://") || url1.startsWith("https://")) {
                if (url1.startsWith("http://")) {
                    url1 = url1.substring(7);   // strip away "http://"
                }
                else {
                    url1 = url1.substring(8);   // strip away "https://"
                }
                int argIndex = url1.indexOf('/');
                if (argIndex >= 0) {
                    url1 = url1.substring(argIndex); // strip away the host name, up to '/'
                }
            }
            if (url2.startsWith("http://") || url2.startsWith("https://")) {
                if (url2.startsWith("http://")) {
                    url2 = url2.substring(7);   // strip away "http://"
                }
                else {
                    url2 = url2.substring(8);   // strip away "https://"
                }
                int argIndex = url2.indexOf('/');
                if (argIndex >= 0) {
                    url2 = url2.substring(argIndex); // strip away the host name, up to '/'
                }
            }
           match = url1.equals(url2);
        }
        return match;
    }

    /**
     * Checks whether two URLs refer to the same content request.
     * The URL parameters have already been lower-cased,
     * but any URL reference or argument parts are considered significant.
     * @param url1 the first URL
     * @param url2 the second URL
     * @return <code>null</code> if the URL strings refer to the same content.
     */
    private boolean isContentRequestMatch(String url1, String url2) {
        // For file URIs, strip away URI prefix variants:
        if (url1.startsWith("file:") && url2.startsWith("file:")) {

            url1 = url1.substring(5);   // strip away "file:"
            // Remove leading slashes:
            while ((url1.length() > 0) && (url1.charAt(0) == '/')) {
                url1 = url1.substring(1);
            }
            url2 = url2.substring(5);   // strip away "file:"
            // Remove leading slashes:
            while ((url2.length() > 0) && (url2.charAt(0) == '/')) {
                url2 = url2.substring(1);
            }
            //TODO: Could go the extra step of canonicalization, but is there a need?
        }

        return url1.equals(url2);
    }

    /**
     * Finds the most recent content item that matches a specified URL.
     * @param url       the request URL of the content to search for.
     * @return          the content corresponding to the URL
     *                  or <code>null</code> if non-existent.
     */
    AttributedContent findMostRecentContentReference(String url) {
      AttributedContent content = null;

      if (url != null) {
        url = url.toLowerCase();
        // Strip off any reference part:
        int refIndex = url.indexOf('#');
        if (refIndex >= 0) {
          url = url.substring(0, refIndex);
        }

        // Search backwards to find most recent match:
        AttributedContent c = null;
        int i = 0;
        for (i = currentItem; i-- > 0; ) {
          c = (AttributedContent)this.receivedContent.get (String.valueOf(i));
          if (c != null) {
            String cURL = c.getLocation();
            if (cURL != null) {
              cURL = cURL.toLowerCase();
              // Strip off any reference part:
              refIndex = cURL.indexOf('#');
              if (refIndex >= 0) {
                cURL = cURL.substring(0, refIndex);
              }
              if (isContentReferenceMatch(cURL, url)) {
                content = c;

                // skip redirects (301 = moved permanently, 302 = moved temporarily)
                while ((c.getRequestStatus(new StringBuffer()) == 302) ||
                       (c.getRequestStatus(new StringBuffer()) == 301)) {
                  boolean foundNext = false;
                  String[][] headers = c.getHeaders();
                  for (int k=0; k<headers.length; k++) {
                    if (headers[k][0].equals("Location")) {
                      url = headers[k][1];
                      for (i++; i<=currentItem; i++) {
                        c = (AttributedContent)receivedContent.get (String.valueOf(i));
                        if (c != null) {
                          cURL = c.getLocation().toLowerCase();
                          refIndex = cURL.indexOf('#');
                          if (refIndex >= 0) {
                            cURL = cURL.substring(0, refIndex);
                          }
                          if (isContentReferenceMatch(cURL, url)) {
                            foundNext = true;
                            content = c;
                            break;
                          }
                        }
                      }
                      break;
                    }
                  }
                  if (!foundNext) break;
                } // while redirects
                break;
              }
            }
          }
        }
        //at this point we have the first response record for the request
      }

      return content;
    }


    /**
     * Finds the most recent content request that matches a specified URL.
     * @param url       the request URL of the request to search for.
     * @return          the content request corresponding to the URL
     *                  or <code>null</code> if non-existent.
     */
    DeviceContentRequest findMostRecentContentRequest(String url) {
        DeviceContentRequest request = null;

        if (url != null) {
            url = url.toLowerCase();
            // Search backwards to find most recent match:
            DeviceContentRequest r = null;

            for (int i = currentItem; i-- > 0; ) {
              r = (DeviceContentRequest)this.requestedContent.get (String.valueOf(i));
              if (r != null) {
                String rURL = r.getLocation().toLowerCase();
                if (isContentReferenceMatch(rURL, url)) {
                    request = r;
                    break;
                }
              }
            }
        }
        return request;
    }

    /**
     * Checks whether a unit of received content is contained in the cache.
     * @param content   a content object to check.
     * @return          <code>true</code> if content is contained in the cache.
     */
    public static boolean isCacheHit(AttributedContent content) {
        boolean cached = false;
        if (content != null) {
            cached = Boolean.TRUE.equals(content.getAttribute(IS_CACHED));
        }
        return cached;
    }

    /**
     * Gets the original request for a unit of received content.
     * @param content   a content object to check.
     * @return          the original content request or <code>null</code>.
     */
    public static DeviceContentRequest getDeviceContentRequest(AttributedContent content) {
        DeviceContentRequest request = null;
        if (content != null) {
            request = (DeviceContentRequest)content.getAttribute(ORIGINAL_REQUEST);
        }
        return request;
    }

    /**
     * Gets the size of a content list model.
     * @param type      the model type, one of:
     *                  <ul>
     *                    <li><code>CONTENT_REQUESTS</code></li>
     *                    <li><code>CONTENT_RESPONSES</code></li>
     *                    <li><code>CONTENT_REQUESTS_AND_RESPONSES</code></li>
     *                  </ul>
     * @return          the number of items in the content list model.
     */
    public int getModelSize(int type) {
        int size = 0;
        switch (type) {
        case CONTENT_REQUESTS:
            size = this.requestedContent.size();
            break;
        case CONTENT_RESPONSES:
            size = this.receivedContent.size();
            break;
        case CONTENT_REQUESTS_AND_RESPONSES:
            size = this.aggregatedContent.size();
            break;
        }
        return size;
    }

    /**
     * Gets an item of list model data at a specified list model location.
     * The list model data may be of type <code>DeviceContentRequest</code>
     * or <code>AttributedContent</code>.
     * @param type      the model type, one of:
     *                  <ul>
     *                    <li><code>CONTENT_REQUESTS</code></li>
     *                    <li><code>CONTENT_RESPONSES</code></li>
     *                    <li><code>CONTENT_REQUESTS_AND_RESPONSES</code></li>
     *                  </ul>
     * @param key       the item key.
     * @return          the list model data, or <code>null</code> if not defined.
     */
    public Object getModelDataAt(int type, String key) {
        Object data = null;
        switch (type) {
          case CONTENT_REQUESTS:
              data = this.requestedContent.get(key);
              break;
          case CONTENT_RESPONSES:
              data = this.receivedContent.get(key);
              break;
          case CONTENT_REQUESTS_AND_RESPONSES:
              data = this.aggregatedContent.get(key);
              break;
        }
        return data;
    }

    /**
     * Adds a listener to the list that's notified each time
     * a change to the content list model occurs.
     * @param l     the <code>ContentListListener</code>
     */
    public void addContentListListener(ContentListListener l) {
        if (this.listenerList == null) {
            this.listenerList = new EventListenerList();
        }
        this.listenerList.add(ContentListListener.class, l);
    }

    /**
     * Removes a listener from the list that's notified each time
     * a change to the content list model occurs.
     * @param l     the <code>ContentListListener</code>
     */
    public void removeContentListListener(ContentListListener l) {
        if (this.listenerList != null) {
            this.listenerList.remove(ContentListListener.class, l);
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param type      the model type, one of:
     *                  <ul>
     *                    <li><code>CONTENT_REQUESTS</code></li>
     *                    <li><code>CONTENT_RESPONSES</code></li>
     *                    <li><code>CONTENT_REQUESTS_AND_RESPONSES</code></li>
     *                  </ul>
     * @param elem    the item added.
     * @param key     the key corresponding to the item added.
     */
    private void fireContentAdded(int type, Object elem, String key) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == ContentListListener.class) {
                  ((ContentListListener)listeners[i+1]).contentAdded(type, elem, key);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param type      the model type, one of:
     *                  <ul>
     *                    <li><code>CONTENT_REQUESTS</code></li>
     *                    <li><code>CONTENT_RESPONSES</code></li>
     *                    <li><code>CONTENT_REQUESTS_AND_RESPONSES</code></li>
     *                  </ul>
     * @param key     the key corresponding to the item added.
     */
    private void fireContentRemoved(int type, String key) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == ContentListListener.class) {
                    ((ContentListListener)listeners[i+1]).contentRemoved(type, key);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     */
    private void fireAllContentRemoved() {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == ContentListListener.class) {
                    ((ContentListListener)listeners[i+1]).allContentRemoved();
                }
            }
        }
    }


    /**
     * A listener for content list model events.
     */
    public interface ContentListListener extends EventListener {
        /**
         * Called when content has been added to a list model.
         *
         * @param type the model type, one of:
         *             <ul>
         *             <li><code>CONTENT_REQUESTS</code></li>
         *             <li><code>CONTENT_RESPONSES</code></li>
         *             <li><code>CONTENT_REQUESTS_AND_RESPONSES</code></li>
         *             </ul>
         * @param elem      the item added.
         * @param key       the item key.
         */
        public void contentAdded(int type, Object elem, String key);

        /**
         * Called when content has been removed from a list model. This
         * removal will only happen in case the log is getting too big to
         * be maintained by the model in this class.
         *
         * @param type the model type, one of:
         *             <ul>
         *             <li><code>CONTENT_REQUESTS</code></li>
         *             <li><code>CONTENT_RESPONSES</code></li>
         *             <li><code>CONTENT_REQUESTS_AND_RESPONSES</code></li>
         *             </ul>
         * @param key       the item key.
         */
         // note (7/26/04)!
         // the contentAdded() method now takes an additional argument:
         // elem, which is the actual content. Previously, the content was
         // retrieved from the key, but to support HTTP streaming, we now have
         // the case where the content is just an additional chunk to an
         // existing response. In this case, the key is null. So, to make
         // things easier (or better: possible), we pass the content object itself.

        public void contentRemoved(int type, String key);

        /**
         * Called when all content has been removed from all list models. This
         * will be called when there is a switch of device and a new log is
         * thus in effect, and the entries from the old device are cleared.
         */
        public void allContentRemoved();
    }   // End of ContentListListener


    /** The attribute name for an "is cached" flag. (type: Boolean) */
    private static final String IS_CACHED = "is_cached";

    /** The attribute name for an "original request" instance. (type: DeviceContentRequest) */
    private static final String ORIGINAL_REQUEST = "original_request";

    /** The attribute name for an "transaction id" value. (type: Long) */
    private static final String TRANSACTION_ID = "transaction_id";

    /** The model identifier for content requests. */
    public static final int CONTENT_REQUESTS                 = 1;
    /** The model identifier for content responses. */
    public static final int CONTENT_RESPONSES                = 2;
    /** The model identifier for content requests and responses. */
    public static final int CONTENT_REQUESTS_AND_RESPONSES   = 3;

    private static final int AGGREGATED_BUFFER_LIMIT = 100;

    private Browser browser;

    private Hashtable requestedContent = null;     // Type = DeviceContentRequest
    private Hashtable receivedContent = null;      // Type = AttributedContent
    private Hashtable aggregatedContent = null;    // Type = DeviceContentRequest (requests),
                                                //        AttributedContent (responses)
    private PushMessages pushMessages = null;
    private boolean deviceHTTPContentListenerSupported = false;
    private DeviceHTTPContentListener deviceHTTPContentListener = null;
    private EventListenerList listenerList;

    // keeps track of the transaction ids
    // used to prevent counting the same transaction more than once
    // in the other tables
    private Map transactions = null;

    // the following static variables hold the key of the first and last
    // content element respectively.
    private static int firstItem= 1;
    private static int currentItem = 1;
}   // End of ContentLog
