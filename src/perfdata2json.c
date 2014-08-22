/* Copyright (C) 2013-2014 Stéphane Urbanovski <s.urbanovski@ac-nancy-metz.fr>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation;
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include <json-c/json.h>

#include <libperfdataparser.h>

#define BUF_SIZE 4096


int main(int argc, char **argv) {


	char buffer[BUF_SIZE];
	char *errmsg = NULL;
	
	int parsedCount = 0;
	char *parsedString = NULL;
	
	json_object *json;
	
	while ( fgets(buffer, BUF_SIZE, stdin) ) {
	
		size_t current = 0;
	
		size_t len = strlen(buffer);
	
		typePerfdata *perfdata1 = NULL;
	
		//FIXME: Iterate ?
		if ( len > 0 && buffer[len-1] == '\n') {
			buffer[len-1] = '\0';
			len--;
		} else {
			fprintf(stderr, "Warn: No new line\n");
		}
	
		while ( current < len ) {
			TRACE ("  current = %d",(int)current);
			
			parsedCount = parsePerfdata(buffer + current, &errmsg, &perfdata1);
			parsedString = strndup(buffer + current,parsedCount);
			current += parsedCount;
			
			if ( errmsg != NULL ) {
				
				parsedString = strdup(buffer + current);
				fprintf(stderr,"ERROR: parse error : %s\n", errmsg);
				fprintf(stderr,"  Parsed string : %s\n", parsedString);
				free(errmsg);
				errmsg = NULL;
				continue;
			}
			free(parsedString);
			if ( perfdata1 != NULL ) {
				
				// Do something usefull
				
				TRACE ("Got something usable in '%s' : '%s' = %f %s", perfdata1->data, perfdata1->name, perfdata1->value, perfdata1->unit);
				TRACE ("  min = %f, max = %f", perfdata1->min, perfdata1->max );
				TRACE ("  warn : min = %f , max = %f (%d)", perfdata1->warn_min, perfdata1->warn_max, perfdata1->warn_ins );
				TRACE ("  crit : min = %f , max = %f (%d)", perfdata1->crit_min, perfdata1->crit_max, perfdata1->crit_ins );
				
				json = json_object_new_object();
				json_object_object_add(json, "name", json_object_new_string(perfdata1->name));
				json_object_object_add(json, "value", json_object_new_double(perfdata1->value));
				
				if ( perfdata1->unit != NULL) {
					json_object_object_add(json, "unit", json_object_new_string(perfdata1->unit));
				}
				
				json_object_object_add(json, "warn_min", json_object_new_double(perfdata1->warn_min));
				json_object_object_add(json, "warn_max", json_object_new_double(perfdata1->warn_max));
				json_object_object_add(json, "warn_range", json_object_new_int(perfdata1->warn_range));
				
				json_object_object_add(json, "crit_min", json_object_new_double(perfdata1->crit_min));
				json_object_object_add(json, "crit_max", json_object_new_double(perfdata1->crit_max));
				json_object_object_add(json, "crit_range", json_object_new_int(perfdata1->crit_range));
				
				if ( ! isnan(perfdata1->min) ) {
					json_object_object_add(json, "min", json_object_new_double(perfdata1->min));
				}
				if ( ! isnan(perfdata1->max) ) {
					json_object_object_add(json, "max", json_object_new_double(perfdata1->max));
				}
				
				if ( perfdata1->data != NULL) {
					json_object_object_add(json, "data", json_object_new_string(perfdata1->data));
				}
				
				printf("%s\n", json_object_get_string(json));
				json_object_put(json);
				
				
				// Cleanup memory
			
				if ( errmsg != NULL) {
					free(errmsg);
				}
			
				free(perfdata1->name);
				if ( perfdata1->unit != NULL) {
					free(perfdata1->unit);
				}
				if ( perfdata1->data != NULL) {
					free(perfdata1->data);
				}
			}
//             sleep(1);
		}
	}

	if (ferror(stdin)) {
		perror("Error reading from stdin.");
		exit(3);
	}
	
	TRACE("%s","End test");

	return 0;
}
