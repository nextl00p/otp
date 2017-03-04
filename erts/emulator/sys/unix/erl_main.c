// #define UID_ERL_LOAD_NFS

/*
 * %CopyrightBegin%
 * 
 * Copyright Ericsson AB 2000-2009. All Rights Reserved.
 * 
 * The contents of this file are subject to the Erlang Public License,
 * Version 1.1, (the "License"); you may not use this file except in
 * compliance with the License. You should have received a copy of the
 * Erlang Public License along with this software. If not, it can be
 * retrieved online at http://www.erlang.org/.
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 * 
 * %CopyrightEnd%
 */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "sys.h"
#include "erl_vm.h"
#include "global.h"

#ifdef __rtems__
#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/console.h>
#include <bsp.h>
#include <assert.h>
#include <rtems/libio.h>

#include <grisp/pin-config.h>
#include <grisp/led.h>
#include <grisp/init.h>


#define MNT "/media/mmcsd-0-0/"

static void Init(rtems_task_argument arg)
{
  char *argv[] = { "erl.rtems", /* "-vsgMatpmX", */ "--", "-root", MNT "otp",
		   "-home", MNT "home", "-boot", "start_sasl", 
		   /* "-noshell", "-noinput", */
		   /* "-config", "/mnt/uid", */
		   /* "-internal_epmd", "epmd_sup", "-sname", "uid" */
		   "-init_debug", "-loader_debug"
  };
  int argc = sizeof(argv)/sizeof(*argv);

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;

  printf("\nerl_main: starting ... ");

  grisp_led_set1(false, false, false);
  grisp_led_set2(true, true, true);
  printf("mounting sd card\n");
  grisp_init_sd_card();
  grisp_init_lower_self_prio();
  grisp_init_libbsd();
  
  /* Wait for the SD card */
  sc = grisp_init_wait_for_sd();
  if(sc == RTEMS_SUCCESSFUL) {
    printf("sd card mounted\n");
  } else {
    printf("ERROR: SD could not be mounted after timeout\n");
    grisp_led_set2(true, false, false);
  }

  printf("making temp dirs\n");

  printf("mkdir /tmp\n");
  rv = mkdir("/tmp", 0755);
  assert(rv == 0);

  printf("mkdir /tmp/log\n");
  rv = mkdir("/tmp/log", 0755);
  assert(rv == 0);

  printf("Setting environment\n");
  setenv("BINDIR", MNT "otp/lib/erlang/bin", 1);
  setenv("ROOTDIR", MNT "otp", 1);
  setenv("PROGNAME", "erl.rtems", 1);
  setenv("ERL_INETRC", MNT "home/erl_inetrc", 1);
  setenv("ERL_LIBS", MNT "apps", 1);
  setenv("HOME", MNT "home", 1);

  printf("starting erlang runtime\n");
  erl_start(argc, argv);
  printf("erlang runtime exited\n");
  sleep(2);
  exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

/* increase max file size in IMFS to 64MB */
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK 256 

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 64

#define CONFIGURE_MAXIMUM_TASKS 32
#define CONFIGURE_MAXIMUM_DRIVERS 8
#define CONFIGURE_MAXIMUM_SEMAPHORES 32
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 4
#define CONFIGURE_MAXIMUM_TIMERS 4
#define CONFIGURE_MAXIMUM_PERIODS 4

#define CONFIGURE_MAXIMUM_POSIX_THREADS 4
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 16

#define CONFIGURE_EXTRA_TASK_STACKS (512 * 1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (512 * 1024)

#define CONFIGURE_IDLE_TASK_STACK_SIZE (512 * 1024)

/* #define CONFIGURE_STACK_CHECKER_ENABLED */

/* #define CONFIGURE_UNLIMITED_OBJECTS */
#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 8
#define CONFIGURE_MAXIMUM_POSIX_KEYS      16

#define CONFIGURE_PIPES_ENABLED
#define CONFIGURE_MAXIMUM_PIPES 16

#define CONFIGURE_INIT

#define CONFIGURE_MALLOC_DIRTY

#include <rtems/confdefs.h>

#else
  int
    main(int argc, char **argv)
  {
    erl_start(argc, argv);
    return 0;
  }
#endif
