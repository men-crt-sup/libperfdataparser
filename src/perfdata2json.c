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
	
		typePerfdata *perfStuct = NULL;
	
		if ( len > 0 && buffer[len-1] == '\n') {
			buffer[len-1] = '\0';
			len--;
		}
	
		while ( current < len ) {
			TRACE ("  current = %d",(int)current);
			
			parsedCount = parsePerfdata(buffer + current, &errmsg, &perfStuct);
			parsedString = strndup(buffer + current,parsedCount);
			current += parsedCount;
			
			if ( errmsg != NULL ) {
				
				fprintf(stderr,"ERROR: parse error : %s\n", errmsg);
				fprintf(stderr,"  Parsed string : %s\n", parsedString);
				free(errmsg);
				errmsg = NULL;
				continue;
			}
			free(parsedString);
			if ( perfStuct != NULL ) {
				
				// Do something usefull
				
				TRACE ("Got something usable in '%s' : '%s' = %f %s", perfStuct->data, perfStuct->name, perfStuct->value, perfStuct->unit);
				TRACE ("  min = %f, max = %f", perfStuct->min, perfStuct->max );
				TRACE ("  warn : min = %f , max = %f (%d)", perfStuct->warn_min, perfStuct->warn_max, perfStuct->warn_ins );
				TRACE ("  crit : min = %f , max = %f (%d)", perfStuct->crit_min, perfStuct->crit_max, perfStuct->crit_ins );
				
				json = json_object_new_object();
				json_object_object_add(json, "name", json_object_new_string(perfStuct->name));
				json_object_object_add(json, "value", json_object_new_double(perfStuct->value));
				
				if ( perfStuct->unit != NULL) {
					json_object_object_add(json, "unit", json_object_new_string(perfStuct->unit));
				}
				
				json_object_object_add(json, "w_min", json_object_new_double(perfStuct->warn_min));
				json_object_object_add(json, "w_max", json_object_new_double(perfStuct->warn_max));
				json_object_object_add(json, "w_range", json_object_new_int(perfStuct->warn_range));
				
				json_object_object_add(json, "c_min", json_object_new_double(perfStuct->crit_min));
				json_object_object_add(json, "c_max", json_object_new_double(perfStuct->crit_max));
				json_object_object_add(json, "c_range", json_object_new_int(perfStuct->crit_range));
				
				if ( ! isnan(perfStuct->min) ) {
					json_object_object_add(json, "min", json_object_new_double(perfStuct->min));
				}
				if ( ! isnan(perfStuct->max) ) {
					json_object_object_add(json, "max", json_object_new_double(perfStuct->max));
				}
				
				// TODO: Make this one optional :
				if ( perfStuct->data != NULL) {
					json_object_object_add(json, "data", json_object_new_string(perfStuct->data));
				}
				
				printf("%s\n", json_object_get_string(json));
				json_object_put(json);
				
				
				// Cleanup memory
			
				if ( errmsg != NULL) {
					free(errmsg);
				}
			
				free(perfStuct->name);
				if ( perfStuct->unit != NULL) {
					free(perfStuct->unit);
				}
				if ( perfStuct->data != NULL) {
					free(perfStuct->data);
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
