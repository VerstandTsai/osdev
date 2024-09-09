#ifndef _TERMIOS_H
#define _TERMIOS_H

// Output flags
#define ONLCR   1

// Local flags
#define ICANON  1
#define ECHO    2

// Number of special characters
#define NCCS 16

#define SPECIAL_CHARS \
    {-1}

// Special character indices
enum {
    VEOF,
    VEOL,
    VERASE,
    VINTR,
    VKILL,
    VQUIT,
    VSTART,
    VSTOP,
    VSUSP
};

typedef unsigned int tcflag_t;
typedef char cc_t;

struct termios {
    tcflag_t c_oflag;
    tcflag_t c_lflag;
    cc_t c_cc[NCCS];
};

#endif // _TERMIOS_H

