#include < iostream > 
#include < WS2tcpip.h > 
#include < Windows.h > 
#include < conio.h > 
#pragma comment(lib, "ws2_32.lib")

#define dPort "666"
/*
* Macros for display count connected users
*/
#define PRINTNUSERS if (ClientCount)\
  printf("%d user on-line\n",ClientCount);\
			    else printf("No User on line\n");

SOCKET	sConnect,
sListen;
SOCKET* pSConnections;

int ClientCount = 0;

void SendMessageToClient(int ID){
	char* pCBuffer;
	while (1){
		pCBuffer = new char[1024];
		memset(pCBuffer, 0, sizeof(pCBuffer));
		int iRecvSize = recv(pSConnections[ID], pCBuffer, 1024, NULL);
		if (iRecvSize){
			//printf("%s", pCBuffer); //Display message on server... this is no good for good boys :-)
			for (register int i = 0; i <= ClientCount; i++){
				if (i != ID){
					send(pSConnections[i], pCBuffer, strlen(pCBuffer), NULL);
				}
			}
		}
		if (iRecvSize <= 0){
			break;
		}
		Sleep(75);
	}
	closesocket(pSConnections[ID]);
	ClientCount--;
	printf("Client disconnect...\n"); //Just log for server
	PRINTNUSERS //Just log for server
		free(pCBuffer);
}

int main(){
	/*
	* Initial WSADATA structure contains information about the Windows Sockets.
	*/
	//About WSADATA
	//https://msdn.microsoft.com/ru-ru/library/37k8e5x7.aspx
	WSAData wsaDATA;
	int iWSA = WSAStartup(MAKEWORD(2, 2), &wsaDATA);
	if (iWSA != 0){
		perror("WSAStartup");
		return 1;
	}

	//About
	//http://www.opennet.ru/man.shtml?category=3&russian=0&topic=getaddrinfo
	struct addrinfo sAdrInfo_1;
	struct addrinfo* psAdrInfo_2;

	/*
	* Dynamic memory allocation for new socket.
	*/
	//About reinterpret_cast<>()
	//https://msdn.microsoft.com/ru-ru/library/e0w9f63b.aspx
	pSConnections = reinterpret_cast<SOCKET*>(calloc(64, sizeof(SOCKET)));
	ZeroMemory(&sAdrInfo_1, sizeof(sAdrInfo_1));
	sAdrInfo_1.ai_family = AF_INET;
	sAdrInfo_1.ai_flags = AI_PASSIVE;
	sAdrInfo_1.ai_protocol = IPPROTO_TCP;
	getaddrinfo(NULL, dPort, &sAdrInfo_1, &psAdrInfo_2);
	/*
	* socket()
	*/
	sListen = socket(psAdrInfo_2->ai_family, psAdrInfo_2->ai_socktype, psAdrInfo_2->ai_protocol);

	/*
	* bind()
	*/
	bind(sListen, psAdrInfo_2->ai_addr, psAdrInfo_2->ai_addrlen);
	/*
	* listen()
	*/
	listen(sListen, SOMAXCONN);
	freeaddrinfo(psAdrInfo_2);
	/*
	* If server start it`s ok - put message about this to display
	*/
	printf("Start server...\n");
	PRINTNUSERS //Just log for server
		char m_connect[] = "Your connect to server...\n";
	while (1){
		/*
		* accept()
		*/
		if (sConnect = accept(sListen, NULL, NULL)){
			printf("Client connect...\n"); //Just log for server
			pSConnections[ClientCount] = sConnect;
			/*
			* Send message to client about you`r connected to server
			*/
			send(pSConnections[ClientCount], m_connect, strlen(m_connect), NULL);
			ClientCount++;
			/*
			* Create new Thread
			*/
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendMessageToClient,
				reinterpret_cast<LPVOID>(ClientCount - 1), NULL, NULL);
		}
		PRINTNUSERS //Just log for server
			Sleep(75);
	}

	/*
	* Free dynamic memory.
	*/
	//About free
	//http://cppstudio.com/post/850/
	free(pSConnections);
	closesocket(sListen);
	closesocket(sConnect);
	pSConnections = NULL;
	return 0;
}