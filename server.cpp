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
#define MAX_LEN 200
#define NUM_COLORS 6
#define MAX_USER 20
char account_list[20][10];
char password_list[20][10];
int number_of_user=0;
using namespace std;

struct information
{
	char account[10];
	char password[10];
	int status;// 1: online, 0: offline
} user[MAX_USER];
struct terminal
{
	int id;
	string name;
	int socket;
	thread th;
};

vector<terminal> clients;
string def_col = "\033[0m";
string colors[] = { "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m" };
int seed = 0;
mutex cout_mtx, clients_mtx;

string color(int code);
void set_name(int id, char name[]);
void shared_print(string str, bool endLine);
int broadcast_message(string message, int sender_id);
int broadcast_message(int num, int sender_id);
void end_connection(int id);
void handle_client(int client_socket, int id);

int main()
{
	mkdir("./server_dir", 0777);
	DIR* d = opendir("./server_dir");
	
	struct dirent* dir;
	if (d) 
	{
		for (int i = 0; ((dir = readdir(d)) != NULL);i++)
		{
			if (!strcmp(".", dir->d_name) || !strcmp("..", dir->d_name))
				continue;
			memset(user[i].account, '\0', 10);
			strcpy(user[i].account, dir->d_name);
			cout << user[i].account << endl;
			char file_path[40] = "./server_dir/";
			strcat(file_path, dir->d_name);
			strcat(file_path, "/password");
			FILE* fp = fopen(file_path, "r");
			if (fp == NULL)
			{
				exit(1);
			}
			fread(user[i].password, sizeof(char), 10, fp);
			fclose(fp);
			cout << user[i].password << endl;
			number_of_user++;
		}
		closedir(d);
		
	}
	for (int i = 0; i < number_of_user; i++)
	{
		user[i].status = 0;
	}
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

	struct sockaddr_in client;
	int client_socket;
	unsigned int len = sizeof(sockaddr_in);

	cout << colors[NUM_COLORS - 1] << "\n\t  ====== This is the server ======   " << endl << def_col;
	//open or make a server_dir for record information about client
	
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

string color(int code)
{
	return colors[code % NUM_COLORS];
}

// Set name of client
void set_name(int id, char name[])
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].id == id)
		{
			clients[i].name = string(name);
		}
	}
}

// For synchronisation of cout statements
void shared_print(string str, bool endLine = true)
{
	lock_guard<mutex> guard(cout_mtx);
	cout << str;
	if (endLine)
		cout << endl;
}

// Broadcast message to all clients except the sender
int broadcast_message(string message, int sender_id)
{
	char temp[MAX_LEN];
	strcpy(temp, message.c_str());
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].id != sender_id)
		{
			send(clients[i].socket, temp, sizeof(temp), 0);
		}
	}
}

// Broadcast a number to all clients except the sender
int broadcast_message(int num, int sender_id)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].id != sender_id)
		{
			send(clients[i].socket, &num, sizeof(num), 0);
		}
	}
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
	char name[MAX_LEN], str[MAX_LEN], password[MAX_LEN], account[MAX_LEN], command[MAX_LEN], account_of_friend[MAX_LEN];
	recv(client_socket, command, sizeof(command), 0);
	cout << "sombody want";
	while (1)
	{
		if (!strcmp(command, "1"))
		{
			cout << "sign in" << endl;
			while (1)
			{
				recv(client_socket, account, sizeof(account), 0);
				cout << "we receive the account " << account << endl;
				for (int i = 0; i <= number_of_user; i++)
				{
					cout << "finding account...." << endl;
					if (!strcmp(account, user[i].account))
					{
						char found_account[2] = "1";
						cout << "we found the account " << account << endl;
						send(client_socket, found_account, sizeof(found_account), 0);
						send(client_socket, user[i].password, sizeof(user[i].password), 0);
						user[i].status = 1;
						break;
					}
					else if (i == number_of_user)
					{
						char found_account[2] = "0";
						cout << "account not found" << endl;
						send(client_socket, found_account, sizeof(found_account), 0);
						goto sign_up;
					}
				}
			}
			

		}
		else
		{
			sign_up:
			{
				cout << "sign up" << endl;
				recv(client_socket, account, sizeof(account), 0);
				recv(client_socket, password, sizeof(account), 0);
				memset(user[number_of_user].account,'\0',MAX_LEN);
				memset(user[number_of_user].password, '\0', MAX_LEN);
				strcpy(user[number_of_user].account, account);
				strcpy(user[number_of_user].password, password);
				number_of_user++;
				user[number_of_user].status = 1;
				break;
			}
				

		}
	}
	
	while (1)
	{
		recv(client_socket, command, sizeof(command), 0);
		if (!strcmp(command, "1"))
		{
			char no_friend[2];
			recv(client_socket, no_friend, sizeof(no_friend), 0);
			if (no_friend[0] == 0)
				goto add_friend;
			break;
		}
		else
		{
			add_friend:
			{
				recv(client_socket, name, sizeof(name), 0);
				for (int i = 0; i < number_of_user; i++)
				{
					if (!strcmp(account_of_friend, user[i].account))
					{
						char found_account[2] = "1";
						send(client_socket, found_account, sizeof(found_account), 0);
					}
					else if (i == 19)
					{
						char found_account[2] = "0";
						send(client_socket, found_account, sizeof(found_account), 0);
					}
				}
			}
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