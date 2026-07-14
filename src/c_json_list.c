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
};

P_C_JSON_LIST c_json_list_new() {
    P_C_JSON_LIST param;

    param = c_json_mem_malloc(sizeof(struct st_list));
    if (param == NULL) {
        return NULL;
    }

    return param;
}

bool c_json_list_add(P_C_JSON_LIST param, void *ptr) {
    P_LIST_NODE node_new;

    if (param == NULL || ptr == NULL) {
        return false;
    }

    /// Alocando novo node
    node_new = c_json_mem_malloc(sizeof(struct st_list_node));
    if (node_new == NULL) {
        return false;
    }

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

    if (param == NULL) {
        return false;
    }

    node_current = param->node;
    while (node_current != NULL) {
        node_next = node_current->next;
        if (cb_free != NULL) {
            cb_free(node_current->ptr);
        }

        c_json_mem_free(node_current);
        node_current = node_next;
    }

    param->node = NULL;
    param->count = 0;

    c_json_mem_free(param);

    return true;
}
