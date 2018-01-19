/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Debugger for RTEMS.
 */

#ifndef _RTEMS_DEBUGGER_REMOTE_h
#define _RTEMS_DEBUGGER_REMOTE_h

#include <rtems/rtems-debugger.h>
#include <rtems/debugger/rtems-debugger-server.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Debugger remote.
 */
struct rtems_debugger_remote
{
  const char* name;
  int         (*begin)(rtems_debugger_remote* remote, const char* device);
  int         (*end)(rtems_debugger_remote* remote);
  int         (*connect)(rtems_debugger_remote* remote);
  int         (*disconnect)(rtems_debugger_remote* remote);
  bool        (*isconnected)(rtems_debugger_remote* remote);
  ssize_t     (*read)(rtems_debugger_remote* remote, void* buf, size_t nbytes);
  ssize_t     (*write)(rtems_debugger_remote* remote, const void* buf, size_t nbytes);
  void*       data;
};

/**
 * Register a remote with the server.
 */
int rtems_debugger_remote_register(rtems_debugger_remote* remote);

/**
 * Find a remote by name.
 */
rtems_debugger_remote* rtems_debugger_remote_find(const char* name);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
