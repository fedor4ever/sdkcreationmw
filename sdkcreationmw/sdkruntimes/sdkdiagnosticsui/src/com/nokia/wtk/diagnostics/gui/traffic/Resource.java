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

import com.nokia.wtk.diagnostics.views.support.ColorStyle;
/**
 * <p>Title: Resource Strings for Traffic view</p>
 * <p>Description: </p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */

interface Resource {

    public static final String RESET_SESSION_BUTTON_LABEL =
        "traffic.button.reset.label";
    public static final String RESET_SESSION_BUTTON_TOOLTIP =
        "traffic.button.reset.tooltip";
    public static final String FLUSH_CACHE_BUTTON_LABEL =
        "cache.button.clear-cache.label";
    public static final String FLUSH_CACHE_BUTTON_TOOLTIP =
        "cache.button.clear-cache.tooltip";
    public static final String SESSION_NOT_AVAILABLE_MSG =
        "traffic.message.no-session";
    public static final String VIEWTYPE_LABEL = "traffic.label.view-type";

    public final static String REQUESTS_CHOICE_LABEL =
        "traffic.label.view.reqonly";
    public final static String REQUESTS_CHOICE_TOOLTIP =
        "traffic.tooltip.view.reqonly";
    public final static String RESPONSES_CHOICE_LABEL =
        "traffic.label.view.rcvonly";
    public final static String RESPONSES_CHOICE_TOOLTIP =
        "traffic.tooltip.view.rcvonly";
    public final static String PUSH_CHOICE_LABEL = "traffic.label.view.push";
    public final static String PUSH_CHOICE_TOOLTIP =
        "traffic.tooltip.view.push";
    public final static String SMS_RECEIVED_CHOICE_LABEL =
        "traffic.label.view.sms.received";
    public final static String SMS_RECEIVED_CHOICE_TOOLTIP =
        "traffic.tooltip.view.sms.received";
    public final static String SMS_SENT_CHOICE_LABEL =
        "traffic.label.view.sms.sent";
    public final static String SMS_SENT_CHOICE_TOOLTIP =
        "traffic.tooltip.view.sms.sent";
    public final static String BLUETOOTH_RECEIVED_CHOICE_LABEL =
        "traffic.label.view.bluetooth.received";
    public final static String BLUETOOTH_RECEIVED_CHOICE_TOOLTIP =
        "traffic.tooltip.view.bluetooth.received";
    public final static String BLUETOOTH_SENT_CHOICE_LABEL =
        "traffic.label.view.bluetooth.sent";
    public final static String BLUETOOTH_SENT_CHOICE_TOOLTIP =
        "traffic.tooltip.view.bluetooth.sent";
    public final static String TCPUDP_SENT_CHOICE_LABEL =
        "traffic.label.view.tcpudp.sent";
    public final static String TCPUDP_SENT_CHOICE_TOOLTIP =
        "traffic.tooltip.view.tcpudp.sent";
    public final static String TCPUDP_RECEIVED_CHOICE_LABEL =
        "traffic.label.view.tcpudp.received";
    public final static String TCPUDP_RECEIVED_CHOICE_TOOLTIP =
        "traffic.tooltip.view.tcpudp.received";
    public final static String ALL_CHOICE_LABEL = "traffic.label.view.all";
    public final static String NONE_CHOICE_LABEL = "traffic.label.view.none";
    public final static String ALL_CHOICE_TOOLTIP = "traffic.tooltip.view.all";
    public final static String NONE_CHOICE_TOOLTIP =
        "traffic.tooltip.view.none";

    public static final String FLUSH_BUTTON_TOOLTIP = "flush_button_tooltip";
    public static final String FLUSH_BUTTON_LABEL = "flush_button_label";

    final public static String CONTENT_REQUEST_ICON =
        "trafficTreeModels.content.request.icon";
    final public static String CONTENT_RECEIVED_ICON =
        "trafficTreeModels.content.received.icon";
    final public static String PUSH_RECEIVED_ICON =
        "pushTreeModels.content.received.icon";
    final public static String CONTENT_DATA_ICON =
        "trafficTreeModels.content.data.icon";
    final public static String CONTENT_DECODED_DATA_ICON =
        "trafficTreeModels.content.decoded.data.icon";
    final public static String CONTENT_HEADERS_ICON =
        "trafficTreeModels.content.headers.icon";
    final public static String MMS_RECEIVED_ICON =
        "pushTreeModels.mms.received.icon";
    final public static String MMS_SENT_ICON = "pushTreeModels.mms.sent.icon";
    final public static String SMS_RECEIVED_ICON =
        "smsTreeModels.received.icon";
    final public static String SMS_SENT_ICON =
        "smsTreeModels.sent.icon";
    final public static String SMS_FRAGMENT_RECEIVED_ICON =
        "smsTreeModels.fragment.received.icon";
    final public static String SMS_FRAGMENT_SENT_ICON =
        "smsTreeModels.fragment.sent.icon";
    final public static String BLUETOOTH_RECEIVED_ICON =
        "bluetoothTreeModels.received.icon";
    final public static String BLUETOOTH_SENT_ICON =
        "bluetoothTreeModels.sent.icon";

    final public static String openBracket =
        "<font " + ColorStyle.BLACK + ">[</font>";
    final public static String closeBracket =
        "<font " + ColorStyle.BLACK + ">]</font>";

    final public static String TRAFFIC_SUMMARY_SIZE_TXT =
        "trafficSummary.text.size";
    final public static String TRAFFIC_SUMMARY_BYTES_TXT =
        "trafficSummary.text.bytes";
    final public static String TRAFFIC_SUMMARY_PUSHED_TXT =
        "trafficSummary.text.pushed";
    final public static String TRAFFIC_SUMMARY_MMS_RECEIVED_TXT =
        "trafficSummary.text.mms.received";
    final public static String TRAFFIC_SUMMARY_MMS_SENT_TXT =
        "trafficSummary.text.mms.sent";
    final public static String TRAFFIC_SUMMARY_BLUETOOTH_RECEIVED_TXT =
        "trafficSummary.text.bluetooth.received";
    final public static String TRAFFIC_SUMMARY_BLUETOOTH_SENT_TXT =
        "trafficSummary.text.bluetooth.sent";
    final public static String TRAFFIC_SUMMARY_BLUETOOTH_RECEIVED_BR_TXT =
        "trafficSummary.text.bluetooth.receivedbr";
    final public static String TRAFFIC_SUMMARY_BLUETOOTH_SENT_BR_TXT =
        "trafficSummary.text.bluetooth.sentbr";
    // sms
    final public static String SMS_FRAG_SENT_TEXT =
        "trafficSummary.text.sms.fragment.sent";
    final public static String SMS_FRAG_RECEIVED_TEXT =
        "trafficSummary.text.sms.fragment.received";
    final public static String SMS_MSG_SENT_TEXT =
        "trafficSummary.text.sms.message.sent";
    final public static String SMS_MSG_RECEIVED_TEXT =
        "trafficSummary.text.sms.message.received";
    // end sms

    // bluetooth
    final public static String BLUETOOTH_PACKET_SENT_TEXT =
        "trafficSummary.text.bluetooth.packet.sent";
    final public static String BLUETOOTH_PACKET_RECEIVED_TEXT =
        "trafficSummary.text.bluetooth.packet.received";
    // end bluetooth

    // tcpudp
    final public static String TCPUDP_RECEIVED_ICON =
        "tcpudpTreeNodes.node.received.icon";
    final public static String TCPUDP_SENT_ICON =
        "tcpudpTreeNodes.node.sent.icon";
    final public static String TCP_STRING =
        "tcpudpTreeNodes.socketNode.tcp.label";
    final public static String UDP_STRING =
        "tcpudpTreeNodes.socketNode.udp.label";

    // all-none

}
