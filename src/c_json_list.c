/**
 * @file    c_json_list.c
 * @author  Thiago Silveira
 * @date    14/07/2026
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "c_json_mem.h"
#include "c_json_list.h"

typedef struct st_list_node {
    int index;
    void *ptr;
    struct st_list_node *next;
} *P_LIST_NODE;

struct st_list {
    int count;
    P_LIST_NODE node;  // Raiz da lista
    P_LIST_NODE tail;  // Ultimo ponteiro da lista
    c_json_list_malloc_fn malloc_fn;
    c_json_list_free_fn free_fn;
};

P_C_JSON_LIST c_json_list_new_with_alloc(c_json_list_malloc_fn malloc_fn, c_json_list_free_fn free_fn) {
    P_C_JSON_LIST param;

    if (malloc_fn == NULL || free_fn == NULL) {
        return NULL;
    }

    param = malloc_fn(sizeof(struct st_list));
    if (param == NULL) {
        return NULL;
    }

    memset(param, 0, sizeof(struct st_list));
    param->malloc_fn = malloc_fn;
    param->free_fn = free_fn;

    return param;
}

P_C_JSON_LIST c_json_list_new(void) {
    return c_json_list_new_with_alloc(c_json_mem_malloc, c_json_mem_free);
}

bool c_json_list_add(P_C_JSON_LIST param, void *ptr) {
    P_LIST_NODE node_new;

    if (param == NULL || ptr == NULL) {
        return false;
    }

    /// Alocando novo node
    node_new = param->malloc_fn(sizeof(struct st_list_node));
    if (node_new == NULL) {
        return false;
    }

    memset(node_new, 0, sizeof(struct st_list_node));

    /// Preenchando dados
    node_new->ptr = ptr;
    node_new->next = NULL;
    node_new->index = param->count;

    if (param->node == NULL) {  /// Adicionando primeiro node
        param->node = node_new;
        param->tail = node_new;
    } else {  /// Seguintes nodes
        param->tail->next = node_new;
        param->tail = node_new;
    }

    param->count++;
    return true;
}

bool c_json_list_free(P_C_JSON_LIST param, c_json_list_cb_free cb_free) {
    P_LIST_NODE node_next;
    P_LIST_NODE node_current;
    c_json_list_free_fn free_fn;

    if (param == NULL) {
        return false;
    }

    free_fn = param->free_fn;
    node_current = param->node;
    while (node_current != NULL) {
        node_next = node_current->next;
        if (cb_free != NULL) {
            cb_free(node_current->ptr);
        }

        free_fn(node_current);
        node_current = node_next;
    }

    param->node = NULL;
    param->count = 0;

    free_fn(param);

    return true;
}
