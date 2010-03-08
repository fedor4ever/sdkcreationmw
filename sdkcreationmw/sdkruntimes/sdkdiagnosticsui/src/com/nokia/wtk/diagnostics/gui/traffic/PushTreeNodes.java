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

import java.awt.BorderLayout;
import java.awt.Component;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTree;
import javax.swing.LookAndFeel;
import javax.swing.border.EtchedBorder;
import javax.swing.tree.TreeCellRenderer;

import com.nokia.wtk.app.AppEnvironment;

import com.nokia.wtk.util.MimeType;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.StringUtils;
import com.nokia.wtk.util.wbxml.DecoderTool;

import com.nokia.wtk.diagnostics.views.support.ColorStyle;
import com.nokia.wtk.diagnostics.views.support.ContentNode;
import com.nokia.wtk.diagnostics.browser.AttributedContent;

/**
 * A model for collecting push information from devices
 */
class PushTreeNodes implements Resource, MessageWriters {

    static {
        AppEnvironment.addResources(
            "resources/PushTreeNodes.properties",
            PushTreeNodes.class);
    }

    static private Icon pushReceivedIcon =
        AppEnvironment.getIcon(PUSH_RECEIVED_ICON);
    static private Icon contentDataIcon =
        AppEnvironment.getIcon(CONTENT_DATA_ICON);
    static private Icon mmsReceivedIcon =
        AppEnvironment.getIcon(MMS_RECEIVED_ICON);
    static private Icon mmsSentIcon = AppEnvironment.getIcon(MMS_SENT_ICON);

    /*
     * Inner Class: A tree node for received AttributedContent.
     */
    static class PushNode extends TopLevelContentNode {

        /**
         * Creates a new PushNode.
         * @param content the received content
         * @param type the type of the containing model.
         */
        PushNode(AttributedContent content) {
            setUserObject(content);
            //this.data = content.getData();

            add(new ContentDataNode(content, false));
            if (isDecodable(content)) {
                add(new ContentDataNode(content, true));
            }
            add(new HeaderListNode(content));
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            AttributedContent content = (AttributedContent) getUserObject();
            StringBuffer sbuf = new StringBuffer();
            String location = content.getLocation();
            if (getParent() instanceof PushNode) {
                sbuf.append("Original Response: ");
            } else {
                String category = getMessageCategory(content.getType());
                if (asHTML) {
                    sbuf.append("<b>");
                }
                String eventType = null;

                // temorary hack to distinguish between MMS Received/Sent
                // if this is MMS content, use the location field to determine it this
                // is an MMS reveived or MMS sent (temporary hack), add this information
                // to the buffer
                if (category.equals("MMS")) {
                    if (location != null) {
                        if (location.equalsIgnoreCase("sent")) {
                            eventType = " Sent: ";
                        } else {
                            // default to MMS received
                            eventType = " Pushed: ";
                        }
                    } else {
                        // default to MMS received
                        eventType = " Pushed: ";
                    }
                } else {
                    eventType = " Pushed: ";
                }
                sbuf.append(eventType + category + ", ");
                if (asHTML) {
                    sbuf.append("</b>");
                }
            }
            // end temorary hack to distinguish between MMS Received/Sent

            // Show content data summary:
            if (asHTML) {
                if (getParent() instanceof PushNode) {
                    sbuf.append("<font " + ColorStyle.GREEN + ">");
                } else {
                    sbuf.append("<font " + ColorStyle.BLACK + ">");
                }
            }
            sbuf.append(ContentDataNode.toString(content));
            if (asHTML) {
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        //
        // Can we show a decoded view for this content?
        //
        private boolean isDecodable(AttributedContent content) {
            String type = content.getType();
            if (type == null || type.length() == 0)
                return false;
            if (type.equalsIgnoreCase(MimeType.PUSH_SI_BINARY)
                || type.equalsIgnoreCase(MimeType.PUSH_SL_BINARY)
                || type.equalsIgnoreCase(MimeType.CACHE_OP_BINARY)) {
                return true;
            }
            return false;
        }

        // Return printable abbreviation of message type
        //
        public String getMessageCategory(String ct) {
            String nada = "";
            if (ct == null || ct.trim().length() == 0)
                return nada;
            if (ct.equalsIgnoreCase(MimeType.PUSH_SI_BINARY)
                || ct.equalsIgnoreCase(MimeType.PUSH_SI_SOURCE)) {
                return "SI";
            }
            if (ct.equalsIgnoreCase(MimeType.PUSH_SL_BINARY)
                || ct.equalsIgnoreCase(MimeType.PUSH_SL_SOURCE)) {
                return "SL";
            }
            if (ct.equalsIgnoreCase(MimeType.CACHE_OP_BINARY)
                || ct.equalsIgnoreCase(MimeType.CACHE_OP_SOURCE)) {
                return "CO";
            }
            if (ct.equalsIgnoreCase(MimeType.MULTIPART_MIXED_WSP)
                || ct.equalsIgnoreCase(MimeType.MULTIPART_RELATED_WSP)
                || ct.equalsIgnoreCase(MimeType.MULTIPART_ALTERNATIVE_WSP)) {
                return "Multipart";
            }
            if (ct.equalsIgnoreCase(MimeType.MMS_MESSAGE)) {
                return "MMS";
            }
            if (ct.equalsIgnoreCase(MimeType.DRM_RIGHTS_BIN)) {
              return "DR";
            }

            return nada;
        }

        public Icon getIcon() {
            AttributedContent content = (AttributedContent) getUserObject();
            String location = content.getLocation();
            String category = getMessageCategory(content.getType());

            // temorary hack to distinguish between MMS Received/Sent
            // if this is MMS content, use the location field to determine it this
            // is an MMS reveived or MMS sent (temporary hack), add this information
            // to the buffer
            if (category.equals("MMS")) {
                if (location == null || !location.equalsIgnoreCase("sent")) {
                    return mmsReceivedIcon;
                } else {
                    return mmsSentIcon;
                }
            } else {
                return pushReceivedIcon;
            }
        }

        public String getSummary() {
            String txt = "";
            StringBuffer label = new StringBuffer();
            AttributedContent content = (AttributedContent) getUserObject();
            // change text for MMS traffic - temporary hack
            String contentType = content.getType();
            String actionType =
                AppEnvironment.getString(TRAFFIC_SUMMARY_PUSHED_TXT);
            if (contentType.equalsIgnoreCase(MimeType.MMS_MESSAGE)) {
                // string indicating "received" or "sent" is temporarily
                // in AttributedContent.location
                String direction = content.getLocation();
                if (direction == null
                    || (!direction.equalsIgnoreCase("sent"))) {
                    actionType =
                        AppEnvironment.getString(
                            TRAFFIC_SUMMARY_MMS_RECEIVED_TXT);
                } else {
                    actionType =
                        AppEnvironment.getString(TRAFFIC_SUMMARY_MMS_SENT_TXT);
                }
            }
            label.append(actionType);
            // end change text for MMS traffic - temporary hack

            label.append(content.getType()).append(", ").append(
                content.getEncoding());
            return label.toString();
        }

        /* (non-Javadoc)
         * @see com.nokia.wtk.diagnostics.gui.traffic.TopLevelContentNode#isFiltered(com.nokia.wtk.diagnostics.gui.traffic.Filter)
         */
        boolean isFiltered(Filter f) {
          /*
            AttributedContent content = (AttributedContent) getUserObject();
            String location = content.getLocation();

            String category = getMessageCategory(content.getType());
            if (category.equals("MMS")) {
                if (location == null || !location.equalsIgnoreCase("sent")) {
                    return !f.isMMSReceivedVisible();
                } else {
                    return !f.isMMSSentVisible();
                }
            } else {
              */
                return !f.isPushedVisible();
//            }

        }

        /* (non-Javadoc)
         * Returns false. Push nodes are all unrelated to each other and all
         * other kinds of nodes.
         */
        boolean setRelated(TopLevelContentNode node) {
            return isRelated();
        }

        /* (non-Javadoc)
         * Returns false. Push nodes are all unrelated to each other and all
         * other kinds of nodes.
         */
        boolean isRelated() {
            return false;
        }
    } // End of PushNode

    /**
     * Inner Class: A tree node for AttributedContent data.
     */
    static class ContentDataNode extends ContentNode {

        /**
         * Creates a new ContentDataNode.
         * @param content the content containing the data
         */
        protected ContentDataNode(
            AttributedContent content,
            boolean showDecoded) {
            super(false);
            this.content = content;
            this.showDecoded = showDecoded;
            ContentNode dataNode;
            if (showDecoded) {
                dataNode = new DecodedContentDataDisplayNode(content);
            } else {
                dataNode = new ContentDataDisplayNode(content);
            }
            add(dataNode);

            // Allow UI to gain access to the content:
            setUserObject(this);
            dataNode.setUserObject(this);
        }

        /**
         * Gets the content data.
         */
        byte[] getData() {
            return content.getData();
        }

        /**
         * Gets the content data type.
         */
        String getType() {
            return content.getType();
        }

        /**
         * Gets the content data encoding.
         */
        String getEncoding() {
            return content.getEncoding();
        }

        /**
         * Gets the text representation of content data.
         * @param content the content data
         */
        protected static String toString(AttributedContent content) {
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
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            StringBuffer sbuf = new StringBuffer();
            if (showDecoded) {
                if (asHTML) {
                    return "<font "
                        + ColorStyle.DARKGRAY
                        + ">Decoded Content Data</font>";
                } else {
                    return "Decoded Content Data";
                }
            }
            sbuf.append("Raw Content Data: ");
            if (asHTML) {
                sbuf.append("<font " + ColorStyle.GREEN + ">");
            }
            sbuf.append(toString(content));
            if (asHTML) {
                sbuf.append("</font>");
            }
            return sbuf.toString();
        }

        public Icon getIcon() {
            return contentDataIcon;
        }

        private AttributedContent content;
        private boolean showDecoded = false;
    } // End of ContentDataNode

    /*
     * Inner Class: A tree node for content data display.
     */
    static class ContentDataDisplayNode
        extends ContentNode
        implements TreeCellRenderer {

        /**
         * Creates a new ContentDataDisplayNode.
         * @param content the content containing the data
         */
        private ContentDataDisplayNode(AttributedContent content) {
            super(false);
            this.content = content;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            if (asHTML) {
                return null; // Using a special TreeCellRenderer instead
            } else {
                return getFormattedText();
            }
        }

        public String toString() {
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
            JPanel panel = new JPanel(new BorderLayout(0, 0));
            panel.setBorder(
                BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));

            JTextArea textArea = new JTextArea(getFormattedText());
            panel.add(textArea, BorderLayout.CENTER);
            if (selected) {
                LookAndFeel.installColors(
                    textArea,
                    "TextField.selectedBackground",
                    "TextField.selectedForeground");
            }
            return panel;
        }

        /**
         * Gets the formatted text for the data.
         * @return the formatted text
         */

        private String getFormattedText() {
            String formattedText = null;
            // always display the data in hex dump, with accompanying text
            // when possible, that's what getBinaryString returns
            formattedText = getBinaryString(content.getData());
            return formattedText;
        }

        /**
         * Returns the string representation of the binary data
         * @return the string representation of the binary data.
         */
        private String getBinaryString(byte[]data) {
          if (data.length > 0) {
            try {
              ByteArrayOutputStream buf = new ByteArrayOutputStream();
              int n = Math.min(data.length, CONTENT_BUF_MAX_LENGTH);
              buf.write(data, 0, n);
              StringWriter sw = new StringWriter();
              PrintWriter pw = new PrintWriter(sw);
              StringUtils.hexDump(pw, buf.toByteArray());
              String hex = sw.toString();
              if (hex.endsWith("\n")) hex = hex.substring(0,hex.length()-1);
              return hex;
            } catch (OutOfMemoryError e) {
              return "No more content can be displayed, system out of memory!";
            }
          }
          else {
            return "empty";
          }
        }

        //TODO: Future right-click menu to enable/disable custom formatter?
        private boolean allowCustomFormatters = true;
        private AttributedContent content;
        // only these max number of bytes will be displayed in the
        // data content node.
        private final int  CONTENT_BUF_MAX_LENGTH = 50000;

    } // End of ContentDataDisplayNode

    /*
     * Inner Class: A tree node for decoded content data display.
     */
    static class DecodedContentDataDisplayNode
        extends ContentNode
        implements TreeCellRenderer {

        /**
         * Creates a new DecodedContentDataDisplayNode.
         * @param content the content containing the data
         */
        protected DecodedContentDataDisplayNode(AttributedContent content) {
            super(false);
            this.content = content;
        }

        /**
         * Gets the text representation of this node.
         * @return the text representation of this node
         */
        public String getAsText(boolean asHTML) {
            if (asHTML) {
                return null; // Using a special TreeCellRenderer instead
            } else {
                return getFormattedText();
            }
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
            JPanel panel = new JPanel(new BorderLayout(0, 0));
            panel.setBorder(
                BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));

            JTextArea textArea = new JTextArea(getFormattedText());
            textArea.setEditable(true);
            if (selected) {
                LookAndFeel.installColors(
                    textArea,
                    "TextField.selectedBackground",
                    "TextField.selectedForeground");
            }
            panel.add(textArea, BorderLayout.CENTER);
            return panel;
        }

        public String toString() {
            return getFormattedText();
        }

        /**
         * Gets the formatted text for the data.
         * @return the formatted text
         */

        private String getFormattedText() {
          String text;
          try {
            boolean ok;
            ByteArrayInputStream in = new ByteArrayInputStream(content.getData());
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            ok = DecoderTool.decode(in,out);
            if (!ok) {
              text = "unable to parse the binary data";
            }
            else {
              text = out.toString();
              StringWriter sw = new StringWriter();
              PrintWriter pw = new PrintWriter(sw);
              String encoding = content.getEncoding();
              if (encoding != null) {
                encoding = encoding.trim();
              }
              if ((encoding != null) && (encoding.length() > 0)) {
                pw.print(XML_PREAMBLE1);
                pw.print(encoding);
                pw.println(XML_PREAMBLE2);
              }
              else {
                pw.println(XML_PREAMBLE);
              }

              String contentType = content.getType().trim();
              if (contentType.equalsIgnoreCase(MimeType.PUSH_SI_BINARY)
                || contentType.equalsIgnoreCase(MimeType.PUSH_SI_SOURCE)) {
                pw.println(HEADER_SI);
              }
              else if (contentType.equalsIgnoreCase(MimeType.PUSH_SL_BINARY)
                  || contentType.equalsIgnoreCase(MimeType.PUSH_SL_SOURCE)) {
                pw.println(HEADER_SL);
              }
              else if (contentType.equalsIgnoreCase(MimeType.CACHE_OP_BINARY)
                 || contentType.equalsIgnoreCase(MimeType.CACHE_OP_SOURCE)) {
                pw.println(HEADER_CO);
             }
             pw.println();
             pw.println();
             pw.println(text);
             pw.flush();
             text = sw.toString();
           }
         } catch (Throwable x) {
             text = "unable to parse the binary data";
             x.printStackTrace();
         }
         return text;
       }

       //TODO: Future right-click menu to enable/disable custom formatter?
       private boolean allowCustomFormatters = true;
       private AttributedContent content;

       // hardcoded strings for XML and DTD declarations.
       // Note: in the future, we may want to generate these in
       // the WBXML package as part of the message decoding

       // XML version string
       private static final String XML_PREAMBLE = "<?xml version=\"1.0\"?>";

       // XML encoding string 1
       public static final String XML_PREAMBLE1 = "<?xml version=\"1.0\" encoding=\"";

       // XML encoding string 2
       public static final String XML_PREAMBLE2 = "\"?>";

       // XML DOCTYPE header string for SI
       public static final String HEADER_SI = "<!DOCTYPE si PUBLIC " +
            "\"-//WAPFORUM//DTD SI 1.0//EN\" \n \"http://www.wapforum.org/DTD/si.dtd\">";

       // XML DOCTYPE header string for SL
       public static final String HEADER_SL = "<!DOCTYPE sl PUBLIC " +
            "\"-//WAPFORUM//DTD SL 1.0//EN\" \n \"http://www.wapforum.org/DTD/sl.dtd\">";

       // XML DOCTYPE header string for CO
       public static final String HEADER_CO = "<!DOCTYPE co PUBLIC " +
         "\"-//WAPFORUM//DTD CO 1.0//EN\" \n \"http://www.wapforum.org/DTD/co.dtd\">";

    } // End of ContentDataDisplayNode

}
