/*#include "apue.h"*/
#include "apue_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
 
void	err_quit(const char *, ...);
void	err_sys(const char *, ...);

/*
 * Default file access permissions for new files.
 */
#define FILE_MODE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* Write to the DB */
void test1() {
	DBHANDLE	db;

	if ((db = db_open("db1", O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) == NULL)
		err_sys("db_open error");

	if (db_store(db, "AlphaAlpha", "data1", DB_INSERT) != 0)
		err_quit("db_store error for alpha");
	if (db_store(db, "betabetabetabetabetabeta", "Data for beta", DB_INSERT) != 0)
		err_quit("db_store error for beta");
	if (db_store(db, "gamma", "record3", DB_INSERT) != 0)
		err_quit("db_store error for gamma");

	db_close(db);
}

/* delete everything in the DB */
void test2() {
	DBHANDLE	db;

	if ((db = db_open("db1", O_RDWR, FILE_MODE)) == NULL)
		err_sys("db_open error");

	db_rewind(db);
	char key[1024];
	char *data;
	while ((data = db_nextrec(db, &key[0])) != 0) {
		fprintf(stderr, "deleting %s ... ", key);
		db_delete(db, key);
		fprintf(stderr, "record deleted\n");
		db_rewind(db);
	}

	db_close(db);
}

void test3() {
	DBHANDLE	db;
	char *data;
	char new_data[32];
	
	if ((db = db_open("db2", O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) == NULL)
		err_sys("db_open error");

	if (db_store(db, "AlphaAlpha", "data1", DB_INSERT) != 0)
		err_quit("db_store error for alpha");
	if (db_store(db, "betabetabetabetabetabeta", "Data for beta", DB_INSERT) != 0)
		err_quit("db_store error for beta");
	if (db_store(db, "gamma", "record3", DB_INSERT) != 0)
		err_quit("db_store error for gamma");

	/* test the write - update - unlock functions */
#define key1 "betabetabetabetabetabeta"
	data = db_start_write_update(db, key1);
	fprintf(stderr, "betabetabetabetabetabeta: %s\n", data);
	
	strncpy(new_data, "new_beta_data", 31);
	if (db_finish_write_update(db, key1, new_data) != 0)
		err_quit("db_finish_write_update failed");
	
	fprintf(stderr, "betabetabetabetabetabeta (fetch): %s\n",
			db_fetch(db, key1));
	
	/* test the write - update - read lock - unlock functions */
#define key2 "AlphaAlpha"
	data = db_start_write_update(db, key2);
	fprintf(stderr, "AlphaAlpha: %s\n", data);
	
	strncpy(new_data, "data9", 31);
	if (db_write_update_and_read_lock(db, key2, new_data) == 0)
		err_quit("db_write_update_and_read_lock failed");
	
	db_read_unlock(db, key2);
	
	fprintf(stderr, "AlphaAlpha (fetch): %s\n", db_fetch(db, key2));

	db_close(db);
}

int main(void) {
	test1();
	test2();
	test3();
	exit(0);
}
