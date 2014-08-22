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


/*
	Usefull link (perfdata specs) :
		http://nagiosplug.sourceforge.net/developer-guidelines.html
		http://docs.icinga.org/latest/en/perfdata.html
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>


#include <libperfdataparser.h>


int parseThresholds (char *thresholdString, char **errmsg, double *thmin, double *thmax, char *range) {
	
	double doubleValue = NAN;
	int nbCharUsed = 0;
	int c = 0;
	
	
	TRACE ("  parseThresholds('%s')",thresholdString);
	
	if ( thresholdString[0] == '@' ) {
		TRACE ("  parseThresholds() Found a range marker (@) at %d",0);
		*range = RANGE_IN;
		++nbCharUsed;
	}
	
	c = parseDouble (thresholdString + nbCharUsed, errmsg, &doubleValue);
	
	if ( c > 0 ) {
		nbCharUsed += c;
		*thmin = doubleValue;
	}
	
	if ( thresholdString[nbCharUsed] == ':' ) {
		TRACE ("  parseThresholds() Found a range separator (:) at %d",nbCharUsed+1);
		
		if ( *range == RANGE_NONE ) {
			*range = RANGE_OUT;
		}
		
		++nbCharUsed;
		
		c = parseDouble (thresholdString + nbCharUsed, errmsg, &doubleValue);
		
		if ( c > 0 ) {
			*thmax = doubleValue;
			nbCharUsed += c;
		}
	} else {
		*thmax = doubleValue;
	}
	TRACE ("  parseThresholds() nbCharUsed = %d",nbCharUsed);
	return nbCharUsed;
}

int parseDouble (char *stringValue, char **errmsg, double *doubleValue) {

	char *tmpBuf = NULL;
	
	double val = NAN;
	char *endptr = NULL;
	int offset = 0;
	int i = 0;
	
	
	TRACE ("  parseDouble('%s')",stringValue);
	
	offset = strspn(stringValue, "-0123456789.,E");
	TRACE ("  offset = %d", offset);
	
	if ( offset == 0 ) {
		TRACE ("INFO: %s","No value found");
		return 0;
	}
	tmpBuf = malloc(offset);
	
	for ( i = 0; i < offset; ++i ) {
		if ( stringValue[i] == ',' ) {
			tmpBuf[i] = '.';
		} else {
			tmpBuf[i] = stringValue[i];
		}
	}
	tmpBuf[offset]='\0';
	
	TRACE ("Found a value string : '%s'", tmpBuf);
	
	// Parse value
		
	errno = 0;
	val = strtod(tmpBuf, &endptr);
	
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 &&  val == 0)) {
		*errmsg = strdup("Error found while parsing value");
		free(tmpBuf);
		return 0;
	}
	if ( endptr == NULL ) {
		TRACE ("WARN: %s","endptr == NULL !");
		free(tmpBuf);
		return 0;
	}
	
	int nbCharUsed = endptr - tmpBuf;
	
	if ( nbCharUsed == 0 ) {
		*errmsg = strdup("Nothing usable found !");
		TRACE ("ERROR: %s",errmsg);
		free(tmpBuf);
		return 0;
	}
	if ( nbCharUsed < offset ) {
		*errmsg = strdup("Not all characters used by strtod conversion !");
		TRACE ("WARN: Not all characters used by strtod conversion : '%s'",tmpBuf+nbCharUsed);
	}
	*doubleValue = val;
	free(tmpBuf);
	TRACE ("  parseDouble() nbCharUsed = %d",nbCharUsed);
	return nbCharUsed;
	
	
}



int parsePerfdata( char *perfString, char **errmsg, typePerfdata **p_perfdata ) {
	
	
	int i = 0;
	int offset = 0;
	
	char *name = NULL;
	int name1 = -1;
	int name2 = -1;
	
	int equ = -1;
	int end = 0;
	
	double val = 0;
	int val1 = -1;
	int val2 = -1;
	
	int unit1 = -1;
	int unit2 = -1;
	
	char *tmp = NULL;
	
	typePerfdata *perfdata = NULL;
	
	int perfLen = strlen(perfString);
	
#if DEBUG
	printf ("--------------------------------------------------------------------------------------\n");

	printf ("     |00000000001111111111222222222233333333334444444444555555555566666666667777777777\n");
	printf ("     |01234567890123456789012345678901234567890123456789012345678901234567890123456789\n");
	printf ("data |%s| (len=%d)\n",perfString,perfLen);

	printf ("--------------------------------------------------------------------------------------\n");
#endif
	
	
	// Search for '='
	for ( i = 0; i < perfLen; i++ ) {
		if ( perfString[i] == '=' ) {
			equ = i;
			break;
		}
		if ( perfString[i] == ';' ) {
			*errmsg = strdup("ERROR: ';' found before '=' !");
			return perfLen + i;
		}
	}
	TRACE ("  equ = %d",equ);
	if ( equ < 0 ) {
		*errmsg = strdup("ERROR: Equal not found !");
		return perfLen;
	}
	end = equ;
	
	// Drop leading spaces or quote
	for ( i = 0; i < equ; i++ ) {
		if ( perfString[i] == ' ' || perfString[i] == '\'') continue;
		name1 = i;
		break;
	}
	TRACE ("  name1 = %d",name1);
	if ( name1 < 0 ) {
		*errmsg = strdup("ERROR: Empty ?");
		TRACE ("WARN: %s","perfdata name not found (2) %d");
		goto CLEANUP;
	}
	
	// Drop spaces or quote after perf name
	for ( i = equ - 1; i >= name1; i-- ) {
		if ( perfString[i] == ' ' || perfString[i] == '\'') continue;
		name2 = i;
		break;
	}
	TRACE ("  name2 = %d",name2);
	if ( name2 < 0 ) {
		TRACE ("WARN: %s","perfdata name not found (2) - Yes, this is a bug");
		*errmsg = strdup("Can't find var name !");
		return perfLen;
	}
	end = name1;

	name = strndup(perfString+name1,name2-name1+1);
	TRACE ("Found name between %d and %d : '%s'",name1, name2, name);

	// Drop spaces after '='
	for ( val1 = equ + 1; val1 < perfLen; val1++ ) {
		if ( perfString[val1] == ' ') continue;
		break;
	}
	TRACE ("  val1 = %d",val1);
	
	

	offset = strspn(perfString+val1, "-0123456789.,");
	if ( offset == 0 ) {
		TRACE ("ERROR: %s","No value after =");
		return perfLen;
	}
	
	val2 = val1 + offset -1;
	
	
	TRACE ("  val2 = %d",val2);

	tmp = strndup(perfString+val1,offset);
	TRACE ("Found 'value' string between %d and %d : '%s'",val1, val2, tmp);

	
	parseDouble (tmp, errmsg, &val);
	free(tmp);
	
	if ( *errmsg != NULL ) {
		TRACE ("WARN: %s","parseDouble failed for 'value'");
		return val2;
	}
	
	//We have now the minimum information for a perfdata
	perfdata = malloc(sizeof(typePerfdata));
	
	perfdata->data = NULL;

	perfdata->name = name;
	perfdata->value = val;
	
	perfdata->unit = NULL;
	perfdata->warn_min = -INFINITY;
	perfdata->warn_max = INFINITY;
	perfdata->warn_range = RANGE_NONE;
	perfdata->crit_min = -INFINITY;
	perfdata->crit_max = INFINITY;
	perfdata->crit_range = RANGE_NONE;
	perfdata->min = NAN;
	perfdata->max = NAN;
	
	*p_perfdata = perfdata;
	
	unit1 = val2 + 1;
	end = unit1;
	
	// Search a space  or a ;
	for ( unit2=unit1; unit2 <= perfLen; unit2++ ) {
		if ( perfString[unit2] == ';' || perfString[unit2] == ' ' || perfString[unit2] == '\0' ) {
			break;
		}
	}
	end = unit2;
	
	if ( unit2 > unit1 ) {
		perfdata->unit = strndup(perfString+unit1,unit2-unit1);
		TRACE ("Found 'unit' string between %d and %d : '%s'",unit1, unit2-1, perfdata->unit);
	}
	
	if ( perfString[end] != ';' ) {
		// this is the end
		goto CLEANUP;
	}
	
	
	// Search warning threshold
	val1 = unit2 + 1;
	val2 = val1 + parseThresholds (perfString+val1, errmsg, &perfdata->warn_min, &perfdata->warn_max, &perfdata->warn_range);
	end = val2;
	
	if ( perfString[end] == ' ' || perfString[end] == '\0' ) {
		goto CLEANUP;
	}
	if ( perfString[end] != ';' ) {
		TRACE (" Unexpected character found after warning value : %c",perfString[end]);
		goto CLEANUP;
	}
	
	
	// Search critical threshold
	val1 = val2 + 1;
	val2 = val1 + parseThresholds (perfString+val1, errmsg, &perfdata->crit_min, &perfdata->crit_max, &perfdata->crit_range);
	end = val2;
	
	if ( perfString[end] == ' ' || perfString[end] == '\0' ) {
		goto CLEANUP;
	}
	if ( perfString[end] != ';' ) {
		TRACE (" Unexpected character found after critical value : %c",perfString[end]);
		goto CLEANUP;
	}
	
	// Search min value
	val1 = val2 + 1;
	val2 = val1 + parseDouble (perfString+val1, errmsg, &perfdata->min);
	end = val2;
	
	if ( perfString[end] == ' ' || perfString[end] == '\0' ) {
		goto CLEANUP;
	}
	if ( perfString[end] != ';' ) {
		TRACE (" Unexpected character found after min value : %c",perfString[end]);
		goto CLEANUP;
	}
	
	// Search max value
	val1 = val2 + 1;
	val2 = val1 + parseDouble (perfString+val1, errmsg, &perfdata->max);
	end = val2;
	
	
	CLEANUP:
	
	TRACE (" Cleanup (1) : end = %d",end);
	// Search the end of current perfdata (next space)
	short found = 0;
	for ( ; end < perfLen; ++end ) {
		if ( perfString[end] == ' ') {
			found = 1;
		} else if ( found ) {
			break;
		}
	}
	
	if ( perfdata != NULL ) {
		perfdata->data = strndup(perfString,end);
	}
	TRACE (" Cleanup (2) : end = %d",end);
	
	return end;
	
}


