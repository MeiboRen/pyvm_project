student_grades = {"Alice": 85, "Bob": 92, "Charlie": 78}
print("Original grades:", student_grades)
student_grades["David"] = 88
print("After adding David:", student_grades)

student_grades["Charlie"] = 82
print("After updating Charlie:", student_grades)

student_grades.setdefault("Eve", 90)
print("After setdefault Eve:", student_grades)

student_grades.setdefault("Alice", 100)
print("After setdefault Alice (no change):", student_grades)

if "Bob" in student_grades:
    student_grades.pop("Bob")
    print("After removing Bob:", student_grades)

print("\nAll students and grades:")
for name, grade in student_grades.items():
    print(name + ": " + str(grade))

if "Alice" in student_grades:
    print("Alice is in the class")
else:
    print("Alice is not in the class")

print("All students:", list(student_grades.keys()))
print("All grades:", list(student_grades.values()))

'''
Original grades: {Charlie: 78, Bob: 92, Alice: 85}
After adding David: {Charlie: 78, Bob: 92, Alice: 85, David: 88}
After updating Charlie: {Charlie: 82, Bob: 92, Alice: 85, David: 88}
After setdefault Eve: {Charlie: 82, Bob: 92, Alice: 85, David: 88, Eve: 90}
After setdefault Alice (no change): {Charlie: 82, Bob: 92, Alice: 85, David: 88, Eve: 90}
After removing Bob: {Charlie: 82, Eve: 90, Alice: 85, David: 88}

All students and grades:
Charlie: 82
Eve: 90
Alice: 85
David: 88
Alice is in the class
All students: [Charlie, Eve, Alice, David]
All grades: [82, 90, 85, 88]
'''
