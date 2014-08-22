#define RANGE_NONE 0
#define RANGE_IN   1
#define RANGE_OUT  2

typedef struct {
	char   *data;
	char   *name;
	double value;
	char   *unit;
	double warn_min;
	double warn_max;
	char   warn_range;
	double crit_min;
	double crit_max;
	char   crit_range;
	double min;
	double max;
} typePerfdata;

int parsePerfdata( char *perfString, char **errmsg, typePerfdata **p_perfdata );

int parseDouble (char *stringValue, char **errmsg, double *doubleValue);


#ifdef DEBUG
	#define TRACE(fmt, args...) fprintf(stderr, "[%s:%u] " fmt "\n",__FILE__,__LINE__, args)
#else
	#define TRACE(fmt, args...)
#endif