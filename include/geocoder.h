/*
 * Copyright (c) 2011-2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TIZEN_LOCATION_GEOCODER_H__
#define __TIZEN_LOCATION_GEOCODER_H__

#include <tizen_type.h>
#include <tizen_error.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup CAPI_LOCATION_GEOCODER_MODULE
 * @{
 */

/**
 * @brief The geocoder handle
 */
typedef struct geocoder_s *geocoder_h;
typedef struct geocoder_preference_s *geocoder_preference_h;


/**
 * @brief Enumerations of error code for Geocoder
 */
typedef enum
{
    GEOCODER_ERROR_NONE = TIZEN_ERROR_NONE,				/**< Successful */
    GEOCODER_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,		/**< Out of memory */
    GEOCODER_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,			/**< Invalid parameter */
    GEOCODER_ERROR_TIMED_OUT = TIZEN_ERROR_TIMED_OUT,  /**< Timeout error, no answer */
    GEOCODER_ERROR_NETWORK_FAILED = TIZEN_ERROR_LOCATION_CLASS | 0x02,			/**< Network unavailable*/
    GEOCODER_ERROR_SERVICE_NOT_AVAILABLE = TIZEN_ERROR_LOCATION_CLASS | 0x03,	/**< Service unavailable */
    GEOCODER_ERROR_NOT_FOUND = TIZEN_ERROR_LOCATION_CLASS | 0x04,	/**< Result not found */
    GEOCODER_ERROR_INVALID_KEY = TIZEN_ERROR_LOCATION_CLASS | 0x05,   /**< Invalid key */
} geocoder_error_e;

/**
 * @brief	Called once for each position information converted from the given address information.
 * @param[in] result The result of request
 * @param[in] latitude The latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude The longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre geocoder_foreach_positions_from_address() will invoke this callback.
 * @see geocoder_foreach_positions_from_address()
 */
typedef bool(*geocoder_get_position_cb)(geocoder_error_e result, double latitude, double longitude, void *user_data);


/**
 * @brief   Called when the address information has converted from position information.
 * @remarks You should not free all string values.
 * @param[in] result The result of request
 * @param[in] building_number	 The building number
 * @param[in] postal_code	The postal delivery code
 * @param[in] street	The full street name
 * @param[in] city	The city name
 * @param[in] district	The municipal district name
 * @param[in] state	The state or province region of a nation
 * @param[in] country_code	The country code
 * @param[in] user_data The user data passed from the foreach function
 * @pre geocoder_get_address_from_position() will invoke this callback.
 * @see	geocoder_get_address_from_position()
 */
typedef void (*geocoder_get_address_cb)(geocoder_error_e result, const char *building_number, const char *postal_code, const char *street, const  char *city, const char *district, const char *state, const char *country_code, void *user_data);

/**
 * @brief Called once for each available keys
 * @remarks @a key is valid only in this function.
 * @param   [in] key  The key string
 * @param   [in] uesr_data  The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 */
typedef bool (*geocoder_preference_available_key_cb)(const char *key , void *user_data);

/**
 * @brief Creates a new geocoder handle.
 * @details
 * A geocoder handle can be used to change position information to address information and vice versa.
 * @remarks @a geocoder must be released geocoder_destroy() by you.
 * @param   [out] geocoder  A handle of a new geocoder handle on success
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @see	geocoder_destroy()
 */
int geocoder_create(geocoder_h *geocoder);

/**
 * @brief	Destroys the geocoder handle and releases all its resources.
 * @param   [in] geocoder	The geocoder handle to destroy
 * @return  0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	geocoder_create()
 */
int geocoder_destroy(geocoder_h geocoder);

/**
 * @brief Gets the address for a given position, asynchronously.
 * @remarks This function requires network access.
 * @param[in] geocoder The geocoder handle
 * @param[in] latitude The latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude The longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] callback The callback which will receive address information
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED	Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @post This function invokes geocoder_get_address_cb().
 * @see	geocoder_get_address_cb()
 * @see geocoder_foreach_positions_from_address()
 */
int geocoder_get_address_from_position(geocoder_h geocoder, double latitude, double longitude, geocoder_get_address_cb callback, void *user_data);

/**
 * @brief Gets the positions for a given address, asynchronously.
 * @details This function gets positions for a given free-formed address string.
 * @remarks This function requires network access.
 * @param[in] geocoder  The geocoder handle
 * @param[in] address	The free-formed address
 * @param[in] callback	The geocoder get positions callback function
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED	Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @post It invokes geocoder_get_position_cb() to get changes in position.
 * @see	geocoder_get_position_cb()
 * @see geocoder_get_address_from_position()
 */
int geocoder_foreach_positions_from_address(geocoder_h geocoder, const char *address, geocoder_get_position_cb callback, void *user_data);

/**
 * @brief Called once for each position information converted from the given address information.
 * @param[in] result The result of request
 * @param[in] request_id The id of request
 * @param[in] index The current index of request
 * @param[in] total_count The total number of request
 * @param[in] latitude The latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude The longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre geocoder_geocode() will invoke this callback.
 * @see geocoder_geocode()
 */
typedef void (*geocoder_get_geocode_cb)(geocoder_error_e result, int request_id, int index, int total_count, double latitude, double longitude, void *user_data);

/**
 * @brief Called when the address information has converted from position information.
 * @remarks You should not free all string values.
 * @param[in] result The result of request
 * @param[in] request_id The id of request
 * @param[in] index The current index of request
 * @param[in] total_count The total number of request
 * @param[in] building_number The building number
 * @param[in] postal_code The postal delivery code
 * @param[in] street The full street name
 * @param[in] city The city name
 * @param[in] district The municipal district name
 * @param[in] state The state or province region of a nation
 * @param[in] country_code The country code
 * @param[in] country The country
 * @param[in] county The county
 * @param[in] text The description
 * @param[in] user_data The user data passed from the foreach function
 * @pre geocoder_reverse_geocode() will invoke this callback.
 * @see geocoder_reverse_geocode()
 */
typedef void (*geocoder_get_reverse_geocode_cb)(geocoder_error_e result, int request_id, int index, int total_count, const char *building_number, const char *postal_code, const char *street, const  char *city, const char *district, const char *state, const char *country_code, const char *country, const char *county, const char *text, void *user_data);

/**
 * @brief Gets the positions for a given address, asynchronously.
 * @details This function gets positions for a given free-formed address string.
 * @remarks This function requires network access.
 * @param[in] geocoder The geocoder handle
 * @param[in] address The free-formed address
 * @param[in] callback The geocoder get positions callback function
 * @param[in] user_data The user data to be passed to the callback function
 * @param[out] request_id The id of request
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @post It invokes geocoder_get_geocode_cb() to get changes in position.
 * @see	geocoder_get_geocode_cb()
 * @see geocoder_reverse_geocode()
 */
int geocoder_geocode(geocoder_h geocoder, const char *address, geocoder_get_geocode_cb callback, void *user_data, int *request_id);

/**
 * @brief Gets the address for a given position, asynchronously.
 * @remarks This function requires network access.
 * @param[in] geocoder The geocoder handle
 * @param[in] latitude The latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude The longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] callback The callback which will receive address information
 * @param[in] user_data The user data to be passed to the callback function
 * @param[out] request_id The id of request
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @post This function invokes geocoder_get_reverse_geocode_cb().
 * @see geocoder_get_reverse_geocode_cb()
 * @see geocoder_geocode()
 */
int geocoder_reverse_geocode(geocoder_h geocoder, double latitude, double longitude, geocoder_get_reverse_geocode_cb callback, void *user_data, int *request_id);

/**
 * @brief Cancels the reuqest.
 * @remarks This function requires network access.
 * @param[in] geocoder The geocoder handle
 * @param[in] request_id The id of request
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @see geocoder_reverse_geocode()
 * @see geocoder_geocode()
 */
int geocoder_cancel(geocoder_h geocoder, int request_id);


/**
 * @brief Creates a new geocoder preference handle.
 * @remarks @a preference must be released geocoder_preference_destroy() by you.
 * @param[out] preference A handle of a new geocoder preference handle on success
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @see geocoder_preference_destroy()
 */
int geocoder_preference_create (geocoder_preference_h *preference);

/**
 * @brief Destroys the geocoder preference handle and releases all its resources.
 * @param[in] preference The geocoder preference handle to destroy
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_preference_create()
 */
int geocoder_preference_destroy (geocoder_preference_h preference);

/**
 * @brief Gets the preference value
 * @remarks @a value must be released @c free() by you.
 * @param[in] preference The geocoder preference handle
 * @param[in] key The key of preference
 * @param[out] value The preference value
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_preference_set()
 */
int geocoder_preference_get(geocoder_preference_h preference, const char *key, char **value);

/**
 * @brief Sets the preference value
 * @param[in] preference The geocoder preference handle
 * @param[in] key The key of preference
 * @param[in] value The preference value to set, could be NULL if want to remove value
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_preference_get()
 */
int geocoder_preference_set(geocoder_preference_h preference, const char *key, const char *value);

/**
 * @brief Gets the number of maximum result
 * @param[in] preference The geocoder preference handle
 * @param[out] max_result The number of maximum result
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_preference_set_max_result()
 */
int geocoder_preference_get_max_result(geocoder_preference_h preference, int *max_result);

/**
 * @brief Sets the number of maximum result
 * @param[in] preference The geocoder preference handle
 * @param[in] max_result The number of maximum result
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_preference_get_max_result()
 */
int geocoder_preference_set_max_result(geocoder_preference_h preference, int max_result);

/**
 * @brief Retrieves all available preference keys
 * @param[in] preference The geocoder preference handle
 * @param[in] key The key of preference
 * @param[out] value The preference value
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_preference_set()
 * @see geocoder_preference_get()
 */
int geocoder_preference_foreach_available_keys(geocoder_h geocoder, geocoder_preference_available_key_cb callback , void * user_data);

/**
 * @brief Gets the geocoder preference
 * @param[in] geocoder The geocoder handle
 * @param[out] preference The geocoder preference handle
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_service_set_preference()
 */
int geocoder_service_get_preference(geocoder_h geocoder, geocoder_preference_h *preference);

/**
 * @brief Sets the geocoder preference
 * @param[in] geocoder The geocoder handle
 * @param[in] preference The geocoder preference handle
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER Invalid parameter
 * @see geocoder_service_get_preference()
 */
int geocoder_service_set_preference(geocoder_h geocoder, geocoder_preference_h preference);



/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_LOCATION_GEOCODER_H__ */
