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



package com.nokia.wtk.bookmarks;

  /**
   * This interface specifies calls to examine and load Strings that could
   * be URLs contained in a bookmark. It is the contract that any entity wishing
   * to use the Bookmark class needs to implement.
   */
  public interface BookmarkLoader {
      /**
       * Is this request made with a full schema? Needs to be implemented by any
       * class wanting to use bookmarks.
       * @param location the location to answer the query about
       * @retunr true if param location is a URL with a schema, false otherwise
       */
      public boolean isSchemaRequest(String loaction);

      /**
       * Request to load the location. Needs to be implemented by any
       * class wanting to use bookmarks.
       * @param location the location to load. Will be a URL with a schema, and
       * a file, and a host
       */
      public void loadContent(String location);

      /**
       * Tests if bookmark loader supports load operation.
       * @returns <code>true</code> if loader supports load operation;
       * <code>false</code> othrewise.
       */
      public boolean isLoadSupported();

      /**
       * Gets the current URI location of the load operation.
       * @returns the URI String of the current load operation; returns
       * <code>null</code> if no URI present.
       */
      public String getLocation();
  }
