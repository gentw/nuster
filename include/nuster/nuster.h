/*
 * include/nuster/nuster.h
 * This file defines everything related to nuster.
 *
 * Copyright (C) Jiang Wenyuan, < koubunen AT gmail DOT com >
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, version 2.1
 * exclusively.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _NUSTER_H
#define _NUSTER_H

#define NUSTER_VERSION    "5.0.0.21"
#define NUSTER_COPYRIGHT  "2017-present, Jiang Wenyuan, <koubunen AT gmail DOT com >"

#include <common/chunk.h>
#include <types/applet.h>

#include <nuster/common.h>
#include <nuster/shctx.h>
#include <nuster/memory.h>
#include <nuster/http.h>
#include <nuster/cache.h>
#include <nuster/nosql.h>
#include <nuster/manager.h>
#include <nuster/persist.h>

struct nst_proxy {
    struct nst_rule     *rule;
    struct nst_rule_key *key;

    int                  rule_cnt;
    int                  key_cnt;
};

struct nuster {
    struct nst_cache *cache;
    struct nst_nosql *nosql;

    struct {
        struct applet cache;
        struct applet nosql;
        struct applet purger;
        struct applet stats;
    } applet;

    struct nst_proxy **proxy;
};

extern struct nuster nuster;


void nuster_init();

/* parser */
int nuster_parse_global_cache(const char *file, int linenum, char **args);
int nuster_parse_global_nosql(const char *file, int linenum, char **args);
int nuster_parse_global_manager(const char *file, int linenum, char **args);

static inline void nuster_housekeeping() {
    nst_cache_housekeeping();
    nst_nosql_housekeeping();
}

static inline int nuster_check_applet(struct stream *s, struct channel *req, struct proxy *px) {
    return nst_manager(s, req, px) || nst_nosql_check_applet(s, req, px);
}

int nst_key_build(struct stream *s, struct http_msg *msg, struct nst_rule *rule,
        struct nst_http_txn *txn, struct nst_key *key, enum http_meth_t method);

#endif /* _NUSTER_H */
