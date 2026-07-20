/**
 * @file    c_json_mem.h
 * @author  Thiago Silveira
 * @date    13/07/2026
 *
 * @copyright Copyright (c) 2026
 *
 * @brief Alocador de memoria com limpeza segura na liberacao.
 *
 * @details Cada bloco alocado guarda o tamanho pedido em um cabecalho
 * `size_t` imediatamente antes do buffer retornado ao chamador. Na
 * liberacao, o conteudo e zerado com escrita `volatile` antes do `free`.
 */

#ifndef INC_C_JSON_MEM_H_
#define INC_C_JSON_MEM_H_

#include <stddef.h>

#ifndef C_JSON_API
#if defined(_WIN32)
#define C_JSON_API __declspec(dllexport)
#else
#define C_JSON_API extern
#endif
#endif

/**
 * @brief Aloca `len` bytes zerados (comportamento equivalente ao calloc do buffer).
 *
 * @details O bloco real contem `sizeof(size_t) + len` bytes. O tamanho `len`
 * e armazenado no cabecalho; o ponteiro retornado aponta para o inicio do buffer.
 *
 * @param len Quantidade de bytes solicitada para o buffer util.
 *
 * @return Ponteiro para o buffer, ou `NULL` em caso de falha/overflow.
 */
C_JSON_API void *c_json_mem_malloc(size_t len);

/**
 * @brief Limpa de forma segura e libera um bloco obtido por `c_json_mem_malloc`.
 *
 * @details Aceita `NULL` (no-op), no estilo do `free` ANSI C. O wipe cobre o
 * cabecalho e o buffer util antes de devolver a memoria ao heap.
 *
 * @param ptr Ponteiro retornado por `c_json_mem_malloc`, ou `NULL`.
 */
C_JSON_API void c_json_mem_free(void *ptr);

#endif /* INC_C_JSON_MEM_H_ */
