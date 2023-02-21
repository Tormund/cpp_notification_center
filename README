# Generic Notification Center for C++
Requires `c++17`.
Uses variadic templates to allow have different types of callback in one Notification Center.
Also implemented runtime checks for argument types in `post` and `addObserver` function for same notification.
Callback can be std::function or lambda.

## Example:
```cpp
    enum MyNotifications {
        eHelloNotification, eMessageNotification, eByeNotification
    };

    void test()
    {
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

        nc.post(eMessageNotification, "some data");
        nc.post(eHelloNotification);
        nc.post(eByeNotification);
    }
```


