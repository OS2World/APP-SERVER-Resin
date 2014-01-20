/*
 * Copyright 1998-2000 Caucho Technology.  All rights reserved.
 */

package jsp;

import java.io.*;
import java.util.*;

import javax.servlet.http.*;
import javax.servlet.*;

/**
 * This final example shows the full-blown template architecture using
 * JSP as an output template.  This architecture is suitable when the
 * state processing and calculations become sufficiently complicated.
 *
 * For this example, the template mechanism is overkill because there
 * are only two states and almost no calculation.  That's deliberate so
 * the example code doesn't obscure the architecture, but it's not
 * intended to suggest that all JSP applications should use this architecture.
 *
 * In a real project, it may be most appropriate to start with the
 * simple guestbook.jsp architecture and progress to guestbook_bean.jsp
 * and GuestJsp as it becomes more clear how to factor the application.
 * That way, the project won't become overdesigned.  For the guestbook,
 * using GuestJsp is overkill.  The simple guestbook.jsp or
 * guestbook.xtp is simpler, more maintainable and a more appropriate
 * architecture.
 *
 * GuestJsp provides a simple guest book application using JSP as a
 * template to format results.  GuestXtp and GuestJsp are identical
 * except GuestJsp uses JSP for output templates and GuestXtp uses XTP
 * for output templates.
 *
 * <p>The URL looks something like http://localhost/servlet/GuestJsp.
 * 
 * <p>The guest book has three output templates: login.jsp, add.jsp, and
 * admin.jsp.  
 *
 * <p>login.jsp is used for users who haven't logged in yet.  It
 * displays the guest book comments and provides a form for users to log in.
 *
 * <p>add.jsp is used logged in users.  After displaying the guest book
 * comments, it provides a form to add or edit the user's comment.  Each
 * user has a single comment, so a duplicate comment will replace the
 * old one.
 *
 * <p>admin.jsp is used for the administrator (Voldemort, tmriddle).
 * The admin page lets the administrator delete comments from obnoxious
 * users.
 *
 * <p>Communication between the Servlet and the JSP pages is handled through
 * request attributes.
 * <ul>
 * <li>guest_book - the guest book ArrayList
 * <li>message - an error message if available
 * <li>Name - the user name if available (for form default)
 * <li>Comment - the user comment if available (for form default)
 * </ul>
 *
 * <p>The guest book is stored in the application with attribute "guest-book".
 * So both GuestJsp and GuestJsp share the same guest book.  Login information
 * is stored in the session object.
 */
public class GuestJsp extends HttpServlet {
  /**
   * Secret administrator name.
   */
  private String adminName;
  /**
   * Secret administrator password.
   */
  private String adminPassword;
  /**
   * "Secret" user password.
   */
  private String userPassword = "quidditch";

  /**
   * Initialize the servlet.
   *
   * <p>The admin name and password come from the configuration file.
   */
  public void init(ServletConfig conf)
    throws ServletException
  {
    super.init(conf);

    adminName = conf.getInitParameter("admin-name");
    if (adminName == null)
      adminName = "Voldemort";
    adminPassword = conf.getInitParameter("admin-password");
    if (adminPassword == null)
      adminPassword = "tmriddle";
  }

  /**
   * GETs are handled by <code>handleRequest</code>
   */
  public void doGet (
	HttpServletRequest	request,
	HttpServletResponse	response
    ) throws ServletException, IOException
  {
    handleRequest(request, response);
  }

  /**
   * POSTs are handled by <code>handleRequest</code>
   */
  public void doPost (
	HttpServletRequest	request,
	HttpServletResponse	response
    ) throws ServletException, IOException
  {
    handleRequest(request, response);
  }

  /**
   * Dispatch the request based on the form parameter 'action'.
   *
   * <p>The available actions are 'login', 'add', and 'delete'.
   */
  private void handleRequest(HttpServletRequest request,
                             HttpServletResponse response)
    throws ServletException, IOException
  {
    ServletContext app = getServletContext();
    GuestBook guestBook;

    // Get the guest book, creating a new one if necessary
    synchronized (app) {
      guestBook = (GuestBook) app.getAttribute("guest_book");
      if (guestBook == null) {
	guestBook  = new GuestBook();
	app.setAttribute("guest_book", guestBook);
      }
    }

    // name of the jsp template
    String template;

    String action = request.getParameter("action");
    HttpSession session = request.getSession(true);
    String name = (String) session.getValue("name");

    // Logged in users get to add a comment.
    // Unknown users need to login
    if (name == null)
      template = "/login.jsp";
    else
      template = "/add.jsp";

    String message = null;
    if (action == null) {
    }
    else if (action.equals("login")) {
      template = login(request, response);
    }
    else if (name == null) {
      message = "Please login";
    }
    // Only logged in users can use 'action=add'
    else if (action.equals("add") && session.getValue("name") != null) {
      String comment = (String) request.getParameter("Comment");

      message = addComment(guestBook, name, comment);
    }
    // Only the admin can use 'action=delete'
    else if (action.equals("delete") && session.getValue("admin") != null) {
      template = "/admin.jsp";

      message = deleteComment(guestBook, request.getParameter("Name"));
    }

    // Error messages are passed to the template with the message attribute.
    if (message != null)
      request.setAttribute("message", message);

    // Fill in the "Name" attribute so the template can use it as a default
    // form value.
    name = (String) session.getValue("name");
    if (name == null)
      name = "";
    request.setAttribute("Name", name);

    // Fill in the "Comment" attribute so the template can use it as a default
    // form value.
    String comment = null;
    if (name != null) {
      Guest guest = guestBook.getGuest(name);
      if (guest != null)
	comment = guest.getComment();
    }
    if (comment == null)
      comment = "";
    request.setAttribute("Comment", comment);
     
    // Dispatch to the template.  Note the template is relative to
    // the application root, but the application can be deeper in the
    // URL tree.  The example Resin server has APP_DIR = /examples/template
    RequestDispatcher disp = app.getRequestDispatcher(template);

    // The guest book needs synchronization because multiple servlets can
    // be running in parallel.  To make the templates easier, synchronize
    // it here so they don't need to worry about it.
    synchronized (guestBook) {
      disp.forward(request, response);
    }
  }

  /**
   * Attempt to log the user in.  Successful login is stored in the session.
   * Unsuccessful login generates a message.
   *
   * @return the template to format the results.
   */
  private String login(HttpServletRequest request,
                       HttpServletResponse response)
  {
    HttpSession session = request.getSession(true);
    String name = request.getParameter("Name");
    String password = request.getParameter("Password");

    if (name == null || name.length() == 0) {
      request.setAttribute("message", "Bad Name");
      return "/login.jsp";
    }
    else if (name.equals(adminName) &&
	     (password != null && password.equals(adminPassword))) {
      session.putValue("name", name);
      session.putValue("admin", "admin");
      return "/admin.jsp";
    }
    else if (password == null || ! password.equals(userPassword)) {
      request.setAttribute("message", "Bad Password");
      return "/login.jsp";
    }
    else {
      session.putValue("name", name);
      return "/add.jsp";
    }
  }

  /**
   * Attempts to add a comment to the guest book.
   *
   * @param name user name for the comment
   * @param comment user's comment
   *
   * @return An error message or null if no error.
   */
  private String addComment(GuestBook guestBook, String name, String comment)
  {
    if (name == null || name.length() == 0) {
      return "Bad Login";
    }
    if (comment == null || comment.length() == 0) {
      return "Missing comment";
    }

    guestBook.addGuest(name, comment);

    return null;
  }
  
  /**
   * Deletes the guest book entry for a given user.  The caller is
   * responsible for checking that the administrator has logged in.
   *
   * @return error message or null if no error.
   */
  private String deleteComment(GuestBook guestBook, String name)
  {
    if (name == null || name.length() == 0)
      return "Missing user";

    if (guestBook.removeGuest(name) == null)
      return "Bad User: '" + name + "'";

    else
      return null;
  }
}
