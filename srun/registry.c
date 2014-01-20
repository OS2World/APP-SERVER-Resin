/*
 * Copyright (c) 1999-2000 Caucho Technology.  All rights reserved.
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

 * Caucho Technology AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES
 * SUFFERED BY LICENSEE OR ANY THIRD PARTY AS A RESULT OF USING OR
 * DISTRIBUTING SOFTWARE. IN NO EVENT WILL Caucho OR ITS LICENSORS BE LIABLE
 * FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL,
 * CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND
 * REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF OR
 * INABILITY TO USE SOFTWARE, EVEN IF HE HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGES.      
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "cse.h"

/*
 * stream_t is a convenience class containing the parse state.
 */
typedef struct rstream_t {
  config_t *config;  /* configuration we're reading into */
  FILE *file;        /* input stream to read */
  char *path;        /* file name  */
  int line;          /* current line */

  char *buf;         /* expandable buffer for reading tokens */
  int length;        /* current length of the token in the buffer */
  int capacity;      /* total buffer capacity */
} rstream_t;

/*
 * Read a byte from the input stream, counting lines.
 */
static int
buf_read(rstream_t *is)
{
  int ch = fgetc(is->file);
  if (ch == '\n')
    is->line++;

  return ch;
}

/*
 * append a character to the token, expanding if necessary
 */
static void
buf_append(rstream_t *is, int ch)
{
  if (is->length + 2 >= is->capacity) {
    char *old = is->buf;

    is->capacity *= 2;
    
    is->buf = cse_alloc(is->config, 2 * is->capacity);
    cse_free(is->config, old);
  }

  is->buf[is->length++] = ch;
}

/*
 * Add a child to the current registry node.
 */
static registry_t *
registry_add(rstream_t *is, registry_t *node, char *key)
{
  registry_t *child = (registry_t *) cse_alloc(is->config, sizeof(registry_t));
  memset(child, 0, sizeof(registry_t));

  child->parent = node;
  child->key = key;

  child->prev = node->last;
  
  if (node->last) {
    node->last->next = child;
    node->last = child;
  } else {
    node->first = child;
    node->last = child;
  }

  return child;
}

/*
 * Parse an attribute.  
 *
 * The 'id' attribute is equivalent to a value of the parent node.
 *
 * entity references are not handled, i.e. &apos;, &quot; and &amp;.
 */
static int 
parse_attribute(rstream_t *is, registry_t *node, int ch)
{
  registry_t *attr;

  for (; isspace(ch); ch = buf_read(is)) {
  }

  is->length = 0;
  for (;
       ch >= 0 && ! isspace(ch) && ch != '=' && ch != '/' && ch != '>';
       ch = buf_read(is)) {
    buf_append(is, ch);
  }

  if (is->length == 0) {
    ungetc(ch, is->file);
    return 0;
  }

  is->buf[is->length] = 0;
  if (! strcmp(is->buf, "id"))
    attr = node;
  else
    attr = registry_add(is, node, cse_strdup(is->config, is->buf));
  
  for (; isspace(ch); ch = buf_read(is)) {
  }

  if (ch != '=') {
    ungetc(ch, is->file);
    return 0;
  }

  for (ch = buf_read(is); isspace(ch); ch = buf_read(is)) {
  }

  if (ch < 0 || ch == '/' || ch == '>') {
    cse_error(is->config, "%s:%d: expected attribute at '%c'\n",
	      is->path, is->line, ch);
    return -1;
  }

  if (ch == '\'' || ch == '\"') {
    int end = ch;

    is->length = 0;
    for (ch = buf_read(is); ch >= 0 && ch != end; ch = buf_read(is))
      buf_append(is, ch);

    if (ch != end) {
      cse_error(is->config, "%s:%d: expected '%c' at '%c'\n",
		is->path, is->line, end, ch);
      return -1;
    }

    is->buf[is->length] = 0;
    attr->value = cse_strdup(is->config, is->buf);

    return ' ';
  }
  else {
    is->length = 0;
    for (;
	 ch >= 0 && ! isspace(ch) && ch != '/' && ch != '>';
	 ch = buf_read(is)) {
      buf_append(is, ch);
    }

    is->buf[is->length] = 0;
    attr->value = cse_strdup(is->config, is->buf);

    return ch;
  }
}

/*
 * parses an open tag, including the attributes.
 */
static int
parse_tag(rstream_t *is, registry_t **p_node, int ch)
{
  registry_t *node = *p_node;
  registry_t *child = 0;
  is->length = 0;

  for (;
       ch >= 0 && ! isspace(ch) && ch != '/' && ch != '>';
       ch = buf_read(is)) {
    buf_append(is, ch);
  }

  if (is->length == 0) {
    cse_error(is->config, "%s:%d: unexpected char at '%c'\n",
	      is->path, is->line, ch);
    return -1;
  }

  is->buf[is->length] = 0;
  child = registry_add(is, node, cse_strdup(is->config, is->buf));

  while ((ch = parse_attribute(is, child, ch)) > 0) {
  }

  if (ch < 0) {
    cse_error(is->config, "%s:%d: unexpected end of file\n",
	      is->path, is->line);
    return ch;
  }
  else if (ch == 0)
    ch = buf_read(is);

  for (; isspace(ch); ch = buf_read(is)) {
  }

  if (ch == '>') {
    *p_node = child;
    return 0;
  }

  else if (ch != '/') {
    cse_error(is->config, "%s:%d: expected '/' at '%c'\n",
	      is->path, is->line, ch);
    return -1;
  }

  ch = buf_read(is);
  if (ch == '>')
    return 1;
  else {
    cse_error(is->config, "%s:%d: expected '>' at '%c'\n",
	      is->path, is->line, ch);
    return -1;
  }
}

/*
 * parses a close tag
 */
static int
parse_end_tag(rstream_t *is, registry_t **node)
{
  int ch;

  is->length = 0;

  for (ch = buf_read(is);
       ch >= 0 && ! isspace(ch) && ch != '/' && ch != '>';
       ch = buf_read(is)) {
    buf_append(is, ch);
  }

  if (is->length == 0) {
    cse_error(is->config, "%s:%d: expected end tag at '%c'\n",
	      is->path, is->line, ch);
    return -1;
  }

  for (; ch >= 0 && isspace(ch); ch = buf_read(is)) {
  }

  if (ch != '>') {
    cse_error(is->config, "%s:%d: expected '>' at '%c'\n",
	      is->path, is->line, ch);
    return -1;
  }

  is->buf[is->length] = 0;
  /* XXX: need to check */
  *node = (*node)->parent;

  return 1;
}

/*
 * skips to the end of the comment
 */
static void
skip_comment(rstream_t *is)
{
  int ch;

  while ((ch = buf_read(is)) >= 0) {
    while (ch == '-') {
      ch = buf_read(is);
      while (ch == '-') {
	if ((ch = buf_read(is)) == '>')
	  return;
      }
    }
  }

  cse_error(is->config, "%s:%d: expected comment end at end of file\n",
	    is->path, is->line);
}

/*
 * skips to the end of the doctype declaration
 */
static void
skip_doctype(rstream_t *is)
{
  int ch;

  while ((ch = buf_read(is)) >= 0) {
    if (ch == '>')
      return;
    else if (ch == '"') {
      for (ch = buf_read(is);
           ch >= 0 && ch != '"';
           ch = buf_read(is)) {
      }
    }
    else if (ch == '\'') {
      for (ch = buf_read(is);
           ch >= 0 && ch != '\'';
           ch = buf_read(is)) {
      }
    }
  }

  cse_error(is->config, "%s:%d: expected doctype end at end of file\n",
	    is->path, is->line);
}

/*
 * skips to the end of the processing instruction
 */
static void
skip_pi(rstream_t *is)
{
  int ch;

  while ((ch = buf_read(is)) >= 0) {
    while (ch == '?') {
      if ((ch = buf_read(is)) == '>')
        return;
    }
  }

  cse_error(is->config, "%s:%d: expected pi end at end of file\n",
	    is->path, is->line);
}

static registry_t *
parse_include(config_t *config, registry_t *node, char *path)
{
  char buf[4096];
  int p = strlen(path);
  char *value = cse_find_value(node->first, "href");
  char *subpath;
  registry_t *subnode;
  registry_t *ptr;
  FILE *is;

  LOG(("pi %s\n", path, value ? value : "null"));

  if (! value || ! value[0])
    return node;

  for (p = strlen(path); p >= 0; p--) {
    if (path[p] == '/' || path[p] == '\\')
      break;
  }
  if (p < 0)
    p = 0;
  
  strcpy(buf, path);
  buf[p] = 0;

  if (value[0] == '/' || value[0] == '\\' ||
      value[1] == ':' &&
      ((value[0] >= 'a' && value[0] <= 'z') ||
       (value[0] >= 'A' && value[0] <= 'Z'))) {
    subpath = value;
  }
  else {
    strcat(buf, "/");
    strcat(buf, value);
    subpath = buf;
  }

  LOG(("subpath %s\n", subpath));

  is = fopen(subpath, "r");
  if (! is)
    return node;

  subnode = cse_parse(is, config, subpath);
  fclose(is);
  if (! subnode || ! subnode->first)
    return node;

  /* Now insert the included configuration in place of the resin:include */
  subnode->parent = node->parent;
  if (node->prev)
    node->prev->next = subnode->first;
  else
    node->parent->first = subnode;
  subnode->prev = node->prev;
  node->parent->last = subnode->last;

  for (ptr = subnode->first; ptr; ptr = ptr->next) {
    ptr->parent = node->parent;
  }

  return subnode;
}

/*
 * Parses the configuration file.
 */
registry_t *
cse_parse(FILE *file, config_t *config, char *path)
{
  int ch;
  rstream_t is;
  int spaceOnly = 1;
  registry_t *root;
  registry_t *node;

  is.file = file;
  is.path = path;
  is.line = 1;
  is.config = config;
  is.length = 0;
  is.capacity = 1024;
  is.buf = cse_alloc(config, is.capacity);

  root = (registry_t *) cse_alloc(config, sizeof(registry_t));
  memset(root, 0, sizeof(registry_t));
  root->key = "";
  node = root;

  while ((ch = buf_read(&is)) >= 0) {
    switch (ch) {
    case ' ': case '\t': case '\n': case '\f':
      buf_append(&is, ch);
      break;

    case '<':
      if (spaceOnly)
	is.length = 0;
      else if (! node->value && ! node->first) {
	is.buf[is.length] = 0;
	node->value = cse_strdup(config, is.buf);
      }

      ch = buf_read(&is);
      if (ch == '!') {
        if ((ch = buf_read(&is)) == 'D') {
          skip_doctype(&is);
          break;
        }
	else if (ch != '-')
	  cse_error(config, "%s:%d: expected comment start at '%c'\n", is.path, is.line, ch);
	if ((ch = buf_read(&is)) != '-')
	  cse_error(config, "%s:%d: expected comment start at '%c'\n", is.path, is.line, ch);
	skip_comment(&is);
      }
      else if (ch == '?') {
        skip_pi(&is);
      }
      else if (ch != '/') {
	if (parse_tag(&is, &node, ch) < 0)
	  return 0;

        if (node->last && ! strcmp(node->last->key, "resin:include"))
          parse_include(config, node->last, path);
      }
      else {
	if (parse_end_tag(&is, &node) < 0)
	  return 0;
        if (node->last && ! strcmp(node->last->key, "resin:include"))
          parse_include(config, node->last, path);
      }
      break;

    default:
      spaceOnly = 0;
      buf_append(&is, ch);
      break;
    }
  }

  return root;
}

/*
 * Returns the next matching node.
 *
 * e.g. to iterate through all foo
 *   for (node = cse_next_link(root->first, "foo");
 *        node;
 *        node = cse_next_line(node->next, "foo")) {
 *   }
 *
 */

registry_t *
cse_next_link(registry_t *node, char *key)
{
  for (; node; node = node->next) {
    if (! strcmp(node->key, key))
      return node;
  }

  return 0;
}

char *
cse_find_value(registry_t *node, char *key)
{
  for (; node; node = node->next) {
    if (! strcmp(node->key, key))
      return node->value;
  }

  return 0;
}

void
cse_print_registry(registry_t *registry)
{
  if (! registry)
    return;

  for (; registry; registry = registry->next) {
    if (registry->first) {
      cse_log("<%s id=%s>\n", registry->key,
	      registry->value ? registry->value : "");
      cse_print_registry(registry->first);
      cse_log("</%s>\n", registry->key);
    }
    else
      cse_log("%s=%s\n", registry->key, registry->value ? registry->value : "");
  }
}
