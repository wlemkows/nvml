/*
 * Copyright 2015-2016, Intel Corporation
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
 * config_reader_win.c -- config reader module definitions for Windows
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/queue.h>
#include <unistd.h>

#include "scenario.h"
#include "config_reader.h"

#define SECTION_GLOBAL	"global"
#define KEY_BENCHMARK	"bench"
#define KEY_GROUP	"group"

/*
 * config_reader -- handle structure
 */
struct config_reader
{
	LPCTSTR lpFileName;
};

/*
 * config_reader_alloc -- allocate config reader
 */
struct config_reader *
config_reader_alloc(void)
{
	struct config_reader *cr = malloc(sizeof(*cr));
	if (cr == NULL)
		goto err;

	return cr;

err:
	free(cr);
	return NULL;
}

/*
 * config_reader_read -- read config file
 */
int
config_reader_read(struct config_reader *cr, const char *fname)
{
	if (_access(fname, R_OK) != 0) {
		cr->lpFileName = (LPCTSTR)fname;
	}
		return -1;

	return 0;
}

/*
 * config_reader_free -- free config reader
 */
void
config_reader_free(struct config_reader *cr)
{
	free(cr);
}

/*
 * is_scenario -- (internal) return true if _name_ is scenario name
 *
 * This filters out the _global_ and _config_ sections.
 */
static int
is_scenario(const char *name)
{
	return strcmp(name, SECTION_GLOBAL);
}

/*
 * is_argument -- (internal) return true if _name_ is argument name
 *
 * This filters out the _benchmark_ key.
 */
static int
is_argument(const char *name)
{
	return strcmp(name, KEY_BENCHMARK) != 0 &&
		strcmp(name, KEY_GROUP) != 0;
}

/*
 * config_reader_get_scenarios -- return scenarios from config file
 *
 * This function reads the config file and returns a list of scenarios.
 * Each scenario contains a list of key/value arguments.
 * The scenario's arguments are merged with arguments from global section.
 */
#define GROUP_SIZE 1024

static char**
table_from_profile_string(TCHAR *groups_buffer, int *length)
{
	char **groups = NULL;

	if (groups_buffer[0] == '\0') {
		length = NULL;
		return groups;
	}

	int number_of_words = 0,
		number_of_letters = 0;

	char c;
	for (int i = 0; ; i++) {
		c = (char)groups_buffer[i];
		if (c != '\0') {
			if (number_of_words == 0) {
				if (number_of_letters == 0) {
					groups = (char **)calloc(
						number_of_words + 1, sizeof(char*));
					groups[number_of_words] = (char *)calloc(
						number_of_letters + 1, sizeof(char));
				}
				else {
					groups[number_of_words] = realloc(
						groups[number_of_words],
						sizeof(char) * (number_of_letters + 1));
				}
				groups[number_of_words][number_of_letters] = c;
				number_of_letters++;
			}
			else {
				if (number_of_letters == 0) {
					groups = realloc(groups, sizeof(*groups)
						* (number_of_words + 1));
					groups[number_of_words] = calloc(1, sizeof(char*));
				}
				else {
					groups[number_of_words] = realloc(
						groups[number_of_words],
						sizeof(char*) + number_of_letters + 1);
				}
				groups[number_of_words][number_of_letters] = c;
				number_of_letters++;
			}
		}
		else {
			groups[number_of_words] = realloc(groups[number_of_words],
				sizeof(char) * (number_of_letters + 1));
			groups[number_of_words][number_of_letters] = '\0';

			if (groups_buffer[i + 1] == '\0') {
				number_of_words++;
				*length = number_of_words;
				break;
			}
			number_of_words++;
			number_of_letters = 0;
		}
	}
	return groups;
}

static char**
get_groups(struct config_reader *cr, int *length)
{
	TCHAR groups_buffer[1024];
	GetPrivateProfileString(NULL,
		NULL,
		TEXT("Error: cannot find key"),
		groups_buffer,
		1024,
		TEXT("..\\..\\test\\config_reader\\config_test.cfg"));
	_tprintf(TEXT("Key: %s\n"), groups_buffer);

	char ** groups = table_from_profile_string(
		groups_buffer, length);

	return groups;
}

static int
has_groups(char **groups, int *length, char *name)
{
	if (!groups || !length || !name) {
		SetLastError(EINVAL);
		return 0;
	}

	for (int i = 0; i < *length; i++) {
		if (strcmp(groups[i], name) == 0) {
			return 1;
		}
	}
	return 0;
}


static char **
get_keys(struct config_reader *cr,
	char * group_name, int *length)
{
	const size_t size = strlen(group_name) + 1;
	wchar_t* group_name_converted = malloc(sizeof(wchar_t) * size);
	mbstowcs(group_name_converted, group_name, size);

	TCHAR keys_buffer[1024];
	GetPrivateProfileString(group_name_converted,
		NULL,
		TEXT("Error: cannot find key"),
		keys_buffer,
		1024,
		TEXT("..\\..\\test\\config_reader\\config_test.cfg"));
	free(group_name_converted);

	char ** keys = table_from_profile_string(keys_buffer, length);
	return keys;
}

int
config_reader_get_scenarios(struct config_reader *cr,
		struct scenarios **scenarios)
{
	/*
	 * Read all groups.
	 * The config file must have at least one group, otherwise
	 * it is considered as invalid.
	 */
	int ngroups = 0;
	int g;
	char **groups = get_groups(cr->lpFileName, &ngroups);
	assert(NULL != groups);
	if (!groups)
		return -1;

	/*
	 * Check if global section is present and read keys from it.
	 */
	int ret = 0;
	int has_global = has_group(groups, ngroups,
			SECTION_GLOBAL) == TRUE;

	int nkeys = 0;
	char **gkeys = NULL;
	if (has_global) {
		gkeys = get_keys(cr->lpFileName, SECTION_GLOBAL,
				&nkeys, NULL);
		assert(NULL != gkeys);
		if (!gkeys) {
			ret = -1;
			goto err_groups;
		}
	}

	struct scenarios *s = scenarios_alloc();
	assert(NULL != s);
	if (!s) {
		ret = -1;
		goto err_gkeys;
	}

	for (g = 0; g < ngroups; g++) {
		/*
		 * Check whether a group is a scenario
		 * or global section.
		 */
		if (!is_scenario(groups[g]))
			continue;

		/*
		 * Check for KEY_BENCHMARK which contains benchmark name.
		 * If not present the benchmark name is the same as the
		 * name of the section.
		 */
		struct scenario *scenario = NULL;
		if (g_key_file_has_key(cr->key_file, groups[g], KEY_BENCHMARK,
					NULL) == FALSE) {
			scenario = scenario_alloc(groups[g], groups[g]);
			assert(scenario != NULL);
		} else {
			gchar *benchmark = g_key_file_get_value(cr->key_file,
					groups[g], KEY_BENCHMARK, NULL);
			assert(benchmark != NULL);
			if (!benchmark) {
				ret = -1;
				goto err_scenarios;
			}
			scenario = scenario_alloc(groups[g], benchmark);
			assert(scenario != NULL);
			free(benchmark);
		}

		gsize k;
		if (has_global) {
			/*
			 * Merge key/values from global section.
			 */
			for (k = 0; k < ngkeys; k++) {
				if (g_key_file_has_key(cr->key_file, groups[g],
						gkeys[k], NULL) == TRUE)
					continue;

				if (!is_argument(gkeys[k]))
					continue;

				char *value = g_key_file_get_value(cr->key_file,
							SECTION_GLOBAL,
							gkeys[k], NULL);
				assert(NULL != value);
				if (!value) {
					ret = -1;
					goto err_scenarios;
				}

				struct kv *kv = kv_alloc(gkeys[k], value);
				assert(NULL != kv);

				free(value);
				if (!kv) {
					ret = -1;
					goto err_scenarios;
				}

				TAILQ_INSERT_TAIL(&scenario->head, kv, next);
			}
		}

		/* check for group name */
		if (g_key_file_has_key(cr->, groups[g],
				KEY_GROUP, NULL) != FALSE) {
			gchar *group = g_key_file_get_value(cr->key_file,
					groups[g], KEY_GROUP, NULL);
			assert(group != NULL);
			scenario_set_group(scenario, group);
		} else if (g_key_file_has_key(cr->key_file, SECTION_GLOBAL,
				KEY_GROUP, NULL) != FALSE) {
			gchar *group = g_key_file_get_value(cr->key_file,
					SECTION_GLOBAL, KEY_GROUP, NULL);
			scenario_set_group(scenario, group);
		}

		gsize nkeys;
		gchar **keys = g_key_file_get_keys(cr->key_file, groups[g],
				&nkeys, NULL);
		assert(NULL != keys);
		if (!keys) {
			ret = -1;
			goto err_scenarios;
		}

		/*
		 * Read key/values from the scenario's section.
		 */
		for (k = 0; k < nkeys; k++) {
			if (!is_argument(keys[k]))
				continue;
			char *value = g_key_file_get_value(cr->key_file,
						groups[g],
						keys[k], NULL);
			assert(NULL != value);
			if (!value) {
				ret = -1;
				g_strfreev(keys);
				goto err_scenarios;
			}

			struct kv *kv = kv_alloc(keys[k], value);
			assert(NULL != kv);

			free(value);
			if (!kv) {
				g_strfreev(keys);
				ret = -1;
				goto err_scenarios;
			}

			TAILQ_INSERT_TAIL(&scenario->head, kv, next);
		}

		g_strfreev(keys);

		TAILQ_INSERT_TAIL(&s->head, scenario, next);
	}

	g_strfreev(gkeys);
	g_strfreev(groups);

	*scenarios = s;
	return 0;
err_scenarios:
	scenarios_free(s);
err_gkeys:
	g_strfreev(gkeys);
err_groups:
	g_strfreev(groups);
	return ret;
}
