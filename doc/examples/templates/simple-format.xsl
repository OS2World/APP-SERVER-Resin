<!--
   - simple-format.xsl is a basic stylesheet.
   -
   - 1) Most tags are just copied from the input to the output.
   - 2) The basic page layout is automatically generated.
   - 3) response.encodeURL() calls are automatically inserted.
   -
   - The stylesheet is written in 'XSLT-lite', but can be trivially
   - converted to strict XSLT.
  -->

<!--
   - disable-output-escaping makes sure that '<' is not printed as '&lt;'
  -->
<xsl:output disable-output-escaping='true' indent='no'/>

<!--
   - by default, copy the input to the output
  -->
*|@*
<<
<xsl:copy>
  <xsl:apply-templates select='node()|@*'/>
</xsl:copy>
>>

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

<!--
   - Automatically encode URLs for forms and anchors
  -->
form[@action] <<
<form action='<%= response.encodeURL("{@action}") %>'>
  <xsl:apply-templates select="node()|@*[not(name(.)='action')]"/>
</form>
>>

a[@href] <<
<a href='<%= response.encodeURL("{@href}") %>'>
  <xsl:apply-templates select="node()|@*[not(name(.)='href')]"/>
</a>
>>



