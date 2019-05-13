# chat
  Simple chat application written in C++, using Windows Sockets.
  Run in MS Visual Studio 2017.
  
#### Commands:

	"-login":
		Initate the login process. Next input will be saved as the username.
	"-logout":
		Log out current user.
	"-exit":
		Close application.
	"-mock n":
		Mock messages saved in lorem.txt file n times.
	"-find str":
		Find str as a substring in currently available messages. Print the amount of occurances found and the time passed.
	"-connect prt":
		Connect to a chat opened on port "prt". If no chat opened, start the server on port "prt".
	"-save":
		Save the latest message to clipboard.
	"-load":
		Push the string saved in clipboard as message.
		
#### Learned:
 - Using Winsock to communicate between PCs via internet connection.
 - Communication between processes via Windows Clipboard.
 - Creating threads.
 - Multithreaded tasks.
