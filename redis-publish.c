#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hiredis.h"

char *channel_name = "default";
char *server_host = "localhost";
int server_port = 6379;

void display_help(char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s [options] message\n\n", basename(prog));
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c, --channel   <channel>   redis channel to broadcast the message on (%s)\n", channel_name);
    fprintf(stderr, "  -s, --server    <server>    redis server hostname (%s)\n", server_host);
    fprintf(stderr, "  -p, --port      <port>      redis server port number (%u)\n", server_port);
    fprintf(stderr, "  -h, --help                  display this help\n\n");
}

int process_args(int argc, char **argv) {
    char *server_port_str = getenv("REDIS_PORT");

    if (getenv("REDIS_SERVER")) {
        server_host = getenv("REDIS_SERVER");
    }

    if (getenv("REDIS_CHANNEL")) {
        channel_name = getenv("REDIS_CHANNEL");
    }

    static struct option long_options[] = {
        { "channel",  required_argument, 0, 'c' },
        { "server",   required_argument, 0, 's' },
        { "help",     no_argument,       0, 'h' },
        { NULL,       0,                 0, 0   }
    };

    int c;

    while ((c = getopt_long(argc, argv, "c:p:s:h", long_options, NULL)) != -1) {
        switch (c) {
            case 'c':
                channel_name = optarg;
                break;
            case 'p':
                server_port_str = optarg;
                break;
            case 's':
                server_host = optarg;
                break;
            default:
                display_help(argv[0]);
                return -1;
        }
    }

    if (server_port_str) {
        if (sscanf(server_port_str, "%d", &server_port) != 1) {
            if (errno != 0) {
                perror("sscanf: server port");
            } else {
                fprintf(stderr, "Non-numeric characters in server port\n");
            }
            return -1;
        }
    }

    if (optind + 1 != argc) {
        display_help(argv[0]);
        return -1;
    }

    return optind;
}

int main(int argc, char **argv) {
    int c = process_args(argc, argv);
    if (c < 1) {
        return 1;
    }

    redisContext *conn;
    redisReply *reply;

    const char* message = argv[c];

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    conn = redisConnectWithTimeout(server_host, server_port, timeout);
    if (conn->err) {
        fprintf(stderr, "Error connecting to \"%s\" on port %d: %s\n", server_host, server_port, conn->errstr);
        redisFree(conn);
        return 2;
    }

    reply = redisCommand(conn, "PUBLISH %s %s", channel_name, message);
    if (reply) {
        if (reply->type == REDIS_REPLY_INTEGER) {
            printf("Announced \"%s\" to %lld \"%s\" subscribers\n", message, reply->integer, channel_name);
        }
        freeReplyObject(reply);
    }

    redisFree(conn);

    return 0;
}
