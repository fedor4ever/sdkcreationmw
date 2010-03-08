/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
 * $Id: BMPImage.java,v 1.4 2005/08/26 22:45:11 slava Exp $
 */

// ========================================================================
//  Name        : BMPImage.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================


package com.nokia.wtk.util.gui;

/* java.io */
import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;
import java.io.OutputStream;
import java.io.ByteArrayInputStream;
import java.io.InterruptedIOException;

/* java.util */
import java.util.Hashtable;

/* java.awt */
import java.awt.Image;
import java.awt.Toolkit;

/* java.awt.image */
import java.awt.image.ColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageProducer;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Trace;

/**
 * Windows BMP image.
 *
 */
public class BMPImage {
    private static final int BMP_FILE_HEADER_SIZE = 14;
    private static final int BMP_INFO_HEADER_SIZE = 40;

    private static final int BI_RGB = 0;
    private static final int BI_RLE8 = 1;
    private static final int BI_RLE4 = 2;
    private static final int BI_BITFIELDS = 3;

    private int bitCount;
    private int bitmapWidth;
    private int bitmapHeight;
    private int [] bitmapColors;
    private int [] bitmapPixels;
    private Image bitmapImage;

    /**
     * Creates empty <code>BMPImage</code>.
     */
    public BMPImage() {
        bitmapPixels = new int[0];
    }

    /**
     * Creates <code>BMPImage</code> from the data
     * @param data the data (hopfully) containing a BMP image
     * @throws IOException  if the I/O error occurs or the stream does not
     *                      appear to contains a valid BMP image
     */
    public BMPImage(byte [] data) throws IOException {
        this(new ByteArrayInputStream(data));
    }

    /**
     * Creates <code>BMPImage</code> from the input stream
     * @param in the input stream to read
     * @throws IOException  if the I/O error occurs or the stream does not
     *                      appear to contains a valid BMP image
     */
    public BMPImage(InputStream in) throws IOException {

        // Read the Windows Bitmap File Header
        //
        // -----------------------------------------------------------------
        // Size Type        Description
        // -----------------------------------------------------------------
        // 2    Character   File type (should be the characters 'B' and 'M')
        // 4    32-bit Int  Total size of the file in bytes
        // 2    16-bit Int  Reserved
        // 2    16-bit Int  Reserved
        // 4    32-bit Int  Byte-offset in file where the bits begin

        if (in.read() != 'B' || in.read() != 'M') {
            throw new ImageFormatException("Not a .BMP file");
        }
        int fileSize = readInt(in);
        in.skip(4);
        int bitmapOffset = readInt(in);

        // Read the Windows Bitmap Info Header
        //
        // -----------------------------------------------------------------
        // Size Type        Description
        // -----------------------------------------------------------------
        // 4    32-bit Int  Size (in bytes) of the info header
        // 4    32-bit Int  Width of bitmap (in pixels)
        // 4    32-bit Int  Height of bitmap (in pixels)
        // 2    16-bit Int  Number of bitplanes (should be 1)
        // 2    16-bit Int  Number of bits per pixel (should be 1,4,8, or 24)
        // 4    32-bit Int  Type of compression used
        // 4    32-bit Int  Actual number of bytes in bitmap
        // 4    32-bit Int  Number of horizontal pixels per meter
        // 4    32-bit Int  Number of vertical pixels per meter
        // 4    32-bit Int  Number of colors actually used
        // 4    32-bit Int  Number of important colors

        int bitmapInfoSize = readInt(in);
        bitmapWidth = readInt(in);
        bitmapHeight = readInt(in);
        in.skip(2);
        bitCount = readUnsignedShort(in);
        int compression = readInt(in);
        int imageSize = readInt(in);
        in.skip(8);
        int colorsUsed = readInt(in);
        int colorsImportant = readInt(in);
        if (colorsUsed == 0) colorsUsed = 1 << bitCount;

        // These are only needed if compression field is BI_BITFIELDS
        int redMask = 0;
        int greenMask = 0;
        int blueMask = 0;
        int redShift = 0;
        int greenShift = 0;
        int blueShift = 0;

        // Read the bitmap's color table
        switch (bitCount) {
        case 1:
        case 4:
        case 8:
            if (compression == BI_BITFIELDS) {
                throw new ImageFormatException("BI_BITFIELDS unexpected");
            }
            bitmapColors = new int[colorsUsed];
            for (int i=0; i<colorsUsed; i++) {
                bitmapColors[i] = (readInt(in) | 0xff000000);
            }
            break;

        case 16:
        case 24:
            if (bitmapOffset > (BMP_FILE_HEADER_SIZE+BMP_INFO_HEADER_SIZE)) {
                int n = bitmapOffset-BMP_FILE_HEADER_SIZE-BMP_INFO_HEADER_SIZE;
                if (compression == BI_BITFIELDS) {
                    if (n != 12) {
                        throw new ImageFormatException("Invalid RGB masks");
                    }
                    redMask = readInt(in);
                    greenMask = readInt(in);
                    blueMask = readInt(in);

                    // Validate masks
                    if ((redMask & greenMask & blueMask) != 0) {
                        throw new ImageFormatException("Invalid RGB masks");
                    }

                    // calculate shifts
                    int k;
                    for (k=redMask;   k!=0 && (k&1)==0; k>>=1) redShift++;
                    for (k=greenMask; k!=0 && (k&1)==0; k>>=1) greenShift++;
                    for (k=blueMask;  k!=0 && (k&1)==0; k>>=1) blueShift++;
                } else {
                    // Just skip the rest of the header
                    if (in.skip(n) < n) {
                        throw new EOFException();
                    }
                }
            } else if (compression == BI_BITFIELDS) {
                throw new ImageFormatException("Missing RGB masks");
            }
            break;

        case 0:
        case 32:
            throw new ImageFormatException("Unsupported bpp "+ bitCount);

        default:
            throw new ImageFormatException("Invalid bpp "+ bitCount);
        }

        // Create space for the pixels
        bitmapPixels = new int[bitmapWidth * bitmapHeight];

        // Read the pixels from the stream based on the compression type
        switch (compression) {
        case BI_RGB:
        case BI_BITFIELDS:
            switch (bitCount) {
            case 16:
                readRGB16(in, bitmapWidth, bitmapHeight, bitmapPixels,
                    redMask,redShift,greenMask,greenShift,blueMask,blueShift);
                break;
            case 24:
                readRGB24(in, bitmapWidth, bitmapHeight, bitmapPixels);
                break;
            default:
                readRGB(in, bitmapWidth, bitmapHeight, bitCount, bitmapPixels);
                break;
            }
            break;

        case BI_RLE8:
            readRLE(in,bitmapWidth,bitmapHeight,bitCount,bitmapPixels,imageSize,8);
            break;

        case BI_RLE4:
            readRLE(in,bitmapWidth,bitmapHeight,bitCount,bitmapPixels,imageSize,4);
            break;

        default:
            throw new ImageFormatException("Unsupported compression "+compression);
        }
    }

    /**
     * Writes <code>BMPImage</code> to the output stream.
     * @param out the output stream to write to
     * @throws IOException  if the I/O error occurs
     */
    public void write(OutputStream out) throws IOException {
        int bpp, paletteCount;
        if (bitmapColors == null) {
            paletteCount = 0;
            bpp = 24;
        } else {
            paletteCount = (1 << bitmapColors.length);
            bpp = 8;
        }

        // write the header
        writeHeader(out, bitmapWidth, bitmapHeight, bpp, paletteCount);

        if (bitmapColors != null) {
            // Write the bitmap's color table
            for (int i=0; i<paletteCount; i++) {
                writeInt(out, bitmapColors[i] & 0x00ffffff);
            }

            // write the pixels
            int n = bitmapPixels.length;
            for (int i=0; i<n; i++) {
                out.write(bitmapPixels[i]);
            }
        } else {
            // write the colors
            writeRGB24(out,bitmapWidth,bitmapHeight,bitmapPixels);
        }
    }

    /**
     * Gets the bitmap width.
     * @return the bitmap width.
     */
    public int getWidth() {
        return bitmapWidth;
    }

    /**
     * Gets the bitmap height.
     * @return the bitmap height.
     */
    public int getHeight() {
        return bitmapHeight;
    }

    /**
     * Gets the number of bits per pixel.
     * @return the number of bits per pixel.
     */
    public int getBpp() {
        return bitCount;
    }

    /**
     * Gets the bitmap pixels. Note that this method returns the reference
     * to the internally store array of pixels rather than creates a copy.
     *
     * @return the bitmap pixels.
     */
    public int [] getPixels() {
        return bitmapPixels;
    }

    /**
     * Gets the bitmap colors (palette)
     *
     * @return the bitmap colors (palette) or <code>null</code> if the
     *   bitmap has no palette
     */
    public int [] getPalette() {
        return bitmapColors;
    }

    /**
     * Gets the pixels at (x,y).
     * @param x the x coordinate of the pixel
     * @param y the y coordinate of the pixel
     * @return the pixels at (x,y).
     */
    public int getPixel(int x, int y) {
        return bitmapPixels[bitmapWidth * y + x];
    }

    /**
     * Gets the non-transparent image representing this bitmap.
     * @return the image representing this bitmap
     */
    public Image getImage() {
        if (bitmapImage == null) {
            bitmapImage = createImage(-1);
        }
        return bitmapImage;
    }

    /**
     * Create the (possibly) transparent image representing this bitmap.
     * @param t the value of the transparent pixel
     * @return the image representing this bitmap
     */
    public Image createImage(int t) {
        ImageProducer producer;
        if (bitmapColors == null) {
            producer = new MemoryImageSource(bitmapWidth, bitmapHeight,
                bitmapPixels, 0, bitmapWidth);
        } else {
            int n = bitmapColors.length;
            byte [] r = new byte[n];
            byte [] g = new byte[n];
            byte [] b = new byte[n];
            for (int i=0; i<n; i++) {
                int rgb = bitmapColors[i];
                r[i] = (byte)(rgb >> 16);
                g[i] = (byte)(rgb >> 8);
                b[i] = (byte)rgb;
            }
            ColorModel colorModel;
            if (t >= 0) {
                colorModel = new IndexColorModel(bitCount,n,r,g,b,t);
            } else {
                colorModel = new IndexColorModel(bitCount,n,r,g,b);
            }
            producer = new MemoryImageSource(bitmapWidth, bitmapHeight,
                colorModel, bitmapPixels, 0, bitmapWidth);
        }
        return Toolkit.getDefaultToolkit().createImage(producer);
    }

    /**
     * Reads an unsigned short integer from the stream in LSB first format.
     * @param in the stream to read
     * @return the number read from the stream
     * @throws IOException if an I/O error occurs
     */
    private static int readUnsignedShort(InputStream in) throws IOException {
        int byte0, byte1;
        if ((byte0 = in.read()) < 0 || (byte1 = in.read()) < 0) {
            throw new EOFException();
        }
        return (byte0 | (byte1 << 8));
    }

    /**
     * Reads a signed integer from the stream in LSB first format.
     * @param in the stream to read
     * @return the number read from the stream
     * @throws IOException if an I/O error occurs
     */
    private static int readInt(InputStream in) throws IOException {
        int byte0, byte1, byte2, byte3;
        if ((byte0 = in.read()) < 0 ||
            (byte1 = in.read()) < 0 ||
            (byte2 = in.read()) < 0 ||
            (byte3 = in.read()) < 0) {
            throw new EOFException();
        }
        return (byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24));
    }

    /**
     * Reads in pixels in 16-bit format. Oddly, all windows bitmaps are
     * stored upside-down - the bottom line is stored first.
     *
     * @param w      the bitmap width
     * @param h      the bitmap height
     * @param pixels receives the pixels
     * @param in     the stream to read
     * @param rmask  the mask for the red component
     * @param rshift the shift value for the red component
     * @param gmask  the mask for the green component
     * @param gshift the shift value for the green component
     * @param bmask  the mask for the blue component
     * @param bshift the shift value for the blue component
     * @throws IOException if an I/O error occurs
     */
    private static void readRGB16(InputStream in, int w, int h, int pixels[],
        int rmask, int rshift, int gmask, int gshift, int bmask, int bshift)
        throws IOException {

        // Bytes to skip at the end of each scan line
        int align = ((w*2) % 4) / 2;

        // Start storing at the bottom of the array
        int rscale = 256/((rmask >> rshift)+1);
        int gscale = 256/((gmask >> gshift)+1);
        int bscale = 256/((bmask >> bshift)+1);
        for (int y=h-1; y>=0; y--) {
            int pos = y * w;
            for (int x=0; x<w; x++) {
                int c = readUnsignedShort(in);
                int r = ((c & rmask) >> rshift) * rscale;
                int g = ((c & gmask) >> gshift) * gscale;
                int b = ((c & bmask) >> bshift) * bscale;
                pixels[pos++] = (0xff000000 | (r << 16) | (g << 8) | b);
            }
            if (align > 0) {
                in.skip(align);
            }
        }
    }

    /**
     * Reads in pixels in 24-bit format. There is no color table, and the
     * pixels are stored in 3-byte blocks. Oddly, all windows bitmaps are
     * stored upside-down - the bottom line is stored first.
     *
     * @param in     the stream to read
     * @param w      the bitmap width
     * @param h      the bitmap height
     * @param pixels receives the pixels
     * @throws IOException if an I/O error occurs
     */
    private static void readRGB24(InputStream in, int w, int h, int pixels[])
    throws IOException {

        // Bytes to skip at the end of each scan line
        int align = ((((w*3) % 4) == 0) ? 0 : (4 - ((w*3) % 4)));

        // Start storing at the bottom of the array
        for (int y = h-1; y>=0; y--) {
            int pos = y * w;
            for (int x=0; x<w; x++) {

                // Read in the red, green, and blue components (BGR)
                int r,g,b;
                if ((b = in.read()) < 0 ||
                    (g = in.read()) < 0 ||
                    (r = in.read()) < 0) {
                    throw new EOFException();
                }

                // Turn the red, green, and blue values into an RGB color with
                // an alpha value of 255 (fully opaque)
                pixels[pos++] = (0xff000000 | (r << 16) | (g << 8) | b);
            }
            if (align > 0) {
                in.skip(align);
            }
        }
    }

    /**
     * Write a signed integer to the stream in LSB first format.
     * @param out the stream to write to
     * @param n the number to write
     * @throws IOException if an I/O error occurs
     */
    private static void writeInt(OutputStream out, int n) throws IOException {
        out.write(n & 0xff);
        out.write((n >> 8) & 0xff);
        out.write((n >> 16) & 0xff);
        out.write((n >> 24) & 0xff);
    }

    /**
     * Write a signed short integer to the stream in LSB first format.
     * @param out the stream to write to
     * @param n the number to write
     * @throws IOException if an I/O error occurs
     */
    private static void writeShort(OutputStream out, int n)
    throws IOException {
        out.write(n & 0xff);
        out.write((n >> 8) & 0xff);
    }

    /**
     * Reads in pixels values that are stored uncompressed.
     * The bits represent indices into the color table.
     * All windows bitmaps are stored upside-down -
     * the bottom line is stored first.
     *
     * @param width         the bitmap width
     * @param height        the bitmap height
     * @param bitCount      number of bits per pixel
     * @param pixels        receives the pixels
     * @param in            the stream to read
     * @throws IOException  if an I/O error occurs
     */
    private static void readRGB(InputStream in, int width, int height,
        int bitCount, int pixels[]) throws IOException {

        // How many pixels can be stored in a byte?
        int pixelsPerByte = 8 / bitCount;

        // A bit mask containing the number of bits in a pixel
        int bitMask = (1 << bitCount) - 1;

        // The shift values that will move each pixel to the far right
        int bitShifts[] = new int[pixelsPerByte];
        for (int i=0; i < pixelsPerByte; i++) {
            bitShifts[i] = 8 - ((i+1) * bitCount);
        }

        int rowSize = ((width*bitCount + 7)/8);
        int alignedRowSize = (rowSize + 3) & (-4);

        // Start at the bottom of the pixel array and work up
        for (int y=height-1; y >= 0; y--) {
            int currByte = 0;
            int whichBit = pixelsPerByte;
            int pos = y * width;
            for (int x=0; x < width; x++) {

                // If the current bit position is past the number of pixels in
                // a byte, we advance to the next byte
                if (whichBit >= pixelsPerByte) {
                    whichBit = 0;
                    currByte = in.read();
                }

                // Get the next pixel from the current byte
                pixels[pos++] = (currByte >> bitShifts[whichBit++]) & bitMask;

            }
            if (alignedRowSize > rowSize) {
                in.skip(alignedRowSize - rowSize);
            }
        }
    }

    /**
     * Reads run-length encoded data in either RLE4 or RLE8 format.
     *
     * @param width         the bitmap width
     * @param height        the bitmap height
     * @param bitCount      number of bits per pixel
     * @param pixels        receives the pixels
     * @param in            the stream to read
     * @param imageSize     the total size of the image
     * @param pixelSize     the sizae of the pixel
     * @throws IOException  if an I/O error occurs
     */
    private static void readRLE(InputStream in, int width, int height,
        int bitCount, int pixels[], int imageSize, int pixelSize)
        throws IOException {
        int x = 0;
        int y = height-1;

        // padded width
        int padw = (((width+3)/4)*4);

        // You already know how many bytes are in the image, so only go
        // through that many.
        for (int i=0; i < imageSize;) {

            // RLE encoding is defined by two bytes
            int byte1 = in.read();
            int byte2 = in.read();
            i += 2;

            // If byte 0 == 0, this is an escape code
            if (byte1 == 0) {

                // If escaped, byte 2 == 0 means you are at end of line
                if (byte2 == 0) {
                    if (x > 0) {
                        x = 0;
                        y--;
                    }

                // If escaped, byte 2 == 1 means end of bitmap
                } else if (byte2 == 1) {
                    return;

                // if escaped, byte 2 == 2 adjusts the current x and y by
                // an offset stored in the next two words
                } else if (byte2 == 2) {
                    int xoff = (char)readUnsignedShort(in);
                    i+= 2;
                    int yoff = (char)readUnsignedShort(in);
                    i+= 2;
                    x += xoff;
                    y -= yoff;

                // If escaped, any other value for byte 2 is the number
                // of pixels that you should read as pixel values (these
                // pixels are not run-length encoded)
                } else {
                    int istart = i;
                    if (pixelSize == 4) {
                        int currByte = 0;
                        for (int j=0; j<byte2; j++) {
                            if ((j & 1) == 0) {
                                // Read in the next byte
                                currByte = in.read();
                                i++;
                            }
                            if (x < width) {
                                // The pixels are 4-bits, so half the time
                                // you shift the current byte to the right
                                // as the pixel value
                                if ((j & 1) == 0) {
                                    pixels[y*width+x] = ((currByte>>4) & 0xf);
                                } else {
                                    // The rest of the time, you mask out the
                                    // upper 4 bits
                                    pixels[y*width+x] = (currByte & 0xf);
                                }
                            }
                            x++;
                            if (x >= padw) {
                                x = 0;
                                y--;
                            }
                        }
                    } else {
                        for (int j=0; j<byte2; j++) {
                            int currByte = in.read();
                            i++;

                            if (x < width) {
                                pixels[y*width+x] = currByte;
                            }
                            x++;
                            if (x >= padw) {
                                x = 0;
                                y--;
                            }
                        }
                    }

                    // The pixels must be word-aligned, so if you read an
                    // uneven number of bytes, read and ignore a byte to get
                    // aligned again.
                    if (((i-istart) & 1) == 1) {
                        in.read();
                        i++;
                    }
                }


            // If the first byte was not 0, it is the number of pixels that
            // are encoded by byte 2
            } else {
                if (pixelSize == 4) {
                    for (int j=0; j<byte1; j++) {
                        if (x < width) {
                            // If j is odd, use the upper 4 bits
                            if ((j & 1) == 0) {
                                pixels[y*width+x] = ((byte2 >> 4) & 0xf);
                            } else {
                                pixels[y*width+x] = (byte2 & 0xf);
                            }
                        }
                        x++;
                        if (x >= padw) {
                            x = 0;
                            y--;
                        }
                    }
                } else {
                    for (int j=0; j<byte1; j++) {
                        if (x < width) {
                            pixels[y*width+x] = (byte2);
                        }
                        x++;
                        if (x >= padw) {
                            x = 0;
                            y--;
                        }
                    }
                }
            }
        }
    }

    //======================================================================
    //              W R I T E
    //======================================================================

    public static void write16(Image image, OutputStream out)
        throws IOException {
        write(image, out, 0xF800, 0x07E0, 0x001F);
    }

    public static void write(Image image, OutputStream out,
        int rmask, int gmask, int bmask) throws IOException {

        if (rmask == 0 ||
            gmask == 0 ||
            bmask == 0 ||
            (rmask & gmask & bmask) != 0 ||
            ((rmask | gmask | bmask) & 0xffff0000) != 0) {
            throw new IllegalArgumentException("invalid masks");
        }

        Consumer consumer = new Consumer();
        consumer.consume(image.getSource());
        synchronized (consumer) {
            if (!consumer.isComplete()) {
                try {
                    consumer.wait();
                } catch (InterruptedException x) {
                    Trace.printStackTrace(x);
                    throw new InterruptedIOException("image load failed");
                }
            }
        }

        int loadStatus = consumer.getLoadStatus();
        if (loadStatus == ImageConsumer.IMAGEERROR ||
            loadStatus == ImageConsumer.IMAGEABORTED) {
            throw new InterruptedIOException("image load error " + loadStatus);
        }

        // write the header
        int w = consumer.getWidth();
        int h = consumer.getHeight();
        writeHeader(out, w, h, 16, 0);

        // write the bitfields
        writeInt(out, rmask);
        writeInt(out, gmask);
        writeInt(out, bmask);

        // write the data
        writeRGB16(out, w, h, rmask, gmask, bmask, consumer.getBytes());
    }

    public static void write(Image image, OutputStream out)
    throws IOException {
        Consumer consumer = new Consumer();
        consumer.consume(image.getSource());
        synchronized (consumer) {
            if (!consumer.isComplete()) {
                try {
                    consumer.wait();
                } catch (InterruptedException x) {
                    Trace.printStackTrace(x);
                    throw new InterruptedIOException("image load failed");
                }
            }
        }

        int loadStatus = consumer.getLoadStatus();
        if (loadStatus == ImageConsumer.IMAGEERROR ||
            loadStatus == ImageConsumer.IMAGEABORTED) {
            throw new InterruptedIOException("image load error " + loadStatus);
        }

        // write the header
        int w = consumer.getWidth();
        int h = consumer.getHeight();
        writeHeader(out, w, h, 24, 0);

        // write the data
        writeRGB24(out, w, h, consumer.getBytes());
    }

    /**
     * Writes the bitmap header to the output stream.
     *
     * @param out the output stream to write to
     * @param bitmapWidth the bitmap width
     * @param bitmapHeight the bitmap height
     * @param bpp number of bits per pixel (should be either 16 or 24)
     * @param paletteCount number of entries in the palette
     * @throws IOException  if the I/O error occurs
     */
    private static void writeHeader(OutputStream out, int bitmapWidth,
        int bitmapHeight, int bpp, int paletteCount)
        throws IOException {

        // calculate file size
        int bitFields = ((bpp == 16) ? 12 : 0);
        int bmpsize = BMP_INFO_HEADER_SIZE + bitFields + 4 * paletteCount;
        int rowsize = (bitmapWidth * bpp / 8 + 3) & (-4);
        int datasize = bitmapHeight * rowsize;

        int dataOffset = BMP_FILE_HEADER_SIZE + bmpsize;
        int fileSize = dataOffset + datasize;

        // Write the Windows Bitmap File Header
        //
        // -----------------------------------------------------------------
        // Size Type        Description
        // -----------------------------------------------------------------
        // 2    Character   File type (should be the characters 'B' and 'M')
        // 4    32-bit Int  Total size of the file in bytes
        // 2    16-bit Int  Reserved
        // 2    16-bit Int  Reserved
        // 4    32-bit Int  Byte-offset in file where the bits begin

        out.write('B');
        out.write('M');
        writeInt(out, fileSize);
        writeInt(out, 0);
        writeInt(out, dataOffset);

        // Write the Windows Bitmap Info Header
        //
        // -----------------------------------------------------------------
        // Size Type        Description
        // -----------------------------------------------------------------
        // 4    32-bit Int  Size (in bytes) of the info header
        // 4    32-bit Int  Width of bitmap (in pixels)
        // 4    32-bit Int  Height of bitmap (in pixels)
        // 2    16-bit Int  Number of bitplanes (should be 1)
        // 2    16-bit Int  Number of bits per pixel (should be 1,4,8, or 24)
        // 4    32-bit Int  Type of compression used
        // 4    32-bit Int  Actual number of bytes in bitmap
        // 4    32-bit Int  Number of horizontal pixels per meter
        // 4    32-bit Int  Number of vertical pixels per meter
        // 4    32-bit Int  Number of colors actually used
        // 4    32-bit Int  Number of important colors
        writeInt(out, BMP_INFO_HEADER_SIZE);
        writeInt(out, bitmapWidth);
        writeInt(out, bitmapHeight);
        writeShort(out, 1);
        writeShort(out, bpp);
        writeInt(out, (bitFields != 0) ? BI_BITFIELDS : BI_RGB);
        writeInt(out, datasize);
        writeInt(out, 2835);  // 72dpi
        writeInt(out, 2835);  // 72dpi
        writeInt(out, paletteCount);
        writeInt(out, 0);
    }

    /**
     * Writes pixels in 24-bit format. There is no color table, and the
     * pixels are stored in 3-byte blocks. Oddly, all windows bitmaps are
     * stored upside-down - the bottom line is stored first.
     *
     * @param w      the bitmap width
     * @param h      the bitmap height
     * @param pixels the pixels to write
     * @param out    the stream to write to
     * @throws IOException if an I/O error occurs
     */
    private static void writeRGB24(OutputStream out,int w,int h,int pixels[])
    throws IOException {
        // Bytes to add at the end of each scan line
        int align = ((((w*3) % 4) == 0) ? 0 : (4 - ((w*3) % 4)));

        // Start writing from the bottom of the array
        for (int y=h-1; y>=0; y--) {
            int pos = y * w;
            for (int x=0; x<w; x++) {
                int c = pixels[pos++];
                out.write(c & 0xff);
                out.write((c >> 8) & 0xff);
                out.write((c >> 16) & 0xff);
            }
            // align at word boundary
            for (int i=0; i<align; i++) {
                out.write(0);
            }
        }
    }

    /**
     * Writes pixels in 24-bit format. There is no color table, and the
     * pixels are stored in 3-byte blocks. Oddly, all windows bitmaps are
     * stored upside-down - the bottom line is stored first.
     *
     * @param w      the bitmap width
     * @param h      the bitmap height
     * @param rmask  the red bitmask
     * @param gmask  the green bitmask
     * @param bmask  the blue bitmask
     * @param pixels the pixels to write
     * @param out    the stream to write to
     * @throws IOException if an I/O error occurs
     */
    private static void writeRGB16(OutputStream out, int w, int h,
        int rmask, int gmask, int bmask, int pixels[]) throws IOException {

        // calculate shifts
        int rshift = 0; while ((rmask & 1) == 0) { rmask >>= 1; rshift++; }
        int gshift = 0; while ((gmask & 1) == 0) { gmask >>= 1; gshift++; }
        int bshift = 0; while ((bmask & 1) == 0) { bmask >>= 1; bshift++; }
        int rbits = 0; while ((rmask & (1 << rbits)) != 0) rbits++;
        int gbits = 0; while ((gmask & (1 << gbits)) != 0) gbits++;
        int bbits = 0; while ((bmask & (1 << bbits)) != 0) bbits++;

        // Bytes to add at the end of each scan line
        int align = ((w*2) % 4) / 2;

        // Start writing from the bottom of the array
        for (int y=h-1; y>=0; y--) {
            int pos = y * w;
            for (int x=0; x<w; x++) {
                int c = pixels[pos++];
                int b = (c & 0xff);
                int g = ((c >> 8) & 0xff);
                int r = ((c >> 16) & 0xff);
                writeShort(out, (short)(((b >> (8-bbits)) << bshift) |
                                       ((g >> (8-gbits)) << gshift) |
                                       ((r >> (8-rbits)) << rshift)));
            }
            // align at word boundary
            for (int i=0; i<align; i++) {
                out.write(0);
            }
        }
    }

    /**
     * Receives the image and stores it in the int array
     */
    private static class Consumer implements ImageConsumer {
        private int [] data;
        private int width;
        private int height;
        private int loadStatus = IMAGEERROR;
        private boolean complete = false;
        private ImageProducer producer;

        void consume(ImageProducer source) {
            complete = false;
            producer = source;
            producer.startProduction(this);
        }

        int [] getBytes() {
            return data;
        }

        int getWidth() {
            return width;
        }

        int getHeight() {
            return height;
        }

        int getLoadStatus() {
            return loadStatus;
        }

        boolean isComplete() {
            return complete;
        }

        public void setDimensions(int w, int h) {
            width = w;
            height = h;
            data = new int[width * height];
        }

        public void setProperties(Hashtable props) {}
        public void setColorModel(ColorModel model) {}
        public void setHints(int hintflags) {}
        public synchronized void imageComplete(int status) {
            complete = true;
            loadStatus = status;
            producer.removeConsumer(this);
            notifyAll();
        }

        /**
         * Delivers the pixels of the image with one or more calls
         * to this method.  Each call specifies the location and
         * size of the rectangle of source pixels that are contained in
         * the array of pixels.  The specified ColorModel object should
         * be used to convert the pixels into their corresponding color
         * and alpha components.  Pixel (m,n) is stored in the pixels array
         * at index (n * scansize + m + off).  The pixels delivered using
         * this method are all stored as bytes.
         * @param x the x coordinate of the upper-left corner of the
         *        area of pixels to be set
         * @param y the y coordinate of the upper-left corner of the
         *        area of pixels to be set
         * @param w the width of the area of pixels
         * @param h the height of the area of pixels
         * @param model the specified <code>ColorModel</code>
         * @param pix the array of pixels
         * @param off the offset into the <code>pixels</code> array
         * @param scansize the distance from one row of pixels to the next in
         * the <code>pixels</code> array
         * @see ColorModel
         */
        public void setPixels(int x, int y, int w, int h,
            ColorModel model, byte pix[], int off, int scansize) {
            for (int dy=0; dy<h; dy++) {
                int y1 = y + dy;
                int srcoff = off + scansize * dy;
                for (int dx=0; dx<w; dx++) {
                    data[y1*width+x+dx] = model.getRGB(pix[srcoff+dx]&0xff);
                }
            }
        }

        /**
         * The pixels of the image are delivered using one or more calls
         * to the setPixels method.  Each call specifies the location and
         * size of the rectangle of source pixels that are contained in
         * the array of pixels.  The specified ColorModel object should
         * be used to convert the pixels into their corresponding color
         * and alpha components.  Pixel (m,n) is stored in the pixels array
         * at index (n * scansize + m + off).  The pixels delivered using
         * this method are all stored as ints.
         * this method are all stored as ints.
         * @param x the x coordinate of the upper-left corner of the
         *        area of pixels to be set
         * @param y the y coordinate of the upper-left corner of the
         *        area of pixels to be set
         * @param w the width of the area of pixels
         * @param h the height of the area of pixels
         * @param model the specified <code>ColorModel</code>
         * @param pix the array of pixels
         * @param off the offset into the <code>pixels</code> array
         * @param scansize the distance from one row of pixels to the next in
         * the <code>pixels</code> array
         * @see ColorModel
         */
        public void setPixels(int x, int y, int w, int h,
            ColorModel model, int pix[], int off, int scansize) {
            for (int dy=0; dy<h; dy++) {
                int y1 = y + dy;
                int srcoff = off + scansize * dy;
                for (int dx=0; dx<w; dx++) {
                    data[y1*width+x+dx] = model.getRGB(pix[srcoff+dx]);
                }
            }
        }
    }
}
