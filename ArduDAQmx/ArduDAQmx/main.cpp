// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h" //TODO: move all precompiled headers to stdafx.h
#include "ansi_c.h"
#include "macrodef.h"
#include "msunistd.h"
#include "ArduDAQmx.h"

int main()
{
	ArduDAQmxInit();
	enumerateDAQmxDevices(1);
	getchar();
	return 0;
}