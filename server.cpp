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

struct terminal {
	int id;
	string name;
	int socket;
	thread th;
};

vector<terminal> clients;
int seed = 0;
mutex cout_mtx, clients_mtx;
sqlite3 *db;
void handle_client(int client_socket, int id);
string chatting_user1;
string chatting_user2;

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
	//int rc = sqlite3_open("chatroom.db", &db);

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
	int number_of_clients = clients.size();
	for (int i = 0; i < number_of_clients; i++)
	{
		if (clients[i].th.joinable())
			clients[i].th.join();
	}

	close(server_socket);
	return 0;
}

void end_connection(int id)
{
	int number_of_clients = clients.size();
	for (int i = 0; i < number_of_clients; i++)
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

void broadcast_message(string message_s,string targetname)
{
	int number_of_clients = clients.size();
	char message[MAX_LEN] = { 0 };
	strcpy(message,message_s.c_str());
	for (int i = 0; i < number_of_clients; i++)
	{
		if (clients[i].name == targetname)
		{
			send(clients[i].socket, message, MAX_LEN, 0);
		}
	}
}

void shared_print(string str, int current_chat_id, bool endLine = true)
{
	lock_guard<mutex> guard(cout_mtx);
	cout << current_chat_id << " " << str;
	if (endLine)
		cout << endl;
}

void handle_client(int client_socket, int id)
{
	char username[20], password[20], command[20],target_user[20];
	string username_s, password_s,target_user_s;
	int ret = 0,ID,current_chat_id;
	cout << "sombody want";
	while (1)
	{
		recv(client_socket, command, sizeof(command), 0);
		if (command[0] == '1')
		{
			cout << "sign in" << endl;
			recv(client_socket, username, sizeof(username), 0);
			recv(client_socket, password, sizeof(password), 0);
			username_s.assign(username);
			password_s.assign(password);
			ID = login(username_s, password_s);
			clients[id].id = ID;
			clients[id].name = username_s;
			if (ID != -1)
				send(client_socket, "1", sizeof("1"), 0);
			else
				send(client_socket, "0", sizeof("0"), 0);
		}
		else if (command[0] == '2')
		{
			cout << "sign up" << endl;
			recv(client_socket, username, sizeof(username), 0);
			recv(client_socket, password, sizeof(password), 0);
			username_s.assign(username);
			password_s.assign(password);
			ID = sign_up(username_s, password_s);
			if (ID != -1)
				send(client_socket, "1", sizeof("1"), 0);
			else
				send(client_socket, "0", sizeof("0"), 0);
		}
		else if (command[0] == '4') {
			recv(client_socket, target_user, sizeof(target_user), 0);
			target_user_s.assign(target_user);
			ret = add_friend(username_s, target_user_s);
			if (ret == 5)
				send(client_socket, "1", sizeof("1"), 0);
			else
				send(client_socket, "0", sizeof("0"), 0);
		}
		else if (command[0] == '5') {
			recv(client_socket, target_user, sizeof(target_user), 0);
			target_user_s.assign(target_user);
			ret = remove_friend(username_s, target_user_s);
			if (ret)
				send(client_socket, "1", sizeof("1"), 0);
			else
				send(client_socket, "0", sizeof("0"), 0);
		}
		else if(command[0] == '6') {
			recv(client_socket, target_user, sizeof(target_user), 0);
			target_user_s.assign(target_user);
			current_chat_id = get_chatid(username_s, target_user_s);
			break;
		}
	}
	
	//std::string ID_s = std::to_string(ID);
	while (1)
	{
		char str[MAX_LEN];
		int bytes_received = recv(client_socket, str, sizeof(str), 0);
		if (bytes_received <= 0)
			break;
		if (strcmp(str, "#exit") == 0)
		{
			// Display leaving message
			string message = username_s + string(" has left");
			broadcast_message("#NULL", target_user_s);
			//broadcast_message(ID_s, target_user_s);
			broadcast_message(message, target_user_s);
			shared_print(message,current_chat_id);
			end_connection(ID);
			break;
		}
		broadcast_message(username_s, target_user_s);
		//broadcast_message(ID_s, target_user_s);
		broadcast_message(string(str), target_user_s);
		shared_print(username_s + " : "  + str,current_chat_id);
	}
}
