#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../src/rson.h"

#define ENUM2STR_HELPER(x) #x
#define ENUM2STR(x) ENUM2STR_HELPER(x)

size_t file_read(char *filename, char **dump)
{
    if(!filename || !dump) return 0;
    FILE *file = fopen(filename, "rb");
    if(!file) return 0;

    // get file length 
    fseek(file, 0, SEEK_END);
    size_t bytes_file = ftell(file);
    fseek(file, 0, SEEK_SET);

    // allocate memory
    if(!(*dump = realloc(*dump, bytes_file + 1))) return 0;
    
    // read file
    size_t bytes_read = fread(*dump, 1, bytes_file, file);
    if(bytes_file != bytes_read) return 0;
    (*dump)[bytes_read] = 0;
    
    // close file
    fclose(file);
    return bytes_read;
}

int main(void)
{
    printf("sizeof(RsonObject) = %zu\n", sizeof(RsonObject));
    printf("sizeof(Rson) = %zu\n", sizeof(Rson));
    // printf("sizeof(Rson) = %lld\n", sizeof(Rson));


    Rson rson = {0};
    char *json_file = "../data/test.json";
    // char *json_file = "./data/escapes.json";
    // char *json_file = "./.vscode/tasks.json";
    // char *json_file = "C:/dev/c/sf-city-lots-json/citylots.json";
    // char *json_file = "./data/escapes.json";
    char *json_str = 0;

    size_t bytes = file_read(json_file, &json_str);
    if(!bytes)
    {
        printf("Could not read file '%s'.\n", json_file);
        return 1;
    }

    RsonList state = rson_from_json(&rson, 0, json_str, false);
    state += rson_from_json(&rson, "\"members\".[0]", json_str, true);
    printf("state (after parsing) = %d\n", state);
    if(state)
    {
        printf("Error, exiting\n");
        return 1;
    }
    rson_print(&rson, true);

    // Rson *get = rson_get(&rson, "\\\"\u2115\u2282\u211D\n\f\"", &state);
    // TODO: FIX the output from the 4 lines below should be identical
    // Rson *get = rson_get(&rson, "", &state);
    // printf("hex values: %p<->%p\n", &rson, get);
    // get = rson_get(&rson, 0, &state);
    // printf("hex values: %p<->%p\n", &rson, get);

    // printf("BELOW IS THE VALUE WE GOT:\n");
    
    // (Rson){.id = RSON_INT, .content.i = 10, .count = 1};
    // R2str r2str = {0};
    // rson_r2str(&rson, false, &r2str);
    // r2str_file_write(&r2str, "test_out.json");
    // r2str_free(&r2str);
    free(json_str);
    json_str = 0;
    // state = rson_string(&rson, &json_str);

    return 0;
}
