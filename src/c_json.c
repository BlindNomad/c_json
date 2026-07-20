/**
 * @file    c_json.c
 * @author  Thiago Silveira
 *
 * @brief   Biblioteca JSON que encapsula a cJSON
 *
 * @version 1.0
 * @date    14/07/2026
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#include "c_json_list.h"
#include "c_json_mem.h"
#include "c_json.h"

/* Declaracao interna: lista com alocadores customizados (nao exportada no .h). */
P_C_JSON_LIST c_json_list_new_with_alloc(void *(*malloc_fn)(size_t), void (*free_fn)(void *));

// ============================================================
//  DEFINES
// ============================================================

#define MAX_JSON_LEN 1048576  // 1024 * 1024

// ============================================================
//  ESTRUTURA DE DADOS
// ============================================================

struct c_json {
    char *json_string;
    cJSON *json_object;
    P_C_JSON_LIST json_object_getted;
    E_C_JSON_CONFIG config;
    cJSON_Hooks hooks;
};

// ============================================================
//  FUNCOES PRIVADAS E UTILITARIAS
// ============================================================

/**
 * @brief   Funcao interna para processar o to_string e format
 *
 * @param   @c  param   Parametros do JSON
 * @param   @c  escape  Indentacao do JSON a ser exportado
 *
 * @return  @c  char* Buffer alocado a ser retornado
 */
char *c_json_private_apply_format(C_JSON param, E_C_JSON_STR_ESCAPE escape) {
    char *out = NULL;

    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    switch (escape) {
        case C_JSON_STR_FORMATTED:
            out = cJSON_Print(param->json_object);
            break;
        case C_JSON_STR_UNFORMATTED:
        case C_JSON_STR_UNFORMATTED_WO_SPACE:
            out = cJSON_PrintUnformatted(param->json_object);
            break;
        case C_JSON_STR_UNFORMATTED_WO_SPACE_WO_ESCAPE:
            out = cJSON_PrintUnformatted(param->json_object);
            if (out != NULL) {
                cJSON_Minify(out);
            }
            break;
        default:
            goto escape_invalid;
    }

    return out;

escape_invalid:
json_object_null:
param_not_initialized:
    return NULL;
}

/**
 * @brief   Funcao que valida se TAG ja foi adicionado ou nao
 *
 * @param   @c param    Parametros do JSON
 * @param   @c TAG      Identificacao da
 *
 * @return  @c true     Tag Existe
 * @return  @c false    Tag Nao Existe
 */
bool c_json_private_verify_if_exist(C_JSON param, const char *TAG) {
    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    return cJSON_GetObjectItemCaseSensitive(param->json_object, TAG) != NULL;

json_object_null:
param_not_initialized:
tag_null:
    return false;
}

/**
 * @brief Cria um C_JSON com alocadores por instancia (uso interno).
 */
static C_JSON c_json_private_new_with_hooks(const cJSON_Hooks *hooks) {
    C_JSON param;

    if (hooks == NULL) {
        goto hooks_null;
    }
    if (hooks->malloc_fn == NULL) {
        goto malloc_fn_null;
    }
    if (hooks->free_fn == NULL) {
        goto free_fn_null;
    }

    param = hooks->malloc_fn(sizeof(struct c_json));
    if (param == NULL) {
        goto alloc_failed;
    }

    memset(param, 0, sizeof(struct c_json));
    param->hooks = *hooks;
    param->json_string = NULL;
    param->json_object = cJSON_CreateObjectWithHooks(&param->hooks);
    if (param->json_object == NULL) {
        goto json_object_alloc_failed;
    }

    param->json_object_getted = c_json_list_new_with_alloc(param->hooks.malloc_fn, param->hooks.free_fn);
    if (param->json_object_getted == NULL) {
        goto list_alloc_failed;
    }

    c_json_set_config(param, C_JSON_CONFIG_UPDATE_SAME_TAG);

    return param;

list_alloc_failed:
    cJSON_Delete(param->json_object);
    param->json_object = NULL;
json_object_alloc_failed:
    param->hooks.free_fn(param);
alloc_failed:
free_fn_null:
malloc_fn_null:
hooks_null:
    return NULL;
}

C_JSON c_json_new(void) {
    cJSON_Hooks hooks;

    hooks.malloc_fn = c_json_mem_malloc;
    hooks.free_fn = c_json_mem_free;
    return c_json_private_new_with_hooks(&hooks);
}

void c_json_free_cb(void *ptr) {
    c_json_free(ptr);
}

C_JSON c_json_free(C_JSON param) {
    cJSON_Hooks hooks;

    if (param == NULL) {
        return NULL;
    }

    hooks = param->hooks;

    if (param->json_object != NULL) {
        cJSON_Delete(param->json_object);
    }

    if (param->json_string != NULL) {
        hooks.free_fn(param->json_string);
    }

    if (param->json_object_getted != NULL) {
        c_json_list_free(param->json_object_getted, c_json_free_cb);
    }

    hooks.free_fn(param);

    return NULL;
}

bool c_json_parser(C_JSON param, const char *value) {
    cJSON *parsed;

    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }

    if (param->json_object != NULL) {
        cJSON_Delete(param->json_object);
        param->json_object = NULL;
    }

    parsed = cJSON_ParseWithHooks(value, &param->hooks);
    if (parsed == NULL) {
        goto parse_failed;
    }

    param->json_object = parsed;

    return true;

parse_failed:
param_not_initialized:
value_null:
    return false;
}

bool c_json_serialize(C_JSON param, char **value, size_t *size, E_C_JSON_STR_ESCAPE escape) {
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    if (param->json_string != NULL) {
        param->hooks.free_fn(param->json_string);
        param->json_string = NULL;
    }

    *value = c_json_private_apply_format(param, escape);
    if (*value == NULL) {
        goto format_failed;
    }

    param->json_string = *value;

    if (size != NULL) {
        *size = strnlen(param->json_string, MAX_JSON_LEN);
    }

    return true;

format_failed:
json_object_null:
value_null:
param_not_initialized:
    return false;
}

const char *c_json_version(void) {
    return JSON_LIB_VERSION;
}

bool c_json_print(C_JSON param, E_C_JSON_STR_ESCAPE escape) {
    char *out;

    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    out = c_json_private_apply_format(param, escape);
    if (out == NULL) {
        goto format_failed;
    }

    printf("%s\n", out);
    param->hooks.free_fn(out);

    return true;

format_failed:
json_object_null:
param_not_initialized:
    return false;
}

bool c_json_set_config(C_JSON param, E_C_JSON_CONFIG config) {
    if (param == NULL) {
        goto param_not_initialized;
    }

    switch (config) {
        case C_JSON_CONFIG_ALLOW_SAME_TAG:
        case C_JSON_CONFIG_UPDATE_SAME_TAG:
        case C_JSON_CONFIG_ERROR_SAME_TAG:
            param->config = config;
            return true;
        default:
            goto config_invalid;
    }

config_invalid:
param_not_initialized:
    return false;
}

bool c_json_add_boolean(C_JSON param, const char *TAG, bool value) {
    bool exist;
    cJSON *p_json;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    exist = c_json_private_verify_if_exist(param, TAG);
    if (exist == true && param->config == C_JSON_CONFIG_ERROR_SAME_TAG) {
        goto same_tag_error;
    }
    if (exist == true && param->config == C_JSON_CONFIG_UPDATE_SAME_TAG) {
        cJSON_DeleteItemFromObjectCaseSensitive(param->json_object, TAG);
    }

    p_json = cJSON_AddBoolToObject(param->json_object, TAG, value);
    if (p_json == NULL) {
        goto add_bool_failed;
    }

    return true;

add_bool_failed:
same_tag_error:
json_object_null:
param_not_initialized:
tag_null:
    return false;
}

bool c_json_add_string(C_JSON param, const char *TAG, const char *value) {
    bool exist;
    cJSON *p_json;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    exist = c_json_private_verify_if_exist(param, TAG);
    if (exist == true && param->config == C_JSON_CONFIG_ERROR_SAME_TAG) {
        goto same_tag_error;
    }
    if (exist == true && param->config == C_JSON_CONFIG_UPDATE_SAME_TAG) {
        cJSON_DeleteItemFromObjectCaseSensitive(param->json_object, TAG);
    }

    p_json = cJSON_AddStringToObject(param->json_object, TAG, value);
    if (p_json == NULL) {
        goto add_string_failed;
    }

    return true;

add_string_failed:
same_tag_error:
json_object_null:
value_null:
param_not_initialized:
tag_null:
    return false;
}

bool c_json_add_object(C_JSON param, const char *TAG, C_JSON value) {
    bool exist;
    cJSON *copy;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    exist = c_json_private_verify_if_exist(param, TAG);
    if (exist == true && param->config == C_JSON_CONFIG_ERROR_SAME_TAG) {
        goto same_tag_error;
    }
    if (exist == true && param->config == C_JSON_CONFIG_UPDATE_SAME_TAG) {
        cJSON_DeleteItemFromObjectCaseSensitive(param->json_object, TAG);
    }

    copy = cJSON_DuplicateWithHooks(value->json_object, 1, &param->hooks);
    if (copy == NULL) {
        goto duplicate_failed;
    }

    if (!cJSON_AddItemToObject(param->json_object, TAG, copy)) {
        goto add_item_failed;
    }

    return true;

add_item_failed:
    cJSON_Delete(copy);
duplicate_failed:
same_tag_error:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

bool c_json_add_number(C_JSON param, const char *TAG, const double value) {
    bool exist;
    cJSON *p_json;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    exist = c_json_private_verify_if_exist(param, TAG);
    if (exist == true && param->config == C_JSON_CONFIG_ERROR_SAME_TAG) {
        goto same_tag_error;
    }
    if (exist == true && param->config == C_JSON_CONFIG_UPDATE_SAME_TAG) {
        cJSON_DeleteItemFromObjectCaseSensitive(param->json_object, TAG);
    }

    p_json = cJSON_AddNumberToObject(param->json_object, TAG, value);
    if (p_json == NULL) {
        goto add_number_failed;
    }

    return true;

add_number_failed:
same_tag_error:
json_object_null:
param_not_initialized:
tag_null:
    return false;
}

bool c_json_add_char(C_JSON param, const char *TAG, char value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_u_char(C_JSON param, const char *TAG, unsigned char value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_short(C_JSON param, const char *TAG, int16_t value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_u_short(C_JSON param, const char *TAG, uint16_t value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_int(C_JSON param, const char *TAG, int32_t value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_u_int(C_JSON param, const char *TAG, uint32_t value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_long(C_JSON param, const char *TAG, int64_t value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_u_long(C_JSON param, const char *TAG, uint64_t value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_double(C_JSON param, const char *TAG, double value) {
    return c_json_add_number(param, TAG, (double)value);
}

bool c_json_add_array_boolean(C_JSON param, const char *TAG, bool value) {
    cJSON *json_value = NULL;
    cJSON *json_array = NULL;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    json_array = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (json_array == NULL) {
        json_array = cJSON_CreateArrayWithHooks(&param->hooks);
        if (json_array == NULL) {
            goto create_array_failed;
        }
        if (!cJSON_AddItemToObject(param->json_object, TAG, json_array)) {
            goto add_array_to_object_failed;
        }
    } else if (cJSON_IsArray(json_array) == false) {
        goto item_not_array;
    }

    json_value = cJSON_CreateBoolWithHooks(value, &param->hooks);
    if (json_value == NULL) {
        goto create_bool_failed;
    }

    if (!cJSON_AddItemToArray(json_array, json_value)) {
        goto add_bool_to_array_failed;
    }

    return true;

add_bool_to_array_failed:
    cJSON_Delete(json_value);
create_bool_failed:
item_not_array:
json_object_null:
param_not_initialized:
tag_null:
create_array_failed:
    return false;

add_array_to_object_failed:
    cJSON_Delete(json_array);
    return false;
}

bool c_json_add_array_object(C_JSON param, const char *TAG, int type, C_JSON value) {
    cJSON *copy = NULL;
    cJSON *json_array = NULL;

    (void)type;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }
    if (value->json_object == NULL) {
        goto value_json_object_null;
    }

    json_array = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (json_array == NULL) {
        json_array = cJSON_CreateArrayWithHooks(&param->hooks);
        if (json_array == NULL) {
            goto create_array_failed;
        }
        if (!cJSON_AddItemToObject(param->json_object, TAG, json_array)) {
            goto add_array_to_object_failed;
        }
    } else if (cJSON_IsArray(json_array) == false) {
        goto item_not_array;
    }

    copy = cJSON_DuplicateWithHooks(value->json_object, 1, &param->hooks);
    if (copy == NULL) {
        goto duplicate_failed;
    }

    if (!cJSON_AddItemToArray(json_array, copy)) {
        goto add_copy_to_array_failed;
    }

    return true;

add_copy_to_array_failed:
    cJSON_Delete(copy);
duplicate_failed:
item_not_array:
value_json_object_null:
json_object_null:
param_not_initialized:
value_null:
tag_null:
create_array_failed:
    return false;

add_array_to_object_failed:
    cJSON_Delete(json_array);
    return false;
}

bool c_json_add_array_string(C_JSON param, const char *TAG, const char *value) {
    cJSON *json_value = NULL;
    cJSON *json_array = NULL;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    json_array = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (json_array == NULL) {
        json_array = cJSON_CreateArrayWithHooks(&param->hooks);
        if (json_array == NULL) {
            goto create_array_failed;
        }
        if (!cJSON_AddItemToObject(param->json_object, TAG, json_array)) {
            goto add_array_to_object_failed;
        }
    } else if (cJSON_IsArray(json_array) == false) {
        goto item_not_array;
    }

    json_value = cJSON_CreateStringWithHooks(value, &param->hooks);
    if (json_value == NULL) {
        goto create_string_failed;
    }

    if (!cJSON_AddItemToArray(json_array, json_value)) {
        goto add_string_to_array_failed;
    }

    return true;

add_string_to_array_failed:
    cJSON_Delete(json_value);
create_string_failed:
item_not_array:
json_object_null:
param_not_initialized:
tag_null:
create_array_failed:
    return false;

add_array_to_object_failed:
    cJSON_Delete(json_array);
    return false;
}

bool c_json_add_array_number(C_JSON param, const char *TAG, double value) {
    cJSON *json_array = NULL;
    cJSON *json_value = NULL;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    json_array = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (json_array == NULL) {
        json_array = cJSON_CreateArrayWithHooks(&param->hooks);
        if (json_array == NULL) {
            goto create_array_failed;
        }
        if (!cJSON_AddItemToObject(param->json_object, TAG, json_array)) {
            goto add_array_to_object_failed;
        }
    } else if (cJSON_IsArray(json_array) == false) {
        goto item_not_array;
    }

    json_value = cJSON_CreateNumberWithHooks(value, &param->hooks);
    if (json_value == NULL) {
        goto create_number_failed;
    }

    if (!cJSON_AddItemToArray(json_array, json_value)) {
        goto add_number_to_array_failed;
    }

    return true;

add_number_to_array_failed:
    cJSON_Delete(json_value);
create_number_failed:
item_not_array:
json_object_null:
param_not_initialized:
tag_null:
create_array_failed:
    return false;

add_array_to_object_failed:
    cJSON_Delete(json_array);
    return false;
}

bool c_json_add_array_char(C_JSON param, const char *TAG, char value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_u_char(C_JSON param, const char *TAG, unsigned char value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_short(C_JSON param, const char *TAG, int16_t value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_u_short(C_JSON param, const char *TAG, uint16_t value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_int(C_JSON param, const char *TAG, int32_t value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_u_int(C_JSON param, const char *TAG, uint32_t value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_long(C_JSON param, const char *TAG, int64_t value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_u_long(C_JSON param, const char *TAG, uint64_t value) {
    return c_json_add_array_number(param, TAG, (double)value);
}

bool c_json_add_array_double(C_JSON param, const char *TAG, double value) {
    return c_json_add_array_number(param, TAG, value);
}



C_JSON c_json_get_object(C_JSON param, const char *TAG) {
    C_JSON value;
    cJSON *item;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    /// Capturando dado
    item = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (item == NULL) {
        goto item_null;
    }

    /// Verifica tipagem
    if (cJSON_IsObject(item) == false) {
        goto item_not_object;
    }

    /// Criando o dados para o usuario
    value = c_json_private_new_with_hooks(&param->hooks);
    if (value == NULL) {
        goto new_failed;
    }

    if (value->json_object != NULL) {
        cJSON_Delete(value->json_object);
    }
    value->json_object = cJSON_DuplicateWithHooks(item, 1, &value->hooks);
    if (value->json_object == NULL) {
        goto duplicate_failed;
    }

    /// Adicionando na lista somente em caso de sucesso
    c_json_list_add(param->json_object_getted, value);

    return value;

duplicate_failed:
    c_json_free(value);
new_failed:
item_not_object:
item_null:
json_object_null:
param_not_initialized:
tag_null:
    return NULL;
}

bool c_json_get_string(C_JSON param, const char *TAG, char **value) {
    cJSON *item;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    item = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (item == NULL) {
        goto item_null;
    }
    if (cJSON_IsString(item) == false) {
        goto item_not_string;
    }

    *value = cJSON_GetStringValue(item);

    return true;

item_not_string:
item_null:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

bool c_json_get_boolean(C_JSON param, const char *TAG, bool *value) {
    cJSON *item;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    item = cJSON_GetObjectItem(param->json_object, TAG);
    if (item == NULL) {
        goto item_null;
    }
    if (cJSON_IsBool(item) == false) {
        goto item_not_bool;
    }

    *value = cJSON_IsTrue(item) ? true : false;

    return true;

item_not_bool:
item_null:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

bool c_json_get_number(C_JSON param, const char *TAG, double *value) {
    cJSON *item;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    item = cJSON_GetObjectItem(param->json_object, TAG);
    if (item == NULL) {
        goto item_null;
    }
    if (cJSON_IsNumber(item) == false) {
        goto item_not_number;
    }

    *value = item->valuedouble;

    return true;

item_not_number:
item_null:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

bool c_json_get_char(C_JSON param, const char *TAG, int8_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }
    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > INT8_MAX) {
        goto overflow_max;
    }
    if (tmp < INT8_MIN) {
        goto overflow_min;
    }
    if ((tmp - (int8_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (int8_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_u_char(C_JSON param, const char *TAG, uint8_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }
    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT8_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint8_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint8_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_short(C_JSON param, const char *TAG, int16_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > INT16_MAX) {
        goto overflow_max;
    }
    if (tmp < INT16_MIN) {
        goto overflow_min;
    }
    if ((tmp - (int16_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (int16_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_u_short(C_JSON param, const char *TAG, uint16_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT16_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint16_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint16_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_int(C_JSON param, const char *TAG, int32_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > INT32_MAX) {
        goto overflow_max;
    }
    if (tmp < INT32_MIN) {
        goto overflow_min;
    }
    if ((tmp - (int32_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (int32_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_u_int(C_JSON param, const char *TAG, uint32_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT32_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint32_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint32_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_long(C_JSON param, const char *TAG, int64_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > INT64_MAX) {
        goto overflow_max;
    }
    if (tmp < INT64_MIN) {
        goto overflow_min;
    }
    if ((tmp - (int64_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (int64_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_u_long(C_JSON param, const char *TAG, uint64_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_number(param, TAG, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT64_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint64_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint64_t)tmp;
    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_double(C_JSON param, const char *TAG, double *value) {
    return c_json_get_number(param, TAG, value);
}

bool c_json_get_array_size(C_JSON param, const char *TAG, size_t *value) {
    cJSON *json_array;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    json_array = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (cJSON_IsArray(json_array) == false) {
        goto item_not_array;
    }

    *value = cJSON_GetArraySize(json_array);

    return true;

item_not_array:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

bool c_json_get_array_boolean(C_JSON param, const char *TAG, uint32_t index, bool *value) {
    size_t len;
    cJSON *item;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    item = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (cJSON_IsArray(item) == false) {
        goto item_not_array;
    }

    c_json_get_array_size(param, TAG, &len);
    if (index >= len) {
        goto index_out_of_bounds;
    }

    item = cJSON_GetArrayItem(item, index);
    if (cJSON_IsBool(item) == false) {
        goto item_not_bool;
    }

    *value = cJSON_IsTrue(item) ? true : false;

    return true;

item_not_bool:
index_out_of_bounds:
item_not_array:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

C_JSON c_json_get_array_object(C_JSON param, const char *TAG, uint32_t index) {
    size_t len;
    C_JSON value;
    cJSON *json_array;
    cJSON *json_object;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    /// Capturando Objeto Array
    json_array = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (cJSON_IsArray(json_array) == false) {
        goto item_not_array;
    }

    /// Validando tamanho do array
    c_json_get_array_size(param, TAG, &len);
    if (index >= len) {
        goto index_out_of_bounds;
    }

    /// Capturando string e validando se de fato e
    json_object = cJSON_GetArrayItem(json_array, index);
    if (cJSON_IsObject(json_object) == false) {
        goto item_not_object;
    }

    /// Criando o dados para o usuario
    value = c_json_private_new_with_hooks(&param->hooks);
    if (value == NULL) {
        goto new_failed;
    }

    // Duplica o objeto para que o usuario tenha uma copia independente
    if (value->json_object != NULL) {
        cJSON_Delete(value->json_object);
    }
    value->json_object = cJSON_DuplicateWithHooks(json_object, 1, &value->hooks);
    if (value->json_object == NULL) {
        goto duplicate_failed;
    }

    // Adicionando na lista somente em caso de sucesso
    c_json_list_add(param->json_object_getted, value);

    return value;

duplicate_failed:
    c_json_free(value);
new_failed:
item_not_object:
index_out_of_bounds:
item_not_array:
json_object_null:
param_not_initialized:
tag_null:
    return NULL;
}

bool c_json_get_array_string(C_JSON param, const char *TAG, uint32_t index, char **value) {
    size_t len;
    cJSON *json_array;
    cJSON *json_string;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    /// Capturando Objeto Array
    json_array = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (cJSON_IsArray(json_array) == false) {
        goto item_not_array;
    }

    /// Validando tamanho do array
    c_json_get_array_size(param, TAG, &len);
    if (index >= len) {
        goto index_out_of_bounds;
    }

    /// Capturando string e validando se de fato e
    json_string = cJSON_GetArrayItem(json_array, index);
    if (cJSON_IsString(json_string) == false) {
        goto item_not_string;
    }

    *value = cJSON_GetStringValue(json_string);

    return true;

item_not_string:
index_out_of_bounds:
item_not_array:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

C_JSON_API bool c_json_get_array_number(C_JSON param, const char *TAG, uint32_t index, double *value) {
    size_t len;
    cJSON *item;

    if (TAG == NULL) {
        goto tag_null;
    }
    if (value == NULL) {
        goto value_null;
    }
    if (param == NULL) {
        goto param_not_initialized;
    }
    if (param->json_object == NULL) {
        goto json_object_null;
    }

    item = cJSON_GetObjectItemCaseSensitive(param->json_object, TAG);
    if (cJSON_IsArray(item) == false) {
        goto item_not_array;
    }

    c_json_get_array_size(param, TAG, &len);
    if (index >= len) {
        goto index_out_of_bounds;
    }

    item = cJSON_GetArrayItem(item, index);
    if (cJSON_IsNumber(item) == false) {
        goto item_not_number;
    }
    *value = cJSON_GetNumberValue(item);

    return true;

item_not_number:
index_out_of_bounds:
item_not_array:
json_object_null:
param_not_initialized:
value_null:
tag_null:
    return false;
}

bool c_json_get_array_char(C_JSON param, const char *TAG, uint32_t index, char *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_array_number(param, TAG, index, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT8_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint8_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint8_t)tmp;

    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_array_short(C_JSON param, const char *TAG, uint32_t index, int16_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_array_number(param, TAG, index, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > INT16_MAX) {
        goto overflow_max;
    }
    if (tmp < INT16_MIN) {
        goto overflow_min;
    }
    if ((tmp - (int16_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (int16_t)tmp;

    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_array_u_short(C_JSON param, const char *TAG, uint32_t index, uint16_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_array_number(param, TAG, index, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT16_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint16_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint16_t)tmp;

    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_array_int(C_JSON param, const char *TAG, uint32_t index, int32_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_array_number(param, TAG, index, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > INT32_MAX) {
        goto overflow_max;
    }
    if (tmp < INT32_MIN) {
        goto overflow_min;
    }
    if ((tmp - (int32_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (int32_t)tmp;

    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_array_u_int(C_JSON param, const char *TAG, uint32_t index, uint32_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_array_number(param, TAG, index, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT32_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint32_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint32_t)tmp;

    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_array_long(C_JSON param, const char *TAG, uint32_t index, int64_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_array_number(param, TAG, index, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > INT64_MAX) {
        goto overflow_max;
    }
    if (tmp < INT64_MIN) {
        goto overflow_min;
    }
    if ((tmp - (int64_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (int64_t)tmp;

    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_array_u_long(C_JSON param, const char *TAG, uint32_t index, uint64_t *value) {
    bool ret;
    double tmp = 0;

    if (value == NULL) {
        goto value_null;
    }

    ret = c_json_get_array_number(param, TAG, index, &tmp);
    if (ret == false) {
        goto get_number_failed;
    }
    if (tmp > UINT64_MAX) {
        goto overflow_max;
    }
    if (tmp < 0) {
        goto overflow_min;
    }
    if ((tmp - (uint64_t)tmp) != 0.0) {
        goto not_integer;
    }

    *value = (uint64_t)tmp;

    return true;

not_integer:
overflow_min:
overflow_max:
get_number_failed:
value_null:
    return false;
}

bool c_json_get_array_double(C_JSON param, const char *TAG, uint32_t index, double *value) {
    return c_json_get_array_number(param, TAG, index, value);
}
