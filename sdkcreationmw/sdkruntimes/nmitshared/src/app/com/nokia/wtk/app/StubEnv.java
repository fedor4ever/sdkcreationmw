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



package com.nokia.wtk.app;

/**
 * Fake AppEnvironment for unit testing of component classes.
 *    Ensures calls to AppEnv services are not confused.
 *
 * Usage:
 * class Foo {
 *    public static void main(String[] args) {
 *        StubEnv appenv = new StubEnv();
 *        ...
 *    }
 * }
 *
 */
public class StubEnv extends AppEnvironment {
    /*
     * constructor
     */
    public StubEnv() {
        appEnvironmentImplementation = this;
    }
    /*
     * Only required method.
     */
    public ApplicationWindow getMainWindow() {
            return null;
    }
}
