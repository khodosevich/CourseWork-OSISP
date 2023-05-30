#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>


using namespace std;

const string IP = "127.0.0.1";

enum Status {
    FREE,
    BUSY
};

struct Port {
    int number;
    Status status;
};

vector<Port> scanAllPorts(int startPort, int endPort);
void printHelp();
void printTable(vector<Port> ports);
bool scanOnePort(int portToCheck);
void writeToFile(const vector<Port>& ports, const string& filename);


int main(int argc, char *argv[]) {
    int opt;
    int startPort = 1;
    int endPort = 65535;

    bool helpFlag = false;
    bool tableFlag = false;
    int portToCheck = -1;
    bool writeFile = false;
    bool scaningOnePort = false;
    string filename;


    while ((opt = getopt(argc, argv, "hs:e:t:p:Tf:")) != -1) {
        switch (opt) {
            case 'h':
                helpFlag = true;
                break;
            case 's':
                startPort = atoi(optarg);
                break;
            case 'e':
                endPort = atoi(optarg);
                break;
            case 'f':
                writeFile = true;
                filename = optarg;
                break;
            case 'T':
                tableFlag = true;
                break;
            case 'p':
                scaningOnePort = true;
                portToCheck = atoi(optarg);
                cout << "\nport to check: " << portToCheck << endl;
                break;
            default:
                cerr << "Unknown option: " << char(opt) << endl;
                return 1;
        }
    }


    if (helpFlag) {
        printHelp();
        return 0;
    }

    if(scaningOnePort){
         if(portToCheck < 0 || portToCheck > 65535){
           cout << "this port is out of range ( 0 - 65535)" << endl;
        }else{
                Port p;
                vector<Port> port;
            if(scanOnePort(portToCheck)){
                p.number = portToCheck;
                p.status = FREE;
                port.push_back(p);

                printTable(port);
            }else{
                p.number = portToCheck;
                p.status = BUSY;
                port.push_back(p);

                printTable(port);
            }
            if(writeFile){
                    cout << "writing to file...." << endl;
                    writeToFile(port,filename);
            }
        }
    }


    vector<Port> ports = scanAllPorts(startPort, endPort );
    
    if (tableFlag) {
        printTable(ports);
    } else {
        for (auto port : ports) {
            string status1 = port.status == FREE ? "free" : "busy";
            cout << "IP: " << IP << "\tPort: " << port.number << "\tStatus: " << status1 << endl;
        }
    }

    if(writeFile){
        cout << "writing to file...." << endl;
        writeToFile(ports,filename);
    }

    return 0;
}

bool scanOnePort(int portToCheck){

    int port = portToCheck;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    addr.sin_port = htons(port);

    int status = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));

    if (status == 0){
        close(sockfd);
        return 1;
    } else {
        return 0;
    }

}

vector<Port> scanAllPorts(int startPort, int endPort) {
    vector <Port> ports;


    int sock;
    struct sockaddr_in server;

    for (int port = startPort; port <= endPort; port++) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            cout << "Could not create socket" << endl;
            continue;
        }
        server.sin_addr.s_addr = inet_addr(IP.c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
            close(sock);
            continue;
        }

        Port p;
        p.number = port;
        p.status = FREE;
        ports.push_back(p);

        close(sock);
    }

    return ports;
}

void writeToFile(const vector<Port>& ports, const string& filename) {
    FILE* file = fopen(filename.c_str(), "a");
    if (file == nullptr) {
        cerr << "Failed to open file " << filename << " for writing\n";
        return;
    }

    fprintf(file,"\nports:\n");

    for (int i = 0; i < ports.size(); i++)
    {
        if (ports[i].status == FREE) {
            fprintf(file, "Port %d is free\n", ports[i].number);
        }else{
            fprintf(file, "Port %d is busy\n", ports[i].number);
        }
    }

    fclose(file);
}
 
void printHelp() {
    cout << "Usage: portscanner [OPTIONS]\n\n"
         << "Options:\n"
         << " -h\t\t\tShow this help message\n"
         << " -s <port>\t\tStart scanning from port (default: 1)\n"
         << " -e <port>\t\tEnd scanning at port (default: 65535)\n"
         << " -p <port>\t\tScanning one port\n"
         << " -T\t\t\tPrint results as a table\n"
        << " -f\t\t\twrite to file\n"
         << endl;
}


void printTable(vector<Port> ports) {
    cout << "+----------------------+-------+-------+" << endl;
    cout << "|      IP address      | Port  | Status|" << endl;
    cout << "+----------------------+-------+-------+" << endl;
    for (auto port : ports) {
        printf("|\t%-15s| %-5d | %-5s |\n", IP.c_str() ,port.number, port.status == FREE ? "free" : "busy");
    }
    cout << "+----------------------+-------+-------+" << endl;

    
}