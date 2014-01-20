<!--
   - Formats the top level of the template.  header.xsl assumes the XTP
   - has been read in as HTML.  So it can assume the existence of the
   - html and body tags.
  -->

<xsl:stylesheet>
<xsl:output disable-output-escaping=true/>

<!--
   - Format the top-level page.
  -->

/html <<
<html>
<head>
  <title><{head/title}></title>
</head>

<body bgcolor=white background="/images/background.gif">
  <!-- Column Formatting -->
  <table cellpadding=0 cellspacing=0 border=0 width="100%" summary="">
  <tr valign=top>

  <!-- Left Column: logo and navigation -->
  <td width=120>
  <img src="/images/caucho.gif" width=120 height=40 alt=caucho/><br/>
  </td>

  <!-- Spacing Column -->
  <td width=30>
  <img src="/images/pixel.gif" alt="" width=30 height=1/>
  </td>

  <!-- Center Column: title and content -->
  <td width='70%'>

  <!-- top title -->
  <table width="100%" cellspacing=0 cellpadding=0 border=0 summary="">
  <tr class=toptitle><td background="/images/hbleed.gif">
      <font class=toptitle size="+3">&nbsp;<{head/title}></font>
  </td></tr><tr><td><br/>

  <!-- the actual page contents -->
  <xsl:apply-templates select='body/node()'/>

  </td></tr></table>

  </td><td width="20%">&nbsp;</td></tr></table>
</body>
</html>
>>

</xsl:stylesheet>
