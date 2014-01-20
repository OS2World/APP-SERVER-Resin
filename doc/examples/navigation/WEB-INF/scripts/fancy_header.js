/**
 * This script provides a somewhat fancier basic site template.
 *  It provides family navigation on the left and adds some gifs.
 *
 * Usage:
 * <%
 *   import fancy_header;
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

out.writeln(@<<END);
<center>
<table width="90%" class=toc border=3>
<tr><td>
END

  navigation.writeContents(out, uri);

out.writeln(@<<END);
</table>
</center>
END
}

function writeHeader(out, application, title, uri) 
{
  var navigation = Navigation.getNavigation(application);

  var top = navigation.toc.documentElement.attribute.top;

  top = navigation.toc.documentElement.attribute.top;

  out.writeln(@<<END);
<html>
<head>
<title>$(title)</title>
<link rel="STYLESHEET" href="$(top)css/default.css" type="text/css">
</head>
<body bgcolor=white background="$(top)images/background.gif">

<!-- Column Formatting -->
<table cellpadding=0 cellspacing=0 border=0 width="100%" summary="">
<tr valign=top>

<!-- Left Column: logo and navigation -->
<td width=120>
<img src="$(top)images/caucho.gif" width=120 height=40 alt=caucho><br>

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

<!-- top title -->
<table width="100%" cellspacing=0 cellpadding=0 border=0 summary="">
<tr class=toptitle>
<td background="$(top)images/hbleed.gif"><font class=toptitle size="+3">&nbsp;
$(title)</font></td></tr>
<tr><td><br>
END
}

function writeFooter(out, application, uri, style)
{
  out.writeln(@<<END);
<!-- Footer -->
</td></tr></table>
END

  if (style == "prevnext") {
    out.writeln("<hr>");
    var navigation = Navigation.getNavigation(application);

    navigation.writePrevNext(out, uri);
  }
  out.writeln(@<<END);

</td><td width="20%">&nbsp;</td></tr></table>
</body>
</html>
END
}
