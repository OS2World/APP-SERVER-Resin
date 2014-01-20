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

#ifndef CSE_H
#define CSE_H

typedef struct registry_t {
  struct registry_t *parent;
  struct registry_t *next;
  struct registry_t *prev;
  struct registry_t *first;
  struct registry_t *last;

  char *key;
  char *value;
} registry_t;

typedef struct location_t {
  struct location_t *next;
  char *host;
  int port;
  char *prefix;
  char *suffix;
  int is_exact;
} location_t;

typedef struct srun_t {
  char *hostname;
  struct in_addr *host;
  int port;

  int is_backup;
  int session;

  int is_dead;
  unsigned int last_time;

  void *mutex;
  int sockets[1024];
  int max_sockets;
  int n_sockets;
  int is_default;

  int connect_timeout;
  int live_time;
  int dead_time;
} srun_t;

typedef struct config_t {
  void *server;
  void *pool;
  void *lock;
  char *error;
  int disable_caucho_status;

  char *path;
  char *resin_home;
  char *error_page;
  registry_t *registry;

  location_t *locations;

  struct srun_t *srun_list;
  int srun_capacity;
  int srun_size;

  int last_modified;
  int last_update;
  int update_count;
} config_t;

#define BUF_LENGTH 8192

typedef struct stream_t {
  struct srun_t *srun;
  void *pool;
  int update_count;
  
  int socket;
  struct config_t *config;

  unsigned char read_buf[BUF_LENGTH + 1];
  int read_offset;
  int read_length;

  unsigned char write_buf[BUF_LENGTH + 1];
  int write_length;
} stream_t;

#define CSE_NULL            '?'
#define CSE_PATH_INFO       'b'
#define CSE_PROTOCOL        'c'
#define CSE_METHOD          'd'
#define CSE_QUERY_STRING    'e'
#define CSE_SERVER_NAME     'f'
#define CSE_SERVER_PORT     'g'
#define CSE_REMOTE_HOST     'h'
#define CSE_REMOTE_ADDR     'i'
#define CSE_REMOTE_PORT     'j'
#define CSE_REAL_PATH       'k'
#define CSE_REMOTE_USER     'm'
#define CSE_AUTH_TYPE       'n'
#define CSE_URI             'o'
#define CSE_CONTENT_LENGTH  'p'
#define CSE_CONTENT_TYPE    'q'
#define CSE_IS_SECURE       'r'
#define CSE_SESSION_GROUP   's'
#define CSE_CLIENT_CERT     't'
#define CSE_SERVER_TYPE		'u'

#define CSE_HEADER          'H'
#define CSE_VALUE           'V'

#define CSE_STATUS          'S'
#define CSE_SEND_HEADER     'G'

#define CSE_PING            'P'
#define CSE_QUERY           'Q'

#define CSE_ACK             'A'
#define CSE_DATA            'D'
#define CSE_FLUSH           'F'
#define CSE_KEEPALIVE       'K'
#define CSE_END             'Z'
#define CSE_CLOSE           'X'

#ifdef DEBUG
#define LOG(x) cse_log x
#else
#define LOG(x)
#endif

#ifndef WIN32
#define closesocket(x) close(x)
#endif

char *cse_alloc(config_t *config, int size);
char *cse_strdup(config_t *config, const char *string);
void cse_free(config_t *config, void *value);
void cse_error(config_t *config, char *format, ...);

registry_t *cse_parse(FILE *is, config_t *config, char *path);

registry_t *cse_next_link(registry_t *reg, char *key);
char *cse_find_value(registry_t *reg, char *key);
void cse_print_registry(registry_t *registry);

void cse_init_config(config_t *config);
void cse_update_config(config_t *config, int now);

int cse_match_request(config_t *config, const char *host, int port,
                      const char *url);

srun_t *
cse_add_host_int(config_t *config, const char *hostname,
		 int port, int is_backup, int is_default);
srun_t *cse_add_host(config_t *config, const char *host, int port);
srun_t *cse_add_backup(config_t *config, const char *host, int port);

void cse_log(char *fmt, ...);

int cse_session_from_string(char *source, char *cookie);

int cse_open(stream_t *s, config_t *config, srun_t *srun, void *p, int wait);
void cse_close(stream_t *s, char *msg);
void cse_close_stream(stream_t *s);
void cse_close_sockets(config_t *config);
void cse_recycle(stream_t *s);
int cse_flush(stream_t *s);
int cse_fill_buffer(stream_t *s);
int cse_read_byte(stream_t *s);
void cse_write(stream_t *s, const char *buf, int length);
int cse_read_all(stream_t *s, char *buf, int len);
int cse_skip(stream_t *s, int length);
int cse_read_limit(stream_t *s, char *buf, int buflen, int readlen);

void cse_write_packet(stream_t *s, char code, const char *buf, int length);
void cse_write_string(stream_t *s, char code, const char *buf);
int cse_read_string(stream_t *s, char *buf, int length);

void cse_kill_socket_cleanup(int socket, void *pool);
void cse_set_socket_cleanup(int socket, void *pool);
int
cse_open_connection(stream_t *s, config_t *config,
		    int session_index, char *ip,
		    unsigned int request_time, int rand,
		    void *pool);

void *cse_create_lock();
int cse_lock(void *lock);
void cse_unlock(void *lock);

#endif /* CSE_H */

