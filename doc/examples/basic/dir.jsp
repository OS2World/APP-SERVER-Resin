<%@ page language=javascript %><html>
<!-- dir.jsp
  -- Creates a page recursively listing all subdirectories and files.
  -->
<head><title>Directory Listing</title></head>
<body bgcolor=#ffffff>

<h1>Directory Listing</h1>

<%
//
// Resin file I/O roughly follows java.io with the exceptions:
//   1) All filenames are urls using '/' as the filename separator.
//   2) The myriad stream classes, readers, and writers are coalesced 
//      into a single stream object.
//
// This function recursively lists the directory contents.
// 
// Example:
//
// top
// <ul>
// <li>sub1
// <li>sub2
// <ul>
// <li>subsub
// </ul>
// </ul>
//
function readdir(path)
{
  out.writeln(path.path.split('/').reverse()[0]);
  if (path.isDirectory()) {
    out.writeln("<ul>");
    for (file in path.list()) {
      out.write("<li>");
      readdir(path(file));
    }
    out.writeln("</ul>");
  }
}

readdir(File("data"));

%>

</body>
</html>
