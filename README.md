# c_json

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Biblioteca em C que encapsula a [cJSON](https://github.com/DaveGamble/cJSON) para fornecer uma API simplificada, robusta e segura para manipulação de objetos JSON. O projeto prioriza portabilidade (Linux, Android, Windows), gerenciamento centralizado de memória e integração com CMake.

---

## Funcionalidades

- **API simplificada:** funções diretas para adicionar e obter tipos (`int`, `string`, `bool`, `double`, arrays, objetos) sem interagir diretamente com a cJSON.
- **Gerenciamento de memória:** o objeto principal libera sub-objetos e strings retornadas por `get`, evitando vazamentos e double-free.
- **Configuração de chaves duplicadas** (`E_C_JSON_CONFIG`):
  - `C_JSON_CONFIG_ALLOW_SAME_TAG` — permite múltiplas chaves com o mesmo nome.
  - `C_JSON_CONFIG_UPDATE_SAME_TAG` — atualiza o valor existente (padrão).
  - `C_JSON_CONFIG_ERROR_SAME_TAG` — retorna erro se a chave já existir.
- **Segurança de tipos:** funções `get` específicas com validação de tipo e proteção contra overflow numérico.
- **Suporte a arrays:** adição e leitura de todos os tipos primitivos e objetos.
- **cJSON incluída:** fontes e headers da cJSON já vêm no repositório (`src/` e `inc/`).

## Estrutura do Projeto

```
c_json/
├── inc/              # Headers públicos (c_json.h e deps cJSON)
├── src/              # Implementação (+ cJSON vendored)
├── CMakeLists.txt
├── LICENSE
└── README.md
```

## Pré-requisitos

| Ferramenta | Uso |
|------------|-----|
| GCC ou Clang | Compilação (C11) |
| CMake ≥ 3.16 | Build |

No Ubuntu/Debian:

```bash
sudo apt install build-essential cmake
```

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# opcional: instalar biblioteca e headers
# cmake --install build
```

Artefato gerado: `build/libc_json.so`.

## Exemplo básico

```c
#include <stdio.h>
#include "c_json.h"

int main(void) {
    C_JSON json = c_json_new();

    c_json_add_string(json, "projeto", "c_json");
    c_json_add_int(json, "versao", 2);
    c_json_add_boolean(json, "ativo", true);

    char *json_str = NULL;
    size_t size = 0;
    c_json_serialize(json, &json_str, &size, C_JSON_STR_FORMATTED);
    printf("JSON gerado:\n%s\n", json_str);

    char *nome = NULL;
    if (c_json_get_string(json, "projeto", &nome)) {
        printf("Projeto: %s\n", nome);
    }

    json = c_json_free(json);
    return 0;
}
```

## Exemplo completo

Objeto aninhado, array de itens, serialização, parse e leitura. Os handles retornados por `c_json_get_object` / `c_json_get_array_object` são liberados automaticamente ao chamar `c_json_free` no objeto pai.

```c
#include <stdio.h>
#include "c_json.h"

int main(void) {
    C_JSON root = c_json_new();
    C_JSON meta = c_json_new();
    C_JSON item = c_json_new();

    /* Metadados aninhados */
    c_json_add_string(meta, "autor", "Thiago");
    c_json_add_string(meta, "licenca", "MIT");
    c_json_add_object(root, "meta", meta);
    meta = c_json_free(meta);

    /* Array de objetos */
    c_json_add_string(item, "nome", "alpha");
    c_json_add_int(item, "prioridade", 1);
    c_json_add_array_object(root, "itens", 0, item);
    item = c_json_free(item);

    item = c_json_new();
    c_json_add_string(item, "nome", "beta");
    c_json_add_int(item, "prioridade", 2);
    c_json_add_array_object(root, "itens", 0, item);
    item = c_json_free(item);

    c_json_add_array_string(root, "tags", "c");
    c_json_add_array_string(root, "tags", "json");

    /* Serializa */
    char *out = NULL;
    size_t out_size = 0;
    c_json_serialize(root, &out, &out_size, C_JSON_STR_FORMATTED);
    printf("Documento:\n%s\n", out);

    /* Parse de volta e lê campos */
    C_JSON parsed = c_json_new();
    if (c_json_parser(parsed, out)) {
        C_JSON meta_get = c_json_get_object(parsed, "meta");
        char *autor = NULL;
        if (meta_get && c_json_get_string(meta_get, "autor", &autor)) {
            printf("Autor: %s\n", autor);
        }

        size_t n = 0;
        if (c_json_get_array_size(parsed, "itens", &n)) {
            for (uint32_t i = 0; i < n; i++) {
                C_JSON elem = c_json_get_array_object(parsed, "itens", i);
                char *nome = NULL;
                int32_t prio = 0;
                if (elem &&
                    c_json_get_string(elem, "nome", &nome) &&
                    c_json_get_int(elem, "prioridade", &prio)) {
                    printf("Item[%u]: %s (prio %d)\n", i, nome, prio);
                }
            }
        }
    }

    root = c_json_free(root);
    parsed = c_json_free(parsed);
    return 0;
}
```

Compilar um programa de exemplo contra a biblioteca (após o build):

```bash
gcc -std=c11 exemplo.c -Iinc -Lbuild -lc_json -Wl,-rpath,$PWD/build -o exemplo
./exemplo
```

## Licença

Distribuído sob a licença [MIT](LICENSE). Copyright © 2025 Thiago Silveira.

A biblioteca inclui [cJSON](https://github.com/DaveGamble/cJSON), também sob licença MIT, em `src/` e `inc/`.
