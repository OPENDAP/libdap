Updated for version 3.11.0

Now constraint expressions can have multiple function calls that return data.

I've bumped up the DAP version from 3.3 to 3.4 to reflect this change.

Updated for Version 3.10.2

BaseType::transfer_attributes() and related methods provide a way forhandlers to customize how attributes from a DAS object are merged intoa DDS.

In the past we supported a kind of client-side system that could augmentthe attributes call the 'AIS' (Ancillary Information System). This hasbeen removed - our server now supports the NcML language to do much the samething but in a way that can be set on the server once for all users. It's alsoan emerging convention that's gaining wide support within the community.

Updated for Version 3.10.0

DAP 3.3 is now supported; see http://docs.opendap.org/index.php/DAP3/4.

This version of libdap contains many changes that are needed for bothDAP 4 and the NcML handler. This version of the library is requiredfor the Hyrax 1.6 handlers.

The 'deflate' program is no longer part of this library package sincewe are no longer supporting the old data server system (based on WWW'sCGI specification).

Updated for version 3.9.2

Now libdap supports DAP 3.2. You can read about the evolving DAP 3.x protocolat http://docs.opendap.org/index.php/DAP3/4. If your client sends theXDAP-Accept header with a value of 3.2 the DDX is different (it includesprotocol information and also an xmlbase element).

Behavior change for the DAS: In the past the format handlers added doublequotes to the values of string attributes when they added those values to theAttrTable object. This meant that the value of the attribute in the C++object was actually not correct since it contained quotes not found in theoriginal attribute value. I modified libdap so that if an attribute value inthe C++ AttrTable object does not have quotes then those quotes are addedwhen the value is output in a DAS response (but not a DDX since there's noneed to quote the value in that response). This ensures that the text in theDAS wire representation will parse whether a handler has added quotes or not(paving the way for fixed handlers). At the same time I fixed all of ourhandlers so that they no longer add the erroneous quotes. This fixes aproblem with the DDX where the quotes were showing up as part of theattribute value. The change to libdap is such that a broken handler will notbe any more broken but a fixed handler will work for both DAS and DDXgeneration.

If you have a handler and it's not adding quotes to the String attribute values - good, don't change that! If your handler does add quotes, pleasemodify it so the DDX will be correct.  

Our handler's old, broken, behavior can be resurrected by removing the ATTR_STRING_QUOTE FIX define in the appropriate files.

Updated for version 3.8.2 (23 June 2008)

HTTP Cache and win32 installer fixes (the latter are actually in the 3.8.1installer for winXP). API change: The functions used to merge ancillary datahave been moved to their own class (Ancillary).

Updated for version 3.8.1 (10 June 2008)

The syntax for PROXY_SERVER in the .dodsrc file was relaxed. See the .dodsrcfile for more information.

Updated for Version 3.8.0 (29 February 2008)

Older version of this package contain the 'Server3' CGI software; thathas been removed from this version.

Hyrax BES module

  For Hyrax this package includes the library modules that can be  dynamically loaded into the OPeNDAP Back-End Server (BES) as well as  help files for the modules. To load these modules into the BES  simply edit the BES configuration file, its default location in  /etc/bes/bes.conf. Directions for editing the configuration file  follow, however, you can run the bes-dap-data.sh script to edit the  script automatically. Or, if building from source you can use the  bes-conf target of make to run the script for you.

  To edit the bes.conf script by hand:

  Set the BES user, group and admin email (see the comments in the  bes.conf file for instructions).

  Change the parameter BES.modules to include the three dap-server  modules ascii, usage and www as follows (example assumes you have  installed the netcdf_handler module):

    BES.modules=dap,cmds,ascii,usage,www,nc


  And add the following three lines below this:

    BES.module.ascii=/usr/lib/bes/libascii_module.so
    BES.module.usage=/usr/lib/bes/libusage_module.so
    BES.module.www=/usr/lib/bes/libwww_module.so


Updated for Version 3.7.9 (13 November 2007)

    DAP-SERVER.Help.TXT=/usr/share/bes/dap-server_help.txt
    DAP-SERVER.Help.HTML=/usr/share/bes/dap-server_help.html
    DAP-SERVER.Help.XML=/usr/share/bes/dap-server_help.html


  The next time the BES is started these modules will be loaded and  the ascii, info_page, and html_form responses will be supported as  well as the help files for dap-server specifying the syntax of these  commands.

