/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
 * $Id: XRpcThreadPool.java,v 1.2 2005/08/26 22:45:28 slava Exp $
 */

// ========================================================================
//  Name        : XRpcThreadPool.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.xml.rpc;

/**
 * A thread pool.
 *
 */
public class XRpcThreadPool {

    private static int threadIndex = 0;

    private Object poolLock = new Object();
    private PooledThread [] pool;
    private int poolCount = 0;
    private int activeCount = 0;

    /**
     * Creates <code>XRpcThreadPool</code> of default size
     */
    public XRpcThreadPool() {
        this(5);
    }

    /**
     * Creates <code>XRpcThreadPool</code> of specified size
     * @param n the thread pool size
     */
    public XRpcThreadPool(int n) {
        if (n < 0) throw new IllegalArgumentException("n < 0");
        pool = new PooledThread[n];
    }

    /**
     * Sets the thread pool size
     */
    public void setPoolSize(int n) {
        if (n < 0) throw new IllegalArgumentException("n < 0");
        synchronized (poolLock) {
            if (n != pool.length) {
                PooledThread [] newPool = new PooledThread[n];
                while (poolCount > n) {
                    pool[--poolCount].cancel();
                }
                if (poolCount > 0) {
                    System.arraycopy(pool,0,newPool,0,poolCount);
                }
                pool = newPool;
            }
        }
    }

    /**
     * Returns the current pool size
     */
    public int getPoolSize() {
        synchronized (poolLock) {
            return pool.length;
        }
    }

    /**
     * Runs the specified <code>Runnable</code> object on a separate thread.
     * Returns as soon as the request has been submitted.
     *
     * @param item the work item to be run on another thread
     */
    public void run(Runnable item) {
        PooledThread thread;
        synchronized (poolLock) {
            if (poolCount > 0) {
                thread = pool[--poolCount];
            } else {
                thread = new PooledThread();
            }
            activeCount++;
        }

        thread.setWorkItem(item);
    }

    /**
     * Stops all pooled threads
     */
    public void clear() {
        synchronized (poolLock) {
            while (poolCount > 0) {
                PooledThread thread = pool[--poolCount];
                thread.cancel();
                activeCount++;
            }
        }
    }

    /**
     * Returns the index of the next thread
     */
    private static synchronized int nextThreadIndex() {
        return ++threadIndex;
    }

    /**
     * A thread that can run multiple <code>Runnable</code> items.
     */
    private class PooledThread extends Thread {
        private Runnable item = null;
        private boolean cancel = false;

        PooledThread() {
            super("XRPC-Thread-" + nextThreadIndex());
            start();
        }

        synchronized void setWorkItem(Runnable x) {
            item = x;
            notify();
        }

        synchronized void cancel() {
            cancel = true;
            notify();
        }

        public void run() {
            while (!cancel) {

                // wait for a work item
                synchronized (this) {
                    while (item == null && !cancel) {
                        try { wait(); }
                        catch (Throwable x) { x.printStackTrace(); }
                    }
                }

                if (item != null) {
                    try { item.run(); }
                    catch (Throwable x) { x.printStackTrace(); }
                    finally { item = null; }
                }

                synchronized (poolLock) {
                    activeCount--;
                    if (!cancel) {
                        if (poolCount < pool.length) {
                            pool[poolCount++] = this;
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }

}
