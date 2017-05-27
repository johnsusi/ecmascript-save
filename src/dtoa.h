#ifndef DTOA_H
#define DTOA_H

#ifdef __cplusplus

extern "C" char *dtoa(double d, int mode, int ndigits, int *decpt, int *sign,
                      char **rve);
extern "C" void freedtoa(char *);

extern "C" double strtod(const char *s00, char **se);

#endif

#endif