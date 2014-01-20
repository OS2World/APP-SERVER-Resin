/**
 * This script is a basic site template.  It provides family navigation on
 * the left and not much else.
 *
 * Usage:
 * <%
 *   import header;
 *   writeHeader(out, application, "Title", request.requestURI);
 * %>
 *
 * Text of the page goes here.
 *
 * <%
 *   writeFooter(out)
 * %>
 */

import Navigation;

function writeSitemap(out, application, uri)
{
  var navigation = Navigation.getNavigation(application);

  navigation.writeContents(out, uri);
}

function writeHeader(out, application, title, uri) 
{
  var navigation = Navigation.getNavigation(application);

  var top = navigation.toc.documentElement.attribute.top;

  out.writeln(@<<END);
<html>
<head>
<title>$(title)</title>
</head>
<body bgcolor=white>

<!-- Column Formatting -->
<table cellpadding=0 cellspacing=0 border=0 width="100%" summary="">
<tr valign=top>

<!-- Left Column: Navigation -->
<td width=120>

END

  navigation.writeFamilyNavigation(out, uri);

  out.writeln(@<<END);
</td>

<!-- Spacing Column -->
<td width=30>
<img src="$(top)images/pixel.gif" alt="" width=30 height=1>
</td>

<!-- Center Column: title and content -->
<td width='70%'>

END
}

function writeFooter(out)
{
  out.writeln(@<<END);
<!-- Footer -->
</td><td width="20%">&nbsp;</td></tr></table>
</body>
</html>
END
}
