#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#define FLAG_ALT 1
#define FLAG_ZERO 2
#define FLAG_LEFT 4
#define FLAG_BLANK 8
#define FLAG_SIGN 16

enum LengthModifier {
    LEN_NONE,
    LEN_CHAR,
    LEN_SHORT,
    LEN_LONG,
    LEN_LLONG,
    LEN_LDOUBLE,
    LEN_SIZE,
    LEN_PTRDIFF
};

static int read_flags(const char **strref) {
    int flags = 0;
    for (int loop=1; loop; (*strref)++) {
        switch (**strref) {
        case '#': flags |= FLAG_ALT; break;
        case '0': flags |= FLAG_ZERO; break;
        case '-': flags |= FLAG_LEFT; break;
        case ' ': flags |= FLAG_BLANK; break;
        case '+': flags |= FLAG_SIGN; break;
        default: loop = 0;
        }
    }
    (*strref)--;
    return flags;
}

static int read_int(const char **strref) {
    int number = 0;
    for (; **strref >= '0' && **strref <= '9'; (*strref)++)
        number = number * 10 + **strref - '0';
    return number;
}

static enum LengthModifier read_length(const char **strref) {
    switch (**strref) {
    case 'h':
        (*strref)++;
        if (**strref == 'h') {
            (*strref)++;
            return LEN_CHAR;
        } else return LEN_SHORT;
    case 'l':
        (*strref)++;
        if (**strref == 'l') {
            (*strref)++;
            return LEN_LLONG;
        } else return LEN_LONG;
    case 'L': (*strref)++; return LEN_LDOUBLE;
    case 'z': (*strref)++; return LEN_SIZE;
    case 't': (*strref)++; return LEN_PTRDIFF;
    default: return LEN_NONE;
    }
}

static void lludiv(
    unsigned long long dividend,
    unsigned long long divisor,
    unsigned long long *quotient,
    unsigned long long *remainder
) {
    unsigned long long sub = divisor;
    while (!(sub >> 63)) sub <<= 1;
    *quotient = 0;
    *remainder = dividend;
    while (sub >= divisor) {
        *quotient <<= 1;
        if (sub <= *remainder) {
            *remainder -= sub;
            *quotient |= 1;
        }
        sub >>= 1;
    }
}

static char *itoa(unsigned long long number, char *str, int base, int precision) {
    char buffer[256];
    int i = 0;
    while (precision-- > 0 || number) {
        unsigned long long remainder;
        if (number > 0xffffffff) {
            lludiv(number, base, &number, &remainder);
        } else {
            unsigned int temp = number;
            remainder = temp % base;
            number = temp / base;
        }
        buffer[i++] = remainder + (remainder > 9 ? 'a'-10 : '0');
    }
    char *ptr;
    for (ptr=str; i--; ptr++) *ptr = buffer[i];
    *ptr = '\0';
    return str;
}

int vsprintf(char *str, const char *format, va_list ap) {
    *str = '\0';
    while (*format) {
        // Normal characters
        if (*format != '%') {
            strncat(str, format++, 1);
            continue;
        }

        // "%%"
        if (*++format == '%') {
            strncat(str, format++, 1);
            continue;
        }

        // Format string

        // Parse the format string
        int flags = read_flags(&format);
        int field_width = read_int(&format);
        int precision = -1;
        if (*format == '.') {
            format++;
            precision = read_int(&format);
        }
        enum LengthModifier length = read_length(&format);
        char conversion = *format++;

        // Read the argument and convert

#define READ_SIGNED(dest, type) { \
    type _signed = va_arg(ap, type); \
    if (_signed < 0) { \
        negative = 1; \
        dest = -_signed; \
    } else dest = _signed; \
}

#define READ_UNSIGNED(dest, type) { \
    type _unsigned = va_arg(ap, type); \
    dest = _unsigned; \
}

#define READ_INTEGER(dest, issigned, type) { \
    if (issigned) { READ_SIGNED(dest, type); } \
    else { READ_UNSIGNED(dest, unsigned type); } \
}

        char buffer[256] = "";
        char prefix[4] = "";
        int issigned = 0;
        int negative = 0;
        switch (conversion) {
        case 'd':
        case 'i':
        case 'u':
        case 'o':
        case 'x':
        case 'X':
            {
                unsigned long long number;
                issigned = conversion == 'd' || conversion == 'i' ? 1 : 0;
                switch (length) {
                case LEN_NONE:  READ_INTEGER(number, issigned, int      ); break;
                case LEN_CHAR:  READ_INTEGER(number, issigned, char     ); break;
                case LEN_SHORT: READ_INTEGER(number, issigned, short    ); break;
                case LEN_LONG:  READ_INTEGER(number, issigned, long     ); break;
                case LEN_LDOUBLE:
                case LEN_LLONG: READ_INTEGER(number, issigned, long long); break;
                case LEN_SIZE:
                    if (issigned) { READ_SIGNED(number, int); }
                    else { READ_UNSIGNED(number, size_t); }
                    break;
                case LEN_PTRDIFF:
                    if (issigned) { READ_SIGNED(number, ptrdiff_t); }
                    else { READ_UNSIGNED(number, unsigned int); }
                    break;
                }

                int base = 10;
                if (conversion == 'o') base = 8;
                if (conversion == 'x' || conversion == 'X') base = 16;
                itoa(number, buffer, base, precision < 0 ? 1 : precision);
            }

            if (conversion == 'X')
                for (char *ptr=buffer; *ptr; ptr++)
                    if (*ptr >= 'a' && *ptr <= 'f')
                        *ptr += 'A' - 'a';

            if (flags & FLAG_ALT) {
                switch (conversion) {
                case 'o': strcpy(prefix, "0"); break;
                case 'x': strcpy(prefix, "0x"); break;
                case 'X': strcpy(prefix, "0X"); break;
                }
            }
            break;
        case 'c':
            buffer[0] = va_arg(ap, char);
            buffer[1] = '\0';
            break;
        case 's':
            strcpy(buffer, va_arg(ap, char*));
            break;
        case 'p':
            itoa((unsigned)va_arg(ap, void*), buffer, 16, 1);
            strcpy(prefix, "0x");
            break;
        }

        if (issigned) {
            if (negative) strcpy(prefix, "-");
            else if (flags & FLAG_SIGN) strcpy(prefix, "+");
        }

        char padding[256] = "";
        int pad = field_width - strlen(prefix) - strlen(buffer);
        if (pad < 0) pad = 0;

        // Concatenate the result
        if (flags & FLAG_LEFT) {
            memset(padding, ' ', pad);
            strcat(str, prefix);
            strcat(str, buffer);
            strcat(str, padding);
        } else if (flags & FLAG_ZERO) {
            memset(padding, '0', pad);
            strcat(str, prefix);
            strcat(str, padding);
            strcat(str, buffer);
        } else {
            memset(padding, ' ', pad);
            strcat(str, padding);
            strcat(str, prefix);
            strcat(str, buffer);
        }
    }
    return strlen(str);
}

