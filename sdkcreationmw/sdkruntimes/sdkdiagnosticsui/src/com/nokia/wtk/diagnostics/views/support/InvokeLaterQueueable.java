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


package com.nokia.wtk.diagnostics.views.support;

/**
 * Title:        InvokeLaterQueueable
 *
 * Description:  This class is built to deal with the fact that when quickly refreshing
 * and redirecting during stress tests, a big queue of identical Runnables
 * gets built up at the SwingUtilities.invokeLater() queue. Objects from
 * classes derived of this class will only allow the latest instantiation.
 *
 * Example:
 * Suppose a lot of refreshes can be queued based on some action,
 * but only the last queued refresh need be executed. Implement a
 * InvokeLaterQueueable like this:
 * <code>
 *  private static
 *  class   Refresher
 *  extends InvokeLaterQueueable {
 *      protected void doRun() {
 *          // do the refresh thing
 *     }
 *  }
 * </code>
 *  and attach it to whatever keeps notifying a refresh may be
 *  in order like this:
 *
 * <code>
 *  final private static Refresher refresher = new Refresher();
 *
 *   // this gets called often, only latest call will be executed when
 *   // Swing thread has the time
 *   refresher.queue();
 *
 * </code>
 *
 * Company:      Nokia
 * @version 1.0
 */

/* javax.swing */
import javax.swing.SwingUtilities;

public abstract class InvokeLaterQueueable implements Runnable {

    protected boolean go = false;

    synchronized public final void queue() {
        if (!go) {   // first available, so start queue
            SwingUtilities.invokeLater(this);
            go = true;
        }
    }

    public final void run() {
        synchronized (this) {
            go = false;
        }
        doRun();
    }

   /**
    * Implement this with the actual action that needs to be undertaken
    */
   protected abstract void doRun();
}
