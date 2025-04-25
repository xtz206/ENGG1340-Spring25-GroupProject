# TODO_LIST

---

## Renderers

- [ ] Render color for maps
- [ ] Clear recognition sign (or, variance) for different missile types
- [x] Better way to showcase operation and INFOs
- [ ] Radar WIN, showing appr.pos of atkMissiles
- [x] StartMenu of game
- [ ] **Showcase new attributes of other elements**

---

## GameCore

### Missile

- [x] DiverseAttribute (i.e. Levels of speed, damage, showcased by renderer)

### Enemy

- [x] RandomMissileFactory/RandomMissileBuilder (i.e. Generate missiles with random targets)
- [x] AttackWaves (i.e. Dynamically manage multi atkMissiles at a time)
- [x] More fierce attack (i.e. Increment Missile Num and Speed over time)
- [ ] CounterAttack (i.e. Own HP, can be deduced by player through some method, effect on its atk performance)

### Economy

- [ ] Fine-tune parameters (i.e. control income & cost for ops)
- [ ] More Usages (e.g. attack enemy / research)

### Research

- [x] **Try** add research tree
- [ ] New render method
- [x] Interaction with Econ
- [x] Research on Econ
- [x] Research on Missiles
- [x] Research on Cities
- [x] Research on global (e.g autoDefense)

### Player

- [ ] CounterAtk OP
- [ ] Method to get atkMissile target (e.g. select city, know how many missiles towards it)

### Others

- [ ] Levels of game
- [ ] **Try** Infinite Mode
- [ ] **Try** Animated Tutorial **Otherwise** Instruction with legend
- [ ] **Q:** Can we make turn increment automatically to situmulate time **@XTC**

---

## Statistics

- [ ] Counting Turns; Missile launched; Missile defended;
- [ ] Generate and store score
- [ ] Data make game more realistic (e.g. casualty)
