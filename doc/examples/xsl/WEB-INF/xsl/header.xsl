<xsl:stylesheet>
<xsl:output method='html' disable-output-encoding=true/>

<#!
  import Navigation;

  var top

  function initNavigation(document, application)
  {
    navigation = Navigation.getNavigation(application);

    top = navigation.toc.documentElement.attribute.top;
  }
#>

<xsl:template match="html">
  <# initNavigation(node.ownerDocument, out.page.servletContext) #>
  <html>
  <head>
    <title><{title}></title>
    <link rel="STYLESHEET" type="text/css">
      <xsl:attribute name='href'><#= top #>css/default.css</xsl:attribute>
    </link>
  </head>

  <xsl:apply-templates select='body'/>
  </html>
</xsl:template>

<xsl:template match="body">
  <body bgcolor=white>
  <xsl:attribute name='background'><#= top #>images/background.gif</xsl:attribute>
  <!-- Column Formatting -->
  <table cellpadding=0 cellspacing=0 border=0 width="100%" summary="">
  <tr valign=top>

  <!-- Left Column: logo and navigation -->
  <td width=120>
  <img src="<#= top #>images/caucho.gif" width=120 height=40 alt=caucho>
    <xsl:attribute name='src'><#= top #>images/caucho.gif</xsl:attribute>
  </img>
  <br/>
 
  <# navigation.writeFamilyNavigation(out, out.page.request.requestURI); #>
  </td>
  
  <!-- Spacing Column -->
  <td width=30>
  <img alt="" width=30 height=1>
    <xsl:attribute name='src'><#= top #>images/caucho.gif</xsl:attribute>
  </img>
  </td>

  <!-- Center Column: title and content -->
  <td width='70%'>

  <!-- top title -->
  <table width="100%" cellspacing=0 cellpadding=0 border=0 summary="">
  <tr class=toptitle>
  <td>
  <xsl:attribute name='background'><#= top #>images/hbleed.gif</xsl:attribute>
      <font class=toptitle size="+3">&nbsp;<{../head/title}></font>
  </td></tr>
  <tr><td><br/>

  <xsl:apply-templates/>

  <!-- Footer -->
  </td></tr></table>

  <hr/>
  <# navigation.writePrevNext(out, out.page.request.requestURI); #>
  </td><td width="20%">&nbsp;</td></tr></table>
  </body>
</xsl:template>

</xsl:stylesheet>
