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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <geocoder_private.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_GEOCODER"

/*
* Internal Macros
*/
#define GEOCODER_CHECK_CONDITION(condition,error,msg)	\
		if(condition) {} else \
		{ LOGE("[%s] %s(0x%08x)",__FUNCTION__, msg,error); return error;}; \

#define GEOCODER_NULL_ARG_CHECK(arg)	\
	GEOCODER_CHECK_CONDITION(arg != NULL,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER")

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

static int __convert_error_code(int code, char* func_name)
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
	LOGE("[%s] %s(0x%08x) : core fw error(0x%x)",func_name,msg, ret, code);
	return ret;	
}

static void __cb_address_from_position (LocationError error, LocationAddress *addr, LocationAccuracy *acc, gpointer userdata)
{
	__addr_callback_data * callback = (__addr_callback_data*)userdata;
	if( callback == NULL || callback->callback == NULL)
	{
		LOGI("[%s] callback is NULL )",__FUNCTION__);
		return ;
	}

	if(error != LOCATION_ERROR_NONE || addr == NULL)
	{
		callback->callback(__convert_error_code(error,(char*)__FUNCTION__), NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, callback->data);
	}
	else
	{
		LOGI("[%s] Address - building number: %s, postal code: %s, street: %s, city: %s, district:  %s, state: %s, country code: %s", __FUNCTION__ , addr->building_number, addr->postal_code, addr->street, addr->city, addr->district, addr->state, addr->country_code);
		callback->callback(GEOCODER_ERROR_NONE, addr->building_number, addr->postal_code, addr->street, addr->city, addr->district, addr->state, addr->country_code, callback->data);
	}
	free(callback);
}

static void __cb_position_from_address (LocationError error, GList *position_list, GList *accuracy_list, gpointer userdata)
{
	__pos_callback_data * callback = (__pos_callback_data*)userdata;
	if( callback == NULL || callback->callback == NULL)
	{
		LOGI("[%s] callback is NULL )",__FUNCTION__);
		return ;
	}

	if(error != LOCATION_ERROR_NONE || position_list == NULL || position_list->data ==NULL || accuracy_list==NULL )
	{
		callback->callback(__convert_error_code(error,(char*)__FUNCTION__), 0, 0, callback->data);
	}
	else
	{
		while(position_list)
		{
			LocationPosition *pos = position_list->data;
			if ( callback->callback(GEOCODER_ERROR_NONE, pos->latitude, pos->longitude, callback->data) != TRUE )
			{
				LOGI("[%s] User quit the loop ",  __FUNCTION__);
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
int	geocoder_create(geocoder_h* geocoder)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	if(location_init()!=LOCATION_ERROR_NONE)
	{
		LOGE("[%s] GEOCODER_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : fail to location_init", __FUNCTION__, GEOCODER_ERROR_SERVICE_NOT_AVAILABLE);
		return GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
	}

	geocoder_s *handle = (geocoder_s*)malloc(sizeof(geocoder_s));
	if(handle==NULL)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x)", __FUNCTION__, GEOCODER_ERROR_OUT_OF_MEMORY);
		return GEOCODER_ERROR_OUT_OF_MEMORY;
	}

	memset(handle, 0 , sizeof(geocoder_s));
	
	handle->object = location_new(LOCATION_METHOD_HYBRID);
	if(handle->object  == NULL)
	{
		free(handle);
		LOGE("[%s] GEOCODER_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : fail to location_new", __FUNCTION__, GEOCODER_ERROR_SERVICE_NOT_AVAILABLE);
		return GEOCODER_ERROR_SERVICE_NOT_AVAILABLE;
	}

	*geocoder = (geocoder_h)handle;
	return GEOCODER_ERROR_NONE;
}

int	geocoder_destroy(geocoder_h geocoder)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	geocoder_s *handle = (geocoder_s*)geocoder;

	int ret = location_free(handle->object);
	if(ret!=GEOCODER_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	free(handle);
	return GEOCODER_ERROR_NONE;
}

int	geocoder_get_address_from_position(geocoder_h geocoder, double latitude, double longitude, geocoder_get_address_cb callback, void *user_data)
{
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
		LOGE("[%s] GEOCODER_ERROR_OUT_OF_MEMORY(0x%08x) : fail to create callback data", __FUNCTION__, GEOCODER_ERROR_OUT_OF_MEMORY);
		return GEOCODER_ERROR_OUT_OF_MEMORY;
	}
	calldata->callback = callback;
	calldata->data = user_data;
	
	ret = location_get_address_from_position_async(handle->object, pos, __cb_address_from_position, calldata);
	location_position_free(pos);
	if( ret != LOCATION_ERROR_NONE)
	{
		free(calldata);
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	return GEOCODER_ERROR_NONE;
}

int	 geocoder_foreach_positions_from_address(geocoder_h geocoder,const char* address, geocoder_get_position_cb callback, void *user_data)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(address);
	GEOCODER_NULL_ARG_CHECK(callback);
	geocoder_s *handle = (geocoder_s*)geocoder;

	char* addr_str = g_strdup(address);

	__pos_callback_data * calldata = (__pos_callback_data *)malloc(sizeof(__pos_callback_data));
	if( calldata == NULL)
	{
		LOGE("[%s] GEOCODER_ERROR_OUT_OF_MEMORY(0x%08x) : fail to create callback data", __FUNCTION__, GEOCODER_ERROR_OUT_OF_MEMORY);
		return GEOCODER_ERROR_OUT_OF_MEMORY;
	}
	calldata->callback = callback;
	calldata->data = user_data;

	int ret;	
	ret = location_get_position_from_freeformed_address_async(handle->object, addr_str,__cb_position_from_address, calldata);
	g_free(addr_str);
	if( ret != LOCATION_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	return GEOCODER_ERROR_NONE;
}
