def sum(iter, i):
    tmp = i
    for e in iter:
        tmp += e
    return tmp

def range(*args):
    start, step = 0, 1
    if len(args) == 1:
        end = args[0]
    elif len(args) == 2:
        start = args[0]
        end = args[1]
    else:
        start = args[0]
        end = args[1]
        step = args[2]

    l = []
    if start < end and step > 0:
        while start < end:
            l.append(start)
            start += step
    elif start > end and step < 0:
        while start > end:
            l.append(start)
            start += step
    return l

def map(func, iter):
    l = []
    for i in iter:
        l.append(func(i))
    return l

def filter(func, iter):
    l = []
    for i in iter:
        if func(i):
            l.append(i)
    return l

class Exception(object):
    def __init__(self, *args):
        self.info = args
    def __repr__(self):
        return " ".join(self.info)

class TypeError(Exception):
    pass

class NameError(Exception):
    pass

class ZeroDivisionError(Exception):
    pass

class StopIteration(Exception):
    pass

class ModuleNotFoundError(Exception):
    pass
