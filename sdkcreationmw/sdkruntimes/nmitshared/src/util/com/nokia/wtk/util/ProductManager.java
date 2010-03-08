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


package com.nokia.wtk.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.LineNumberReader;

import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Text;

/**
 * Discovers the NMIT 4.0 family installed products on the machine.
 * @see Installation design doc.
 */

public class ProductManager {

  /**
   * Gets the shared ProductManager instance.
   * @return    the singleton instance.
   */
  public static synchronized ProductManager getProductManager() {
    if (productManager == null) {
      productManager = new ProductManager();
    }
    return productManager;
  }

  /**
   * Gets an array of installed products.
   * @return  an array of productss or an zero lenght array if products
   *          not found.
   */
  public InstalledProduct[] getInstalledProducts() {
    return this.products;
  }

  /**
   * Gets <code>InstalledProduct</code> by product name.
   * @param name   the name of the product to search for.
   * @return the <code>InstalledProduct</code> or <code>null</code> if not
   * installed.
   */
  public InstalledProduct getInstalledProduct(String productName) {
    if (productName != null) {
      for (int i = 0; i < products.length; i++) {
        if (productName.equalsIgnoreCase(products[i].getName())) {
          return products[i];
        }
      }
    }
    return null;
  }

  /**
   * Interface that represents an installed product.
   */
  public interface InstalledProduct {

    /**
     * Gets the type of product
     * @return the product type
     */
    public ProductType getType();

    /**
     * Gets the name of the product.
     * @return the name of the product
     */
    public String getName();

    /**
     * Gets the version of the product.
     * @return the version of the product
     */
    public String getVersion();

    /**
     * Gets the installed location of the product.
     * @return the installed location of the product
     */
    public File getLocation();

    /**
     * Gets the absolute path of the product execuatble.
     * @return the absolute path of the product execuatble; or <code>null</code>
     * if executable name not specified.
     */
    public String getExecutable();
  }

  /**
   * Represents the installed product type.
   */
  public static class ProductType {

    String type = null;
    private ProductType(String type) {
      this.type = type;
    }

    public final static ProductType EMULATOR = new ProductType("emulator");
    public final static ProductType SERVER = new ProductType("server");
    public final static ProductType TOOL = new ProductType("tool");
    public final static ProductType UNKNOWN = new ProductType("unknown");

    private static final ProductType TYPES[] = {EMULATOR, SERVER, TOOL,UNKNOWN};

    public String toString() {
      return type;
    }

    public boolean equals(ProductType product) {
      return type.equals(product);
    }

    private static ProductType createProductType(String text) {
      for (int i = 0; i < TYPES.length; i++) {
        if (TYPES[i].toString().equals(text)) {
          return TYPES[i];
        }
      }
      return UNKNOWN;
    }
  }

  /** The installation registry pointer file */
  private static final String POINTER_FILE_NAME = "registryPointer";

  /** The installation registry file */
  private static final String REGISTRY_FILE_NAME = "installationRegistry.xml";

  /** The shared instance */
  private static ProductManager productManager = null;

  /** List of installed products */
  private InstalledProduct products[] = null;

  /** products discovery done ? */
  private boolean discovered = false;

  /**
   * Private constructor.
   */
  private ProductManager() {
    products = new InstalledProduct [0];
    discoverProducts();
  }

  /**
   * Discovers the installed products if not discovered already.
   */
  private void discoverProducts() {
    String[] prods = new String [0];
    /* Discover products upon first call of this method */
    if (!discovered) {
      try {
        /* Get the pointer file from the current directory */
        String currDir = System.getProperty("user.dir");
        LineNumberReader pointer = new LineNumberReader(new BufferedReader(new
        FileReader(new File(currDir, POINTER_FILE_NAME))));
        String regPointer = pointer.readLine();/* Installation Registry dir */

        /* Get the emulator's locations from the Installation Registry file */
        File regFile = new File(regPointer, REGISTRY_FILE_NAME);
        ProductsRegistry r = new ProductsRegistry(regFile);
        this.products = r.getProducts();
        /*
        StringBuffer s = new StringBuffer();
        for (int i = 0; i < prods.length; i++) {
          s.append(prods[i] + "\n");
        }
        if (prods.length == 0) { s.append("None.");}
        debugOut("Discovered products are: " + s.toString());
        */
      /*@todo: Inform tool about these failures */
      } catch (FileNotFoundException f){
        debugOut(POINTER_FILE_NAME + " pointer file not found.");
      } catch (IOException io) {
        debugOut("Reading from pointer file failed.");
      } catch (RegistryException r) {
        debugOut(r.getMessage());
      }
      discovered = true; /* Device search done */
    }
  }


  private static final String DEBUG_PREFIX = "ProductLib: ";
  static void debugOut(String message) {
    //System.out.println(DEBUG_PREFIX + message);
  }


  /**
   * Inner class representing the instaled product.
   */
  private static class Product implements InstalledProduct{
    private ProductType type;
    private String name;
    private String version;
    private File location;
    private String exe;

    /**
     * Constructs the product
     */
    Product(ProductType type) {
      if (type == null) {
        throw new IllegalArgumentException("Invalid Parameters.");
      }
      this.type = type;
    }

    /* Getter methods */
    public ProductType getType() {
      return type;
    }
    public String getName() {
      return name;
    }
    public String getVersion() {
      return version;
    }
    public File getLocation() {
      return location;
    }
    public String getExecutable() {
      return exe;
    }

    /* Setter methods */
    public void setName(String n) {
      name = n;
    }
    public void setVersion(String ver) {
      version = ver;
    }
    public void setLocation(File loc) {
      location = loc;
    }
    public void setExecutable(String e) {
      exe = e;
    }
  }

  /**
   * Represents the product registry.
   * @See Installation design specs
   */
  private static class ProductsRegistry {

   /* Constants */
    private final static String ELEMENT_REGISTRY = "registry";
    private final static String ELEMENT_PRODUCT = "product";
    private final static String ELEMENT_NAME = "name";
    private final static String ELEMENT_VERSION = "version";
    private final static String ELEMENT_LOCATION = "location";
    private final static String ELEMENT_EXECUTABLE = "executable";
    private final static String ATTRIBUTE_TYPE = "type";
    private final static String NL = "\r\n";
    private final static String LARROW = "<";
    private final static String LCARROW = "</"; /* left closing arrow */
    private final static String RARROW = ">";
    private final static String SPACE = " ";
    private final static String EQ = "=";
    private final static String QUOTE = "\"";

    /* The DOM of the product registry data */
    private Document registry = null;

    /**
     * Creats the registry based on the specified file.
     * @exception throws <code>RegistryException</code> if error occours
     * while reading the products data from the specified registry.
     */
    ProductsRegistry(File reg) throws RegistryException {
      try {
        registry =
        DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(reg);
      } catch(Exception e) {
        //e.printStackTrace();
        throw new RegistryException(
          "Error parsing the installation registry: " + reg.getAbsolutePath());
      }
    }

    /**
     * Gets the installed products from the registry.
     * @return the installed products
     */
    public Product[] getProducts() {
      Product[] prods = new Product[0];
      NodeList es = registry.getElementsByTagName(ELEMENT_REGISTRY);
      if (es == null || es.getLength() != 1) { return prods; }

      Node reg = es.item(0); /* the 'registry' element */
      NodeList products = reg.getChildNodes(); /* 'product' element list */
      if (products == null) {return prods;} /* no 'product' element found */

      for (int i = 0; i < products.getLength(); i++) {
        Product p = null;
        Node product = products.item(i); /* 'product' element */

        if (product != null) {
          NamedNodeMap prodAttrs = product.getAttributes();
          if (prodAttrs != null) {

            for(int j = 0; j < prodAttrs.getLength(); j++) {
              Node n = prodAttrs.item(j);
            }

            /* extract 'type' attribute */
            Attr typeAttr = (Attr) prodAttrs.getNamedItem(ATTRIBUTE_TYPE);
            if (typeAttr != null) {

              /* extract 'type' attribute value */
              String typeValue = typeAttr.getValue();
              p = new Product(ProductType.createProductType(typeValue));
              p.setName(getSubElementText((Element)product, ELEMENT_NAME));
              p.setVersion(getSubElementText((Element)product,ELEMENT_VERSION));
              File loc =
                new File(getSubElementText((Element)product,ELEMENT_LOCATION));
              p.setLocation(loc);
              p.setExecutable(
                getSubElementText((Element)product,ELEMENT_EXECUTABLE));

              prods = (Product[])ArrayUtils.add(prods, p);
            }
          }
        }
      }
      return prods;
    }

    /**
     * Gets the text from the specified sub-element of the specified element.
     * @param e the element containing sub-element
     * @param subElemName the tag name of the element
     * @return the text from the specified sub-element of the specified element.
     */
    private static String getSubElementText(Element e, String subElemName) {
      String eText = null;
      Element product = e;
      /* product:sub element list */
      NodeList subElms =
      ((Element)product).getElementsByTagName(subElemName);

      /* should have only one sub element of this type within product */
      if (subElms != null && subElms.getLength() == 1) {
        /* extract product:sub element */
        Element sub = (Element) subElms.item(0);
        if (sub != null) {
          NodeList textElms = sub.getChildNodes();

          /* should have only one text element within sub */
          if (textElms != null && textElms.getLength() == 1) {
            Node text = textElms.item(0);

            /* should be sub text node */
            if (text instanceof Text) {
                 eText = text.getNodeValue().trim();
            }
          }
        }
      }

      return eText;
    }
  } //End of ProductRegistry

  /**
   * Exception class for registry parsing and maintaining.
   */
  private static class RegistryException extends RuntimeException {
    RegistryException(String msg) {
      super(msg);
    }
  }

  public static void main(String args[]) {
    ProductManager p = ProductManager.getProductManager();
    InstalledProduct prods[] = p.getInstalledProducts();
    for (int i = 0; i < prods.length; i++) {
      System.out.print(prods[i].getName());
    }
    InstalledProduct prod = p.getInstalledProduct("Update Manager");
    if (prod != null) {
      System.out.print("Prod Name:" + prod.getName());
    } else {
      System.out.print("NULL");
    }
  }
}

