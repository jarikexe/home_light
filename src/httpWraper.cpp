#include <HTTPClient.h>
// class HttpWraperCustom {
    static void sendRequestHui(String url) {
        HTTPClient http;
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
        }
        else {
        Serial.println("http request error");
        }
        http.end();
    } 
// };

    