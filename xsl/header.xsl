<!--
   - Formats the top level of the template.  header.xsl assumes the XTP
   - has been read in as HTML.  So it can assume the existence of the
   - html and body tags.
  -->

<xsl:stylesheet parsed-content=false>
<xsl:output disable-output-escaping=yes/>
<#@ page import='com.caucho.web.*' import='com.caucho.vfs.*' #>
<#@ cache #>
<#!
  String top = "/";
  String title = null;
  Navigation nav = null;

  String topnav(Object a, Object b, Object c) { return ""; }

  void initNavigation(XslWriter out)
    throws IOException
  {
    PageContext page = out.getPage();
    ServletContext app = page.getServletContext();
    HttpServletRequest req = (HttpServletRequest) page.getRequest();
    String realPath = req.getRealPath(req.getServletPath());

    Path path = out.getPwd().lookupNative(realPath).lookup("../toc.xml");
    out.addCacheDepend(path);

    nav = new Navigation(path);

    top = nav.getTop();
    if (top == null || top == "")
      top = "/";
  }

  void writeFamilyNavigation(XslWriter out)
  {
    PageContext page = out.getPage();
    HttpServletRequest req = (HttpServletRequest) page.getRequest();

    NavItem item = nav.findLink(req.getRequestURI());

    if (item == null || item.getParent() == null)
      return;

    out.println("<table width='100%' border='0' cellpadding='0'>");

    printParents(out, item.getParent());

    out.println("<tr><td><hr>");

    Iterator iter = item.getParent().children();
    while (iter.hasNext()) {
      NavItem peer = (NavItem) iter.next();
      out.println("<tr><td><a href='" + peer.getLink() + "'>" + peer.getBrief() +
                  "</a>");
    }
    iter = item.children();
    if (iter.hasNext()) {
      out.println("<tr><td><hr>");
    }
    while (iter.hasNext()) {
      NavItem peer = (NavItem) iter.next();
      out.println("<tr><td><a href='" + peer.getLink() + "'>" + peer.getBrief() +
                  "</a>");
    }
    out.println("</table>");
  }

  void printParents(XslWriter out, NavItem item)
  {
     if (item == null)
       return;

     printParents(out, item.getParent());
     out.println("<tr><td><a href='" + item.getLink() + "'>" + item.getBrief() +
                  "</a>");
  }
#>

html
<<
  <# 
     initNavigation(out);
     title = XPath.evalString("head/title", node);
     if (title == null)
       title = "";
  #>
  <html>
  <head>
  <title><#= title #></title>
  <link rel="STYLESHEET" href="<#= top #>css/default.css" type="text/css">
  </head>

  <xsl:apply-templates select='body'/>
  </html>
>>

html/body
<<
  <body bgcolor=white background="<#= top #>images/background.gif">

  <table cellpadding="2" cellspacing="0" border="0" width="100%" summary="">
  <tr valign="top"><td width="120">
    <table cellspacing="0" cellpadding="0" border="0" width="100%">
    <tr><td>
      <img src="<#= top #>images/caucho.gif" width="120" height="40" alt="caucho"><br>
      <!-- Left Navigation -->
      <# writeFamilyNavigation(out); #>
    </td></tr>
    </table>
  </td>
  <td width="30">
    <img src="<#= top #>images/pixel.gif" alt="" width="30" height="1">
  </td>
  <td width="*">
    <xsl:comment> top navigation </xsl:comment>
    <table width="100%" cellspacing="0" cellpadding="0" border="0" summary="">
    <tr class="toptitle">
      <td width="90%" background="<#= top#>images/hbleed.gif">
        <font class=toptitle size="+3">
          &nbsp;<#= title #>
        </font>
      </td>
    </tr>
    </table>
  <!-- vspace -->
  <img src="<#= top #>images/pixel.gif" alt="" width="1" height="20"><br>

  <!-- Actual Contents -->
  <xsl:apply-templates/>

  <!-- footer -->
  <hr>
  <!-- <# if (nav.isThreaded()) nav.writePrevNext(out, filename); #> -->
  <table border=0 cellspacing=0 width='100%'>
  <tr><td><em>Copyright &copy; 1998-2000 Caucho Technology. All rights reserved.</em>
    </td>
    <td align=right><img src="<#= top #>images/logo.gif" width=96 height=32></td>
  </tr>
  </table>

  </td>
  <td width="20%"></td>
  </tr>
  </table>
  </body>
>>

</xsl:stylesheet>
