/*
 *   This software is Copyright 2011 by Sean Groarke <sgroarke@gmail.com>
 *   All rights reserved.
 *
 *   This file is part of npd6.
 *
 *   npd6 is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   npd6 is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with npd6.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Id: npd6config.h 66 2011-08-23 14:54:08Z npd6Project $
 * $HeadURL: https://npd6.googlecode.com/svn/trunk/npd6config.h $
 */

// Order of #defines must equal order in the string array
#define NPD6PREFIX      0
#define NPD6INTERFACE   1
#define NPD6OPTFLAG     2
#define NPD6LOCALIG     3
#define NPD6ROUTERNA    4
#define NPD6MAXHOPS     5
#define NPD6TARGETS     6
#define NPD6LISTTYPE    7
#define NPD6LISTADDR    8

#define CONFIGTOTAL     9
#define NOMATCH         -1
char *configStrs[CONFIGTOTAL] =
{
    "prefix",
    "interface",
    "linkOption",
    "ignoreLocal",
    "routerNA",
    "maxHops",
    "collectTargets",
    "listtype",
    "addrlist"
};

// For logging system
#define NPD6LOGGING     "logging"
#define NPD6FILE        "file"
#define NPD6SYSLOG      "syslog"

#define SET             "true"
#define UNSET           "false"

#define NPD6NONE        "none"
#define NPD6BLACK       "black"
#define NPD6WHITE       "white"

