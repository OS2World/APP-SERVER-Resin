<!--
   - form.xsl uses XTP to provide sophisticated form behaviour with simple 
   - XTP source.  It produces JSP with language=javascript.  Using Java
   - is simple as well.  All stylesheets must produce the same JSP language.
   -
   - Although this example uses the 'xslt-lite' syntax, the same functionality
   - is available using strict XSL.  For those more familiar with strict XSL,
   - note that unknown elements, e.g. <input>, are treated as text, not
   - as a created element.
  -->

<xsl:stylesheet>
<xsl:output disable-output-escaping indent='false'/>

@ct:form-request-value
<<
<xsl:attribute name='value'><%=
  request.getAttribute("<{.}>") == null ? "" : request.getAttribute("<{.}>")
%></xsl:attribute>
>>

form[@action] <<
<form action='<%= response.encodeURL("{@action}") %>'>
  <xsl:apply-templates select="node()|@*[not(name(.)='action')]"/>
</form>
>>

ct:hidden <<
<input type='hidden'>
  <xsl:apply-templates select='@*'/>
</input>
>>

ct:password <<
<input type='password'>
  <xsl:apply-templates select='@*'/>
</input>
>>

ct:submit <<
<input type='submit'>
  <xsl:apply-templates select='@*'/>
</input>
>>

ct:label <<
<xsl:apply-templates/>
>>

ct:label[@value] <<
<{@value}>
>>

ct:label[@ct:form-request-value] <<
<%=
  request.getAttribute("<{@ct:form-request-value}>") == null ? "" :
    request.getAttribute("<{@ct:form-request-value}>")
%>
>>

ct:form-table[@action] <<
<form action='<%= response.encodeURL("{@action}") %>'>
  <xsl:apply-templates select="@*[not(name(.)='action')]"/>

  <xsl:apply-templates select='ct:hidden[@name]'/>

  <table>
  <xsl:for-each select='*[name(.)!="ct:hidden"]'>
    <xsl:if test='@name'>
        <tr><td><{@name}></td>
            <td><xsl:apply-templates select='.'/></td></tr>
    </xsl:if>

    <xsl:if test='not(@name)'>
        <tr><td colspan=2><xsl:apply-templates select='.'/></td></tr>
    </xsl:if>
  </xsl:for-each>
  </table>  
</form>
>>

</xsl:stylesheet>
