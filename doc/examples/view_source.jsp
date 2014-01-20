<%@ page import='com.caucho.vfs.*' %><% 
/*
 * view_source.jsp just writes the verbatim source to the browser.
 */
// Chroot to the current directory so no one can use this as a p
// security hold
Path chroot = Pwd.lookupNative(request.getRealPath(".")).createRoot();
ReadStream is = chroot.lookupNative(request.getQueryString()).openRead();

int ch;
out.println("<body bgcolor=white>");
out.println("<pre>");
while ((ch = is.read()) >= 0) {
  if (ch == '<')
    out.print("&lt;");
  else
    out.print((char) ch);
}
is.close();
out.println("</pre>");
out.println("</body>");

%>
