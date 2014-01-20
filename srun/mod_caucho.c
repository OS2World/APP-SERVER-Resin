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
 * DISTRIBUTING SOFTWARE. IN NO EVENT WILL CAUCHO OR ITS LICENSORS BE LIABLE
 * FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL,
 * CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND
 * REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF OR
 * INABILITY TO USE SOFTWARE, EVEN IF HE HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGES.
 *
 * @author Scott Ferguson
 *
 * $Id: mod_caucho.c,v 1.18.6.12 2000/09/22 20:54:22 ferg Exp $
 */

#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_log.h"
#include "http_main.h"
#include "util_script.h"
#include "http_conf_globals.h"
#include <stdlib.h>
#include <errno.h>

#include "cse.h"

/*
 * Apache magic module declaration.
 */
module MODULE_VAR_EXPORT caucho_module;

#define BUF_LENGTH 8192
#define DEFAULT_PORT 6802
#define DEAD_TIME 5
#define LIVE_TIME 5

static char *g_error_page = 0;

void
cse_log(char *fmt, ...)
{
#ifdef DEBUG
  va_list args;
  FILE *file = fopen("/tmp/log", "a+");

  if (file) {  
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);
    fclose(file);
  }
#endif
}

void *
cse_malloc(int length)
{
	return malloc(length);
}

char *
cse_alloc(config_t *config, int length)
{
  return ap_palloc(config->pool, length);
}

char *
cse_strdup(config_t *config, const char *string)
{
  return ap_pstrdup(config->pool, string);
}

void *
cse_create_lock()
{
#ifdef WIN32
	return CreateMutex(0, 0, "resin");
#else
	return 0;
#endif
}

int
cse_lock(void *lock)
{
#ifdef WIN32
	if (lock)
		WaitForSingleObject(lock, INFINITE);
	return 1;
#else
  return 1;
#endif
}

void
cse_unlock(void *lock)
{
#ifdef WIN32
	if (lock)
		ReleaseMutex(lock);
#endif
}

void
cse_error(config_t *config, char *format, ...)
{
  char buf[BUF_LENGTH];
  va_list args;

  va_start(args, format);
  vsprintf(buf, format, args);
  va_end(args);

  config->error = ap_pstrdup(config->pool, buf);
}

void
cse_set_socket_cleanup(int socket, void *pool)
{
  if (socket > 0)
    ap_note_cleanups_for_socket(pool, socket);
}

void
cse_kill_socket_cleanup(int socket, void *pool)
{
  if (socket > 0)
    ap_kill_cleanups_for_socket(pool, socket);
}

void cse_free(config_t *config, void *data) {}

#ifdef WIN32

static void
cse_module_init(struct server_rec *server, struct pool *pool)
{
  ap_add_version_component("Resin/1.1.5");
}
#else
static void
cse_module_init(struct server_rec *server, struct pool *pool)
{
  ap_add_version_component("Resin/1.1.5");
}
#endif

static void *
cse_create_server_config(pool *p, server_rec *server)
{
  config_t *config = (config_t *) ap_palloc(p, sizeof(config_t));
  srun_t *srun_list = (srun_t *) ap_palloc(p, 16 * sizeof(srun_t));
  srun_t *srun;

  memset(config, 0, sizeof(config_t));
  memset(srun_list, 0, 16 * sizeof(srun_t));

  config->server = server;
  config->pool = p;
  config->srun_list = srun_list;
  config->srun_capacity = 16;
  config->srun_size = 1;
  
  srun = config->srun_list;

  srun->port = DEFAULT_PORT;
  srun->session = 0;
  
  srun->max_sockets = 32;

  return (void *) config;
}

/**
 * Retrieves the caucho configuration from Apache
 */
static config_t *
cse_get_module_config(server_rec *s)
{
  return (config_t *) ap_get_module_config(s->module_config, &caucho_module);
}

/**
 * Parse the CauchoConfigHost configuration in the apache config file.
 */
static const char *
cse_config_file_command(cmd_parms *cmd, void *mconfig, char *value)
{
  config_t *config = cse_get_module_config(cmd->server);
 
  config->path = ap_server_root_relative(cmd->pool, value);

  cse_init_config(config);

  return config->error;
}

/**
 * Parse the CauchoHosts configuration in the apache config file.
 */
static const char *
cse_host_command(cmd_parms *cmd, void *mconfig, char *host_arg, char *port_arg)
{
  config_t *config = cse_get_module_config(cmd->server);
  int port = port_arg ? atoi(port_arg) : DEFAULT_PORT;

  cse_add_host(config, host_arg, port);

  return config->error;
}

/**
 * Parse the CauchoBackup configuration in the apache config file.
 */
static const char *
cse_backup_command(cmd_parms *cmd, void *mconfig,
		   char *host_arg, char *port_arg)
{
  config_t *config = cse_get_module_config(cmd->server);
  int port = port_arg ? atoi(port_arg) : DEFAULT_PORT;

  cse_add_backup(config, host_arg, port);

  return config->error;
}

/**
 * Parse the CauchoKeepalive configuration in the apache config file.
 */
static const char *
cse_keepalive_command(cmd_parms *cmd, void *mconfig, int flag)
{
  /* g_srun_keepalive = flag; */

  return 0;
}

/**
 * Parse the CauchoHosts configuration in the apache config file.
 */
static const char *
cse_error_page_command(cmd_parms *cmd, void *mconfig, char *error_page_arg)
{
  g_error_page = error_page_arg;

  return 0;
}

/**
 * Look at the request to see if Caucho should handle it.
 */
static int
cse_dispatch(request_rec *r)
{
  config_t *config = cse_get_module_config(r->server);
  const char *host = ap_get_server_name(r);
  const char *uri = r->uri;

  if (config == NULL)
    return DECLINED;

  /*
  //cse_update_config(config, r->request_time);
  */
  
  /* Check for exact virtual host match */
  if (cse_match_request(config, host, ap_get_server_port(r), uri)) {
    ap_set_module_config(r->request_config, &caucho_module, 0);
    r->handler = "caucho-request";
    return OK;
  }

  if (! config->disable_caucho_status && ! strcmp(uri, "/caucho-status")) {
    ap_set_module_config(r->request_config, &caucho_module, 0);
    r->handler = "caucho-status";
    return OK;
  }

  return DECLINED;
}

/**
 * Gets the session index from the request
 *
 * Cookies have priority over the query
 *
 * @return -1 if no session
 */
static int
get_session_index(request_rec *r)
{
  array_header *hdrs_arr = ap_table_elts(r->headers_in);
  table_entry *hdrs = (table_entry *) hdrs_arr->elts;
  int i;
  int session;

  for (i = 0; i < hdrs_arr->nelts; ++i) {
    if (! hdrs[i].key || ! hdrs[i].val)
      continue;

    if (strcasecmp(hdrs[i].key, "Cookie"))
      continue;

    session = cse_session_from_string(hdrs[i].val, "JSESSIONID=");
    if (session >= 0)
      return session;
  }

  return cse_session_from_string(r->uri, ";jsessionid=");
}

static int
handle_include(char *uri, request_rec *r)
{
  request_rec *subrequest = ap_sub_req_lookup_uri(uri, r);
  int status;

  LOG(("forwarding %s %d\n", uri, subrequest->status));
  
  if (subrequest->status != HTTP_OK)
    return -1;

  /* copy the args from the original request */
  subrequest->path_info = r->path_info;
  subrequest->args = r->args;

  /* Run it. */

  status = ap_run_sub_req(subrequest);

  ap_destroy_sub_req(subrequest);

  return status;
}

static int
connection_error(config_t *config, srun_t *srun, request_rec *r)
{
  char *error_page = config->error_page;
  
  if (g_error_page)
    error_page = g_error_page;

  if (error_page) {
    int status = handle_include(error_page, r);
    if (status == OK)
      return status;
  }

  r->content_type = "text/html";
  if (error_page) {
    r->status = 302;
    ap_table_set(r->headers_out, "Location", error_page);
  }
  else
    r->status = 503;

  ap_send_http_header(r);

  ap_rputs("<html><body bgcolor='white'>", r);
  ap_rprintf(r, "<h1>Can't contact servlet runner at %s:%d</h1>", 
	     srun->hostname ? srun->hostname : "localhost",
	     srun->port);
  ap_rputs("</body></html>", r);

  ap_kill_timeout(r);

  return OK;
}

/**
 * Writes request parameters to srun.
 */
static void
write_env(stream_t *s, request_rec *r)
{
  char buf[1024];
  int i;
  
  conn_rec *c = r->connection;
  const char *host;
  int port;

  cse_write_string(s, CSE_PROTOCOL, r->protocol);
  cse_write_string(s, CSE_METHOD, r->method);
  for (i = 0; r->unparsed_uri[i] && r->unparsed_uri[i] != '?'; i++) 
      buf[i] = r->unparsed_uri[i];
  buf[i] = 0;
  cse_write_string(s, CSE_URI, buf);

  if (r->args)
    cse_write_string(s, CSE_QUERY_STRING, r->args);

  cse_write_string(s, CSE_SERVER_NAME, ap_get_server_name(r));
  port = ap_get_server_port(r);
  cse_write_string(s, CSE_SERVER_PORT, ap_psprintf(r->pool, "%u", port));

  host = ap_get_remote_host(c, r->per_dir_config, REMOTE_HOST);
  if (host)
    cse_write_string(s, CSE_REMOTE_HOST, host);
  else
    cse_write_string(s, CSE_REMOTE_HOST, c->remote_ip);

  cse_write_string(s, CSE_REMOTE_ADDR, c->remote_ip);
  cse_write_string(s, CSE_REMOTE_PORT,
		   ap_psprintf(r->pool, "%d", ntohs(c->remote_addr.sin_port)));
  
  if (c->user)
    cse_write_string(s, CSE_REMOTE_USER, c->user);
  if (c->ap_auth_type)
    cse_write_string(s, CSE_AUTH_TYPE, c->ap_auth_type);

  cse_write_string(s, CSE_SESSION_GROUP,
		   ap_psprintf(r->pool, "%d", s->srun->session));

  /* mod_ssl */
#ifdef EAPI
  {
    static char *vars[] = { "SSL_CLIENT_S_DN",
                            "SSL_CIPHER",
                            "SSL_CIPHER_EXPORT",
                            "SSL_PROTOCOL",
                            "SSL_CIPHER_USEKEYSIZE",
                            "SSL_CIPHER_ALGKEYSIZE",
                            0};
    char *var;
    int i;
    int v;
    
    if ((v = ap_hook_call("ap::mod_ssl::var_lookup", &var, r->pool, r->server,
                          r->connection, r, "SSL_CLIENT_CERT"))) {
      cse_write_string(s, CSE_CLIENT_CERT, var);
    }

    for (i = 0; vars[i]; i++) {
      if ((v = ap_hook_call("ap::mod_ssl::var_lookup", &var,
                            r->pool, r->server, r->connection, r, vars[i]))) {
        cse_write_string(s, CSE_HEADER, vars[i]);
        cse_write_string(s, CSE_VALUE, var);
      }
    }
  }
#endif  
}

/**
 * Writes headers to srun.
 */
static void
write_headers(stream_t *s, request_rec *r)
{
  array_header *hdrs_arr = ap_table_elts(r->headers_in);
  table_entry *hdrs = (table_entry *) hdrs_arr->elts;
  int i;

  for (i = 0; i < hdrs_arr->nelts; ++i) {
    if (! hdrs[i].key)
      continue;

    /*
     * Content-type and Content-Length are special cased for a little
     * added efficiency.
     */

    if (! strcasecmp(hdrs[i].key, "Content-type"))
      cse_write_string(s, CSE_CONTENT_TYPE, hdrs[i].val);
    else if (! strcasecmp(hdrs[i].key, "Content-length"))
      cse_write_string(s, CSE_CONTENT_LENGTH, hdrs[i].val);
    else {
      cse_write_string(s, CSE_HEADER, hdrs[i].key);
      cse_write_string(s, CSE_VALUE, hdrs[i].val);
    }
  }
}

static void
write_added_headers(stream_t *s, request_rec *r)
{
  array_header *hdrs_arr = ap_table_elts(r->subprocess_env);
  table_entry *hdrs = (table_entry *) hdrs_arr->elts;
  int i;

  for (i = 0; i < hdrs_arr->nelts; ++i) {
    if (! hdrs[i].key)
      continue;

    if (! strcmp(hdrs[i].key, "HTTPS") &&
	! strcmp(hdrs[i].val, "on")) {
      cse_write_string(s, CSE_IS_SECURE, "");
    }
    else if (! r->connection->user && ! strcmp(hdrs[i].key, "SSL_CLIENT_DN"))
      cse_write_string(s, CSE_REMOTE_USER, hdrs[i].val);
      

    cse_write_string(s, CSE_HEADER, hdrs[i].key);
    cse_write_string(s, CSE_VALUE, hdrs[i].val);
  }
}

/**
 * Writes a response from srun to the client
 */
static int
cse_write_response(stream_t *s, int len, request_rec *r)
{
  while (len > 0) {
    int sublen;
    int writelen;
    int sentlen;

    if (s->read_offset >= s->read_length && cse_fill_buffer(s) < 0)
      return -1;

    sublen = s->read_length - s->read_offset;
    if (len < sublen)
      sublen = len;

    writelen = sublen;
    while (writelen > 0) {
      sentlen = ap_rwrite(s->read_buf + s->read_offset, writelen, r);
      if (sentlen < 0) {
	cse_close(s, "write");
	return -1;
      }

      writelen -= sublen;
    }
    
    s->read_offset += sublen;
    len -= sublen;
  }
  
  return 1;
}

/**
 * Copy data from the JVM to the browser.
 */
static int
send_data(stream_t *s, request_rec *r, int ack, int *keepalive)
{
  int code = CSE_END;
  char buf[8193];
  char key[8193];
  char value[8193];
  int i;
  
  do {
    int l1, l2, l3;
    int len;

    ap_reset_timeout(r);
    
    code = cse_read_byte(s);
    l1 = cse_read_byte(s) & 0xff;
    l2 = cse_read_byte(s) & 0xff;
    l3 = cse_read_byte(s);
    len = (l1 << 16) + (l2 << 8) + (l3 & 0xff);

    if (s->socket < 0)
      return -1;

    switch (code) {
    case CSE_STATUS:
      cse_read_all(s, buf, len);
      buf[len] = 0;
      for (i = 0; buf[i] && buf[i] != ' '; i++) {
      }
      i++;
      r->status = atoi(buf + i);
      r->status_line = ap_pstrdup(r->pool, buf + i);
      break;

    case CSE_HEADER:
      cse_read_all(s, key, len);
      key[len] = 0;
      cse_read_string(s, value, sizeof(value));
      if (! strcasecmp(key, "content-type"))
	r->content_type = ap_pstrdup(r->pool, value);
      else if (! strcasecmp(key, "set-cookie"))
	ap_table_add(r->headers_out, key, value);
      else
	ap_table_set(r->headers_out, key, value);
      break;

    case CSE_DATA:
      if (cse_write_response(s, len, r) < 0)
	return -1;
      break;

    case CSE_FLUSH:
      ap_rflush(r);
      break;

    case CSE_KEEPALIVE:
      *keepalive = 1;
      break;

    case CSE_SEND_HEADER:
      ap_send_http_header(r);
      break;

    case -1:
      break;

    default:
      cse_read_all(s, buf, len);
      break;
    }
  } while (code > 0 && code != CSE_END && code != CSE_CLOSE && code != ack);

  return code;
}

/**
 * handles a client request
 */
static int
write_request(stream_t *s, request_rec *r, config_t *config, int *keepalive)
{
  int len;
  int code;

  write_env(s, r);
  write_headers(s, r);
  write_added_headers(s, r);

  /* read post data */
  if (ap_should_client_block(r)) {
    char buf[BUF_LENGTH];
    int isFirst = 1;

    while ((len = ap_get_client_block(r, buf, BUF_LENGTH)) > 0) {
      ap_reset_timeout(r);

      if (! isFirst) {
        code = send_data(s, r, CSE_ACK, keepalive);
        if (code < 0 || code == CSE_END || code == CSE_CLOSE)
          break;
      }

      cse_write_packet(s, CSE_DATA, buf, len);
      isFirst = 0;
    }
  }

  cse_write_packet(s, CSE_END, 0, 0);

  code = send_data(s, r, CSE_END, keepalive);
  
  return code == CSE_END;
}

/**
 * Handle a request.
 */
static int
caucho_request(request_rec *r)
{
  config_t *config = cse_get_module_config(r->server);
  stream_t s;
  int retval;
  int keepalive = 0;
  int reuse;
  int session_index;
  char *ip;
  int rand = 0;
  
#ifndef _WIN32
  rand = random();
#endif

  if ((retval = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)))
    return retval;

  ap_soft_timeout("servlet request", r);

  session_index = get_session_index(r);
  ip = r->connection->remote_ip;
 
  if (! cse_open_connection(&s, config, session_index, ip,
                            r->request_time, rand, r->pool)) {
    connection_error(config, config->srun_list, r);
  
    ap_kill_timeout(r);
    return OK;
  }

  reuse = write_request(&s, r, config, &keepalive);
  
  ap_kill_timeout(r);
  ap_rflush(r);

  if (reuse)
    cse_recycle(&s);
  else
    cse_close(&s, "no reuse");

  return OK;
}

/**
 * Print the statistics for each JVM.
 */
static void
jvm_status(config_t *config, request_rec *r)
{
  int i;
  stream_t s;
  int code;

  ap_rputs("<center><table border=2 cellspacing=0 cellpadding=2 width='80%'>\n", r);
  /*
  ap_rputs("<tr><th>Host\n", r);
  ap_rputs("    <th>Minute<br>%cpu/thread\n", r);
  ap_rputs("    <th>Hour<br>%cpu/thread\n", r);
  ap_rputs("    <th>Day<br>%cpu/thread\n", r);
  */


  ap_rputs("<center><table border=2 width='80%'>\n", r);
  ap_rputs("<tr><th>Host</th>\n", r);
  ap_rputs("    <th>Connect<br>Timeout</th>\n", r);
  ap_rputs("    <th>Live<br>Time</th>\n", r);
  ap_rputs("    <th>Dead<br>Time</th>\n", r);
  ap_rputs("</tr>\n", r);

  for (i = 0; i < config->srun_size; i++) {
    srun_t *srun = config->srun_list + i;
    int port = srun->port;

    ap_rputs("<tr>", r);

    if (! cse_open(&s, config, srun, r->pool, 1)) {
      ap_rprintf(r, "<td bgcolor='#ff6666'>%s:%d%s (down)</td>",
	       srun->hostname ? srun->hostname : "localhost",
	       port, srun->is_backup ? "*" : "");
    }
    else {
      char buf[8193];
      FILE *file;
      registry_t *reg;
 
      ap_rprintf(r, "<td bgcolor='#66ff66'>%s:%d%s (ok)</td>",
                 srun->hostname ? srun->hostname : "localhost",
                 port, srun->is_backup ? "*" : "");
    }
    
    cse_recycle(&s);
    
    ap_rprintf(r, "<td align=right>%d</td><td align=right>%d</td><td align=right>%d</td>",
               srun->connect_timeout, srun->live_time, srun->dead_time);
    ap_rputs("</tr>\n", r);
  }
  ap_rputs("</table></center>\n", r);
}

/**
 * Print a summary of the configuration so users can understand what's
 * going on.  Ping the server to check that it's up.
 */
static int
caucho_status(request_rec *r)
{
  config_t *config = cse_get_module_config(r->server);
  location_t *loc;
 
  r->content_type = "text/html";
  ap_soft_timeout("caucho status", r);
  if (r->header_only) {
    ap_kill_timeout(r);

    return OK;
  }

  ap_send_http_header(r);

  ap_rputs("<html><title>Status : Caucho Servlet Engine</title>\n", r);
  ap_rputs("<body bgcolor=white>\n", r);
  ap_rputs("<h1>Status : Caucho Servlet Engine</h1>\n", r);
  
  jvm_status(config, r);

  ap_rputs("<p><center><table border=2 cellspacing=0 cellpadding=2 width='80%'>\n", r);
  ap_rputs("<tr><th width='50%'>Host\n", r);
  ap_rputs("    <th>url-pattern\n", r);
  
  loc = config ? config->locations : 0;
  for (; loc; loc = loc->next) {
    ap_rprintf(r, "<tr bgcolor='#ffcc66'><td>%s<td>%s%s%s%s</tr>\n", 
	       loc->host && loc->host[0] ? loc->host : "*",
	       loc->prefix,
	       ! loc->is_exact && ! loc->suffix ? "/*" : 
	       loc->suffix && loc->prefix[0] ? "/" : "",
	       loc->suffix ? "*" : "",
	       loc->suffix ? loc->suffix : "");
  }

  ap_rputs("</table></center>\n", r);
  ap_rputs("</body></html>\n", r);
  ap_kill_timeout(r);

  return OK;
}

/**
 * When a child process starts, clear the srun structure so it doesn't
 * mistakenly think the old sockets areopen.
 */
static void
cse_open_child(server_rec *server, pool *p)
{
  config_t *config = cse_get_module_config(server);
  int i;

  LOG(("[%d] open child\n", getpid()));

  for (i = 0; i < config->srun_size; i++) {
    srun_t *srun = config->srun_list + i;
    srun->n_sockets = 0;
    srun->max_sockets = 32;
    srun->is_dead = 0;
    srun->last_time = 0;
  }
}

static void
cse_close_child(server_rec *server, pool *p)
{
  config_t *config = cse_get_module_config(server);
  int i;
  
  LOG(("[%d] close child\n", getpid()));

  for (i = 0; i < config->srun_size; i++) {
    srun_t *srun = config->srun_list + i;
    while (srun->n_sockets > 0) {
      int socket = srun->sockets[--srun->n_sockets];
      
      send(socket, "X\0\0\0", 4, 0);
      closesocket(socket);
    }

    srun->n_sockets = 0;
  }
}

/*
 * Only needed configuration is pointer to resin.conf
 */
static command_rec caucho_commands[] = {
    {"CauchoConfigFile", cse_config_file_command, NULL, RSRC_CONF, TAKE1,
     "Pointer to the Caucho configuration file."},
    {"CauchoHost", cse_host_command, NULL, RSRC_CONF, TAKE12,
     "Servlet runner host."},
    {"CauchoBackup", cse_backup_command, NULL, RSRC_CONF, TAKE12,
     "Servlet runner backup."},
    {"CauchoKeepalive", cse_keepalive_command, NULL, RSRC_CONF, FLAG,
     "keeps srun connections alive."},
    {"CauchoErrorPage", cse_error_page_command, NULL, RSRC_CONF, TAKE1,
     "Error page when connections fail."},
    {NULL}
};

/*
 * Caucho right has two content handlers:
 * caucho-status: summary information for debugging
 * caucho-request: dispatch a Caucho request
 */
static const handler_rec caucho_handlers[] =
{
    {"caucho-status", caucho_status},
    {"caucho-request", caucho_request},
    {NULL}
};

/* 
 * module configuration
 */
module caucho_module =
{
    STANDARD_MODULE_STUFF,
    cse_module_init,            /* module initializer */
    NULL,                       /* per-directory config creator */
    NULL,                       /* dir config merger */
    cse_create_server_config,   /* server config creator */
    NULL,                       /* server config merger */
    caucho_commands,            /* command table */
    caucho_handlers,            /* [7] list of handlers */
    NULL,                       /* [2] filename-to-URI translation */
    NULL,                       /* [5] check/validate user_id */
    NULL,                       /* [6] check user_id is valid *here* */
    NULL,                       /* [4] check access by host address */
    cse_dispatch,               /* [7] MIME type checker/setter */
    NULL,                       /* [8] fixups */
    NULL,                       /* [10] logger */
    NULL,                       /* [3] header parser */
    cse_open_child,             /* apache child process init */
    cse_close_child,            /* apache child process exit/cleanup */
    NULL,                       /* [1] post read_request handling */
#ifdef EAPI
    NULL,                       /* add_module */
    NULL,                       /* del_module */
    NULL,                       /* rewrite_command */
    NULL,                       /* new_connection */
#endif    
};


