
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

//#define old
#define DODS_DEBUG

// TODO: Remove unneeded includes.

#include <pthread.h>
#include <limits.h>
#include <unistd.h>   // for stat
#include <fcntl.h>
#include <sys/types.h>  // for stat and mkdir
#include <sys/stat.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <set>

#include "Error.h"
#include "InternalErr.h"
#include "ResponseTooBigErr.h"
#ifndef WIN32
#include "SignalHandler.h"
#endif
#include "HTTPCacheInterruptHandler.h"
#include "HTTPCacheTable.h"

#include "util_mit.h"
#include "util.h"
#include "debug.h"
#include "apue_db.h"

#ifdef WIN32
#include <direct.h>
#include <time.h>
#include <fcntl.h>
#define MKDIR(a,b) _mkdir((a))
#define REMOVE(a) remove((a))
#define MKSTEMP(a) _open(_mktemp((a)),_O_CREAT,_S_IREAD|_S_IWRITE)
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_STR "\\"
#else
#define MKDIR(a,b) mkdir((a), (b))
#define REMOVE(a) remove((a))
#define MKSTEMP(a) mkstemp((a))
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR "/"
#endif

#define CACHE_META ".meta"
#define CACHE_INDEX ".index"
#define CACHE_DB "db"
#define CACHE_EMPTY_ETAG "@cache@"

#define NO_LM_EXPIRATION 24*3600 // 24 hours
#define MAX_LM_EXPIRATION 48*3600 // Max expiration from LM

// If using LM to find the expiration then take 10% and no more than
// MAX_LM_EXPIRATION.
#ifndef LM_EXPIRATION
#define LM_EXPIRATION(t) (min((MAX_LM_EXPIRATION), static_cast<int>((t) / 10)))
#endif

const int CACHE_TABLE_SIZE = 1499;

using namespace std;

namespace libdap {

#if 0
/** Compute the hash value for a URL.
    @param url
    @return An integer hash code between 0 and CACHE_TABLE_SIZE. */

int
get_hash(const string &url)
{
    int hash = 0;

    for (const char *ptr = url.c_str(); *ptr; ptr++)
        hash = (int)((hash * 3 + (*(unsigned char *)ptr)) % CACHE_TABLE_SIZE);

    return hash;
}
#endif

#define FILE_MODE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

HTTPCacheTable::HTTPCacheTable(const string &cache_root, int block_size) :
	d_size_fd(-1),
	d_cache_root(cache_root),
	d_block_size(block_size),
#if 0
	d_current_size(0),
#endif
	d_new_entries(0)
{
#if 0
	d_cache_index = cache_root + CACHE_INDEX;
#endif
	
    d_cache_db = cache_root + CACHE_DB;
    d_db = db_open(d_cache_db.c_str(), O_RDWR | O_CREAT, FILE_MODE);
	if (d_db == NULL)
		throw Error("Could not open/create the cache database.");

	string size_file = d_cache_db + ".size";
	d_size_fd = open(size_file.c_str(), O_RDWR | O_CREAT, FILE_MODE);
	if (d_size_fd < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not get size file");
	
	if (writew_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not lock size file");
	
	struct stat buf;
	if (fstat(d_size_fd, &buf) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not stat size file");
	if (buf.st_size == 0) {
		unsigned long sz = 0;
		if (write(d_size_fd, &sz, sizeof(unsigned long)) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not init size ");
	}

	if (un_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not unlock size file");

#ifdef old	
	d_cache_table = new CacheEntries*[CACHE_TABLE_SIZE];
	
	// Initialize the cache table.
    for (int i = 0; i < CACHE_TABLE_SIZE; ++i)
        d_cache_table[i] = 0;
    
    cache_index_read();
#endif
}

/** Called by for_each inside ~HTTPCache().
    @param e The cache entry to delete. */

static inline void
delete_cache_entry(HTTPCacheTable::CacheEntry *e)
{
#ifdef old
    DBG2(cerr << "Deleting CacheEntry: " << e << endl);
    DESTROY(&e->get_lock());
    delete e;
#endif
}

HTTPCacheTable::~HTTPCacheTable() {
	db_close(d_db);
	close(d_size_fd);
	
#ifdef old
	for (int i = 0; i < CACHE_TABLE_SIZE; ++i) {
		HTTPCacheTable::CacheEntries *cp = get_cache_table()[i];
		if (cp) {
			// delete each entry
			for_each(cp->begin(), cp->end(), delete_cache_entry);
			
			// now delete the vector that held the entries
			delete get_cache_table()[i];
			get_cache_table()[i] = 0;
		}
	}
	
	delete[] d_cache_table;
#endif
}

#if 0
/** Functor which deletes and nulls a single CacheEntry if it has expired.
    This functor is called by expired_gc which then uses the
    erase(remove(...) ...) idiom to really remove all the vector entries that
    belonged to the deleted CacheEntry objects.

    @see expired_gc. */

class DeleteExpired : public unary_function<HTTPCacheTable::CacheEntry *&, void> {
	time_t d_time;
	HTTPCacheTable &d_table;

public:
	DeleteExpired(HTTPCacheTable &table, time_t t) :
		d_time(t), d_table(table) {
		if (!t)
			d_time = time(0); // 0 == now
	} 

	void operator()(HTTPCacheTable::CacheEntry *&e) {
		if (e && !e->readers && (e->freshness_lifetime
				< (e->corrected_initial_age + (d_time - e->response_time)))) {
			DBG(cerr << "Deleting expired cache entry: " << e->url << endl);
			d_table.remove_cache_entry(e);
			delete e; e = 0;
		}
	}
};
#endif
// @param t base deletes againt this time, defaults to 0 (now)
void HTTPCacheTable::delete_expired_entries(time_t t) {
	// Find the expired entries
	db_rewind(d_db);
	
	vector<CacheEntry*> expired;
	char key[1024];
	char *data;
	HTTPCacheTable::CacheEntry *e = 0;
	while ((data = db_nextrec(d_db, &key[0])) != 0) {
		if (data) {
			e = cache_index_parse_line(data);
			if (!e->readers && (e->freshness_lifetime 
								< (e->corrected_initial_age 
								   + (max(t,time(0)) - e->response_time)))) {
				expired.push_back(e);
			}
			else {
				delete e; e = 0;
			}
		}
	}
	
	db_rewind(d_db);
	vector<CacheEntry*>::iterator i;
	for (i = expired.begin(); i != expired.end(); ++i) {
		// delete each entry that's still in the database and then remove the
		// associated files (on unix if other processes have access to those
		// then the files will remain until the last descriptor is closed.
		db_delete(d_db, (*i)->url.c_str());
		remove_cache_entry(*i);
		delete *i;
	}
	
#if 0	
	// Walk through and delete all the expired entries.
	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
		HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
		if (slot) {
			for_each(slot->begin(), slot->end(), DeleteExpired(*this, time));
			slot->erase(remove(slot->begin(), slot->end(),
					static_cast<HTTPCacheTable::CacheEntry *>(0)), slot->end());
		}
	}
#endif
}

/** Functor which deletes and nulls a single CacheEntry which has less than
    or equal to \c hits hits or if it is larger than the cache's
    max_entry_size property.

    @see hits_gc. */

class DeleteByHits : public unary_function<HTTPCacheTable::CacheEntry *&, void> {
	HTTPCacheTable &d_table;
	int d_hits;

public:
	DeleteByHits(HTTPCacheTable &table, int hits) :
		d_table(table), d_hits(hits) {
	}

	void operator()(HTTPCacheTable::CacheEntry *&e) {
		if (e && !e->readers && e->hits <= d_hits) {
			DBG(cerr << "Deleting cache entry: " << e->url << endl);
			d_table.remove_cache_entry(e);
			delete e; e = 0;
		}
	}
};

void 
HTTPCacheTable::delete_by_hits(int hits) {
#if 0
	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
        if (get_cache_table()[cnt]) {
            HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
            for_each(slot->begin(), slot->end(), DeleteByHits(*this, hits));
            slot->erase(remove(slot->begin(), slot->end(),
                               static_cast<HTTPCacheTable::CacheEntry*>(0)),
                        slot->end());

        }
    }
#endif
}

/** Functor which deletes and nulls a single CacheEntry which is larger than 
    a given size.
    @see hits_gc. */

class DeleteBySize : public unary_function<HTTPCacheTable::CacheEntry *&, void> {
	HTTPCacheTable &d_table;
	unsigned int d_size;

public:
	DeleteBySize(HTTPCacheTable &table, unsigned int size) :
		d_table(table), d_size(size) {
	}

	void operator()(HTTPCacheTable::CacheEntry *&e) {
		if (e && !e->readers && e->size > d_size) {
			DBG(cerr << "Deleting cache entry: " << e->url << endl);
			d_table.remove_cache_entry(e);
			delete e; e = 0;
		}
	}
};

void HTTPCacheTable::delete_by_size(unsigned int size) {
#if 0
	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
        if (get_cache_table()[cnt]) {
            HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
            for_each(slot->begin(), slot->end(), DeleteBySize(*this, size));
            slot->erase(remove(slot->begin(), slot->end(),
                               static_cast<HTTPCacheTable::CacheEntry*>(0)),
                        slot->end());

        }
    }
#endif
}

/** @name Cache Index

    These methods manage the cache's index file. Each cache holds an index
    file named \c .index which stores the cache's state information. */

//@{

/** Remove the cache index file.

    A private method.

    @return True if the file was deleted, otherwise false. */

bool
HTTPCacheTable::cache_index_delete()
{
	return (REMOVE(d_cache_db.c_str()) == 0);
#if 0
	db_rewind(d_db);
	char key[1024];
	char *data;
	while ((data = db_nextrec(d_db, &key[0])) != 0) {
		DBG(cerr << "deleting " << key << " from the db index" << endl);
		db_delete(d_db, key);
		db_rewind(d_db);
	}
#endif		
#ifdef old
	d_new_entries = 0;
	
    return (REMOVE(d_cache_index.c_str()) == 0);
#endif
}

/** Read the saved set of cached entries from disk. Consistency between the
    in-memory cache and the index is maintained by only reading the index
    file when the HTTPCache object is created!

    A private method.

    @return True when a cache index was found and read, false otherwise. */

bool
HTTPCacheTable::cache_index_read()
{
	return true;
#ifdef old
    FILE *fp = fopen(d_cache_index.c_str(), "r");
    // If the cache index can't be opened that's OK; start with an empty
    // cache. 09/05/02 jhrg
    if (!fp) {
        return false;
    }

    char line[1024];
    while (!feof(fp) && fgets(line, 1024, fp)) {
    	add_entry_to_cache_table(cache_index_parse_line(line));
        DBG2(cerr << line << endl);
    }

    int res = fclose(fp) ;
    if (res) {
        DBG(cerr << "HTTPCache::cache_index_read - Failed to close " << (void *)fp << endl);
    }

    d_new_entries = 0;
    
    return true;
#endif
}

/** Parse one record from the database. The result is returned in a newly 
    allocated CacheEntry object.

    A private method.

    @param line A single line from the \c .index file.
    @return A CacheEntry initialized with the information from \c line. */

HTTPCacheTable::CacheEntry *
HTTPCacheTable::cache_index_parse_line(const char *line)
{
    // Read the line and create the cache object
	HTTPCacheTable::CacheEntry *entry = new HTTPCacheTable::CacheEntry;
#if 0
    INIT(&entry->d_lock);
#endif
    istringstream iss(line);
    iss >> entry->url;
    iss >> entry->cachename;

    iss >> entry->etag;
    if (entry->etag == CACHE_EMPTY_ETAG)
        entry->etag = "";

    iss >> entry->lm;
    iss >> entry->expires;
    iss >> entry->size;
    iss >> entry->range; // range is not used. 10/02/02 jhrg

#if 0
    iss >> entry->hash;
#endif
    iss >> entry->hits;
    iss >> entry->freshness_lifetime;
    iss >> entry->response_time;
    iss >> entry->corrected_initial_age;

    iss >> entry->must_revalidate;

    return entry;
}

#ifdef old
/** Functor which writes a single CacheEntry to the \c .index file. */

class WriteOneCacheEntryToFILE :
	public unary_function<HTTPCacheTable::CacheEntry *, void>
{

    FILE *d_fp;

public:
    WriteOneCacheEntryToFILE(FILE *fp) : d_fp(fp)
    {}

    void operator()(HTTPCacheTable::CacheEntry *e)
    {
        if (e && fprintf(d_fp,
                         "%s %s %s %ld %ld %ld %c %d %d %ld %ld %ld %c\r\n",
                         e->url.c_str(),
                         e->cachename.c_str(),
                         e->etag == "" ? CACHE_EMPTY_ETAG : e->etag.c_str(),
                         (long)(e->lm),
                         (long)(e->expires),
                         e->size,
                         e->range ? '1' : '0', // not used. 10/02/02 jhrg
                         e->hash,
                         e->hits,
                         (long)(e->freshness_lifetime),
                         (long)(e->response_time),
                         (long)(e->corrected_initial_age),
                         e->must_revalidate ? '1' : '0') < 0)
            throw Error("Cache Index. Error writing cache index\n");
    }
};
#endif
/** Functor which writes a single CacheEntry to the \c .index file. */
#if 0
class WriteOneCacheEntry :
	public unary_function<HTTPCacheTable::CacheEntry *, void>
{

    DBHANDLE d_db;

public:
    WriteOneCacheEntry(DBHANDLE db) : d_db(db)
    {}

    void operator()(HTTPCacheTable::CacheEntry *e)
    {
    	if (!e)
            throw Error("Cache Index. Error writing cache index\n");
    	string data;
    	data = e->url;
    	data.append(" ");
    	data.append(e->cachename);
    	data.append(" ");
    	data.append(e->etag == "" ? CACHE_EMPTY_ETAG : e->etag);
    	data.append(" ");
    	data.append(long_to_string(e->lm));
    	data.append(" ");
    	data.append(long_to_string(e->expires));
    	data.append(" ");
    	data.append(long_to_string(e->size));
    	data.append(" ");
    	data.append(e->range ? "1" : "0");
    	data.append(" ");
    	data.append(long_to_string(e->hash));
    	data.append(" ");
    	data.append(long_to_string(e->hits));
    	data.append(" ");
    	data.append(long_to_string(e->freshness_lifetime));
    	data.append(" ");
    	data.append(long_to_string(e->response_time));
    	data.append(" ");
    	data.append(long_to_string(e->corrected_initial_age));
    	data.append(" ");
    	data.append(e->must_revalidate ? "1" : "0");
    	
    	int status = db_store(d_db, e->url.c_str(), data.c_str(),DB_INSERT);
    	if (status != 0)
             throw Error("Cache Index. Error writing cache index\n");
    }
};
#endif

/** Walk through the list of cached objects and write the cache index file to
    disk. If the file does not exist, it is created. If the file does exist,
    it is overwritten. As a side effect, zero the new_entries counter.

    A private method.

    @exception Error Thrown if the index file cannot be opened for writing.
    @note The HTTPCache destructor calls this method and silently ignores
    this exception. */
void
HTTPCacheTable::cache_index_write()
{
#ifdef old
    DBG(cerr << "Cache Index. Writing index " << d_cache_index << endl);

    // Open the file for writing.
    FILE * fp = NULL;
    if ((fp = fopen(d_cache_index.c_str(), "wb")) == NULL) {
        throw Error(string("Cache Index. Can't open `") + d_cache_index
                    + string("' for writing"));
    }

    // Walk through the list and write it out. The format is really
    // simple as we keep it all in ASCII.

    for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
        HTTPCacheTable::CacheEntries *cp = get_cache_table()[cnt];
        if (cp)
            for_each(cp->begin(), cp->end(), WriteOneCacheEntryToFILE(fp));
    }

    /* Done writing */
    int res = fclose(fp);
    if (res) {
        DBG(cerr << "HTTPCache::cache_index_write - Failed to close "
            << (void *)fp << endl);
    }

    d_new_entries = 0;
#endif
}

//@} End of the cache index methods.

/** Make/return the directory path for cache files. The cache stores all responses
    in one directory, 'responses,' within the cache root.
 
	@return The pathname to the directory (even if it already existed).
    @exception Error Thrown if the directory cannot be created.*/

string
HTTPCacheTable::get_hash_directory()
{
    string p = d_cache_root + "responses";
    struct stat stat_info;
    
    if (stat(p.c_str(), &stat_info) == -1) {
        DBG2(cerr << "Cache....... Create dir " << p << endl);
        if (MKDIR(p.c_str(), 0777) < 0) {
            DBG2(cerr << "Cache....... Can't create..." << endl);
            throw Error("Could not create cache slot to hold response! Check the write permissions on your disk cache directory. Cache root: " + d_cache_root + ".");
        }
    }
    else {
        DBG2(cerr << "Cache....... Directory " << p << " already exists"
             << endl);
    }

    return p;
    
#ifdef old
    struct stat stat_info;
    ostringstream path;

    path << d_cache_root << hash;
    string p = path.str();

    if (stat(p.c_str(), &stat_info) == -1) {
        DBG2(cerr << "Cache....... Create dir " << p << endl);
        if (MKDIR(p.c_str(), 0777) < 0) {
            DBG2(cerr << "Cache....... Can't create..." << endl);
            throw Error("Could not create cache slot to hold response! Check the write permissions on your disk cache directory. Cache root: " + d_cache_root + ".");
        }
    }
    else {
        DBG2(cerr << "Cache....... Directory " << p << " already exists"
             << endl);
    }

    return p;
#endif
}

/** Make the files which will store the response body and headers. Set the
    name of these files to the \e entry cachename field. This method uses
    mkstemp to make the new filename.

    @note mkstemp opens the file it creates, which is a good thing but it makes
    tracking resources hard for the HTTPCache object (because an exception
    might cause a file descriptor resource leak). So I close that file
    descriptor here.

    A private method.

    @param entry The cache entry object to operate on.
    @exception Error If the file for the response's body cannot be created. */

void
HTTPCacheTable::create_location(HTTPCacheTable::CacheEntry *entry)
{
    string cache_dir = get_hash_directory();
#ifdef WIN32
    cache_dir += "\\dodsXXXXXX";
#else
    cache_dir += "/dodsXXXXXX"; // mkstemp uses six characters.
#endif

    // mkstemp uses the storage passed to it; must be writable and local.
    char *templat = new char[cache_dir.size() + 1];
    strcpy(templat, cache_dir.c_str());

    // Open truncated for update. NB: mkstemp() returns a file descriptor.
    // man mkstemp says "... The file is opened with the O_EXCL flag,
    // guaranteeing that when mkstemp returns successfully we are the only
    // user." 09/19/02 jhrg
    int fd = MKSTEMP(templat); // fd mode is 666 or 600 (Unix)
    if (fd < 0) {
        delete[] templat; templat = 0;
        close(fd);
        throw Error("The HTTP Cache could not create a file to hold the response; it will not be cached.");
    }

    entry->cachename = templat;
    delete[] templat; templat = 0;
    close(fd);
}


/** compute real disk space for an entry. */
static inline int
entry_disk_space(int size, unsigned int block_size)
{
    unsigned int num_of_blocks = (size + block_size) / block_size;
    
    DBG(cerr << "size: " << size << ", block_size: " << block_size
        << ", num_of_blocks: " << num_of_blocks << endl);

    return num_of_blocks * block_size;
}

/** @name Methods to manipulate instances of CacheEntry. */

//@{

/** Add a new CacheEntry to the cache table.
 	
 	@todo Add replace_cache_entry() method for updates like changes to the 
 	locking information
 	@param entry The CacheEntry instance to add. */
void
HTTPCacheTable::add_new_entry_to_cache_table(CacheEntry *entry)
{
	string data;
	data = entry->url;
	data.append(" ");
	data.append(entry->cachename);
	data.append(" ");
	data.append(entry->etag == "" ? CACHE_EMPTY_ETAG : entry->etag);
	data.append(" ");
	data.append(long_to_string(entry->lm));
	data.append(" ");
	data.append(long_to_string(entry->expires));
	data.append(" ");
	data.append(long_to_string(entry->size));
	data.append(" ");
	data.append(entry->range ? "1" : "0");
	data.append(" ");
#if 0
	data.append(long_to_string(entry->hash));
	data.append(" ");
#endif
	data.append(long_to_string(entry->hits));
	data.append(" ");
	data.append(long_to_string(entry->freshness_lifetime));
	data.append(" ");
	data.append(long_to_string(entry->response_time));
	data.append(" ");
	data.append(long_to_string(entry->corrected_initial_age));
	data.append(" ");
	data.append(entry->must_revalidate ? "1" : "0");
	
	int status = db_store(d_db, entry->url.c_str(), data.c_str(), DB_INSERT);
	if (status != 0)
         throw Error("Cache Index. Error writing cache index\n");
	
	update_current_size(entry->size);
	
#if 0
	WriteOneCacheEntry woce(d_db);
	woce(entry);
#endif	
	// print the entry to a parseable string
	// store it in the db
#ifdef old
    int hash = entry->hash;

    if (!d_cache_table[hash])
        d_cache_table[hash] = new CacheEntries;

    d_cache_table[hash]->push_back(entry);
    
    DBG(cerr << "add_entry_to_cache_table, current_size: " << d_current_size
        << ", entry->size: " << entry->size << ", block size: " << d_block_size 
        << endl);
    
    d_current_size += entry_disk_space(entry->size, d_block_size);

    DBG(cerr << "add_entry_to_cache_table, current_size: " << d_current_size << endl);
    
    increment_new_entries();
#endif
}

/** Get a pointer to a CacheEntry from the cache table.

	@todo Add read locking information
    @param url Look for this URL. */
HTTPCacheTable::CacheEntry *
HTTPCacheTable::get_locked_entry_from_cache_table(const string &url) /*const*/
{
	db_rewind(d_db);
	char *data = db_fetch(d_db, url.c_str());
	HTTPCacheTable::CacheEntry *e = 0;	
	if (data)
		e = cache_index_parse_line(data);
	
	return e;
	
#ifdef old
    return get_locked_entry_from_cache_table(get_hash(url), url);
#endif
}

#if 0
/** Get a pointer to a CacheEntry from the cache table. Providing a way to
    pass the hash code into this method makes it easier to test for correct
    behavior when two entries collide. 10/07/02 jhrg

    @param hash The hash code for \c url.
    @param url Look for this URL.
    @return The matching CacheEntry instance or NULL if none was found. */
HTTPCacheTable::CacheEntry *
HTTPCacheTable::get_locked_entry_from_cache_table(int hash, const string &url) /*const*/
{
	throw InternalErr(__FILE__, __LINE__, "old");
#ifdef old
	DBG(cerr << "url: " << url << "; hash: " << hash << endl);
	DBG(cerr << "d_cache_table: " << hex << d_cache_table << dec << endl);
    if (d_cache_table[hash]) {
        CacheEntries *cp = d_cache_table[hash];
        for (CacheEntriesIter i = cp->begin(); i != cp->end(); ++i) {
            // Must test *i because perform_garbage_collection may have
            // removed this entry; the CacheEntry will then be null.
            if ((*i) && (*i)->url == url) {
            	(*i)->lock_read_response();	// Lock the response
            	(*i)->lock();
                return *i;
            }
        }
    }

    return 0;
#endif
}
#endif

/** Get a pointer to a CacheEntry from the cache table. 

	@todo Add write lock feature
    @param url Look for this URL.
    @return The matching CacheEntry instance or NULL if none was found. */
HTTPCacheTable::CacheEntry *
HTTPCacheTable::get_write_locked_entry_from_cache_table(const string &url)
{
	db_rewind(d_db);
	char *data = db_fetch(d_db, url.c_str());
	HTTPCacheTable::CacheEntry *e = 0;	
	if (data)
		e = cache_index_parse_line(data);
	
	return e;
	
#ifdef old
	int hash = get_hash(url);
    if (d_cache_table[hash]) {
        CacheEntries *cp = d_cache_table[hash];
        for (CacheEntriesIter i = cp->begin(); i != cp->end(); ++i) {
            // Must test *i because perform_garbage_collection may have
            // removed this entry; the CacheEntry will then be null.
            if ((*i) && (*i)->url == url) {
            	(*i)->lock_write_response();	// Lock the response
            	(*i)->lock();
                return *i;
            }
        }
    }

    return 0;
#endif
}

/** Remove a CacheEntry. Ddelete the entry's files on disk. The caller should 
    remove the entry from the index database and delete the entry object.
    The total size of the cache is decremented once the entry's files are 
    removed.

	@todo add current size update that's MP-safe
    @param entry The CacheEntry to delete.
    @exception InternalErr Thrown if \c entry is in use. */
void
HTTPCacheTable::remove_cache_entry(HTTPCacheTable::CacheEntry *entry)
{
    REMOVE(entry->cachename.c_str());
    REMOVE(string(entry->cachename + CACHE_META).c_str());

    DBG(cerr << "remove_cache_entry, current_size: " << get_current_size() << endl);

    unsigned int eds = entry_disk_space(entry->size, get_block_size());
    update_current_size(-eds);
#if 0
    set_current_size((eds > get_current_size()) ? 0 : get_current_size() - eds);
#endif
    DBG(cerr << "remove_cache_entry, current_size: " << get_current_size() << endl);
    
#ifdef old
    // This should never happen; all calls to this method are protected by
    // the caller, hence the InternalErr.
    if (entry->readers)
        throw InternalErr(__FILE__, __LINE__, "Tried to delete a cache entry that is in use.");

    REMOVE(entry->cachename.c_str());
    REMOVE(string(entry->cachename + CACHE_META).c_str());

    DBG(cerr << "remove_cache_entry, current_size: " << get_current_size() << endl);

    unsigned int eds = entry_disk_space(entry->size, get_block_size());
    set_current_size((eds > get_current_size()) ? 0 : get_current_size() - eds);
    
    DBG(cerr << "remove_cache_entry, current_size: " << get_current_size() << endl);
#endif
}

#ifdef old
/** Functor which deletes and nulls a CacheEntry if the given entry matches
    the url. */
class DeleteCacheEntry: public unary_function<HTTPCacheTable::CacheEntry *&, void>
{
    string d_url;
    HTTPCacheTable *d_cache_table;

public:
    DeleteCacheEntry(HTTPCacheTable *c, const string &url)
            : d_url(url), d_cache_table(c)
    {}

    void operator()(HTTPCacheTable::CacheEntry *&e)
    {
        if (e && e->url == d_url) {
        	e->lock_write_response();
            d_cache_table->remove_cache_entry(e);
        	e->unlock_write_response();
            delete e; e = 0;
        }
    }
};
#endif
/** Find the CacheEntry for the given url and remove both its information in
    the persistent store and the entry in d_cache_table. If \c url is not in
    the cache, this method does nothing.

    @param url Remove this URL's entry.
    @exception InternalErr Thrown if the CacheEntry for \c url is locked. */
void
HTTPCacheTable::remove_entry_from_cache_table(const string &url)
{
	db_rewind(d_db);
	char *data = db_fetch(d_db, url.c_str());
	HTTPCacheTable::CacheEntry *e = 0;
	if (data) {
		e = cache_index_parse_line(data);
		remove_cache_entry(e);
		db_delete(d_db, url.c_str());
		delete e; e = 0;
	}

#ifdef old
	
    int hash = get_hash(url);
    if (d_cache_table[hash]) {
        CacheEntries *cp = d_cache_table[hash];
        for_each(cp->begin(), cp->end(), DeleteCacheEntry(this, url));
        cp->erase(remove(cp->begin(), cp->end(), static_cast<HTTPCacheTable::CacheEntry*>(0)),
                  cp->end());
    }
#endif
}

#if 0

// Used by purge_cache()

/** Functor to delete and null all unlocked HTTPCacheTable::CacheEntry objects. */

class DeleteUnlockedCacheEntry :
	public unary_function<HTTPCacheTable::CacheEntry *&, void> {
	HTTPCacheTable &d_table;

public:
	DeleteUnlockedCacheEntry(HTTPCacheTable &t) :
		d_table(t) {
	}
	void operator()(HTTPCacheTable::CacheEntry *&e) {
		if (e) {
			d_table.remove_cache_entry(e);
			delete e; e = 0;
		}
	}
};

void HTTPCacheTable::delete_all_entries() {
	// Walk through the cache table and, for every entry in the cache, delete
	// it on disk and in the cache table.
	for (int cnt = 0; cnt < CACHE_TABLE_SIZE; cnt++) {
		HTTPCacheTable::CacheEntries *slot = get_cache_table()[cnt];
		if (slot) {
			for_each(slot->begin(), slot->end(), DeleteUnlockedCacheEntry(*this));
			slot->erase(remove(slot->begin(), slot->end(), static_cast<HTTPCacheTable::CacheEntry *>(0)), 
					    slot->end());
		}
	}
	
	cache_index_delete();
}

#endif

/** Calculate the corrected_initial_age and freshness lifetime of the object. 
 	Record the times in the \e entry value-result parameter.
    We use the time when
    this function is called as the response_time as this is when we have
    received the complete response. This may cause a delay if the response
    header is very big but should not cause any incorrect behavior.

    A private method.

    @param entry The CacheEntry object, a value-result parameter.
    @param request_time When was the request made? I think this value must be
    passed into the method that calls this method... */

void
HTTPCacheTable::calculate_times(HTTPCacheTable::CacheEntry *entry, 
		int default_expiration, time_t request_time)
{
    entry->response_time = time(NULL);
    time_t apparent_age = max(0, static_cast<int>(entry->response_time - entry->date));
    time_t corrected_received_age = max(apparent_age, entry->age);
    time_t response_delay = entry->response_time - request_time;
    entry->corrected_initial_age = corrected_received_age + response_delay;

    // Estimate an expires time using the max-age and expires time. If we
    // don't have an explicit expires time then set it to 10% of the LM date
    // (although max 24 h). If no LM date is available then use 24 hours.
    time_t freshness_lifetime = entry->max_age;
    if (freshness_lifetime < 0) {
        if (entry->expires < 0) {
            if (entry->lm < 0) {
                freshness_lifetime = default_expiration;
            }
            else {
                freshness_lifetime = LM_EXPIRATION(entry->date - entry->lm);
            }
        }
        else
            freshness_lifetime = entry->expires - entry->date;
    }

    entry->freshness_lifetime = max(0, static_cast<int>(freshness_lifetime));

    DBG2(cerr << "Cache....... Received Age " << entry->age
         << ", corrected " << entry->corrected_initial_age
         << ", freshness lifetime " << entry->freshness_lifetime << endl);
}

/** Parse various headers from the vector (which can be retrieved from
    libcurl once a response is received) and load the CacheEntry object with
    values. This method should only be called with headers from a response
    (it should not be used to parse request headers).

    A private method.

    @param entry Store values from the headers here.
    @param headers A vector of header lines. */

void HTTPCacheTable::parse_headers(HTTPCacheTable::CacheEntry *entry,
		unsigned long max_entry_size, const vector<string> &headers) {
	vector<string>::const_iterator i;
	for (i = headers.begin(); i != headers.end(); ++i) {
		// skip a blank header.
		if ((*i).empty())
			continue;

		// skip a header with no colon in it.
		if ((*i).find(':') == string::npos)
			continue;

		string header = (*i).substr(0, (*i).find(':'));
		string value = (*i).substr((*i).find(": ") + 2);

		if (header == "ETag") {
			entry->etag = value;
		} else if (header == "Last-Modified") {
			entry->lm = parse_time(value.c_str());
		} else if (header == "Expires") {
			entry->expires = parse_time(value.c_str());
		} else if (header == "Date") {
			entry->date = parse_time(value.c_str());
		} else if (header == "Age") {
			entry->age = parse_time(value.c_str());
		} else if (header == "Content-Length") {
			unsigned long clength = strtoul(value.c_str(), 0, 0);
			if (clength > max_entry_size)
				entry->set_no_cache(true);
		} else if (header == "Cache-Control") {
			// Ignored Cache-Control values: public, private, no-transform,
			// proxy-revalidate, s-max-age. These are used by shared caches.
			// See section 14.9 of RFC 2612. 10/02/02 jhrg
			if (value == "no-cache" || value == "no-store")
				// Note that we *can* store a 'no-store' response in volatile
				// memory according to RFC 2616 (section 14.9.2) but those
				// will be rare coming from DAP servers. 10/02/02 jhrg
				entry->set_no_cache(true);
			else if (value == "must-revalidate")
				entry->must_revalidate = true;
			else if (value.find("max-age") != string::npos) {
				string max_age = value.substr(value.find("=" + 1));
				entry->max_age = parse_time(max_age.c_str());
			}
		}
	}
}

//@} End of the CacheEntry methods.

// @TODO Change name to record locked response
void HTTPCacheTable::bind_entry_to_data(HTTPCacheTable::CacheEntry *entry, FILE *body) {
#ifdef old
	entry->hits++;  // Mark hit
    d_locked_entries[body] = entry; // record lock, see release_cached_r...

    entry->unlock();			// Unlock the entry object so others can read it
#endif
}

void HTTPCacheTable::uncouple_entry_from_data(FILE *body) {
#ifdef old
	HTTPCacheTable::CacheEntry *entry = d_locked_entries[body];
    if (!entry)
        throw InternalErr("There is no cache entry for the response given.");

    d_locked_entries.erase(body);
    entry->unlock_read_response();

    if (entry->readers < 0)
        throw InternalErr("An unlocked entry was released");
#endif
}

#if 0
// This is used when purging the cache
bool HTTPCacheTable::is_locked_read_responses() {
	return !d_locked_entries.empty();
}
#endif
} // namespace libdap
