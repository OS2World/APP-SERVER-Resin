<%@ page language=javascript %><%
/* file.jsp
 * Returns a random 'fortune' file from a directory.
 *
 *
 * Resin file I/O roughly follows java.io with the major exceptions:
 *   1) All filenames are urls using '/' as the filename separator.
 *   2) The myriad stream classes, readers, and writers are coalesced 
 *      into a single stream object.
 */

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

%>
<html><title>Confucian Analects</title> 
<body bgcolor=white>
<center><table bgcolor=cyan cellpadding=10>
<tr><td>

<%
// Import the file

while ((line = file.readln()) !== null) {
  out.writeln(line);
}
file.close();

%>

</table></center>
</body></html>

