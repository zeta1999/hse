/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2015-2020 Micron Technology, Inc.  All rights reserved.
 */

#ifndef HSE_C1_OPS_H
#define HSE_C1_OPS_H

#include "c1_utils.h"
#include "c1_kv.h"

/* MTF_MOCK_DECL(c1_ops) */

/* struct c1 -
 *
 * @c1_version: This is the OMF version used during c1 replay when records
 *              are read from media. On a running system, this field is never
 *              looked at and it stays the same as what got filled during c1
 *              replay.
 */
struct c1 {
    struct mutex            c1_list_mtx;
    struct mutex            c1_active_mtx;
    struct mutex            c1_alloc_mtx;
    struct mutex            c1_txn_mtx;
    atomic_t                c1_active_cnt;
    bool                    c1_rdonly;
    bool                    c1_vbldr;
    u64                     c1_ingest_kvseqno;
    u64                     c1_kvdb_seqno;
    u64                     c1_txnid;
    u64                     c1_kvms_gen;
    u16                     c1_version; /* used only during replay */
    struct c1_io *          c1_io;
    struct c1_journal *     c1_jrnl;
    struct c1_replay        c1_rep;
    struct list_head        c1_tree_reset;
    struct list_head        c1_tree_inuse;
    struct list_head        c1_tree_clean;
    struct list_head        c1_tree_new;
    struct list_head        c1_txn;
    struct ikvdb *          c1_ikvdb;
    struct c0sk *           c1_c0sk;
    struct ikvdb_c1_replay *c1_replay_hdl;
    struct c1_kvcache       c1_kvc[HSE_C1_DEFAULT_STRIPE_WIDTH];

    /* Perf counters */
    struct perfc_set c1_pcset_op;
    struct perfc_set c1_pcset_kv;
    struct perfc_set c1_pcset_tree;
};

typedef merr_t
c1_journal_replay_cb(struct c1 *c1, u32 cmd, void *rec, void *rec2);

static inline void
c1_set_hdr(struct c1_hdr_omf *hdr, int type, int len)
{
    omf_set_c1hdr_type(hdr, type);
    omf_set_c1hdr_len(hdr, len - sizeof(*hdr));
}

static inline bool
c1_rdonly(struct c1 *c1)
{
    return c1->c1_rdonly;
}

/* MTF_MOCK */
u64
c1_kvmsgen(struct c1 *c1);

static inline bool
c1_vbldr(struct c1 *c1)
{
    return c1->c1_vbldr;
}

static inline struct c0sk *
c1_c0sk_get(struct c1 *c1)
{
    return c1->c1_c0sk;
}

BullseyeCoverageSaveOff static inline void
c1_set_kvdb_seqno(struct c1 *c1, u64 seqno)
{
    if ((c1->c1_kvdb_seqno == C1_INVALID_SEQNO) || (c1->c1_kvdb_seqno < seqno))
        c1->c1_kvdb_seqno = seqno;
}

static inline u64
c1_get_kvdb_seqno(struct c1 *c1)
{
    return c1->c1_kvdb_seqno;
}
BullseyeCoverageRestore

    /* MTF_MOCK */
    struct c1 *
    c1_create(const char *mpname);

void
c1_destroy(struct c1 *c1);

/* MTF_MOCK */
u64
c1_get_size(u64 size);

/* MTF_MOCK */
u64
c1_get_capacity(u64 capacity);

/* MTF_MOCK */
bool
c1_jrnl_reaching_capacity(struct c1 *c1);

/* MTF_MOCK */
merr_t
c1_replay_on_ikvdb(
    struct c1 *        c1,
    struct ikvdb *     ikvdb,
    u64                cnid,
    u64                seqno,
    struct kvs_ktuple *kt,
    struct kvs_vtuple *vt,
    bool               tomb);

/* MTF_MOCK */
bool
c1_ingest_seqno(struct c1 *c1, u64 seqno);

#if defined(HSE_UNIT_TEST_MODE) && HSE_UNIT_TEST_MODE == 1
#include "c1_ops_ut.h"
#endif /* HSE_UNIT_TEST_MODE */

#endif /* HSE_C1_OPS_H */
