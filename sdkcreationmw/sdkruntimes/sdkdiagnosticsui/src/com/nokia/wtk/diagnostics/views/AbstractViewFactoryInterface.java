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





package com.nokia.wtk.diagnostics.views;

/* javax.swing */
import javax.swing.JComponent;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.PropertySet;

/**
 * <p>Title: </p>
 * <p>Description: This is a factory that encapsulates all the elements that
 * go into a Pod. Note that calling any method should have
 * the result be made from scratch. For example, calling createComponent()
 * should return a new version of the large component every time, not just
 * the same one. </p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
public interface AbstractViewFactoryInterface {

    /**
     * Returns the string that uniquely identifies the view. It's used as a
     * key for the view's properties
     *
     * @return the view key
     */
    public String getKey();

    /**
     * Returns properties of the view
     * @return properties of the view
     */
    public PropertySet getViewProperties();

    /**
     * Returns the elements to show when the pod is opened large to show.
     * @returns a ViewAndControlsInterface object that will be shown,
     * both the content panel and the controls, when the user requests
     * full disclosure.
     */
    public ViewAndControlsInterface getPodComponent();

    /**
     * Returns the small component to show.
     * @returns a JComponent that will be shown when the user just wishes
     * to see the overview.
     */
    public JComponent getSmallPodComponent();

    /**
     * @returns the short display name of the of the component.
     */
    public String getViewTitle();

    /**
     * @returns a longer description of the component
     */
    public String getViewDescription();

    /**
     * @returns true if the view is expanded by default
     */
    public boolean getDefaultViewOpen();

    /**
     * @returns the default width of the view
     */
    public int getDefaultViewWidth();

    /**
     * @returns the default height of the view
     */
    public int getDefaultViewHeight();

    /**
     * Registers with {@link InterViewCommunicator} to receive inter-views
     * notifications. This method is invoked when view is being added to
     * diagnostics.
     *
     * @param comm the communicator to register with
     */
    public void bindToCommunicator(InterViewCommunicator comm);

    /**
     * Unregisters from {@link InterViewCommunicator}. This method is invoked
     * when view is being removed from diagnostics
     *
     * @param comm the communicator to unregister from
     */
    public void unbindFromCommunicator(InterViewCommunicator comm);
}
