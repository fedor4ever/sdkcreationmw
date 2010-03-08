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

import java.awt.Insets;
import java.awt.Container;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.LayoutManager;
import javax.swing.JComponent;

/**
 *
 * A layout manager similar to java.awt.FlowLayout, but arranges components vertically.
 * Components expand horizontally to fill available extra space.
 * Components will not expand vertically to fill extra space.The extra empty
 * vertical space will be allocated at the bottom of the container.
 * Unlike FlowLayout, components will not wrap to form another column if there isn't
 * enough space vertically.
 *
 */


public class DiagWinLayout implements LayoutManager {



// ---------------------------------- Constructors ------------------------------------
/**
* Constructs an instance of DiagWinLayout with a vertical gap of 5 pixels,
*/
  public DiagWinLayout() {
    this(5);
  }

/**
* Constructs a DiagWinLayout instance with the specified vgap
*
* @param vgap An int value indicating the vertical separation of the components
*/
  public DiagWinLayout(int vgap) {
    this.m_vgap = vgap;
  }


// ---------------------------------- Public Methods -------------------------------

/**
 * Lays out the container.
 */
  public void layoutContainer(Container parent){
    synchronized(parent.getTreeLock()) {
      //get component count
      int componentCount = parent.getComponentCount();

      // get actual container size
      Dimension parentSize = parent.getSize();
      Insets insets = parent.getInsets();

      int parentWidth = parentSize.width - insets.left + insets.right;
      int parentHeight = parentSize.height - insets.top - insets.bottom;
      int totalComponentsHeight = 0;

      // find out the sum of all visible components heights
      int resizableCount = 0;
      int visibleCount = 0;
      boolean [] resizable = new boolean[componentCount];
      for(int i=0; i < componentCount; i++) {
        Component comp = parent.getComponent(i);
        if (comp.isVisible()) {
          if (visibleCount++ > 0) totalComponentsHeight += m_vgap;
          Dimension compSize = comp.getPreferredSize();
          totalComponentsHeight += compSize.height;
          resizable[i] = false;
          if (comp instanceof ViewPanel) {
            ViewPanel vp = (ViewPanel)comp;
            if (vp.isBigPanelShowing()) {
              resizable[i] = true;
              resizableCount++;
            }
          }
        }
      }

      if (visibleCount > 0) {

        // layout the components
        int x = insets.left;
        if (resizableCount == 0 || totalComponentsHeight == parentHeight) {

          // simple case - all components retain their preferred height
          // we may only need to fix the width
          int y = insets.top;
          for(int i=0; i < componentCount; i++) {
            Component comp = parent.getComponent(i);
            if (comp.isVisible()) {
              Dimension compPrefSize = comp.getPreferredSize();
              comp.setBounds(x, y, parentWidth, compPrefSize.height);
              y +=  compPrefSize.height + m_vgap;
            }
          }

        } else if (totalComponentsHeight < parentHeight) {

          // distribute extra space between resizable view panels
          int y = insets.top;
          int extraSpace = parentHeight - totalComponentsHeight;
          int delta = extraSpace/resizableCount;
          int lastDelta = delta + (extraSpace % resizableCount);
          int resizableComponentsDone = 0;
          for(int i=0; i < componentCount; i++) {
            Component comp = parent.getComponent(i);
            if (comp.isVisible()) {
              Dimension compPrefSize = comp.getPreferredSize();
              int compHeight = compPrefSize.height;
              if (resizable[i]) {
                resizableComponentsDone++;
                if (resizableComponentsDone == resizableCount) {
                  // the last resizable component
                  compHeight += lastDelta;
                } else {
                  compHeight += delta;
                }
                ViewPanel vp = (ViewPanel)comp;
                JComponent panel = vp.getDisclosurePanel().getHiddenPanel();
                Dimension panelPrefSize = panel.getPreferredSize();
                int h = panelPrefSize.height;
                int dx = compPrefSize.width - panelPrefSize.width;
                int dy = compPrefSize.height - panelPrefSize.height;
                if ((panelPrefSize.width + dx) != parentWidth ||
                    (panelPrefSize.height + dy) != compHeight) {
                  panelPrefSize.width = Math.max(parentWidth - dx, 0);
                  panelPrefSize.height = Math.max(compHeight - dy, 0);
                  panel.setPreferredSize(panelPrefSize);
                }
              }
              comp.setBounds(x, y, parentWidth, compHeight);
              y += compHeight + m_vgap;
            }
          }

        } else {

          // descrease the size of the resizable components
          int spaceLeft = totalComponentsHeight - parentHeight;
          while (spaceLeft > 0 && resizableCount > 0) {
            int y = insets.top;
            int delta = spaceLeft/resizableCount;
            int lastDelta = delta + (spaceLeft % resizableCount);
            int resizableComponentsDone = 0;
            for(int i=0; i < componentCount; i++) {
              Component comp = parent.getComponent(i);
              if (comp.isVisible()) {
                Dimension compPrefSize = comp.getPreferredSize();
                int compHeight = compPrefSize.height;
                if (resizable[i]) {
                  int compPrefHeight = compHeight;
                  resizableComponentsDone++;
                  if (resizableComponentsDone == resizableCount) {
                    // the last resizable component
                    compHeight -= lastDelta;
                  } else {
                    compHeight -= delta;
                  }
                  Dimension min = comp.getMinimumSize();
                  if (compHeight < min.height) {
                    compHeight = min.height;
                  }

                  if (compHeight >= compPrefHeight) {
                    // cannot make this component any smaller
                    resizable[i] = false;
                    resizableCount--;
                  }

                  spaceLeft -= (compPrefHeight - compHeight);
                  ViewPanel vp = (ViewPanel)comp;
                  JComponent panel = vp.getDisclosurePanel().getHiddenPanel();
                  Dimension panelPrefSize = panel.getPreferredSize();
                  int h = panelPrefSize.height;
                  int dx = compPrefSize.width - panelPrefSize.width;
                  int dy = compPrefSize.height - panelPrefSize.height;
                  if ((panelPrefSize.width + dx) != parentWidth ||
                      (panelPrefSize.height + dy) != compHeight) {
                    panelPrefSize.width = Math.max(parentWidth - dx, 0);
                    panelPrefSize.height = Math.max(compHeight - dy, 0);
                    panel.setPreferredSize(panelPrefSize);
                  }
                }
                comp.setBounds(x, y, parentWidth, compHeight);
                y += compHeight + m_vgap;
              }
            }
          }
        }
      }
    }
  }


 /**
  * Returns the minimum dimensions needed to layout the
  * visible components contained in the specified container.
  *
  * @param parent the component which needs to be laid out
  *
  * @return    the minimum dimensions to lay out the
  *            subcomponents of the specified container
  */
  public Dimension minimumLayoutSize(Container parent) {
    return layoutSize(parent,true);
  }

 /**
  * Returns the preferred dimensions  to layout the
  * visible components contained in the specified container.
  *
  * @param parent the component which needs to be laid out
  *
  * @return    the preferrd dimensions to lay out the
  *            subcomponents of the specified container
  */
  public Dimension preferredLayoutSize(Container parent) {
    return layoutSize(parent,false);
  }

/**
 * Not used by this class */
  public void addLayoutComponent(String name,Component comp){}

/**
 * Not used by this class
 */
  public void removeLayoutComponent(Component comp){}

  public String toString() {
    return getClass().getName() + "[vgap="+ m_vgap + "]";
 }


// ---------------------------------- Private Members ----------------------------

/**
 * Returns the container preferred size, where the width is equal to the width
 * of the widest component, and the height is the sum of all the components heights
 * + top and bottom insets.
 *
 * @param parent the parent (container) object
 * @param minimum specifies whether to return the minimum or preferred size
 *
 * @return the container preferred/minimum size
 */
  private Dimension layoutSize (Container parent, boolean minimum) {
    Dimension parentDim = new Dimension(0,0);
    Dimension componentDim;

    synchronized(parent.getTreeLock()) {
      int componentCount = parent.getComponentCount();
      for(int i=0; i < componentCount; i++){
        Component comp = parent.getComponent(i);
        if(comp.isVisible()){
          componentDim = minimum ? comp.getMinimumSize() : comp.getPreferredSize();
          parentDim.width = Math.max(parentDim.width, componentDim.width);
          parentDim.height += componentDim.height;
          if(i > 0) {
            parentDim.height += m_vgap;
          }
        }
      }
    }
    Insets insets = parent.getInsets();
    parentDim.width += insets.left + insets.right;
    parentDim.height += insets.top + insets.bottom;
    if (parentDim.height < 5) parentDim.height = 5;
    return parentDim;
  }


//the vertical gap between components, defaults to 5
  private int m_vgap = 5;

}
