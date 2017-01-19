def dec(f):
    return lambda: "DECORATED: " + f()

@dec
def foo():
    return "foo()"

print foo()
