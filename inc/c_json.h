/**
 * @file    c_json.h
 * @author  Thiago Silveira
 *
 * @version 1.0
 * @date    14/07/2026
 *
 * @copyright Copyright (c) 2026
 *
 * @brief Uma camada de abstracao sobre a cJSON para simplificar a manipulacao de
 * objetos JSON em C. A biblioteca fornece uma API robusta, segura e orientada
 * para o gerenciamento de memoria.
 */

#ifndef INC_C_JSON_H_
#define INC_C_JSON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#if defined(_WIN32)
#define C_JSON_API __declspec(dllexport)
#else
#define C_JSON_API extern
#endif

/**
 * @def JSON_LIB_VERSION
 * @brief Define a versao atual da biblioteca.
 */
#define JSON_LIB_VERSION "2.0.0"

// ============================================================
//  TIPOS E ESTRUTURAS DE DADOS
// ============================================================

/**
 * @brief Define as opcoes de formatacao para a saida de string JSON.
 */
typedef enum format_type {
    C_JSON_STR_UNFORMATTED = 0,           ///< Saida de string compacta sem espacos ou quebras de linha.
    C_JSON_STR_FORMATTED = 1,             ///< Saida de string formatada com indentacao para melhor legibilidade.
    C_JSON_STR_UNFORMATTED_WO_SPACE = 2,  ///< Funcionalmente identico a C_JSON_STR_UNFORMATTED.
    C_JSON_STR_UNFORMATTED_WO_SPACE_WO_ESCAPE = 3  ///< Reservado para uso futuro.
} E_C_JSON_STR_ESCAPE;

/**
 * @brief Configura o comportamento da biblioteca ao adicionar uma chave duplicada.
 */
typedef enum c_json_config {
    C_JSON_CONFIG_ALLOW_SAME_TAG = 0,   ///< Permite multiplas chaves com o mesmo nome (TAG).
    C_JSON_CONFIG_UPDATE_SAME_TAG = 1,  ///< Substitui o valor existente pelo novo valor.
    C_JSON_CONFIG_ERROR_SAME_TAG = 2    ///< Retorna um erro se a chave ja existir.
} E_C_JSON_CONFIG;

/**
 * @brief Um ponteiro opaco para a estrutura interna da biblioteca.
 * Atua como um manipulador (handle) para o objeto JSON.
 */
typedef struct c_json *C_JSON;

// ============================================================
//  FUNCOES DE GERENCIAMENTO DA BIBLIOTECA
// ============================================================

/**
 * @brief Cria e inicializa um novo objeto C_JSON.
 *
 * @details Este objeto encapsula a logica de manipulacao JSON e gerenciamento de memoria.
 * A configuracao padrao para chaves duplicadas e `C_JSON_CONFIG_UPDATE_SAME_TAG`.
 *
 * @return Um ponteiro para o novo objeto `C_JSON` ou `NULL` se a alocacao falhar.
 */
C_JSON_API C_JSON c_json_new(void);

/**
 * @brief Libera todos os recursos de memoria alocados por um objeto C_JSON.
 *
 * @details Esta funcao libera o objeto JSON principal, a ultima string gerada por
 * `c_json_serialize()` e todos os sub-objetos retornados por `c_json_get_object()`
 * e `c_json_get_array_object()`.
 *
 * @param param O ponteiro para o objeto `C_JSON` a ser liberado.
 * @return Sempre retorna `NULL`, o que permite a pratica de `meu_json = c_json_free(meu_json);`.
 */
C_JSON_API C_JSON c_json_free(C_JSON param);

/**
 * @brief Retorna a versao da biblioteca.
 *
 * @return Uma string literal constante no formato "MAJOR.MINOR.PATCH".
 */
C_JSON_API const char *c_json_version(void);

/**
 * @brief Define o comportamento da biblioteca quando uma chave duplicada e adicionada.
 *
 * @param param O objeto `C_JSON` a ser configurado.
 * @param config A configuracao desejada (uma das opcoes de `E_C_JSON_CONFIG`).
 * @return `true` se a configuracao foi aplicada com sucesso, `false` se o `param` for `NULL` ou a `config` for
 * invalida.
 */
C_JSON_API bool c_json_set_config(C_JSON param, E_C_JSON_CONFIG config);

// ============================================================
//  FUNCOES BASICAS DA BIBLIOTECA
// ============================================================

/**
 * @brief Analisa uma string JSON e a carrega para o objeto `C_JSON`.
 *
 * @note O conteudo existente no objeto `param` sera substituido.
 *
 * @param param O objeto `C_JSON` de destino.
 * @param value A string C terminada em nulo contendo o JSON.
 * @return `true` para sucesso, `false` se o parse falhar ou os argumentos forem invalidos.
 */
C_JSON_API bool c_json_parser(C_JSON param, const char *value);

/**
 * @brief Converte um objeto `C_JSON` para sua representacao em string.
 *
 * @warning A memoria da string retornada (`value`) e gerenciada pelo objeto `C_JSON`
 * (`param`). Nao chame `free()` neste ponteiro. Ele sera liberado por
 * `c_json_free()` ou em uma chamada subsequente a esta mesma funcao.
 *
 * @param param O objeto `C_JSON` a ser serializado.
 * @param value Um ponteiro para `char*` que recebera o endereco da string gerada.
 * @param size Um ponteiro para `size_t` que recebera o tamanho da string (excluindo o terminador nulo), pode ser
 * `NULL`.
 * @param escape A opcao de formatacao desejada (consulte `E_C_JSON_STR_ESCAPE`).
 * @return `true` para sucesso, `false` para falha.
 */
C_JSON_API bool c_json_serialize(C_JSON param, char **value, size_t *size, E_C_JSON_STR_ESCAPE escape);

/**
 * @brief Imprime a representacao em string do objeto JSON no console (`stdout`).
 *
 * @note Esta funcao gerencia sua propria memoria de string temporaria, nao exigindo
 * liberacao manual por parte do usuario.
 *
 * @param param O objeto `C_JSON` a ser impresso.
 * @param escape A opcao de formatacao da string.
 * @return `true` para sucesso, `false` para erro.
 */
C_JSON_API bool c_json_print(C_JSON param, E_C_JSON_STR_ESCAPE escape);

// ============================================================
//  FUNCOES DE MANIPULACAO DE DADOS (ADICAO)
// ============================================================

/**
 * @brief Adiciona um valor booleano a um objeto JSON.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor booleano a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro (ex: chave duplicada com `C_JSON_CONFIG_ERROR_SAME_TAG`).
 */
C_JSON_API bool c_json_add_boolean(C_JSON param, const char *TAG, bool value);

/**
 * @brief Adiciona uma string a um objeto JSON.
 *
 * @note A biblioteca cria uma copia interna da string `value`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value A string a ser copiada.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_string(C_JSON param, const char *TAG, const char *value);

/**
 * @brief Adiciona um objeto JSON aninhado.
 *
 * @note A biblioteca realiza uma copia profunda (deep copy) do objeto `value`.
 *
 * @warning O objeto original `value` continua sob a responsabilidade do chamador
 * e deve ser liberado com `c_json_free()` separadamente.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave para o objeto aninhado.
 * @param value O objeto `C_JSON` a ser copiado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_object(C_JSON param, const char *TAG, C_JSON value);

/**
 * @brief Adiciona um numero de ponto flutuante (double) a um objeto JSON.
 *
 * @note Esta e a funcao base para todas as outras funcoes de adicao de numeros.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave para o novo numero.
 * @param value O valor `double`.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_number(C_JSON param, const char *TAG, const double value);

/**
 * @brief Adiciona um valor `char` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_char(C_JSON param, const char *TAG, char value);

/**
 * @brief Adiciona um valor `unsigned char` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_u_char(C_JSON param, const char *TAG, unsigned char value);

/**
 * @brief Adiciona um valor `int16_t` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_short(C_JSON param, const char *TAG, int16_t value);

/**
 * @brief Adiciona um valor `uint16_t` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_u_short(C_JSON param, const char *TAG, uint16_t value);

/**
 * @brief Adiciona um valor `int32_t` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_int(C_JSON param, const char *TAG, int32_t value);

/**
 * @brief Adiciona um valor `uint32_t` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_u_int(C_JSON param, const char *TAG, uint32_t value);

/**
 * @brief Adiciona um valor `int64_t` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_long(C_JSON param, const char *TAG, int64_t value);

/**
 * @brief Adiciona um valor `uint64_t` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_u_long(C_JSON param, const char *TAG, uint64_t value);

/**
 * @brief Adiciona um valor `double` a um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_add_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do novo campo.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_double(C_JSON param, const char *TAG, double value);

/**
 * @brief Adiciona um valor booleano ao final de um array.
 *
 * @note Se o array com a TAG nao existir, ele sera criado. Se a TAG existir, mas nao for um array, a funcao falha.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor booleano a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_boolean(C_JSON param, const char *TAG, bool value);

/**
 * @brief Adiciona um objeto a um array.
 *
 * @note Se o array com a TAG nao existir, ele sera criado. Uma copia profunda do objeto `value` e adicionada.
 *
 * @warning O objeto `value` original continua sob a responsabilidade do chamador e deve ser liberado separadamente.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param type Ignora, pode passar qualquer coisa
 * @param value O objeto `C_JSON` a ser copiado e adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_object(C_JSON param, const char *TAG, int type, C_JSON value);

/**
 * @brief Adiciona uma string ao final de um array.
 *
 * @note Se o array com a TAG nao existir, ele sera criado. Se a TAG existir, mas nao for um array, a funcao falha.
 * A biblioteca cria uma copia interna da string `value`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value A string a ser copiada e adicionada.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_string(C_JSON param, const char *TAG, const char *value);

/**
 * @brief Adiciona um numero (`double`) ao final de um array.
 *
 * @note Esta e a funcao base para todas as outras funcoes de adicao numerica em array.
 * Se o array com a TAG nao existir, ele sera criado. Se a TAG existir, mas nao for um array, a funcao falha.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor `double` a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_number(C_JSON param, const char *TAG, double value);

/**
 * @brief Adiciona um valor `char` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_char(C_JSON param, const char *TAG, char value);

/**
 * @brief Adiciona um valor `unsigned char` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_u_char(C_JSON param, const char *TAG, unsigned char value);

/**
 * @brief Adiciona um valor `int16_t` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_short(C_JSON param, const char *TAG, int16_t value);

/**
 * @brief Adiciona um valor `uint16_t` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_u_short(C_JSON param, const char *TAG, uint16_t value);

/**
 * @brief Adiciona um valor `int32_t` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_int(C_JSON param, const char *TAG, int32_t value);

/**
 * @brief Adiciona um valor `uint32_t` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_u_int(C_JSON param, const char *TAG, uint32_t value);

/**
 * @brief Adiciona um valor `int64_t` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_long(C_JSON param, const char *TAG, int64_t value);

/**
 * @brief Adiciona um valor `uint64_t` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_u_long(C_JSON param, const char *TAG, uint64_t value);

/**
 * @brief Adiciona um valor `double` ao final de um array.
 *
 * @note Wrapper tipado de `c_json_add_array_number()`.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param value O valor a ser adicionado.
 * @return `true` para sucesso, `false` em caso de erro.
 */
C_JSON_API bool c_json_add_array_double(C_JSON param, const char *TAG, double value);

// ============================================================
//  FUNCOES DE MANIPULACAO DE DADOS (OBTENCAO)
// ============================================================

/**
 * @brief Obtem um valor booleano de um objeto JSON.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada e o valor e do tipo correto, `false` caso contrario.
 */
C_JSON_API bool c_json_get_boolean(C_JSON param, const char *TAG, bool *value);

/**
 * @brief Obtem um objeto aninhado.
 *
 * @warning O handle `C_JSON` retornado e gerenciado pelo objeto `param`.
 * Nao o libere com `c_json_free()`. Ele se torna invalido apos a liberacao do objeto pai.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do objeto aninhado.
 * @return Um novo handle `C_JSON` para o objeto aninhado ou `NULL` se a chave nao for encontrada ou o tipo estiver
 * incorreto.
 */
C_JSON_API C_JSON c_json_get_object(C_JSON param, const char *TAG);

/**
 * @brief Obtem uma string de um objeto JSON.
 *
 * @warning O ponteiro retornado aponta para a memoria interna da biblioteca.
 * Nao modifique nem libere este ponteiro. Ele e valido apenas enquanto o objeto `C_JSON` pai existir.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave da string.
 * @param value Um ponteiro de saida para `char*` que recebera o endereco da string.
 * @return `true` se a chave foi encontrada e o valor e do tipo string, `false` caso contrario.
 */
C_JSON_API bool c_json_get_string(C_JSON param, const char *TAG, char **value);

/**
 * @brief Obtem um numero de ponto flutuante (`double`) de um objeto JSON.
 *
 * @note Esta e a funcao base para todas as outras funcoes de obtencao numerica.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada e o valor e numerico, `false` caso contrario.
 */
C_JSON_API bool c_json_get_number(C_JSON param, const char *TAG, double *value);

/**
 * @brief Obtem um valor `int8_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_char(C_JSON param, const char *TAG, int8_t *value);

/**
 * @brief Obtem um valor `uint8_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_u_char(C_JSON param, const char *TAG, uint8_t *value);

/**
 * @brief Obtem um valor `int16_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_short(C_JSON param, const char *TAG, int16_t *value);

/**
 * @brief Obtem um valor `uint16_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_u_short(C_JSON param, const char *TAG, uint16_t *value);

/**
 * @brief Obtem um valor `int32_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_int(C_JSON param, const char *TAG, int32_t *value);

/**
 * @brief Obtem um valor `uint32_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_u_int(C_JSON param, const char *TAG, uint32_t *value);

/**
 * @brief Obtem um valor `int64_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_long(C_JSON param, const char *TAG, int64_t *value);

/**
 * @brief Obtem um valor `uint64_t` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada, o valor e numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_u_long(C_JSON param, const char *TAG, uint64_t *value);

/**
 * @brief Obtem um valor `double` de um objeto JSON.
 *
 * @note Wrapper tipado de `c_json_get_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do campo.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se a chave foi encontrada e o valor e numerico, `false` caso contrario.
 */
C_JSON_API bool c_json_get_double(C_JSON param, const char *TAG, double *value);

/**
 * @brief Obtem o numero de elementos de um array JSON.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param value Um ponteiro de saida para armazenar o tamanho.
 * @return `true` se a chave foi encontrada e o valor e um array, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_size(C_JSON param, const char *TAG, size_t *value);

/**
 * @brief Obtem um valor booleano de um array JSON.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento no indice for booleano, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_boolean(C_JSON param, const char *TAG, uint32_t index, bool *value);

/**
 * @brief Obtem um objeto de um array.
 *
 * @warning O handle `C_JSON` retornado e gerenciado pelo objeto `param`.
 * Nao o libere com `c_json_free()`. Ele se torna invalido apos a liberacao do objeto pai.
 *
 * @param param O objeto `C_JSON` principal.
 * @param TAG A chave do array.
 * @param index O indice do objeto dentro do array.
 * @return Um novo handle `C_JSON` para o objeto no indice especificado ou `NULL` se o array/indice nao for encontrado
 * ou o tipo estiver incorreto.
 */
C_JSON_API C_JSON c_json_get_array_object(C_JSON param, const char *TAG, uint32_t index);

/**
 * @brief Obtem uma string de um array JSON.
 *
 * @warning O ponteiro retornado aponta para a memoria interna da biblioteca.
 * Nao modifique nem libere este ponteiro. Ele e valido apenas enquanto o objeto `C_JSON` pai existir.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para `char*` que recebera o endereco da string.
 * @return `true` se o elemento no indice for uma string, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_string(C_JSON param, const char *TAG, uint32_t index, char **value);

/**
 * @brief Obtem um numero (`double`) de um array JSON.
 *
 * @note Esta e a funcao base para todas as outras funcoes de obtencao numerica em array.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento no indice for numerico, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_number(C_JSON param, const char *TAG, uint32_t index, double *value);

/**
 * @brief Obtem um valor `char` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_char(C_JSON param, const char *TAG, uint32_t index, char *value);

/**
 * @brief Obtem um valor `int16_t` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_short(C_JSON param, const char *TAG, uint32_t index, int16_t *value);

/**
 * @brief Obtem um valor `uint16_t` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_u_short(C_JSON param, const char *TAG, uint32_t index, uint16_t *value);

/**
 * @brief Obtem um valor `int32_t` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_int(C_JSON param, const char *TAG, uint32_t index, int32_t *value);

/**
 * @brief Obtem um valor `uint32_t` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_u_int(C_JSON param, const char *TAG, uint32_t index, uint32_t *value);

/**
 * @brief Obtem um valor `int64_t` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_long(C_JSON param, const char *TAG, uint32_t index, int64_t *value);

/**
 * @brief Obtem um valor `uint64_t` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`. Falha se o valor estiver fora da faixa do tipo de destino.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico e cabe no tipo, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_u_long(C_JSON param, const char *TAG, uint32_t index, uint64_t *value);

/**
 * @brief Obtem um valor `double` de um array JSON.
 *
 * @note Wrapper tipado de `c_json_get_array_number()`.
 *
 * @param param O objeto `C_JSON`.
 * @param TAG A chave do array.
 * @param index O indice do elemento dentro do array.
 * @param value Um ponteiro de saida para armazenar o valor.
 * @return `true` se o elemento for numerico, `false` caso contrario.
 */
C_JSON_API bool c_json_get_array_double(C_JSON param, const char *TAG, uint32_t index, double *value);

#endif  // INC_C_JSON_H_
