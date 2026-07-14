/**
 * @file    c_json_list.h
 * @author  Thiago Silveira
 * @date    14/07/2026
 *
 * @copyright Copyright (c) 2026
 *
 * @brief   Cabeçalho de uma implementação de lista encadeada genérica (singly-linked list)
 * para armazenar ponteiros `void*`.
 */

#ifndef INC_C_JSON_LIST_H_
#define INC_C_JSON_LIST_H_

#include <stdbool.h>

/**
 * @brief Ponteiro opaco para a estrutura da lista.
 * Funciona como um handle para a lista encadeada.
 */
typedef struct st_list *P_C_JSON_LIST;

/**
 * @brief Define um tipo para uma função de callback usada para liberar a memória
 * dos dados customizados armazenados na lista.
 *
 * @details Como a lista armazena ponteiros `void*`, ela não sabe como liberar a memória
 * dos dados para os quais esses ponteiros apontam. Esta função de callback
 * permite que o código que criou os dados forneça a lógica de liberação correta.
 *
 * @param ptr O ponteiro para o dado customizado que deve ser liberado.
 */
typedef void (*c_json_list_cb_free)(void *ptr);

/**
 * @brief Cria e aloca uma nova lista encadeada vazia.
 *
 * @return Um ponteiro (`P_C_JSON_LIST`) para a nova lista, ou `NULL` em caso de falha
 * de alocação de memória.
 */
P_C_JSON_LIST c_json_list_new(void);

/**
 * @brief Libera toda a memória associada à lista, incluindo seus nós e,
 * opcionalmente, os dados apontados por cada nó.
 *
 * @param param   A lista a ser liberada.
 * @param cb_free Uma função de callback que sabe como liberar a memória de cada item
 * (`void *ptr`) armazenado na lista.
 * @note  Se `cb_free` for `NULL`, a memória dos dados apontados **não** será liberada,
 * apenas a estrutura da lista (nós e o cabeçalho) será liberada.
 *
 * @return `true` em caso de sucesso, `false` se a lista (`param`) for `NULL`.
 */
bool c_json_list_free(P_C_JSON_LIST param, c_json_list_cb_free cb_free);

/**
 * @brief Adiciona um novo ponteiro de dados ao final da lista.
 *
 * @attention A lista armazena apenas o ponteiro (`ptr`), ela **não** faz uma cópia do
 * dado. A responsabilidade pela alocação e validade da memória do dado
 * é do chamador durante todo o ciclo de vida da lista.
 *
 * @param param A lista onde o item será adicionado.
 * @param ptr   O ponteiro para o dado a ser armazenado.
 *
 * @return `true` em caso de sucesso, `false` se houver falha de alocação de um nó
 * ou se os parâmetros de entrada forem `NULL`.
 */
bool c_json_list_add(P_C_JSON_LIST param, void *ptr);

#endif  // INC_C_JSON_LIST_H_
