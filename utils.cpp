#include <stdio.h>
#include <string.h>

#include <boost/format.hpp>

#include "utils.h"

std::string systemError(const std::string &msgPrefix)
{
    char buf[1024] = {0};
    return str(boost::format("%1%: %2%") % msgPrefix % strerror_r(errno, buf, sizeof(buf)));
}
