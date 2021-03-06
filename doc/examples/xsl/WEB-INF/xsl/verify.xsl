<!-- JS control -->

<xsl:stylesheet>
<xsl:output disable-output-escaping=true/>
<#@ page language='javascript' #>
<#@ cache #>

<xsl:template name=good-element>
&lt;<{name(.)}> <xsl:apply-templates select='@*'/>&gt;
<xsl:apply-templates/>
&lt;/<{name(.)}>&gt;
</xsl:template>

<xsl:template name=good-attribute>
<xsl:text> </xsl:text><{name(.)}>="<{.}>"<xsl:text/>
</xsl:template>

* <<
<font color=red>&lt;<{name(.)}></font
><xsl:apply-templates select='@*'/><font color=red>&gt;</font>
<xsl:apply-templates/>
<font color=red>&lt;/<{name(.)}>&gt;</font>
>>

@* <<
<font color=red><xsl:text> </xsl:text><{name(.)}>="<{.}>"</font>
>>

text() <#
var data = node.nodeValue;
data = data.replace(/\n/g, '<br>');
data = data.replace(/ /g, '&nbsp;');
out.write(data);
#>

/html/body/top <<
<xsl:call-template name='good-element'/>
>>

/html/body/top/chapter <<
<xsl:call-template name='good-element'/>
>>

/html/body/top/chapter/@title <<
<xsl:call-template name='good-attribute'/>
>>

chapter/verse <<
<xsl:call-template name='good-element'/>
>>

chapter/verse/@title <<
<xsl:call-template name='good-attribute'/>
>>

/ <<
<body bgcolor=white>
<xsl:apply-templates select='/html/body/node()'/>
</body>
>>

</xsl:stylesheet>
