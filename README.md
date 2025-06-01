# CloudWorx-Desktop-Client

Welcome to the **CloudWorx Desktop Client**!
This application is built in C++ using CMake and Qt Creator. It is one of the three main components of our CloudWorx platform.

## What is CloudWorx?

CloudWorx is a secure, end-to-end encrypted file sharing platform. Your files are stored completely encrypted and are inaccessible to us. With CloudWorx, you can:

* Share files securely
* Revoke access at any time
* Store personal data all within one platform

---

## Getting Started

### 1. Running the Application (No coding required)

If you just want to run the app and don’t want to dig into the code, follow these steps:

```bash
git clone <repo-url>
```

* Copy the `.exe` file from the repository.
* Paste it into any folder/directory of your choice.
* Open Qt Creator’s terminal, navigate (`cd`) to the folder where the `.exe` is located.
* Run the deployment tool:

  ```bash
  windeployqt.exe
  windeployqt.exe .
  ```
* Now, double-click `CloudWorx.exe` and enjoy the app!

---

### 2. Building and Modifying the Project (For Developers)

If you want to explore the code, make changes, or build the project yourself, here’s what you need to do:

1. Create a folder named `libs` inside your project directory.

2. Download and place the following libraries inside `libs`:

   * **libsodium**
   * **libcurl**

3. Download and install **OpenSSL** for your system.

   * The default include path in the CMake file is set to:

     ```
     C:/Program Files/OpenSSL-Win64/include
     ```
   * This path is Windows 64-bit specific. If you’re on a different OS or architecture, update this path in `CMakeLists.txt` accordingly.

4. Open the project in Qt Creator.

5. Build and run the project — it should compile and work correctly.

---
