#include <iostream>
#include <string>

using namespace std;

#ifndef _RELATIONSHIP_H
#define _RELATIONSHIP_H

int list_friends(string username);
int list_block(string username);
int add_friend(string user1, string user2);
int add_block(string user1, string user2);
int remove_friend(string user1, string user2);
int unblock(string user1, string user2);

#endif
