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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <geocoder_private.h>
#include  <location-geocode.h>

/*
* Internal Implementation
*/

typedef struct {
	void *data;
	geocoder_get_address_cb callback;
}__addr_callback_data;

typedef struct {
	void *data;
	geocoder_get_position_cb callback;
}__pos_callback_data;

typedef struct {
	int request_id;
	void *data;
	geocoder_get_geocode_cb callback;
}__geocode_callback_data;

typedef struct {
	int request_id;
	void *data;
	geocoder_get_reverse_geocode_cb callback;
}__reverse_geocode_callback_data;

static int __convert_error_code(int code)
{
	int ret;
	char* msg = "GEOCODER_ERROR_NONE";
	switch(code)
	{
		case LOCATION_ERROR_NONE:
			ret = GEOCODER_ERROR_NONE;
			msg = "GEOCODER_ERROR_NONE";
			break;
		case LOCATION_ERROR_NETWORK_FAILED:
		case LOCATION_ERROR_NETWORK_NOT_CONNECTED:
			ret = GEOCODER_ERROR_NETWORK_FAILED;
			msg = "GEOCODER_ERROR_NETWORK_FAILED";
			break;
		case LOCATION_ERROR_PARAMETER:
			ret = GEOCODER_ERROR_INVALID_PARAMETER;
			msg = "GEOCODER_ERROR_INVALID_PARAMETER";
			break;
		case LOCATION_ERROR_NOT_FOUND:
			ret = GEOCODER_ERROR_NOT_FOUND;
			msg = "GEOCODER_ERROR_NOT_FOUND";
			break;
		case LOCATION_ERROR_NOT_ALLOWED:
		case LOCATION_ERROR_NOT_AVAILABLE:
		case LOCATION_ERROR_CONFIGURATION:
		case LOCATION_ERROR_UNKNOWN:
		default:
			msg = "GEOCODER_ERROR_SERVICE_NOT_AVAILABLE";
			ret = GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
	}
	GEOCODER_LOGE("%s(0x%08x) : core fw error(0x%x)", msg, ret, code);
	return ret;
}

static void __cb_address_from_position (LocationError error, LocationAddress *addr, LocationAccuracy *acc, gpointer userdata)
{
	__addr_callback_data * callback = (__addr_callback_data*)userdata;
	if( callback == NULL || callback->callback == NULL)
	{
		GEOCODER_LOGI("callback is NULL )");
		return ;
	}

	if(error != LOCATION_ERROR_NONE || addr == NULL)
	{
		callback->callback(__convert_error_code(error), NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, callback->data);
	}
	else
	{
		GEOCODER_LOGI("[%s] Address - building number: %s, postal code: %s, street: %s, city: %s, district:  %s, state: %s, country code: %s", __FUNCTION__ , addr->building_number, addr->postal_code, addr->street, addr->city, addr->district, addr->state, addr->country_code);
		callback->callback(GEOCODER_ERROR_NONE, addr->building_number, addr->postal_code, addr->street, addr->city, addr->district, addr->state, addr->country_code, callback->data);
	}
	free(callback);
}

static void __cb_position_from_address (LocationError error, GList *position_list, GList *accuracy_list, gpointer userdata)
{
	__pos_callback_data * callback = (__pos_callback_data*)userdata;
	if( callback == NULL || callback->callback == NULL)
	{
		GEOCODER_LOGI("callback is NULL )");
		return ;
	}

	if(error != LOCATION_ERROR_NONE || position_list == NULL || position_list->data ==NULL || accuracy_list==NULL )
	{
		callback->callback(__convert_error_code(error), 0, 0, callback->data);
	}
	else
	{
		while(position_list)
		{
			LocationPosition *pos = position_list->data;
			if ( callback->callback(GEOCODER_ERROR_NONE, pos->latitude, pos->longitude, callback->data) != TRUE )
			{
				GEOCODER_LOGI("[%s] User quit the loop ");
				break;
			}
			position_list = g_list_next(position_list);
		}
	}
	free(callback);
}

/*
* Public Implementation
*/
EXPORT_API int geocoder_create(geocoder_h* geocoder)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	if(location_init()!=LOCATION_ERROR_NONE)
	{
		GEOCODER_LOGE("GEOCODER_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : fail to location_init", GEOCODER_ERROR_SERVICE_NOT_AVAILABLE);
		return GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
	}

	geocoder_s *handle = (geocoder_s*)malloc(sizeof(geocoder_s));
	if(handle==NULL)
	{
		GEOCODER_LOGE("OUT_OF_MEMORY(0x%08x)", GEOCODER_ERROR_OUT_OF_MEMORY);
		return GEOCODER_ERROR_OUT_OF_MEMORY;
	}

	memset(handle, 0 , sizeof(geocoder_s));

	if( GEOCODER_ERROR_NONE != geocoder_preference_create(&handle->preference ) ){
		free(handle);
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_OUT_OF_MEMORY);
	}
	handle->object = location_map_new(NULL);
	if(handle->object  == NULL)
	{
		free(handle);
		GEOCODER_LOGE("GEOCODER_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : fail to location_map_new", GEOCODER_ERROR_SERVICE_NOT_AVAILABLE);
		return GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
	}

	*geocoder = (geocoder_h)handle;
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_destroy(geocoder_h geocoder)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	geocoder_s *handle = (geocoder_s*)geocoder;

	int ret = location_map_free(handle->object);
	if(ret!=GEOCODER_ERROR_NONE)
	{
		return __convert_error_code(ret);
	}
	free(handle);
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_get_address_from_position(geocoder_h geocoder, double latitude, double longitude, geocoder_get_address_cb callback, void *user_data)
{
	unsigned int reqid;//Need to check whether we need to change the parameter of the API.
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(callback);
	GEOCODER_CHECK_CONDITION(latitude>=-90 && latitude<=90 ,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER");
	GEOCODER_CHECK_CONDITION(longitude>=-180 && longitude<=180,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER");

	geocoder_s *handle = (geocoder_s*)geocoder;
	int ret;
	LocationPosition *pos = NULL;
	pos = location_position_new (0, latitude, longitude, 0, LOCATION_STATUS_2D_FIX);

	__addr_callback_data * calldata = (__addr_callback_data *)malloc(sizeof(__addr_callback_data));
	if( calldata == NULL)
	{
		GEOCODER_LOGE("GEOCODER_ERROR_OUT_OF_MEMORY(0x%08x) : fail to create callback data", GEOCODER_ERROR_OUT_OF_MEMORY);
		return GEOCODER_ERROR_OUT_OF_MEMORY;
	}
	calldata->callback = callback;
	calldata->data = user_data;

	ret = location_map_get_address_from_position_async(handle->object, pos, __cb_address_from_position, calldata,&reqid);
	location_position_free(pos);
	if( ret != LOCATION_ERROR_NONE)
	{
		free(calldata);
		return __convert_error_code(ret);
	}
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_foreach_positions_from_address(geocoder_h geocoder,const char* address, geocoder_get_position_cb callback, void *user_data)
{
	unsigned int reqid;//Need to check whether we need to change the parameter of the API.
	int ret;
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(address);
	GEOCODER_NULL_ARG_CHECK(callback);

	geocoder_s *handle = (geocoder_s*)geocoder;
	char* addr_str = g_strdup(address);

	__pos_callback_data * calldata = (__pos_callback_data *)malloc(sizeof(__pos_callback_data));
	if( calldata == NULL)
	{
		GEOCODER_LOGE("GEOCODER_ERROR_OUT_OF_MEMORY(0x%08x) : fail to create callback data", GEOCODER_ERROR_OUT_OF_MEMORY);
		return GEOCODER_ERROR_OUT_OF_MEMORY;
	}
	calldata->callback = callback;
	calldata->data = user_data;

	ret = location_map_get_position_from_freeformed_address_async(handle->object, addr_str, (LocationGeocodePreference *)handle->preference, __cb_position_from_address, calldata,&reqid);
	g_free(addr_str);
	if( ret != LOCATION_ERROR_NONE)
	{
		return __convert_error_code(ret);
	}
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int
geocoder_geocode(geocoder_h geocoder, const char *address, geocoder_get_geocode_cb callback, void *user_data, int *request_id)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(callback);

	unsigned int reqid;
	int ret;
	geocoder_s *handle = (geocoder_s*)geocoder;

	__geocode_callback_data * calldata = (__geocode_callback_data *)malloc(sizeof(__geocode_callback_data));
	if( calldata == NULL)
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_OUT_OF_MEMORY);

	calldata->callback = callback;
	calldata->data = user_data;

	ret = location_map_get_position_from_freeformed_address_async(handle->object , address, (LocationGeocodePreference *)handle->preference, __cb_position_from_address, calldata, &reqid);

	if( ret != 0 ){
		free(calldata);
		return __convert_error_code(ret);
	}

	if( request_id )
		*request_id = reqid;
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_reverse_geocode(geocoder_h geocoder, double latitude, double longitude, geocoder_get_reverse_geocode_cb callback, void *user_data, int *request_id)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(callback);
	GEOCODER_CHECK_CONDITION(latitude>=-90 && latitude<=90 ,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER");
	GEOCODER_CHECK_CONDITION(longitude>=-180 && longitude<=180,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER");


	unsigned int reqid;
	int ret;
	geocoder_s *handle = (geocoder_s*)geocoder;

	LocationPosition *pos = NULL;
	pos = location_position_new (0, latitude, longitude, 0, LOCATION_STATUS_2D_FIX);

	__reverse_geocode_callback_data * calldata = (__reverse_geocode_callback_data *)malloc(sizeof(__reverse_geocode_callback_data));
	if( calldata == NULL)
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_OUT_OF_MEMORY);

	calldata->callback = callback;
	calldata->data = user_data;

	ret = location_map_get_address_from_position_async(handle->object,pos ,__cb_address_from_position, calldata, &reqid);

	location_position_free(pos);
	if( ret != 0 ){
		free(calldata);
		return __convert_error_code(ret);
	}

	if( request_id )
		*request_id = reqid;
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_cancel(geocoder_h geocoder, int request_id)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);

	geocoder_s *handle = (geocoder_s*)geocoder;
	return __convert_error_code(location_map_cancel_geocode_request(handle->object, request_id));

}

EXPORT_API int geocoder_preference_create (geocoder_preference_h *preference)
{
	GEOCODER_NULL_ARG_CHECK(preference);
	LocationGeocodePreference *pref;
	pref = location_geocode_pref_new();
	if( pref == NULL)
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_OUT_OF_MEMORY);
	*preference = (geocoder_preference_h)pref;

	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_preference_destroy (geocoder_preference_h preference)
{
	GEOCODER_NULL_ARG_CHECK(preference);
	location_geocode_pref_free((LocationGeocodePreference*)preference);
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_preference_get(geocoder_preference_h preference, const char *key, char **value)
{
	GEOCODER_NULL_ARG_CHECK(preference);
	GEOCODER_NULL_ARG_CHECK(key);
	GEOCODER_NULL_ARG_CHECK(value);
	char *v = (char*)location_geocode_pref_get_property((LocationGeocodePreference*)preference, key);
	if( v == NULL )
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_INVALID_KEY);
	*value = strdup(v);
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_preference_set(geocoder_preference_h preference, const char *key, const char *value)
{
	GEOCODER_NULL_ARG_CHECK(preference);
	GEOCODER_NULL_ARG_CHECK(key);

	int ret = location_geocode_pref_set_property((LocationGeocodePreference*)preference,key, value);
	if( !ret )
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_INVALID_KEY);
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_preference_get_max_result(geocoder_preference_h preference, int *max_result)
{
	GEOCODER_NULL_ARG_CHECK(preference);
	GEOCODER_NULL_ARG_CHECK(max_result);
	int ret = location_geocode_pref_get_max_result((LocationGeocodePreference*)preference );
	if( ret == 0 )
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_INVALID_PARAMETER);
	*max_result = ret;
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_preference_set_max_result(geocoder_preference_h preference, int max_result)
{
	GEOCODER_NULL_ARG_CHECK(preference);
	GEOCODER_CHECK_CONDITION(max_result >= 0 , GEOCODER_ERROR_INVALID_PARAMETER, "max count value is negative");
	int ret = location_geocode_pref_set_max_result((LocationGeocodePreference*)preference, max_result);
	if( !ret )
		GEOCODER_PRINT_ERROR_CODE_RETURN(GEOCODER_ERROR_INVALID_PARAMETER);
	return GEOCODER_ERROR_NONE;
}

EXPORT_API int geocoder_preference_foreach_available_keys(geocoder_h geocoder, geocoder_preference_available_key_cb callback , void * user_data)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(callback);

	geocoder_s *handle = (geocoder_s*)geocoder;

	GList *keys=NULL;
	location_map_get_provider_capability_key(handle->object, MAP_SERVICE_GEOCODE_PREF_PROPERTY, &keys);
	if( keys == NULL )
		return GEOCODER_ERROR_NOT_FOUND;

	while(keys){
		char *key = keys->data;
		if( !callback(key,user_data) )
			break;
		keys = keys->next;
	}
	g_list_free_full (keys, g_free);
	return GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
}

EXPORT_API int geocoder_service_get_preference(geocoder_h geocoder, geocoder_preference_h *preference)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(preference);
	geocoder_s *handle = (geocoder_s*)geocoder;

	if( handle->preference == NULL ){
		geocoder_preference_create(&handle->preference );
	}
	*preference = (geocoder_preference_h)location_geocode_pref_copy((LocationGeocodePreference*)handle->preference);
	return GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
}

EXPORT_API int geocoder_service_set_preference(geocoder_h geocoder, geocoder_preference_h preference)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(preference);

	geocoder_s *handle = (geocoder_s*)geocoder;

	if( handle->preference ){
		geocoder_preference_destroy(handle->preference);
		handle->preference = (geocoder_preference_h)location_geocode_pref_copy((LocationGeocodePreference*)preference);
	}
	return GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
}
