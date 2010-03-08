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



package com.nokia.wtk.diagnostics.gui.log;

import java.awt.Color;
import com.nokia.wtk.util.Trace;
import com.nokia.wtk.diagnostics.views.support.MultiplexingMessageLog;

/**
 *
 * To change this generated comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
class DeviceMessageHandler
    implements com.nokia.wtk.device.DeviceMessageHandler {

    private MessageLogView.WriterLog debugLog;
    private MessageLogView.WriterLog errorLog;
    private MessageLogView.WriterLog warningLog;
    private MessageLogView.WriterLog consoleLog;

    DeviceMessageHandler(MessageLogView log) {
        debugLog = log.getDefaultLog();
        errorLog = log.createStyledLog(Color.red);
        warningLog = log.createStyledLog(Color.blue);
        consoleLog = log.createStyledLog(Color.black);
        MultiplexingMessageLog.DebugLog.addMessageLog(debugLog);
        MultiplexingMessageLog.ErrorLog.addMessageLog(errorLog);
        MultiplexingMessageLog.WarningLog.addMessageLog(warningLog);
        MultiplexingMessageLog.ConsoleLog.addMessageLog(consoleLog);
    }

    /**
     * Checks debug message enable status.
     * @see com.nokia.wtk.device.DeviceMessageHandler#isDebugEnabled()
     * @return              <code>true</code> if debug messages are enabled.
     */
    public boolean isDebugEnabled() {
        return Trace.isVerbose();
    }

    public void debug(String arg0, Throwable arg1) {
      Trace.Debug.println(arg0);
      if (arg1 != null) {
        Trace.Debug.printStackTrace(arg1);
      }
    }

    public void warning(String arg0, Throwable arg1) {
      Trace.Warning.println(arg0);
      if (arg1 != null) {
        Trace.Warning.printStackTrace(arg1);
      }
    }

    public void error(String arg0, Throwable arg1) {
      Trace.Error.println(arg0);
      if (arg1 != null) {
        Trace.Error.printStackTrace(arg1);
      }
    }
    public void console(String arg0, Throwable arg1) {
      Trace.Console.println(arg0);
      if (arg1 != null) {
        Trace.Console.printStackTrace(arg1);
      }
    }
}
