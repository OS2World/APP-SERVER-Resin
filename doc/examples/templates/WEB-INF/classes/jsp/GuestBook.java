/*
 * Copyright 1998-2000 Caucho Technology.  All rights reserved.
 */

package jsp;

import java.io.*;
import java.util.*;

import javax.servlet.http.*;
import javax.servlet.*;

/**
 * The inner class needs to be public so the jsp can get it.
 */
public class GuestBook {
  private HashMap guests;

  public GuestBook()
  {
    guests = new HashMap();
  }

  public synchronized void addGuest(String name, String comment)
  {
    guests.put(name, new Guest(name, comment));
  }

  public synchronized Guest getGuest(String name)
  {
    return (Guest) guests.get(name);
  }

  public String getComment(String name)
  {
    Guest guest = getGuest(name);

    if (guest == null)
      return null;
    else
      return guest.getComment();
  }

  public Iterator iterator()
  {
    return guests.values().iterator();
  }

  public synchronized Guest removeGuest(String name)
  {
    return (Guest) guests.remove(name);
  }
}
