#include "zmq.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <map>
#include <signal.h>
#include <fstream>
#include <algorithm>
#include <thread>

using namespace std;

string adr, adrChild;
zmq::context_t context(1);
zmq::socket_t mainSocket(context, ZMQ_REQ);
zmq::context_t contextChild(1);
zmq::socket_t childSocket(contextChild, ZMQ_REP);
vector<int> ChildrenId;
std::map<string, int> dict;
int idThisNode, childNodeId;

void sendMessage(const string& messageString, zmq::socket_t& socket) {
    zmq::message_t messageBack(messageString.size());
    memcpy(messageBack.data(), messageString.c_str(), messageString.size());

    if (!socket.send(messageBack)) {
        cerr << "Error: can't send message from node with pid " << getpid() << endl;
    }
}

void mapAccess(string receivedMessage, string idProcString, int idProc) {
    cout << "Function started in thread: " << std::this_thread::get_id() << endl;
    sleep(1);

    int value;
    bool isSpace = false;
    string key, valueStr, returnMessage;
    vector<int> answer;

    for (int i = 6 + idProcString.size(); i < receivedMessage.size(); ++i) {
        if (receivedMessage[i] != ' ' && !isSpace) {
            key += receivedMessage[i];
        } else {
            isSpace = true;
            valueStr += receivedMessage[i];
        }
    }

    if (isSpace) {
        value = stoi(valueStr);

        dict[key] = value;
        returnMessage += idProcString;

    } else {
        if (dict[key]){
            returnMessage += to_string(dict[key]);
        } else {
            returnMessage = "'" + key + "'" + " not found";
            
        }
    }

    cout << endl << "OK: " << returnMessage << endl;
    cout << "Function completed in thread: " << std::this_thread::get_id() << endl;
}

void funcCreate(string receivedMessage) {
    bool isSpace = false;
    int idNewProc, parentIdNewProc;
    string idNewProcString, parentIdNewProcString;

    for (int i = 7; i < receivedMessage.size(); ++i) {
        if (receivedMessage[i] == ' ') {
            isSpace = true;
        } else if (receivedMessage[i] != ' ' && !isSpace) {
            idNewProcString += receivedMessage[i];
        } else if (receivedMessage[i] != ' ' && isSpace) {
            parentIdNewProcString += receivedMessage[i];
        }
    }

    idNewProc = stoi(idNewProcString);
    parentIdNewProc = stoi(parentIdNewProcString);

    if (idNewProc == idThisNode) {
        sendMessage("Error: Already exists", mainSocket);
    } else {

        if (childNodeId == 0 && parentIdNewProc == idThisNode) {

            childNodeId = idNewProc;
            childSocket.bind(adrChild + to_string(childNodeId));
            adrChild += to_string(childNodeId);

            char* adrChildTmp = new char[adrChild.size() + 1];
            memcpy(adrChildTmp, adrChild.c_str(), adrChild.size() + 1);
            char* childIdTmp = new char[to_string(childNodeId).size() + 1];
            memcpy(childIdTmp, to_string(childNodeId).c_str(), to_string(childNodeId).size() + 1);
            char* args[] = {"./child", adrChildTmp, childIdTmp, NULL};

            int procesId = fork();

            if (procesId == 0) {
                execv("./child", args);
                ChildrenId.push_back(idNewProc);
            } else if (procesId < 0) {
                cerr << "Error in forking in node with pid: " << getpid() << endl;
            } else {
                zmq::message_t messageFromNode;

                if (!childSocket.recv(messageFromNode)) {
                    cerr << "Error: can't receive message from child node in node with pid:" << getpid()
                         << endl;
                }

                if (!mainSocket.send(messageFromNode)) {
                    cerr << "Error: can't send message to main node from node with pid:" << getpid() << endl;
                }
            }

            delete[] adrChildTmp;
            delete[] childIdTmp;

        } else if (childNodeId == 0 && parentIdNewProc != idThisNode) {
            sendMessage("Error: there is no such parent", mainSocket);
        } else if (childNodeId != 0 && parentIdNewProc == idThisNode) {
            sendMessage("Error: this parent already has a child", mainSocket);
        } else {
            sendMessage(receivedMessage, childSocket);
            zmq::message_t message;
            if (!childSocket.recv(message)) {
                cerr << "Error: can't receive message from child node in node with pid: " << getpid() << endl;
            }
            if (!mainSocket.send(message)) {
                cerr << "Error: can't send message to main node from node with pid: " << getpid() << endl;
            }
        }
    }
}

void funcExec(string receivedMessage) {
    int idProc;
    string idProcString;

    for (int i = 5; i < receivedMessage.size(); ++i) {
        if (receivedMessage[i] != ' ') {
            idProcString += receivedMessage[i];
        } else {
            break;
        }
    }

    idProc = stoi(idProcString);

    if (idProc == idThisNode) {

        thread workThread(mapAccess, receivedMessage, idProcString, idProc);

        workThread.detach();

        string returnMessage = "The child process performs calculations and outputs them when it finishes calculations";
        sendMessage(returnMessage, mainSocket);

    } else {

        if (childNodeId == 0) {
            sendMessage("Error: id: Not found", mainSocket);
        } else {
            zmq::message_t message(receivedMessage.size());
            memcpy(message.data(), receivedMessage.c_str(), receivedMessage.size());

            if (!childSocket.send(message)) {
                cerr << "Error: can't send message to child node from node with pid: " << getpid() << endl;
            }
            if (!childSocket.recv(message)) {
                cerr << "Error: can't receive message from child node in node with pid: " << getpid() << endl;
            }
            if (!mainSocket.send(message)) {
                cerr << "Error: can't send message to main node from node with pid: " << getpid() << endl;
            }
        }
    }
}

void funcPing(string receivedMessage) {
    int idProc;
    string idProcString;

    for (int i = 5; i < receivedMessage.size(); ++i) {
        if (receivedMessage[i] != ' ') {
            idProcString += receivedMessage[i];
        } else {
            break;
        }
    }

    idProc = stoi(idProcString);

    if (idProc == idThisNode) {
        sendMessage("OK: 1", mainSocket);
    } else {
        if (childNodeId == 0) {
            sendMessage("OK: 0", mainSocket);
        } else {
            zmq::message_t message(receivedMessage.size());
            memcpy(message.data(), receivedMessage.c_str(), receivedMessage.size());
            childSocket.send(message);
            childSocket.recv(message);
            mainSocket.send(message);
        }
    }
}

void funcKill(string receivedMessage) {
    int idProcToKill;
    string idProcToKillString;

    for (int i = 5; i < receivedMessage.size(); ++i) {
        if (receivedMessage[i] != ' ') {
            idProcToKillString += receivedMessage[i];
        } else {
            break;
        }
    }

    idProcToKill = stoi(idProcToKillString);

    if (childNodeId == 0) {
        sendMessage("Error: there isn`t node with this id child", mainSocket);
    } else {
        if (childNodeId == idProcToKill) {
            sendMessage("OK: " + to_string(childNodeId), mainSocket);
            sendMessage("DIE", childSocket);
            childSocket.unbind(adrChild);
            adrChild = "tcp://127.1.1.1:300";
            childNodeId = 0;
        } else {
            zmq::message_t message(receivedMessage.size());
            memcpy(message.data(), receivedMessage.c_str(), receivedMessage.size());
            childSocket.send(message);
            childSocket.recv(message);
            mainSocket.send(message);
        }
    }
}

int main(int argc, char* argv[]) {
    adr = argv[1];
    mainSocket.connect(argv[1]);

    sendMessage("OK: " + to_string(getpid()), mainSocket);
    idThisNode = stoi(argv[2]);
    childNodeId = 0;
    adrChild = "tcp://127.1.1.1:300";

    while (true) {

        zmq::message_t messageMain;
        mainSocket.recv(messageMain);
        string receivedMessage(static_cast<char*>(messageMain.data()), messageMain.size());
        string command;

        for (char element: receivedMessage) {
            if (element != ' ') {
                command += element;
            } else {
                break;
            }
        }

        if (command == "exec") {
            funcExec(receivedMessage);
        } else if (command == "create") {
            funcCreate(receivedMessage);
        } else if (command == "ping") {
            funcPing(receivedMessage);
        } else if (command == "kill") {
            funcKill(receivedMessage);
        } else if (command == "DIE") {
            if (childNodeId != 0) {
                sendMessage("DIE", childSocket);
                childSocket.unbind(adrChild);
            }
            mainSocket.unbind(adr);
            return 0;
        }
    }
}