/* $Id$
 * Functions for database communication */

#ifndef  _DBMYSQL_H
#define  _DBMYSQL_H

#include "/usr/include/mysql/mysql.h"
#include "debug.h"
#include "imap4.h"
#include "mime.h"
#include "list.h"

/* size of the messageblk's */
#define READ_BLOCK_SIZE 524288		/* be carefull, MYSQL has a limit */

struct session;
struct list;

enum table_aliases /* prototype for aliases table */
{
	ALIASES_ALIAS_IDNR,
	ALIASES_ALIAS,
	ALIASES_USERIDNR
};

enum table_user /* prototype for user table */
{
	USER_USERIDNR,
	USER_USERID,
	USER_PASSWD,
	USER_CLIENTID,
	USER_MAXMAIL
};

enum table_mailbox /* prototype for mailbox table */
{
	MAILBOX_MAILBOXIDNR,
	MAILBOX_OWNERIDNR,
	MAILBOX_NAME,
	MAILBOX_SEEN_FLAG,
	MAILBOX_ANSWERED_FLAG,
	MAILBOX_DELETED_FLAG,
	MAILBOX_FLAGGED_FLAG,
	MAILBOX_RECENT_FLAG,
	MAILBOX_DRAFT_FLAG,
	MAILBOX_NO_INFERIORS,
	MAILBOX_NO_SELECT,
	MAILBOX_PERMISSIONS
};
	
enum table_message /* prototype for message table */
{
	MESSAGE_MESSAGEIDNR,
	MESSAGE_MAILBOXIDNR,
	MESSAGE_MESSAGESIZE,
	MESSAGE_SEEN_FLAG,
	MESSAGE_ANSWERED_FLAG,
	MESSAGE_DELETED_FLAG,
	MESSAGE_FLAGGED_FLAG,
	MESSAGE_RECENT_FLAG,
	MESSAGE_DRAFT_FLAG,
	MESSAGE_UNIQUE_ID,
	MESSAGE_STATUS
};

enum table_messageblk /* prototype for messageblk table */
{
	MESSAGEBLK_MESSAGEBLKNR,
	MESSAGEBLK_MESSAGEIDNR,
	MESSAGEBLK_MESSAGEBLK,
	MESSAGEBLK_BLOCKSIZE
};


typedef struct
{
  int inited;
  int nblocks,currblock,blockpos;
  MYSQL_RES *result;
  MYSQL_ROW block;
} msgfetch_info_t;


typedef struct 
{
  unsigned long block,pos;
} db_pos_t;


typedef struct 
{
  struct list mimeheader;           /* the MIME header of this part (if present) */
  struct list rfcheader;            /* RFC822 header of this part (if present) */
  db_pos_t bodystart,bodyend;       /* the body of this part */
  unsigned long bodysize;
  unsigned long bodylines;
  db_pos_t headerstart,headerend;   /* the header of this part */
  unsigned long headersize;
  struct list children;             /* the children (multipart msg) */
} mime_message_t;

  

/* 
 * PROTOTYPES 
 */

int db_connect();
int db_query (char *query);
int db_check_user (char *username, struct list *userids);
unsigned long db_get_inboxid (unsigned long *useridnr);
unsigned long db_insert_message (unsigned long *useridnr);
unsigned long db_update_message (unsigned long *messageidnr, char *unique_id,
				 unsigned long messagesize);
unsigned long db_insert_message_block (char *block, int nextblock);
int db_check_id (char *id);
int db_disconnect();
unsigned long db_insert_result ();
int db_send_message_lines (void *fstream, unsigned long messageidnr, long lines);
int db_send_message_special (void *fstream, unsigned long messageidnr, long lines, char *firstblock);
unsigned long db_validate (char *user, char *password);
unsigned long db_md5_validate (char *username,unsigned char *md5_apop_he, char *apop_stamp);
int db_createsession (unsigned long useridnr, struct session *sessionptr);
int db_update_pop (struct session *sessionptr);
unsigned long db_set_deleted ();
unsigned long db_deleted_purge();

/* mailbox functionality */
unsigned long db_findmailbox(const char *name, unsigned long useridnr);
int db_getmailbox(mailbox_t *mb, unsigned long userid);
int db_createmailbox(const char *name, unsigned long ownerid);
int db_listmailboxchildren(unsigned long uid, unsigned long **children, int *nchildren,
			   const char *filter);
int db_removemailbox(unsigned long uid, unsigned long ownerid);
int db_isselectable(unsigned long uid);
int db_noinferiors(unsigned long uid);
int db_setselectable(unsigned long uid, int value);
int db_removemsg(unsigned long uid);
int db_movemsg(unsigned long to, unsigned long from);
int db_copymsg(unsigned long msgid, unsigned long destmboxid);
int db_getmailboxname(unsigned long uid, char *name);
int db_setmailboxname(unsigned long uid, const char *name);
int db_expunge(unsigned long uid,unsigned long **msgids,int *nmsgs);
int db_build_msn_list(mailbox_t *mb);
unsigned long db_first_unseen(unsigned long uid);

/* message functionality */
int db_get_msgflag(const char *name, unsigned long mailboxuid, unsigned long msguid);
int db_set_msgflag(const char *name, unsigned long mailboxuid, unsigned long msguid, int val);
int db_get_msgdate(unsigned long mailboxuid, unsigned long msguid, char *date);

int db_init_msgfetch(unsigned long uid);
int db_update_msgbuf(int minlen);
void db_close_msgfetch();
void db_give_msgpos(db_pos_t *pos);
unsigned long db_give_range_size(db_pos_t *start, db_pos_t *end);

void db_free_msg(mime_message_t *msg);
void db_reverse_msg(mime_message_t *msg);

int db_fetch_headers(unsigned long msguid, mime_message_t *msg);
int db_add_mime_children(struct list *brothers, char *splitbound);
int db_start_msg(mime_message_t *msg, char *stopbound);

int db_dump_range(FILE *outstream,db_pos_t start, db_pos_t end, unsigned long msguid,
		  int offset, int cnt);
int db_msgdump(mime_message_t *msg, unsigned long msguid);

#endif
