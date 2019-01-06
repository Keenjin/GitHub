class Parent2(object):
    def __init__(self):
        print("Parent2")

class Parent1(object):
    def __init__(self):
        print("Parent1")

class Child(Parent2,Parent1):
    def __init__(self):
        print("Child before")
        super().__init__()
        print("Child after")

Child()