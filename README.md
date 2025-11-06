# STM32 Self-Balancing Control

A self-balancing robot based on the **STM32 microcontroller**, implementing both **PID** and **LQR** control algorithms.
This project also applies **signal filtering** techniques to improve sensor accuracy and control stability:

* **Moving Average Filter** for encoder smoothing
* **Complementary Filter** for IMU fusion (gyroscope + accelerometer)

---

## 🧭 Overview

This project demonstrates the design of a **self-balancing two-wheel robot** using an STM32 MCU.
It integrates:

* Real-time angle estimation with IMU sensors
* Encoder feedback for wheel speed
* Dual control strategies (PID and LQR)
* Data filtering for smoother, more accurate control
---

## ⚙️ System Architecture / Kiến trúc hệ thống

```
[ Encoder ] ---> [ Moving Average Filter ] ---> [ Wheel Speed ]
                      ↓
[ IMU (Gyro + Accel) ] ---> [ Complementary Filter ] ---> [ Tilt Angle θ, θ_dot ]
                      ↓
            [ PID / LQR Controller ]
                      ↓
             [ PWM Output to Motors ]
```

---

## 🧩 Features

| Module                | Description                                        | Mô tả                                                       |
| :-------------------- | :------------------------------------------------- | :---------------------------------------------------------- |
| **Encoder Filtering** | Moving Average Filter to smooth velocity readings  | Bộ lọc trung bình trượt làm mượt dữ liệu encoder            |
| **Angle Estimation**  | Complementary Filter combining gyro & accel        | Bộ lọc bổ sung hợp nhất gyro và accelerometer               |
| **PID Control**       | Classic feedback control for balance               | Điều khiển phản hồi cổ điển PID                             |
| **LQR Control**       | Optimal control minimizing system energy           | Điều khiển tối ưu hóa năng lượng hệ thống                   |
| **Visualization**     | Serial plotting / Python interface for angle & PWM | Giao diện hiển thị qua Serial/Python để giám sát góc và PWM |

---

## 🧠 Control Algorithms

### 1. PID Control

[
u = K_p e + K_i \int e ,dt + K_d \frac{de}{dt}
]

### 2. LQR Control

[
u = -Kx
]
where ( x = [\theta, \dot{\theta}, v, \dot{v}]^T )

---

## 🔧 Hardware Setup 

| Component    | Description                      |
| :----------- | :------------------------------- |
| MCU          | STM32L476RGT6 series       |
| IMU          | ICM20948               |
| Motor Driver | STSP240                |
| Encoders     | DC motor encoders (e.g., 11 PPR) |
| Power        | 2S/3S LiPo battery               |
| Frame        | Two-wheel balancing chassis      |

---

## 📂 Project Structure

```
STM32-Self-Balancing-Control/
├── Core/
│   ├── Src/
│   ├── Inc/
│   └── main.c
├── Drivers/
├── Filters/
│   ├── moving_average.c
│   └── complementary_filter.c
├── Control/
│   ├── pid.c
│   └── lqr.c
├── Utils/
│   └── encoder.c
├── README.md
```

---

## 🧪 Implementation Notes / Ghi chú triển khai

* Sampling period: 10 ms (100 Hz loop)
* Complementary filter α = 0.98
* Moving average window = 10 samples
* PID gains tuned manually via step response
* LQR gains obtained from linearised model in MATLAB

---

## 🧰 Build & Flash

**Toolchain:** STM32CubeIDE / PlatformIO
**Steps:**

1. Clone this repository

   ```bash
   git clone https://github.com/NhatTran-97/STM32-Self-Balancing-Control.git
   ```
2. Open project in STM32CubeIDE
3. Build and flash firmware to STM32 board
4. Connect via UART/USB to monitor data

---

## 🎥 Demo (optional placeholder)

> *Add a GIF or image of the balancing robot here*
>
> Ví dụ: ảnh robot đang tự cân bằng hoặc biểu đồ góc nghiêng.

---

## 📚 References 

* [Complementary Filter explained](https://www.pieter-jan.com/node/11)
* [LQR Design for Inverted Pendulum](https://ctms.engin.umich.edu/CTMS/index.php?example=InvertedPendulum&section=ControlStateSpace)
* STM32Cube HAL documentation
* ICM20948 datasheet

---

**Author:** [NhatTran-97](https://github.com/NhatTran-97)
**License:** MIT License
