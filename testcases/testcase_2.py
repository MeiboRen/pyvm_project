def process_numbers(numbers):
    even_numbers = []
    odd_numbers = []
    
    for num in numbers:
        if num % 2 == 0:
            even_numbers.append(num)
        else:
            odd_numbers.append(num)
    
    return even_numbers, odd_numbers

def calculate_squares(n):
    squares = []
    for i in range(1, n + 1):
        squares.append(i * i)
    return squares

def sum_numbers(numbers):
    total = 0
    for num in numbers:
        total += num
    return total

numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
evens, odds = process_numbers(numbers)
squares = calculate_squares(5)
total = sum_numbers(numbers)

print("Even numbers:", evens)
print("Odd numbers:", odds)
print("Squares:", squares)
print("Sum:", total)

'''
Even numbers: [2, 4, 6, 8, 10]
Odd numbers: [1, 3, 5, 7, 9]
Squares: [1, 4, 9, 16, 25]
Sum: 55
'''
