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



package com.nokia.wtk.util.gui;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/* java.awt */
import java.awt.BorderLayout;

/* javax.swing */
import javax.swing.JPanel;
import javax.swing.JComponent;
import javax.swing.JTabbedPane;

/* javax.swing.border */
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;

/**
 * A convenient subclass of {@link JTabbedPane} that simplifies adding
 * property pages to the tabbed pane. Also takes care of updating the
 * tab labels, tooltips or icons if they change
 *
 */
public class PropSheet extends JTabbedPane {

    /** The border around the page component */
    private static final Border PAGE_BORDER =
                new EmptyBorder(GUI.GAP,GUI.GAP,GUI.GAP,GUI.GAP);

    /**
     * Creates empty <code>PropertySheet</code>
     */
    public PropSheet() {
    }

    /**
     * Creates <code>PropertySheet</code> filled with property pages
     *
     * @param pages the property pages to initialize the dialog with.
     */
    public PropSheet(PropPage[] pages) {
        addPages(pages);
    }

    /**
     * Adds a number of {@link PropPage}s to the tabbed pane.
     *
     * @param pages the pages to add
     */
    public void addPages(PropPage[] pages) {
        if (pages != null) {
            for (int i=0; i<pages.length; i++) {
                addPage(pages[i]);
            }
        }
    }

    /**
     * Adds a single {@link PropPage} to the tabbed pane.
     *
     * @param page the page to add
     * @return the actual tab component added to the tabbed pane
     */
    public JComponent addPage(PropPage page) {
        PagePanel panel = new PagePanel(page);
        addTab(page.getLabel(), page.getIcon(), panel, page.getToolTip());
        setEnabledAt(indexOfComponent(panel), page.isEnabled());
        return panel;
    }

    /**
     * Gets the {@link PropPage} at the specified index. Note that
     * <code>getComponent(index)</code> would not do it.
     *
     * @param index the index of the page
     * @return the {@link PropPage} at the specified index.
     */
    public PropPage getPageAt(int index)  {
        PagePanel pagePanel = (PagePanel)getComponentAt(index);
        return pagePanel.getPage();
    }

    //=======================================================================
    //          P A G E
    //=======================================================================

    /**
     * This is the component which is actually added to the tabbed pane.
     * It contains the property page
     */
    private class PagePanel extends JPanel implements PropertyChangeListener {
        private PropPage page;

        PagePanel(PropPage page) {
            super(new BorderLayout());
            this.page = page;
            this.setBorder(PAGE_BORDER);
            this.add(page.getComponent(), BorderLayout.CENTER);
            page.addPropertyChangeListener(this);
        }

        PropPage getPage() {
            return page;
        }

        int getPageIndex() {
            return indexOfComponent(this);
        }

        public void propertyChange(PropertyChangeEvent evt) {
            String name = evt.getPropertyName();
            if (PropPage.ENABLED_PROPERTY.equals(name)) {
                setEnabledAt(getPageIndex(), page.isEnabled());
            } else if (PropPage.ICON_PROPERTY.equals(name)) {
                setIconAt(getPageIndex(), page.getIcon());
            } else if (PropPage.LABEL_PROPERTY.equals(name)) {
                setTitleAt(getPageIndex(), page.getLabel());
            } else if (PropPage.TOOLTIP_PROPERTY.equals(name)) {
                setToolTipTextAt(getPageIndex(), page.getToolTip());
            }
        }
    }
}
