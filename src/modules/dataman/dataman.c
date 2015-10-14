/****************************************************************************
 *
 *   Copyright (c) 2013, 2014 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
/**
 * @file dataman.c
 * DATAMANAGER driver.
 *
 * @author Jean Cyr
 * @author Lorenz Meier
 * @author Julian Oes
 * @author Thomas Gubler
 */

#include <nuttx/config.h>

__EXPORT int dataman_main(int argc, char *argv[]);

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <systemlib/systemlib.h>
#include <systemlib/err.h>
#include <queue.h>
#include <string.h>

#include "dataman.h"
#include <systemlib/param/param.h>

/** Write to the data manager file */
__EXPORT ssize_t
dm_write(dm_item_t item, unsigned char index, dm_persitence_t persistence, const void *buf, size_t count)
{ return -1; }

/** Retrieve from the data manager file */
__EXPORT ssize_t
dm_read(dm_item_t item, unsigned char index, void *buf, size_t count)
{ return -1; }

__EXPORT int
dm_clear(dm_item_t item)
{ return -1; }

__EXPORT void
dm_lock(dm_item_t item)
{}

__EXPORT void
dm_unlock(dm_item_t item)
{}

__EXPORT int
dm_restart(dm_reset_reason reason)
{ return -1; }

int
dataman_main(int argc, char *argv[])
{
	errx(1, "dataman disabled.\n");
	return 1;
}
