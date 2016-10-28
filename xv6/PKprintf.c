#include "types.h"
#include "user.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
pad(int fd, int size)
{
  while (--size >= 0)
    putc(fd, ' ');

}

static int
isnumeric(char c)
{
  return c >= '0' && c <= '9';
}

static int
getpad(char *fmt, int *offset)
{

  int i = 0;
  int value = 0;

  if (fmt == 0) return 0;

  while (fmt != 0 && isnumeric(*fmt)) {
    value = value * 10 + (*fmt - '0');
    ++fmt;
    ++i;
  }

  *offset += i;
  return value;

}

static int
printstr(int fd, char *s)
{
  int size = 0;
  for (; *s; s++) {
    size++;
    putc(fd, *s);
  }
  return size;
}



static int
printint(int fd, int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg, size;
  uint x;

  size = 0;
  neg = 0;
  if (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);
  if (neg)
    buf[i++] = '-';

  while (--i >= 0) {
    putc(fd, buf[i]);
    ++size;
  }

  return size;
}

static int
print_float (int fd, int secs, int partial_secs)
{
  char buf1[20];
  char buf2[20];
  char buf3[40];
  char *s1;
  char *s2;

  //int len;

  // convert secs to a ascii chars starting at buf[0]
  s1 = itoa(secs, buf1, 20);

  s2 = itoa(partial_secs, buf2, 20);

  if (partial_secs < 10) {
    concat(s1, ".0", buf3);
  } else {
    concat(s1, ".", buf3);
  }

  //concat(s1, ".", buf3);
  concat(buf3, s2, buf3);

  return printstr(fd, buf3);

}

// Print to the given fd. Only understands %d, %x, %p, %s, %f.
void
pkprintf(int fd, char *fmt, ...)
{
  char *s;
  int c,
      i,
      state,
      prefix,
      size;

  int
  f1, // floating point integers
  f2, // floating point fraction
  fState; // 0 if f1 & f1 not set, 1 if f1 set

  uint *ap;

  fState = 0;
  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for (i = 0; fmt[i]; i++) {
    c = fmt[i] & 0xff;
    if (state == 0) {
      if (c == '%') {
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if (state == '%') {
      if (c == 'd') {
        size = printint(fd, *ap, 10, 1);
        ap++;
        if (prefix != 0) {
          pad(fd, MAX(prefix - size, 0));
          prefix = 0;
        }
      } else if (c == 'x' || c == 'p') {
        size = printint(fd, *ap, 16, 0);
        ap++;
        if (prefix != 0) {
          pad(fd, MAX(prefix - size, 0));
          prefix = 0;
        }
      } else if (c == 's') {
        s = (char*)*ap;
        ap++;
        if (s == 0)
          s = "(null)";

        size = printstr(fd, s);

        if (prefix != 0) {
          pad(fd, MAX(prefix - size, 0));
          prefix = 0;
        }
      } else if (c == 'c') {
        putc(fd, *ap);
        ap++;
      } else if (c == '%') {
        putc(fd, c);
      } else if (c == '-') {
        prefix = getpad(fmt + i + 1, &i);
      } else if (c == 'f') {
        if (fState == 0) {
          f1 = *ap;
          ++ap;
          fState = 1;
        } else if (fState == 1) {
          f2 = *ap;
          ++ap;
          fState = 0;

          size = print_float(fd, f1, f2);
          if (prefix != 0) {
            pad(fd, MAX(prefix - size, 0));
            prefix = 0;
          }
        }

      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }


      state = 0;
    }
  }
}
