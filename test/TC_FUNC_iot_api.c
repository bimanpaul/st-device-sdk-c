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
#include <iot_error.h>
#include <iot_internal.h>
#include <iot_os_util.h>
#include <iot_easysetup.h>
#include <string.h>
#include "TC_MOCK_functions.h"

#define UNUSED(x) (void**)(x)

static char device_info_sample[] = {
        "{\n"
        "\t\"deviceInfo\": {\n"
        "\t\t\"firmwareVersion\": \"MyTestingFirmwareVersion\",\n"
        "\t\t\"privateKey\": \"privateKey_here\",\n"
        "\t\t\"publicKey\": \"publicKey_here\",\n"
        "\t\t\"serialNumber\": \"serialNumber_here\"\n"
        "\t}\n"
        "}"
};

int TC_iot_api_memleak_detect_setup(void **state)
{
    UNUSED(state);
    set_mock_detect_memory_leak(true);
    return 0;
}

int TC_iot_api_memleak_detect_teardown(void **state)
{
    UNUSED(state);
    set_mock_detect_memory_leak(false);
    return 0;
}

void TC_iot_api_device_info_load_null_parameters(void **state)
{
    iot_error_t err;
    struct iot_device_info info;
    UNUSED(state);

    // When: All parameters null
    err = iot_api_device_info_load(NULL, 10, NULL);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);

    // When: device_info is null
    err = iot_api_device_info_load(NULL, 10, &info);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);

    // When: info is null
    err = iot_api_device_info_load(device_info_sample, sizeof(device_info_sample), NULL);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);
}

void TC_iot_api_device_info_load_success(void **state)
{
    iot_error_t err;
    struct iot_device_info info;
    UNUSED(state);

    // When: valid input
    err = iot_api_device_info_load(device_info_sample, sizeof(device_info_sample), &info);
    // Then: success
    assert_int_equal(err, IOT_ERROR_NONE);
    assert_string_equal("MyTestingFirmwareVersion", info.firmware_version);

    // local teardown
    iot_api_device_info_mem_free(&info);
}

void TC_iot_api_device_info_load_internal_failure(void **state)
{
    iot_error_t err;
    struct iot_device_info info;
    UNUSED(state);

    for (unsigned int i = 0; i < 2; i++) {
        // Given: i-th malloc failure
        memset(&info, '\0', sizeof(struct iot_device_info));
        do_not_use_mock_iot_os_malloc_failure();
        set_mock_iot_os_malloc_failure_with_index(i);
        // When: valid input
        err = iot_api_device_info_load(device_info_sample, sizeof(device_info_sample), &info);
        // Then: success
        assert_int_not_equal(err, IOT_ERROR_NONE);
        // local teardown
        iot_api_device_info_mem_free(&info);
    }

    // Teardown
    do_not_use_mock_iot_os_malloc_failure();
}

static char device_info_sample_without_firmware_version[] = {
        "{\n"
        "\t\"deviceInfo\": {\n"
        "\t\t\"privateKey\": \"privateKey_here\",\n"
        "\t\t\"publicKey\": \"publicKey_here\",\n"
        "\t\t\"serialNumber\": \"serialNumber_here\"\n"
        "\t}\n"
        "}"
};

void TC_iot_api_device_info_load_without_firmware_version(void **state)
{
    iot_error_t err;
    struct iot_device_info info;
    UNUSED(state);

    // Given
    memset(&info, '\0', sizeof(struct iot_device_info));
    // When: malformed json
    err = iot_api_device_info_load(device_info_sample_without_firmware_version, sizeof(device_info_sample_without_firmware_version), &info);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);

    // local teardown
    iot_api_device_info_mem_free(&info);
}

static char onboarding_profile_template[] = {
        "{\n"
        "  \"onboardingConfig\": {\n"
        "    \"deviceOnboardingId\": \"NAME\",\n"
        "    \"mnId\": \"MNID\",\n"
        "    \"setupId\": \"999\",\n"
        "    \"vid\": \"VID\",\n"
        "    \"deviceTypeId\": \"TYPE\",\n"
        "    \"ownershipValidationTypes\": [\n"
        "      \"JUSTWORKS\",\n"
        "      \"BUTTON\",\n"
        "      \"PIN\",\n"
        "      \"QR\"\n"
        "    ],\n"
        "    \"identityType\": \"ED25519_or_CERTIFICATE\"\n"
        "  }\n"
        "}"
};

void TC_iot_api_onboarding_config_load_null_parameters(void **state)
{
    iot_error_t err;
    struct iot_devconf_prov_data devconf;
    UNUSED(state);

    // When: All parameters null
    err = iot_api_onboarding_config_load(NULL, 0, NULL);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);

    // When: NULL pointer at output parameter
    err = iot_api_onboarding_config_load(onboarding_profile_template, sizeof(onboarding_profile_template), NULL);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);

    // When: NULL pointer at output parameter
    err = iot_api_onboarding_config_load(NULL, 0, &devconf);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);
}

void TC_iot_api_onboarding_config_load_template_parameters(void **state)
{
    iot_error_t err;
    struct iot_devconf_prov_data devconf;
    UNUSED(state);

    // Given
    memset(&devconf, '\0', sizeof(struct iot_devconf_prov_data));
    // When: template is used as parameter
    err = iot_api_onboarding_config_load(onboarding_profile_template, sizeof(onboarding_profile_template), &devconf);
    // Then: returns error
    assert_int_not_equal(err, IOT_ERROR_NONE);

    // local teardown
    iot_api_onboarding_config_mem_free(&devconf);
}

static char onboarding_profile_example[] = {
        "{\n"
        "  \"onboardingConfig\": {\n"
        "    \"deviceOnboardingId\": \"STDK\",\n"
        "    \"mnId\": \"fTST\",\n"
        "    \"setupId\": \"001\",\n"
        "    \"vid\": \"STDK_BULB_0001\",\n"
        "    \"deviceTypeId\": \"Switch\",\n"
        "    \"ownershipValidationTypes\": [\n"
        "      \"JUSTWORKS\",\n"
        "      \"BUTTON\",\n"
        "      \"PIN\",\n"
        "      \"QR\"\n"
        "    ],\n"
        "    \"identityType\": \"ED25519\"\n"
        "  }\n"
        "}"
};

void TC_iot_api_onboarding_config_load_success(void **state)
{
    iot_error_t err;
    struct iot_devconf_prov_data devconf;
    UNUSED(state);

    // When: valid parameters
    err = iot_api_onboarding_config_load(onboarding_profile_example, sizeof(onboarding_profile_example), &devconf);
    // Then: success
    assert_int_equal(err, IOT_ERROR_NONE);
    assert_string_equal("STDK", devconf.device_onboarding_id);
    assert_string_equal("fTST", devconf.mnid);
    assert_string_equal("001", devconf.setupid);
    assert_string_equal("STDK_BULB_0001", devconf.vid);
    assert_string_equal("Switch", devconf.device_type);
    assert_true((unsigned)devconf.ownership_validation_type & (unsigned)IOT_OVF_TYPE_BUTTON);
    assert_true((unsigned)devconf.ownership_validation_type & (unsigned)IOT_OVF_TYPE_JUSTWORKS);
    assert_true((unsigned)devconf.ownership_validation_type & (unsigned)IOT_OVF_TYPE_PIN);
    assert_true((unsigned)devconf.ownership_validation_type & (unsigned)IOT_OVF_TYPE_QR);

    // Local teardown
    iot_api_onboarding_config_mem_free(&devconf);
}

void TC_iot_api_onboarding_config_load_internal_failure(void **state)
{
    iot_error_t err;
    struct iot_devconf_prov_data devconf;
    UNUSED(state);

    for (unsigned int i = 0; i < 6; i++) {
        // Given: i-th malloc failure
        memset(&devconf, '\0', sizeof(struct iot_devconf_prov_data));
        do_not_use_mock_iot_os_malloc_failure();
        set_mock_iot_os_malloc_failure_with_index(i);
        // When: valid parameters
        err = iot_api_onboarding_config_load(onboarding_profile_example, sizeof(onboarding_profile_example), &devconf);
        // Then: failure
        assert_int_not_equal(err, IOT_ERROR_NONE);
        // Local teardown
        iot_api_onboarding_config_mem_free(&devconf);
    }

    // Teardown
    do_not_use_mock_iot_os_malloc_failure();
}

static char onboarding_profile_without_mnid[] = {
        "{\n"
        "  \"onboardingConfig\": {\n"
        "    \"deviceOnboardingId\": \"STDK\",\n"
        "    \"setupId\": \"001\",\n"
        "    \"vid\": \"STDK_BULB_0001\",\n"
        "    \"deviceTypeId\": \"Switch\",\n"
        "    \"ownershipValidationTypes\": [\n"
        "      \"JUSTWORKS\",\n"
        "      \"BUTTON\",\n"
        "      \"PIN\",\n"
        "      \"QR\"\n"
        "    ],\n"
        "    \"identityType\": \"ED25519\"\n"
        "  }\n"
        "}"
};

void TC_iot_api_onboarding_config_without_mnid(void **state)
{
    iot_error_t err;
    struct iot_devconf_prov_data devconf;
    UNUSED(state);

    // Given
    memset(&devconf, '\0', sizeof(struct iot_devconf_prov_data));
    // When: malformed parameters
    err = iot_api_onboarding_config_load(onboarding_profile_without_mnid, sizeof(onboarding_profile_without_mnid), &devconf);
    // Then: returns fail
    assert_int_not_equal(err, IOT_ERROR_NONE);

    // Local teardown
    iot_api_onboarding_config_mem_free(&devconf);
}

void TC_iot_get_time_in_sec_null_parameters(void **state)
{
    iot_error_t err;
    UNUSED(state);

    // When: null parameters
    err = iot_get_time_in_sec(NULL, 0);
    // Then: return error
    assert_int_not_equal(err, IOT_ERROR_NONE);
}

void TC_iot_get_time_in_sec_success(void **state)
{
    iot_error_t err;
    char time_buffer[32];
    UNUSED(state);

    // Given
    memset(time_buffer, '\0', sizeof(time_buffer));
    // When: valid parameters
    err = iot_get_time_in_sec(time_buffer, sizeof(time_buffer));
    // Then: return success
    assert_int_equal(err, IOT_ERROR_NONE);
    assert_true(strlen(time_buffer) > 0);
    assert_int_not_equal(atol(time_buffer), 0);
}

void TC_iot_get_time_in_ms_null_parmaeters(void **state)
{
    iot_error_t err;
    UNUSED(state);

    // When: null parameters
    err = iot_get_time_in_ms(NULL, 0);
    // Then: return error
    assert_int_not_equal(err, IOT_ERROR_NONE);
}

void TC_iot_get_time_in_ms_success(void **state)
{
    iot_error_t err;
    char time_buffer[32];
    UNUSED(state);

    // Given
    memset(time_buffer, '\0', sizeof(time_buffer));
    // When: valid parameters
    err = iot_get_time_in_ms(time_buffer, sizeof(time_buffer));
    // Then: return success
    assert_int_equal(err, IOT_ERROR_NONE);
    assert_true(strlen(time_buffer) > 0);
    assert_int_not_equal(atol(time_buffer), 0);
}

void TC_iot_get_time_in_sec_by_long_null_parameters(void **state)
{
    iot_error_t err;
    UNUSED(state);

    // When: null parameter
    err = iot_get_time_in_sec_by_long(NULL);
    // Then: return error
    assert_int_not_equal(err, IOT_ERROR_NONE);
}

void TC_iot_get_time_in_sec_by_long_success(void **state)
{
    iot_error_t err;
    long seconds = 0;
    UNUSED(state);

    // When: valid parameter
    err = iot_get_time_in_sec_by_long(&seconds);
    // Then: return success
    assert_int_equal(err, IOT_ERROR_NONE);
    assert_true(seconds > 0);
}