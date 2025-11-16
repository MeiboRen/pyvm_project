text = "Hello, World! This is a test string."

print("Original text:", text)
print("Length:", len(text))

print("\nProcessing characters:")
vowels = "aeiouAEIOU"
vowel_count = 0
consonant_count = 0
letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

i = 0
while i < len(text):
    char = text[i]
    if char not in letters:
        i += 1
        continue
    
    if char in vowels:
        vowel_count += 1
        print("Vowel at position " + str(i) + ": " + char)
    else:
        consonant_count += 1
    
    if vowel_count + consonant_count >= 10:
        print("Reached 10 letters, stopping early")
        break
    
    i += 1

print("Total vowels found: " + str(vowel_count))
print("Total consonants found: " + str(consonant_count))

print("\nCounting down with continue:")
count = 10
while count > 0:
    count -= 1
    if count % 2 == 0:
        continue
    print("Odd count: " + str(count))
    
'''
Original text: Hello, World! This is a test string.
Length: 36

Processing characters:
Vowel at position 1: e
Vowel at position 4: o
Vowel at position 8: o
Reached 10 letters, stopping early
Total vowels found: 3
Total consonants found: 7

Counting down with continue:
Odd count: 9
Odd count: 7
Odd count: 5
Odd count: 3
Odd count: 1
'''
