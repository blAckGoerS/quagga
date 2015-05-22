/* Header file exported by rt_netlink.c to zebra.
 * Copyright (C) 1997, 98, 99 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef _ZEBRA_RT_NETLINK_H
#define _ZEBRA_RT_NETLINK_H

#ifdef HAVE_NETLINK

/* @PEMP copy from iproute2/lib/utils.c */

/* Modifiers to NEW request */
#define NLM_F_REPLACE	0x100	/* Override existing		*/
#define NLM_F_EXCL		0x200	/* Do not touch, if it exists	*/
#define NLM_F_CREATE	0x400	/* Create, if it does not exist	*/
#define NLM_F_APPEND	0x800	/* Add to end of list		*/

enum {
	FRA_UNSPEC,
	FRA_DST,	/* destination address */
	FRA_SRC,	/* source address */
	FRA_IIFNAME,	/* interface name */
#define FRA_IFNAME	FRA_IIFNAME
	FRA_GOTO,	/* target to jump to (FR_ACT_GOTO) */
	FRA_UNUSED2,
	FRA_PRIORITY,	/* priority/preference */
	FRA_UNUSED3,
	FRA_UNUSED4,
	FRA_UNUSED5,
	FRA_FWMARK,	/* mark */
	FRA_FLOW,	/* flow/class id */
	FRA_UNUSED6,
	FRA_SUPPRESS_IFGROUP,
	FRA_SUPPRESS_PREFIXLEN,
	FRA_TABLE,	/* Extended table id */
	FRA_FWMASK,	/* mask for netfilter mark */
	FRA_OIFNAME,
	__FRA_MAX
};
#define FRA_MAX (__FRA_MAX - 1)

#define NL_PKT_BUF_SIZE 8192

extern int
addattr32 (struct nlmsghdr *n, int maxlen, int type, int data);
extern int
addattr_l (struct nlmsghdr *n, int maxlen, int type, void *data, int alen);

extern int
rta_addattr_l (struct rtattr *rta, int maxlen, int type, void *data, int alen);

extern const char *
nl_msg_type_to_str (uint16_t msg_type);

extern const char *
nl_rtproto_to_str (u_char rtproto);


#endif /* HAVE_NETLINK */

#endif /* _ZEBRA_RT_NETLINK_H */
