
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
 
#ifdef __GNUG__
// #pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: ResponseTooBigErr.cc,v 1.7 2004/07/07 21:08:48 jimg Exp $"};

#include <string>

#include "ResponseTooBigErr.h"

ResponseTooBigErr::ResponseTooBigErr() : Error()
{
    _error_code=unknown_error;
}

ResponseTooBigErr::ResponseTooBigErr(const string &msg) : Error()
{
    _error_code=unknown_error;
    _error_message="";
    _error_message+="A caching error was encounterd:\n";
    _error_message+=msg+"\n";
}


// $Log: ResponseTooBigErr.cc,v $
// Revision 1.7  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.5.2.1  2004/03/04 19:53:03  jimg
// Fixed $Id --> $Id: ResponseTooBigErr.cc,v 1.7 2004/07/07 21:08:48 jimg Exp $
//
// Revision 1.6  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.5  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.4  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.1  2002/10/08 05:30:28  jimg
// Added.
//
