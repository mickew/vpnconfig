/*
 * callbacks.c
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


#include "support.h"

G_MODULE_EXPORT void
on_btnok_clicked( GtkWidget *widget,
				 AppData *data )
{
	confsave(gtk_entry_get_text(GTK_ENTRY(data->tbhost)),
			 gtk_entry_get_text(GTK_ENTRY(data->tbgroupname)),
			 gtk_entry_get_text(GTK_ENTRY(data->tbgrouppassword)),
			 gtk_entry_get_text(GTK_ENTRY(data->tbusername)),
			 gtk_entry_get_text(GTK_ENTRY(data->tbuserpassword)));
	gtk_widget_destroy(GTK_WIDGET(data->main_window));
}

