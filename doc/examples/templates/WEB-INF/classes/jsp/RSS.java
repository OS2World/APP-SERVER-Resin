package jsp;

import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

import com.caucho.vfs.*;

/**
 * The RSS servlet grabs data from a given URL and forwards it to a
 * formatting XSL.
 *
 * <p>It only checks the server every hour, otherwise caching the result.
 * Note, this assumes that you've enabled &lt;cache> in resin.conf
 *
 * <p>RSS may only be called internally.  This restriction is necessary
 * to prevent outsiders from looking at our filesystem or internal
 * file servers.
 */
public class RSS extends HttpServlet {
  /**
   * The modified time is at the top of the hour.
   */
  public long getLastModified(HttpServletRequest req)
  {
    long time = System.currentTimeMillis();
    
    return time - time % (3600 * 1000);
  }

  public void doGet(HttpServletRequest req, HttpServletResponse res)
    throws IOException, ServletException
  {
    String url = req.getParameter("url");
    
    // Only allow internal requests
    String included;
    included = (String) req.getAttribute("javax.servlet.include.request_uri");
    
    if (url == null || included == null) {
      res.sendError(res.SC_FORBIDDEN);
      return;
    }

    // Use the standard Xsl filter
    res.setContentType("x-application/xsl");
    // And use the rss stylesheet to format it
    req.setAttribute("caucho.xsl.stylesheet", "rss.xsl");
    
    // Copy the stream to the output.  Because we're using Resin's VFS,
    // we can handle any known file scheme, including file: and http:.
    ReadStream is = Pwd.lookupNative(url).openRead();
    try {
      is.writeToStream(res.getOutputStream());
    } finally {
      is.close();
    }
  }
}
