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

/* java.io */
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.ByteArrayInputStream;
import java.io.UnsupportedEncodingException;

/* java.awt */
import java.awt.Color;
import java.awt.Component;
import java.awt.BorderLayout;
import java.awt.MediaTracker;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTree;
import javax.swing.JLabel;
import javax.swing.ImageIcon;
import javax.swing.UIManager;
import javax.swing.LookAndFeel;
import javax.swing.BorderFactory;
import javax.swing.border.EtchedBorder;
import javax.swing.tree.TreeCellRenderer;

/* com.nokia.wtk */
import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.HTTPStatus;
import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.CharacterEncoding;
import com.nokia.wtk.util.gui.BMPImage;
import com.nokia.wtk.util.gui.WBMPImage;
import com.nokia.wtk.util.gui.SolidBorder;
import com.nokia.wtk.util.wbxml.WMLDeck;
import com.nokia.wtk.util.wbxml.WMLElement;
import com.nokia.wtk.util.wbxml.DecoderTool;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.diagnostics.browser.AttributedContent;
import com.nokia.wtk.diagnostics.browser.ContentLog;
import com.nokia.wtk.device.DeviceContentReceived;
import com.nokia.wtk.device.DeviceContentRequest;
import com.nokia.wtk.device.DeviceContentData;
import com.nokia.wtk.diagnostics.gui.contentViewers.ContentViewers;
import com.nokia.wtk.diagnostics.views.support.ColorStyle;
import com.nokia.wtk.diagnostics.views.support.ContentNode;

/**
 * A collection of tools and lists to build a model of the Traffic over
 * the loader. (No Push nodes, those are in PushTreeNodes)
 */
class TrafficTreeNodes implements Resource {

    static {
        AppEnvironment.addResources(
            "resources/TrafficTreeNodes.properties",
            TrafficTreeNodes.class);
    }
    static private final String DEFAULT_TEXT_ENCODING = "UTF-8";

    static private Icon contentRequestIcon =
        AppEnvironment.getIcon(CONTENT_REQUEST_ICON);
    static private Icon contentReceivedIcon =
        AppEnvironment.getIcon(CONTENT_RECEIVED_ICON);
    static private Icon contentDataIcon =
        AppEnvironment.getIcon(CONTENT_DATA_ICON);
    static private Icon contentDecodedDataIcon =
        AppEnvironment.getIcon(CONTENT_DECODED_DATA_ICON);
    static private Icon imageDataIcon =
        AppEnvironment.getIcon("trafficTreeModels.content.image.icon");

    // Inner Class: A tree node for a content request.
    static class ContentRequestNode extends TopLevelContentNode {

        /**
         * Creates a new ContentRequestNode.
         * @param request the content request
         */
        ContentRequestNode(DeviceContentRequest request) {
            setUserObject(request);

            // Create and assign child nodes:
            String s = request.getMethod();
            if ((s != null) && (s.length() > 0)) {
                add(new RequestMethodNode(s));
            }
            add(new HeaderListNode(request));
            if (request.getMethod().equals("POST")) {
                byte[] data = request.getData();
                if ((data != null) && (data.length > 0)) {
                    add(new ContentDataNode(request, true));
                }
            }
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            DeviceContentRequest request =
                (DeviceContentRequest) getUserObject();
            StringBuffer sbuf = new StringBuffer();

            if (getParent() instanceof ContentReceivedNode) {
                sbuf.append("Original Request: ");
            } else {
                if (asHTML) {
                    sbuf.append("<font " + ColorStyle.GREEN + ">");
                    sbuf.append("<b>");
                }
                sbuf.append("Request: ");
                if (asHTML) {
                    sbuf.append("</b>");
                    sbuf.append("</font>");
                }
            }

            if (asHTML) {
                if (getParent() instanceof ContentReceivedNode) {
                    sbuf.append(
                        "<font face=monospace " + ColorStyle.GREEN + ">");
                } else {
                    sbuf.append(
                        "<font face=monospace " + ColorStyle.BLACK + ">");
                }
            }
            sbuf.append(request.getLocation());
            if (asHTML) {
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentRequestIcon;
        }

        public String getSummary() {
            DeviceContentRequest request =
                (DeviceContentRequest) getUserObject();
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(request.getMethod()).append(", ");
            sbuf.append(request.getLocation());
            return sbuf.toString();
        }

        /* (non-Javadoc)
         * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#isFiltered(com.nokia.wtk.diagnostics.gui.traffic.Filter)
         */
        boolean isFiltered(Filter f) {
            return !f.isRequestsVisible();
        }

    } // End of ContentRequestNode

    //
    // Inner Class: A tree node for received content.
    //
    static class ContentReceivedNode extends TopLevelContentNode {
        /**
         * Creates a new ContentReceivedNode.
         *
         * @param content the received content
         * @param tid the transaction id, <code>null</code> is none
         */
        ContentReceivedNode(DeviceContentReceived content, Long tid) {
            setUserObject(content);
            transactionId = tid;

            String location = content.getLocation();
            // Create and assign child nodes:
            String request = content.getRequestLocation();
            if ((request != null) && !location.equals(request)) {
                add(new OriginalRequestNode(request));
                //TODO: Correlate request URL with request node and link to that
                //add(new ContentRequestNode(findContentRequest(request), type));
            }
            add(new LocationNode(content.getLocation()));

            // Get and save status code and message to later use:
            StringBuffer requestStatusMsg = new StringBuffer();
            this.statusCode = content.getRequestStatus(requestStatusMsg);
            if ((this.statusCode > 0) && (requestStatusMsg.length() > 0)) {
                add(
                    new RequestStatusNode(
                        this.statusCode,
                        (this.statusMsg = requestStatusMsg.toString())));
            }
            add(new HeaderListNode(content));
            tryToCreateContentDataNode();
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            DeviceContentReceived content =
                (DeviceContentReceived) getUserObject();
            StringBuffer sbuf = new StringBuffer();
            String location = content.getLocation();

            if ((content instanceof AttributedContent)
                && ContentLog.isCacheHit((AttributedContent) content)) {
                if (asHTML) {
                    sbuf.append("<b>");
                }
                sbuf.append("Cached Response: ");
                if (asHTML) {
                    sbuf.append("</b>");
                }
            } else if (getParent() instanceof ContentReceivedNode) {
                sbuf.append("Original Response: ");
            } else {
                if (asHTML) {
                    sbuf.append("<b>");
                }
                sbuf.append("Response: ");
                if (asHTML) {
                    sbuf.append("</b>");
                }
            }
            if (RequestStatusNode.isError(this.statusCode)) {
                if (asHTML) {
                    sbuf.append("<font " + ColorStyle.BRIGHTRED + ">");
                    sbuf.append("<b>");
                }
                sbuf.append("Error ");
                sbuf.append(
                    RequestStatusNode.toString(
                        this.statusCode,
                        this.statusMsg));
                if (asHTML) {
                    sbuf.append("</b>");
                    sbuf.append("</font>");
                }
            } else { // Show content data summary:
                if (asHTML) {
                    if (getParent() instanceof ContentReceivedNode) {
                        sbuf.append("<font " + ColorStyle.GREEN + ">");
                    } else {
                        sbuf.append("<font " + ColorStyle.BLACK + ">");
                    }
                }
                if (dataNode != null) {
                  sbuf.append(dataNode.getContentInfo());
                } else {
                    sbuf.append(RequestStatusNode.toString(statusCode,statusMsg));
                }
                if (asHTML) {
                    sbuf.append("</font>");
                }
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentReceivedIcon;
        }

        public String getSummary() {
            DeviceContentReceived rec = getContent();
            StringBuffer sbuf = new StringBuffer();

            if (statusCode != HTTPStatus.OK) {
                sbuf.append("Status: ").append(statusCode).append(" ");
                sbuf.append(statusMsg);
                sbuf.append(", ");
            }
            sbuf.append(rec.getType());

            String encoding = rec.getEncoding();
            if ((encoding != null) && (encoding.length() > 0)) {
                sbuf.append(", ").append(encoding);
            }
            if (dataNode != null) {
              sbuf.append(", ").append(dataNode.getDataLength()).append(" bytes");
            }
            return sbuf.toString();
        }

        public void contentChanged() {
            imageIcon = null;
            if (dataNode != null) {
                dataNode.contentChanged();
                tryToCreateDecodedContentDataNode();
                tryToCreateImageNode();
            } else {
                tryToCreateContentDataNode();
            }
        }

        private void tryToCreateContentDataNode() {
            if (dataNode == null) {
                DeviceContentReceived content = getContent();
                boolean needDataNode = false;
                if (statusCode == HTTPStatus.OK) {
                    needDataNode = true;
                } else {
                    byte [] data = content.getData();
                    if (data != null && data.length > 0) {
                        needDataNode = true;
                    }
                }
                if (needDataNode) {
                    dataNode = new ContentDataNode(content, false);
                    add(dataNode);
                    tryToCreateDecodedContentDataNode();
                    tryToCreateImageNode();
                }
            }
        }

        private void tryToCreateDecodedContentDataNode() {
            if (decodedDataNode == null) {
                DeviceContentReceived content = getContent();
                try {
                    decodedDataNode = new ContentDecodedDataNode(content);
                    add(decodedDataNode);
                } catch (Exception x) {
                }
            }
        }

        private void tryToCreateImageNode() {
            if (imageNode == null) {
                DeviceContentReceived content = getContent();
                String mimeType = content.getType();
                if (mimeType != null && (
                    mimeType.startsWith("image/") ||
                    mimeType.startsWith("application/"))) {
                    imageIcon = createImage(content);
                    if (imageIcon != null) {
                        imageNode = new ImageNode(this);
                        add(imageNode);
                    }
                }
            }
        }

        /**
         * This method is invoked by ImageDisplayNode to get the image.
         * Note that the image may change as we receive more data.
         * @return the current image.
         */
        ImageIcon getImage() {
            if (imageIcon == null) {
                imageIcon = createImage(getContent());
            }
            return imageIcon;
        }

        /**
         * Creates image from the content data
         * @param content the content which presumably contains an image
         * @return the image object, or <code>null</code> if data don't seem
         *   to contain a valid image.
         */
        private static ImageIcon createImage(DeviceContentReceived content) {
            byte [] data = content.getData();
            if (data == null || data.length == 0) {
                // Quickly fail if we have no data
                return null;
            }

            ImageIcon imageIcon = null;
            try {
                imageIcon = new ImageIcon(data);
            } catch (Throwable x) {
            }
            if (imageIcon != null &&
                imageIcon.getImageLoadStatus() == MediaTracker.COMPLETE &&
                imageIcon.getIconWidth() > 0 &&
                imageIcon.getIconHeight() > 0) {
                return imageIcon;
            }

            // BMP and WBMP are not supported by Java
            imageIcon = null;
            if (content.getType().equals(MimeType.IMAGE_WBMP)) {
                try {
                    imageIcon = new ImageIcon(new WBMPImage(data).getImage());
                } catch (Throwable x) {
                }
            } else {
                // Different servers use different content type for
                // Windows bitmaps. Try it without checking the content
                // type. If it's not a bitmap, it's going to fail very
                // quickly
                try {
                    imageIcon = new ImageIcon(new BMPImage(data).getImage());
                } catch (Throwable y) {
                }
            }
            return imageIcon;
        }

        private DeviceContentReceived getContent() {
            return (DeviceContentReceived)getUserObject();
        }

        /* (non-Javadoc)
         * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#isFiltered(com.nokia.wtk.diagnostics.gui.traffic.Filter)
         */
        boolean isFiltered(Filter f) {
            return !f.isResponsesVisible();
        }

        /**
         * Returns the transaction id for this node.
         * @return the transaction id, <code>null</code> is none.
         */
        Long getTransactionId() {
            return transactionId;
        }

        private Long transactionId;
        private int statusCode; // content response status code
        private String statusMsg;
        private ContentDataNode dataNode;
        private ContentDecodedDataNode decodedDataNode;
        private ImageNode imageNode;
        private ImageIcon imageIcon;

    } // End of ContentReceivedNode

    //
    // Inner Class: A tree node for a location URL.
    //
    private static class LocationNode extends ContentNode {

        /**
         * Creates a new LocationNode.
         * @param location the location URL string
         */
        private LocationNode(String location) {
            super(true);
            this.location = location;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.BLACK + ">");
            }
            sbuf.append("Request URL = ");
            if (asHTML) {
                sbuf.append(openBracket);
                sbuf.append("<font face=monospace " + ColorStyle.GREEN + ">");
            } else {
                sbuf.append("[");
            }
            sbuf.append(ContentNode.toString(location));
            if (asHTML) {
                sbuf.append("</font>");
                sbuf.append(closeBracket);
                sbuf.append("</font>");
            } else {
                sbuf.append("]");
            }
            return sbuf.toString();
        }

        private String location;
    } // End of LocationNode

    //
    // Inner Class: A tree node for an original request location URL.
    //
    private static class OriginalRequestNode extends ContentNode {

        /**
         * Creates a new OriginalRequestNode.
         * @param location the location URL string
         */
        private OriginalRequestNode(String location) {
            super(true);
            this.location = location;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
           if (asHTML) {
               sbuf.append("<font " + ColorStyle.BLACK + ">");
           }
            sbuf.append("Original Request URL = ");
            if (asHTML) {
                sbuf.append("</font>");
                sbuf.append(openBracket);
                sbuf.append("<font face=monospace " + ColorStyle.GREEN + ">");
            } else {
                sbuf.append("[");
            }
            sbuf.append(ContentNode.toString(location));
            if (asHTML) {
                sbuf.append("</font>");
                sbuf.append(closeBracket);
            } else {
                sbuf.append("]");
            }
            return sbuf.toString();
        }

        private String location;
    } // End of OriginalRequestNode

    //
    // Inner Class: A tree node for a content request method.
    //
    private static class RequestMethodNode extends ContentNode {

        /**
         * Creates a new RequestMethodNode.
         * @param method the content request method
         */
        private RequestMethodNode(String method) {
            super(true);
            this.method = method;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.BLACK + ">");
            }
            sbuf.append("Request Method = ");
            if (asHTML) {
                sbuf.append(openBracket);
                sbuf.append("<font " + ColorStyle.GREEN + ">");
            } else {
                sbuf.append("[");
            }
            sbuf.append(method);
            if (asHTML) {
                sbuf.append("</font>");
                sbuf.append(closeBracket);
            } else {
                sbuf.append("]");
            }
            return sbuf.toString();
        }

        private String method;
    } // End of RequestMethodNode


    //
    // Inner Class: A tree node for content data.
    //
    static class ContentDataNode extends ContentNode {

        /**
         * Creates a new ContentDataNode.
         * @param content the content containing the data
         */
        ContentDataNode(DeviceContentData content, boolean postData) {
            super(false);
            this.content = content;
            this.postData = postData;
            if (getDataLength() > 0) createDataNode();

            // Allow UI to gain access to the content:
            setUserObject(this);
        }

        private void createDataNode() {
            if (dataNode == null) {
                dataNode = new ContentDataDisplayNode(content);
                add(dataNode);
                dataNode.setUserObject(this);
            }
        }

        /**
         * Gets the content data.
         * (Used by TrafficView)
         */
        byte[] getData() {
            return content.getData();
        }

        /**
         * Gets the content data type.
         * (Used by TrafficView)
         */
        String getType() {
            return content.getType();
        }

        /**
         * Gets the content data encoding.
         * (Used by TrafficView)
         */
        String getEncoding() {
            return content.getEncoding();
        }

        /**
         * Gets the text representation of content data.
         * @param content the content data
         */
        private static String toString(DeviceContentReceived content) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(content.getData().length);
            sbuf.append(" bytes");
            String type = content.getType();
            if ((type != null) && (type.length() > 0)) {
                sbuf.append(", ");
                sbuf.append(type);
                String encoding = content.getEncoding();
                if ((encoding != null) && (encoding.length() > 0)) {
                    sbuf.append("; ");
                    sbuf.append(encoding);
                }
            }
            return sbuf.toString();
        }

        /**
         * Gets the text representation of content data.
         * @return the string representation of content data
         */
        private String getContentInfo() {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(getDataLength());
            sbuf.append(" bytes");
            String type = content.getType();
            if ((type != null) && (type.length() > 0)) {
                sbuf.append(", ");
                sbuf.append(type);
                String encoding = content.getEncoding();
                if ((encoding != null) && (encoding.length() > 0)) {
                    sbuf.append("; ");
                    sbuf.append(encoding);
                }
            }
            return sbuf.toString();
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            if (postData) {
              sbuf.append("POST Data: ");
            }
            else {
              sbuf.append("Content Data: ");
            }
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.GREEN + ">");
            }
            sbuf.append(getContentInfo());
            if (asHTML) {
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentDataIcon;
        }

        public void contentChanged() {
            if (getDataLength() > 0) {
                createDataNode();
            }
        }

        public int getDataLength() {
          byte [] data = content.getData();
          return (data == null) ? 0 : data.length;
        }

        public DeviceContentData getContent() {
          return content;
        }

        private DeviceContentData content;
        private ContentDataDisplayNode dataNode;
        boolean postData = false;

    } // End of ContentDataNode

    //
    // Inner Class: A tree node for content data.
    //
    private static class ContentDecodedDataNode extends ContentNode {
        private DeviceContentData content;
        private ContentDecodedDataDisplayNode dataNode;


        /**
         * Creates a new DecodedContentDataNode.
         * @param content the content containing the data
         */
        ContentDecodedDataNode(DeviceContentData content) throws Exception {
            super(false);
            this.content = content;
            recreateDataNode();
        }

        private void recreateDataNode() throws Exception {
            if (dataNode != null) {
                remove(dataNode);
                dataNode = null;
            }
            String type = content.getType();
            if (type != null) {
                if (type.indexOf("wbxml") > 0 ||
                    type.equals(MimeType.WML_BINARY) ||
                    type.equals(MimeType.PUSH_SI_BINARY) ||
                    type.equals(MimeType.PUSH_SL_BINARY)) {
                    byte[] data = content.getData();
                    InputStream in = new ByteArrayInputStream(data);
                    WMLElement e = DecoderTool.decodeWBXML(in);
                    if (e != null) {
                        dataNode = new ContentDecodedDataDisplayNode(content,e);
                        add(dataNode);
                    }
                } else {
                    throw new Exception("Non-decodable content");
                }
            }
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.GREEN + ">");
            }
            sbuf.append("Decoded Content Data");
            if (asHTML) {
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentDecodedDataIcon;
        }

        public void contentChanged() throws Exception {
            recreateDataNode();
        }
    }

    //
    // Inner Class: A tree node for content data display.
    //
    private static class ContentDataDisplayNode extends ContentNode
        implements TreeCellRenderer {

        /**
         * Creates a new ContentDataDisplayNode.
         * @param content the content containing the data
         */
        ContentDataDisplayNode(DeviceContentData content) {
            super(false);
            this.content = content;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            if (asHTML) return null; // Using a special TreeCellRenderer
            return getFormattedText();
        }

        /**
         * Gets the renderer component for this node.
         * @return the renderer component
         */
        public Component getTreeCellRendererComponent(
            JTree tree,
            Object value,
            boolean selected,
            boolean expanded,
            boolean leaf,
            int row,
            boolean hasFocus) {

            // ASSUMPTION: DeviceContentReceived never changes
            if (panel == null) {
                panel = new JPanel(new BorderLayout());
                textArea = new JTextArea();
                panel.setBorder(
                    BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
                panel.add(textArea, BorderLayout.CENTER);
                textArea.setFont(LookFactory.getStandardCodeFont());
            }
            textArea.setText(getFormattedText());

            if (selected) {
                LookAndFeel.installColors(
                    textArea,
                    "Table.selectedForeground",
                    "Table.selectedBackground");
            } else {
                LookAndFeel.installColors(
                    textArea,
                    "Table.background",
                    "Table.foreground");
            }

            return panel;
        }

        public String toString() {
            return getFormattedText();
        }

        /**
         * Gets the formatted text for the data.
         * @return the formatted text
         */
        protected String getFormattedText() {
            String formattedText = null;
            String mimetype = content.getType();
            // Use default formatting if no custom formatter exists.
            // Attempt to convert bytes to text based on content type:
            if (ContentViewers.isTextContentType(mimetype)) {
                String encoding = content.getEncoding();
                if (encoding == null) {
                  encoding = DEFAULT_TEXT_ENCODING;
                }
                try {
                    formattedText = getTextString(encoding);
                } catch (UnsupportedEncodingException ex) {
                    formattedText = getBinaryString();
                }
            } else {
                formattedText = getBinaryString();
            }
            return formattedText;
        }

        /**
         * Returns a string for text content type.
         * If the content is larger than the maximum value, we truncate it.
         *
         * @param enc the character encoding
         * @return the text string
         */
        private String getTextString(String enc)
            throws UnsupportedEncodingException {
            byte [] data = content.getData();
            if (data == null) return "";
            try {
                if (data.length > CONTENT_BUF_MAX_LENGTH) {
                    StringBuffer sb = new StringBuffer(CONTENT_BUF_MAX_LENGTH+30);
                    sb.append(new String(data, 0, CONTENT_BUF_MAX_LENGTH, enc));
                    sb.append("\nData truncated...");
                    return sb.toString();
                } else {
                    return new String(data, enc);
                }
            } catch (OutOfMemoryError e) {
                return "No more content can be displayed, system out of memory!";
            }
        }

        /**
         * Returns the string representation of the binary data
         * @return the string representation of the content byte array.
         */
        private String getBinaryString() {
            byte [] data = content.getData();
            if (data == null) return "";
            try {
                int len = data.length;
                if (len > CONTENT_BUF_MAX_LENGTH) len = CONTENT_BUF_MAX_LENGTH;
                StringWriter sw = new StringWriter();
                PrintWriter pw = new PrintWriter(sw);
                StringUtils.hexDump(pw, data, 0, len);
                if (len < data.length) {
                    pw.println();
                    pw.print("Data truncated...");
                }
                String hex = sw.toString();
                if (hex.endsWith("\n")) hex = hex.substring(0,hex.length()-1);
                return hex;
            } catch (OutOfMemoryError e) {
                return "No more content can be displayed, system out of memory!";
            }
        }

        private DeviceContentData content;

        // only these max number of bytes will be displayed in the
        // data content node.
        private final int  CONTENT_BUF_MAX_LENGTH = 50000;

        //  reuse panel/text area.
        private static JPanel panel;
        private static JTextArea textArea;

    } // End of ContentDataDisplayNode

    private static class ContentDecodedDataDisplayNode
        extends ContentDataDisplayNode {
        private static final String XML_PREAMBLE = "<?xml version=\"1.0\"?>";
        private static final String XML_PREAMBLE1 = "<?xml version=\"1.0\" encoding=\"";
        private static final String XML_PREAMBLE2 = "\"?>";
        private WMLElement element;
        ContentDecodedDataDisplayNode(DeviceContentData content,
                                      WMLElement root) {
            super(content);
            element = root;
        }
        protected String getFormattedText() {
            String charset = null;
            String doctype = null;
            if (element instanceof WMLDeck) {
                WMLDeck deck = (WMLDeck)element;
                doctype = deck.getDoctypeString();
                CharacterEncoding enc = deck.getEncoding();
                if (enc != null) charset = enc.getMIMEName().toLowerCase();
            }

            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            if (charset != null) {
                pw.print(XML_PREAMBLE1);
                pw.print(charset);
                pw.println(XML_PREAMBLE2);
            } else {
                pw.println(XML_PREAMBLE);
            }
            if (doctype != null) pw.println(doctype);
            element.dumpElement(pw);
            pw.flush();
            return sw.toString();
        }
    }

    private static class ImageNode extends ContentNode {

        private ContentReceivedNode contentNode;

        /**
         * Creates a new ImageNode.
         * @param image the image
         */
        ImageNode(ContentReceivedNode parent) {
            super(false);
            contentNode = parent;
            add(new ImageDisplayNode(this));
        }

        /**
         * Returns the image represented by this node. Note that image may
         * change as we receive more data.
         * @return  the image represented by this node. Theoretically, can
         *    return <code>null</code>
         */
        ImageIcon getImage() {
            return contentNode.getImage();
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            ImageIcon image = getImage();
            if (image == null) {
                return "Image?";
            } else {
                StringBuffer sb = new StringBuffer(asHTML ? 42 : 16);
                sb.append("Image: ");
                if (asHTML) {
                    sb.append("<font " + ColorStyle.GREEN + ">");
                }
                sb.append(image.getIconWidth());
                sb.append("x");
                sb.append(image.getIconHeight());
                if (asHTML) {
                    sb.append("</font>");
                }
                return sb.toString();
            }
        }

        public Icon getIcon() {
            return imageDataIcon;
        }

    } // End of ImageNode

    private static class ImageDisplayNode extends ContentNode
    implements TreeCellRenderer {

        private static JLabel imageRenderer;
        private static SolidBorder imageBorder;
        private ImageNode parentNode;

        /**
         * Creates a new ImageDisplayNode.
         * @param parent the parent node
         */
        ImageDisplayNode(ImageNode parent) {
            super(false);
            parentNode = parent;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            return parentNode.getAsText(asHTML);
        }

        /**
         * Gets the renderer component for this node.
         * @return the renderer component
         */
        public Component getTreeCellRendererComponent(JTree tree, Object value,
            boolean selected, boolean expanded, boolean leaf, int row,
            boolean hasFocus) {
            ImageIcon image = parentNode.getImage();
            if (image == null) {
                return null;
            } else {
                if (imageRenderer == null) {
                    imageBorder = new SolidBorder(1);
                    imageRenderer = new JLabel();
                    imageRenderer.setBorder(imageBorder);
                    imageRenderer.setBackground(Color.LIGHT_GRAY);
                }

                imageRenderer.setIcon(image);
                imageRenderer.setOpaque(selected);
                imageBorder.setFillColor(selected ?
                    LookFactory.getCodeColorSet().selectionColor :
                    UIManager.getColor("Tree.hash"));

                return imageRenderer;
            }
        }

    } // End of ImageDisplayNode

    //
    // Inner Class: A tree node for a content request status.
    //
    private static class RequestStatusNode extends ContentNode {

        /**
         * Creates a new RequestStatusNode.
         * @param code the request status code
         * @param message the request status message
         */
        private RequestStatusNode(int code, String message) {
            super(true);
            this.code = code;
            this.message = message;
        }

        /**
         * Checks a status code for error status.
         * @return <code>true</code> if code value represents an error.
         */
        private static boolean isError(int code) {
            return (code >= 400);
            // see RFC2068
        }

        /**
         * Checks a status code for error status.
         * @return <code>true</code> if code value represents an error.
         */
        private static String toString(int code, String message) {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append(code);
            if ((message != null) && (message.length() > 0)) {
                sbuf.append(": \"");
                sbuf.append(message);
                sbuf.append("\"");
            }
            return sbuf.toString();
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.BLACK + ">");
            }
            sbuf.append("Request Status = ");
            if (asHTML) {
                if (isError(code)) {
                    // Highlight Error Status in Red:
                    sbuf.append("<font " + ColorStyle.BRIGHTRED + ">");
                } else {
                    sbuf.append("<font " + ColorStyle.GREEN + ">");
                }
            }
            sbuf.append(toString(code, message));
            if (asHTML) {
                sbuf.append("</font>");
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        private int code;
        private String message;
    } // End of RequestStatusNode

} // End of TrafficTreeNodes
