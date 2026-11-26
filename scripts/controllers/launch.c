/* Copyright (C) 2024 John Törnblom

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ps5-homebrew-dev-protocol - Title-aware launch controller.
 * Copyright (C) 2026 BlackBearReloaded
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Launches one compile-time title ID for the foreground user. Exits 0 when
 * the launch request is accepted and 1 otherwise.
 *
 * Modified in 2026 by BlackBearReloaded from launch_app() in shsrv's
 * bundles/launch/launch.c (https://github.com/ps5-payload-dev/shsrv):
 * reduced to a single compile-time title ID with bounded API result logging.
 */

#include <stdint.h>
#include <ps5/klog.h>

#ifndef BOOTSTRAP_TITLE_ID
#error BOOTSTRAP_TITLE_ID must be defined
#endif

typedef struct {
    uint32_t size, user_id, options;
    uint64_t crash_report;
    uint32_t flags;
} launch_context_t;

int sceUserServiceInitialize(void *);
int sceUserServiceGetForegroundUser(uint32_t *);
void sceUserServiceTerminate(void);
int sceSystemServiceLaunchApp(const char *, char **, launch_context_t *);

int main(void)
{
    // A foreign process needs Skip Launch Check to start the registered title.
    launch_context_t context = {.size = sizeof(launch_context_t), .flags = 1};
    char *args[] = {0};
    int rc;

    klog_printf("[launch] title=%s\n", BOOTSTRAP_TITLE_ID);
    rc = sceUserServiceInitialize(0);
    klog_printf("[launch] sceUserServiceInitialize rc=0x%08x\n", (unsigned int)rc);
    if (rc != 0)
        return 1;
    rc = sceUserServiceGetForegroundUser(&context.user_id);
    klog_printf("[launch] sceUserServiceGetForegroundUser rc=0x%08x\n", (unsigned int)rc);
    if (rc == 0)
    {
        rc = sceSystemServiceLaunchApp(BOOTSTRAP_TITLE_ID, args, &context);
        klog_printf("[launch] sceSystemServiceLaunchApp rc=0x%08x\n", (unsigned int)rc);
    }
    sceUserServiceTerminate();
    return rc < 0;
}
