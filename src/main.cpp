/**
   httpUpdateSecure.ino

    Created on: 16.10.2018 as an adaptation of the ESP8266 version of httpUpdate.ino

*/

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include <time.h>

WiFiMulti WiFiMulti;

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

  Serial.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }

  Serial.println(F(""));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

/**
 * This is lets-encrypt-x3-cross-signed.pem
 */
const char* rootCACertificate = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgISA0yY6f8XkXfZ7j0M9xG5wU3PMA0GCSqGSIb3DQEBCwUA
MEoxCzAJBgNVBAYTAlVTMRMwEQYDVQQKEwpMZXQncyBFbmNyeXB0MR8wHQYDVQQD
ExZJc1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4WhcNMzUwNjA0MTEwNDM4WjBK
MQswCQYDVQQGEwJVUzETMBEGA1UEChMKTGV0J3MgRW5jcnlwdDEfMB0GA1UEAxMW
SXNSRyBSb290IFgxMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA1I0G
K8nRqHnZt9nKbCGq+M0G+VHu3xP6kUwTCSnQb5dbSpkV1kR+myjI1p7P7Xg/x9Wd
ql+R93fO4nTjxCgIDIn0P83QDYQGmDz+/6i4oS/bkDmg3tkvPp5sw9cTxW4LOM6Y
hU+j0uwTjcJ96A2e4LZZ+vXokZk5U+Kph3q1pWx6Q0gkJq5hQ3X/SlxQn6p8R0Ml
oF5VwrFm1MHX6B6kUCYRt8yB0s8X4CWv9UnXe8V1cXqR3/Zw0iB6joFG0VpOLovc
fLh/6F3dOlJ15Q0+q3q6aOSv+H7DnZjZ8ZMxw4zVIG9F6BBG6d9uD6YKgB5J0h4Q
FlC4CNRMHx4c3/8XszvI+uYtHBGV/q4osn74+0F2Vg3P0iU4yI2ZkR4vSv5pFppA
HBlr5H4kZqUcq5MQo8hZdAfU5A8I0g3DRaY1Xoa2m5T4g9xqP0R4hnfQF9XvnFM0
FjKjF4OzvY7k5/3uI4KDFYhnCKfiy2GhbCMThnYc/6p9K5+3uZk5/EfSmX2GhxP+
8gn9ReH0DEIk2H3JwFL+Kqkh+Sv0KHwZB4UjN92/XvM2xZxy1ZJfAQ1L0tpxE2rR
PttM6v+LrKpFp3GvFP+V1U8mj0Xw/VbWeTdjfiN5nW0hZjGKu1gQFrq1r3hkg19r
rQIDAQABo0IwQDAdBgNVHQ4EFgQUuQmYkP8Hv3S8pOqtbZ0xH4O2r8AwHwYDVR0j
BBgwFoAUuQmYkP8Hv3S8pOqtbZ0xH4O2r8AwDQYJKoZIhvcNAQELBQADggIBACJp
XQhIWwHcqZpYqBvE02m8/EYyxq0U6xXgn2y4e6ZKxXOgGxq5+v6+/6oBqXGgZpFz
Y5e2rDgM2O/NRFh3v8bzQ0u7u9IZc2Om6cVf6/8y4gXNUFhP2fMkD4UpJ5yqvlRS
6Y8OXQ4bE3IYgF8dVfn39cZsK9Nk8AzT/E2IXRLCmU5Ti6rP1ecQ5pTtAQlJ+y9O
G3XhDx/9sGRUQ7XO4f4xHkG2VqH0fRoT8Cj+v5IidvEAvYZB0QjU1Z+Jx7l8VZ8D
z/kQZq7M8YX5x4dqU+6bYv8v5eRIJjXr+guhb7p1x1P6z3FqjOW+uZp71rV3hCVZ
WmCq5aCj1+F8MlCaa4uXqFVBGyMHT+zRY6E60Pq1syu1vVIXG3G3C9qkF8TAvHzQ
L4yjVjXc3m7r1GrCw1G4+qZ2hrcUMO1gMUSfKiP+6Phr5qR2JxwRAOtrUWeC0L3h
kQkRjEZGncnzMpr3WvFRs/fJPTQdEv4NLJ5a+ShRzUu8tY5Ufhn3blsZvhKShXjs
g5A3MJUKS4GfE0pOk8G6/4j6gLQJ5yAcmc7L58Sx5u8H7c0rNSOdz4lYx3k6UbrH
9dQtbFwVjaF+Khp1r9FMyH+E/XFmsiE5QXhVgu0q1o5dX1BBlVb4gHhVgPo/TnD1
0u0GvMIflUhhGdtpHQqj
-----END CERTIFICATE-----
)EOF";

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("SSID", "PASSWORD");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    setClock();

    WiFiClientSecure client;
    client.setCACert(rootCACertificate);

    // Reading data over SSL may be slow, use an adequate timeout
    client.setTimeout(12000 / 1000); // timeout argument is defined in seconds for setTimeout

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    // httpUpdate.setLedPin(LED_BUILTIN, HIGH);

    t_httpUpdate_return ret = httpUpdate.update(client, "https://server/file.bin", "", [](HTTPClient *client) {
      client->setAuthorization("test", "password");
    });
    // Or:
    //t_httpUpdate_return ret = httpUpdate.update(client, "server", 443, "/file.bin");


    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}