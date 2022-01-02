# chat_room
simple chat room code in C++ socket


log in and some add/remove friend
since I have no idea about mySQL, I think use file and directory will be easier.

1.accounting system :
  
  make a directory when someone sign up an account, and a file named password with content which is the password that user input
  
  or 
  
  make a file named 【user_list】 and write down the user's name and the password. (simpler)
  
2.add friend

  make a file under the user's directory and name the directory by the name of user's friend.
  
  or 
  
  make a file named 【user_name】_【name of user's friend】
  
  write the chatting history into the file.
  
3.remove friend

  remove the file.

4.one-to-one chat

  record the condition of user
  
  a. chatting
      
      in this condition, other user can send message, but main_user can not reply. That's, can write the file, but cannot connect with socket.
      
  b. available
  
