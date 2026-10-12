🎨 Image Processing Studio

A desktop image processing application built with C++23 and Qt 6.

The application provides 16 image processing filters and operations through an interactive graphical interface, with support for image preview, Undo/Redo, and a clean layered architecture.

⸻

✨ Features

* 🖼️ Before / After Preview — Compare the original and processed images.
* 🎨 21 Image Filters & Operations — Including Blur, Crop, Resize, Merge, Infrared, Brightness, Frames, and more.
* ↩️ Undo / Redo — Navigate through previous image states.
* 🎛️ Interactive Qt GUI — Buttons, sliders, dialogs, and custom QSS styling.
* 💾 Load & Save Images — Import and export image files.
* 🧩 Extensible Architecture — Easily add new filters without changing the core application logic.

⸻

🏗️ Architecture

The project follows a Layered Architecture to separate responsibilities and make the application easier to maintain and extend.

┌─────────────────────────────────────┐
│        PRESENTATION LAYER           │
│      Qt GUI / MainWindow / QSS      │
└──────────────────┬──────────────────┘
                   ↓
┌─────────────────────────────────────┐
│        APPLICATION LAYER            │
│ Controller / UndoRedoManager        │
└───────────────┬───────────┬─────────┘
                ↓           ↓
┌────────────────────┐  ┌────────────────────┐
│ IMAGE PROCESSING   │  │ INFRASTRUCTURE     │
│                    │  │                    │
│ IImageFilter       │  │ Image Loader       │
│ 16 Filters         │  │ Image Saver        │
└──────────┬─────────┘  └─────────┬──────────┘
           └────────────┬─────────┘
                        ↓
              ┌──────────────────┐
              │   MODELS LAYER   │
              │      Image       │
              └──────────────────┘

Layer Responsibilities

Layer	Responsibility
Presentation	GUI and user interaction
Application	Coordinates application logic
Image Processing	Implements image filters
Infrastructure	File loading and saving
Models	Core image representation

⸻

🎯 Design Patterns

Strategy Pattern

All image filters implement a common interface:

class IImageFilter
{
public:
    virtual Image apply(const Image& image) = 0;
    virtual ~IImageFilter() = default;
};

This allows different filters such as:

IImageFilter
├── BlurFilter
├── CropFilter
├── ResizeFilter
├── MergeFilter
├── InfraredFilter
└── ...

Adapter Pattern

An adapter is used to connect the application’s image model with Qt’s QImage, keeping framework-specific details separated from the processing logic.

⸻

🧠 OOP Concepts

The project demonstrates practical usage of:

* Encapsulation
* Abstraction
* Inheritance
* Polymorphism
* Interfaces
* Composition
* Virtual Functions

⸻

📁 Project Structure

ImageProcessingStudio/
│
├── Presentation/
├── Application/
├── ImageProcessing/
├── Infrastructure/
├── Models/
├── Resources/
│
├── main.cpp
├── CMakeLists.txt
└── README.md

⸻

🛠️ Technologies

* C++23
* Qt 6
* Qt Widgets
* QImage
* CMake
* Git / GitHub

⸻

🚀 Build

Clone the repository:

git clone https://github.com/your-username/ImageProcessingStudio.git
cd ImageProcessingStudio

Build the project using CMake and your installed Qt 6 environment.

⸻

🎓 Project Goals

This project was developed to practice:

* Object-Oriented Programming
* Image Processing
* Qt Desktop Development
* Layered Architecture
* Design Patterns
* Clean Code
* Git-based Team Collaboration

⸻

👥 Team

Developed as a collaborative C++ & Qt project.

⸻

⭐ Image Processing Studio — A structured desktop application for image processing using C++ and Qt.
