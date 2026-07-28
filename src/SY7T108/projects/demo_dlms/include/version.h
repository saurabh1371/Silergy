/***************************************************************************
 * This code and information is provided "as is" without warranty of any
 * kind, either expressed or implied, including but not limited to the 
 * implied warranties of merchantability and/or fitness for a particular
 * purpose.
 *
 * Copyright (C) 2005 Silergy Corp. All Rights Reserved.
 *
 * FILE: version.h
 *    
 * DESCRIPTION: Meter software version number file.
 * What it means: <num1>.<num2>.<num3>, e.g. 8.9a
 * <num1> changes for total redesigns.
 * <num2> changes for each formal release, new feature or a 
 * specification change.
 * <num3> if present, distinguishes minor versions with fixed defects.
 * 
 * $Id: version.h 25648 2020-06-10 11:00:29Z phuddar $
 ***************************************************************************/

#ifndef VERSION_H
#define VERSION_H

// Common device driver versions
extern const char * const fw_ver_common[];

// Specific (for a specific model) device driver versions
extern const char * const fw_ver_specific[];

// SDK Library versions 
extern const char * const fw_ver_lib[];

// Unchanging copyright text.
extern const char copyrightA[], ic_version[], copyrightB[];
extern const char software_version[]; // Changes every compile.

/***************************************************************************
 * Copyright (C) 2017 Silergy Corp. All Rights Reserved.
 * this program is fully protected by the United States copyright 
 * laws and is the property of Silergy Corp.
 ***************************************************************************/
#endif /* VERSION_H */
