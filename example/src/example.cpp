/*  ========================================================================
    Copyright (C) 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    ========================================================================
*/
#include "example.h"

#include <fstream>
#include <string>

static int writeToFile(const std::string & fullPath, uint8_t value);

int setHealthState(const char * serviceName, uint8_t healthState)
{
    return writeToFile( std::string(serviceName) + ".health", healthState);
}

int setOperatingStatus(const char * serviceName, uint8_t operatingStatus)
{
    return writeToFile( std::string(serviceName) + ".operating", operatingStatus);
}

static int writeToFile(const std::string & fullPath, uint8_t value)
{
    try
    {
        std::ofstream outfile(fullPath, std::ofstream::trunc);
        outfile << std::to_string(value) << std::endl;
        outfile.close();
    }
    catch(...)
    {
        return -1;
    }

    return 0;
}