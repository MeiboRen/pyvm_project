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
    
    def get_history(self):
        return self.history

calc = Calculator()
print(calc.add(5, 3))
print(calc.multiply(4, 6))
print("History:", calc.get_history())

try:
    result = 10 / 0
except ZeroDivisionError:
    print("Cannot divide by zero")
    
'''
8
24
History: [5 + 3 = 8, 4 * 6 = 24]
Cannot divide by zero
'''
