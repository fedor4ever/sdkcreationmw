// ButtonsPanelListener.java
//
// Copyright (c) Symbian Software Ltd 2003 - 2004.  All rights reserved.
//
package com.symbian.tools.j2me.sei.utils;



/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public interface ButtonsPanelListener {

  /**
   * Handle Close button pressed
   */
  public void handleClosePressed();

  /**
   * Handle Revert button pressed
   */
  public void handleRevertPressed();

  /**
   * Handle Apply button pressed
   */
  public void handleApplyPressed();

  /**
   * Handle set-defaults button pressed
   */
  public void handleSetDefaultsPressed();
}