Movement
WASD — Move
Mouse — Look around
Space — Jump
Combat & Interaction
Left Mouse Button — Attack
E — Equip item
F — Drop item
M — Pause game
I  — Inventory
Quitting
Press M to pause, then Main Menu, then click Quit


Animation System Overhaul The biggest change in this batch: a full reusable animation framework replacing the old single-purpose PlayerAnimation class (commit 5b10bd2, by justamicksup).




Added UCharacterAnimationComponent — a drop-in component any character can use to play locomotion and action animations without character-specific code.
Added UCharacterAnimationSet — a data asset holding per-character animation references (Idle, Walk, Run, Crouch, Strafe, Jump, plus vehicle states like Row/Ride/Throttle, and action animations like Attack, Shoot, Reload, Cast, Heal, Craft, Pickup, Dodge, Roll, Sleep, Rage).
Added UCharacterAnimInstance — automatically computes ground speed, movement direction, in-air state, and crouch state every tick, then resolves a locomotion state (Idle/Walk/Run/Strafe/Jump) without manual Blueprint wiring.
ABaseCharacter now hooks into this system through an OnActionAnimationEnded delegate instead of the old OnAttackEnded callback pattern, so attack-animation timing is decoupled from any single weapon class.
Net effect for testers: attack animations and movement blending should look and feel more consistent, and this groundwork makes it much easier to add new animations (dodge, cast, craft, etc.) going forward without further engine-side plumbing.

Weapon & Pickup Fixes
Fixed a crash when dropping items and re-enabled weapon drops after they'd been broken (commit 594fa99, Matthew Pelletier). If you previously hit a crash pressing F to drop a weapon, that's resolved.
Fixed weapon pickup so items can be picked up reliably again (commit b980571, Matthew Pelletier).
BaseWeapon now correctly triggers pickup logic through BaseWeapon instead of a narrower path (commit 671994b, Dominick Nardi).
Removed the now-unused RiflePickup class and its Data Asset reference, consolidating pickup logic into the generic weapon pickup system (commits 4b4b268, 652ab6a).
Inventory fix addressing item-handling issues (commit 1cac546, Dominick Nardi).

Audio & Settings
Lava ambience is now affected by the Settings volume slider instead of playing at a fixed level (commit 355978, Zachary Clevidence).
Button hover and click sounds now correctly multiply SFX volume by Master volume, replacing a getter-function approach that wasn't syncing properly (commit a40c1c0).
Footstep sound for the player character (Abote) is now wired to the volume settings (commit f69afa8).
Hurt/damage audio synced to volume settings as well (commit f063f64).
Volume getter functions were made Blueprint-callable, so UI widgets can read them directly (commit a9a0ee6).

Level & World
Updated level map geometry/layout (commit 35deda6, Jeremy Cahill).
