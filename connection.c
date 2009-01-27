#include <strophe.h>
#include "utils.h"
#include "connection.h"
#include "main.h"

char *msg_to_send;

void conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status, const int error, xmpp_stream_error_t * const stream_error, void * const userdata)
{
	xmpp_stanza_t *reply, *query, *text;
	xmpp_ctx_t *ctx = (xmpp_ctx_t *)userdata;

	if (status == XMPP_CONN_CONNECT) {
		reply = xmpp_stanza_new(ctx);
		xmpp_stanza_set_name(reply, "message");
		xmpp_stanza_set_type(reply, "normal");
		xmpp_stanza_set_attribute(reply, "to", aids_conf.jabber_receiver_id);
		xmpp_stanza_set_attribute(reply, "xmlns", "jabber:client");

		query = xmpp_stanza_new(ctx);
		xmpp_stanza_set_name(query, "body");

		text = xmpp_stanza_new(ctx);
		xmpp_stanza_set_text(text, msg_to_send);
		xmpp_stanza_add_child(query, text);
		xmpp_stanza_add_child(reply, query);

		xmpp_send(conn, reply);
		xmpp_stanza_release(reply);
		xmpp_disconnect(conn);
	}
	else
	{
		logger(stdout, DEBUG, "Inactive");
		xmpp_stop(ctx);
	}
}

/**
 * Send message using XMPP protocol.
 *
 * @param msg message to send
 * @see aids_conf
 */
void send_message(char *msg)
{
	xmpp_ctx_t *ctx;
	xmpp_conn_t *conn;

	/* init library */
	xmpp_initialize();

	/* create a context */
	ctx = xmpp_ctx_new(NULL, NULL);

	/* create a connection */
	conn = xmpp_conn_new(ctx);

	/* setup authentication information */
	xmpp_conn_set_jid(conn, aids_conf.jabber_id);
	xmpp_conn_set_pass(conn, aids_conf.jabber_pass);

	msg_to_send = msg;
	xmpp_connect_client(conn, NULL, 0, conn_handler, ctx);

	xmpp_run(ctx);

	xmpp_conn_release(conn);
	xmpp_ctx_free(ctx);

	/* final shutdown of the library */
	xmpp_shutdown();

	return;
}
