/*
 * Copyright (c) 2011 Thomas Adam <thomas@xteddy.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "gtkabber.h"
#include "callback.h"

void
new_tab_page(struct Gtkabber *gtkabber, GtkWidget *socket)
{
	gtk_widget_show( socket );
	gtk_notebook_append_page( GTK_NOTEBOOK(gtkabber->toplevel.notebook),
			socket, NULL );
	gtk_notebook_set_current_page( GTK_NOTEBOOK(gtkabber->toplevel.notebook), -1 );

	gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK(
				gtkabber->toplevel.notebook), socket, TRUE);

	printf("%d\n", gtk_socket_get_id( GTK_SOCKET( socket ) ) );
	fflush(stdout);
}

void
setup_new_socket_for_plug(struct Gtkabber *gtkabber)
{
	struct ContainerData	 *cd;
	static int __id;

	{
		cd = malloc( sizeof *cd );
		if (cd == NULL)
		{
			fprintf( stderr, "Can't malloc() *cd\n" );
			exit( EXIT_FAILURE );
		}
		memset( cd, '\0', sizeof *cd );
	}

	cd->id = ++__id;
	cd->socket = gtk_socket_new();

	new_tab_page( gtkabber, GTK_WIDGET(cd->socket) );

	cd->xwin_id = gtk_socket_get_id( GTK_SOCKET( cd->socket ) );
	gtkabber->toplevel.next_socket_id = cd->xwin_id;

	gtkabber->socket_list = g_list_append( gtkabber->socket_list,
			(gpointer)cd );
}

void
unique_instance(int argc, char **args, UniqueApp *app)
{
	int	 i = 0;

	/* FIXME - rework. */
	for(; i < argc; ++i)
	{
		if (strcmp( args[i], "next-socket" ) == 0)
		{

			UniqueMessageData* data = unique_message_data_new();
			unique_message_data_set_text(data, args[i], strlen(args[i]));

			unique_app_send_message(app, COMMAND_PRINT_XID, data);
			unique_message_data_free(data);
		}
	}

	g_object_unref(app);
}

void
setup_toplevel_win(struct Gtkabber *gtkabber, UniqueApp *app)
{
	/* FIXME - create setup_signals() */
	gtkabber->toplevel.toplevel_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(gtkabber->toplevel.toplevel_win, "delete-event",
			gtk_main_quit, NULL);	
	gtk_window_set_default_size(
			GTK_WINDOW(gtkabber->toplevel.toplevel_win),
			800, 600);

	gtkabber->toplevel.notebook = gtk_notebook_new();
	
	/* GtkNotebook options. */
	gtk_notebook_popup_enable(GTK_NOTEBOOK(
				gtkabber->toplevel.notebook));
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(
				gtkabber->toplevel.notebook), TRUE);
	gtk_notebook_set_tab_border(GTK_NOTEBOOK(
				gtkabber->toplevel.notebook), 1);
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK(
				gtkabber->toplevel.notebook),
			GTK_POS_LEFT);

	gtk_container_add(GTK_CONTAINER(gtkabber->toplevel.toplevel_win),
			gtkabber->toplevel.notebook);

	gtk_widget_show_all(gtkabber->toplevel.toplevel_win);

	unique_app_watch_window (app, GTK_WINDOW(gtkabber->toplevel.toplevel_win));		
	g_signal_connect(G_OBJECT(app), "message-received",
			G_CALLBACK(message_received_cb), (gpointer)gtkabber);

	g_signal_connect(gtkabber->toplevel.notebook, "page-removed",
			G_CALLBACK(page_removed_cb), (gpointer)gtkabber);
}

int
main(int argc, char **argv)
{
	struct Gtkabber		 gtkabber;
	gboolean		 version = FALSE;
	GError			*error = NULL;
	UniqueApp		*unique_app;
	int			 i = 1;
	
	if (!g_thread_supported())
		g_thread_init(NULL);

	memset( &gtkabber, '\0', sizeof( struct Gtkabber ) );
	
	GOptionEntry cmdline_ops[] = {
		{
			"version",
			'v',
			0,
			G_OPTION_ARG_NONE,
			&version,
			"Print version information",
			NULL
		},
		{ NULL }
	};

	if (!gtk_init_with_args( &argc, &argv, "foo",
				 cmdline_ops,
				 NULL, &(error)))
	{
		g_printerr( "Can't init gtk: %s\n", error->message );
		g_error_free( error );
		return EXIT_FAILURE;
	}

	unique_app = unique_app_new_with_commands( "gtkabber.gtkabber", NULL,
		       "next-socket", COMMAND_PRINT_XID, NULL );
	if( unique_app_is_running( unique_app ))
	{
		unique_instance( argc, argv, unique_app );
		return EXIT_SUCCESS;
	} 
		
	if (version)
	{
		fprintf(stderr, "%s %s\n", "gtkabber ", VERSION);
		return EXIT_SUCCESS;
	}

	setup_toplevel_win( &gtkabber, unique_app );
	for(; i < argc; ++i)
	{
		if (strcmp( argv[i], "next-socket" ) == 0)
			setup_new_socket_for_plug( &gtkabber );
	}
	gtk_main();
	g_object_unref(unique_app);

	return EXIT_SUCCESS;
}
