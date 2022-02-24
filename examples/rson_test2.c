#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../src/rson.h"


int main(void)
{
    Rson rson = {0};

    RsonList state = RSON_OK;
    
    #if 0
    state += state ? 0 : rson_from_json(&rson, "", "[{},0]", false);
    state += state ? 0 : rson_from_json(&rson, "[0]", "{\"name\": \"Raphael\"}", true);
    state += state ? 0 : rson_from_json(&rson, "[0]", "{\"age\": 20}", true);
    state += state ? 0 : rson_from_json(&rson, "[0]", "{\"student\": true}", true);
    // state += state ? 0 : rson_from_json(&rson, "[1]", "{\"name\": \"John\"}", true);
    // state += state ? 0 : rson_from_json(&rson, "[1]", "{\"age\": 60}", true);
    // state += state ? 0 : rson_from_json(&rson, "[1]", "{\"student\": false}", true);
    state += state ? 0 : rson_from_json(&rson, "[2]", "10", false);
    #endif

    state += state ? 0 : rson_from_json(&rson, "", "{\"abc\": \"test\"}", false);
    state += state ? 0 : rson_from_json(&rson, "", "{\"test\": \"abc\"}", true);
    
    if(state)
    {
        printf("Error %d, exiting\n", state);
        return 1;
    }
    rson_print(&rson, true);

    return 0;
}
