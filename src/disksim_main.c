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

#include "disksim_global.h"
#include "disksim_iodriver.h"
#include "disksim_ioqueue.h"
#include "disksim_simresult.h"

int main(int argc, char **argv) {
    time_fliter = (timefliter*)(calloc(1, sizeof(struct timefliter)));
    // time_t startTime, endTime;
    // time(&startTime);
    time(&time_fliter->startTime);
    setlinebuf(stdout);
    setlinebuf(stderr);

    printf("DiskSim v%s\n", VERSION4);
    if (argc == 2) {
        disksim_restore_from_checkpoint(argv[1]);
    } else {
        // printf("sizeof disksim = %ld\n", sizeof(struct disksim));
        disksim = (disksim_t *)calloc(1, sizeof(struct disksim));
        disksim_initialize_disksim_structure(disksim);
        //Cherry: 读 trace 是在 setup 函数里面读取
        disksim_setup_disksim(argc, argv);
    }

    // Cherry: 至此请求还没有切分为事件
    disksim_run_simulation();
    // time(&endTime);
    time(&time_fliter->endTime);
    //  printf( "base     queue: listlen %d, numoutstanding %d\n", disksim->iodriver_info->overallqueue->base.listlen, disksim->iodriver_info->overallqueue->base.numoutstanding );
    //  printf( "priority queue: listlen %d, numoutstanding %d\n", disksim->iodriver_info->overallqueue->priority.listlen, disksim->iodriver_info->overallqueue->priority.numoutstanding );
    //  printf( "timeout  queue: listlen %d, numoutstanding %d\n", disksim->iodriver_info->overallqueue->timeout.listlen, disksim->iodriver_info->overallqueue->timeout.numoutstanding );

    printf("event_count = %ld \n", disksim->event_count);
    // printf("total_req = %d\n", disksim->totalreqs);
    // printf("extraqlen = %d\n", disksim->extraqlen);

    disksim_cleanup_and_printstats(time_fliter);
    exit(0);
}

// End of file
