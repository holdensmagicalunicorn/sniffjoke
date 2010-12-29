/*
 *   SniffJoke is a software able to confuse the Internet traffic analysis,
 *   developed with the aim to improve digital privacy in communications and
 *   to show and test some securiy weakness in traffic analysis software.
    
 *   Copyright (C) 2010 vecna <vecna@delirandom.net>
 *                      evilaliv3 <giovanni.pellerano@evilaliv3.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Debug.h"

Debug::Debug() :
	debuglevel(ALL_LEVEL),
	logstream(NULL),
	session_logstream(NULL),
	packet_logstream(NULL)
{}

bool Debug::appendOpen(uint8_t thislevel, const char fname[LARGEBUF], FILE **previously)
{
	if(*previously != NULL) {
		log(thislevel, "requested close of logfile %s", fname);
		fclose(*previously);
	}

	if(debuglevel >= thislevel) 
	{
		if((*previously = fopen(fname, "a+")) == NULL) {
			return false;
		}

		log(thislevel, "opened file %s successful with debug level %d", fname, debuglevel);
	}

	return true;
}

bool Debug::resetLevel(const char logfname[LARGEBUF], const char sessionlog[LARGEBUF], const char packetlog[LARGEBUF]) 
{
	if(!appendOpen(ALL_LEVEL, logfname, &logstream))
		return false;

	if(!appendOpen(SESSION_DEBUG, sessionlog, &session_logstream))
		return false;

	if(!appendOpen(PACKETS_DEBUG, packetlog, &packet_logstream))
		return false;

	return true;
}

void Debug::log(uint8_t errorlevel, const char *msg, ...) 
{
	if (errorlevel <= debuglevel) { 
		va_list arguments;
		time_t now = time(NULL);
		FILE *output_flow;

		if (logstream != NULL)
			output_flow = logstream;
		else
			output_flow = stderr;

		if (errorlevel == PACKETS_DEBUG && packet_logstream != NULL)
			output_flow = packet_logstream;

		if (errorlevel == SESSION_DEBUG && session_logstream != NULL)
			output_flow = session_logstream;

		char time_str[sizeof("YYYY-MM-GG HH:MM:SS")];
		strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

		va_start(arguments, msg);
		fprintf(output_flow, "%s ", time_str);
		vfprintf(output_flow, msg, arguments);
		fprintf(output_flow, "\n");
		fflush(output_flow);
		va_end(arguments);
	}
}

void Debug::downgradeOpenlog(uint16_t uid, uint16_t gid) {

	/* this should not be called when is not the root process to do */
	if(getuid() && getgid())
		return;

	if(logstream != NULL)
		fchown(fileno(logstream), uid, gid);

	if(packet_logstream != NULL)
		fchown(fileno(packet_logstream), uid, gid);

	if(session_logstream != NULL)
		fchown(fileno(session_logstream), uid, gid);
}

