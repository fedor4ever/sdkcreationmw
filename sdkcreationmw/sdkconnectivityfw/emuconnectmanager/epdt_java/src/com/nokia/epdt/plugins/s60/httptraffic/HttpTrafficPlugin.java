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


package com.nokia.epdt.plugins.s60.httptraffic;

import java.util.Map;
import java.util.HashMap;

import java.awt.Image;
import javax.swing.JPanel;
import javax.swing.JComponent;
import javax.swing.SwingUtilities;

import com.nokia.wtk.util.Trace;
import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.PropertySet;

import com.nokia.wtk.device.DeviceHTTPContentListener;

import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserRequest;
import com.nokia.wtk.diagnostics.browser.BrowserResponse;

import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;

import com.nokia.wtk.diagnostics.gui.traffic.TrafficView;

import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.CoreModel;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageQueue;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;


public class HttpTrafficPlugin extends CustomPlugin
                               implements MessageListener,
                                          AbstractViewFactoryInterface
{
  public static final byte HTTP_REQ_EVT = 0;
  public static final byte HTTP_RES_HDR_EVT = 4;
  public static final byte HTTP_RES_BODY_EVT = 5;
  public static final byte HTTP_RES_COMPLETE_EVT = 6;
  public static final byte HTTP_TRANS_CLOSE_EVT = 3;

  public HttpTrafficPlugin ()
  {
    super(_uid, "HTTP Traffic", PluginType.DIAGNOSTICS);
  }

  public Image getIcon (int type)
  {
    return null;
  }

  public JPanel getJPanelView ()
  {
    return null;
  }

  public void bindMessageInterface (MessageInterface m)
  {
    m.setMessageListener(this, _uid);
    this._messageOutputQueue = m.getMessageOutputQueue();
  }

  public void bindActionInterface (ActionInterface a)
  {
    return;
  }

  public void bindCore (CoreController c)
  {
    _coreModel = c.getModel();
  }


  public void messageReceived (Message message)
  {
    HttpTrafficPluginData  pluginData = new HttpTrafficPluginData (message.getBody());
    byte   eventType = pluginData.getEventType ();
    Integer origTransId = new Integer (pluginData.getTransId ());
    HttpTransaction  trans = (HttpTransaction) _transactions.get(origTransId);

    if (Trace.detailOutOn()) {
        Trace.Debug.println("Symbian sent:");
        Trace.Debug.println(StringUtils.hexDump(message.getBody()));
    }

    if (trans == null) {
      trans = new HttpTransaction (origTransId.intValue(), pluginData.getProcId ());
      _transactions.put (origTransId, trans);
    }

    switch (eventType) {
      case HTTP_REQ_EVT:
        trans.addRequest (pluginData.getReqURL());
        reportHTTPRequest (pluginData);
        break;
      case HTTP_RES_HDR_EVT:
        if (trans.isProcessingResponse()) {
          trans.removeRequest ();
        }
        // no break
      case HTTP_RES_BODY_EVT:
        reportHTTPResponse (pluginData, trans);
        trans.setProcessingResponse(true);
        break;
      case HTTP_RES_COMPLETE_EVT:
        trans.removeRequest ();
        trans.setProcessingResponse(false);
        break;
      case HTTP_TRANS_CLOSE_EVT:
        _transactions.remove (origTransId);
        break;
    }

    if (Trace.Debug.isEnabled()) {
        byte [] data = pluginData.getHTTPBody();
        int bodyLen = (data == null) ? 0 : data.length;
        Trace.Debug.println("==== Event Type: " + pluginData.getEventType() + " ====");
        Trace.Debug.println("Orig trans ID: " + pluginData.getTransId());
        Trace.Debug.println("Uniq trans ID: " + trans.getCurrentTransId());
        Trace.Debug.println("Request URL: " + trans.getCurrentReqURL());
        Trace.Debug.println("Status: " + pluginData.getHTTPStatus());
        Trace.Debug.println("Headers: " + pluginData.getHTTPHeaders());
        Trace.Debug.println("Method: " + pluginData.getMethod());
        Trace.Debug.println("URL: " + pluginData.getReqURL());
        Trace.Debug.println("Body: " + bodyLen + " bytes");
        Trace.Debug.println("<=======");
    }
  }


  private void reportHTTPRequest (HttpTrafficPluginData  reqData)
  {
    if (getHttpListener() != null)
    {
      BrowserRequest  req = new BrowserRequest (reqData.getReqURL (),
                                                reqData.getMethod (),
                                                reqData.getHTTPHeaders (),
                                                reqData.getHTTPBody ());

      // It's better to invoke Diagnostics listeners on the UI thread.
      SwingUtilities.invokeLater(new ContentRequested(req));
    }
    return;
  }


  private void reportHTTPResponse (HttpTrafficPluginData  resData,
                                   HttpTransaction trans)
  {
    if (getHttpListener() != null)
    {
      BrowserResponse  res = new BrowserResponse (trans.getCurrentReqURL (),
                                                  resData.getHTTPStatus (),
                                                  null, /* response URL */
                                                  null, /* ContentType */
                                                  null, /* Encoding */
                                                  resData.getHTTPHeaders (),
                                                  resData.getHTTPBody (),
                                                  false,
                                                  trans.getCurrentTransId ());
      // It's better to invoke Diagnostics listeners on the UI thread.
      SwingUtilities.invokeLater(new ContentReceived(res));
    }
  }

    /**
     * Creates and returns TrafficView. This method should be called after
     * DebugViews object has been created.
     *
     * @return the TrafficView implementation
     */
    private AbstractViewFactoryInterface getTrafficView() {
        if (_trafficView == null) {
            // Try to reuse the Browser instance created by DebugViews
            Browser browser = (Browser)_coreModel.getInstanceOf(Browser.class);
            if (browser == null) {
                browser = new Browser(_coreModel);
                _coreModel.setInstanceOf(Browser.class, browser);
            }
            _trafficView = new TrafficView(browser,
                TrafficView.TRAFFIC_FILTER_HTTP_REQUESTS |
                TrafficView.TRAFFIC_FILTER_HTTP_RESPONSES);
        }
        return _trafficView;
    }

    /**
     * Returns {@link DeviceHTTPContentListener} registered with the
     * {@link CoreDevice}
     *
     * @return the registered listener, <code>null</code> if none.
     */
    private DeviceHTTPContentListener getHttpListener() {
        return (DeviceHTTPContentListener)_coreModel.getInstanceOf(
                DeviceHTTPContentListener.class);
    }

    // ======================================================================
    // AbstractViewFactoryInterface
    // ======================================================================

    /**
     * Returns the string that uniquely identifies the view. It's used as a
     * key for the view's properties
     *
     * @return the view key
     */
    public String getKey() {
        return "httptraffic";
    }

    /**
     * Returns properties of the view
     * @return properties of the view
     */
    public PropertySet getViewProperties() {
        return getTrafficView().getViewProperties();
    }

    /**
     * Returns the elements to show when the pod is opened large to show.
     * @return a ViewAndControlsInterface object that will be shown,
     * both the content panel and the controls, when the user requests
     * full disclosure.
     */
    public ViewAndControlsInterface getPodComponent() {
        return getTrafficView().getPodComponent();
    }

    /**
     * Returns the small component to show.
     * @return a JComponent that will be shown when the user just wishes
     * to see the overview.
     */
    public JComponent getSmallPodComponent() {
        return getTrafficView().getSmallPodComponent();
    }

    /**
     * @return the short display name of the of the component.
     */
    public String getViewTitle() {
        return getTrafficView().getViewTitle();
    }

    /**
     * @return a longer description of the component
     */
    public String getViewDescription() {
        return getTrafficView().getViewDescription();
    }

    /**
     * @return true if the view is expanded by default
     */
    public boolean getDefaultViewOpen() {
        return getTrafficView().getDefaultViewOpen();
    }

    /**
     * @return the default width of the view
     */
    public int getDefaultViewWidth() {
        return getTrafficView().getDefaultViewWidth();
    }


    /**
     * @return the default height of the view
     */
    public int getDefaultViewHeight() {
        return getTrafficView().getDefaultViewHeight();
    }

    /**
     * Registers with {@link InterViewCommunicator} to receive inter-views
     * notifications. This method is invoked when view is being added to
     * diagnostics.
     *
     * @param comm the communicator to register with
     */
    public void bindToCommunicator(InterViewCommunicator comm) {
        getTrafficView().bindToCommunicator(comm);
    }

    /**
     * Unregisters from {@link InterViewCommunicator}. This method is invoked
     * when view is being removed from diagnostics
     *
     * @param comm the communicator to unregister from
     */
    public void unbindFromCommunicator(InterViewCommunicator comm) {
        getTrafficView().unbindFromCommunicator(comm);
    }

    // ======================================================================
    // ContentRequested
    // ======================================================================

    private class ContentRequested implements Runnable {
        private BrowserRequest request;
        ContentRequested(BrowserRequest req) {
            request = req;
        }
        public void run() {
            DeviceHTTPContentListener httpListener = getHttpListener();
            if (httpListener != null) {
                httpListener.contentRequested(request);
            }
        }
    }

    // ======================================================================
    // ContentReceived
    // ======================================================================

    private class ContentReceived implements Runnable {
        private BrowserResponse response;
        ContentReceived(BrowserResponse resp) {
            response = resp;
        }
        public void run() {
            DeviceHTTPContentListener httpListener = getHttpListener();
            if (httpListener != null) {
                httpListener.contentReceived(response, false);
            }
        }
    }

    private static Integer _uid = new Integer (0x10206FF1);
    private MessageQueue _messageOutputQueue;
    private CoreModel _coreModel = null;
    private Map _transactions = new HashMap();
    private AbstractViewFactoryInterface _trafficView;
}
