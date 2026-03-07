#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "msg_store.h"
#include "sensor.h"

static sensor_msg_t make_msg(int id, float value) {
    return sensor_msg_make(MSG_TEMP, id, value, time(NULL));
}

static bool is_expected_id(int id) {
    return id == 1 || id == 2 || id == 3;
}

int main(void) {
    msg_storage_t storage;

    // init
    assert(msg_storage_init(&storage, 3) == STORAGE_EMPTY);
    assert(msg_storage_get_capacity(&storage) == 3);
    assert(msg_storage_get_count(&storage) == 0);
    assert(msg_storage_is_empty(&storage));
    assert(msg_storage_health_check(&storage) == STORAGE_OK);

    // push
    sensor_msg_t m1 = make_msg(1, 10.0f);
    sensor_msg_t m2 = make_msg(2, 20.0f);
    sensor_msg_t m3 = make_msg(3, 30.0f);

    assert(msg_storage_push(&storage, &m1) == OPERATION_SUCCESS);
    assert(msg_storage_push(&storage, &m2) == OPERATION_SUCCESS);
    assert(msg_storage_push(&storage, &m3) == OPERATION_SUCCESS);

    assert(msg_storage_get_count(&storage) == 3);
    assert(msg_storage_get_total_pushed(&storage) == 3);

    // pop: validate operation/statistics and payload integrity
    sensor_msg_t out = {0};

    assert(msg_storage_pop(&storage, &out) == OPERATION_SUCCESS);
    assert(is_expected_id(out.payload.id));

    assert(msg_storage_pop(&storage, &out) == OPERATION_SUCCESS);
    assert(is_expected_id(out.payload.id));

    assert(msg_storage_pop(&storage, &out) == OPERATION_SUCCESS);
    assert(is_expected_id(out.payload.id));

    assert(msg_storage_get_total_popped(&storage) == 3);
    assert(msg_storage_get_count(&storage) == 0);
    assert(msg_storage_is_empty(&storage));

    // cannot pop empty storage
    assert(msg_storage_pop(&storage, &out) == OPERATION_FAILURE);

    msg_storage_delete(&storage);

    printf("msg_store unit tests passed.\n");
    return 0;
}
