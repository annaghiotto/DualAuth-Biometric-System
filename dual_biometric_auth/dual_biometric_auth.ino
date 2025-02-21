#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

// Buttons & LEDs
const int button1Pin = 5; // Authenticate
const int button2Pin = 6; // Enroll
const int button3Pin = 7; // Exit
const int greenLedPin = 9;  // Green LED
const int redLedPin = 10;    // Red LED
const int yellowLedPin = 11; // Yellow LED
int lastButton1State = LOW;
int lastButton2State = LOW;
int lastButton3State = LOW;

// Serial ports setup
SoftwareSerial fingerprintSerial(2, 3); // Fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);

// ECG sensor
const int ecgPin = A0;
const int ecgLength = 300;
const int ecgUser0[ecgLength] = {2237, 2275, 2312, 2341, 2368, 2393, 2395, 2381, 2356, 2315, 2269, 2206, 2132, 2060, 1982, 1901, 1825, 1371, 1377, 1409, 1449, 1503, 1555, 1608, 1669, 1731, 1791, 1852, 1896, 1936, 1971, 1994, 2011, 2026, 2043, 2061, 2083, 1747, 1683, 1621, 1573, 1535, 1508, 1490, 1482, 1475, 1469, 1463, 1450, 1442, 1438, 1436, 1424, 1417, 1409, 1417, 1436, 1430, 1425, 1430, 1443, 1458, 1477, 1503, 1529, 1554, 1577, 1593, 1607, 1620, 1623, 1621, 1618, 1609, 1602, 1593, 1585, 1456, 1477, 1500, 1526, 1549, 1572, 1596, 1608, 1610, 1603, 1589, 1569, 1555, 1548, 1537, 1525, 1506, 1485, 1463, 1442, 1580, 1575, 1572, 1568, 1566, 1570, 1572, 1576, 1581, 1582, 1581, 1572, 1567, 1555, 1543, 1530, 1519, 1503, 1494, 1495, 1512, 1531, 1551, 1564, 1566, 1557, 1551, 1546, 1542, 1541, 1544, 1553, 1560, 1566, 1571, 1562, 1553, 1542, 1535, 1537, 1549, 1560, 1571, 1575, 1580, 1589, 1603, 1617, 1632, 1638, 1643, 1643, 1639, 1633, 1615, 1594, 1572, 1551, 1531, 1519, 1514, 1517, 1529, 1544, 1566, 1593, 1622, 1634, 1647, 1660, 1670, 1672, 1672, 1660, 1649, 1634, 1616, 1607, 1607, 1616, 1636, 1661, 1692, 1719, 1742, 1756, 1769, 1783, 1795, 1814, 1811, 1793, 1776, 1755, 1736, 1716, 1698, 1682, 1672, 1670, 1674, 1684, 1697, 1718, 1733, 1755, 1765, 1787, 1791, 1796, 1803, 1813, 1820, 1832, 1838, 1843, 1853, 1862, 1869, 1880, 1895, 1905, 1916, 1919, 1922, 1927, 1928, 1924, 1911, 1895, 1872, 1850, 1826, 1800, 1780, 1764, 1747, 1723, 1707, 1702, 1712, 1691, 1685, 1684, 1693, 1707, 1716, 1737, 1759, 1767, 1769, 1769, 1764, 1742, 1723, 1698, 1675, 1649, 1626, 1612, 1714, 1733, 1751, 1774, 1809, 1849, 1892, 1927, 1965, 1988, 2000, 2007, 2001, 1986, 1958, 1920, 1879, 1832, 1786, 1743, 1602, 1597, 1607, 1618, 1632, 1642, 1648, 1660, 1669, 1671, 1672, 1661, 1647, 1631, 1607, 1584, 1554, 1523, 1494, 1476, 1459, 1450, 1442};
const int ecgUser1[ecgLength] = {1871, 1894, 1916, 1935, 1955, 1973, 1985, 1994, 1994, 1985, 1972, 1960, 1942, 1922, 1896, 1878, 1864, 1856, 1856, 1859, 1866, 1867, 1867, 1861, 1856, 1867, 1874, 1885, 1904, 1913, 1922, 1933, 1935, 1935, 1934, 1930, 1923, 1922, 1924, 1927, 1930, 1927, 1930, 1932, 1937, 1948, 1947, 1932, 1916, 1895, 1877, 1864, 1856, 1856, 1864, 1883, 1903, 1917, 1929, 1940, 1948, 1952, 1960, 1958, 1974, 1981, 1986, 1990, 1999, 2006, 2013, 2016, 2014, 2012, 2006, 2000, 1995, 1985, 1974, 1964, 1952, 1937, 1919, 1895, 1870, 1860, 1857, 1861, 1861, 1869, 1878, 1902, 1926, 1953, 1974, 1987, 1996, 2001, 2000, 1996, 1977, 1953, 1936, 1907, 1886, 1857, 1841, 1827, 1834, 1839, 1847, 1861, 1882, 1904, 1914, 1926, 1940, 1952, 1967, 1976, 1999, 2005, 2005, 2000, 2001, 2003, 2008, 2008, 2005, 2000, 1998, 1992, 1986, 1975, 1961, 1947, 1927, 1915, 1901, 1890, 1878, 1869, 1865, 1859, 1859, 1868, 1883, 1906, 1935, 1962, 1980, 1991, 1988, 1974, 1956, 1931, 1908, 2246, 2281, 2320, 2346, 2376, 2400, 2426, 2456, 2480, 2502, 2516, 2533, 2538, 2543, 2540, 2530, 2522, 2500, 2462, 2401, 1896, 1886, 1875, 1869, 1859, 1847, 1836, 1838, 1842, 1854, 1873, 1909, 1952, 1999, 2047, 2093, 2131, 2157, 2185, 2210, 2330, 2262, 2173, 2093, 2009, 1930, 1862, 1807, 1763, 1734, 1712, 1696, 1674, 1668, 1662, 1675, 1690, 1717, 1752, 1791, 1823, 1843, 1854, 1868, 1881, 1896, 1907, 1917, 1919, 1916, 1914, 1921, 1926, 1942, 1953, 1969, 1984, 1991, 1996, 1993, 1980, 1975, 1965, 1952, 1942, 1931, 1923, 1919, 1917, 1914, 1915, 1913, 1907, 1908, 1917, 1930, 1951, 1973, 1993, 2009, 2021, 2029, 2034, 2039, 2040, 2034, 2021, 2012, 2012, 2017, 2026, 2034, 2035, 2024, 2006, 1994, 1974, 1954, 1937, 1922, 1921, 1922, 1922, 1927, 1939, 1950, 1965, 1991, 2011, 2030, 2053, 2075, 2089, 2097, 2104, 2104, 2100, 2089, 2078, 2066, 2058, 2054, 2058};
const int *ecgUsers[] = {ecgUser0, ecgUser1};

int recognizedUserId = -1; // Global variable to store recognized user ID

void setup() {
  Serial.begin(115200);   // Communication with Raspberry Pi
  finger.begin(57600);    // Fingerprint sensor
  //analogReference(EXTERNAL);    // IMPORTANT: Setup AREF pin

  // Buttons & LEDs configuration
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor initialized.");
  } else {
    Serial.println("Fingerprint sensor initialization failed.");
  }

  Serial.println("Arduino ready for commands.");
  finger.emptyDatabase();
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
  digitalWrite(yellowLedPin, HIGH);
}

void loop() {
  turnOnLeds();
  
  // Reading commands from Buttons
  int button1State = digitalRead(button1Pin);
  int button2State = digitalRead(button2Pin);
  int button3State = digitalRead(button3Pin);

  // Check if buttons were pressed
  if (button1State == LOW && lastButton1State == HIGH) {
    turnOffLeds();
    Serial.println("AUTH");
    handleFingerprintAuth();
  }
  if (button2State == LOW && lastButton2State == HIGH) {
    turnOffLeds();
    Serial.println("ENROLL");
  }
  if (button3State == LOW && lastButton3State == HIGH) {
    turnOffLeds();
    Serial.println("EXIT");

    // Reset states
    recognizedUserId = -1; // Reset user ID
    finger.emptyDatabase(); // Clear fingerprint database

    // Reinitialize buttons and LEDs
    lastButton1State = LOW;
    lastButton2State = LOW;
    lastButton3State = LOW;

    // Restart Arduino's main loop
    Serial.println("Arduino ready for commands.");
  }

  // Update buttons state
  lastButton1State = button1State;
  lastButton2State = button2State;
  lastButton3State = button3State;

  if (Serial.available()) {
    turnOffLeds();
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("AUTH")) {
      handleFingerprintAuth();
    } else if (command.startsWith("ENROLL")) {
      int userId = command.substring(7).toInt();
      handleFingerprintEnroll(userId);
    } else if (command == "ECG") {
      acquireAndSendECGData();
    } else if (command == "MATCH_SUCCESS") {
      greenLight();
    } else if (command == "MATCH_FAIL") {
      redLight();
    } else {
      Serial.println("ERROR: Unknown command.");
    }
  }
}

// Handle fingerprint authentication
void handleFingerprintAuth() {
  Serial.println("Finger authentication started...");
  Serial.println("Place your finger on the sensor.");
  
  digitalWrite(yellowLedPin, HIGH);
  
  while (finger.getImage() != FINGERPRINT_OK);
  digitalWrite(yellowLedPin, LOW);

  int result = finger.image2Tz();
  if (result == FINGERPRINT_OK) {
    Serial.println("Fingerprint image converted to template.");

    result = finger.fingerFastSearch();
    if (result == FINGERPRINT_OK) {
      recognizedUserId = finger.fingerID;
      Serial.print("USER_ID: ");
      Serial.println(finger.fingerID);
    } else {
      Serial.print("ERROR: Fingerprint not recognized. Error code: ");
      Serial.println(result);
    }
  } else {
    Serial.print("ERROR: Failed to process fingerprint. Error code: ");
    Serial.println(result);
  }
}


// Enrollment function
void handleFingerprintEnroll(int userId) {
  Serial.print("Enrolling user ID: ");
  Serial.println(userId);

  Serial.println("Place your finger on the sensor.");
  digitalWrite(yellowLedPin, HIGH);
  while (finger.getImage() != FINGERPRINT_OK);

  int result = finger.image2Tz(1);
  if (result == FINGERPRINT_OK) {
    Serial.println("First image captured successfully.");
  } else {
    Serial.print("First image capture failed with error: ");
    Serial.println(result);
    redLight();
    return;
  }

  Serial.println("First fingerprint processed. Remove your finger.");
  digitalWrite(yellowLedPin, LOW);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  // Acquisizione del secondo campione
  Serial.println("Place the same finger again.");
  digitalWrite(yellowLedPin, HIGH);
  while (finger.getImage() != FINGERPRINT_OK);

  result = finger.image2Tz(2);
  if (result == FINGERPRINT_OK) {
    Serial.println("Second image captured successfully.");
  } else {
    Serial.print("Second image capture failed with error: ");
    Serial.println(result);
    redLight();
    return;
  }
  digitalWrite(yellowLedPin, LOW);

  result = finger.createModel();
  if (result != FINGERPRINT_OK) {
    Serial.println("ERROR: Failed to create fingerprint model.");
    redLight();
    return;
  }
  result = finger.storeModel(userId);
  if (result == FINGERPRINT_OK) {
    Serial.println("ENROLL_SUCCESS");
    greenLight();
  } else {
    Serial.println("ERROR: Failed to store fingerprint.");
    redLight();
  }
}

// ECG
void acquireAndSendECGData() {
  const int numBlocks = 16; // Total number of blocks to send
  const int sampleInterval = 2000;

  // **Commented-out analog reading method (original logic)**
  /*
  int ecgData[blockSize]; // Buffer to store ECG samples

  long unsigned startECG = millis(); // Start time of data acquisition
  for (int blockNum = 0; blockNum < numBlocks; blockNum++) {
    unsigned long blockStartTime = micros(); // Start time for the block
    
    // Collect samples for the current block
    for (int i = 0; i < ecgLength; i++) {
      unsigned long sampleStartTime = micros();
      
      ecgData[i] = analogRead(ecgPin); // Read ECG value from the sensor

      while (micros() - sampleStartTime < sampleInterval); // Wait for the sample interval
    }

    // Send the collected block of data to Raspberry Pi in CSV format
    for (int i = 0; i < ecgLength; i++) {
      Serial.print(ecgData[i]); // Send each sample
      if (i < ecgLength - 1) {
        Serial.print(","); // Add a comma between samples
      }
    }
    Serial.println(); // End the block with a newline

    while (micros() - blockStartTime < ecgLength * sampleInterval); // Wait for the block interval
  }

  // Calculate the actual sampling frequency
  unsigned long elapsedTime = millis() - startECG; // Total elapsed time in milliseconds
  int actualFrequency = (numBlocks * ecgLength * 1000) / elapsedTime; // Actual frequency in Hz

  // Send the calculated frequency to Raspberry Pi
  Serial.print("FREQ:");
  Serial.println(actualFrequency); // Send the frequency value
  */

  // **Send preloaded data instead**
  const int *ecgData = nullptr;
  
  if (recognizedUserId >= 0 && recognizedUserId < sizeof(ecgUsers)/sizeof(ecgUsers[0])) {
    const int *ecgData = ecgUsers[recognizedUserId];
  } else {
    // Handle invalid recognizedUserId
    Serial.println("Invalid recognizedUserId");
  }
  
  for (int blockNum = 0; blockNum < numBlocks; blockNum++) {
    for (int i = 0; i < ecgLength; i++) {
      Serial.print(ecgData[i]); // Send each sample
      if (i < ecgLength - 1) {
        Serial.print(","); // Add a comma between samples
      }
    }
    Serial.println(); // End the block with a newline

    delay(10); // Adjust as required for timing consistency
  }

  Serial.println("FREQ:1000"); // Send sampling frequency (1000 Hz for preloaded data)
}

// Turn on green LED
void greenLight() {
    digitalWrite(greenLedPin, HIGH);
    delay(3000);
    digitalWrite(greenLedPin, LOW);
}
// Turn on red LED
void redLight() {
    digitalWrite(redLedPin, HIGH);
    delay(3000);
    digitalWrite(redLedPin, LOW);
}
// Turn off LEDs
void turnOffLeds() {
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
}
// Turn on LEDs
void turnOnLeds() {
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
  digitalWrite(yellowLedPin, HIGH);
}
