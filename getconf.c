/*-
 * Copyright (c) 1996, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by J.T. Conklin.
 *
 * Mostly rewritten to be used in Alpine Linux (with musl c-library)
 * by Timo Teräs.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct conf_variable {
	const char *name;
	enum { SYSCONF, CONFSTR, PATHCONF, CONSTANT, UCONSTANT, NUM_TYPES } type;
	long value;
};

static const struct conf_variable conf_table[] = {
{ "PATH",			CONFSTR,	_CS_PATH		},

/* Utility Limit Minimum Values */
{ "POSIX2_BC_BASE_MAX",		CONSTANT,	_POSIX2_BC_BASE_MAX	},
{ "POSIX2_BC_DIM_MAX",		CONSTANT,	_POSIX2_BC_DIM_MAX	},
{ "POSIX2_BC_SCALE_MAX",	CONSTANT,	_POSIX2_BC_SCALE_MAX	},
{ "POSIX2_BC_STRING_MAX",	CONSTANT,	_POSIX2_BC_STRING_MAX	},
{ "POSIX2_COLL_WEIGHTS_MAX",	CONSTANT,	_POSIX2_COLL_WEIGHTS_MAX },
{ "POSIX2_EXPR_NEST_MAX",	CONSTANT,	_POSIX2_EXPR_NEST_MAX	},
{ "POSIX2_LINE_MAX",		CONSTANT,	_POSIX2_LINE_MAX	},
{ "POSIX2_RE_DUP_MAX",		CONSTANT,	_POSIX2_RE_DUP_MAX	},
{ "POSIX2_VERSION",		CONSTANT,	_POSIX2_VERSION		},

/* POSIX.1 Minimum Values */
{ "_POSIX_AIO_LISTIO_MAX",	CONSTANT,	_POSIX_AIO_LISTIO_MAX	},
{ "_POSIX_AIO_MAX",		CONSTANT,       _POSIX_AIO_MAX		},
{ "_POSIX_ARG_MAX",		CONSTANT,	_POSIX_ARG_MAX		},
{ "_POSIX_CHILD_MAX",		CONSTANT,	_POSIX_CHILD_MAX	},
{ "_POSIX_LINK_MAX",		CONSTANT,	_POSIX_LINK_MAX		},
{ "_POSIX_MAX_CANON",		CONSTANT,	_POSIX_MAX_CANON	},
{ "_POSIX_MAX_INPUT",		CONSTANT,	_POSIX_MAX_INPUT	},
{ "_POSIX_MQ_OPEN_MAX",		CONSTANT,	_POSIX_MQ_OPEN_MAX	},
{ "_POSIX_MQ_PRIO_MAX",		CONSTANT,	_POSIX_MQ_PRIO_MAX	},
{ "_POSIX_NAME_MAX",		CONSTANT,	_POSIX_NAME_MAX		},
{ "_POSIX_NGROUPS_MAX",		CONSTANT,	_POSIX_NGROUPS_MAX	},
{ "_POSIX_OPEN_MAX",		CONSTANT,	_POSIX_OPEN_MAX		},
{ "_POSIX_PATH_MAX",		CONSTANT,	_POSIX_PATH_MAX		},
{ "_POSIX_PIPE_BUF",		CONSTANT,	_POSIX_PIPE_BUF		},
{ "_POSIX_SSIZE_MAX",		CONSTANT,	_POSIX_SSIZE_MAX	},
{ "_POSIX_STREAM_MAX",		CONSTANT,	_POSIX_STREAM_MAX	},
{ "_POSIX_TZNAME_MAX",		CONSTANT,	_POSIX_TZNAME_MAX	},

/* Symbolic Utility Limits */
{ "BC_BASE_MAX",		SYSCONF,	_SC_BC_BASE_MAX		},
{ "BC_DIM_MAX",			SYSCONF,	_SC_BC_DIM_MAX		},
{ "BC_SCALE_MAX",		SYSCONF,	_SC_BC_SCALE_MAX	},
{ "BC_STRING_MAX",		SYSCONF,	_SC_BC_STRING_MAX	},
{ "COLL_WEIGHTS_MAX",		SYSCONF,	_SC_COLL_WEIGHTS_MAX	},
{ "EXPR_NEST_MAX",		SYSCONF,	_SC_EXPR_NEST_MAX	},
{ "LINE_MAX",			SYSCONF,	_SC_LINE_MAX		},
{ "RE_DUP_MAX",			SYSCONF,	_SC_RE_DUP_MAX		},

/* Optional Facility Configuration Values */
{ "_POSIX2_C_BIND",		SYSCONF,	_SC_2_C_BIND		},
{ "POSIX2_C_DEV",		SYSCONF,	_SC_2_C_DEV		},
{ "POSIX2_CHAR_TERM",		SYSCONF,	_SC_2_CHAR_TERM		},
{ "POSIX2_FORT_DEV",		SYSCONF,	_SC_2_FORT_DEV		},
{ "POSIX2_FORT_RUN",		SYSCONF,	_SC_2_FORT_RUN		},
{ "POSIX2_LOCALEDEF",		SYSCONF,	_SC_2_LOCALEDEF		},
{ "POSIX2_SW_DEV",		SYSCONF,	_SC_2_SW_DEV		},
{ "POSIX2_UPE",			SYSCONF,	_SC_2_UPE		},

/* POSIX.1 Configurable System Variables */
{ "AIO_LISTIO_MAX",		SYSCONF,	_SC_AIO_LISTIO_MAX	},
{ "AIO_MAX",			SYSCONF,	_SC_AIO_MAX		},
{ "ARG_MAX",			SYSCONF,	_SC_ARG_MAX 		},
{ "CHILD_MAX",			SYSCONF,	_SC_CHILD_MAX		},
{ "CLK_TCK",			SYSCONF,	_SC_CLK_TCK		},
{ "MQ_OPEN_MAX",		SYSCONF,	_SC_MQ_OPEN_MAX		},
{ "MQ_PRIO_MAX",		SYSCONF,	_SC_MQ_PRIO_MAX		},
{ "NGROUPS_MAX",		SYSCONF,	_SC_NGROUPS_MAX		},
{ "OPEN_MAX",			SYSCONF,	_SC_OPEN_MAX		},
{ "STREAM_MAX",			SYSCONF,	_SC_STREAM_MAX		},
{ "TZNAME_MAX",			SYSCONF,	_SC_TZNAME_MAX		},
{ "_POSIX_JOB_CONTROL",		SYSCONF,	_SC_JOB_CONTROL 	},
{ "_POSIX_SAVED_IDS",		SYSCONF,	_SC_SAVED_IDS		},
{ "_POSIX_VERSION",		SYSCONF,	_SC_VERSION		},

{ "LINK_MAX",			PATHCONF,	_PC_LINK_MAX		},
{ "MAX_CANON",			PATHCONF,	_PC_MAX_CANON		},
{ "MAX_INPUT",			PATHCONF,	_PC_MAX_INPUT		},
{ "NAME_MAX",			PATHCONF,	_PC_NAME_MAX		},
{ "PATH_MAX",			PATHCONF,	_PC_PATH_MAX		},
{ "PIPE_BUF",			PATHCONF,	_PC_PIPE_BUF		},
{ "_POSIX_CHOWN_RESTRICTED",	PATHCONF,	_PC_CHOWN_RESTRICTED	},
{ "_POSIX_NO_TRUNC",		PATHCONF,	_PC_NO_TRUNC		},
{ "_POSIX_VDISABLE",		PATHCONF,	_PC_VDISABLE		},

/* POSIX.1b Configurable System Variables */
{ "PAGESIZE",			SYSCONF,	_SC_PAGESIZE		},
{ "_POSIX_ASYNCHRONOUS_IO",	SYSCONF,	_SC_ASYNCHRONOUS_IO	},
{ "_POSIX_FSYNC",		SYSCONF,	_SC_FSYNC		},
{ "_POSIX_MAPPED_FILES",	SYSCONF,	_SC_MAPPED_FILES	},
{ "_POSIX_MEMLOCK",		SYSCONF,	_SC_MEMLOCK		},
{ "_POSIX_MEMLOCK_RANGE",	SYSCONF,	_SC_MEMLOCK_RANGE	},
{ "_POSIX_MEMORY_PROTECTION",	SYSCONF,	_SC_MEMORY_PROTECTION	},
{ "_POSIX_MESSAGE_PASSING",	SYSCONF,	_SC_MESSAGE_PASSING	},
{ "_POSIX_MONOTONIC_CLOCK",	SYSCONF,	_SC_MONOTONIC_CLOCK	},
{ "_POSIX_PRIORITY_SCHEDULING", SYSCONF,	_SC_PRIORITY_SCHEDULING },
{ "_POSIX_SEMAPHORES",		SYSCONF,	_SC_SEMAPHORES		},
{ "_POSIX_SHARED_MEMORY_OBJECTS", SYSCONF,	_SC_SHARED_MEMORY_OBJECTS },
{ "_POSIX_SYNCHRONIZED_IO",	SYSCONF,	_SC_SYNCHRONIZED_IO	},
{ "_POSIX_TIMERS",		SYSCONF,	_SC_TIMERS		},

{ "_POSIX_SYNC_IO",		PATHCONF,	_PC_SYNC_IO		},

/* POSIX.1c Configurable System Variables */
{ "LOGIN_NAME_MAX",		SYSCONF,	_SC_LOGIN_NAME_MAX	},
{ "_POSIX_THREADS",		SYSCONF,	_SC_THREADS		},

/* POSIX.1j Configurable System Variables */
{ "_POSIX_BARRIERS",		SYSCONF,	_SC_BARRIERS		},
{ "_POSIX_READER_WRITER_LOCKS", SYSCONF,	_SC_READER_WRITER_LOCKS	},
{ "_POSIX_SPIN_LOCKS",		SYSCONF,	_SC_SPIN_LOCKS		},

/* XPG4.2 Configurable System Variables */
{ "IOV_MAX",			SYSCONF,	_SC_IOV_MAX		},
{ "PAGE_SIZE",			SYSCONF,	_SC_PAGE_SIZE		},
{ "_XOPEN_SHM",			SYSCONF,	_SC_XOPEN_SHM		},

/* X/Open CAE Spec. Issue 5 Version 2 Configurable System Variables */
{ "FILESIZEBITS",		PATHCONF,	_PC_FILESIZEBITS	},

/* POSIX.1-2001 XSI Option Group Configurable System Variables */
{ "ATEXIT_MAX",			SYSCONF,	_SC_ATEXIT_MAX		},

/* POSIX.1-2001 TSF Configurable System Variables */
{ "GETGR_R_SIZE_MAX",		SYSCONF,	_SC_GETGR_R_SIZE_MAX	},
{ "GETPW_R_SIZE_MAX",		SYSCONF,	_SC_GETPW_R_SIZE_MAX	},

/* POSIX.1-2008 */
{ "POSIX2_SYMLINKS",		PATHCONF,	_PC_2_SYMLINKS		},
{ "POSIX_ALLOC_SIZE_MIN",	PATHCONF,	_PC_ALLOC_SIZE_MIN	},
{ "POSIX_REC_INCR_XFER_SIZE",	PATHCONF,	_PC_REC_INCR_XFER_SIZE	},
{ "POSIX_REC_MAX_XFER_SIZE",	PATHCONF,	_PC_REC_MAX_XFER_SIZE	},
{ "POSIX_REC_MIN_XFER_SIZE",	PATHCONF,	_PC_REC_MIN_XFER_SIZE	},
{ "POSIX_REC_XFER_ALIGN",	PATHCONF,	_PC_REC_XFER_ALIGN	},
{ "SYMLINK_MAX",		PATHCONF,	_PC_SYMLINK_MAX		},
{ "_POSIX_ASYNC_IO",		PATHCONF,	_PC_ASYNC_IO		},
{ "_POSIX_PRIO_IO",		PATHCONF,	_PC_PRIO_IO		},
/* not yet supported by musl
{ "_POSIX_TIMESTAMP_RESOLUTION", PATHCONF,	_PC_TIMESTAMP_RESOLUTION },
*/
{ "AIO_PRIO_DELTA_MAX",		SYSCONF,	_SC_AIO_PRIO_DELTA_MAX	},
{ "DELAYTIMER_MAX",		SYSCONF,	_SC_DELAYTIMER_MAX	},
{ "HOST_NAME_MAX",		SYSCONF,	_SC_HOST_NAME_MAX	},
{ "_POSIX_ADVISORY_INFO",	SYSCONF,	_SC_ADVISORY_INFO	},
{ "_POSIX_CLOCK_SELECTION",	SYSCONF,	_SC_CLOCK_SELECTION	},
{ "_POSIX_CPUTIME",		SYSCONF,	_SC_CPUTIME		},
{ "_POSIX_IPV6",		SYSCONF,	_SC_IPV6		},
{ "_POSIX_PRIORITIZED_IO",	SYSCONF,	_SC_PRIORITIZED_IO	},
{ "_POSIX_RAW_SOCKETS",		SYSCONF,	_SC_RAW_SOCKETS		},
{ "_POSIX_REALTIME_SIGNALS",	SYSCONF,	_SC_REALTIME_SIGNALS	},
{ "_POSIX_REGEXP",		SYSCONF,	_SC_REGEXP		},
{ "_POSIX_SHELL",		SYSCONF,	_SC_SHELL		},
{ "_POSIX_SPAWN",		SYSCONF,	_SC_SPAWN		},
{ "_POSIX_SPORADIC_SERVER",	SYSCONF,	_SC_SPORADIC_SERVER	},
{ "_POSIX_SS_REPL_MAX",		SYSCONF,	_SC_SS_REPL_MAX		},
{ "_POSIX_THREAD_ATTR_STACKADDR", SYSCONF,	_SC_THREAD_ATTR_STACKADDR },
{ "_POSIX_THREAD_ATTR_STACKSIZE", SYSCONF,	_SC_THREAD_ATTR_STACKSIZE },
{ "_POSIX_THREAD_CPUTIME",	SYSCONF,	_SC_THREAD_CPUTIME	},
{ "_POSIX_THREAD_PRIO_INHERIT",	SYSCONF,	_SC_THREAD_PRIO_INHERIT	},
{ "_POSIX_THREAD_PRIO_PROTECT",	SYSCONF,	_SC_THREAD_PRIO_PROTECT	},
{ "_POSIX_THREAD_PRIORITY_SCHEDULING", SYSCONF,	_SC_THREAD_PRIORITY_SCHEDULING },
{ "_POSIX_THREAD_PROCESS_SHARED", SYSCONF,	_SC_THREAD_PROCESS_SHARED },
{ "_POSIX_THREAD_ROBUST_PRIO_INHERIT", SYSCONF,	_SC_THREAD_ROBUST_PRIO_INHERIT },
{ "_POSIX_THREAD_ROBUST_PRIO_PROTECT", SYSCONF,	_SC_THREAD_ROBUST_PRIO_PROTECT },
{ "_POSIX_THREAD_SAFE_FUNCTIONS", SYSCONF,	_SC_THREAD_SAFE_FUNCTIONS },
{ "_POSIX_THREAD_SPORADIC_SERVER", SYSCONF,	_SC_THREAD_SPORADIC_SERVER },
{ "_POSIX_TIMEOUTS",		SYSCONF,	_SC_TIMEOUTS		},
{ "_POSIX_TRACE",		SYSCONF,	_SC_TRACE		},
{ "_POSIX_TRACE_EVENT_FILTER",	SYSCONF,	_SC_TRACE_EVENT_FILTER	},
{ "_POSIX_TRACE_EVENT_NAME_MAX", SYSCONF,	_SC_TRACE_EVENT_NAME_MAX },
{ "_POSIX_TRACE_INHERIT",	SYSCONF,	_SC_TRACE_INHERIT	},
{ "_POSIX_TRACE_LOG",		SYSCONF,	_SC_TRACE_LOG		},
{ "_POSIX_TRACE_NAME_MAX",	SYSCONF,	_SC_TRACE_NAME_MAX	},
{ "_POSIX_TRACE_SYS_MAX",	SYSCONF,	_SC_TRACE_SYS_MAX	},
{ "_POSIX_TRACE_USER_EVENT_MAX", SYSCONF,	_SC_TRACE_USER_EVENT_MAX },
{ "_POSIX_TYPED_MEMORY_OBJECTS", SYSCONF,	_SC_TYPED_MEMORY_OBJECTS },
{ "_POSIX_V7_ILP32_OFF32",	SYSCONF,	_SC_V7_ILP32_OFF32	},
{ "_POSIX_V7_ILP32_OFFBIG",	SYSCONF,	_SC_V7_ILP32_OFFBIG	},
{ "_POSIX_V7_LP64_OFF64",	SYSCONF,	_SC_V7_LP64_OFF64	},
{ "_POSIX_V7_LPBIG_OFFBIG",	SYSCONF,	_SC_V7_LPBIG_OFFBIG	},
{ "_POSIX_V6_ILP32_OFF32",	SYSCONF,	_SC_V6_ILP32_OFF32	},
{ "_POSIX_V6_ILP32_OFFBIG",	SYSCONF,	_SC_V6_ILP32_OFFBIG	},
{ "_POSIX_V6_LP64_OFF64",	SYSCONF,	_SC_V6_LP64_OFF64	},
{ "_POSIX_V6_LPBIG_OFFBIG",	SYSCONF,	_SC_V6_LPBIG_OFFBIG	},
{ "_POSIX2_PBS",		SYSCONF,	_SC_2_PBS		},
{ "_POSIX2_PBS_ACCOUNTING",	SYSCONF,	_SC_2_PBS_ACCOUNTING	},
{ "_POSIX2_PBS_CHECKPOINT",	SYSCONF,	_SC_2_PBS_CHECKPOINT	},
{ "_POSIX2_PBS_LOCATE",		SYSCONF,	_SC_2_PBS_LOCATE	},
{ "_POSIX2_PBS_MESSAGE",	SYSCONF,	_SC_2_PBS_MESSAGE	},
{ "_POSIX2_PBS_TRACK",		SYSCONF,	_SC_2_PBS_TRACK		},
{ "_POSIX2_VERSION",		SYSCONF,	_SC_2_VERSION		},
{ "PTHREAD_DESTRUCTOR_ITERATIONS", SYSCONF,	_SC_THREAD_DESTRUCTOR_ITERATIONS },
{ "PTHREAD_KEYS_MAX",		SYSCONF,	_SC_THREAD_KEYS_MAX	},
{ "PTHREAD_STACK_MIN",		SYSCONF,	_SC_THREAD_STACK_MIN	},
{ "PTHREAD_THREADS_MAX",	SYSCONF,	_SC_THREAD_THREADS_MAX	},
{ "RTSIG_MAX",			SYSCONF,	_SC_RTSIG_MAX		},
{ "SEM_NSEMS_MAX",		SYSCONF,	_SC_SEM_NSEMS_MAX	},
{ "SEM_VALUE_MAX",		SYSCONF,	_SC_SEM_VALUE_MAX	},
{ "SIGQUEUE_MAX",		SYSCONF,	_SC_SIGQUEUE_MAX	},
{ "SYMLOOP_MAX",		SYSCONF,	_SC_SYMLOOP_MAX		},
{ "TIMER_MAX",			SYSCONF,	_SC_TIMER_MAX		},
{ "TTY_NAME_MAX",		SYSCONF,	_SC_TTY_NAME_MAX	},
{ "_XOPEN_CRYPT",		SYSCONF,	_SC_XOPEN_CRYPT		},
{ "_XOPEN_ENH_I18N",		SYSCONF,	_SC_XOPEN_ENH_I18N	},
{ "_XOPEN_REALTIME",		SYSCONF,	_SC_XOPEN_REALTIME	},
{ "_XOPEN_REALTIME_THREADS",	SYSCONF,	_SC_XOPEN_REALTIME_THREADS },
{ "_XOPEN_STREAMS",		SYSCONF,	_SC_XOPEN_STREAMS	},
{ "_XOPEN_UNIX",		SYSCONF,	_SC_XOPEN_UNIX		},
/* not supported by musl
{ "_XOPEN_UUCP",		SYSCONF,	_SC_XOPEN_UUCP		},
*/
{ "_XOPEN_VERSION",		SYSCONF,	_SC_XOPEN_VERSION	},

/* Commonly provided extensions */
{ "_PHYS_PAGES",		SYSCONF,	_SC_PHYS_PAGES		},
{ "_AVPHYS_PAGES",		SYSCONF,	_SC_AVPHYS_PAGES	},
{ "_NPROCESSORS_CONF",		SYSCONF,	_SC_NPROCESSORS_CONF	},
{ "_NPROCESSORS_ONLN",		SYSCONF,	_SC_NPROCESSORS_ONLN	},

/* Data type related extensions */
{ "CHAR_BIT",			CONSTANT,	CHAR_BIT		},
{ "CHAR_MAX",			CONSTANT,	CHAR_MAX		},
{ "CHAR_MIN",			CONSTANT,	CHAR_MIN		},
{ "INT_MAX",			CONSTANT,	INT_MAX			},
{ "INT_MIN",			CONSTANT,	INT_MIN			},
{ "LONG_BIT",			CONSTANT,	LONG_BIT		},
{ "LONG_MAX",			CONSTANT,	LONG_MAX		},
{ "LONG_MIN",			CONSTANT,	LONG_MIN		},
{ "SCHAR_MAX",			CONSTANT,	SCHAR_MAX		},
{ "SCHAR_MIN",			CONSTANT,	SCHAR_MIN		},
{ "SHRT_MAX",			CONSTANT,	SHRT_MAX		},
{ "SHRT_MIN",			CONSTANT,	SHRT_MIN		},
{ "SSIZE_MAX",			CONSTANT,	SSIZE_MAX		},
{ "UCHAR_MAX",			UCONSTANT,	(long) UCHAR_MAX	},
{ "UINT_MAX",			UCONSTANT,	(long) UINT_MAX		},
{ "ULONG_MAX",			UCONSTANT,	(long) ULONG_MAX	},
{ "USHRT_MAX",			UCONSTANT,	(long) USHRT_MAX	},
{ "WORD_BIT",			CONSTANT,	WORD_BIT		},

{ NULL, CONSTANT, 0L }
};

static int all = 0;

static void usage(const char *p)
{
	(void)fprintf(stderr, "Usage: %s system_var\n\t%s -a\n"
	    "\t%s path_var pathname\n\t%s -a pathname\n", p, p, p, p);
	exit(EXIT_FAILURE);
}

static void print_long(const char *name, long val)
{
	if (all) printf("%s = %ld\n", name, val);
	else printf("%ld\n", val);
}

static void print_ulong(const char *name, unsigned long val)
{
	if (all) printf("%s = %lu\n", name, val);
	else printf("%lu\n", val);
}

static void print_string(const char *name, const char *val)
{
	if (all) printf("%s = %s\n", name, val);
	else printf("%s\n", val);
}

static int print_constant(const struct conf_variable *cp, const char *pathname)
{
	print_long(cp->name, cp->value);
	return 0;
}

static int print_uconstant(const struct conf_variable *cp, const char *pathname)
{
	print_ulong(cp->name, (unsigned long) cp->value);
	return 0;
}

static int print_sysconf(const struct conf_variable *cp, const char *pathname)
{
	long val;

	errno = 0;
	if ((val = sysconf((int)cp->value)) == -1) {
		if (errno != 0) err(EXIT_FAILURE, "sysconf(%ld)", cp->value);
		return -1;
	}
	print_long(cp->name, val);
	return 0;
}

static int print_confstr(const struct conf_variable *cp, const char *pathname)
{
	size_t len;
	char *val;

	errno = 0;
	if ((len = confstr((int)cp->value, NULL, 0)) == 0) goto error;
	if ((val = malloc(len)) == NULL) err(EXIT_FAILURE, "Can't allocate %zu bytes", len);
	errno = 0;
	if (confstr((int)cp->value, val, len) == 0) goto error;
	print_string(cp->name, val);
	free(val);
	return 0;
error:
	if (errno != EINVAL) err(EXIT_FAILURE, "confstr(%ld)", cp->value);
	return -1;
}

static int print_pathconf(const struct conf_variable *cp, const char *pathname)
{
	long val;

	errno = 0;
	if ((val = pathconf(pathname, (int)cp->value)) == -1) {
		if (all && errno == EINVAL) return 0;
		if (errno != 0) err(EXIT_FAILURE, "pathconf(%s, %ld)", pathname, cp->value);
		return -1;
	}
	print_long(cp->name, val);
	return 0;
}

typedef int (*handler_t)(const struct conf_variable *cp, const char *pathname);
static const handler_t type_handlers[NUM_TYPES] = {
	[SYSCONF]	= print_sysconf,
	[CONFSTR]	= print_confstr,
	[PATHCONF]	= print_pathconf,
	[CONSTANT]	= print_constant,
	[UCONSTANT]	= print_uconstant,
};

int main(int argc, char **argv)
{
	const char *progname = argv[0];
	const struct conf_variable *cp;
	const char *varname, *pathname;
	int ch, found = 0;

	(void)setlocale(LC_ALL, "");
	while ((ch = getopt(argc, argv, "a")) != -1) {
		switch (ch) {
		case 'a':
			all = 1;
			break;
		case '?':
		default:
			usage(progname);
		}
	}
	argc -= optind;
	argv += optind;

	if (!all) {
		if (argc == 0)
			usage(progname);
		varname = argv[0];
		argc--;
		argv++;
	} else
		varname = NULL;

	if (argc > 1)
		usage(progname);
	pathname = argv[0];	/* may be NULL */

	for (cp = conf_table; cp->name != NULL; cp++) {
		if (!all && strcmp(varname, cp->name) != 0) continue;
		if ((cp->type == PATHCONF) == (pathname != NULL)) {
			if (type_handlers[cp->type](cp, pathname) < 0)
				print_string(cp->name, "undefined");
			found = 1;
		} else if (!all)
			errx(EXIT_FAILURE, "%s: invalid variable type", cp->name);
	}
	if (!all && !found) errx(EXIT_FAILURE, "%s: unknown variable", varname);
	(void)fflush(stdout);
	return ferror(stdout) ? EXIT_FAILURE : EXIT_SUCCESS;
}
