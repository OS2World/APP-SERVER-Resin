<!--
   - Some general useful formatting patterns.
   -
   - <ct:columns>
   -   <ct:column>...</ct:column>
   -   ...
   -   <ct:column>...</ct:column>
   - </ct:columns>
  -->

ct:columns
<<
<table width="100%" cellspacing=0 cellpadding=0>
<tr>
<xsl:apply-templates select='ct:column'/>
</tr>
</table>
>>

ct:column
<<
<td valign="top"><xsl:apply-templates/></td>
>>

ct:column[@width]
<<
<td valign="top" width="{@width}"><xsl:apply-templates/></td>
>>

<!--
   - Include a syndicated page
  -->
ct:use-rss
<<
<jsp:include page="/RSS?url={.}"/>
>>
