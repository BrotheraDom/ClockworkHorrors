## Controls

### Movement

| Key | Action |
|---|---|
| `WASD` | Move |
| `Mouse` | Look around |
| `Space` | Jump |

### Combat & Interaction

| Key | Action |
|---|---|
| `Left Mouse Button` | Attack |
| `Scroll` | Change Spell |
| `E` | Equip item |
| `F` | Drop item |
| `P` | Pause game |
| `I` | Open inventory |

### Quitting

1. Press `M` to open the pause menu.
2. Select **Main Menu**.
3. Click **Quit**.

---

## Recent Updates

### Animation System Overhaul

The largest change in this update is a reusable character animation framework that replaces the former single-purpose `PlayerAnimation` class.

**Commit:** `5b10bd2` — justamicksup

- Added `UCharacterAnimationComponent`, a reusable component that lets any character play locomotion and action animations without character-specific animation code.
- Added `UCharacterAnimationSet`, a data asset for assigning per-character animation references:
  - Locomotion: Idle, Walk, Run, Crouch, Strafe, Jump
  - Vehicle states: Row, Ride, Throttle
  - Actions: Attack, Shoot, Reload, Cast, Heal, Craft, Pickup, Dodge, Roll, Sleep, Rage
- Added `UCharacterAnimInstance`, which automatically calculates ground speed, movement direction, in-air state, crouch state, and the current locomotion state.
- Updated `ABaseCharacter` to use an `OnActionAnimationEnded` delegate rather than the previous `OnAttackEnded` callback pattern.

**Result for testers:** Movement blending and attacks should feel more consistent. The new system also makes adding future actions—such as dodging, casting, crafting, or healing—far easier without additional engine-side animation plumbing.

### Weapon & Pickup Fixes

- Fixed a crash caused by dropping items and restored weapon dropping functionality.  
  **Commit:** `594fa99` — Matthew Pelletier
- Fixed weapon pickups so weapons can be collected reliably again.  
  **Commit:** `b980571` — Matthew Pelletier
- Updated `BaseWeapon` to correctly trigger the generic pickup workflow.  
  **Commit:** `671994b` — Dominick Nardi
- Removed the unused `RiflePickup` class and its associated Data Asset reference, consolidating weapon pickups into the generic system.  
  **Commits:** `4b4b268`, `652ab6a`
- Fixed inventory item-handling issues.  
  **Commit:** `1cac546` — Dominick Nardi

### Audio & Settings

- Lava ambience now respects the Settings volume slider.  
  **Commit:** `355978` — Zachary Clevidence
- Button hover and click sounds now correctly apply both SFX and Master volume values.  
  **Commit:** `a40c1c0`
- Added volume-setting support for Abote’s footstep audio.  
  **Commit:** `f69afa8`
- Synced hurt and damage audio with the volume settings.  
  **Commit:** `f063f64`
- Made volume getter functions Blueprint-callable so UI widgets can access them directly.  
  **Commit:** `a9a0ee6`

### Level & World

- Updated level map geometry and layout.  
  **Commit:** `35deda6` — Jeremy Cahill
