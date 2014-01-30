#include "client_thread.hh"

void ClientThread::run(bool unthreaded) {
	if (unthreaded) {
		main();
		delete this;
	}
	else {
		PThread::run();
	}
}

ClientThread::ClientThread(Client *_client) : client(_client) {
	client->ref();
}

ClientThread::~ClientThread() {
	client->unref();
}

