#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <mutex>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include <sys/stat.h>
#include <dirent.h> 
#include "database.h"
#include "relationship.h"
#include "login_interface.h"
#include "message.h"

#define MAX_LEN 1280
#define NUM_COLORS 6

using namespace std;

bool exit_flag = false;
thread t_send, t_recv;
int client_socket;
string def_col = "\033[0m";
string colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};

void catch_ctrl_c(int signal);
string color(int code);
void print_command();
void send_message(int client_socket, string username, int chat_id);
void recv_message(int client_socket);

int main(){
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = htons(10000); // Port no. of server
	client.sin_addr.s_addr = INADDR_ANY;
	//client.sin_addr.s_addr=inet_addr("127.0.0.1"); // Provide IP address of server
	bzero(&client.sin_zero, 0);

	if ((connect(client_socket, (struct sockaddr*)&client, sizeof(struct sockaddr_in))) == -1)
	{
		perror("connect: ");
		exit(-1);
	}

	signal(SIGINT, catch_ctrl_c);

    //LOGIN INTERFACE
    cout << "Welcome to the client." << endl;
    cout << "1. SIGN IN" << endl;
    cout << "2. SIGN UP" << endl;

    string command_s, username_s, password_s, target_user_s;
    char command[20], username[20], password[20], target_user[20];
	int found_account = -1;
	int chat_id = 0;
    while(1){
        cin.getline(command, MAX_LEN);
        send(client_socket, command, sizeof(command), 0);

        if((!strcmp(command, "1")) && found_account == -1) {
            cout << "Please enter your username: ";
            cin.getline(username, MAX_LEN);
			send(client_socket, username, sizeof(username), 0);
			cout << "Please enter your password: ";
            cin.getline(password, MAX_LEN);
			send(client_socket, password, sizeof(password), 0);
			recv(client_socket, command, sizeof(command), 0);
			found_account = atoi(command);
			if(found_account) {
				cout << "WRONG USERNAME/PASSWORD. PLEASE TRY AGAIN." << endl;
				cout << "PLEASE SIGN UP IF YOU DON'T HAVE AN ACCOUNT." << endl;
			} else{
				cout << "LOGIN SUCCESSFUL" << endl;
				username_s = username;
				print_command();
			}
        }
		else if(command[0] == '2'  && found_account == -1){
			cout << "Please enter a username: ";
			cin.getline(username, MAX_LEN);
			send(client_socket, username, sizeof(username), 0);
			cout << "Please enter your password: ";
            cin.getline(password, MAX_LEN);
			send(client_socket, password, sizeof(password), 0);
			recv(client_socket, command, sizeof(command), 0);
			found_account = atoi(command);
			if(found_account){
				cout << "USERNAME HAS BEEN TAKEN." << endl;
				cout << "PLEASE TRY AGAIN." << endl;
			} else{
				cout << "SIGN UP SUCCESSFUL" << endl;
				print_command();
			}
		}
		else if(command[0] == '3' && found_account != -1){
			send(client_socket, username, sizeof(username), 0);
			list_friends(username);
		}
		else if(command[0] == '4' && found_account){
			cout << "Please enter target's username: ";
			cin.getline(target_user, MAX_LEN);
			send(client_socket, target_user, sizeof(target_user), 0);
			recv(client_socket, command, sizeof(command), 0);

			if(command[0] == '5') cout << "Friend Added." << endl;
			else cout << "Add friend error." << endl;
		}
		else if(command[0] == '5' && found_account){
			cout << "Please enter target's username: ";
			cin.getline(target_user, MAX_LEN);
			send(client_socket, target_user, sizeof(target_user), 0);
			recv(client_socket, command, sizeof(command), 0);

			if(command[0] == '0') cout << "User not found." << endl;
			else if(command[0] == '1') cout << "User removed.\n";
		}
		else if(command[0] == '6' && found_account){
			cout << "Please enter friend's username: ";
			cin.getline(target_user, MAX_LEN);
			send(client_socket, target_user, sizeof(target_user), 0);
			//recv(client_socket, ret, sizeof(ret), 0);
			chat_id = get_chatid(username_s, target_user_s);
			message_history(chat_id);
		}
		else
			cout << "Command not found.\n";
    }

	thread t1(send_message, client_socket, username_s, chat_id);
	thread t2(recv_message, client_socket);

	t_send = move(t1);
	t_recv = move(t2);

	if (t_send.joinable())
		t_send.join();
	if (t_recv.joinable())
		t_recv.join();

	return 0;
}

string color(int code){
	return colors[code%NUM_COLORS];
}

// Handler for "Ctrl + C"
void catch_ctrl_c(int signal)
{
	char str[MAX_LEN] = "#exit";
	send(client_socket, str, sizeof(str), 0);
	exit_flag = true;
	t_send.detach();
	t_recv.detach();
	close(client_socket);
	exit(signal);
}

void print_command(){
	cout << "3. LIST FRIEND" << endl;
	cout << "4. ADD FRIEND" << endl;
	cout << "5. REMOVE FRIEND" << endl;
	cout << "6. CHAT FRIEND" << endl;
}


// Send message to everyone
void send_message(int client_socket, string username, int chat_id)
{
	
	while (1)
	{
		cout << "You : " ;
		char str[MAX_LEN];
		string body;
		body.assign(str);
		cin.getline(str, MAX_LEN);
		record_message(username, chat_id, body);
		send(client_socket, str, sizeof(str), 0);
		if (strcmp(str, "#exit") == 0)
		{
			exit_flag = true;
			t_recv.detach();
			close(client_socket);
			return;
		}
	}
}

// Receive message
void recv_message(int client_socket)
{
	while (1)
	{
		if (exit_flag)
			return;
		char name[MAX_LEN], str[MAX_LEN];
		int color_code;
		int bytes_received = recv(client_socket, name, sizeof(name), 0);
		if (bytes_received <= 0)
			continue;
		//recv(client_socket, &color_code, sizeof(color_code), 0);
		recv(client_socket, str, sizeof(str), 0);
		//eraseText(6);
		if (strcmp(name, "#NULL") != 0)
			cout << color(color_code) << name << " : " << def_col << str << endl;
		else
			cout << color(color_code) << str << endl;
		cout << "You : " ;
		fflush(stdout);
	}
}
