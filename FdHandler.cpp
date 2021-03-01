#include <unistd.h>

#include "FdHandler.h"

FdHandler::~FdHandler()
{
    close(fd);
}