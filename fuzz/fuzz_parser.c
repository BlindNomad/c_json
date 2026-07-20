#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "c_json.h"

#define FUZZ_MAX_INPUT_SIZE (1024U * 1024U)

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    char *buf = NULL;
    C_JSON json = NULL;

    if (data == NULL || size == 0 || size > FUZZ_MAX_INPUT_SIZE) {
        return 0;
    }

    buf = (char *)malloc(size + 1U);
    if (buf == NULL) {
        return 0;
    }

    memcpy(buf, data, size);
    buf[size] = '\0';

    json = c_json_new();
    if (json != NULL && c_json_parser(json, buf)) {
        for (int i = 0; i <= (int)C_JSON_STR_UNFORMATTED_WO_SPACE_WO_ESCAPE; i++) {
            char *out = NULL;
            size_t out_size = 0;

            if (c_json_serialize(json, &out, &out_size, (E_C_JSON_STR_ESCAPE)i)) {
                (void)c_json_parser(json, out);
            }
        }
    }

    c_json_free(json);
    free(buf);

    return 0;
}
