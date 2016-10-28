// itoa
// modified version of code supplied at
// http://opensource.apple.com//source/groff/groff-12/groff/libgroff/itoa.c
char*
itoa(int i, char *buffer, int len)
{
	
	char *p = buffer + len - 1;	/* points to terminating '\0' */
	*p = '\0';
	if (i >= 0) {
		do {
			*--p = '0' + (i % 10);
			i /= 10;
		} while (i != 0);
		return p;
	
	} else {			/* i < 0 */
		do {
			*--p = '0' - (i % 10);
		    i /= 10;
		} while (i != 0);
			*--p = '-';
	    }
	return p;
}