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

int _convert_error_code(int code, char* func_name)
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
			ret = GEOCODER_ERROR_NETWORK_FAILED;
			msg = "GEOCODER_ERROR_NETWORK_FAILED";
			break;
		case LOCATION_ERROR_PARAMETER:
			ret = GEOCODER_ERROR_INVALID_PARAMETER;
			msg = "GEOCODER_ERROR_INVALID_PARAMETER";
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

static void cb_address_from_position (LocationError error, LocationAddress *addr, LocationAccuracy *acc, gpointer userdata)
{
	geocoder_s * handle = (geocoder_s*)userdata;
	if(handle->user_cb[_GEOCODER_CB_ADDRESS_FROM_POSITION])
	{
		if(error != LOCATION_ERROR_NONE || addr == NULL)
		{
			_convert_error_code(error,(char*)__FUNCTION__);
			((geocoder_get_address_cb)handle->user_cb[_GEOCODER_CB_ADDRESS_FROM_POSITION])(NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, handle->user_data[_GEOCODER_CB_ADDRESS_FROM_POSITION]);			
		}
		else
		{
			 LOGI("[%s] Address - building number: %s, postal code: %s, street: %s, city: %s, district:  %s, state: %s, country code: %s", __FUNCTION__ , addr->building_number, addr->postal_code, addr->street, addr->city, addr->district, addr->state, addr->country_code);
			((geocoder_get_address_cb)handle->user_cb[_GEOCODER_CB_ADDRESS_FROM_POSITION])(addr->building_number, addr->postal_code, addr->street, addr->city, addr->district, addr->state, addr->country_code, handle->user_data[_GEOCODER_CB_ADDRESS_FROM_POSITION]);
		}
	}
	else
	{
		LOGI("[%s] callback is NULL )",__FUNCTION__);
	}
}

static void cb_position_from_address (LocationError error, LocationPosition *pos, LocationAccuracy *acc, gpointer userdata)
{
	geocoder_s * handle = (geocoder_s*)userdata;
	if(handle->user_cb[_GEOCODER_CB_POSITION_FROM_ADDRESS])
	{
		if(error != LOCATION_ERROR_NONE || pos == NULL)
		{
			_convert_error_code(error,(char*)__FUNCTION__);
		}
		else
		{
			int i;
			int temp_max = 1;
			for( i=0 ; i < temp_max ; i++)
			{
				 LOGI("[%s] Position[%d] - time: %d, lat: %f, long: %f, alt: %f, status: %d", __FUNCTION__, (i+1), pos->timestamp, pos->latitude, pos->longitude, pos->altitude, pos->status);
				if ( ((geocoder_get_position_cb)handle->user_cb[_GEOCODER_CB_POSITION_FROM_ADDRESS])(pos->latitude, pos->longitude, handle->user_data[_GEOCODER_CB_POSITION_FROM_ADDRESS]) != TRUE )
				{
					 LOGI("[%s] User quit the loop [count : %d]",  __FUNCTION__, (i+1));
					break;
				}
			}
		}
	}
	else
	{
		LOGI("[%s] callback is NULL )",__FUNCTION__);
	}
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
		return _convert_error_code(ret,(char*)__FUNCTION__);
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

	handle->user_cb[_GEOCODER_CB_ADDRESS_FROM_POSITION] = callback;
	handle->user_data[_GEOCODER_CB_ADDRESS_FROM_POSITION] = user_data;
	
	ret = location_get_address_from_position_async(handle->object, pos, cb_address_from_position, handle);
	if( ret != LOCATION_ERROR_NONE)
	{
		return _convert_error_code(ret,(char*)__FUNCTION__);
	}
	location_position_free(pos);
	return GEOCODER_ERROR_NONE;
}

int geocoder_get_address_from_position_sync(geocoder_h geocoder, double latitude, double longitude, char **building_number, char **postal_code, char **street, char **city, char **district, char **state, char **country_code)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_CHECK_CONDITION(latitude>=-90 && latitude<=90 ,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER");
	GEOCODER_CHECK_CONDITION(longitude>=-180 && longitude<=180,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER");
	
	geocoder_s *handle = (geocoder_s*)geocoder;
	int ret;
	LocationAddress *addr = NULL;
	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;

	pos = location_position_new (0, latitude, longitude, 0, LOCATION_STATUS_2D_FIX);
	ret = location_get_address_from_position(handle->object, pos, &addr, &acc);
	if( ret != LOCATION_ERROR_NONE)
	{
		return _convert_error_code(ret,(char*)__FUNCTION__);
	}

	if(building_number)
	{
		*building_number = NULL;
		if(addr->building_number)
			*building_number = strdup(addr->building_number);
	}

	if(postal_code)
	{
		*postal_code = NULL;
		if(addr->postal_code)
			*postal_code = strdup(addr->postal_code);
	}

	if(street)
	{
		*street = NULL;
		if(addr->street)
			*street = strdup(addr->street);
	}

	if(city)
	{
		*city = NULL;
		if(addr->city)
			*city = strdup(addr->city);
	}

	if(state)
	{
		*state = NULL;
		if(addr->state)
			*state = strdup(addr->state);
	}

	if(district)
	{
		*district = NULL;
		if(addr->district)
			*district = strdup(addr->district);
	}

	if(country_code)
	{
		*country_code = NULL;
		if(addr->country_code)
			*country_code = strdup(addr->country_code);
	}
	
	location_address_free(addr);
	location_position_free(pos);
	location_accuracy_free(acc);
	return GEOCODER_ERROR_NONE;
}



int	 geocoder_foreach_positions_from_address(geocoder_h geocoder,const char* address, geocoder_get_position_cb callback, void *user_data)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(address);
	GEOCODER_NULL_ARG_CHECK(callback);
	geocoder_s *handle = (geocoder_s*)geocoder;

	char* addr_str = g_strdup(address);
	handle->user_cb[_GEOCODER_CB_POSITION_FROM_ADDRESS] = callback;
	handle->user_data[_GEOCODER_CB_POSITION_FROM_ADDRESS] = user_data;

	int ret;	
	ret = location_get_position_from_freeformed_address_async(handle->object, addr_str,cb_position_from_address, handle);
	if( ret != LOCATION_ERROR_NONE)
	{
		g_free(addr_str);       
		return _convert_error_code(ret,(char*)__FUNCTION__);
	}
	return GEOCODER_ERROR_NONE;
}

int	 geocoder_foreach_positions_from_address_sync(geocoder_h geocoder,const char* address, geocoder_get_position_cb callback, void *user_data)
{
	GEOCODER_NULL_ARG_CHECK(geocoder);
	GEOCODER_NULL_ARG_CHECK(address);
	GEOCODER_NULL_ARG_CHECK(callback);
	geocoder_s *handle = (geocoder_s*)geocoder;

	int ret;
	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	char* addr_str = g_strdup(address);
			 
	ret = location_get_position_from_freeformed_address(handle->object, addr_str, &pos, &acc);
	if( ret != LOCATION_ERROR_NONE)
	{
		g_free(addr_str);       
		return _convert_error_code(ret,(char*)__FUNCTION__);
	}
		 
	int i;
	int temp_max = 1;
	for( i=0 ; i < temp_max ; i++)
	{
		if ( callback(pos->latitude, pos->longitude, user_data) != TRUE )
		{
			 LOGI("[%s] User quit the loop [count : %d]",  __FUNCTION__, (i+1));
			break;
		}
	}
	g_free(addr_str);
	location_position_free(pos);
	location_accuracy_free(acc);
	return GEOCODER_ERROR_NONE;
}
