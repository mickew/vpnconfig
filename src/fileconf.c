/*
 * fileconf.c
 * 
 * Copyright 2014 Mikael Wågberg <mickew@Ubuntu-VMDT>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "support.h"

#define MAXLINE 255
#define HOST_KEY "IPSec gateway"
#define GROUPNAME_KEY "IPSec ID"
#define GROUPPASSWORD_KEY "IPSec obfuscated secret"
#define USERNAME_KEY "Xauth username"
#define USERPASSWORD_KEY "Xauth obfuscated password"
#define CONF_FILE_NAME "/usr/local/etc/vpn.conf"

void getvalue(const char *key, char *buff, char **value)
{
	int i, ii, k;
	char *res;

	i = strlen(key)+1;
	ii = strlen(buff);
	res = malloc(ii* sizeof(char));
	strncpy(res, &buff[i], ii);
	k = strlen(res)-1;
	*value = malloc(k* sizeof(char));
	strncpy(*value, res, k);
	(*value)[k] = '\0';
	free(res);	
}

int confsave(const char *host,
			  const char *groupName,
			  const char *groupPassword,
			  const char *userName,
			  const char *userPassword)
{
	int ret = 0;
	char *ecres;
	FILE *f;
	f = fopen(CONF_FILE_NAME, "w");
	fprintf(f, "IPSec gateway %s\n", host);
	fprintf(f, "IPSec ID %s\n", groupName);
	ret = conf_encrypt(groupPassword, &ecres);
	//fprintf(f, "IPSec secret %s\n", groupPassword);
	fprintf(f, "IPSec obfuscated secret %s\n", ecres);
	fprintf(f, "Xauth username %s\n", userName);
	ret = conf_encrypt(userPassword, &ecres);
	//fprintf(f, "Xauth password %s\n", userPassword);
	fprintf(f, "Xauth obfuscated password %s\n", ecres);
	free(ecres);
	fclose(f);
	return ret;
}

int confread(char **host,
			 char **groupName,
			 char **groupPassword,
			 char **userName,
			 char **userPassword)
{
	int ret;
	FILE *f;
	char buff[MAXLINE];
	char *res;
	if ((f = fopen(CONF_FILE_NAME, "r")) != NULL)
	{
		while (fgets(buff, 255, f) !=NULL)
		{
			if (strstr(buff, HOST_KEY))
			{
				getvalue(HOST_KEY, buff, host);
				continue;
			}
			if (strstr(buff, GROUPNAME_KEY))
			{
				getvalue(GROUPNAME_KEY, buff, groupName);
				continue;			
			}
			if (strstr(buff, GROUPPASSWORD_KEY))
			{
				getvalue(GROUPPASSWORD_KEY, buff, &res);
				ret = conf_decrypt(res, groupPassword);
				continue;			
			}
			if (strstr(buff, USERNAME_KEY))
			{
				getvalue(USERNAME_KEY, buff, userName);
				continue;			
			}
			if (strstr(buff, USERPASSWORD_KEY))
			{
				getvalue(USERPASSWORD_KEY, buff, &res);
				ret = conf_decrypt(res, userPassword);
				continue;			
			}
		}
		fclose(f);
	}
	else
	{
		ret = 1;
	}
	return ret;
}
