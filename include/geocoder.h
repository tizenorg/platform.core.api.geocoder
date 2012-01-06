/*
* Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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


/**
 * @brief Enumerations of error code for Geocoder
 */
typedef enum
{
    GEOCODER_ERROR_NONE = TIZEN_ERROR_NONE,				/**< Successful */
    GEOCODER_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,		/**< Out of memory */
    GEOCODER_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,			/**< Invalid parameter */
    GEOCODER_ERROR_NETWORK_FAILED = TIZEN_ERROR_LOCATION_CLASS | 0x02,			/**< Network unavailable*/
    GEOCODER_ERROR_SERVICE_NOT_AVAILABLE = TIZEN_ERROR_LOCATION_CLASS | 0x03,	/**< Service unavailable */
    GEOCODER_ERROR_TIMED_OUT = TIZEN_ERROR_TIMED_OUT,  /**< Timeout error, no answer */
} geocoder_error_e;

/**
 * @brief	Called once for each position information converted from the given address information.
 * @param[in] latitude The latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude The longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre Either geocoder_foreach_positions_from_address() or geocoder_foreach_positions_from_address_sync() will invoke this callback.
 * @see geocoder_foreach_positions_from_address()
 * @see geocoder_foreach_positions_from_address_sync()
 */
typedef bool(*geocoder_get_position_cb)(double latitude, double longitude, void *user_data);


/**
 * @brief   Called when the address information has converted from position information.
 * @remarks You should not free all string values.
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
typedef void (*geocoder_get_address_cb)(const char *building_number, const char *postal_code, const char *street, const  char *city, const char *district, const char *state, const char *country_code, void *user_data);

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
 * @see geocoder_get_address_from_position_sync()
 * @see geocoder_foreach_positions_from_address()
 * @see geocoder_foreach_positions_from_address_sync()
 */
int geocoder_get_address_from_position(geocoder_h geocoder, double latitude, double longitude, geocoder_get_address_cb callback, void *user_data);

/**
 * @brief Gets the address for a given position.
 * @details The function tries to get the data immediately. If the attempt is unsuccessful, a corresponding error code is returned.
 * @remarks This function requires network access. \n All output values must be released with @c free() by you.
 * @param[in] geocoder The geocoder handle
 * @param[in] latitude The latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude The longitude [-180.0 ~ 180.0] (degrees)
 * @param[out] building_number	 The building number
 * @param[out] postal_code	The postal delivery code
 * @param[out] street	The full street name
 * @param[out] city	The city name
 * @param[out] district	The municipal district name
 * @param[out] state	The state or province region of a nation
 * @param[out] country_code	The country code
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED	Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @see geocoder_get_address_from_position()
 * @see geocoder_foreach_positions_from_address()
 * @see geocoder_foreach_positions_from_address_sync()
 */
int geocoder_get_address_from_position_sync(geocoder_h geocoder, double latitude, double longitude, char **building_number, char **postal_code, char **street, char **city, char **district, char **state, char **country_code);


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
 * @see geocoder_foreach_positions_from_address_sync()
 * @see geocoder_get_address_from_position()
 * @see geocoder_get_address_from_position_sync()
 */
int geocoder_foreach_positions_from_address(geocoder_h geocoder, const char *address, geocoder_get_position_cb callback, void *user_data);


/**
 * @brief Gets the positions for a given address.
 * @details This function gets the position for a given free-formed address string.
 * @remarks This function requires network access.
 * @param[in] geocoder The geocoder handle
 * @param[in] address   The free-formed address
 * @param[in] callback	The geocoder get positions callback function
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #GEOCODER_ERROR_NONE Successful
 * @retval #GEOCODER_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #GEOCODER_ERROR_NETWORK_FAILED	Network connection failed
 * @retval #GEOCODER_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @retval #GEOCODER_ERROR_TIMED_OUT Timed out
 * @post It invokes geocoder_get_position_cb() to get changes in position.
 * @see	geocoder_get_position_cb()
 * @see geocoder_foreach_positions_from_address()
 * @see geocoder_get_address_from_position()
 * @see geocoder_get_address_from_position_sync()
 */
int geocoder_foreach_positions_from_address_sync(geocoder_h geocoder, const char *address, geocoder_get_position_cb callback, void *user_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_LOCATION_GEOCODER_H__ */
