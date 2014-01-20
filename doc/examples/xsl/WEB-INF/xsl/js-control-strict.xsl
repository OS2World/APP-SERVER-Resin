<!-- JS control -->

<xsl:stylesheet>
<xsl:output method='html' disable-output-escaping=true/>

<xtp:directive.page language='javascript'/>
<xtp:directive.cache/>

<xsl:import href='copy.xsl'/>
<xsl:import href='header.xsl'/>

<xsl:template match='/'>
  <jsp:directive.page language='javascript'/>
  <xsl:apply-imports/>
</xsl:template>

<xsl:template match='ct:get'>
  <jsp:expression><xsl:value-of select='@expr'/></jsp:expression>
</xsl:template>

<xsl:template match='ct:if'>
  <jsp:scriptlet>if (<xsl:value-of select='@expr'/>) {</jsp:scriptlet>
    <xsl:apply-templates/>
  <jsp:scriptlet>}</jsp:scriptlet>
</xsl:template>

<xsl:template match='ct:iter'>
  <jsp:scriptlet>
    for (var i in <xsl:value-of select='@expr'/>) {
  </jsp:scriptlet>
    <xsl:apply-templates/>
  <jsp:scriptlet>}</jsp:scriptlet>
</xsl:template>

<xsl:template match='ct:iter[@index]'>
  <jsp:scriptlet>
    for (var <xsl:value-of select='@index'/> in 
         <xsl:value-of select='@expr'/>) {
  </jsp:scriptlet>
    <xsl:apply-templates/>
  <jsp:scriptlet>}</jsp:scriptlet>
</xsl:template>

<xtp:declaration>
function dist(x1, y1, x2, y2)
{
  return Math.sqrt((x1 - x2) * (x1 - x2) +
                   (y1 - y2) * (y1 - y2));
}
</xtp:declaration>

<xsl:template match='ct:dist'>
(<xsl:value-of select='@x1'/>,<xsl:value-of select='@y1'/>
 <xsl:text>) to (</xsl:text>
 <xsl:value-of select='@x2'/>,<xsl:value-of select='@y2'/>
 <xsl:text>) = </xsl:text>
<xtp:expression>
dist(node.attribute.x1, node.attribute.y1,
     node.attribute.x2, node.attribute.y2)
</xtp:expression>
</xsl:template>

</xsl:stylesheet>