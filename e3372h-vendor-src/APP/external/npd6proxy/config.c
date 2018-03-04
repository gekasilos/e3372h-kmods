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

/* $Id: config.c 93 2012-04-16 08:59:51Z sgroarke $
* $HeadURL: https://npd6.googlecode.com/svn/trunk/config.c $
*/

#include "includes.h"
#include "npd6.h"
#include "npd6config.h"

//*******************************************************
// Take supplied filename and open it, then parse the contents.
// Upon return following set:
//
// OR we failed -1.
int readConfig(char *configFileName)
{
    char linein[256];
    int len;
    const char delimiters[] = "=";
    char *lefttoken, *righttoken;
    char *cp;
    // Used if building white/blacklist
    struct  in6_addr    listEntry;

    strncpy( interfacestr, NPDAEMON_WAN_IF_NAME, sizeof(interfacestr));
    flog(LOG_INFO, "Supplied interface is %s", interfacestr);
    listType = NOLIST;
    if (tRoot != NULL)
    {
        tdestroy(tRoot, free);
        tEntries = 0;
    }
    tRoot = NULL;
    collectTargets = 100;
    naLinkOptFlag = 0;
    nsIgnoreLocal = 1;
    naRouter = 1;
    maxHops = 255;
    interfaceIdx = if_nametoindex(interfacestr);
    if (!interfaceIdx)
    {
       flog(LOG_ERR, "Could not get ifIndex for interface %s", interfacestr);
       return 1;
    }

    while (getLinkaddress( interfacestr, linkAddr) )
    {
       flog(LOG_ERR, "failed to convert interface specified to a link-level address continue.");
	   sleep(1);
    }
    return 0;
    if ((configFileFD = fopen(configFileName, "r")) == NULL)
    {
        fprintf(stderr, "Can't open %s: %s\n", configFileName, strerror(errno));
        flog(LOG_ERR, "Can't open config file %s: %s", configFileName, strerror(errno));
    }

    // This is real simple config file parsing...
    do {
        int strToken, strIdx;

        len = 0;
        if (fgets(linein, 128, configFileFD) == NULL)
            break;
        // Tidy it up
            stripwhitespace(linein);
            // Special mega-hacky thing for blank lines:
            len = strlen(linein);
            if (len==0)
            {
                len=1;
                continue;
            }

            // Tokenize
            cp = strdupa(linein);
            lefttoken = strtok(cp, delimiters);
            righttoken = strtok(NULL, delimiters);
            if ( (lefttoken == NULL) || (righttoken == NULL) )
            {
                continue;
            }

            // Match token
            strIdx = -1;
            for(strToken = 0; strToken < CONFIGTOTAL; strToken++)
            {
                if( !strcmp(lefttoken, configStrs[strToken]) )
                {
                    strIdx = strToken;
                    // Matched, so drop to next step
                    break;
                }
            }
            flog(LOG_DEBUG2, "Matched config item index: %d", strIdx);

            // If config params are being added, it should only be required
            // to update the strings in npd6config.h and then insert a
            // case XXXXXXX: here with self-contined code inside.
            switch (strIdx) {
                case NOMATCH:
                    flog(LOG_DEBUG2, "Found noise in config file. Skipping.");
                    continue;

                case NPD6PREFIX:
                    strncpy( prefixaddrstr, righttoken, sizeof(prefixaddrstr));
                    flog(LOG_DEBUG, "Raw prefix: %s", prefixaddrstr);
                    // We need to pad it up and record the length in bits
                    prefixaddrlen = prefixset(prefixaddrstr);
                    flog(LOG_INFO, "Padded prefix: %s, length = %d", prefixaddrstr, prefixaddrlen);
                    if ( prefixaddrlen <= 0 )
                    {
                        flog(LOG_ERR, "Invalid prefix.");
                    }
                    // Build a binary image of it
                    build_addr(prefixaddrstr, &prefixaddr);
                    break;

                case NPD6INTERFACE:
                    if ( strlen( righttoken) > INTERFACE_STRLEN )
                    {
                        flog(LOG_ERR, "Invalid length interface name");
                        return 1;
                    }
                    strncpy( interfacestr, righttoken, sizeof(interfacestr));
                    flog(LOG_INFO, "Supplied interface is %s", interfacestr);
                    break;

                case NPD6OPTFLAG:
                    if ( !strcmp( righttoken, SET ) )
                    {
                        flog(LOG_INFO, "linkOption flag SET");
                        naLinkOptFlag = 1;
                    }
                    else if ( !strcmp( righttoken, UNSET ) )
                    {
                        flog(LOG_INFO, "linkOption flag UNSET");
                        naLinkOptFlag = 0;
                    }
                    else
                    {
                        flog(LOG_ERR, "linkOption flag - Bad value");
                        return 1;
                    }
                    break;

                case NPD6LOCALIG:
                    if ( !strcmp( righttoken, SET ) )
                    {
                        flog(LOG_INFO, "ignoreLocal flag SET");
                        nsIgnoreLocal = 1;
                    }
                    else if ( !strcmp( righttoken, UNSET ) )
                    {
                        flog(LOG_INFO, "ignoreLocal flag UNSET");
                        nsIgnoreLocal = 0;
                    }
                    else
                    {
                        flog(LOG_ERR, "ignoreLocal flag - Bad value");
                        return 1;
                    }
                    break;

                case NPD6ROUTERNA:
                    if ( !strcmp( righttoken, SET ) )
                    {
                        flog(LOG_INFO, "routerNA flag SET");
                        naRouter = 1;
                    }
                    else if ( !strcmp( righttoken, UNSET ) )
                    {
                        flog(LOG_INFO, "routerNA flag UNSET");
                        naRouter = 0;
                    }
                    else
                    {
                        flog(LOG_ERR, "routerNA flag - Bad value");
                        return 1;
                    }
                    break;

                case NPD6MAXHOPS:
                    maxHops = -1;
                    maxHops = atoi(righttoken);

                    if ( (maxHops < 0) || (maxHops > MAXMAXHOPS) )
                    {
                        flog(LOG_ERR, "maxHops - invalid value specified in config.");
                        return 1;
                    }
                    else
                    {
                        flog(LOG_INFO, "maxHops set to %d", maxHops);
                    }
                    break;

                case NPD6TARGETS:
                    // If we arrive here and the tRoot tree already exists,
                    // then we're re-reading the config and so need to zap
                    // the tRoot data first.
                    if (tRoot != NULL)
                    {
                        tdestroy(tRoot, free);
                        tEntries = 0;
                    }
                    collectTargets = -1;
                    tRoot = NULL;
                    collectTargets = atoi(righttoken);

                    if ( (collectTargets < 0) || (collectTargets > MAXTARGETS) )
                    {
                        flog(LOG_ERR, "collectTargets - invalid value specified in config.");
                        return 1;
                    }
                    else
                    {
                        flog(LOG_INFO, "collectTargets set to %d", collectTargets);
                    }
                    break;
                case NPD6LISTTYPE:
                    if ( !strcmp( righttoken, NPD6NONE ) )
                    {
                        flog(LOG_INFO, "List-type = NONE");
                        listType = NOLIST;
                    }
                    else if ( !strcmp( righttoken, NPD6BLACK ) )
                    {
                        flog(LOG_INFO, "List-type = BLACK");
                        listType = BLACKLIST;
                    }
                    else if( !strcmp( righttoken, NPD6WHITE ) )
                    {
                        flog(LOG_INFO, "List-type = WHITE");
                        listType = WHITELIST;
                    }
                    else
                    {
                        flog(LOG_ERR, "List-type = <invalid value> - Setting to NONE");
                        listType = NOLIST;
                    }
                    break;
                case NPD6LISTADDR:
                    if (build_addr( righttoken, &listEntry) )
                    {
                        flog(LOG_DEBUG, "Address %s valid.", righttoken);
                        storeListEntry(&listEntry);
                    }
                    else
                    {
                        flog(LOG_ERR, "Address %s invalid.", righttoken);
                    }
                    break;
            }
    } while (len);



    // Now do some final checks to ensure all required params were supplied
    if ( ! strcmp(prefixaddrstr, NULLSTR) )
    {
        flog(LOG_ERR, "Prefix not defined in config file.");
        return 1;
    }
    if ( ! strcmp(interfacestr, NULLSTR) )
    {
        flog(LOG_ERR, "interface not defined in config file.");
        return 1;
    }

    // Work out the interface index
    interfaceIdx = if_nametoindex(interfacestr);
    if (!interfaceIdx)
    {
        flog(LOG_ERR, "Could not get ifIndex for interface %s", interfacestr);
        return 1;
    }

    if (getLinkaddress( interfacestr, linkAddr) )
    {
        flog(LOG_ERR, "failed to convert interface specified to a link-level address.");
        return 1;
    }


    return 0;
}
