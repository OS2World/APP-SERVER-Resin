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
 * $Id: stream.c,v 1.7.6.11 2000/09/22 20:54:23 ferg Exp $
 */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "cse.h"

#define DEFAULT_PORT 6802
#define DEAD_TIME 120
#define LIVE_TIME 10
#define CONNECT_TIMEOUT 2

static int g_count;

void
cse_close(stream_t *s, char *msg)
{
  int socket = s->socket;

  LOG(("[%d] close %d %s\n", getpid(), s->socket, msg));

  s->socket = -1;
  cse_kill_socket_cleanup(socket, s->pool);
  closesocket(socket);
}

#ifdef WIN32

static int
cse_connect(struct sockaddr_in *sin, srun_t *srun)
{
  unsigned int sock;
  unsigned long is_nonblock;

  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock == INVALID_SOCKET) {
    LOG(("mod_caucho can't create socket.\n"));
    return -1; /* bad socket */
  }

  is_nonblock = 1;
  ioctlsocket(sock, FIONBIO, &is_nonblock);
  if (connect(sock, (struct sockaddr *) sin, sizeof(struct sockaddr_in))) {
    WSAEVENT event = WSACreateEvent();
    WSANETWORKEVENTS networkResult;
    int result;

    WSAEventSelect(sock, event, FD_CONNECT);
    result = WSAWaitForMultipleEvents(1, &event, 0,
                                      srun->connect_timeout * 1000, 0);
    WSAEnumNetworkEvents(sock, event, &networkResult);
    WSAEventSelect(sock, 0, 0);
    WSACloseEvent(event);

    if (result != WSA_WAIT_EVENT_0 ||
 	networkResult.iErrorCode[FD_CONNECT_BIT] != NO_ERROR) {
      closesocket(sock);

      return -1;
    }
  }

  is_nonblock = 0;
  ioctlsocket(sock, FIONBIO, &is_nonblock);
  LOG(("connect"));

  return sock;
}

#else

static int
cse_connect(struct sockaddr_in *sin, srun_t *srun)
{
  int sock;
  fd_set write_fds;
  struct timeval timeout;
  int flags;
  int error = 0;
  int len = sizeof(error);

  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    LOG(("mod_caucho can't create socket.\n"));
    return -1; /* bad socket */
  }

  flags = fcntl(sock, F_GETFL);
  fcntl(sock, F_SETFL, O_NONBLOCK|flags);
  FD_ZERO(&write_fds);
  FD_SET(sock, &write_fds);

  timeout.tv_sec = srun->connect_timeout;
  timeout.tv_usec = 0;

  if (! connect(sock, (const struct sockaddr *) sin, sizeof(*sin))) {
    fcntl(sock, F_SETFL, flags);

    return sock;
  }
  else if (errno != EWOULDBLOCK && errno != EINPROGRESS) {
    LOG(("connect quickfailed %x %d %d\n", sin->sin_addr.s_addr,
	 ntohs(sin->sin_port), errno));
    
    close(sock);

    return -1;
  }
  else if (select(sock + 1, 0, &write_fds, 0, &timeout) <= 0) {
    LOG(("timeout %x %d %d\n", sin->sin_addr.s_addr,
	 ntohs(sin->sin_port), errno));

    fcntl(sock, F_SETFL, flags);

    close(sock);
    
    return -1;
  }
  else if (! FD_ISSET(sock, &write_fds) ||
           getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0 ||
           error) {
    LOG(("connect failed %x %d %d\n", sin->sin_addr.s_addr,
	 ntohs(sin->sin_port), errno));
    close(sock);

    return -1;
  }
  else {
    fcntl(sock, F_SETFL, flags);

    LOG(("[%d] connect %x:%d -> %d\n", getpid(),
         sin->sin_addr.s_addr, ntohs(sin->sin_port), sock));
    
    return sock;
  }
}

#endif

static int
cse_connect_wait(struct sockaddr_in *sin)
{
  int sock;

  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    LOG(("mod_caucho can't create socket.\n"));
    return -1; /* bad socket */
  }
  
  if (! connect(sock, (const struct sockaddr *) sin, sizeof(*sin))) {
    return sock;
  }
  
  LOG(("cse_connect_wait can't connect %x %d %d\n", sin->sin_addr.s_addr,
       ntohs(sin->sin_port), errno));

  closesocket(sock);
    
  return -1;
}

int
cse_open(stream_t *s, config_t *config, srun_t *srun,
         void *p, int wait)
{
  struct sockaddr_in sin;
 
  s->config = config;
  s->socket = -1;
  s->update_count = 0;
  s->pool = p;
  s->write_length = 0;
  s->read_length = 0;
  s->read_offset = 0;
  s->srun = srun;

  sin.sin_family = AF_INET;
  if (srun->host)
    sin.sin_addr = *srun->host;
  else
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (srun->port <= 0)
    srun->port = DEFAULT_PORT;

  sin.sin_port = htons((short) srun->port);

  if (wait || srun->connect_timeout <= 0)
    s->socket = cse_connect_wait(&sin);
  else
    s->socket = cse_connect(&sin, srun);

  return s->socket >= 0;
}

int
cse_flush(stream_t *s)
{
  int len;

  if (s->write_length > 0) {
    len = send(s->socket, s->write_buf, s->write_length, 0);

    if (len != s->write_length)
      return -1;

    s->write_length = 0;
  }

  return 0;
}

int
cse_fill_buffer(stream_t *s)
{
  int i = 0;
  
  if (s->socket < 0 || cse_flush(s) < 0)
    return -1;

  s->read_offset = 0;
  s->read_length = recv(s->socket, s->read_buf, BUF_LENGTH, 0);

  if (s->read_length <= 0) {
    cse_close(s, "fill_buffer");
    
    return -1;
  }
  
  return s->read_length;
}

int
cse_read_byte(stream_t *s)
{
  if (s->read_offset >= s->read_length) {
    if (cse_fill_buffer(s) < 0)
      return -1;
  }

  return s->read_buf[s->read_offset++];
}

void
cse_write(stream_t *s, const char *buf, int length)
{
  /* XXX: writev??? */

  if (s->write_length + length > BUF_LENGTH) {
    if (s->write_length > 0) {
      int len;

      len = send(s->socket, s->write_buf, s->write_length, 0);
			       
      s->write_length = 0;
      
      if (len < 0) {
	cse_close(s, "write");
	return;
      }
    }

    if (length > BUF_LENGTH) {
      int len;

      len = send(s->socket, s->write_buf, s->write_length, 0);
			       
      if (len < 0)
	cse_close(s, "write");

      return;
    }
  }

  memcpy(s->write_buf + s->write_length, buf, length);
  s->write_length += length;
}

int
cse_read_all(stream_t *s, char *buf, int len)
{
  int read_len = len;
  
  while (len > 0) {
    int sublen;

    if (s->read_offset >= s->read_length) {
      if (cse_fill_buffer(s) < 0)
	return -1;
    }

    sublen = s->read_length - s->read_offset;
    if (len < sublen)
      sublen = len;

    memcpy(buf, s->read_buf + s->read_offset, sublen);

    buf += sublen;
    len -= sublen;
    s->read_offset += sublen;
  }

  return read_len;
}

int
cse_skip(stream_t *s, int len)
{
  while (len > 0) {
    int sublen;

    if (s->read_offset >= s->read_length) {
      if (cse_fill_buffer(s) < 0)
	return -1;
    }

    sublen = s->read_length - s->read_offset;
    if (len < sublen)
      sublen = len;

    len -= sublen;
    s->read_offset += sublen;
  }

  return 1;
}

int
cse_read_limit(stream_t *s, char *buf, int buflen, int readlen)
{
  int result;
  
  if (buflen > readlen) {
    result = cse_read_all(s, buf, readlen);
    buf[readlen] = 0;
  }
  else {
    result = cse_read_all(s, buf, buflen);
    buf[buflen - 1] = 0;
    cse_skip(s, readlen - buflen);
  }

  return result;
}

/**
 * write a packet to srun
 *
 * @param s stream to srun
 * @param code packet code
 * @param buf data buffer
 * @param length length of data in buffer
 */
void
cse_write_packet(stream_t *s, char code, const char *buf, int length)
{
  char temp[4];

  temp[0] = code;
  temp[1] = (length >> 16) & 0xff;
  temp[2] = (length >> 8) & 0xff;
  temp[3] = (length) & 0xff;

  cse_write(s, temp, 4);
  if (length >= 0)
    cse_write(s, buf, length);
}

/**
 * writes a string to srun
 */
void
cse_write_string(stream_t *s, char code, const char *buf)
{
  if (buf)
    cse_write_packet(s, code, buf, strlen(buf));
}

int
cse_read_string(stream_t *s, char *buf, int length)
{
  int code;
  int l1, l2, l3;
  int read_length;

  length--;

  code = cse_read_byte(s);
  l1 = cse_read_byte(s) & 0xff;
  l2 = cse_read_byte(s) & 0xff;
  l3 = cse_read_byte(s);
  read_length = (l1 << 16) + (l2 << 8) + (l3 & 0xff);

  if (s->socket < 0 || l3 < 0)
    return -1;

  return cse_read_limit(s, buf, length, read_length);
}

/**
 * Decodes the first 3 characters of the session to see which
 * JVM owns it.
 */
static int
decode_session(char *session)
{
  int value = 0;
  int i;

  for (i = 2; i >= 0; i--) {
    int code = session[i];

    if (code >= 'a' && code <= 'z')
      value = 64 * value + code - 'a';
    else if (code >= 'A' && code <= 'Z')
      value = 64 * value + code - 'A' + 26;
    else if (code >= '0' && code <= '9')
      value = 64 * value + code - 'A' + 52;
    else if (code == '_')
      value = 64 * value + 62;
    else if (code == '/')
      value = 64 * value + 63;
    else
      return -1;
  }

  if (i > -1)
    return -1;
  else
    return value;
}

int
cse_session_from_string(char *source, char *cookie)
{
  char *match = strstr(source, cookie);
  
  if (match)
    return decode_session(match + strlen(cookie));

  return -1;
}

/**
 * Adds a new host to the configuration
 */
srun_t *
cse_add_host_int(config_t *config, const char *hostname,
		 int port, int is_backup, int is_default)
{
  struct hostent *hostent;
  srun_t *srun;

  if (config->srun_size == 1 &&
      (config->srun_list->is_default || ! config->srun_list->host))
    config->srun_size = 0;

  /* Resize if too many hosts. */
  if (config->srun_size >= config->srun_capacity) {
    int capacity = config->srun_capacity;
    srun_t *srun_list;

    if (capacity == 0)
      capacity = 4;

    srun_list = (srun_t *) cse_alloc(config, 2 * capacity * sizeof(srun_t));
    memset(srun_list, 0, 2 * capacity * sizeof(srun_t));
    if (config->srun_list)
      memcpy(srun_list, config->srun_list, capacity * sizeof(srun_t));
    config->srun_capacity = 2 * capacity;
    cse_free(config, config->srun_list);
    config->srun_list = srun_list;
  }

  hostent = gethostbyname(hostname);
  if (hostent && hostent->h_addr) {
    srun = &config->srun_list[config->srun_size];

    srun->hostname = cse_strdup(config, hostname);
    srun->host = (struct in_addr *) cse_alloc(config, sizeof (struct in_addr));
    memcpy(srun->host, hostent->h_addr, sizeof(struct in_addr));
    srun->port = port;
    srun->is_backup = is_backup;
    srun->n_sockets = 0;
    srun->max_sockets = 32;

    srun->is_default = is_default && config->srun_size == 0;

    srun->connect_timeout = CONNECT_TIMEOUT;
    srun->live_time = LIVE_TIME;
    srun->dead_time = DEAD_TIME;

    /*
     * XXX: for dynamic srun
     * srun->session = cse_query_session(config, srun, config->pool);
     */
    srun->session = config->srun_size;

    config->srun_size++;

    return srun;
  }
  else {
    cse_error(config, "CSE cannot find host %s\n", hostname);

    return 0;
  }
}

/**
 * Select a client JVM randomly.
 */

static int
random_host(char *ip, int requestTime, int rand)
{
  int host = requestTime * 23 + g_count++;
  host = 23 * host + rand;

  for (; *ip; ip++) {
    host = 23 * host + *ip;
  }

  if (host < 0)
    return -host;
  else
    return host;
}

/**
 * reuse the socket
 */
static void
cse_reuse(stream_t *s, config_t *config, srun_t *srun,
	  int request_time, void *pool)
{
  s->pool = pool;
  s->config = config;
  s->write_length = 0;
  s->read_length = 0;
  s->read_offset = 0;
  s->socket = srun->sockets[--srun->n_sockets];
  s->update_count = config->update_count;
  s->srun = srun;

  srun->is_dead = 0;
  srun->last_time = request_time;
  
  LOG(("reopen %d\n", s->socket));
}

/**
 * Try to recycle the socket so the next request can reuse it.
 */
void
cse_recycle(stream_t *s)
{
  int socket = s->socket;
  srun_t *srun = s->srun;

  cse_lock(s->config->lock);
  
  if (socket >= 0 && s->config->update_count == s->update_count &&
      srun && srun->n_sockets < srun->max_sockets) {
    s->socket = -1;
    cse_kill_socket_cleanup(socket, s->pool);
    srun->sockets[srun->n_sockets++] = socket;
    cse_unlock(s->config->lock);
    LOG(("recycle %d\n", socket));
  }
  else {
    cse_unlock(s->config->lock);
    LOG(("close2 %d\n", socket));
    cse_write_packet(s, CSE_CLOSE, 0, 0);

    cse_close(s, "recycle");
  }
}

/**
 * Try to reuse a socket
 */
static int
cse_reuse_socket(stream_t *s, config_t *config, srun_t *srun,
		 unsigned int request_time, void *pool)
{
  cse_lock(config->lock);

  if (request_time < srun->last_time)
    srun->last_time = request_time;

  if (srun->n_sockets > 0 &&
      srun->last_time + srun->live_time >= request_time) {
    cse_reuse(s, config, srun, request_time, pool);
    cse_unlock(config->lock);
    return 1;
  }
  else if (srun->n_sockets > 0) {
    while (srun->n_sockets > 0) {
      int socket = srun->sockets[--srun->n_sockets];

      send(socket, "X\0\0\0", 4, 0);
      closesocket(socket);
      LOG(("close reuse-timeout %d\n", socket));
    }
  }
  cse_unlock(config->lock);

  return 0;
}

void
cse_close_sockets(config_t *config)
{
  int i;
  
  for (i = 0; i < config->srun_size; i++) {
    srun_t *srun = config->srun_list + i;
    int j;

    for (j = 0; j < srun->n_sockets; j++) {
      int socket = srun->sockets[j];
      if (socket >= 0) {
        send(socket, "X\0\0\0", 4, 0);
        closesocket(socket);
      }
    }

    srun->n_sockets = 0;
  }

  config->srun_size = 0;
}

static int
open_connection(stream_t *s, config_t *config,
                int session_index, char *ip,
                unsigned int request_time, int rand,
                void *pool)
{
  int size;
  int i;
  int host;
  int incr;

  size = config->srun_size;
  if (size < 1)
    size = 1;

  if (session_index < 0) {
    host = random_host(ip, request_time, rand) % size;
  }
  else {
    for (host = 0; host < size; host++) {
      if (config->srun_list[host].session == session_index)
	break;
    }
    if (host == size) {
      host = random_host(ip, request_time, rand) % size;
    }
  }
  
  if (host < 0)
    host = -host;
  
  incr = 2 * (host & 1) - 1;

  /*
  // XXX: if the session belongs to a host, we should retry for a few seconds?
  */

  /* try to reuse first */
  if (session_index < 0) {
    for (i = 0; i < size; i++) {
      srun_t *srun = config->srun_list + (host + i * (incr + size)) % size;

      if (srun->is_backup) {
      }
      else if (cse_reuse_socket(s, config, srun, request_time, pool)) {
        return 1;
      }
    }
  }

  for (i = 0; i < size; i++) {
    srun_t *srun = config->srun_list + (host + i * (incr + size)) % size;

    if (session_index < 0 && srun->is_backup) {
    }
    else if (cse_reuse_socket(s, config, srun, request_time, pool)) {
      return 1;
    }
    else if (srun->is_dead &&
             srun->last_time + srun->dead_time >= request_time) {
    }
    else if (cse_open(s, config, srun, pool, 0)) {
      s->srun = srun;
      srun->is_dead = 0;
      srun->last_time = request_time;
      return 1;
    }
  }

  /* Okay, the primaries failed.  So try the secondaries. */
  for (i = 0; i < size; i++) {
    srun_t *srun = config->srun_list + (host + i * (incr + size)) % size;

    if (cse_reuse_socket(s, config, srun, request_time, pool)) {
      return 1;
    }
    else if (cse_open(s, config, srun, pool, 1)) {
      s->srun = srun;
      srun->is_dead = 0;
      srun->last_time = request_time;
      return 1;
    }
    else {
      srun->is_dead = 1;
      srun->last_time = request_time;
    }
  }
  
  return 0;
}

int
cse_open_connection(stream_t *s, config_t *config,
                    int session_index, char *ip,
                    unsigned int request_time, int rand,
                    void *pool)
{
  s->config = config;
  s->socket = -1;
  s->update_count = 0;
  s->pool = pool;
  s->write_length = 0;
  s->read_length = 0;
  s->read_offset = 0;
  s->srun = 0;
  
  if (open_connection(s, config, session_index,
                      ip, request_time, rand, pool)) {
    cse_set_socket_cleanup(s->socket, pool);
    return 1;
  }
  else
    return 0;
}
