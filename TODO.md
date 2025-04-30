# TODO_LIST

---

## Render && UI

- [ ] Render color for maps
- [ ] Colorful UI
- [x] Better way to showcase operation and INFOs
- [x] showing counts of attack missiles
- [x] StartMenu of game

---

## GameCore

### Missile

- [x] DiverseAttribute (i.e. Levels of speed, damage, showcased by renderer)

### Enemy

- [x] RandomMissileFactory/RandomMissileBuilder (i.e. Generate missiles with random targets)
- [x] AttackWaves (i.e. Dynamically manage multi attack missiles at a time)
- [x] More fierce attack (i.e. Increment Missile Num and Speed over time)
- [x] CounterAttack (i.e. Own HP, can be deduced by player through some method, effect on its atk performance)

### Economy

- [ ] Fine-tune parameters (i.e. control income & cost for ops)
- [x] More Usages (e.g. attack enemy / research)

### Research

- [x] **Try** add research tree
- [x] New render method
- [x] Interaction with Econ
- [x] Research on Econ
- [x] Research on Missiles
- [x] Research on Cities
- [x] Research on global (e.g autoDefense)

### Player

- [x] CounterAtk OP
- [x] Method to get atkMissile target (e.g. select city, know how many missiles towards it)

### Others

- [x] Levels of game
- [x] Basic Text Tutorial (i.e. how to play, keyboard shortcuts and actions)
- [ ] Restart Button at pause menu and end menu
- [ ] Change another map
- [ ] overwrite/empty warning when saving/loading

---

## Statistics

- [x] Counting Turns; Missile launched; Missile defended;
- [ ] Generate and store score
- [ ] Data make game more realistic (e.g. casualty)
- [ ] Display statistics at end of game

---

## Refactor

- [x] Refactor Game/Menu activation operations

---

## Test & Bugfix

- [ ] Test Control of Flow / Keyboard Shortcuts
- [ ] Test Game Difficulty & Balance, then fine-tune parameters accordingly
- [ ] Fix bugs in file saving/loading
