#ifndef ALERT_H
#define TALERT_H

class Alert
{
public:
    Alert();
    ~Alert();

public:
    int soundBuzzer();
    void stopBuzzer();
    void sendNotificationtoApp();
    void checkInternetConnection();
};

#endif