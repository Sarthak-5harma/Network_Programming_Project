# 🎮 UDP Multiplayer Guessing Game

A fun, real-time multiplayer number guessing game built using **C++ and Unix Sockets (UDP)** as a course project for **Network Programming**.

---

## 📌 Project Overview

This is a terminal-based guessing game where multiple clients can connect to a server and guess a randomly generated number. The server sends real-time hints, broadcasts victory messages, and resets the game automatically.

Each player is identified by their IP:Port. The game logic is simple, interactive, and gamified using emojis for a better experience.

---

## 🔧 Technologies Used

- **Language:** C++
- **Networking:** Unix Sockets (UDP)
- **Platform:** Linux (Ubuntu)
- **Concurrency:** Threads for broadcast listening on clients

---

## 🚀 How to Run the Project

### 1. Compile the Server and Client

```bash
g++ server.cpp -o server
g++ client.cpp -o client -pthread
```

### 2. Run the Server

```bash
./server
```

### 3. Run One or More Clients (in Separate Terminals)

```bash
./client
```

---

## 🎮 How to Play

- The server picks a **random number between 1 and 100**.
- Clients send guesses using the terminal.
- Server responds with:
  -  `"Too high"`
  -  `"Too low"`
  -  `"Correct!"`
- When someone guesses correctly:
  - The server **broadcasts the winner** to all clients.
  - The game **resets** with a new secret number.
- The server maintains and displays a **scoreboard** (currently only shown on the server side).

---

## 🧠 Concepts Demonstrated

- ✅ UDP socket communication
- ✅ Multiple concurrent clients using broadcast
- ✅ Stateless guessing logic with stateful scoreboard
- ✅ Broadcast messaging over LAN
- ✅ Fun, gamified terminal UX

---


## 📋 Sample Output

### Server Output

```
🎮*************************************🎮
🎮   Welcome to the UDP Guessing Game   🎮
🎮       Scoreboard Enabled!           🎮
🎮*************************************🎮
Server listening on port 4000
🎯 Secret number (for debugging): 15
Received guess 20 from 127.0.0.1:59316 ❌ (Too high 🔥)
Received guess 11 from 127.0.0.1:47280 ❌ (Too low 🧊)
Received guess 15 from 127.0.0.1:59316 ✅ (Correct!) 🎯
*********************************************************
🏆 Player 127.0.0.1:59316 guessed the number correctly! 🎉🎉🎉
📊 Current Scoreboard:
   127.0.0.1:59316 - 1
🔄 A new number has been generated. Game continues! 🎲
*********************************************************
🎯 New secret number (for debugging): 48
```

### Client Output

```
🎮*************************************🎮
🎮   UDP Guessing Game Client Started  🎮
🎮*************************************🎮
Connected to server 127.0.0.1:4000
Enter your guess (or 'exit'): 20
Too high 🔥
Enter your guess (or 'exit'): 15
✅ Correct! 🎉 You nailed it! 🏆
[BROADCAST] 🎉 Player 127.0.0.1:59316 guessed it right! New number generated! 🎯
```
---

## 📦 Project Structure

```bash
.
├── server.cpp       # UDP Game Server
├── client.cpp       # UDP Game Client
├── README.md        # Project Documentation
```
---

## 👨‍💻 Contributors
- Sarthak Sharma
- Maheswor Ghimire KC
- Manij KC

Network Programming Course

Nepal College of Information Technology
