// inc/daemonize.h
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "share_state.h"
#include "sig_handling.h"


pid_t daemonize_process(void);