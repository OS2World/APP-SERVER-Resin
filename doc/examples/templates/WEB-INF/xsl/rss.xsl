<!--
   - Formatting for the various syndication formats.
   -
   - Netscape RSS:
   - <rss>
   -   <channel>
   -     <title>...</title>
   -     <link>...</link>
   -     <item>
   -       <title>...</title>
   -       <link>...</link>
   -       <description>...</description>
   -     </item>
   -     ...
   -   </channel>
   -   ...
   - </rss>
  -->

rdf:RDF
<<
<xsl:apply-templates select='channel'/>
>>

channel
<<
<table border=3 width="100%">
<tr><td>
<table width="100%" border=0 cellpadding=0 cellspacing=0 bgcolor="#ccddff">
<tr><td><{title}></td></tr>
</table>

<xsl:apply-templates select='item|../item'/>

</td></tr>
</table>
>>

channel[link]
<<
<table border=3 width="100%" cellpadding=0 cellspacing=0>
<tr><td>
<table width="100%" border=0 cellpadding=0 cellspacing=0 bgcolor="#ccddff">
<tr><td><a href="{link}"><{title}></a></td></tr>
</table>

<ul>
<xsl:apply-templates select='item|../item'/>
</ul>

</td></tr>
</table>
>>

item
<<
<li/><{title}>: <{description}>
>>

item[link]
<<
<li/><a href="{link}"><{title}></a>: <{description}>
>>
