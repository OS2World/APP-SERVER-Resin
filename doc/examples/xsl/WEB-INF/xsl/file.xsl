<xsl:stylesheet>

<xtp:directive.page language='javascript'/>
<xsl:output disable-output-escaping='yes'/>

<!-- The file brings in the header -->
<xsl:import href='copy.xsl'/>
<xsl:import href='header.xsl'/>

<!--
   - <box color=[color]> [contents] </box>
   -
   - Define a box for centering and highlighting.
   - The optional attribute 'color' selects the background.
  -->

<xsl:template match='box'>
  <center><table bgcolor='{if(@color,@color,"cyan")}' cellpadding=10>
  <tr><td>

    <xsl:apply-templates/>

  </td></tr>
  </table></center>
</xsl:template>

<!--
   - <fortune/>
   -
   - Returns a random fortune.
  -->

<xsl:template match='fortune'>
<%@ page language='javascript' %>
<% getFortune(out) %>
<%!
/*
 * Returns a random 'fortune' file from a directory.
 *
 *
 * Resin file I/O roughly follows java.io with the major exceptions:
 *   1) All filenames are urls using '/' as the filename separator.
 *   2) The myriad stream classes, readers, and writers are coalesced 
 *      into a single stream object.
 */

function getFortune(out)
{
  // 'fortune' represents the subdirectory.  The current directory
  // is the directory of this file.

  var fortune = File('data/fortune');
  var list = fortune.list();

  // Selects a random file in the directory.
  // Subdirectories and emacs backup files are ignored.
  do {
    // random number [0, list.length) converted to an integer.
    var n = Math.floor(Math.random() * list.length);

    var filename = list[n];
  } while (! fortune(filename).isFile() || filename.match(/~$/));
  
  // Open a read stream to the file and copy it to the output.

  var file = fortune(filename).openRead();

  // Import the file
  while ((line = file.readln()) !== null) {
    out.writeln(line);
  }
  file.close();
}
%>
</xsl:template>
</xsl:stylesheet>
