/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * DESCRIPTION: English strings for the I/O for numbers and strings
 * 
 * Copyright 2013, Silergy Corp. All rights reserved.
 *
 * AUTHOR:  RP,MTF,RGV
 * $Id: cli_buf_str_en.c 16491 2018-01-31 00:42:04Z rvandewa $
 ****************************************************************************/

#include "options.h"
#include "cli_buf_str.h"   // Test the definitions.

/* strings for the "months" */
static const char jan[] = "JAN ";
static const char feb[] = "FEB ";
static const char mar[] = "MAR ";
static const char apr[] = "APR ";
static const char may[] = "MAY ";
static const char jun[] = "JUN ";
static const char jul[] = "JUL ";
static const char aug[] = "AUG ";
static const char sep[] = "SEP ";
static const char oct[] = "OCT ";
static const char nov[] = "NOV ";
static const char dec[] = "DEC ";

const char * const month_str_ary[12]
    = { jan, feb, mar, apr, may, jun,
        jul, aug, sep, oct, nov, dec };

/* strings for the "days of week" */
static const char sun[] = "SUN ";
static const char mon[] = "MON ";
static const char tue[] = "TUE ";
static const char wed[] = "WED ";
static const char thu[] = "THU ";
static const char fri[] = "FRI ";
static const char sat[] = "SAT ";
const char * const week_day_str_ary[7]
    = { sun, mon, tue, wed, thu, fri, sat };

const char colon[] = ":";
const char query[] = "?";
const char bad_st_flag[] = " (bad ST flag)";
const char comma_space[] = ", ";
const char clock_is_set[] = " clock is set ";
const char clock_is_not_set[] = " clock is not set ";
const char of366[] = "/366";

/***************************************************************************
 * Copyright (C) 2013 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright      
 * laws and is the property of Silergy Corp.
 ***************************************************************************/

