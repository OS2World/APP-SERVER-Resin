<!-- The counter brings in the header -->
<xtp:directive.page language='javascript'/>

<xsl:import href='copy.xsl'/>
<xsl:import href='header.xsl'/>

<!-- 
   - This counter executes when the JTP is processed by
   - the XSL stylesheet.
  -->

<xsl:template match='counter'>
  <jsp:directive.page language='javascript'/>
  <jsp:expression>application.attribute.counter++</jsp:expression>
</xsl:template>
