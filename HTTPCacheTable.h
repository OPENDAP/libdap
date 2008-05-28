
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002 OPeNDAP, Inc.
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

#ifndef _http_cache_table_h
#define _http_cache_table_h

//#define DODS_DEBUG

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <pthread.h>

#ifdef WIN32
#include <io.h>   // stat for win32? 09/05/02 jhrg
#endif

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#ifndef _error_h
#include "Error.h"
#endif

#ifndef _internalerr_h
#include "InternalErr.h"
#endif

#ifndef _debug_h
#include "debug.h"
#endif

#define LOCK(m) pthread_mutex_lock((m))
#define TRYLOCK(m) pthread_mutex_trylock((m))
#define UNLOCK(m) pthread_mutex_unlock((m))
#define INIT(m) pthread_mutex_init((m), 0)
#define DESTROY(m) pthread_mutex_destroy((m))

// Defined in the apue_db library which we include
extern "C" int lock_reg(int, int, int, off_t, int, off_t);
#define	read_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define	readw_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define	write_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define	writew_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define	un_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

typedef	void *	DBHANDLE; 	// defined in apue_db but don't include that in 
							// a public header.

using namespace std;

namespace libdap
{

#if 0
int get_hash(const string &url);
#endif
/** The table of entires in the client-side cache.
    @todo This class needed to have the locking mechanism in place to work 
    or it must become a singleton. */
class HTTPCacheTable {
public:
	/** A struct used to store information about responses in the
	cache's volatile memory. 

	About entry locking: An entry is locked using both a mutex and a
	counter. The counter keeps track of how many clients are accessing a
	given entry while the mutex provides a guarantee that updates to the
	counter are MT-safe. In addition, the HTTPCache object maintains a
	map which binds the FILE* returned to a client with a given entry.
	This way the client can tell the HTTPCache object that it is done
	with <code>FILE *response</code> and the class can arrange to update
	the lock counter and mutex. */
	struct CacheEntry
	{
	private:
		string url;  // Location
#if 0
		int hash;
#endif
		int hits;  // Hit counts
	    string cachename;

	    string etag;
	    time_t lm;  // Last modified
	    time_t expires;
	    time_t date;  // From the response header.
	    time_t age;
	    time_t max_age;  // From Cache-Control

	    unsigned long size; // Size of cached entity body
	    bool range;  // Range is not currently supported. 10/02/02 jhrg

	    time_t freshness_lifetime;
	    time_t response_time;
	    time_t corrected_initial_age;

	    bool must_revalidate;
	    bool no_cache;  // This field is not saved in the index.

	    int readers;
	    int writer;
#if 0
	    pthread_mutex_t d_response_lock;	// set if being read
	    pthread_mutex_t d_response_write_lock;			// set if being written
	    
	    // This lock prevents access to the fields of a CacheEntry. Might not
	    // be needed.
	    pthread_mutex_t d_lock;
#endif	    
	    // Allow HTTPCacheTable methods access and the test class, too
	    friend class HTTPCacheTable;
		friend class HTTPCacheTest;
	    
		// Allow access by the fucntors used in HTTPCacheTable
		friend class DeleteCacheEntry;
	    friend class WriteOneCacheEntry;
	    friend class WriteOneCacheEntryToFILE;
	    friend class DeleteExpired;
	    friend class DeleteByHits;
	    friend class DeleteBySize;

	public:
		string get_cachename() {
			return cachename;
		}
		string get_etag() {
			return etag;
		}
		time_t get_lm() {
			return lm;
		}
		time_t get_expires() {
			return expires;
		}
		time_t get_max_age() {
			return max_age;
		}
		void set_size(unsigned long sz) {
			size = sz;
		}
		time_t get_freshness_lifetime() {
			return freshness_lifetime;
		}
		time_t get_response_time() {
			return response_time;
		}
		time_t get_corrected_initial_age() {
			return corrected_initial_age;
		}
		bool get_must_revalidate() {
			return must_revalidate;
		}
		void set_no_cache(bool state) {
			no_cache = state;
		}
	    bool is_no_cache() { return no_cache; }
	    
	    void lock() {
#if 0
	    	DBG(cerr << "Locking entry... (" << hex << &d_lock << dec << ") ");
	    	LOCK(&d_lock);
	        DBGN(cerr << "Done" << endl);
#endif
	    }
	    void unlock() {
#if 0
	    	DBG(cerr << "Unlocking entry... (" << hex << &d_lock << dec << ") ");
	    	UNLOCK(&d_lock);
	        DBGN(cerr << "Done" << endl);
#endif
	    }
#if 0
	    pthread_mutex_t &get_lock() { return d_lock; }
#endif	    
	    void lock_read_response() {
#if 0
	    	DBG(cerr << "Try locking read response... (" << hex << &d_response_lock << dec << ") ");
	        int status = TRYLOCK(&d_response_lock);
	        if (status != 0 /*&& status == EBUSY*/) {
	    		// If locked, wait for any writers
	    		LOCK(&d_response_write_lock);
	    		UNLOCK(&d_response_write_lock);
	    	}
	        DBGN(cerr << "Done" << endl);
	        readers++;			// REcord number of readers
#endif
	    }
	    
	    void unlock_read_response() {
#if 0
	    	readers--;
			if (readers == 0) {
				DBG(cerr << "Unlocking read response... (" << hex << &d_response_lock << dec << ") ");
				UNLOCK(&d_response_lock);
				DBGN(cerr << "Done" << endl);
			}
#endif
	    }
	    
	    void lock_write_response() {
#if 0
	    	DBG(cerr << "locking write response... (" << hex << &d_response_lock << dec << ") ");
	    	LOCK(&d_response_lock);
	    	LOCK(&d_response_write_lock);
	        DBGN(cerr << "Done" << endl);
#endif
	    }
	    
	    void unlock_write_response() {
#if 0
	    	DBG(cerr << "Unlocking write response... (" << hex << &d_response_lock << dec << ") ");
    		UNLOCK(&d_response_write_lock);
	    	UNLOCK(&d_response_lock);
	        DBGN(cerr << "Done" << endl);
#endif
	    }
	    
	    CacheEntry() :
			url(""), /*hash(-1),*/ hits(0), cachename(""), etag(""), lm(-1),
					expires(-1), date(-1), age(-1), max_age(-1), size(0),
					range(false), freshness_lifetime(0), response_time(0),
					corrected_initial_age(0), must_revalidate(false),
					no_cache(false), readers(0), writer(0) {
#if 0
	    	INIT(&d_response_lock);
	    	INIT(&d_response_write_lock);
			INIT(&d_lock);
#endif
	    }
		CacheEntry(const string &u) :
			url(u), /*hash(-1),*/ hits(0), cachename(""), etag(""), lm(-1),
					expires(-1), date(-1), age(-1), max_age(-1), size(0),
					range(false), freshness_lifetime(0), response_time(0),
					corrected_initial_age(0), must_revalidate(false),
					no_cache(false), readers(0), writer(0) {
#if 0
			INIT(&d_response_lock);
	    	INIT(&d_response_write_lock);
			INIT(&d_lock);
#endif
#if 0
			hash = get_hash(url);
#endif
		}
	};

	// Typedefs for CacheTable. A CacheTable is a vector of vectors of
	// CacheEntries. The outer vector is accessed using the hash value.
	// Entries with matching hashes occupy successive positions in the inner
	// vector (that's how hash collisions are resolved). Search the inner
	// vector for a specific match.
#if 0
	typedef vector<CacheEntry *> CacheEntries;
	typedef CacheEntries::iterator CacheEntriesIter;

	typedef CacheEntries **CacheTable;// Array of pointers to CacheEntries
#endif
	friend class HTTPCacheTest;
	
private:
#if 0
	CacheTable d_cache_table; // ***
#endif
	DBHANDLE d_db;
    int d_size_fd;
    
	string d_cache_root;
    unsigned int d_block_size; // File block size.
#if 0
    unsigned long d_current_size;
    string d_cache_index; //***
#endif
    string d_cache_db;
    int d_new_entries;
    
    map<FILE *, HTTPCacheTable::CacheEntry *> d_locked_entries;
    
	// Make these private to prevent use
	HTTPCacheTable(const HTTPCacheTable &) {
		throw InternalErr(__FILE__, __LINE__, "unimplemented");
	}
	
	HTTPCacheTable &operator=(const HTTPCacheTable &) {
		throw InternalErr(__FILE__, __LINE__, "unimplemented");
	}
	
	HTTPCacheTable() {
		throw InternalErr(__FILE__, __LINE__, "unimplemented");
	}
#if 0
	CacheTable &get_cache_table() { return d_cache_table; }
#endif
	CacheEntry *get_locked_entry_from_cache_table(int hash, const string &url); /*const*/
    string get_hash_directory();	

public:
	HTTPCacheTable(const string &cache_root, int block_size);
	~HTTPCacheTable();
	
	//@{ @name Accessors/Mutators

	unsigned long get_current_size() const {
		if (readw_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not lock size file");
		long size;
		try {
			if (lseek(d_size_fd, 0, SEEK_SET) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not seek size file");
			if (read(d_size_fd, &size, sizeof(long)) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not read size file");
			if (un_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not unlock size file");
		}
		catch(...) {
			un_lock(d_size_fd, (off_t)0, SEEK_SET, (off_t)0);
			throw;
		}
		return size;
	}
	
	void set_current_size(long sz) { 
		if (writew_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not lock size file");
		try {
			if (lseek(d_size_fd, 0, SEEK_SET) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not seek size file");
			if (write(d_size_fd, &sz, sizeof(long)) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not write size ");
			if (un_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not unlock size file");
		}
		catch(...) {
			un_lock(d_size_fd, 0, SEEK_SET, 0);
			throw;
		}
	}
	
	void update_current_size(long sz) { 
		if (writew_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not lock size file");

		try {
			long size;
			if (lseek(d_size_fd, 0, SEEK_SET) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not seek size file");
			if (read(d_size_fd, &size, sizeof(long)) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not read size file");
			
			size = max((long)0, size + sz); // sz might be negative
			if (lseek(d_size_fd, 0, SEEK_SET) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not seek size file");
			if (write(d_size_fd, &size, sizeof(long)) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not write size ");
			
			if (un_lock(d_size_fd, 0, SEEK_SET, 0) < 0)
				throw InternalErr(__FILE__, __LINE__, "Could not unlock size file");
		}
		catch(...) {
			un_lock(d_size_fd, (off_t)0, SEEK_SET, (off_t)0);
			throw;
		}
	}
		
	unsigned int get_block_size() const { return d_block_size; }
	void set_block_size(unsigned int sz) { d_block_size = sz; }
	
	// Remove these - new DB won't need them
	int get_new_entries() const { return d_new_entries; }
	void increment_new_entries() { ++d_new_entries; }
	
	string get_cache_root() { return d_cache_root; }
	void set_cache_root(const string &cr) { d_cache_root = cr; }
	//@}

	// Implement this using nextrec from db
	void delete_expired_entries(time_t time = 0);
	void delete_by_hits(int hits);
	void delete_by_size(unsigned int size);
#if 0
	// Used by purge_cache()
	void delete_all_entries();
#endif	
	// Don't need these
	bool cache_index_delete();
	bool cache_index_read();
	CacheEntry *cache_index_parse_line(const char *line);
	void cache_index_write();

    void create_location(CacheEntry *entry);

    // This should write the entry to the DB
	void add_new_entry_to_cache_table(CacheEntry *entry);
	void remove_cache_entry(HTTPCacheTable::CacheEntry *entry);

	void remove_entry_from_cache_table(const string &url);
	CacheEntry *get_locked_entry_from_cache_table(const string &url);
	CacheEntry *get_write_locked_entry_from_cache_table(const string &url);

	void calculate_times(HTTPCacheTable::CacheEntry *entry,
			int default_expiration, time_t request_time);
	void parse_headers(HTTPCacheTable::CacheEntry *entry, 
			unsigned long max_entry_size, const vector<string> &headers);
	
	// These should move back to HTTPCache
	void bind_entry_to_data(CacheEntry *entry, FILE *body);
	void uncouple_entry_from_data(FILE *body);
#if 0
	// Used when purging the cache
	bool is_locked_read_responses();
#endif
};

} // namespace libdap
#endif
