/*
 * support.h
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

#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include <gtk/gtk.h>

/* Convenience macros for obtaining objects from UI file */
#define APP_GET_OBJECT( builder, name, type, data ) \
	data->name = type( gtk_builder_get_object( builder, #name ) )
#define APP_GET_WIDGET( builder, name, data ) \
	APP_GET_OBJECT( builder, name, GTK_WIDGET, data )

const char *sGui;

/* Main data structure definition */
typedef struct _AppData AppData;
struct _AppData
{
	/* Widgets */
	GtkWidget *main_window;  	/* Main application window */
	GtkWidget *btnok;   		/* Ok button */
	GtkWidget *tbhost;			/* Host Entry box */
	GtkWidget *tbgroupname;		/* Group Name Entry box */
	GtkWidget *tbgrouppassword;	/* Group Password Entry box */
	GtkWidget *tbusername;		/* User Name Entry box */
	GtkWidget *tbuserpassword;	/* User Password Entry box */
};

int conf_encrypt(const char *pw, char **result);
int conf_decrypt(const char *encryptedpw, char **result);

int confsave(const char *host,
			  const char *groupName,
			  const char *groupPassword,
			  const char *userName,
			  const char *userPassword);
int confread(char **host,
			 char **groupName,
			 char **groupPassword,
			 char **userName,
			 char **userPassword);


#endif /* __SUPPORT_H__ */
