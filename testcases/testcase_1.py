class Calculator:
    def __init__(self):
        self.history = []
    
    def add(self, a, b):
        result = a + b
        self.history.append(f"{a} + {b} = {result}")
        return result
    
    def multiply(self, a, b):
        result = a * b
        self.history.append(f"{a} * {b} = {result}")
        return result

    def divide(self, a, b):
        result = a / b
        self.history.append(f"{a} / {b} = {result}")
        return result

    def floor_divide(self, a, b):
        result = a // b
        self.history.append(f"{a} // {b} = {result}")
        return result

    def modulo(self, a, b):
        result = a % b
        self.history.append(f"{a} % {b} = {result}")
        return result
    
    def get_history(self):
        return self.history

calc = Calculator()
print(calc.add(5, 3))
print(calc.multiply(4, 6))
print(calc.divide(5, 2))
print(calc.divide(7.5, 2.5))
print(calc.floor_divide(7.5, 2))
print(calc.modulo(15.7, 4.2))
print("History:", calc.get_history())

try:
    result = 10 / 0
except ZeroDivisionError:
    print("Cannot divide by zero")
    
'''
8
24
2.5
3
3
3.1
History: [5 + 3 = 8, 4 * 6 = 24, 5 / 2 = 2.5, 7.5 / 2.5 = 3, 7.5 // 2 = 3, 15.7 % 4.2 = 3.1]
Cannot divide by zero
'''
