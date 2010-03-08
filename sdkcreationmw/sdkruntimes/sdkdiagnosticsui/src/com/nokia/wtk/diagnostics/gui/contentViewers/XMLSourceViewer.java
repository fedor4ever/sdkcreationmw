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





package com.nokia.wtk.diagnostics.gui.contentViewers;

import java.io.UnsupportedEncodingException;
import javax.swing.tree.TreeModel;

import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.util.MimeType;

/**
 * ContentViewer for XML source.
 */
public class XMLSourceViewer implements ContentViewer {

  /**
   * Constructor.
   * @param content the content this XMLSourceViewer represents.
   */
  public XMLSourceViewer( DeviceContentReceived content ) {
      this.content = content;

      String encoding = content.getEncoding();
      if ( encoding == null || encoding.length() < 1 ) {
        encoding = DEFAULT_ENCODING;
      }


      if ( encoding.equals( "UCS-2" ) ) {
          encoding = "UTF-16";
      }

      try {
          source = new String( content.getData(), encoding );
      } catch ( UnsupportedEncodingException e ) {
          throw new IllegalArgumentException( encoding );
      }

      createStatsString();
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getMimeType()
   */
  public String getMimeType() {
      return MimeType.TEXT_XHTML;
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getBinaryMimeType()
   */
  public String getBinaryMimeType() {
      return null;
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getURL()
   */
  public String getURL() {
      return content.getLocation();
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#setURL(java.lang.String)
   */
  public void setURL(String newURL ) {
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getStats()
   */
  public String getStats() {
    return stats;
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getTreeModel()
   */
  public TreeModel getTreeModel() {
    return null;
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#isBinary()
   */
  public boolean isBinary() {
    return false;
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getByteCode()
   */
  public byte[] getByteCode() {
    return null;
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getDecodedContent()
   */
  public String getDecodedContent() {
    return null;
  }

  /**
   * @see com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewer#getOriginalSource()
   */
  public String getOriginalSource() {
    return source;
  }

  private void createStatsString() {
      StringBuffer buf = new StringBuffer();
      buf.append( source.length() );
      buf.append( " " );
      buf.append( SOURCE_UNITS );
      buf.append( ", " );
      buf.append( content.getEncoding() );

      stats = buf.toString();
  }

  private static final String SOURCE_UNITS = "source characters";

  private String source;
  private String stats;
  private DeviceContentReceived content;

  static private final String DEFAULT_ENCODING = "UTF-8";

}
