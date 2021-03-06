#include <limits.h>
#include <std.h>
#include <stdarg.h>

#include <std/string.h>

/* Return the `ldiv_t' representation of NUMER over DENOM.  */
ldiv_t ldiv (unsigned long int numer, unsigned long int denom)
{
    ldiv_t result;

    result.quot = numer / denom;
    result.rem = numer % denom;

  /* The ANSI standard says that |QUOT| <= |NUMER / DENOM|, where
     NUMER / DENOM is to be computed in infinite precision.  In
     other words, we should always truncate the quotient towards
     zero, never -infinity.  Machine division and remainer may
     work either way when one or both of NUMER or DENOM is
     negative.  If only one is negative and QUOT has been
     truncated towards -infinity, REM will have the same sign as
     DENOM and the opposite sign of NUMER; if both are negative
     and QUOT has been truncated towards -infinity, REM will be
     positive (will have the opposite sign of NUMER).  These are
     considered `wrong'.  If both are NUM and DENOM are positive,
     RESULT will always be positive.  This all boils down to: if
     NUMER >= 0, but REM < 0, we got the wrong answer.  In that
     case, to get the right answer, add 1 to QUOT and subtract
     DENOM from REM.  */

    if (numer && result.rem < 0)
    {
        ++result.quot;
        result.rem -= denom;
    }

    return result;
}


#define BUFSIZE (sizeof(unsigned long long) * 8 + 1)

char *ltoa(unsigned long long N, char *str, int base)
{
      register int i = 2;
      unsigned long long uarg;
      char *tail, *head = str, buf[BUFSIZE] = {0};

      if (36 < base || 2 > base)
            base = 10;                    /* can only use 0-9, A-Z        */
      tail = &buf[BUFSIZE - 1];           /* last character position      */
      *tail-- = '\0';

      if (10 == base)
      {
            *head++ = '-';
            uarg    = -N;
      }
      else  uarg = N;

      if (uarg)
      {
            for (i = 1; uarg; ++i)
            {
                  register ldiv_t res;

                  res       = ldiv(uarg, base);
                  *tail-- = (char)(res.rem + ((9L < res.rem) ?
                                  ('A' - 10L) : '0'));
                  uarg    = res.quot;
            }
      }
      else  *tail-- = '0';

      memcpy(head, ++tail, i);
      return str;
}

int itoa(int value, char *sp, int radix) {
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);    
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix;
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign) 
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp) {
        *sp++ = *--tp;
	}

	*sp = '\0';
    return len;
}

int putchar(int ic) {
	char c = (char) ic;
	//terminal_write(&c, sizeof(c));
	return ic;
}


int atoi(char* str) {
	int returnVal = 0;

	int multiplier = 1;

	if(str[0] == '-') 
		multiplier = -1;

	for(int i = 0; i < strlen(str); i++) {
		returnVal = returnVal*10 + str[i] - '0';
	}

	return returnVal * multiplier;
}


static bool print(char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == -1)
			return false;
	return true;
}

int sprintf(char* buffer, const char* format, ...) {
	va_list parameters;
	va_start(parameters, format);

	// set the buffer to zero before any writing
	int i = 0;
	while(buffer[i] != 0) { buffer[i] = 0; i++; }

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		buffer[written] = format[0];

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%') {
				buffer[written+amount] = format[amount];
				amount++;
			}
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print((char*)format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		char* format_begun_at = (char*)(format++);

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int);
			buffer[written] = c;
			written++;
		} 
		else if (*format == 'x') {
			format++;

			char fmt_buffer[16] = {0};
			ltoa(va_arg(parameters, int), fmt_buffer, 16);

			// buffer[written] = '0';
			// buffer[written+1] = 'x';

			size_t amount = 0;
			while (fmt_buffer[amount] != '\0') {
				buffer[written+amount] = fmt_buffer[amount];
				amount++;
			}

			written+=amount;
		} 
		else if (*format == 'd' || *format == 'i') {
			format++;

			char fmt_buffer[16] = {0};
			itoa(va_arg(parameters, int), fmt_buffer, 10);

			size_t amount = 0;
			while (fmt_buffer[amount] != '\0') {
				buffer[written+amount] = fmt_buffer[amount];
				amount++;
			}

			written+=amount;
		} 
		else if (*format == 's') {
			format++;
			string str = va_arg(parameters, string);

			written += str.size();
		} 
		else {
			format = format_begun_at;
			size_t len = strlen((char*)format);

			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}


int vsprintf(char* buffer, const char* format, va_list parameters) {
	// set the buffer to zero before any writing
	int i = 0;
	while(buffer[i] != 0) { buffer[i] = 0; i++; }

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		buffer[written] = format[0];

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%') {
				buffer[written+amount] = format[amount];
				amount++;
			}
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print((char*)format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		char* format_begun_at = (char*)(format++);

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int);
			buffer[written] = c;
			written++;
		} 
		else if (*format == 'x') {
			format++;

			char fmt_buffer[16] = {0};

			ltoa(va_arg(parameters, int), fmt_buffer, 16);

			buffer[written] = '0';
			buffer[written+1] = 'x';

			size_t amount = 0;
			while (fmt_buffer[amount] != '\0') {
				buffer[written+amount+2] = fmt_buffer[amount];
				amount++;
			}

			written+=amount+2;
		} 
		else if (*format == 'd' || *format == 'i') {
			format++;

			char fmt_buffer[16] = {0};
			itoa(va_arg(parameters, int), fmt_buffer, 10);

			size_t amount = 0;
			while (fmt_buffer[amount] != '\0') {
				buffer[written+amount] = fmt_buffer[amount];
				amount++;
			}

			written+=amount;
			
		} 
		else if (*format == 's') {
			format++;
			char * str = va_arg(parameters, char*);

			size_t amount = 0;
			while (str[amount] != '\0') {
				buffer[written+amount] = str[amount];
				amount++;
			}

			written += amount;
		} 
		else {
			format = format_begun_at;
			size_t len = strlen((char*)format);

			written += len;
			format += len;
		}
	}
	return written;
}
