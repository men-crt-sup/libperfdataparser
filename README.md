libperfdataparser
=================

C library for parsing Nagios(R) compatible perfdata

Prerequisites:
A recent `json-c` (only needed for command line tool)

Provided `perfdata2json` command line tool read perfdata string from 'stdin' and output parsed json result

Example

```bash
echo '/var=42GB;50;60;0;90' | ./perfdata2json 
{ "name": "\/var", "value": 42, "unit": "GB", "w_min": 50, "w_max": 50, "w_range": 0, "c_min": 60, "c_max": 60, "c_range": 0, "min": 0, "max": 90, "data": "\/var=42GB;50;60;0;90" }
```