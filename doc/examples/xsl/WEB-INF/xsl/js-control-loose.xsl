<!-- JS control -->

<#@ page language='javascript' #>
<xsl:output disable-output-escaping/>

<xsl:import href='copy.xsl'/>
<xsl:import href='header.xsl'/>

/ <<
<%@ page language=javascript %>
<xsl:apply-imports/>
>>

ct:get << 
<%= <{@expr}> %>
>>

ct:if <<
<% if (<{@expr}>) { %>
  <xsl:apply-templates/>
<% } %>
>>

ct:iter <<
<% for (var i in <{@expr}>) { %>
  <xsl:apply-templates/>
<% } %>
>>

ct:iter[@index] <<
<% for (var <{@index}> in <{@expr}>) {
  <xsl:apply-templates/>
<% } %>
>>

<#!
function dist(x1, y1, x2, y2)
{
  return Math.sqrt((x1 - x2) * (x1 - x2) +
                   (y1 - y2) * (y1 - y2));
}
#>

ct:dist <<
(<{@x1}>,<{@y1}>) to (<{@x2}>,<{@y2}>) = <#=
dist(node.attribute.x1, node.attribute.y1,
     node.attribute.x2, node.attribute.y2)
#>
>>

</xsl:stylesheet>