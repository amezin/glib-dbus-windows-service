#include <stdlib.h>

#include <gio/gio.h>

#include "test-dbus-interface.h"

int main(int argc, char *argv[])
{
    g_autoptr(GError) error = NULL;

    g_autoptr(GFile) address_file = g_file_new_for_path("dbus-server-address");
    g_autoptr(GBytes) address = g_file_load_bytes(address_file, NULL, NULL, &error);
    if (!address) {
        g_warning("Can't read server address file: %s", error->message);
        return EXIT_FAILURE;
    }

    g_message("Connecting to %s", (const char *)g_bytes_get_data(address, NULL));

    g_autoptr(GDBusConnection) connection = g_dbus_connection_new_for_address_sync(
        g_bytes_get_data(address, NULL),
        G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
        NULL,
        NULL,
        &error
    );

    if (!connection) {
        g_warning("Can't create D-Bus connection: %s", error->message);
        return EXIT_FAILURE;
    }

    g_message("Connected");

    g_autoptr(TestPeerInterface) iface = test_peer_interface_proxy_new_sync(
        connection,
        G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS,
        NULL,
        "/",
        NULL,
        &error
    );

    if (!iface) {
        g_warning("Can't create D-Bus interface: %s", error->message);
        return EXIT_FAILURE;
    }

    g_message("Created interface");

    g_autofree gchar *response = NULL;
    if (!test_peer_interface_call_hello_world_sync(iface, "test", &response, NULL, &error)) {
        g_warning("Method call failed: %s", error->message);
        return EXIT_FAILURE;
    }

    g_print("Response: %s\n", response);
    return EXIT_SUCCESS;
}
