/* $Id$ */

int
main(int argc, char *argv[])
{
	int c;

	while ((c = getopt(argc, argv, "a:c:F:Ii:lm:n:o:p:suV")) != -1)
		switch (c) {
		case 'a':
			break;
		case 'c':
			break;
		case 'F':
			break;
		case 'I':
			break;
		case 'i':
			break;
		case 'l':
			break;
		case 'm':
			break;
		case 'n':
			break;
		case 'o':
			break;
		case 'p':
			break;
		case 's':
			break;
		case 'u':
			break;
		case 'V':
			break;
		}
	argv += optind;
}

void
usage(void)
{
	extern char *__progname;

	(void)fprintf(stderr,
	    "usage: %s [-IlsuV] [-a n] [-c n] [-F type] [-i list] [-m n]\n"
	    "          [-n file] [-o option] [-p prefix]\n", __progname);
	exit(EX_USAGE);
}
