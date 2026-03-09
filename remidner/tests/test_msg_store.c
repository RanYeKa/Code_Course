#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "msg_store.h"
#include "sensor.h"

static int g_checks_run = 0;
static int g_checks_failed = 0;

static sensor_msg_t make_msg(int id, float value) {
    return sensor_msg_make(MSG_TEMP, id, value, time(NULL));
}

static void print_section(const char *title) {
    printf("\n==================================================\n");
    printf("%s\n", title);
    printf("==================================================\n");
}

static void report_check_result(bool pass, const char *desc) {
    g_checks_run++;
    if (pass) {
        printf("[PASS] %s\n", desc);
    } else {
        g_checks_failed++;
        printf("[FAIL] %s\n", desc);
    }
}

static void check_true(const char *desc, bool condition) {
    report_check_result(condition, desc);
}

static void check_int(const char *desc, long expected, long actual) {
    g_checks_run++;
    if (expected == actual) {
        printf("[PASS] %s | expected=%ld actual=%ld\n", desc, expected, actual);
    } else {
        g_checks_failed++;
        printf("[FAIL] %s | expected=%ld actual=%ld\n", desc, expected, actual);
    }
}

static void check_ulong(const char *desc, unsigned long expected, unsigned long actual) {
    g_checks_run++;
    if (expected == actual) {
        printf("[PASS] %s | expected=%lu actual=%lu\n", desc, expected, actual);
    } else {
        g_checks_failed++;
        printf("[FAIL] %s | expected=%lu actual=%lu\n", desc, expected, actual);
    }
}

static void check_float(const char *desc, float expected, float actual, float tol) {
    float diff = expected - actual;
    if (diff < 0.0f) {
        diff = -diff;
    }

    g_checks_run++;
    if (diff <= tol) {
        printf("[PASS] %s | expected=%.3f actual=%.3f\n", desc, expected, actual);
    } else {
        g_checks_failed++;
        printf("[FAIL] %s | expected=%.3f actual=%.3f diff=%.3f tol=%.3f\n",
               desc, expected, actual, diff, tol);
    }
}

static void print_sensor_msg_brief(const sensor_msg_t *msg, const char *label) {
    if (!msg) {
        printf("%s: <null>\n", label);
        return;
    }

    printf("%s: type=%d id=%d value=%.2f\n",
           label,
           msg->type,
           msg->payload.id,
           msg->payload.value);
}

static void dummy_msg_print(const void *msg_ptr) {
    const sensor_msg_t *msg = (const sensor_msg_t *)msg_ptr;
    if (!msg) {
        printf("  [dummy printer] null message\n");
        return;
    }

    printf("  [dummy printer] type=%d id=%d value=%.2f\n",
           msg->type,
           msg->payload.id,
           msg->payload.value);
}

static void validate_message_core(const char *prefix,
                                  const sensor_msg_t *expected,
                                  const sensor_msg_t *actual) {
    char desc[128];

    snprintf(desc, sizeof(desc), "%s -> message type", prefix);
    check_int(desc, expected->type, actual->type);

    snprintf(desc, sizeof(desc), "%s -> payload.id", prefix);
    check_int(desc, expected->payload.id, actual->payload.id);

    snprintf(desc, sizeof(desc), "%s -> payload.value", prefix);
    check_float(desc, expected->payload.value, actual->payload.value, 0.001f);
}

static void pop_and_validate(msg_storage_t *storage,
                             sensor_msg_t *out,
                             const char *label,
                             const sensor_msg_t *expected,
                             unsigned long expected_total_popped,
                             long expected_count_after_pop) {
    storage_operation_status_t rc;

    rc = msg_storage_pop(storage, out);
    printf("%s returned: %d\n", label, rc);
    print_sensor_msg_brief(out, label);

    {
        char desc[128];

        snprintf(desc, sizeof(desc), "%s return code", label);
        check_int(desc, OPERATION_SUCCESS, rc);

        validate_message_core(label, expected, out);

        snprintf(desc, sizeof(desc), "count after %s", label);
        check_int(desc, expected_count_after_pop, (long)msg_storage_get_count(storage));

        snprintf(desc, sizeof(desc), "total_popped after %s", label);
        check_ulong(desc, expected_total_popped, msg_storage_get_total_popped(storage));
    }
}

int main(void) {
    msg_storage_t storage;
    sensor_msg_t out = {0};

    print_section("MSG STORAGE UNIT TEST START");

    /*
     * 1) Invalid init checks
     */
    print_section("1) INVALID INIT TESTS");
    check_int("init(NULL, 3) should return STORAGE_ERROR",
              STORAGE_ERROR,
              msg_storage_init(NULL, 3));

    check_int("health_check(NULL) should return STORAGE_ERROR",
              STORAGE_ERROR,
              msg_storage_health_check(NULL));

    check_int("init(valid, 0) should return STORAGE_ERROR",
              STORAGE_ERROR,
              msg_storage_init(&storage, 0));

    /*
     * 2) Valid init
     */
    print_section("2) VALID INIT TESTS");
    {
        storage_status_t init_rc = msg_storage_init(&storage, 3);
        printf("msg_storage_init(&storage, 3) returned %d\n", init_rc);

        check_int("init(valid, 3) return code", STORAGE_EMPTY, init_rc);
        check_int("health_check after init", STORAGE_OK, msg_storage_health_check(&storage));
        check_int("capacity after init", 3, (long)msg_storage_get_capacity(&storage));
        check_int("count after init", 0, (long)msg_storage_get_count(&storage));
        check_ulong("total_pushed after init", 0UL, msg_storage_get_total_pushed(&storage));
        check_ulong("total_popped after init", 0UL, msg_storage_get_total_popped(&storage));
        check_int("status after init", STORAGE_OK, msg_storage_get_status(&storage));
        check_true("storage is empty after init", msg_storage_is_empty(&storage));
        check_true("storage is not full after init", !msg_storage_is_full(&storage));
        check_int("head time on empty storage", INVALID_SIZE, (long)msg_storage_get_head_time(&storage));
        check_int("tail time on empty storage", INVALID_SIZE, (long)msg_storage_get_tail_time(&storage));
    }

    /*
     * 3) Prepare messages
     */
    print_section("3) PREPARE 6 TEST MESSAGES");
    sensor_msg_t m1 = make_msg(1, 10.0f);
    sensor_msg_t m2 = make_msg(2, 20.0f);
    sensor_msg_t m3 = make_msg(3, 30.0f);
    sensor_msg_t m4 = make_msg(4, 40.0f);
    sensor_msg_t m5 = make_msg(5, 50.0f);
    sensor_msg_t m6 = make_msg(6, 60.0f);

    print_sensor_msg_brief(&m1, "m1");
    print_sensor_msg_brief(&m2, "m2");
    print_sensor_msg_brief(&m3, "m3");
    print_sensor_msg_brief(&m4, "m4");
    print_sensor_msg_brief(&m5, "m5");
    print_sensor_msg_brief(&m6, "m6");

    /*
     * 4) Invalid push/pop parameter tests
     */
    print_section("4) INVALID PARAMETER TESTS");
    check_int("push(NULL, &m1) should fail",
              OPERATION_FAILURE,
              msg_storage_push(NULL, &m1));

    check_int("push(&storage, NULL) should fail",
              OPERATION_FAILURE,
              msg_storage_push(&storage, NULL));

    check_int("pop(NULL, &out) should fail",
              OPERATION_FAILURE,
              msg_storage_pop(NULL, &out));

    check_int("pop(&storage, NULL) should fail",
              OPERATION_FAILURE,
              msg_storage_pop(&storage, NULL));

    /*
     * 5) Basic push test with 3 messages
     */
    print_section("5) BASIC PUSH TESTS (3 MESSAGES INTO 3 SLOTS)");
    {
        storage_operation_status_t push_rc;

        push_rc = msg_storage_push(&storage, &m1);
        printf("push m1 returned: %d\n", push_rc);
        check_int("push m1", OPERATION_SUCCESS, push_rc);
        check_int("count after push m1", 1, (long)msg_storage_get_count(&storage));
        check_ulong("total_pushed after push m1", 1UL, msg_storage_get_total_pushed(&storage));
        check_true("storage not empty after push m1", !msg_storage_is_empty(&storage));
        check_true("storage not full after push m1", !msg_storage_is_full(&storage));
        check_true("head time valid after first push",
                   msg_storage_get_head_time(&storage) != (time_t)INVALID_SIZE);
        check_true("tail time valid after first push",
                   msg_storage_get_tail_time(&storage) != (time_t)INVALID_SIZE);

        push_rc = msg_storage_push(&storage, &m2);
        printf("push m2 returned: %d\n", push_rc);
        check_int("push m2", OPERATION_SUCCESS, push_rc);
        check_int("count after push m2", 2, (long)msg_storage_get_count(&storage));
        check_ulong("total_pushed after push m2", 2UL, msg_storage_get_total_pushed(&storage));
        check_true("storage not full after push m2", !msg_storage_is_full(&storage));

        push_rc = msg_storage_push(&storage, &m3);
        printf("push m3 returned: %d\n", push_rc);
        check_int("push m3", OPERATION_SUCCESS, push_rc);
        check_int("count after push m3", 3, (long)msg_storage_get_count(&storage));
        check_ulong("total_pushed after push m3", 3UL, msg_storage_get_total_pushed(&storage));
        check_true("storage is full after push m3", msg_storage_is_full(&storage));
        check_true("storage is not empty after push m3", !msg_storage_is_empty(&storage));
    }

    /*
     * 6) Print test
     */
    print_section("6) PRINT TEST");
    printf("Calling msg_storage_print() with dummy printer:\n");
    msg_storage_print(&storage, dummy_msg_print);
    check_true("msg_storage_print executed", true);

    /*
     * 7) Basic pop test
     *
     * Current implementation behaves LIFO-like.
     */
    print_section("7) BASIC POP TESTS");
    pop_and_validate(&storage, &out, "pop #1", &m3, 1UL, 2);
    pop_and_validate(&storage, &out, "pop #2", &m2, 2UL, 1);
    pop_and_validate(&storage, &out, "pop #3", &m1, 3UL, 0);

    check_true("storage is empty after all pops", msg_storage_is_empty(&storage));
    check_true("storage is not full after all pops", !msg_storage_is_full(&storage));
    check_int("status after all pops", STORAGE_EMPTY, msg_storage_get_status(&storage));
    check_int("head time after all pops", INVALID_SIZE, (long)msg_storage_get_head_time(&storage));
    check_int("tail time after all pops", INVALID_SIZE, (long)msg_storage_get_tail_time(&storage));

    /*
     * 8) Pop from empty
     */
    print_section("8) EMPTY POP TEST");
    {
        storage_operation_status_t pop_rc = msg_storage_pop(&storage, &out);
        printf("pop on empty storage returned: %d\n", pop_rc);
        check_int("pop from empty storage should fail", OPERATION_FAILURE, pop_rc);
    }

    /*
     * 9) Circular overwrite test:
     * push 4 messages into 3-slot storage
     *
     * Expected logical content after overwrite:
     *   oldest m1 overwritten, remaining logical set: m2, m3, m4
     *
     * Since current pop behavior is LIFO-like, expected pops are:
     *   m4, m3, m2
     */
    print_section("9) CIRCULAR OVERWRITE TEST (4 PUSHES INTO 3 SLOTS)");
    msg_storage_cleanup(&storage);

    check_int("count after cleanup before overwrite test", 0, (long)msg_storage_get_count(&storage));
    check_ulong("total_pushed reset before overwrite test", 0UL, msg_storage_get_total_pushed(&storage));
    check_ulong("total_popped reset before overwrite test", 0UL, msg_storage_get_total_popped(&storage));

    msg_storage_push(&storage, &m1);
    msg_storage_push(&storage, &m2);
    msg_storage_push(&storage, &m3);
    msg_storage_push(&storage, &m4);

    printf("Storage after pushing m1,m2,m3,m4 into capacity 3:\n");
    msg_storage_print(&storage, dummy_msg_print);

    /*
     * Ideal circular-buffer expectation:
     * count should remain capped at capacity = 3
     * total_pushed should be 4
     */
    check_int("capacity remains 3 in overwrite test", 3, (long)msg_storage_get_capacity(&storage));
    check_ulong("total_pushed after 4 pushes into 3 slots", 4UL, msg_storage_get_total_pushed(&storage));

    /*
     * This check is intentionally strong.
     * If it fails, it exposes the overwrite/count bug.
     */
    check_int("count should stay capped at 3 after overwrite", 3, (long)msg_storage_get_count(&storage));
    check_true("storage should report full after overwrite", msg_storage_is_full(&storage));

    pop_and_validate(&storage, &out, "overwrite pop #1", &m4, 1UL, 2);
    pop_and_validate(&storage, &out, "overwrite pop #2", &m3, 2UL, 1);
    pop_and_validate(&storage, &out, "overwrite pop #3", &m2, 3UL, 0);

    {
        storage_operation_status_t pop_rc = msg_storage_pop(&storage, &out);
        printf("overwrite pop #4 returned: %d\n", pop_rc);
        check_int("overwrite pop #4 should fail because only 3 logical slots exist",
                  OPERATION_FAILURE,
                  pop_rc);
    }

    /*
     * 10) Stress overwrite test:
     * push 6 messages into 3-slot storage, then 4 pops
     *
     * Expected logical content after all pushes:
     *   m4, m5, m6 remain
     * LIFO-like pop order:
     *   m6, m5, m4
     * 4th pop should fail
     */
    print_section("10) STRESS OVERWRITE TEST (6 PUSHES INTO 3 SLOTS, THEN 4 POPS)");
    msg_storage_cleanup(&storage);

    msg_storage_push(&storage, &m1);
    msg_storage_push(&storage, &m2);
    msg_storage_push(&storage, &m3);
    msg_storage_push(&storage, &m4);
    msg_storage_push(&storage, &m5);
    msg_storage_push(&storage, &m6);

    printf("Storage after pushing m1..m6 into capacity 3:\n");
    msg_storage_print(&storage, dummy_msg_print);

    check_ulong("total_pushed after 6 pushes into 3 slots", 6UL, msg_storage_get_total_pushed(&storage));
    check_int("count should stay capped at 3 after 6 pushes into 3 slots", 3, (long)msg_storage_get_count(&storage));
    check_true("storage should report full after 6 pushes into 3 slots", msg_storage_is_full(&storage));

    pop_and_validate(&storage, &out, "stress pop #1", &m6, 1UL, 2);
    pop_and_validate(&storage, &out, "stress pop #2", &m5, 2UL, 1);
    pop_and_validate(&storage, &out, "stress pop #3", &m4, 3UL, 0);

    {
        storage_operation_status_t pop_rc = msg_storage_pop(&storage, &out);
        printf("stress pop #4 returned: %d\n", pop_rc);
        check_int("stress pop #4 should fail because only 3 logical slots exist",
                  OPERATION_FAILURE,
                  pop_rc);
    }

    /*
     * 11) Cleanup test
     */
    print_section("11) CLEANUP TEST");
    check_true("cleanup(NULL) should not crash", true);
    msg_storage_cleanup(NULL);

    msg_storage_push(&storage, &m1);
    msg_storage_push(&storage, &m2);

    check_int("count before cleanup", 2, (long)msg_storage_get_count(&storage));
    check_ulong("total_pushed before cleanup", 8UL, msg_storage_get_total_pushed(&storage));

    msg_storage_cleanup(&storage);
    printf("msg_storage_cleanup(&storage) called\n");

    check_int("count after cleanup", 0, (long)msg_storage_get_count(&storage));
    check_ulong("total_pushed after cleanup reset", 0UL, msg_storage_get_total_pushed(&storage));
    check_ulong("total_popped after cleanup reset", 0UL, msg_storage_get_total_popped(&storage));
    check_true("storage empty after cleanup", msg_storage_is_empty(&storage));
    check_int("status after cleanup", STORAGE_EMPTY, msg_storage_get_status(&storage));
    check_int("head after cleanup -> INVALID_SIZE via getter", INVALID_SIZE, (long)msg_storage_get_head_time(&storage));
    check_int("tail after cleanup -> INVALID_SIZE via getter", INVALID_SIZE, (long)msg_storage_get_tail_time(&storage));

    /*
     * 12) Delete test
     */
    print_section("12) DELETE TEST");
    check_true("delete(NULL) should not crash", true);
    msg_storage_delete(NULL);

    msg_storage_delete(&storage);
    printf("msg_storage_delete(&storage) called\n");

    check_true("storage pointer is NULL after delete", storage.storage == NULL);
    check_int("status after delete", STORAGE_ERROR, msg_storage_get_status(&storage));

    /*
     * 13) Operations after delete should fail
     */
    print_section("13) POST-DELETE ERROR TESTS");
    check_int("push after delete should fail",
              OPERATION_FAILURE,
              msg_storage_push(&storage, &m1));

    check_int("pop after delete should fail",
              OPERATION_FAILURE,
              msg_storage_pop(&storage, &out));

    /*
     * Summary
     */
    print_section("TEST SUMMARY");
    printf("Total checks : %d\n", g_checks_run);
    printf("Passed       : %d\n", g_checks_run - g_checks_failed);
    printf("Failed       : %d\n", g_checks_failed);

    if (g_checks_failed == 0) {
        printf("\nALL TESTS PASSED.\n");
        return 0;
    }

    printf("\nSOME TESTS FAILED.\n");
    return 1;
}