# chat_room
simple chat room code in C++ socket

## two ways to set a database
1. file
  easier
2. sqlite
  some of my friend use this
  
## C 
## install CGI environment in Ubuntu
ref : https://blog.csdn.net/prince1394/article/details/80295098?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-1.no_search_link&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-1.no_search_link&utm_relevant_index=2

1. install Apache2

   enter 
   ###### sudo apt install apache2
 
2.set up CGI

  under /etc/apache2/conf-available/
  
  ###### sudo vim serve-cgi-bin.conf
  
  modify
  
  ###### ScriptAlias /cgi-bin/ /usr/lib/cgi-bin/
  ###### <Directory "/usr/lib/cgi-bin">
  ######  AllowOverride None
  ###### Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
  ###### Require all granted
  ###### </Directory>
  
  to 
  
  ###### ScriptAlias /cgi-bin/ /var/www/html/cgi-bin/
  ###### <Directory "/var/www/html/cgi-bin/">
  ######  AllowOverride None
  ###### Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
  ###### Require all granted
  ###### AddHandler cgi-script cgi
  ###### </Directory>

  under /etc/apache2/mods-available/
  
  ###### sudo vim cgid.load
  
  add 
  
  ###### AddHandler cgi-script .cgi .pl .py .sh
  
  To connect file, enter
  
  ###### sudo ln -s /etc/apache2/mods-available/cgid.load /etc/apache2/mods-enabled/cgid.load
  
 To Restart apache2,enter
 
 ###### sudo /etc/init.d/apache2 restart
 
 Make a new file
 
 ###### sudo mkdir /var/www/html/cgi-bin/
  
3. create a CGI program

   under    /var/www/html/cgi-bin/ 
   
   ###### sudo vim helloworld.c
   
   type 
   #include <stdio.h>
   int main()
    {
        printf("Content-Type: text/html\n\n");
        printf("Hello World!\n");
        return 0;
    }
    compile 
    ###### sudo gcc /var/www/html/cgi-bin/helloworld.c -o /var/www/html/cgi-bin/helloworld.cgi
    change CGI priority
    ###### sudo chmod 755 /var/www/html/cgi-bin/helloworld.cgi
    
 4. test
    ###### http://localhost/cgi-bin/helloworld.cgi
