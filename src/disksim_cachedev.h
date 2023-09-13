/*
 * DiskSim Storage Subsystem Simulation Environment (Version 4.0)
 * Revision Authors: John Bucy, Greg Ganger
 * Contributors: John Griffin, Jiri Schindler, Steve Schlosser
 *
 * Copyright (c) of Carnegie Mellon University, 2001-2008.
 *
 * This software is being provided by the copyright holders under the
 * following license. By obtaining, using and/or copying this software,
 * you agree that you have read, understood, and will comply with the
 * following terms and conditions:
 *
 * Permission to reproduce, use, and prepare derivative works of this
 * software is granted provided the copyright and "No Warranty" statements
 * are included with all reproductions and derivative works and associated
 * documentation. This software may also be redistributed without charge
 * provided that the copyright and "No Warranty" statements are included
 * in all redistributions.
 *
 * NO WARRANTY. THIS SOFTWARE IS FURNISHED ON AN "AS IS" BASIS.
 * CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED AS TO THE MATTER INCLUDING, BUT NOT LIMITED
 * TO: WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY
 * OF RESULTS OR RESULTS OBTAINED FROM USE OF THIS SOFTWARE. CARNEGIE
 * MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT
 * TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * COPYRIGHT HOLDERS WILL BEAR NO LIABILITY FOR ANY USE OF THIS SOFTWARE
 * OR DOCUMENTATION.
 *
 */

/*
 * DiskSim Storage Subsystem Simulation Environment (Version 2.0)
 * Revision Authors: Greg Ganger
 * Contributors: Ross Cohen, John Griffin, Steve Schlosser
 *
 * Copyright (c) of Carnegie Mellon University, 1999.
 *
 * Permission to reproduce, use, and prepare derivative works of
 * this software for internal use is granted provided the copyright
 * and "No Warranty" statements are included with all reproductions
 * and derivative works. This software may also be redistributed
 * without charge provided that the copyright and "No Warranty"
 * statements are included in all redistributions.
 *
 * NO WARRANTY. THIS SOFTWARE IS FURNISHED ON AN "AS IS" BASIS.
 * CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED AS TO THE MATTER INCLUDING, BUT NOT LIMITED
 * TO: WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY
 * OF RESULTS OR RESULTS OBTAINED FROM USE OF THIS SOFTWARE. CARNEGIE
 * MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT
 * TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 */

/*
 * DiskSim Storage Subsystem Simulation Environment
 * Authors: Greg Ganger, Bruce Worthington, Yale Patt
 *
 * Copyright (C) 1993, 1995, 1997 The Regents of the University of Michigan
 *
 * This software is being provided by the copyright holders under the
 * following license. By obtaining, using and/or copying this software,
 * you agree that you have read, understood, and will comply with the
 * following terms and conditions:
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose and without fee or royalty is
 * hereby granted, provided that the full text of this NOTICE appears on
 * ALL copies of the software and documentation or portions thereof,
 * including modifications, that you make.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS," AND COPYRIGHT HOLDERS MAKE NO
 * REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED. BY WAY OF EXAMPLE,
 * BUT NOT LIMITATION, COPYRIGHT HOLDERS MAKE NO REPRESENTATIONS OR
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR
 * THAT THE USE OF THE SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY
 * THIRD PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS. COPYRIGHT
 * HOLDERS WILL BEAR NO LIABILITY FOR ANY USE OF THIS SOFTWARE OR
 * DOCUMENTATION.
 *
 *  This software is provided AS IS, WITHOUT REPRESENTATION FROM THE
 * UNIVERSITY OF MICHIGAN AS TO ITS FITNESS FOR ANY PURPOSE, AND
 * WITHOUT WARRANTY BY THE UNIVERSITY OF MICHIGAN OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE REGENTS
 * OF THE UNIVERSITY OF MICHIGAN SHALL NOT BE LIABLE FOR ANY DAMAGES,
 * INCLUDING SPECIAL , INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 * WITH RESPECT TO ANY CLAIM ARISING OUT OF OR IN CONNECTION WITH THE
 * USE OF OR IN CONNECTION WITH THE USE OF THE SOFTWARE, EVEN IF IT HAS
 * BEEN OR IS HEREAFTER ADVISED OF THE POSSIBILITY OF SUCH DAMAGES
 *
 * The names and trademarks of copyright holders or authors may NOT be
 * used in advertising or publicity pertaining to the software without
 * specific, written prior permission. Title to copyright in this software
 * and any associated documentation will at all times remain with copyright
 * holders.
 */

#ifndef DISKSIM_CACHEDEV_H
#define DISKSIM_CACHEDEV_H

#include "disksim_global.h"
#include "disksim_iosim.h"
#include "disksim_ioqueue.h"
#include "config.h"
#include "disksim_cache.h"

/* cache event types */

#define CACHE_EVENT_IOREQ 0
#define CACHE_EVENT_READ 1
#define CACHE_EVENT_POPULATE_ONLY 2
#define CACHE_EVENT_POPULATE_ALSO 3
#define CACHE_EVENT_WRITE 4
#define CACHE_EVENT_FLUSH 5
#define CACHE_EVENT_IDLEFLUSH_READ 6
#define CACHE_EVENT_IDLEFLUSH_FLUSH 7

/* cache write schemes */

#define CACHE_WRITE_MIN 1
#define CACHE_WRITE_SYNCONLY 1
#define CACHE_WRITE_THRU 2
#define CACHE_WRITE_BACK 3
#define CACHE_WRITE_MAX 3

/* cache background flush types */

#define CACHE_FLUSH_MIN 0
#define CACHE_FLUSH_DEMANDONLY 0
#define CACHE_FLUSH_PERIODIC 1
#define CACHE_FLUSH_MAX 1

#define DEFAULT_CACHE_ASSOC 512

#define DEFAULT_BLOCK_SIZE 8 /* 512 B */

// 策略
#define CACHE_FIFO 0
#define CACHE_LRU 1

// Lru的默认值
#define CACHE_LRU_NULL 0xFFFF

#define CACHE_LRU_HOT_PCT_DEFAULT 75

#define DIRTY_THRESH_MIN 10
#define DIRTY_THRESH_MAX 90
#define DIRTY_THRESH_DEF 90

#define MAX_CLEAN_IOS_TOTAL 4
#define MAX_CLEAN_IOS_SET 2

// cache块的定义
#define INVALID 0x0001
#define VALID 0x0002 /* Valid */
#define DIRTY 0x0040 /* Dirty, needs writeback to disk */

// Lru的状态
#define LRU_HOT 0x0001  /* On Hot LRU List */
#define LRU_WARM 0x0002 /* On Warm LRU List */

#define CACHE_FIFO 0
#define CACHE_LRU 1

#define NUM_BLOCK_HASH_BUCKETS 512

// cache块 有两个状态cache的状态，以及LRU的状态。
struct cacheblock {
    int cache_state;  // cache的状态,cache的状态，有效，无效，dirty，

    int lru_prev, lru_next;  // lru的前一个和后一个

    int blkno;      // 底层HDD的块号
    int lru_state;  // 维持的lru的状态

    int index;  // 对应的SSD的索引下标

    int hash_prev, hash_next;  // 无效链表的hash值
};

// cache_set的结构
struct cache_set {
    // 两个时间戳.
    int set_fifo_next;
    int set_clean_next;  // 清理set的索引
    int clean_inprog;    // 正在清理
    int nr_dirty;        // 脏块的数量
    int dirty_fallow;

    unsigned long fallow_tstamp;
    unsigned long fallow_next_cleaning;

    int hotlist_lru_head, hotlist_lru_tail;    // lRU的hot链表
    int warmlist_lru_head, warmlist_lru_tail;  // Lru的warm链表
    int lru_hot_blocks, lru_warm_blocks;

    int hash_buckets[NUM_BLOCK_HASH_BUCKETS];
    int invalid_head;  // 空闲链表
};

struct blkno_index_pair {
    int blkno;  // cache对应的扇区号
    int index;  // cache块的下标
};

struct cache_dev_event {
    double time;
    int type;
    struct cache_dev_event *next;
    struct cache_dev_event *prev;
    void (**donefunc)(void *, ioreq_event *); /* Function to call when complete */
    void *doneparam;                          /* parameter for donefunc */
    int flags;
    ioreq_event *req;
    struct cache_dev_event *waitees;
    int validpoint;
    int index;
    int blkno;
};

struct cache_dev_stats {
    int reads;
    int readblocks;
    int readhitsfull;
    int readmisses;
    int popwrites;
    int popwriteblocks;
    int writes;
    int writeblocks;
    int writehitsfull;
    int writemisses;
    int destagereads;
    int destagereadblocks;
    int destagewrites;
    int destagewriteblocks;
    int maxbufferspace;
    int uncachedRead;
    int uncacheWrite;
};

struct cache_dev {
    struct cache_if hdr;
    void (**issuefunc)(void *, ioreq_event *); /* to issue a disk access    */
    void *issueparam;                          /* first param for issuefunc */
    struct ioq *(**queuefind)(void *, int);    /* to get ioqueue ptr for dev*/
    void *queuefindparam;                      /* first param for queuefind */
    // void (**wakeupfunc)(void *,void *);	/* to re-activate slept proc */
    void (**wakeupfunc)(void *, struct cacheevent *); /* to re-activate slept proc */
    void *wakeupparam;                                /* first param for wakeupfunc */
    int size;                                         /* in 512B blks  */
    int cache_devno;                                  /* device used for cache */
    int real_devno;                                   /* device for which cache is used */
    int maxreqsize;
    int writescheme;
    int flush_policy;
    double flush_period;
    double flush_idledelay;
    int bufferspace;
    struct cache_dev_event *ongoing_requests;
    struct cache_dev_stats stat;
    char *name;
    int set_numbers;           // set的数量
    int cache_numbers;         // cache的数量
    struct cacheblock *caches; /* Hash table for cache blocks */
    struct cache_set *cache_sets;
    int dirty_thresh_set;                            /* Per set dirty threshold to start cleaning */
    int max_clean_ios_set;                           /* Max cleaning IOs per set */
    int max_clean_ios_total;                         /* Total max cleaning IOs */
    unsigned int assoc; /* Cache associativity */    // 每个cache集合的大小，有多少个块
    unsigned int block_size; /* Cache block size */  // 每个块有多少个扇区
    int lru_hot_pct;
    int lru_warm_blocks;
    int lru_hot_blocks;
    int cache_policy;  // 不同的缓存替换算法
};

void cachedev_setcallbacks(void);
void find_valid_blkno(struct cache_dev *cache, int blkno, int start_index, int *index);
int hash_block(struct cache_dev *cache, int blkno);
// void cache_reclaim_lru_movetail(struct cache_dev * cache, int index);
int find_invalid_blkno(struct cache_dev *cache, int set);
int cache_lookup(struct cache_dev *cache, int blkno, int *index);
void find_reclaim_blkno(struct cache_dev *cache, int start_index, int *index);
void cache_reclaim_fifo_get_old_block(struct cache_dev *cache, int start_index, int *index);
void cache_reclaim_lru_get_old_block(struct cache_dev *cache, int start_index, int *index);

void cache_inval_blocks(struct cache_dev *cache, int blkno, int bcount);

void cache_init_lru_lists(struct cache_dev *cache);
void cache_reclaim_add_block_to_list_lru(struct cache_dev *cache, int index);

void cache_invalid_insert(struct cache_dev *cache, int index);

void cache_invalid_remove(struct cache_dev *cache, int index);

void cache_lru_accessed(struct cache_dev *cache, int index);

void cache_reclaim_move_to_mru(struct cache_dev *cache, int index);
void cache_reclaim_remove_block_from_list(struct cache_dev *cache, int index);
void cache_reclaim_add_block_to_list_mru(struct cache_dev *cache, int index);
int cache_reclaim_promote_block(struct cache_dev *cache, int index);

int cache_reclaim_demote_block(struct cache_dev *cache, int index);
void cache_clean_set(struct cache_dev *cache, int set);

void cache_hash_remove(struct cache_dev *cache, int index);

int cache_hash_lookup(struct cache_dev *cache, int set, int blkno);

int *cache_get_hash_bucket(struct cache_dev *cache, struct cache_set *cache_set, int blkno);
void cache_hash_insert(struct cache_dev *cache, int index);
void cache_inval_block_set(struct cache_dev *cache, int start_set, int io_start);

#endif  // DISKSIM_CACHEDEV_H
