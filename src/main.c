/*
 * main.c
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


#include <stdlib.h>
#include <stdio.h>
#include "support.h"

#define UI_FILE "mainwindow.glade"

void getvalues(AppData *data)
{
	char *host, *groupName, *groupPassword, *userName, *userPassword;
	int ret = confread(&host, &groupName, &groupPassword, &userName, &userPassword);
	if (ret == 0)
	{
		gtk_entry_set_text(GTK_ENTRY(data->tbhost), host);
		gtk_entry_set_text(GTK_ENTRY(data->tbgroupname), groupName);
		gtk_entry_set_text(GTK_ENTRY(data->tbgrouppassword), groupPassword);
		gtk_entry_set_text(GTK_ENTRY(data->tbusername), userName);
		gtk_entry_set_text(GTK_ENTRY(data->tbuserpassword), userPassword);
		free(host);
		free(groupName);
		free(groupPassword);
		free(userName);
		free(userPassword);
	}
}

int main(int argc, char *argv[])
{
	AppData *data;
	GtkBuilder *builder;
	GError *error = NULL;
	
	gtk_init(&argc, &argv);
	
	builder = gtk_builder_new ();
	if( ! gtk_builder_add_from_string( builder, sGui, -1, &error ) )
//	if( ! gtk_builder_add_from_file( builder, UI_FILE, &error ) )
	{
		g_warning( "%s", error->message );
		g_free( error );
		return( 1 );
	}
	
	/* Allocate data structure */
	data = g_slice_new( AppData );

	/* Get objects from UI */
#define GW( name ) APP_GET_WIDGET( builder, name, data )
	GW( main_window );
	GW( btnok );
	GW( tbhost );
	GW( tbgroupname );
	GW( tbgrouppassword );
	GW( tbusername );
	GW( tbuserpassword );
#undef GW

	getvalues (data);
	gtk_builder_connect_signals( builder, data );
	
	g_object_unref( G_OBJECT( builder ) );
	gtk_widget_show( data->main_window );
	gtk_main();
	
	return 0;
}

