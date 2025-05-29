# IDE Autonomus Car Project

A hands-on, multi-demo **I**nterface & **D**igital **E**lectronics (IDE) line-following robot car built for the CMPE-570/670 course.  The project showcases progressive control strategies—from simple threshold steering to full PID control—while teaching sensor interfacing, real-time image processing, and embedded Python development.

---

## Table of Contents
1. [Project Highlights](#project-highlights)
2. [Hardware Bill of Materials](#hardware-bill-of-materials)
3. [Software Stack](#software-stack)
4. [Directory Layout](#directory-layout)
5. [Installation](#installation)
6. [Running the Demos](#running-the-demos)
7. [PID-Gain Tuning Guide](#pid-gain-tuning-guide)
8. [Roadmap](#roadmap)
9. [Contributing](#contributing)
10. [License](#license)

---

## Project Highlights
| Demo | Status | Purpose | Key Techniques |
|------|--------|---------|----------------|
| **Demo 1 – Threshold Steering** | ✅ *Completed (2025-04-16)* | Achieve basic line following by steering toward binary mask centroid. | Color thresholding, binary morphology, proportional steering. |
| **Demo 2 – PID Control** | 🚧 *In Progress (2025-05-29)* | Maintain smoother trajectory at higher speed. | Tunable PID loop on lateral error, runtime plotting. |
| **Future Demos** | 🗓️ | Planned extensions (e.g., dynamic speed, obstacle avoidance, autonomous modes). | TBD |

---

## Hardware Bill of Materials
| Qty | Component | Notes |
|-----|-----------|-------|
| 1 | Raspberry Pi 4 (Model B, 4 GB) | 64-bit ARM fits Surface Pro cross-compile toolchain. |
| 1 | Pi Camera v2 / HQ Camera | Mounted ~10 cm above track. |
| 1 | L298N or TB6612FNG motor driver | Dual DC-motor control. |
| 2 | 6 V DC gear motors + wheels | Continuous drive. |
| 1 | Swivel caster wheel | Front support. |
| 1 | 7.4 V Li-Po battery + switch | Power. |
| — | Dupont wires, chassis, standoffs | Assembly. |

> **Tip :**  The parts list is flexible—any microcontroller that can stream a grayscale camera image at ≥30 FPS and drive two DC motors will work.

---

## Software Stack
* **Python 3.11**  (tested)
* **OpenCV 4.10** – image processing
* **NumPy** – math
* **RPi.GPIO / gpiozero** – GPIO access
* **matplotlib** (optional) – live plots
* **pytest** – unit tests

The code runs natively on the Pi or cross-compiled on a **Windows ARM** Surface Pro with VS Code Remote-SSH.

---

## Directory Layout

