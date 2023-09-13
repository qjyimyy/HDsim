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

/***************************************************************************
  This is a fairly simple module for managing a device that is being used
  as a cache for another device.  It does not have many functionalities
  that a real device-caching-device would have, such as
        1. No actual "cache"-iness.  That is, it simply maps given locations
           to the same locations on the cache device.  This limits the max
           amount of real device space that can be cached, and misses
           opportunities for creating cache-device locality where none
           exists in the original workload.
        2. No prefetching.
        3. No optimization.  For example, see #1.  Also, no grouping of
           writes to cache-device.  Also, no going to original device when
           cache-device is busy.
        4. No locking.  Specifically, requests for the same space can be in
           progress in parallel.  This could cause inconsistencies, with
           unfortunate sequencing.
        5. No buffer space limitations.  Specifically, there is no limit on
           the amount of buffer space used at once for managing the cache
           and real devices.
***************************************************************************/
#define CACHE_DEVICE 2
#include "disksim_cachedev.h"

#include "modules/modules.h"

// 最大的请求，使用cache的块大小
static int cachedev_get_maxreqsize(struct cache_if *c) {
    struct cache_dev *cache = (struct cache_dev *)c;
    return cache->block_size;
}

static void cachedev_empty_donefunc(void *doneparam, ioreq_event *req) {
    addtoextraq((event *)req);
}

static void cachedev_add_ongoing_request(struct cache_dev *cache, void *crq) {
    struct cache_dev_event *cachereq = (struct cache_dev_event *)crq;
    cachereq->next = cache->ongoing_requests;
    cachereq->prev = NULL;
    if (cachereq->next) {
        cachereq->next->prev = cachereq;
    }
    cache->ongoing_requests = cachereq;
}

static void cachedev_remove_ongoing_request(struct cache_dev *cache, struct cache_dev_event *cachereq) {
    if (cachereq->next) {
        cachereq->next->prev = cachereq->prev;
    }
    if (cachereq->prev) {
        cachereq->prev->next = cachereq->next;
    }
    if (cache->ongoing_requests == cachereq) {
        cache->ongoing_requests = cachereq->next;
    }
}

static struct cache_dev_event *cachedev_find_ongoing_request(struct cache_dev *cache, ioreq_event *req) {
    struct cache_dev_event *tmp = cache->ongoing_requests;

    /* Is this enough to ensure equivalence?? */
    while ((tmp != NULL) && ((req->opid != tmp->req->opid) || (req->blkno != tmp->req->blkno) || (req->bcount != tmp->req->bcount) || (req->buf != tmp->req->buf))) {
        tmp = tmp->next;
    }

    return (tmp);
}

/* Return a count of how many dirty blocks are on the cachedev. */
static int cachedev_count_dirty_blocks(struct cache_dev *cache) {
    int dirtyblocks = 0;
    int i;
    for (i = 0; i < cache->cache_numbers; i++) {
        if (cache->caches[i].cache_state & DIRTY) {
            dirtyblocks++;
        }
    }
    return (dirtyblocks);
}

//
static int cachedev_isreadhit(struct cache_dev *cache, ioreq_event *req, int *index) {
    int res;
    struct cacheblock *cacheblk;

    res = cache_lookup(cache, req->blkno, index);

    if (res > 0) {
        cacheblk = &cache->caches[*index];
        if ((cacheblk->cache_state & VALID) && (cacheblk->blkno == req->blkno)) {
            cache->stat.readhitsfull++;
            return (1);
        }
    }

    cache->stat.readmisses++;
    // 请求相关的块，无效
    cache_inval_blocks(cache, req->blkno, req->bcount);
    // 不命中，重新执行读取

    if (cache->caches[*index].cache_state & VALID) {
        cache_hash_remove(cache, *index);
    }

    // 未命中的场景：
    cache->caches[*index].cache_state = VALID;
    // cache->caches[*index].cache_state |= DIRTY;

    cache->caches[*index].blkno = req->blkno;
    cache_hash_insert(cache, *index);
    cache_clean_set(cache, hash_block(cache, req->blkno));
    return (0);
}

static int cachedev_iswritehit(struct cache_dev *cache, ioreq_event *req, int *index) {
    int res;
    struct cacheblock *cacheblk;
    int queued;

    res = cache_lookup(cache, req->blkno, index);

    if (res != -1) {
        /* Cache Hit */
        cacheblk = &cache->caches[*index];

        if ((cacheblk->cache_state & VALID) && (cacheblk->blkno == req->blkno)) {
            // 写命中的场景
            cacheblk->cache_state |= DIRTY;
            cacheblk->blkno = req->blkno;

            cache->stat.writehitsfull++;

            cache_clean_set(cache, hash_block(cache, req->blkno));

            // 都是写命中
            return (1);
        } else {
            /*如果是ditry的话要执行下刷的逻辑*/
            /* Cache Miss, found block to recycle */

            cache_inval_blocks(cache, req->blkno, req->bcount);

            if (cacheblk->cache_state & VALID) {
                cache_hash_remove(cache, *index);
            }

            cacheblk->cache_state = VALID;
            cacheblk->blkno = req->blkno;

            cache_hash_insert(cache, *index);
            cacheblk->cache_state |= DIRTY;
            cache->stat.writemisses++;
            cache_clean_set(cache, hash_block(cache, req->blkno));
            return 1;
        }
    }

    cache_clean_set(cache, hash_block(cache, req->blkno));
    return (0);
}

//
static int cachedev_find_dirty_cache_blocks(struct cache_dev *cache, int *blknoPtr, int *bcountPtr) {
    int index;
    int i = 0;
    for (i = 0; i < cache->cache_numbers; i++) {
        if (cache->caches[i].cache_state & DIRTY) {
            *blknoPtr = cache->caches[i].index * cache->block_size;
            break;
        }
    }
    if (*blknoPtr == -1) {
        return (0);
    }
    *bcountPtr = cache->block_size;
    return (1);
}

static void cachedev_periodic_callback(timer_event *timereq) {
    fprintf(stderr, "cachedev_periodic_callback not yet supported\n");
    ASSERT(0);
    exit(0);
}

static void cachedev_idlework_callback(void *idleworkparam, int idledevno) {
    struct cache_dev *cache = (struct cache_dev *)idleworkparam;
    struct cache_dev_event *flushdesc;
    ioreq_event *flushreq;
    int blkno, bcount;
    struct ioq *queue;

    ASSERT(idledevno == cache->real_devno);

    queue = (*cache->queuefind)(cache->queuefindparam, cache->real_devno);
    if (ioqueue_get_number_in_queue(queue) != 0) {
        return;
    }

    queue = (*cache->queuefind)(cache->queuefindparam, cache->cache_devno);
    if (ioqueue_get_number_in_queue(queue) != 0) {
        return;
    }

    if (cachedev_find_dirty_cache_blocks(cache, &blkno, &bcount) == 0) {
        return;
    }

    /* Just assume that bufferspace is available */
    cache->bufferspace += bcount;
    if (cache->bufferspace > cache->stat.maxbufferspace) {
        cache->stat.maxbufferspace = cache->bufferspace;
    }

    flushdesc = (struct cache_dev_event *)getfromextraq();
    flushdesc->type = CACHE_EVENT_IDLEFLUSH_READ;
    flushdesc->index = blkno / cache->block_size;
    flushdesc->blkno = blkno;
    flushreq = (ioreq_event *)getfromextraq();
    flushreq->buf = flushdesc;
    flushreq->devno = cache->cache_devno;
    flushreq->blkno = blkno;
    flushreq->bcount = bcount;

    flushreq->type = IO_ACCESS_ARRIVE;
    flushreq->flags = READ;
    (*cache->issuefunc)(cache->issueparam, flushreq);
    cache->stat.destagereads++;
    cache->stat.destagereadblocks += bcount;
}

/* Gets the appropriate block, locked and ready to be accessed read or write */
// 访问读写
static int cachedev_get_block(struct cache_if *c, ioreq_event *req, void (**donefunc)(void *, ioreq_event *), void *doneparam) {
    struct cache_dev *cache = (struct cache_dev *)c;
    struct cache_dev_event *rwdesc = (struct cache_dev_event *)getfromextraq();
    ioreq_event *fillreq;
    int devno;
    int index = -1;
    // fprintf (outputfile, "totalreqs = %d\n", disksim->totalreqs);
    // fprintf (outputfile, "%.5f: Entered cache_get_block: rw %d, devno %d, blkno %d, size %d\n", simtime, (req->flags & READ), req->devno, req->blkno, req->bcount);

    if (req->devno != cache->real_devno) {
        fprintf(stderr, "cachedev_get_block trying to cache blocks for wrong device (%d should be %d)\n", req->devno, cache->real_devno);
        ASSERT(0);
        exit(1);
    }

    /* Ignore request overlap and locking issues for now.  */
    /* Also ignore buffer space limitation issues for now. */
    cache->bufferspace += req->bcount;
    if (cache->bufferspace > cache->stat.maxbufferspace) {
        cache->stat.maxbufferspace = cache->bufferspace;
    }

    rwdesc->type = (req->flags & READ) ? CACHE_EVENT_READ : CACHE_EVENT_WRITE;
    rwdesc->donefunc = donefunc;
    rwdesc->doneparam = doneparam;

    // 大概懂了，会在cache上面挂载一个rw请求，再将原来的请求挂上去。
    rwdesc->req = req;
    req->next = NULL;
    req->prev = NULL;
    rwdesc->flags = 0;
    cachedev_add_ongoing_request(cache, rwdesc);

    // 不对齐的写或者是写的bcount数量比较少的情况下
    if (req->flags & READ) {
        if (req->blkno % cache->block_size != 0 || req->bcount < cache->block_size) {
            cache_inval_blocks(cache, req->blkno, req->bcount);
            if (req->flags & READ) {
                cache->stat.uncachedRead++;
                cache->stat.reads++;
                rwdesc->blkno = req->blkno;
                rwdesc->index = index;
                /* For now, just assume both device's store bits at same LBNs */
                fillreq = ioreq_copy(req);
                // fillreq的buf填充的原来的请求。
                fillreq->buf = rwdesc;
                fillreq->type = IO_ACCESS_ARRIVE;
                fillreq->devno = cache->real_devno;
                // fprintf (outputfile, "fillreq: devno %d, blkno %d, buf %p\n", fillreq->devno, fillreq->blkno, fillreq->buf);
                // 这里调用 controller_smart_issue_access，访问下面的设备cache_free_block_clean
                // 第一个参数是cltr结构体，第二个是传递的请求
                (*cache->issuefunc)(cache->issueparam, fillreq);
                return (1);
            } else {
                // controller_smart_host_data_transfer
                (*donefunc)(doneparam, req);
                return (0);
            }
        }
    }

    if (req->flags & READ) {
        cache->stat.reads++;
        cache->stat.readblocks += req->bcount;
        /* Send read straight to whichever device has it (preferably cachedev). */

        if (cachedev_isreadhit(cache, req, &index)) {
            // cache命中 和 未命中的判定
            devno = cache->cache_devno;

        } else {
            devno = cache->real_devno;
        }

        rwdesc->blkno = req->blkno;
        rwdesc->index = index;
        /* For now, just assume both device's store bits at same LBNs */
        fillreq = ioreq_copy(req);
        // fillreq的buf填充的原来的请求。
        fillreq->buf = rwdesc;
        fillreq->type = IO_ACCESS_ARRIVE;
        fillreq->devno = devno;
        if (devno == cache->cache_devno) {
            fillreq->blkno = rwdesc->index * cache->block_size;
        }
        // fprintf (outputfile, "fillreq: devno %d, blkno %d, buf %p\n", fillreq->devno, fillreq->blkno, fillreq->buf);
        // 这里调用 controller_smart_issue_access，访问下面的设备cache_free_block_clean
        // 第一个参数是cltr结构体，第二个是传递的请求
        (*cache->issuefunc)(cache->issueparam, fillreq);

        return (1);
    } else {
        /* Grab buffer space and let the controller fill in data to be written. */
        /* (for now, just assume that there is buffer space available)          */
        // 调用它，传输数据完成
        // controller_smart_host_data_transfer

        (*donefunc)(doneparam, req);
        return (0);
    }
}

// 读请求完成会调用cache_free_block_clean,清理掉，悬挂的请求。
/* frees the block after access complete, block is clean so remove locks */
/* and update lru                                                        */
static void cachedev_free_block_clean(struct cache_if *c, ioreq_event *req) {
    struct cache_dev *cache = (struct cache_dev *)c;
    struct cache_dev_event *rwdesc;

    // fprintf (outputfile, "%.5f: Entered cache_free_block_clean: blkno %d, bcount %d, devno %d\n", simtime, req->blkno, req->bcount, req->devno);

    /* For now, just find relevant rwdesc and free it.                       */
    /* Later, write it to the cache device (and update the cache map thusly. */

    rwdesc = cachedev_find_ongoing_request(cache, req);
    // 找到相应的cache读写请求
    ASSERT(rwdesc != NULL);

    if (rwdesc->type == CACHE_EVENT_READ) {
        cache->bufferspace -= req->bcount;
        cachedev_remove_ongoing_request(cache, rwdesc);
        addtoextraq((event *)rwdesc);
    } else {
        ASSERT(rwdesc->type == CACHE_EVENT_POPULATE_ALSO);
        // cache写请求
        rwdesc->type = CACHE_EVENT_POPULATE_ONLY;
    }
}

/* a delayed write - set dirty bits, remove locks and update lru.        */
/* If cache doesn't allow delayed writes, forward this to async          */

static int cachedev_free_block_dirty(struct cache_if *c, ioreq_event *req, void (**donefunc)(void *, ioreq_event *), void *doneparam) {
    struct cache_dev *cache = (struct cache_dev *)c;
    ioreq_event *flushreq;
    struct cache_dev_event *writedesc;
    int index = -1;
    // fprintf (outputfile, "%.5f, Entered cache_free_block_dirty: blkno %d, size %d\n", simtime, req->blkno, req->bcount);

    writedesc = cachedev_find_ongoing_request(cache, req);

    ASSERT(writedesc != NULL);
    ASSERT(writedesc->type == CACHE_EVENT_WRITE);

    writedesc->donefunc = donefunc;
    writedesc->doneparam = doneparam;
    writedesc->req = req;
    // 这里再对下面的设备访问
    req->type = IO_REQUEST_ARRIVE;
    req->next = NULL;
    req->prev = NULL;
    /* For now, just assume both device's store bits at same LBNs */
    flushreq = ioreq_copy(req);
    flushreq->type = IO_ACCESS_ARRIVE;
    flushreq->buf = writedesc;

    if (flushreq->blkno % cache->block_size != 0 || flushreq->bcount < cache->block_size) {
        cache->stat.writes++;
        cache->stat.uncacheWrite++;
        cache_inval_blocks(cache, flushreq->blkno, flushreq->bcount);
        flushreq->devno = cache->real_devno;
        writedesc->blkno = req->blkno;
        flushreq->blkno = req->blkno;
        (*cache->issuefunc)(cache->issueparam, flushreq);
        return 1;
    }

    if (cachedev_iswritehit(cache, req, &index)) {
        flushreq->devno = cache->cache_devno;
    }
    cache->stat.writes++;
    cache->stat.writeblocks += req->bcount;
    // flushreq->devno = cache->cache_devno;
    writedesc->blkno = req->blkno;
    writedesc->index = index;
    flushreq->blkno = writedesc->index * cache->block_size;
    // fprintf (outputfile, "flushreq: devno %d, blkno %d, buf %p\n", flushreq->devno, flushreq->blkno, flushreq->buf);
    // controller_smart_issue_access
    (*cache->issuefunc)(cache->issueparam, flushreq);

#if 0
   if (cache->flush_idledelay >= 0.0) {
      ioqueue_reset_idledetecter((*cache->queuefind)(cache->queuefindparam, req->devno), 0);
   }
#endif

    return (1);
}

int cachedev_sync(struct cache_if *c) {
    return (0);
}

// wakeup会调用这个事件
// controller完成事件之后的，回调cacheqing请求访问完成
static void *cachedev_disk_access_complete(struct cache_if *c,
                                           ioreq_event *curr) {
    // rw存储cache的读写类型
    struct cache_dev *cache = (struct cache_dev *)c;
    // 这里会将原始的请求拿出来,curr->buf
    struct cache_dev_event *rwdesc = (struct cache_dev_event *)curr->buf;
    struct cache_dev_event *tmp = NULL;

    // fprintf (outputfile, "Entered cache_disk_access_complete: blkno %d, bcount %d, devno %d, buf %p\n", curr->blkno, curr->bcount, curr->devno, curr->buf);
    switch (rwdesc->type) {
        case CACHE_EVENT_READ:
            /* Consider writing same buffer to cache_devno, in order to populate it.*/
            /* Not clear whether it is more appropriate to do it from here or from  */
            /* "free_block_clean" -- do it here for now to get more overlap.        */

            if (curr->devno == cache->real_devno) {
                // 这里是不对齐写和小写的特殊处理
                if (curr->blkno % cache->block_size == 0 && curr->bcount == cache->block_size) {
                    // 开始写cache
                    ioreq_event *flushreq = ioreq_copy(rwdesc->req);
                    flushreq->type = IO_ACCESS_ARRIVE;
                    // flushreq中的buf存了原来的请求。
                    flushreq->buf = rwdesc;
                    flushreq->flags = WRITE;
                    flushreq->devno = cache->cache_devno;
                    rwdesc->type = CACHE_EVENT_POPULATE_ALSO;
                    // controller_smart_issue_access访问
                    // 写ssd的块号 要更新一下
                    flushreq->blkno = rwdesc->index * cache->block_size;
                    // controller_smart_issue_access
                    (*cache->issuefunc)(cache->issueparam, flushreq);
                    cache->stat.popwrites++;
                    cache->stat.popwriteblocks += rwdesc->req->bcount;
                }
            }
            /* Ongoing read request can now proceed, so call donefunc from get_block*/
            // 调用this 传输完成请求 controller_smart_host_data_transfer_complete
            // controller_smart_host_data_transfer
            // controller_smart_host_data_transfer
            // 发回去 完成请求
            (*rwdesc->donefunc)(rwdesc->doneparam, rwdesc->req);

            break;

        case CACHE_EVENT_WRITE:
            /* finished writing to cache-device */
            if (curr->devno == cache->cache_devno) {
                if (curr->blkno % cache->block_size == 0 && curr->bcount == cache->block_size) {
                    if (cache->writescheme == CACHE_WRITE_THRU) {
                        ioreq_event *flushreq = ioreq_copy(rwdesc->req);
                        flushreq->type = IO_ACCESS_ARRIVE;
                        flushreq->blkno = rwdesc->blkno;
                        flushreq->buf = rwdesc;
                        flushreq->flags = WRITE;
                        flushreq->devno = cache->real_devno;
                        rwdesc->type = CACHE_EVENT_FLUSH;
                        (*cache->issuefunc)(cache->issueparam, flushreq);
                        cache->stat.destagewrites++;
                        cache->stat.destagewriteblocks += rwdesc->req->bcount;
                    }
                }
            }

            // 这里会将完成的请求发送回去
            // controller_smart_request_complete
            (*rwdesc->donefunc)(rwdesc->doneparam, rwdesc->req);

            // 写请求删了
            if (rwdesc->type != CACHE_EVENT_FLUSH) {
                cachedev_remove_ongoing_request(cache, rwdesc);
                addtoextraq((event *)rwdesc);
                cache->bufferspace -= curr->bcount;
            }

            break;

        case CACHE_EVENT_POPULATE_ONLY:
            // 写cache完成
            cachedev_remove_ongoing_request(cache, rwdesc);
            addtoextraq((event *)rwdesc);
            cache->bufferspace -= curr->bcount;
            break;

        case CACHE_EVENT_POPULATE_ALSO:
            rwdesc->type = CACHE_EVENT_READ;
            break;

        case CACHE_EVENT_FLUSH:
            cache->caches[rwdesc->index].cache_state = VALID;
            cachedev_remove_ongoing_request(cache, rwdesc);
            addtoextraq((event *)rwdesc);
            cache->bufferspace -= curr->bcount;
            break;

        case CACHE_EVENT_IDLEFLUSH_READ: {
            cache->caches[rwdesc->index].cache_state = VALID;
            ioreq_event *flushreq = ioreq_copy(curr);
            flushreq->type = IO_ACCESS_ARRIVE;
            flushreq->flags = WRITE;
            flushreq->devno = cache->real_devno;
            flushreq->blkno = cache->caches[rwdesc->index].blkno;
            rwdesc->type = CACHE_EVENT_IDLEFLUSH_FLUSH;
            (*cache->issuefunc)(cache->issueparam, flushreq);
            cache->stat.destagewrites++;
            cache->stat.destagewriteblocks += curr->bcount;
        } break;

        case CACHE_EVENT_IDLEFLUSH_FLUSH:
            cache->caches[rwdesc->index].cache_state = INVALID;
            cachedev_remove_ongoing_request(cache, rwdesc);
            addtoextraq((event *)rwdesc);
            cachedev_idlework_callback(cache, curr->devno);
            cache->bufferspace -= curr->bcount;
            break;

        default:
            ddbg_assert2(0, "Unknown cachedev event type");
            break;
    }

    addtoextraq((event *)curr);

    /* returned cacheevent will get forwarded to cachedev_wakeup_continue... */
    return (tmp);
}

static void
cachedev_wakeup_complete(struct cache_if *c, void *d) {  // really struct cache_dev_event
    struct cache_dev_event *desc = (struct cache_dev_event *)d;
    struct cache_dev *cache = (struct cache_dev *)c;
    ASSERT(0);

    // ???

#if 0
   switch(desc->type) {
   case CACHE_EVENT_READ:
      cache_read_continue(cache, desc);
      break;
   case CACHE_EVENT_WRITE:
     cache_write_continue(cache, desc);
     break;
   case CACHE_EVENT_FLUSH:
      (*desc->donefunc)(desc->doneparam, desc->req);
      addtoextraq((event *) desc);
      break;

   default:
     ddbg_assert2(0, "Unknown cachedev event type");
     break;
   }
#endif
}

// reset开始：
static void cachedev_resetstats(struct cache_if *c) {
    int i, j;
    struct cache_dev *cache = (struct cache_dev *)c;
    cache->stat.reads = 0;
    cache->stat.readblocks = 0;
    cache->stat.readhitsfull = 0;
    cache->stat.readmisses = 0;
    cache->stat.popwrites = 0;
    cache->stat.popwriteblocks = 0;
    cache->stat.writes = 0;
    cache->stat.writeblocks = 0;
    cache->stat.writehitsfull = 0;
    cache->stat.writemisses = 0;
    cache->stat.destagereads = 0;
    cache->stat.destagereadblocks = 0;
    cache->stat.destagewrites = 0;
    cache->stat.destagewriteblocks = 0;
    cache->stat.maxbufferspace = 0;
    cache->stat.uncachedRead = 0;
    cache->stat.uncacheWrite = 0;

    cache->lru_hot_blocks = 0;
    cache->lru_warm_blocks = 0;
    // hot的比例

    cache->lru_hot_pct = CACHE_LRU_HOT_PCT_DEFAULT;

    // 将所有的cache进行初始化
    for (i = 0; i < cache->set_numbers; i++) {
        cache->cache_sets[i].set_fifo_next = i * cache->assoc;
        cache->cache_sets[i].set_clean_next = i * cache->assoc;

        cache->cache_sets[i].fallow_tstamp = 0;
        cache->cache_sets[i].fallow_next_cleaning = 0;
        // clean的头部就是从头开始
        cache->cache_sets[i].nr_dirty = 0;
        cache->cache_sets[i].clean_inprog = 0;

        // hot链表的头部
        cache->cache_sets[i].hotlist_lru_head = CACHE_LRU_NULL;
        cache->cache_sets[i].hotlist_lru_tail = CACHE_LRU_NULL;
        // Lru温链表的实现
        cache->cache_sets[i].warmlist_lru_head = CACHE_LRU_NULL;
        cache->cache_sets[i].warmlist_lru_tail = CACHE_LRU_NULL;
        // 无效链表和hash桶
        cache->cache_sets[i].invalid_head = CACHE_LRU_NULL;
        cache->cache_sets[i].lru_hot_blocks = 0;
        cache->cache_sets[i].lru_warm_blocks = 0;

        for (j = 0; j < NUM_BLOCK_HASH_BUCKETS; j++) {
            cache->cache_sets[i].hash_buckets[j] = CACHE_LRU_NULL;
        }
    }

    // 初始化Lru list块的状态
    cache_init_lru_lists(cache);
    // 将所有的cache中一个set排列起来，组成一个lru free
    /* Push all blocks into the set specific LRUs */
    for (i = 0; i < cache->cache_numbers; i++) {
        cache->caches[i].cache_state = INVALID;
        cache->caches[i].index = i;
        cache->caches[i].blkno = 0;
        // cache_reclaim_lru_movetail(cache, i);
        // 每一个插入到无效链表里面
        cache->caches[i].hash_next = CACHE_LRU_NULL;
        cache->caches[i].hash_prev = CACHE_LRU_NULL;
        cache_invalid_insert(cache, i);
    }

    /* Cleaning Thresholds */
    cache->dirty_thresh_set = (cache->assoc * DIRTY_THRESH_DEF) / 100;
    // dirty块占据百分之20的时候开始
    cache->max_clean_ios_total = MAX_CLEAN_IOS_TOTAL;
    cache->max_clean_ios_set = MAX_CLEAN_IOS_SET;
}

// cache设置回调
void cachedev_setcallbacks(void) {
    disksim->donefunc_cachedev_empty = cachedev_empty_donefunc;
    disksim->idlework_cachedev = cachedev_idlework_callback;
    disksim->timerfunc_cachedev = cachedev_periodic_callback;
}

static void cachedev_initialize(struct cache_if *c,
                                void (**issuefunc)(void *, ioreq_event *),
                                void *issueparam,
                                struct ioq *(**queuefind)(void *, int),
                                void *queuefindparam,
                                void (**wakeupfunc)(void *, struct cacheevent *),
                                void *wakeupparam,
                                int numdevs) {
    struct cache_dev *cache = (struct cache_dev *)c;
    StaticAssert(sizeof(struct cache_dev_event) <= DISKSIM_EVENT_SIZE);

    cache->issuefunc = issuefunc;
    cache->issueparam = issueparam;
    cache->queuefind = queuefind;
    cache->queuefindparam = queuefindparam;
    cache->wakeupfunc = wakeupfunc;
    cache->wakeupparam = wakeupparam;
    cache->bufferspace = 0;
    cache->ongoing_requests = NULL;

    cachedev_resetstats(c);

    if (cache->flush_idledelay) {
        struct ioq *queue = (*queuefind)(queuefindparam, cache->real_devno);
        ASSERT(queue != NULL);
        ioqueue_set_idlework_function(queue,
                                      &disksim->idlework_cachedev,
                                      cache,
                                      cache->flush_idledelay);
    }

    if (device_get_number_of_blocks(cache->cache_devno) < cache->size) {
        fprintf(stderr, "Size of cachedev exceeds that of actual cache device (devno %d): %d > %d\n", cache->cache_devno, cache->size, device_get_number_of_blocks(cache->cache_devno));
        ddbg_assert(0);
    }
}

static void
cachedev_cleanstats(struct cache_if *cache) {
}

static void
cachedev_printstats(struct cache_if *c, char *prefix) {
    struct cache_dev *cache = (struct cache_dev *)c;
    int reqs = cache->stat.reads + cache->stat.writes;
    int blocks = cache->stat.readblocks + cache->stat.writeblocks;

    fprintf(outputfile, "%scache requests:             %6d\n", prefix, reqs);
    if (reqs == 0) {
        return;
    }

    fprintf(outputfile, "%scache hit:  %6.4f   cache miss:   %6.4f \n", prefix, (((double)cache->stat.readhitsfull + (double)cache->stat.writehitsfull) / (double)reqs), (((double)cache->stat.readmisses + (double)cache->stat.writemisses + (double)cache->stat.uncacheWrite + (double)cache->stat.uncachedRead) / (double)reqs));

    fprintf(outputfile, "%scache read requests:        %6d  \t%6.4f\n", prefix, cache->stat.reads, ((double)cache->stat.reads / (double)reqs));

    if (cache->stat.reads) {
        fprintf(outputfile, "%scache blocks read:           %6d  \t%6.4f\n", prefix, cache->stat.readblocks, ((double)cache->stat.readblocks / (double)blocks));
        fprintf(outputfile, "%scache read misses:          %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.readmisses, ((double)cache->stat.readmisses / (double)reqs), ((double)cache->stat.readmisses / (double)cache->stat.reads));

        fprintf(outputfile, "%scache read full hits:       %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.readhitsfull, ((double)cache->stat.readhitsfull / (double)reqs), ((double)cache->stat.readhitsfull / (double)cache->stat.reads));

        fprintf(outputfile, "%scache population writes:         %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.popwrites, ((double)cache->stat.popwrites / (double)reqs), ((double)cache->stat.popwrites / (double)cache->stat.reads));

        fprintf(outputfile, "%scache block population writes:    %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.popwriteblocks, ((double)cache->stat.popwriteblocks / (double)blocks), ((double)cache->stat.popwriteblocks / (double)cache->stat.readblocks));
    }

    fprintf(outputfile, "%s Readreq  %6d  cached read:      %6d  uncached read: %6d  uncached read ratio: %6.4f   \n", prefix, cache->stat.reads, cache->stat.reads - cache->stat.uncachedRead, cache->stat.uncachedRead, (double)cache->stat.uncachedRead / (double)cache->stat.reads);
    fprintf(outputfile, "%s Writereq  %6d  cached write:      %6d  uncached write: %6d   uncached write ratio: %6.4f\n", prefix, cache->stat.writes, cache->stat.writes - cache->stat.uncacheWrite, cache->stat.uncacheWrite, (double)cache->stat.uncacheWrite / (double)cache->stat.writes);

    fprintf(outputfile, "%scache write requests:       %6d  \t%6.4f\n", prefix, cache->stat.writes, ((double)cache->stat.writes / (double)reqs));

    if (cache->stat.writes) {
        fprintf(outputfile, "%scache blocks written:        %6d  \t%6.4f\n", prefix, cache->stat.writeblocks, ((double)cache->stat.writeblocks / (double)blocks));
        fprintf(outputfile, "%scache write misses:         %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.writemisses, ((double)cache->stat.writemisses / (double)reqs), ((double)cache->stat.writemisses / (double)cache->stat.writes));

        fprintf(outputfile, "%scache full write hits:   %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.writehitsfull, ((double)cache->stat.writehitsfull / (double)reqs), ((double)cache->stat.writehitsfull / (double)cache->stat.writes));

        fprintf(outputfile, "%scache destage pre-reads:     %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.destagereads, ((double)cache->stat.destagereads / (double)reqs), ((double)cache->stat.destagereads / (double)cache->stat.writes));
        fprintf(outputfile, "%scache block destage pre-reads: %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.destagereadblocks, ((double)cache->stat.destagereadblocks / (double)blocks), ((double)cache->stat.destagereadblocks / (double)cache->stat.writeblocks));

        fprintf(outputfile, "%scache destages (write):     %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.destagewrites, ((double)cache->stat.destagewrites / (double)reqs), ((double)cache->stat.destagewrites / (double)cache->stat.writes));

        fprintf(outputfile, "%scache block destages (write): %6d  \t%6.4f  \t%6.4f\n", prefix, cache->stat.destagewriteblocks, ((double)cache->stat.destagewriteblocks / (double)blocks), ((double)cache->stat.destagewriteblocks / (double)cache->stat.writeblocks));

        fprintf(outputfile, "%scache end dirty blocks:      %6d  \t%6.4f\n", prefix, cachedev_count_dirty_blocks(cache), ((double)cachedev_count_dirty_blocks(cache) / (double)cache->stat.writeblocks));
    }

    fprintf(outputfile, "%scache bufferspace use end:             %6d\n", prefix, cache->bufferspace);

    fprintf(outputfile, "%scache bufferspace use max:             %6d\n", prefix, cache->stat.maxbufferspace);
}

static struct cache_if *
cachedev_copy(struct cache_if *c) {
    struct cache_dev *cache = (struct cache_dev *)c;
    struct cache_dev *newCache = (struct cache_dev *)DISKSIM_malloc(sizeof(struct cache_dev));

    ASSERT(newCache != NULL);

#ifdef DEBUG_CACHEDEV
    fprintf(outputfile, "*** %f: Entered cachedev::cachedev_copy\n", simtime);
#endif

    bzero(newCache, sizeof(struct cache_dev));

    newCache->issuefunc = cache->issuefunc;
    newCache->issueparam = cache->issueparam;
    newCache->queuefind = cache->queuefind;
    newCache->queuefindparam = cache->queuefindparam;
    newCache->wakeupfunc = cache->wakeupfunc;
    newCache->wakeupparam = cache->wakeupparam;
    newCache->size = cache->size;
    newCache->maxreqsize = cache->maxreqsize;

    return (struct cache_if *)newCache;
}

static struct cache_if disksim_cache_dev = {
    cachedev_setcallbacks,
    cachedev_initialize,
    cachedev_resetstats,
    cachedev_printstats,
    cachedev_cleanstats,
    cachedev_copy,
    cachedev_get_block,
    cachedev_free_block_clean,
    cachedev_free_block_dirty,
    cachedev_disk_access_complete,
    cachedev_wakeup_complete,
    cachedev_sync,
    cachedev_get_maxreqsize
};

struct cache_if *disksim_cachedev_loadparams(struct lp_block *b) {
    int c;
    struct cache_dev *result;

    result = (struct cache_dev *)calloc(1, sizeof(struct cache_dev));
    result->hdr = disksim_cache_dev;

    result->name = b->name ? strdup(b->name) : 0;

    // #include "modules/disksim_cachedev_param.c"
    lp_loadparams(result, b, &disksim_cachedev_mod);

    return (struct cache_if *)result;
}

int hash_block(struct cache_dev *cache, int blkno) {
    // 一个set有多少个cache块，一个块有多少个扇区
    return (blkno / (cache->assoc * cache->block_size)) % cache->set_numbers;
}

// 查找一个有效的块号：
void find_valid_blkno(struct cache_dev *cache, int blkno, int start_index, int *index) {
    *index = cache_hash_lookup(cache, start_index / cache->assoc, blkno);

    if (*index == -1) {
        return;
    }
    if (cache->cache_policy == CACHE_LRU) {
        cache_lru_accessed(cache, *index);
    }
}

// 未命中，查找无效的 没问题
int find_invalid_blkno(struct cache_dev *cache, int set) {
    struct cache_set *cache_set;
    int index;
    struct cacheblock *cacheblk;

    cache_set = &cache->cache_sets[set];

    index = cache_set->invalid_head;

    if (index == CACHE_LRU_NULL)
        return -1;

    index += (set * cache->assoc);

    cacheblk = &cache->caches[index];
    // 从freelist中剔除当前的cache块。
    cache_invalid_remove(cache, index);

    if (index != -1) {
        if (cache->cache_policy == CACHE_LRU) {
            cache_lru_accessed(cache, index);
        }
    }
    return index;
}

/*
int find_invalid_blkno(struct cache_dev  *cache, int start_index)
{
        int i;
        int end_index = start_index + cache->assoc;
        for (i = start_index; i < end_index; i++) {
                if (cache->caches[i].cache_state == INVALID) {
                        cache_reclaim_lru_movetail(cache, i);
                        return i;
                }
        }
        return -1;
}
*/

// 查找命中情况
int cache_lookup(struct cache_dev *cache, int blkno, int *index) {
    // 取一个hash得到set编号
    unsigned long set_number = hash_block(cache, blkno);

    int invalid, oldest_clean = -1;
    int start_index;

    // cache块的start 索引
    start_index = cache->assoc * set_number;

    find_valid_blkno(cache, blkno, start_index, index);

    if (*index > 0) {
        return VALID;
    }

    // 查找无效的块
    invalid = find_invalid_blkno(cache, set_number);

    if (invalid == -1) {
        /* We didn't find an invalid entry, search for oldest valid entry */
        find_reclaim_blkno(cache, start_index, &oldest_clean);
    }

    /*
     * Cache miss :
     * We can't choose an entry marked INPROG, but choose the oldest
     * INVALID or the oldest VALID entry.
     */

    *index = start_index + cache->assoc;
    if (invalid != -1) {
        *index = invalid;
    } else if (oldest_clean != -1) {
        *index = oldest_clean;
    } else {
    }

    // index 记录替换下来的索引，返回为有效或者是无效
    if (*index < (start_index + cache->assoc))
        return INVALID;
    else {
        return -1;
    }
}

void find_reclaim_blkno(struct cache_dev *cache, int start_index, int *index) {
    // 这里使用FiFo来作为替换策略
    if (cache->cache_policy == CACHE_FIFO) {
        cache_reclaim_fifo_get_old_block(cache, start_index, index);
    } else {
        cache_reclaim_lru_get_old_block(cache, start_index, index);
    }
}

// FIFO来回收块
void cache_reclaim_fifo_get_old_block(struct cache_dev *cache, int start_index, int *index) {
    int set = start_index / cache->assoc;
    struct cache_set *cache_set = &cache->cache_sets[set];
    int end_index = start_index + cache->assoc;

    int slots_searched = 0;
    int i;

    i = cache_set->set_fifo_next;

    while (slots_searched < cache->assoc) {
        if (cache->caches[i].cache_state == VALID) {
            *index = i;
            break;
        }
        slots_searched++;
        i++;
        if (i == end_index) {
            i = start_index;
        }
    }
    i++;
    if (i == end_index) {
        i = start_index;
    }
    cache_set->set_fifo_next = i;
}

void cache_reclaim_lru_get_old_block(struct cache_dev *cache, int start_index, int *index) {
    int lru_rel_index;

    struct cacheblock *cacheblk;

    int set = start_index / cache->assoc;

    struct cache_set *cache_set = &cache->cache_sets[set];

    *index = -1;

    lru_rel_index = cache_set->warmlist_lru_head;

    while (lru_rel_index != CACHE_LRU_NULL) {
        cacheblk = &cache->caches[lru_rel_index + start_index];
        // 回收一个cache块
        if (cacheblk->cache_state == VALID) {
            *index = cacheblk - &cache->caches[0];
            cache_reclaim_move_to_mru(cache, *index);
            break;
        }
        lru_rel_index = cacheblk->lru_next;
    }

    if (*index != -1)
        return;
    /*
     * We did not find a block on the "warm" LRU list that we could take, pick
     * a block from the "hot" LRU list.
     */

    lru_rel_index = cache_set->hotlist_lru_head;
    while (lru_rel_index != CACHE_LRU_NULL) {
        cacheblk = &cache->caches[lru_rel_index + start_index];
        // 这里改一下
        if (cacheblk->cache_state == VALID) {
            *index = cacheblk - &cache->caches[0];
            /*
             * Swap this block with the MRU block in the warm list.
             * To maintain equilibrium between the lists
             * 1) We put this block in the MRU position on the warm list
             * 2) Remove the block in the LRU position on the warm list and
             * 3) Move that block to the LRU position on the hot list.
             */
            if (!cache_reclaim_demote_block(cache, *index)) {
                /*
                 * We cannot demote this block to the warm list
                 * just move it to the MRU position.
                 */
                cache_reclaim_move_to_mru(cache, *index);
            }
            break;
        }
        lru_rel_index = cacheblk->lru_next;
    }
}

void cache_inval_blocks(struct cache_dev *cache, int blkno, int bcount) {
    int io_start = blkno / cache->block_size * cache->block_size;
    ;
    int start_set = hash_block(cache, io_start);
    // 4KB 粒度对齐。
    cache_inval_block_set(cache, start_set, io_start);
}

void cache_inval_block_set(struct cache_dev *cache, int start_set, int io_start) {
    int index;
    struct cacheblock *cacheblk;

    index = cache_hash_lookup(cache, start_set, io_start);

    if (index == -1) {
        return;
    }
    cacheblk = &cache->caches[index];

    if (!(cacheblk->cache_state & DIRTY)) {
        cache_hash_remove(cache, index);
        cacheblk->cache_state = INVALID;
        cache_invalid_insert(cache, index);
        return;
    }
    if (cacheblk->cache_state & DIRTY) {
        cacheblk->cache_state = VALID;
        return;
    }
    return;
}

// lru链表的初始化
void cache_init_lru_lists(struct cache_dev *cache) {
    int start_index = 0;
    int i = 0;
    int j = 0;
    int block_index = 0;
    // hot块的个数

    int hot_blocks_set = (cache->lru_hot_pct * cache->assoc) / 100;

    struct cacheblock *cacheblk;
    // 将所有的cache进行初始化
    for (i = 0; i < cache->set_numbers; i++) {
        // 每一个set的起始索引
        start_index = i * cache->assoc;
        for (j = 0; j < hot_blocks_set; j++) {
            block_index = start_index + j;
            cacheblk = &cache->caches[block_index];
            cacheblk->lru_prev = CACHE_LRU_NULL;
            cacheblk->lru_next = CACHE_LRU_NULL;
            cacheblk->lru_state = LRU_HOT;
            // 添加到Lru端
            cache_reclaim_add_block_to_list_lru(cache, block_index);
        }

        for (; j < cache->assoc; j++) {
            block_index = start_index + j;
            cacheblk = &cache->caches[block_index];
            cacheblk->lru_prev = CACHE_LRU_NULL;
            cacheblk->lru_next = CACHE_LRU_NULL;
            cacheblk->lru_state = LRU_WARM;
            cache_reclaim_add_block_to_list_lru(cache, block_index);
        }
    }
}

// no
void cache_reclaim_add_block_to_list_lru(struct cache_dev *cache, int index) {
    int set = index / cache->assoc;
    int start_index = set * cache->assoc;
    int my_index = index - start_index;

    struct cacheblock *cacheblk = &cache->caches[index];
    struct cache_set *cache_set = &cache->cache_sets[set];

    cacheblk->lru_prev = CACHE_LRU_NULL;

    // 添加到LRU的头部， LRU端  ---  MRU端
    if (cacheblk->lru_state & LRU_WARM) {
        cacheblk->lru_next = cache_set->warmlist_lru_head;
        if (cache_set->warmlist_lru_head == CACHE_LRU_NULL) {
            cache_set->warmlist_lru_tail = my_index;
        } else {
            cache->caches[cache_set->warmlist_lru_head + start_index].lru_prev = my_index;
        }
        cache_set->warmlist_lru_head = my_index;
    } else {
        cacheblk->lru_next = cache_set->hotlist_lru_head;
        if (cache_set->hotlist_lru_head == CACHE_LRU_NULL) {
            cache_set->hotlist_lru_tail = my_index;
        } else {
            cache->caches[cache_set->hotlist_lru_head + start_index].lru_prev = my_index;
        }
        cache_set->hotlist_lru_head = my_index;
    }

    if (cacheblk->lru_state & LRU_WARM) {
        cache_set->lru_warm_blocks++;
        cache->lru_warm_blocks++;
    } else {
        cache_set->lru_hot_blocks++;
        cache->lru_hot_blocks++;
    }
}

// 插入到无效链表,也没问题
void cache_invalid_insert(struct cache_dev *cache, int index) {
    struct cache_set *cache_set;
    struct cacheblock *cacheblk;

    int set = index / cache->assoc;

    int start_index = set * cache->assoc;

    int set_ix = index % cache->assoc;

    /* index validity checks */

    cacheblk = &cache->caches[index];
    /* It has to be an INVALID block */
    /* It cannot be on the per-set hash */
    /* Insert this block at the head of the invalid list */
    cache_set = &cache->cache_sets[set];

    cacheblk->hash_next = cache_set->invalid_head;

    if (cache_set->invalid_head != CACHE_LRU_NULL) {
        cache->caches[start_index + cache_set->invalid_head].hash_prev = set_ix;
    }
    cache_set->invalid_head = set_ix;
}

// cache块命中的场景：
void cache_lru_accessed(struct cache_dev *cache, int index) {
    struct cacheblock *cacheblk = &cache->caches[index];

    if (cacheblk->lru_state & LRU_HOT) {
        cache_reclaim_move_to_mru(cache, index);
    } else {
        if (cacheblk->cache_state == INVALID)
            cache_reclaim_move_to_mru(cache, index);
        return;
    }
    // 提升索引，到hot链表中
    if (!cache_reclaim_promote_block(cache, index)) {
        /* Could not promote block, move it to mru on warm list */
        cache_reclaim_move_to_mru(cache, index);
    }
}

// 移动到mru尾端，没问题
void cache_reclaim_move_to_mru(struct cache_dev *cache, int index) {
    struct cacheblock *cacheblk = &cache->caches[index];

    cache_reclaim_remove_block_from_list(cache, index);
    /* And add it to LRU Tail (MRU side) of its list */
    cache_reclaim_add_block_to_list_mru(cache, index);
}

// 删除 没问题
void cache_reclaim_remove_block_from_list(struct cache_dev *cache, int index) {
    int set = index / cache->assoc;
    int start_index = set * cache->assoc;

    struct cacheblock *cacheblk = &cache->caches[index];
    struct cache_set *cache_set = &cache->cache_sets[set];

    if ((cacheblk->lru_prev == CACHE_LRU_NULL) && (cacheblk->lru_next == CACHE_LRU_NULL)) {
        /*
         * Is this the only member on the list ? Or is this not on the list
         * at all ?
         */
        if (cacheblk->lru_state & LRU_WARM) {
            if (cache_set->warmlist_lru_head == CACHE_LRU_NULL && cache_set->warmlist_lru_tail == CACHE_LRU_NULL)
                return;
        } else {
            if (cache_set->hotlist_lru_head == CACHE_LRU_NULL && cache_set->hotlist_lru_tail == CACHE_LRU_NULL)
                return;
        }
    }

    if (cacheblk->lru_prev != CACHE_LRU_NULL) {
        cache->caches[cacheblk->lru_prev + start_index].lru_next = cacheblk->lru_next;
    } else {
        if (cacheblk->lru_state & LRU_WARM) {
            cache_set->warmlist_lru_head = cacheblk->lru_next;
        } else {
            cache_set->hotlist_lru_head = cacheblk->lru_next;
        }
    }

    if (cacheblk->lru_next != CACHE_LRU_NULL) {
        cache->caches[cacheblk->lru_next + start_index].lru_prev = cacheblk->lru_prev;
    } else {
        if (cacheblk->lru_state & LRU_WARM) {
            cache_set->warmlist_lru_tail = cacheblk->lru_prev;
        } else {
            cache_set->hotlist_lru_tail = cacheblk->lru_prev;
        }
    }
    if (cacheblk->lru_state & LRU_WARM) {
        cache->lru_warm_blocks--;
        cache_set->lru_warm_blocks--;
        if (cache_set->lru_warm_blocks == 0) {
        }
    } else {
        cache->lru_hot_blocks--;
        cache_set->lru_hot_blocks--;
        if (cache_set->lru_hot_blocks == 0) {
        }
    }
}

// no problem
void cache_reclaim_add_block_to_list_mru(struct cache_dev *cache, int index) {
    int set = index / cache->assoc;
    int start_index = set * cache->assoc;

    int my_index = index - start_index;
    struct cacheblock *cacheblk = &cache->caches[index];
    struct cache_set *cache_set = &cache->cache_sets[set];

    cacheblk->lru_next = CACHE_LRU_NULL;

    if (cacheblk->lru_state & LRU_WARM) {
        // 如果状态是在温链表
        cacheblk->lru_prev = cache_set->warmlist_lru_tail;
        if (cache_set->warmlist_lru_tail == CACHE_LRU_NULL) {
            cache_set->warmlist_lru_head = my_index;
        } else {
            cache->caches[cache_set->warmlist_lru_tail + start_index].lru_next = my_index;
        }
        cache_set->warmlist_lru_tail = my_index;

    } else {
        cacheblk->lru_prev = cache_set->hotlist_lru_tail;
        if (cache_set->hotlist_lru_tail == CACHE_LRU_NULL) {
            cache_set->hotlist_lru_head = my_index;
        } else {
            cache->caches[cache_set->hotlist_lru_tail + start_index].lru_next = my_index;
        }
        cache_set->hotlist_lru_tail = my_index;
    }
    // 加上去更新统计信息

    if (cacheblk->lru_state & LRU_WARM) {
        cache->lru_warm_blocks++;
        cache_set->lru_warm_blocks++;
    } else {
        cache->lru_hot_blocks++;
        cache_set->lru_hot_blocks++;
    }
}

// 提升索引 没问题
int cache_reclaim_promote_block(struct cache_dev *cache, int index) {
    struct cacheblock *cacheblk = &cache->caches[index];

    int hot_block;
    int set = index / cache->assoc;
    int start_index = set * cache->assoc;

    struct cache_set *cache_set = &cache->cache_sets[set];

    hot_block = cache_set->hotlist_lru_head;

    if (hot_block == CACHE_LRU_NULL)
        /* We cannot swap this block into the hot list */
        return 0;

    hot_block += start_index;

    /* Remove warm block from its list first */
    cache_reclaim_remove_block_from_list(cache, index);

    /* Remove hot block identified above from its list */
    cache_reclaim_remove_block_from_list(cache, hot_block);

    /* Swap the 2 blocks */

    cacheblk->lru_state &= ~LRU_WARM;
    cacheblk->lru_state |= LRU_HOT;
    // 加入hot链表
    cache_reclaim_add_block_to_list_lru(cache, index);

    cacheblk = &cache->caches[hot_block];

    cacheblk->lru_state &= ~LRU_HOT;
    cacheblk->lru_state |= LRU_WARM;
    cache_reclaim_add_block_to_list_mru(cache, hot_block);
    return 1;
}

// 从invalid链表里面，剔除一个
void cache_invalid_remove(struct cache_dev *cache, int index) {
    struct cache_set *cache_set;
    struct cacheblock *cacheblk;
    int start_index, set;

    cacheblk = &cache->caches[index];
    set = index / cache->assoc;
    start_index = set * cache->assoc;
    cache_set = &cache->cache_sets[set];

    if (cacheblk->hash_prev != CACHE_LRU_NULL) {
        cache->caches[start_index + cacheblk->hash_prev].hash_next =
            cacheblk->hash_next;
    } else {
        cache_set->invalid_head = cacheblk->hash_next;
    }
    if (cacheblk->hash_next != CACHE_LRU_NULL) {
        cache->caches[start_index + cacheblk->hash_next].hash_prev = cacheblk->hash_prev;
    }
    cacheblk->hash_prev = CACHE_LRU_NULL;
    cacheblk->hash_next = CACHE_LRU_NULL;
}

// 没问题
int cache_reclaim_demote_block(struct cache_dev *cache, int index) {
    struct cacheblock *cacheblk = &cache->caches[index];
    int warm_block;
    int set = index / cache->assoc;

    struct cache_set *cache_set = &cache->cache_sets[set];

    int start_index = set * cache->assoc;

    warm_block = cache_set->warmlist_lru_tail;

    if (warm_block == CACHE_LRU_NULL) {
        return 0;
    }

    warm_block += start_index;
    /* Remove hot block from its list first */
    cache_reclaim_remove_block_from_list(cache, index);
    /* Remove warm block identified above from its list */
    cache_reclaim_remove_block_from_list(cache, warm_block);
    /* Swap the 2 blocks */

    cacheblk->lru_state &= ~LRU_HOT;
    cacheblk->lru_state |= LRU_WARM;
    cache_reclaim_add_block_to_list_mru(cache, index);

    cacheblk = &cache->caches[warm_block];
    cacheblk->lru_state &= ~LRU_WARM;
    cacheblk->lru_state |= LRU_HOT;
    cache_reclaim_add_block_to_list_lru(cache, warm_block);
    return 1;
}

// 最后一个更新函数:清理脏块
void cache_clean_set(struct cache_dev *cache, int set) {
    int to_clean = 0;
    int start_index = set * cache->assoc;
    int nr_dirty = 0;
    int end_index = start_index + cache->assoc;
    int i = 0;
    int nr_writes = 0;
    struct cache_set *cache_set = &cache->cache_sets[set];
    struct cacheblock *cacheblk;
    int scanned = 0;

    for (i = start_index; i < end_index; i++) {
        if ((cache->caches[i].cache_state & DIRTY)) {
            nr_dirty++;
        }
    }
    if (nr_dirty < cache->dirty_thresh_set) {
        return;
    } else {
        to_clean = nr_dirty - cache->dirty_thresh_set;
    }

    if (cache->cache_policy == CACHE_FIFO) {
        i = cache_set->set_clean_next;
        while (scanned < cache->assoc && nr_writes < to_clean && nr_writes < cache->max_clean_ios_set) {
            cacheblk = &cache->caches[i];
            if (cacheblk->cache_state & DIRTY) {
                cacheblk->cache_state &= ~DIRTY;
                nr_writes++;
            }
            scanned++;
            i++;
            if (i == end_index) {
                i = start_index;
            }
        }
        cache_set->set_clean_next = i;
    } else {
        int lru_rel_index;
        int iter = 0;
        for (iter = 0; iter < 2; iter++) {
            if (iter == 0) {
                lru_rel_index = cache_set->warmlist_lru_head;
            } else {
                lru_rel_index = cache_set->hotlist_lru_head;
            }
            while (lru_rel_index != CACHE_LRU_NULL && nr_writes < to_clean && nr_writes < cache->max_clean_ios_set) {
                cacheblk = &cache->caches[lru_rel_index + start_index];
                if (cacheblk->cache_state & DIRTY) {
                    // 触发回写
                    cacheblk->cache_state &= ~DIRTY;
                }
                lru_rel_index = cacheblk->lru_next;
                nr_writes++;
            }
        }
    }
    return;
}

// 没问题
void cache_hash_remove(struct cache_dev *cache, int index) {
    struct cache_set *cache_set;
    struct cacheblock *cacheblk;
    int *hash_bucket;

    int start_index, set;

    if (index == -1)
        return;
    set = index / cache->assoc;
    cache_set = &cache->cache_sets[set];
    cacheblk = &cache->caches[index];
    start_index = set * cache->assoc;
    hash_bucket = cache_get_hash_bucket(cache, cache_set, cacheblk->blkno);

    if (cacheblk->hash_prev != CACHE_LRU_NULL) {
        cache->caches[start_index + cacheblk->hash_prev].hash_next = cacheblk->hash_next;
    } else {
        *hash_bucket = cacheblk->hash_next;
    }
    if (cacheblk->hash_next != CACHE_LRU_NULL) {
        cache->caches[start_index + cacheblk->hash_next].hash_prev =
            cacheblk->hash_prev;
    }
    cacheblk->hash_prev = CACHE_LRU_NULL;
    cacheblk->hash_next = CACHE_LRU_NULL;
}

// 没问题
int cache_hash_lookup(struct cache_dev *cache, int set, int blkno) {
    struct cache_set *cache_set = &cache->cache_sets[set];
    int index;
    struct cacheblock *cacheblk;
    int set_ix;

    set_ix = *cache_get_hash_bucket(cache, cache_set, blkno);

    while (set_ix != CACHE_LRU_NULL) {
        index = set * cache->assoc + set_ix;
        cacheblk = &cache->caches[index];

        if (blkno == cacheblk->blkno) {
            return index;
        }
        set_ix = cacheblk->hash_next;
    }
    return -1;
}

// 没问题
int *cache_get_hash_bucket(struct cache_dev *cache, struct cache_set *cache_set, int blkno) {
    unsigned int hash = blkno;
    return &cache_set->hash_buckets[hash % NUM_BLOCK_HASH_BUCKETS];
}

// 没问题
void cache_hash_insert(struct cache_dev *cache, int index) {
    struct cache_set *cache_set = &cache->cache_sets[index / cache->assoc];
    struct cacheblock *cacheblk;

    int *hash_bucket;
    int set_ix = index % cache->assoc;

    int start_index = (index / cache->assoc) * cache->assoc;

    cacheblk = &cache->caches[index];

    hash_bucket = cache_get_hash_bucket(cache, cache_set, cacheblk->blkno);

    cacheblk->hash_prev = CACHE_LRU_NULL;

    cacheblk->hash_next = *hash_bucket;

    if (*hash_bucket != CACHE_LRU_NULL) {
        cache->caches[start_index + *hash_bucket].hash_prev = set_ix;
    }
    *hash_bucket = set_ix;
}
