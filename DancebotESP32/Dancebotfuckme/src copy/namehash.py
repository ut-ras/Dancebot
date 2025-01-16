"""
File: namehash.py
Last Modified: 9/22/20
Author: Matthew Yu
Description:
    Simple script to calculate the hashes of specific Demobot names.
    Uses the hash function defined in DemobotNetwork.h DemobotNetwork::hash().
"""


def hash(arr):
    tot = 0
    count = 0
    for char in arr:
        power = 1
        for i in range(count):
            power *= 2
        tot += arr[count] * power
        count += 1
    return tot


dancebotStrings = [
    "Dancebot1",
    "Dancebot2",
    "Dancebot3",
    "Dancebot4",
    "Dancebot5",
    "Mothership",
    "Polargraph",
    "Marquee",
    "TowerOfPower",
]


dancebotList = []
for string in dancebotStrings:
    print(string)
    dancebotList.append([ord(ele) for sub in string for ele in sub])

print()

dancebotHash = []
for ele in dancebotList:
    print(ele)
    dancebotHash.append(hash(ele))

print()

for hash in dancebotHash:
    print(hash)