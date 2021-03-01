#include <stdlib.h>
#include <string.h>

#include <gio/gio.h>

#include "test-dbus-interface.h"

static gboolean handle_hello_world(TestPeerInterface *iface,
                                   GDBusMethodInvocation *invocation,
                                   const gchar *greeting,
                                   gpointer user_data G_GNUC_UNUSED)
{
    g_message("Received request: %s", greeting);
    test_peer_interface_complete_hello_world(iface, invocation, greeting);
    return TRUE;
}

static gboolean handle_connection(GDBusServer *server,
                                  GDBusConnection *connection,
                                  gpointer user_data)
{
    GDBusInterfaceSkeleton *skeleton = G_DBUS_INTERFACE_SKELETON(user_data);
    g_autoptr(GError) error = NULL;

    if (g_dbus_interface_skeleton_export(skeleton, connection, "/", &error))
        return TRUE;

    g_warning("Can't export D-Bus interface: %s", error->message);
    return FALSE;
}

int main(void)
{
    g_autoptr(GError) error = NULL;

    g_autoptr(TestPeerInterface) skeleton = test_peer_interface_skeleton_new();
    g_signal_connect(skeleton, "handle-hello-world", G_CALLBACK(handle_hello_world), NULL);

    g_autofree gchar *guid = g_dbus_generate_guid();
    g_autoptr(GDBusServer) server = g_dbus_server_new_sync(
        "tcp:host=localhost",
        G_DBUS_SERVER_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS,
        guid,
        NULL,
        NULL,
        &error
    );

    if (!server) {
        g_warning("Can't create D-Bus server: %s", error->message);
        return EXIT_FAILURE;
    }

    g_signal_connect_object(server, "new-connection", G_CALLBACK(handle_connection), skeleton, 0);
    g_dbus_server_start(server);

    const gchar *client_address = g_dbus_server_get_client_address(server);
    g_message("D-Bus server is listening on %s", client_address);

    g_autoptr(GFile) address_file = g_file_new_for_path("dbus-server-address");
    gboolean file_written = g_file_replace_contents(
        address_file,
        client_address,
        strlen(client_address),
        NULL,
        FALSE,
        G_FILE_CREATE_REPLACE_DESTINATION,
        NULL,
        NULL,
        &error
    );

    if (!file_written) {
        g_warning("Can't write server address to file: %s", error->message);
        return EXIT_FAILURE;
    }

    g_autoptr(GMainLoop) loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return EXIT_SUCCESS;
}
