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

 * CAUCHO TECHNOLOGY AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES
 * SUFFERED BY LICENSEE OR ANY THIRD PARTY AS A RESULT OF USING OR
 * DISTRIBUTING SOFTWARE. IN NO EVENT WILL Caucho OR ITS LICENSORS BE LIABLE
 * FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL,
 * CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND
 * REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF OR
 * INABILITY TO USE SOFTWARE, EVEN IF HE HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGES.      
 */

/*
 * config.c is responsible for scanning the parsed registry and grabbing
 * relevant data.
 *
 * Important data include the web-app and the servlet-mapping so any filter
 * can properly dispatch the request.
 *
 * Also, config.c needs to grab the srun and srun-backup blocks to properly
 * send the requests to the proper JVM.
 */

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cse.h"

static void
cse_add_unique_location(config_t *config, char *host, int port, char *prefix,
                        char *suffix, int is_exact)
{
  location_t *loc;

  for (loc = config->locations; loc; loc = loc->next) {
    if ((host == 0) != (loc->host == 0))
      continue;
    else if (host && strcmp(host, loc->host))
      continue;
    else if (port != loc->port)
      continue;
    else if (is_exact != loc->is_exact)
      continue;
    else if ((prefix == 0) != (loc->prefix == 0))
      continue;
    else if (prefix && strcmp(prefix, loc->prefix))
      continue;
    else if ((suffix == 0) != (loc->suffix == 0))
      continue;
    else if (suffix && strcmp(suffix, loc->suffix))
      continue;

    return;
  }

  loc = (location_t *) cse_alloc(config, sizeof(location_t));

  memset(loc, 0, sizeof(*loc));

  loc->next = config->locations;
  config->locations = loc;

  loc->host = host;
  loc->port = port;
  loc->prefix = prefix;
  loc->suffix = suffix;
  loc->is_exact = is_exact;

  LOG(("loc %s:%d %s %s %x\n", 
       loc->host ? loc->host : "(null)",
       loc->port,
       loc->prefix ? loc->prefix : "(null)",
       loc->suffix ? loc->suffix : "(null)",
       loc->next));
}

/**
 * Add a location pattern to the list of recognized locations
 *
 * @param config the configuration
 * @param host the host for the pattern
 * @param prefix the web-app prefix
 * @param pattern the url-pattern to match
 *
 * @return the new location
 */
static void
cse_add_location(config_t *config, char *host, char *prefix, char *pattern)
{
  char cleanPrefix[4096];
  char *port_string;
  int prefixLength;
  int cleanLength;
  int i;

  char *loc_host;
  int loc_port = 0;
  char *loc_prefix = 0;
  char *loc_suffix = 0;
  int loc_is_exact = 0;
    
#ifdef WIN32
  if (host) {
    host = strdup(host);
    for (i = 0; host[i]; i++)
      host[i] = tolower(host[i]);
  }
  if (prefix) {
    prefix = strdup(prefix);
    for (i = 0; prefix[i]; i++)
      prefix[i] = tolower(prefix[i]);
  }
  if (pattern) {
    pattern = strdup(pattern);
    for (i = 0; pattern[i]; i++)
      pattern[i] = tolower(pattern[i]);
  }
#endif /* WIN32 */

  cleanPrefix[0] = 0;

  loc_host = host ? strdup(host) : 0;
  if (loc_host && (port_string = strchr(loc_host, ':'))) {
    *port_string = 0;
    loc_port = atoi(port_string + 1);
  }

  if (! prefix[0] && pattern[0] && pattern[0] != '/' && pattern[0] != '*')
    strcpy(cleanPrefix, "/");
  else if (prefix[0] && prefix[0] != '/')
    strcpy(cleanPrefix, "/");

  strcat(cleanPrefix, prefix);
  prefixLength = strlen(cleanPrefix);
  if (prefixLength > 0 && cleanPrefix[prefixLength - 1] == '/')
    cleanPrefix[prefixLength - 1] = 0;

  if (! pattern[0]) {
    loc_prefix = cse_strdup(config, cleanPrefix);
    loc_suffix = 0;
  }
  else if (pattern[0] == '*') {
    loc_prefix = cse_strdup(config, cleanPrefix);
    loc_suffix = pattern + 1;
  }
  else {
    if (pattern[0] != '/')
      strcat(cleanPrefix, "/");
    strcat(cleanPrefix, pattern);

    cleanLength = strlen(cleanPrefix);

    if (strlen(pattern) <= 1)
      cleanPrefix[cleanLength - 1] = 0;
    else if (cleanPrefix[cleanLength - 1] != '*')
      loc_is_exact = 1;
    else if (cleanLength > 2)
      cleanPrefix[cleanLength - 2] = 0;

    loc_prefix = cse_strdup(config, cleanPrefix);
    loc_suffix = 0;
  }

  cse_add_unique_location(config, loc_host, loc_port, loc_prefix,
                          loc_suffix, loc_is_exact);
}

static char *
cse_app_dir(config_t *config, registry_t *node, char *parent)
{
  char buf[4096];
  char *value;

  if (! node)
    return parent;
    
  value = cse_find_value(node->first, "app-dir");

  if (! parent || ! *parent)
    parent = ".";

  if ((! value || ! value[0]) && ! strcmp(node->key, "web-app")) {
    value = node->value;
    
    if (value && value[0] == '/')
      value++;
  }
  
  if (! value || ! value[0])
    return parent;

  /* XXX: in theory only for win32 */
  if (value[0] == '/' || value[0] == '\\' ||
      (value[1] == ':' &&
       ((value[0] >= 'a' && value[0] <= 'z') ||
        (value[0] >= 'A' && value[0] <= 'Z')))) {
    return value;
  }

  /* since this only happens on startup, the memory leak is okay */
  sprintf(buf, "%s/%s", parent, value);
  
  return cse_strdup(config, buf);
}

static void
cse_init_web_app_contents(config_t *config, registry_t *node,
                          char *host, char *prefix)
{
  /* This is added automatically on the Java side, but it seems
  // inappropriate to add it to the plugin side.
  // cse_add_location(config, host, "", "*.jsp");
  // cse_add_location(config, host, "", "*.xtp");

  // WEB-INF passes to Resin so Resin can hide it.
  */
  cse_add_location(config, host, prefix, "/WEB-INF/*");
  
  for (; node; node = node->next) {
    if (! strcmp(node->key, "servlet-mapping")) {
      registry_t *url_pattern = cse_next_link(node->first, "url-pattern");

      if (url_pattern && url_pattern->value)
	cse_add_location(config, host, prefix, url_pattern->value);
    }
  }

}

static void
cse_init_web_app(config_t *config, registry_t *node,
                 char *host, char *prefix, char *app_dir)
{
  char buf[4096];
  FILE *file;

  if (! node)
    return;
  
  cse_init_web_app_contents(config, node->first, host, prefix);
  
  /* XXX: should the registry itself get updated? i.e. adding this
  // node somewhere?
  */
  sprintf(buf, "%s/WEB-INF/web.xml", app_dir);

  file = fopen(buf, "r");
  if (file) {
    registry_t *web = cse_parse(file, config, cse_strdup(config, buf));
    fclose(file);

    if (web && web->first)
      cse_init_web_app_contents(config, web->first->first, host, prefix);
  }
}

static void
cse_init_host(config_t *config, registry_t *node, char *host,
              char *app_dir)
{
  char *host_dir = cse_app_dir(config, node, app_dir);
                               
  cse_init_web_app(config, node, host, "", app_dir);

  for (node = node->first; node; node = node->next) {
    if (node->value && ! strcmp(node->key, "web-app")) {
      char *prefix = node->value;
      char *subdir = cse_app_dir(config, node, host_dir);

      cse_init_web_app(config, node, host, prefix, subdir);
    }
  }
}

static void
cse_init_server(config_t *config, registry_t *node)
{
  char *app_dir = cse_app_dir(config, node, config->resin_home);
                               
  cse_init_host(config, node, "", config->resin_home);

  if (! node)
    return;

  for (node = node->first; node; node = node->next) {
    if (node->value && ! strcmp(node->key, "host")) {
      char *name = node->value;
      char *host_dir = cse_app_dir(config, node, app_dir);

      cse_init_host(config, node, name, host_dir);
    }
  }
}

/**
 * Adds a new backup to the configuration
 */
srun_t *
cse_add_host(config_t *config, const char *hostname, int port)
{
  return cse_add_host_int(config, hostname, port, 0, 0);
}

/**
 * Adds a new backup to the configuration
 */
srun_t *
cse_add_backup(config_t *config, const char *hostname, int port)
{
  return cse_add_host_int(config, hostname, port, 1, 0);
}

/**
 * initialize a single srun host
 */
static void
cse_init_host_node(config_t *config, registry_t *node,
		   int is_backup, int is_default)
{
  int port;
  char *host;
  registry_t *host_node = cse_next_link(node, "srun-host");
  registry_t *port_node = cse_next_link(node, "srun-port");
  srun_t *srun = 0;
  registry_t *subnode;

  if (! host_node && ! is_default)
    host_node = cse_next_link(node, "host");
  if (! port_node && ! is_default)
    port_node = cse_next_link(node, "port");
    
  if (port_node && port_node->value && *port_node->value) {
    port = atoi(port_node->value);
  }
  else
    port = 6802;
    
  if (host_node && host_node->value && *host_node->value) {
    host = host_node->value;
  }
  else
    host = "localhost";

  srun = cse_add_host_int(config, host, port, is_backup, is_default);
  if (! srun)
    return;

  subnode = cse_next_link(node, "connect-timeout");
  if (subnode && subnode->value && *subnode->value)
    srun->connect_timeout = atoi(subnode->value);
  
  subnode = cse_next_link(node, "live-time");
  if (subnode && subnode->value && *subnode->value) {
    srun->live_time = atoi(subnode->value);
    if (srun->live_time < 5)
      srun->live_time = 5;
  }
  
  subnode = cse_next_link(node, "dead-time");
  if (subnode && subnode->value && *subnode->value) {
    srun->dead_time = atoi(subnode->value);
    if (srun->dead_time < 5)
      srun->dead_time = 5;
  }
}

/**
 * Initialize all the load-balancing hosts in the configuration.
 */
static void 
cse_init_hosts(config_t *config, registry_t *top)
{
  int has_srun = 0;
  registry_t *node;
  
  for (node = cse_next_link(top, "srun");
       node;
       node = cse_next_link(node->next, "srun")) {
    has_srun = 1;
    cse_init_host_node(config, node->first, 0, 0);
  }

  for (node = cse_next_link(top, "srun-backup");
       node;
       node = cse_next_link(node->next, "srun-backup")) {
    has_srun = 1;
    cse_init_host_node(config, node->first, 1, 0);
  }

  if (! has_srun)
    cse_init_host_node(config, top, 0, 1);
}

void
cse_log_config(config_t *config)
{
  location_t *loc = config->locations;
  for (; loc; loc = loc->next) {
    LOG(("cfg host:%s prefix:%s suffix:%s next:%x\n", 
         loc->host ? loc->host : "null",
         loc->prefix ? loc->prefix : "null",
         loc->suffix ? loc->suffix : "null",
         loc->next));
  }
}

static void
cse_init_error_page(config_t *config, registry_t *node)
{
  for (node = cse_next_link(node, "error-page");
       node;
       node = cse_next_link(node->next, "error-page")) {
    char *exn = cse_find_value(node->first, "exception-type");
    char *code = cse_find_value(node->first, "error-code");
    
    if (exn && ! strcmp(exn, "connection"))
      config->error_page = cse_find_value(node->first, "location");
    else if (! exn && ! code && ! config->error_page)
      config->error_page = cse_find_value(node->first, "location");
  }
}

void
cse_init_config(config_t *config)
{
  registry_t *root = 0;
  registry_t *node = 0;
  FILE *is;
  char *caucho_status;

  if (! config->resin_home) {
    int p = strlen(config->path);
    
    for (;
         p >= 0 && (config->path[p] != '/' && config->path[p] != '\\');
         p--) {
    }
    
    for (p--;
         p >= 0 && (config->path[p] != '/' && config->path[p] != '\\');
         p--) {
    }

    if (p >= 0) {
      config->resin_home = strdup(config->path);
      config->resin_home[p] = 0;
    }
    else {
      config->resin_home = ".";
    }
  }

  is = fopen(config->path, "r");
  if (! is) {
    cse_error(config, "Resin cannot open config file `%s'\n", config->path);
    return;
  }

  root = cse_parse(is, config, config->path);
  fclose(is);

  config->registry = root;
  cse_print_registry(root);
  
  if (root)
    node = cse_next_link(root->first, "caucho.com");
  if (node)
    node = cse_next_link(node->first, "http-server");

  if (node) {
    caucho_status = cse_find_value(node->first, "caucho-status");
	if (caucho_status && 
		(! strcmp(caucho_status, "false") ||
                 ! strcmp(caucho_status, "no")))
		config->disable_caucho_status = 1;
  }

  if (node)
    cse_init_server(config, node);

  if (node)
    cse_init_hosts(config, node->first);

  if (node)
    cse_init_error_page(config, node->first);

  config->lock = cse_create_lock();
}

/**
 * tests if 'full' starts with 'part'
 *
 * If it's not an exact match, the next character of 'full' must be '/'.
 * That way, a servlet mapping of '/foo/*' will match /foo, /foo/bar, but
 * not /foolish.
 */
static int
cse_starts_with(const char *full, const char *part)
{
  char ch1, ch2;

  while ((ch2 = *part++) && (ch1 = *full++) && ch1 == ch2) {
  }

  if (ch2)
    return 0;
  else if (! *full)
    return 1;
  else
    return *full == '/';
}

static int
cse_match_suffix(const char *full, const char *suffix)
{
  int len = strlen(suffix);

  do {
    char *match = strstr(full, suffix);
    if (! match)
      return 0;
    
    if (! match[len] || match[len] == '/')
      return 1;

    full = match + len;
  } while (*full);

  return 0;
}

static int
cse_match_location(location_t *loc,
                   const char *host, int port,
                   const char *uri)
{
  for (; loc; loc = loc->next) {
    /*
    LOG(("match host:%s:%d prefix:%s suffix:%s with host:%s uri:%s next:%x\n", 
         loc->host ? loc->host : "null",
         loc->port,
         loc->prefix ? loc->prefix : "null",
         loc->suffix ? loc->suffix : "null",
         host, uri, loc->next));
    */
    
    if (*(loc->host) && ! strstr(loc->host, host))
      continue;

    else if (port && loc->port && port != loc->port)
      continue;

    else if (loc->is_exact && ! strcmp(uri, loc->prefix))
      return 1;
    
    else if (loc->is_exact)
      continue;
      
    else if (! cse_starts_with(uri, loc->prefix))
      continue;
    
    else if (loc->suffix && ! cse_match_suffix(uri, loc->suffix))
      continue;

    return 1;
  }

  return 0;
}

/**
 * Tests if the request matches a Resin URL.
 */
int
cse_match_request(config_t *config, const char *host,
                  int port, const char *uri)
{
  char *param = strstr(uri, ";jsessionid=");
  const char *test = uri;
  char buf[4096];
  int i;
  
  if (param) {
    strcpy(buf, uri);
    buf[param - uri] = 0;
    test = buf;
    
#ifdef WIN32
    for (i = 0; buf[i]; i++)
      buf[i] = tolower(buf[i]);
#endif /* WIN32 */
  }
  else {
#ifdef WIN32
    strcpy(buf, uri);
    test = buf;
    for (i = 0; buf[i]; i++)
      buf[i] = tolower(buf[i]);
#endif /* WIN32 */
 }

  return (cse_match_location(config->locations, host, port, test) ||
          strstr(test, "/j_security_check"));
}
