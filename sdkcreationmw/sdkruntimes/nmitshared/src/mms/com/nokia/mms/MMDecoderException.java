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


package com.nokia.mms;

/**
 * Thrown when an error occurs decoding a buffer representing a
 * Multimedia Message
 *
 */

public class MMDecoderException extends Exception {

  public MMDecoderException(String errormsg) {
    super(errormsg);
  }

  }


