#include <czmq.h>

int main(void) {
	zsock_t *responder = zsock_new(ZMQ_REP);
	int rc = zsock_bind (responder, "tcp://127.0.0.1:5555");
	assert(rc == 5555);

	while (1) {
		char *str = zstr_recv(responder);
		printf("Received Hello\n");
		sleep(1);
		zstr_send(responder, "World");
		zstr_free(&str);
	}
	return 0;
}
