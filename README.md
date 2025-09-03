Working Title: Reactors of the Lost Arc
Code Name: ROTLA
a project in ue 5.4.4

Next Commit Message:

Bugs:
1 - When sliding if you slide while jumping when you land you can change direction while sliding. If you just slide while running or after running it does not allow you to change direction while sliding todo: make where changing direction while sliding cannot be done when jumping. - Fixed
2 - When sliding during a jump it will launch the player into the air todo: fix - Fixed
3 - The weapon overwrite data function doesn't load new meshes correctly and will not work if the mesh hasn't been loaded into memory already. In addition this breaks the entire weapon because the data doesn't get pulled from the registry correctly. - Fixed
4 - The DebugProjectiles don't delete themselves as they should - Fixed
5 - The Debug Console doesn't work in packaged games - Fixed

General Prototype Roadmap
00.01V — Combat Prototype
•	Establish functional combat loop with basic AI, weapons, inventory, and loot system
00.02V — Exploration & Collectibles Prototype
•	Implement exploration mechanics
•	Add collectible systems (e.g., extractors, lore items)
•	Allow player to interact with environment meaningfully
00.03V — NPC System Prototype
•	Create interactive NPC system
•	Implement dialogue interaction basics
•	Introduce quest-giver functionality and basic NPC behaviors
00.04V — Quest System Prototype
•	Implement quest tracking system
•	Allow for basic quest objectives: fetch, defeat, explore
•	Integrate with NPC system
00.05V — Morality System Prototype
•	Create alignment tracking system
•	Implement basic morality decisions and consequences
•	Allow player choices to affect future options
00.06V — Upgrade System Prototype
•	Implement modular upgrade mechanics for the player
•	Allow part swapping and upgrades to affect player stats
•	Integrate upgrade costs, crafting, and unlock requirements
01.00V — Prototype Complete; Playtest & Debug
•	Conduct full playtest of integrated systems
•	Address critical bugs, gameplay flow issues, and balance problems

00.01 -- Combat Prototype Roadmap
00.00.01V — Core Movement Implementation
•	Add basic player movement: walk, sprint, jump, crouch, slide
00.00.02V — Weapon System Foundation
•	Fix movement bugs
•	Implement expandable weapon system framework
•	Add first test weapon (debug object)
•	Adjustable parameters: range, projectile count, damage, planned damage type, spread, accuracy, projectile type(affects trajectory)
00.00.03V — Enemy Health System
•	Fix weapon bugs
•	Add killable enemy with basic health and death behavior
00.00.04V — Basic Enemy AI (Moving Target)
•	Fix AI/health bugs
•	Implement basic enemy AI: patrol, follow simple paths, act as moving target
00.00.05V — Armed Enemy with Combat AI
•	Fix AI bugs
•	Equip enemy with weapon and AI capable of shooting at the player
00.00.06V — Loot System Base
•	Fix AI/combat bugs
•	Create basic loot drop system
•	Enemies can drop their weapon upon death
00.00.07V — Player Inventory & Pickup System
•	Fix loot system bugs
•	Implement player inventory system
•	Ability to pick up and equip dropped weapons
00.01.00V — Internal Playtest & Bugfixes
•	Conduct internal playtesting session
•	Address discovered bugs, refine controls, weapon feel, and AI balance

Notes:
Core Gameplay Features and Systems
1.	Extractors
•	Expendable devices used to extract and display past memories
•	Due to embedded virus, extractors overload and explode after short use
•	Collectible items that offer lore and flashback insights
•	Add tension and risk to exploration
2.	Ancient Knowledge and Human Ally
•	Players can recover ancient knowledge during exploration
•	Knowledge is brought to a scholar-engineer NPC (an immortal human)
•	NPC helps unlock/purchase new equipment and reveals lore
•	Story element: Player’s robot core is leaking power and slowly dying
•	Inverted mortality dynamic: immortal human, decaying robot
3.	Modular Robot Upgrade System
•	Upgrade system based on replacing parts: arms, legs, chest, sensors, subsystems, processors
•	Ocular/sensor systems are deeply customizable, including:
o	Sonar
o	X-Ray
o	Heat vision
o	No visual processors (non-visual builds)
o	Orthographic view
o	Zoom/grayscale modes
o	Drone view with dual control (bot and drone simultaneously)
o	More TBD
4.	Power System
•	Core power source is broken, based on forgotten tech
•	Two power types:
o	Instant draw (capacitors)
o	Over-time power (generators)
•	Batteries and capacitors interact:
o	Capacitors can be charged from generators or batteries
o	Passive drain from sensor systems
o	Active draw for power attacks
•	Power systems must be balanced depending on build and equipment
5.	Visual Perception Without Ocular Systems
•	Player sees their own mesh as a blue wireframe (self-awareness system)
•	Enemies appear as temporary red wireframes when attacking (predictive combat detection)
•	Static objects outlined in white when contacted (proximity and collision awareness)
•	Quest or interactable objects highlighted with gold and blue wireframes when identified
•	If power is insufficient for ocular systems, world objects are wireframed based on interaction or movement, with moving objects only temporarily visible
Planned Features
•	Cover system for both player and AI, allowing tactical movement, peeking, and shooting from cover positions
Extra Lore Notes
•	The robot (player) won the war, but the war was manipulated by unseen antagonists
•	Most humans have become immortal through technological means
•	A small faction of “naturalists” reject immortality and are seen as unusual or eccentric, with a “hippie” vibe
•	The scholar-engineer is not part of the naturalist faction and fully embraces immortality
•	The robot is a one-of-a-kind relic classified tech from a forgotten age
•	In a last-ditch effort to win the war, the player’s faction removed all safety limiters from their war bots and granted them the ability to edit their own code
•	This led to emergent sentience among certain bots, including the player
•	Some powerful war bots (bosses) survived the war and hide across the world with unique, extreme ideologies
•	The player can choose to fight these bosses for major rewards or side with them for alternative outcomes
•	An alignment system based on the player’s choices affects available options over time
•	The alignment system is tied to “rewriting your morality module” — once the player drifts too far toward any alignment, their morality module locks them into that path, preventing reversal
Prologue Description
The game begins with a cinematic sequence. The camera flies over a vast, lifeless swamp. Nothing stirs; not even insects dare to approach the haunted waters. The locals whisper of ghostly moans and strange humming from creatures long past. Some claim that on certain nights, if you listen closely, you can hear cries in a forgotten language.
Suddenly, the camera dives into the water. Dust and silt explode in clouds, obscuring vision. As the sediment settles, a battlefield of shattered robots emerges — cracked, splintered, some melted, all relics of a brutal conflict. The camera presses forward, passing over miles of submerged wreckage until it reaches a clearer oceanic outlet.
Here, life thrives. The camera glides through a coral reef, where nature reclaims the remnants of war. The focus settles on a unique robot — your character — entombed in coral, seaweed, and barnacles. Fish and sea creatures swim nearby. An electric eel enters the frame, its body crackling with energy. It lunges at a large fish, but instead strikes the dormant robot. Electricity surges through the conductive metal, frying the eel — and awakening the robot.
The camera fades to black and attaches to the newly awakened pawn. A boot-up interface appears, running diagnostics, assessing functionality, determining shutdown duration. The system attempts to contact the server, but fails — the robot is ancient, obsolete, forgotten.
To understand what happened, the system accesses its long-term memory banks and black box.
The scene fades in to the memory — a massive battle from the robot’s past. The player assumes control with full power, abilities, and systems at their peak. This tutorial segment showcases the robot’s combat prowess and abilities. Just before the memory reveals the full truth, alarms blare, the screen flashes red — a virus hidden in the memory banks activates. The system enacts emergency protocols, halting the memory replay.
The player returns to the present. The robot’s systems are crippled. The immediate objective is clear: escape the reef. The HUD flickers online, revealing the wireframe perception mode. The player navigates the environment with limited sensory input, learning basic movement and interaction mechanics.
The prologue concludes when the player reaches shallow waters and encounters the scholar — the first friendly NPC. The scholar explains the existence of extractors and provides the player with their first one, setting the stage for the game’s exploration and lore discovery mechanics.