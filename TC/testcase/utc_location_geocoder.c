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

#include <tet_api.h>
#include <location/geocoder.h>
#include <glib.h>


enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_location_geocoder_create_p(void);
static void utc_location_geocoder_create_n(void);
static void utc_location_geocoder_destroy_p(void);
static void utc_location_geocoder_destroy_n(void);
static void utc_location_geocoder_destroy_n_02(void);
static void utc_location_geocoder_get_address_from_position_p(void);
static void utc_location_geocoder_get_address_from_position_n(void);
static void utc_location_geocoder_get_address_from_position_n_02(void);
static void utc_location_geocoder_get_address_from_position_n_03(void);
static void utc_location_geocoder_get_address_from_position_n_04(void);
static void utc_location_geocoder_get_address_from_position_n_05(void);
static void utc_location_geocoder_get_address_from_position_n_06(void);
static void utc_location_geocoder_foreach_positions_from_address_p(void);
static void utc_location_geocoder_foreach_positions_from_address_n(void);
static void utc_location_geocoder_foreach_positions_from_address_n_02(void);
static void utc_location_geocoder_foreach_positions_from_address_n_03(void);
static void utc_location_geocoder_foreach_positions_from_address_n_04(void);



struct tet_testlist tet_testlist[] = {
	{ utc_location_geocoder_create_p, POSITIVE_TC_IDX },
	{ utc_location_geocoder_create_n, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_destroy_p, POSITIVE_TC_IDX },
	{ utc_location_geocoder_destroy_n, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_destroy_n_02, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_get_address_from_position_p, POSITIVE_TC_IDX },
	{ utc_location_geocoder_get_address_from_position_n, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_get_address_from_position_n_02, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_get_address_from_position_n_03, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_get_address_from_position_n_04, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_get_address_from_position_n_05, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_get_address_from_position_n_06, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_foreach_positions_from_address_p, POSITIVE_TC_IDX },
	{ utc_location_geocoder_foreach_positions_from_address_n, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_foreach_positions_from_address_n_02, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_foreach_positions_from_address_n_03, NEGATIVE_TC_IDX },
	{ utc_location_geocoder_foreach_positions_from_address_n_04, NEGATIVE_TC_IDX },
	{ NULL, 0 },
};

static GMainLoop *g_mainloop = NULL;
static GThread *event_thread;

gpointer GmainThread(gpointer data){
	g_mainloop = g_main_loop_new (NULL, 0);
	g_main_loop_run (g_mainloop);

	return NULL;
}

static void startup(void)
{
	g_setenv("PKG_NAME", "org.tizen.capi-location-geocoder-test", 1);
	g_setenv("LOCATION_TEST_ENABLE", "1", 1);

#if !GLIB_CHECK_VERSION (2, 31, 0)
	if( !g_thread_supported() )
	{
		g_thread_init(NULL);
	}
#endif

	GError *gerr = NULL;
	event_thread = g_thread_create(GmainThread, NULL, 1, &gerr);
}

static void cleanup(void)
{
	g_main_loop_quit (g_mainloop);
	g_thread_join(event_thread);
}

/**
 * @brief Positive test case of player_create()
 */
static void utc_location_geocoder_create_p(void)
{
	char* api_name = "geocoder_create";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		geocoder_destroy(geocoder);
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_location_geocoder_create_n(void)
{
	char* api_name = "geocoder_create";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(NULL)) != GEOCODER_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_location_geocoder_destroy_p(void)
{
	char* api_name = "geocoder_destroy";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		if ((ret =geocoder_destroy(geocoder)) == GEOCODER_ERROR_NONE)
		{
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_location_geocoder_destroy_n(void)
{
	char* api_name = "geocoder_destroy";
	int ret;
	if ((ret = geocoder_destroy(NULL)) != GEOCODER_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void utc_location_geocoder_destroy_n_02(void)
{
	char* api_name = "geocoder_destroy";
	int ret;
	geocoder_h geocoder;
	if ((ret = geocoder_destroy(geocoder)) != GEOCODER_ERROR_NONE)
	{
		dts_pass(api_name);
	}
	dts_message(api_name, "Call log: %d", ret);
	dts_fail(api_name);
}

static void get_address_cb(geocoder_error_e result, const char *building_number, const char *postal_code, const char *street, const  char *city, const char *district, const char *state, const char *country_code, void *user_data)
{
	char* api_name = "geocoder_get_address_from_position";
	dts_message(api_name,"building number: %s, postal code: %s, street: %s, city: %s, district:  %s, state: %s, country code: %s\n", building_number,postal_code,street,city,district,state,country_code);
}

static void utc_location_geocoder_get_address_from_position_p(void)
{
	char* api_name = "geocoder_get_address_from_position";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_get_address_from_position(geocoder,37.258,127.056,get_address_cb,(void*)geocoder);
		if(ret == GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
		dts_message(api_name, "Ret : %d", ret);
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_get_address_from_position_n(void)
{
	char* api_name = "geocoder_get_address_from_position";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_get_address_from_position(NULL,37.258,127.056,get_address_cb,(void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_get_address_from_position_n_02(void)
{
	char* api_name = "geocoder_get_address_from_position";
	int ret;
	geocoder_h geocoder;
	geocoder_h geocoder_02 = NULL;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_get_address_from_position(geocoder_02, 37.258, 127.056, get_address_cb, (void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_get_address_from_position_n_03(void)
{
	char* api_name = "geocoder_get_address_from_position";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_get_address_from_position(geocoder, -91, -181, get_address_cb, (void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_get_address_from_position_n_04(void)
{
	char* api_name = "geocoder_get_address_from_position";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_get_address_from_position(geocoder, 37.258, 127.056, NULL,(void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_get_address_from_position_n_05(void)
{
	char* api_name = "geocoder_get_address_from_position";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_get_address_from_position(geocoder, -91, 127.056, get_address_cb, (void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_get_address_from_position_n_06(void)
{
	char* api_name = "geocoder_get_address_from_position";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_get_address_from_position(geocoder, 37.258, -181, get_address_cb, (void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}


static bool get_position_cb(geocoder_error_e result, double latitude, double longitude, void *user_data)
{
	geocoder_h geocoder = (geocoder_h) user_data;

	char* api_name = "geocoder_foreach_positions_from_address";
	dts_message(api_name,"latitude:%ld, state:%s, longitude:%ld\n",latitude, longitude);
	geocoder_destroy(geocoder);
//	dts_pass(api_name);
	return FALSE;
}

static gboolean _destroy_if_timeout (gpointer user_data)
{
	char* api_name = "_destroy_if_timeout";
	geocoder_h _geocoder = (geocoder_h) user_data;

	dts_message(api_name, "Timeout");
	geocoder_destroy(_geocoder);

	return FALSE;
}

// TODO : Fail
static void utc_location_geocoder_foreach_positions_from_address_p(void)
{
	char* api_name = "geocoder_foreach_positions_from_address";
	int ret;
	geocoder_h geocoder;
	if ((ret = geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		char *address="suwon";
		ret = geocoder_foreach_positions_from_address(geocoder,address, get_position_cb,(void*)geocoder);
		if(ret == GEOCODER_ERROR_NONE)
		{
			g_timeout_add_seconds(60, _destroy_if_timeout, geocoder);
			dts_pass(api_name);

		}
	}

	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_foreach_positions_from_address_n(void)
{
	char* api_name = "geocoder_foreach_positions_from_address";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		char *address="suwon";
		ret = geocoder_foreach_positions_from_address(NULL, address, get_position_cb,(void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_foreach_positions_from_address_n_02(void)
{
	char* api_name = "geocoder_foreach_positions_from_address";
	int ret;
	geocoder_h geocoder;
	geocoder_h geocoder_02 = NULL;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		char *address="suwon";
		ret = geocoder_foreach_positions_from_address(geocoder_02, address, get_position_cb, (void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_foreach_positions_from_address_n_03(void)
{
	char* api_name = "geocoder_foreach_positions_from_address";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		ret = geocoder_foreach_positions_from_address(geocoder, NULL, get_position_cb, (void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}

static void utc_location_geocoder_foreach_positions_from_address_n_04(void)
{
	char* api_name = "geocoder_foreach_positions_from_address";
	int ret;
	geocoder_h geocoder;
	if ((ret =geocoder_create(&geocoder)) == GEOCODER_ERROR_NONE)
	{
		char *address="suwon";
		ret = geocoder_foreach_positions_from_address(geocoder, address, NULL, (void*)geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			geocoder_destroy(geocoder);
			dts_pass(api_name);
		}
	}
	dts_message(api_name, "Call log: %d", ret);
	geocoder_destroy(geocoder);
	dts_fail(api_name);
}





