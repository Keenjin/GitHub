states = {
    'Oregon':'OR',
    'Florida':'FL',
    'California':'CA',
    'New York':'NY',
    'Michigan':'MI'
}

cities = {
    'CA':'San Francisco',
    'MI':'Detroit',
    'FL':'Jacksonville'
}

cities['NY'] = 'New York'
cities['OR'] = 'Portland'

print('-' * 10)
print("NY state has: ", cities['NY'])

print('-' * 10)
for state, abbrev in list(states.items()):
    print(f"{state} is avvreviated {abbrev}")
