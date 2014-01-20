<!-- The default XSL file just brings in the header -->
<#@ page language='javascript' #>
<xsl:output method='html' disable-output-encoding='true'/>
<xsl:import href='copy.xsl'/>
<xsl:import href='header.xsl'/>
<xsl:import href='content.xsl'/>
