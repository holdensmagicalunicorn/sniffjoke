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
 * Some sniffer don't keep attention to the sequence and the data, but to
 * the acknowledge sequence number. a shift sequence hack work sending a fake
 * ACK-packet with a seq_ack totally wrong. this was one of the hacks I've tried
 * to use without GUILTY/PRESCRIPTION invalidation, but as INNOCENT, because 
 * if the ack is shifted more than the window value, the remote host must
 * invalidate them
 *
 * SOURCE: deduction, 
 * VERIFIED IN:
 * KNOW BUGS:
 * WRITTEN IN VERSION: 0.4.0
 */

#include "service/Hack.h"

class shift_ack : public Hack
{
#define HACK_NAME "unexpected ACK shift"

public:

    virtual void createHack(const Packet &origpkt, uint8_t availableScramble)
    {
        Packet * const pkt = new Packet(origpkt);

        pkt->ip->id = htons(ntohs(pkt->ip->id) - 10 + (random() % 20));

        pkt->tcp->ack_seq = htonl(ntohl(pkt->tcp->ack_seq) - MTU + random() % 2 * MTU);

        pkt->position = ANY_POSITION;
        pkt->wtf = pktRandomDamage(availableScramble & supportedScramble);
        pkt->choosableScramble = (availableScramble & supportedScramble);

        pktVector.push_back(pkt);
    }

    virtual bool Condition(const Packet &origpkt, uint8_t availableScramble)
    {
        if (!(availableScramble & supportedScramble))
        {
            origpkt.SELFLOG("no scramble avalable for %s", HACK_NAME);
            return false;
        }

        return (
                !origpkt.tcp->syn &&
                !origpkt.tcp->rst &&
                !origpkt.tcp->fin &&
                origpkt.tcp->ack
                );
    }

    virtual bool initializeHack(uint8_t configuredScramble)
    {
        supportedScramble = configuredScramble;
        return true;
    }

    shift_ack(bool forcedTest) : Hack(HACK_NAME, forcedTest ? AGG_ALWAYS : AGG_RARE)
    {
    }
};

extern "C" Hack* CreateHackObject(bool forcedTest)
{
    return new shift_ack(forcedTest);
}

extern "C" void DeleteHackObject(Hack *who)
{
    delete who;
}

extern "C" const char *versionValue()
{
    return SW_VERSION;
}
