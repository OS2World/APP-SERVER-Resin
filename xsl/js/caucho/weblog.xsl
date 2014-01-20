<xsl:stylesheet parsed-content=false>

<xsl:template name='ct:weblog-edit'>
<% if (request.attribute.editor) { %>
<hr>
<form method='POST' action='<%= request.requestURI %>'>
<table>
<!--
<tr><td>Title</td>
    <td><input name=title size=40 value='<%= request.attribute.title %>'></td>
<tr><td>Link</td>
    <td><input name=link size=40  value='<%= request.attribute.link %>'></td>
-->
<tr><!-- <td>Description</td> --><td><textarea name=description cols=60 rows=10><%=
  request.attribute.description
%></textarea></td>
</table>
<input type=hidden name='edit' value='<%= request.attribute.edit %>'>
<input type=submit value='Add Entry'>
</form>
<% } %>
</xsl:template>

ct:weblog
<<
<%
  var _ct_weblog_list = application.attribute.weblog;
  if (_ct_weblog_list) {
    var _ct_weblog_count = 0;
    for (var _ct_weblog in _ct_weblog_list) {
      if (_ct_weblog_count++ > 5)
        break;
%>
<xsl:apply-templates/>
<%
    }
  }
%>
>>

ct:weblog//ct:date
<<
<%= new Date(_ct_weblog.time) %>
>>

ct:weblog//ct:date[@format]
<<
<%= new Date(_ct_weblog.time).format("<{@format}>") %>
>>

ct:webitem
<<
<%
  var _ct_webitem_count = 0;
  for (var _ct_weblog_item in _ct_weblog) {
%>
<xsl:apply-templates/>
<%
    if (request.attribute.editor && _ct_weblog_count == 1)
      out.write("<\a href='", request.attribute.uri,
                "?edit=", _ct_webitem_count++, "'>edit</a>");
  }
%>
>>

ct:webitem//ct:title
<<
<%
/*
  var _ct_weblog_title = _ct_weblog_item.title;
  if (! _ct_weblog_title)
    _ct_weblog_title = _ct_weblog_item.link;

  if (_ct_weblog_item.link)
    out.write("<\a href='", _ct_weblog_item.link, "'>",
              _ct_weblog_title, "</a>");
  else if (_ct_weblog_title)
    out.write(_ct_weblog_title);
*/
%>
>>

ct:webitem//ct:description
<<
<%
  if (_ct_weblog_item.description)
    out.write(_ct_weblog_item.description);
%>
>>

ct:webitem//ct:date
<<
<%= new Date(_ct_weblog_item.time) %>
>>

ct:webitem//ct:date[@format]
<<
<%= new Date(_ct_weblog.time).format("<{@format}>") %>
>>

</xsl:stylesheet>
