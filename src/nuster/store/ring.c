/*
 * nuster ring functions.
 *
 * Copyright (C) Jiang Wenyuan, < koubunen AT gmail DOT com >
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 */

#include <nuster/nuster.h>

int
nst_ring_init(nst_ring_t *ring, nst_memory_t *memory) {

    ring->memory = memory;
    ring->head   = NULL;
    ring->tail   = NULL;

    return nst_shctx_init(ring);
}

/*
 * create a new nst_ring_data and insert it to nst_ring list
 */
nst_ring_data_t *
nst_ring_alloc_data(nst_ring_t *ring) {
    nst_ring_data_t  *data = nst_memory_alloc(ring->memory, sizeof(*data));

    if(data) {
        memset(data, 0, sizeof(*data));

        nst_shctx_lock(ring);

        if(ring->head == NULL) {
            ring->head = data;
            ring->tail = data;
            data->next = data;
        } else {

            if(ring->head == ring->tail) {
                ring->head->next = data;
                data->next       = ring->head;
                ring->tail       = data;
            } else {
                data->next       = ring->head;
                ring->tail->next = data;
                ring->tail       = data;
            }
        }

        nst_shctx_unlock(ring);
    }

    return data;
}

/*
 * free invalid nst_ring_data
 */
void
nst_ring_cleanup(nst_ring_t *ring) {
    nst_ring_data_t  *data = NULL;

    if(ring->head) {

        if(ring->head == ring->tail) {

            if(nst_ring_data_invalid(ring->head)) {
                data       = ring->head;
                ring->head = NULL;
                ring->tail = NULL;
            }

        } else {

            if(nst_ring_data_invalid(ring->head)) {
                data             = ring->head;
                ring->tail->next = ring->head->next;
                ring->head       = ring->head->next;
            } else {
                ring->tail = ring->head;
                ring->head = ring->head->next;
            }

        }

    }

    if(data) {
        nst_ring_item_t  *item = data->item;

        while(item) {
            nst_ring_item_t  *tmp = item;
            item                  = item->next;

            nst_memory_free(ring->memory, tmp);
        }

        nst_memory_free(ring->memory, data);
    }
}

int
nst_ring_store_add(nst_ring_t *ring, nst_ring_data_t *data, nst_ring_item_t **tail,
        const char *buf, uint32_t len, uint32_t info) {

    nst_ring_item_t  *item;

    if(data->invalid) {
        return NST_ERR;
    }

    item = nst_ring_alloc_item(ring, len);

    if(!item) {
        data->invalid = 1;

        return NST_ERR;
    }

    memcpy(item->data, buf, len);

    item->info = info;
    item->next = NULL;

    if(*tail) {
        (*tail)->next = item;
    } else {
        data->item = item;
    }

    *tail = item;

    return NST_OK;
}
