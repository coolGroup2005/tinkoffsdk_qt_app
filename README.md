# Invest Manager Desktop Application

![d](https://img.shields.io/badge/language-C++_-blue)
![d](https://img.shields.io/badge/contributors-4_-green)
![d](https://img.shields.io/badge/group-232–1_-pink)

InvestManager is a comprehensive desktop application which is designed to help investors of all levels with the tools and insights they may need to navigate the world of financial markets. Built upon the robust foundation of C++, leveraging the Tinkoff Invest API for seamless market access, and adorned with an intuitive Qt-based user interface, InvestManager offers a streamlined and efficient approach to managing your investment portfolio.

## 📝 Features

1) Portfolio management:
   * Effortlessly track and manage your shares and bonds in a centralized location.
   * Analyze historical data and visualize trends on the market.
  
2) Market exploration:
    * Access comprehensive market data and historical charts.
    * Explore and discover new investment opportunities through advanced search and filtering.
  

3) User-Friendly Interface:
    * Navigate effortlessly through the application with a clean and intuitive Qt-based interface.

## 💻 Technologies

| Component     |Role              |
|----------------------|-----------------------------------------------------------------------------------|
| C++                  | Ensuring high performance and efficiency.                                         |
| Tinkoff Invest API   | Providing secure and reliable access to market data and trading functionality.    |
| Qt                   | Delivering a modern and user-friendly interface.                                  |

<br/>

## 🔎 Detailed Overview 

<details open>
<summary style="font-size: 20px; font-weight: 600; text-decoration: underline">
 Homepage
</summary> <br />

> **Description**
>
> This page displays all favourite shares and gives a possibility to filter them by trading status. By clicking on one particular share, it is possible to see a detailed graph with historical candles over selected period
>   
> ![Figi demo](./assets/home.gif) 
</details>


<details open>
<summary style="font-size: 20px; font-weight: 600; text-decoration: underline">
 Statistics page
</summary> <br />

> **Description**
>
>
> This page displays top most profitable and non-profitable shares over selected period. Options: 
> * 1 day
> * 1 week
> * 1 month
> 
> In order to compute the result, application makes a lot of requests to the server to get all neccessary data, so computation may take some time, especially if the network connection is unstable. In order to see demonstration of this functionality quickly, one may set combo box to active mode.  
>
> ![Statistics demo](./assets/statistics.gif)
</details>

<details open>
<summary style="font-size: 20px; font-weight: 600; text-decoration: underline">
 Portfolio
</summary> <br />


> **Description**
>
>
> This page displays information about portfolio of the user. It is possible to switch between accounts and update information in real time.
>   
> ![Figi demo](./assets/portfolio.gif)
</details>


<details open>
<summary style="font-size: 20px; font-weight: 600; text-decoration: underline">
 Database Figi
</summary> <br />

> **Description**
>
>
> This page ensures quick access to the unique figi of a particular share and helps to instantly check its trading status.
>   
> ![Figi demo](./assets/figi.gif)
</details>

<br /><br />


## ❗️  Prerequisites 
<details open>
<summary>
 Install this to use the app
</summary> <br />

## Building
- Linux (tested on rpm and deb-based distros)
  - To compile this project via CMake you need to manually replace two files in `build/_deps/grpc-src/third_party/abseil-cpp/absl/strings/internal/str_format`: [extension.cc](https://pastebin.com/uqrCrtyA) and [extension.h](https://pastebin.com/4LvmszCY)
  - Also, in `build/_deps/tinkoffinvestsdk-src/cmake/common.cmake` you might also need to change `set (CMAKE_CXX_STANDARD 11)` to `set (CMAKE_CXX_STANDARD 17)`
- MacOS (tested on Ventura (Apple Silicon) and Catalina (Intel))
  - See `build/deps/tinkoffinvest-src/CMakeLists.txt`, check that `cmake_minimum_required version` is `3.8`
  - See `build/deps/tinkoffinvest-src/cmake/common.cmake` and check that gRPC version is [the latest one](https://github.com/grpc/grpc.git). You might also need to change `set (CMAKE_CXX_STANDARD 11)` to `set (CMAKE_CXX_STANDARD 17)`
  - Modify a piece of code related to absl in `build/deps/tinkoffinvest-src/cmake/common.cmake`:

    ```
    set(ABSL_ENABLE_INSTALL ON)
    FetchContent_Declare(
      absl
      GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
      GIT_TAG        origin/master
      OVERRIDE_FIND_PACKAGE
    )
    
    FetchContent_MakeAvailable(absl)
    message(STATUS "Using gRPC via add_subdirectory (FetchContent).")
    include(FetchContent)
    FetchContent_Declare(
      grpc
      GIT_REPOSITORY https://github.com/grpc/grpc.git
      GIT_TAG        v1.64.0)
    FetchContent_MakeAvailable(grpc)
    ```
- Windows
  - Install [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) and go to Linux tutorial


</details>

<br />

## 📖 Documentation 
link to documentation: ``https://pelancha.github.io/index.html``



