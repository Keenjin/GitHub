from sys import exit
from random import randint
from textwrap import dedent

class Scene(object):
    def enter(self):
        print("This scene is not yet configured.")
        print("Subclass it and implement enter().")
        exit(1)

class Engine(object):
    def __init__(self, scene_map):
        self.scene_map = scene_map

    def play(self):
        current_scene = self.scene_map.opening_scene()
        last_scene = self.scene_map.next_scene('finished')

        while current_scene != last_scene:
            next_scene_name = current_scene.enter()
            current_scene = self.scene_map.next_scene(next_scene_name)

        current_scene.enter()

class Death(Scene):
    quips = [
        "You died. You kinda suck at this.",
        "Your Mom would be proud...if she were smarter.",
        "Such a luser.",
        "I have a small puppy that's better at this.",
        "You're worse than your Dad's jokes."
    ]

    def enter(self):
        print(Death.quips[randint(0, len(self.quips) - 1)])
        exit(1)

class CentralCorridor(Scene):
    def enter(self):
        print(dedent("""
            The Gothons of Planet Percal #25 have invaded 
            destroyed your entire ... 

            You're running down the central corridor to the 
            Armory when a Gothon jumps out, red scaly skin .
        """))
    
        action = input("> ")

        if action == "shoot!":
            print(dedent("""
                Quick on the draw you yank out youre blaste 
                it at the Gothon. His clown costume is flo .
            """))
            return 'death'
        
        elif action == "dodge!":
            print(dedent("""
                Like a world class boxer you dodge, weave, 
                slide right as the Gothon's blaster cranks.
            """))
            return 'death'

        elif action == "tell a joke":
            print(dedent("""
                Lucky for you they made you learn Gothon in
                the academy. You tell the one Gothon joke .
            """))
            return 'laser_weapon_armory'

        else:
            print("DOES NOT COMPUTE!")
            return 'central_corridor'

class LaserWeaponArmory(Scene):
    def enter(self):
        print(dedent("""
            You do a dive roll into the Weapon Armory, cross 
            the room for more Gothons that might be hiding .
        """))

        code = f"{randint(1,9)}{randint(1,9)}{randint(1,9)}"
        guess = input("[keypad]> ")
        guesses = 0

        while guess != code and guesses < 10:
            print("BZZZZZEDDD!")
            guesses += 1
            guess = input("[keypad]> ")

        if guess == code:
            print(dedent("""
                The container clicks open and the seal broke 
                gas out. You grab the neutron bomb and run .
            """))
            return 'the_bridge'
        
        else:
            print(dedent("""
                The lock buzzes one last time and then you 
                sickening melting sound as the mechanism .
            """))
            return 'death'

class TheBridge(Scene):
    def enter(self):
        print(dedent("""
            You burst onto the Bridge with the netron destroy
            under your arm and surprise 5 Gothons who are .
        """))

        action = input("> ")

        if action == "throw the bomb":
            print(dedent("""
                In a panic you throw the bomb at the group 
                and make a leap for the door. Right as you.
            """))
            return 'death'

        elif action == "slowly place the bomb":
            print(dedent("""
                You point your blaster at the bomb under 
                the Gothons put their hands up and start.
            """))
            return 'escape_pod'

        else:
            print("DOES NOT COMPUTE!")
            return 'the_bridge'

class EscapePod(Scene):
    def enter(self):
        print(dedent("""
            You rush through the ship desperately trying 
            the escape pod before the whole ship explodes.
        """))

        good_pod = randint(1,5)
        guess = input("[pod #]> ")

        if int(guess) != good_pod:
            print(dedent(f"""
                You jump into pod {guess} and hit the eject 
                The pod escapes out into the void of space.
            """))
            return 'death'

        else:
            print(dedent(f"""
                You jump into pod {guess} and hit the eject
                The pod easily slides out into space head.
            """))
            return 'finished'

class Finished(Scene):
    def enter(self):
        print("You won! Good job.")
        return 'finished'

class Map(object):
    scenes = {
        'central_corridor': CentralCorridor(),
        'laser_weapon_armory': LaserWeaponArmory(),
        'the_bridge': TheBridge(),
        'escape_pod': EscapePod(),
        'death': Death(),
        'finished': Finished(),
    }
    def __init__(self, start_scene):
        self.start_scene = start_scene

    def next_scene(self, scene_name):
        val = Map.scenes.get(scene_name)
        return val

    def opening_scene(self):
        return self.next_scene(self.start_scene)

a_map = Map('central_corridor')
a_game = Engine(a_map)
a_game.play()