// udf_http.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <winsock2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <curl/curl.h>
#include "udf_http.h"

#pragma warning(disable:4996)
extern "C" {
	my_bool http_get_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
	char *http_get(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
	void http_get_deinit(UDF_INIT *initid);

}

static void *myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}

static size_t
result_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct st_curl_results *res = (struct st_curl_results *)data;

	res->result = (char *)myrealloc(res->result, res->size + realsize + 1);
	if (res->result)
	{
		memcpy(&(res->result[res->size]), ptr, realsize);
		res->size += realsize;
		res->result[res->size] = 0;
	}
	return realsize;
}

/* ------------------------HTTP GET----------------------------- */
my_bool http_get_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	st_curl_results *container;
	if (args->arg_count != 1)
	{
		strncpy(message,"one argument must be supplied: http_get('<url>').",MYSQL_ERRMSG_SIZE);
		return 1;
	}
	args->arg_type[0] = STRING_RESULT;
	initid->max_length = CURL_UDF_MAX_SIZE;
	container = (st_curl_results *)malloc(sizeof(st_curl_results));
	initid->ptr = (char *)container;
	return 0;
}

char *http_get(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	/*st_curl_results *res = (st_curl_results *)initid->ptr;
	res->result = (char*)malloc(sizeof(char));
	res->size = 0;
	strcpy(res->result, args->args[0]);
	*length = strlen(res->result);
	return ((char *)res->result);*/
	CURLcode retref;
	CURL *curl;
	st_curl_results *res = (st_curl_results *)initid->ptr;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	res->result = NULL;
	res->size = 0;

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, args->args[0]);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, result_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)res);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "mysql-udf-http/1.0");
		retref = curl_easy_perform(curl);
		if (retref) {
			fprintf(stderr, "error\n");
			if (res->result)
				strcpy(res->result, "");
			res->size = 0;
		}
	}
	curl_easy_cleanup(curl);
	*length = res->size;
	return ((char *)res->result);
}

void http_get_deinit(UDF_INIT *initid)
{
	st_curl_results *res = (st_curl_results *)initid->ptr;
	if (res->result)
		free(res->result);
	free(res);
	return;
}
