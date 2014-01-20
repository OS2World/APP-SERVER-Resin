/*
 * Copyright 1998-1999 Caucho Technology.  All rights reserved.
 */

package jsp;

import java.io.*;
import java.util.*;

import javax.servlet.http.*;
import javax.servlet.*;

/**
 * The inner class needs to be public so the jsp can get it.
 */
public class Guest {
  private String name;
  private String comment;

  Guest(String name, String comment)
  {
    this.name = name;
    this.comment = comment;
  }

  public String getName()
  {
    return name;
  }

  public String getComment()
  {
    return comment;
  }

  public void setComment(String comment)
  {
    this.comment = comment;
  }
}
