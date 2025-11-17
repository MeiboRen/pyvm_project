from testcase_3_import import factorial, is_prime, primes_cache

print("Factorial of 5:", factorial(5))
print("Factorial of 7:", factorial(7))

primes = []
for i in range(2, 20):
    if is_prime(i):
        primes.append(i)

print("Primes under 20:", primes)
print("Cached primes:", primes_cache)

print("Is 17 prime?", is_prime(17))
print("Is 15 prime?", is_prime(15))

prime_set = set(primes)
print("Prime set:", prime_set)
print("Union:", prime_set.union({23, 29}))
print("Intersection:", prime_set.intersection({2, 3, 4, 5, 6}))

'''
Factorial of 5: 120
Factorial of 7: 5040
Primes under 20: [2, 3, 5, 7, 11, 13, 17, 19]
Cached primes: [2, 3, 5, 7, 11, 13, 17, 19]
Is 17 prime? True
Is 15 prime? False
Prime set: {2, 3, 5, 7, 11, 13, 17, 19}
Union: {2, 3, 5, 7, 11, 13, 17, 19, 29, 23}
Intersection: {2, 3, 5}
'''
