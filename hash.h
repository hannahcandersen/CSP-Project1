#ifndef HASH_H
#define HASH_H

#include "tuple.h"
#include <stddef.h>

size_t hash(Tuple *tuple, size_t hash_bits);

#endif