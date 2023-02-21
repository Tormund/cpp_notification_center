// should be compiled with c++17
#include <iostream>
#include <string>
#include "notification_center.hpp"


void stringNotificationCenterTest()
{
    NotificationCenter<std::string> nc{};

    int intObserver = 10;
    void* ptrObserver = &intObserver;
    nc.addObserver(intObserver, "hi", [](double data1, int data2, std::string data3){
        std::cout << "intObserver received hi "<< data1 << ", " << data2 << ", " << data3 << std::endl;
    });
    nc.addObserver(ptrObserver, "hi", [](double data1, int data2, std::string data3){
        std::cout << "ptrObserver received hi "<< data1 << ", " << data2 << ", " << data3 << std::endl;
    });
    nc.post("hi", 3.14, 42, std::string("hello world"));

    nc.addObserver(intObserver, "onSmthHappen", [](){
        std::cout << "something happen" << std::endl;
    });
    nc.post("onSmthHappen");

    nc.removeObserver(intObserver, "onSmthHappen");

    int* arr = new int[2];
    arr[0] = 42;
    arr[1] = 13;

    nc.addObserver(intObserver, "arr", [](int * arr){
        std::cout << "arr: " << arr[0] << ", " << arr[1] << std::endl;
    });
    nc.post("arr", arr);

    nc.removeObserver(intObserver);
    nc.removeObserver(ptrObserver);
    delete[] arr;
}

enum MyNotifications {
    eHelloNotification, eMessageNotification, eByeNotification
};

void enumNotificationCenterTest()
{
    std::cout << "Enum notification center:" << std::endl;

    NotificationCenter<MyNotifications> nc;
    int observer = 10;

    std::function<void()> helloCb = [](){
        std::cout << "eHelloNotification" << std::endl;
    };

    nc.addObserver(observer, eHelloNotification, helloCb);

    nc.addObserver(observer, eMessageNotification, [](const char* message){
        std::cout << "eMessageNotification: " << message << std::endl;
    });

    nc.addObserver(observer, eByeNotification, [](){
        std::cout << "eByeNotification" << std::endl;
    });

    nc.post(eHelloNotification);
    nc.post(eMessageNotification, "some data");
    nc.post(eByeNotification);
}


int main()
{
    stringNotificationCenterTest();
    enumNotificationCenterTest();
    std::cout << "Done!\n";
    std::string in;
    std::getline(std::cin, in);
}