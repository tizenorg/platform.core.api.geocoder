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

#ifndef __TIZEN_LOCATION_GEOCODER_PRIVATE_H__
#define	__TIZEN_LOCATION_GEOCODER_PRIVATE_H__

#include <location/location.h>
#include <location/location-map-service.h>
#include <geocoder.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_LOCATION_GEOCODER"

/*
* Internal Macros
*/
#define GEOCODER_LOGD(fmt,args...)  LOGD(fmt, ##args)
#define GEOCODER_LOGW(fmt,args...)  LOGW(fmt, ##args)
#define GEOCODER_LOGI(fmt,args...)  LOGI(fmt, ##args)
#define GEOCODER_LOGE(fmt,args...)  LOGE(fmt, ##args)

#define GEOCODER_CHECK_CONDITION(condition, error, msg)	\
	do { \
		if (condition) { \
		} else { \
			GEOCODER_LOGE("%s(0x%08x)", msg, error); \
			return error; \
		} \
	} while (0)

#define GEOCODER_NULL_ARG_CHECK(arg)	\
	GEOCODER_CHECK_CONDITION(arg != NULL,GEOCODER_ERROR_INVALID_PARAMETER,"GEOCODER_ERROR_INVALID_PARAMETER")

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	_GEOCODER_CB_ADDRESS_FROM_POSITION,
	_GEOCODER_CB_POSITION_FROM_ADDRESS,
	_GEOCODER_CB_TYPE_NUM
}_geocoder_cb_e;

typedef struct _geocoder_s{
	LocationMapObject* object;
} geocoder_s;

#ifdef __cplusplus
}
#endif

#endif //__TIZEN_LOCATION_GEOCODER_PRIVATE_H__
