/*
 * Copyright 2017, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * setenv.c -- set NVML registry variables on Windows during installation
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "setenv.h"


/*
 * get_reg_env - get value from registry
 */
int
get_reg_env(wchar_t *subkey, wchar_t *var, wchar_t *data, unsigned long size)
{
	LONG result;
	HKEY key = NULL;

	result = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE, subkey, 0,
		KEY_QUERY_VALUE, &key);

	if (result != ERROR_SUCCESS) {
		if (result == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
		} else {
			printf("Error opening key.\n");
		}
		return 1;
	}

	result = RegGetValueW(
		HKEY_LOCAL_MACHINE, subkey, var,
		RRF_RT_ANY, NULL, (PVOID)data, &size);

	if (result != ERROR_SUCCESS) {
		printf("Cannot get registry value %S\n", subkey);
		if (result == ERROR_MORE_DATA) {
			printf("Variable is too long to read value\n");
		} else {
			printf("Cannot add NVML to system path, error: %ld\n",
				result);
		}
		return 1;
	}

	result = RegCloseKey(key);
	if (result != ERROR_SUCCESS) {
		printf("Cannot close registry key, error: %ld\n", result);
		return 1;
	}

	printf("Successfully get register value\n");
	return 0;
}

/*
 * set_reg_env - set registry value
 */
int
set_reg_env(wchar_t *subkey, wchar_t *var, wchar_t *out)
{
	LONG result;
	HKEY key = NULL;

	result = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE, subkey, 0,
		KEY_SET_VALUE, &key);
	if (result != ERROR_SUCCESS) {
		if (result == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
		} else {
			printf("Error opening key.\n");
		}
		return 1;
	}

	result = RegSetValueExW(
		key, var, 0, REG_EXPAND_SZ,
		(BYTE *)out, lstrlenW(out) * sizeof(wchar_t) + 1);
	if (result != ERROR_SUCCESS) {
		printf("Cannot set registry value, error: %ld\n", result);
		return 1;
	}

	result = RegCloseKey(key);
	if (result != ERROR_SUCCESS) {
		printf("Cannot close registry key, error: %ld\n", result);
		return 1;
	}
	printf("Successfully updated PATH value\n");
	return 0;
}

int
main()
{
	wchar_t nvml_exe[MAX_PATH];
	unsigned long buffer_nvml_exe = MAX_PATH;

	wchar_t path_env[MAX_ENV_PATH];
	unsigned long buffer_path_env = MAX_ENV_PATH;

	wchar_t path_out[MAX_ENV_PATH + 2] = { 0 };

	if (get_reg_env(HKLM_ENV, NVML_EXE,
		nvml_exe, buffer_nvml_exe)) {
		goto end;
	}

	if (get_reg_env(HKLM_ENV, PATH_ENV,
		path_env, buffer_path_env)) {
		goto end;
	}

	int ret = _snwprintf_s(
		path_out, MAX_ENV_PATH, MAX_ENV_PATH,
		L"%s;%s", path_env, nvml_exe);
	if (ret < 0 || ret >= MAX_ENV_PATH) {
		printf("Cannot add NVML to system PATH because of truncate\n");
		goto end;
	}

	return 0;

end:
	printf("Installation will be continued. Press Enter to continue...\n");
	getchar();
	return 0;
}
