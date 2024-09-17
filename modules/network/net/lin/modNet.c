/*
 * Copyright (c) 2016-2019  Moddable Tech, Inc.
 *
 *   This file is part of the Moddable SDK Runtime.
 * 
 *   The Moddable SDK Runtime is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   The Moddable SDK Runtime is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 * 
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with the Moddable SDK Runtime.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "xs.h"

#include <ifaddrs.h>
#include <netdb.h>
#include <resolv.h>
#include <arpa/inet.h>

void xs_net_get(xsMachine *the)
{
	const char *prop = xsToString(xsArg(0));
	if (0 == c_strcmp(prop, "IP")) {
		int localhost = 0;
		struct ifaddrs *ifaddr, *ifa;
		if (getifaddrs(&ifaddr) == -1)
			return;
		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == NULL)
				continue;
			if (ifa->ifa_addr->sa_family == AF_INET) {
				struct sockaddr_in* address = (struct sockaddr_in*)ifa->ifa_addr;
				int ip = ntohl(address->sin_addr.s_addr);
				char buffer[22];
				snprintf(buffer, 22, "%u.%u.%u.%u", (ip & 0xff000000) >> 24, (ip & 0x00ff0000) >> 16, (ip & 0x0000ff00) >> 8, (ip & 0x000000ff));
				if (!c_strcmp(buffer, "127.0.0.1"))
					localhost = 1;
				else {
					xsResult = xsString(buffer);
					break;
				}
			}
		}
		if (!xsTest(xsResult) && localhost)
			xsResult = xsString("127.0.0.1");
		freeifaddrs(ifaddr);
	}
}

