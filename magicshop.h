#ifndef MARISA_MAGICSHOP_H
#define MARISA_MAGICSHOP_H

#include <inttypes.h>
#include <magic.h>

magic_t magic_cookie;

u_int8_t *MarisaHttpd_GenerateDefaultErrorPage(u_int16_t errcode);
u_int8_t *MarisaHttpd_DetermineFiletype(u_int8_t *filepath);

#endif
