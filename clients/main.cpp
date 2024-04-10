#include "net.h"

using namespace std;

int main()
{
#if defined(_WIN32) || defined(_WIN64)
	system("chcp 65001");
#endif
    net client;
    client.netStart();
    return 0;
}