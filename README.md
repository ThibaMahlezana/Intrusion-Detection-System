# Intrusion-Detection-System

## About the Project
This intrusion detection application is designed to monitor live camera feeds for unauthorized access or trespassing. Utilizing C++ with OpenCV and NCNN for real-time image processing, and ImGui for a user-friendly interface, the application allows users to select specific areas of interest. If a person is detected within these areas, an alarm is triggered to alert users, enhancing security measures.

## Motivation
The motivation behind this project stems from a growing concern about crime rates and the need for improved security measures. I believe that AI-driven computer vision systems can play a crucial role in monitoring homes and businesses, enabling quicker responses to security threats. By leveraging technology, we can create a safer environment for everyone.

## Built with
- **C++:** Core programming language for performance and efficiency.
- **OpenCV:** Library for real-time computer vision.
- **NCNN:** High-performance neural network inference framework for mobile and edge devices.
- **ImGui:** Immediate mode GUI for creating intuitive user interfaces.
- **CMake:** Build system for managing project compilation.

## Getting Started
To get started with this project, follow these instructions to set up your environment:

### Prerequisites
- C++ compiler (GCC, Clang, or Visual Studio)
- CMake (version 3.10 or higher)
- OpenCV (version 4.0 or higher)
- NCNN
- ImGui

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/ThibaMahlezana/Intrusion-Detection-System.git
   cd Intrusion-Detection-System
   
2. Create a build directory and navigate into it:
   ```bash
   mkdir build && cd build
   
3. Run CMake to configure the project:
   ```bash
   cmake ..
   
4. Compile the project:
   ```bash
   make
   
6. Run the application:
   ```bash
   ./SECURITY_SYSTEM

## Screenshots
<img src="https://github.com/ThibaMahlezana/Intrusion-Detection-System/blob/main/screenshots/System%20Capture%201.PNG" width="600">

<img src="https://github.com/ThibaMahlezana/Intrusion-Detection-System/blob/main/screenshots/System%20Capture%202.PNG" width="600">

<img src="https://github.com/ThibaMahlezana/Intrusion-Detection-System/blob/main/screenshots/System%20Capture%203.PNG" width="600">

<img src="https://github.com/ThibaMahlezana/Intrusion-Detection-System/blob/main/screenshots/System%20Capture%204.PNG" width="600">

<img src="https://github.com/ThibaMahlezana/Intrusion-Detection-System/blob/main/screenshots/System%20Capture%205.PNG" width="600">

<img src="https://github.com/ThibaMahlezana/Intrusion-Detection-System/blob/main/screenshots/System%20Capture%206.PNG" width="600">

## Usage
1. Launch the application.
2. Allow access to your camera feed (Input webcam index, video url, or RTSP url).
3. Select the area of interest to monitor for intrusion.
4. Start the detection process.
5. An alarm will sound if a person is detected within the marked area.

## Contributing
Contributions are welcome! Please follow these steps to contribute:

1. Fork the repository.
2. Create a new branch (git checkout -b feature-branch).
3. Make your changes.
4. Commit your changes (git commit -m 'Add new feature').
5. Push to the branch (git push origin feature-branch).
6. Open a pull request.

## License
This project is licensed under the MIT License.

## Contact
For inquiries or suggestions, feel free to reach out:

- **Name**: Thiba Mahlezana
- **Email:** thiba.ma@gmail.com

## Acknowledgements
- OpenCV contributors for their outstanding work in computer vision.
- NCNN developers for providing a lightweight neural network framework.
- ImGui community for making GUI development simpler and more accessible.


