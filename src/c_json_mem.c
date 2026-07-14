/**
 * @file    c_json_mem.c
 * @author  Thiago Silveira
 * @date    13/07/2026
 *
 * @copyright Copyright (c) 2026
 *
 * @brief Implementacao do alocador com wipe seguro.
 */

#include <stdint.h>
#include <stdlib.h>

#include "c_json_mem.h"

/**
 * @brief Sobrescreve `len` bytes com zero usando ponteiro volatile.
 *
 * @details Impede que o otimizador elimine a limpeza como dead-store.
 *
 * @param ptr Endereco inicial a limpar.
 * @param len Quantidade de bytes.
 */
static void c_json_private_mem_wipe(void *ptr, size_t len) {
    volatile unsigned char *p;
    size_t i;

    if (ptr == NULL || len == 0) {
        return;
    }

    p = (volatile unsigned char *)ptr;
    for (i = 0; i < len; i++) {
        p[i] = 0;
    }
}

void *c_json_mem_malloc(size_t len) {
    size_t total;
    size_t *header;

    if (len > SIZE_MAX - sizeof(size_t)) {
        return NULL;
    }

    total = sizeof(size_t) + len;
    header = (size_t *)calloc(1, total);
    if (header == NULL) {
        return NULL;
    }

    *header = len;
    return (void *)(header + 1);
}

void c_json_mem_free(void *ptr) {
    size_t *header;
    size_t len;
    size_t total;

    if (ptr == NULL) {
        return;
    }

    header = ((size_t *)ptr) - 1;
    len = *header;
    total = sizeof(size_t) + len;

    c_json_private_mem_wipe(header, total);
    free(header);
}
