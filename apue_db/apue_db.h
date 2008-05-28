#ifndef _APUE_DB_H
#define _APUE_DB_H

#ifdef __cplusplus
extern "C" {
#endif
	
typedef	void *	DBHANDLE;

DBHANDLE  db_open(const char *, int, ...);
void      db_close(DBHANDLE);
char     *db_fetch(DBHANDLE, const char *);
int       db_store(DBHANDLE, const char *, const char *, int);
int       db_delete(DBHANDLE, const char *);
void      db_rewind(DBHANDLE);
char     *db_nextrec(DBHANDLE, char *);

#if 0
int       db_is_key_locked(DBHANDLE h, const char *key); 
#endif
char     *db_start_write_update(DBHANDLE h, const char *key);
int       db_finish_write_update(DBHANDLE h, const char *key, const char *data);

char     *db_write_update_and_read_lock(DBHANDLE h, const char *key, const char *data);
void      db_read_unlock(DBHANDLE h, const char *key);


/*
 * Flags for db_store().
 */
#define DB_INSERT	   1	/* insert new record only */
#define DB_REPLACE	   2	/* replace existing record */
#define DB_STORE	   3	/* replace or insert */

/*
 * Implementation limits.
 */
#define IDXLEN_MIN	   6	/* key, sep, start, sep, length, \n */
#define IDXLEN_MAX	1024	/* arbitrary */
#define DATLEN_MIN	   2	/* data byte, newline */
#define DATLEN_MAX	1024	/* arbitrary */

#ifdef __cplusplus
}
#endif

#endif /* _APUE_DB_H */
