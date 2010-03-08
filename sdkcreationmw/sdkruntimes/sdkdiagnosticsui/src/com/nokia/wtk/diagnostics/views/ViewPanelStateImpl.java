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

/**
 * <p>Title: ViewPanelStateImpl</p>
 * <p>Description: An implementation of ViewState. </p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
class ViewPanelStateImpl implements ViewsManager.ViewState {

    private boolean flexible_ = true;

    private final ViewPanel comp_;

    public ViewPanelStateImpl(ViewPanel comp, ViewsManager stack) {
        comp_ = comp;
    }

    public ViewPanel getViewPanel() {
        return comp_;
    }

    /**
     * Tell the SliderPanel whether the user should be able to resize
     * the component in the SliderPanel.
     * @param v if true, the SliderPanel will show appropriate resizing
     * controls, and inform the StackedSliderPanel to allow resizing.
     * If false, no controls will be shown and the SliderPanel will not
     * allow resizing
     */
    public void setFlexible(boolean v) {
        flexible_ = v;
        setSliderVisible(v);
    }

    /**
     * Findf out whether the user can resize the component inside the
     * SliderPanel
     * @return whether the SliderPanel will allow resizing
     */
    public boolean isFlexible() {
        return flexible_;
    }

    void setSliderVisible(boolean view) {
        if (view) {
            comp_.getSlider().setEnabled(true);
        } else if (this.isSliderVisible()) {
            comp_.getSlider().setEnabled(false);
        }
    }

    boolean isSliderVisible() {
        return comp_.getSlider().isEnabled();
    }
}
