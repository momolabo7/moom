def get_lowest_common_multiple(a, b):
    if a > b:
        n1 = a
        n2 = b
    else:
        n1 = b
        n2 = a
    for i in range(1, n2):
        mult = n1 * i
        if mult % n2 == 0:
            return mult
    return n1 * n2

class Fraction:
    def __init__(self, numerator, denominator):
        self.numerator = numerator
        self.denominator = denominator
    def __str__(self):
        return f"{self.numerator}/{self.denominator}"
    def __add__(self, other):
        numerator = self.numerator * other.denominator + other.numerator * self.denominator
        denominator = self.denominator * other.denominator
        return Fraction(numerator, denominator)
    def __neg__(self):
        return Fraction(-self.numerator, self.denominator)
    def __sub__(self, other):
        return self + (-other);
    def __mul__(self, other):
        if isinstance(other, int) or isinstance(other, float):
            return Fraction(self.numerator * other, self.denominator) 
        else:
            # Assume that it's fraction
            return Fraction(self.numerator * other.numerator, self.denominator * other.denominator) 
    def __rmul__(self, other):
        if isinstance(other, int) or isinstance(other, float):
            return Fraction(self.numerator * other, self.denominator) 
        else:
            # Assume that it's fraction
            return Fraction(self.numerator * other.numerator, self.denominator * other.denominator) 
    def __eq__(self, other):
        lcm = get_lowest_common_multiple(self.denominator, other.denominator)
        multiplyer_a = lcm / self.denominator
        multiplyer_b = lcm / other.denominator

        multiplied_a = self.numerator * multiplyer_a
        multiplied_b = other.numerator * multiplyer_b

        return multiplied_a == multiplied_b
    def __ne__(self, other):
        return not self == other
    def __gt__(self, other):
        lcm = get_lowest_common_multiple(self.denominator, other.denominator)
        multiplyer_a = lcm / self.denominator
        multiplyer_b = lcm / other.denominator

        multiplied_a = self.numerator * multiplyer_a
        multiplied_b = other.numerator * multiplyer_b

        return multiplied_a > multiplied_b
    def __lt__(self, other):
        return (not self > other) and (not self == other)
    def __ge__(self, other):
        return self > other or self == other
    def __le__(self, other):
        return self < other or self == other

print("=== Testing constructor === ")
f1 = Fraction(1, 2)
f2 = Fraction(1, 4)
f3 = Fraction(2, 4)
print(f1);
print(f2);
print(f3);

print("=== Testing __neg__() === ")
print(-f1);
print(-f2);

print("=== Testing __add__() === ")
print(f"{f1} +  {f2} = {f1 + f2}");

print("=== Testing __sub__() === ")
print(f"{f1} - {f2} = {f1 - f2}");

print("=== Testing __mul__() with same type === ")
print(f"{f1} * {f2} = {f1 * f2}");

print("=== Testing __mul__() with int type === ")
print(f"{f1} * 2 = {f1 * 2} ");

print("=== Testing __mul__() with float type === ")
print(f"{f1} * 1.5 = {f1 * 1.5} ");

print("=== Testing __rmul__() with int type === ")
print(f"2 * {f1}= {2 * f1} ");

print("=== Testing __rmul__() with float type === ")
print(f"1.5 * {f1} = {1.5 * f1} ");

print("=== Testing __eq__() ")
print(f"{f1} == {f3} = {f1 == f3}")
print(f"{f2} == {f3} = {f2 == f3}")

print("=== Testing __ne__() ")
print(f"{f1} != {f3} = {f1 != f3}")
print(f"{f2} != {f3} = {f2 != f3}")

print("=== Testing __gt__() ")
print(f"{f1} > {f3} = {f1 > f3}")
print(f"{f2} > {f3} = {f2 > f3}")
print(f"{f1} > {f2} = {f1 > f2}")

print("=== Testing __lt__() ")
print(f"{f1} < {f3} = {f1 < f3}")
print(f"{f2} < {f3} = {f2 < f3}")
print(f"{f1} < {f2} = {f1 < f2}")

print("=== Testing __ge__() ")
print(f"{f1} >= {f3} = {f1 >= f3}")
print(f"{f2} >= {f3} = {f2 >= f3}")
print(f"{f1} >= {f2} = {f1 >= f2}")

print("=== Testing __le__() ")
print(f"{f1} <= {f3} = {f1 <= f3}")
print(f"{f2} <= {f3} = {f2 <= f3}")
print(f"{f1} <= {f2} = {f1 <= f2}")
