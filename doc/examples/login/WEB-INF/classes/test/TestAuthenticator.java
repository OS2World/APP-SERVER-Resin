/*
 * Copyright (c) 1998-1999 Caucho Technology -- all rights reserved
 *
 * Caucho Technology permits redistribution, modification and use
 * of this file in source and binary form ("the Software") under the
 * Caucho Public License ("the License").  In particular, the following
 * conditions must be met:
 *
 * 1. Each copy or derived work of the Software must preserve the copyright
 *    notice and this notice unmodified.
 *
 * 2. Redistributions of the Software in source or binary form must include 
 *    an unmodified copy of the License, normally in a plain ASCII text
 *
 * 3. The names "Resin" or "Caucho" are trademarks of Caucho Technology and
 *    may not be used to endorse products derived from this software.
 *    "Resin" or "Caucho" may not appear in the names of products derived
 *    from this software.
 *
 * 4. Caucho Technology requests that attribution be given to Resin
 *    in any manner possible.  We suggest using the "Resin Powered"
 *    button or creating a "powered by Resin(tm)" link to
 *    http://www.caucho.com for each page served by Resin.
 *
 * This Software is provided "AS IS," without a warranty of any kind. 
 * ALL EXPRESS OR IMPLIED REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.

 * CAUCHO TECHNOLOGY AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES
 * SUFFERED BY LICENSEE OR ANY THIRD PARTY AS A RESULT OF USING OR
 * DISTRIBUTING SOFTWARE. IN NO EVENT WILL CAUCHO OR ITS LICENSORS BE LIABLE
 * FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL,
 * CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND
 * REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF OR
 * INABILITY TO USE SOFTWARE, EVEN IF HE HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGES.      
 *
 * @author Scott Ferguson
 *
 * $Id: TestAuthenticator.java,v 1.3 2000/04/25 19:33:25 ferg Exp $
 */

package test;

import java.util.*;
import java.security.*;

import com.caucho.util.*;
import com.caucho.server.http.*;

/**
 * Implementation of a custom authenticator class.
 */
public class TestAuthenticator extends AbstractAuthenticator {
  /**
   * init is called when the application starts.
   */
  public void init(CauchoApplication app, RegistryNode params)
  {
  }

  /**
   * For more sophisticated sites, this would allow cookie-based
   * authentication so the user doesn't always have to login.
   */
  public Principal authenticateCookie(String cookieValue)
  {
    return null;
  }

  /**
   * Authenticate for the user and password.
   */
  public Principal authenticate(String user, String password)
  {
    if (password != null && password.equals("quidditch"))
      return new BasicPrincipal(user);
    else
      return null;
  }

  /**
   * If we were storing cookies, this would store the <cookie, user> pair
   * in some sort of persistent storage.
   */
  public boolean updateCookie(Principal user, String cookieValue)
  {
    return true;
  }

  /**
   * True if the user is in the role.  We're just hard-coding the
   * "user" role.  Logged in users are in the "user" role.
   */
  public boolean isUserInRole(Principal user, String role)
  {
    return user != null && "user".equals(role);

  }
}
