ct:section[title]
<<
<p/>
<table width='100%' class='section' cellspacing=0 cellpadding=3 border=0>
<tr><td>
  <font size='+1'><xsl:apply-templates select='title/*'/></font>
</td></tr>
</table>
<xsl:apply-templates select='*[name(.)!="title"]'/>
>>

ct:section[@title]
<<
<p/>
<table width='100%' class='section' cellspacing=0 cellpadding=3 border=0>
<tr><td>
  <font size='+1'><{@title}></font>
</td></tr>
<tr><td>
<xsl:apply-templates select='*'/>
</td></tr>
</table>
>>
