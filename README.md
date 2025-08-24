<div align="center">
  <img height="400" alt="PacSnake" src="https://github.com/user-attachments/assets/7e2f10e2-6b34-40a6-8150-0cecba73cf84" />
</div>

# PacSnake  
Classic Snake game - with a twist! (Well, not yet, but it’s coming.)

This project started as an experiment: *can [Forge](https://github.com/Wuszt/Forge) be used to make even a simplest game?*  

## About  
The level was created in a separate editor app and is loaded by the game at runtime.  
The game project is standalone but depends on the Forge engine.  

The snake can be controlled either by a human or by an AI. The entire game state is decoupled from rendering, which means the simulation can run without visuals — leaving the door open to plug in machine learning in the future. 

## AI  
The AI is based on an A* pathfinding approach. It’s able to avoid blocking itself most of the time, and “it works”…  
but it has one flaw - It can create one-cell “holes” in the level that are unreachable. Once that happens, the game can continue forever without ending.  

A more robust solution would be using a [Hamiltonian cycle](https://en.wikipedia.org/wiki/Hamiltonian_path).  
That would guarantee every cell is eventually visited and prevent creating "holes", though it would need some tweaks to look less robotic and not always force the snake through every cell when a straight path to the apple exists.  

## Demo  
https://github.com/user-attachments/assets/fc4b6d69-3ce4-4b9e-9689-7d81675aa36e


