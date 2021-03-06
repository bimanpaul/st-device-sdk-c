/* ***************************************************************************
 *
 * Copyright (c) 2020 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <st_dev.h>
#include <string.h>
#include <iot_capability.h>
#include "TC_MOCK_functions.h"

#define UNUSED(x) (void*)(x)

int TC_iot_capability_setup(void **state)
{
    UNUSED(*state);

    set_mock_detect_memory_leak(true);

    return 0;
}

int TC_iot_capability_teardown(void **state)
{
    UNUSED(*state);

    do_not_use_mock_iot_os_malloc_failure();
    set_mock_detect_memory_leak(false);

    return 0;
}

void TC_st_cap_attr_create_int_null_attribute(void **state)
{
    IOT_EVENT* event;
    UNUSED(*state);

    // When: all null parameters
    event = st_cap_attr_create_int(NULL, 10, NULL);
    // Then: return null
    assert_null(event);

    // When: attribute is null
    event = st_cap_attr_create_int(NULL, 10, "F");
    // Then: return null
    assert_null(event);
}

void TC_st_cap_attr_create_int_null_unit(void **state)
{
    IOT_EVENT* event;
    iot_cap_evt_data_t* event_data = NULL;
    UNUSED(*state);

    // When: unit is null
    event = st_cap_attr_create_int("temperature", 10, NULL);
    // Then: return proper event data with unit type unused
    event_data = (iot_cap_evt_data_t*) event;
    assert_int_equal(event_data->evt_unit.type, IOT_CAP_UNIT_TYPE_UNUSED);
    assert_int_equal(event_data->evt_value.type, IOT_CAP_VAL_TYPE_INTEGER);
    assert_string_equal("temperature", event_data->evt_type);

    // Teardown
    st_cap_attr_free(event);
}

void TC_st_cap_attr_create_int_with_unit(void **state)
{
    IOT_EVENT* event;
    iot_cap_evt_data_t* event_data = NULL;
    UNUSED(*state);

    // When: unit is "F"
    event = st_cap_attr_create_int("temperature", 10, "C");
    // Then: return proper event data with unit type string
    event_data = (iot_cap_evt_data_t*) event;
    assert_int_equal(event_data->evt_unit.type, IOT_CAP_UNIT_TYPE_STRING);
    assert_string_equal("C", event_data->evt_unit.string);
    assert_int_equal(event_data->evt_value.type, IOT_CAP_VAL_TYPE_INTEGER);
    assert_string_equal("temperature", event_data->evt_type);

    // Teardown
    st_cap_attr_free(event);
}

void TC_st_cap_attr_create_int_internal_failure(void **state)
{
    IOT_EVENT* event;
    UNUSED(*state);

    // Given: malloc will fail
    set_mock_iot_os_malloc_failure();
    // When
    event = st_cap_attr_create_int("temperature", 10, "C");
    // Then: return null
    assert_null(event);
}

void TC_st_cap_attr_create_number_null_attribute(void **state)
{
    IOT_EVENT* event;
    UNUSED(*state);

    // When: all null parameters
    event = st_cap_attr_create_number(NULL, 56.7, NULL);
    // Then: return null
    assert_null(event);

    // When: attribute is null
    event = st_cap_attr_create_number(NULL, 56.7, "kg");
    // Then: return null
    assert_null(event);
}

void TC_st_cap_attr_create_number_null_unit(void **state)
{
    IOT_EVENT* event;
    iot_cap_evt_data_t* event_data = NULL;
    UNUSED(*state);

    // When: unit is null
    event = st_cap_attr_create_number("bodyWeightMeasurement", 56.7, NULL);
    // Then: return proper event data with unit type unused
    event_data = (iot_cap_evt_data_t*) event;
    assert_int_equal(event_data->evt_unit.type, IOT_CAP_UNIT_TYPE_UNUSED);
    assert_int_equal(event_data->evt_value.type, IOT_CAP_VAL_TYPE_NUMBER);
    assert_float_equal(event_data->evt_value.number, 56.7, 0);
    assert_string_equal(event_data->evt_type, "bodyWeightMeasurement");

    // Teardown
    st_cap_attr_free(event);
}

void TC_st_cap_attr_create_number_with_unit(void **state)
{
    IOT_EVENT* event;
    iot_cap_evt_data_t* event_data = NULL;
    UNUSED(*state);

    // When: unit is null
    event = st_cap_attr_create_number("bodyWeightMeasurement", 56.7, "kg");
    // Then: return proper event data with unit type string
    event_data = (iot_cap_evt_data_t*) event;
    assert_int_equal(event_data->evt_unit.type, IOT_CAP_UNIT_TYPE_STRING);
    assert_string_equal(event_data->evt_unit.string, "kg");
    assert_int_equal(event_data->evt_value.type, IOT_CAP_VAL_TYPE_NUMBER);
    assert_float_equal(event_data->evt_value.number, 56.7, 0);
    assert_string_equal(event_data->evt_type, "bodyWeightMeasurement");

    // Teardown
    st_cap_attr_free(event);
}

void TC_st_cap_attr_create_number_internal_failure(void **state)
{
    IOT_EVENT* event;
    UNUSED(*state);

    // Given: malloc will fail
    set_mock_iot_os_malloc_failure();
    // When
    event = st_cap_attr_create_number("bodyWeightMeasurement", 56.7, "kg");
    // Then: return null
    assert_null(event);
}

void TC_st_cap_attr_create_string_null_unit(void **state)
{
    IOT_EVENT* event;
    iot_cap_evt_data_t* event_data = NULL;
    UNUSED(*state);

    // When: unit is null
    event = st_cap_attr_create_string("powerSource", "battery", NULL);
    // Then: return proper event data with unit type string
    event_data = (iot_cap_evt_data_t*) event;
    assert_int_equal(event_data->evt_unit.type, IOT_CAP_UNIT_TYPE_UNUSED);
    assert_int_equal(event_data->evt_value.type, IOT_CAP_VAL_TYPE_STRING);
    assert_string_equal(event_data->evt_value.string, "battery");
    assert_string_equal(event_data->evt_type, "powerSource");

    // Teardown
    st_cap_attr_free(event);
}

void TC_st_cap_attr_create_string_with_unit(void **state)
{
    IOT_EVENT* event;
    iot_cap_evt_data_t* event_data = NULL;
    UNUSED(*state);

    // When: unit is null
    event = st_cap_attr_create_string("fakeAttribute", "fakeValue", "fakeUnit");
    // Then: return proper event data with unit type string
    event_data = (iot_cap_evt_data_t*) event;
    assert_int_equal(event_data->evt_unit.type, IOT_CAP_UNIT_TYPE_STRING);
    assert_string_equal(event_data->evt_unit.string, "fakeUnit");
    assert_int_equal(event_data->evt_value.type, IOT_CAP_VAL_TYPE_STRING);
    assert_string_equal(event_data->evt_value.string, "fakeValue");
    assert_string_equal(event_data->evt_type, "fakeAttribute");

    // Teardown
    st_cap_attr_free(event);
}

void TC_st_cap_attr_create_string_internal_failure(void **state)
{
    IOT_EVENT* event;
    UNUSED(*state);

    // Given: malloc will fail
    set_mock_iot_os_malloc_failure();
    // When
    event = st_cap_attr_create_string("fakeAttribute", "fakeValue", "fakeUnit");
    // Then: return null
    assert_null(event);
}

void TC_st_cap_attr_create_string_null_parameters(void **state)
{
    IOT_EVENT* event;
    UNUSED(*state);

    // When: all null parameters
    event = st_cap_attr_create_string(NULL, "fakeValue", NULL);
    // Then: return null
    assert_null(event);

    // When: attribute is null
    event = st_cap_attr_create_string(NULL, "fakeValue", "fakeUnit");
    // Then: return null
    assert_null(event);

    // When: value is null
    event = st_cap_attr_create_string("fakeAttribute", NULL, "fakeUnit");
    // Then: return null
    assert_null(event);

    // When: all null
    event = st_cap_attr_create_string(NULL, NULL, NULL);
    // Then: return null
    assert_null(event);
}

void test_cap_init_callback(IOT_CAP_HANDLE *handle, void *usr_data)
{
    assert_non_null(handle);
    UNUSED(usr_data);
}

void TC_st_cap_handle_init_invalid_argument(void **state)
{
    IOT_CAP_HANDLE *cap_handle;
    char *usr_data;
    UNUSED(*state);

    // Given
    usr_data = strdup("UserString");
    // When: IOT_CTX null
    cap_handle = st_cap_handle_init(NULL, "main", "switch", test_cap_init_callback, usr_data);
    // Then
    assert_null(cap_handle);
    // Teardown
    free(usr_data);

    // Given
    usr_data = strdup("UserString");
    // When: IOT_CTX, capability null
    cap_handle = st_cap_handle_init(NULL, "main", NULL, test_cap_init_callback, usr_data);
    // Then
    assert_null(cap_handle);
    // Teardown
    free(usr_data);

    // Given
    usr_data = strdup("UserString");
    // When: IOT_CTX, component and capability null
    cap_handle = st_cap_handle_init(NULL, NULL, NULL, test_cap_init_callback, usr_data);
    // Then
    assert_null(cap_handle);
    // Teardown
    free(usr_data);

    // Given
    usr_data = strdup("UserString");
    // When: IOT_CTX, component,capability and init_cb null
    cap_handle = st_cap_handle_init(NULL, NULL, NULL, NULL, usr_data);
    // Then
    assert_null(cap_handle);
    // Teardown
    free(usr_data);

    // When: all null
    cap_handle = st_cap_handle_init(NULL, NULL, NULL, NULL, NULL);
    // Then
    assert_null(cap_handle);
}

void TC_st_cap_handle_init_internal_failure(void **state)
{
    IOT_CAP_HANDLE *cap_handle;
    IOT_CTX *context;
    char *usr_data;
    UNUSED(*state);

    for (int i = 0; i < 2; i++) {
        // Given: valid parameters but n-th malloc failure
        usr_data = strdup("UserString");
        context = (IOT_CTX*) malloc(sizeof(struct iot_context));
        memset(context, 0, sizeof(struct iot_context));
        set_mock_iot_os_malloc_failure_with_index(i);
        // When
        cap_handle = st_cap_handle_init(context, "main", "switch", test_cap_init_callback, usr_data);
        // Then
        assert_null(cap_handle);
        // Teardown
        free(context);
        free(usr_data);
        do_not_use_mock_iot_os_malloc_failure();
    }
}

void TC_st_cap_handle_init_success(void **state)
{
    IOT_CAP_HANDLE *cap_handle;
    struct iot_cap_handle *handle;
    struct iot_context *ctx = NULL;
    IOT_CTX *context;
    char *usr_data;
    UNUSED(*state);


    // Given
    usr_data = strdup("UserString");
    context = (IOT_CTX*)malloc(sizeof(struct iot_context));
    memset(context, 0, sizeof(struct iot_context));
    // When
    cap_handle = st_cap_handle_init(context, "main", "switch", test_cap_init_callback, usr_data);
    // Then
    handle = (struct iot_cap_handle*)cap_handle;
    ctx = (struct iot_context*) context;
    assert_non_null(cap_handle);
    assert_ptr_equal(ctx->cap_handle_list->handle, handle);
    assert_null(ctx->cap_handle_list->next);
    assert_null(handle->cmd_list);
    assert_string_equal(handle->component, "main");
    assert_string_equal(handle->capability, "switch");
    assert_ptr_equal(handle->init_cb, test_cap_init_callback);
    assert_ptr_equal(handle->init_usr_data, usr_data);
    assert_ptr_equal(handle->ctx, ctx);
    // Teardown
    if (handle->capability) {
        iot_os_free((void*)handle->capability);
    }
    if (handle->component) {
        iot_os_free((void*)handle->component);
    }
    if (ctx->cap_handle_list) {
        iot_os_free(ctx->cap_handle_list);
    }
    iot_os_free(cap_handle);
    free(context);
    free(usr_data);

    // Given: Already existing handle in conext
    usr_data = strdup("UserString");
    context = (IOT_CTX*) malloc(sizeof(struct iot_context));
    memset(context, 0, sizeof(struct iot_context));
    ctx = (struct iot_context*) context;
    ctx->cap_handle_list = malloc(sizeof(iot_cap_handle_list_t));
    ctx->cap_handle_list->next = NULL;
    // When
    cap_handle = st_cap_handle_init(context, "main", "switch", test_cap_init_callback, usr_data);
    // Then
    handle = (struct iot_cap_handle*)cap_handle;
    assert_non_null(cap_handle);
    assert_non_null(ctx->cap_handle_list->next);
    assert_ptr_equal(ctx->cap_handle_list->next->handle, handle);
    assert_null(ctx->cap_handle_list->next->next);
    assert_null(handle->cmd_list);
    assert_string_equal(handle->component, "main");
    assert_string_equal(handle->capability, "switch");
    assert_ptr_equal(handle->init_cb, test_cap_init_callback);
    assert_ptr_equal(handle->init_usr_data, usr_data);
    assert_ptr_equal(handle->ctx, ctx);
    // Teardown
    if (handle->capability) {
        iot_os_free((void*)handle->capability);
    }
    if (handle->component) {
        iot_os_free((void*)handle->component);
    }
    if (ctx->cap_handle_list->next) {
        iot_os_free(ctx->cap_handle_list->next);
    }
    if (ctx->cap_handle_list) {
        free(ctx->cap_handle_list);
    }
    iot_os_free(cap_handle);
    free(context);
    free(usr_data);
}

static void test_st_cap_noti_cb(iot_noti_data_t *noti_data, void *noti_usr_data)
{
    assert_non_null(noti_data);
    UNUSED(noti_usr_data);
}

void TC_st_conn_set_noti_cb_null_parameters(void **state)
{
    int ret;
    IOT_CTX* context;
    struct iot_context *internal_context;
    char *user_data;
    UNUSED(*state);

    // When: all parameters null
    ret = st_conn_set_noti_cb(NULL, NULL, NULL);
    // Then
    assert_int_not_equal(ret, 0);

    // Given
    internal_context = (struct iot_context *)malloc(sizeof(struct iot_context));
    memset(internal_context, 0, sizeof(struct iot_context));
    context = (IOT_CTX*) internal_context;
    // When: notification callback null
    ret = st_conn_set_noti_cb(context, NULL, NULL);
    // Then
    assert_int_not_equal(ret, 0);
    // Teardown
    free(context);

    // When: context null
    ret = st_conn_set_noti_cb(NULL, test_st_cap_noti_cb, NULL);
    // Then
    assert_int_not_equal(ret, 0);

    // Given
    user_data = strdup("fakeData");
    // When: context, notification callback null
    ret = st_conn_set_noti_cb(NULL, NULL, (void*)user_data);
    // Then
    assert_int_not_equal(ret, 0);
    // Teardown
    free(user_data);
}

void TC_st_conn_set_noti_cb_success(void **state)
{
    int ret;
    IOT_CTX* context;
    struct iot_context *internal_context;
    char *user_data;
    UNUSED(*state);

    // Given
    internal_context = (struct iot_context *)malloc(sizeof(struct iot_context));
    memset(internal_context, 0, sizeof(struct iot_context));
    context = (IOT_CTX*) internal_context;
    user_data = strdup("fakeData");
    // When: notification callback null
    ret = st_conn_set_noti_cb(context, test_st_cap_noti_cb, (void*)user_data);
    // Then
    assert_int_equal(ret, 0);
    assert_ptr_equal(internal_context->noti_cb, test_st_cap_noti_cb);
    assert_ptr_equal(internal_context->noti_usr_data, user_data);
    // Teardown
    free(context);
    free(user_data);
}

static void test_cap_cmd_cb(IOT_CAP_HANDLE *cap_handle,
                      iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    assert_non_null(cap_handle);
    UNUSED(cmd_data);
    UNUSED(usr_data);
}

void TC_st_cap_cmd_set_cb_invalid_parameters(void **state)
{
    int ret;
    struct iot_cap_handle *internal_handle;
    IOT_CAP_HANDLE* handle;
    char *user_data;

    // When: all null
    ret = st_cap_cmd_set_cb(NULL, NULL, NULL, NULL);
    // Then
    assert_int_not_equal(ret, 0);

    // Given
    user_data = strdup("fakeData");
    // When: null handle
    ret = st_cap_cmd_set_cb(NULL, "fakeCommand", test_cap_cmd_cb, (void*)user_data);
    // Then
    assert_int_not_equal(ret, 0);
    // Teardown
    free(user_data);

    // Given
    internal_handle = (struct iot_cap_handle*) malloc(sizeof(struct iot_cap_handle));
    memset(internal_handle, '\0', sizeof(struct iot_cap_handle));
    handle = (IOT_CAP_HANDLE*) internal_handle;
    user_data = strdup("fakeData");
    // When: cmd_type null
    ret = st_cap_cmd_set_cb(handle, NULL, test_cap_cmd_cb, (void*)user_data);
    // Then
    assert_int_not_equal(ret, 0);
    assert_null(internal_handle->cmd_list);
    // Teardown
    free(user_data);
    free(internal_handle);

    // Given
    internal_handle = (struct iot_cap_handle*) malloc(sizeof(struct iot_cap_handle));
    memset(internal_handle, '\0', sizeof(struct iot_cap_handle));
    handle = (IOT_CAP_HANDLE*) internal_handle;
    user_data = strdup("fakeData");
    // When: cmd_cb null
    ret = st_cap_cmd_set_cb(handle, "fakeCommand", NULL, (void*)user_data);
    // Then
    assert_int_not_equal(ret, 0);
    assert_null(internal_handle->cmd_list);
    // Teardown
    free(user_data);
    free(internal_handle);
}

void TC_st_cap_cmd_set_cb_success(void **state)
{
    int ret;
    struct iot_cap_handle *internal_handle;
    IOT_CAP_HANDLE* handle;
    char *user_data;

    // Given
    internal_handle = (struct iot_cap_handle*) malloc(sizeof(struct iot_cap_handle));
    memset(internal_handle, '\0', sizeof(struct iot_cap_handle));
    handle = (IOT_CAP_HANDLE*) internal_handle;
    user_data = strdup("fakeData");
    // When
    ret = st_cap_cmd_set_cb(handle, "fakeCommand", test_cap_cmd_cb, (void*)user_data);
    // Then
    assert_int_equal(ret, 0);
    assert_non_null(internal_handle->cmd_list);
    assert_non_null(internal_handle->cmd_list->command);
    assert_null(internal_handle->cmd_list->next);
    assert_string_equal(internal_handle->cmd_list->command->cmd_type, "fakeCommand");
    assert_ptr_equal(internal_handle->cmd_list->command->cmd_cb, test_cap_cmd_cb);
    assert_ptr_equal(internal_handle->cmd_list->command->usr_data, user_data);
    // Teardown
    free(user_data);
    iot_os_free((void*)internal_handle->cmd_list->command->cmd_type);
    iot_os_free(internal_handle->cmd_list->command);
    iot_os_free(internal_handle->cmd_list);
    free(internal_handle);
}