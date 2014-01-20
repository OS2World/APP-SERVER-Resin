/*
 * Copyright 1998-1998 Caucho Technology -- all rights reserved
 *
 * Caucho Technology forbids redistribution of any part of this software
 * in any form, including derived works and generated binaries.
 *
 * This Software is provided "AS IS," without a warranty of any kind. 
 * ALL EXPRESS OR IMPLIED REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.

 * CAUCHO TECHNOLOGY AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES
 * SUFFERED BY LICENSEE OR ANY THIRD PARTY AS A RESULT OF USING OR
 * DISTRIBUTING SOFTWARE. IN NO EVENT WILL CAUCHO OR ITS LICENSORS BE LIABLE
 * FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL,
 * CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND
 * REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF OR
 * INABILITY TO USE SOFTWARE, EVEN IF HE HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGES.      
 *
 * @author Scott Ferguson
 *
 * $Id: Navigation.java,v 1.2 2000/01/04 18:33:26 ferg Exp $
 */

package com.caucho.web;

import java.util.*;
import java.io.*;

import org.w3c.dom.*;

import com.caucho.util.*;
import com.caucho.vfs.*;
import com.caucho.xml.*;
import com.caucho.xpath.*;

public class Navigation {
  private Document doc;
  private Tree top;

  public Navigation(Path pwd)
    throws IOException
  {
    doc = new Xml().parse(pwd);
    top = new Tree(null);

    Node node = XPath.find("navigation", doc);
    if (node != null)
      fillChildren(top, node.getFirstChild());
  }

  public String getTop()
  {
    try {
      return XPath.evalString("/navigation/@top", doc);
    } catch (Exception e) {
      return "/";
    }
  }

  private void fillChildren(Tree tree, Node childNode)
  {
    for (; childNode != null; childNode = childNode.getNextSibling()) {
      if (! childNode.getNodeName().equals("item"))
	continue;

      try {
	Element elt = (Element) childNode;

	NavItem item = new NavItem();
	if (elt.getAttribute("link") != null)
	  item.setLink(elt.getAttribute("link"));
	else
	  item.setLink(XPath.evalString("link", elt));

	if (elt.getAttribute("title") != null)
	  item.setTitle(elt.getAttribute("title"));
	else
	  item.setTitle(XPath.evalString("title", elt));

	if (elt.getAttribute("brief") != null)
	  item.setBrief(elt.getAttribute("brief"));
	else
	  item.setBrief(XPath.evalString("brief", elt));

	if (item.getBrief() == null || item.getBrief().equals(""))
	  item.setBrief(item.getTitle());

	if (elt.getAttribute("description") != null)
	  item.setDescription(elt.getAttribute("description"));
	else
	  item.setDescription(XPath.evalString("description", elt));

	Tree child = tree.append(item);
	item.setTree(child);
	fillChildren(child, childNode.getFirstChild());
      } catch (XPathParseException e) {
      }
    }
  }

  public NavItem findLink(String url)
  {
    int p = url.lastIndexOf('/');
    if (p >= 0)
      url = url.substring(p + 1);

    Iterator iter = top.dfs();
    while (iter.hasNext()) {
      Tree tree = (Tree) iter.next();
      NavItem item = (NavItem) tree.getData();

      if (item.getLink().equals(url))
	return item;
    }

    return null;
  }
}
