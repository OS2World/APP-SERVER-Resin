<!--
   - defaultcopy.xsl just has a simple rule to copy the input to the output.
   - This way, unknown tags in the XTP file are copied.  So an XTP writer
   - can safely use HTML tags
   -
   - defaultcopy.xsl should be the first imported stylesheet so it will
   - have the lowest importance.
  -->

*|@*
<<
<xsl:copy>
  <xsl:apply-templates select='node()|@*'/>
</xsl:copy>
>>
