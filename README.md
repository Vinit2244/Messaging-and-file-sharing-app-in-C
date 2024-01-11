# MessMe - A WhatsApp-like Messaging App for macOS

![MessMe Logo](logo.png)

MessMe is a messaging application developed in C specifically for macOS, featuring a server-client architecture. The application allows users to sign in or sign up, send and receive various file types, save credentials for easy login, and perform other essential messaging functions.

## Table of Contents
- [MessMe - A WhatsApp-like Messaging App for macOS](#messme---a-whatsapp-like-messaging-app-for-macos)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
  - [Usage](#usage)
    - [Sign Up/Sign In](#sign-upsign-in)
    - [File Sharing](#file-sharing)
    - [Account Management](#account-management)
    - [Viewing Transactions](#viewing-transactions)
  - [Development Details](#development-details)
    - [Data Structures](#data-structures)
    - [Concurrency](#concurrency)
  - [To-Do List](#to-do-list)
  - [Contributing](#contributing)
  - [Contributors](#contributors)

## Features

- User authentication (sign up/sign in) with username and password
- Secure storage of credentials for quick login
- File sharing for various types (images, PDF, docx, audio, live image capture, etc.)
- Account management (delete account, log out)
- Transaction logging with the ability to view transactions
- Efficient data structures for fast data storage and retrieval
- Concurrency for handling multiple client requests simultaneously
- Error handling and color coding for improved user experience

## Getting Started

### Prerequisites

- macOS operating system
- [C Compiler](link/to/c-compiler)

## Usage

### Sign Up/Sign In

Upon launching the client, users are prompted to sign up or sign in. The application requests a username and password, and the server checks the availability of the username. Successful login allows users to save their password for future convenience.

### File Sharing

Users can share various file types when both parties are online. Supported file types include images, PDFs, docx, audio, and live image capture.

### Account Management

Users can delete their accounts or log out from the application, providing flexibility and control over their account status.

### Viewing Transactions

The server maintains a log of all transactions, accessible by pressing `ctrl+z` in the client's CLI.

## Development Details

### Data Structures

Efficient data structures are employed for fast data storage and retrieval, ensuring optimal performance.

### Concurrency

The application utilizes concurrency to serve multiple client requests simultaneously, improving responsiveness.

## To-Do List

- Implement a general name generating function for shared files
- Introduce a location-sharing feature
- Develop a web application for MessMe

## Contributing

Contributions are welcome! Feel free to open an issue or submit a pull request.

## Contributors
Vinit Mehta - This is my personal project
