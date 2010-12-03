/*
 *   SniffJoke is a software able to confuse the Internet traffic analysis,
 *   developed with the aim to improve digital privacy in communications and
 *   to show and test some securiy weakness in traffic analysis software.
 *   
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

/*
 * HACK COMMENT:, every hacks require intensive comments because should cause 
 * malfunction, or KILL THE INTERNET :)
 *
 * fake close is used because a sniffer could read a FIN like a session closing
 * tcp-flag, and stop the session monitoring/reassembly.
 *
 * SOURCE: phrack, deduction, 
 * VERIFIED IN:
 * KNOW BUGS:
 */

#include "Hack.h"

class fake_close_fin : public Hack
{
#define HACK_NAME	"Fake FIN"
public:
	virtual void createHack(const Packet &orig_packet)
	{
		Packet* pkt = new Packet(orig_packet);

		orig_packet.selflog(HACK_NAME, "Original packet");		

		pkt->TCPPAYLOAD_resize(0);

		pkt->ip->id = htons(ntohs(pkt->ip->id) + (random() % 10));
		pkt->tcp->seq = htonl(ntohl(pkt->tcp->seq) - pkt->datalen + 1);

		pkt->tcp->psh = 0;
		pkt->tcp->fin = 1;

		pkt->position = ANTICIPATION;
		pkt->wtf = RANDOMDAMAGE;
		pkt->proto = TCP;

		pkt->selflog(HACK_NAME, "Hacked packet");

		pktVector.push_back(pkt);
	}

	virtual bool Condition(const Packet &orig_packet)
	{
		return (
			!orig_packet.tcp->syn &&
			!orig_packet.tcp->rst &&
			!orig_packet.tcp->fin &&
			orig_packet.tcp->ack
		);
	}

	fake_close_fin() : Hack(HACK_NAME, PACKETS30PEEK) {}
};

extern "C"  Hack* CreateHackObject() {
	return new fake_close_fin();
}

extern "C" void DeleteHackObject(Hack *who) {
	delete who;
}

extern "C" const char *versionValue() {
 	return SW_VERSION;
}
