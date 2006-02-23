
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2003 OPeNDAP, Inc.
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

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

#include "AISResources.h"
#include "AISDatabaseParser.h"

using namespace std;

/** Output the XML fragment for a Resource. This function is a friend of the
    Resource class. 
    @see Resource. */
ostream &
operator<<(ostream &os, const Resource &r)
{
    os << "<ancillary";
    if (r.d_rule != Resource::overwrite) {
	os << " rule=\"";
	(r.d_rule == Resource::fallback) ? os << "fallback\"": os << "replace\"";
    }
    os << " url=\"" << r.d_url << "\"/>";

    return os;
}

/** Output the XML for a collection of AIS resources. This function is a
    friend of the AISResource class.
    @see AISResources */
ostream &
operator<<(ostream &os, const AISResources &ais_res)
{
    os << "<?xml version=\"1.0\" encoding=\"US-ASCII\" standalone=\"yes\"?>"
       << endl;
    os << "<!DOCTYPE ais SYSTEM \"http://www.opendap.org/ais/ais_database.dtd\">" << endl;
    os << "<ais xmlns=\"http://xml.opendap.org/ais\">" << endl;

    for (AISResources::ResourceRegexpsCIter pos = ais_res.d_re.begin(); 
	pos != ais_res.d_re.end(); ++pos) {
	os << "<entry>" << endl;
	// write primary
	os << "<primary regexp=\"" << pos->first << "\"/>" << endl;
	// write the vector of Resource objects
	for (ResourceVectorCIter i = pos->second.begin(); 
	     i != pos->second.end(); ++i) {
	    os << *i << endl;
	}
	os << "</entry>" << endl;
    }

	//  Under VC++ 6.x, 'pos' is twice tagged as twice in the
	//  same scope (this method - not just within for blocks), so
	//  I gave it another name.  ROM - 6/14/03
#ifdef WIN32
    for (AISResources::ResourceMapCIter pos2 = ais_res.d_db.begin();
	 pos2 != ais_res.d_db.end(); ++pos2) {
	os << "<entry>" << endl;
	// write primary
	os << "<primary url=\"" << pos2->first << "\"/>" << endl;
	// write the vector of Resource objects
	for (ResourceVectorCIter i = pos2->second.begin(); 
	     i != pos2->second.end(); ++i) {
	    os << *i << endl;
	}
	os << "</entry>" << endl;
    }
#else
    for (AISResources::ResourceMapCIter pos = ais_res.d_db.begin();
	 pos != ais_res.d_db.end(); ++pos) {
	os << "<entry>" << endl;
	// write primary
	os << "<primary url=\"" << pos->first << "\"/>" << endl;
	// write the vector of Resource objects
	for (ResourceVectorCIter i = pos->second.begin(); 
	     i != pos->second.end(); ++i) {
	    os << *i << endl;
	}
	os << "</entry>" << endl;
    }
#endif

    os << "</ais>" << endl;

    return os;
}

/** Use an existing AIS database to build an instance. 
    @param database Pathname of the database/document. */
AISResources::AISResources(const string &database) throw(AISDatabaseReadFailed)
{
    read_database(database);
}

/** Add the given ancillary resource to the in-memory collection of
    mappings between primary and ancillary data sources. 
    @param url The target of the new mapping.
    @param ancillary Match this ancillary resource to the target (primary). */
void 
AISResources::add_url_resource(const string &url, const Resource &ancillary)
{
    add_url_resource(url, ResourceVector(1, ancillary));
}

/** Add a vector of AIS resources for the given primary data source URL. If
    there is already an entry for the primary, append the new ancillary
    resources to those.
    @param url The target of the new mapping.
    @param rv Ancillary resources matched to this primary resource. */
void 
AISResources::add_url_resource(const string &url, const ResourceVector &rv)
{
    ResourceMapIter pos = d_db.find(url);
    if (pos == d_db.end()) {
	d_db.insert(std::make_pair(url, rv));
    }
    else {
	// There's already a ResourceVector, append to it.
	for (ResourceVectorCIter i = rv.begin(); i != rv.end(); ++i)
	    pos->second.push_back(*i);
    }
}

/** Add the given ancillary resource to the in-memory collection of
    mappings between regular expressions and ancillary data sources. 
    @param re The target of the new mapping.
    @param ancillary  Match this ancillary resource to the target (primary). */
void 
AISResources::add_regexp_resource(const string &re, const Resource &ancillary)
{
    add_regexp_resource(re, ResourceVector(1, ancillary));
}

/** Add a vector of AIS resources for the given primary data source regular
    expression. If there is already an entry for the primary, append the new
    ancillary resources to those.

    @param re The target of the new mapping.
    @param rv Ancillary resources matched to this primary resource. */
void 
AISResources::add_regexp_resource(const string &re, const ResourceVector &rv)
{
    ResourceRegexpsIter pos = find_if(d_re.begin(), d_re.end(), 
				      FindRegexp(re));
    if (pos == d_re.end()) {
	d_re.push_back(std::make_pair(re, rv));
    }
    else {
	// There's already a ResourceVector, append to it.
	for (ResourceVectorCIter i = rv.begin(); i != rv.end(); ++i)
	    pos->second.push_back(*i);
    }
}

/** Return True if the given primary resource is listed in the current
    set of AIS resource mappings. That is, return true if there are some
    AIS resources registered for the given primary resource.
    @param primary The URL of the primary resource. 
    @return True if there are AIS resources for <code>primary</code>. */
bool 
AISResources::has_resource(const string &primary) const
{
    return ((d_db.find(primary) != d_db.end())
	    || (find_if(d_re.begin(), d_re.end(), MatchRegexp(primary)) 
		!= d_re.end()));

}

/** Return a vector of AIS Resource objects which are bound to the given
    primary resource. If a given \c primary resource has both an explicit
    entry for itself \e and matches a regular expression, the AIS resources
    for both will be combined in one ResourceVector and returned.

    @todo Make this return an empty ResourceVector is no matching resources
    are found. Clients would not need to call has_resource() which would save
    some time.

    @param primary The URL of the primary resource
    @return a vector of Resource objects.
    @exception NoSuchPrimaryResource thrown if <code>primary</code> is
    not present in the current mapping. */
ResourceVector
AISResources::get_resource(const string &primary)
    throw(NoSuchPrimaryResource)
{
    ResourceVector rv;
    const ResourceMapIter &i = d_db.find(primary);

    if (i != d_db.end())
	rv = i->second;

    const ResourceRegexpsIter &j = find_if(d_re.begin(), d_re.end(),
					   MatchRegexp(primary));
    if (j != d_re.end())
	copy(j->second.begin(), j->second.end(), inserter(rv, rv.begin()));

    if (rv.size() == 0)
	throw NoSuchPrimaryResource();

    return rv;	
}

/** Read the AIS database (an XML 'configuration file') and internalize it.
    @param database A file/pathname to the AIS database.
    @exception AISDatabaseReadFailed thrown if the database could not be
    read. */
void 
AISResources::read_database(const string &database) 
    throw(AISDatabaseReadFailed)
{
    AISDatabaseParser parser;

    parser.intern(database, this);
}

/** Write the current in-memory mapping of primary and ancillary
    resources to the named file so that the read_database() method can
    read them and recreate the in-memory mapping.
    @param filename A local file; write the database to this file. Create
    if necessary.
    @exception AISDatabaseWriteFailed thrown if the database could not be
    written. */
void 
AISResources::write_database(const string &filename)
    throw(AISDatabaseWriteFailed)
{
    ofstream fos;
    fos.open(filename.c_str());

    if (!fos)
	throw AISDatabaseWriteFailed("Could not open file :" + filename);

    fos << *this << endl;
    
    if (!fos)
	throw AISDatabaseWriteFailed();
}

