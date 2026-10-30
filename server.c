#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#define PORT 8080
#define MAX 100

struct Aircraft
{
    int id;
    char name[50];
    char squadron[50];
    char status[20];
};

struct Aircraft fleet[MAX];
int count=0;

void loadData()
{
    FILE *fp=fopen("aircraft.txt","r");

    if(fp==NULL)
        return;

    while(fscanf(fp,"%d %s %s %s",
          &fleet[count].id,
          fleet[count].name,
          fleet[count].squadron,
          fleet[count].status)==4)
    {
        count++;
    }

    fclose(fp);
}

void saveData()
{
    FILE *fp=fopen("aircraft.txt","w");

    for(int i=0;i<count;i++)
    {
        fprintf(fp,"%d %s %s %s\n",
        fleet[i].id,
        fleet[i].name,
        fleet[i].squadron,
        fleet[i].status);
    }

    fclose(fp);
}

void addAircraft(int id,char name[],char squadron[],char status[])
{
    fleet[count].id=id;

    strcpy(fleet[count].name,name);
    strcpy(fleet[count].squadron,squadron);
    strcpy(fleet[count].status,status);

    count++;

    saveData();
}

void showPage(SOCKET client)
{
    char buffer[2048];

    strcpy(buffer,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type:text/html\r\n\r\n");

    send(client,buffer,strlen(buffer),0);

    FILE *fp=fopen("index.html","r");

    while(fgets(buffer,sizeof(buffer),fp))
    {
        send(client,buffer,strlen(buffer),0);
    }

    fclose(fp);

    sprintf(buffer,
    "<script>"
    "document.getElementById('total').innerHTML='%d';"
    "</script>",
    count);

    send(client,buffer,strlen(buffer),0);

    send(client,
    "<table class='fleet'>"
    "<tr>"
    "<th>ID</th>"
    "<th>Aircraft</th>"
    "<th>Squadron</th>"
    "<th>Status</th>"
    "</tr>",
    108,0);

    for(int i=0;i<count;i++)
    {
        sprintf(buffer,
        "<tr>"
        "<td>%d</td>"
        "<td>%s</td>"
        "<td>%s</td>"
        "<td>%s</td>"
        "</tr>",
        fleet[i].id,
        fleet[i].name,
        fleet[i].squadron,
        fleet[i].status);

        send(client,buffer,strlen(buffer),0);
    }

    strcpy(buffer,"</table></body></html>");

    send(client,buffer,strlen(buffer),0);
}

int main()
{
    WSADATA wsa;

    SOCKET server,client;

    struct sockaddr_in address;

    char request[4096];

    WSAStartup(MAKEWORD(2,2),&wsa);

    loadData();

    server=socket(AF_INET,SOCK_STREAM,0);

    address.sin_family=AF_INET;
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=htons(PORT);

    bind(server,
    (struct sockaddr*)&address,
    sizeof(address));

    listen(server,5);

    printf("IAF Aircraft Management System Running\n");
    printf("http://localhost:8080\n");

    while(1)
    {
        client=accept(server,NULL,NULL);

        recv(client,request,sizeof(request),0);

        showPage(client);

        closesocket(client);
    }

    closesocket(server);

    WSACleanup();

    return 0;
}