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
#include <glib.h>
#include <geocoder.h>

geocoder_h geocoder;

static void  get_addr_cb(geocoder_error_e result, const char *building_number, const char *postal_code, const char *street, const  char *city, const char *district, const char *state, const char *country_code, void *user_data)
{
		printf("get_addr_cb() ===> building number: %s, postal code: %s, street: %s, city: %s, district:  %s, state: %s, country code: %s\n", building_number,postal_code,street,city,district,state,country_code);
}

static bool  get_pos_cb(geocoder_error_e result, double latitude, double longitude, void *user_data)
{
		printf ("get_pos_cb() ===> latitude  : %g, longitude : %g\n",latitude, longitude);
		return true;
}

int geocoder_test()
{
	int ret;
	if (geocoder ==NULL)
	{
		ret = geocoder_create(&geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			printf ("geocoder_create return error : %d=n", ret);
		}
	}

	ret = geocoder_get_address_from_position(geocoder,37.258,127.056,get_addr_cb, NULL);
	if(ret != GEOCODER_ERROR_NONE)
	{
		printf ("geocoder_get_address_from_position return error : %d\n", ret);
	}
	return 1;
}

int reverse_geocoder_test()
{
	int ret;
	if (geocoder ==NULL)
	{
		ret = geocoder_create(&geocoder);
		if(ret != GEOCODER_ERROR_NONE)
		{
			printf ("geocoder_create return error : %d=n", ret);
		}
	}
	char *address="suwon";

	ret = geocoder_foreach_positions_from_address (geocoder,address, get_pos_cb, (void*)geocoder);
	if(ret != GEOCODER_ERROR_NONE)
	{
		printf ("geocoder_foreach_positions_from_address_sync return error : %d\n", ret);
	}
	return 1;
}


static GMainLoop *loop = NULL;

static gboolean exit_program (gpointer data)
{
	g_main_loop_quit (loop);
	printf("Quit g_main_loop\n");
	return FALSE;
}


int main(int argc, char ** argv)
{
	loop =g_main_loop_new (NULL, TRUE);
	g_setenv("PKG_NAME", "org.tizen.capi-location-geocoder-test", 1);

	geocoder_test();
	reverse_geocoder_test();
	g_timeout_add_seconds(30, exit_program, NULL);
	g_main_loop_run (loop);
	
	return 0;
}

