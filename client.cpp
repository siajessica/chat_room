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
#define MAX_LEN 200
#define NUM_COLORS 6

using namespace std;

bool exit_flag = false;
thread t_send, t_recv;
int client_socket;
string def_col = "\033[0m";
string colors[] = { "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m" };

char friend_list[20][10];
int number_of_friend = 0;


void catch_ctrl_c(int signal);
string color(int code);
int eraseText(int cnt);
void send_message(int client_socket);
void recv_message(int client_socket);
void set_dir(char account[], char complete[]);
void file_search(char client_dir[]);
int main()
{
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
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
	//sign in or sign up
	char found_account[2], password_c[MAX_LEN], password_s[MAX_LEN], account[MAX_LEN],command[MAX_LEN],client_dir[20], account_of_friend[MAX_LEN];
	cout << colors[NUM_COLORS - 1] << "\n\t  1.Sign in" << endl << def_col;
	cout << colors[NUM_COLORS - 1] << "\n\t  2.Sign up" << endl << def_col;
	while (1)
	{
		cin.getline(command, MAX_LEN);
		send(client_socket, command, sizeof(command), 0);
		if (!strcmp(command, "1"))
		{
			cout << colors[NUM_COLORS - 1] << "\n\t  Plaese enter your account:" << endl << def_col;
			cin.getline(account, MAX_LEN);
			send(client_socket, account, sizeof(account), 0);
			recv(client_socket, found_account, sizeof(found_account), 0);
			if(found_account[0] == '0')
			{
				cout << colors[NUM_COLORS - 1] << "\n\t  Account not found. Please sign up one" << endl << def_col;
				goto sign_up;
			}
			recv(client_socket, password_s, sizeof(password_s), 0);
			cout << colors[NUM_COLORS - 1] << "\n\t  Plaese enter your password:" << endl << def_col;
			cin.getline(password_c, MAX_LEN);
			while(strcmp(password_c, password_s))
			{
				cout << colors[NUM_COLORS - 1] << "\n\t  Plaese try again:" << endl << def_col;
				cin.getline(password_c, MAX_LEN);
			}
			
			break;
		}
		else if (!strcmp(command, "2"))
		{
			sign_up:
			{
				cout << colors[NUM_COLORS - 1] << "\n\t  Plaese enter your account:" << endl << def_col;
				cin.getline(account, MAX_LEN);
				send(client_socket, account, sizeof(account), 0);
				cout << colors[NUM_COLORS - 1] << "\n\t  Plaese enter your password:" << endl << def_col;
				cin.getline(password_c, MAX_LEN);
				send(client_socket, password_c, sizeof(password_c), 0);
				break;
			}
		}
		else
		{
			cout << colors[NUM_COLORS - 1] << "\n\t  Command not found. Try again." << endl << def_col;
		}
		
	}
	set_dir(account,client_dir);
	char file_path[40];
	memset(file_path, '\0', 40);
	strcat(file_path, client_dir);
	strcat(file_path, "/password");

	FILE* fp = fopen(file_path, "a+");
	if (fp == NULL)
	{
		exit(1);
	}
	cout << password_c << endl;
	fwrite(password_c,sizeof(char),sizeof(password_c),fp);
	fclose(fp);
	cout << colors[NUM_COLORS - 1] << "\n\t  ========= Welcom back, " << account <<  "========" << endl << def_col;
	
	while (1)
	{
		cout << colors[NUM_COLORS - 1] << "\n\t  1.Friend list" << endl << def_col;
		cout << colors[NUM_COLORS - 1] << "\n\t  2.Add new friend" << endl << def_col;
		cin.getline(command, MAX_LEN);
		send(client_socket, command, sizeof(command), 0);
		if (!strcmp(command, "1"))
		{
			file_search(client_dir);
			cout << number_of_friend << endl;
			char no_friend[2] = "1";
			if (number_of_friend == 0)
			{
				no_friend[0] = '0';
				send(client_socket, no_friend, sizeof(no_friend), 0);
				cout << colors[NUM_COLORS - 1] << "\n\t  Let's add some new friend." << endl << def_col;
				goto add_friend;
			}
			char choose_a_friend[MAX_LEN];
			int friend_code=0;
			cin.getline(choose_a_friend, MAX_LEN);
			for (int i = 0; i < strlen(choose_a_friend); i++)
			{
				friend_code *= 10;
				friend_code += (choose_a_friend[i] - 48);
			}
			while (friend_code < 1 || friend_code >20)
			{
				cout << colors[NUM_COLORS - 1] << "\n\t  Somthing wrong. Please enter again." << endl << def_col;
				cin.getline(choose_a_friend, MAX_LEN);
				for (int i = 0; i < strlen(choose_a_friend); i++)
				{
					friend_code *= 10;
					friend_code += (choose_a_friend[i] - 48);
				}
			}
			cout << colors[NUM_COLORS - 1] << "\n\t  1.Start to chat" << endl << def_col;
			cout << colors[NUM_COLORS - 1] << "\n\t  2.Remove this friend" << endl << def_col;
			cin.getline(command, MAX_LEN);
			if (!strcmp(command, "1"))
			{
				//start to chat with friend
				break;
			}
			else if (!strcmp(command, "2"))
			{
				char path[30]="./server_dir/";
				strcat(path, client_dir);
				strcat(path, "/");
				strcat(path, friend_list[friend_code - 1]);
				rmdir(path);
				memset(friend_list[friend_code - 1], '\0', 10);
			}
			break;
		}
		else if(!strcmp(command, "2"))
		{
			add_friend:
			{
				cout << colors[NUM_COLORS - 1] << "\n\t  Please enter the account of your new friend:" << endl << def_col;
				cin.getline(account_of_friend, MAX_LEN);
				send(client_socket, account_of_friend, sizeof(account_of_friend), 0);
				recv(client_socket, found_account, sizeof(found_account), 0);
				if (found_account[0] != '0')
				{
					cout << colors[NUM_COLORS - 1] << "\n\t  Account not found. Your friend haven't had an account yet" << endl << def_col;
				}
				else
				{
					cout << colors[NUM_COLORS - 1] << "\n\t  You can chat with " << account_of_friend << " now!" << endl << def_col;
					cout << colors[NUM_COLORS - 1] << "\n\t  1.Start to chat" << endl << def_col;
					cout << colors[NUM_COLORS - 1] << "\n\t  2.Chat later" << endl << def_col;
					cin.getline(command, MAX_LEN);
					if (!strcmp(command, "1"))
					{
						break;
					}
				}
			}
			
		}
		else
		{
			cout << colors[NUM_COLORS - 1] << "\n\t  Command not found. Try again." << endl << def_col;
		}
	}
	//start to chat

	thread t1(send_message, client_socket);
	thread t2(recv_message, client_socket);

	t_send = move(t1);
	t_recv = move(t2);

	if (t_send.joinable())
		t_send.join();
	if (t_recv.joinable())
		t_recv.join();

	return 0;
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

string color(int code)
{
	return colors[code % NUM_COLORS];
}

// Erase text from terminal
int eraseText(int cnt)
{
	char back_space = 8;
	for (int i = 0; i < cnt; i++)
	{
		cout << back_space;
	}
}

// Send message to everyone
void send_message(int client_socket)
{
	while (1)
	{
		cout << colors[1] << "You : " << def_col;
		char str[MAX_LEN];
		cin.getline(str, MAX_LEN);
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
		recv(client_socket, &color_code, sizeof(color_code), 0);
		recv(client_socket, str, sizeof(str), 0);
		eraseText(6);
		if (strcmp(name, "#NULL") != 0)
			cout << color(color_code) << name << " : " << def_col << str << endl;
		else
			cout << color(color_code) << str << endl;
		cout << colors[1] << "You : " << def_col;
		fflush(stdout);
	}
}

void set_dir(char account[],char complete[])
{
	char head[] = "./server_dir/";
	memset(complete, '\0', 20);
	strcat(complete, head);
	strcat(complete, account);
	mkdir(complete, 0777);
}

void file_search(char client_dir[])
{
	DIR* d = opendir(client_dir);
	struct dirent* dir;
	if (d) 
	{
		for (number_of_friend = 0; ((dir = readdir(d)) != NULL); number_of_friend++)
		{
			if(!strcmp(dir->d_name, ".") &&!strcmp(dir->d_name, ".."))
			{
				memset(friend_list[number_of_friend],'\0',10);
				strcpy(friend_list[number_of_friend], dir->d_name);
				cout << number_of_friend +1 << "." << friend_list[number_of_friend] << endl << def_col;
			}
			else
			{
				number_of_friend--;
			}
		}
		closedir(d);
	}
}