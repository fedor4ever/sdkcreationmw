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





package com.nokia.wtk.diagnostics.gui.traffic;

import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import javax.swing.Icon;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceMessageFragment;
import com.nokia.wtk.diagnostics.views.support.ColorStyle;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
/**
 * <p>Title: </p>
 * <p>Description: A tree node for a header list item.</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */

class HeaderListNode extends ContentNode implements Resource {

    static {
        AppEnvironment.addResources(
            "resources/HeaderListNode.properties",
            HeaderListNode.class);
    }
    static private Icon contentHeadersIcon =
        AppEnvironment.getIcon(CONTENT_HEADERS_ICON);

    /**
     * Creates a new HeaderListNode.
     * @param owner the owner of the header information.
     * (Either <code>DeviceContentRequest</code> or <code>DeviceContentReceived</code>)
     */
    HeaderListNode(Object owner) {
        // Create and assign child nodes:
        if (owner instanceof DeviceContentRequest) {
            DeviceContentRequest request = (DeviceContentRequest) owner;
            processHeaders(request.getHeaders());
        } else if (owner instanceof DeviceContentReceived) {
            DeviceContentReceived content = (DeviceContentReceived) owner;
            processHeaders(content.getHeaders());
        } else if (owner instanceof DeviceMessageFragment) {
            DeviceMessageFragment frag = (DeviceMessageFragment) owner;
            Map map = frag.getHeaders();
            Set keys = map.keySet();
            for (Iterator iter = keys.iterator(); iter.hasNext();) {
                Object key = iter.next();
                add(
                    new HeaderNode(
                        frag.getHeaderName(key.toString()),
                        map.get(key).toString()));
            }
        }
    }

    private void processHeaders(String[][] headers) {
        if (headers != null) {
            for (int i = 0; i < headers.length; i++) {
                add(new HeaderNode(headers[i][0], headers[i][1]));
            }
        }
    }
    // Show node as a branch, even if empty:
    public boolean isLeaf() {
        return false;
    }

    /**
     * Gets the text representation of this node.
     * @return the text representation of this node
     */
    public String getAsText(boolean asHTML) {
        StringBuffer sbuf = new StringBuffer("Headers ");
        if (asHTML) {
            sbuf.append("<font " + ColorStyle.GREEN + ">");
            sbuf.append("<i>");
        }
        sbuf.append("(");
        int count = getChildCount();
        if (count > 0) {
            sbuf.append(count);
        } else {
            sbuf.append("none");
        }
        sbuf.append(")");
        if (asHTML) {
            sbuf.append("</i>");
            sbuf.append("</font>");
        }
        return sbuf.toString();
    }

    public Icon getIcon() {
        return contentHeadersIcon;
    }

}
