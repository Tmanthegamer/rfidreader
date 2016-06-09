#pragma once

#include <windows.h>
#include <stdio.h>
#include "StdAfx.h"
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"
#include "Platform.h"

BOOL getDevices();

int getReaders();

BOOL physicalDisconnect();

BOOL physicalConnect();