/* $Id$ */

#ifndef MIME_H_
#define MIME_H_
#include "dbmysql.h"
#include "debug.h"
#include "list.h"

#define MIME_FIELD_MAX 128
#define MIME_VALUE_MAX 4096
#define MEM_BLOCK 1024

struct mime_record
{
  char field[MIME_FIELD_MAX];
  char value[MIME_VALUE_MAX];
};


int mime_list(char *blkdata, struct list *mimelist);
void mime_findfield(const char *fname, struct list *mimelist, struct mime_record **mr);
int mail_adr_list(char *scan_for_field, struct list *targetlist, struct list *mimelist,
		  struct list *users, char *header, unsigned long headersize);
int mail_adr_list_special(int offset, int max, char *address_array[], struct list *users);
int mime_readheader(char *blkdata, unsigned long *blkidx, struct list *mimelist);

#endif
