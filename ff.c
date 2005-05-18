/* $Id$ */

#include <sys/param.h>
#include <sys/mount.h>
#include <sys/queue.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

#define _PATH_SBIN "/sbin"

struct opt {
	char			*o_opt;
	SLIST_ENTRY(opt)	 o_next;
};

__dead void	usage(void);
int		ff(char *, char **);

SLIST_HEAD(, opt)		 optlh;
size_t				 nopts;
const char			*type;

char *searchpaths[] = {
	_PATH_SBIN,
	NULL
};

void
addopt(char *s)
{
	struct opt *o;

	if ((o = malloc(sizeof(*o))) == NULL)
		err(EX_OSERR, "malloc");
	o->o_opt = s;
	SLIST_INSERT_HEAD(&optlh, o, o_next);
	nopts++;
}

int
main(int argc, char *argv[])
{
	int i, c, tstatus, status;
	struct opt *o, *next;
	char **newargv;

	SLIST_INIT(&optlh);
	while ((c = getopt(argc, argv, "a:c:F:Ii:lm:n:o:p:su")) != -1)
		switch (c) {
		case 'a':
			addopt("-a");
			addopt(optarg);
			break;
		case 'c':
			addopt("-c");
			addopt(optarg);
			break;
		case 'F':
			type = optarg;
			break;
		case 'I':
			addopt("-I");
			break;
		case 'i':
			addopt("-i");
			addopt(optarg);
			break;
		case 'l':
			addopt("-l");
			break;
		case 'm':
			addopt("-m");
			addopt(optarg);
			break;
		case 'n':
			addopt("-n");
			addopt(optarg);
			break;
		case 'o':
			addopt("-o");
			addopt(optarg);
			break;
		case 'p':
			addopt("-p");
			addopt(optarg);
			break;
		case 's':
			addopt("-s");
			break;
		case 'u':
			addopt("-u");
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	argv += optind;
	if (*argv == NULL)
		usage();
	if ((newargv = calloc(nopts + 1, sizeof(*newargv))) == NULL)
		err(EX_OSERR, "calloc");
	i = 0;
	for (o = SLIST_FIRST(&optlh); o != NULL; o = next) {
		newargv[i++] = o->o_opt;
		next = SLIST_NEXT(o, o_next);
		free(o);
	}
	newargv[i] = NULL;
	status = 0;
	while (*argv != NULL) {
		tstatus = ff(*argv, newargv);
		if (!status)
			status = tstatus;
	}
	free(newargv);
	errno = 0;
	do {
		if (wait(&tstatus) == -1)
			if (errno != ECHILD)
				err(EX_OSERR, "wait");
		if (!status)
			status = tstatus;
	} while (errno != ECHILD);
	exit(status);
}

int
ff(char *path, char **argv)
{
	char **p, fn[MAXPATHLEN], *type;
	struct statfs sfs;
	int status;
	pid_t pid;

	if (statfs(path, &sfs) == -1) {
		warn("%s", path);
		return (EX_UNAVAILABLE);
	}
	type = sfs.f_fstypename;
	pid = fork();
	switch (pid) {
	case -1:
		err(EX_OSERR, "fork");
		/* NOTREACHED */
	case 0: /* Child. */
		for (p = searchpaths; *p != NULL; p++) {
			snprintf(fn, sizeof(fn), "%s/ff_%s", *p, type);
			execv(fn, argv);
			if (errno != ENOENT)
				warn("%s", fn);
		}
		warnx("%s: no support for file system type", type);
		exit(EX_UNAVAILABLE);
		/* NOTREACHED */
	}
	if (waitpid(pid, &status, WAIT_MYPGRP) == -1)
		err(EX_OSERR, "waitpid");
	return (EX_OK);
}

void
usage(void)
{
	extern char *__progname;

	(void)fprintf(stderr,
	    "usage: %s [-Ilsu] [-a n] [-c n] [-F type] [-i list] [-m n]\n"
	    "          [-n file] [-o option] [-p prefix]\n", __progname);
	exit(EX_USAGE);
}
