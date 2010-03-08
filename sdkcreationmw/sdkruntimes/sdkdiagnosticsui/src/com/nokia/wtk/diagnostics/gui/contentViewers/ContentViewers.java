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

/* java.util.*/
import java.util.Map;
import java.util.HashMap;

/* javax.swing.tree*/
import javax.swing.tree.TreeModel;

/* com.nokia.wtk.device */
import com.nokia.wtk.device.DeviceContentReceived;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.MimeType;

/* com.nokia.wtk.util.wbxml */
import com.nokia.wtk.util.wbxml.WMLDeck;
import com.nokia.wtk.util.wbxml.DecoderTool;

/**
 * This class is a static map that maps content types into content viewers.
 *
 */
public abstract class ContentViewers {

    /**
     * Maps MIME type strings into {@link Handler} objects
     */
    private static final Map handlerMap = new HashMap(3);

    /**
     * Default handle for binary content
     */
    private static final Handler DEFAULT_BINARY_HANDLER = new BinaryHandler();

    /**
     * Default handle for text content
     */
    private static final Handler DEFAULT_TEXT_HANDLER = new TextHandler();

    static {
        handlerMap.put(MimeType.WML_BINARY, new WMLBinaryHandler());
        handlerMap.put(MimeType.WML_SOURCE, new WMLSourceHandler());
        handlerMap.put(MimeType.TEXT_XHTML, new XMLSourceHandler());
        handlerMap.put(MimeType.TEXT_XML, new XMLSourceHandler());
    }

    /**
     * Disallows instantiation of this class
     */
    private ContentViewers() {
        throw new InternalError("This cannot happen");
    }

    /**
     * Returns appropriate viewer for the given content.
     * @return <code>null</code> if no appropiate viewer is available
     * or input parameter is <code>null</code>.
     */
    public static ContentViewer getViewer(DeviceContentReceived content) {
        ContentViewer viewer = null;
        if (content != null) {
            String type = content.getType();
            if (type != null) {
                try {
                    Handler handler = (Handler)handlerMap.get(type);
                    if (handler != null) {
                        viewer = handler.createViewer(content);
                    } else {
                        if (isTextContentType(type)) {
                            handler = DEFAULT_TEXT_HANDLER;
                        } else {
                            handler = DEFAULT_BINARY_HANDLER;
                        }
                        viewer = handler.createViewer(content);
                    }
                } catch (Exception x) {
                    printStackTrace(x);
                }
            }
        }
        return viewer;
    }

    /**
     * Determines whether this content type is a text
     */
    public static boolean isTextContentType(String mimeType) {
        if (mimeType != null) {
            return mimeType.substring(0,5).equalsIgnoreCase("text/") ||
                   mimeType.equalsIgnoreCase(MimeType.APP_SMIL) ||
                   mimeType.equalsIgnoreCase(MimeType.TEXT_XHTML) ||
                   mimeType.equalsIgnoreCase(MimeType.WML2_SOURCE) ||
                   mimeType.equalsIgnoreCase("application/wml+xml") ||
                   mimeType.equalsIgnoreCase("application/vnd.wap.xhtml+xml") ||
                   mimeType.equalsIgnoreCase("application/x-mrv.xml") ||
                   mimeType.equalsIgnoreCase("application/x-mrm-txt") ||
                   mimeType.equalsIgnoreCase("application/vnd.oma.dd+xml");
        }
        return false;
    }

    /**
     * Prints a stack trace
     */
    private static void printStackTrace(Throwable x) {
        x.printStackTrace();
    }

    //=======================================================================
    //      F A C T O R Y    I N T E R F A C E
    //=======================================================================

    /**
     * Producer of {@link ContentViewer} objects.
     */
    public interface Handler {

        /**
         * Creates a new viewer for the content represented by the
         * {@link DeviceContentReceived} parameter.
         *
         * @exception IllegalArgumentException the MIME type of the content
         *            is not supported by this factory
         */
        public ContentViewer createViewer(DeviceContentReceived content);
    }

    //=======================================================================
    //      H A N D L E R S
    //=======================================================================

    /**
     * Handles encoded WML deck
     */
    private static class WMLBinaryHandler implements Handler {
        public ContentViewer createViewer(DeviceContentReceived content) {
            String mimeType = content.getType();
            if (!MimeType.WML_BINARY.equals(mimeType)) {
                throw new IllegalArgumentException(mimeType);
            }

            // decode the deck
            byte [] data = content.getData();
            try {
                WMLDeck deck = DecoderTool.decode(data);
                if (deck != null) {

                    // create the viewer
                    WMLContentViewer viewer = new WMLContentViewer(deck);
                    String url = content.getLocation();
                    if (url != null) viewer.setURL(url);
                    return viewer;
                }
            } catch (Exception x) {
                printStackTrace(x);
            }

            // cannot parse the content
            if (data != null && data.length > 0) {
                return DEFAULT_BINARY_HANDLER.createViewer(content);
            }

            // nothing we can do
            return null;
        }
    }

    private static class WMLSourceHandler implements Handler {
        public ContentViewer createViewer( DeviceContentReceived content ) {
            String mimeType = content.getType();
            String encoding = content.getEncoding();
            if ( !MimeType.WML_SOURCE.equals( mimeType ) ) {
                throw new IllegalArgumentException( mimeType );
            }

            return new WMLSourceViewer( content );
        }
    }


    private static class XMLSourceHandler implements Handler {
        public ContentViewer createViewer( DeviceContentReceived content ) {
            String mimeType = content.getType();
            String encoding = content.getEncoding();
            if ( !MimeType.TEXT_XHTML.equals( mimeType ) &&
                 !MimeType.TEXT_XML.equals( mimeType ) ) {
                throw new IllegalArgumentException( mimeType );
            }

            return new XMLSourceViewer( content );
        }
    }


    /**
     * Handles unknown binary content
     */
    private static class BinaryHandler implements Handler {
        public ContentViewer createViewer(DeviceContentReceived content) {
            byte [] data = content.getData();
            if (data != null && data.length > 0) {
                return new Viewer(content);
            }
            return null;
        }

        /**
         * Default viewer for binary content
         */
        private static final class Viewer extends BasicViewer {

            private DeviceContentReceived content;
            private String stats;

            Viewer(DeviceContentReceived content) {
                super(content, true);
                this.content = content;
                byte [] data = content.getData();
                int size = 0;
                if (data != null) size = data.length;
                stats = size + " bytes";
            }

            public String getStats() {
                return stats;
            }
        }
    }

    /**
     * Handles unknown text content
     */
    private static class TextHandler implements Handler {
        public ContentViewer createViewer(DeviceContentReceived content) {
            byte [] data = content.getData();
            if (data != null && data.length > 0) {
                return new Viewer(content);
            }
            return null;
        }

        /**
         * Default viewer for text content
         */
        private static final class Viewer extends BasicViewer {
            private String text;

            Viewer(DeviceContentReceived content) {
                super(content, false);
                text = new String(content.getData());
            }
            public String getOriginalSource() {
                return text;
            }
        }
    }

    /**
     * Basic implenmentation of {@link ContentViewer} interface
     */
    private static class BasicViewer implements ContentViewer {

        private DeviceContentReceived content;
        private boolean binary;
        private String url;

        protected BasicViewer(DeviceContentReceived content, boolean binary) {
            url = content.getLocation();
            this.content = content;
            this.binary = binary;
        }

        public String getMimeType() {
            return content.getType();
        }

        public String getBinaryMimeType() {
            return (binary ? getMimeType() : null);
        }

        public String getURL() {
            return url;
        }

        public void setURL(String newURL) {
            url = newURL;
        }

        public String getStats() {

            StringBuffer sb = new StringBuffer();
            if (content != null) {
                sb.append(content.getType());
                sb.append(", " + content.getData().length + " bytes" );
                if (content.getEncoding() != null) {
                    sb.append(", ").append(content.getEncoding());
                }
            }
            return sb.toString();
        }

        public TreeModel getTreeModel() {
            return null;
        }

        public boolean isBinary() {
            return binary;
        }

        public byte [] getByteCode() {
            return (binary ? content.getData() : null);
        }

        public String getDecodedContent() {
            return null;
        }

        public String getOriginalSource() {
            return null;
        }
    }
}
