#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
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

using namespace std;

bool exit_flag = false;
thread t_send, t_recv;
int client_socket;

void catch_ctrl_c(int signal);
void print_command();

int main(){
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)v{
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

    string command, username, password, target_user;
	int found_account = 0;
	int ret = 0;

    while(1){
        cin.getline(command, MAX_LEN);
        send(client_socket, command, sizeof(command), 0);
        if(command == "1"){
            cout << "Please enter your username: ";
            cin.getline(username, MAX_LEN);
			send(client_socket, username, sizeof(username), 0);
			cout << "Please enter your password: ";
            cin.getline(password, MAX_LEN);
			send(client_socket, password, sizeof(password), 0);
			recv(client_socket, found_account, sizeof(found_account), 0);
			if(!found_account){
				cout << "WRONG USERNAME/PASSWORD. PLEASE TRY AGAIN." << endl;
				cout << "PLEASE SIGN UP IF YOU DON'T HAVE AN ACCOUNT." << endl;
			} else{
				cout << "LOGIN SUCCESSFUL" << endl;
				print_command();
			}
        }
		else if(command == "2"){
			cout << "Please enter a username: ";
			cin.getline(username, MAX_LEN);
			send(client_socket, username, sizeof(username), 0);
			cout << "Please enter your password: ";
            cin.getline(password, MAX_LEN);
			send(client_socket, password, sizeof(password), 0);
			recv(client_socket, found_account, sizeof(found_account), 0);
			if(!found_account){
				cout << "USERNAME HAS BEEN TAKEN." << endl;
				cout << "PLEASE TRY AGAIN." << endl;
			} else{
				cout << "SIGN UP SUCCESSFUL" << endl;
				print_command();
			}
		}
		else if(command == "3" && found_account){
			send(client_socket, username, sizeof(username), 0);
			list_friends(username);
		}
		else if(command == "4" && found_account){
			cout << "Please enter target's username: ";
			cin.getline(target_user, MAX_LEN);
			send(client_socket, target_user, sizeof(target_user), 0);
			recv(client_socket, ret, sizeof(ret), 0);

			if(ret == 0) cout << "User not found." << endl;
			else if(ret == 1) cout << "ERROR CANNOT ADD FRIEND: You have been blocked by the user.\n";
			else if(ret == 2) cout << "ERROR BLOCKED: Please unblock the user before adding as friend.\n";
			else if(ret == 3) cout << "You are already friends." << endl;
			else if(ret == 5) cout << "Friend Added." << endl;
		}
		else if(command == "5" && found_account){
			cout << "Please enter target's username: ";
			cin.getline(target_user, MAX_LEN);
			send(client_socket, target_user, sizeof(target_user), 0);
			recv(client_socket, ret, sizeof(ret), 0);

			if(ret == 0) cout << "User not found." << endl;
			else if(ret == 1) cout << "User already blocked.\n";
			else if(ret == 2) "User blocked successfully." << endl;
		}
		else if(command == "6" && found_account){
			cout << "Please enter friend's username: ";
			cin.getline(target_user, MAX_LEN);
			send(client_socket, target_user, sizeof(target_user), 0);
			recv(client_socket, ret, sizeof(ret), 0);
		}
    }
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