import os

def pdec(dec):
    def idec(func):
        def ifunc(*args, **kwargs):
            return dec(func)(*args, **kwargs)
        return ifunc
    return idec


def default_decorator(func):
    def ifunc(*args, **kwargs):
        print "TRACE:", func.__name__, args
        return func(*args, **kwargs)
    return ifunc


def logging_decorator(func):
    def ifunc(*args, **kwargs):
        result = func(*args, **kwargs)
        with open('log.txt', 'a') as logfile:
            logfile.write("%s%s: %s\n" % (func.__name__, args, result))
        return result
    return ifunc


dec = pdec(globals()[os.getenv('DECORATOR', "default_decorator")])


def sum(x, y):
    return x + y

def product(x, y):
    return x * y


sum = dec(sum)
product = dec(product)


print sum(3, 4) + product(3, 4)
