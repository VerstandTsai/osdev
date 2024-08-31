#ifndef _STDARG_H
#define _STDARG_H

typedef char* va_list;

#define rounded_sizeof(type) \
    ((sizeof(type) + sizeof(int) - 1) / sizeof(int) * sizeof(int))

#define va_start(ap, last) \
    ((ap) = (char*)&(last) + sizeof(last))

#define va_arg(ap, type) ( \
    (ap) += rounded_sizeof(type), \
    *(type*)((ap) - rounded_sizeof(type)) \
)

#define va_end(ap)

#endif // _STDARG_H

