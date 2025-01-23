#include <iostream>
#include <unit.h>
#include <net.h>

#define IMEI "864205069515823"
#define REMOTE "192.168.1.1"
#define PORT "1234"

int main() {
    gps::unit U1(IMEI);
    U1.push(REMOTE, PORT);
}