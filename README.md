# DualAuth Biometric System
**Politecnico de Lisboa, Instituto Superior de Engenharia de Lisboa (ISEL)<br>**
Custodio J., Ghiotto A.<br>
Sensores e Eletronica, Prof. A. Fantoni, AY. 2024-25

This report presents the design and implementation of a multimodal biometric authentication system that integrates fingerprint and electrocardiogram (ECG) data for enhanced security. The system employs an Arduino and Raspberry Pi for hardware control and data processing, utilizing a fingerprint sensor to identify users and an ECG sensor to authenticate their identity based on physiological signals. An OLED display serves as the user interface, allowing for action selection between enrollment, authentication, and system reset via physical buttons. Due to hardware limitations, the ECG sensor is non-functional, and pre-loaded ECG data is used to simulate real-time authentication under ideal conditions. Thresholds for cosine similarity are set at 0.8 for all ECG features, including R-peaks, heart rate, and filtered ECG signals, ensuring consistent authentication results in the simulation. While the system operates effectively in ideal conditions, occasional failures occur due to the low-cost hardware components, with the fingerprint sensor being particularly sensitive to dust and dirt. The results demonstrate the feasibility of combining fingerprint and ECG data for a secure dual-authentication system. However, further testing with real ECG data and improved hardware is required to validate the system's robustness in practical scenarios. Despite these limitations, the project provides a foundation for future development of cost-effective and secure multimodal biometric systems.


<p align="center">
  <img src="https://github.com/user-attachments/assets/8d620982-0597-4f5f-8720-6099cbb0254f" alt="image">
</p>

*Hardware connections of the biometric authentication system. The Arduino Uno interfaces with the ECG sensor (A0), fingerprint sensor (digital pins 2 and 3 for serial communication), push buttons (digital inputs), and LEDs (digital outputs with 330Ω resistors). The Raspberry Pi connects to the Arduino via USB for data processing and communication.*

<p align="center">
  <img src=https://github.com/user-attachments/assets/20ed4f38-52f0-4e23-8df9-8064d40b9b91>
</p>

*Physical implementation of the biometric authentication system.*
