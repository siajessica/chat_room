#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <sys/stat.h>
#include <dirent.h> 
#include <sqlite3.h>
#include "database.h"
#include "login_interface.h"
#include "relationship.h"
#include "message.h"

#define MAX_LEN 1280
#define MAX_USER 20

using namespace std;

struct terminal{
	int id;
	string name;
	int socket;
	thread th;
}

vector<terminal> clients;
int seed = 0;
mutex cout_mtx, clients_mtx;
sqlite3 *db;

int main(){
	int server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(10000);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 0);

	if ((bind(server_socket, (struct sockaddr*)&server, sizeof(struct sockaddr_in))) == -1)
	{
		perror("bind error: ");
		exit(-1);
	}

	if ((listen(server_socket, 8)) == -1)
	{
		perror("listen error: ");
		exit(-1);
	}

	createDB();
	int rc = sqlite3_open("chatroom.db", &db);

	struct sockaddr_in client;
	int client_socket;
	unsigned int len = sizeof(sockaddr_in);

	while (1)
	{
		if ((client_socket = accept(server_socket, (struct sockaddr*)&client, &len)) == -1)
		{
			perror("accept error: ");
			exit(-1);
		}
		seed++;
		thread t(handle_client, client_socket, seed);
		lock_guard<mutex> guard(clients_mtx);
		clients.push_back({ seed, string("Anonymous"),client_socket,(move(t)) });
	}

	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].th.joinable())
			clients[i].th.join();
	}

	close(server_socket);
	return 0;
}

void end_connection(int id)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].id == id)
		{
			lock_guard<mutex> guard(clients_mtx);
			clients[i].th.detach();
			clients.erase(clients.begin() + i);
			close(clients[i].socket);
			break;
		}
	}
}

void handle_client(int client_socket, int id)
{
	string username, password, command, target_user;
	int ret;
	recv(client_socket, command, sizeof(command), 0);
	cout << "sombody want";
	while (1)
	{
		if (command == "1")
		{
			cout << "sign in" << endl;
			recv(client_socket, username, sizeof(username), 0);
			recv(client_socket, password, sizeof(password), 0);
			ret = login(username, password);
			send(client_socket, ret, sizeof(ret), 0);		
		}
		else if(command = "2")
		{
			cout << "sign up" << endl;
			recv(client_socket, username, sizeof(username), 0);
			recv(client_socket, password, sizeof(password), 0);
			ret = sign_up(username, password);
			send(client_socket, ret, sizeof(ret), 0);
		}
		else if(command == "4"){
			recv(client_socket, target_user, sizeof(target_user), 0);
			ret = add_friend(username, target_user);
			send(client_socket, ret, sizeof(ret), 0);
		}
		else if(command == "5"){
			recv(client_socket, target_user, sizeof(target_user), 0);
			ret = remove_friend(username, target_user);
			send(client_socket, ret, sizeof(ret), 0);
		}
	}
	
	//star to chat
	strcpy(name, account);
	set_name(id, name);
	// Display welcome message
	string welcome_message = string(name) + string(" has joined");
	broadcast_message("#NULL", id);
	broadcast_message(id, id);
	broadcast_message(welcome_message, id);
	shared_print(color(id) + welcome_message + def_col);

	while (1)
	{
		int bytes_received = recv(client_socket, str, sizeof(str), 0);
		if (bytes_received <= 0)
			return;
		if (strcmp(str, "#exit") == 0)
		{
			// Display leaving message
			string message = string(name) + string(" has left");
			broadcast_message("#NULL", id);
			broadcast_message(id, id);
			broadcast_message(message, id);
			shared_print(color(id) + message + def_col);
			end_connection(id);
			return;
		}
		broadcast_message(string(name), id);
		broadcast_message(id, id);
		broadcast_message(string(str), id);
		shared_print(color(id) + name + " : " + def_col + str);
	}
}