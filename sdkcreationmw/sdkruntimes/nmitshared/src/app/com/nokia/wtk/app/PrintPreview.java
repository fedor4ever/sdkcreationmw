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


package com.nokia.wtk.app;

/* java.awt */
import java.awt.Graphics;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Color;
import java.awt.Insets;
import java.awt.Component;
import java.awt.Cursor;

/* java.awt.event */
import java.awt.event.ActionEvent;

/* java.awt.print */
import java.awt.print.PageFormat;
import java.awt.print.Printable;
import java.awt.print.PrinterException;
import java.awt.print.PrinterJob;

/* java.awt.image */
import java.awt.image.BufferedImage;

/* javax.swing */
import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

/* javax.swing.border */
import javax.swing.border.MatteBorder;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:
 * Company:      Nokia
 * @version 1.0
 */

public class PrintPreview {

    public PrintPreview(Printable target, String title) {
        int pageWidth;
        int pageHeight;
        PreviewContainer previewContainer;

        final Printable printable = target;
        final JDialog diag = ApplicationDialogs.createDialog(title, true);

        AbstractAction cancel = new AbstractAction(ApplicationDialogs.getCancelLabel()) {
                public void actionPerformed(ActionEvent e) {
                    diag.dispose();
                }
            };
        AbstractAction print = new AbstractAction(
                                    AppEnvironment.getString(AppEnvironment.printText)) {
                public void actionPerformed(ActionEvent e) {
                    try {
                        // Use default printer, no dialog
                        PrinterJob prnJob = PrinterJob.getPrinterJob();
                        prnJob.setPrintable(printable);
                        diag.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
                        prnJob.print();
                        diag.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
                        diag.dispose();
                    } catch (PrinterException ex) {
                        ex.printStackTrace();
                        System.err.println("Printing error: "+ex.toString());
                    }
                }
            };
        Action[] actions = { print, cancel };

        previewContainer = new PreviewContainer();
        PrinterJob prnJob = PrinterJob.getPrinterJob();
        PageFormat pageFormat = prnJob.defaultPage();
        if ((pageFormat.getHeight() == 0) || (pageFormat.getWidth() == 0)) {
            System.err.println("Unable to determine default page size");
            return;
        }
        pageWidth = (int)(pageFormat.getWidth());
        pageHeight = (int)(pageFormat.getHeight());
        int scale = 40;
        int w = (int)(pageWidth * scale/100);
        int h = (int)(pageHeight * scale/100);
        int pageIndex = 0;

        try {
            while (true) {
                BufferedImage img = new BufferedImage(pageWidth,
                                        pageHeight, BufferedImage.TYPE_INT_RGB);
                Graphics g = img.getGraphics();
                g.setColor(Color.white);
                g.fillRect(0, 0, pageWidth, pageHeight);
                if (printable.print(g, pageFormat, pageIndex) != Printable.PAGE_EXISTS) {
                    break;
                }
                PagePreview pp = new PagePreview(w, h, img);
                previewContainer.add(pp);
                pageIndex++;
            }
        } catch (PrinterException e) {
            e.printStackTrace();
            System.err.println("Printing error: "+e.toString());
        }
        JScrollPane ps = new JScrollPane(previewContainer);
        LookFactory.layoutDialog(diag, ps, actions, print);
        ApplicationDialogs.invokeModalDialog(diag);
    }
}

    class PreviewContainer extends JPanel {

        protected int H_GAP = 16;
        protected int V_GAP = 10;

        public Dimension getPreferredSize() {
            int amnt = getComponentCount();
            if (amnt == 0) {
                return new Dimension(H_GAP, V_GAP);
            }
            Component comp = getComponent(0);
            Dimension d = comp.getPreferredSize();
            int w = d.width;
            int h = d.height;
            Dimension dp = getParent().getSize();
            int columns = Math.max((dp.width - H_GAP) / (w + H_GAP), 1);
            int rows = amnt / columns;
            if (rows * columns < amnt) {
                rows++;
            }
            int ww = columns * (w + H_GAP) + H_GAP;
            int hh = rows * (h + V_GAP) + V_GAP;
            Insets ins = getInsets();
            return new Dimension(ww+ins.left + ins.right, hh + ins.top + ins.bottom);
        }

        public Dimension getMaximumSize() {
            return getPreferredSize();
        }

        public Dimension getMinimumSize() {
            return getPreferredSize();
        }

        public void doLayout() {
            Insets ins = getInsets();
            int x = ins.left + H_GAP;
            int y = ins.top + V_GAP;
            int amnt = getComponentCount();
            if (amnt == 0) {
                return;
            }
            Component comp = getComponent(0);
            Dimension dc = comp.getPreferredSize();
            int w = dc.width;
            int h = dc.height;
            Dimension dp = getParent().getSize();
            int columns = Math.max((dp.width-H_GAP)/(w+H_GAP), 1);
            int rows = amnt / columns;
            if (columns * rows < amnt) {
                rows++;
            }
            int index = 0;
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < columns; j++) {
                    if (index >= amnt) {
                        return;
                    }
                    comp = getComponent(index);
                    index++;
                    comp.setBounds(x, y, w, h);
                    x += w + H_GAP;
                }
                y += h + V_GAP;
                x = ins.left + H_GAP;
            }
        }
    }

    class PagePreview extends JPanel {
        private int width;
        private int height;
        private Image source;
        private Image image;

        public PagePreview(int w, int h, Image s) {
            width = w;
            height = h;
            source = s;
            image = source.getScaledInstance(width, height, Image.SCALE_SMOOTH);
            image.flush();
            setBackground(Color.white);
            setBorder(new MatteBorder(1, 1, 2, 2, Color.black));
        }

        public void setScaledSize(int w, int h) {
            width = w;
            height = h;
            image = source.getScaledInstance(width, height, Image.SCALE_SMOOTH);
            repaint();
        }

        public Dimension getPreferredSize() {
            Insets insets = getInsets();
            return new Dimension(width + insets.left + insets.right,
                                    height + insets.top + insets.bottom);
        }

        public Dimension getMaximumSize() {
            return getPreferredSize();
        }

        public Dimension getMinimumSize() {
            return getPreferredSize();
        }

        public void paint(Graphics g) {
            g.setColor(getBackground());
            g.fillRect(0, 0, getWidth(), getHeight());
            g.drawImage(image, 0, 0, this);
            paintBorder(g);
        }
    }


