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

#include "callback.h"

UniqueResponse
message_received_cb (UniqueApp         *app,
                     UniqueCommand      command,
                     UniqueMessageData *message,
                     guint              time_,
                     gpointer           user_data)
{
	UniqueResponse res;
	struct Gtkabber *gtkabber = (struct Gtkabber *)user_data;

	switch (command)
	{
		case COMMAND_PRINT_XID:
			setup_new_socket_for_plug( gtkabber );
			res = UNIQUE_RESPONSE_OK;
			break;
		default:
			res = UNIQUE_RESPONSE_OK;
			break;
	}

	return res;
}

/* Eventually needed to update parent window when child window emits
 * PropertyNotify event for things like client title changes, etc.
 */
void
property_notify_cb(GtkWindow *window, GdkEvent *e, gpointer unused)
{
	return;
}

void
page_removed_cb(GtkNotebook *nb, GtkWidget *widget, guint id, gpointer data)
{
	struct Gtkabber *g = (struct Gtkabber *)data;

	GList *temp;

	for(temp = g->socket_list; g->socket_list ; g->socket_list = g->socket_list->next )
	{
		if( g->socket_list->data == widget )
			temp = g_list_delete_link( g->socket_list, temp );
	}

	return;
}
