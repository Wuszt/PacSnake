<img width="400" height="400" alt="ChatGPT Image Aug 24, 2025, 05_02_55 PM" src="https://github.com/user-attachments/assets/10e9bbbd-d714-4a9f-ad34-67048059d679" />

# PacSnake  
Classic Snake game - with a twist! (Well, not yet, but it’s coming.)

This project started as an experiment: *can [Forge](https://github.com/Wuszt/Forge) be used to make even a simplest game?*  

---

## About  
The level was built using a separate editor app. The game then loads that level at runtime.  
The game project itself is standalone, but depends on the Forge engine.  

---

## Gameplay  
The snake can be controlled either by a human or by an AI.  

---

## AI  
The AI is based on an A* pathfinding approach. It’s able to avoid blocking itself most of the time, and “it works”…  
but it has one flaw - It can create one-cell “holes” in the level that are unreachable. Once that happens, the game can continue forever without ending.  

A more robust solution would be using a [Hamiltonian cycle](https://en.wikipedia.org/wiki/Hamiltonian_path).  
That would guarantee every cell is eventually visited and prevent creating "holes", though it would need some tweaks to look less robotic and not always force the snake through every cell when a straight path to the apple exists.  

---

## Demo  
https://github.com/user-attachments/assets/fc4b6d69-3ce4-4b9e-9689-7d81675aa36e


