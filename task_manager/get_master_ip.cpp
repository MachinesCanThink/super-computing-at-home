#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>

using namespace std;

typedef struct ip_address {
	string wlan_ip;
	string eth_ip;
} ip_address;

void getMastersIpAddress(ip_address&);
void initIpAddressStructure(ip_address&);
void writeAddressToFile(ip_address);

int main(int argc, char *argv[])
{
	ip_address address;

	initIpAddressStructure(address);

	getMastersIpAddress(address);

	cout <<"IP address : " <<address.eth_ip <<"  " <<address.wlan_ip <<endl;

	writeAddressToFile(address);

	return 0;
}

void initIpAddressStructure(ip_address &address)
{
	address.wlan_ip = "-1";
	address.eth_ip = "-1";
}

void getMastersIpAddress(ip_address &address)
{
	struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    string ip_address;
    string space;

    space = "  ";

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }

        if (ifa->ifa_addr->sa_family == AF_INET) { 
        	// check it is IP4 is a valid IP4 Address 
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN); 

            if (ifa->ifa_name == "lo") {
            	;
            } else if (ifa->ifa_name[0] == 'e') {
            	address.eth_ip = ifa->ifa_name + space + addressBuffer;
            } else if (ifa->ifa_name[0] == 'w') {
            	address.wlan_ip = ifa->ifa_name + space + addressBuffer;
            }
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { 
        	// check it is IP6 is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        } 
    }

    if (ifAddrStruct!=NULL) 
    	freeifaddrs(ifAddrStruct);
}

void writeAddressToFile(ip_address address) 
{
	ofstream output;

	output.open("master_ip_address");

	output <<address.eth_ip <<endl;
	output <<address.wlan_ip <<endl;

	output.close();
}